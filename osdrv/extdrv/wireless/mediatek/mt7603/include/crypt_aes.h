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
    AES

    Abstract:
    RFC 3394: Advanced Encryption Standard (AES) Key Wrap Algorithm    
    RFC 3601: Counter with CBC-MAC (CCM)
    RFC 4493: The AES-CMAC Algorithm
    FIPS PUBS 197: ADVANCED ENCRYPTION STANDARD (AES)
    NIST 800-38A: Recommendation for Block Cipher Modes of Operation
    NIST 800-38C: The CCM Mode for Authentication and Confidentiality 
    
    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
    Eddy        2009/05/19      Create AES-Key Wrap
    Eddy        2009/04/20      Create AES-CMAC, AES-CCM    
    Eddy        2009/01/19      Create AES-128, AES-192, AES-256, AES-CBC
***************************************************************************/
#endif /* CRYPT_GPL_ALGORITHM */

#ifndef __CRYPT_AES_H__
#define __CRYPT_AES_H__

#ifdef CRYPT_TESTPLAN
#include "crypt_testplan.h"
#else
#include "rt_config.h"
#endif /* CRYPT_TESTPLAN */

#ifdef CRYPT_GPL_ALGORITHM

#if 1
#define GETU32(p) cpu2be32(get_unaligned32((UINT32 *) (p)))
#define PUTU32(ct, st) put_unaligned(cpu2be32(st), (UINT32*)(ct)) /*{ *((UINT32 *)(ct)) = cpu2be32((st)); } */
#else

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64))
# define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
# define GETU32(p) SWAP(*((UINT32 *)(p)))
# define PUTU32(ct, st) { *((UINT32 *)(ct)) = SWAP((st)); }
#else
# define GETU32(pt) (((UINT32)(pt)[0] << 24) ^ ((UINT32)(pt)[1] << 16) ^ ((UINT32)(pt)[2] <<  8) ^ ((UINT32)(pt)[3]))
# define PUTU32(ct, st) { (ct)[0] = (UINT8)((st) >> 24); (ct)[1] = (UINT8)((st) >> 16); (ct)[2] = (UINT8)((st) >>  8); (ct)[3] = (UINT8)(st); }
#endif

#endif

#define AES_ENCRYPT	1
#define AES_DECRYPT	0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

/* This should be a hidden type, but EVP requires that the size be known */
struct aes_key_st {
#ifdef AES_LONG
    unsigned long rd_key[4 *(AES_MAXNR + 1)];
#else
    unsigned int rd_key[4 *(AES_MAXNR + 1)];
#endif
    int rounds;
};
typedef struct aes_key_st AES_KEY;

typedef struct _EVP_CIPHER_CTX_ {
	unsigned long flag;
	unsigned long type;  
	unsigned long encrypt; /*1: Encrypt 0: Decrypt, */
	unsigned char key[16];
	unsigned char iv[8 + 16];
	unsigned long bufferlen;
	unsigned char buffer[AES_BLOCK_SIZE];
	AES_KEY        aesKey;
} EVP_CIPHER_CTX, *PEVP_CIPHER_CTX;

void evp_aes_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);
void evp_aes_decrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);
int AES_set_encrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);
int EVP_aes_128_cbc(void);
int EVP_EncryptInit(EVP_CIPHER_CTX *ctx, int type, unsigned char *key, unsigned char *iv);
int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen, unsigned char *inbuf, int inlen);
int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen);
int EVP_DecryptInit(EVP_CIPHER_CTX *ctx, int type, unsigned char *key, unsigned char *iv);
int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen, unsigned char *inbuf, int inlen);
int EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen);
void evp_aes_cbc_encrypt(const unsigned char *in, unsigned char *out,
		     const unsigned long length, const AES_KEY *key,
		     unsigned char *ivec, const int enc);

void WscEncryptData(
    unsigned char *plainText,   int ptx_len,
    unsigned char *key, unsigned char *iv,
    unsigned char *cipherText,  int *ctx_len);
    
void WscDecryptData(
    unsigned char *cipherText,  int ctx_len, 
    unsigned char *key, unsigned char *iv,
    unsigned char *plainText,   int *ptx_len);	

#define AES_CBC_Encrypt(Plain, PlainL, Key, KeyL, IV, IVL, Cipher, CipherL) \
    WscEncryptData((Plain), (PlainL), (Key), (IV), (Cipher), (int *) (CipherL));

#define AES_CBC_Decrypt(Cipher, CipherL, Key, KeyL, IV, IVL, Plain, PlainL) \
    WscDecryptData((Cipher), (CipherL), (Key), (IV), (Plain), (int *) (PlainL));

typedef	struct
{
	uint32 erk[64];		/* encryption round	keys */
	uint32 drk[64];		/* decryption round	keys */
	int	nr;				/* number of rounds	*/
}
aes_context;

int	 rtmp_aes_set_key( aes_context *ctx,	uint8 *key,	int	nbits );
void rtmp_aes_encrypt( aes_context *ctx,	uint8 input[16], uint8 output[16] );
void rtmp_aes_decrypt( aes_context *ctx,	uint8 input[16], uint8 output[16] );

VOID AES_GTK_KEY_WRAP( 
	IN UCHAR *key,
	IN UCHAR *plaintext,
	IN UINT p_len,
	OUT UCHAR *ciphertext,
	OUT UINT *c_len);
    
