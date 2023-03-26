/* $Id: t_bdCPP.c $ */

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

#include "bigd.h"
#include "bigdRand.h"

#include <iostream>
using namespace std;
int main()
{
	int ver;
	ver = bdVersion();
	cout << "bdVersion = " << ver << "\n";

	BIGD u, v, w;

	/* Create new BIGD objects */
	u = bdNew();
	v = bdNew();
	w = bdNew();

	/* Compute 2 * 0xdeadbeefface */
	bdSetShort(u, 2);
	bdConvFromHex(v, "deadbeefface");
	bdMultiply(w, u, v);

	/* Display the result */
	bdPrintHex("", u, " * ");
	bdPrintHex("0x", v, " = ");
	bdPrintHex("0x", w, "\n");
	/* and again in decimal format */
	bdPrintDecimal("", u, " * ");
	bdPrintDecimal("", v, " = ");
	bdPrintDecimal("", w, "\n");


	/* Add with digit overflow */
	/* ffffffff+ffffffff=00000001 fffffffe */
	bdSetShort(u, 0xffffffff);
	bdSetShort(v, 0xffffffff);
	bdAdd(w, u, v);
	bdPrintHex("", u, "+");
	bdPrintHex("", v, "=");
	bdPrintHex("", w, "\n");

	/* Generate some random digits using internal RNG */
	cout << "Some random numbers:\n";
	bdSetShort(u, bdRandDigit());
	bdPrintHex("short: ", u, "\n");
	bdRandomBits(u, 512);
	bdPrintHex("512 bits: ", u, "\n");

	/* Free all objects we made */
	bdFree(&u);
	bdFree(&v);
	bdFree(&w);

	return 0;
}
