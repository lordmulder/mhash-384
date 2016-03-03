#ifndef INC_COMMON_H
#define INC_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static inline void crit_exit(const char *const msg)
{
	printf("\n\n%s\n\n", msg);
	flushall();
#ifndef NDEBUG
	abort();
#endif
	for(;;) _exit(666);
}

static inline uint32_t hamming_distance(const uint32_t *const a, const uint32_t *const b, const size_t len)
{
	uint32_t dist = 0;
	for (size_t k = 0; k < len; k++)
	{
		uint32_t c = a[k] ^ b[k];
		while (c)
		{
			++dist;
			c &= (c - 1);
		}
	}
	return dist;
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