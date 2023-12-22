#ifndef _EVP_ENC_H
#define _EVP_ENC_H


#if 1
/*
/*#undef SWAP32 */
/*#define SWAP32(x) \ */
/*    ((unsigned long)( \ */
/*    (((unsigned long)(x) & (unsigned long) 0x000000ffUL) << 24) | \ */
/*    (((unsigned long)(x) & (unsigned long) 0x0000ff00UL) <<  8) | \ */
/*    (((unsigned long)(x) & (unsigned long) 0x00ff0000UL) >>  8) | \ */
/*    (((unsigned long)(x) & (unsigned long) 0xff000000UL) >> 24) )) */
*/
#ifndef WINBOND
#define GETU32(p) cpu2be32(get_unaligned32((UINT32 *) (p))) /*cpu2be32((*(UINT32 *)(p))) */
#else
UINT32 WINBOND_GET32(unsigned char *source);
UINT16 WINBOND_GET16(unsigned char *source);
#define GETU32(p) (WINBOND_GET32(p))
#endif /* WINBOND */
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
int RT_EVP_aes_128_cbc(void);
int RT_EVP_EncryptInit(EVP_CIPHER_CTX *ctx, int type, unsigned char *key, unsigned char *iv);
int RT_EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen, unsigned char *inbuf, int inlen);
int RT_EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen);
int RT_EVP_DecryptInit(EVP_CIPHER_CTX *ctx, int type, unsigned char *key, unsigned char *iv);
int RT_EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen, unsigned char *inbuf, int inlen);
int RT_EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *outbuf, int *outlen);
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

int WscDeriveKey(
    unsigned char *kdk, unsigned int kdk_len, 
    unsigned char *prsnlString, unsigned int str_len,
    unsigned char *key, unsigned int keyBits );

#endif  /* _EVP_ENC_H */
