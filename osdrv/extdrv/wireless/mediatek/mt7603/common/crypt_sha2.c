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
    SHA2

    Abstract:
    FIPS 180-2: Secure Hash Standard (SHS)
    
    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
    Eddy        2008/11/24      Create SHA1
    Eddy        2008/07/23      Create SHA256
***************************************************************************/
#endif /* CRYPT_GPL_ALGORITHM */

#ifdef CRYPT_TESTPLAN
#include "crypt_testplan.h"
#else
#include "rt_config.h"
#endif /* CRYPT_TESTPLAN */

#ifdef CRYPT_GPL_ALGORITHM

#if defined(__cplusplus)
extern "C"
{
#endif

#ifdef _MSC_VER
#pragma intrinsic(memcpy)
#endif

#if 0 && defined(_MSC_VER)
#define rotl32 _lrotl
#define rotr32 _lrotr
#else
#define rotl32(x,n)   (((x) << n) | ((x) >> (32 - n)))
#define rotr32(x,n)   (((x) >> n) | ((x) << (32 - n)))
#endif

#if !defined(bswap_32)
#define bswap_32(x) ((rotr32((x), 24) & 0x00ff00ff) | (rotr32((x), 8) & 0xff00ff00))
#endif

#ifdef	__BIG_ENDIAN
#undef  SWAP_BYTES
#else
#define SWAP_BYTES
#endif

#if 0

#define ch(x,y,z)       (((x) & (y)) ^ (~(x) & (z)))
#define maj(x,y,z)      (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#else   /* Thanks to Rich Schroeppel and Colin Plumb for the following      */

#define ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define maj(x,y,z)      (((x) & (y)) | ((z) & ((x) ^ (y))))

#endif

/* round transforms for SHA256 and SHA512 compression functions */

#define vf(n,i) v[(n - i) & 7]

#define hf(i) (p[i & 15] += \
    g_1(p[(i + 14) & 15]) + p[(i + 9) & 15] + g_0(p[(i + 1) & 15]))

#define v_cycle(i,j)                                \
    vf(7,i) += (j ? hf(i) : p[i]) + k_0[i+j]        \
    + s_1(vf(4,i)) + ch(vf(4,i),vf(5,i),vf(6,i));   \
    vf(3,i) += vf(7,i);                             \
    vf(7,i) += s_0(vf(0,i))+ maj(vf(0,i),vf(1,i),vf(2,i))

#if defined(SHA_224) || defined(SHA_256)

#define SHA256_MASK (SHA256_BLOCK_SIZE - 1)

#if defined(SWAP_BYTES)
#define bsw_32(p,n) \
    { int _i = (n); while(_i--) ((uint_32t*)p)[_i] = bswap_32(((uint_32t*)p)[_i]); }
#else
#define bsw_32(p,n)
#endif

#define s_0(x)  (rotr32((x),  2) ^ rotr32((x), 13) ^ rotr32((x), 22))
#define s_1(x)  (rotr32((x),  6) ^ rotr32((x), 11) ^ rotr32((x), 25))
#define g_0(x)  (rotr32((x),  7) ^ rotr32((x), 18) ^ ((x) >>  3))
#define g_1(x)  (rotr32((x), 17) ^ rotr32((x), 19) ^ ((x) >> 10))
#define k_0     k256

/* rotated SHA256 round definition. Rather than swapping variables as in    */
/* FIPS-180, different variables are 'rotated' on each round, returning     */
/* to their starting positions every eight rounds                           */

#define q(n)  v##n

#define one_cycle(a,b,c,d,e,f,g,h,k,w)  \
    q(h) += s_1(q(e)) + ch(q(e), q(f), q(g)) + k + w; \
    q(d) += q(h); q(h) += s_0(q(a)) + maj(q(a), q(b), q(c))

/* SHA256 mixing data   */

const uint_32t k256[64] =
{   0x428a2f98ul, 0x71374491ul, 0xb5c0fbcful, 0xe9b5dba5ul,
    0x3956c25bul, 0x59f111f1ul, 0x923f82a4ul, 0xab1c5ed5ul,
    0xd807aa98ul, 0x12835b01ul, 0x243185beul, 0x550c7dc3ul,
    0x72be5d74ul, 0x80deb1feul, 0x9bdc06a7ul, 0xc19bf174ul,
    0xe49b69c1ul, 0xefbe4786ul, 0x0fc19dc6ul, 0x240ca1ccul,
    0x2de92c6ful, 0x4a7484aaul, 0x5cb0a9dcul, 0x76f988daul,
    0x983e5152ul, 0xa831c66dul, 0xb00327c8ul, 0xbf597fc7ul,
    0xc6e00bf3ul, 0xd5a79147ul, 0x06ca6351ul, 0x14292967ul,
    0x27b70a85ul, 0x2e1b2138ul, 0x4d2c6dfcul, 0x53380d13ul,
    0x650a7354ul, 0x766a0abbul, 0x81c2c92eul, 0x92722c85ul,
    0xa2bfe8a1ul, 0xa81a664bul, 0xc24b8b70ul, 0xc76c51a3ul,
    0xd192e819ul, 0xd6990624ul, 0xf40e3585ul, 0x106aa070ul,
    0x19a4c116ul, 0x1e376c08ul, 0x2748774cul, 0x34b0bcb5ul,
    0x391c0cb3ul, 0x4ed8aa4aul, 0x5b9cca4ful, 0x682e6ff3ul,
    0x748f82eeul, 0x78a5636ful, 0x84c87814ul, 0x8cc70208ul,
    0x90befffaul, 0xa4506cebul, 0xbef9a3f7ul, 0xc67178f2ul,
};

/* Compile 64 bytes of hash data into SHA256 digest value   */
/* NOTE: this routine assumes that the byte order in the    */
/* ctx->wbuf[] at this point is such that low address bytes */
/* in the ORIGINAL byte stream will go into the high end of */
/* words on BOTH big and little endian systems              */

