/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Generate tables utility program                                                     */
/* Copyright(c) 2016-2018 LoRd_MuldeR <mulder2@gmx.de>                                             */
/*                                                                                                 */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software   */
/* and associated documentation files(the "Software"), to deal in the Software without             */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,      */
/* distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions:                            */
/*                                                                                                 */
/* The above copyright notice and this permission notice shall be included in all copies or        */
/* substantial portions of the Software.                                                           */
/*                                                                                                 */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING   */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND      */
/* NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,     */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.         */
/* ----------------------------------------------------------------------------------------------- */

#include "common.h"
#include "thread_utils.h"
#include "msws.h"
#include "boxmuller.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <io.h>
#include <float.h>
#include <math.h>

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384U
#define DISTANCE_MIN 192U

#define THREAD_COUNT 8U

#undef ENABLE_TRACE

#define ROW_NUM (UINT8_MAX+2)           /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

#define TARGET_PROCESSING_TIME 5400.0

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

#define MAGIC_NUMBER 0x837EAF3A7F3A4DFDui64
#define THREAD_ID (pthread_getw32threadid_np(pthread_self()))

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static const char SPINNER[4] = { '/', '-', '\\', '|' };
static const double SQRT2 = 1.41421356237309504880168872420969807856967187537694;

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

#ifdef ENABLE_TRACE
#define TRACE(X, ...) printf("[%04X] " X "\n", THREAD_ID, __VA_ARGS__)
#else
#define TRACE(X, ...) __noop()
#endif

static inline void print_row(uint8_t *const row_buffer)
{
	for (size_t w = 0; w < ROW_LEN; ++w)
	{
		printf("%02X", row_buffer[w]);
	}
	puts("");
}

static inline void flip_bit_at(uint8_t *const row_buffer, const uint_fast16_t pos)
{
	row_buffer[pos >> 3] ^= ((uint8_t)(1U << (pos & 0x7)));
}

static inline void flip_rand_n(uint8_t *const row_buffer, msws_t rand, const uint32_t n)
{
	bool taken[HASH_LEN];
	memset(&taken, 0, sizeof(bool) * HASH_LEN);
	for (uint_fast32_t i = 0; i < n; ++i)
	{
		uint_fast16_t next;
		do
		{
			next = (uint_fast16_t)msws_uint32_max(rand, HASH_LEN);
		}
		while (taken[next]);
		flip_bit_at(row_buffer, next);
		taken[next] = true;
	}
}

static inline uint_fast32_t get_distance_rows(const uint8_t table[ROW_NUM][ROW_LEN], const uint_fast16_t index_1, const uint_fast16_t index_2)
{
	return hamming_distance(table[index_1], table[index_2], ROW_LEN);
}

#define ERROR_ENC(MAX,ACC) (((MAX) << 22U) | (ACC))
#define ERROR_DEC_ACC(VAL) ((VAL) & 0x3FFFFF)
#define ERROR_DEC_MAX(VAL) (((VAL) >> 22U) & 0x3FF)
static inline uint_fast32_t get_error_table(const uint8_t table[ROW_NUM][ROW_LEN], const uint_fast32_t limit)
{
	uint_fast32_t error_max = 0U, error_acc = 0U;
	const uint_fast32_t limit_acc = ERROR_DEC_ACC(limit), limit_max = ERROR_DEC_MAX(limit);
	for (uint_fast16_t i = 0; i < ROW_NUM; i++)
	{
		for (uint_fast16_t j = i + 1U; j < ROW_NUM; j++)
		{
			const uint_fast32_t dist = get_distance_rows(table, i, j);
			if (dist < DISTANCE_MIN)
			{
				const uint_fast32_t current = DISTANCE_MIN - dist;
				if (current > error_max)
				{
					if ((error_max = current) > limit_max)
					{
						return limit;
					}
				}
				if (((error_acc += current) >= limit_acc) && (error_max == limit_max))
				{
					return limit;
				}
			}
		}
	}
	return ERROR_ENC(error_max, error_acc);
}

static inline void copy_table(uint8_t dst[ROW_NUM][ROW_LEN], const uint8_t src[ROW_NUM][ROW_LEN])
{
	for (uint_fast16_t i = 0; i < ROW_NUM; i++)
	{
		memcpy(&dst[i][0], &src[i][0], sizeof(uint8_t) * ROW_LEN);
	}
}

