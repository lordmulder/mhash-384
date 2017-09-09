/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Generate tables utility program                                                     */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                             */
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
#define DISTANCE_MIN 182U

#define THREAD_COUNT 8U

#undef ENABLE_STATS
#undef ENABLE_TRACE

#define ROW_NUM (UINT8_MAX+2)           /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

#define MAGIC_NUMBER 0x3C6058A7C1132CB2ui64
#define THREAD_ID (pthread_getw32threadid_np(pthread_self()))

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][ROW_LEN];
static uint8_t g_thread_buffer[THREAD_COUNT][ROW_LEN];

static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

#ifdef ENABLE_STATS
#include <intrin.h>
static volatile long long g_stat_too_hi = 0i64;
static volatile long long g_stat_too_lo = 0i64;
#endif //ENABLE_STATS

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

static inline void flip_bit_at(uint8_t *const row_buffer, const size_t pos)
{
	row_buffer[pos >> 3] ^= ((uint8_t)(1U << (pos & 0x7)));
}

static inline void flip_rand_n(uint8_t *const row_buffer, msws_t rand, const uint32_t n)
{
	bool taken[HASH_LEN];
	memset(&taken, 0, sizeof(bool) * HASH_LEN);
	for (uint32_t i = 0; i < n; ++i)
	{
		size_t next;
		do
		{
			next = msws_uint32_max(rand, HASH_LEN);
		}
		while (taken[next]);
		flip_bit_at(row_buffer, next);
		taken[next] = true;
	}
}

static inline bool check_distance_rows(const uint32_t distance_max, const size_t index_1, const size_t index_2)
{
	const uint32_t dist = hamming_distance(&g_table[index_1][0], &g_table[index_2][0], ROW_LEN);
	return (dist <= distance_max) && (dist >= DISTANCE_MIN);
}

static inline uint32_t check_distance_buff(const uint32_t distance_max, const size_t index, const uint8_t *const row_buffer, const uint32_t limit)
{
	uint32_t error = 0U;
#ifdef ENABLE_STATS
	bool reason;
#endif //ENABLE_STATS
	for (size_t k = 0; k < index; k++)
	{

		const uint32_t dist = hamming_distance(&g_table[k][0], row_buffer, ROW_LEN);
		if (dist > distance_max)
		{
			const uint32_t current = dist - distance_max;
			if (current > error)
			{
#ifdef ENABLE_STATS
				reason = false;
#endif //ENABLE_STATS
				if ((error = current) >= limit)
				{
					break; /*early termination*/
				}
			}
		}
		else if (dist < DISTANCE_MIN)
		{
			const uint32_t current = DISTANCE_MIN - dist;
			if (current > error)
			{
#ifdef ENABLE_STATS
				reason = true;
#endif //ENABLE_STATS
				if ((error = current) >= limit)
				{
					break; /*early termination*/
				}
			}
		}
	}
#ifdef ENABLE_STATS
	_InterlockedIncrement64(reason ? &g_stat_too_lo : &g_stat_too_hi);
#endif //ENABLE_STATS
	return error;
}

static void dump_table(FILE *out)
{
	fputs("uint8_t MHASH_384_TABLE_XOR[UINT8_MAX+2][MHASH_384_LEN] =\n{\n", out);
	for (size_t i = 0; i < ROW_NUM; i++)
	{
		fputs("\t{ ", out);
		for (size_t j = 0; j < ROW_LEN; j++)
		{
			if (j > 0)
			{
				fputc(',', out);
			}
			fprintf(out, "0x%02X", g_table[i][j]);
		}
		fprintf(out, " }%s /*%02X*/\n", (i != (ROW_NUM - 1)) ? "," : " ", (uint32_t)(i % 0x100));
	}
	fputs("};\n", out);
}

//-----------------------------------------------------------------------------
// Thread function
//-----------------------------------------------------------------------------

typedef struct
{
	size_t index;
	uint8_t *row_buffer;
	sem_t *stop;
	pthread_mutex_t *mutex;
	uint32_t distance_max;
}
thread_data_t;

