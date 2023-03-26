/* $Id: bigdigitsRand.h $ */

/** @file
    Interface for BigDigits "mp" random number functions using a "pretty-good" internal RNG
*/

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

/*	[v2.2] changed name from spRandom to bigdigitsRand */

#ifndef BIGDIGITSRAND_H_
#define BIGDIGITSRAND_H_ 1

#include "bigdigits.h"

#ifdef __cplusplus
extern "C" {
#endif

/**	Returns a "better" pseudo-random digit using internal RNG. */
DIGIT_T spBetterRand(void);

/** Generate a random mp number of bit length at most \c nbits using internal RNG 
@param[out] a to receive generated random number
@param[in]  ndigits number of digits in a
@param[in]  nbits maximum number of bits
@returns Number of digits actually set 
*/
size_t mpRandomBits(DIGIT_T a[], size_t ndigits, size_t nbits);

/* Added in [v2.4] */
/** Generate array of random octets (bytes) using internal RNG
 *  @remarks This function is in the correct form for BD_RANDFUNC to use in bdRandomSeeded(). 
  * \c seed is ignored. */
int mpRandomOctets(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen);

#ifdef __cplusplus
}
#endif

#endif /* BIGDIGITSRAND_H_ */
