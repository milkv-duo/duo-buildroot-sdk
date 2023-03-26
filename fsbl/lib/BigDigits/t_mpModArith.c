/* $Id: t_mpModArith.c $ */

/* Some more tests of the BigDigits "mp" functions. */

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


#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "bigdigits.h"



/** Hard-coded maximum number of bits we choose to handle */
#define EC_MAXBITS 640
/** Number of digits needed to represent an integer of EC_MAXBITS bits */
#define EC_NDIGITS (EC_MAXBITS/BITS_PER_DIGIT)

/** Unsigned integer of EC_MAXBITS bits */
typedef DIGIT_T EC_INT[EC_NDIGITS];

/* This module can be compiled with the global preprocessor definitions
	MSVC: /D "NO_ALLOCS" /D "MAX_FIXED_BIT_LENGTH=640"
	GCC:  -D NO_ALLOCS -D MAX_FIXED_BIT_LENGTH=640
*/


/** Make a random number of up to `nbits` bits */
size_t make_random(EC_INT a, size_t nbits)
{
	 
	size_t n;

	/* Half the time, pick a shorter bitlength at random from [6,nbits] */
	if (spSimpleRand(0, 1)) {
		n = (size_t)spSimpleRand(6, (DIGIT_T)nbits);
	}
	else {
		n = nbits;
	}

	mpQuickRandBits(a, EC_NDIGITS, n);

	return n;
}


