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

#ifndef INC_MHASH384_COMMON_H
#define INC_MHASH384_COMMON_H

#include <cstring>
#include <cstdio>

#if defined(_MSC_VER) && (_MSC_VER <= 1600)
#define PRIu64 "llu"
#else
#include <cinttypes>
#endif

#ifdef _WIN32
#define CHAR_T wchar_t
#define EXTRY_POINT wmain
#define _STR_(X) L##X
#define FPRINTF fwprintf
#define FPUTS fputws
#define STRPBRK wcspbrk
#define STRICMP _wcsicmp
#define STRNICMP _wcsnicmp
#define FOPEN _wfopen
#define FORCE_EXIT _exit
#ifdef __USE_MINGW_ANSI_STDIO
#define PRI_char L"hs"
#define PRI_CHAR L"ls"
#else
#define PRI_char L"S"
#define PRI_CHAR L"s"
#endif
#else
#define CHAR_T char
#define EXTRY_POINT main
#define _STR_(X) X
#define FPRINTF fprintf
#define FPUTS fputs
#define STRPBRK strpbrk
#define STRICMP strcasecmp
#define STRNICMP strncasecmp
#define FOPEN fopen
#define FORCE_EXIT _Exit
#define PRI_char "s"
#define PRI_CHAR "s"
#endif

#define STR(X) _STR_(X)

#endif /*INC_MHASH384_COMMON_H*/
