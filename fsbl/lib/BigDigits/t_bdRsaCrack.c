/* $Id: t_bdRsaCrack.c $ */

/* 
	This code uses the free BIGDIGITS library version 2.3 available from
	http://di-mgt.com.au/bigdigits.html
	to show how the Chinese Remainder Theorem and Gauss's Method can be used
	to find a message encrypted by the RSA algorithm without having to factor 
	the modulus. The same message must have been encrypted with public exponent
	e = 3 to three different recipients without any random padding.
	Ref: http://www.di-mgt.com.au/crt.html#crackingrsa
	Copyright (C) 2011 DI Management Services Pty Ltd. All rights reserved.
*/

/*
	Last updated:
	$Date: 2011-11-11 11:11:11 $
	$Author: dai $
*/

#include <stdio.h>
#include "bigd.h"

int debug = 0;

int main(void)
{
	BIGD m, e, n1, n2, n3, c1, c2, c3, N, N1, N2, N3, d1, d2, d3, s, t, x;
	m = bdNew();
	e = bdNew();
	n1 = bdNew();
	n2 = bdNew();
	n3 = bdNew();
	c1 = bdNew();
	c2 = bdNew();
	c3 = bdNew();
	N = bdNew();
	N1 = bdNew();
	N2 = bdNew();
	N3 = bdNew();
	d1 = bdNew();
	d2 = bdNew();
	d3 = bdNew();
	s = bdNew();
	t = bdNew();
	x = bdNew();

	/* Read in modulus values for three 512-bit public keys */
	bdConvFromHex(n1, "A9C737DD808D02866FBF1ACF05DE2EB124137007A4965EC4DCBFA6D02F97E0123A8FD3691E414A1382F38AB39B09975705ECEAF1131A283C937B309F1C1417F9");
	bdConvFromHex(n2, "B7E9E114A08ADFF12F762D7F0E1F16202E1EB7A7F2852369BDF44865783D19111E6D61B31DE987BCB9775099E220A798D4F99CD3E5F04C64F87A35C0268A83E9");
	bdConvFromHex(n3, "C2BDBD4E36BA20D37D5D1E968F09F2FC7B41A97F3052274E4892D50D5FB337C923048AED7D393135EE55711E5C74975867F13D3845BAC9588B4BE170D08BAB57");
	//bdSetShort(n1, 87);
	//bdSetShort(n2, 115);
	//bdSetShort(n3, 187);
	
	printf("Three public modulus values, 512 bits each...\n");
	bdPrintHex("n1=", n1, "\n");
	bdPrintHex("n2=", n2, "\n");
	bdPrintHex("n3=", n3, "\n");
	
	/* The three public keys all have public exponent e = 3 */
	bdSetShort(e, 3);
	bdPrintHex("e=", e, "\n");
	
	/* Set the common message m with no random padding (but large enough so m^3 > n_i) */
	bdConvFromHex(m, "02ffffffffffffffffffffffffffffffffffffffffff0000deadbeefcafe");
	//bdSetShort(m, 10);
	printf("Message to be encrypted...\n");
	bdPrintHex("m=", m, "\n");
	
	/* Create three ciphertexts, c_i = m^e mod n_i */
	bdModExp(c1, m, e, n1);
	bdModExp(c2, m, e, n2);
	bdModExp(c3, m, e, n3);

	printf("Three ciphertexts...\n");
	bdPrintHex("c1=", c1, "\n");
	bdPrintHex("c2=", c2, "\n");
	bdPrintHex("c3=", c3, "\n");

	/* An eavedropper has the public values n1, n2, n3, c1, c2 and c3 */

	/* Check that n1, n2, n3 are coprime in pairs */
	bdGcd(t, n1, n2);
	bdPrintHex("gcd(n1,n2)=", t, "\n");
	bdGcd(t, n2, n3);
	bdPrintHex("gcd(n2,n3)=", t, "\n");
	bdGcd(t, n3, n1);
	bdPrintHex("gcd(n3,n1)=", t, "\n");

	/* Compute N = n1 * n2 * n3 */
	bdMultiply(t, n1, n2);
	bdMultiply(N, t, n3);
	if (debug) bdPrintDecimal("N=", N, "\n");

	/* Compute N_i = N/n_i for i = 1,2,3 */
	bdMultiply(N1, n2, n3);
	bdMultiply(N2, n1, n3);
	bdMultiply(N3, n1, n2);
	if (debug) bdPrintDecimal("N1=", N1, "\n");
	if (debug) bdPrintDecimal("N2=", N2, "\n");
	if (debug) bdPrintDecimal("N3=", N3, "\n");

	/* Compute d_i = N_i^{-1} mod n_i for i = 1,2,3  */
	bdModInv(d1, N1, n1);
	bdModInv(d2, N2, n2);
	bdModInv(d3, N3, n3);
	if (debug) bdPrintDecimal("d1=", d1, "\n");
	if (debug) bdPrintDecimal("d2=", d2, "\n");
	if (debug) bdPrintDecimal("d3=", d3, "\n");

	/* Compute x = c_1 N_1 d_1 + c_2 N_2 d_2 + c_3 N_3 d_3 (mod N) */
	bdModMult(s, c1, N1, N);
	bdModMult(x, s, d1, N);
	bdModMult(s, c2, N2, N);
	bdModMult(t, s, d2, N);
	bdAdd_s(x, x, t);
	bdModMult(s, c3, N3, N);
	bdModMult(t, s, d3, N);
	bdAdd_s(s, x, t);
	bdModulo(x, s, N);
	printf("Computed value of x = c_1 N_1 d_1 + c_2 N_2 d_2 + c_3 N_3 d_3 (mod N)...\n");
	bdPrintHex("x=", x, "\n");

	/* Compute the integer cube root of x */
	bdCubeRoot(t, x);
	bdPrintHex("m'=cuberoot(x)=", t, "\n");

	/* Do we have the correct answer? */
	if (bdCompare(t, m) == 0)
		printf("HOORAY! We have found the correct answer, m' = m.\n");
	else
		printf("OH BU**AR! We didn't find the correct answer.\n");


//clean_up:
	bdFree(&m);
	bdFree(&e);
	bdFree(&n1);
	bdFree(&n2);
	bdFree(&n3);
	bdFree(&c1);
	bdFree(&c2);
	bdFree(&c3);
	bdFree(&N);
	bdFree(&N1);
	bdFree(&N2);
	bdFree(&N3);
	bdFree(&d1);
	bdFree(&d2);
	bdFree(&d3);
	bdFree(&s);
	bdFree(&t);
	bdFree(&x);
	
	return 0;
}