void_ret sha256_compile(sha256_ctx ctx[1])
{
#if !defined(UNROLL_SHA2)

    uint_32t j, *p = ctx->wbuf, v[8];

    memcpy(v, ctx->hash, 8 * sizeof(uint_32t));

    for(j = 0; j < 64; j += 16)
    {
        v_cycle( 0, j); v_cycle( 1, j);
        v_cycle( 2, j); v_cycle( 3, j);
        v_cycle( 4, j); v_cycle( 5, j);
        v_cycle( 6, j); v_cycle( 7, j);
        v_cycle( 8, j); v_cycle( 9, j);
        v_cycle(10, j); v_cycle(11, j);
        v_cycle(12, j); v_cycle(13, j);
        v_cycle(14, j); v_cycle(15, j);
    }

    ctx->hash[0] += v[0]; ctx->hash[1] += v[1];
    ctx->hash[2] += v[2]; ctx->hash[3] += v[3];
    ctx->hash[4] += v[4]; ctx->hash[5] += v[5];
    ctx->hash[6] += v[6]; ctx->hash[7] += v[7];

#else

    uint_32t *p = ctx->wbuf,v0,v1,v2,v3,v4,v5,v6,v7;

    v0 = ctx->hash[0]; v1 = ctx->hash[1];
    v2 = ctx->hash[2]; v3 = ctx->hash[3];
    v4 = ctx->hash[4]; v5 = ctx->hash[5];
    v6 = ctx->hash[6]; v7 = ctx->hash[7];

    one_cycle(0,1,2,3,4,5,6,7,k256[ 0],p[ 0]);
    one_cycle(7,0,1,2,3,4,5,6,k256[ 1],p[ 1]);
    one_cycle(6,7,0,1,2,3,4,5,k256[ 2],p[ 2]);
    one_cycle(5,6,7,0,1,2,3,4,k256[ 3],p[ 3]);
    one_cycle(4,5,6,7,0,1,2,3,k256[ 4],p[ 4]);
    one_cycle(3,4,5,6,7,0,1,2,k256[ 5],p[ 5]);
    one_cycle(2,3,4,5,6,7,0,1,k256[ 6],p[ 6]);
    one_cycle(1,2,3,4,5,6,7,0,k256[ 7],p[ 7]);
    one_cycle(0,1,2,3,4,5,6,7,k256[ 8],p[ 8]);
    one_cycle(7,0,1,2,3,4,5,6,k256[ 9],p[ 9]);
    one_cycle(6,7,0,1,2,3,4,5,k256[10],p[10]);
    one_cycle(5,6,7,0,1,2,3,4,k256[11],p[11]);
    one_cycle(4,5,6,7,0,1,2,3,k256[12],p[12]);
    one_cycle(3,4,5,6,7,0,1,2,k256[13],p[13]);
    one_cycle(2,3,4,5,6,7,0,1,k256[14],p[14]);
    one_cycle(1,2,3,4,5,6,7,0,k256[15],p[15]);

    one_cycle(0,1,2,3,4,5,6,7,k256[16],hf( 0));
    one_cycle(7,0,1,2,3,4,5,6,k256[17],hf( 1));
    one_cycle(6,7,0,1,2,3,4,5,k256[18],hf( 2));
    one_cycle(5,6,7,0,1,2,3,4,k256[19],hf( 3));
    one_cycle(4,5,6,7,0,1,2,3,k256[20],hf( 4));
    one_cycle(3,4,5,6,7,0,1,2,k256[21],hf( 5));
    one_cycle(2,3,4,5,6,7,0,1,k256[22],hf( 6));
    one_cycle(1,2,3,4,5,6,7,0,k256[23],hf( 7));
    one_cycle(0,1,2,3,4,5,6,7,k256[24],hf( 8));
    one_cycle(7,0,1,2,3,4,5,6,k256[25],hf( 9));
    one_cycle(6,7,0,1,2,3,4,5,k256[26],hf(10));
    one_cycle(5,6,7,0,1,2,3,4,k256[27],hf(11));
    one_cycle(4,5,6,7,0,1,2,3,k256[28],hf(12));
    one_cycle(3,4,5,6,7,0,1,2,k256[29],hf(13));
    one_cycle(2,3,4,5,6,7,0,1,k256[30],hf(14));
    one_cycle(1,2,3,4,5,6,7,0,k256[31],hf(15));

    one_cycle(0,1,2,3,4,5,6,7,k256[32],hf( 0));
    one_cycle(7,0,1,2,3,4,5,6,k256[33],hf( 1));
    one_cycle(6,7,0,1,2,3,4,5,k256[34],hf( 2));
    one_cycle(5,6,7,0,1,2,3,4,k256[35],hf( 3));
    one_cycle(4,5,6,7,0,1,2,3,k256[36],hf( 4));
    one_cycle(3,4,5,6,7,0,1,2,k256[37],hf( 5));
    one_cycle(2,3,4,5,6,7,0,1,k256[38],hf( 6));
    one_cycle(1,2,3,4,5,6,7,0,k256[39],hf( 7));
    one_cycle(0,1,2,3,4,5,6,7,k256[40],hf( 8));
    one_cycle(7,0,1,2,3,4,5,6,k256[41],hf( 9));
    one_cycle(6,7,0,1,2,3,4,5,k256[42],hf(10));
    one_cycle(5,6,7,0,1,2,3,4,k256[43],hf(11));
    one_cycle(4,5,6,7,0,1,2,3,k256[44],hf(12));
    one_cycle(3,4,5,6,7,0,1,2,k256[45],hf(13));
    one_cycle(2,3,4,5,6,7,0,1,k256[46],hf(14));
    one_cycle(1,2,3,4,5,6,7,0,k256[47],hf(15));

    one_cycle(0,1,2,3,4,5,6,7,k256[48],hf( 0));
    one_cycle(7,0,1,2,3,4,5,6,k256[49],hf( 1));
    one_cycle(6,7,0,1,2,3,4,5,k256[50],hf( 2));
    one_cycle(5,6,7,0,1,2,3,4,k256[51],hf( 3));
    one_cycle(4,5,6,7,0,1,2,3,k256[52],hf( 4));
    one_cycle(3,4,5,6,7,0,1,2,k256[53],hf( 5));
    one_cycle(2,3,4,5,6,7,0,1,k256[54],hf( 6));
    one_cycle(1,2,3,4,5,6,7,0,k256[55],hf( 7));
    one_cycle(0,1,2,3,4,5,6,7,k256[56],hf( 8));
    one_cycle(7,0,1,2,3,4,5,6,k256[57],hf( 9));
    one_cycle(6,7,0,1,2,3,4,5,k256[58],hf(10));
    one_cycle(5,6,7,0,1,2,3,4,k256[59],hf(11));
    one_cycle(4,5,6,7,0,1,2,3,k256[60],hf(12));
    one_cycle(3,4,5,6,7,0,1,2,k256[61],hf(13));
    one_cycle(2,3,4,5,6,7,0,1,k256[62],hf(14));
    one_cycle(1,2,3,4,5,6,7,0,k256[63],hf(15));

    ctx->hash[0] += v0; ctx->hash[1] += v1;
    ctx->hash[2] += v2; ctx->hash[3] += v3;
    ctx->hash[4] += v4; ctx->hash[5] += v5;
    ctx->hash[6] += v6; ctx->hash[7] += v7;
#endif
}

