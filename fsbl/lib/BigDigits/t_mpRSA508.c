/* $Id: t_mpRSA508.c $ */

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

/* Test vector using BIGDIGITS to show RSA digital signature using 508-bit test vectors from 
"Some Examples of the PKCS Standards"
An RSA Laboratories Technical Note
Burton S. Kaliski Jr.
Revised November 1, 1993
*/

/* ---
From the document:-
In the example, the modulus n is the following 508-bit
integer:

n = 0a 66 79 1d c6 98 81 68 de 7a b7 74 19 bb 7f b0 c0 01 c6
27 10 27 00 75 14 29 42 e1 9a 8d 8c 51 d0 53 b3 e3 78 2a 1d
e5 dc 5a f4 eb e9 94 68 17 01 14 a1 df e6 7c dc 9a 9a f5 5d
65 56 20 bb ab

The public exponent e is F4 (65537):

e = 01 00 01

The private exponent d:

d = 01 23 c5 b6 1b a3 6e db 1d 36 79 90 41 99 a8 9e a8 0c 09
b9 12 2e 14 00 c0 9a dc f7 78 46 76 d0 1d 23 35 6a 7d 44 d6
bd 8b d5 0e 94 bf c7 23 fa 87 d8 86 2b 75 17 76 91 c1 1d 75
76 92 df 88 81

RSA signing according to PKCS #1 has two general steps:
An encryption block is constructed from a block type, a
padding string, and the prefixed message digest; then the
encryption block is exponentiated with Test User 1's private
exponent.

The encryption block EB is the following 64-octet string:

00
01                                               block type
ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff padding string
ff ff ff ff ff ff ff ff ff ff ff
00                                              00 separator
30 20
   30 0c                                     digestAlgorithm
      06 08 2a 86 48 86 f7 0d 02 02          algorithm = md2
      05 00                                parameters = NULL
   04 10                                              digest
      dc a9 ec f1 c1 5c 1b d2 66 af f9 c8 79 93 65 cd

i.e. the 64-octet string:
00 01 ff ff ff ff ff ff ff ff ff ff ff ff ff ff
ff ff ff ff ff ff ff ff ff ff ff ff ff 00 30 20
30 0c 06 08 2a 86 48 86 f7 0d 02 02 05 00 04 10
dc a9 ec f1 c1 5c 1b d2 66 af f9 c8 79 93 65 cd

The resulting encrypted message digest (the signature) is
the following 64-octet string:

06 db 36 cb 18 d3 47 5b 9c 01 db 3c 78 95 28 08
02 79 bb ae ff 2b 7d 55 8e d6 61 59 87 c8 51 86
3f 8a 6c 2c ff bc 89 c3 f7 5a 18 d9 6b 12 7c 71
7d 54 d0 d8 04 8d a8 a0 54 46 26 d1 7a 2a 8f be

CAUTION: Note that this example is for a digital signature, not encryption
to keep a secret. To use RSA to encrypt a message, you should use *random* 
padding in the encryption block, different each time, not the fixed block 
of FF padding bytes used here, and you exponentiate with the recipient's 
*public* exponent. The recipient then deciphers using his private exponent.

*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bigdigits.h"

#define MOD_SIZE 16	/* max 512-bits */


void pr_bytes(unsigned char *b, size_t nbytes)
{
	size_t i;

	for (i = 0; i < nbytes; i++)
	{
		if (i && (i % 16 == 0)) printf("\n");
		printf("%02x ", b[i]);
	}
	printf("\n");
}

