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
#define _ITERATOR_DEBUG_LEVEL 0

#include "mhash_384.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <thread>
#include <typeinfo>
#include <csignal>
#include <intrin.h>
#include <ctime>
#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef NDEBUG
static const uint64_t MAX_VALUES = 0xF0D181;
#define THREAD_COUNT 1U
#define ABORT(X) abort()
#else
static const uint64_t MAX_VALUES = 0xEE6B277;
#define THREAD_COUNT 8U
#define ABORT(X) exit((X))
#endif

#define MAX_MSGLEN 8U

/*----------------------------------------------------------------------*/
/* Hash Value                                                           */
/*----------------------------------------------------------------------*/

class HashValue
{
public:
	HashValue(const uint8_t *const digest)
	{
		memcpy(m_digest, digest, sizeof(uint8_t) * mhash_384::MHash384::HASH_LEN);
	}

	HashValue(const std::vector<uint8_t> &digest)
	{
		if (digest.size() != mhash_384::MHash384::HASH_LEN)
		{
			throw std::runtime_error("Invalid digest size detected!");
		}
		memcpy(m_digest, digest.data(), sizeof(uint8_t) * mhash_384::MHash384::HASH_LEN);
	}

	bool operator==(const HashValue& other) const
	{
		return (memcmp(m_digest, other.m_digest, sizeof(uint8_t) * mhash_384::MHash384::HASH_LEN) == 0);
	}

	const uint8_t &operator[](const size_t i) const
	{
		return m_digest[i];
	}

	const uint8_t *cdata(void) const
	{
		return &m_digest[0];
	}

private:
	uint8_t m_digest[mhash_384::MHash384::HASH_LEN];
};

struct MyHasher {
	size_t operator() (const HashValue &hashValue) const
	{
		size_t h = 0;
		for (size_t i = 0; i < sizeof(size_t); ++i)
		{
			h = (h << 8) | hashValue[i];
		}
#ifdef _M_X64
		return _byteswap_uint64(h);
#else
		return _byteswap_ulong(h);
#endif
	}
};

/*----------------------------------------------------------------------*/
/* Globals                                                              */
/*----------------------------------------------------------------------*/

typedef std::unordered_set<HashValue, MyHasher> HashMap;

static HashMap g_hashSet;
static volatile int64_t stats[mhash_384::MHash384::HASH_LEN][256U];
static CRITICAL_SECTION g_spinlock;
static volatile bool g_stopped = false;

/*----------------------------------------------------------------------*/
/* Utility functions                                                    */
/*----------------------------------------------------------------------*/

#define _HEX_CHAR_MAP(X,Y) _HEX_CHARS[((X) >> (Y)) & 0xFU]
static const char *const _HEX_CHARS = "0123456789ABCDEF";

inline bool next_value(uint8_t *const value, const size_t len)
{
	size_t pos = len - 1U;
	uint8_t prev = value[pos];
	value[pos] += THREAD_COUNT;
	if (value[pos] > prev)
	{
		return true;
	}
	while (pos > 0)
	{
		prev = value[--pos];
		value[pos]++;
		if (value[pos] > prev)
		{
			return true;
		}
	}
	return false;
}

inline static uint64_t median3(const uint64_t a, const uint64_t b, const uint64_t c)
{
	if (a < c)
	{
		if (b < a)
			return a;
		else if (c < b)
			return c;
		else
			return b;
	}
	else if (b < c)
		return c;
	else if (a < b)
		return a;
	else
		return b;
}

inline static void hex2str(const uint8_t *const src, char *const dst, const size_t len)
{
	uint_fast16_t get, put = 0U;
	for (get = 0U; get < len; ++get)
	{
		dst[put++] = _HEX_CHAR_MAP(src[get], 4U);
		dst[put++] = _HEX_CHAR_MAP(src[get], 0U);
	}
	dst[put] = '\0';
}

inline static void print_status(const uint8_t *const value, const size_t len, const HashValue *const hash)
{
	char buffer_hash[(2U * mhash_384::MHash384::HASH_LEN) + 1U], buffer_msg[(2U * MAX_MSGLEN) + 1U];
	hex2str(value, buffer_msg, len);
	hex2str(hash->cdata(), buffer_hash, mhash_384::MHash384::HASH_LEN);
	printf("%s -> %s [%.2f]\n", buffer_msg, buffer_hash, g_hashSet.size() / (double)MAX_VALUES);
}