/* SHA256 hash data in an array of bytes into hash buffer   */
/* and call the hash_compile function as required.          */

void_ret sha256_hash(const unsigned char data[], unsigned int len, sha256_ctx ctx[1])
{   uint_32t pos = (uint_32t)(ctx->count[0] & SHA256_MASK),
             space = SHA256_BLOCK_SIZE - pos;
    const unsigned char *sp = data;

    if((ctx->count[0] += len) < len)
        ++(ctx->count[1]);

    while(len >= space)     /* tranfer whole blocks while possible  */
    {
        memcpy(((unsigned char*)ctx->wbuf) + pos, sp, space);
        sp += space; len -= space; space = SHA256_BLOCK_SIZE; pos = 0;
        bsw_32(ctx->wbuf, SHA256_BLOCK_SIZE >> 2)
        sha256_compile(ctx);
    }

    memcpy(((unsigned char*)ctx->wbuf) + pos, sp, len);
}

/* SHA256 Final padding and digest calculation  */

static void sha_end1(unsigned char hval[], sha256_ctx ctx[1], const unsigned int hlen)
{   uint_32t    i = (uint_32t)(ctx->count[0] & SHA256_MASK);

    /* put bytes in the buffer in an order in which references to   */
    /* 32-bit words will put bytes with lower addresses into the    */
    /* top of 32 bit words on BOTH big and little endian machines   */
    bsw_32(ctx->wbuf, (i + 3) >> 2)

    /* we now need to mask valid bytes and add the padding which is */
    /* a single 1 bit and as many zero bits as necessary. Note that */
    /* we can always add the first padding byte here because the    */
    /* buffer always has at least one empty slot                    */
    ctx->wbuf[i >> 2] &= 0xffffff80 << 8 * (~i & 3);
    ctx->wbuf[i >> 2] |= 0x00000080 << 8 * (~i & 3);

    /* we need 9 or more empty positions, one for the padding byte  */
    /* (above) and eight for the length count.  If there is not     */
    /* enough space pad and empty the buffer                        */
    if(i > SHA256_BLOCK_SIZE - 9)
    {
        if(i < 60) ctx->wbuf[15] = 0;
        sha256_compile(ctx);
        i = 0;
    }
    else    /* compute a word index for the empty buffer positions  */
        i = (i >> 2) + 1;

    while(i < 14) /* and zero pad all but last two positions        */
        ctx->wbuf[i++] = 0;

    /* the following 32-bit length fields are assembled in the      */
    /* wrong byte order on little endian machines but this is       */
    /* corrected later since they are only ever used as 32-bit      */
    /* word values.                                                 */
    ctx->wbuf[14] = (ctx->count[1] << 3) | (ctx->count[0] >> 29);
    ctx->wbuf[15] = ctx->count[0] << 3;
    sha256_compile(ctx);

    /* extract the hash value as bytes in case the hash buffer is   */
    /* mislaigned for 32-bit words                                  */
    for(i = 0; i < hlen; ++i)
        hval[i] = (unsigned char)(ctx->hash[i >> 2] >> (8 * (~i & 3)));
}

#endif

#if defined(SHA_224)

const uint_32t i224[8] =
{
    0xc1059ed8ul, 0x367cd507ul, 0x3070dd17ul, 0xf70e5939ul,
    0xffc00b31ul, 0x68581511ul, 0x64f98fa7ul, 0xbefa4fa4ul
};

void_ret sha224_begin(sha224_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i224, 8 * sizeof(uint_32t));
}

void_ret sha224_end(unsigned char hval[], sha224_ctx ctx[1])
{
    sha_end1(hval, ctx, SHA224_DIGEST_SIZE);
}

void_ret sha224(unsigned char hval[], const unsigned char data[], unsigned int len)
{   sha224_ctx  cx[1];

    sha224_begin(cx);
    sha224_hash(data, len, cx);
    sha_end1(hval, cx, SHA224_DIGEST_SIZE);
}

#endif

#if defined(SHA_256)

const uint_32t i256[8] =
{
    0x6a09e667ul, 0xbb67ae85ul, 0x3c6ef372ul, 0xa54ff53aul,
    0x510e527ful, 0x9b05688cul, 0x1f83d9abul, 0x5be0cd19ul
};

void_ret sha256_begin(sha256_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i256, 8 * sizeof(uint_32t));
}

void_ret sha256_end(unsigned char hval[], sha256_ctx ctx[1])
{
    sha_end1(hval, ctx, SHA256_DIGEST_SIZE);
}

void_ret sha256(unsigned char hval[], const unsigned char data[], unsigned int len)
{   sha256_ctx  cx[1];

    sha256_begin(cx);
    sha256_hash(data, len, cx);
    sha_end1(hval, cx, SHA256_DIGEST_SIZE);
}

#endif

#if defined(SHA_384) || defined(SHA_512)

#define SHA512_MASK (SHA512_BLOCK_SIZE - 1)

#define rotr64(x,n)   (((x) >> n) | ((x) << (64 - n)))

#if !defined(bswap_64)
#define bswap_64(x) (((uint_64t)(bswap_32((uint_32t)(x)))) << 32 | bswap_32((uint_32t)((x) >> 32)))
#endif

