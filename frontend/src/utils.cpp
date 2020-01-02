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

/*
 * Convert byte array to Base64-string
 * implementation based on code created by Joe DF <https://github.com/joedf/base64.c>
 */
std::string bytes_to_base64(const uint8_t *const data, const size_t len)
{
	static const char *const B64CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	uint32_t s[3U];
	std::ostringstream result;
	
	size_t j = 0U;
	for (size_t i = 0U; i < len; ++i)
	{
		s[j++] = data[i];
		if (j >= 3U)
		{
			result << B64CHARS[ (s[0U] & 0xFF) >> 2U];
			result << B64CHARS[((s[0U] & 0x03) << 4U) + ((s[1U] & 0xF0) >> 4U)];
			result << B64CHARS[((s[1U] & 0x0F) << 2U) + ((s[2U] & 0xC0) >> 6U)];
			result << B64CHARS[  s[2U] & 0x3F];
			j = 0U;
		}
	}
	
	if (j > 0U)
	{
		if (j == 1U)
		{
			s[1U] = 0;
		}
		result << B64CHARS[(s[0U] & 0xFF) >> 2U];
		result << B64CHARS[((s[0U] & 0x03) << 4U) + ((s[1U] & 0xF0) >> 4U)];
		result << (j == 2U) ? B64CHARS[((s[1U] & 0x0F) << 2U)] : '=';
		result << '=';
	}

	return result.str();
}
