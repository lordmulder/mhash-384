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
#include "msws.h"
#include "thread_utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <io.h>

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN     384U
#define DISTANCE_MIN 1009U

#define ROW_NUM 48U   /*total number of rows*/
#define ROW_LEN 256U  /*number of indices per row*/

#define THREAD_COUNT 8U

#define ENABLE_TRACE

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static size_t g_spinpos = 0;
static uint8_t g_table[ROW_NUM][ROW_LEN];

static const char SPINNER[4] = { '/', '-', '\\', '|' };
static const double SQRT2 = 1.41421356237309504880168872420969807856967187537694;

static const uint8_t INDICES[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

#define MAX_ROUNDS INT32_C(999983)

#ifdef ENABLE_TRACE
#define TRACE(X, ...) printf(X "\n", __VA_ARGS__)
#else
#define TRACE(X, ...) __noop()
#endif

static inline void random_permutation(msws_t rand, uint8_t *const row_buffer)
{
	for (uint_fast16_t i = 0; i < ROW_LEN; ++i)
	{
		const uint_fast16_t j = msws_uint32_max(rand, i + 1U);
		row_buffer[i] = row_buffer[j];
		row_buffer[j] = INDICES[i];
	}
}

static inline void make_permutation(msws_t rand, uint8_t *const row_buffer)
{
	bool valid;
	uint_fast16_t flipped_sum;
	do
	{
		random_permutation(rand, row_buffer);
		valid = true;
		flipped_sum = 0U;
		for (uint_fast16_t i = 0; i < ROW_LEN; ++i)
		{
			const uint8_t flipped = HAMMING_DISTANCE_LUT[i][row_buffer[i]];
			if ((flipped < 1U) || (flipped > 7U))
			{
				valid = false;
				break;
			}
			flipped_sum += flipped;
		}
	}
	while ((!valid) || (flipped_sum != ROW_LEN * (CHAR_BIT / 2U)));
}

static inline uint_fast32_t check_table(const uint_fast16_t row_idx, const uint8_t *const row_buffer)
{
	uint_fast32_t min_dist = UINT_FAST32_MAX;
	for (uint_fast16_t i = 0; i < row_idx; ++i)
	{
		const uint_fast32_t dist = hamming_distance(g_table[i], row_buffer, ROW_LEN);
		if (dist < min_dist)
		{
			min_dist = dist;
		}
	}
	return min_dist;
}

static void dump_table(FILE *const out)
{
	fprintf(out, "uint8_t MHASH_384_TABLE_SBX[%uU][MHASH_384_LEN] =\n{\n", ROW_LEN);
	for (size_t i = 0; i < ROW_LEN; i++)
	{
		fputs("\t{ ", out);
		for (size_t j = 0; j < ROW_NUM; j++)
		{
			if (j > 0)
			{
				fputs(", ", out);
			}
			fprintf(out, "0x%02X", g_table[j][i]); /*i and j are intentionally reversed here!*/
		}
		fprintf(out, " }%s /*%02X*/\n", (!i) ? ", " : " ", (uint32_t)(i % 0x100));
	}
	fputs("};\n", out);
}

//-----------------------------------------------------------------------------
// Thread function
//-----------------------------------------------------------------------------

typedef struct
{
	uint_fast16_t row_idx;
	uint8_t row_buffer[ROW_LEN];
	sem_t *stop;
	pthread_mutex_t *mutex;
}
thread_data_t;

static void* thread_main(void *const param)
{
	thread_data_t *const data = (thread_data_t*)param;
	msws_t rand;
	uint16_t counter = 0U;

	msws_init(rand, make_seed());
	do
	{
		make_permutation(rand, data->row_buffer);
		if (!(++counter & 0xFFF))
		{
			if (SEM_TRYWAIT(data->stop))
			{
				return NULL;
			}
		}
	}
	while (check_table(data->row_idx, data->row_buffer) < DISTANCE_MIN);

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
// MAIN
//-----------------------------------------------------------------------------

static pthread_t g_thread_id[THREAD_COUNT + 1];
static thread_data_t g_thread_data[THREAD_COUNT];
static sem_t g_stop_flag;
static pthread_mutex_t g_stop_mutex;

int wmain(int argc, wchar_t *argv[])
{
	printf("MHash GenTableSBX [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min: %d, RowLen %d\n\n", HASH_LEN, DISTANCE_MIN, ROW_LEN);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	for (uint_fast16_t i = 0; i < ROW_LEN; ++i)
	{
		if (INDICES[i] != ((uint8_t)i))
		{
			crit_exit("FATAL: Invalid indices array detected!");
		}
	}

	SEM_INIT(&g_stop_flag);
	MUTEX_INIT(&g_stop_mutex);

	memset(&g_thread_id, 0, sizeof(pthread_t) * (THREAD_COUNT + 1));
	memset(&g_thread_data, 0, sizeof(thread_data_t) * THREAD_COUNT);

	for (uint_fast16_t row_idx = 0U; row_idx < ROW_NUM; ++row_idx)
	{
		char time_string[64];
		printf("\aRow %03u of %03u [%c]", (uint32_t)(row_idx + 1U), ROW_NUM, SPINNER[g_spinpos]);
		g_spinpos = (g_spinpos + 1) % 4;

		PTHREAD_CREATE(&g_thread_id[THREAD_COUNT], NULL, thread_spin, &g_stop_flag);
		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			g_thread_data[t].row_idx = row_idx;
			g_thread_data[t].stop = &g_stop_flag;
			g_thread_data[t].mutex = &g_stop_mutex;
			memset(&g_thread_data[t].row_buffer, 0, sizeof(uint8_t) * ROW_LEN);
			PTHREAD_CREATE(&g_thread_id[t], NULL, thread_main, &g_thread_data[t]);
			PTHREAD_SET_PRIORITY(g_thread_id[t], -15);
		}

		for (size_t t = 0; t < THREAD_COUNT; t++)
		{
			void *return_value = NULL;
			PTHREAD_JOIN(g_thread_id[t], &return_value);
			if (return_value)
			{
				memcpy(&g_table[row_idx][0], g_thread_data[t].row_buffer, sizeof(uint8_t) * ROW_LEN);
			}
		}

		PTHREAD_JOIN(g_thread_id[THREAD_COUNT], NULL);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);
	}

	dump_table(stdout);

	printf("\n-----\n\n");

	printf("COMPLETED.\n\n");
	return getchar();
}
