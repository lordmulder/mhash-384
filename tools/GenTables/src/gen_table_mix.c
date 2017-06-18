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

#define DISTANCE_MIN 45

#define ROW_NUM 997                     /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of indices per row*/

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

#define MAGIC_NUMBER 0x4ECB4ABE0047B220ui64

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][HASH_LEN];
static uint8_t g_mixed[ROW_NUM][HASH_LEN];

static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

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

static void print_row(uint8_t *const row_buffer)
{
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		printf(i ? ",%02X" : "%02X", row_buffer[i]);
	}
	puts("");
}

static inline void apply_permutation(uint8_t *const row_buffer, const uint8_t *const permutation)
{
	uint8_t tmp;
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		row_buffer[i] = ((uint8_t)i);
	}
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		tmp = row_buffer[permutation[i]];
		row_buffer[permutation[i]] = row_buffer[i];
		row_buffer[i] = tmp;
	}
}

static inline bool test_permutation(const size_t index, const uint8_t *const permutation)
{
	uint8_t row_buffer[ROW_LEN];
	apply_permutation(row_buffer, permutation);
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		if (row_buffer[i] == ((uint8_t)i))
		{
			return false;
		}
	}
	for (size_t i = 0; i < index; ++i)
	{
		uint32_t distance = 0U;
		for (size_t j = 0; j < ROW_LEN; ++j)
		{
			if (g_mixed[i][j] != row_buffer[j])
			{
				distance++;
			}
		}
		if (distance < DISTANCE_MIN)
		{
			return false;
		}
	}
	return true;
}

static dump_table(FILE *out)
{
	fprintf(out, "uint8_t MHASH_384_TABLE_MIX[%u][MHASH_384_LEN] =\n{\n", ROW_NUM);
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
		fprintf(out, " }%s /*%03u*/\n", (i != (ROW_NUM - 1)) ? "," : " ", (uint32_t)i);
	}
	fputs("};\n", out);
}

//-----------------------------------------------------------------------------
// Save / Load
//-----------------------------------------------------------------------------

static bool save_table_data(const wchar_t *const filename, const size_t rows_completed_in)
{
	FILE *const file = _wfopen(filename, L"wb");
	if (file)
	{
		bool success = true;
		const uint64_t magic_number = MAGIC_NUMBER;
		const uint32_t hash_len = HASH_LEN, distance_min = DISTANCE_MIN, rows_completed = (uint32_t)rows_completed_in;
		fwrite(&magic_number, sizeof(uint64_t), 1, file);
		fwrite(&hash_len, sizeof(uint32_t), 1, file);
		fwrite(&distance_min, sizeof(uint32_t), 1, file);
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
		return success;
	}
	else
	{
		printf("ERROR: Failed to open table file for writing!\n");
		return false;
	}
}

static bool load_table_data(const wchar_t *const filename, size_t *const rows_completed_out)
{
	FILE *const file = _wfopen(filename, L"rb");
	if (file)
	{
		bool success = true;
		uint64_t magic_number;
		uint32_t hash_len, distance_min, rows_completed;
		fread(&magic_number, sizeof(uint64_t), 1, file);
		fread(&hash_len, sizeof(uint32_t), 1, file);
		fread(&distance_min, sizeof(uint32_t), 1, file);
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
		if ((hash_len != HASH_LEN) || (distance_min != DISTANCE_MIN) || (rows_completed > ROW_NUM))
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
			if (!test_permutation(i, &g_table[i][0]))
			{
				printf("ERROR: Table distance verification has failed!\n");
				success = false;
				goto failed;
			}
			apply_permutation(&g_mixed[i][0], &g_table[i][0]);
		}
	failed:
		fclose(file);
		if (success && rows_completed_out)
		{
			*rows_completed_out = (size_t)rows_completed;
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
	FILE *file_out = NULL;
	uint32_t counter = 0;
	size_t initial_row_index = 0;
	char time_string[64];

	printf("MHash GenTableMIX [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min: %d\n\n", HASH_LEN, DISTANCE_MIN);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	if (argc < 2)
	{
		printf("Table file not specified!\n\n");
		printf("Usage:\n");
		printf("   GenTables_MIX.exe <table_file>\n\n");
		return 1;
	}

	twister_t rand;
	rand_init(&rand, make_seed());

	if (_waccess(argv[1], 4) == 0)
	{
		printf("Loading existing table data and proceeding...\n");
		if (!load_table_data(argv[1], &initial_row_index))
		{
			return 1;
		}
	}

	for (size_t i = initial_row_index; i < ROW_NUM; ++i)
	{
		printf("Row %03u of %03u [%c]", (uint32_t)(i + 1U), ROW_NUM, SPINNER[g_spinpos]);
		counter = 0U;
		time_t ref_time = time(NULL);
		do
		{
			const time_t curr_time = time(NULL);
			if (!((++counter) % 0xFFFFF))
			{

				if (curr_time - ref_time >= 180i64)
				{
					ref_time = curr_time;
					rand_init(&rand, make_seed());
					printf("\b\b\b[%c]", '!');
				}
				else
				{
					printf("\b\b\b[%c]", SPINNER[g_spinpos]);
					g_spinpos = (g_spinpos + 1) % 4;
				}
			}
			build_permutation(&g_table[i][0], &rand);
		}
		while(!test_permutation(i, &g_table[i][0]));

		apply_permutation(&g_mixed[i][0], &g_table[i][0]);
		get_time_str(time_string, 64);
		printf("\b\b\b[#] - %s\n", time_string);

		if (!save_table_data(argv[1], i + 1U))
		{
			return 1; /*failed to save current table data*/
		}
	}

	printf("\n-----\n\n");

	dump_table(stdout);
	if (fopen_s(&file_out, "table_MIX." DISTANCE_STR ".txt", "w") == 0)
	{
		dump_table(file_out);
		fclose(file_out);
	}

	printf("\n-----\n\n");

	printf("COMPLETED.\n\n");
	return getchar();
}
