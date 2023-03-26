/* $Id: t_spExtras.c $ */

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

/* Some tests for `extra' single-precision functions
   -- some tests assume word size >= 32 bits
*/

#include <stdio.h>
#include <assert.h>
#include "bigdigits.h"
#include "spExtras.h"

#ifndef NELEMS
#define NELEMS(x) ((sizeof(x)) / (sizeof((x)[0])))
#endif /* NELEMS */

static int is_in_list(DIGIT_T a, DIGIT_T *list, size_t n)
/* Returns true (1) if a is in list of n elements; else false (0) */
{
	size_t i;
	int result = 0;

	for (i = 0; i < n; i++)
	{
		if (a == list[i])
		{
			result = 1;
			break;
		}
	}
	return result;
}

int main(void)
{
	DIGIT_T x, y, g;
	DIGIT_T m, e, n, c, z, t;
	DIGIT_T p, v, u, w, a, q;
	int res, i, ntries;
	DIGIT_T primes32[] = { 5, 17, 65, 99 };
	DIGIT_T primes16[] = { 15, 17, 39 };

	/* Test greatest common divisor (gcd) */
	printf("Test spGcd:\n");
	/* simple one */
	x = 15; y = 27;
	g = spGcd(x, y);
	printf("gcd(%" PRIuBIGD ", %" PRIuBIGD ") = %" PRIuBIGD "\n", x, y, g);
	assert(g == 3);

	/* contrived using small primes */
	x = 53 * 37; y = 53 * 83;
	g = spGcd(x, y);
	printf("gcd(%" PRIuBIGD ", %" PRIuBIGD ") = %" PRIuBIGD "\n", x, y, g);
	assert(g == 53);

	/* contrived using bigger primes */
	x = 0x0345 * 0xfedc; y = 0xfedc * 0x0871;
	g = spGcd(x, y);
	printf("gcd(0x%" PRIxBIGD ", 0x%" PRIxBIGD ") = 0x%" PRIxBIGD "\n", x, y, g);
	assert(g == 0xfedc);

	/* Known primes: 2^16-15, 2^32-5 */
	y = 0x10000 - 15;
	x = spSimpleRand(1, y);
	g = spGcd(x, y);
	printf("gcd(0x%" PRIxBIGD ", 0x%" PRIxBIGD ") = %" PRIxBIGD "\n", x, y, g);
	assert(g == 1);

	y = 0xffffffff - 5 + 1;
	x = spSimpleRand(1, y);
	g = spGcd(x, y);
	printf("gcd(0x%" PRIxBIGD ", 0x%" PRIxBIGD ") = %" PRIxBIGD "\n", x, y, g);
	assert(g == 1);

	/* Test spModExp */
	printf("Test spModExp:\n");
	/* Verify that (m^e mod n).(z^e mod n) == (m.z)^e mod n 
	   for random m, e, n, z */
	/* Generate some random numbers */
	n = spSimpleRand(MAX_DIGIT / 2, MAX_DIGIT);
	m = spSimpleRand(1, n -1);
	e = spSimpleRand(1, n -1);
	z = spSimpleRand(1, n -1);
	/* Compute c = m^e mod n */
	spModExp(&c, m, e, n);
	printf("c=m^e mod n=%" PRIxBIGD "^%" PRIxBIGD " mod %" PRIxBIGD "=%" PRIxBIGD "\n", m, e, n, c);
	/* Compute x = c.z^e mod n */
	printf("z=%" PRIxBIGD "\n", z);
	spModExp(&t, z, e, n);
	spModMult(&x, c, t, n);
	printf("x = c.z^e mod n =   %" PRIxBIGD "\n", x);
	/* Compute y = (m.z)^e mod n */
	spModMult(&t, m, z, n);
	spModExp(&y, t, e, n);
	printf("y = (m.z)^e mod n = %" PRIxBIGD "\n", x);
	/* Check they are equal */
	assert(x == y);

	/* Test spModInv */
	printf("Test spModInv:\n");
	/* Use identity that (vp-1)^-1 mod p = p-1 
	   for prime p and integer v */
	/* known prime */
	p = 0x10000 - 15;
	/* small multiplier */
	v = spSimpleRand(2, 10);
	u = v * p - 1;
	printf("u = vp-1 = %" PRIuBIGD " * %" PRIuBIGD " - 1 = %" PRIuBIGD "\n", v, p, u);
	/* compute w = u^-1 mod p */
	spModInv(&w, u, p);
	printf("w = u^-1 mod p = %" PRIuBIGD "\n", w);
	/* check wu mod p == 1 */
	spModMult(&c, w, u, p);
	printf("Check 1 == wu mod p = %" PRIuBIGD "\n", c);
	assert(c == 1);
	/* Try mod inversion that should fail */
	/* Set u = pq so that gcd(u, p) != 1 */
	q = 31;
	u = p * q;
	printf("p=%" PRIuBIGD " q=%" PRIuBIGD " pq=%" PRIuBIGD "\n", p, q, u);
	printf("gcd(pq, p) = %" PRIuBIGD " (i.e. not 1)\n", spGcd(u, p));
	res = spModInv(&w, u, p);
	printf("w = (pq)^-1 mod p returns error %d (expected 1)\n", res);
	assert(res != 0);

	/* Test spIsPrime */
	printf("Test spIsPrime:\n");
	/* Find primes just less than 2^32. Ref: Knuth p408 */
	for (n = 0xffffffff, a = 1; a < 100; a++, n--)
	{
		if (spIsPrime(n, 50))
		{
			printf("2^32-%" PRIuBIGD " is prime\n", a);
			assert(is_in_list(a, primes32, NELEMS(primes32)));
		}
	}
	/* And just less than 2^16 */
	for (n = 0xffff, a = 1; a < 50; a++, n--)
	{
		if (spIsPrime(n, 50))
		{
			printf("2^16-%" PRIuBIGD " is prime\n", a);
			assert(is_in_list(a, primes16, NELEMS(primes16)));
		}
	}
	/* Generate a random prime < MAX_DIGIT */
	n = spSimpleRand(0, MAX_DIGIT);
	/* make sure odd */
	n |= 0x01;
	/* expect to find a prime approx every lg(n) numbers, 
	   so for sure within 100 times that */
	ntries = BITS_PER_DIGIT * 100;
	for (i = 0; i < ntries; i++)
	{
		if (spIsPrime(n, 50))
			break;
		n += 2;
	}
	printf("Random prime, n = %" PRIuBIGD " (0x%" PRIxBIGD ")\n", n, n);
	printf("found after %d candidates\n", i+1);
	if (i >= ntries)
		printf("Didn't manage to find a prime in %d tries!\n", ntries);
	else
		assert(spIsPrime(n, 50));

	res = spIsPrime(n, 50);
	printf("spIsPrime(%" PRIuBIGD ") is %s\n", n, (res ? "TRUE" : "FALSE"));

	/* Check using (less accurate) Fermat test (these could fail) */
	w = 2;
	spModExp(&x, w, n-1, n);
	printf("Fermat test: %" PRIxBIGD "^(n-1) mod n = %" PRIxBIGD " (%s)\n", w, x, (x == 1 ? "PASSED" : "FAILED!"));
	w = 3;
	spModExp(&x, w, n-1, n);
	printf("Fermat test: %" PRIxBIGD "^(n-1) mod n = %" PRIxBIGD " (%s)\n", w, x, (x == 1 ? "PASSED" : "FAILED!"));
	w = 5;
	spModExp(&x, w, n-1, n);
	printf("Fermat test: %" PRIxBIGD "^(n-1) mod n = %" PRIxBIGD " (%s)\n", w, x, (x == 1 ? "PASSED" : "FAILED!"));

	/* Try a known Fermat liar (Carmichael number) */
	n = 561;
	printf("Try n = 561 = 3*11*17 (a 'Fermat liar')\n");
	w = 5;
	spModExp(&x, w, n-1, n);
	printf("Fermat test: %" PRIxBIGD "^(n-1) mod n = %" PRIxBIGD " (%s)\n", w, x, (x == 1 ? "PASSED" : "FAILED!"));
	res = spIsPrime(n, 50);
	printf("spIsPrime(%" PRIuBIGD ") is %s\n", n, (res ? "TRUE" : "FALSE"));
	assert(!res);

	return 0;
}
