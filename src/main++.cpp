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

/*Includes*/
#include "mhash_384.h"
#include "utilities.h"

/*Test includes*/
#ifndef NO_SELFTEST
#include "self_test.h"
#endif

/*CRT includes*/
#include <errno.h>
#include <time.h>
#include <float.h>

/*Constants*/
#define BUFF_SIZE 4096

static int process_file(const int multi_file, const param_t *const param, uint64_t *const bytes_total, CHAR *const file_name)
{
	FILE *source;
	uint64_t file_size, bytes_processed;
	uint8_t buffer[BUFF_SIZE];
	uint_fast32_t count;
	uint_fast16_t update_iter;

	/*check if file is accessible*/
	if (file_name && (!is_file_readable(file_name)))
	{
		if (!param->ignore_errors)
		{
			print_logo();
			FPRINTF(stderr, T("Given input file is not readable:\n%s\n\n%s\n\n"), file_name ? file_name : T("<STDIN>"), STRERROR(errno));
		}
		return 0;
	}

	/*open source file*/
	if (!(source = file_name ? FOPEN(file_name, T("rb")) : stdin))
	{
		if (!param->ignore_errors)
		{
			print_logo();
			FPRINTF(stderr, T("Failed to open input file:\n%s\n\n%s\n\n"), file_name ? file_name : T("<STDIN>"), STRERROR(errno));
		}
		return 0;
	}

	/*determine file size*/
	file_size = get_file_size(source);

	/*initialization*/
	mhash_384::MHash384 context;
	update_iter = 0;
	bytes_processed = 0;

	/*process file contents*/
	while (!(ferror(source) || feof(source)))
	{
		count = (uint_fast32_t)fread(buffer, sizeof(uint8_t), BUFF_SIZE, source);
		if (count > 0)
		{
			context.update(buffer, count);
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

	/*check file error status*/
	if ((!g_interrupted) && ferror(source))
	{
		if (!param->ignore_errors)
		{
			print_logo();
			FPUTS(T("File read error has occurred!\n"), stderr);
		}
		fclose(source);
		return 0;
	}

	/*update gloabl counter*/
	*bytes_total += bytes_processed;

	/*final progress*/
	if (param->show_progress)
	{
		print_progress(file_size, bytes_processed);
		FPRINTF(stderr, T(" %s\n"), g_interrupted ? T("stop!") : T("done"));
		fflush(stderr);
	}

	/*finalize the hash*/
	const std::vector<uint8_t> result = context.finalize();

	/*output result as Hex string*/
	if (param->raw_output)
	{
		if (fwrite(result.data(), sizeof(uint8_t), result.size(), stdout) != result.size())
		{
			FPUTS(T("File write error has occurred!\n"), stderr);
			if (source != stdin)
			{
				fclose(source);
			}
			return 0;
		}
	}
	else
	{
		print_digest(stdout, result.data(), param->use_upper_case, param->curly_brackets);
		if (multi_file)
		{
			FPRINTF(stdout, param->curly_brackets ? T("  /* %s */") : T("  %s"), file_name);
		}
		FPUTC(T('\n'), stdout);
	}

	/*check for interruption*/
	if (g_interrupted)
	{
		FPUTS(T("\nInterrupted!\n\n"), stderr);
		fflush(stderr);
		FORCE_EXIT(SIGINT);
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

	/*set up std i/o streams*/
	SETMODE(stdin,  0);
	SETMODE(stderr, 0);

	/*disable buffering*/
	setvbuf(stderr, NULL, _IONBF, 0);

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
		FPUTS("Not compiled with self-test code!\n", stderr);
		return 1;
#else
		return self_test();
#endif
	}

	/*set up stdout mode*/
	SETMODE(stdout, param.raw_output);

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
			if (!process_file(multi_file, &param, &bytes_total, argv[file_id++]))
			{
				retval = EXIT_FAILURE;
				if (!param.ignore_errors)
				{
					break; /*fail!*/
				}
			}
		}
	}
	else
	{
		if(!process_file(0U, &param, &bytes_total, NULL))
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
		const double throughput = bytes_total / (time_total + DBL_EPSILON);
		FPRINTF(stderr, T("\nProcessed %") T(PRIu64) T(" bytes in %.1f seconds, %.1f bytes/sec.\n\n"), bytes_total, time_total, throughput);
		fflush(stderr);
	}

	/*exit*/
	return retval;
}
