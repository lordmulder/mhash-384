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

#ifndef INC_TWISTER_H
#define INC_TWISTER_H

#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Mersenne-Twister
//-----------------------------------------------------------------------------

#define TWISTER_N 624
#define TWISTER_M 397
#define TWISTER_UPPER_MASK 0x80000000UL
#define TWISTER_LOWER_MASK 0x7fffffffUL

typedef struct
{
	uint32_t mt[TWISTER_N];
	int mti;
}
twister_t;

static void rand_init(twister_t *const ctx, const uint32_t seed)
{
	static const uint32_t SEED[32] =
	{
		0x1C0154B6UL, 0xF75D104EUL, 0xA7921CA4UL, 0x32C08836UL, 0x7455465FUL, 0x41A7F7BCUL, 0xB9245834UL, 0x084F141AUL,
		0xACF06595UL, 0xFCB5F0D6UL, 0x4686AC6DUL, 0xBB94FC41UL, 0x1E3F9036UL, 0x7AEF9807UL, 0x9A7EDF87UL, 0x10E208D7UL,
		0x72FCCFD0UL, 0x2033A7DAUL, 0x503701BAUL, 0x4CA49457UL, 0x76E15B31UL, 0x9EE51B83UL, 0x222E9F5EUL, 0xAD8327AEUL,
		0x70E13020UL, 0x29FD6519UL, 0x6EEF97B2UL, 0xECF6E01AUL, 0x202A69F8UL, 0x26F36CC1UL, 0xBEB0D794UL, 0x0143C398UL
	};
	ctx->mt[0] = seed ^ SEED[seed % 32];
	for (ctx->mti = 1; ctx->mti < TWISTER_N; ctx->mti++)
	{
		ctx->mt[ctx->mti] = (1812433253UL * (ctx->mt[ctx->mti - 1] ^ (ctx->mt[ctx->mti - 1] >> 30)) + ctx->mti);
		ctx->mt[ctx->mti] &= 0xffffffffUL;
	}
}

static inline uint32_t rand_next_uint(twister_t *const ctx)
{
	uint32_t y;
	static const uint32_t mag01[2] = { 0x0UL, 0x9908B0DFUL };
	if (ctx->mti >= TWISTER_N)
	{
		int kk;
		if (ctx->mti == TWISTER_N + 1)
		{
			rand_init(ctx, 5489UL);
		}
		for (kk = 0; kk < TWISTER_N - TWISTER_M; kk++)
		{
			y = (ctx->mt[kk] & TWISTER_UPPER_MASK) | (ctx->mt[kk + 1] & TWISTER_LOWER_MASK);
			ctx->mt[kk] = ctx->mt[kk + TWISTER_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (; kk < TWISTER_N - 1; kk++)
		{
			y = (ctx->mt[kk] & TWISTER_UPPER_MASK) | (ctx->mt[kk + 1] & TWISTER_LOWER_MASK);
			ctx->mt[kk] = ctx->mt[kk + (TWISTER_M - TWISTER_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (ctx->mt[TWISTER_N - 1] & TWISTER_UPPER_MASK) | (ctx->mt[0] & TWISTER_LOWER_MASK);
		ctx->mt[TWISTER_N - 1] = ctx->mt[TWISTER_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
		ctx->mti = 0;
	}
	y = ctx->mt[ctx->mti++];
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9D2C5680UL;
	y ^= (y << 15) & 0xEFC60000UL;
	y ^= (y >> 18);
	return y;
}

static inline void rand_next_bytes(twister_t *const ctx, uint8_t *const out, const size_t size)
{
	const size_t words = size / sizeof(uint32_t);
	const size_t bytes = size % sizeof(uint32_t);
	uint8_t *put = out;
	for (size_t i = 0; i < words; ++i)
	{
		const uint32_t rnd = rand_next_uint(ctx);
		memcpy(put, &rnd, sizeof(uint32_t));
		put += sizeof(uint32_t);
	}
	if(bytes)
	{
		const uint32_t rnd = rand_next_uint(ctx);
		memcpy(put, &rnd, bytes);
	}
}

static inline uint32_t next_rand_range(twister_t *const ctx, const uint32_t n)
{
	static const uint32_t DIV[64] =
	{
		0xFFFFFFFF,0xFFFFFFFF,0x80000000,0x55555556,0x40000000,0x33333334,0x2AAAAAAB,0x24924925,0x20000000,0x1C71C71D,0x1999999A,0x1745D175,0x15555556,0x13B13B14,0x12492493,0x11111112,
		0x10000000,0x0F0F0F10,0x0E38E38F,0x0D79435F,0x0CCCCCCD,0x0C30C30D,0x0BA2E8BB,0x0B21642D,0x0AAAAAAB,0x0A3D70A4,0x09D89D8A,0x097B425F,0x0924924A,0x08D3DCB1,0x08888889,0x08421085,
		0x08000000,0x07C1F07D,0x07878788,0x07507508,0x071C71C8,0x06EB3E46,0x06BCA1B0,0x06906907,0x06666667,0x063E7064,0x06186187,0x05F417D1,0x05D1745E,0x05B05B06,0x0590B217,0x0572620B,
		0x05555556,0x0539782A,0x051EB852,0x05050506,0x04EC4EC5,0x04D4873F,0x04BDA130,0x04A7904B,0x04924925,0x047DC120,0x0469EE59,0x0456C798,0x04444445,0x04325C54,0x04210843,0x04104105
	};
	return (n < 64) ? (rand_next_uint(ctx) / DIV[n]) : (rand_next_uint(ctx) / (UINT32_MAX / n + 1U));
}

#endif //INC_TWISTER_H
