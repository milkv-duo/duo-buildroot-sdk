/* spExtras.c */

/***** BEGIN LICENSE BLOCK *****
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2001-15 David Ireland, D.I. Management Services Pty Limited
 * <http://www.di-mgt.com.au/bigdigits.html>. All rights reserved.
 *
 ***** END LICENSE BLOCK *****/
/*
 * Last updated:
 * $Date: 2015-10-22 10:23:00 $
 * $Revision: 2.5.0 $
 * $Author: dai $
 */


#include <assert.h>
#include "bigdigits.h"
#include "spExtras.h"

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m)
{	/*	Computes a = (x * y) mod m */
	
	/* Calc p[2] = x * y */
	DIGIT_T p[2];
	spMultiply(p, x, y);

	/* Then modulo */
	*a = mpShortMod(p, m, 2);
	return 0;
}

DIGIT_T spGcd_old(DIGIT_T x, DIGIT_T y)
{	/*	Returns gcd(x, y) */

	/* Ref: Schneier 2nd ed, p245 */
	
	DIGIT_T g;

	if (x + y == 0)
		return 0;	/* Error */

	g = y;
	while (x > 0)
	{
		g = x;
		x = y % x;
		y = g;
	}
	return g;
}

DIGIT_T spGcd(DIGIT_T a, DIGIT_T b)
{	/*	Returns gcd(a, b) */

	/* Ref: Cohen, Algorithm 1.3.5 (Binary GCD) 
	and Menezes, Algorithm 14.54 */

	DIGIT_T r, t;
	unsigned int k;

	/* 1. [Reduce size once] */
	if (a < b)
	{	/* exchange a and b */
		t = a;
		a = b;
		b = t;
	}
	if (0 == b)
	{
		return a;
	}
	r = a % b;
	a = b;
	b = r;

	if (0 == b)
	{
		return a;
	}
	/* 2. [Compute power of 2] */
	k = 0;	
	while (ISEVEN(a) && ISEVEN(b))
	{
		a >>= 1;	/* a <-- a/2 */
		b >>= 1;	/* b <-- b/2 */
		k++;		/* g <-- 2g */
	}
	
	while (a != 0)
	{
		while (ISEVEN(a))
		{
			a >>= 1;	/* a <-- a/2 until a is odd */
		}
		while (ISEVEN(b))
		{
			b >>= 1;	/* b <-- a/2 until b is odd */
		}
		/* t <-- |a-b|/2 */
		if (b > a)
			t = (b - a) >> 1;
		else
			t = (a - b) >> 1;
		/* If a >= b then a = t, otherwise b = t */
		if (a >= b)
			a = t;
		else
			b = t;
	}
	/* Output (2^k.b) */
	return (b << k);
}

/* spIsPrime */

static DIGIT_T SMALL_PRIMES[] = { 2, 3, 5, 7, 11, 13, 17, 19 };
#define N_SMALL_PRIMES sizeof(SMALL_PRIMES)/sizeof(DIGIT_T)

int spIsPrime(DIGIT_T w, size_t t)
{	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) 
	*/
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379
		and ANSI 9.42-2003 Annex B.1.1.
	*/

	unsigned int i, j;
	DIGIT_T m, a, b, z;
	int failed;

	/*	First check for small primes */
	for (i = 0; i < N_SMALL_PRIMES; i++)
	{
		if (w % SMALL_PRIMES[i] == 0)
			return 0;	/* Failed */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */

	m = w - 1;
	for (a = 0; ISEVEN(m); a++)
		m >>= 1;	/* Divide by 2 until m is odd */

	/*
	assert((1 << a) * m + 1 == w);
	*/

	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate a random integer 1 < b < w */
		/* [v2.1] changed to 1 < b < w-1 */
		b = spSimpleRand(2, w - 2);

		/*
		assert(1 < b && b < w-1);
		*/

		/* Step 4. Set j = 0 and z = b^m mod w */
		j = 0;
		spModExp(&z, b, m, w);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			if ((j == 0 && z == 1) || (z == w - 1))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (j > 0 && z == 1)
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			j++;
			if (j < a)
				spModMult(&z, z, z, w);
			/* Loop: if j < a go to Step 5 */
		} while (j < a);

		if (failed)
		{	/* Step 8. Not a prime - stop */
			return 0;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* If got here, probably prime => success */
	return 1;
}

/* spModExp */

/* Two alternative functions for spModExp */

int spModExpK(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{	/*	Computes exp = x^n mod d */
	/*	Ref: Knuth Vol 2 Ch 4.6.3 p 462 Algorithm A
	*/
	DIGIT_T y = 1;		/* Step A1. Initialise */

	while (n > 0)
	{							/* Step A2. Halve N */
		if (n & 0x1)			/* If odd */
			spModMult(&y, y, x, d);	/*   Step A3. Multiply Y by Z */	
		
		n >>= 1;					/* Halve N */
		if (n > 0)				/* Step A4. N = 0? Y is answer */
			spModMult(&x, x, x, d);	/* Step A5. Square Z */
	}

	*exp = y;
	return 0;
}

int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T e, DIGIT_T m)
{	/*	Computes exp = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	DIGIT_T y;	/* Temp variable */

	/* Find most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e & mask)
			break;
	}

	y = x;
	/* For j = k-2 downto 0 step -1 */
	for (mask >>= 1; mask > 0; mask >>= 1)
	{
		spModMult(&y, y, y, m);		/* Square */
		if (e & mask)
			spModMult(&y, y, x, m);	/* Multiply */
	}

	*exp = y;
	return 0;
}

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342 
		ignoring u2, v2, t2
		and avoiding negative numbers
	*/
	DIGIT_T u1, u3, v1, v3, t1, t3, q, w;
	int bIterations = 1;
	int result;
	
	/* Step X1. Initialise */
	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	while (v3 != 0)	/* Step X2. */
	{	/* Step X3. */
		q = u3 / v3;	/* Divide and */
		t3 = u3 % v3;
		w = q * v1;		/* "Subtract" */
		t1 = u1 + w;
		/* Swap */
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		bIterations = -bIterations;
	}

	if (bIterations < 0)
		*inv = v - u1;
	else
		*inv = u1;

	/* Make sure u3 = gcd(u,v) == 1 */
	if (u3 != 1)
	{
		result = 1;
		*inv = 0;
	}
	else
		result = 0;

	return result;
}

