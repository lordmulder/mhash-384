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

#define MASH384_SIZE 48U

static uint32_t next_rand(const uint32_t max)
{
	uint32_t rnd;
	if(rand_s(&rnd) != 0)
	{
		abort();
	}
	return rnd % max;
}

static bool create_permutation(uint32_t *const out, const uint32_t max_retry)
{
	bool map[MASH384_SIZE];
	memset(map, 0U, sizeof(bool) * MASH384_SIZE);
	uint32_t retry = 0U;
	for(size_t i = 0U; i < MASH384_SIZE; ++i)
	{
		uint32_t index;
		do
		{
			index = next_rand(MASH384_SIZE);
			if(++retry > max_retry)
			{
				return false;
			}
		}
		while(map[index] || ((i > 0U) && (((index / 8U) == (out[i - 1U] / 8U)) || ((index % 8U) == (out[i - 1U] % 8U)))));
		out[i] = index;
		map[index] = true;
	}
	return true;
}

static void create_permutation(uint32_t *const out)
{
}

int main()
{
	uint32_t permutation[MASH384_SIZE];

	for(uint32_t i = 997U; i < UINT32_MAX; ++i)
	{
		if(create_permutation(permutation, i))
		{
			break; /*success*/
		}
	}

	for(size_t i = 0U; i < MASH384_SIZE; ++i)
	{
		printf("%02u %02u %02u\n", permutation[i], permutation[i] / 8U, permutation[i] % 8U);
	}
	puts("");

	for(size_t i = 0U; i < MASH384_SIZE; ++i)
	{
		printf(((i % 16) > 0U) ? ", 0x%02X" : "0x%02X", permutation[i]);
		if((i % 16U) == 15U)
		{
			puts(",");
		}
	}

	puts("\nCompleted.\n");
	return EXIT_SUCCESS;
}
