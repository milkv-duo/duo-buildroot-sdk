/* $Id: t_bdTest.c $ */

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

/* Various tests of "bd" functions, not exhaustive */

#if _MSC_VER >= 1100
	/* Detect memory leaks in MSVC++ */ 
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bigd.h"
#include "bigdRand.h"

static void pr_bytesmsg(const char *msg, unsigned char *bytes, size_t nbytes)
/* Display a message followed by the hex values of a byte array */
{
	size_t i;
	printf("%s", msg);
	for (i = 0; i < nbytes; i++)
		printf("%02x", bytes[i]);
	printf("\n");
}	

int my_rand(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen)
/* Our own (very insecure) random generator call-back function using good old rand() 
   This demonstrates the required format for BD_RANDFUNC
   -- replace this in practice with your own cryptographically-secure function.
*/
{
	unsigned int myseed;
	size_t i;
	int offset;

	/* Use time - then blend in seed, if any */
	myseed = (unsigned)time(NULL);
	if (seed)
	{
		for (offset = 0, i = 0; i < seedlen; i++, offset = (offset + 1) % sizeof(unsigned))
			myseed ^= ((unsigned int)seed[i] << (offset * 8));
	}

	srand(myseed);
	while (nbytes--)
	{
		*bytes++ = rand() & 0xFF;
	}

	return 0;
}

int fermat_test(BIGD n)
{
/* Carries out a quick Fermat primality test on n > 4.
   Returns 1 if n is prime and 0 if composite or -1 if n < 5.
   This is not foolproof but we use it as a 
   check on our main bdIsPrime function.
   It will return a false positive for Fermat Liars, which
   are very rare.
*/
	BIGD a, e, r;
	int isprime = 1;

	/* For any integer a, 1<=a<=n-1, if a^(n-1) mod n != 1
	   then n is composite. */

	if (bdShortCmp(n, 5) < 0) return -1;

	a = bdNew();
	e = bdNew();
	r = bdNew();
	/* e = n -1 */
	bdSetEqual(e, n);
	bdDecrement(e);

	/* Set a = 2 and compute a^(n-1) mod n */
	bdSetShort(a, 2);
	bdModExp(r, a, e, n);
	if (bdShortCmp(r, 1) != 0)
		isprime = 0;

	/* a = 3 */
	bdSetShort(a, 3);
	bdModExp(r, a, e, n);
	if (bdShortCmp(r, 1) != 0)
		isprime = 0;

	/* a = 5 */
	bdSetShort(a, 5);
	bdModExp(r, a, e, n);
	if (bdShortCmp(r, 1) != 0)
		isprime = 0;

	bdFree(&a);
	bdFree(&e);
	bdFree(&r);

	return isprime;
}

/** Compute w = u - v using cutoff subtraction so `w >= 0` always. 
 *  @return 1 if subtraction underflowed or 0 for normal result
 */
int cutoff_subtraction(BIGD w, BIGD u, BIGD v)
{
	int carry = 0;
	if (bdCompare(u, v) < 0) {
		bdSetZero(w);
		carry = 1;
	}
	else {
		bdSubtract(w, u, v);
	}
	return carry;
}