/*----------------------------------------------------------------------*/
/* THREAD                                                               */
/*----------------------------------------------------------------------*/

static void thread_check_hashes(const uint8_t *const value, const uint_fast32_t len, std::queue<HashValue> &queue)
{
	EnterCriticalSection(&g_spinlock);

	while (!queue.empty())
	{
		const HashValue &hashValue = queue.front();
		const bool collision = (g_hashSet.size() < MAX_VALUES) ? (!g_hashSet.insert(hashValue).second) : (g_hashSet.find(hashValue) != g_hashSet.cend());
		if (collision)
		{
			print_status(value, len, &hashValue);
			fprintf(stderr, "\nCOLLISION DETECTED !!!\n\n");
			ABORT(666);
		}

		static uint_fast32_t counter = 0U;
		++counter;
		if ((len == 1U) || ((len == 2U) && (counter >= 251U)) || ((len == 3U) && (counter >= 65521U)) || ((len > 3U) && (counter >= 1048573U)))
		{
			print_status(value, len, &hashValue);
			counter = 0U;
		}

		queue.pop(); /*remove the front element*/
	}

	LeaveCriticalSection(&g_spinlock);

	/*for (uint_fast16_t i = 0; i < mhash_384::MHash384::HASH_LEN; ++i)
	{
		_InterlockedIncrement64(&stats[i][hashValue[i]]);
	}*/
}

static void thread_main(const uint32_t offset)
{
	std::queue<HashValue> queue;
	uint8_t *value = NULL;
	for (uint_fast32_t len = 1; len <= MAX_MSGLEN; ++len)
	{
		value = (uint8_t*)realloc(value, sizeof(uint8_t) * len);
		memset(value, 0, sizeof(uint8_t) * len);
		value[len - 1U] += (offset % UINT8_MAX);
		do
		{
			mhash_384::MHash384 mhash384;
			mhash384.update(value, len);
			queue.push(mhash384.finalize());
			if (queue.size() >= 31U)
			{
				thread_check_hashes(value, len, queue);
				if (g_stopped)
				{
					return; /*signaled*/
				}
			}
		}
		while (next_value(value, len));
	}
}

/*----------------------------------------------------------------------*/
/* MAIN                                                                 */
/*----------------------------------------------------------------------*/

static void ctrl_c_signal_handler(int sval)
{
	g_stopped = true;
	signal(sval, ctrl_c_signal_handler);
}

int main()
{
	std::thread* threads[THREAD_COUNT];
	
	signal(SIGINT, ctrl_c_signal_handler);
	InitializeCriticalSectionAndSpinCount(&g_spinlock, 0x00000400);
	const clock_t start = clock();

	try
	{
		g_hashSet.reserve(MAX_VALUES);
		for (uint32_t t = 0U; t < THREAD_COUNT; ++t)
		{
			threads[t] = new std::thread(thread_main, t);
			if (!SetThreadPriority(threads[t]->native_handle(), THREAD_BASE_PRIORITY_IDLE))
			{
				abort();
			}
		}
		for (uint32_t t = 0U; t < THREAD_COUNT; ++t)
		{
			threads[t]->join();
		}
	}
	catch (std::exception& ba)
	{
		fprintf(stderr, "\nEXCEPTION: %s - %s !!!\n\n", typeid(ba).name(), ba.what());
	}

	/*for (uint_fast16_t i = 0; i < mhash_384::MHash384::HASH_LEN; ++i)
	{
		const double divisor = (double)median3(stats[i][0U], stats[i][1U], stats[i][2U]);
		putc('\n', stdout);
		for (uint_fast16_t j = 0; j < 256U; ++j)
		{
			printf("stats[%02X][%02X] = %llu (%.2f)", i, j, stats[i][j], stats[i][j] / divisor);
			putc((j & 1U) ? '\n' : '\t', stdout);
		}
	}*/

	printf("\nCOMPLETED after %.1f seconds.\n", (clock() - start) / (double)CLOCKS_PER_SEC);
	return 0;
}

