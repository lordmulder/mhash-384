#ifndef INC_COMMON_H
#define INC_COMMON_H

#define _CRT_RAND_S  
#include <stdlib.h> 
#include <stdint.h>
#include <time.h>
#include <stdio.h>

#include "hd_table.h"

static inline void crit_exit(const char *const msg)
{
	printf("\n\n%s\n\n", msg);
	flushall();
#ifndef NDEBUG
	abort();
#endif
	for(;;) _exit(666);
}

static inline uint32_t make_seed(void)
{
	uint32_t seed;
	if (rand_s(&seed) != 0)
	{
		crit_exit("FATAL: System PRNG initialization has failed!");
	}
	return seed;
}

static inline invert_byte_buffer(uint8_t *const buffer, const size_t size)
{
	const size_t words = size / sizeof(uint32_t);
	const size_t bytes = size % sizeof(uint32_t);
	uint8_t *pos = buffer;
	for (size_t i = 0; i < words; ++i)
	{
		*((uint32_t*)pos) = (~(*((uint32_t*)pos)));
		pos += sizeof(uint32_t);
	}
	for (size_t i = 0; i < bytes; ++i)
	{
		*pos = (~(*pos));
		pos += sizeof(uint8_t);
	}
}

static inline uint32_t hamming_distance(const uint8_t *const a, const uint8_t *const b, const size_t len)
{
	uint32_t distance = 0U;
	for (size_t i = 0; i < len; ++i)
	{
		distance += HAMMING_DISTANCE_LUT[a[i]][b[i]];
	}
	return distance;
}

static inline void get_time_str(char *const time_string, const size_t buff_size)
{
	time_t current_time;
	struct tm time_info;
	time(&current_time);
	if (localtime_s(&time_info, &current_time))
	{
		crit_exit("FATAL: localtime_s() has failed!");
	}
	const size_t len = strftime(time_string, buff_size, "%H:%M:%S", &time_info);
	if ((len < 1) || (len >= buff_size))
	{
		crit_exit("FATAL: strftime() has failed!");
	}
}

static uint32_t adler32(const uint8_t *const buffer, size_t buflength)
{
	uint32_t s1 = 1, s2 = 0;
	for (size_t n = 0; n < buflength; n++)
	{
		s1 = (s1 + buffer[n]) % 65521;
		s2 = (s2 + s1) % 65521;
	}
	return (s2 << 16) | s1;
}

#endif //INC_COMMON_H