int main(void)
{
	/* Note the neat way to declare variables of EC_NDIGITS */
	EC_INT u, v, w, m, p, x, y;
	size_t nbits;
	int r;

	/*	Force linker to include copyright notice in 
		executable object image
	*/
	copyright_notice();
		

	printf("Testing BIGDIGITS 'mp' modular arithmetic functions.\n");

	// First show it works for small numbers...
	mpSetDigit(u, 17, EC_NDIGITS);
	mpSetDigit(v, 13, EC_NDIGITS);
	mpSetDigit(m, 25, EC_NDIGITS);
	mpPrintDecimal("u=", u, EC_NDIGITS, ", ");
	mpPrintDecimal("v=", v, EC_NDIGITS, ", ");
	mpPrintDecimal("m=", m, EC_NDIGITS, "\n");
	mpModAdd(w, u, v, m, EC_NDIGITS);
	mpPrintDecimal("w=u+v(mod m)=", w, EC_NDIGITS, "\n");
	/* Check that w == 5 */
	assert(mpShortCmp(w, 5, EC_NDIGITS) == 0);

	// Add assign w+=u [NB (w,w,u) not (w,u,w)]
	mpModAdd(w, w, u, m, EC_NDIGITS);
	mpPrintDecimal("w+=u(mod m)=", w, EC_NDIGITS, "\n");
	// Subtract assign w-=u
	mpModSub(w, w, u, m, EC_NDIGITS);
	mpPrintDecimal("w-=u(mod m)=", w, EC_NDIGITS, "\n");

	// Subtract modulo m
	mpModSub(w, v, u, m, EC_NDIGITS);
	mpPrintDecimal("w=v-u(mod m)=", w, EC_NDIGITS, "\n");
	mpModSub(w, u, v, m, EC_NDIGITS);
	mpPrintDecimal("w=u-v(mod m)=", w, EC_NDIGITS, "\n");

	// Add then subtract modulo m
	mpModAdd(w, u, v, m, EC_NDIGITS);
	mpPrintDecimal("w=u+v(mod m)=", w, EC_NDIGITS, "\n");
	/* y = w - v */
	mpModSub(y, w, v, m, EC_NDIGITS);
	mpPrintDecimal("y=w-v(mod m)=", y, EC_NDIGITS, "\n");
	/* Check y == u */
	printf("y == u => %s\n", (mpEqual(y, u, EC_NDIGITS) ? "OK" : "ERROR"));
	assert(mpEqual(y, u, EC_NDIGITS));



	printf("Select a 192-bit prime number...\n");
	mpConvFromHex(p, EC_NDIGITS, "fffffffffffffffffffffffffffffffeffffffffffffffff");
	mpPrintHex("p=0x", p, EC_NDIGITS, "\n");
	mpPrintDecimal("p=", p, EC_NDIGITS, "\n");
	r = mpIsPrime(p, EC_NDIGITS, 50);
	printf("mpIsPrime(p)=%d\n", r);
	assert(r != 0);

	printf("Find the square root of a quadratic residue modulo p ...\n");
	/* Pick a number, any number, less than p */
	nbits = mpBitLength(p, EC_NDIGITS);
	make_random(u, nbits - 1);
	mpPrintDecimal("u=", u, EC_NDIGITS, "\n");
	/* Square it modulo p => a quadratic residue modulo p */
	mpModSquare(w, u, p, EC_NDIGITS);
	mpPrintDecimal("w=u^2(mod p)=", w, EC_NDIGITS, "\n");
	/* Check we have a quadratic residue */
	r = mpJacobi(w, p, EC_NDIGITS);
	printf("Legendre symbol (w|p)=%d (expected 1)\n", r);
	assert(r == 1);
	/* Compute one modular square root */
	mpModSqrt(x, w, p, EC_NDIGITS);
	mpPrintDecimal("x=sqrt(w)(mod p)=", x, EC_NDIGITS, "\n");
	/* and the other */
	mpSubtract(y, p, x, EC_NDIGITS);
	mpPrintDecimal("y=p-x=", y, EC_NDIGITS, "\n");
	/* One of x or y is the same as u */
	assert(mpEqual(x, u, EC_NDIGITS) || mpEqual(y, u, EC_NDIGITS));
	if (mpEqual(x, u, EC_NDIGITS)) {
		printf("u == x\n");
	}
	else {
		printf("u == p-x\n");
	}

	printf("Find a number that is not a quadratic residue mod p...\n");
	do {
		make_random(v, nbits);
	} while (mpJacobi(v, p, EC_NDIGITS) != -1);
	mpPrintDecimal("v=", v, EC_NDIGITS, "\n");
	printf("Legendre symbol (v|p)=%d (expected -1)\n", mpJacobi(v, p, EC_NDIGITS));
	r = mpModSqrt(x, v, p, EC_NDIGITS);
	printf("mpModSqrt(v) returns %d (-1 => square root does not exist)\n", r);
	assert(r != 0);


	printf("\nAdd and subtract modulo p...\n");
	/* Add two random numbers w = u + v (mod p) */
	make_random(u, nbits - 1);
	make_random(v, nbits - 1);
	mpPrintDecimal("u=", u, EC_NDIGITS, "\n");
	mpPrintDecimal("v=", v, EC_NDIGITS, "\n");
	mpModAdd(w, u, v, p, EC_NDIGITS);
	mpPrintDecimal("w=u+v(mod p)=", w, EC_NDIGITS, "\n");
	/* y = w - v (mod p) */
	mpModSub(y, w, v, p, EC_NDIGITS);
	mpPrintDecimal("y=w-v(mod p)=", y, EC_NDIGITS, "\n");
	/* Check y == u */
	printf("y == u => %s\n", (mpEqual(y, u, EC_NDIGITS) ? "OK" : "ERROR"));
	assert(mpEqual(y, u, EC_NDIGITS));

	printf("Do 'add assign' and 'subtract assign'...\n");
	mpSetEqual(w, u, EC_NDIGITS);
	mpPrintDecimal("y=          ", w, EC_NDIGITS, "\n");
	mpPrintDecimal("w=y=        ", w, EC_NDIGITS, "\n");
	mpPrintDecimal("v=          ", v, EC_NDIGITS, "\n");
	/* Now "add assign" w += v (mod p) */
	mpModAdd(w, w, v, p, EC_NDIGITS);
	mpPrintDecimal("w+=v(mod p)=", w, EC_NDIGITS, "\n");
	/* and  "subtract assign" w -= v (mod p) */
	mpModSub(w, w, v, p, EC_NDIGITS);
	mpPrintDecimal("w-=v(mod p)=", w, EC_NDIGITS, "\n");
	/* Check w == y */
	printf("w == y => %s\n", (mpEqual(w, y, EC_NDIGITS) ? "OK" : "ERROR"));
	assert(mpEqual(w, y, EC_NDIGITS));

	printf("\nDivide an integer by 2 modulo p - quick method...\n");
	mpPrintDecimal("u=           ", u, EC_NDIGITS, "\n");
	mpModHalve(w, u, p, EC_NDIGITS);
	mpPrintDecimal("w=u/2(mod p)=", w, EC_NDIGITS, "\n");
	// Check result
	mpModAdd(v, w, w, p, EC_NDIGITS);
	mpPrintDecimal("v=w*2(mod p)=", v, EC_NDIGITS, "\n");
	printf("v == u => %s\n", (mpEqual(v, u, EC_NDIGITS) ? "OK" : "ERROR"));
	assert(mpEqual(v, u, EC_NDIGITS));


	/* DEMONSTRATE "QUICK" MODULO REDUCTION FOR SPECIAL CASE */
	printf("\nSpecial case modulo reduction...\n");
	/* Compute v = u mod p for u in range 0 <= u < 4p */
	mpPrintHex("p=", p, EC_NDIGITS, "\n");
	nbits = mpBitLength(p, EC_NDIGITS);
	printf("p is %d bits\n", nbits);
	// Generate a random number approx 2-4x bigger than p
	mpQuickRandBits(u, EC_NDIGITS, nbits + 2);
	mpPrintHex("u=", u, EC_NDIGITS, "\n");
	printf("u is %d bits\n", mpBitLength(u, EC_NDIGITS));
	// Compute v = u mod p for this special case
	mpModSpecial(v, u, p, EC_NDIGITS);
	mpPrintHex("v=u(mod p)=", v, EC_NDIGITS, "\n");
	// Check that result v < p
	assert(mpCompare(v, p, EC_NDIGITS) < 0);
	// And check again using the more expensive mpModulo fn
	mpModulo(w, u, EC_NDIGITS, p, EC_NDIGITS);
	mpPrintHex("mpModulo()=", w, EC_NDIGITS, "\n");
	assert(mpEqual(v, w, EC_NDIGITS));


	printf("\nDemonstrate mpPrintBits():\n");
	mpSetDigit(u, 0xef, EC_NDIGITS);
	mpPrintHex("u=0x", u, EC_NDIGITS, " => ");
	mpPrintBits("'", u, EC_NDIGITS, "'b\n");

	mpSetDigit(u, 0x28, EC_NDIGITS);
	mpPrintHex("u=0x", u, EC_NDIGITS, " => ");
	mpPrintBits("'", u, EC_NDIGITS, "'b\n");

	mpSetDigit(u, 0, EC_NDIGITS);
	mpPrintHex("u=0x", u, EC_NDIGITS, " => ");
	mpPrintBits("'", u, EC_NDIGITS, "'b\n");

	mpSetDigit(u, 0xdeadbeef, EC_NDIGITS);
	mpPrintHex("u=0x", u, EC_NDIGITS, " => ");
	mpPrintBits("'", u, EC_NDIGITS, "'b\n");
	printf("(correct)=       %s\n", "11011110101011011011111011101111");

	mpConvFromHex(u, EC_NDIGITS, "04deadbeefcafebabe1");
	mpPrintHex("u=0x", u, EC_NDIGITS, " => ");
	mpPrintBits("\n'", u, EC_NDIGITS, "'b\n");

	mpPrintHex("p=0x", p, EC_NDIGITS, " => ");
	mpPrintBits("\n'", p, EC_NDIGITS, "'b\n");


	/* Display version number */
	printf("\nVERSION=%d\n", mpVersion());
	printf("Compiled on [%s]\n", mpCompileTime());

	printf("OK, successfully completed tests.\n");
	
	return 0;
}

