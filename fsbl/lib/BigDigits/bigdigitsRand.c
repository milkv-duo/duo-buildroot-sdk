/* $Id: bigdigitsRand.c $ */

/***** BEGIN LICENSE BLOCK *****
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2001-16 David Ireland, D.I. Management Services Pty Limited
 * <http://www.di-mgt.com.au/bigdigits.html>. All rights reserved.
 *
 ***** END LICENSE BLOCK *****/
/*
 * Last updated:
 * $Date: 2016-03-31 09:51:00 $
 * $Revision: 2.6.1 $
 * $Author: dai $
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigdigits.h"
#include "bigdigitsRand.h"

static uint32_t btrrand(void);

/**********************/
/* EXPORTED FUNCTIONS */
/**********************/

DIGIT_T spBetterRand(void)
{	/*	Returns a "better" pseudo-random digit. */
	return (DIGIT_T)btrrand();
}

/* Added [v2.2] */
size_t mpRandomBits(DIGIT_T a[], size_t ndigits, size_t nbits)
	/* Generate a random mp number <= 2^{nbits}-1 using internal RNG */
{
	const int bits_per_digit = BITS_PER_DIGIT;
	size_t i;
	int j;
	DIGIT_T r;

	mpSetZero(a, ndigits);
	r = spBetterRand();
	j = bits_per_digit;
	for (i = 0; i < nbits; i++)
	{
		if (j <= 0)
		{
			r = spBetterRand();
			j = bits_per_digit;
		}
		mpSetBit(a, ndigits, i, r & 0x1);
		r >>= 1;
		j--;
	}

	return i;
}

/** Generate array of random octets (bytes) using internal RNG. 
This function is in the correct form for BD_RANDFUNC.
Seed is ignored here.
*/
int mpRandomOctets(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen)
{
	const int octets_per_digit = sizeof(DIGIT_T);
	size_t i;
	int j;
	DIGIT_T r;

	r = spBetterRand();
	j = octets_per_digit;
	for (i = 0; i < nbytes; i++)
	{
		if (j <= 0)
		{
			r = spBetterRand();
			j = octets_per_digit;
		}
		bytes[i] = r & 0xFF;
		r >>= 8;
		j--;
	}
	return (int)i;
}

/**********************/
/* INTERNAL FUNCTIONS */
/**********************/

/******************************************************************************
Generates a pseudo-random DIGIT value using the generator algorithm from 
ANSI X9.31 Appendix A.2.4 "Generating Pseudo Random Numbers Using the DEA"
(formerly ANSI X9.17, Appendix C) but with the `Tiny Encryption Algorithm` (TEAX)
replacing the `DES E-D-E two-key triple-encryption` algorithm (Double DES)
This variant has much less code, and is faster.

CAUTION: not thread-safe as it uses a static variable.

This is "pretty good", but not quite cryptographically secure since the seed is
only generated from the current time and process ID. 
However, it is much better than just using the plain-old-rand() function. 
The output should always pass the FIPS 140-2 statistical test.
Users can make their own call as to the security of this approach.
It's certainly sufficient for generating random digits for tests.
******************************************************************************/

/******************************************************************************
ANSI X9.17/X9.31 ALGORITHM:
Given

	* D, a 64-bit representation of the current date-time
	* S, a secret 64-bit seed that will be updated by this process
	* K, a secret encryption key

Step 1. Compute the 64-bit block X = G(S, K, D) as follows:

   1. I = E(D, K)
   2. X = E(I XOR S, K)
   3. S' = E(X XOR I, K)

where E(p, K) is the encryption of the 64-bit block p using key K.

Step 2. Return X and set S = S' for the next cycle. 

******************************************************************************
THIS VARIANT:
1. Replace `Double DES` algorithm with `TEAX`.
2. Replace effective 112-bit Double DES key with 128-bit TEAX key.
******************************************************************************/

#define KEY_WORDS 4
static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k);

/* CAUTION: We use a static structure to store our values in. */
static struct {
	int seeded;
	uint32_t seed[2];
	uint32_t key[KEY_WORDS];
} m_generator;

#if !(defined(ONLY_ANSI)) && (defined(_WIN32) || defined(WIN32))
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#elif !(defined(ONLY_ANSI)) && (defined(unix) || defined (linux) || defined(__linux))
#else
#endif
/* Cross-platform ways to get a 64-bit time value and the process ID */
#if defined(unix) || defined(__unix__) || defined(linux) || defined(__linux__)
static void get_time64(uint32_t t[2])
{
	#include <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv, NULL);
	memcpy(t, &tv, 2*sizeof(uint32_t));
}
#include <unistd.h>
#define processid getpid
#elif defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void get_time64(uint32_t t[2])
{
	FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	t[0] = ft.dwHighDateTime;
	t[1] = ft.dwLowDateTime;
}
#define processid GetCurrentProcessId
#else /* ANSI FALLBACK */
static void get_time64(uint32_t t[2])
{
	/* Best we can do with strict ANSI */
	/* [v2.2] added clock() as well as time() to improve precision.
	 * OK, so this isn't actually the time, but it's an independent value accurate to
	 * one millisecond, which is what we want.
	 */

	t[0] = (uint32_t)time(NULL);
	t[1] = (uint32_t)clock();
}
unsigned long processid(void)
{ return 0; }
#endif

