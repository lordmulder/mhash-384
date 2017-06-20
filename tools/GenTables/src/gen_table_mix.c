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
#include "boxmuller.h"

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

#define MAGIC_NUMBER 0x76A3D06509A73016ui64

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][HASH_LEN];

static size_t g_spinpos = 0;
static char SPINNER[4] = { '/', '-', '\\', '|' };

//-----------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------

static inline void swap(uint8_t *const row_buffer, const size_t a, const size_t b)
{
	const uint8_t temp = row_buffer[a];
	row_buffer[a] = row_buffer[b];
	row_buffer[b] = temp;
}

static inline void random_permutation(twister_t *const rand, uint8_t *const row_buffer)
{
	for (uint32_t i = 0; i < ROW_LEN; ++i)
	{
		row_buffer[i] = ((uint8_t)i);
	}
	for (uint32_t i = 0; i < ROW_LEN - 1U; ++i)
	{
		swap(row_buffer, i, next_rand_range(rand, ROW_LEN - i) + i);
	}
}

static inline void swap_multiple_random(twister_t *const rand, uint8_t *const row_buffer, const size_t count)
{
	bool map[ROW_LEN];
	memset(&map[0], 0, sizeof(bool) * ROW_LEN);
	for (size_t i = 0U; i < count; ++i)
	{
		size_t a, b;
		do
		{
			a = next_rand_range(rand, ROW_LEN);
			b = next_rand_range(rand, ROW_LEN);
		} 
		while(map[a] || (a == b));
		map[a] = map[b] = true;
		swap(row_buffer, a, b);
	}
}

static inline void rotate_row(uint8_t *const row_buffer)
{
	const uint8_t temp = row_buffer[0];
	for (uint32_t k = 0U; k < ROW_LEN - 1U; ++k)
	{
		row_buffer[k] = row_buffer[k + 1U];
	}
	row_buffer[ROW_LEN - 1U] = temp;
}

static inline void reverse_row(uint8_t *const row_buffer)
{
	size_t j = ROW_LEN - 1U;
	for (size_t i = 0U; i < ROW_LEN / 2U; ++i)
	{
		swap(row_buffer, i, j--);
	}
}

static inline uint32_t check_permutation(const size_t index, const uint8_t *const row_buffer)
{
	uint32_t error = 0U;
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		if (row_buffer[i] == ((uint8_t)i))
		{
			++error;
		}
	}
	if(error)
	{
		return ROW_LEN + error;
	}
	for (size_t i = 0; i < index; ++i)
	{
		uint32_t distance = 0U;
		for (size_t j = 0; j < ROW_LEN; ++j)
		{
			if (g_table[i][j] != row_buffer[j])
			{
				distance++;
			}
		}
		if (distance < DISTANCE_MIN)
		{
			error = max_ui32(error, DISTANCE_MIN - distance);
		}
	}
	return error;
}

static void print_row(const uint8_t *const row_buffer)
{
	for (size_t i = 0; i < ROW_LEN; ++i)
	{
		printf(i ? ",%02X" : "%02X", row_buffer[i]);
	}
	puts("");
}

static inline void permutation_to_shuffle_indices(const uint8_t *const row_buffer, uint8_t *const indices_out)
{
	uint8_t reference[ROW_LEN];
	for (uint32_t i = 0U; i < ROW_LEN; ++i)
	{
		reference[i] = ((uint8_t)i);
	}
	for (uint32_t i = 0U; i < ROW_LEN; ++i)
	{
		indices_out[i] = UINT8_MAX;
		for (uint32_t j = i; j < ROW_LEN; ++j)
		{
			if (reference[j] == row_buffer[i])
			{
				swap(&reference[0], i, j);
				indices_out[i] = ((uint8_t)j);
				break;
			}
		}
	}
	for (uint32_t i = 0; i < ROW_LEN; ++i)
	{
		if ((indices_out[i] == UINT8_MAX) || (reference[i] != row_buffer[i]))
		{
			puts("ERROR: Failed to derive shuffle indices!\n");
		}
	}
}