#if defined(SWAP_BYTES)
#define bsw_64(p,n) \
    { int _i = (n); while(_i--) ((uint_64t*)p)[_i] = bswap_64(((uint_64t*)p)[_i]); }
#else
#define bsw_64(p,n)
#endif

/* SHA512 mixing function definitions   */

#ifdef   s_0
# undef  s_0
# undef  s_1
# undef  g_0
# undef  g_1
# undef  k_0
#endif

#define s_0(x)  (rotr64((x), 28) ^ rotr64((x), 34) ^ rotr64((x), 39))
#define s_1(x)  (rotr64((x), 14) ^ rotr64((x), 18) ^ rotr64((x), 41))
#define g_0(x)  (rotr64((x),  1) ^ rotr64((x),  8) ^ ((x) >>  7))
#define g_1(x)  (rotr64((x), 19) ^ rotr64((x), 61) ^ ((x) >>  6))
#define k_0     k512

/* SHA384/SHA512 mixing data    */

const uint_64t  k512[80] =
{
    li_64(428a2f98d728ae22), li_64(7137449123ef65cd),
    li_64(b5c0fbcfec4d3b2f), li_64(e9b5dba58189dbbc),
    li_64(3956c25bf348b538), li_64(59f111f1b605d019),
    li_64(923f82a4af194f9b), li_64(ab1c5ed5da6d8118),
    li_64(d807aa98a3030242), li_64(12835b0145706fbe),
    li_64(243185be4ee4b28c), li_64(550c7dc3d5ffb4e2),
    li_64(72be5d74f27b896f), li_64(80deb1fe3b1696b1),
    li_64(9bdc06a725c71235), li_64(c19bf174cf692694),
    li_64(e49b69c19ef14ad2), li_64(efbe4786384f25e3),
    li_64(0fc19dc68b8cd5b5), li_64(240ca1cc77ac9c65),
    li_64(2de92c6f592b0275), li_64(4a7484aa6ea6e483),
    li_64(5cb0a9dcbd41fbd4), li_64(76f988da831153b5),
    li_64(983e5152ee66dfab), li_64(a831c66d2db43210),
    li_64(b00327c898fb213f), li_64(bf597fc7beef0ee4),
    li_64(c6e00bf33da88fc2), li_64(d5a79147930aa725),
    li_64(06ca6351e003826f), li_64(142929670a0e6e70),
    li_64(27b70a8546d22ffc), li_64(2e1b21385c26c926),
    li_64(4d2c6dfc5ac42aed), li_64(53380d139d95b3df),
    li_64(650a73548baf63de), li_64(766a0abb3c77b2a8),
    li_64(81c2c92e47edaee6), li_64(92722c851482353b),
    li_64(a2bfe8a14cf10364), li_64(a81a664bbc423001),
    li_64(c24b8b70d0f89791), li_64(c76c51a30654be30),
    li_64(d192e819d6ef5218), li_64(d69906245565a910),
    li_64(f40e35855771202a), li_64(106aa07032bbd1b8),
    li_64(19a4c116b8d2d0c8), li_64(1e376c085141ab53),
    li_64(2748774cdf8eeb99), li_64(34b0bcb5e19b48a8),
    li_64(391c0cb3c5c95a63), li_64(4ed8aa4ae3418acb),
    li_64(5b9cca4f7763e373), li_64(682e6ff3d6b2b8a3),
    li_64(748f82ee5defb2fc), li_64(78a5636f43172f60),
    li_64(84c87814a1f0ab72), li_64(8cc702081a6439ec),
    li_64(90befffa23631e28), li_64(a4506cebde82bde9),
    li_64(bef9a3f7b2c67915), li_64(c67178f2e372532b),
    li_64(ca273eceea26619c), li_64(d186b8c721c0c207),
    li_64(eada7dd6cde0eb1e), li_64(f57d4f7fee6ed178),
    li_64(06f067aa72176fba), li_64(0a637dc5a2c898a6),
    li_64(113f9804bef90dae), li_64(1b710b35131c471b),
    li_64(28db77f523047d84), li_64(32caab7b40c72493),
    li_64(3c9ebe0a15c9bebc), li_64(431d67c49c100d4c),
    li_64(4cc5d4becb3e42b6), li_64(597f299cfc657e2a),
    li_64(5fcb6fab3ad6faec), li_64(6c44198c4a475817)
};

/* Compile 128 bytes of hash data into SHA384/512 digest    */
/* NOTE: this routine assumes that the byte order in the    */
/* ctx->wbuf[] at this point is such that low address bytes */
/* in the ORIGINAL byte stream will go into the high end of */
/* words on BOTH big and little endian systems              */

void_ret sha512_compile(sha512_ctx ctx[1])
{   uint_64t    v[8], *p = ctx->wbuf;
    uint_32t    j;

    memcpy(v, ctx->hash, 8 * sizeof(uint_64t));

    for(j = 0; j < 80; j += 16)
    {
        v_cycle( 0, j); v_cycle( 1, j);
        v_cycle( 2, j); v_cycle( 3, j);
        v_cycle( 4, j); v_cycle( 5, j);
        v_cycle( 6, j); v_cycle( 7, j);
        v_cycle( 8, j); v_cycle( 9, j);
        v_cycle(10, j); v_cycle(11, j);
        v_cycle(12, j); v_cycle(13, j);
        v_cycle(14, j); v_cycle(15, j);
    }

    ctx->hash[0] += v[0]; ctx->hash[1] += v[1];
    ctx->hash[2] += v[2]; ctx->hash[3] += v[3];
    ctx->hash[4] += v[4]; ctx->hash[5] += v[5];
    ctx->hash[6] += v[6]; ctx->hash[7] += v[7];
}

/* Compile 128 bytes of hash data into SHA256 digest value  */
/* NOTE: this routine assumes that the byte order in the    */
/* ctx->wbuf[] at this point is in such an order that low   */
/* address bytes in the ORIGINAL byte stream placed in this */
/* buffer will now go to the high end of words on BOTH big  */
/* and little endian systems                                */

