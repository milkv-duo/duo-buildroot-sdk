/* $Id: t_mpJacobi.c $ */

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

#include <stdio.h>
#include <assert.h>
#include "bigdigits.h"

#define TEST_LEN (1024/8)

char *strA = 
	"6BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBAA9993E364706816ABA3E25717850"
	"C26C9CD0D89D33CC";

const char *strN = 
	"CCD34C2F4D95FFAD1420E666C07E39D1450A1330"
	"4C3F5891EDE57595C772A3691AB51D2BECE1476B"
	"8F22AE223365F183BC3EE2D4CACDBA3AD0C4D478"
	"1C523A10EFE6203D6F3BC226BF9A459727B8F122"
	"C482D8C86019F9A869329187096430A6C67CB103"
	"742BCBC66906AD23836EBABB511D5D80AB8CB599"
	"74E9AAC62D785C45";

int main(void)
{
	size_t ndigits = TEST_LEN;
	DIGIT_T a[TEST_LEN];
	DIGIT_T n[TEST_LEN];
	int j;

	/* Use example from X9.31 Appendix D.5.2 */
	mpConvFromHex(a, ndigits, strA);
	mpConvFromHex(n, ndigits, strN);
	mpPrintHex("n=\n", n, ndigits, "\n");
	mpPrintHex("a=\n", a, ndigits, "\n");
	j= mpJacobi(a, n, ndigits);
	printf("Jacobi(a/n)=%d\n", j);
	assert(j == -1);

	/* Divide a by 2 */
	mpShiftRight(a, a, 1, ndigits);
	mpPrintHex("a=a/2=\n", a, ndigits, "\n");
	j= mpJacobi(a, n, ndigits);
	printf("Jacobi(a/n)=%d\n", j);
	assert(j == +1);

	/* Make n|a */
	mpShortMult(a, n, 7, ndigits);
	mpPrintHex("a=n*7=\n", a, ndigits, "\n");
	j= mpJacobi(a, n, ndigits);
	printf("Jacobi(a/n)=%d\n", j);
	assert(j == 0);

	printf("Jacobi tests completed OK\n");

	return 0;
}
