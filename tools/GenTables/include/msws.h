/**************************************************************************\
*                                                                          *
*  Middle Square Weyl Sequence Random Number Generator                     *
*                                                                          *
*  msws() - returns a 32 bit unsigned int [0,0xffffffff]                   *
*                                                                          *
*  The state vector consists of three 64 bit words:  x, w, and s           *
*                                                                          *
*  x - random output [0,0xffffffff]                                        *
*  w - Weyl sequence (period 2^64)                                         *
*  s - an odd constant                                                     *
*                                                                          *
*  The Weyl sequence is added to the square of x.  The middle is extracted *
*  by shifting right 32 bits:                                              *
*                                                                          *
*  x *= x; x += (w += s); return x = (x>>32) | (x<<32);                    *
*                                                                          *
*  The constant s should be set to a random 64-bit pattern with the upper  *
*  32 bits non-zero and the least significant bit set to 1.  There are     *
*  on the order of 2^63 numbers of this type.  Since the stream length     *
*  is 2^64, this provides about 2^127 random numbers in total.             *
*                                                                          *
*  Note:  This version of the RNG includes an idea proposed by             *
*  Richard P. Brent (creator of the xorgens RNG).  Brent suggested         *
*  adding the Weyl sequence after squaring instead of before squaring.     *
*  This provides a basis for uniformity in the output.                     *
*                                                                          *
*  Copyright (c) 2014-2017 Bernard Widynski                                *
*                                                                          *
*  This code can be used under the terms of the GNU General Public License *
*  as published by the Free Software Foundation, either version 3 of the   *
*  License, or any later version. See the GPL license at URL               *
*  http://www.gnu.org/licenses                                             *
*                                                                          *
\**************************************************************************/

#ifndef _INC_MSWS_H
#define _INC_MSWS_H

#include <stdint.h>

typedef struct
{
	uint64_t x;
	uint64_t w;
	uint64_t s;
}
msws_t;

inline static uint32_t msws_uint32(msws_t *const ctx)
{
	ctx->x *= ctx->x; ctx->x += (ctx->w += ctx->s);
	return (uint32_t)(ctx->x = (ctx->x >> 32) | (ctx->x << 32));
}

inline static void msws_init(msws_t *const ctx, const uint32_t seed)
{
	ctx->x = UINT64_C(0); ctx->w = UINT64_C(0);
	ctx->s = ((uint64_t)seed + 0xB5AD4ECEDA1CE2A9) | UINT64_C(1);
	for (int i = 0; i < 13; ++i)
	{
		volatile uint32_t q = msws_uint32(ctx);
	}
}

static inline uint32_t msws_range(msws_t *const ctx, const uint32_t n)
{
	static const uint32_t DIV[64] =
	{
		0xFFFFFFFF, 0xFFFFFFFF, 0x80000000, 0x55555556,
		0x40000000, 0x33333334, 0x2AAAAAAB, 0x24924925,
		0x20000000, 0x1C71C71D, 0x1999999A, 0x1745D175,
		0x15555556, 0x13B13B14, 0x12492493, 0x11111112,
		0x10000000, 0x0F0F0F10, 0x0E38E38F, 0x0D79435F,
		0x0CCCCCCD, 0x0C30C30D, 0x0BA2E8BB, 0x0B21642D,
		0x0AAAAAAB, 0x0A3D70A4, 0x09D89D8A, 0x097B425F,
		0x0924924A, 0x08D3DCB1, 0x08888889, 0x08421085,
		0x08000000, 0x07C1F07D, 0x07878788, 0x07507508,
		0x071C71C8, 0x06EB3E46, 0x06BCA1B0, 0x06906907,
		0x06666667, 0x063E7064, 0x06186187, 0x05F417D1,
		0x05D1745E, 0x05B05B06, 0x0590B217, 0x0572620B,
		0x05555556, 0x0539782A, 0x051EB852, 0x05050506,
		0x04EC4EC5, 0x04D4873F, 0x04BDA130, 0x04A7904B,
		0x04924925, 0x047DC120, 0x0469EE59, 0x0456C798,
		0x04444445, 0x04325C54, 0x04210843, 0x04104105
	};
	return (n < 64) ? (msws_uint32(ctx) / DIV[n]) : (msws_uint32(ctx) / (UINT32_MAX / n + 1U));
}

inline static void msws_bytes(msws_t *const ctx, uint8_t *buffer, size_t len)
{
	if (((len & 3U) == 0U) && ((((uintptr_t)buffer) & 3U) == 0U))
	{
		uint32_t *ptr = (uint32_t*)buffer;
		for (len >>= 2U; len; --len)
		{
			*ptr++ = msws_uint32(ctx);
		}
	}
	else
	{
		uint32_t tmp;
		for(size_t sft = 0U; len; --len, sft = (sft + 1U) & 3U)
		{
			*buffer++ = (uint8_t)(sft ? (tmp >>= 8U) : (tmp = msws_uint32(ctx)));
		}
	}
}

#endif _INC_MSWS_H