void_ret sha512_hash(const unsigned char data[], unsigned int len, sha512_ctx ctx[1])
{   uint_32t pos = (uint_32t)(ctx->count[0] & SHA512_MASK),
             space = SHA512_BLOCK_SIZE - pos;
    const unsigned char *sp = data;

    if((ctx->count[0] += len) < len)
        ++(ctx->count[1]);

    while(len >= space)     /* tranfer whole blocks while possible  */
    {
        memcpy(((unsigned char*)ctx->wbuf) + pos, sp, space);
        sp += space; len -= space; space = SHA512_BLOCK_SIZE; pos = 0;
        bsw_64(ctx->wbuf, SHA512_BLOCK_SIZE >> 3);
        sha512_compile(ctx);
    }

    memcpy(((unsigned char*)ctx->wbuf) + pos, sp, len);
}

/* SHA384/512 Final padding and digest calculation  */

static void sha_end2(unsigned char hval[], sha512_ctx ctx[1], const unsigned int hlen)
{   uint_32t    i = (uint_32t)(ctx->count[0] & SHA512_MASK);

    /* put bytes in the buffer in an order in which references to   */
    /* 32-bit words will put bytes with lower addresses into the    */
    /* top of 32 bit words on BOTH big and little endian machines   */
    bsw_64(ctx->wbuf, (i + 7) >> 3);

    /* we now need to mask valid bytes and add the padding which is */
    /* a single 1 bit and as many zero bits as necessary. Note that */
    /* we can always add the first padding byte here because the    */
    /* buffer always has at least one empty slot                    */
    ctx->wbuf[i >> 3] &= li_64(ffffffffffffff00) << 8 * (~i & 7);
    ctx->wbuf[i >> 3] |= li_64(0000000000000080) << 8 * (~i & 7);

    /* we need 17 or more empty byte positions, one for the padding */
    /* byte (above) and sixteen for the length count.  If there is  */
    /* not enough space pad and empty the buffer                    */
    if(i > SHA512_BLOCK_SIZE - 17)
    {
        if(i < 120) ctx->wbuf[15] = 0;
        sha512_compile(ctx);
        i = 0;
    }
    else
        i = (i >> 3) + 1;

    while(i < 14)
        ctx->wbuf[i++] = 0;

    /* the following 64-bit length fields are assembled in the      */
    /* wrong byte order on little endian machines but this is       */
    /* corrected later since they are only ever used as 64-bit      */
    /* word values.                                                 */
    ctx->wbuf[14] = (ctx->count[1] << 3) | (ctx->count[0] >> 61);
    ctx->wbuf[15] = ctx->count[0] << 3;
    sha512_compile(ctx);

    /* extract the hash value as bytes in case the hash buffer is   */
    /* misaligned for 32-bit words                                  */
    for(i = 0; i < hlen; ++i)
        hval[i] = (unsigned char)(ctx->hash[i >> 3] >> (8 * (~i & 7)));
}

#endif

#if defined(SHA_384)

/* SHA384 initialisation data   */

const uint_64t  i384[80] =
{
    li_64(cbbb9d5dc1059ed8), li_64(629a292a367cd507),
    li_64(9159015a3070dd17), li_64(152fecd8f70e5939),
    li_64(67332667ffc00b31), li_64(8eb44a8768581511),
    li_64(db0c2e0d64f98fa7), li_64(47b5481dbefa4fa4)
};

void_ret sha384_begin(sha384_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i384, 8 * sizeof(uint_64t));
}

void_ret sha384_end(unsigned char hval[], sha384_ctx ctx[1])
{
    sha_end2(hval, ctx, SHA384_DIGEST_SIZE);
}

void_ret sha384(unsigned char hval[], const unsigned char data[], unsigned int len)
{   sha384_ctx  cx[1];

    sha384_begin(cx);
    sha384_hash(data, len, cx);
    sha_end2(hval, cx, SHA384_DIGEST_SIZE);
}

#endif

#if defined(SHA_512)

/* SHA512 initialisation data   */

const uint_64t  i512[80] =
{
    li_64(6a09e667f3bcc908), li_64(bb67ae8584caa73b),
    li_64(3c6ef372fe94f82b), li_64(a54ff53a5f1d36f1),
    li_64(510e527fade682d1), li_64(9b05688c2b3e6c1f),
    li_64(1f83d9abfb41bd6b), li_64(5be0cd19137e2179)
};

void_ret sha512_begin(sha512_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i512, 8 * sizeof(uint_64t));
}

void_ret sha512_end(unsigned char hval[], sha512_ctx ctx[1])
{
    sha_end2(hval, ctx, SHA512_DIGEST_SIZE);
}

void_ret sha512(unsigned char hval[], const unsigned char data[], unsigned int len)
{   sha512_ctx  cx[1];

    sha512_begin(cx);
    sha512_hash(data, len, cx);
    sha_end2(hval, cx, SHA512_DIGEST_SIZE);
}

#endif

#if defined(SHA_2)

#define CTX_224(x)  ((x)->uu->ctx256)
#define CTX_256(x)  ((x)->uu->ctx256)
#define CTX_384(x)  ((x)->uu->ctx512)
#define CTX_512(x)  ((x)->uu->ctx512)

/* SHA2 initialisation */

int_ret sha2_begin(unsigned int len, sha2_ctx ctx[1])
{
    switch(len)
    {
#if defined(SHA_224)
        case 224:
        case  28:   CTX_256(ctx)->count[0] = CTX_256(ctx)->count[1] = 0;
                    memcpy(CTX_256(ctx)->hash, i224, 32);
                    ctx->sha2_len = 28; return EXIT_SUCCESS;
#endif
#if defined(SHA_256)
        case 256:
        case  32:   CTX_256(ctx)->count[0] = CTX_256(ctx)->count[1] = 0;
                    memcpy(CTX_256(ctx)->hash, i256, 32);
                    ctx->sha2_len = 32; return EXIT_SUCCESS;
#endif
#if defined(SHA_384)
        case 384:
        case  48:   CTX_384(ctx)->count[0] = CTX_384(ctx)->count[1] = 0;
                    memcpy(CTX_384(ctx)->hash, i384, 64);
                    ctx->sha2_len = 48; return EXIT_SUCCESS;
#endif
#if defined(SHA_512)
        case 512:
        case  64:   CTX_512(ctx)->count[0] = CTX_512(ctx)->count[1] = 0;
                    memcpy(CTX_512(ctx)->hash, i512, 64);
                    ctx->sha2_len = 64; return EXIT_SUCCESS;
#endif
        default:    return EXIT_FAILURE;
    }
}

