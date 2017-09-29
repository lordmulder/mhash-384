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
#define DISTANCE_MIN 192U

#define THREAD_COUNT 8U

#undef ENABLE_TRACE

#define ROW_NUM (UINT8_MAX+2)           /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

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

static size_t g_spinpos = 0;

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
	for (uint_fast32_t i = 0; i < n; ++i)
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

static inline uint_fast32_t get_distance_rows(const uint8_t table[ROW_NUM][ROW_LEN], const uint_fast16_t index_1, const uint_fast16_t index_2)
{
	return hamming_distance(table[index_1], table[index_2], ROW_LEN);
}

#define ERROR_ACC(MAX,ACC) (((MAX) << 20U) | ((ACC) & 0xFFFFF))
static inline uint_fast32_t get_error_table(const uint8_t table[ROW_NUM][ROW_LEN], const uint_fast32_t limit)
{
	uint_fast32_t error_max = 0U, error_acc = 0U;
	for (uint_fast16_t i = 0; i < ROW_NUM; i++)
	{
		for (uint_fast16_t j = i + 1U; j < ROW_NUM; j++)
		{
			const uint_fast32_t dist = get_distance_rows(table, i, j);
			if (dist < DISTANCE_MIN)
			{
				const uint_fast32_t current = DISTANCE_MIN - dist;
				error_acc += current;
				if (error_acc > 0xFFFFF)
				{
					printf("Overflow!\n");
					abort();
				}
				if (current > error_max)
				{
					error_max = current;
				}
				if (ERROR_ACC(error_max, error_acc) >= limit)
				{
					return limit; /*early termination*/
				}
			}
		}
	}
	return ERROR_ACC(error_max, error_acc);
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
	sem_t *stop;
	pthread_mutex_t *mutex;
}
thread_data_t;

#define CHECK_TERMINATION() do \
{  \
	if (SEM_TRYWAIT(data->stop))  \
	{ \
		return NULL; \
	} \
} \
while(0) \

static void* thread_main(void *const param)
{
	thread_data_t *const data = (thread_data_t*)param;
	msws_t rand;
	bxmller_t bxmller;
	uint8_t backup[ROW_LEN];
	const uint_fast32_t error_initial = get_error_table(data->table, UINT_FAST32_MAX);
	TRACE("Initial error: %08X", error_initial);
	while(error_initial)
	{
		msws_init(rand, make_seed());
		gaussian_noise_init(&bxmller);
		uint_fast16_t row_offset = msws_uint32_max(rand, ROW_NUM);
		for (int_fast16_t round = 0; round < 97; ++round)
		{
			TRACE("Rand-init round %u of 97", round + 1);
			for (uint_fast16_t row_iter = 0U; row_iter < ROW_NUM; ++row_iter)
			{
				const uint16_t row_index = (row_iter + row_offset) % ROW_NUM;
				memcpy(backup, data->table[row_index], sizeof(uint8_t) * ROW_LEN);
				for (uint_fast16_t rand_loop = 0; rand_loop < 997U; ++rand_loop)
				{
					msws_bytes(rand, data->table[row_index], ROW_LEN);
					const uint_fast32_t error = get_error_table(data->table, error_initial);
					if (error < error_initial)
					{
						TRACE("Improved by rand-init (%08X -> %08X) [row: %03u]", error_initial, error, row_index);
						goto success;
					}
				}
				memcpy(data->table[row_index], backup, sizeof(uint8_t) * ROW_LEN);
			}
			CHECK_TERMINATION();
		}
		TRACE("Restarting");
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

static bool save_table_data(const uint8_t table[ROW_NUM][ROW_LEN], const wchar_t *const filename)
{
	wchar_t filename_temp[_MAX_PATH];
	swprintf_s(filename_temp, _MAX_PATH, L"%s~%X", filename, make_seed());
	FILE *const file = _wfopen(filename_temp, L"wb");
	if (file)
	{
		bool success = true;
		const uint64_t magic_number = MAGIC_NUMBER;
		const uint32_t hash_len = HASH_LEN, distance_min = DISTANCE_MIN;
		fwrite(&magic_number, sizeof(uint64_t), 1, file);
		fwrite(&hash_len, sizeof(uint32_t), 1, file);
		fwrite(&distance_min, sizeof(uint32_t), 1, file);
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

static bool load_table_data(uint8_t table[ROW_NUM][ROW_LEN], const wchar_t *const filename)
{
	FILE *const file = _wfopen(filename, L"rb");
	if (file)
	{
		bool success = true;
		uint64_t magic_number;
		uint32_t hash_len, distance_min;
		fread(&magic_number, sizeof(uint64_t), 1, file);
		fread(&hash_len, sizeof(uint32_t), 1, file);
		fread(&distance_min, sizeof(uint32_t), 1, file);
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
		if ((hash_len != HASH_LEN) || (distance_min != DISTANCE_MIN))
		{
			printf("ERROR: Table properties are incompatibe with this instance!\n");
			success = false;
			goto failed;
		}
		for (uint_fast16_t i = 0; i < ROW_NUM; ++i)
		{
			uint32_t checksum_expected;
			if ((fread(&checksum_expected, sizeof(uint32_t), 1, file) != 1) || (fread(&table[i][0], sizeof(uint8_t), ROW_LEN, file) != ROW_LEN))
			{
				printf("ERROR: Failed to read table data from file!\n");
				success = false;
				goto failed;
			}
			if (adler32(&table[i][0], ROW_LEN) != checksum_expected)
			{
				printf("ERROR: Table checksum does *not* match table contents!\n");
				success = false;
				goto failed;
			}
		}
	failed:
		fclose(file);
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

static uint8_t g_table[ROW_NUM][ROW_LEN];
static thread_data_t thread_data[THREAD_COUNT];
static pthread_t thread_id[THREAD_COUNT + 1];

int wmain(int argc, wchar_t *argv[])
{
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	FILE *file_out = NULL;

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

	memset(&thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	if (_waccess(argv[1], 4) == 0)
	{
		printf("Loading existing table data and proceeding...\n");
		if (!load_table_data(g_table, argv[1]))
		{
			return 1;
		}
	}
	else
	{
		msws_t rand;
		printf("Generating new initial random table...\n");
		msws_init(rand, make_seed());
		for (size_t i = 0; i < ROW_NUM; i++)
		{
			msws_bytes(rand, &g_table[i][0], ROW_LEN);
		}
	}

	uint_fast32_t error = get_error_table(g_table, UINT_FAST32_MAX);
	while(error > 0)
	{
		char time_string[64];
		printf("\aRemaining error: %08X [%c]", error, SPINNER[g_spinpos]);
		g_spinpos = (g_spinpos + 1) % 4;

		PTHREAD_CREATE(&thread_id[THREAD_COUNT], NULL, thread_spin, &stop_flag);
		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			thread_data[t].stop = &stop_flag;
			thread_data[t].mutex = &stop_mutex;
			copy_table(thread_data[t].table, g_table);
			PTHREAD_CREATE(&thread_id[t], NULL, thread_main, &thread_data[t]);
			PTHREAD_SET_PRIORITY(thread_id[t], -15);
		}

		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			void *return_value = NULL;
			PTHREAD_JOIN(thread_id[t], &return_value);
			if (return_value)
			{
				const uint_fast32_t error_thread = get_error_table(thread_data[t].table, error);
				if (error_thread < error)
				{
					copy_table(g_table, thread_data[t].table);
					error = error_thread;
				}
			}
		}

		PTHREAD_JOIN(thread_id[THREAD_COUNT], NULL);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);

		if (!save_table_data(g_table, argv[1]))
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
