/* spExtras.h */

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

/* Interface to extra BigDigits "sp" functions that operate with single digits */

#ifndef SPEXTRAS_H_
#define SPEXTRAS_H_ 1

#include "bigdigits.h"

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m);
	/* Computes a = (x * y) mod m */

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v);
	/* Computes inv = u^-1 mod v */

DIGIT_T spGcd(DIGIT_T x, DIGIT_T y);
	/* Returns gcd(x, y) */

int spIsPrime(DIGIT_T w, size_t t);
	/* Returns true if w is a probable prime, else false; t tests */

/* int spModExp(DIGIT_T *exp, DIGIT_T x, DIGIT_T n, DIGIT_T d); */
	/* Computes exp = x^n mod d */

/* Two alternatives for spModExp:
   define USE_KNUTH_MODEXP before <spExtras.h> to use Knuth method, 
   otherwise defaults to use Binary left-to-right method */
#ifdef USE_KNUTH_MODEXP
#define spModExp spModExpK
#else
#define spModExp spModExpB
#endif	/* USE_KNUTH_MODEXP */

int spModExpK(DIGIT_T *exp, DIGIT_T x, DIGIT_T n, DIGIT_T d);
int spModExpB(DIGIT_T *exp, DIGIT_T x, DIGIT_T n, DIGIT_T d);

#endif /* SPEXTRAS_H_ */

