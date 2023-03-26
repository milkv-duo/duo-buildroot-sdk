/* $Id: t_bdRSA_blinded.c $ */

/* Carry out RSA calculations using constant-time exponentiation and blinding */

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


#if _MSC_VER >= 1100
	/* Detect memory leaks in MSVC++ */ 
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "bigd.h"
#include "bigdRand.h"


int main(void)
{
	size_t nbits;
	BIGD N, e, d;
	BIGD m, c, r, g, R, S, x, z, y;
	int isequal;
	size_t i;
	clock_t start, finish;
	double tinv, tinv_ct, tblind;
	int b;
	const size_t ntimes = 3;

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

	printf("Carry out RSA calculation using constant-time exponentiation and blinding.\n");

	/* Initialise */
	bdNewVars(&N, &e, &d, &m, &c, &y, &r, &g, &R, &S, &x, &z);

	/* Read in public and private keys as BIGDs */
	/*
	Source: Alice's 1024-bit RSA key from [RFC4134]:
	Hoffman, P., Ed., "Examples of S/MIME Messages", RFC 4134, July 2005.
	*/

	bdConvFromHex(N, "E08973398DD8F5F5E88776397F4EB005BB5383DE0FB7ABDC7DC775290D052E6D12DFA68626D4D26FAA5829FC97ECFA82510F3080BEB1509E4644F12CBBD832CFC6686F07D9B060ACBEEE34096A13F5F7050593DF5EBA3556D961FF197FC981E6F86CEA874070EFAC6D2C749F2DFA553AB9997702A648528C4EF357385774575F");
	bdConvFromHex(e, "010001");
	bdConvFromHex(d, "A403C327477634346CA686B57949014B2E8AD2C862B2C7D748096A8B91F736F275D6E8CD15906027314735644D95CD6763CEB49F56AC2F376E1CEE0EBF282DF439906F34D86E085BD5656AD841F313D72D395EFE33CBFF29E4030B3D05A28FB7F18EA27637B07957D32F2BDE8706227D04665EC91BAF8B1AC3EC9144AB7F21");

	printf("Public key:\n");
	bdPrintHex("N=", N, "\n");
	bdPrintHex("e=", e, "\n");
	printf("Private key:\n");
	bdPrintHex("d=", d, "\n");

	nbits = bdBitLength(N);
	printf("Key size = %d bits\n", nbits);

	/* Set a random message m < N */
	bdQuickRandBits(m, nbits-8);
	bdPrintHex("Random plaintext m =\n", m, "\n");

	/* Encrypt c = m^e mod N */
	/* NB no need to use constant-time modexp when using public exponent */
	bdModExp(c, m, e, N);
	bdPrintHex("Ciphertext c=m^e mod N=\n", c, "\n");

	printf("\nSIMPLE DECRYPTION USING PRIVATE EXPONENT...\n");
	/* Check decrypt m1 = c^d mod N */
	start = clock();
	/* Use standard non-constant-time modexp with private exponent 
		=> faster but vulnerable to simple power attack (SPA) */
	bdModExp(y, c, d, N);
	finish = clock();
	tinv = (double)(finish - start) / CLOCKS_PER_SEC;
	bdPrintHex("y=c^d mod N (standard)\n", y, "\n");
	isequal = bdCompare_ct(y, m);
	printf("Decryption %s\n", (isequal == 0 ? "OK" : "FAILED!"));
	assert(isequal == 0);
	printf("Decryption by standard modexp took %.3f secs\n", tinv);

	/* Now decrypt using constant-time modexp */
	start = clock();
	/* Resistant to simple power attack, but not differential power analysis (DPA) */
	bdModExp_ct(y, c, d, N);
	finish = clock();
	tinv_ct = (double)(finish - start) / CLOCKS_PER_SEC;
	bdPrintHex("y=c^d mod N (constant-time)\n", y, "\n");
	isequal = bdCompare_ct(y, m);
	printf("Decryption %s\n", (isequal == 0 ? "OK" : "FAILED!"));
	assert(isequal == 0);
	printf("Decryption by constant-time modexp took %.3f secs\n", tinv_ct);
	printf("Ratio constant-time/standard = %.3f\n", tinv_ct / tinv);

	/*
	USE BLINDING TO DEFEAT DPA
	Differential power analysis depends on the attacker knowing the base c of the exponentiation.
	This can succeed even if constant-time exponentiation is used.
	Ref: Jean-Sebastian Coron, "Resistance Against Differential Power Analysis for 
	Elliptic Curve Cryptosystems", August 1999.
	   -- This version adapted from Coron's elliptic curve point scalar multiplication 
	   to RSA-style modular exponentiation.
	*/

	/* Algorithm: Modular exponentiation with blinding.
	Input: RSA ciphertext c, private exponent d, public exponent e, modulus N
	Output: RSA plaintext c^d mod N
	1. Select a random integer r < N with gcd(r,N) = 1
	2. Compute R = r^e mod N and S = r^-1 mod N
	3. Compute x = c.R mod N
	4. Compute z = x^d mod N
	5. Compute y = S.z mod N
	6. Return y
	*/

	/* 
	You could store the values R and S initially and refresh on each execution by
	    R <-- R^{2(-1)^b}
		S <-- S^{2(-1)^b}
	where b is a random bit generated at each new execution.
	*/

	printf("\nUSING BLINDING...\n");
	start = clock();
	printf("1. Select a random integer r < N with gcd(r,N) = 1 \n");
	do {
		bdRandomBits(r, nbits);
		bdModulo(r, r, N);
		bdGcd(g, r, N);
	} while (bdShortCmp(g, 1) != 0);
	bdPrintHex("r=", r, "\n");
	bdPrintHex("[CHECK] gcd(r,n)=", g, "\n");

	printf("2. Compute R = r^e mod N and S = r^-1 mod N \n");
	bdModExp(R, r, e, N);	/* NB public exponent, so no need for CT */
	bdModInv(S, r, N);
	bdPrintHex("R=r^e mod N=", R, "\n");
	bdPrintHex("S=r^-1 mod N=", S, "\n");

	printf("3. Compute x = c.R mod N \n");
	bdModMult(x, c, R, N);
	bdPrintHex("x = c.R mod N=", x, "\n");

	printf("4. Compute z = x^d mod N \n");
	bdModExp_ct(z, x, d, N);
	bdPrintHex("z = b^d mod N=", z, "\n");

	printf("5. Compute y = S.z mod N \n");
	bdModMult(y, S, z, N);
	bdPrintHex("y = S.z mod N=", y, "\n");

	finish = clock();	/* Assume effect of printing is negligible (?) */
	tblind = (double)(finish - start) / CLOCKS_PER_SEC;

	printf("6. Return y = c^d mod N \n");

	isequal = bdCompare_ct(y, m);
	printf("Decryption %s\n", (isequal == 0 ? "OK" : "FAILED!"));
	assert(isequal == 0);
	printf("Decryption using blinding took %.3f secs\n", tblind);
	printf("Ratio blinding/standard (with printing) = %.3f\n", tblind / tinv);

	printf("\nREPEAT WITH REFRESHED R AND S...\n");
	tblind = 0;

	for (i = 0; i < ntimes; i++) {
		printf("Iteration number %d:\n", i+1);

		/* Generate a random bit */
		b = bdRandDigit() & 0x1;
		printf("Random bit b=%d\n", b);

		start = clock();
		/* R <-- R^{2(-1)^b}, S <-- S^{2(-1)^b} */
		bdModMult(R, R, R, N);
		bdModMult(S, S, S, N);
		if (b) {
			bdModInv(R, R, N);
			bdModInv(S, S, N);
		}

		/* 3. Compute x = c.R mod N */
		bdModMult(x, c, R, N);

		/* 4. Compute z = x^d mod N */
		bdModExp_ct(z, x, d, N);

		/* 5. Compute y = S.z mod N */
		bdModMult(y, S, z, N);

		finish = clock();
		tblind += (double)(finish - start) / CLOCKS_PER_SEC;

		bdPrintHex("R <-- R^{2(-1)^b}=\n", R, "\n");
		bdPrintHex("S <-- S^{2(-1)^b}=\n", S, "\n");

		/* 6. Return y = c^d mod N */
		bdPrintHex("Output y=\n", y, "\n");

		isequal = bdCompare_ct(y, m);
		printf("Decryption %s\n", (isequal == 0 ? "OK" : "FAILED!"));
		assert(isequal == 0);

		printf("\n");
	}

	tblind /= ntimes;
	printf("Decryption using blinding took on average %.3f secs over %d repeats\n", tblind, ntimes);
	printf("Ratio blinding/standard = %.3f\n", tblind / tinv);


	/* Clean up */
	bdFreeVars(&N, &e, &d, &m, &c, &y, &r, &g, &R, &S, &x, &z);

	printf("\nOK, SUCCESSFULLY COMPLETED TESTS WITHOUT ERROR.\n");

	return 0;
}