//static void dump_table(FILE *out)
//{
//	fputs("uint8_t MHASH_384_TABLE_XOR[UINT8_MAX+2][MHASH_384_LEN] =\n{\n", out);
//	for (size_t i = 0; i < ROW_NUM; i++)
//	{
//		fputs("\t{ ", out);
//		for (size_t j = 0; j < ROW_LEN; j++)
//		{
//			if (j > 0)
//			{
//				fputc(',', out);
//			}
//			fprintf(out, "0x%02X", g_table[i][j]);
//		}
//		fprintf(out, " }%s /*%02X*/\n", (i != (ROW_NUM - 1)) ? "," : " ", (uint32_t)(i % 0x100));
//	}
//	fputs("};\n", out);
//}

//-----------------------------------------------------------------------------
// Thread function
//-----------------------------------------------------------------------------

typedef struct
{
	uint8_t table[ROW_NUM][ROW_LEN];
	uint_fast32_t threshold;
	uint_fast16_t row_offset;
	sem_t *stop;
	pthread_mutex_t *mutex;
}
thread_data_t;

typedef struct
{
	size_t spin_pos;
	time_t missed_cticks;
	sem_t *stop;
}
thread_spin_t;

#define CHECK_SUCCESS(ERR_CURR,ERR_INIT,THRESHLD) \
	(((ERR_CURR) < (ERR_INIT)) && (((ERR_INIT) - (ERR_CURR) >= (THRESHLD)) || (!(ERR_CURR))))

#define CHECK_TERMINATION() do \
{  \
	if (SEM_TRYWAIT(data->stop))  \
	{ \
		return NULL; \
	} \
} \
while(0) \

static const int MAX_ROUNDS = (THREAD_COUNT > 1) ? (ROW_NUM / (THREAD_COUNT / 2U)) : ROW_NUM;

