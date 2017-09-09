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

//-----------------------------------------------------------------------------
// Const
//-----------------------------------------------------------------------------

#define HASH_LEN 384U

#define ROW_NUM 251U                    /*total number of rows*/
#define ROW_LEN (HASH_LEN / CHAR_BIT)   /*number of indices per row*/
#define DISTANCE_MIN (ROW_LEN - 1U)     /*min. hamming distance*/

#undef ENABLE_TRACE

#define __DISTANCE_STR(X) #X
#define _DISTANCE_STR(X) __DISTANCE_STR(X)
#define DISTANCE_STR _DISTANCE_STR(DISTANCE_MIN)

#define MAGIC_NUMBER 0x76A3D06509A73016ui64

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

static uint8_t g_table[ROW_NUM][HASH_LEN];
static size_t g_spinpos = 0;

static const char SPINNER[4] = { '/', '-', '\\', '|' };
static const double SQRT2 = 1.41421356237309504880168872420969807856967187537694;

static const uint8_t INDICES[UINT8_MAX + 1U] =
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

#ifdef ENABLE_TRACE
#define TRACE(X, ...) printf(X "\n", __VA_ARGS__)
#else
#define TRACE(X, ...) __noop()
#endif

static inline void swap(uint8_t *const row_buffer, const uint_fast16_t a, const uint_fast16_t b)
{
	const uint8_t temp = row_buffer[a];
	row_buffer[a] = row_buffer[b];
	row_buffer[b] = temp;
}

static inline void random_permutation(msws_t rand, uint8_t *const row_buffer)
{
	for (uint_fast16_t i = 0; i < ROW_LEN; ++i)
	{
		const uint_fast16_t j = msws_uint32_max(rand, i + 1U);
		row_buffer[i] = row_buffer[j];
		row_buffer[j] = INDICES[i];
	}
}

static inline void swap_multiple_random(msws_t rand, uint8_t *const row_buffer, const uint_fast16_t count)
{
	for (uint_fast16_t i = 0U; i < count; ++i)
	{
		uint_fast16_t a, b;
		do
		{
			a = msws_uint32_max(rand, ROW_LEN);
			b = msws_uint32_max(rand, ROW_LEN);
		} 
		while(a == b);
		swap(row_buffer, a, b);
	}
}

static inline void rotate_row(uint8_t *const row_buffer)
{
	const uint8_t temp = row_buffer[0];
	for (uint_fast16_t k = 0U; k < ROW_LEN - 1U; ++k)
	{
		row_buffer[k] = row_buffer[k + 1U];
	}
	row_buffer[ROW_LEN - 1U] = temp;
}

static inline void reverse_row(uint8_t *const row_buffer)
{
	uint_fast16_t j = ROW_LEN - 1U;
	for (uint_fast16_t i = 0U; i < ROW_LEN / 2U; ++i)
	{
		swap(row_buffer, i, j--);
	}
}

static inline uint_fast16_t row_distance(const uint8_t *const row_a, const uint8_t *const row_b)
{
	uint_fast16_t distance = 0U;
	for (uint_fast16_t j = 0; j < ROW_LEN; ++j)
	{
		if (row_a[j] != row_b[j])
		{
			distance++;
		}
	}
	return distance;
}

#define ERROR_ACC(X,Y) ((X >= Y) ? ((X << 8U) | Y) : ((Y << 8U) | X))
static inline uint_fast16_t check_permutation(const uint_fast16_t index, const uint8_t *const row_buffer, const uint_fast16_t limit)
{
	uint_fast16_t error = 0U, failed = 0U;
	uint_fast16_t distance = row_distance(&INDICES[0], &row_buffer[0]);
	if (distance < DISTANCE_MIN)
	{
		error = DISTANCE_MIN - distance;
		failed = 1U;
		if (ERROR_ACC(failed, error) >= limit)
		{
			return limit; /*early termination*/
		}
	}
	for (uint_fast16_t i = 0; i < index; ++i)
	{
		distance = row_distance(&g_table[i][0], &row_buffer[0]);
		if (distance < DISTANCE_MIN)
		{
			error = max_ui16(error, DISTANCE_MIN - distance);
			failed++;
			if (ERROR_ACC(failed, error) >= limit)
			{
				return limit; /*early termination*/
			}
		}
	}
	return ERROR_ACC(failed, error);
}

