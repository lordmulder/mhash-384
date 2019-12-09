/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2019 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

#ifndef INC_MHASH384_H
#define INC_MHASH384_H

#ifdef __cplusplus
#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <string>
#else
#include <stdlib.h>
#include <stdint.h>
#endif

/*
 * MHash-384 digest size: 48 bytes
 */
#define MHASH384_WORDS 6U
#define MHASH384_SIZE (sizeof(uint64_t) * MHASH384_WORDS)

/*
 * Enable "extern C" on C++ compilers
 */
#ifdef __cplusplus
#define MHASH384_API extern "C"
#else
#define MHASH384_API
#endif

/*
 * Context for hash computation: 384-bit status + 8-bit counter
 */
typedef struct _mhash_384_t
{
	uint8_t rnd;
	uint64_t hash[MHASH384_WORDS];
}
mhash384_t;

/*
 * MHash-384 public functions
 */
MHASH384_API void mhash384_init    (mhash384_t *const ctx);
MHASH384_API void mhash384_update  (mhash384_t *const ctx, const uint8_t *const data_in, const size_t len);
MHASH384_API void mhash384_final   (mhash384_t *const ctx, uint8_t *const digest_out);
MHASH384_API void mhash384_get     (uint8_t *const digest_out, const uint8_t *const data_in, const size_t len);
MHASH384_API void mhash384_version (uint16_t *const major, uint16_t *const minor, uint16_t *const patch);
#ifndef MHASH384_NOSELFTEST
MHASH384_API bool mhash384_selftest(void);
#endif //MHASH384_NOSELFTEST

/*
 * MHash384 wrapper class for C++
 */
#ifdef __cplusplus
class MHash384
{
public:
	MHash384(void) : finished(false)
	{
		::mhash384_init(&ctx);
	}

	void update(const std::uint8_t *const data, const size_t len)
	{
		if(finished)
		{
			throw new std::logic_error("Hash computation is finished. Must call reset() first!");
		}
		::mhash384_update(&ctx, data, len);
	}

	void update(const std::vector<std::uint8_t> &data)
	{
		update(data.data(), data.size());
	}

	void update(const std::string &text)
	{
		update(reinterpret_cast<const std::uint8_t*>(text.c_str()), text.length());
	}

	const std::uint8_t *finish(void)
	{
		if(!finished)
		{
			::mhash384_final(&ctx, digest);
			finished = true;
		}
		return digest;
	}

	void reset(void)
	{
		::mhash384_init(&ctx);
		finished = false;
	}

protected:
	::mhash384_t ctx;
	std::uint8_t digest[MHASH384_SIZE];
	bool finished;
};
#endif /*__cplusplus*/

#endif /*INC_MHASH384_H*/