static void* thread_main(void *const param)
{
	thread_data_t *const data = (thread_data_t*)param;
	msws_t rand;
	bxmller_t bxmller;
	uint8_t backup[ROW_LEN];
	const uint_fast32_t error_initial = get_error_table(data->table, UINT_FAST32_MAX);
	uint_fast32_t error = error_initial;
	uint_fast16_t row_index = data->row_offset;
	int16_t row_iter = 0;
	bool reduceFlag = false, improvedFlag = true;
	TRACE("Initial error: %08X [row offset: %03u, threshold: %u]", error_initial, data->row_offset, data->threshold);
	while(error)
	{
		msws_init(rand, make_seed());
		gaussian_noise_init(&bxmller);
		//--------------------//
		//--- RAND REPLACE ---//
		//--------------------//
		for (row_index = data->row_offset, row_iter = 0; row_iter < 0/*MAX_ROUNDS*/; row_index = (row_index + 1U) % ROW_NUM, ++row_iter)
		{
			TRACE("Rand-replace round %d of %d", row_iter + 1, MAX_ROUNDS);
			memcpy(backup, data->table[row_index], sizeof(uint8_t) * ROW_LEN);
			for (uint_fast16_t rand_loop = 0; rand_loop < 9973U; ++rand_loop)
			{
				msws_bytes(rand, data->table[row_index], ROW_LEN);
				const uint_fast32_t error_next = get_error_table(data->table, error);
				if (error_next < error)
				{
					TRACE("Improved by rand-replace (%08X -> %08X) [row: %03u]", error, error_next, row_index);
					memcpy(backup, data->table[row_index], sizeof(uint8_t) * ROW_LEN);
					row_iter = -1;
					improvedFlag = true;
					error = error_next;
				}
			}
			memcpy(data->table[row_index], backup, sizeof(uint8_t) * ROW_LEN);
			if (CHECK_SUCCESS(error, error_initial, data->threshold))
			{
				TRACE("Success by rand-replace <<<---");
				goto success;
			}
			CHECK_TERMINATION();
		}
		if (improvedFlag)
		{
			improvedFlag = false;
			//---------------------//
			//----- XCHG BYTE -----//
			//---------------------//
			for (row_index = data->row_offset, row_iter = 0; row_iter < 0/*MAX_ROUNDS*/; row_index = (row_index + 1U) % ROW_NUM, ++row_iter)
			{
				TRACE("Xchg-byte round %d of %d", row_iter + 1, MAX_ROUNDS);
				for (uint_fast16_t xchg_pos = 0U; xchg_pos < ROW_LEN; ++xchg_pos)
				{
					uint8_t value_backup = data->table[row_index][xchg_pos];
					for (uint_fast16_t value_next = 0U; value_next <= UINT8_MAX; ++value_next)
					{
						if ((uint8_t)value_next != value_backup)
						{
							data->table[row_index][xchg_pos] = (uint8_t)value_next;
							const uint_fast32_t error_next = get_error_table(data->table, error);
							if (error_next < error)
							{
								TRACE("Improved by xchg-byte (%08X -> %08X) [row: %03u]", error, error_next, row_index);
								value_backup = data->table[row_index][xchg_pos];
								row_iter = -1;
								improvedFlag = true;
								error = error_next;
							}
						}
					}
					data->table[row_index][xchg_pos] = value_backup;
				}
				if (CHECK_SUCCESS(error, error_initial, data->threshold))
				{
					TRACE("Success by xchg-byte <<<---");
					goto success;
				}
				CHECK_TERMINATION();
			}
			//---------------------//
			//------ FLIP-2 -------//
			//---------------------//
			for (row_index = data->row_offset, row_iter = 0; row_iter < 0/*MAX_ROUNDS*/; row_index = (row_index + 1U) % ROW_NUM, ++row_iter)
			{
				TRACE("Flip-2 round %d of %d", row_iter + 1, MAX_ROUNDS);
				for (uint_fast16_t flip_pos_x = 0U; flip_pos_x < HASH_LEN; ++flip_pos_x)
				{
					flip_bit_at(data->table[row_index], flip_pos_x);
					bool revert_x = true;
					for (uint_fast16_t flip_pos_y = flip_pos_x + 1U; flip_pos_y < HASH_LEN; ++flip_pos_y)
					{
						flip_bit_at(data->table[row_index], flip_pos_y);
						const uint_fast32_t error_next = get_error_table(data->table, error);
						if (error_next < error)
						{
							TRACE("Improved by flip-2 (%08X -> %08X) [row: %03u]", error, error_next, row_index);
							revert_x = false;
							row_iter = -1;
							improvedFlag = true;
							error = error_next;
						}
						else
						{
							flip_bit_at(data->table[row_index], flip_pos_y);
						}
					}
					if (revert_x)
					{
						flip_bit_at(data->table[row_index], flip_pos_x);
					}
				}
				if (CHECK_SUCCESS(error, error_initial, data->threshold))
				{
					TRACE("Success by flip-2 <<<---");
					goto success;
				}
				CHECK_TERMINATION();
			}
			//---------------------//
			//------ FLIP-4 -------//
			//---------------------//
			for (row_index = data->row_offset, row_iter = 0; row_iter < MAX_ROUNDS; row_index = (row_index + 1U) % ROW_NUM, ++row_iter)
			{
				//const uint_fast16_t flip_pos_w = (uint_fast16_t) msws_uint32_max(rand, HASH_LEN);
				//flip_bit_at(data->table[row_index], flip_pos_w);
				//bool revert_w = true;
				for (uint_fast16_t flip_pos_x = 0U; flip_pos_x < HASH_LEN; ++flip_pos_x)
				{
					TRACE("Flip-4 round %d of %d [step %u of %u]", row_iter + 1, MAX_ROUNDS, flip_pos_x + 1U, HASH_LEN);
					flip_bit_at(data->table[row_index], flip_pos_x);
					bool revert_x = true;
					for (uint_fast16_t flip_pos_y = flip_pos_x + 1U; flip_pos_y < HASH_LEN; ++flip_pos_y)
					{
						flip_bit_at(data->table[row_index], flip_pos_y);
						bool revert_y = true;
						for (uint_fast16_t flip_pos_z = flip_pos_y + 1U; flip_pos_z < HASH_LEN; ++flip_pos_z)
						{
							flip_bit_at(data->table[row_index], flip_pos_z);
							bool revert_z = true;
							const uint_fast32_t error_next = get_error_table(data->table, error);
							if (error_next < error)
							{
								TRACE("Improved by flip-3 (%08X -> %08X) [row: %03u, x:%u, y:%u, z:%u]", error, error_next, row_index, flip_pos_x, flip_pos_y, flip_pos_z);
								revert_x = revert_y = revert_z = false;
								row_iter = -1;
								improvedFlag = true;
								error = error_next;
							}
							for (uint_fast16_t flip_rnd = 0U; flip_rnd < 8U; ++flip_rnd)
							{
								uint_fast16_t flip_pos_r;
								do
								{
									flip_pos_r = (uint_fast16_t)msws_uint32_max(rand, HASH_LEN);
								}
								while ((flip_pos_r == flip_pos_x) || (flip_pos_r == flip_pos_y) || (flip_pos_r == flip_pos_z));
								flip_bit_at(data->table[row_index], flip_pos_r);
								const uint_fast32_t error_next = get_error_table(data->table, error);
								if (error_next < error)
								{
									TRACE("Improved by flip-4 (%08X -> %08X) [row: %03u, x: %u, y: %u, z: %u, r: %u]", error, error_next, row_index, flip_pos_x, flip_pos_y, flip_pos_z, flip_pos_r);
									revert_x = revert_y = revert_z = false;
									row_iter = -1;
									improvedFlag = true;
									error = error_next;
								}
								else
								{
									flip_bit_at(data->table[row_index], flip_pos_r);
								}
							}
							if(revert_z)
							{
								flip_bit_at(data->table[row_index], flip_pos_z);
							}
						}
						if (revert_y)
						{
							flip_bit_at(data->table[row_index], flip_pos_y);
						}
					}
					if (revert_x)
					{
						flip_bit_at(data->table[row_index], flip_pos_x);
					}
					if (CHECK_SUCCESS(error, error_initial, data->threshold))
					{
						TRACE("Success by flip-4 <<<---");
						goto success;
					}
					CHECK_TERMINATION();
				}
			}
		}
		//--------------------//
		//------ FLIP-N ------//
		//--------------------//
		for (row_index = data->row_offset, row_iter = 0; row_iter < MAX_ROUNDS; row_index = (row_index + 1U) % ROW_NUM, ++row_iter)
		{
			TRACE("Flip-N round %d of %d", row_iter + 1, MAX_ROUNDS);
			memcpy(backup, data->table[row_index], sizeof(uint8_t) * ROW_LEN);
			for (uint_fast16_t refine_attempt = 0; refine_attempt < 11U; ++refine_attempt)
			{
				const uint32_t flip_count = gaussian_noise_next(rand, &bxmller, 1.4142, 4U, HASH_LEN);
				for (uint_fast16_t refine_step = 0; refine_step < 997U; ++refine_step)
				{
					flip_rand_n(data->table[row_index], rand, flip_count);
					const uint_fast32_t error_next = get_error_table(data->table, error);
					if (error_next < error)
					{
						TRACE("Improved by flip-%u (%08X -> %08X)", flip_count, error, error_next);
						memcpy(backup, data->table[row_index], sizeof(uint8_t) * ROW_LEN);
						row_iter = -1;
						improvedFlag = true;
						error = error_next;
					}
					else
					{
						memcpy(data->table[row_index], backup, sizeof(uint8_t) * ROW_LEN);
					}
				}
			}
			if (CHECK_SUCCESS(error, error_initial, data->threshold))
			{
				TRACE("Success by flip-n <<<---");
				goto success;
			}
			CHECK_TERMINATION();
		}
		//-------------------//
		//----- RESTART -----//
		//-------------------//
		if ((reduceFlag) && (data->threshold > 1U))
		{
			data->threshold =  data->threshold / 2U;
			reduceFlag = false;
			TRACE("Threshold reduced to: %u", data->threshold);
			if (CHECK_SUCCESS(error, error_initial, data->threshold))
			{
				TRACE("Success by lower threshold :-P <<<---");
				goto success;
			}
		}
		else
		{
			reduceFlag = true;
		}
		TRACE("Restarting!");
	}

success:
	if (get_error_table(data->table, UINT_FAST32_MAX) >= error_initial)
	{
		fprintf(stderr, "ERROR MISCOMPARE!\n");
		abort();
	}

	MUTEX_LOCK(data->mutex);
	if (SEM_TRYWAIT(data->stop))
	{
		MUTEX_UNLOCK(data->mutex);
		return data->table;
	}

	SEM_POST(data->stop, THREAD_COUNT);
	MUTEX_UNLOCK(data->mutex);
	return data->table; /*success*/
}

