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

#include "self_test.h"
#include "utils.h"
#include "mhash384.h"

#include <cstring>
#include <unordered_set>
#include <array>
#include <iostream>
#include <fstream>
#include <algorithm>

/*
 * Test-case specification
 */
typedef struct _test_case_t
{
	uint32_t count;
	const char *string;
}
test_case_t;

/*
 * Pre-defined test-cases
 */
static const test_case_t SELFTEST_INPUT[] =
{
	{ 0x00000001, "" },
	{ 0x00000001, "abc" },
	{ 0x00000001, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
	{ 0x00000001, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu" },
	{ 0x000186A0, "aaaaaaaaaa" },
	{ 0x01000000, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno" },
	{ 0x00000001, "The quick brown fox jumps over the lazy dog" },
	{ 0x00000001, "The quick brown fox jumps over the lazy cog" },
	{ 0x00000001, "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern" },
	{ 0x00000001, "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern" },
	{ 0x00000001, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." },
	{ 0x00000001, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamc0 laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." },
	{ 0x00000000, NULL} /*EOL*/
};

/*
 * Expected hash values
 */
static const uint8_t SELFTEST_EXPECTED[][MHASH384_SIZE] =
{
	{ 0xD6, 0x29, 0xCB, 0x37, 0xF7, 0x61, 0x56, 0x62, 0x1E, 0x09, 0xC9, 0x3B, 0x8D, 0x10, 0x29, 0xC5, 0xF0, 0xF7, 0xB7, 0x78, 0xDE, 0x5B, 0xD9, 0x57, 0x82, 0x30, 0x67, 0x3F, 0xE7, 0xFA, 0x4C, 0x45, 0xA2, 0x30, 0x8A, 0xB4, 0xE9, 0xD9, 0x9A, 0xA0, 0xEA, 0x19, 0x4C, 0xBF, 0x62, 0x30, 0xC2, 0x02 },
	{ 0x1D, 0x7B, 0x15, 0x93, 0xD7, 0xAA, 0x4D, 0xD7, 0xDC, 0x8D, 0xA6, 0xD6, 0xBD, 0x59, 0x1D, 0x8B, 0x50, 0x7C, 0xA5, 0x14, 0xE1, 0xC7, 0xEC, 0x37, 0xF4, 0x3E, 0x78, 0xDD, 0x6B, 0x83, 0x25, 0x3C, 0x35, 0x54, 0x03, 0xF6, 0x10, 0x09, 0x58, 0x28, 0x90, 0x48, 0x0C, 0x4C, 0xD5, 0x27, 0xF4, 0x05 },
	{ 0x9B, 0x98, 0xCE, 0x4C, 0x3F, 0x4E, 0xA9, 0x01, 0x8E, 0x1E, 0xD3, 0x03, 0x47, 0x17, 0xE3, 0xE5, 0xD8, 0xDA, 0x20, 0xAB, 0x37, 0x5B, 0xAF, 0xE3, 0x72, 0xD0, 0x77, 0x80, 0xFF, 0xDE, 0xFD, 0xA3, 0x2B, 0xA3, 0xC8, 0x22, 0x15, 0x5E, 0x28, 0x83, 0xE3, 0x33, 0xAF, 0xCA, 0xAE, 0xAF, 0xA6, 0x14 },
	{ 0x45, 0x41, 0x6D, 0x1F, 0xFD, 0x44, 0xD3, 0x94, 0x0C, 0x5A, 0xF5, 0xB4, 0x25, 0xB5, 0xDA, 0x07, 0xFD, 0x8C, 0x06, 0xBA, 0xB7, 0xC5, 0x8C, 0xD1, 0x33, 0x89, 0xBC, 0x28, 0x46, 0x50, 0xED, 0x60, 0x90, 0xA9, 0xA1, 0x95, 0x2C, 0x29, 0xFE, 0xB1, 0xB2, 0xC6, 0xAF, 0xBF, 0x5F, 0x99, 0x61, 0x08 },
	{ 0x20, 0x77, 0x4C, 0x57, 0x0A, 0xE2, 0xE6, 0xA4, 0x9A, 0x01, 0xDA, 0xE9, 0x48, 0x67, 0x2A, 0xE9, 0x4D, 0x46, 0x5D, 0x91, 0x67, 0x13, 0x0F, 0xE8, 0x7B, 0xF2, 0x41, 0x48, 0x64, 0x9C, 0xF1, 0x21, 0xE8, 0xA5, 0x07, 0x59, 0x50, 0x7B, 0xF9, 0x71, 0x34, 0xF0, 0xD2, 0x8C, 0xEE, 0x91, 0xDC, 0xA2 },
	{ 0xC1, 0x88, 0x43, 0xD4, 0xDD, 0x9E, 0x04, 0x79, 0xD3, 0x8A, 0x55, 0xE8, 0xC7, 0x5E, 0x22, 0x01, 0xF0, 0xE9, 0x3B, 0x6E, 0xF1, 0xF9, 0x16, 0xEB, 0xAF, 0x3C, 0x40, 0x74, 0xD1, 0x88, 0x94, 0x5C, 0x7B, 0x96, 0x57, 0xF4, 0x52, 0x69, 0xA1, 0xDA, 0xAB, 0x4A, 0x8F, 0xB6, 0xF6, 0x3D, 0x0B, 0xEE },
	{ 0xE2, 0x90, 0x1E, 0xDB, 0x76, 0xAA, 0x83, 0xF2, 0xD5, 0x2F, 0xEA, 0x9A, 0x34, 0xEB, 0x2B, 0xC0, 0x5E, 0xDF, 0xA9, 0x57, 0x1C, 0x81, 0x26, 0x20, 0x24, 0x5B, 0xBE, 0xBA, 0x95, 0xFA, 0x2D, 0x82, 0xCB, 0xB5, 0xBA, 0xD9, 0x3E, 0xB6, 0x4D, 0x4B, 0x72, 0xB4, 0x66, 0x35, 0xD5, 0x65, 0x40, 0x0B },
	{ 0x8B, 0x81, 0xCD, 0x9C, 0x1D, 0xEE, 0x8B, 0xCC, 0x64, 0x2F, 0x8B, 0x1E, 0xA9, 0x82, 0xCE, 0x79, 0xD1, 0x5F, 0x50, 0xE6, 0x69, 0x39, 0xFF, 0x34, 0x29, 0x4E, 0x63, 0xD9, 0x3A, 0xE3, 0xC6, 0x10, 0xF9, 0x46, 0x4F, 0xEF, 0xEF, 0x30, 0x29, 0x74, 0x09, 0xB6, 0x22, 0x31, 0xF0, 0x28, 0xAB, 0xC0 },
	{ 0xA5, 0x45, 0xE6, 0x31, 0x9F, 0x6A, 0xCE, 0x0C, 0xEF, 0x62, 0xBC, 0xAA, 0xF5, 0xD4, 0x88, 0x85, 0x87, 0x51, 0xB9, 0x6C, 0x2A, 0x48, 0x8D, 0x5B, 0xE6, 0xEC, 0x63, 0xD5, 0x00, 0x06, 0xC9, 0xD5, 0x4D, 0xAD, 0x66, 0x16, 0x84, 0x88, 0xF0, 0x3B, 0x4A, 0x86, 0x8D, 0x3B, 0xA6, 0x10, 0xCC, 0xF8 },
	{ 0x2B, 0xEC, 0xB8, 0x9D, 0xD5, 0x5F, 0xC9, 0xF2, 0xF3, 0x38, 0x4D, 0xF6, 0x1D, 0x62, 0x40, 0xDD, 0xA5, 0x4F, 0xC2, 0x75, 0xD1, 0x86, 0xA4, 0x5A, 0x88, 0x3C, 0x03, 0xAA, 0x87, 0x3D, 0xDA, 0xF4, 0xFA, 0xD4, 0x74, 0x42, 0x12, 0xB9, 0x2D, 0x26, 0x05, 0x4B, 0x92, 0x85, 0x2D, 0xEA, 0x5F, 0x1D },
	{ 0x62, 0x83, 0xCE, 0x91, 0xDA, 0x8A, 0xF5, 0x09, 0x1C, 0x7B, 0x06, 0xE1, 0x35, 0x2D, 0xC9, 0x47, 0xB9, 0xE3, 0xB8, 0x96, 0x14, 0x80, 0x7C, 0x04, 0xC4, 0x3D, 0xA3, 0x22, 0x32, 0x1F, 0xBA, 0xDA, 0xD3, 0xCC, 0xCA, 0xBE, 0xCB, 0x5C, 0x1B, 0x1B, 0x26, 0x54, 0x32, 0xA6, 0x03, 0x91, 0x21, 0xE1 },
	{ 0x5D, 0xCB, 0x7C, 0x4B, 0x27, 0xBA, 0x84, 0x16, 0x88, 0xFB, 0x6A, 0x86, 0x0E, 0x97, 0xA4, 0x52, 0x56, 0x91, 0x75, 0x33, 0xF6, 0x34, 0x48, 0xCF, 0x66, 0x67, 0x81, 0xEA, 0x4B, 0x5F, 0xAB, 0x20, 0xE1, 0xC7, 0x46, 0xC8, 0x82, 0x10, 0x99, 0xA9, 0x15, 0x22, 0x2E, 0x0D, 0x77, 0x62, 0x40, 0x98 }
};

/*
 * Hash function helper class for std::unordered_set
 */
struct KeyHasher
{
	std::size_t operator()(const std::array<std::uint8_t,MHASH384_SIZE> &key) const
	{
		if(key.size() >= sizeof(std::size_t))
		{
			return *reinterpret_cast<const size_t*>(key.data());
		}
		else if(key.size() >= sizeof(uint32_t))
		{
			return *reinterpret_cast<const uint32_t*>(key.data());
		}
		else if(key.size() >= sizeof(uint16_t))
		{
			return *reinterpret_cast<const uint16_t*>(key.data());
		}
		else
		{
			return (key.size() > 0U) ? key[0U] : 0U;
		}
	}
};

/*
 * Comparison function helper class for std::unordered_set
 */
struct KeyEqualTo
{
	bool operator()(const std::array<std::uint8_t,MHASH384_SIZE> &a, const std::array<std::uint8_t,MHASH384_SIZE> &b) const
	{
		if (a.size() == b.size())
		{
			return (memcmp(a.data(), b.data(), a.size()) == 0);
		}
		return false;
	}
};

/*
 * Unordered set of hash values
 */
typedef std::unordered_set<std::array<std::uint8_t,MHASH384_SIZE>, KeyHasher, KeyEqualTo> UnorderedHashSet;
typedef UnorderedHashSet::iterator HashSetIter;

/*
 * Compute hash and compare against reference
 */
static bool test_string(const uint32_t count, const char *const text, const uint8_t *const expected, const bool base64, const bool lower_case)
{
	MHash384 mhash384;
	for(uint32_t i = 0U; i < count; ++i)
	{
		mhash384.update(text);
	}

	const uint8_t *const digest = mhash384.finish();
	const bool success = (!memcmp(digest, expected, MHASH384_SIZE));

	const std::string string = base64 ? bytes_to_base64(digest, MHASH384_SIZE) : bytes_to_hex(digest, MHASH384_SIZE, lower_case);
	const CHAR_T *const result = success ? STR("OK") : STR("Error!");
	FPRINTF(stderr, STR("%") PRI_char STR(" - %") PRI_CHAR STR("\n"), string.c_str(), result);
	
	fflush(stderr);
	return success;
}

/*
 * Compute hash and append to hashset
 */
static bool append_string(UnorderedHashSet &hash_set, std::vector<std::array<uint64_t,256U>> &stats, const char *const text, const bool base64, const bool lower_case)
{
	std::array<uint8_t, MHASH384_SIZE> digest;
	mhash384_compute(digest.data(), reinterpret_cast<const uint8_t*>(text), strlen(text));

	const std::string string = base64 ? bytes_to_base64(digest.data(), MHASH384_SIZE) : bytes_to_hex(digest.data(), MHASH384_SIZE, lower_case);
	FPRINTF(stderr, STR("%") PRI_char STR("\n"), string.c_str());
	
	for(size_t i = 0U; i < MHASH384_SIZE; ++i)
	{
		stats[i][digest[i]]++;
	}

	std::pair<HashSetIter, bool> result = hash_set.insert(digest);
	if(!result.second)
	{
		FPRINTF(stderr, STR("Collision detected: \"%") PRI_char STR("\"\n"), text);
	}
	
	fflush(stderr);
	return result.second;
}

/*
 * Self-testing routine
 */
bool self_test(const bool keep_going, const bool base64, const bool lower_case)
{
	bool success = mhash384_selftest();

	if(success)
	{
		for(size_t i = 0U; SELFTEST_INPUT[i].count > 0U; ++i)
		{
			if(!test_string(SELFTEST_INPUT[i].count, SELFTEST_INPUT[i].string, SELFTEST_EXPECTED[i], base64, lower_case))
			{
				success = false;
				if(!keep_going)
				{
					break; /*failure*/
				}
			}
		}
	}

	if(success)
	{
		FPUTS(STR("\nSelf-test completed successfully :-)\n"), stderr);
	}
	else
	{
		FPUTS(STR("\nError: Self-test has failed! :-(\n"), stderr);
	}

	return success;
}

/*
 * Stress-testing routine
 */
bool stress_test(const CHAR_T *const file_name, const bool keep_going, const bool base64, const bool lower_case)
{
	std::ifstream infile;
	if(file_name)
	{
		infile.open(file_name);
	}

	std::istream &instream = file_name ? infile : std::cin;
	if(!instream.good())
	{
		FPUTS(STR("Error: Failed to open input file for reading!\n"), stderr);
		return false;
	}

	std::vector<std::array<uint64_t,256U>> stats(MHASH384_SIZE);
	for(size_t i = 0U; i < MHASH384_SIZE; ++i)
	{
		stats[i].fill(0U);
	}

	bool success = true;
	UnorderedHashSet hash_set;
	std::string line;

	while(instream.good())
	{
		std::getline(instream, line);
		if(!instream.fail()) 
		{
			if(!append_string(hash_set, stats, line.c_str(), base64, lower_case))
			{
				success = false;
				if(!keep_going)
				{
					break; /*collison*/
				}
			}
		}
	}

	FPUTS(STR("\n[STATS]\n"), stderr);
	for(size_t i = 0U; i < MHASH384_SIZE; ++i)
	{
		uint64_t min_value = UINT64_MAX;
		uint64_t max_value = 0U;
		for(size_t j = 0U; j < 256U; ++j)
		{
			min_value = std::min(min_value, stats[i][j]);
			max_value = std::max(max_value, stats[i][j]);
		}
		const double ratio = (max_value > 0U) ? min_value / static_cast<double>(max_value): 0.0;
		FPRINTF(stderr, STR("%02u: %06") STR(PRIu64) STR(" <-> %06") STR(PRIu64) STR(" [%.2f]"), static_cast<unsigned int>(i), min_value, max_value, ratio);
		FPUTS(((i % 3U) == 2U) ? STR("\n") : STR("  "), stderr);
	}

	if(success)
	{
		if(instream.eof() && (!instream.bad()))
		{
			FPRINTF(stderr, STR("\nStress-test completed successfully. [%") STR(PRIu64) STR("]\n"), static_cast<uint64_t>(hash_set.size()));
		}
		else
		{
			FPUTS(STR("\nError: File read error has occurred!\n"), stderr);
			success = false;
		}
	}
	else
	{
		FPRINTF(stderr, STR("\nStress-test ended *with* collision! [%") STR(PRIu64) STR("]\n"), static_cast<uint64_t>(hash_set.size()));
	}
	
	if(file_name && infile.is_open())
	{
		infile.close();
	}

	return success;
}
