/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Example application (utility functions)                                            */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 */
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

/*Hash size*/
#ifdef __cplusplus
static const uint16_t HASH_LENGTH = mhash_384::MHash384::HASH_LEN;
#else
static const uint16_t HASH_LENGTH = MHASH_384_LEN;
#endif

/*Parameters*/
typedef struct param_t
{
	const CHAR *filename;
	int enable_bench;
	int show_progress;
	int test_mode;
	int use_upper_case;
}
param_t;

/*Version*/
typedef struct version_t
{
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
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
	fprintf(stderr, "\nMHash384 v%u.%u.%u, simple fast portable header-only hashing library [%s]\n", version.major, version.minor, version.patch, __DATE__);
	fprintf(stderr, "Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>, released under the MIT License.\n\n");
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
	fprintf(stderr, "Built with " COMPILER_TYPE " v%u.%u.%u on " SYSTEM_TYPE " [" COMPILER_ARCH "]\n\n", COMPILER_VERS_MAJOR, COMPILER_VERS_MINOR, COMPILER_VERS_PATCH);
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  mhash384" EXE_SUFFIX " [options] [input_file]\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -p, --progress  show progress while processing\n");
	fprintf(stderr, "  -u, --upper     print digest in upper case letters\n");
	fprintf(stderr, "  -b, --bench     compute and print throughput\n");
	fprintf(stderr, "  -v, --version   print the version string and exit\n");
	fprintf(stderr, "  -t, --test      execute self-test and exit\n");
	fprintf(stderr, "  -h, --help      print this help screen and exit\n\n");
	fprintf(stderr, "If no input file is specified, input will be read from STDIN.\n\n");
}

/*Parse arguments*/
static int parse_arguments(param_t *param, int argc, CHAR *argv[])
{
	int i, stop = 0;
	memset(param, 0, sizeof(param_t));
	for (i = 1; i < argc; ++i)
	{
		if (!stop)
		{
			if (!STRICMP(argv[i], T("--")))
			{
				stop = 1;
				continue;
			}
			else if (!STRICMP(argv[i], T("-b")) || !STRICMP(argv[i], T("--bench")))
			{
				param->enable_bench = 1;
				continue;
			}
			else if (!STRICMP(argv[i], T("-t")) || !STRICMP(argv[i], T("--test")))
			{
				param->test_mode = 1;
				continue;
			}
			else if (!STRICMP(argv[i], T("-p")) || !STRICMP(argv[i], T("--progress")))
			{
				param->show_progress = 1;
				continue;
			}
			else if (!STRICMP(argv[i], T("-u")) || !STRICMP(argv[i], T("--upper")))
			{
				param->use_upper_case = 1;
				continue;
			}
			else if (!STRICMP(argv[i], T("-h")) || !STRICMP(argv[i], T("--help")) || !STRICMP(argv[i], T("/?")))
			{
				print_logo();
				print_help();
				return 0;
			}
			else if (!STRICMP(argv[i], T("-v")) || !STRICMP(argv[i], T("--version")))
			{
				const version_t version = get_version();
				printf("mhash-384 version %u.%u.%u (built %s)\n", version.major, version.minor, version.patch, __DATE__);
				return 0;
			}
			else if ((argv[i][0] == T('-')) && (argv[i][1] != T('\0')))
			{
				print_logo();
				fprintf(stderr, "Unknown option:\n" FMT_S "\n\n", argv[i]);
				return 0;
			}
		}
		if (!param->filename)
		{
			param->filename = argv[i];
		}
		else
		{
			print_logo();
			fprintf(stderr, "Excess argument:\n" FMT_S "\n\n", argv[i]);
			return 0;
		}
	}
	if (param->filename && (!STRICMP(param->filename, T("-"))) && (!stop))
	{
		param->filename = NULL;
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
#define PUT_HEX_CHAR(X,Y,Z) putchar(X[((Y) >> (Z)) & 0xFU])
static void print_digest(const uint8_t *const digest, const int uppercase)
{
	static const char *const HEX_UPR = "0123456789ABCDEF";
	static const char *const HEX_LWR = "0123456789abcdef";
	const char *const hex = uppercase ? HEX_UPR : HEX_LWR;
	uint16_t count;
	for (count = 0; count < HASH_LENGTH; ++count)
	{
		PUT_HEX_CHAR(hex, digest[count], 4);
		PUT_HEX_CHAR(hex, digest[count], 0);
	}
	printf("\n");
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
