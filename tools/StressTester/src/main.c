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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NDEBUG
#define ABORT(X) abort()
#else
#define ABORT(X) exit((X))
#endif

/*----------------------------------------------------------------------*/
/* Hash Tree                                                            */
/*----------------------------------------------------------------------*/

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
	tree_insert_helper(&g_hash_tee, value, MHASH_384_LEN, &inserted);
	return inserted;
}

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

inline static void print_value(const uint8_t *const value, const size_t len)
{
	for (size_t k = 0; k < len; k++)
	{
		fprintf(stderr, "%02X", value[k]);
	}
}

inline static void test_hash(const uint8_t *const value, uint8_t *const result, const uint_fast32_t len)
{
	mhash_384_t context;

	mhash_384_initialize(&context);
	mhash_384_update(&context, value, len);
	mhash_384_finalize(&context, result);

	if (!tree_insert(result))
	{
		fprintf(stderr, "\nCOLLISION DETECTED !!!\n\n");
		ABORT(666);
	}
}

/*----------------------------------------------------------------------*/
/* MAIN                                                                 */
/*----------------------------------------------------------------------*/

int main()
{
	uint8_t *value = NULL;
	uint8_t result[MHASH_384_LEN];
	uint16_t count = 0U;

	memset(&g_hash_tee, 0, sizeof(tree_t));
	
	for (uint_fast32_t len = 1; len < 4; ++len)
	{
		value = (uint8_t*)realloc(value, sizeof(uint8_t) * len);
		memset(value, 0, sizeof(uint8_t) * len);
		do
		{
			test_hash(value, result, len);
			if ((len == 1) || ((len == 2) && (count++ >= 31)) || ((len > 2) && (count++ >= 997)))
			{
				print_value(value, len);
				fputs(" - ", stderr);
				print_value(result, MHASH_384_LEN);
				fputs("\n", stderr);
				count = 0;
			}
		}
		while (next_value(value, len));
	}

	fprintf(stderr, "\nCOMPLETED.\n\n");
	return 0;
}

