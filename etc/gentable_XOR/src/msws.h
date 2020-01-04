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
*  Copyright (c) 2014-2018 Bernard Widynski                                *
*                                                                          *
*  This code can be used under the terms of the GNU General Public License *
*  as published by the Free Software Foundation, either version 3 of the   *
*  License, or any later version. See the GPL license at URL               *
*  http://www.gnu.org/licenses                                             *
*                                                                          *
\**************************************************************************/

/**************************************************************************\
*                                                                          *
*  Modifications applied by LoRd_MuldeR <mulder2@gmx.de>:                  *
*                                                                          *
*  1. Added function to get random 32-Bit value with upper limit           *
*  2. Added function to get random 64-Bit value                            *
*  3. Added function to get random byte-sequence of arbitrary length       *
*  4. Improved initialization (seeding) function                           *
*  5. Made all functions thread-safe by avoiding the use of static vars    *
*  6. Implemented C++ wrapper class, for convenience                       *
*                                                                          *
\**************************************************************************/

#ifndef _INC_MSWS_H
#define _INC_MSWS_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t msws_t[3];

inline static uint32_t msws_uint32(msws_t ctx)
{
	ctx[0] *= ctx[0]; ctx[0] += (ctx[1] += ctx[2]);
	return (uint32_t)(ctx[0] = (ctx[0] >> 32) | (ctx[0] << 32));
}

static inline uint32_t msws_uint32_max(msws_t ctx, const uint32_t max)
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
	return (max < 64)
		? (msws_uint32(ctx) / DIV[max])
		: (msws_uint32(ctx) / (UINT32_MAX / max + 1U));
}

inline static uint64_t msws_uint64(msws_t ctx)
{
	return (((uint64_t)msws_uint32(ctx)) << 32U) | msws_uint32(ctx);
}

inline static void msws_bytes(msws_t ctx, uint8_t *buffer, size_t len)
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

inline static void msws_init(msws_t ctx, const uint32_t seed)
{
	ctx[0] = UINT64_C(0); ctx[1] = UINT64_C(0);
	ctx[2] = (((uint64_t)seed) << 1U) + 0xB5AD4ECEDA1CE2A9;
	for (int i = 0; i < 13; ++i)
	{
		volatile uint32_t q = msws_uint32(ctx);
	}
}

#endif //_INC_MSWS_H
