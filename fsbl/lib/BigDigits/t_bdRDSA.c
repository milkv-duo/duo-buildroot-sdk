/* $Id: t_bdRDSA.c $ */

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

/* EXAMPLE OF THE rDSA from ANSI X9.31 -1998
   Digital Signatures Using Reversible Public Key
   Cryptography for the Financial Services Industry (rDSA), 
   September 9, 1998 
   Appendix D.1 Odd e = 3 with 1024-bit n
   and D.3 Odd e =3 with 2048-bit n
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
#include <assert.h>
#include "bigd.h"


int rDSA_SignAndVerify(BIGD ir, BIGD n, BIGD e, BIGD d)
{
	BIGD s;
	BIGD tmp;
	BIGD rr, ir1, rr1;
	int result;

	/* Initialise variables */
	s = bdNew();
	rr = bdNew();
	ir1 = bdNew();
	rr1 = bdNew();
	tmp = bdNew();

	bdPrintHex("n=\n", n, "\n");
	bdPrintHex("e=", e, "\n");
	bdPrintHex("d=\n", d, "\n");
	bdPrintHex("IR=\n", ir, "\n");

	/* COMPUTATION BY SIGNER */
	/* (n, e) is the public key
	   (n, d) is the private key
	   IR is the encoded message
	   Keep d secret
	*/

	/* For odd e, RR = IR. The value RR^d mod n is now computed */
	bdModExp(tmp, ir, d, n);
	bdPrintHex("RR^d mod n=\n", tmp, "\n");

	/* S = min { RR^d mod n, n-(RR^d mod n) } */
	/* i.e. if this value > n/2, the signature S = n - (RR^d mod n) */
	bdSubtract(s, n, tmp);
	if (bdCompare(s, tmp) > 0)
		bdSetEqual(s, tmp);
	bdPrintHex("S=\n", s, "\n");

	/* D.1.3 Signature Verification */

	/* The value (S')^e mod n is computed to yield RR'. */
	bdModExp(rr, s, e, n);
	bdPrintHex("RR'=\n", rr, "\n");

	/* If RR' = 12 mod 16, then IR' = RR';
	   If n - RR' = 12 mod 16, then IR' = n - RR'; */

	if (bdShortMod(tmp, rr, 16) == 12)
		bdSetEqual(ir1, rr);
	else
		bdSubtract(ir1, n, rr);
	
	bdPrintHex("IR'=\n", ir1, "\n");

	/* Since IR' is identical to the computed hash IR, and the signature verification is
	   successful. */
	result = bdIsEqual(ir1, ir);
	if (result)
		printf("Signature verified OK\n");
	else
		printf("Signature verification FAILED!\n");

	/* Clean up */
	bdFree(&s);
	bdFree(&rr);
	bdFree(&ir1);
	bdFree(&rr1);
	bdFree(&tmp);

	return result;	/* TRUE = success */
}

