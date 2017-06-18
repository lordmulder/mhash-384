/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Self-test code                                                                     */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 */
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

#ifndef MHASH_CLI_SELFTEST_INCLUDED
#define MHASH_CLI_SELFTEST_INCLUDED

/*Include*/
#include "mhash_384.h"

/*CRT includes*/
#include <stdlib.h>
#include <stdio.h>

/*C++ support*/
#ifdef __cplusplus
using namespace mhash_384;
using namespace mhash_384::internals;
#endif

/*Abort helper macro*/
#ifndef NDEBUG
#define ABORT_TEST(X) abort()
#else
#define ABORT_TEST(X) exit((X))
#endif

/*Assert macro*/
#define MY_ASSERT(EXP, MSG) do \
{ \
	if(!(EXP)) \
	{ \
		fprintf(stderr, "FAILURE: %s!\n\n", (MSG)); \
		for(;;) ABORT_TEST(1); \
	} \
} \
while(0)

/*Test vectors and reference outputs*/
#define TEST_COUNT 8
static const char *const TEST_VECTOR[TEST_COUNT] =
{
	"", "A", "AA", "AAA",
	"Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
	"Frank jagt im komplett verwahrlosten Taxi quer durch Bayern",
	"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.\nUt wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.\nNam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis.\nAt vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum. sanctus sea sed takimata ut vero voluptua. est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat.\nConsetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus.\nLorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.\nUt wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.\nNam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo.",
	"@/NZhR/&=#8<Ojh=K_zHo;Q.sF|rZNVSiaN(9|gjylXU\"qaXs7j@l7**aCss$=`Ar3qNQ1E1JEf\\+[~J_u!b!9,f(+T2jV8yKodz8jIF]V5Zg@#M]eC?=i=.Ct%smOnj:7{r:spEaiOcz<PxZ^y$zzdfi]\\/g2.Up%/'YO]W8Be^.PA`W!11?85;0F'\\oPPLu7c\\Gb2|De<6Y0AH_a{<OB;3t.f2*V?O_5GA,4QFI~Xqi_$5T5ko)#Rb4\\9YZ$d!R,fJ)1@Y}f5]&dUq3z{WaDw.qbKuHbCy\"x.Gi}dG#RAR@6yUfTLLA>z?+nia%s@?rl/Tla/d]1rN]DB|;;k3D%6#8{(-\\o#>)6He`D@Q?aF:?lwX0K%0V\"Y6qk:A0tCMAZ&uVDFWks8R{x]l4te]\"IowlsH[&qv-<?m|bvNC#rZTD1L'x~LH(-.}e{!RbwrALR3n5sl(y'9PKSKr:t}4g|^k6\\Z#w\\Z7:QIZ;Df{hfb,80]fJ-^*hR\")[0ND#?s\"!i>OIl/ht,%98_@F7)KX9,i1(CaJd?!`F/r43T1Vrf%NzrncO$f;'a\"$?pr},r<[^@t^||%l[@PEC9O1kn](`RgA%l[#^a:ot$'3&:|[FB#KAN#2!]kYE\"Sx!Hua.Uq>x2z2'D8/m{'eiB+J5|r;K4g'oayq{K_~;yM#pf5-}HBn>uOJeh55~${r$9a1=kLg!bBi+TF7FR_j>`@b@&BvZ@=2r~M3Y.Q*\"fPE[;D[pHOV>G,1jm*oMsE\\t.b5GqKJMum<kMs\\C}43;Z^@[H)^e.&US9UMpTje(E*4*>^VlH)R^l2)tBHfqS8yYo=Ya^}5'&P>F,G+6Fu7=U.gi!>k1\\Uq6D&=t=g]&Cz>Uka7($wc/hz=cSGqJ#T3Tv0FK&WqYj.YT5u:h1x@<;#c\\YDP\\atcxi`7W2Kg2eD9DyCB=s\"=$M6\\diQ;~#N@zYtH-{m#fV%(BN>c'!%\\7v3f\\x{\\_.76e?P=l\\Vf.,L3="
};
static const uint8_t TEST_RESULT[TEST_COUNT][MHASH_384_LEN] =
{
	{ 0x99,0x81,0xA5,0xEC,0x65,0x62,0xB1,0xC7,0x49,0x51,0x78,0x94,0x59,0x73,0x1B,0xAF,0xA7,0x48,0x0F,0x9D,0x9E,0x9A,0x19,0xCC,0x3C,0xA1,0x04,0x63,0x0C,0xA9,0xAB,0x62,0xFD,0x6E,0x6C,0x62,0x42,0xED,0x98,0x92,0xF9,0x95,0xD2,0xA8,0x0D,0xC2,0x62,0x1F },
	{ 0xB7,0xF7,0x1D,0x1B,0x1D,0xE6,0xA9,0xD4,0x15,0x5B,0x8A,0x9F,0xBB,0x2B,0xB1,0x9C,0x30,0x40,0x9F,0x74,0x06,0xCB,0x0F,0x4C,0x39,0x7D,0x99,0x7F,0xDE,0xA4,0x68,0x6F,0x98,0x9E,0xF5,0x44,0x48,0x22,0xF1,0x37,0x89,0x6F,0x9D,0xC5,0x05,0x41,0x45,0xF8 },
	{ 0x34,0x34,0x80,0x11,0x15,0x14,0xF5,0xFA,0x8C,0x89,0x68,0xEF,0x2B,0x0D,0x4B,0xF1,0x26,0xC4,0x07,0x7C,0xAC,0xBD,0x02,0x41,0xF6,0x25,0x81,0xFF,0xD4,0x43,0x98,0x7C,0xBF,0xE6,0x29,0x41,0x7B,0x3E,0xA0,0x5E,0xC6,0xCA,0x74,0x52,0xBD,0x24,0x4E,0x0F },
	{ 0x8A,0xA7,0x24,0x9E,0x43,0x96,0x1C,0x90,0x27,0x53,0xF1,0x53,0xA5,0x07,0xA1,0xDE,0xE3,0xC4,0x17,0xD2,0x74,0x13,0x79,0x32,0xD3,0x87,0x15,0xFE,0xFB,0x14,0x73,0xAA,0xDD,0xB9,0x14,0x77,0x58,0x01,0x7C,0xFD,0xD6,0x89,0x45,0x68,0x99,0x79,0x7A,0x05 },
	{ 0x99,0x04,0x23,0x92,0x45,0xCF,0x29,0x9F,0x5F,0x4A,0xD4,0x00,0x1D,0xD4,0x92,0x61,0x53,0x80,0x01,0xB1,0xA9,0x4D,0xF4,0x97,0x5E,0x10,0x2F,0x75,0x22,0x90,0xD1,0x59,0x0D,0x3A,0xFA,0x3A,0xA1,0xD0,0xF4,0x70,0xD6,0x77,0x67,0xA3,0x05,0xF9,0x26,0x36 },
	{ 0x30,0x7D,0xEF,0xE2,0x9B,0xC3,0x30,0x57,0x4D,0x79,0xAA,0x2B,0x0C,0xAB,0x7C,0x4F,0xAF,0x8D,0xCB,0x73,0x5D,0xF2,0xDD,0xDA,0x5D,0x8E,0x5F,0x23,0x3C,0x28,0x1B,0xAF,0xA9,0x25,0xCC,0x72,0x33,0xE4,0x7A,0x44,0xEC,0xA3,0xBB,0x23,0x62,0x76,0x9E,0x5C },
	{ 0x56,0xD1,0x98,0x42,0x26,0x38,0x71,0x60,0x92,0x3D,0xBB,0x51,0xD4,0x5C,0x9B,0x79,0x1E,0x50,0xA5,0xB9,0xD9,0x31,0x3B,0x5D,0x6D,0x96,0x3C,0xE1,0xC4,0xF5,0xF8,0xEA,0x36,0x6C,0x56,0x11,0x02,0x63,0x3E,0x37,0x43,0xCC,0xE5,0x57,0xBA,0xDA,0xFF,0xA6 },
	{ 0xA2,0x87,0xB6,0x63,0x40,0xD0,0x6E,0xFA,0x50,0x26,0xD1,0x36,0x37,0xB7,0x75,0x7F,0x44,0x8D,0xB6,0x3B,0x9B,0x5B,0xAC,0x12,0xAD,0x2B,0x4C,0x4A,0x45,0xB8,0xB9,0xAE,0x30,0x3C,0x04,0xE3,0xE6,0xC0,0x45,0x96,0x14,0x1E,0x4F,0xD1,0x50,0x53,0x68,0x77 }
};

