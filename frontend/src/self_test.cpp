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
	{ 0x4C, 0x4B, 0x82, 0xD0, 0x7B, 0x36, 0x8E, 0x1C, 0x22, 0xD0, 0xDE, 0x37, 0x59, 0xC3, 0x2D, 0x44, 0xDA, 0x71, 0xBE, 0x62, 0x83, 0xE8, 0x55, 0x0A, 0x24, 0x68, 0xDC, 0x1F, 0xEC, 0x38, 0x91, 0x9F, 0x7E, 0xDB, 0x6C, 0x1B, 0xA0, 0x83, 0x78, 0xEC, 0x58, 0x3A, 0xE6, 0x12, 0xAB, 0x0E, 0x02, 0xBA },
	{ 0x91, 0x71, 0xD8, 0x3E, 0xE7, 0xDE, 0xDE, 0x36, 0xCA, 0xF2, 0x7C, 0x26, 0x44, 0x89, 0x7F, 0x31, 0x14, 0xA0, 0xF6, 0x7B, 0x6E, 0x91, 0x93, 0xAA, 0x1A, 0xB2, 0x34, 0x62, 0xEA, 0x81, 0x5E, 0xDE, 0xA5, 0x35, 0x00, 0x26, 0x71, 0xE0, 0x86, 0x49, 0x3B, 0x41, 0xA5, 0x28, 0xA2, 0x6F, 0xD8, 0xB3 },
	{ 0x29, 0x0B, 0xD2, 0x16, 0x2C, 0x21, 0x05, 0xA0, 0x82, 0x41, 0x72, 0xA8, 0x87, 0x5E, 0xE3, 0x3B, 0xB6, 0x5A, 0x98, 0xDC, 0x09, 0x28, 0x10, 0x04, 0x41, 0xB4, 0x1B, 0x93, 0x99, 0xF6, 0xA8, 0xEA, 0x09, 0x79, 0x48, 0x34, 0x50, 0x4A, 0x3E, 0x81, 0x7D, 0x49, 0xD2, 0x9B, 0xC2, 0x0A, 0x52, 0x0A },
	{ 0x0B, 0x3F, 0x13, 0xA6, 0x8A, 0xA8, 0xD8, 0xF0, 0xC5, 0xB9, 0xBF, 0x8B, 0xE5, 0xAE, 0xCC, 0xB7, 0x3E, 0x0D, 0x13, 0x73, 0x2C, 0x92, 0x90, 0x00, 0x6B, 0x6D, 0xC9, 0x39, 0xAD, 0xA7, 0x9C, 0x48, 0xAE, 0x36, 0x2E, 0x54, 0x5A, 0x06, 0x7D, 0x2C, 0x1F, 0xB0, 0x74, 0x9C, 0x60, 0xA4, 0x92, 0x43 },
	{ 0x56, 0x22, 0x8E, 0x94, 0x32, 0x47, 0x1B, 0x09, 0xA7, 0xF6, 0x96, 0xD0, 0xDE, 0xFA, 0x15, 0xE6, 0x64, 0xD3, 0xE7, 0xAC, 0xD2, 0x7E, 0x2D, 0x39, 0xF8, 0x64, 0xC0, 0x50, 0x06, 0xF1, 0xF7, 0x70, 0x12, 0xF4, 0xF4, 0xCC, 0xE7, 0x45, 0x0C, 0x52, 0xB6, 0xC1, 0xCF, 0xAB, 0x84, 0xFA, 0xEC, 0x63 },
	{ 0x3A, 0x19, 0x9A, 0x67, 0x3F, 0xAB, 0x29, 0x00, 0xAB, 0x80, 0xFE, 0xC1, 0x18, 0x5F, 0x79, 0x35, 0x9F, 0xEC, 0x44, 0xB8, 0x87, 0x28, 0xE3, 0xD6, 0x2D, 0xC3, 0x1A, 0x93, 0x6C, 0x62, 0xDB, 0x05, 0xEF, 0x35, 0x71, 0x6F, 0xED, 0x30, 0x74, 0xE9, 0x31, 0x0D, 0xDD, 0xF6, 0x9E, 0xD5, 0x67, 0x1C },
	{ 0x79, 0xF7, 0x6C, 0xA5, 0x3D, 0x52, 0x91, 0x62, 0xE6, 0x32, 0x15, 0x2E, 0xDE, 0x82, 0xA4, 0x03, 0xF8, 0xF9, 0x96, 0xDE, 0xAA, 0x00, 0x9C, 0xC5, 0x12, 0x25, 0x0B, 0xAF, 0xF9, 0x10, 0xAC, 0x24, 0xDF, 0x13, 0x81, 0xF7, 0xEF, 0x1F, 0x43, 0xDA, 0xC2, 0x6F, 0x63, 0xEE, 0x0C, 0xFF, 0x3C, 0xDF },
	{ 0x8A, 0x2A, 0x58, 0xB2, 0x00, 0x20, 0xF7, 0x70, 0x0F, 0xFF, 0x62, 0x9B, 0x0D, 0x72, 0x38, 0xD3, 0xD5, 0x31, 0x1A, 0xC2, 0xA9, 0xAD, 0xA6, 0x06, 0xE6, 0x9A, 0xD7, 0xBE, 0xBF, 0x2B, 0x62, 0x58, 0xAE, 0xC7, 0x40, 0x80, 0xDE, 0xC0, 0x4A, 0xD5, 0x9F, 0x3B, 0x93, 0x26, 0x12, 0x1D, 0xFF, 0x66 },
	{ 0xD2, 0xE0, 0x7E, 0xA3, 0x7E, 0xF1, 0xE0, 0xE5, 0x2B, 0xB7, 0x04, 0xDE, 0xC3, 0x33, 0x0C, 0x33, 0x78, 0xB9, 0x43, 0xFE, 0x24, 0x2C, 0xF3, 0xB0, 0x8B, 0x93, 0xD1, 0x8D, 0xBD, 0x61, 0xD4, 0xAB, 0x7C, 0x42, 0xE5, 0x81, 0xDB, 0xFD, 0xBF, 0xD2, 0xF5, 0xD8, 0xED, 0xF8, 0x2C, 0x3B, 0x35, 0xD6 },
	{ 0xE9, 0x7C, 0x79, 0x0B, 0x19, 0x45, 0x32, 0xA5, 0x9B, 0xC8, 0x40, 0x90, 0xB5, 0xC6, 0x8C, 0x5B, 0x0D, 0x05, 0x0C, 0x6F, 0xE9, 0x37, 0xAB, 0xDF, 0x48, 0x0C, 0xC1, 0x9C, 0x34, 0x5B, 0x72, 0xFE, 0xF9, 0x25, 0xD8, 0x3B, 0xF9, 0xB4, 0x2D, 0x1A, 0x8F, 0x57, 0x2A, 0xDE, 0x7A, 0x50, 0x9F, 0xF9 },
	{ 0xA7, 0x72, 0xD7, 0xB9, 0x84, 0xAB, 0xC7, 0x90, 0xA9, 0xFF, 0xF5, 0x1F, 0x3B, 0xD7, 0xC6, 0xA5, 0x38, 0x44, 0xA2, 0x33, 0xA5, 0x64, 0xA9, 0x70, 0x87, 0x2C, 0x41, 0x34, 0x5A, 0xFE, 0x19, 0x98, 0x3B, 0x8D, 0x3C, 0xE3, 0x0B, 0x90, 0x0F, 0xD7, 0xFD, 0xD6, 0x6C, 0xED, 0x03, 0xD0, 0xCD, 0x6E },
	{ 0x61, 0x4A, 0x6B, 0x25, 0xBD, 0x67, 0x32, 0x16, 0xED, 0xEA, 0xB6, 0xA0, 0x51, 0xA8, 0xB4, 0x86, 0x9F, 0x9A, 0xD8, 0x0C, 0xC5, 0xDD, 0x4A, 0xE6, 0x29, 0xDD, 0xFB, 0x70, 0xCA, 0xA7, 0x0E, 0x49, 0xD5, 0x1E, 0x70, 0x27, 0xFF, 0x35, 0xA1, 0x83, 0xA2, 0x78, 0xFE, 0x97, 0xF8, 0x75, 0x9C, 0xF9 }
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
static bool test_string(const uint32_t count, const char *const text, const uint8_t *const expected, const options_t &options)
{
	MHash384 mhash384;
	for(uint32_t i = 0U; i < count; ++i)
	{
		mhash384.update(text);
	}

	const uint8_t *const digest = mhash384.finish();
	const bool success = (!memcmp(digest, expected, MHASH384_SIZE));

	const std::string string = options.base64 ? bytes_to_base64(digest, MHASH384_SIZE) : bytes_to_hex(digest, MHASH384_SIZE, options.lower_case);
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
bool self_test(const options_t &options)
{
	bool success = mhash384_selftest();

	if(success)
	{
		for(size_t i = 0U; SELFTEST_INPUT[i].count > 0U; ++i)
		{
			if(!test_string(SELFTEST_INPUT[i].count, SELFTEST_INPUT[i].string, SELFTEST_EXPECTED[i], options))
			{
				success = false;
				if(!options.keep_going)
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
bool stress_test(const CHAR_T *const file_name, const options_t &options)
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
			if(!append_string(hash_set, stats, line.c_str(), options.base64, options.lower_case))
			{
				success = false;
				if(!options.keep_going)
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