int main(void)
{
	BIGD e, n, d;
	BIGD ir;
	int result;

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

	printf("EXAMPLE OF THE rDSA from ANSI X9.31-1998 Appendix D\n");

	/* Initialise variables */
	e = bdNew();
	n = bdNew();
	d = bdNew();
	ir = bdNew();

	/* INPUT */
	/* Convert integers from hex format to BIGD (spaces are ignored) */
	bdConvFromHex(n, "ACD1CC46 DFE54FE8 F9786672 664CA269 0D0AD7E5"
		"003BC642 7954D939 EEE8B271 52E6A947 450D7FA9"
		"80172DE0 64D6569A 28A83FE7 0FA840F5 E9802CB8"
		"984AB34B D5C1E639 9EC21E4D 3A3A69BE 4E676F39"
		"5AAFEF7C 4925FD4F AEE9F9E5 E48AF431 5DF0EC2D"
		"B9AD7A35 0B3DF2F4 D15DC003 9846D1AC A3527B1A"
		"75049E3F E34F43BD");
	bdConvFromHex(d, "1CCDA20B CFFB8D51 7EE96668 66621B11 822C7950"
		"D55F4BB5 BEE37989 A7D17312 E326718B E0D79546"
		"EAAE87A5 6623B919 B1715FFB D7F16028 FC400774"
		"1961C88C 5D7B4DAA AC8D36A9 8C9EFBB2 6C8A4A0E"
		"6BC15B35 8E528A1A C9D0F042 BEB93BCA 16B541B3"
		"3F80C933 A3B76928 5C462ED5 677BFE89 DF07BED5"
		"C127FD13 241D3C4B");
	bdConvFromHex(ir, "6BBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBAA999 3E364706 816ABA3E 25717850"
		"C26C9CD0 D89D33CC");
	/* Set e = 3 directly */
	bdSetShort(e, 3);

	result = rDSA_SignAndVerify(ir, n, e, d);
	assert(result);

	/* Repeat for D.3 Odd e =3 with 2048-bit n */
	bdConvFromHex(n, "BCCA30CA 5280BC51 E758DBAF FEDFE46A 5D2A78D8"
		"3969E154 780F9B27 AD4E76E6 3E8FA960 29C04DE3"
		"F2C4B01F ECBEBB44 FC2F5F8A C69BD0E3 278FF065"
		"8A6F9AC2 3BCFB2C9 AAAF1AC4 D5571A55 D3B1EC35"
		"50CF34D8 F789235D D3B5C4E2 F3DF1761 19D918F2"
		"D35E805B 62FC35FE E5FD0785 4824B984 6DC1665E"
		"35A31873 BF6A24FE 50842D0A A0305C35 D0F45B0D"
		"7C2F1E94 32D850D6 7956D383 BBEF52FC 2ACBF7AE"
		"6F7CA214 88A56456 BDF66726 96EE037C 3CAA2199"
		"904E37AA 40134E10 155FC813 93A225BD 129C4B3B"
		"C91AD3A5 FC958A6A BCABE355 0390B677 87625D78"
		"F8D3172B 673C4482 CE354B89 51D7E8C4 DDCE5D4C"
		"DFA6790C 6CE8C02C 8D807AE2 6F27FE33");
	bdConvFromHex(d, "7DDC2086 E1AB2836 9A3B3D1F FF3FED9C 3E1C5090"
		"26469638 500A676F C8DEF9EE D45FC640 1BD58942"
		"A1D8756A 9DD47CD8 A81F9507 2F128B42 1A5FF599"
		"06F511D6 D28A7731 1C74BC83 38E4BC39 37CBF2CE"
		"35DF7890 A5061793 E2792DEC A294BA40 BBE610A1"
		"E23F003C ECA823FF 43FE0503 856DD102 F3D6443E"
		"CE6CBAF7 D4F16DFD BAF7C2F8 CB4955E2 F2DF29FD"
		"A7A4DD7B 93785252 E2CA4AAD 14E8B1A1 7881D381"
		"DAD9061E C8AB50DD CBF8B56A FA464F01 D28DFDFF"
		"0CAFF6B5 588E8F77 6FA128DE 7C102494 7D5D8067"
		"3F545A5C 73FBC16B 609A33A1 1021DD8F 37A4E7C4"
		"78AA19E3 BCB1BBD8 F4AA9232 65F1F1D5 5236753A"
		"C8C10D60 F0FAB073 66439A90 5E2C63AB");
	bdConvFromHex(ir, "6BBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB"
		"BBBBBBBB BBBBBBBB BBBBBBBB BBBAA999 3E364706"
		"816ABA3E 25717850 C26C9CD0 D89D33CC");

	result = rDSA_SignAndVerify(ir, n, e, d);
	assert(result);

	/* Clean up */
	bdFree(&e);
	bdFree(&n);
	bdFree(&d);
	bdFree(&ir);

	printf("OK, successfully completed tests.\n");

	return 0;
}