static dump_table(FILE *out)
{
	fprintf(out, "uint8_t MHASH_384_TABLE_MIX[%u][MHASH_384_LEN] =\n{\n", ROW_NUM);
	for (size_t i = 0; i < ROW_NUM; i++)
	{
		uint8_t shuffle_indices[ROW_LEN];
		permutation_to_shuffle_indices(&g_table[i][0], &shuffle_indices[0]);
		fputs("\t{ ", out);
		for (size_t j = 0; j < ROW_LEN; j++)
		{
			if (j > 0)
			{
				fputc(',', out);
			}
			fprintf(out, "0x%02X", shuffle_indices[j]);
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
			if (check_permutation(i, &g_table[i][0]) != 0)
			{
				printf("ERROR: Table distance verification has failed!\n");
				success = false;
				goto failed;
			}
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

	bxmller_t bxmller;
	gaussian_noise_init(&bxmller);

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
		uint8_t counter = 0U;
		time_t ref_time = time(NULL);
		uint8_t temp[ROW_LEN];
		for (;;)
		{
			random_permutation(&rand, &g_table[i][0]);
			uint32_t error = check_permutation(i, &g_table[i][0]);
			printf("\b\b\b[%c]", '!');
			for (uint32_t rand_init = 0U; rand_init < 99991U; ++rand_init)
			{
				random_permutation(&rand, &temp[0]);
				const uint32_t error_next = check_permutation(i, &temp[0]);
				if (error_next < error)
				{
					memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
					if (!((error = error_next) > 0U))
					{
						goto success;
					}
				}
			}
			if (error > 0U)
			{
				for (size_t retry = 0U; retry < 9973U; ++retry)
				{
					bool improved = false;
					for (uint32_t rotate = 0U; rotate < ROW_LEN; ++rotate)
					{
						rotate_row(&g_table[i][0]);
						const uint32_t error_next = check_permutation(i, &g_table[i][0]);
						if (error_next < error)
						{
							improved = true;
							if (!((error = error_next) > 0U))
							{
								goto success;
							}
							break;
						}
						reverse_row(&g_table[i][0]);
						const uint32_t error_next_rev = check_permutation(i, &g_table[i][0]);
						if (error_next_rev >= error)
						{
							reverse_row(&g_table[i][0]); /*revert*/
						}
						else
						{
							improved = true;
							if (!((error = error_next_rev) > 0U))
							{
								goto success;
							}
							break;
						}
					}
					for (uint32_t swap_x = 0; swap_x < ROW_LEN; ++swap_x)
					{
						for (uint32_t swap_y = swap_x + 1U; swap_y < ROW_LEN; ++swap_y)
						{
							swap(&g_table[i][0], swap_x, swap_y);
							const uint32_t error_next = check_permutation(i, &g_table[i][0]);
							if (error_next >= error)
							{
								swap(&g_table[i][0], swap_x, swap_y); /*revert*/
							}
							else
							{
								improved = true;
								if (!((error = error_next) > 0U))
								{
									goto success;
								}
							}
						}
					}
					for (uint32_t loop = 0; loop < 9973U; ++loop)
					{
						const uint32_t swap_count = gaussian_noise_next(&rand, &bxmller, 8.0, 2U, (ROW_LEN / 2U));
						if (!(++counter))
						{
							const time_t curr_time = time(NULL);
							if (curr_time - ref_time >= 180i64)
							{
								ref_time = curr_time;
								rand_init(&rand, make_seed());
								printf("\b\b\b[%c]", '$');
							}
							else
							{
								printf("\b\b\b[%c]", SPINNER[g_spinpos]);
								g_spinpos = (g_spinpos + 1) % 4;
							}
						}
						for (uint32_t round = 0; round < 97U; ++round)
						{
							memcpy(&temp[0], &g_table[i][0], sizeof(uint8_t) * ROW_LEN);
							swap_multiple_random(&rand, &temp[0], swap_count);
							const uint32_t error_next = check_permutation(i, &temp[0]);
							if (error_next < error)
							{
								memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
								improved = true;
								if (!((error = error_next) > 0U))
								{
									goto success;
								}
							}
						}
					}
					if (!improved)
					{
						break; /*early termination*/
					}
				}
			}
			else
			{
				break; /*success*/
			}
		}

	success:
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
