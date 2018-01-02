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

/*The "main" function*/
int MAIN(int argc, CHAR *argv[])
{
	param_t param;
	FILE *source;
	uint8_t buffer[BUFF_SIZE], result[MHASH_384_LEN];
	uint_fast32_t count;
	uint64_t size_total, size_processed;
	uint_fast16_t update_iter;
	clock_t ts_start, ts_finish;
	mhash_384_t context;

#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
#endif

	/*install CTRL+C handler*/
	signal(SIGINT, sigint_handler);

	/*process command-line arguments*/
	if (!parse_arguments(&param, argc, argv))
	{
		return 1;
	}

	/*run self-test, if in test mode*/
	if (param.test_mode)
	{
		print_logo();
#ifdef NO_SELFTEST
		fprintf(stderr, "Not compiled with self-test code!\n");
		return 1;
#else
		return self_test();
#endif
	}

	/*open source file*/
	if (!(source = param.filename ? FOPEN(param.filename, T("rb")) : stdin))
	{
		print_logo();
		fprintf(stderr, "Failed to open input file:\n" FMT_S "\n\n%s\n\n", param.filename ? param.filename : T("<STDIN>"), strerror(errno));
		return 1;
	}

	/*determine file size*/
	size_total = get_file_size(source);

	/*setup state*/
	ts_start = clock();
	update_iter = 0;
	size_processed = 0;

	/*initialization*/
	mhash_384_initialize(&context);

	/*process file contents*/
	while(!(ferror(source) || feof(source)))
	{
		count = (uint_fast32_t) fread(buffer, sizeof(uint8_t), BUFF_SIZE, source);
		if (count > 0)
		{
			mhash_384_update(&context, buffer, count);
			size_processed += count;
		}
		if (g_interrupted || (count != BUFF_SIZE))
		{
			break; /*stop*/
		}
		if (param.show_progress && ((update_iter++ & 0x3FF) == 0))
		{
			print_progress(size_total, size_processed);
			fflush(stderr);
		}
	}

	/*check for interruption*/
	if (g_interrupted)
	{
		fprintf(stderr, "\nInterrupted!\n\n");
		return SIGINT;
	}

	/*final progress*/
	if (param.show_progress)
	{
		print_progress(size_total, size_processed);
		fprintf(stderr, " done\n\n");
		fflush(stderr);
	}

	/*check file error status*/
	if (ferror(source))
	{
		print_logo();
		fprintf(stderr, "File read error has occurred!\n");
		fclose(source);
		return 1;
	}

	/*finalize the hash*/
	mhash_384_finalize(&context, result);
	ts_finish = clock();

	/*output stats*/
	if (param.enable_bench)
	{
		const double time_total = ((double)(ts_finish - ts_start)) / ((double)CLOCKS_PER_SEC);
		const double throughput = (size_processed) / time_total;
		fprintf(stderr, "Processed %" PRIu64 " bytes in %.1f seconds, %.1f bytes/sec.\n\n", size_processed, time_total, throughput);
		fflush(stderr);
	}

	/*output result as Hex string*/
	print_digest(result, param.use_upper_case);
	fflush(stdout);

	/*clean up*/
	if (source != stdin)
	{
		fclose(source);
	}

	/*completed*/
	return 0;
}
