/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Example application (plain C)                                                      */
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

/*Disable some warnings*/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

/*Includes*/
#include "mhash_384.h"
#include "compat.h"
#include "utilities.h"

/*Test includes*/
#ifndef NO_SELFTEST
#include "self_test.h"
#endif

/*CRT includes*/
#include <errno.h>
#include <time.h>

/*Constants*/
#define BUFF_SIZE 4096

/*Win32-only headers*/
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

static int process_file(const int multi_file, const param_t *const param, CHAR *const file_name)
{
	FILE *source;
	uint64_t file_size, bytes_processed;
	mhash_384_t context;
	uint8_t buffer[BUFF_SIZE], result[MHASH_384_LEN];
	uint_fast32_t count;
	uint_fast16_t update_iter;

	/*open source file*/
	if (!(source = file_name ? FOPEN(file_name, T("rb")) : stdin))
	{
		print_logo();
		fprintf(stderr, "Failed to open input file:\n" FMT_S "\n\n%s\n\n", file_name ? file_name : T("<STDIN>"), strerror(errno));
		return 0;
	}

	/*determine file size*/
	file_size = get_file_size(source);

	/*initialization*/
	mhash_384_initialize(&context);
	update_iter = 0;
	bytes_processed = 0;

	/*process file contents*/
	while (!(ferror(source) || feof(source)))
	{
		count = (uint_fast32_t)fread(buffer, sizeof(uint8_t), BUFF_SIZE, source);
		if (count > 0)
		{
			mhash_384_update(&context, buffer, count);
			bytes_processed += count;
		}
		if (g_interrupted || (count != BUFF_SIZE))
		{
			break; /*stop*/
		}
		if (param->show_progress && ((update_iter++ & 0x3FF) == 0))
		{
			print_progress(file_size, bytes_processed);
			fflush(stderr);
		}
	}

	/*check for interruption*/
	if (g_interrupted)
	{
		fprintf(stderr, "\nInterrupted!\n\n");
		return SIGINT;
	}

	/*check file error status*/
	if (ferror(source))
	{
		print_logo();
		fprintf(stderr, "File read error has occurred!\n");
		fclose(source);
		return 0;
	}

	/*final progress*/
	if (param->show_progress)
	{
		print_progress(file_size, bytes_processed);
		fprintf(stderr, " done\n");
		fflush(stderr);
	}

	/*finalize the hash*/
	mhash_384_finalize(&context, result);

	/*output result as Hex string*/
	if (param->raw_output)
	{
		if (fwrite(result, sizeof(uint8_t), MHASH_384_LEN, stdout) != MHASH_384_LEN)
		{
			fprintf(stderr, "File write error has occurred!\n");
			if (source != stdin)
			{
				fclose(source);
			}
			return 0;
		}
	}
	else
	{
		print_digest(stdout, result, param->use_upper_case, param->curly_brackets);
		if (multi_file)
		{
			fprintf(stdout, "  " FMT_S, file_name);
		}
		putc('\n', stdout);
	}

	/*flush*/
	fflush(stdout);

	/*clean up*/
	if (source != stdin)
	{
		fclose(source);
	}

	return 1;
}

/*The "main" function*/
int MAIN(int argc, CHAR *argv[])
{
	param_t param;
	int retval, file_id;
	clock_t ts_start, ts_finish;
	uint64_t bytes_total;

#ifdef _WIN32
	_setmode(_fileno(stdin),  _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	/*install CTRL+C handler*/
	signal(SIGINT, sigint_handler);

	/*process command-line arguments*/
	if ((file_id = parse_arguments(&param, argc, argv)) < 1)
	{
		return 1;
	}

	/*select mode of operation*/
	switch (param.opmode)
	{
	case OPMODE_HELP:
		print_help();
		return 0;
	case OPMODE_VERS:
		print_vers();
		return 0;
	case OPMODE_TEST:
#ifdef NO_SELFTEST
		fprintf(stderr, "Not compiled with self-test code!\n");
		return 1;
#else
		return self_test();
#endif
	}

	/*initialize*/
	ts_start = clock();
	bytes_total = 0;
	retval = EXIT_SUCCESS;

	/*process all input files*/
	if (file_id < argc)
	{
		const int multi_file = file_id < (argc - 1);
		while (file_id < argc)
		{
			if (!process_file(multi_file, &param, argv[file_id++]))
			{
				retval = EXIT_FAILURE;
				break;
			}
		}
	}
	else
	{
		if(!process_file(0U, &param, NULL))
		{
			retval = EXIT_FAILURE;
		}
	}

	/*finalize*/
	ts_finish = clock();

	/*output stats*/
	if (param.enable_bench)
	{
		const double time_total = (ts_finish - ts_start) / (double)CLOCKS_PER_SEC;
		const double throughput = bytes_total / time_total;
		fprintf(stderr, "\nProcessed %" PRIu64 " bytes in %.1f seconds, %.1f bytes/sec.\n\n", bytes_total, time_total, throughput);
		fflush(stderr);
	}

	/*exit*/
	return retval;
}
