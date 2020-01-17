/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            */
/*                                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software  */
/* and associated documentation files (the "Software"), to deal in the Software without           */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  */
/* Software is furnished to do so, subject to the following conditions:                           */
/*                                                                                                */
/* The above copyright notice and this permission notice shall be included in all copies or       */
/* substantial portions of the Software.                                                          */
/*                                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        */
/* ---------------------------------------------------------------------------------------------- */

#define _CRT_RAND_S 1
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>

#define MASH384_WORDS 6U
#define MIN_DIFF 2U

static uint32_t mix_table[256U][MASH384_WORDS];

static uint32_t next_rand(const uint32_t max)
{
	uint32_t rnd;
	if(rand_s(&rnd) != 0)
	{
		abort();
	}
	return rnd % max;
}

static bool _create_permutation(uint32_t *const out, const uint32_t max_retry)
{
	bool map[MASH384_WORDS];
	memset(map, 0U, sizeof(bool) * MASH384_WORDS);
	uint32_t retry = 0U;
	for(size_t i = 0U; i < MASH384_WORDS; ++i)
	{
		uint32_t index;
		do
		{
			index = next_rand(MASH384_WORDS);
			if(++retry > max_retry)
			{
				return false;
			}
		}
		while((index == i) || map[index]);
		out[i] = index;
		map[index] = true;
	}
	return true;
}

static void create_permutation(uint32_t *const out)
{
	for(uint32_t i = 13U; i < UINT32_MAX; ++i)
	{
		if(_create_permutation(out, i))
		{
			return; /*success*/
		}
	}
	abort();
}

static uint32_t check_difference_row(const size_t row_1, const size_t row_2)
{
	uint32_t diff = 0U;
	for(size_t j = 0U; j < MASH384_WORDS; ++j)
	{
		if(mix_table[row_1][j] != mix_table[row_2][j])
		{
			++diff;
		}
	}
	return diff;
}

static uint32_t check_difference_table(const size_t max_row)
{
	uint32_t min_diff = UINT32_MAX;
	for(size_t i = 0U; i < max_row; ++i)
	{
		min_diff = std::min(min_diff, check_difference_row(i, max_row));
	}
	return min_diff;
}

static void print_row(const size_t row)
{
	for(size_t j = 0U; j < MASH384_WORDS; ++j)
	{
		printf((j > 0U) ? ", 0x%02X" : "{ 0x%02X", mix_table[row][j]);
	}
	printf(" }, /*%02X*/\n", row & 0xFF);
}

int main()
{
	printf("MHash GenTableMIX [%s]\n\n", __DATE__);

	create_permutation(mix_table[0U]);
	print_row(0U);

	for(size_t i = 1U; i < 256U; ++i)
	{
		uint32_t retry = 0U;
		do
		{
			if(++retry >= 999983U)
			{
				puts("\nFailed !!!\n");
				return EXIT_FAILURE;
			}
			create_permutation(mix_table[i]);
		}
		while((check_difference_row(i, i - 1U) < MASH384_WORDS) || ((i == 255U) && (check_difference_row(i, 0U) < MASH384_WORDS)) || (check_difference_table(i) < MIN_DIFF));
		print_row(i);
	}

	printf("\nCOMPLETED.\n\n");
	getchar();
	return EXIT_SUCCESS;
}