/* Given a 32-bit random seed, create a 64-bit seed S and 128-bit key K for the global generator */
static void btrseed(uint32_t seed)
{
	int i;
	uint32_t t[2];

	/* Use plain old rand function to generate our global 64-bit seed S and initial 128-bit key K_0 */
	srand(seed);
	/* Only trust the lowest 8 bits from rand()... */
	for (i = 0; i < 2; i++)	
		m_generator.seed[i] = (rand()&0xFF)<<24|(rand()&0xFF)<<16|(rand()&0xFF)<<8|(rand()&0xFF);
	for (i = 0; i < KEY_WORDS; i++)
		m_generator.key[i] =  (rand()&0xFF)<<24|(rand()&0xFF)<<16|(rand()&0xFF)<<8|(rand()&0xFF);

	/* Set flag so we only do this once */
	m_generator.seeded = 1;

	/* [v2.4] Blend in the current 64-bit time and re-encrypt the key with itself */
	/* Note that the `block` in E(block, key) is 64 bits, but the `key` is 128 bits */

	/* T = E(time, K_0) -- encrypt 64-bit time using K_0 */
	get_time64(t);
	encipher(t, t, m_generator.key);
	/* K_1[0..63] = E(T XOR K_0[0..63], K_0) 
	   -- encrypt left 64 bits of K_0 using K_0 --> left 64 bits of K_1 
	   -- right 64 bits of K_0 --> right 64 bits of K_1 */
	m_generator.key[0] ^= t[0];
	m_generator.key[1] ^= t[1];
	encipher(&m_generator.key[0], &m_generator.key[0], m_generator.key);
	/* K_2[64..127] = E(T XOR K_1[64..127], K_1) 
	   -- encrypt right 64 bits of K_1 using K_1 --> right 64 bits of K_2 
	   -- left 64 bits of K_! --> left 64 bits of K_2 */
	m_generator.key[2] ^= t[0];
	m_generator.key[3] ^= t[1];
	encipher(&m_generator.key[2], &m_generator.key[2], m_generator.key);
	/* Output global key K = K_2 */
}

static uint32_t btrrand(void)
/* Returns one 32-bit word */
{
	uint32_t inter[2], x[2];

	/* Set seed if not already seeded */
	if (!m_generator.seeded)
	{	/* [v2.4] added process ID so processes launched at same time should give different values */
		btrseed((uint32_t)time(NULL)<<16 ^ ((uint32_t)clock()<<8) ^ (uint32_t)processid());
	}

	/* I = E(D, K) */
	get_time64(inter);
	encipher(inter, inter, m_generator.key);

	/* X = E(I XOR S, K) */
	x[0] = inter[0] ^ m_generator.seed[0];
	x[1] = inter[1] ^ m_generator.seed[1];
	encipher(x, x, m_generator.key);

	/* S' = E(X XOR I, K) */
	inter[0] ^= x[0];
	inter[1] ^= x[1];
	encipher(inter, m_generator.seed, m_generator.key);

	return x[0];
}

/************************************************

The Tiny Encryption Algorithm (TEA) by 
David Wheeler and Roger Needham of the
Cambridge Computer Laboratory.

Placed in the Public Domain by
David Wheeler and Roger Needham.

**** ANSI C VERSION (New Variant) ****

Notes:

TEA is a Feistel cipher with XOR and
and addition as the non-linear mixing
functions. 

Takes 64 bits of data in v[0] and v[1].
Returns 64 bits of data in w[0] and w[1].
Takes 128 bits of key in k[0] - k[3].

TEA can be operated in any of the modes
of DES. Cipher Block Chaining is, for example,
simple to implement.

n is the number of iterations. 32 is ample,
16 is sufficient, as few as eight may be OK.
The algorithm achieves good dispersion after
six iterations. The iteration count can be
made variable if required.

Note this is optimised for 32-bit CPUs with
fast shift capabilities. It can very easily
be ported to assembly language on most CPUs.

delta is chosen to be the real part of (the
golden ratio Sqrt(5/4) - 1/2 ~ 0.618034
multiplied by 2^32).

This version has been amended to foil two
weaknesses identified by David A. Wagner
(daw@cs.berkeley.edu): 1) effective key
length of old-variant TEA was 126 not 128
bits 2) a related key attack was possible
although impractical.

************************************************/

static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k)
{
	register uint32_t y=v[0],z=v[1],sum=0,delta=0x9E3779B9,n=32;

	while(n-->0)
	{
		y+= (((z<<4) ^ (z>>5)) + z) ^ (sum + k[sum & 3]);
		sum += delta;
		z+= (((y<<4) ^ (y>>5)) + y) ^ (sum + k[sum>>11 & 3]);
	}

	w[0]=y; w[1]=z;
}

