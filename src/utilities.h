/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Example application (utility functions)                                            */
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

#ifndef MHASH_CLI_UTILS_INCLUDED
#define MHASH_CLI_UTILS_INCLUDED

/*Enable large files*/
#define __USE_LARGEFILE64 1

/*Includes*/
#include "compat.h"
#include "sysinfo.h"

/*CRT includes*/
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>

/*POSIX API*/
#ifdef __unix__
#include <unistd.h>
#endif

/*Win32-only header*/
#ifdef _WIN32
#include <io.h>
#endif

/*Hash size*/
#ifdef __cplusplus
#define MY_HASH_LENGTH mhash_384::MHash384::HASH_LEN
#else
#define MY_HASH_LENGTH MHASH_384_LEN
#endif

/*Build date*/
static const CHAR *const BUILD_DATE = T(__DATE__);

/*Mode*/
#define OPMODE_HELP 1
#define OPMODE_VERS 2
#define OPMODE_TEST 3

/*Parameters*/
typedef struct param_t
{
	int opmode;
	int enable_bench;
	int show_progress;
	int use_upper_case;
	int curly_brackets;
	int raw_output;
	int ignore_errors;
}
param_t;

/*Version*/
typedef struct version_t
{
	uint_fast16_t major;
	uint_fast16_t minor;
	uint_fast16_t patch;
}
version_t;

/*Get version*/
static version_t get_version(void)
{
	version_t version_info;
#ifdef __cplusplus
	mhash_384::MHash384::version(version_info.major, version_info.minor, version_info.patch);
#else
	version_info.major = MHASH_384_VERSION_MAJOR;
	version_info.minor = MHASH_384_VERSION_MINOR;
	version_info.patch = MHASH_384_VERSION_PATCH;
#endif
	return version_info;
}

/*The logo*/
static void print_logo(void)
{
	const version_t version = get_version();
	FPRINTF(stderr, T("\nMHash384 v%u.%u.%u, simple fast portable header-only hashing library [%s]\n"), (unsigned int)version.major, (unsigned int)version.minor, (unsigned int)version.patch, BUILD_DATE);
	FPRINTF(stderr, T("Copyright(c) 2016-%s LoRd_MuldeR <mulder2@gmx.de>, released under the MIT License.\n\n"), &BUILD_DATE[7]);
}

/*Lib version*/
static void print_vers(void)
{
	const version_t version = get_version();
	FPRINTF(stdout, T("mhash-384 version %u.%u.%u (built %s)\n"), (unsigned int)version.major, (unsigned int)version.minor, (unsigned int)version.patch, BUILD_DATE);
}

/*File name suffix*/
#ifdef _WIN32
#define EXE_SUFFIX ".exe"
#else
#define EXE_SUFFIX ""
#endif

/*Print help screen*/
static void print_help(void)
{
	print_logo();
	FPRINTF(stderr, T("Built with %s v%u.%u.%u on %s [%s]\n\n"), T(COMPILER_TYPE), COMPILER_VERS_MAJOR, COMPILER_VERS_MINOR, COMPILER_VERS_PATCH, T(SYSTEM_TYPE), T(COMPILER_ARCH));
	FPUTS(T("Usage:\n"), stderr);
	FPRINTF(stderr, T("  mhash384%s [options] [input_file]...\n\n"), T(EXE_SUFFIX));
	FPUTS(T("Options:\n"), stderr);
	FPUTS(T("  -p, --progress  show progress while processing\n"), stderr);
	FPUTS(T("  -u, --upper     print digest in upper case letters\n"), stderr);
	FPUTS(T("  -c, --curly     print digest using C-style curly brackets\n"), stderr);
	FPUTS(T("  -r, --raw       output \"raw\" bytes (no \"hex\" encoding)\n"), stderr);
	FPUTS(T("  -b, --bench     compute and print throughput\n"), stderr);
	FPUTS(T("  -i, --ignore    ignore errors and proceed with ntext file\n"), stderr);
	FPUTS(T("  -v, --version   print the version string and exit\n"), stderr);
	FPUTS(T("  -t, --test      execute self-test and exit\n"), stderr);
	FPUTS(T("  -h, --help      print this help screen and exit\n\n"), stderr);
	FPUTS(T("If no input file is specified, input will be read from STDIN.\n\n"), stderr);
}

/*Check specific option*/
#define IS_OPTION(ARGV, IS_LONG, NAME_SHRT, NAME_LONG) \
	((IS_LONG) ? (!STRICMP((ARGV), (NAME_LONG))) : (TOLOWER(*(ARGV)) == (NAME_SHRT)))

