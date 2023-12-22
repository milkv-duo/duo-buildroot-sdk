/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

#ifndef CRYPT_GPL_ALGORITHM
/****************************************************************************
    Module Name:
    HMAC

    Abstract:
    FIPS 198: The Keyed-Hash Message Authentication Code (HMAC)
    
    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
    Eddy        2008/11/24      Create HMAC-SHA1, HMAC-SHA256
***************************************************************************/
#endif /* CRYPT_GPL_ALGORITHM */

#ifndef __CRYPT_HMAC_H__
#define __CRYPT_HMAC_H__

#ifdef CRYPT_TESTPLAN
#include "crypt_testplan.h"
#else
#include "rt_config.h"
#endif /* CRYPT_TESTPLAN */

#ifdef CRYPT_GPL_ALGORITHM

#if defined(__cplusplus)
extern "C" {
#endif

#if 1				/* add for WSC CryptoLib ++ */
#define USE_SHA256
#endif

#if !defined(USE_SHA1) && !defined(USE_SHA256)
#error define USE_SHA1 or USE_SHA256 to set the HMAC hash algorithm
#endif

#ifdef USE_SHA1
#define HASH_INPUT_SIZE     SHA1_BLOCK_SIZE
#define HASH_OUTPUT_SIZE    SHA1_DIGEST_SIZE
#define sha_ctx             sha1_ctx
#define sha_begin           sha1_begin
#define sha_hash            sha1_hash
#define sha_end             sha1_end
#endif

#ifdef USE_SHA256
#define HASH_INPUT_SIZE     SHA256_BLOCK_SIZE
#define HASH_OUTPUT_SIZE    SHA256_DIGEST_SIZE
#define sha_ctx             sha256_ctx
#define sha_begin           sha256_begin
#define sha_hash            sha256_hash
#define sha_end             sha256_end
#endif

#define HMAC_OK                0
#define HMAC_BAD_MODE         -1
#define HMAC_IN_DATA  0xffffffff

typedef struct
{   unsigned char   key[HASH_INPUT_SIZE];
    sha_ctx         ctx[1];
    unsigned int   klen;
} hmac_ctx;

void hmac_sha_begin(hmac_ctx cx[1]);
int  hmac_sha_key(const unsigned char key[], unsigned int key_len, hmac_ctx cx[1]);
void hmac_sha_data(const unsigned char data[], unsigned int data_len, hmac_ctx cx[1]);
void hmac_sha_end(unsigned char mac[], unsigned int mac_len, hmac_ctx cx[1]);
void hmac_sha(const unsigned char key[], unsigned int key_len,
          const unsigned char data[], unsigned int data_len,
          unsigned char mac[], unsigned int mac_len);

#define RT_HMAC_SHA256(Key, KeyL, Data, DataL, Mac, MacL) \
    hmac_sha((Key), (KeyL), (Data), (DataL), (Mac), (MacL))

#if defined(__cplusplus)
}
#endif
#else /* CRYPT_GPL_ALGORITHM */

#ifdef SHA1_SUPPORT
#define HMAC_SHA1_SUPPORT
VOID RT_HMAC_SHA1(
	IN const UINT8 Key[],
	IN UINT KeyLen,
	IN const UINT8 Message[],
	IN UINT MessageLen,
	OUT UINT8 MAC[],
	IN UINT MACLen);
#endif /* SHA1_SUPPORT */

#ifdef SHA256_SUPPORT
#define HMAC_SHA256_SUPPORT
VOID RT_HMAC_SHA256(
	IN const UINT8 Key[],
	IN UINT KeyLen,
	IN const UINT8 Message[],
	IN UINT MessageLen,
	OUT UINT8 MAC[],
	IN UINT MACLen);
#endif /* SHA256_SUPPORT */

#ifdef MD5_SUPPORT
#define HMAC_MD5_SUPPORT
VOID RT_HMAC_MD5(
	IN const UINT8 Key[],
	IN UINT KeyLen,
	IN const UINT8 Message[],
	IN UINT MessageLen,
	OUT UINT8 MAC[],
	IN UINT MACLen);
#endif /* MD5_SUPPORT */

#endif /* CRYPT_GPL_ALGORITHM */

#endif /* __CRYPT_HMAC_H__ */
