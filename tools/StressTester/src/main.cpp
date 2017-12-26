/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Stress tester                                                                       */
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

#define _CRT_RAND_S

#include "mhash_384.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <typeinfo>

#ifndef NDEBUG
#define ABORT(X) abort()
#else
#define ABORT(X) exit((X))
#endif

static const uint64_t MAX_VALUES = 0x11945561;
                                     
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

	const void print(void) const
	{
		for (size_t k = 0; k < mhash_384::MHash384::HASH_LEN; k++)
		{
			printf("%02X", m_digest[k]);
		}
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
/* Hash Tree                                                            */
/*----------------------------------------------------------------------*/

/*
typedef struct tree_t
{
	uint8_t *tail;
	struct tree_t *next[UINT8_MAX+1];
}
tree_t;

static tree_t g_hash_tee;

inline static void tree_insert_helper(tree_t *const node, const uint8_t *const value, const size_t bytes_left, int *const inserted)
{
	if (bytes_left > 0)
	{
		if (node->tail)
		{
			if (!memcmp(node->tail, value, sizeof(uint8_t) * (bytes_left)))
			{
				return;
			}
		}
		if (node->next[*value] == NULL)
		{
			*inserted = 1;
			if ((node->next[*value] = (tree_t*)calloc(1, sizeof(tree_t))) == NULL)
			{
				fprintf(stderr, "\nOUT OF MEMORY !!!\n\n");
				ABORT(-1);
			}
			if (bytes_left > 1)
			{
				if ((node->next[*value]->tail = (uint8_t*)malloc(sizeof(uint8_t) * (bytes_left - 1))) == NULL)
				{
					fprintf(stderr, "\nOUT OF MEMORY !!!\n\n");
					ABORT(-1);
				}
				memcpy(node->next[*value]->tail, value + 1, sizeof(uint8_t) * (bytes_left - 1));
			}
			if (node->tail)
			{
				uint8_t *const temp = node->tail;
				node->tail = NULL;
				tree_insert_helper(node, temp, bytes_left, inserted);
				free(temp);
			}
		}
		else
		{
			tree_insert_helper(node->next[*value], value + 1, bytes_left - 1, inserted);
		}
	}
}

inline static int tree_insert(const uint8_t *const value)
{
	int inserted = 0;
	tree_insert_helper(&g_hash_tee, value, mhash_384::MHash384::HASH_LEN, &inserted);
	return inserted;
}
*/

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

/*----------------------------------------------------------------------*/
/* Test functions                                                       */
/*----------------------------------------------------------------------*/

typedef std::unordered_set<HashValue, MyHasher> HashMap;
static HashMap g_hashSet;
static uint64_t stats[mhash_384::MHash384::HASH_LEN][256U];

inline static void print_value(const uint8_t *const value, const size_t len)
{
	for (size_t k = 0; k < len; k++)
	{
		printf("%02X", value[k]);
	}
}

inline static void print_status(const uint8_t *const value, const size_t len, const HashMap::iterator &iter)
{
	print_value(value, len);
	fputs(" - ", stdout);
	iter->print();
	putc('\n', stdout);
}

inline static HashMap::iterator test_hash(const uint8_t *const value, const uint_fast32_t len)
{
	mhash_384::MHash384 mhash384;
	mhash384.update(value, len);
	const HashValue hashValue(mhash384.finalize());
	const std::pair<HashMap::iterator, bool> ret = g_hashSet.insert(hashValue);
	if (!ret.second)
	{
		print_status(value, len, ret.first);
		fprintf(stderr, "\nCOLLISION DETECTED !!!\n\n");
		ABORT(666);
	}
	for (uint_fast16_t i = 0; i < mhash_384::MHash384::HASH_LEN; ++i)
	{
		stats[i][hashValue[i]]++;
	}
	return ret.first;
}

/*----------------------------------------------------------------------*/
/* MAIN                                                                 */
/*----------------------------------------------------------------------*/

int main()
{
	uint8_t *value = NULL;
	uint64_t count = 0U;
	
	try
	{
		g_hashSet.reserve(MAX_VALUES);
		for (uint_fast32_t len = 1; len < 8U; ++len)
		{
			value = (uint8_t*)realloc(value, sizeof(uint8_t) * len);
			memset(value, 0, sizeof(uint8_t) * len);
			do
			{
				const HashMap::iterator iter = test_hash(value, len);
				if (++count >= MAX_VALUES)
				{
					goto completed; /*done*/
				}
				if ((len == 1) || ((len == 2) && (!(count % 31))) || ((len > 2) && (!(count % 997))))
				{
					print_status(value, len, iter);
				}
			}
			while (next_value(value, len));
		}
	}
	catch (std::exception& ba)
	{
		fprintf(stderr, "\nEXCEPTION: %s - %s !!!\n\n", typeid(ba).name(), ba.what());
	}

completed:
	for (uint_fast16_t i = 0; i < mhash_384::MHash384::HASH_LEN; ++i)
	{
		const double divisor = (double)median3(stats[i][0U], stats[i][1U], stats[i][2U]);
		putc('\n', stdout);
		for (uint_fast16_t j = 0; j < 256U; ++j)
		{
			printf("stats[%02X][%02X] = %llu (%.2f)", i, j, stats[i][j], stats[i][j] / divisor);
			putc((j & 1U) ? '\n' : '\t', stdout);
		}
	}

	puts("\nCOMPLETED.\n");
	return 0;
}

