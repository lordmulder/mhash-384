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
#include "msws.h"

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
#define DISTANCE_MIN (HASH_LEN / 2U)

#define ENABLE_TRACE

#define ROW_NUM 2U                      /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

static const uint8_t RAND_DATA[ROW_LEN] =
{
	0xEF, 0xDC, 0xB6, 0x6D, 0x4E, 0xCC, 0x1A, 0xA4, 0xAF, 0x9B, 0x3F, 0x20, 0x46, 0x98, 0xFA, 0xC6,
	0xA3, 0x06, 0xB4, 0x11, 0x98, 0x7C, 0x28, 0x2A, 0xE8, 0x92, 0x49, 0xC2, 0x64, 0xA9, 0xE1, 0xC8,
	0xA4, 0xAB, 0x93, 0x16, 0x1F, 0x35, 0x96, 0x77, 0x35, 0x2F, 0xC8, 0xB5, 0x6B, 0x6E, 0x37, 0xAE
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][ROW_LEN];

static const char SPINNER[4] = { '/', '-', '\\', '|' };
static const double SQRT2 = 1.41421356237309504880168872420969807856967187537694;

static size_t g_spinpos = 0;

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

#ifdef ENABLE_TRACE
#define TRACE(X, ...) printf("[TRACE] " X "\n", __VA_ARGS__)
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

static void dump_table(FILE *out)
{
	fprintf(out, "uint8_t MHASH_384_TABLE_INI[%uU][MHASH_384_LEN] =\n{\n", ROW_NUM);
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
		fprintf(out, " }%s /*%02X*/\n", (i != (ROW_NUM - 1)) ? ", " : "  ", (uint32_t)(i % 0x100));
	}
	fputs("};\n", out);
}

static bool check_buffer(const uint8_t *const a, const uint8_t *const b)
{
	static const uint8_t MAX_DIST = (CHAR_BIT / 2U) + 1U;
	static const uint8_t MIN_DIST = (CHAR_BIT / 2U) - 1U;
	for (uint_fast16_t i = 0; i < ROW_LEN; i++)
	{
		const uint8_t dist = HAMMING_DISTANCE_LUT[a[i]][b[i]];
		if ((dist < MIN_DIST) || (dist > MAX_DIST))
		{
			return false;
		}
		if (((a[i] & 0x0F) == (b[i] & 0x0F)) || ((a[i] & 0xF0) == (b[i] & 0xF0)))
		{
			return false;
		}
	}
	if (hamming_distance(g_table[0], g_table[1], ROW_LEN) != DISTANCE_MIN)
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int wmain(int argc, wchar_t *argv[])
{
	msws_t rnd;

	printf("MHash GenTableINI [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min: %d, MSVC: %u\n\n", HASH_LEN, DISTANCE_MIN, _MSC_FULL_VER);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	for (size_t i = 0; i < ROW_NUM; i++)
	{
		memset(&g_table[i][0], 0, sizeof(uint8_t) * ROW_LEN);
	}

	msws_init(rnd, make_seed());
	memcpy(g_table[0], RAND_DATA, sizeof(uint8_t) * ROW_LEN);

	printf("Table is being generated, please be patient...\n");
	do
	{
		msws_bytes(rnd, g_table[1], ROW_LEN);
	}
	while (!check_buffer(g_table[0], g_table[1]));

	printf("Done\n\n");
	dump_table(stdout);

	printf("\n-----\n\n");

	printf("COMPLETED.\n\n");
	return getchar();
}