int main(void)
{
	BIGD u, v, w, q, r, p, b, a;
	BIGD n, e, m, c, z, mz, cz, t;
	BIGD x, y;
	bdigit_t overflow, d;
	int cmp, res;
	unsigned char ff[64], bytes[16];
	int i, mc, jac;
	size_t nbytes, nbits;
	char s[128];

/* MSVC memory leak checking stuff */
#if _MSC_VER >= 1100
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
#endif

	printf("Tests for BIGD functions\n");

	/* Initialize - new way in [v2.6]*/
	bdNewVars(&u, &v, &w, &p, &q, &r, &x, &y);
	//u = bdNew();
	//v = bdNew();
	//w = bdNew();
	//p = bdNew();
	//q = bdNew();
	//r = bdNew();
	//x = bdNew();
	//y = bdNew();

	printf("\nSIMPLE ARITHMETIC OPERATIONS...\n");
	bdPrintDecimal("At start w=", w, "\n");
	assert(bdIsZero(w));

	/* Addition */
	/* 1 + 1 = 2 */
	bdSetShort(u, 1);
	bdSetShort(v, 1);
	bdAdd(w, u, v);
	bdPrintDecimal("1+1=", w, "\n");
	assert(bdShortCmp(w, 2) == 0);

	/* Add with digit overflow */
	bdSetShort(u, 0xffffffff);
	bdSetShort(v, 0xffffffff);
	bdAdd(w, u, v);
	bdPrintHex("ffffffff+ffffffff=", w, "\n");
	/* ffffffff+ffffffff=00000001 fffffffe */
	assert(bdSizeof(w) == 2);

	/* Bigger random digits */
	bdSetRandTest(u, 10);
	bdSetRandTest(v, 10);
	bdAdd(w, u, v);
	bdPrintHex("u=\n", u, "\n");
	bdPrintHex("v=\n", v, "\n");
	bdPrintHex("w=u+v=\n", w, "\n");

	/* Subtract */
	bdSubtract(r, w, v);
	bdPrintHex("w-v=\n", r, "\n");
	assert(bdCompare(r, u) == 0);

	/* Multiplication */
	/* 2 * 3 = 6 */
	bdSetShort(u, 2);
	bdSetShort(v, 3);
	bdMultiply(w, u, v);
	bdPrintDecimal("2 * 3=", w, "\n");
	assert(bdShortCmp(w, 6) == 0);

	bdSetShort(u, 0xffffffff);
	bdSetShort(v, 0xffffffff);
	bdMultiply(w, u, v);
	bdPrintHex("ffffffff*ffffffff=", w, "\n");
	/* ffffffff*ffffffff=fffffffe 00000001 */
	assert(bdGetBit(w, 0) == 1);

	/* Use ShortMult */
	bdShortMult(p, u, 0xffffffff);
	bdPrintHex("(Short)ffffffff*ffffffff=", p, "\n");
	assert(bdCompare(p, w) == 0);

	/* Use larger random u, v */
	bdSetRandTest(u, 10);
	bdSetRandTest(v, 20);
	bdPrintHex("u=\n", u, "\n");
	bdPrintHex("v=\n", v, "\n");

	/* Use ShortMult by one: w = v * 1 */
	bdShortMult(w, v, 1);
	bdPrintHex("(Short)v*1=\n", w, "\n");
	assert(bdCompare(w, v) == 0);

	/* Use ShortMult by two */
	bdShortMult(w, u, 2);
	bdPrintHex("(Short)u*2=\n", w, "\n");
	assert(bdIsEven(w));

	/* Big multiplication */
	bdMultiply(w, u, v);
	bdPrintHex("w=u*v=\n", w, "\n");

	/* Divide expecting q = w/v == u and r = w%v == 0 */
	bdDivide(q, r, w, v);
	bdPrintHex("w/v=\n", q, "\n");
	bdPrintHex("w%v=", r, "\n");
	assert(bdIsEqual(q, u));
	assert(bdShortCmp(r, 0) == 0);

	/* Modulo */
	bdIncrement(w);
	bdModulo(r, w, v);
	bdPrintHex("w+1 mod v=", r, "\n");
	assert(bdShortCmp(r, 1) == 0);

	bdDecrement(w);
	bdModulo(r, w, u);
	bdPrintHex("w+1-1 mod u=", r, "\n");
	assert(bdShortCmp(r, 0) == 0);

	/* Use a short divisor q=w/2 */
	bdShortDiv(q, r, w, 2);
	bdPrintHex("(ShortDiv)w/2=\n", q, "\n");
	bdPrintHex("(ShortDiv)w%2=", r, "\n");

	printf("w is %s\n", bdIsOdd(w) ? "ODD" : "EVEN");
	bdShortMod(r, w, 2);
	bdPrintHex("w mod 2=", r, "\n");
	bdIncrement(w);
	printf("w+1 is %s\n", bdIsOdd(w) ? "ODD" : "EVEN");
	bdShortMod(r, w, 2);
	bdPrintHex("++w mod 2=", r, "\n");
	bdDecrement(w);

	/* Check with short mult u = q*2 */
	bdShortMult(u, q, 2);
	bdPrintHex("(ShortMult) (w/2) * 2=\n", u, "\n");

	printf("\nSQUARE AND SQUARE ROOT...\n");
	/* Square a random number w = u^2 */
	bdSetRandTest(u, 10);
	bdPrintHex("random u=\n", u, "\n");
	bdSquare(w, u);
	bdPrintHex("(Square) u^2=", w, "\n");
	/* check against product p = u * u */
	bdMultiply(p, u, u);
	bdPrintHex("u*u=", p, "\n");
	assert(bdIsEqual(p, w));
	/* Compute integer square root [new in v2.1] */
	bdSqrt(p, w);
	bdPrintHex("sqrt(w)=", p, "\n");
	assert(bdIsEqual(p, u));

	printf("\nMODULAR ARITHMETIC...\n");
	/* Check that (u mod n + v mod n) mod n == (u+v) mod n */
	bdSetRandTest(u, 10);
	bdSetRandTest(v, 10);
	bdSetRandTest(w, 10);
	bdPrintHex("(Modulo)\nNew random u=", u, "\n");
	bdPrintHex("v=", v, "\n");
	bdPrintHex("w=", w, "\n");
	/* r = u mod w */
	bdModulo(r, u, w);
	/* q = v mod w */
	bdModulo(q, v, w);
	/* q = u mod n + v mod n */
	bdAdd(q, q, r);
	/* r = (u mod w + v mod w) mod w */
	bdModulo(r, q, w);
	bdPrintHex("(u mod w + v mod w) mod w=\n", r, "\n");
	/* q = (u+v) mod w */
	bdSetEqual(p, u);
	bdAdd(p, p, v);
	bdModulo(q, p, w);
	bdPrintHex("(u+v) mod w=\n", q, "\n");
	assert(bdIsEqual(r, q));

	/* ModMult and ModSquare */
	bdModMult(p, u, v, w);
	bdPrintHex("u * v mod w=\n", p, "\n");
	bdModMult(p, u, u, w);
	bdPrintHex("u * u mod w=\n", p, "\n");
	bdModSquare(q, u, w);
	bdPrintHex("u^2 mod w=\n", q, "\n");
	assert(bdIsEqual(p, q));


	/* Clear and start again */
	bdSetZero(u);
	bdSetZero(v);
	bdSetZero(w);

	printf("\nARITHMETIC BOUNDARY CONDITIONS...\n");
	/* This causes Divide algorithm to `Add Back' with 32-bit digits 
	   See Knuth 4.3.1 Algorithm D Step D6 */
	printf("Check `Add Back' in Divide algorithm\n");
	bdConvFromHex(u, "7fffffff 80000001 00000000 00000000");
	bdConvFromHex(v, "00000000 80000000 80000002 00000005");
	bdDivide(q, r, u, v);
	bdPrintHex("u=", u, "\n");
	bdPrintHex("v=", v, "\n");
	bdPrintHex("q=u/v=", q, "\n");
	bdPrintHex("r=u mod v=", r, "\n");
	/* check that qv + r = u */
	bdMultiply(w, q, v);
	bdAdd(w, w, r);
	bdPrintHex("qv+r=", w, "\n");
	assert(bdIsEqual(w, u));
	/* And check that r = u mod v >= (1-2/b)v 
	   See Knuth 4.3.1 Exercise 21 */
	bdConvFromHex(p, "1 00000000");
	bdSetEqual(w, p);
	bdShortSub(w, w, 2);
	bdMultiply(u, w, v);
	bdDivide(q, w, u, p);
	bdPrintHex("(1-2/b)=", q, "\n");
	cmp = bdCompare(r, q);
	printf("u mod v %s (1-2/b)v\n", (cmp >= 0 ? ">=" : "<"));


	printf("\nCope, sort of, with `negative` numbers...\n");
	bdSetZero(u);
	overflow = bdShortSub(w, u, 2);
	bdPrintHex("w=0-2=", w, "\n");
	printf("overflow=%" PRIuBIGD "\n", overflow);
	overflow = bdIncrement(w);
	bdPrintHex("w+1=", w, "\n");
	printf("overflow=%" PRIuBIGD "\n", overflow);
	overflow = bdIncrement(w);
	bdPrintHex("(Note error!) w+1=", w, "\n");
	printf("overflow=%" PRIuBIGD "\n", overflow);

	printf("\nUse cut-off subtraction...\n");
	bdSetShort(u, 3);
	bdSetShort(v, 7);
	cutoff_subtraction(w, u, v);
	bdPrintDecimal("3-7=", w, "\n");
	// But OK if no underflow
	cutoff_subtraction(w, v, u);
	bdPrintDecimal("7-3=", w, "\n");

	printf("\nCatch problems working close to zero...\n");
	bdSetZero(u);
	overflow = bdShortAdd(u, u, 1);
	bdPrintHex("u=0+1=", u, "\n");
	printf("overflow=%" PRIuBIGD " (expected 0)\n", overflow);

	printf("Compute w = 0 + 0...\n");
	bdSetZero(u);
	bdSetZero(v);
	bdAdd(w, u, v);
	printf("bdPrint(w)=");
	bdPrint(w, 0x1);
	bdPrintDecimal("w=0+0=", w, "\n");
	bdPrintHex("w=0+0=0x", w, "\n");
	bdPrintBits("w=0+0='", w, "'b\n");
	assert(bdIsZero(w));

	bdAdd(u, w, v);
	bdPrintHex("w+0=", u, "\n");
	bdAdd(w, w, v);
	bdPrintHex("w+=0=", w, "\n");


	/* Set u = v = ffff...ffff */
	for (i = 0; i < sizeof(ff); i++)
		ff[i] = 0xff;
	bdConvFromOctets(u, ff, sizeof(ff));
	bdConvFromOctets(v, ff, sizeof(ff));
	bdPrintHex("u=", u,"\n");
	bdPrintHex("v=", v,"\n");

	/* Compute u * v */
	bdMultiply(w, u, v);
	bdPrintHex("w = u * v =\n", w,"\n");
	/* Set v = w / u */
	bdDivide(v, r, w, u);
	bdPrintHex("v = w / u =\n", v,"\n");
	bdPrintHex("rem=", r,"\n");
	assert(bdIsEqual(u, v));
	assert(bdShortCmp(r, 0) == 0);

	bdIncrement(u);
	bdDecrement(v);
	bdMultiply(w, u, v);
	bdPrintHex("w = ++u * --v =\n", w,"\n");
	bdDivide(w, r, u, v);
	bdPrintHex("q = u / v =", w,"\n");
	bdPrintHex("rem=", r,"\n");
	assert(bdShortCmp(w, 1) == 0);
	assert(bdShortCmp(r, 2) == 0);


	/* Clear and start again */
	bdSetZero(u);
	bdSetZero(v);
	bdSetZero(w);

	bdSetShort(u, 0);	/* Fixed [v2.3]: This used to cause an error */
	bdPrintDecimal("bdSetShort(u, 0)=> u=", u, "\n");
	bdSetShort(v, 3);
	bdModulo(w, u, v);
	bdPrintDecimal("0 mod 3=", w, "\n");

	/* Set a new random value for v */
	bdSetRandTest(v, 10);
	bdPrintHex("Random v=\n", v,"\n");
	bdShortAdd(u, v, 1);
	bdPrintHex("u = v + 1 =\n", u,"\n");
	cmp = bdCompare(u, v);
	printf("bdCompare(u-v) = %d\n", cmp);
	assert(cmp > 0);
	cmp = bdCompare(v, u);
	printf("bdCompare(v-u) = %d\n", cmp);
	assert(cmp < 0);
	cmp = bdCompare(u, u);
	printf("bdCompare(u-u) = %d\n", cmp);
	assert(cmp == 0);

	/* Compare with short */
	bdSetShort(w, 0xfffffffe);
	bdPrintHex("w=", w,"\n");
	cmp = bdShortCmp(w, 1);
	assert(cmp > 0);
	printf("bdShortCmp(w-1) = %d\n", cmp);
	cmp = bdShortCmp(w, 0xffffffff);
	assert(cmp < 0);
	printf("bdShortCmp(w-0xffffffff) = %d\n", cmp);
	cmp = bdShortCmp(w, 0xfffffffd);
	printf("bdShortCmp(w-0xfffffffd) = %d\n", cmp);
	assert(cmp > 0);
	/* Convert w back to a short and compare */
	d = bdToShort(w);	/* NB This will truncate if w is too big */
	printf("d=bdToShort(w)=%" PRIxBIGD "\n", d);
	cmp = bdShortIsEqual(w, d);
	printf("bdShortIsEqual(w, d)=%d\n", cmp);
	assert(cmp == 1);

	printf("\nBIT SHIFT OPERATIONS...\n");
	/* Try shifting bits */
	bdPrintHex("w      = ", w,"\n");
	bdFree(&u);
	u = bdNew();
	bdShiftLeft(u, w, 2);
	bdPrintHex("w << 2 = ", u,"\n");
	assert(bdGetBit(u, 33) == 1);
	bdShiftRight(v, u, 3);
	bdPrintHex("(w << 2) >> 3 = ", v,"\n");
	assert(bdGetBit(v, 31) == 0);

	/* Bigger shift */
	bdShiftLeft(u, w, 128);
	bdPrintHex("w << 128 = ", u,"\n");
	assert(bdGetBit(u, 159) == 1);
	bdShiftRight(v, u, 129);
	bdPrintHex("(w << 128) >> 129 = ", v,"\n");
	assert(bdGetBit(v, 31) == 0);

	/* Use a new variable */
	b = bdNew();
	bdSetZero(b);	/* overkill */
	bdSetBit(b, 510, 1);
	bdSetBit(b, 477, 1);
	bdSetBit(b, 31, 1);
	bdSetBit(b, 0, 1);
	bdPrintHex("b=\n", b,"\n");

	bdSetBit(b, 512, 1);
	bdSetBit(b, 31, 0);
	bdPrintHex("b'=\n", b,"\n");
	printf("Bit 512 is %d\n", bdGetBit(b, 512));
	assert(bdGetBit(b, 512) == 1);
	printf("Bit 511 is %d\n", bdGetBit(b, 511));
	assert(bdGetBit(b, 511) == 0);
	assert(bdGetBit(b, 477) == 1);
	/* NB one-based bit lengths so expecting 513 */
	printf("Bit Length = %u\n", bdBitLength(b));
	assert(bdBitLength(b) == 513);
	bdPrintHex("b'=\n", b,"\n");
	/* Destroy b */
	bdFree(&b);
	
	printf("\nRANDOM NUMBER GENERATION...\n");
	/* Create a random number using `my_rand' callback function */
	bdRandomSeeded(r, 508, (const unsigned char*)"", 0, my_rand);
	bdPrintHex("random=\n", r,"\n");
	/* NB bit length may be less than 508 */
	printf("%u bits\n", bdBitLength(r));
	assert(bdBitLength(r) <= 508);

	/* Create a random number using the internal RNG */
	bdRandomBits(r, 509);
	bdPrintHex("random=\n", r,"\n");
	printf("%u bits\n", bdBitLength(r));
	assert(bdBitLength(r) <= 509);

	printf("\nPRIME NUMBERS...\n");
	/* Generate two random primes */
	printf("Generating two primes...\n");
	bdGeneratePrime(p, 128, 5, (const unsigned char*)"1", 1, my_rand);
	bdPrintHex("prime p=\n", p,"\n");
	bdGeneratePrime(q, 128, 5, (const unsigned char*)"abcdef", 6, my_rand);
	bdPrintHex("prime q=\n", q,"\n");

	/* Check primality with more tests (64 vs 5) */
	/* NB vv small chance this could fail */
	printf("Checking prime: p ");
	res = bdIsPrime(p, 64);
	printf("%s\n", (res ? "is OK" : "is NOT prime!!"));
	/* And again using Fermat test */
	res = fermat_test(p);
	if (res == 0) printf("WARNING: p failed Fermat test.\n");

	/* w = product pq */
	bdMultiply(w, p, q);
	bdPrintHex("pq=\n", w,"\n");

	/* Product pq should not be prime */
	res = bdIsPrime(w, 5);
	printf("Composite pq %s\n", (res ? "IS prime" : "is NOT prime"));
	assert(res == 0);

	/* Check GCD: gcd(pq, p) == p */
	bdGcd(r, w, p);
	bdPrintHex("gcd(pq, p) =\n", r,"\n");
	assert(bdIsEqual(r, p));

	/* --Modular inversion-- */
	printf("\nMODULAR INVERSION...\n");
	/* set v to be a small random multiplier */
	bdSetRandTest(v, 1);
	bdIncrement(v);	/* Make sure not zero */
	bdPrintHex("multiplier, v=", v,"\n");
	/* set u = vp - 1 */
	bdMultiply(u, v, p);
	bdDecrement(u);
	bdPrintHex("p=\n", p,"\n");
	bdPrintHex("u = vp - 1 =\n", u,"\n");

	/* compute w = u^-1 mod p */
	res = bdModInv(w, u, p);
	bdPrintHex("w = u^-1 mod p=\n", w,"\n");
	assert(res == 0);

	/* check that wu mod p == 1 */
	bdModMult(r, w, u, p);
	bdPrintHex("wu mod p = ", r,"\n");
	assert(bdShortCmp(r, 1) == 0);

	/* Try mod inversion that should fail */
	/* Set u = pq so that gcd(u, p) != 1 */
	bdMultiply(u, p, q);
	res = bdModInv(w, u, p);
	printf("w = (pq)^-1 mod p returns (expected) error %d\n", res);
	assert(res != 0);
	bdPrintHex("output, w=", w,"\n");

	/* Do some bit string operations */
	printf("\nBIT STRING OPERATIONS...\n");

	/* Use XOR to swop two variables without a temp variable, i.e.
	   a = a XOR b; b = a XOR b; a = a XOR b; */
	printf("Swop u and w:\n");
	/* Generate 2 random numbers (u,w) each 144 bits long */
	bdRandomBits(u, 144);
	bdRandomBits(w, 144);
	bdPrintHex("u= ", u,"\n");
	bdPrintHex("w= ", w,"\n");
	/* remember these for later (p=u, q=w) */
	bdSetEqual(p, u);
	bdSetEqual(q, w);
	/* Swop using XOR */
	bdXorBits(u, u, w);
	bdXorBits(w, u, w);
	bdXorBits(u, u, w);
	bdPrintHex("u'=", u,"\n");
	bdPrintHex("w'=", w,"\n");
	/* check they have swopped accurately */
	assert(bdIsEqual(u, q));
	assert(bdIsEqual(w, p));

	/* Try some simple AND and OR ops */
	printf("AND and OR ops:\n");
	/* Set every even bit in u and every odd bit in v */
	bdSetZero(u);
	bdSetZero(v);
	for (i = 0; i < 144 / 2; i++)
	{
		bdSetBit(u, (i * 2), 1);
		bdSetBit(v, (i * 2) + 1, 1);
	}
	bdPrintHex("u=      ", u,"\n");
	bdPrintHex("v=      ", v,"\n");
	bdAndBits(w, u, v);
	bdPrintHex("u AND v=", w,"\n");
	/* expected answer = zero */
	assert(bdIsZero(w));
	bdOrBits(w, u, v);
	bdPrintHex("u OR v= ", w,"\n");
	/* expected answer p = 2^144 - 1 */
	bdSetZero(p);
	bdSetBit(p, 144, 1);
	bdDecrement(p);
	bdPrintHex("2^144-1=", p,"\n");
	assert(bdIsEqual(p, w));

	/* Invert all bits */
	bdSetZero(p);
	bdSetZero(q);
	bdSetShort(p, 0xfffe);
	bdShiftLeft(p, p, 32);
	bdShortAdd(p, p, 0xfffe);
	bdPrintHex("p=    ", p,"\n");
	bdNotBits(q, p);
	bdPrintHex("NOT p=", q,"\n");
	/* check p AND (NOT p) == 0 */
	bdAndBits(w, p, q);
	bdPrintHex("p AND (NOT p)=", w,"\n");
	assert(bdIsZero(w));

	/* Compose a 400-bit string by concatenating random 160-bit sections 
	   and then truncating the result */
	printf("\nBit string concatenation and truncation:\n");
	/*
	ALGORITHM: (based on a similar one in RFC 2631/ANSI X.42)
	Set m' = m/160 where / represents integer division with rounding upwards
	Set U = 0
	For i = 0 to m' - 1
		Set R = FUNC_160 and V = FUNC_160 where FUNC_160 generates a unique 160-bit value
		U = U + (R XOR V) * 2^(160 * i)
	Form q from U by computing U mod (2^m) and setting the most
		significant bit (the 2^(m-1) bit) and the least significant bit to 1. 
		In terms of boolean operations, q = U OR 2^(m-1) OR 1. 
		Note that 2^(m-1) < q < 2^m
	*/
	/* In this example, m=400 and m'=3, and we just generate 160-bit random values */
	mc = 3;
	bdSetZero(u);	/* Set U = 0 */
	for (i = 0; i < mc; i++)
	{
		bdRandomBits(r, 160);	/* R = FUNC_160 */
		bdRandomBits(v, 160);	/* V = FUNC_160 */
		bdXorBits(p, r, v);		/* p = (R XOR V) */
		/* w = p * 2^160i, i.e. shift p left by 160i bits */
		bdShiftLeft(w, p, (160 * i));
		bdOrBits(u, u, w);	/* q = q + w */
		printf("w(%d)=", i);
		bdPrintHex("", w,"\n");
	}
	bdPrintHex("u = w(0)+w(1)+w(2) =\n", u,"\n");
	bdModPowerOf2(u, 400);	/* U = U mod (2^m) */
	bdPrintHex("u = u mod (2^m) = // `ModPowerOf2' \n", u,"\n");
	/* q = U OR 2^(m-1) OR 1 */
	bdSetEqual(q, u);		/* q = U */
	bdSetBit(q, 400-1, 1);	/* q = q OR 2^(m-1) */
	bdSetBit(q, 0, 1);		/* q = q OR 1 */
	bdPrintHex("q = u OR 2^(m-1) OR 1 =\n", q,"\n");
	/* Check that 2^(m-1) < q < 2^m */
	printf("BitLength(q)=%d\n", bdBitLength(q));
	bdSetZero(p);
	bdSetBit(p, 400-1, 1);	/* p = 2^(m-1) */
	assert(bdCompare(p, q) < 1);	/* p < q */
	bdSetZero(r);
	bdSetBit(r, 400, 1);	/* r = 2^m */
	assert(bdCompare(q, r) < 1);	/* q < r */
	printf("OK, checked that 2^(m-1) < q < 2^m\n");
	

	/* Create a new set for Modular exponentiation test */
	printf("\nMODULAR EXPONENTIATION...\n");
	n = bdNew();
	e = bdNew();
	m = bdNew();
	c = bdNew();
	z = bdNew();
	mz = bdNew();
	cz = bdNew();
	t = bdNew();

	/* n is a prime modulus */
	printf("Generating a prime...\n");
	bdGeneratePrime(n, 513, 5, NULL, 0, my_rand);
	bdPrintHex("prime n=\n", n,"\n");
	printf("bdBitLength(n)=%u bdSizeof(n)=%u\n", bdBitLength(n), bdSizeof(n));
	/* Check primality using Fermat test (NB could fail!) */
	res = fermat_test(n);
	if (res == 0) printf("WARNING: n failed Fermat test (this can happen!).\n");

	/* exponent e and base m are some random numbers < n */
	bdSetRandTest(e, bdSizeof(n) - 1);
	bdPrintHex("e=", e,"\n");
	bdSetRandTest(m, bdSizeof(n) - 1);
	bdPrintHex("random m=\n", m,"\n");

	/* Compute c = m^e mod n */
	bdModExp(c, m, e, n);
	bdPrintHex("c = m^e mod n =\n", c,"\n");

	/* Now use a random z and check that 
	   c.z^e == (m.z)^e (mod n) */
	bdSetRandTest(z, bdSizeof(n));
	bdPrintHex("random z=\n", z,"\n");

	/* Compute cz = c.z^e mod n (Note use of temp t) */
	bdModExp(t, z, e, n);
	bdModMult(cz, c, t, n);
	bdPrintHex("c.z^e mod n=\n", cz,"\n");
	/* Compute mz = (m.z)^e mod n */
	bdModMult(t, m, z, n);
	bdModExp(mz, t, e, n);
	bdPrintHex("(m.z)^e mod n==\n", mz,"\n");
	if (!bdIsEqual(mz, cz))
		printf("ERROR: (c.z^e == (m.z)^e (mod n) DOES NOT MATCH m'\n");
	else
		printf("c.z^e == (m.z)^e (mod n) checks OK\n");
	assert(bdIsEqual(mz, cz));

	/* Repeat using constant-time exponentiation */

	printf("Using constant-time exponentiation...\n");
	/* Compute cz = c.z^e mod n (Note use of temp t) */
	bdModExp_ct(t, z, e, n);
	bdModMult(cz, c, t, n);
	bdPrintHex("c.z^e mod n=\n", cz,"\n");
	/* Compute mz = (m.z)^e mod n */
	bdModMult(t, m, z, n);
	bdModExp_ct(mz, t, e, n);
	bdPrintHex("(m.z)^e mod n==\n", mz,"\n");
	if (!bdIsEqual(mz, cz))
		printf("ERROR: (c.z^e == (m.z)^e (mod n) DOES NOT MATCH m'\n");
	else
		printf("c.z^e == (m.z)^e (mod n) checks OK\n");
	assert(bdIsEqual(mz, cz));


	printf("\nCONVERSIONS WITH HEX AND DECIMAL...\n");
	/* Do some conversions with hex and decimal strings */
	bdSetZero(u);
	bdConvFromHex(u, "DeadBeefCafeBabeBeddedDefacedDeafBeadFacade");
	bdPrintHex("From Hex: ", u,"\n");
	bdConvToHex(u, s, sizeof(s));
	printf("To Hex: %s\n", s);
	bdConvFromDecimal(u, "1234567890123456789012345678901234567890");
	bdPrintHex("From Decimal: ", u,"\n");
	bdConvToDecimal(u, s, sizeof(s));
	printf("To Decimal: %s\n", s);
	cmp = strcmp(s, "1234567890123456789012345678901234567890");
	assert(cmp == 0);

	/* 987654321 x 81 = 80000000001 */
	bdConvFromDecimal(u, "987654321");
	bdShortMult(v, u, 81);
	bdConvToDecimal(v, s, sizeof(s));
	printf("987654321 x 81 = %s\n", s);
	cmp = strcmp(s, "80000000001");
	assert(cmp == 0);

	/* 123456789 x 9 + 10 = 1111111111 */
	bdConvFromDecimal(u, "123456789");
	bdShortMult(v, u, 9);
	bdShortAdd(w, v, 10);
	bdConvToDecimal(w, s, sizeof(s));
	printf("123456789 x 9 + 10 = %s\n", s);
	cmp = strcmp(s, "1111111111");
	assert(cmp == 0);

	/* Convert an empty decimal string to a BIGD */
	bdSetShort(u, 0xfdfdfdfd);
	bdConvFromDecimal(u, "");
	bdPrintHex("bdConvFromDecimal(u, '')=", u,"\n");
	assert(bdIsZero(u));
	/* ditto an empty hex string */
	bdSetShort(u, 0xfdfdfdfd);
	bdConvFromHex(u, "");
	bdPrintHex("bdConvFromHex(u, '')=", u,"\n");
	assert(bdIsZero(u));
	/* Convert zero BIGD value to a decimal string */
	bdSetZero(u);
	bdConvToDecimal(u, s, sizeof(s));
	printf("Decimal zero = '%s'\n", s);
	cmp = strcmp(s, "0");
	assert(cmp == 0);
	/* ditto to a hex string */
	bdConvToHex(u, s, sizeof(s));
	printf("Hex zero = '%s'\n", s);
	cmp = strcmp(s, "0");
	assert(cmp == 0);
	/* Convert a zero BIGD to an array of octets */
	memset(bytes, 0xdf, sizeof(bytes));
	bdSetZero(u);
	nbytes = bdConvToOctets(u, bytes, sizeof(bytes));
	printf("bdConvToOctets returns %d: ", nbytes);
	/* show that all bytes are zero */
	pr_bytesmsg("", bytes, sizeof(bytes));
	assert(0 == bytes[sizeof(bytes)-1] && 0 == bytes[0]);
	/* Convert an zero-length array of octets to a BIGD */
	bdSetShort(u, 0xfdfdfdfd);
	bdConvFromOctets(u, bytes, 0);
	bdPrintHex("bdConvFromOctets(nbytes=0)=", u,"\n");
	assert(bdIsZero(u));

	bdFree(&n);
	printf("\nJACOBI AND LEGENDRE SYMBOLS...\n");
	/* Try computing some Jacobi and Legendre symbol values */
	a = bdNew();
	n = bdNew();
	bdSetShort(a, 158);
	bdSetShort(n, 235);
	jac = bdJacobi(a, n);
	printf("Jacobi(158, 235)=%d\n", jac);
	assert(-1 == jac);
	bdSetShort(a, 2183);
	bdSetShort(n, 9907);
	jac = bdJacobi(a, n);
	printf("Jacobi(2183, 9907)=%d\n", jac);
	assert(1 == jac);
	bdSetShort(a, 1001);
	jac = bdJacobi(a, n);
	printf("Jacobi(1001, 9907)=%d\n", jac);
	assert(-1 == jac);
	bdShortMult(a, n, 10000);
	jac = bdJacobi(a, n);
	printf("Jacobi(10000 * 9907, 9907)=%d\n", jac);
	assert(0 == jac);

	printf("\nSQUARE AND SQUARE ROOT...\n");
	/* Square a random number w = u^2 */
	bdSetRandTest(u, 10);
	bdPrintHex("random u=\n", u, "\n");
	bdSquare(w, u);
	bdPrintHex("(Square) u^2=", w, "\n");
	/* check against product p = u * u */
	bdMultiply(p, u, u);
	bdPrintHex("u*u=", p, "\n");
	assert(bdIsEqual(p, w));
	/* and check against power function */
	bdPower(p, u, 2);
	bdPrintHex("u power 2=", p, "\n");
	assert(bdIsEqual(p, w));
	/* Compute integer square root [new in v2.1] */
	bdSqrt(p, w);
	bdPrintHex("sqrt(u^2)=", p, "\n");
	assert(bdIsEqual(p, u));
	/* Now test sqrt(u^2 - 1) */
	bdDecrement(w);
	bdPrintHex("u^2-1=", w, "\n");
	bdSqrt(r, w);
	bdPrintHex("sqrt(u^2-1)=", r, "\n");
	/* Check difference is 1 */
	bdSubtract(q, p, r);
	bdPrintHex("difference=", q, " (expected 1)\n");
	assert(bdShortCmp(q, 1) == 0);

	printf("\nCUBE AND CUBE ROOT...\n");
	/* Cube a random number w = u^2 */
	bdPrintHex("u=", u, "\n");
	bdSquare(v, u);
	bdMultiply(w, v, u);
	bdPrintHex("(Cube) u^3=", w, "\n");
	/* Check that power function works */
	bdPower(v, u, 0);
	bdPrintHex("u^0=", v, "\n");
	bdPower(v, u, 1);
	bdPrintHex("u^1=", v, "\n");
	bdPower(v, u, 3);
	bdPrintHex("(Power) u^3=", v, "\n");
	assert(bdIsEqual(v, w));
	/* Compute integer cube root (new in [v2.3]) */
	bdCubeRoot(p, w);
	bdPrintHex("cuberoot(u^3)=", p, "\n");
	assert(bdIsEqual(p, u));
	/* Now test cuberoot(u^2 - 1) */
	bdDecrement(w);
	bdPrintHex("u^3-1=", w, "\n");
	bdCubeRoot(r, w);
	bdPrintHex("cuberoot(u^3-1)=", r, "\n");
	/* Check difference is 1 */
	bdSubtract(q, p, r);
	bdPrintHex("difference=", q, " (expected 1)\n");
	assert(bdShortCmp(q, 1) == 0);

	printf("\nQUICK-AND-DIRTY RANDOM NUMBERS (bitlen <= nbits)...\n");
	for (i = 0; i < 5; i++)
	{
		int nbits = 128 + i;
		bdQuickRandBits(a, nbits);
		printf("q-rand(%d)=", nbits);
		bdPrintHex("", a, "");
		printf("\t%d bits\n", bdBitLength(a));
	}

	printf("\nPRINT BITS...\n");
	bdSetShort(u, 0xdeadbeef);
	bdPrintHex("u=0x", u, " => ");
	bdPrintBits("'", u, "'b\n");
	printf("(correct)=       %s\n", "11011110101011011011111011101111");
	bdSetShort(u, 0);
	bdPrintHex("u=0x", u, " => ");
	bdPrintBits("'", u, "'b\n");
	bdSetShort(u, 1);
	bdPrintHex("u=0x", u, " => ");
	bdPrintBits("'", u, "'b\n");

	printf("\nMODULAR SQUARE ROOT...\n");
	printf("Select a 192-bit prime number...\n");
	bdConvFromHex(p, "fffffffffffffffffffffffffffffffeffffffffffffffff");
	bdPrintHex("p=0x", p, "\n");
	bdPrintDecimal("p=", p, "\n");
	res = bdIsPrime(p, 50);
	printf("bdIsPrime(p)=%d\n", res);
	assert(r != 0);
	nbits = bdBitLength(p);
	printf("p is %d bits\n", nbits);
	// Pick a number just smaller than p
	bdQuickRandBits(u, nbits - 1);
	bdPrintDecimal("u=               ", u, "\n");
	// Square it
	bdModSquare(w, u, p);
	bdPrintDecimal("w=u^2(mod p)=    ", w, "\n");
	/* Check we have a quadratic residue */
	jac = bdJacobi(w, p);
	printf("Legendre symbol (w|p)=%d (expected 1)\n", jac);
	assert(jac == 1);
	/* Compute one modular square root */
	bdModSqrt(x, w, p);
	bdPrintDecimal("x=sqrt(w)(mod p)=", x, "\n");
	/* and the other */
	bdSubtract(y, p, x);
	bdPrintDecimal("other root y=p-x=", y, "\n");
	assert(bdIsEqual(x, u) || bdIsEqual(y, u));
	if (bdIsEqual(x, u)) {
		printf("u == x => OK\n");
	}
	else {
		printf("u == p-x => OK\n");
	}

	printf("\nFind a number that is not a quadratic residue mod p...\n");
	do {
		bdQuickRandBits(v, nbits - 1);
	} while (bdJacobi(v, p) != -1);
	bdPrintDecimal("v=", v, "\n");
	printf("Legendre symbol (v|p)=%d (expected -1)\n", bdJacobi(v, p));
	res = bdModSqrt(x, v, p);
	printf("bdModSqrt(v) returns %d (-1 => square root does not exist)\n", res);
	assert(res != 0);

	printf("\nADD AND SUBTRACT MODULO M ...\n");
	/* Add two random numbers w = u + v mod p */
	bdQuickRandBits(u, nbits - 1);
	bdQuickRandBits(v, nbits - 1);
	bdPrintDecimal("u=           ", u, "\n");
	bdPrintDecimal("v=           ", v, "\n");
	bdModAdd(w, u, v, p);
	bdPrintDecimal("w=u+v(mod p)=", w, "\n");

	/* y = w - v */
	bdModSub(y, w, v, p);
	bdPrintDecimal("y=w-v(mod p)=", y, "\n");
	/* Check y == u */
	printf("y == u => %s\n", (bdIsEqual(y, u) ? "OK" : "ERROR"));
	assert(bdIsEqual(y, u));

	printf("\nDivide an integer by 2 modulo p using bdModHalve()...\n");
	bdSetEqual(w, u);
	bdPrintDecimal("w=           ", u, "\n");
	bdModHalve(w, w, p);
	bdPrintDecimal("w=w/2(mod p)=", w, "\n");
	// Check result
	bdModAdd(v, w, w, p);
	bdPrintDecimal("v=w+w(mod p)=", v, "\n");
	printf("v == u => %s\n", (bdIsEqual(v, u) ? "OK" : "ERROR"));
	assert(bdIsEqual(v, u));


	/* Finally, show the current version number */
	printf("\nVERSION=%d\n", bdVersion());
	printf("Compiled on [%s]\n", bdCompileTime());


	/* Clear up */

	bdFree(&a);
	bdFree(&n);
	bdFree(&e);
	bdFree(&m);
	bdFree(&c);
	bdFree(&z);
	bdFree(&mz);
	bdFree(&cz);
	bdFree(&t);


	/* Old method... */
	//bdFree(&u);
	//bdFree(&v);
	//bdFree(&w);
	//bdFree(&q);
	//bdFree(&r);
	//bdFree(&p);
	//bdFree(&x);
	//bdFree(&y);

	/* New way in [v2.6] */
	bdFreeVars(&u, &v, &w, &p, &q, &r, &x, &y);
	printf("OK, successfully completed tests.\n");

	return 0;
}