static void* thread_spin(void *const param)
{
	thread_spin_t *const data = (thread_spin_t*)param;
	time_t ref = time(NULL);
	for (;;)
	{
		const time_t curr = time(NULL);
		const time_t diff = curr - ref;
		ref = curr;
		if (diff > 5)
		{
			data->missed_cticks += (diff - 5);
			TRACE("Missed timer ticks: %lld (+%lld)", data->missed_cticks, (diff - 5));
		}
		if (SEM_TRYWAIT(data->stop))
		{
			printf("\b\b\b[!]");
			return NULL;
		}
		printf("\b\b\b[%c]", SPINNER[data->spin_pos]);
		data->spin_pos = (data->spin_pos + 1) % 4;
		_sleep(997U);
	}
}

//-----------------------------------------------------------------------------
// Save / Load
//-----------------------------------------------------------------------------

static bool save_table_data(const uint8_t table[ROW_NUM][ROW_LEN], const double threshold_in, const wchar_t *const filename)
{
	wchar_t filename_temp[_MAX_PATH];
	swprintf_s(filename_temp, _MAX_PATH, L"%s~%X", filename, make_seed());
	FILE *const file = _wfopen(filename_temp, L"wb");
	if (file)
	{
		bool success = true;
		const uint64_t magic_number = MAGIC_NUMBER;
		const uint32_t hash_len = HASH_LEN, distance_min = DISTANCE_MIN, threshold = (uint32_t)(round(threshold_in * 1024.0));
		fwrite(&magic_number, sizeof(uint64_t), 1, file);
		fwrite(&hash_len, sizeof(uint32_t), 1, file);
		fwrite(&distance_min, sizeof(uint32_t), 1, file);
		fwrite(&threshold, sizeof(uint32_t), 1, file);
		for (uint32_t i = 0; i < ROW_NUM; ++i)
		{
			const uint32_t checksum = adler32(&table[i][0], ROW_LEN);
			fwrite(&checksum, sizeof(uint32_t), 1, file);
			fwrite(&table[i][0], sizeof(uint8_t), ROW_LEN, file);
		}
		if (ferror(file))
		{
			printf("ERROR: Failed to write table data!\n");
			success = false;
		}
		fclose(file);
		if (success)
		{
			for (size_t i = 0; i < 42; ++i)
			{
				if (_wremove(filename))
				{
					if (errno != ENOENT)
					{
						printf("ERROR: Failed to delete existing file!\n");
						sched_yield();
						continue;
					}
				}
				break;
			}
			if (_wrename(filename_temp, filename))
			{
				printf("ERROR: Failed to rename temp file!\n");
				success = false;
			}
		}
		else
		{
			_wremove(filename_temp);
		}
		return success;
	}
	else
	{
		printf("ERROR: Failed to open table file for writing!\n");
		return false;
	}
}