int main(void)
{
	DIGIT_T n[MOD_SIZE], e[MOD_SIZE], d[MOD_SIZE];
	DIGIT_T  s[MOD_SIZE], m[MOD_SIZE], m1[MOD_SIZE], s1[MOD_SIZE];
	size_t nbytes;
	char decimal[MOD_SIZE*4];

	/* Data in big-endian byte format:- 
	*/
	unsigned char nn[] = {
		0x0A, 0x66, 0x79, 0x1D, 0xC6, 0x98, 0x81, 0x68, 
		0xDE, 0x7A, 0xB7, 0x74, 0x19, 0xBB, 0x7F, 0xB0, 
		0xC0, 0x01, 0xC6, 0x27, 0x10, 0x27, 0x00, 0x75, 
		0x14, 0x29, 0x42, 0xE1, 0x9A, 0x8D, 0x8C, 0x51, 
		0xD0, 0x53, 0xB3, 0xE3, 0x78, 0x2A, 0x1D, 0xE5, 
		0xDC, 0x5A, 0xF4, 0xEB, 0xE9, 0x94, 0x68, 0x17, 
		0x01, 0x14, 0xA1, 0xDF, 0xE6, 0x7C, 0xDC, 0x9A, 
		0x9A, 0xF5, 0x5D, 0x65, 0x56, 0x20, 0xBB, 0xAB,
	};

	unsigned char ee[] = { 0x01, 0x00, 0x01 };

	unsigned char dd[] = {
		0x01, 0x23, 0xC5, 0xB6, 0x1B, 0xA3, 0x6E, 0xDB, 
		0x1D, 0x36, 0x79, 0x90, 0x41, 0x99, 0xA8, 0x9E, 
		0xA8, 0x0C, 0x09, 0xB9, 0x12, 0x2E, 0x14, 0x00, 
		0xC0, 0x9A, 0xDC, 0xF7, 0x78, 0x46, 0x76, 0xD0, 
		0x1D, 0x23, 0x35, 0x6A, 0x7D, 0x44, 0xD6, 0xBD, 
		0x8B, 0xD5, 0x0E, 0x94, 0xBF, 0xC7, 0x23, 0xFA, 
		0x87, 0xD8, 0x86, 0x2B, 0x75, 0x17, 0x76, 0x91, 
		0xC1, 0x1D, 0x75, 0x76, 0x92, 0xDF, 0x88, 0x81,
	};

	unsigned char mm[] = {
		0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x30, 0x20, 
		0x30, 0x0C, 0x06, 0x08, 0x2A, 0x86, 0x48, 0x86, 
		0xF7, 0x0D, 0x02, 0x02, 0x05, 0x00, 0x04, 0x10, 
		0xDC, 0xA9, 0xEC, 0xF1, 0xC1, 0x5C, 0x1B, 0xD2, 
		0x66, 0xAF, 0xF9, 0xC8, 0x79, 0x93, 0x65, 0xCD,
	};

	unsigned char ss[] = {
		0x06, 0xDB, 0x36, 0xCB, 0x18, 0xD3, 0x47, 0x5B, 
		0x9C, 0x01, 0xDB, 0x3C, 0x78, 0x95, 0x28, 0x08, 
		0x02, 0x79, 0xBB, 0xAE, 0xFF, 0x2B, 0x7D, 0x55, 
		0x8E, 0xD6, 0x61, 0x59, 0x87, 0xC8, 0x51, 0x86, 
		0x3F, 0x8A, 0x6C, 0x2C, 0xFF, 0xBC, 0x89, 0xC3, 
		0xF7, 0x5A, 0x18, 0xD9, 0x6B, 0x12, 0x7C, 0x71, 
		0x7D, 0x54, 0xD0, 0xD8, 0x04, 0x8D, 0xA8, 0xA0, 
		0x54, 0x46, 0x26, 0xD1, 0x7A, 0x2A, 0x8F, 0xBE,
	};

	printf("Test BIGDIGITS using 508-bit RSA key from 'Some Examples of the PKCS Standards'\n");

	/* Convert bytes to BIGDIGITS */
	mpConvFromOctets(n, MOD_SIZE, nn, sizeof(nn));
	mpConvFromOctets(e, MOD_SIZE, ee, sizeof(ee));
	mpConvFromOctets(d, MOD_SIZE, dd, sizeof(dd));
	mpConvFromOctets(m, MOD_SIZE, mm, sizeof(mm));
	mpConvFromOctets(s1, MOD_SIZE, ss, sizeof(ss));


	mpPrintHex("n =", n, MOD_SIZE, "\n");
	mpPrintHex("e =", e, MOD_SIZE, "\n");
	mpPrintHex("d =", d, MOD_SIZE, "\n");
	mpPrintHex("m =", m, MOD_SIZE, "\n");

	/* Sign, i.e. Encrypt with private key, s = m^d mod n */
	mpModExp(s, m, d, n, MOD_SIZE);

	mpPrintHex("s =", s, MOD_SIZE, " ");

	/* Did we get the same answer as expected? */
	if (!mpEqual(s1, s, MOD_SIZE))
		printf("<= ERROR - no match\n");
	else
		printf("<= OK\n");
	assert(mpEqual(s1, s, MOD_SIZE));

	/* Verify, i.e. Decrypt with public key m' = s^e mod n */
	mpModExp(m1, s, e, n, MOD_SIZE);

	mpPrintHex("m'=", m1, MOD_SIZE, " ");

	/* Check that we got back where we started */
	if (!mpEqual(m1, m, MOD_SIZE)) 
		printf("<= ERROR - no match\n");
	else
		printf("<= OK\n");
	assert(mpEqual(m1, m, MOD_SIZE));

	/* Now convert back to octets (bytes) */
	printf("Convert BIGDIGITS back to octets (bytes)...\n");
	memset(mm, 0, sizeof(mm)); 
	nbytes = mpConvToOctets(m, MOD_SIZE, mm, sizeof(mm));
	printf("%d non-zero bytes converted from m:\n", nbytes);
	pr_bytes(mm, sizeof(mm));

	memset(ee, 0, sizeof(ee)); 
	nbytes = mpConvToOctets(e, MOD_SIZE, ee, sizeof(ee));
	printf("%d non-zero bytes converted from e:\n", nbytes);
	pr_bytes(ee, sizeof(ee));

	/* Do a conversion to decimal */
	nbytes = mpConvToDecimal(e, MOD_SIZE, decimal, sizeof(decimal));
	printf("%d non-zero decimal digits converted from e:\n", nbytes);
	printf("%s\n", decimal);
	assert(strcmp(decimal, "65537") == 0);

	printf("OK, successfully completed tests.\n");

	return 0;
}

