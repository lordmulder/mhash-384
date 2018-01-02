/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Stress tester                                                                       */
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

#define _CRT_RAND_S

#include "mhash_384.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef NDEBUG
#define ABORT(X) abort()
#else
#define ABORT(X) exit((X))
#endif

#define THREAD_COUNT 8

/*----------------------------------------------------------------------*/
/* Counter                                                              */
/*----------------------------------------------------------------------*/

inline int next_value(uint8_t *const value, const size_t len)
{
	size_t pos = len;
	while (pos > 0)
	{
		value[--pos] += 1;
		if (value[pos])
		{
			return 1;
		}
	}
	return 0;
}

/*----------------------------------------------------------------------*/
/* Test functions                                                       */
/*----------------------------------------------------------------------*/

static std::mutex g_muext;
static std::condition_variable g_ready;
static uint16_t g_ready_N = 0U;

inline static void print_value(const uint8_t *const value, const size_t len)
{
	for (size_t k = 0; k < len; k++)
	{
		printf("%02X", value[k]);
	}
}

inline static void make_ref(uint8_t *const reference, const int seed)
{
	uint8_t value[1];
	value[0] = (uint8_t)seed;
	mhash_384::MHash384 mhash384;
	mhash384.update(value, 1);
	mhash384.finalize(reference);
}

inline static bool test_hash(const uint8_t *const reference, const uint8_t *const value, const uint_fast32_t len)
{
	uint8_t digest[mhash_384::MHash384::HASH_LEN];
	mhash_384::MHash384 mhash384;
	mhash384.update(value, len);
	mhash384.finalize(digest);
	return (memcmp(reference, digest, mhash_384::MHash384::HASH_LEN) != 0);
}

static void thread_main(const int seed)
{
	uint8_t *value = NULL;
	uint8_t reference[mhash_384::MHash384::HASH_LEN];
	uint_fast16_t count = 0U;

	{
		std::unique_lock<std::mutex> lock(g_muext);
		printf("Thread: %X --> ", seed);
		make_ref(reference, seed);
		print_value(reference, mhash_384::MHash384::HASH_LEN);
		putc('\n', stdout);
		g_ready_N++;
		g_ready.notify_all();
		while (g_ready_N < THREAD_COUNT)
		{
			g_ready.wait(lock);
		}
	}

	for (uint_fast32_t len = 1; len < 8U; ++len)
	{
		value = (uint8_t*)realloc(value, sizeof(uint8_t) * len);
		memset(value, 0, sizeof(uint8_t) * len);
		do
		{
			if (!(count++ % 99991U))
			{
				std::unique_lock<std::mutex> lock(g_muext);
				printf("%X: ", seed);
				print_value(value, len);
				putc('\n', stdout);
			}
			if(!test_hash(reference, value, len))
			{
				if (!((len == 1) && (value[0] == ((uint8_t)seed))))
				{
					fprintf(stderr, "\nCOLLISION DETECTED !!!\n\n");
					ABORT(666);
				}
			}
		}
		while (next_value(value, len));
	}
}

/*----------------------------------------------------------------------*/
/* MAIN                                                                 */
/*----------------------------------------------------------------------*/

int main()
{
	std::thread* threads[THREAD_COUNT];

	for (size_t i = 0; i < THREAD_COUNT; ++i)
	{
		threads[i] = new std::thread(thread_main, (int)i);
	}

	for (size_t i = 0; i < THREAD_COUNT; ++i)
	{
		threads[i]->join();
	}

	puts("\nCOMPLETED.\n");
	return 0;
}

