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

#include "mhash384.h"
#include "utils.h"

#include <sstream>

/*
 * Get base name from path
 */
const CHAR_T * get_basename(const CHAR_T *const path)
{
	const CHAR_T *basename = path;
	while(const CHAR_T *const offset = STRPBRK(basename, STR("/\\")))
	{
		basename = offset + 1U;
	}
	
	return basename;
}

/*
 * Convert byte array to Hex-string
 */
std::string bytes_to_hex(const uint8_t *const data, const size_t len, const bool lower_case)
{
	static const char *const HEXCHARS_UPR = "0123456789ABCDEF";
	static const char *const HEXCHARS_LWR = "0123456789abcdef";
	
	const char *const hexchars = lower_case ? HEXCHARS_LWR : HEXCHARS_UPR;
	std::ostringstream result;
	for(size_t i = 0U; i < len; ++i)
	{
		result << hexchars[(data[i] >> 4) & 0x0F];
		result << hexchars[       data[i] & 0x0F];
	}
	
	return result.str();
}
