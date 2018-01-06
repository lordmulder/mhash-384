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
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>

/*Hash size*/
#ifdef __cplusplus
#define MY_HASH_LENGTH mhash_384::MHash384::HASH_LEN
#else
#define MY_HASH_LENGTH MHASH_384_LEN
#endif

/*Mode*/
#define OPMODE_HELP 1
#define OPMODE_VERS 2
#define OPMODE_TEST 3

/*Parameters*/
typedef struct param_t
{
	const CHAR *filename;
	int opmode;
	int enable_bench;
	int show_progress;
	int use_upper_case;
	int curly_brackets;
	int raw_output;
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
	fprintf(stderr, "\nMHash384 v%u.%u.%u, simple fast portable header-only hashing library [%s]\n", (unsigned int)version.major, (unsigned int)version.minor, (unsigned int)version.patch, __DATE__);
	fprintf(stderr, "Copyright(c) 2016-2018 LoRd_MuldeR <mulder2@gmx.de>, released under the MIT License.\n\n");
}

/*Lib version*/
static void print_vers(void)
{
	const version_t version = get_version();
	printf("mhash-384 version %u.%u.%u (built %s)\n", (unsigned int)version.major, (unsigned int)version.minor, (unsigned int)version.patch, __DATE__);
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
	fprintf(stderr, "Built with " COMPILER_TYPE " v%u.%u.%u on " SYSTEM_TYPE " [" COMPILER_ARCH "]\n\n", COMPILER_VERS_MAJOR, COMPILER_VERS_MINOR, COMPILER_VERS_PATCH);
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  mhash384" EXE_SUFFIX " [options] [input_file]\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -p, --progress  show progress while processing\n");
	fprintf(stderr, "  -u, --upper     print digest in upper case letters\n");
	fprintf(stderr, "  -c, --curly     print digest using C-style curly brackets\n");
	fprintf(stderr, "  -r, --raw       output \"raw\" bytes (no \"hex\" encoding)\n");
	fprintf(stderr, "  -b, --bench     compute and print throughput\n");
	fprintf(stderr, "  -v, --version   print the version string and exit\n");
	fprintf(stderr, "  -t, --test      execute self-test and exit\n");
	fprintf(stderr, "  -h, --help      print this help screen and exit\n\n");
	fprintf(stderr, "If no input file is specified, input will be read from STDIN.\n\n");
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
	int i, j, stop = 0;
	memset(param, 0, sizeof(param_t));
	for (i = 1; i < argc; ++i)
	{
		if (!stop)
		{
			if (argv[i][0] == T('-'))
			{
				if (argv[i][1] == T('-'))
				{
					if (!argv[i][2])
					{
						stop = 1;
						continue;
					}
					if (!parse_option(param, &argv[i][2], 1))
					{
						print_logo();
						fprintf(stderr, "Unknown option:\n" FMT_S "\n\n", argv[i]);
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
							fprintf(stderr, "Unknown option(s):\n" FMT_S "\n\n", argv[i]);
							return 0;
						}
					}
					continue;
				}
			}
		}
		if (!param->filename)
		{
			param->filename = argv[i];
			continue;
		}
		print_logo();
		fprintf(stderr, "Excess argument:\n" FMT_S "\n\n", argv[i]);
		return 0;
	}
	if (param->filename && (!stop) && (!STRICMP(param->filename, T("-"))))
	{
		param->filename = NULL;
	}
	if (param->raw_output && (param->use_upper_case || param->curly_brackets))
	{
		print_logo();
		fprintf(stderr, "Error: Options \"-%c\" and \"-r\" are mutually exclusive!\n\n", param->use_upper_case ? 'u' : 'c');
		return 0;
	}
	return 1;
}

/*file size*/
static uint64_t get_file_size(FILE *const file)
{
	struct stat64 info;
	if (fstat64(fileno(file), &info) || ((info.st_mode & S_IFMT) != S_IFREG))
	{
		return 0;
	}
	return (uint64_t) info.st_size;
}

/*progress*/
static void print_progress(const uint64_t size_total, const uint64_t size_processed)
{
	if (size_total)
	{
		fprintf(stderr, "\r%.1f%% of %" PRIu64 " bytes processed...", ((double)size_processed) / ((double)size_total) * 100.0, size_total);
	}
	else
	{
		fprintf(stderr, "\r%" PRIu64 " bytes processed...", size_processed);
	}
}

/*print digest*/
#define _PUT_HEX_CHAR(X,Y,Z) fputc(X[((Y) >> (Z)) & 0xFU], stream)
static void print_digest(FILE *const stream, const uint8_t *const digest, const int uppercase, const int curly)
{
	static const char *const HEX_UPR = "0123456789ABCDEF";
	static const char *const HEX_LWR = "0123456789abcdef";
	const char *const hex = uppercase ? HEX_UPR : HEX_LWR;
	uint16_t count;
	if (curly)
	{
		fputs("{ ", stream);
	}
	for (count = 0U; count < MY_HASH_LENGTH; ++count)
	{
		if (curly)
		{
			fputs(count ? ", 0x" : "0x", stream);
		}
		_PUT_HEX_CHAR(hex, digest[count], 4);
		_PUT_HEX_CHAR(hex, digest[count], 0);
	}
	if (curly)
	{
		fputs(" }", stream);
	}
	fputc('\n', stream);
}

/*sigint handler*/
static volatile int g_interrupted = 0;
static void sigint_handler(int sig_no)
{
	g_interrupted = 1;
	signal(sig_no, sigint_handler);
	fclose(stdin);
}

#endif /*MHASH_CLI_UTILS_INCLUDED*/
