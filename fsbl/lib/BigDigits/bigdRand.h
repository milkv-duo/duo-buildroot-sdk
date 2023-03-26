/* $Id: bigdRand.h $ */

/** @file
    Interface for BigDigits "bd" random number functions using a "pretty-good" internal RNG

@par The internal random number generator (RNG)
The internal RNG uses a variant of the random number generation algorithm 
in Appendix A of ANSI X9.31-1998, but using the Tiny Encryption Algorithm (TEAX)
instead of the Data Encryption Algorithm (DEA). 
It uses the current time and process ID as a seed. 
Although not strictly crypto secure, it is "pretty good", and certainly much better than
anything using the built-in rand() function in C. Look at the source code and make your own call.

@par 
If you want proper cryptographic security, use the bdRandomSeeded() function with a call to a
secure RNG function that you trust.
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

#ifndef BIGDRAND_H_
#define BIGDRAND_H_ 1

#include "bigd.h"
#include "bigdRand.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Generate a single random digit using internal RNG. */
bdigit_t bdRandDigit(void);

/** Generate a random BIGD number of bit length at most \c nbits using internal RNG
@param[out] a to receive generated random number
@param[in]  nbits maximum number of bits
@returns Number of digits actually set  
*/
size_t bdRandomBits(BIGD a, size_t nbits);

/* Added in [v2.4] */
/** Generate array of random octets (bytes) using internal RNG
 *  @remarks This function is in the correct form for BD_RANDFUNC. 
 */ 
int bdRandomOctets(unsigned char *bytes, size_t nbytes, const unsigned char *seed, size_t seedlen);

/** Generate a number at random from a uniform distribution in [0, n-1] */
size_t bdRandomNumber(BIGD a, BIGD n);

#ifdef __cplusplus
}
#endif

#endif /* BIGDRAND_H_ */