void_ret sha2_hash(const unsigned char data[], unsigned int len, sha2_ctx ctx[1])
{
    switch(ctx->sha2_len)
    {
#if defined(SHA_224)
        case 28: sha224_hash(data, len, CTX_224(ctx)); return;
#endif
#if defined(SHA_256)
        case 32: sha256_hash(data, len, CTX_256(ctx)); return;
#endif
#if defined(SHA_384)
        case 48: sha384_hash(data, len, CTX_384(ctx)); return;
#endif
#if defined(SHA_512)
        case 64: sha512_hash(data, len, CTX_512(ctx)); return;
#endif
    }
}

void_ret sha2_end(unsigned char hval[], sha2_ctx ctx[1])
{
    switch(ctx->sha2_len)
    {
#if defined(SHA_224)
        case 28: sha_end1(hval, CTX_224(ctx), SHA224_DIGEST_SIZE); return;
#endif
#if defined(SHA_256)
        case 32: sha_end1(hval, CTX_256(ctx), SHA256_DIGEST_SIZE); return;
#endif
#if defined(SHA_384)
        case 48: sha_end2(hval, CTX_384(ctx), SHA384_DIGEST_SIZE); return;
#endif
#if defined(SHA_512)
        case 64: sha_end2(hval, CTX_512(ctx), SHA512_DIGEST_SIZE); return;
#endif
    }
}

int_ret sha2(unsigned char hval[], unsigned int size,
                                const unsigned char data[], unsigned int len)
{   sha2_ctx    cx[1];

    if(sha2_begin(size, cx) == EXIT_SUCCESS)
    {
        sha2_hash(data, len, cx); sha2_end(hval, cx); return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

#endif

#if defined(__cplusplus)
}
#endif

#else /* CRYPT_GPL_ALGORITHM */

/* Basic operations */
#define SHR(x,n) (x >> n) /* SHR(x)^n, right shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTR(x,n,w) ((x >> n) | (x << (w - n))) /* ROTR(x)^n, circular right shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTL(x,n,w) ((x << n) | (x >> (w - n))) /* ROTL(x)^n, circular left shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTR32(x,n) ROTR(x,n,32) /* 32 bits word */
#define ROTL32(x,n) ROTL(x,n,32) /* 32 bits word */ 

/* Basic functions */
#define Ch(x,y,z) ((x & y) ^ ((~x) & z))
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
#define Parity(x,y,z) (x ^ y ^ z)

#ifdef SHA1_SUPPORT
/* SHA1 constants */
#define SHA1_MASK 0x0000000f
static const UINT32 SHA1_K[4] = {
    0x5a827999UL, 0x6ed9eba1UL, 0x8f1bbcdcUL, 0xca62c1d6UL
};
static const UINT32 SHA1_DefaultHashValue[5] = {
    0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL, 0xc3d2e1f0UL
};
#endif /* SHA1_SUPPORT */


#ifdef SHA256_SUPPORT
/* SHA256 functions */
#define Zsigma_256_0(x) (ROTR32(x,2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define Zsigma_256_1(x) (ROTR32(x,6) ^ ROTR32(x,11) ^ ROTR32(x,25))
#define Sigma_256_0(x)  (ROTR32(x,7) ^ ROTR32(x,18) ^ SHR(x,3))
#define Sigma_256_1(x)  (ROTR32(x,17) ^ ROTR32(x,19) ^ SHR(x,10))
/* SHA256 constants */
static const UINT32 SHA256_K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL, 
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL, 
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL, 
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};
static const UINT32 SHA256_DefaultHashValue[8] = {
    0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
    0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL
};
#endif /* SHA256_SUPPORT */


#ifdef SHA1_SUPPORT
/*
========================================================================
Routine Description:
    Initial SHA1_CTX_STRUC

Arguments:
    pSHA_CTX        Pointer to SHA1_CTX_STRUC

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA1_Init (
    IN  SHA1_CTX_STRUC *pSHA_CTX)
{
    NdisMoveMemory(pSHA_CTX->HashValue, SHA1_DefaultHashValue, 
        sizeof(SHA1_DefaultHashValue));
    NdisZeroMemory(pSHA_CTX->Block, SHA1_BLOCK_SIZE);
    pSHA_CTX->MessageLen = 0;
    pSHA_CTX->BlockLen   = 0;
} /* End of RT_SHA1_Init */


/*
========================================================================
Routine Description:
    SHA1 computation for one block (512 bits)

Arguments:
    pSHA_CTX        Pointer to SHA1_CTX_STRUC

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA1_Hash (
    IN  SHA1_CTX_STRUC *pSHA_CTX)
{
    UINT32 W_i,t;
    UINT32 W[80];
    UINT32 a,b,c,d,e,T,f_t = 0;
  
    /* Prepare the message schedule, {W_i}, 0 < t < 15 */
    NdisMoveMemory(W, pSHA_CTX->Block, SHA1_BLOCK_SIZE);
    for (W_i = 0; W_i < 16; W_i++) {
        W[W_i] = cpu2be32(W[W_i]); /* Endian Swap */
    } /* End of for */

    for (W_i = 16; W_i < 80; W_i++) {
        W[W_i] = ROTL32((W[W_i - 3] ^ W[W_i - 8] ^ W[W_i - 14] ^ W[W_i - 16]),1);
    } /* End of for */

        
    /* SHA256 hash computation */
    /* Initialize the working variables */
    a = pSHA_CTX->HashValue[0];
    b = pSHA_CTX->HashValue[1];
    c = pSHA_CTX->HashValue[2];
    d = pSHA_CTX->HashValue[3];
    e = pSHA_CTX->HashValue[4];

    /* 80 rounds */
    for (t = 0;t < 20;t++) {
        f_t = Ch(b,c,d);
        T = ROTL32(a,5) + f_t + e + SHA1_K[0] + W[t];
        e = d;
        d = c;
        c = ROTL32(b,30);
        b = a;
        a = T;
     } /* End of for */
    for (t = 20;t < 40;t++) {
        f_t = Parity(b,c,d);
        T = ROTL32(a,5) + f_t + e + SHA1_K[1] + W[t];
        e = d;
        d = c;
        c = ROTL32(b,30);
        b = a;
        a = T;
     } /* End of for */
    for (t = 40;t < 60;t++) {
        f_t = Maj(b,c,d);
        T = ROTL32(a,5) + f_t + e + SHA1_K[2] + W[t];
        e = d;
        d = c;
        c = ROTL32(b,30);
        b = a;
        a = T;
     } /* End of for */
    for (t = 60;t < 80;t++) {
        f_t = Parity(b,c,d);
        T = ROTL32(a,5) + f_t + e + SHA1_K[3] + W[t];
        e = d;
        d = c;
        c = ROTL32(b,30);
        b = a;
        a = T;
     } /* End of for */


     /* Compute the i^th intermediate hash value H^(i) */
     pSHA_CTX->HashValue[0] += a;
     pSHA_CTX->HashValue[1] += b;
     pSHA_CTX->HashValue[2] += c;
     pSHA_CTX->HashValue[3] += d;
     pSHA_CTX->HashValue[4] += e;

    NdisZeroMemory(pSHA_CTX->Block, SHA1_BLOCK_SIZE);
    pSHA_CTX->BlockLen = 0;
} /* End of RT_SHA1_Hash */


