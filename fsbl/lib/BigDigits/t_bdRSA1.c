/* $Id: t_bdRSA1.c $ */

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

/* Test BigDigits "bd" functions using a new RSA key and user input plaintext 

   NOTE: this uses a different algorithm to generate the random number (a better one,
   still not quite cryptographically secure, but much better than using rand()).
   It also uses a more convenient key length of 1024 which is an exact multiple of 8.
   The "message" to be encrypted is accepted from the command-line or defaults to "abc".
   The message length is restricted to 117 bytes by the size of the key (|n|-11).
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
#include "bigdRand.h"


static void pr_bytesmsg(char *msg, unsigned char *bytes, size_t len)
{
	size_t i;
	printf("%s", msg);
	for (i = 0; i < len; i++)
		printf("%02x", bytes[i]);
	printf("\n");
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
	int res;

	/* Initialise */
	g = bdNew();
	p1 = bdNew();
	q1 = bdNew();
	phi = bdNew();

	printf("Generating a %d-bit RSA key...\n", nbits);
	
	/* Set e as a BigDigit from short value ee */
	bdSetShort(e, ee);
	bdPrintHex("e=", e, "\n");

	/* We add an extra byte to the user-supplied seed */
	myseed = malloc(seedlen + 1);
	if (!myseed) return -1;
	memcpy(myseed, seed, seedlen);
	/* Make sure seeds are slightly different for p and q */
	myseed[seedlen] = 0x01;

	/* Do (p, q) in two halves, approx equal */
	nq = nbits / 2 ;
	np = nbits - nq;

	/* Compute two primes of required length with p mod e > 1 and *second* highest bit set */
	start = clock();
	do {
		bdGeneratePrime(p, np, ntests, myseed, seedlen+1, randFunc);
		bdPrintHex("Try p=", p, "\n");
	} while ((bdShortMod(g, p, ee) == 1) || bdGetBit(p, np-2) == 0);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	tmake = duration;
	printf("p is %d bits, bit(%d) is %d\n", bdBitLength(p), np-2, bdGetBit(p,np-2));

	myseed[seedlen] = 0xff;
	start = clock();
	do {
		bdGeneratePrime(q, nq, ntests, myseed, seedlen+1, randFunc);
		bdPrintHex("Try q=", q, "\n");
	} while ((bdShortMod(g, q, ee) == 1) || bdGetBit(q, nq-2) == 0);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	tmake += duration;
	printf("q is %d bits, bit(%d) is %d\n", bdBitLength(q), nq-2, bdGetBit(q,nq-2));
	printf("Prime generation took %.3f secs\n", duration); 

	/* Compute n = pq */
	bdMultiply(n, p, q);
	bdPrintHex("n=\n", n, "\n");
	printf("n is %d bits\n", bdBitLength(n));
	assert(bdBitLength(n) == nbits);

	/* Check that p != q (if so, RNG is faulty!) */
	assert(!bdIsEqual(p, q));

	/* If q > p swap p and q so p > q */
	if (bdCompare(p, q) < 1)
	{	
		printf("Swopping p and q so p > q...\n");
		bdSetEqual(g, p);
		bdSetEqual(p, q);
		bdSetEqual(q, g);
	}
	bdPrintHex("p=", p, "\n");
	bdPrintHex("q=", q, "\n");

	/* Calc p-1 and q-1 */
	bdSetEqual(p1, p);
	bdDecrement(p1);
	bdPrintHex("p-1=\n", p1, "\n");
	bdSetEqual(q1, q);
	bdDecrement(q1);
	bdPrintHex("q-1=\n", q1, "\n");

	/* Compute phi = (p-1)(q-1) */
	bdMultiply(phi, p1, q1);
	bdPrintHex("phi=\n", phi, "\n");

	/* Check gcd(phi, e) == 1 */
	bdGcd(g, phi, e);
	bdPrintHex("gcd(phi,e)=", g, "\n");
	assert(bdShortCmp(g, 1) == 0);

	/* Compute inverse of e modulo phi: d = 1/e mod (p-1)(q-1) */
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

	printf("n is %d bits\n", bdBitLength(n));

	/* Clean up */
	if (myseed) free(myseed);
	bdFree(&g);
	bdFree(&p1);
	bdFree(&q1);
	bdFree(&phi);

	return 0;
}

