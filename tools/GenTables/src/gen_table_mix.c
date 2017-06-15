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

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384

#define ROW_NUM (UINT8_MAX+1)           /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of bits per row*/

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][HASH_LEN];

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

static inline void build_permutation(uint8_t *const row_buffer, twister_t *const rand)
{
	for (uint32_t i = 0; i < ROW_LEN; i++)
	{
		row_buffer[i] = (uint8_t)(i + (next_rand_range(rand, ROW_LEN - i)));
	}
}

static inline void apply_permutation(uint8_t *const row_buffe, const uint8_t *const permutation)
{
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		const uint8_t tmp = row_buffe[i];
		row_buffe[i] = row_buffe[permutation[i]];
		row_buffe[permutation[i]] = tmp;
	}
}

static inline bool test_permutation(const uint8_t *const permutation)
{
	uint8_t row_buffe[ROW_LEN];
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		row_buffe[i] = ((uint8_t)i);
	}

	apply_permutation(row_buffe, permutation);
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		if (row_buffe[i] == ((uint8_t)i))
		{
			return false;
		}
	}

	return true;
}

static dump_table(FILE *out)
{
	fputs("uint8_t MHASH_384_TABLE_MIX[UINT8_MAX+1][MHASH_384_LEN] =\n{\n", out);
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
// MAIN
//-----------------------------------------------------------------------------

int main()
{
	FILE *file_out = NULL;

	printf("MHash GenTableMIX [%s]\n\n", __DATE__);
	printf("HashLen: %d\n\n", HASH_LEN);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	twister_t rand;
	rand_init(&rand, make_seed());

	for (size_t i = 0; i < ROW_NUM; ++i)
	{
		printf("Row %03u of %03u... ", (uint32_t)(i + 1U), ROW_NUM);
		do
		{
			build_permutation(&g_table[i][0], &rand);
		}
		while(!test_permutation(&g_table[i][0]));
		puts("done");
	}

	printf("\n-----\n\n");

	dump_table(stdout);
	if (fopen_s(&file_out, "table_out.MIX.txt", "w") == 0)
	{
		dump_table(file_out);
		fclose(file_out);
	}

	printf("\n-----\n\n");

	printf("COMPLETED.\n\n");
	return getchar();
}
