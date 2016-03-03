#define _CRT_RAND_S 1

#include "twister.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	twister_t rnd_context;
	uint32_t seed = 0;
	uint64_t count = 0;

#ifdef _WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif

	if ((argc <= 1) || (_stricmp(argv[1], "-") == 0) || (sscanf_s(argv[1], "%" PRIu64, &count) != 1))
	{
		count = UINT64_MAX;
	}

	if ((argc <= 2) || (_stricmp(argv[2], "-") == 0) || (sscanf_s(argv[2], "%" PRIu32, &seed) != 1))
	{
		if (rand_s(&seed))
		{
			abort();
		}
	}

	rand_init(&rnd_context, seed);

	while (count >= (BUFF_SIZE * sizeof(uint32_t)))
	{
		uint32_t r[BUFF_SIZE];
		for (size_t i = 0; i < BUFF_SIZE; i++)
		{
			r[i] = rand_next(&rnd_context);
		}
		if (fwrite(r, sizeof(uint32_t), BUFF_SIZE, stdout) != BUFF_SIZE)
		{
			fprintf(stderr, "I/O error!\n");
			return 1;
		}
		count -= (BUFF_SIZE * sizeof(uint32_t));
	}

	while (count >= sizeof(uint32_t))
	{
		const uint32_t r = rand_next(&rnd_context);
		if (fwrite(&r, sizeof(uint32_t), 1, stdout) != 1)
		{
			fprintf(stderr, "I/O error!\n");
			return 1;
		}
		count -= sizeof(uint32_t);
	}

	while (count > 0)
	{
		const int8_t r = (uint8_t) rand_next(&rnd_context);
		if (fwrite(&r, sizeof(uint8_t), 1, stdout) != sizeof(uint8_t))
		{
			fprintf(stderr, "I/O error!\n");
			return 1;
		}
		count -= sizeof(uint8_t);
	}

	return 0;
}

