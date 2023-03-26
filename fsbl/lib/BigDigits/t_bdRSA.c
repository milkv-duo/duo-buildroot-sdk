/* $Id: t_bdRSA.c $ */

/* Test BigDigits "bd" functions using a new RSA key and random data */

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


#if _MSC_VER >= 1100
	/* Detect memory leaks in MSVC++ */ 
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "bigd.h"

static int my_rand(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen)
/* Our own (very insecure) random generator func using good old rand() 
   but in the required format for BD_RANDFUNC
   -- replace this in practice with your own cryptographically-secure function
   -- or use bdRandomOctets() in bigdRand.h
*/
{
	unsigned int myseed;
	size_t i;
	int offset;

	/* Use time for 32-bit seed - then blend in user-supplied seed, if any */
	myseed = (unsigned)time(NULL) ^ (unsigned)clock();
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


#define give_a_sign(c) putchar((c))

static bdigit_t SMALL_PRIMES[] = {
	3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 
	47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 
	103, 107, 109, 113,
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
	179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
	233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
	283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
	353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
	419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
	467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
	547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
	607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
	661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
	739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
	811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
	877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
	947, 953, 967, 971, 977, 983, 991, 997,
};
#define N_SMALL_PRIMES (sizeof(SMALL_PRIMES)/sizeof(bdigit_t))

int generateRSAPrime(BIGD p, size_t nbits, bdigit_t e, size_t ntests, 
				 const unsigned char *seed, size_t seedlen, BD_RANDFUNC randFunc)
/* Create a prime p such that gcd(p-1, e) = 1.
   Returns # prime tests carried out or -1 if failed.
   Sets the TWO highest bits to ensure that the 
   product pq will always have its high bit set.
   e MUST be a prime > 2.
   This function assumes that e is prime so we can
   do the less expensive test p mod e != 1 instead
   of gcd(p-1, e) == 1.
   Uses improvement in trial division from Menezes 4.51.
  */
{
	BIGD u;
	size_t i, j, iloop, maxloops, maxodd;
	int done, overflow, failedtrial;
	int count = 0;
	bdigit_t r[N_SMALL_PRIMES];

	/* Create a temp */
	u = bdNew();

	maxodd = nbits * 100;
	maxloops = 5;

	done = 0;
	for (iloop = 0; !done && iloop < maxloops; iloop++)
	{
		/* Set candidate n0 as random odd number */
		bdRandomSeeded(p, nbits, seed, seedlen, randFunc);
		/* Set two highest and low bits */
		bdSetBit(p, nbits - 1, 1);
		bdSetBit(p, nbits - 2, 1);
		bdSetBit(p, 0, 1);

		/* To improve trial division, compute table R[q] = n0 mod q
		   for each odd prime q <= B
		*/
		for (i = 0; i < N_SMALL_PRIMES; i++)
		{
			r[i] = bdShortMod(u, p, SMALL_PRIMES[i]);
		}

		done = overflow = 0;
		/* Try every odd number n0, n0+2, n0+4,... until we succeed */
		for (j = 0; j < maxodd; j++, overflow = bdShortAdd(p, p, 2))
		{
			/* Check for overflow */
			if (overflow)
				break;

			give_a_sign('.');
			count++;

			/* Each time 2 is added to the current candidate
			   update table R[q] = (R[q] + 2) mod q */
			if (j > 0)
			{
				for (i = 0; i < N_SMALL_PRIMES; i++)
				{
					r[i] = (r[i] + 2) % SMALL_PRIMES[i];
				}
			}

			/* Candidate passes the trial division stage if and only if
			   NONE of the R[q] values equal zero */
			for (failedtrial = 0, i = 0; i < N_SMALL_PRIMES; i++)
			{
				if (r[i] == 0)
				{
					failedtrial = 1;
					break;
				}
			}
			if (failedtrial)
				continue;

			/* If p mod e = 1 then gcd(p, e) > 1, so try again */
			bdShortMod(u, p, e);
			if (bdShortCmp(u, 1) == 0)
				continue;

			/* Do expensive primality test */
			give_a_sign('*');
			if (bdRabinMiller(p, ntests))
			{	/* Success! - we have a prime */
				done = 1;
				break;
			}

		}
	}
	

	/* Clear up */
	bdFree(&u);
	printf("\n");

	return (done ? count : -1);
}

int generateRSAKey(BIGD n, BIGD e, BIGD d, BIGD p, BIGD q, BIGD dP, BIGD dQ, BIGD qInv,
	size_t nbits, bdigit_t ee, size_t ntests, unsigned char *seed, size_t seedlen, 
	BD_RANDFUNC randFunc)
{
	BIGD g, p1, q1, phi;
	size_t np, nq;
	unsigned char *myseed = NULL;
	clock_t start, finish;
	double duration, tmake;
	long ptests;
	int res;

	/* Initialise */
	g = bdNew();
	p1 = bdNew();
	q1 = bdNew();
	phi = bdNew();

	printf("Generating a %d-bit RSA key...\n", nbits);
	
	/* We add an extra byte to the user-supplied seed */
	myseed = malloc(seedlen + 1);
	if (!myseed) return -1;
	memcpy(myseed, seed, seedlen);

	/* Do (p, q) in two halves, approx equal */
	nq = nbits / 2 ;
	np = nbits - nq;

	/* Make sure seeds are slightly different for p and q */
	myseed[seedlen] = 0x01;
	start = clock();
	res = generateRSAPrime(p, np, ee, ntests, myseed, seedlen+1, randFunc);
	finish = clock();
	bdPrintHex("p=", p, "\n");
	assert(res > 0);
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("generateRSAPrime took %.3f secs and %d prime candidates (%.4f s/test)\n", duration, res, duration / res); 
	ptests = res;
	tmake = duration;
	printf("p is %d bits\n", bdBitLength(p));

	myseed[seedlen] = 0xff;
	start = clock();
	res = generateRSAPrime(q, nq, ee, ntests, myseed, seedlen+1, randFunc);
	finish = clock();
	bdPrintHex("q=", q, "\n");
	assert(res > 0);
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("generateRSAPrime took %.3f secs and %d prime candidates (%.4f s/test)\n", duration, res, duration / res); 
	ptests += res;
	tmake += duration;
	printf("q is %d bits\n", bdBitLength(q));
	/* Check that p != q (if so, RNG is faulty!) */
	assert(!bdIsEqual(p, q));

	bdSetShort(e, ee);
	bdPrintHex("e=", e, "\n");

	/* If q > p swap p and q so p > q */
	if (bdCompare(p, q) < 1)
	{	
		bdSetEqual(g, p);
		bdSetEqual(p, q);
		bdSetEqual(q, g);
	}

	/* Calc p-1 and q-1 */
	bdSetEqual(p1, p);
	bdDecrement(p1);
	bdPrintHex("p-1=\n", p1, "\n");
	bdSetEqual(q1, q);
	bdDecrement(q1);
	bdPrintHex("q-1=\n", q1, "\n");

	/* Check gcd(p-1, e) = 1 */
	bdGcd(g, p1, e);
	bdPrintHex("gcd(p-1,e)=", g, "\n");
	assert(bdShortCmp(g, 1) == 0);
	bdGcd(g, q1, e);
	bdPrintHex("gcd(q-1,e)=", g, "\n");
	assert(bdShortCmp(g, 1) == 0);

	/* Compute n = pq */
	bdMultiply(n, p, q);
	bdPrintHex("n=\n", n, "\n");

	/* Compute d = e^-1 mod (p-1)(q-1) */
	bdMultiply(phi, p1, q1);
	bdPrintHex("phi=\n", phi, "\n");
	res = bdModInv(d, e, phi);
	assert(res == 0);
	bdPrintHex("d=\n", d, "\n");

	/* Check ed = 1 mod phi */
	bdModMult(g, e, d, phi);
	bdPrintHex("ed mod phi=", g, "\n");
	assert(bdShortCmp(g, 1) == 0);

	/* Calculate CRT key values */
	printf("CRT values:\n");
	bdModInv(dP, e, p1);
	bdModInv(dQ, e, q1);
	bdModInv(qInv, q, p);
	bdPrintHex("dP=", dP, "\n");
	bdPrintHex("dQ=", dQ, "\n");
	bdPrintHex("qInv=", qInv, "\n");

	printf("\nTime to create key = %.3f secs with %ld prime candidates (%.4f s/test)\n\n", tmake, ptests, tmake / ptests);
	printf("n is %d bits\n", bdBitLength(n));

	/* Clean up */
	if (myseed) free(myseed);
	bdFree(&g);
	bdFree(&p1);
	bdFree(&q1);
	bdFree(&phi);

	return 0;
}

static int debug = 0;

int main(void)
{
	size_t nbits = 1025;	/* (use an odd modulus size to see if it breaks anything!) */
	unsigned ee = 0x3;
	size_t ntests = 50;
	unsigned char *seed = NULL;
	size_t seedlen = 0;

	BIGD n, e, d, p, q, dP, dQ, qInv;
	BIGD m, c, s, hq, h, m1, m2; 
	int res;
	clock_t start, finish;
	double tinv, tcrt;

/* MSVC memory leak checking stuff */
#if _MSC_VER >= 1100
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
#endif

	printf("Test BIGDIGITS with a new %d-bit RSA key and random data.\n", nbits);

	/* Initialise */
	p = bdNew();
	q = bdNew();
	n = bdNew();
	e = bdNew();
	d = bdNew();
	dP= bdNew();
	dQ= bdNew();
	qInv= bdNew();
	m = bdNew();
	c = bdNew();
	s = bdNew();
	m1 = bdNew();
	m2 = bdNew();
	h = bdNew();
	hq = bdNew();

	/* Create RSA key pair (n, e),(d, p, q, dP, dQ, qInv) */
	/* NB we use simple my_rand() here -- you should use a proper cryptographically-secure RNG */
	res = generateRSAKey(n, e, d, p, q, dP, dQ, qInv, nbits, ee, ntests, seed, seedlen, my_rand);
	
	if (res != 0)
	{
		printf("Failed to generate RSA key!\n");
		goto clean_up;
	}

	/* Set a random message m < n */
	bdRandomSeeded(m, bdBitLength(n)-1, NULL, 0, my_rand);
	bdPrintHex("m=\n", m, "\n");

	/* Encrypt c = m^e mod n */
	bdModExp(c, m, e, n);
	bdPrintHex("c=\n", c, "\n");

	/* Check decrypt m1 = c^d mod n */
	start = clock();
	bdModExp(m1, c, d, n);
	finish = clock();
	tinv = (double)(finish - start) / CLOCKS_PER_SEC;
	bdPrintHex("m'=\n", m1, "\n");
	res = bdCompare(m1, m);
	printf("Decryption %s\n", (res == 0 ? "OK" : "FAILED!"));
	assert(res == 0);
	printf("Decrypt by inversion took %.3f secs\n", tinv);

	/* Sign s = m^d mod n */
	bdModExp(s, m, d, n);
	bdPrintHex("s=\n", s, "\n");

	/* Check verify m1 = s^e mod n */
	bdModExp(m1, s, e, n);
	bdPrintHex("m'=\n", m1, "\n");
	res = bdCompare(m1, m);
	printf("Verification %s\n", (res == 0 ? "OK" : "FAILED!"));
	assert(res == 0);

	/* Decrypt using CRT method - Ref: PKCS #1 */
	bdPrintHex("m=", m, "\n");
	bdPrintHex("c=", c, "\n");
	bdPrintHex("p=", p, "\n");
	bdPrintHex("q=", q, "\n");
	start = clock();
	/* Let m_1 = c^dP mod p. */
	bdModExp(m1, c, dP, p);
	if(debug)bdPrintHex("m_1=c^dP mod p=", m1, "\n");
	/* Let m_2 = c^dQ mod q. */
	bdModExp(m2, c, dQ, q);
	if(debug)bdPrintHex("m_2=c^dQ mod q=", m2, "\n");
	if (bdCompare(m1, m2) < 0)
		bdAdd(m1, m1, p);
	bdSubtract(m1, m1, m2);
	if(debug)bdPrintHex("m_1 - m_2=", m1, "\n");
	/* Let h = qInv ( m_1 - m_2 ) mod p. */
	bdModMult(h, qInv, m1, p);
	if(debug)bdPrintHex("h=qInv(m1-m2) mod p=", h, "\n");
	bdMultiply(hq, h, q);
	if(debug)bdPrintHex("hq=", hq, "\n");
	/* Let m = m_2 + hq. */
	bdAdd(m1, m2, hq);
	finish = clock();
	tcrt = (double)(finish - start) / CLOCKS_PER_SEC;
	if(debug)bdPrintHex("m'=m_2 + hq=", m1, "\n");
	bdPrintHex("(CRT)m'=\n", m1, "\n");
	res = bdCompare(m1, m);
	printf("CRT Decryption %s\n", (res == 0 ? "OK" : "FAILED!"));
	assert(res == 0);
	printf("Decrypt by CRT took %.3f secs\n", tcrt);
	printf("c.f. Decrypt by inversion %.3f secs (factor = %.1f)\n", 
		tinv, (tcrt ? tinv / tcrt : 0));
	printf("n is %d bits\n", bdBitLength(n));

	/* Clean up */
clean_up:
	bdFree(&n);
	bdFree(&e);
	bdFree(&d);
	bdFree(&p);
	bdFree(&q);
	bdFree(&dP);
	bdFree(&dQ);
	bdFree(&qInv);
	bdFree(&m);
	bdFree(&c);
	bdFree(&s);
	bdFree(&m1);
	bdFree(&m2);
	bdFree(&h);
	bdFree(&hq);

	printf("OK, successfully completed tests.\n");

	return 0;
}