static MHASH_384_INLINE uint32_t test_distance_xor(const uint8_t *const a, const uint8_t *const b)
{
	uint32_t k, distance = 0;
	for (k = 0; k < MHASH_384_LEN; k++)
	{
		uint8_t c = a[k] ^ b[k];
		while (c)
		{
			++distance;
			c &= (c - 1);
		}
	}
	return distance;
}

static int self_test(void)
{
	uint32_t i, j;
	uint8_t result[MHASH_384_LEN];
	mhash_384_t context;

	/*test XOR table*/
	fprintf(stderr, "Self-test, step 1 of 2 running...\n");
	for (i = 0; i < UINT8_MAX+2; i++)
	{
		for (j = 0; j < UINT8_MAX+2; j++)
		{
			if (i != j)
			{
				const uint32_t distance = test_distance_xor(&MHASH_384_TABLE_XOR[i][0], &MHASH_384_TABLE_XOR[j][0]);
				MY_ASSERT((distance >= 178) && (distance <= 220), "XOR table verification failed");
			}
		}
	}

	/*test hash function*/
	fprintf(stderr, "Self-test, step 2 of 2 running...\n");
	for (i = 0; i < TEST_COUNT; ++i)
	{
		fprintf(stderr, "VECTOR[%u]: ", i);
		mhash_384_initialize(&context);
		mhash_384_update(&context, (const uint8_t*)TEST_VECTOR[i], strlen(TEST_VECTOR[i]));
		mhash_384_finalize(&context, result);
		print_digest(result, 0);
		MY_ASSERT(!memcmp(result, TEST_RESULT[i], sizeof(uint8_t) * MHASH_384_LEN), "Test vector did NOT compare equal");
	}

	/*completed*/
	fprintf(stderr, "Self-test completed succesfully.\n\n");
	return 0;
}

#endif /*MHASH_CLI_SELFTEST_INCLUDED*/
