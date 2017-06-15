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

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384

#define DISTANCE_MIN 46
#define DISTANCE_MAX 48

#define THREAD_COUNT 8

#define BYTE_LEN (HASH_LEN / (8 * sizeof(uint8_t)))
#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[UINT8_MAX+1][BYTE_LEN-1];
static uint8_t g_mixed[UINT8_MAX+1][BYTE_LEN];
static uint8_t g_ordered[BYTE_LEN];
static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

static inline void apply_permutation(const uint8_t *const permut, uint8_t *const data)
{
	uint8_t temp;
	for (size_t i = 0; i < BYTE_LEN - 1; i++)
	{
		const uint8_t j = permut[i];
		temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}

static inline void build_permutation(uint8_t *const row_buffer, twister_t *const rand)
{
	for (uint32_t i = 0; i < BYTE_LEN - 1; i++)
	{
		row_buffer[i] = (uint8_t)(i + (next_rand_range(rand, BYTE_LEN - i)));
	}
}

static uint32_t compute_distance(const uint8_t *const a, const uint8_t *const b)
{
	uint32_t distance = 0;
	for (int i = 0; i < BYTE_LEN; i++)
	{
		if (a[i] != b[i])
		{
			distance++;
		}
	}
	return distance;
}

static inline int check_distance(const size_t index, const uint8_t *const row_buffer)
{
	uint8_t temp_mixed[BYTE_LEN];
	memcpy(temp_mixed, g_ordered, sizeof(uint8_t) * BYTE_LEN);
	apply_permutation(row_buffer, temp_mixed);
	for (size_t k = 0; k < BYTE_LEN; k++)
	{
		if (temp_mixed[k] == ((uint8_t)k))
		{
			return 0;
		}
	}
	for (size_t k = 0; k < index; k++)
	{
		const uint32_t dist = compute_distance(&g_mixed[k][0], temp_mixed);
		if ((dist > DISTANCE_MAX) || (dist < DISTANCE_MIN))
		{
			return 0;
		}
	}
	return 1;
}

static dump_table(FILE *out)
{
	for (size_t i = 0; i < UINT8_MAX+1; i++)
	{
		fprintf(out, "%02X: ", (uint32_t)(i & 0xFF));
		for (size_t j = 0; j < BYTE_LEN-1; j++)
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
		build_permutation(data->row_buffer, data->rand);
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
	uint8_t thread_buffer[THREAD_COUNT][BYTE_LEN-1];
	thread_data_t thread_data[THREAD_COUNT];
	twister_t thread_rand[THREAD_COUNT];
	sem_t stop_flag;
	pthread_mutex_t stop_mutex;
	FILE *file_out = NULL;

	printf("MHash GenTableMIX [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min/Max: %d/%d, Threads: %d\n\n", HASH_LEN, DISTANCE_MIN, DISTANCE_MAX, THREAD_COUNT);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	for (size_t i = 0; i < BYTE_LEN; i++)
	{
		g_ordered[i] = (uint8_t)(i);
	}
	for (size_t i = 0; i < UINT8_MAX+1; i++)
	{
		memset(&g_table[i][0], 0, sizeof(uint8_t) * (BYTE_LEN - 1));
		memcpy(&g_mixed[i][0], g_ordered, sizeof(uint8_t) * BYTE_LEN);
	}

	memset(&thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);
	for (size_t t = 0; t < THREAD_COUNT; t++)
	{
		memset(&thread_buffer[t][0], 0, sizeof(uint8_t) * (BYTE_LEN-1));
		rand_init(&thread_rand[t], make_seed());
	}

	SEM_INIT(&stop_flag);
	MUTEX_INIT(&stop_mutex);

	for (size_t i = 0; i < UINT8_MAX+1; i++)
	{
		uint32_t round = 0, minimum_distance = 0;
		char time_string[64];

		printf("Row %03u of %03u [%c]", (uint32_t)(i+1), UINT8_MAX+1, SPINNER[g_spinpos]);
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
				memcpy(&g_table[i][0], &thread_buffer[t][0], sizeof(uint8_t) * (BYTE_LEN-1));
				apply_permutation(&g_table[i][0], &g_mixed[i][0]);
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

	for (size_t i = 0; i < UINT8_MAX+1; i++)
	{
		int complete = 0;
		for (size_t j = 0; j < BYTE_LEN; j++)
		{
			for (size_t k = 0; k < BYTE_LEN; k++)
			{
				if (g_mixed[i][k] == g_ordered[j])
				{
					complete++;
				}
			}
		}
		if (complete != BYTE_LEN)
		{
			crit_exit("FATAL: Table verification has failed!");
		}
		for (size_t j = 0; j < UINT8_MAX+1; j++)
		{
			uint8_t a[BYTE_LEN], b[BYTE_LEN];
			if (i == j)
			{
				continue;
			}
			memcpy(a, g_ordered, sizeof(uint8_t) * BYTE_LEN);
			memcpy(b, g_ordered, sizeof(uint8_t) * BYTE_LEN);
			apply_permutation(&g_table[i][0], a);
			apply_permutation(&g_table[j][0], b);
			const uint32_t dist = compute_distance(a, b);
			if ((dist > DISTANCE_MAX) || (dist < DISTANCE_MIN))
			{
				crit_exit("FATAL: Table verification has failed!");
			}
		}
	}
	for (size_t i = 0; i < BYTE_LEN; i++)
	{
		if (g_ordered[i] != (uint8_t)(i))
		{
			crit_exit("FATAL: Ordering verification has failed!");
		}
	}

	SEM_DESTROY(&stop_flag);
	MUTEX_DESTROY(&stop_mutex);

	printf("COMPLETED.\n\n");
	return getchar();
}