static bool load_table_data(uint8_t table[ROW_NUM][ROW_LEN], double *const threshold_out, const wchar_t *const filename)
{
	FILE *const file = _wfopen(filename, L"rb");
	if (file)
	{
		uint64_t magic_number;
		uint32_t hash_len, distance_min, threshold;
		fread(&magic_number, sizeof(uint64_t), 1, file);
		fread(&hash_len, sizeof(uint32_t), 1, file);
		fread(&distance_min, sizeof(uint32_t), 1, file);
		fread(&threshold, sizeof(uint32_t), 1, file);
		if (ferror(file) || feof(file))
		{
			printf("ERROR: Failed to read the table header!\n");
			goto failed;
		}
		if (magic_number != MAGIC_NUMBER)
		{
			printf("ERROR: Table file format could not be recognized!\n");
			goto failed;
		}
		if ((hash_len != HASH_LEN) || (distance_min != DISTANCE_MIN))
		{
			printf("ERROR: Table properties are incompatibe with this instance!\n");
			goto failed;
		}
		for (uint_fast16_t i = 0; i < ROW_NUM; ++i)
		{
			uint32_t checksum_expected;
			if ((fread(&checksum_expected, sizeof(uint32_t), 1, file) != 1) || (fread(&table[i][0], sizeof(uint8_t), ROW_LEN, file) != ROW_LEN))
			{
				printf("ERROR: Failed to read table data from file!\n");
				goto failed;
			}
			if (adler32(&table[i][0], ROW_LEN) != checksum_expected)
			{
				printf("ERROR: Table checksum does *not* match table contents!\n");
				goto failed;
			}
		}
		fclose(file);
		*threshold_out = clip_dbl(1.0, ((double)threshold) / 1024.0, 1024.0);
		return true;
	failed:
		fclose(file);
		return false;
	}
	else
	{
		printf("ERROR: Failed to open table file for reading!\n");
		return false;
	}
}

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][ROW_LEN];
static thread_data_t g_thread_data[THREAD_COUNT];
static thread_spin_t g_thread_spin;
static pthread_t g_thread_id[THREAD_COUNT + 1];

