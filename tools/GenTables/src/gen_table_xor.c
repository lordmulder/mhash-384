/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Generate tables utility program                                                     */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                  */
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
#include "twister.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <io.h>

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384

#define DISTANCE_MIN 180
#define DISTANCE_MAX DISTANCE_MIN + 32

#define THREAD_COUNT 8

#define ROW_NUM (UINT8_MAX+2)           /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

#define MAGIC_NUMBER 0X3C6058A7C1132CB2ui64

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][ROW_LEN];
static uint8_t g_thread_buffer[THREAD_COUNT][ROW_LEN];

static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

static inline bool check_distance(const size_t index, const uint8_t *const row_buffer)
{
	for (size_t k = 0; k < index; k++)
	{
		const uint32_t dist = hamming_distance(&g_table[k][0], row_buffer, ROW_LEN);
		if ((dist > DISTANCE_MAX) || (dist < DISTANCE_MIN))
		{
			return false;
		}
	}
	return true;
}

static dump_table(FILE *out)
{
	for (size_t i = 0; i < ROW_NUM; i++)
	{
		fprintf(out, "%02X: ", (uint32_t)(i & 0xFF));
		for (size_t j = 0; j < ROW_LEN; j++)
		{
			fprintf(out, "0x%02X,", g_table[i][j]);
		}
		fprintf(out, "\n");
	}
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
	twister_t *rand;
}
thread_data_t;

static void* thread_main(void *const param)
{
	const thread_data_t *const data = ((const thread_data_t*)param);
	uint16_t check_cnt = 0;
	do
	{
		if (++check_cnt == 0)
		{
			if (SEM_TRYWAIT(data->stop))
			{
				return NULL;
			}
		}
		rand_next_bytes(data->rand, data->row_buffer, ROW_LEN);
	} 
	while (!check_distance(data->index, data->row_buffer));
	
	MUTEX_LOCK(data->mutex);
	if (SEM_TRYWAIT(data->stop))
	{
		MUTEX_UNLOCK(data->mutex);
		return NULL;
	}
	SEM_POST(data->stop, THREAD_COUNT);
	MUTEX_UNLOCK(data->mutex);
	return data->row_buffer;
}

static void* thread_spin(void *const param)
{
	unsigned long delay = 0;
	sem_t *const stop = ((sem_t*)param);
	for (;;)
	{
		if (SEM_TRYWAIT(stop))
		{
			return NULL;
		}
		_sleep(delay);
		if (delay >= 1000)
		{
			printf("\b\b\b[%c]", SPINNER[g_spinpos]);
			g_spinpos = (g_spinpos + 1) % 4;
		}
		else
		{
			delay = (delay > 0) ? (2 * delay) : 1;
		}
	}
}

//-----------------------------------------------------------------------------
// Save / Load
//-----------------------------------------------------------------------------

static bool save_table_data(const wchar_t *const filename, const size_t index)
{
	FILE *const file = _wfopen(filename, L"wb");
	if (file)
	{
		bool success = true;
		const uint64_t magic_number = MAGIC_NUMBER;
		const uint32_t hash_len = HASH_LEN, distance_min = DISTANCE_MIN, distance_max = DISTANCE_MAX, rows_completed = (uint32_t)index;
		fwrite(&magic_number, sizeof(uint64_t), 1, file);
		fwrite(&hash_len, sizeof(uint32_t), 1, file);
		fwrite(&distance_min, sizeof(uint32_t), 1, file);
		fwrite(&distance_max, sizeof(uint32_t), 1, file);
		fwrite(&rows_completed, sizeof(uint32_t), 1, file);
		for (size_t i = 0; i < index; ++i)
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
		return success;
	}
	else
	{
		printf("ERROR: Failed to open table file for writing!\n");
		return false;
	}
}

static bool load_table_data(const wchar_t *const filename, size_t *const index)
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
		}
		else
		{
			if (magic_number != MAGIC_NUMBER)
			{
				printf("ERROR: Table file format could not be recognized!\n");
				success = false;
			}
			else
			{
				if ((hash_len != HASH_LEN) || (distance_min != DISTANCE_MIN) || (distance_max != DISTANCE_MAX) || (rows_completed > ROW_NUM))
				{
					printf("ERROR: Table properties are incompatibe with this instance!\n");
					success = false;
				}
				else
				{
					for (size_t i = 0; i < rows_completed; ++i)
					{
						uint32_t checksum_expected;
						if ((fread(&checksum_expected, sizeof(uint32_t), 1, file) != 1) || (fread(&g_table[i][0], sizeof(uint8_t), ROW_LEN, file) != ROW_LEN))
						{
							printf("ERROR: Failed to read table data from file!\n");
							success = false;
							break;
						}
						else if (adler32(&g_table[i][0], ROW_LEN) != checksum_expected)
						{
							printf("ERROR: Table checksum does *not* match table contents!\n");
							success = false;
							break;
						}
					}
				}
			}
		}
		fclose(file);
		if (success && index)
		{
			*index = rows_completed;
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
	twister_t thread_rand[THREAD_COUNT];
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	FILE *file_out = NULL;
	size_t rows_completed = 0;

	printf("MHash GenTableXOR [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min/Max: %d/%d, Threads: %d\n\n", HASH_LEN, DISTANCE_MIN, DISTANCE_MAX, THREAD_COUNT);

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
		rand_init(&thread_rand[t], make_seed());
	}

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	if (_waccess(argv[1], 4) == 0)
	{
		printf("Loading existing table data and proceeding...\n");
		if (!load_table_data(argv[1], &rows_completed))
		{
			return 1;
		}
	}

	for (size_t i = rows_completed; i < ROW_NUM; i++)
	{
		char time_string[64];
		printf("Row %03u of %03u [%c]", (uint32_t)(i+1U), ROW_NUM, SPINNER[g_spinpos]);
		g_spinpos = (g_spinpos + 1) % 4;

		PTHREAD_CREATE(&thread_id[THREAD_COUNT], NULL, thread_spin, &stop_flag);
		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			thread_data[t].index = i;
			thread_data[t].row_buffer = &g_thread_buffer[t][0];
			thread_data[t].stop = &stop_flag;
			thread_data[t].mutex = &stop_mutex;
			thread_data[t].rand = &thread_rand[t];
			PTHREAD_CREATE(&thread_id[t], NULL, thread_main, &thread_data[t]);
		}

		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			void *return_value = NULL;
			PTHREAD_JOIN(thread_id[t], &return_value);
			if (return_value)
			{
				memcpy(&g_table[i][0], return_value, sizeof(uint8_t) * ROW_LEN);
			}
		}

		PTHREAD_JOIN(thread_id[THREAD_COUNT], NULL);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);

		if (!save_table_data(argv[1], i))
		{
			return 1; /*failed to save current table data*/
		}
	}

	printf("\n-----\n\n");

	dump_table(stdout);
	if (fopen_s(&file_out, "table_out." DISTANCE_STR ".txt", "w") == 0)
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
			const uint32_t dist = hamming_distance(&g_table[i][0], &g_table[j][0], ROW_LEN);
			if ((dist > DISTANCE_MAX) || (dist < DISTANCE_MIN))
			{
				crit_exit("FATAL: Table verification has failed!");
			}
		}
	}

	SEM_DESTROY(&stop_flag);
	MUTEX_DESTROY(&stop_mutex);

	printf("COMPLETED.\n\n");
	return getchar();
}