/*
========================================================================
Routine Description:
    The message is appended to block. If block size > 64 bytes, the SHA1_Hash 
will be called.

Arguments:
    pSHA_CTX        Pointer to SHA1_CTX_STRUC
    message         Message context
    messageLen      The length of message in bytes

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA1_Append (
    IN  SHA1_CTX_STRUC *pSHA_CTX, 
    IN  const UINT8 Message[], 
    IN  UINT MessageLen)
{
    UINT appendLen = 0;
    UINT diffLen   = 0;
    
    while (appendLen != MessageLen) {
        diffLen = MessageLen - appendLen;
        if ((pSHA_CTX->BlockLen + diffLen) <  SHA1_BLOCK_SIZE) {
            NdisMoveMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 
                Message + appendLen, diffLen);
            pSHA_CTX->BlockLen += diffLen;
            appendLen += diffLen;
        } 
        else
        {
            NdisMoveMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 
                Message + appendLen, SHA1_BLOCK_SIZE - pSHA_CTX->BlockLen);
            appendLen += (SHA1_BLOCK_SIZE - pSHA_CTX->BlockLen);
            pSHA_CTX->BlockLen = SHA1_BLOCK_SIZE;
            RT_SHA1_Hash(pSHA_CTX);
        } /* End of if */
    } /* End of while */
    pSHA_CTX->MessageLen += MessageLen;
} /* End of RT_SHA1_Append */


/*
========================================================================
Routine Description:
    1. Append bit 1 to end of the message
    2. Append the length of message in rightmost 64 bits
    3. Transform the Hash Value to digest message

Arguments:
    pSHA_CTX        Pointer to SHA1_CTX_STRUC

Return Value:
    digestMessage   Digest message

Note:
    None
========================================================================
*/
VOID RT_SHA1_End (
    IN  SHA1_CTX_STRUC *pSHA_CTX, 
    OUT UINT8 DigestMessage[])
{
    UINT index;
    UINT64 message_length_bits;

    /* Append bit 1 to end of the message */
    NdisFillMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 1, 0x80);

    /* 55 = 64 - 8 - 1: append 1 bit(1 byte) and message length (8 bytes) */
    if (pSHA_CTX->BlockLen > 55)
        RT_SHA1_Hash(pSHA_CTX);
    /* End of if */

    /* Append the length of message in rightmost 64 bits */
    message_length_bits = pSHA_CTX->MessageLen*8;
    message_length_bits = cpu2be64(message_length_bits);       
    NdisMoveMemory(&pSHA_CTX->Block[56], &message_length_bits, 8);
    RT_SHA1_Hash(pSHA_CTX);

    /* Return message digest, transform the UINT32 hash value to bytes */
    for (index = 0; index < 5;index++)
        pSHA_CTX->HashValue[index] = cpu2be32(pSHA_CTX->HashValue[index]);
        /* End of for */
    NdisMoveMemory(DigestMessage, pSHA_CTX->HashValue, SHA1_DIGEST_SIZE);
} /* End of RT_SHA1_End */


/*
========================================================================
Routine Description:
    SHA1 algorithm

Arguments:
    message         Message context
    messageLen      The length of message in bytes

Return Value:
    digestMessage   Digest message

Note:
    None
========================================================================
*/
VOID RT_SHA1 (
    IN  const UINT8 Message[], 
    IN  UINT MessageLen, 
    OUT UINT8 DigestMessage[])
{

    SHA1_CTX_STRUC sha_ctx;

    NdisZeroMemory(&sha_ctx, sizeof(SHA1_CTX_STRUC));
    RT_SHA1_Init(&sha_ctx);
    RT_SHA1_Append(&sha_ctx, Message, MessageLen);
    RT_SHA1_End(&sha_ctx, DigestMessage);
} /* End of RT_SHA1 */
#endif /* SHA1_SUPPORT */


#ifdef SHA256_SUPPORT
/*
========================================================================
Routine Description:
    Initial SHA256_CTX_STRUC

Arguments:
    pSHA_CTX    Pointer to SHA256_CTX_STRUC

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA256_Init (
    IN  SHA256_CTX_STRUC *pSHA_CTX)
{
    NdisMoveMemory(pSHA_CTX->HashValue, SHA256_DefaultHashValue, 
        sizeof(SHA256_DefaultHashValue));
    NdisZeroMemory(pSHA_CTX->Block, SHA256_BLOCK_SIZE);
    pSHA_CTX->MessageLen = 0;
    pSHA_CTX->BlockLen   = 0;
} /* End of RT_SHA256_Init */


