/* $Id: bigdRand.c $ */

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

/* Random number BIGD functions that rely on spBetterRand */

#include "bigdRand.h"
#include "bigdigitsRand.h"

#include <stdio.h>

bdigit_t bdRandDigit(void)
/* Return a random digit. */
{
	return spBetterRand();
}

size_t bdRandomBits(BIGD a, size_t nbits)
/* Generate a random BIGD number <= 2^{nbits}-1 using internal RNG */
{
	const int bits_per_digit = sizeof(bdigit_t) * 8;
	size_t i;
	int j;
	bdigit_t r;

	bdSetZero(a);
	bdSetBit(a, nbits-1, 0);
	r = bdRandDigit();
	j = bits_per_digit;
	for (i = 0; i < nbits; i++)
	{
		if (j <= 0)
		{
			r = bdRandDigit();
			j = bits_per_digit;
		}
		bdSetBit(a, i, r & 0x1);
		r >>= 1;
		j--;
	}

	return i;
}

/** Generate array of random octets (bytes) using internal RNG. 
This function is in the correct form for BD_RANDFUNC.
Seed is ignored here.
*/
int bdRandomOctets(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen)
{
	return mpRandomOctets(bytes, nbytes, seed, seedlen);
}

size_t bdRandomNumber(BIGD a, BIGD n)
{	/* Generate a number in the range [0, n-1] */
	size_t nbits = bdBitLength(n);
	bdSetZero(a);
	do {
		bdRandomBits(a, nbits);
	} while (bdCompare(a, n) >= 0);
	return bdSizeof(a);
}

