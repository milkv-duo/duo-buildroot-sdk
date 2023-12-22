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
    MD5

    Abstract:
    RFC1321: The MD5 Message-Digest Algorithm
    
    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
    Eddy        2008/11/24      Create md5
***************************************************************************/
#endif /* CRYPT_GPL_ALGORITHM */

#ifndef __CRYPT_MD5_H__
#define __CRYPT_MD5_H__

#ifdef CRYPT_GPL_ALGORITHM

#ifndef	uint8
#define	uint8  unsigned	char
#endif

#ifndef	uint32
#define	uint32 unsigned	long int
#endif

#define MD5_MAC_LEN 16

typedef struct _MD5_CTX {
    ULONG   Buf[4];             /* buffers of four states */
	UCHAR   Input[64];          /* input message */
	ULONG   LenInBitCount[2];   /* length counter for input message, 0 up to 64 bits */
}   MD5_CTX;

VOID MD5Init(MD5_CTX *pCtx);
VOID MD5Update(MD5_CTX *pCtx, UCHAR *pData, ULONG LenInBytes);
VOID MD5Final(UCHAR Digest[16], MD5_CTX *pCtx);
VOID MD5Transform(ULONG Buf[4], ULONG Mes[16]);

void md5_mac(UINT8 *key, size_t key_len, UINT8 *data, size_t data_len, UINT8 *mac);
void hmac_md5(UINT8 *key, size_t key_len, UINT8 *data, size_t data_len, UINT8 *mac);

/* */
/* SHA context */
/* */
typedef	struct _SHA_CTX
{
	ULONG   Buf[5];             /* buffers of five states */
	UCHAR   Input[80];          /* input message */
	ULONG   LenInBitCount[2];   /* length counter for input message, 0 up to 64 bits */
	
}	SHA_CTX;

VOID SHAInit(SHA_CTX *pCtx);
UCHAR SHAUpdate(SHA_CTX *pCtx, UCHAR *pData, ULONG LenInBytes);
VOID SHAFinal(SHA_CTX *pCtx, UCHAR Digest[20]);
VOID SHATransform(ULONG Buf[5], ULONG Mes[20]);

#define SHA_DIGEST_LEN 20

VOID HMAC_SHA1(
	IN UCHAR * text,
	IN UINT text_len,
	IN UCHAR * key,
	IN UINT key_len,
	IN UCHAR * digest);

#define RT_HMAC_MD5(Key, KeyL, Meg, MegL, MAC, MACL) \
    hmac_md5((Key), (KeyL), (Meg), (MegL), (MAC))

#define RT_HMAC_SHA1(Key, KeyL, Meg, MegL, MAC, MACL) \
    HMAC_SHA1((Meg), (MegL), (Key), (KeyL), (MAC))

#else /* CRYPT_GPL_ALGORITHM */

/* Algorithm options */
#define MD5_SUPPORT

#ifdef MD5_SUPPORT
#define MD5_BLOCK_SIZE    64	/* 512 bits = 64 bytes */
#define MD5_DIGEST_SIZE   16	/* 128 bits = 16 bytes */
typedef struct {
	UINT32 HashValue[4];
	UINT64 MessageLen;
	UINT8 Block[MD5_BLOCK_SIZE];
	UINT BlockLen;
} MD5_CTX_STRUC, *PMD5_CTX_STRUC;

VOID RT_MD5_Init(
	IN MD5_CTX_STRUC * pMD5_CTX);
VOID RT_MD5_Hash(
	IN MD5_CTX_STRUC * pMD5_CTX);
VOID RT_MD5_Append(
	IN MD5_CTX_STRUC * pMD5_CTX,
	IN const UINT8 Message[],
	IN UINT MessageLen);
VOID RT_MD5_End(
	IN MD5_CTX_STRUC * pMD5_CTX,
	OUT UINT8 DigestMessage[]);
VOID RT_MD5(
	IN const UINT8 Message[],
	IN UINT MessageLen,
	OUT UINT8 DigestMessage[]);
#endif /* MD5_SUPPORT */

#endif /* CRYPT_GPL_ALGORITHM */

#endif /* __CRYPT_MD5_H__ */