static void print_row(const uint8_t *const row_buffer)
{
	for (uint_fast8_t i = 0; i < ROW_LEN; ++i)
	{
		printf(i ? ",%02X" : "%02X", row_buffer[i]);
	}
	puts("");
}

static void dump_table(FILE *out)
{
	fprintf(out, "uint8_t MHASH_384_TABLE_MIX[%u][MHASH_384_LEN] =\n{\n", ROW_NUM);
	for (uint_fast16_t i = 0; i < ROW_NUM; i++)
	{
		fputs("\t{ ", out);
		for (uint_fast8_t j = 0; j < ROW_LEN; j++)
		{
			if (j > 0)
			{
				fputs(", ", out);
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

static bool save_table_data(const wchar_t *const filename, const uint_fast16_t rows_completed_in)
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

static bool load_table_data(const wchar_t *const filename, uint_fast16_t *const rows_completed_out)
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
		for (uint_fast16_t i = 0; i < rows_completed; ++i)
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
			if (check_permutation(i, &g_table[i][0], UINT16_MAX) != 0U)
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
			*rows_completed_out = (uint_fast16_t)rows_completed;
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
	uint_fast16_t initial_row_index = 0;
	char time_string[64];

	printf("MHash GenTableMIX [%s]\n\n", __DATE__);
	printf("HashLen: %d, Distance Min: %d\n\n", HASH_LEN, DISTANCE_MIN);

	if ((HASH_LEN % (8 * sizeof(uint32_t))) != 0)
	{
		crit_exit("FATAL: Hash length must be a multiple of 32 bits!");
	}

	for (uint_fast8_t i = 0; i < ROW_LEN; ++i)
	{
		if (INDICES[i] != ((uint8_t)i))
		{
			crit_exit("FATAL: Invalid indices array detected!");
		}
	}

	if (argc < 2)
	{
		printf("Table file not specified!\n\n");
		printf("Usage:\n");
		printf("   GenTables_MIX.exe <table_file>\n\n");
		return 1;
	}

	msws_t rand;
	msws_init(rand, make_seed());

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

	for (uint_fast16_t i = initial_row_index; i < ROW_NUM; ++i)
	{
		printf("Row %03u of %03u [%c]", (uint32_t)(i + 1U), ROW_NUM, SPINNER[g_spinpos]);
		uint8_t temp[ROW_LEN];
		uint_fast16_t counter = 0U;
		for (;;)
		{
			random_permutation(rand, &g_table[i][0]);
			uint_fast16_t error = check_permutation(i, &g_table[i][0], UINT16_MAX);
			if (error > 0U)
			{
				static const int_fast16_t MAX_RETRY = 1999U;
				for (int_fast16_t retry = 0; retry < MAX_RETRY; ++retry)
				{
					if (!((++counter) & 0x3))
					{
						TRACE("Randomize round %u of %u", retry, MAX_RETRY);
						printf("\b\b\b[%c]", SPINNER[g_spinpos]);
						g_spinpos = (g_spinpos + 1) % 4;
					}
					for (uint_fast16_t rand_init = 0U; rand_init < 9973U; ++rand_init)
					{
						random_permutation(rand, &temp[0]);
						const uint_fast16_t error_next = check_permutation(i, &temp[0], error);
						if (error_next < error)
						{
							TRACE("Improved by rand init! (%04X -> %04X)", error, error_next);
							retry = -1;
							memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
							if (!((error = error_next) > 0U))
							{
								goto success;
							}
						}
						for (uint_fast8_t rotate = 0U; rotate < ROW_LEN; ++rotate)
						{
							rotate_row(&temp[0]);
							const uint_fast16_t error_next = check_permutation(i, &temp[0], error);
							if (error_next < error)
							{
								TRACE("Improved by rand init + rotate! (%04X -> %04X)", error, error_next);
								retry = -1;
								memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
								if (!((error = error_next) > 0U))
								{
									goto success;
								}
							}
							reverse_row(&temp[0]);
							const uint_fast16_t error_next_reverse = check_permutation(i, &temp[0], error);
							if (error_next_reverse < error)
							{
								TRACE("Improved by rand init + rotate + reverse! (%04X -> %04X)", error, error_next_reverse);
								retry = -1;
								memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
								if (!((error = error_next_reverse) > 0U))
								{
									goto success;
								}
							}
							else
							{
								reverse_row(&temp[0]);
							}
						}
					}
				}
				for (int_fast16_t retry = 0; retry < MAX_RETRY; ++retry)
				{
					TRACE("Optimizer round %u of %u", retry, MAX_RETRY);
					if (!retry)
					{
						msws_init(rand, make_seed());
						TRACE("First round!");
						for (uint_fast8_t swap_x1 = 0; swap_x1 < ROW_LEN; ++swap_x1)
						{
							printf("\b\b\b[%c]", SPINNER[g_spinpos]);
							g_spinpos = (g_spinpos + 1) % 4;
							for (uint_fast8_t swap_y1 = swap_x1 + 1U; swap_y1 < ROW_LEN; ++swap_y1)
							{
								bool revert_1 = true;
								swap(&g_table[i][0], swap_x1, swap_y1);
								const uint_fast16_t error_next = check_permutation(i, &g_table[i][0], error);
								if (error_next < error)
								{
									TRACE("Improved by swap-1! (%04X -> %04X)", error, error_next);
									revert_1 = false;
									retry = -1;
									if (!((error = error_next) > 0U))
									{
										goto success;
									}
								}
								for (uint_fast8_t swap_x2 = 0; swap_x2 < ROW_LEN; ++swap_x2)
								{
									for (uint_fast8_t swap_y2 = swap_x2 + 1U; swap_y2 < ROW_LEN; ++swap_y2)
									{
										bool revert_2 = true;
										swap(&g_table[i][0], swap_x2, swap_y2);
										const uint_fast16_t error_next = check_permutation(i, &g_table[i][0], error);
										if (error_next < error)
										{
											TRACE("Improved by swap-2! (%04X -> %04X)", error, error_next);
											revert_1 = revert_2 = false;
											retry = -1;
											if (!((error = error_next) > 0U))
											{
												goto success;
											}
										}
										for (uint_fast8_t swap_x3 = 0; swap_x3 < ROW_LEN; ++swap_x3)
										{
											for (uint_fast8_t swap_y3 = swap_x3 + 1U; swap_y3 < ROW_LEN; ++swap_y3)
											{
												swap(&g_table[i][0], swap_x3, swap_y3);
												const uint_fast16_t error_next = check_permutation(i, &g_table[i][0], error);
												if (error_next >= error)
												{
													swap(&g_table[i][0], swap_x3, swap_y3); /*revert*/
												}
												else
												{
													TRACE("Improved by swap-3! (%04X -> %04X)", error, error_next);
													revert_1 = revert_2 = false;
													retry = -1;
													if (!((error = error_next) > 0U))
													{
														goto success;
													}
												}
											}
										}
										if (revert_2)
										{
											swap(&g_table[i][0], swap_x2, swap_y2); /*revert*/
										}
									}
								}
								if (revert_1)
								{
									swap(&g_table[i][0], swap_x1, swap_y1); /*revert*/
								}
							}
						}
					}
					const double sigma = SQRT2 * (1.0 + (((double)retry) / ((double)MAX_RETRY)));
					for (int_fast16_t loop = 0; loop < 9973U; ++loop)
					{
						const uint_fast16_t swap_count = (uint_fast16_t)gaussian_noise_next(rand, &bxmller, sigma, 4U, (ROW_LEN / 2U));
						if (!((++counter) & 0x3FFF))
						{
							printf("\b\b\b[%c]", SPINNER[g_spinpos]);
							g_spinpos = (g_spinpos + 1) % 4;
						}
						for (int_fast16_t round = 0; round < 997U; ++round)
						{
							memcpy(&temp[0], &g_table[i][0], sizeof(uint8_t) * ROW_LEN);
							swap_multiple_random(rand, &temp[0], swap_count);
							const uint_fast16_t error_next = check_permutation(i, &temp[0], error);
							if (error_next < error)
							{
								TRACE("Improved by swap-N! (N=%u, %04X -> %04X)", swap_count, error, error_next);
								memcpy(&g_table[i][0], &temp[0], sizeof(uint8_t) * ROW_LEN); /*keep*/
								retry = -1;
								if (!((error = error_next) > 0U))
								{
									goto success;
								}
							}
						}
					}
				}
				TRACE("Restart!");
				msws_init(rand, make_seed());
			}
			else
			{
				break; /*success*/
			}
		}

	success:
		if (check_permutation(i, &g_table[i][0], UINT16_MAX))
		{
			fprintf(stderr, "ERROR MISCOMPARE!\n");
			abort();
		}

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
