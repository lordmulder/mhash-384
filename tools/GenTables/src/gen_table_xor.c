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
#include <stdlib.h>
#include <string.h>
#include <time.h>

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384

#define DISTANCE_MIN 178
#define DISTANCE_MAX 220

#define THREAD_COUNT 8

#define WORD_LEN (HASH_LEN / (8 * sizeof(uint32_t)))
#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint32_t g_table[UINT8_MAX+2][WORD_LEN];
static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

static inline int check_distance(const size_t index, const uint32_t *const row_buffer)
{
	uint32_t min_dist = UINT32_MAX;
	for (size_t k = 0; k < index; k++)
	{
		const uint32_t dist = hamming_distance(&g_table[k][0], row_buffer, WORD_LEN);
		if ((dist > DISTANCE_MAX) || (dist < DISTANCE_MIN))
		{
			return 0;
		}
	}
	return 1;
}

static dump_table(FILE *out)
{
	for (size_t i = 0; i < UINT8_MAX+2; i++)
	{
		fprintf(out, "%02X: ", i & 0xFF);
		for (size_t j = 0; j < WORD_LEN; j++)
		{
			const uint32_t value = g_table[i][j];
			fprintf(out, "0x%02X,", ((value >> 24) & 0xFF));
			fprintf(out, "0x%02X,", ((value >> 16) & 0xFF));
			fprintf(out, "0x%02X,", ((value >> 8) & 0xFF));
			fprintf(out, "0x%02X,", ((value >> 0) & 0xFF));
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
	uint32_t *row_buffer;
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
		for (size_t j = 0; j < WORD_LEN; j++)
		{
			data->row_buffer[j] = rand_next(data->rand);
		}
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
	return ((void*)1);
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
// MAIN
//-----------------------------------------------------------------------------

int main()
{
	pthread_t thread_id[THREAD_COUNT + 1];
	uint32_t thread_buffer[THREAD_COUNT][WORD_LEN];
	thread_data_t thread_data[THREAD_COUNT];
	twister_t thread_rand[THREAD_COUNT];
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	FILE *file_out = NULL;

	printf("MHash GenTableXOR [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min/Max: %d/%d, Threads: %d\n\n", HASH_LEN, DISTANCE_MIN, DISTANCE_MAX, THREAD_COUNT);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	for (size_t i = 0; i < UINT8_MAX+2; i++)
	{
		memset(&g_table[i][0], 0, sizeof(uint32_t) * WORD_LEN);
	}

	memset(&thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);
	for (size_t t = 0; t < THREAD_COUNT; t++)
	{
		memset(&thread_buffer[t][0], 0, sizeof(uint32_t) * WORD_LEN);
		rand_init(&thread_rand[t], t);
	}

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	for (size_t i = 0; i < UINT8_MAX+2; i++)
	{
		uint32_t round = 0, minimum_distance = 0;
		char time_string[64];

		printf("Row %03u of %03u [%c]", i+1, UINT8_MAX+2, SPINNER[g_spinpos]);
		g_spinpos = (g_spinpos + 1) % 4;

		PTHREAD_CREATE(&thread_id[THREAD_COUNT], NULL, thread_spin, &stop_flag);
		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			thread_data[t].index = i;
			thread_data[t].row_buffer = &thread_buffer[t][0];
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
				memcpy(&g_table[i][0], &thread_buffer[t][0], sizeof(uint32_t) * WORD_LEN);
			}
		}
		PTHREAD_JOIN(thread_id[THREAD_COUNT], NULL);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);
	}

	printf("\n-----\n\n");

	dump_table(stdout);
	if (fopen_s(&file_out, "table_out." DISTANCE_STR ".txt", "w") == 0)
	{
		dump_table(file_out);
		fclose(file_out);
	}

	printf("\n-----\n\n");

	for (size_t i = 0; i < UINT8_MAX+2; i++)
	{
		for (size_t j = 0; j < UINT8_MAX+2; j++)
		{
			if (i == j)
			{
				continue;
			}
			const uint32_t dist = hamming_distance(&g_table[i][0], &g_table[j][0], WORD_LEN);
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