/*Parse option*/
static int parse_option(param_t *param, const CHAR *const argv, const int is_long)
{
	if (IS_OPTION(argv, is_long, T('b'), T("bench")))
	{
		param->enable_bench = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('i'), T("ignore")))
	{
		param->ignore_errors = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('p'), T("progress")))
	{
		param->show_progress = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('u'), T("upper")))
	{
		param->use_upper_case = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('c'), T("curly")))
	{
		param->curly_brackets = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('r'), T("raw")))
	{
		param->raw_output = 1;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('h'), T("help")))
	{
		param->opmode = OPMODE_HELP;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('v'), T("version")))
	{
		param->opmode = OPMODE_VERS;
		return 1;
	}
	if (IS_OPTION(argv, is_long, T('t'), T("test")))
	{
		param->opmode = OPMODE_TEST;
		return 1;
	}
	return 0;
}

/*Parse arguments*/
static int parse_arguments(param_t *const param, int argc, CHAR *argv[])
{
	int i, j;
	memset(param, 0, sizeof(param_t));
	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] == T('-'))
		{
			if (argv[i][1] == T('-'))
			{
				if (!argv[i][2])
				{
					++i;
					break; /*stop*/
				}
				if (!parse_option(param, &argv[i][2], 1))
				{
					print_logo();
					FPRINTF(stderr, T("Unknown option:\n%s\n\n"), argv[i]);
					return 0;
				}
				continue;
			}
			else if (argv[i][1])
			{
				for (j = 1; argv[i][j]; ++j)
				{
					if(!parse_option(param, &argv[i][j], 0))
					{
						print_logo();
						FPRINTF(stderr, T("Unknown option(s):\n%s\n\n"), argv[i]);
						return 0;
					}
				}
				continue;
			}
		}
		break; /*no more options*/
	}
	if (param->raw_output && (param->use_upper_case || param->curly_brackets))
	{
		print_logo();
		FPRINTF(stderr, T("Error: Options \"-%c\" and \"-r\" are mutually exclusive!\n\n"), param->use_upper_case ? T('u') : T('c'));
		return 0;
	}
	return i;
}

/*file size*/
static int get_file_info(FILE *const file, uint64_t *const file_size, MODE_T *const file_type)
{
	STAT64_T info;
	if (!FSTAT64(FILENO(file), &info))
	{
		*file_type = (info.st_mode & S_IFMT);
		*file_size = (*file_type == S_IFREG) ? (uint64_t)info.st_size : UINT64_MAX;
		return 1;
	}
	return 0;
}

/*progress*/
static void print_progress(const uint64_t size_total, const uint64_t size_processed)
{
	if (size_total != UINT64_MAX)
	{
		const double progress = (((double)size_processed) / (((double)size_total) + DBL_EPSILON)) * 100.0;
		FPRINTF(stderr, T("\r%.1f%% of %") T(PRIu64) T(" bytes processed..."), progress, size_total);
	}
	else
	{
		FPRINTF(stderr, T("\r%") T(PRIu64) T(" bytes processed..."), size_processed);
	}
}

/*print digest*/
#define _PUT_HEX_CHAR(W,X,Y,Z) FPUTC(W[((X) >> (Y)) & 0xFU], (Z))
static void print_digest(FILE *const stream, const uint8_t *const digest, const int uppercase, const int curly)
{
	static const CHAR *const HEX_UPR = T("0123456789ABCDEF");
	static const CHAR *const HEX_LWR = T("0123456789abcdef");
	const CHAR *const hex = uppercase ? HEX_UPR : HEX_LWR;
	uint16_t count;
	if (curly)
	{
		FPUTS(T("{ "), stream);
	}
	for (count = 0U; count < MY_HASH_LENGTH; ++count)
	{
		if (curly)
		{
			FPUTS(count ? T(", 0x") : T("0x"), stream);
		}
		_PUT_HEX_CHAR(hex, digest[count], 4, stream);
		_PUT_HEX_CHAR(hex, digest[count], 0, stream);
	}
	if (curly)
	{
		FPUTS(T(" }"), stream);
	}
}

/*sigint handler*/
static volatile int g_interrupted;
static void sigint_handler(int sig_no)
{
	g_interrupted = 1;
	signal(sig_no, sigint_handler);
	fclose(stdin);
}

/*clear all errors*/
static void clear_errors()
{
	errno = 0;
	SET_SYSERRNO(0);
}

/*error handler*/
static void print_error(const CHAR *const message, const CHAR *const file_name, const param_t *const param, const int multi_file)
{
	const CHAR *const source_name = file_name ? file_name : T("<STDIN>");
	if (param->ignore_errors && multi_file)
	{
		FPRINTF(stderr, T("Skipped file: %s\n"), source_name); /*skip error message*/
	}
	else
	{
		const errno_t error = errno;
		const unsigned long syserrno = SYSERRNO;
		print_logo();
		if (error && syserrno)
		{
			FPRINTF(stderr, T("%s:\n%s\n\n>>> %s [Code: 0x%X] <<<\n\n"), message, source_name, STRERROR(error), syserrno);
		}
		else
		{
			FPRINTF(stderr, T("%s:\n%s\n\n>>> %s <<<\n\n"), message, source_name, STRERROR(error));
		}
	}
}

#endif /*MHASH_CLI_UTILS_INCLUDED*/
