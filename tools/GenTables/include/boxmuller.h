/* ----------------------------------------------------------------------------------------------- */
/* MHash-384 - Generate tables utility program                                                     */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                             */
/*                                                                                                 */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software   */
/* and associated documentation files(the "Software"), to deal in the Software without             */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,      */
/* distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions:                            */
/*                                                                                                 */
/* The above copyright notice and this permission notice shall be included in all copies or        */
/* substantial portions of the Software.                                                           */
/*                                                                                                 */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING   */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND      */
/* NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,     */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.         */
/* ----------------------------------------------------------------------------------------------- */

#ifndef INC_BOXMULLER_H
#define INC_BOXMULLER_H

#include "common.h"
#include "twister.h"

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
	bool flag;
	double z0, z1;
}
bxmller_t;

static inline void gaussian_noise_init(bxmller_t *const bxmller)
{
	memset(bxmller, 0, sizeof(bxmller_t));
}

static inline uint32_t gaussian_noise_next(twister_t *const rand, bxmller_t *const bxmller, const double sigma, const uint32_t min, const uint32_t max)
{
	static const double TWOPI = 6.283185307179586476925286766559005768394338798750211641949;
	uint32_t result;
	do
	{
		bxmller->flag = (!bxmller->flag);
		if (!bxmller->flag)
		{
			const double value = round(bxmller->z1 * sigma);
			result = (uint32_t)clip_dbl(0.0, value, ((double)UINT32_MAX));
		}
		else
		{
			double u1, u2;
			do
			{
				u1 = rand_next_uint(rand) / ((double)UINT32_MAX);
				u2 = rand_next_uint(rand) / ((double)UINT32_MAX);
			}
			while (u1 <= DBL_MIN);
			bxmller->z0 = sqrt(-2.0 * log(u1)) * cos(TWOPI * u2);
			bxmller->z1 = sqrt(-2.0 * log(u1)) * sin(TWOPI * u2);
			const double value = round(bxmller->z0 * sigma);
			result = (uint32_t)clip_dbl(0.0, value, ((double)UINT32_MAX));
		}
	}
	while ((result < min) || (result > max));
	return result;
}

#endif //INC_BOXMULLER_H
