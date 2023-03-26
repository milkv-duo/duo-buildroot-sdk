/* $Id: t_bdQuickRandBits.c $ */

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

/* 
What is the difference between bdQuickRandBits and bdRandomBits?

bdRandomBits() uses the "pretty-good" internal RNG and requires you to include bigdRand.h 
and compile with bigdRand.c and bigdigitsRand.c. 
You can be pretty sure that the numbers generated will pass all statistical tests for randomness.

bdQuickRandBits() just needs the standard bigd.h include file and you do not need to
add the modules bigdRand.c and bigdigitsRand.c to your project. 
It uses the stdlib rand() function, which is by no means secure, but it will quickly fill up a number
with the required number of random-looking bits. 
It might repeat if you call twice in quick succession. It's quick and dirty, as advertised.

If you *really* want a secure random number, then use bdRandomSeeded() and call your own super-secure
random number generator, throwing in your own bit of extra entropy with the seed.

*/

#include <stdio.h>
#include <assert.h>
#include "bigd.h"
#include "bigdRand.h"

int main(void)
{
	BIGD a;
	int i;

	a = bdNew();
	// Test bdQuickRandBits (quick-and-dirty)
	printf("Testing bdQuickRandBits...\n");
	for (i = 0; i < 10; i++)
	{
		bdQuickRandBits(a, 128);
		bdPrintHex("", a, "\n");
	}
	// Test bdRandomBits (more secure)
	printf("Testing bdRandomBits...\n");
	for (i = 0; i < 10; i++)
	{
		bdRandomBits(a, 128);
		bdPrintHex("", a, "\n");
	}

	return 0;
}