VOID AES_GTK_KEY_UNWRAP( 
	IN	UCHAR	*key,
	OUT	UCHAR	*plaintext,
	OUT	UINT    *p_len,
	IN	UCHAR	*ciphertext,
	IN	UINT    c_len);

#define AES_Key_Wrap(Plain, PlainL, Key, KeyL, Cipher, CipherL) \
    AES_GTK_KEY_WRAP((Key), (Plain), (PlainL), (Cipher), (CipherL))

#define AES_Key_Unwrap(Cipher, CipherL, Key, KeyL, Plain, PlainL) \
    AES_GTK_KEY_UNWRAP((Key), (Plain), (PlainL), (Cipher), (CipherL))
#endif /* CRYPT_GPL_ALGORITHM */

/* AES definition & structure */
#define AES_STATE_ROWS 4     /* Block size: 4*4*8 = 128 bits */
#define AES_STATE_COLUMNS 4
#define AES_BLOCK_SIZES AES_STATE_ROWS*AES_STATE_COLUMNS
#define AES_KEY_ROWS 4
#define AES_KEY_COLUMNS 8    /*Key length: 4*{4,6,8}*8 = 128, 192, 256 bits */
#define AES_KEY128_LENGTH 16
#define AES_KEY192_LENGTH 24
#define AES_KEY256_LENGTH 32
#define AES_CBC_IV_LENGTH 16

typedef struct {
    UINT8 State[AES_STATE_ROWS][AES_STATE_COLUMNS];
    UINT8 KeyWordExpansion[AES_KEY_ROWS][AES_KEY_ROWS*((AES_KEY256_LENGTH >> 2) + 6 + 1)];
} AES_CTX_STRUC, *PAES_CTX_STRUC;


/* AES operations */
VOID RT_AES_KeyExpansion (
    IN UINT8 Key[],
    IN UINT KeyLength,
    INOUT AES_CTX_STRUC *paes_ctx);

VOID RT_AES_Encrypt (
    IN UINT8 PlainBlock[],
    IN UINT PlainBlockSize,
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 CipherBlock[],
    INOUT UINT *CipherBlockSize);

VOID RT_AES_Decrypt (
    IN UINT8 CipherBlock[],
    IN UINT CipherBlockSize,
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 PlainBlock[],
    INOUT UINT *PlainBlockSize);

/* AES Counter with CBC-MAC operations */
VOID AES_CCM_MAC (
    IN UINT8 Payload[],
    IN UINT  PayloadLength,
    IN UINT8 Key[],
    IN UINT  KeyLength,
    IN UINT8 Nonce[],
    IN UINT  NonceLength,
    IN UINT8 AAD[],
    IN UINT  AADLength,
    IN UINT  MACLength,
    OUT UINT8 MACText[]);

INT AES_CCM_Encrypt (
    IN UINT8 PlainText[],
    IN UINT  PlainTextLength,
    IN UINT8 Key[],
    IN UINT  KeyLength,
    IN UINT8 Nonce[],
    IN UINT  NonceLength,
    IN UINT8 AAD[],
    IN UINT  AADLength,
    IN UINT  MACLength,
    OUT UINT8 CipherText[],
    INOUT UINT *CipherTextLength);

INT AES_CCM_Decrypt (
    IN UINT8 CipherText[],
    IN UINT  CipherTextLength,
    IN UINT8 Key[],
    IN UINT  KeyLength,
    IN UINT8 Nonce[],
    IN UINT  NonceLength,
    IN UINT8 AAD[],
    IN UINT  AADLength,
    IN UINT  MACLength,
    OUT UINT8 PlainText[],
    INOUT UINT *PlainTextLength);

/* AES-CMAC operations */
VOID AES_CMAC_GenerateSubKey (
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 SubKey1[],
    OUT UINT8 SubKey2[]);

VOID AES_CMAC (
    IN UINT8 PlainText[],
    IN UINT PlainTextLength,
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 MACText[],
    INOUT UINT *MACTextLength);


#ifndef CRYPT_GPL_ALGORITHM

/* AES-CBC operations */
VOID AES_CBC_Encrypt (
    IN UINT8 PlainText[],
    IN UINT PlainTextLength,
    IN UINT8 Key[],
    IN UINT KeyLength,
    IN UINT8 IV[],
    IN UINT IVLength,
    OUT UINT8 CipherText[],
    INOUT UINT *CipherTextLength);

VOID AES_CBC_Decrypt (
    IN UINT8 CipherText[],
    IN UINT CipherTextLength,
    IN UINT8 Key[],
    IN UINT KeyLength,
    IN UINT8 IV[],
    IN UINT IVLength,
    OUT UINT8 PlainText[],
    INOUT UINT *PlainTextLength);

/* AES key wrap operations */
INT AES_Key_Wrap (
    IN UINT8 PlainText[],
    IN UINT  PlainTextLength,
    IN UINT8 Key[],
    IN UINT  KeyLength,
    OUT UINT8 CipherText[],
    OUT UINT *CipherTextLength);
        
INT AES_Key_Unwrap (
    IN UINT8 CipherText[],
    IN UINT  CipherTextLength,
    IN UINT8 Key[],
    IN UINT  KeyLength,
    OUT UINT8 PlainText [],
    OUT UINT *PlainTextLength);

#endif /* CRYPT_GPL_ALGORITHM */

#endif /* __CRYPT_AES_H__ */

