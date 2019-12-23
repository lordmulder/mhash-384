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
#include "common.h"
#include "self_test.h"
#include "utils.h"
#include "sys_info.h"
#include <ctime>
#include <sys/stat.h>

/* Win32 I/O stuff */
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define fstat  _fstat
#define stat   _stat
#define fileno _fileno
#endif

/* fstat(2) macros */
#if !defined(S_IFMT) && !defined(S_IFDIR) && defined(_S_IFMT) && defined(_S_IFDIR)
#define S_IFMT  _S_IFMT
#define S_IFDIR _S_IFDIR
#endif

/* System type */
#define SYSTEM_TYPE STR(SYSTEM_NAME) STR("-") STR(SYSTEM_ARCH)

/* Buffer size */
static const size_t BUFFER_SIZE = 8192U;

/* Mode of operation */
typedef enum
{
	MODE_DEFAULT  =  0,
	MODE_MANPAGE  =  1,
	MODE_VERSION  =  2,
	MODE_SELFTEST =  3,
	MODE_STRESS   =  4,
	MODE_UNKNOWN  = -1
}
opmode_t;

/*
 * Print the logo
 */
static void print_logo(void)
{
	uint16_t ver_major, ver_minor, ver_patch;
	mhash384_version(&ver_major, &ver_minor, &ver_patch);
	FPRINTF(stderr, ver_patch ? STR("MHash-384 v%u.%02u-%u") : STR("MHash-384 v%u.%02u"), ver_major, ver_minor, ver_patch);
	FPUTS(STR(", ") SYSTEM_TYPE STR(" [") STR(__DATE__) STR("]\n"), stderr);
	FPUTS(STR("Created by LoRd_MuldeR <mulder2@gmx.de>, released under the MIT license.\n\n"), stderr);
	fflush(stderr);
}

/*
 * Print the version
 */
static void print_version(void)
{
	uint16_t ver_major, ver_minor, ver_patch;
	mhash384_version(&ver_major, &ver_minor, &ver_patch);
	FPRINTF(stdout, STR("MHash-384 v%u.%02u-%u, ") SYSTEM_TYPE STR("\n"), ver_major, ver_minor, ver_patch);
	FPRINTF(stdout, STR("Built on ") STR(__DATE__) STR(" at ") STR(__TIME__) STR(", using ") STR(COMPILER_FMT) STR("\n"), COMPILER_ARG);
}

/*
 * Print the manpage
 */
static void print_manpage(const CHAR_T *const argv0)
{
	const CHAR_T *const program_name = get_basename(argv0);
	print_logo();
	FPUTS(STR("Usage:\n"), stderr);
	FPRINTF(stderr, STR("   %") PRI_CHAR STR(" [OPTIONS] [<FILE_1> <FILE_2> ... <FILE_N>]\n\n"), program_name);
	FPUTS(STR("Options:\n"), stderr);
	FPUTS(STR("   --keep-going  Try to proceed with the remaining files, if a file has failed\n"), stderr);
	FPUTS(STR("   --short       Print the digest in short format (no file names)\n"), stderr);
	FPUTS(STR("   --lower-case  Print the digest in lower-case letters (default: upper-case)\n"), stderr);
	FPUTS(STR("   --base64      Print the digest in Base64 format (default: Hex format)\n"), stderr);
	FPUTS(STR("   --help        Print help screen and exit\n"), stderr);
	FPUTS(STR("   --version     Print program version and exit\n"), stderr);
	FPUTS(STR("   --self-test   Run self-test and exit\n"), stderr);
	FPUTS(STR("   --stress      Enable stress test mode; strings are read from the input file\n"), stderr);
	FPUTS(STR("   --benchmark   Measure the overall time required for the operation\n\n"), stderr);
	FPUTS(STR("If *no* input file is specified, data is read from the standard input (stdin)\n"), stderr);
}

/*
 * Parse command-line options
 */