#define KEYSIZE 1024

static int debug = 0;

int main(int argc, char *argv[])
{
	size_t nbits = KEYSIZE;	/* NB a multiple of 8 here */
	int klen, mlen;
	int npad, i;
	unsigned char rb;
	unsigned char block[(KEYSIZE+7)/8];
	unsigned ee = 0x3;
	size_t ntests = 50;
	unsigned char *seed = NULL;
	size_t seedlen = 0;
	char msgstr[sizeof(block)];
	int nchars;
	unsigned char *pmsg = "abc";	/* Default message */

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

	if (argc > 1)
	{	/* Use message supplied in command line */
		pmsg = (unsigned char*)argv[1];
	}

	printf("Test BIGDIGITS with a new %d-bit RSA key and given message data.\n", nbits);

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
	/* NB you should use a proper cryptographically-secure RNG */
	res = generateRSAKey(n, e, d, p, q, dP, dQ, qInv, nbits, ee, ntests, seed, seedlen, bdRandomOctets);
	
	if (res != 0)
	{
		printf("Failed to generate RSA key!\n");
		goto clean_up;
	}

	/* Create a PKCS#1 v1.5 EME message block in octet format */
	/*
	|<-----------------(klen bytes)--------------->|
	+--+--+-------+--+-----------------------------+
	|00|02|PADDING|00|      DATA TO ENCRYPT        |
	+--+--+-------+--+-----------------------------+
	The padding is made up of _at least_ eight non-zero random bytes.
	*/

	/* How big is the key in octets (8-bit bytes)? */
	klen = (nbits + 7) / 8;

	/* CAUTION: make sure the block is at least klen bytes long */
	memset(block, 0, klen);
	mlen = strlen(pmsg);
	npad = klen - mlen - 3;
	if (npad < 8)	/* Note npad is a signed int, not a size_t */
	{
		printf("Message is too long\n");
		exit(1);
	}
	/* Display */
	printf("Message='%s' ", pmsg);
	pr_bytesmsg("0x", pmsg, strlen((char*)pmsg));

	/* Create encryption block */
	block[0] = 0x00;
	block[1] = 0x02;
	/* Generate npad non-zero padding bytes - rand() is OK */
	srand((unsigned)time(NULL));
	for (i = 0; i < npad; i++)
	{
		while ((rb = (rand() & 0xFF)) == 0)
			;/* loop until non-zero*/
		block[i+2] = rb;
	}
	block[npad+2] = 0x00;
	memcpy(&block[npad+3], pmsg, mlen);

	/* Convert to BIGD format */
	bdConvFromOctets(m, block, klen);


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

	/* Extract the message bytes from the decrypted block */
	memset(block, 0, klen);
	bdConvToOctets(m, block, klen);
	assert(block[0] == 0x00);
	assert(block[1] == 0x02);
	for (i = 2; i < klen; i++)
	{	/* Look for zero separating byte */
		if (block[i] == 0x00)
			break;
	}
	if (i >= klen)
		printf("ERROR: failed to find message in decrypted block\n");
	else
	{
		nchars = klen - i - 1;
		memcpy(msgstr, &block[i+1], nchars);
		msgstr[nchars] = '\0';
		printf("Decrypted message is '%s'\n", msgstr);
	}

	/* Sign s = m^d mod n (NB m is not a valid PKCS-v1_5 signature block) */
	bdModExp(s, m, d, n);
	bdPrintHex("s=\n", s, "\n");

	/* Continue as original t_bdRSA ... */

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

	/* Show the current version number */
	printf("Version=%d\n", bdVersion());

	printf("OK, successfully completed tests.\n");

	return 0;
}