int wmain(int argc, wchar_t *argv[])
{
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	uint_fast32_t error = UINT_FAST32_MAX;
	FILE *file_out = NULL;
	double threshold = 256.0;

	printf("MHash GenTableXOR V2 [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min: %d, Threads: %d, MSVC: %u\n\n", HASH_LEN, DISTANCE_MIN, THREAD_COUNT, _MSC_FULL_VER);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	if (argc < 2)
	{
		printf("Table file not specified!\n\n");
		printf("Usage:\n");
		printf("   GenTables_XOR.exe <table_file>\n\n");
		return 1;
	}

	for (size_t i = 0; i < ROW_NUM; i++)
	{
		memset(&g_table[i][0], 0, sizeof(uint8_t) * ROW_LEN);
	}

	memset(&g_thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&g_thread_spin, 0, sizeof(g_thread_spin));
	memset(&g_thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	if (_waccess(argv[1], 4) == 0)
	{
		printf("Loading existing table data and proceeding...\n");
		if (!load_table_data(g_table, &threshold, argv[1]))
		{
			return 1;
		}
		error = get_error_table(g_table, UINT_FAST32_MAX);
	}
	else
	{
		msws_t rand;
		printf("Generating new initial random table...\n");
		msws_init(rand, make_seed());
		for (int_fast32_t round = 0; round < 999983; ++round)
		{
			for (uint_fast16_t i = 0; i < ROW_NUM; i++)
			{
				msws_bytes(rand, g_thread_data[0].table[i], ROW_LEN);
			}
			const uint_fast32_t error_next = get_error_table(g_thread_data[0].table, error);
			if (error_next < error)
			{
				TRACE("Improved by rand-init (%08X -> %08X)", error, error_next);
				copy_table(g_table, g_thread_data[0].table);
				error = error_next;
			}
		}
	}

	while(error > 0)
	{
		char time_string[64];
		const time_t ref_time = time(NULL);
		printf("\aRemaining error: %u (total: %u) [%c]", ERROR_DEC_MAX(error), ERROR_DEC_ACC(error), '*');

		g_thread_spin.stop = &stop_flag;
		g_thread_spin.missed_cticks = 0;

		PTHREAD_CREATE(&g_thread_id[THREAD_COUNT], NULL, thread_spin, &g_thread_spin);

		for (uint_fast16_t t = 0; t < THREAD_COUNT; t++)
		{
			g_thread_data[t].stop = &stop_flag;
			g_thread_data[t].mutex = &stop_mutex;
			g_thread_data[t].row_offset = t * ((ROW_NUM) / THREAD_COUNT);
			g_thread_data[t].threshold = (uint_fast32_t)round(threshold);
			copy_table(g_thread_data[t].table, g_table);
			PTHREAD_CREATE(&g_thread_id[t], NULL, thread_main, &g_thread_data[t]);
			PTHREAD_SET_PRIORITY(g_thread_id[t], -15);
		}

		for (uint_fast16_t t = 0; t < THREAD_COUNT; t++)
		{
			void *return_value = NULL;
			PTHREAD_JOIN(g_thread_id[t], &return_value);
			if (return_value)
			{
				const uint_fast32_t error_thread = get_error_table(g_thread_data[t].table, error);
				if (error_thread < error)
				{
					copy_table(g_table, g_thread_data[t].table);
					error = error_thread;
				}
			}
		}

		PTHREAD_JOIN(g_thread_id[THREAD_COUNT], NULL);
		threshold = clip_dbl(1.0, threshold * (TARGET_PROCESSING_TIME / ((double)(time(NULL) - (ref_time + g_thread_spin.missed_cticks)))), 1024.0);

		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);

		if (!save_table_data(g_table, threshold, argv[1]))
		{
			getchar(); /*failed to save current table data*/
		}
	}

	printf("\n-----\n\n");

	//dump_table(stdout);
	if (fopen_s(&file_out, "table_XOR." DISTANCE_STR ".txt", "w") == 0)
	{
		//dump_table(file_out);
		fclose(file_out);
	}

	printf("\n-----\n\n");

	for (uint_fast16_t i = 0; i < ROW_NUM; i++)
	{
		for (uint_fast16_t j = 0; j < ROW_NUM; j++)
		{
			if (i == j)
			{
				continue; //i==j
			}
			if (get_distance_rows(g_table, i, j) < DISTANCE_MIN)
			{
				crit_exit("FATAL: Table verification has failed!");
			}
		}
	}

	SEM_DESTROY(&stop_flag);
	MUTEX_DESTROY(&stop_mutex);

	printf("COMPLETED.\n\n");
	system("shutdown /s /t 180");
	return getchar();
}