static opmode_t parse_options(int &arg_offset, const int argc, const CHAR_T *const *const argv, bool &keep_going, bool &short_format, bool &base64, bool &lower_case, bool &benchmark)
{
	opmode_t mode = MODE_DEFAULT;
	keep_going = short_format = lower_case = benchmark = false;
	bool stop_here = false;

	while((!stop_here) && (arg_offset < argc) && (!STRNICMP(argv[arg_offset], STR("--"), 2U)))
	{
		const CHAR_T *const argstr = argv[arg_offset] + 2U;
		if(!(*argstr))
		{
			stop_here = true;
		}
		else if(!STRICMP(argstr, STR("keep-going")))
		{
			keep_going = true;
		}
		else if(!STRICMP(argstr, STR("short")))
		{
			short_format = true;
		}
		else if (!STRICMP(argstr, STR("base64")))
		{
			base64 = true;
		}
		else if(!STRICMP(argstr, STR("lower-case")))
		{
			lower_case = true;
		}
		else if(!STRICMP(argstr, STR("help")))
		{
			mode = MODE_MANPAGE;
		}
		else if(!STRICMP(argstr, STR("version")))
		{
			mode = MODE_VERSION;
		}
		else if(!STRICMP(argstr, STR("self-test")))
		{
			mode = MODE_SELFTEST;
		}
		else if(!STRICMP(argstr, STR("stress")))
		{
			mode = MODE_STRESS;
		}
		else if(!STRICMP(argstr, STR("benchmark")))
		{
			benchmark = true;
		}
		else
		{
			print_logo();
			FPRINTF(stderr, STR("Error: Specified option \"%") PRI_CHAR STR("\" is unknown. Please type \"--help\" for details!\n"), argv[arg_offset]);
			fflush(stderr);
			return MODE_UNKNOWN;
		}
		++arg_offset;
	}

	if (base64 && lower_case)
	{
		print_logo();
		FPUTS(STR("Error: Options \"--base64\" and \"--lower-case\" are mutually exclusive!\n"), stderr);
		fflush(stderr);
		return MODE_UNKNOWN;
	}

	return mode;
}

/*
 * Process input file
 */
static bool process_file(const CHAR_T *const file_name, const bool short_format, const bool base64, const bool lower_case)
{
	/* File description */
	const CHAR_T *const file_description = file_name ? file_name : STR("<STDIN>");

	/* Open the input file */
	errno = 0;
	FILE *const input = file_name ? FOPEN(file_name, STR("rb")) : stdin;
	if(!input)
	{
		FPRINTF(stderr, STR("Error: File \"%") PRI_CHAR STR("\" could not be opened for reading! [errno: %d]\n"), file_description, errno);
		fflush(stderr);
		return false;
	}

	/* Check if file is directory (this is required for Linux!)*/
	struct stat file_info;
	if(!fstat(fileno(input), &file_info))
	{
		if((file_info.st_mode & S_IFMT) == S_IFDIR)
		{
			FPRINTF(stderr, STR("Error: File \"%") PRI_CHAR STR("\" is a directory!\n"), file_description);
			fclose(input);
			fflush(stderr);
			return false;
		}
	}

	/* Initialize hash state */
	MHash384 mhash384;
	uint8_t buffer[BUFFER_SIZE];
	bool success = true;

	/* Process complete input */
	for(;;)
	{
		const size_t length = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, input);
		if(!length)
		{
			break; /*EOF or error*/
		}
		mhash384.update(buffer, length);
	}

	/* Print the final digest */
	if(!ferror(input))
	{
		const uint8_t *const digest = mhash384.finish();
		const std::string string = base64 ? bytes_to_base64(digest, MHASH384_SIZE) : bytes_to_hex(digest, MHASH384_SIZE, lower_case);
		const CHAR_T *const source_name = file_name ? file_name : STR("-");
		const CHAR_T *const format = short_format ? STR("%") PRI_char STR("\n") : STR("%") PRI_char STR("  %") PRI_CHAR STR("\n");
		FPRINTF(stdout, format, string.c_str(), source_name);
		fflush(stdout);
	}
	else
	{
		FPRINTF(stderr, STR("Error: File \"%") PRI_CHAR STR("\" encountered an I/O error!\n"), file_description);
		fflush(stderr);
		success = false;
	}

	/* Close the input file */
	if(file_name)
	{
		fclose(input);
	}

	return success;
}