/*
========================================================================
Routine Description:
    SHA256 computation for one block (512 bits)

Arguments:
    pSHA_CTX    Pointer to SHA256_CTX_STRUC

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA256_Hash (
    IN  SHA256_CTX_STRUC *pSHA_CTX)
{
    UINT32 W_i,t;
    UINT32 W[64];
    UINT32 a,b,c,d,e,f,g,h,T1,T2;
    
    /* Prepare the message schedule, {W_i}, 0 < t < 15 */
    NdisMoveMemory(W, pSHA_CTX->Block, SHA256_BLOCK_SIZE);
    for (W_i = 0; W_i < 16; W_i++)
        W[W_i] = cpu2be32(W[W_i]); /* Endian Swap */
        /* End of for */
 
    /* SHA256 hash computation */
    /* Initialize the working variables */
    a = pSHA_CTX->HashValue[0];
    b = pSHA_CTX->HashValue[1];
    c = pSHA_CTX->HashValue[2];
    d = pSHA_CTX->HashValue[3];
    e = pSHA_CTX->HashValue[4];
    f = pSHA_CTX->HashValue[5];
    g = pSHA_CTX->HashValue[6];
    h = pSHA_CTX->HashValue[7];

    /* 64 rounds */
    for (t = 0;t < 64;t++) {
        if (t > 15) /* Prepare the message schedule, {W_i}, 16 < t < 63 */
            W[t] = Sigma_256_1(W[t-2]) + W[t-7] + Sigma_256_0(W[t-15]) + W[t-16];
            /* End of if */
        T1 = h + Zsigma_256_1(e) + Ch(e,f,g) + SHA256_K[t] + W[t];
        T2 = Zsigma_256_0(a) + Maj(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
     } /* End of for */

     /* Compute the i^th intermediate hash value H^(i) */
     pSHA_CTX->HashValue[0] += a;
     pSHA_CTX->HashValue[1] += b;
     pSHA_CTX->HashValue[2] += c;
     pSHA_CTX->HashValue[3] += d;
     pSHA_CTX->HashValue[4] += e;
     pSHA_CTX->HashValue[5] += f;
     pSHA_CTX->HashValue[6] += g;
     pSHA_CTX->HashValue[7] += h;

    NdisZeroMemory(pSHA_CTX->Block, SHA256_BLOCK_SIZE);
    pSHA_CTX->BlockLen = 0;
} /* End of RT_SHA256_Hash */


/*
========================================================================
Routine Description:
    The message is appended to block. If block size > 64 bytes, the SHA256_Hash 
will be called.

Arguments:
    pSHA_CTX    Pointer to SHA256_CTX_STRUC
    message     Message context
    messageLen  The length of message in bytes

Return Value:
    None

Note:
    None
========================================================================
*/
VOID RT_SHA256_Append (
    IN  SHA256_CTX_STRUC *pSHA_CTX, 
    IN  const UINT8 Message[], 
    IN  UINT MessageLen)
{
    UINT appendLen = 0;
    UINT diffLen   = 0;
    
    while (appendLen != MessageLen) {
        diffLen = MessageLen - appendLen;
        if ((pSHA_CTX->BlockLen + diffLen) <  SHA256_BLOCK_SIZE) {
            NdisMoveMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 
                Message + appendLen, diffLen);
            pSHA_CTX->BlockLen += diffLen;
            appendLen += diffLen;
        } 
        else
        {
            NdisMoveMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 
                Message + appendLen, SHA256_BLOCK_SIZE - pSHA_CTX->BlockLen);
            appendLen += (SHA256_BLOCK_SIZE - pSHA_CTX->BlockLen);
            pSHA_CTX->BlockLen = SHA256_BLOCK_SIZE;
            RT_SHA256_Hash(pSHA_CTX);
        } /* End of if */
    } /* End of while */
    pSHA_CTX->MessageLen += MessageLen;
} /* End of RT_SHA256_Append */


/*
========================================================================
Routine Description:
    1. Append bit 1 to end of the message
    2. Append the length of message in rightmost 64 bits
    3. Transform the Hash Value to digest message

Arguments:
    pSHA_CTX        Pointer to SHA256_CTX_STRUC

Return Value:
    digestMessage   Digest message

Note:
    None
========================================================================
*/
VOID RT_SHA256_End (
    IN  SHA256_CTX_STRUC *pSHA_CTX, 
    OUT UINT8 DigestMessage[])
{
    UINT index;
    UINT64 message_length_bits;

    /* Append bit 1 to end of the message */
    NdisFillMemory(pSHA_CTX->Block + pSHA_CTX->BlockLen, 1, 0x80);

    /* 55 = 64 - 8 - 1: append 1 bit(1 byte) and message length (8 bytes) */
    if (pSHA_CTX->BlockLen > 55)
        RT_SHA256_Hash(pSHA_CTX);
    /* End of if */

    /* Append the length of message in rightmost 64 bits */
    message_length_bits = pSHA_CTX->MessageLen*8;
    message_length_bits = cpu2be64(message_length_bits);       
    NdisMoveMemory(&pSHA_CTX->Block[56], &message_length_bits, 8);
    RT_SHA256_Hash(pSHA_CTX);

    /* Return message digest, transform the UINT32 hash value to bytes */
    for (index = 0; index < 8;index++)
        pSHA_CTX->HashValue[index] = cpu2be32(pSHA_CTX->HashValue[index]);
        /* End of for */
    NdisMoveMemory(DigestMessage, pSHA_CTX->HashValue, SHA256_DIGEST_SIZE);
} /* End of RT_SHA256_End */


/*
========================================================================
Routine Description:
    SHA256 algorithm

Arguments:
    message         Message context
    messageLen      The length of message in bytes

Return Value:
    digestMessage   Digest message

Note:
    None
========================================================================
*/
VOID RT_SHA256 (
    IN  const UINT8 Message[], 
    IN  UINT MessageLen, 
    OUT UINT8 DigestMessage[])
{
    SHA256_CTX_STRUC sha_ctx;

    NdisZeroMemory(&sha_ctx, sizeof(SHA256_CTX_STRUC));
    RT_SHA256_Init(&sha_ctx);
    RT_SHA256_Append(&sha_ctx, Message, MessageLen);
    RT_SHA256_End(&sha_ctx, DigestMessage);
} /* End of RT_SHA256 */
#endif /* SHA256_SUPPORT */

#endif /* CRYPT_GPL_ALGORITHM */

/* End of crypt_sha2.c */

