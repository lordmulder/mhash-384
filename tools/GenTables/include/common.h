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

#endif //INC_COMMON_H