static void* thread_main(void *const param)
{
	thread_data_t *const data = (thread_data_t*)param;
	msws_t rand;
	bxmller_t bxmller;
	uint8_t temp[ROW_LEN];
	for(;;)
	{
		TRACE("Maximum distance: %u", data->distance_max);
		msws_init(rand, make_seed());
		gaussian_noise_init(&bxmller);
		msws_bytes(rand, data->row_buffer, ROW_LEN);
		uint32_t error = check_distance_buff(data->distance_max, data->index, data->row_buffer, HASH_LEN);
		if(error > 0U)
		{
			for (int32_t round = 0; round < 4999; ++round)
			{
				if (!(round & 0x3FF))
				{
					if (SEM_TRYWAIT(data->stop))
					{
						return NULL;
					}
				}
				for (size_t rand_loop = 0; rand_loop < 99991U; ++rand_loop)
				{
					msws_bytes(rand, temp, ROW_LEN);
					const uint32_t next_error = check_distance_buff(data->distance_max, data->index, temp, error);
					if (next_error < error)
					{
						round = -1;
						memcpy(data->row_buffer, temp, sizeof(uint8_t) * ROW_LEN);
						if (!((error = next_error) > 0U))
						{
							TRACE("Success by rand-init <<<---");
							goto success;
						}
					}
				}
			}
			for (int32_t round = 0; round < 743; ++round)
			{
				TRACE("Optimizer round %u of 743", round);
				if (!round)
				{
					for (size_t xchg_pos = 0U; xchg_pos < ROW_LEN; ++xchg_pos)
					{
						uint8_t value = (uint8_t)msws_uint32(rand);
						uint8_t original = data->row_buffer[xchg_pos];
						for (size_t xchg_cnt = 0U; xchg_cnt <= UINT8_MAX; ++xchg_cnt, ++value)
						{
							data->row_buffer[xchg_pos] = value;
							const uint32_t next_error = check_distance_buff(data->distance_max, data->index, data->row_buffer, error);
							if (next_error < error)
							{
								TRACE("Improved by xchg-byte");
								original = value;
								round = -1;
								if (!((error = next_error) > 0U))
								{
									TRACE("Success by xchg-byte <<<---");
									goto success;
								}
							}
						}
						data->row_buffer[xchg_pos] = original;
					}
					for (size_t flip_pos_w = 0U; flip_pos_w < HASH_LEN; ++flip_pos_w)
					{
						if (SEM_TRYWAIT(data->stop))
						{
							return NULL;
						}
						flip_bit_at(data->row_buffer, flip_pos_w);
						bool revert_w = true;
						const uint32_t next_error = check_distance_buff(data->distance_max, data->index, data->row_buffer, error);
						if (next_error < error)
						{
							TRACE("Improved by flip-1");
							revert_w = false;
							round = -1;
							if (!((error = next_error) > 0U))
							{
								TRACE("success by flip-1 <<<---");
								goto success;
							}
						}
						for (size_t flip_pos_x = flip_pos_w + 1U; flip_pos_x < HASH_LEN; ++flip_pos_x)
						{
							flip_bit_at(data->row_buffer, flip_pos_x);
							bool revert_x = true;
							const uint32_t next_error = check_distance_buff(data->distance_max, data->index, data->row_buffer, error);
							if (next_error < error)
							{
								TRACE("Improved by flip-2");
								revert_w = false;
								revert_x = false;
								round = -1;
								if (!((error = next_error) > 0U))
								{
									TRACE("success by flip-2 <<<---");
									goto success;
								}
							}
							for (size_t flip_pos_y = flip_pos_x + 1U; flip_pos_y < HASH_LEN; ++flip_pos_y)
							{
								flip_bit_at(data->row_buffer, flip_pos_y);
								bool revert_y = true;
								const uint32_t next_error = check_distance_buff(data->distance_max, data->index, data->row_buffer, error);
								if (next_error < error)
								{
									TRACE("Improved by flip-3");
									revert_w = false;
									revert_x = false;
									revert_y = false;
									round = -1;
									if (!((error = next_error) > 0U))
									{
										TRACE("success by flip-3 <<<---");
										goto success;
									}
								}
								for (size_t flip_pos_z = flip_pos_y + 1U; flip_pos_z < HASH_LEN; ++flip_pos_z)
								{
									flip_bit_at(data->row_buffer, flip_pos_z);
									const uint32_t next_error = check_distance_buff(data->distance_max, data->index, data->row_buffer, error);
									if (next_error < error)
									{
										TRACE("Improved by flip-4");
										revert_w = false;
										revert_x = false;
										revert_y = false;
										round = -1;
										if (!((error = next_error) > 0U))
										{
											TRACE("success by flip-4 <<<---");
											goto success;
										}
									}
									else
									{
										flip_bit_at(data->row_buffer, flip_pos_z);
									}
								}
								if (revert_y)
								{
									flip_bit_at(data->row_buffer, flip_pos_y);
								}
							}
							if (revert_x)
							{
								flip_bit_at(data->row_buffer, flip_pos_x);
							}
						}
						if (revert_w)
						{
							flip_bit_at(data->row_buffer, flip_pos_w);
						}
					}
				}
				for (uint_fast8_t rand_mode = 0; rand_mode < 14U; ++rand_mode)
				{
					memcpy(temp, &data->row_buffer, sizeof(uint8_t) * ROW_LEN);
					for (uint_fast16_t rand_loop = 0; rand_loop < 29927U; ++rand_loop)
					{
						switch (rand_mode)
						{
							case 0x0: msws_bytes(rand, &temp[0U * (ROW_LEN / 2U)], ROW_LEN / 2U); break;
							case 0x1: msws_bytes(rand, &temp[1U * (ROW_LEN / 2U)], ROW_LEN / 2U); break;
							case 0x2: msws_bytes(rand, &temp[0U * (ROW_LEN / 4U)], ROW_LEN / 4U); break;
							case 0x3: msws_bytes(rand, &temp[1U * (ROW_LEN / 4U)], ROW_LEN / 4U); break;
							case 0x4: msws_bytes(rand, &temp[2U * (ROW_LEN / 4U)], ROW_LEN / 4U); break;
							case 0x5: msws_bytes(rand, &temp[3U * (ROW_LEN / 4U)], ROW_LEN / 4U); break;
							case 0x6: msws_bytes(rand, &temp[0U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0x7: msws_bytes(rand, &temp[1U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0x8: msws_bytes(rand, &temp[2U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0x9: msws_bytes(rand, &temp[3U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0xA: msws_bytes(rand, &temp[4U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0xB: msws_bytes(rand, &temp[5U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0xC: msws_bytes(rand, &temp[6U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							case 0xD: msws_bytes(rand, &temp[7U * (ROW_LEN / 8U)], ROW_LEN / 8U); break;
							default: abort();
						}
						const uint32_t next_error = check_distance_buff(data->distance_max, data->index, temp, error);
						if (next_error < error)
						{
							TRACE("Improved by rand-replace (%u)", rand_mode);
							round = -1;
							memcpy(data->row_buffer, temp, sizeof(uint8_t) * ROW_LEN);
							if (!((error = next_error) > 0U))
							{
								TRACE("Success by rand-replace <<<---");
								goto success;
							}
						}
					}
				}
				for (size_t refine_loop = 0; refine_loop < 9973U; ++refine_loop)
				{
					if (!(refine_loop & 0x3FF))
					{
						if (SEM_TRYWAIT(data->stop))
						{
							return NULL;
						}
					}
					const uint32_t flip_count = gaussian_noise_next(rand, &bxmller, 3.14159, 5U, HASH_LEN);
					for (size_t refine_step = 0; refine_step < 997U; ++refine_step)
					{
						memcpy(temp, data->row_buffer, sizeof(uint8_t) * ROW_LEN);
						flip_rand_n(temp, rand, flip_count);
						const uint32_t next_error = check_distance_buff(data->distance_max, data->index, temp, error);
						if (next_error < error)
						{
							TRACE("Improved by rand-flip (%u)", flip_count);
							round = -1;
							memcpy(data->row_buffer, temp, sizeof(uint8_t) * ROW_LEN);
							if (!((error = next_error) > 0U))
							{
								TRACE("Success by rand-flip (%u) <<<---", flip_count);
								goto success;
							}
						}
					}
				}
			}
			TRACE("Restarting");
			data->distance_max = min(data->distance_max + 1U, HASH_LEN); /*bump max. distance*/
		}
		else
		{
			break; /*success*/
		}
	}

success:
	if (check_distance_buff(data->distance_max, data->index, data->row_buffer, HASH_LEN))
	{
		fprintf(stderr, "ERROR MISCOMPARE!\n");
		abort();
	}

	MUTEX_LOCK(data->mutex);
	if (SEM_TRYWAIT(data->stop))
	{
		MUTEX_UNLOCK(data->mutex);
		return NULL;
	}

	SEM_POST(data->stop, THREAD_COUNT);
	MUTEX_UNLOCK(data->mutex);
	return data->row_buffer; /*success*/
}

static void* thread_spin(void *const param)
{
	unsigned long delay = 1U;
	sem_t *const stop = ((sem_t*)param);
	for (;;)
	{
		if (SEM_TRYWAIT(stop))
		{
			printf("\b\b\b[!]");
			return NULL;
		}
		_sleep(delay);
		if (delay >= 500)
		{
#ifdef ENABLE_STATS
			const long long s_too_lo = g_stat_too_lo, s_too_hi = g_stat_too_hi;
			const long long s_too_ac = s_too_lo + s_too_hi;
			printf("Too low: %lld (%.2f), too high: %lld (%.2f)\n", s_too_lo, s_too_lo / ((double)s_too_ac), s_too_hi, s_too_hi / ((double)s_too_ac));
#endif //ENABLE_STATS
			printf("\b\b\b[%c]", SPINNER[g_spinpos]);
			g_spinpos = (g_spinpos + 1) % 4;
		}
		else
		{
			delay *= 2U;
		}
	}
}

//-----------------------------------------------------------------------------
// Save / Load
//-----------------------------------------------------------------------------

static bool save_table_data(const wchar_t *const filename, const size_t rows_completed_in, const uint32_t current_dist_max)
{
	wchar_t filename_temp[_MAX_PATH];
	swprintf_s(filename_temp, _MAX_PATH, L"%s~%X", filename, make_seed());
	FILE *const file = _wfopen(filename_temp, L"wb");
	if (file)
	{
		bool success = true;
		const uint64_t magic_number = MAGIC_NUMBER;
		const uint32_t hash_len = HASH_LEN, distance_min = DISTANCE_MIN, distance_max = current_dist_max, rows_completed = (uint32_t)rows_completed_in;
		fwrite(&magic_number, sizeof(uint64_t), 1, file);
		fwrite(&hash_len, sizeof(uint32_t), 1, file);
		fwrite(&distance_min, sizeof(uint32_t), 1, file);
		fwrite(&distance_max, sizeof(uint32_t), 1, file);
		fwrite(&rows_completed, sizeof(uint32_t), 1, file);
		for (uint32_t i = 0; i < rows_completed; ++i)
		{
			const uint32_t checksum = adler32(&g_table[i][0], ROW_LEN);
			fwrite(&checksum, sizeof(uint32_t), 1, file);
			fwrite(&g_table[i][0], sizeof(uint8_t), ROW_LEN, file);
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

static bool load_table_data(const wchar_t *const filename, size_t *const rows_completed_out, uint32_t *const dist_max_out)
{
	FILE *const file = _wfopen(filename, L"rb");
	if (file)
	{
		bool success = true;
		uint64_t magic_number;
		uint32_t hash_len, distance_min, distance_max, rows_completed;
		fread(&magic_number, sizeof(uint64_t), 1, file);
		fread(&hash_len, sizeof(uint32_t), 1, file);
		fread(&distance_min, sizeof(uint32_t), 1, file);
		fread(&distance_max, sizeof(uint32_t), 1, file);
		fread(&rows_completed, sizeof(uint32_t), 1, file);
		if (ferror(file) || feof(file))
		{
			printf("ERROR: Failed to read the table header!\n");
			success = false;
			goto failed;
		}
		if (magic_number != MAGIC_NUMBER)
		{
			printf("ERROR: Table file format could not be recognized!\n");
			success = false;
			goto failed;
		}
		if ((hash_len != HASH_LEN) || (distance_min != DISTANCE_MIN) || (distance_max > HASH_LEN) || (rows_completed > ROW_NUM))
		{
			printf("ERROR: Table properties are incompatibe with this instance!\n");
			success = false;
			goto failed;
		}
		for (size_t i = 0; i < rows_completed; ++i)
		{
			uint32_t checksum_expected;
			if ((fread(&checksum_expected, sizeof(uint32_t), 1, file) != 1) || (fread(&g_table[i][0], sizeof(uint8_t), ROW_LEN, file) != ROW_LEN))
			{
				printf("ERROR: Failed to read table data from file!\n");
				success = false;
				goto failed;
			}
			if (adler32(&g_table[i][0], ROW_LEN) != checksum_expected)
			{
				printf("ERROR: Table checksum does *not* match table contents!\n");
				success = false;
				goto failed;
			}
			for (size_t j = 0; j < i; j++)
			{
				if (!check_distance_rows(distance_max, i, j))
				{
					printf("ERROR: Table distance verification has failed!\n");
					success = false;
					goto failed;
				}
			}
		}
	failed:
		fclose(file);
		if (success && rows_completed_out)
		{
			*rows_completed_out = (size_t)rows_completed;
			*dist_max_out = distance_max;
		}
		return success;
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

int wmain(int argc, wchar_t *argv[])
{
	pthread_t thread_id[THREAD_COUNT + 1];
	thread_data_t thread_data[THREAD_COUNT];
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	FILE *file_out = NULL;
	size_t initial_row_index = 0;
	uint32_t distance_max = DISTANCE_MIN;

	printf("MHash GenTableXOR [%s]\n\n", __DATE__);
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

	memset(&thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);
	for (size_t t = 0; t < THREAD_COUNT; t++)
	{
		memset(&g_thread_buffer[t][0], 0, sizeof(uint8_t) * ROW_LEN);
	}

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	if (_waccess(argv[1], 4) == 0)
	{
		printf("Loading existing table data and proceeding...\n");
		if (!load_table_data(argv[1], &initial_row_index, &distance_max))
		{
			return 1;
		}
	}

	for (size_t i = initial_row_index; i < ROW_NUM; i++)
	{
		char time_string[64];
		printf("\aRow %03u of %03u [%c]", (uint32_t)(i+1U), ROW_NUM, SPINNER[g_spinpos]);
		g_spinpos = (g_spinpos + 1) % 4;
#ifdef ENABLE_STATS
		g_stat_too_hi = g_stat_too_lo = 0i64;
#endif //INCREMENT_STAT

		PTHREAD_CREATE(&thread_id[THREAD_COUNT], NULL, thread_spin, &stop_flag);
		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			thread_data[t].index = i;
			thread_data[t].row_buffer = &g_thread_buffer[t][0];
			thread_data[t].stop = &stop_flag;
			thread_data[t].mutex = &stop_mutex;
			thread_data[t].distance_max = distance_max;
			PTHREAD_CREATE(&thread_id[t], NULL, thread_main, &thread_data[t]);
			PTHREAD_SET_PRIORITY(thread_id[t], -15);
		}

		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			void *return_value = NULL;
			PTHREAD_JOIN(thread_id[t], &return_value);
			if (return_value)
			{
				memcpy(&g_table[i][0], thread_data[t].row_buffer, sizeof(uint8_t) * ROW_LEN);
				distance_max = max(distance_max, thread_data[t].distance_max);
			}
		}

		PTHREAD_JOIN(thread_id[THREAD_COUNT], NULL);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);

		if (!save_table_data(argv[1], i + 1U, distance_max))
		{
			return 1; /*failed to save current table data*/
		}
	}

	printf("\n-----\n\n");

	dump_table(stdout);
	if (fopen_s(&file_out, "table_XOR." DISTANCE_STR ".txt", "w") == 0)
	{
		dump_table(file_out);
		fclose(file_out);
	}

	printf("\n-----\n\n");

	for (size_t i = 0; i < ROW_NUM; i++)
	{
		for (size_t j = 0; j < ROW_NUM; j++)
		{
			if (i == j)
			{
				continue;
			}
			if (!check_distance_rows(distance_max, i, j))
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