/*
 * Main function
 */
static int mhash384_main(int argc, CHAR_T* argv[])
{
#ifdef _WIN32
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stderr), _O_U8TEXT);
	_setmode(_fileno(stdin ), _O_BINARY);
#endif /*_WIN32*/

	/* Parse all command-line options */
	bool keep_going = false, short_format = false, base64 = false, lower_case = false, benchmark = false, success = false;
	int arg_offset = 1;
	const opmode_t mode = parse_options(arg_offset, argc, argv, keep_going, short_format, base64, lower_case, benchmark);
	if(mode == MODE_UNKNOWN)
	{
		return EXIT_FAILURE;
	}

	/* Remember startup time */
	const clock_t time_start = benchmark ? clock() : 0U;

	/* Select mode of operation */
	switch(mode)
	{
	case MODE_MANPAGE:
		/* Print help screen and exit* */
		print_manpage(argv[0U]);
		success = true;
		break;

	case MODE_VERSION:
		/* Print program version and exit */
		print_version();
		success = true;
		break;

	case MODE_SELFTEST:
		/* Run the self-test */
		print_logo();
		success = self_test(keep_going, base64, lower_case);
		break;

	case MODE_STRESS:
		/* Enable stress-test mode*/
		print_logo();
		success = stress_test((arg_offset < argc) ? argv[arg_offset] : NULL, keep_going, base64, lower_case);
		break;

	default:
		/* Process all input files */
		if(arg_offset < argc)
		{
			while(arg_offset < argc)
			{
				if(process_file(argv[arg_offset++], short_format, base64, lower_case))
				{
					success = true;
				}
				else if(!keep_going)
				{
					success = false;
					break;
				}
			}
		}
		else
		{
			success = process_file(NULL, short_format, base64, lower_case); /*stdin*/
		}
	}

	/* Print total time */
	if(benchmark && ((mode == MODE_DEFAULT) || ((mode >= MODE_SELFTEST) && (mode <= MODE_STRESS))))
	{
		const clock_t total_time = clock() - time_start;
		FPRINTF(stderr, STR("Operation took %.1f second(s).\n"), total_time / ((double)CLOCKS_PER_SEC));
	}

	/* Completed */
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * Global exception handler
 */
static int _main_(int argc, CHAR_T* argv[])
{
#ifdef NDEBUG
	try
	{
		return mhash384_main(argc, argv);
	}
	catch (std::exception& ex)
	{
		FPRINTF(stderr, STR("\nEXCEPTION: %") PRI_char STR("\n"), ex.what());
		FORCE_EXIT(-1);
	}
	catch (const char *const ex)
	{
		FPRINTF(stderr, STR("\nEXCEPTION: %") PRI_char STR("\n"), ex);
		FORCE_EXIT(-1);
	}
	catch (...)
	{
		FPUTS(STR("\nEXCEPTION: An unknown exception has been caught!\n"), stderr);
		FORCE_EXIT(-1);
	}
#else
	return mhash384_main(argc, argv);
#endif
}

/*
 * Entry point function
 */
int EXTRY_POINT(int argc, CHAR_T* argv[])
{
#if defined(_WIN32) && !defined(__GNUC__) && defined(NDEBUG)
	__try
	{
		return _main_(argc, argv);
	}
	__except(1)
	{
		FPUTS(STR("\nEXCEPTION: Unhandeled structured exception caught!\n"), stderr);
		FORCE_EXIT(-1);
	}
#else
	return _main_(argc, argv);
#endif
}
