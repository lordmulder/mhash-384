/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Compatibility layer                                                                */
/* Copyright(c) 2016-2018 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

#ifndef MHASH_CLI_COMPAT_INCLUDED
#define MHASH_CLI_COMPAT_INCLUDED

#ifdef __linux
#define _GNU_SOURCE
#endif

#include <stdlib.h>

/*Unicode support*/
#ifdef _WIN32
#define CHAR wchar_t
#define MAIN(X,Y) wmain(X,Y)
#define FOPEN(X,Y) _wfopen((X),(Y))
#define FPRINTF(X,Y,...) fwprintf((X), (Y), __VA_ARGS__)
#define FPUTS(X,Y) fputws((X),(Y))
#define FPUTC(X,Y) fputwc((X),(Y))
#define STRLEN(X) wcslen((X))
#define STRICMP(X,Y) _wcsicmp(X,Y)
#define TOLOWER(X) towlower(X)
#define STRERROR(X) _wcserror((X))
#define _T(X) L##X
#define T(X) _T(X)
#else
#define CHAR char
#define MAIN(X,Y) main(X,Y)
#define FOPEN(X,Y) fopen((X),(Y))
#define FPRINTF(X,Y,...) fprintf((X), (Y), __VA_ARGS__)
#define FPUTS(X,Y) fputs((X),(Y))
#define FPUTC(X,Y) fputc((X),(Y))
#define STRLEN(X) strlen((X))
#define STRICMP(X,Y) strcasecmp(X,Y)
#define TOLOWER(X) tolower(X)
#define STRERROR(X) strerror((X))
#define T(X) X
#endif

/*MSVC compat*/
#if !defined(_MSC_VER) || (_MSC_VER > 1700)
#include <inttypes.h>
#else
#define PRIu64 "llu"
#endif

/*Win32 compat*/
#if defined(_WIN32) || defined(_WIN64)
#define fileno _fileno
#define stat64 _stat64
#define fstat64 _fstat64
#define SETMODE(X,Y) _setmode(_fileno((X)), (Y) ? _O_BINARY : _O_U8TEXT)
#define FCLOSEALL() _fcloseall()
#define FORCE_EXIT(X) _exit((X))
#else
#define SETMODE(X,Y) ((void)0)
#define FCLOSEALL() fcloseall()
#define FORCE_EXIT(X) _Exit((X))
#endif

/*C++ compat*/
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/*Mingw32 entry point, workaround for lack of wmain() support*/
#ifdef __MINGW32__
typedef struct { int newmode; } _startupinfo;
EXTERN_C int __wgetmainargs(int *_Argc, wchar_t ***_Argv, wchar_t ***_Env, int _DoWildCard, _startupinfo * _StartInfo);
int wmain(int argc, CHAR *argv[]);
int main(void)
{
	_startupinfo startup;
	int argc;
	wchar_t **argv_wide, **env;
	startup.newmode = 0;
	if (__wgetmainargs(&argc, &argv_wide, &env, 1, &startup))
	{
		exit(-1);
	}
	return wmain(argc, argv_wide);
}
#endif /*__MINGW32__*/
#endif /*MHASH_CLI_COMPAT_INCLUDED*/
