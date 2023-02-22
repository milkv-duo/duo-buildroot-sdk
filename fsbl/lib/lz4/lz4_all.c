/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-present, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4
*/

/*-************************************
*  Tuning parameters
**************************************/
/*
 * LZ4_HEAPMODE :
 * Select how default compression functions will allocate memory for their hash table,
 * in memory stack (0:default, fastest), or in memory heap (1:requires malloc()).
 */
#ifndef LZ4_HEAPMODE
#  define LZ4_HEAPMODE 0
#endif

/*
 * LZ4_ACCELERATION_DEFAULT :
 * Select "acceleration" for LZ4_compress_fast() when parameter value <= 0
 */
#define LZ4_ACCELERATION_DEFAULT 1
/*
 * LZ4_ACCELERATION_MAX :
 * Any "acceleration" value higher than this threshold
 * get treated as LZ4_ACCELERATION_MAX instead (fix #876)
 */
#define LZ4_ACCELERATION_MAX 65537


/*-************************************
*  CPU Feature Detection
**************************************/
/* LZ4_FORCE_MEMORY_ACCESS
 * By default, access to unaligned memory is controlled by `memcpy()`, which is safe and portable.
 * Unfortunately, on some target/compiler combinations, the generated assembly is sub-optimal.
 * The below switch allow to select different access method for improved performance.
 * Method 0 (default) : use `memcpy()`. Safe and portable.
 * Method 1 : `__packed` statement. It depends on compiler extension (ie, not portable).
 *            This method is safe if your compiler supports it, and *generally* as fast or faster than `memcpy`.
 * Method 2 : direct access. This method is portable but violate C standard.
 *            It can generate buggy code on targets which assembly generation depends on alignment.
 *            But in some circumstances, it's the only known way to get the most performance (ie GCC + ARMv6)
 * See https://fastcompression.blogspot.fr/2015/08/accessing-unaligned-memory.html for details.
 * Prefer these methods in priority order (0 > 1 > 2)
 */
#ifndef LZ4_FORCE_MEMORY_ACCESS   /* can be defined externally */
#  if defined(__GNUC__) && \
  ( defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) \
  || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) )
#    define LZ4_FORCE_MEMORY_ACCESS 2
#  elif (defined(__INTEL_COMPILER) && !defined(_WIN32)) || defined(__GNUC__)
#    define LZ4_FORCE_MEMORY_ACCESS 1
#  endif
#endif

/*
 * LZ4_FORCE_SW_BITCOUNT
 * Define this parameter if your target system or compiler does not support hardware bit count
 */
#if defined(_MSC_VER) && defined(_WIN32_WCE)   /* Visual Studio for WinCE doesn't support Hardware bit count */
#  undef  LZ4_FORCE_SW_BITCOUNT  /* avoid double def */
#  define LZ4_FORCE_SW_BITCOUNT
#endif



/*-************************************
*  Dependency
**************************************/
/*
 * LZ4_SRC_INCLUDED:
 * Amalgamation flag, whether lz4.c is included
 */
#ifndef LZ4_SRC_INCLUDED
#  define LZ4_SRC_INCLUDED 1
#endif

#ifndef LZ4_STATIC_LINKING_ONLY
#define LZ4_STATIC_LINKING_ONLY
#endif

#ifndef LZ4_DISABLE_DEPRECATE_WARNINGS
#define LZ4_DISABLE_DEPRECATE_WARNINGS /* due to LZ4_decompress_safe_withPrefix64k */
#endif

#define LZ4_STATIC_LINKING_ONLY  /* LZ4_DISTANCE_MAX */
#include "lz4.h"
/* see also "memory routines" below */


/*-************************************
*  Compiler Options
**************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1400)  /* Visual Studio 2005+ */
#  include <intrin.h>               /* only present in VS2005+ */
#  pragma warning(disable : 4127)   /* disable: C4127: conditional expression is constant */
#endif  /* _MSC_VER */

#ifndef LZ4_FORCE_INLINE
#  ifdef _MSC_VER    /* Visual Studio */
#    define LZ4_FORCE_INLINE static __forceinline
#  else
#    if defined (__cplusplus) || defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#      ifdef __GNUC__
#        define LZ4_FORCE_INLINE static inline __attribute__((always_inline))
#      else
#        define LZ4_FORCE_INLINE static inline
#      endif
#    else
#      define LZ4_FORCE_INLINE static
#    endif /* __STDC_VERSION__ */
#  endif  /* _MSC_VER */
#endif /* LZ4_FORCE_INLINE */

/* LZ4_FORCE_O2 and LZ4_FORCE_INLINE
 * gcc on ppc64le generates an unrolled SIMDized loop for LZ4_wildCopy8,
 * together with a simple 8-byte copy loop as a fall-back path.
 * However, this optimization hurts the decompression speed by >30%,
 * because the execution does not go to the optimized loop
 * for typical compressible data, and all of the preamble checks
 * before going to the fall-back path become useless overhead.
 * This optimization happens only with the -O3 flag, and -O2 generates
 * a simple 8-byte copy loop.
 * With gcc on ppc64le, all of the LZ4_decompress_* and LZ4_wildCopy8
 * functions are annotated with __attribute__((optimize("O2"))),
 * and also LZ4_wildCopy8 is forcibly inlined, so that the O2 attribute
 * of LZ4_wildCopy8 does not affect the compression speed.
 */
#if defined(__PPC64__) && defined(__LITTLE_ENDIAN__) && defined(__GNUC__) && !defined(__clang__)
#  define LZ4_FORCE_O2  __attribute__((optimize("O2")))
#  undef LZ4_FORCE_INLINE
#  define LZ4_FORCE_INLINE  static __inline __attribute__((optimize("O2"),always_inline))
#else
#  define LZ4_FORCE_O2
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#ifndef likely
#define likely(expr)     expect((expr) != 0, 1)
#endif
#ifndef unlikely
#define unlikely(expr)   expect((expr) != 0, 0)
#endif

/* Should the alignment test prove unreliable, for some reason,
 * it can be disabled by setting LZ4_ALIGN_TEST to 0 */
#ifndef LZ4_ALIGN_TEST  /* can be externally provided */
# define LZ4_ALIGN_TEST 1
#endif


/*-************************************
*  Memory routines
**************************************/
#ifdef LZ4_USER_MEMORY_FUNCTIONS
/* memory management functions can be customized by user project.
 * Below functions must exist somewhere in the Project
 * and be available at link time */
void* LZ4_malloc(size_t s);
void* LZ4_calloc(size_t n, size_t s);
void  LZ4_free(void* p);
# define ALLOC(s)          LZ4_malloc(s)
# define ALLOC_AND_ZERO(s) LZ4_calloc(1,s)
# define FREEMEM(p)        LZ4_free(p)
#else
# include <stdlib.h>   /* malloc, calloc, free */
# define ALLOC(s)          malloc(s)
# define ALLOC_AND_ZERO(s) calloc(1,s)
# define FREEMEM(p)        free(p)
#endif

#include <string.h>   /* memset, memcpy */
#define MEM_INIT(p,v,s)   memset((p),(v),(s))


/*-************************************
*  Common Constants
**************************************/
#define MINMATCH 4

#define WILDCOPYLENGTH 8
#define LASTLITERALS   5   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MFLIMIT       12   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MATCH_SAFEGUARD_DISTANCE  ((2*WILDCOPYLENGTH) - MINMATCH)   /* ensure it's possible to write 2 x wildcopyLength without overflowing output buffer */
#define FASTLOOP_SAFE_DISTANCE 64
static const int LZ4_minLength = (MFLIMIT+1);

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define LZ4_DISTANCE_ABSOLUTE_MAX 65535
#if (LZ4_DISTANCE_MAX > LZ4_DISTANCE_ABSOLUTE_MAX)   /* max supported by LZ4 format */
#  error "LZ4_DISTANCE_MAX is too big : must be <= 65535"
#endif

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)


/*-************************************
*  Error detection
**************************************/
#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=1)
#  include <assert.h>
#else
#  ifndef assert
#    define assert(condition) ((void)0)
#  endif
#endif

#define LZ4_STATIC_ASSERT(c)   { enum { LZ4_static_assert = 1/(int)(!!(c)) }; }   /* use after variable declarations */

#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=2)
#  include <stdio.h>
   static int g_debuglog_enable = 1;
#  define DEBUGLOG(l, ...) {                          \
        if ((g_debuglog_enable) && (l<=LZ4_DEBUG)) {  \
            fprintf(stderr, __FILE__ ": ");           \
            fprintf(stderr, __VA_ARGS__);             \
            fprintf(stderr, " \n");                   \
    }   }
#else
#  define DEBUGLOG(l, ...) {}    /* disabled */
#endif

static int LZ4_isAligned(const void* ptr, size_t alignment)
{
    return ((size_t)ptr & (alignment -1)) == 0;
}


/*-************************************
*  Types
**************************************/
#include <limits.h>
#if defined(__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
# include <stdint.h>
  typedef  uint8_t BYTE;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef  int32_t S32;
  typedef uint64_t U64;
  typedef uintptr_t uptrval;
#else
# if UINT_MAX != 4294967295UL
#   error "LZ4 code (when not C++ or C99) assumes that sizeof(int) == 4"
# endif
  typedef unsigned char       BYTE;
  typedef unsigned short      U16;
  typedef unsigned int        U32;
  typedef   signed int        S32;
  typedef unsigned long long  U64;
  typedef size_t              uptrval;   /* generally true, except OpenVMS-64 */
#endif

#if defined(__x86_64__)
  typedef U64    reg_t;   /* 64-bits in x32 mode */
#else
  typedef size_t reg_t;   /* 32-bits in x32 mode */
#endif

typedef enum {
    notLimited = 0,
    limitedOutput = 1,
    fillOutput = 2
} limitedOutput_directive;


/*-************************************
*  Reading and writing into memory
**************************************/

/**
 * LZ4 relies on memcpy with a constant size being inlined. In freestanding
 * environments, the compiler can't assume the implementation of memcpy() is
 * standard compliant, so it can't apply its specialized memcpy() inlining
 * logic. When possible, use __builtin_memcpy() to tell the compiler to analyze
 * memcpy() as if it were standard compliant, so it can inline it in freestanding
 * environments. This is needed when decompressing the Linux Kernel, for example.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define LZ4_memcpy(dst, src, size) __builtin_memcpy(dst, src, size)
#else
#define LZ4_memcpy(dst, src, size) memcpy(dst, src, size)
#endif

static unsigned LZ4_isLittleEndian(void)
{
    const union { U32 u; BYTE c[4]; } one = { 1 };   /* don't use static : performance detrimental */
    return one.c[0];
}


#if defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==2)
/* lie to the compiler about data alignment; use with caution */

static U16 LZ4_read16(const void* memPtr) { return *(const U16*) memPtr; }
static U32 LZ4_read32(const void* memPtr) { return *(const U32*) memPtr; }
static reg_t LZ4_read_ARCH(const void* memPtr) { return *(const reg_t*) memPtr; }

static void LZ4_write16(void* memPtr, U16 value) { *(U16*)memPtr = value; }
static void LZ4_write32(void* memPtr, U32 value) { *(U32*)memPtr = value; }

#elif defined(LZ4_FORCE_MEMORY_ACCESS) && (LZ4_FORCE_MEMORY_ACCESS==1)

/* __pack instructions are safer, but compiler specific, hence potentially problematic for some compilers */
/* currently only defined for gcc and icc */
typedef union { U16 u16; U32 u32; reg_t uArch; } __attribute__((packed)) unalign;

static U16 LZ4_read16(const void* ptr) { return ((const unalign*)ptr)->u16; }
static U32 LZ4_read32(const void* ptr) { return ((const unalign*)ptr)->u32; }
static reg_t LZ4_read_ARCH(const void* ptr) { return ((const unalign*)ptr)->uArch; }

static void LZ4_write16(void* memPtr, U16 value) { ((unalign*)memPtr)->u16 = value; }
static void LZ4_write32(void* memPtr, U32 value) { ((unalign*)memPtr)->u32 = value; }

#else  /* safe and portable access using memcpy() */

static U16 LZ4_read16(const void* memPtr)
{
    U16 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static U32 LZ4_read32(const void* memPtr)
{
    U32 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static reg_t LZ4_read_ARCH(const void* memPtr)
{
    reg_t val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static void LZ4_write16(void* memPtr, U16 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

static void LZ4_write32(void* memPtr, U32 value)
{
    LZ4_memcpy(memPtr, &value, sizeof(value));
}

#endif /* LZ4_FORCE_MEMORY_ACCESS */


static U16 LZ4_readLE16(const void* memPtr)
{
    if (LZ4_isLittleEndian()) {
        return LZ4_read16(memPtr);
    } else {
        const BYTE* p = (const BYTE*)memPtr;
        return (U16)((U16)p[0] + (p[1]<<8));
    }
}

static void LZ4_writeLE16(void* memPtr, U16 value)
{
    if (LZ4_isLittleEndian()) {
        LZ4_write16(memPtr, value);
    } else {
        BYTE* p = (BYTE*)memPtr;
        p[0] = (BYTE) value;
        p[1] = (BYTE)(value>>8);
    }
}

/* customized variant of memcpy, which can overwrite up to 8 bytes beyond dstEnd */
LZ4_FORCE_INLINE
void LZ4_wildCopy8(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,8); d+=8; s+=8; } while (d<e);
}

static const unsigned inc32table[8] = {0, 1, 2,  1,  0,  4, 4, 4};
static const int      dec64table[8] = {0, 0, 0, -1, -4,  1, 2, 3};


#ifndef LZ4_FAST_DEC_LOOP
#  if defined __i386__ || defined _M_IX86 || defined __x86_64__ || defined _M_X64
#    define LZ4_FAST_DEC_LOOP 1
#  elif defined(__aarch64__) && !defined(__clang__)
     /* On aarch64, we disable this optimization for clang because on certain
      * mobile chipsets, performance is reduced with clang. For information
      * refer to https://github.com/lz4/lz4/pull/707 */
#    define LZ4_FAST_DEC_LOOP 1
#  else
#    define LZ4_FAST_DEC_LOOP 0
#  endif
#endif

#if LZ4_FAST_DEC_LOOP

LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset_base(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    assert(srcPtr + offset == dstPtr);
    if (offset < 8) {
        LZ4_write32(dstPtr, 0);   /* silence an msan warning when offset==0 */
        dstPtr[0] = srcPtr[0];
        dstPtr[1] = srcPtr[1];
        dstPtr[2] = srcPtr[2];
        dstPtr[3] = srcPtr[3];
        srcPtr += inc32table[offset];
        LZ4_memcpy(dstPtr+4, srcPtr, 4);
        srcPtr -= dec64table[offset];
        dstPtr += 8;
    } else {
        LZ4_memcpy(dstPtr, srcPtr, 8);
        dstPtr += 8;
        srcPtr += 8;
    }

    LZ4_wildCopy8(dstPtr, srcPtr, dstEnd);
}

/* customized variant of memcpy, which can overwrite up to 32 bytes beyond dstEnd
 * this version copies two times 16 bytes (instead of one time 32 bytes)
 * because it must be compatible with offsets >= 16. */
LZ4_FORCE_INLINE void
LZ4_wildCopy32(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

    do { LZ4_memcpy(d,s,16); LZ4_memcpy(d+16,s+16,16); d+=32; s+=32; } while (d<e);
}

/* LZ4_memcpy_using_offset()  presumes :
 * - dstEnd >= dstPtr + MINMATCH
 * - there is at least 8 bytes available to write after dstEnd */
LZ4_FORCE_INLINE void
LZ4_memcpy_using_offset(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd, const size_t offset)
{
    BYTE v[8];

    assert(dstEnd >= dstPtr + MINMATCH);

    switch(offset) {
    case 1:
        MEM_INIT(v, *srcPtr, 8);
        break;
    case 2:
        LZ4_memcpy(v, srcPtr, 2);
        LZ4_memcpy(&v[2], srcPtr, 2);
        LZ4_memcpy(&v[4], v, 4);
        break;
    case 4:
        LZ4_memcpy(v, srcPtr, 4);
        LZ4_memcpy(&v[4], srcPtr, 4);
        break;
    default:
        LZ4_memcpy_using_offset_base(dstPtr, srcPtr, dstEnd, offset);
        return;
    }

    LZ4_memcpy(dstPtr, v, 8);
    dstPtr += 8;
    while (dstPtr < dstEnd) {
        LZ4_memcpy(dstPtr, v, 8);
        dstPtr += 8;
    }
}
#endif


/*-************************************
*  Common functions
**************************************/
static unsigned LZ4_NbCommonBytes (reg_t val)
{
    assert(val != 0);
    if (LZ4_isLittleEndian()) {
        if (sizeof(val) == 8) {
#       if defined(_MSC_VER) && (_MSC_VER >= 1800) && defined(_M_AMD64) && !defined(LZ4_FORCE_SW_BITCOUNT)
            /* x64 CPUS without BMI support interpret `TZCNT` as `REP BSF` */
            return (unsigned)_tzcnt_u64(val) >> 3;
#       elif defined(_MSC_VER) && defined(_WIN64) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanForward64(&r, (U64)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctzll((U64)val) >> 3;
#       else
            const U64 m = 0x0101010101010101ULL;
            val ^= val - 1;
            return (unsigned)(((U64)((val & (m - 1)) * m)) >> 56);
#       endif
        } else /* 32 bits */ {
#       if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(LZ4_FORCE_SW_BITCOUNT)
            unsigned long r;
            _BitScanForward(&r, (U32)val);
            return (unsigned)r >> 3;
#       elif (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_ctz((U32)val) >> 3;
#       else
            const U32 m = 0x01010101;
            return (unsigned)((((val - 1) ^ val) & (m - 1)) * m) >> 24;
#       endif
        }
    } else   /* Big Endian CPU */ {
        if (sizeof(val)==8) {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                        !defined(__TINYC__) && !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clzll((U64)val) >> 3;
#       else
#if 1
            /* this method is probably faster,
             * but adds a 128 bytes lookup table */
            static const unsigned char ctz7_tab[128] = {
                7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
            };
            U64 const mask = 0x0101010101010101ULL;
            U64 const t = (((val >> 8) - mask) | val) & mask;
            return ctz7_tab[(t * 0x0080402010080402ULL) >> 57];
#else
            /* this method doesn't consume memory space like the previous one,
             * but it contains several branches,
             * that may end up slowing execution */
            static const U32 by32 = sizeof(val)*4;  /* 32 on 64 bits (goal), 16 on 32 bits.
            Just to avoid some static analyzer complaining about shift by 32 on 32-bits target.
            Note that this code path is never triggered in 32-bits mode. */
            unsigned r;
            if (!(val>>by32)) { r=4; } else { r=0; val>>=by32; }
            if (!(val>>16)) { r+=2; val>>=8; } else { val>>=24; }
            r += (!val);
            return r;
#endif
#       endif
        } else /* 32 bits */ {
#       if (defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 3) || \
                            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))))) && \
                                        !defined(LZ4_FORCE_SW_BITCOUNT)
            return (unsigned)__builtin_clz((U32)val) >> 3;
#       else
            val >>= 8;
            val = ((((val + 0x00FFFF00) | 0x00FFFFFF) + val) |
              (val + 0x00FF0000)) >> 24;
            return (unsigned)val ^ 3;
#       endif
        }
    }
}


#define STEPSIZE sizeof(reg_t)
LZ4_FORCE_INLINE
unsigned LZ4_count(const BYTE* pIn, const BYTE* pMatch, const BYTE* pInLimit)
{
    const BYTE* const pStart = pIn;

    if (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) {
            pIn+=STEPSIZE; pMatch+=STEPSIZE;
        } else {
            return LZ4_NbCommonBytes(diff);
    }   }

    while (likely(pIn < pInLimit-(STEPSIZE-1))) {
        reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) { pIn+=STEPSIZE; pMatch+=STEPSIZE; continue; }
        pIn += LZ4_NbCommonBytes(diff);
        return (unsigned)(pIn - pStart);
    }

    if ((STEPSIZE==8) && (pIn<(pInLimit-3)) && (LZ4_read32(pMatch) == LZ4_read32(pIn))) { pIn+=4; pMatch+=4; }
    if ((pIn<(pInLimit-1)) && (LZ4_read16(pMatch) == LZ4_read16(pIn))) { pIn+=2; pMatch+=2; }
    if ((pIn<pInLimit) && (*pMatch == *pIn)) pIn++;
    return (unsigned)(pIn - pStart);
}


#ifndef LZ4_COMMONDEFS_ONLY
/*-************************************
*  Local Constants
**************************************/
static const int LZ4_64Klimit = ((64 KB) + (MFLIMIT-1));
static const U32 LZ4_skipTrigger = 6;  /* Increase this value ==> compression run slower on incompressible data */


/*-************************************
*  Local Structures and types
**************************************/
typedef enum { clearedTable = 0, byPtr, byU32, byU16 } tableType_t;

/**
 * This enum distinguishes several different modes of accessing previous
 * content in the stream.
 *
 * - noDict        : There is no preceding content.
 * - withPrefix64k : Table entries up to ctx->dictSize before the current blob
 *                   blob being compressed are valid and refer to the preceding
 *                   content (of length ctx->dictSize), which is available
 *                   contiguously preceding in memory the content currently
 *                   being compressed.
 * - usingExtDict  : Like withPrefix64k, but the preceding content is somewhere
 *                   else in memory, starting at ctx->dictionary with length
 *                   ctx->dictSize.
 * - usingDictCtx  : Like usingExtDict, but everything concerning the preceding
 *                   content is in a separate context, pointed to by
 *                   ctx->dictCtx. ctx->dictionary, ctx->dictSize, and table
 *                   entries in the current context that refer to positions
 *                   preceding the beginning of the current compression are
 *                   ignored. Instead, ctx->dictCtx->dictionary and ctx->dictCtx
 *                   ->dictSize describe the location and size of the preceding
 *                   content, and matches are found by looking in the ctx
 *                   ->dictCtx->hashTable.
 */
typedef enum { noDict = 0, withPrefix64k, usingExtDict, usingDictCtx } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;


/*-************************************
*  Local Utils
**************************************/
int LZ4_versionNumber (void) { return LZ4_VERSION_NUMBER; }
const char* LZ4_versionString(void) { return LZ4_VERSION_STRING; }
int LZ4_compressBound(int isize)  { return LZ4_COMPRESSBOUND(isize); }
int LZ4_sizeofState(void) { return LZ4_STREAMSIZE; }


/*-************************************
*  Internal Definitions used in Tests
**************************************/
#if defined (__cplusplus)
extern "C" {
#endif

int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize);

int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize);

#if defined (__cplusplus)
}
#endif

/*-******************************
*  Compression functions
********************************/
LZ4_FORCE_INLINE U32 LZ4_hash4(U32 sequence, tableType_t const tableType)
{
    if (tableType == byU16)
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-(LZ4_HASHLOG+1)));
    else
        return ((sequence * 2654435761U) >> ((MINMATCH*8)-LZ4_HASHLOG));
}

LZ4_FORCE_INLINE U32 LZ4_hash5(U64 sequence, tableType_t const tableType)
{
    const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG+1 : LZ4_HASHLOG;
    if (LZ4_isLittleEndian()) {
        const U64 prime5bytes = 889523592379ULL;
        return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
    } else {
        const U64 prime8bytes = 11400714785074694791ULL;
        return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
    }
}

LZ4_FORCE_INLINE U32 LZ4_hashPosition(const void* const p, tableType_t const tableType)
{
    if ((sizeof(reg_t)==8) && (tableType != byU16)) return LZ4_hash5(LZ4_read_ARCH(p), tableType);
    return LZ4_hash4(LZ4_read32(p), tableType);
}

LZ4_FORCE_INLINE void LZ4_clearHash(U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* fallthrough */
    case clearedTable: { /* illegal! */ assert(0); return; }
    case byPtr: { const BYTE** hashTable = (const BYTE**)tableBase; hashTable[h] = NULL; return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = 0; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; hashTable[h] = 0; return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putIndexOnHash(U32 idx, U32 h, void* tableBase, tableType_t const tableType)
{
    switch (tableType)
    {
    default: /* fallthrough */
    case clearedTable: /* fallthrough */
    case byPtr: { /* illegal! */ assert(0); return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = idx; return; }
    case byU16: { U16* hashTable = (U16*) tableBase; assert(idx < 65536); hashTable[h] = (U16)idx; return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putPositionOnHash(const BYTE* p, U32 h,
                                  void* tableBase, tableType_t const tableType,
                            const BYTE* srcBase)
{
    switch (tableType)
    {
    case clearedTable: { /* illegal! */ assert(0); return; }
    case byPtr: { const BYTE** hashTable = (const BYTE**)tableBase; hashTable[h] = p; return; }
    case byU32: { U32* hashTable = (U32*) tableBase; hashTable[h] = (U32)(p-srcBase); return; }
    case byU16: { U16* hashTable = (U16*) tableBase; hashTable[h] = (U16)(p-srcBase); return; }
    }
}

LZ4_FORCE_INLINE void LZ4_putPosition(const BYTE* p, void* tableBase, tableType_t tableType, const BYTE* srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    LZ4_putPositionOnHash(p, h, tableBase, tableType, srcBase);
}

/* LZ4_getIndexOnHash() :
 * Index of match position registered in hash table.
 * hash position must be calculated by using base+index, or dictBase+index.
 * Assumption 1 : only valid if tableType == byU32 or byU16.
 * Assumption 2 : h is presumed valid (within limits of hash table)
 */
LZ4_FORCE_INLINE U32 LZ4_getIndexOnHash(U32 h, const void* tableBase, tableType_t tableType)
{
    LZ4_STATIC_ASSERT(LZ4_MEMORY_USAGE > 2);
    if (tableType == byU32) {
        const U32* const hashTable = (const U32*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-2)));
        return hashTable[h];
    }
    if (tableType == byU16) {
        const U16* const hashTable = (const U16*) tableBase;
        assert(h < (1U << (LZ4_MEMORY_USAGE-1)));
        return hashTable[h];
    }
    assert(0); return 0;  /* forbidden case */
}

static const BYTE* LZ4_getPositionOnHash(U32 h, const void* tableBase, tableType_t tableType, const BYTE* srcBase)
{
    if (tableType == byPtr) { const BYTE* const* hashTable = (const BYTE* const*) tableBase; return hashTable[h]; }
    if (tableType == byU32) { const U32* const hashTable = (const U32*) tableBase; return hashTable[h] + srcBase; }
    { const U16* const hashTable = (const U16*) tableBase; return hashTable[h] + srcBase; }   /* default, to ensure a return */
}

LZ4_FORCE_INLINE const BYTE*
LZ4_getPosition(const BYTE* p,
                const void* tableBase, tableType_t tableType,
                const BYTE* srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    return LZ4_getPositionOnHash(h, tableBase, tableType, srcBase);
}

LZ4_FORCE_INLINE void
LZ4_prepareTable(LZ4_stream_t_internal* const cctx,
           const int inputSize,
           const tableType_t tableType) {
    /* If the table hasn't been used, it's guaranteed to be zeroed out, and is
     * therefore safe to use no matter what mode we're in. Otherwise, we figure
     * out if it's safe to leave as is or whether it needs to be reset.
     */
    if ((tableType_t)cctx->tableType != clearedTable) {
        assert(inputSize >= 0);
        if ((tableType_t)cctx->tableType != tableType
          || ((tableType == byU16) && cctx->currentOffset + (unsigned)inputSize >= 0xFFFFU)
          || ((tableType == byU32) && cctx->currentOffset > 1 GB)
          || tableType == byPtr
          || inputSize >= 4 KB)
        {
            DEBUGLOG(4, "LZ4_prepareTable: Resetting table in %p", cctx);
            MEM_INIT(cctx->hashTable, 0, LZ4_HASHTABLESIZE);
            cctx->currentOffset = 0;
            cctx->tableType = (U32)clearedTable;
        } else {
            DEBUGLOG(4, "LZ4_prepareTable: Re-use hash table (no reset)");
        }
    }

    /* Adding a gap, so all previous entries are > LZ4_DISTANCE_MAX back, is faster
     * than compressing without a gap. However, compressing with
     * currentOffset == 0 is faster still, so we preserve that case.
     */
    if (cctx->currentOffset != 0 && tableType == byU32) {
        DEBUGLOG(5, "LZ4_prepareTable: adding 64KB to currentOffset");
        cctx->currentOffset += 64 KB;
    }

    /* Finally, clear history */
    cctx->dictCtx = NULL;
    cctx->dictionary = NULL;
    cctx->dictSize = 0;
}

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time.
 *  Presumed already validated at this stage:
 *  - source != NULL
 *  - inputSize > 0
 */
LZ4_FORCE_INLINE int LZ4_compress_generic_validated(
                 LZ4_stream_t_internal* const cctx,
                 const char* const source,
                 char* const dest,
                 const int inputSize,
                 int *inputConsumed, /* only written when outputDirective == fillOutput */
                 const int maxOutputSize,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    int result;
    const BYTE* ip = (const BYTE*) source;

    U32 const startIndex = cctx->currentOffset;
    const BYTE* base = (const BYTE*) source - startIndex;
    const BYTE* lowLimit;

    const LZ4_stream_t_internal* dictCtx = (const LZ4_stream_t_internal*) cctx->dictCtx;
    const BYTE* const dictionary =
        dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
    const U32 dictSize =
        dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
    const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

    int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
    U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
    const BYTE* const dictEnd = dictionary ? dictionary + dictSize : dictionary;
    const BYTE* anchor = (const BYTE*) source;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimitPlusOne = iend - MFLIMIT + 1;
    const BYTE* const matchlimit = iend - LASTLITERALS;

    /* the dictCtx currentOffset is indexed on the start of the dictionary,
     * while a dictionary in the current context precedes the currentOffset */
    const BYTE* dictBase = !dictionary ? NULL : (dictDirective == usingDictCtx) ?
                            dictionary + dictSize - dictCtx->currentOffset :
                            dictionary + dictSize - startIndex;

    BYTE* op = (BYTE*) dest;
    BYTE* const olimit = op + maxOutputSize;

    U32 offset = 0;
    U32 forwardH;

    DEBUGLOG(5, "LZ4_compress_generic_validated: srcSize=%i, tableType=%u", inputSize, tableType);
    assert(ip != NULL);
    /* If init conditions are not met, we don't have to mark stream
     * as having dirty context, since no action was taken yet */
    if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
    if ((tableType == byU16) && (inputSize>=LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
    if (tableType==byPtr) assert(dictDirective==noDict);      /* only supported use case with byPtr */
    assert(acceleration >= 1);

    lowLimit = (const BYTE*)source - (dictDirective == withPrefix64k ? dictSize : 0);

    /* Update context state */
    if (dictDirective == usingDictCtx) {
        /* Subsequent linked blocks can't use the dictionary. */
        /* Instead, they use the block we just compressed. */
        cctx->dictCtx = NULL;
        cctx->dictSize = (U32)inputSize;
    } else {
        cctx->dictSize += (U32)inputSize;
    }
    cctx->currentOffset += (U32)inputSize;
    cctx->tableType = (U32)tableType;

    if (inputSize<LZ4_minLength) goto _last_literals;        /* Input too small, no compression (all literals) */

    /* First Byte */
    LZ4_putPosition(ip, cctx->hashTable, tableType, base);
    ip++; forwardH = LZ4_hashPosition(ip, tableType);

    /* Main Loop */
    for ( ; ; ) {
        const BYTE* match;
        BYTE* token;
        const BYTE* filledIp;

        /* Find a match */
        if (tableType == byPtr) {
            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

            } while ( (match+LZ4_DISTANCE_MAX < ip)
                   || (LZ4_read32(match) != LZ4_read32(ip)) );

        } else {   /* byU32, byU16 */

            const BYTE* forwardIp = ip;
            int step = 1;
            int searchMatchNb = acceleration << LZ4_skipTrigger;
            do {
                U32 const h = forwardH;
                U32 const current = (U32)(forwardIp - base);
                U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
                assert(matchIndex <= current);
                assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_skipTrigger);

                if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
                assert(ip < mflimitPlusOne);

                if (dictDirective == usingDictCtx) {
                    if (matchIndex < startIndex) {
                        /* there was no match, try the dictionary */
                        assert(tableType == byU32);
                        matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                        match = dictBase + matchIndex;
                        matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else if (dictDirective==usingExtDict) {
                    if (matchIndex < startIndex) {
                        DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
                        assert(startIndex - matchIndex >= MINMATCH);
                        match = dictBase + matchIndex;
                        lowLimit = dictionary;
                    } else {
                        match = base + matchIndex;
                        lowLimit = (const BYTE*)source;
                    }
                } else {   /* single continuous memory segment */
                    match = base + matchIndex;
                }
                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

                DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
                if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
                assert(matchIndex < current);
                if ( ((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
                  && (matchIndex+LZ4_DISTANCE_MAX < current)) {
                    continue;
                } /* too far */
                assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */

                if (LZ4_read32(match) == LZ4_read32(ip)) {
                    if (maybe_extMem) offset = current - matchIndex;
                    break;   /* match found */
                }

            } while(1);
        }

        /* Catch up */
        filledIp = ip;
        while (((ip>anchor) & (match > lowLimit)) && (unlikely(ip[-1]==match[-1]))) { ip--; match--; }

        /* Encode Literals */
        {   unsigned const litLength = (unsigned)(ip - anchor);
            token = op++;
            if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
                (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength/255) > olimit)) ) {
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
            if ((outputDirective == fillOutput) &&
                (unlikely(op + (litLength+240)/255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
                op--;
                goto _last_literals;
            }
            if (litLength >= RUN_MASK) {
                int len = (int)(litLength - RUN_MASK);
                *token = (RUN_MASK<<ML_BITS);
                for(; len >= 255 ; len-=255) *op++ = 255;
                *op++ = (BYTE)len;
            }
            else *token = (BYTE)(litLength<<ML_BITS);

            /* Copy Literals */
            LZ4_wildCopy8(op, anchor, op+litLength);
            op+=litLength;
            DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                        (int)(anchor-(const BYTE*)source), litLength, (int)(ip-(const BYTE*)source));
        }

_next_match:
        /* at this stage, the following variables must be correctly set :
         * - ip : at start of LZ operation
         * - match : at start of previous pattern occurence; can be within current prefix, or within extDict
         * - offset : if maybe_ext_memSegment==1 (constant)
         * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
         * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
         */

        if ((outputDirective == fillOutput) &&
            (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
            /* the match was too close to the end, rewind and go to last literals */
            op = token;
            goto _last_literals;
        }

        /* Encode Offset */
        if (maybe_extMem) {   /* static test */
            DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE*)source));
            assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
            LZ4_writeLE16(op, (U16)offset); op+=2;
        } else  {
            DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
            assert(ip-match <= LZ4_DISTANCE_MAX);
            LZ4_writeLE16(op, (U16)(ip - match)); op+=2;
        }

        /* Encode MatchLength */
        {   unsigned matchCode;

            if ( (dictDirective==usingExtDict || dictDirective==usingDictCtx)
              && (lowLimit==dictionary) /* match within extDict */ ) {
                const BYTE* limit = ip + (dictEnd-match);
                assert(dictEnd > match);
                if (limit > matchlimit) limit = matchlimit;
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, limit);
                ip += (size_t)matchCode + MINMATCH;
                if (ip==limit) {
                    unsigned const more = LZ4_count(limit, (const BYTE*)source, matchlimit);
                    matchCode += more;
                    ip += more;
                }
                DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode+MINMATCH);
            } else {
                matchCode = LZ4_count(ip+MINMATCH, match+MINMATCH, matchlimit);
                ip += (size_t)matchCode + MINMATCH;
                DEBUGLOG(6, "             with matchLength=%u", matchCode+MINMATCH);
            }

            if ((outputDirective) &&    /* Check output buffer overflow */
                (unlikely(op + (1 + LASTLITERALS) + (matchCode+240)/255 > olimit)) ) {
                if (outputDirective == fillOutput) {
                    /* Match description too long : reduce it */
                    U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
                    ip -= matchCode - newMatchCode;
                    assert(newMatchCode < matchCode);
                    matchCode = newMatchCode;
                    if (unlikely(ip <= filledIp)) {
                        /* We have already filled up to filledIp so if ip ends up less than filledIp
                         * we have positions in the hash table beyond the current position. This is
                         * a problem if we reuse the hash table. So we have to remove these positions
                         * from the hash table.
                         */
                        const BYTE* ptr;
                        DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
                        for (ptr = ip; ptr <= filledIp; ++ptr) {
                            U32 const h = LZ4_hashPosition(ptr, tableType);
                            LZ4_clearHash(h, cctx->hashTable, tableType);
                        }
                    }
                } else {
                    assert(outputDirective == limitedOutput);
                    return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
                }
            }
            if (matchCode >= ML_MASK) {
                *token += ML_MASK;
                matchCode -= ML_MASK;
                LZ4_write32(op, 0xFFFFFFFF);
                while (matchCode >= 4*255) {
                    op+=4;
                    LZ4_write32(op, 0xFFFFFFFF);
                    matchCode -= 4*255;
                }
                op += matchCode / 255;
                *op++ = (BYTE)(matchCode % 255);
            } else
                *token += (BYTE)(matchCode);
        }
        /* Ensure we have enough space for the last literals. */
        assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

        anchor = ip;

        /* Test end of chunk */
        if (ip >= mflimitPlusOne) break;

        /* Fill table */
        LZ4_putPosition(ip-2, cctx->hashTable, tableType, base);

        /* Test next position */
        if (tableType == byPtr) {

            match = LZ4_getPosition(ip, cctx->hashTable, tableType, base);
            LZ4_putPosition(ip, cctx->hashTable, tableType, base);
            if ( (match+LZ4_DISTANCE_MAX >= ip)
              && (LZ4_read32(match) == LZ4_read32(ip)) )
            { token=op++; *token=0; goto _next_match; }

        } else {   /* byU32, byU16 */

            U32 const h = LZ4_hashPosition(ip, tableType);
            U32 const current = (U32)(ip-base);
            U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if (dictDirective == usingDictCtx) {
                if (matchIndex < startIndex) {
                    /* there was no match, try the dictionary */
                    matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                    matchIndex += dictDelta;
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;  /* required for match length counter */
                }
            } else if (dictDirective==usingExtDict) {
                if (matchIndex < startIndex) {
                    match = dictBase + matchIndex;
                    lowLimit = dictionary;   /* required for match length counter */
                } else {
                    match = base + matchIndex;
                    lowLimit = (const BYTE*)source;   /* required for match length counter */
                }
            } else {   /* single memory segment */
                match = base + matchIndex;
            }
            LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
            assert(matchIndex < current);
            if ( ((dictIssue==dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
              && (((tableType==byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex+LZ4_DISTANCE_MAX >= current))
              && (LZ4_read32(match) == LZ4_read32(ip)) ) {
                token=op++;
                *token=0;
                if (maybe_extMem) offset = current - matchIndex;
                DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
                            (int)(anchor-(const BYTE*)source), 0, (int)(ip-(const BYTE*)source));
                goto _next_match;
            }
        }

        /* Prepare next loop */
        forwardH = LZ4_hashPosition(++ip, tableType);

    }

_last_literals:
    /* Encode Last Literals */
    {   size_t lastRun = (size_t)(iend - anchor);
        if ( (outputDirective) &&  /* Check output buffer overflow */
            (op + lastRun + 1 + ((lastRun+255-RUN_MASK)/255) > olimit)) {
            if (outputDirective == fillOutput) {
                /* adapt lastRun to fill 'dst' */
                assert(olimit >= op);
                lastRun  = (size_t)(olimit-op) - 1/*token*/;
                lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
            } else {
                assert(outputDirective == limitedOutput);
                return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
            }
        }
        DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
        if (lastRun >= RUN_MASK) {
            size_t accumulator = lastRun - RUN_MASK;
            *op++ = RUN_MASK << ML_BITS;
            for(; accumulator >= 255 ; accumulator-=255) *op++ = 255;
            *op++ = (BYTE) accumulator;
        } else {
            *op++ = (BYTE)(lastRun<<ML_BITS);
        }
        LZ4_memcpy(op, anchor, lastRun);
        ip = anchor + lastRun;
        op += lastRun;
    }

    if (outputDirective == fillOutput) {
        *inputConsumed = (int) (((const char*)ip)-source);
    }
    result = (int)(((char*)op) - dest);
    assert(result > 0);
    DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
    return result;
}

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time;
 *  takes care of src == (NULL, 0)
 *  and forward the rest to LZ4_compress_generic_validated */
LZ4_FORCE_INLINE int LZ4_compress_generic(
                 LZ4_stream_t_internal* const cctx,
                 const char* const src,
                 char* const dst,
                 const int srcSize,
                 int *inputConsumed, /* only written when outputDirective == fillOutput */
                 const int dstCapacity,
                 const limitedOutput_directive outputDirective,
                 const tableType_t tableType,
                 const dict_directive dictDirective,
                 const dictIssue_directive dictIssue,
                 const int acceleration)
{
    DEBUGLOG(5, "LZ4_compress_generic: srcSize=%i, dstCapacity=%i",
                srcSize, dstCapacity);

    if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Unsupported srcSize, too large (or negative) */
    if (srcSize == 0) {   /* src == NULL supported if srcSize == 0 */
        if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* no output, can't write anything */
        DEBUGLOG(5, "Generating an empty block");
        assert(outputDirective == notLimited || dstCapacity >= 1);
        assert(dst != NULL);
        dst[0] = 0;
        if (outputDirective == fillOutput) {
            assert (inputConsumed != NULL);
            *inputConsumed = 0;
        }
        return 1;
    }
    assert(src != NULL);

    return LZ4_compress_generic_validated(cctx, src, dst, srcSize,
                inputConsumed, /* only written into if outputDirective == fillOutput */
                dstCapacity, outputDirective,
                tableType, dictDirective, dictIssue, acceleration);
}


int LZ4_compress_fast_extState(void* state, const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    LZ4_stream_t_internal* const ctx = & LZ4_initStream(state, sizeof(LZ4_stream_t)) -> internal_donotuse;
    assert(ctx != NULL);
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;
    if (maxOutputSize >= LZ4_compressBound(inputSize)) {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (inputSize < LZ4_64Klimit) {
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}

/**
 * LZ4_compress_fast_extState_fastReset() :
 * A variant of LZ4_compress_fast_extState().
 *
 * Using this variant avoids an expensive initialization step. It is only safe
 * to call if the state buffer is known to be correctly initialized already
 * (see comment in lz4.h on LZ4_resetStream_fast() for a definition of
 * "correctly initialized").
 */
int LZ4_compress_fast_extState_fastReset(void* state, const char* src, char* dst, int srcSize, int dstCapacity, int acceleration)
{
    LZ4_stream_t_internal* ctx = &((LZ4_stream_t*)state)->internal_donotuse;
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;

    if (dstCapacity >= LZ4_compressBound(srcSize)) {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
        }
    } else {
        if (srcSize < LZ4_64Klimit) {
            const tableType_t tableType = byU16;
            LZ4_prepareTable(ctx, srcSize, tableType);
            if (ctx->currentOffset) {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, dictSmall, acceleration);
            } else {
                return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
            }
        } else {
            const tableType_t tableType = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            LZ4_prepareTable(ctx, srcSize, tableType);
            return LZ4_compress_generic(ctx, src, dst, srcSize, NULL, dstCapacity, limitedOutput, tableType, noDict, noDictIssue, acceleration);
        }
    }
}


int LZ4_compress_fast(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    int result;
#if (LZ4_HEAPMODE)
    LZ4_stream_t* ctxPtr = ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
    if (ctxPtr == NULL) return 0;
#else
    LZ4_stream_t ctx;
    LZ4_stream_t* const ctxPtr = &ctx;
#endif
    result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);

#if (LZ4_HEAPMODE)
    FREEMEM(ctxPtr);
#endif
    return result;
}


int LZ4_compress_default(const char* src, char* dst, int srcSize, int maxOutputSize)
{
    return LZ4_compress_fast(src, dst, srcSize, maxOutputSize, 1);
}


/* Note!: This function leaves the stream in an unclean/broken state!
 * It is not safe to subsequently use the same state with a _fastReset() or
 * _continue() call without resetting it. */
static int LZ4_compress_destSize_extState (LZ4_stream_t* state, const char* src, char* dst, int* srcSizePtr, int targetDstSize)
{
    void* const s = LZ4_initStream(state, sizeof (*state));
    assert(s != NULL); (void)s;

    if (targetDstSize >= LZ4_compressBound(*srcSizePtr)) {  /* compression success is guaranteed */
        return LZ4_compress_fast_extState(state, src, dst, *srcSizePtr, targetDstSize, 1);
    } else {
        if (*srcSizePtr < LZ4_64Klimit) {
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, byU16, noDict, noDictIssue, 1);
        } else {
            tableType_t const addrMode = ((sizeof(void*)==4) && ((uptrval)src > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
            return LZ4_compress_generic(&state->internal_donotuse, src, dst, *srcSizePtr, srcSizePtr, targetDstSize, fillOutput, addrMode, noDict, noDictIssue, 1);
    }   }
}


int LZ4_compress_destSize(const char* src, char* dst, int* srcSizePtr, int targetDstSize)
{
#if (LZ4_HEAPMODE)
    LZ4_stream_t* ctx = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
    if (ctx == NULL) return 0;
#else
    LZ4_stream_t ctxBody;
    LZ4_stream_t* ctx = &ctxBody;
#endif

    int result = LZ4_compress_destSize_extState(ctx, src, dst, srcSizePtr, targetDstSize);

#if (LZ4_HEAPMODE)
    FREEMEM(ctx);
#endif
    return result;
}



/*-******************************
*  Streaming functions
********************************/

LZ4_stream_t* LZ4_createStream(void)
{
    LZ4_stream_t* const lz4s = (LZ4_stream_t*)ALLOC(sizeof(LZ4_stream_t));
    LZ4_STATIC_ASSERT(LZ4_STREAMSIZE >= sizeof(LZ4_stream_t_internal));    /* A compilation error here means LZ4_STREAMSIZE is not large enough */
    DEBUGLOG(4, "LZ4_createStream %p", lz4s);
    if (lz4s == NULL) return NULL;
    LZ4_initStream(lz4s, sizeof(*lz4s));
    return lz4s;
}

static size_t LZ4_stream_t_alignment(void)
{
#if LZ4_ALIGN_TEST
    typedef struct { char c; LZ4_stream_t t; } t_a;
    return sizeof(t_a) - sizeof(LZ4_stream_t);
#else
    return 1;  /* effectively disabled */
#endif
}

LZ4_stream_t* LZ4_initStream (void* buffer, size_t size)
{
    DEBUGLOG(5, "LZ4_initStream");
    if (buffer == NULL) { return NULL; }
    if (size < sizeof(LZ4_stream_t)) { return NULL; }
    if (!LZ4_isAligned(buffer, LZ4_stream_t_alignment())) return NULL;
    MEM_INIT(buffer, 0, sizeof(LZ4_stream_t_internal));
    return (LZ4_stream_t*)buffer;
}

/* resetStream is now deprecated,
 * prefer initStream() which is more general */
void LZ4_resetStream (LZ4_stream_t* LZ4_stream)
{
    DEBUGLOG(5, "LZ4_resetStream (ctx:%p)", LZ4_stream);
    MEM_INIT(LZ4_stream, 0, sizeof(LZ4_stream_t_internal));
}

void LZ4_resetStream_fast(LZ4_stream_t* ctx) {
    LZ4_prepareTable(&(ctx->internal_donotuse), 0, byU32);
}

int LZ4_freeStream (LZ4_stream_t* LZ4_stream)
{
    if (!LZ4_stream) return 0;   /* support free on NULL */
    DEBUGLOG(5, "LZ4_freeStream %p", LZ4_stream);
    FREEMEM(LZ4_stream);
    return (0);
}


#define HASH_UNIT sizeof(reg_t)
int LZ4_loadDict (LZ4_stream_t* LZ4_dict, const char* dictionary, int dictSize)
{
    LZ4_stream_t_internal* dict = &LZ4_dict->internal_donotuse;
    const tableType_t tableType = byU32;
    const BYTE* p = (const BYTE*)dictionary;
    const BYTE* const dictEnd = p + dictSize;
    const BYTE* base;

    DEBUGLOG(4, "LZ4_loadDict (%i bytes from %p into %p)", dictSize, dictionary, LZ4_dict);

    /* It's necessary to reset the context,
     * and not just continue it with prepareTable()
     * to avoid any risk of generating overflowing matchIndex
     * when compressing using this dictionary */
    LZ4_resetStream(LZ4_dict);

    /* We always increment the offset by 64 KB, since, if the dict is longer,
     * we truncate it to the last 64k, and if it's shorter, we still want to
     * advance by a whole window length so we can provide the guarantee that
     * there are only valid offsets in the window, which allows an optimization
     * in LZ4_compress_fast_continue() where it uses noDictIssue even when the
     * dictionary isn't a full 64k. */
    dict->currentOffset += 64 KB;

    if (dictSize < (int)HASH_UNIT) {
        return 0;
    }

    if ((dictEnd - p) > 64 KB) p = dictEnd - 64 KB;
    base = dictEnd - dict->currentOffset;
    dict->dictionary = p;
    dict->dictSize = (U32)(dictEnd - p);
    dict->tableType = (U32)tableType;

    while (p <= dictEnd-HASH_UNIT) {
        LZ4_putPosition(p, dict->hashTable, tableType, base);
        p+=3;
    }

    return (int)dict->dictSize;
}

void LZ4_attach_dictionary(LZ4_stream_t* workingStream, const LZ4_stream_t* dictionaryStream) {
    const LZ4_stream_t_internal* dictCtx = dictionaryStream == NULL ? NULL :
        &(dictionaryStream->internal_donotuse);

    DEBUGLOG(4, "LZ4_attach_dictionary (%p, %p, size %u)",
             workingStream, dictionaryStream,
             dictCtx != NULL ? dictCtx->dictSize : 0);

    if (dictCtx != NULL) {
        /* If the current offset is zero, we will never look in the
         * external dictionary context, since there is no value a table
         * entry can take that indicate a miss. In that case, we need
         * to bump the offset to something non-zero.
         */
        if (workingStream->internal_donotuse.currentOffset == 0) {
            workingStream->internal_donotuse.currentOffset = 64 KB;
        }

        /* Don't actually attach an empty dictionary.
         */
        if (dictCtx->dictSize == 0) {
            dictCtx = NULL;
        }
    }
    workingStream->internal_donotuse.dictCtx = dictCtx;
}


static void LZ4_renormDictT(LZ4_stream_t_internal* LZ4_dict, int nextSize)
{
    assert(nextSize >= 0);
    if (LZ4_dict->currentOffset + (unsigned)nextSize > 0x80000000) {   /* potential ptrdiff_t overflow (32-bits mode) */
        /* rescale hash table */
        U32 const delta = LZ4_dict->currentOffset - 64 KB;
        const BYTE* dictEnd = LZ4_dict->dictionary + LZ4_dict->dictSize;
        int i;
        DEBUGLOG(4, "LZ4_renormDictT");
        for (i=0; i<LZ4_HASH_SIZE_U32; i++) {
            if (LZ4_dict->hashTable[i] < delta) LZ4_dict->hashTable[i]=0;
            else LZ4_dict->hashTable[i] -= delta;
        }
        LZ4_dict->currentOffset = 64 KB;
        if (LZ4_dict->dictSize > 64 KB) LZ4_dict->dictSize = 64 KB;
        LZ4_dict->dictionary = dictEnd - LZ4_dict->dictSize;
    }
}


int LZ4_compress_fast_continue (LZ4_stream_t* LZ4_stream,
                                const char* source, char* dest,
                                int inputSize, int maxOutputSize,
                                int acceleration)
{
    const tableType_t tableType = byU32;
    LZ4_stream_t_internal* streamPtr = &LZ4_stream->internal_donotuse;
    const BYTE* dictEnd = streamPtr->dictionary + streamPtr->dictSize;

    DEBUGLOG(5, "LZ4_compress_fast_continue (inputSize=%i)", inputSize);

    LZ4_renormDictT(streamPtr, inputSize);   /* avoid index overflow */
    if (acceleration < 1) acceleration = LZ4_ACCELERATION_DEFAULT;
    if (acceleration > LZ4_ACCELERATION_MAX) acceleration = LZ4_ACCELERATION_MAX;

    /* invalidate tiny dictionaries */
    if ( (streamPtr->dictSize-1 < 4-1)   /* intentional underflow */
      && (dictEnd != (const BYTE*)source) ) {
        DEBUGLOG(5, "LZ4_compress_fast_continue: dictSize(%u) at addr:%p is too small", streamPtr->dictSize, streamPtr->dictionary);
        streamPtr->dictSize = 0;
        streamPtr->dictionary = (const BYTE*)source;
        dictEnd = (const BYTE*)source;
    }

    /* Check overlapping input/dictionary space */
    {   const BYTE* sourceEnd = (const BYTE*) source + inputSize;
        if ((sourceEnd > streamPtr->dictionary) && (sourceEnd < dictEnd)) {
            streamPtr->dictSize = (U32)(dictEnd - sourceEnd);
            if (streamPtr->dictSize > 64 KB) streamPtr->dictSize = 64 KB;
            if (streamPtr->dictSize < 4) streamPtr->dictSize = 0;
            streamPtr->dictionary = dictEnd - streamPtr->dictSize;
        }
    }

    /* prefix mode : source data follows dictionary */
    if (dictEnd == (const BYTE*)source) {
        if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset))
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, dictSmall, acceleration);
        else
            return LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, withPrefix64k, noDictIssue, acceleration);
    }

    /* external dictionary mode */
    {   int result;
        if (streamPtr->dictCtx) {
            /* We depend here on the fact that dictCtx'es (produced by
             * LZ4_loadDict) guarantee that their tables contain no references
             * to offsets between dictCtx->currentOffset - 64 KB and
             * dictCtx->currentOffset - dictCtx->dictSize. This makes it safe
             * to use noDictIssue even when the dict isn't a full 64 KB.
             */
            if (inputSize > 4 KB) {
                /* For compressing large blobs, it is faster to pay the setup
                 * cost to copy the dictionary's tables into the active context,
                 * so that the compression loop is only looking into one table.
                 */
                LZ4_memcpy(streamPtr, streamPtr->dictCtx, sizeof(*streamPtr));
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingDictCtx, noDictIssue, acceleration);
            }
        } else {
            if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, dictSmall, acceleration);
            } else {
                result = LZ4_compress_generic(streamPtr, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, usingExtDict, noDictIssue, acceleration);
            }
        }
        streamPtr->dictionary = (const BYTE*)source;
        streamPtr->dictSize = (U32)inputSize;
        return result;
    }
}


/* Hidden debug function, to force-test external dictionary mode */
int LZ4_compress_forceExtDict (LZ4_stream_t* LZ4_dict, const char* source, char* dest, int srcSize)
{
    LZ4_stream_t_internal* streamPtr = &LZ4_dict->internal_donotuse;
    int result;

    LZ4_renormDictT(streamPtr, srcSize);

    if ((streamPtr->dictSize < 64 KB) && (streamPtr->dictSize < streamPtr->currentOffset)) {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, dictSmall, 1);
    } else {
        result = LZ4_compress_generic(streamPtr, source, dest, srcSize, NULL, 0, notLimited, byU32, usingExtDict, noDictIssue, 1);
    }

    streamPtr->dictionary = (const BYTE*)source;
    streamPtr->dictSize = (U32)srcSize;

    return result;
}


/*! LZ4_saveDict() :
 *  If previously compressed data block is not guaranteed to remain available at its memory location,
 *  save it into a safer place (char* safeBuffer).
 *  Note : you don't need to call LZ4_loadDict() afterwards,
 *         dictionary is immediately usable, you can therefore call LZ4_compress_fast_continue().
 *  Return : saved dictionary size in bytes (necessarily <= dictSize), or 0 if error.
 */
int LZ4_saveDict (LZ4_stream_t* LZ4_dict, char* safeBuffer, int dictSize)
{
    LZ4_stream_t_internal* const dict = &LZ4_dict->internal_donotuse;
    const BYTE* const previousDictEnd = dict->dictionary + dict->dictSize;

    if ((U32)dictSize > 64 KB) { dictSize = 64 KB; } /* useless to define a dictionary > 64 KB */
    if ((U32)dictSize > dict->dictSize) { dictSize = (int)dict->dictSize; }

    if (safeBuffer == NULL) assert(dictSize == 0);
    if (dictSize > 0)
        memmove(safeBuffer, previousDictEnd - dictSize, dictSize);

    dict->dictionary = (const BYTE*)safeBuffer;
    dict->dictSize = (U32)dictSize;

    return dictSize;
}



/*-*******************************
 *  Decompression functions
 ********************************/

typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { decode_full_block = 0, partial_decode = 1 } earlyEnd_directive;

#undef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

/* Read the variable-length literal or match length.
 *
 * ip - pointer to use as input.
 * lencheck - end ip.  Return an error if ip advances >= lencheck.
 * loop_check - check ip >= lencheck in body of loop.  Returns loop_error if so.
 * initial_check - check ip >= lencheck before start of loop.  Returns initial_error if so.
 * error (output) - error code.  Should be set to 0 before call.
 */
typedef enum { loop_error = -2, initial_error = -1, ok = 0 } variable_length_error;
LZ4_FORCE_INLINE unsigned
read_variable_length(const BYTE**ip, const BYTE* lencheck,
                     int loop_check, int initial_check,
                     variable_length_error* error)
{
    U32 length = 0;
    U32 s;
    if (initial_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
        *error = initial_error;
        return length;
    }
    do {
        s = **ip;
        (*ip)++;
        length += s;
        if (loop_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
            *error = loop_error;
            return length;
        }
    } while (s==255);

    return length;
}

/*! LZ4_decompress_generic() :
 *  This generic decompression function covers all use cases.
 *  It shall be instantiated several times, using different sets of directives.
 *  Note that it is important for performance that this function really get inlined,
 *  in order to remove useless branches during compilation optimization.
 */
LZ4_FORCE_INLINE int
LZ4_decompress_generic(
                 const char* const src,
                 char* const dst,
                 int srcSize,
                 int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

                 endCondition_directive endOnInput,   /* endOnOutputSize, endOnInputSize */
                 earlyEnd_directive partialDecoding,  /* full, partial */
                 dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
                 const BYTE* const lowPrefix,  /* always <= dst, == dst when no prefix */
                 const BYTE* const dictStart,  /* only if dict==usingExtDict */
                 const size_t dictSize         /* note : = 0 if noDict */
                 )
{
    if (src == NULL) { return -1; }

    {   const BYTE* ip = (const BYTE*) src;
        const BYTE* const iend = ip + srcSize;

        BYTE* op = (BYTE*) dst;
        BYTE* const oend = op + outputSize;
        BYTE* cpy;

        const BYTE* const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

        const int safeDecode = (endOnInput==endOnInputSize);
        const int checkOffset = ((safeDecode) && (dictSize < (int)(64 KB)));


        /* Set up the "end" pointers for the shortcut. */
        const BYTE* const shortiend = iend - (endOnInput ? 14 : 8) /*maxLL*/ - 2 /*offset*/;
        const BYTE* const shortoend = oend - (endOnInput ? 14 : 8) /*maxLL*/ - 18 /*maxML*/;

        const BYTE* match;
        size_t offset;
        unsigned token;
        size_t length;


        DEBUGLOG(5, "LZ4_decompress_generic (srcSize:%i, dstSize:%i)", srcSize, outputSize);

        /* Special cases */
        assert(lowPrefix <= op);
        if ((endOnInput) && (unlikely(outputSize==0))) {
            /* Empty output buffer */
            if (partialDecoding) return 0;
            return ((srcSize==1) && (*ip==0)) ? 0 : -1;
        }
        if ((!endOnInput) && (unlikely(outputSize==0))) { return (*ip==0 ? 1 : -1); }
        if ((endOnInput) && unlikely(srcSize==0)) { return -1; }

	/* Currently the fast loop shows a regression on qualcomm arm chips. */
#if LZ4_FAST_DEC_LOOP
        if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
            DEBUGLOG(6, "skip fast decode loop");
            goto safe_decode;
        }

        /* Fast loop : decode sequences as long as output < iend-FASTLOOP_SAFE_DISTANCE */
        while (1) {
            /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
            assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
            if (endOnInput) { assert(ip < iend); }
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */

                /* copy literals */
                cpy = op+length;
                LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    if ((cpy>oend-32) || (ip+length>iend-32)) { goto safe_literal_copy; }
                    LZ4_wildCopy32(op, ip, cpy);
                } else {   /* LZ4_decompress_fast() */
                    if (cpy>oend-8) { goto safe_literal_copy; }
                    LZ4_wildCopy8(op, ip, cpy); /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                                                 * it doesn't know input length, and only relies on end-of-block properties */
                }
                ip += length; op = cpy;
            } else {
                cpy = op+length;
                if (endOnInput) {  /* LZ4_decompress_safe() */
                    DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
                    /* We don't need to check oend, since we check it once for each loop below */
                    if (ip > iend-(16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
                    /* Literals can only be 14, but hope compilers optimize if we copy by a register size */
                    LZ4_memcpy(op, ip, 16);
                } else {  /* LZ4_decompress_fast() */
                    /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                     * it doesn't know input length, and relies on end-of-block properties */
                    LZ4_memcpy(op, ip, 8);
                    if (length > 8) { LZ4_memcpy(op+8, ip+8, 8); }
                }
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;
            assert(match <= op);

            /* get matchlength */
            length = token & ML_MASK;

            if (length == ML_MASK) {
                variable_length_error error = ok;
                if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
                length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
                if (error != ok) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) { goto _output_error; } /* overflow detection */
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }
            } else {
                length += MINMATCH;
                if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
                    goto safe_match_copy;
                }

                /* Fastpath check: Avoids a branch in LZ4_wildCopy32 if true */
                if ((dict == withPrefix64k) || (match >= lowPrefix)) {
                    if (offset >= 8) {
                        assert(match >= lowPrefix);
                        assert(match <= op);
                        assert(op + 18 <= oend);

                        LZ4_memcpy(op, match, 8);
                        LZ4_memcpy(op+8, match+8, 8);
                        LZ4_memcpy(op+16, match+16, 2);
                        op += length;
                        continue;
            }   }   }

            if (checkOffset && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) {
                        DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
                        length = MIN(length, (size_t)(oend-op));
                    } else {
                        goto _output_error;  /* end-of-block condition violated */
                }   }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) { *op++ = *copyFrom++; }
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }

            /* copy match within block */
            cpy = op + length;

            assert((op <= oend) && (oend-op >= 32));
            if (unlikely(offset<16)) {
                LZ4_memcpy_using_offset(op, match, cpy, offset);
            } else {
                LZ4_wildCopy32(op, match, cpy);
            }

            op = cpy;   /* wildcopy correction */
        }
    safe_decode:
#endif

        /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
        while (1) {
            token = *ip++;
            length = token >> ML_BITS;  /* literal length */

            assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

            /* A two-stage shortcut for the most common case:
             * 1) If the literal length is 0..14, and there is enough space,
             * enter the shortcut and copy 16 bytes on behalf of the literals
             * (in the fast mode, only 8 bytes can be safely copied this way).
             * 2) Further if the match length is 4..18, copy 18 bytes in a similar
             * manner; but we ensure that there's enough space in the output for
             * those 18 bytes earlier, upon entering the shortcut (in other words,
             * there is a combined check for both stages).
             */
            if ( (endOnInput ? length != RUN_MASK : length <= 8)
                /* strictly "less than" on input, to re-enter the loop with at least one byte */
              && likely((endOnInput ? ip < shortiend : 1) & (op <= shortoend)) ) {
                /* Copy the literals */
                LZ4_memcpy(op, ip, endOnInput ? 16 : 8);
                op += length; ip += length;

                /* The second stage: prepare for match copying, decode full info.
                 * If it doesn't work out, the info won't be wasted. */
                length = token & ML_MASK; /* match length */
                offset = LZ4_readLE16(ip); ip += 2;
                match = op - offset;
                assert(match <= op); /* check overflow */

                /* Do not deal with overlapping matches. */
                if ( (length != ML_MASK)
                  && (offset >= 8)
                  && (dict==withPrefix64k || match >= lowPrefix) ) {
                    /* Copy the match. */
                    LZ4_memcpy(op + 0, match + 0, 8);
                    LZ4_memcpy(op + 8, match + 8, 8);
                    LZ4_memcpy(op +16, match +16, 2);
                    op += length + MINMATCH;
                    /* Both stages worked, load the next token. */
                    continue;
                }

                /* The second stage didn't work out, but the info is ready.
                 * Propel it right to the point of match copying. */
                goto _copy_match;
            }

            /* decode literal length */
            if (length == RUN_MASK) {
                variable_length_error error = ok;
                length += read_variable_length(&ip, iend-RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
                if (error == initial_error) { goto _output_error; }
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)(op))) { goto _output_error; } /* overflow detection */
                if ((safeDecode) && unlikely((uptrval)(ip)+length<(uptrval)(ip))) { goto _output_error; } /* overflow detection */
            }

            /* copy literals */
            cpy = op+length;
#if LZ4_FAST_DEC_LOOP
        safe_literal_copy:
#endif
            LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
            if ( ((endOnInput) && ((cpy>oend-MFLIMIT) || (ip+length>iend-(2+1+LASTLITERALS))) )
              || ((!endOnInput) && (cpy>oend-WILDCOPYLENGTH)) )
            {
                /* We've either hit the input parsing restriction or the output parsing restriction.
                 * In the normal scenario, decoding a full block, it must be the last sequence,
                 * otherwise it's an error (invalid input or dimensions).
                 * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
                 */
                if (partialDecoding) {
                    /* Since we are partial decoding we may be in this block because of the output parsing
                     * restriction, which is not valid since the output buffer is allowed to be undersized.
                     */
                    assert(endOnInput);
                    DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
                    DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
                    DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
                    DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
                    /* Finishing in the middle of a literals segment,
                     * due to lack of input.
                     */
                    if (ip+length > iend) {
                        length = (size_t)(iend-ip);
                        cpy = op + length;
                    }
                    /* Finishing in the middle of a literals segment,
                     * due to lack of output space.
                     */
                    if (cpy > oend) {
                        cpy = oend;
                        assert(op<=oend);
                        length = (size_t)(oend-op);
                    }
                } else {
                    /* We must be on the last sequence because of the parsing limitations so check
                     * that we exactly regenerate the original size (must be exact when !endOnInput).
                     */
                    if ((!endOnInput) && (cpy != oend)) { goto _output_error; }
                     /* We must be on the last sequence (or invalid) because of the parsing limitations
                      * so check that we exactly consume the input and don't overrun the output buffer.
                      */
                    if ((endOnInput) && ((ip+length != iend) || (cpy > oend))) {
                        DEBUGLOG(6, "should have been last run of literals")
                        DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip+length, iend);
                        DEBUGLOG(6, "or cpy(%p) > oend(%p)", cpy, oend);
                        goto _output_error;
                    }
                }
                memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
                ip += length;
                op += length;
                /* Necessarily EOF when !partialDecoding.
                 * When partialDecoding, it is EOF if we've either
                 * filled the output buffer or
                 * can't proceed with reading an offset for following match.
                 */
                if (!partialDecoding || (cpy == oend) || (ip >= (iend-2))) {
                    break;
                }
            } else {
                LZ4_wildCopy8(op, ip, cpy);   /* may overwrite up to WILDCOPYLENGTH beyond cpy */
                ip += length; op = cpy;
            }

            /* get offset */
            offset = LZ4_readLE16(ip); ip+=2;
            match = op - offset;

            /* get matchlength */
            length = token & ML_MASK;

    _copy_match:
            if (length == ML_MASK) {
              variable_length_error error = ok;
              length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
              if (error != ok) goto _output_error;
                if ((safeDecode) && unlikely((uptrval)(op)+length<(uptrval)op)) goto _output_error;   /* overflow detection */
            }
            length += MINMATCH;

#if LZ4_FAST_DEC_LOOP
        safe_match_copy:
#endif
            if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) goto _output_error;   /* Error : offset outside buffers */
            /* match starting within external dictionary */
            if ((dict==usingExtDict) && (match < lowPrefix)) {
                if (unlikely(op+length > oend-LASTLITERALS)) {
                    if (partialDecoding) length = MIN(length, (size_t)(oend-op));
                    else goto _output_error;   /* doesn't respect parsing restriction */
                }

                if (length <= (size_t)(lowPrefix-match)) {
                    /* match fits entirely within external dictionary : just copy */
                    memmove(op, dictEnd - (lowPrefix-match), length);
                    op += length;
                } else {
                    /* match stretches into both external dictionary and current block */
                    size_t const copySize = (size_t)(lowPrefix - match);
                    size_t const restSize = length - copySize;
                    LZ4_memcpy(op, dictEnd - copySize, copySize);
                    op += copySize;
                    if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
                        BYTE* const endOfMatch = op + restSize;
                        const BYTE* copyFrom = lowPrefix;
                        while (op < endOfMatch) *op++ = *copyFrom++;
                    } else {
                        LZ4_memcpy(op, lowPrefix, restSize);
                        op += restSize;
                }   }
                continue;
            }
            assert(match >= lowPrefix);

            /* copy match within block */
            cpy = op + length;

            /* partialDecoding : may end anywhere within the block */
            assert(op<=oend);
            if (partialDecoding && (cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                size_t const mlen = MIN(length, (size_t)(oend-op));
                const BYTE* const matchEnd = match + mlen;
                BYTE* const copyEnd = op + mlen;
                if (matchEnd > op) {   /* overlap copy */
                    while (op < copyEnd) { *op++ = *match++; }
                } else {
                    LZ4_memcpy(op, match, mlen);
                }
                op = copyEnd;
                if (op == oend) { break; }
                continue;
            }

            if (unlikely(offset<8)) {
                LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
                op[0] = match[0];
                op[1] = match[1];
                op[2] = match[2];
                op[3] = match[3];
                match += inc32table[offset];
                LZ4_memcpy(op+4, match, 4);
                match -= dec64table[offset];
            } else {
                LZ4_memcpy(op, match, 8);
                match += 8;
            }
            op += 8;

            if (unlikely(cpy > oend-MATCH_SAFEGUARD_DISTANCE)) {
                BYTE* const oCopyLimit = oend - (WILDCOPYLENGTH-1);
                if (cpy > oend-LASTLITERALS) { goto _output_error; } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
                if (op < oCopyLimit) {
                    LZ4_wildCopy8(op, match, oCopyLimit);
                    match += oCopyLimit - op;
                    op = oCopyLimit;
                }
                while (op < cpy) { *op++ = *match++; }
            } else {
                LZ4_memcpy(op, match, 8);
                if (length > 16)  { LZ4_wildCopy8(op+8, match+8, cpy); }
            }
            op = cpy;   /* wildcopy correction */
        }

        /* end of decoding */
        if (endOnInput) {
            DEBUGLOG(5, "decoded %i bytes", (int) (((char*)op)-dst));
           return (int) (((char*)op)-dst);     /* Nb of output bytes decoded */
       } else {
           return (int) (((const char*)ip)-src);   /* Nb of input bytes read */
       }

        /* Overflow error detected */
    _output_error:
        return (int) (-(((const char*)ip)-src))-1;
    }
}


/*===== Instantiate the API decoding functions. =====*/

LZ4_FORCE_O2
int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_partial(const char* src, char* dst, int compressedSize, int targetOutputSize, int dstCapacity)
{
    dstCapacity = MIN(targetOutputSize, dstCapacity);
    return LZ4_decompress_generic(src, dst, compressedSize, dstCapacity,
                                  endOnInputSize, partial_decode,
                                  noDict, (BYTE*)dst, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_fast(const char* source, char* dest, int originalSize)
{
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

/*===== Instantiate a few more decoding cases, used more than once. =====*/

LZ4_FORCE_O2 /* Exported, an obsolete API function. */
int LZ4_decompress_safe_withPrefix64k(const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, withPrefix64k,
                                  (BYTE*)dest - 64 KB, NULL, 0);
}

/* Another obsolete API function, paired with the previous one. */
int LZ4_decompress_fast_withPrefix64k(const char* source, char* dest, int originalSize)
{
    /* LZ4_decompress_fast doesn't validate match offsets,
     * and thus serves well with any prefixed dictionary. */
    return LZ4_decompress_fast(source, dest, originalSize);
}

LZ4_FORCE_O2
static int LZ4_decompress_safe_withSmallPrefix(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                               size_t prefixSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, noDict,
                                  (BYTE*)dest-prefixSize, NULL, 0);
}

LZ4_FORCE_O2
int LZ4_decompress_safe_forceExtDict(const char* source, char* dest,
                                     int compressedSize, int maxOutputSize,
                                     const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_O2
static int LZ4_decompress_fast_extDict(const char* source, char* dest, int originalSize,
                                       const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest, (const BYTE*)dictStart, dictSize);
}

/* The "double dictionary" mode, for use with e.g. ring buffers: the first part
 * of the dictionary is passed as prefix, and the second via dictStart + dictSize.
 * These routines are used only once, in LZ4_decompress_*_continue().
 */
LZ4_FORCE_INLINE
int LZ4_decompress_safe_doubleDict(const char* source, char* dest, int compressedSize, int maxOutputSize,
                                   size_t prefixSize, const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, compressedSize, maxOutputSize,
                                  endOnInputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest-prefixSize, (const BYTE*)dictStart, dictSize);
}

LZ4_FORCE_INLINE
int LZ4_decompress_fast_doubleDict(const char* source, char* dest, int originalSize,
                                   size_t prefixSize, const void* dictStart, size_t dictSize)
{
    return LZ4_decompress_generic(source, dest, 0, originalSize,
                                  endOnOutputSize, decode_full_block, usingExtDict,
                                  (BYTE*)dest-prefixSize, (const BYTE*)dictStart, dictSize);
}

/*===== streaming decompression functions =====*/

LZ4_streamDecode_t* LZ4_createStreamDecode(void)
{
    LZ4_streamDecode_t* lz4s = (LZ4_streamDecode_t*) ALLOC_AND_ZERO(sizeof(LZ4_streamDecode_t));
    LZ4_STATIC_ASSERT(LZ4_STREAMDECODESIZE >= sizeof(LZ4_streamDecode_t_internal));    /* A compilation error here means LZ4_STREAMDECODESIZE is not large enough */
    return lz4s;
}

int LZ4_freeStreamDecode (LZ4_streamDecode_t* LZ4_stream)
{
    if (LZ4_stream == NULL) { return 0; }  /* support free on NULL */
    FREEMEM(LZ4_stream);
    return 0;
}

/*! LZ4_setStreamDecode() :
 *  Use this function to instruct where to find the dictionary.
 *  This function is not necessary if previous data is still available where it was decoded.
 *  Loading a size of 0 is allowed (same effect as no dictionary).
 * @return : 1 if OK, 0 if error
 */
int LZ4_setStreamDecode (LZ4_streamDecode_t* LZ4_streamDecode, const char* dictionary, int dictSize)
{
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    lz4sd->prefixSize = (size_t) dictSize;
    lz4sd->prefixEnd = (const BYTE*) dictionary + dictSize;
    lz4sd->externalDict = NULL;
    lz4sd->extDictSize  = 0;
    return 1;
}

/*! LZ4_decoderRingBufferSize() :
 *  when setting a ring buffer for streaming decompression (optional scenario),
 *  provides the minimum size of this ring buffer
 *  to be compatible with any source respecting maxBlockSize condition.
 *  Note : in a ring buffer scenario,
 *  blocks are presumed decompressed next to each other.
 *  When not enough space remains for next block (remainingSize < maxBlockSize),
 *  decoding resumes from beginning of ring buffer.
 * @return : minimum ring buffer size,
 *           or 0 if there is an error (invalid maxBlockSize).
 */
int LZ4_decoderRingBufferSize(int maxBlockSize)
{
    if (maxBlockSize < 0) return 0;
    if (maxBlockSize > LZ4_MAX_INPUT_SIZE) return 0;
    if (maxBlockSize < 16) maxBlockSize = 16;
    return LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize);
}

/*
*_continue() :
    These decoding functions allow decompression of multiple blocks in "streaming" mode.
    Previously decoded blocks must still be available at the memory position where they were decoded.
    If it's not possible, save the relevant part of decoded data into a safe buffer,
    and indicate where it stands using LZ4_setStreamDecode()
*/
LZ4_FORCE_O2
int LZ4_decompress_safe_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* source, char* dest, int compressedSize, int maxOutputSize)
{
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    int result;

    if (lz4sd->prefixSize == 0) {
        /* The first call, no dictionary yet. */
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd = (BYTE*)dest + result;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        /* They're rolling the current segment. */
        if (lz4sd->prefixSize >= 64 KB - 1)
            result = LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        else if (lz4sd->extDictSize == 0)
            result = LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize,
                                                         lz4sd->prefixSize);
        else
            result = LZ4_decompress_safe_doubleDict(source, dest, compressedSize, maxOutputSize,
                                                    lz4sd->prefixSize, lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)result;
        lz4sd->prefixEnd  += result;
    } else {
        /* The buffer wraps around, or they're switching to another buffer. */
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize,
                                                  lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)result;
        lz4sd->prefixEnd  = (BYTE*)dest + result;
    }

    return result;
}

LZ4_FORCE_O2
int LZ4_decompress_fast_continue (LZ4_streamDecode_t* LZ4_streamDecode, const char* source, char* dest, int originalSize)
{
    LZ4_streamDecode_t_internal* lz4sd = &LZ4_streamDecode->internal_donotuse;
    int result;
    assert(originalSize >= 0);

    if (lz4sd->prefixSize == 0) {
        assert(lz4sd->extDictSize == 0);
        result = LZ4_decompress_fast(source, dest, originalSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd = (BYTE*)dest + originalSize;
    } else if (lz4sd->prefixEnd == (BYTE*)dest) {
        if (lz4sd->prefixSize >= 64 KB - 1 || lz4sd->extDictSize == 0)
            result = LZ4_decompress_fast(source, dest, originalSize);
        else
            result = LZ4_decompress_fast_doubleDict(source, dest, originalSize,
                                                    lz4sd->prefixSize, lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize += (size_t)originalSize;
        lz4sd->prefixEnd  += originalSize;
    } else {
        lz4sd->extDictSize = lz4sd->prefixSize;
        lz4sd->externalDict = lz4sd->prefixEnd - lz4sd->extDictSize;
        result = LZ4_decompress_fast_extDict(source, dest, originalSize,
                                             lz4sd->externalDict, lz4sd->extDictSize);
        if (result <= 0) return result;
        lz4sd->prefixSize = (size_t)originalSize;
        lz4sd->prefixEnd  = (BYTE*)dest + originalSize;
    }

    return result;
}


/*
Advanced decoding functions :
*_usingDict() :
    These decoding functions work the same as "_continue" ones,
    the dictionary must be explicitly provided within parameters
*/

int LZ4_decompress_safe_usingDict(const char* source, char* dest, int compressedSize, int maxOutputSize, const char* dictStart, int dictSize)
{
    if (dictSize==0)
        return LZ4_decompress_safe(source, dest, compressedSize, maxOutputSize);
    if (dictStart+dictSize == dest) {
        if (dictSize >= 64 KB - 1) {
            return LZ4_decompress_safe_withPrefix64k(source, dest, compressedSize, maxOutputSize);
        }
        assert(dictSize >= 0);
        return LZ4_decompress_safe_withSmallPrefix(source, dest, compressedSize, maxOutputSize, (size_t)dictSize);
    }
    assert(dictSize >= 0);
    return LZ4_decompress_safe_forceExtDict(source, dest, compressedSize, maxOutputSize, dictStart, (size_t)dictSize);
}

int LZ4_decompress_fast_usingDict(const char* source, char* dest, int originalSize, const char* dictStart, int dictSize)
{
    if (dictSize==0 || dictStart+dictSize == dest)
        return LZ4_decompress_fast(source, dest, originalSize);
    assert(dictSize >= 0);
    return LZ4_decompress_fast_extDict(source, dest, originalSize, dictStart, (size_t)dictSize);
}


/*=*************************************************
*  Obsolete Functions
***************************************************/
/* obsolete compression functions */
int LZ4_compress_limitedOutput(const char* source, char* dest, int inputSize, int maxOutputSize)
{
    return LZ4_compress_default(source, dest, inputSize, maxOutputSize);
}
int LZ4_compress(const char* src, char* dest, int srcSize)
{
    return LZ4_compress_default(src, dest, srcSize, LZ4_compressBound(srcSize));
}
int LZ4_compress_limitedOutput_withState (void* state, const char* src, char* dst, int srcSize, int dstSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, dstSize, 1);
}
int LZ4_compress_withState (void* state, const char* src, char* dst, int srcSize)
{
    return LZ4_compress_fast_extState(state, src, dst, srcSize, LZ4_compressBound(srcSize), 1);
}
int LZ4_compress_limitedOutput_continue (LZ4_stream_t* LZ4_stream, const char* src, char* dst, int srcSize, int dstCapacity)
{
    return LZ4_compress_fast_continue(LZ4_stream, src, dst, srcSize, dstCapacity, 1);
}
int LZ4_compress_continue (LZ4_stream_t* LZ4_stream, const char* source, char* dest, int inputSize)
{
    return LZ4_compress_fast_continue(LZ4_stream, source, dest, inputSize, LZ4_compressBound(inputSize), 1);
}

/*
These decompression functions are deprecated and should no longer be used.
They are only provided here for compatibility with older user programs.
- LZ4_uncompress is totally equivalent to LZ4_decompress_fast
- LZ4_uncompress_unknownOutputSize is totally equivalent to LZ4_decompress_safe
*/
int LZ4_uncompress (const char* source, char* dest, int outputSize)
{
    return LZ4_decompress_fast(source, dest, outputSize);
}
int LZ4_uncompress_unknownOutputSize (const char* source, char* dest, int isize, int maxOutputSize)
{
    return LZ4_decompress_safe(source, dest, isize, maxOutputSize);
}

/* Obsolete Streaming functions */

int LZ4_sizeofStreamState(void) { return LZ4_STREAMSIZE; }

int LZ4_resetStreamState(void* state, char* inputBuffer)
{
    (void)inputBuffer;
    LZ4_resetStream((LZ4_stream_t*)state);
    return 0;
}

void* LZ4_create (char* inputBuffer)
{
    (void)inputBuffer;
    return LZ4_createStream();
}

char* LZ4_slideInputBuffer (void* state)
{
    /* avoid const char * -> char * conversion warning */
    return (char *)(uptrval)((LZ4_stream_t*)state)->internal_donotuse.dictionary;
}

#endif   /* LZ4_COMMONDEFS_ONLY */
/*
    LZ4 HC - High Compression Mode of LZ4
    Copyright (C) 2011-2017, Yann Collet.

    BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the
    distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    You can contact the author at :
       - LZ4 source repository : https://github.com/lz4/lz4
       - LZ4 public forum : https://groups.google.com/forum/#!forum/lz4c
*/
/* note : lz4hc is not an independent module, it requires lz4.h/lz4.c for proper compilation */


/* *************************************
*  Tuning Parameter
***************************************/

/*! HEAPMODE :
 *  Select how default compression function will allocate workplace memory,
 *  in stack (0:fastest), or in heap (1:requires malloc()).
 *  Since workplace is rather large, heap mode is recommended.
 */
#ifndef LZ4HC_HEAPMODE
#  define LZ4HC_HEAPMODE 1
#endif


/*===    Dependency    ===*/
#define LZ4_HC_STATIC_LINKING_ONLY
#include "lz4hc.h"


/*===   Common definitions   ===*/
#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif
#if defined (__clang__)
#  pragma clang diagnostic ignored "-Wunused-function"
#endif

#define LZ4_COMMONDEFS_ONLY
#ifndef LZ4_SRC_INCLUDED
#include "lz4.c"   /* LZ4_count, constants, mem */
#endif


/*===   Enums   ===*/
typedef enum { noDictCtx, usingDictCtxHc } dictCtx_directive;


/*===   Constants   ===*/
#define OPTIMAL_ML (int)((ML_MASK-1)+MINMATCH)
#define LZ4_OPT_NUM   (1<<12)


/*===   Macros   ===*/
#define MIN(a,b)   ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)   ( (a) > (b) ? (a) : (b) )
#define HASH_FUNCTION(i)         (((i) * 2654435761U) >> ((MINMATCH*8)-LZ4HC_HASH_LOG))
#define DELTANEXTMAXD(p)         chainTable[(p) & LZ4HC_MAXD_MASK]    /* flexible, LZ4HC_MAXD dependent */
#define DELTANEXTU16(table, pos) table[(U16)(pos)]   /* faster */
/* Make fields passed to, and updated by LZ4HC_encodeSequence explicit */
#define UPDATABLE(ip, op, anchor) &ip, &op, &anchor

static U32 LZ4HC_hashPtr(const void* ptr) { return HASH_FUNCTION(LZ4_read32(ptr)); }


/**************************************
*  HC Compression
**************************************/
static void LZ4HC_clearTables (LZ4HC_CCtx_internal* hc4)
{
    MEM_INIT(hc4->hashTable, 0, sizeof(hc4->hashTable));
    MEM_INIT(hc4->chainTable, 0xFF, sizeof(hc4->chainTable));
}

static void LZ4HC_init_internal (LZ4HC_CCtx_internal* hc4, const BYTE* start)
{
    uptrval startingOffset = (uptrval)(hc4->end - hc4->base);
    if (startingOffset > 1 GB) {
        LZ4HC_clearTables(hc4);
        startingOffset = 0;
    }
    startingOffset += 64 KB;
    hc4->nextToUpdate = (U32) startingOffset;
    hc4->base = start - startingOffset;
    hc4->end = start;
    hc4->dictBase = start - startingOffset;
    hc4->dictLimit = (U32) startingOffset;
    hc4->lowLimit = (U32) startingOffset;
}


/* Update chains up to ip (excluded) */
LZ4_FORCE_INLINE void LZ4HC_Insert (LZ4HC_CCtx_internal* hc4, const BYTE* ip)
{
    U16* const chainTable = hc4->chainTable;
    U32* const hashTable  = hc4->hashTable;
    const BYTE* const base = hc4->base;
    U32 const target = (U32)(ip - base);
    U32 idx = hc4->nextToUpdate;

    while (idx < target) {
        U32 const h = LZ4HC_hashPtr(base+idx);
        size_t delta = idx - hashTable[h];
        if (delta>LZ4_DISTANCE_MAX) delta = LZ4_DISTANCE_MAX;
        DELTANEXTU16(chainTable, idx) = (U16)delta;
        hashTable[h] = idx;
        idx++;
    }

    hc4->nextToUpdate = target;
}

/** LZ4HC_countBack() :
 * @return : negative value, nb of common bytes before ip/match */
LZ4_FORCE_INLINE
int LZ4HC_countBack(const BYTE* const ip, const BYTE* const match,
                    const BYTE* const iMin, const BYTE* const mMin)
{
    int back = 0;
    int const min = (int)MAX(iMin - ip, mMin - match);
    assert(min <= 0);
    assert(ip >= iMin); assert((size_t)(ip-iMin) < (1U<<31));
    assert(match >= mMin); assert((size_t)(match - mMin) < (1U<<31));
    while ( (back > min)
         && (ip[back-1] == match[back-1]) )
            back--;
    return back;
}

#if defined(_MSC_VER)
#  define LZ4HC_rotl32(x,r) _rotl(x,r)
#else
#  define LZ4HC_rotl32(x,r) ((x << r) | (x >> (32 - r)))
#endif


static U32 LZ4HC_rotatePattern(size_t const rotate, U32 const pattern)
{
    size_t const bitsToRotate = (rotate & (sizeof(pattern) - 1)) << 3;
    if (bitsToRotate == 0) return pattern;
    return LZ4HC_rotl32(pattern, (int)bitsToRotate);
}

/* LZ4HC_countPattern() :
 * pattern32 must be a sample of repetitive pattern of length 1, 2 or 4 (but not 3!) */
static unsigned
LZ4HC_countPattern(const BYTE* ip, const BYTE* const iEnd, U32 const pattern32)
{
    const BYTE* const iStart = ip;
    reg_t const pattern = (sizeof(pattern)==8) ?
        (reg_t)pattern32 + (((reg_t)pattern32) << (sizeof(pattern)*4)) : pattern32;

    while (likely(ip < iEnd-(sizeof(pattern)-1))) {
        reg_t const diff = LZ4_read_ARCH(ip) ^ pattern;
        if (!diff) { ip+=sizeof(pattern); continue; }
        ip += LZ4_NbCommonBytes(diff);
        return (unsigned)(ip - iStart);
    }

    if (LZ4_isLittleEndian()) {
        reg_t patternByte = pattern;
        while ((ip<iEnd) && (*ip == (BYTE)patternByte)) {
            ip++; patternByte >>= 8;
        }
    } else {  /* big endian */
        U32 bitOffset = (sizeof(pattern)*8) - 8;
        while (ip < iEnd) {
            BYTE const byte = (BYTE)(pattern >> bitOffset);
            if (*ip != byte) break;
            ip ++; bitOffset -= 8;
        }
    }

    return (unsigned)(ip - iStart);
}

/* LZ4HC_reverseCountPattern() :
 * pattern must be a sample of repetitive pattern of length 1, 2 or 4 (but not 3!)
 * read using natural platform endianess */
static unsigned
LZ4HC_reverseCountPattern(const BYTE* ip, const BYTE* const iLow, U32 pattern)
{
    const BYTE* const iStart = ip;

    while (likely(ip >= iLow+4)) {
        if (LZ4_read32(ip-4) != pattern) break;
        ip -= 4;
    }
    {   const BYTE* bytePtr = (const BYTE*)(&pattern) + 3; /* works for any endianess */
        while (likely(ip>iLow)) {
            if (ip[-1] != *bytePtr) break;
            ip--; bytePtr--;
    }   }
    return (unsigned)(iStart - ip);
}

/* LZ4HC_protectDictEnd() :
 * Checks if the match is in the last 3 bytes of the dictionary, so reading the
 * 4 byte MINMATCH would overflow.
 * @returns true if the match index is okay.
 */
static int LZ4HC_protectDictEnd(U32 const dictLimit, U32 const matchIndex)
{
    return ((U32)((dictLimit - 1) - matchIndex) >= 3);
}

typedef enum { rep_untested, rep_not, rep_confirmed } repeat_state_e;
typedef enum { favorCompressionRatio=0, favorDecompressionSpeed } HCfavor_e;

LZ4_FORCE_INLINE int
LZ4HC_InsertAndGetWiderMatch (
    LZ4HC_CCtx_internal* hc4,
    const BYTE* const ip,
    const BYTE* const iLowLimit,
    const BYTE* const iHighLimit,
    int longest,
    const BYTE** matchpos,
    const BYTE** startpos,
    const int maxNbAttempts,
    const int patternAnalysis,
    const int chainSwap,
    const dictCtx_directive dict,
    const HCfavor_e favorDecSpeed)
{
    U16* const chainTable = hc4->chainTable;
    U32* const HashTable = hc4->hashTable;
    const LZ4HC_CCtx_internal * const dictCtx = hc4->dictCtx;
    const BYTE* const base = hc4->base;
    const U32 dictLimit = hc4->dictLimit;
    const BYTE* const lowPrefixPtr = base + dictLimit;
    const U32 ipIndex = (U32)(ip - base);
    const U32 lowestMatchIndex = (hc4->lowLimit + (LZ4_DISTANCE_MAX + 1) > ipIndex) ? hc4->lowLimit : ipIndex - LZ4_DISTANCE_MAX;
    const BYTE* const dictBase = hc4->dictBase;
    int const lookBackLength = (int)(ip-iLowLimit);
    int nbAttempts = maxNbAttempts;
    U32 matchChainPos = 0;
    U32 const pattern = LZ4_read32(ip);
    U32 matchIndex;
    repeat_state_e repeat = rep_untested;
    size_t srcPatternLength = 0;

    DEBUGLOG(7, "LZ4HC_InsertAndGetWiderMatch");
    /* First Match */
    LZ4HC_Insert(hc4, ip);
    matchIndex = HashTable[LZ4HC_hashPtr(ip)];
    DEBUGLOG(7, "First match at index %u / %u (lowestMatchIndex)",
                matchIndex, lowestMatchIndex);

    while ((matchIndex>=lowestMatchIndex) && (nbAttempts>0)) {
        int matchLength=0;
        nbAttempts--;
        assert(matchIndex < ipIndex);
        if (favorDecSpeed && (ipIndex - matchIndex < 8)) {
            /* do nothing */
        } else if (matchIndex >= dictLimit) {   /* within current Prefix */
            const BYTE* const matchPtr = base + matchIndex;
            assert(matchPtr >= lowPrefixPtr);
            assert(matchPtr < ip);
            assert(longest >= 1);
            if (LZ4_read16(iLowLimit + longest - 1) == LZ4_read16(matchPtr - lookBackLength + longest - 1)) {
                if (LZ4_read32(matchPtr) == pattern) {
                    int const back = lookBackLength ? LZ4HC_countBack(ip, matchPtr, iLowLimit, lowPrefixPtr) : 0;
                    matchLength = MINMATCH + (int)LZ4_count(ip+MINMATCH, matchPtr+MINMATCH, iHighLimit);
                    matchLength -= back;
                    if (matchLength > longest) {
                        longest = matchLength;
                        *matchpos = matchPtr + back;
                        *startpos = ip + back;
            }   }   }
        } else {   /* lowestMatchIndex <= matchIndex < dictLimit */
            const BYTE* const matchPtr = dictBase + matchIndex;
            if (LZ4_read32(matchPtr) == pattern) {
                const BYTE* const dictStart = dictBase + hc4->lowLimit;
                int back = 0;
                const BYTE* vLimit = ip + (dictLimit - matchIndex);
                if (vLimit > iHighLimit) vLimit = iHighLimit;
                matchLength = (int)LZ4_count(ip+MINMATCH, matchPtr+MINMATCH, vLimit) + MINMATCH;
                if ((ip+matchLength == vLimit) && (vLimit < iHighLimit))
                    matchLength += LZ4_count(ip+matchLength, lowPrefixPtr, iHighLimit);
                back = lookBackLength ? LZ4HC_countBack(ip, matchPtr, iLowLimit, dictStart) : 0;
                matchLength -= back;
                if (matchLength > longest) {
                    longest = matchLength;
                    *matchpos = base + matchIndex + back;   /* virtual pos, relative to ip, to retrieve offset */
                    *startpos = ip + back;
        }   }   }

        if (chainSwap && matchLength==longest) {    /* better match => select a better chain */
            assert(lookBackLength==0);   /* search forward only */
            if (matchIndex + (U32)longest <= ipIndex) {
                int const kTrigger = 4;
                U32 distanceToNextMatch = 1;
                int const end = longest - MINMATCH + 1;
                int step = 1;
                int accel = 1 << kTrigger;
                int pos;
                for (pos = 0; pos < end; pos += step) {
                    U32 const candidateDist = DELTANEXTU16(chainTable, matchIndex + (U32)pos);
                    step = (accel++ >> kTrigger);
                    if (candidateDist > distanceToNextMatch) {
                        distanceToNextMatch = candidateDist;
                        matchChainPos = (U32)pos;
                        accel = 1 << kTrigger;
                    }
                }
                if (distanceToNextMatch > 1) {
                    if (distanceToNextMatch > matchIndex) break;   /* avoid overflow */
                    matchIndex -= distanceToNextMatch;
                    continue;
        }   }   }

        {   U32 const distNextMatch = DELTANEXTU16(chainTable, matchIndex);
            if (patternAnalysis && distNextMatch==1 && matchChainPos==0) {
                U32 const matchCandidateIdx = matchIndex-1;
                /* may be a repeated pattern */
                if (repeat == rep_untested) {
                    if ( ((pattern & 0xFFFF) == (pattern >> 16))
                      &  ((pattern & 0xFF)   == (pattern >> 24)) ) {
                        repeat = rep_confirmed;
                        srcPatternLength = LZ4HC_countPattern(ip+sizeof(pattern), iHighLimit, pattern) + sizeof(pattern);
                    } else {
                        repeat = rep_not;
                }   }
                if ( (repeat == rep_confirmed) && (matchCandidateIdx >= lowestMatchIndex)
                  && LZ4HC_protectDictEnd(dictLimit, matchCandidateIdx) ) {
                    const int extDict = matchCandidateIdx < dictLimit;
                    const BYTE* const matchPtr = (extDict ? dictBase : base) + matchCandidateIdx;
                    if (LZ4_read32(matchPtr) == pattern) {  /* good candidate */
                        const BYTE* const dictStart = dictBase + hc4->lowLimit;
                        const BYTE* const iLimit = extDict ? dictBase + dictLimit : iHighLimit;
                        size_t forwardPatternLength = LZ4HC_countPattern(matchPtr+sizeof(pattern), iLimit, pattern) + sizeof(pattern);
                        if (extDict && matchPtr + forwardPatternLength == iLimit) {
                            U32 const rotatedPattern = LZ4HC_rotatePattern(forwardPatternLength, pattern);
                            forwardPatternLength += LZ4HC_countPattern(lowPrefixPtr, iHighLimit, rotatedPattern);
                        }
                        {   const BYTE* const lowestMatchPtr = extDict ? dictStart : lowPrefixPtr;
                            size_t backLength = LZ4HC_reverseCountPattern(matchPtr, lowestMatchPtr, pattern);
                            size_t currentSegmentLength;
                            if (!extDict && matchPtr - backLength == lowPrefixPtr && hc4->lowLimit < dictLimit) {
                                U32 const rotatedPattern = LZ4HC_rotatePattern((U32)(-(int)backLength), pattern);
                                backLength += LZ4HC_reverseCountPattern(dictBase + dictLimit, dictStart, rotatedPattern);
                            }
                            /* Limit backLength not go further than lowestMatchIndex */
                            backLength = matchCandidateIdx - MAX(matchCandidateIdx - (U32)backLength, lowestMatchIndex);
                            assert(matchCandidateIdx - backLength >= lowestMatchIndex);
                            currentSegmentLength = backLength + forwardPatternLength;
                            /* Adjust to end of pattern if the source pattern fits, otherwise the beginning of the pattern */
                            if ( (currentSegmentLength >= srcPatternLength)   /* current pattern segment large enough to contain full srcPatternLength */
                              && (forwardPatternLength <= srcPatternLength) ) { /* haven't reached this position yet */
                                U32 const newMatchIndex = matchCandidateIdx + (U32)forwardPatternLength - (U32)srcPatternLength;  /* best position, full pattern, might be followed by more match */
                                if (LZ4HC_protectDictEnd(dictLimit, newMatchIndex))
                                    matchIndex = newMatchIndex;
                                else {
                                    /* Can only happen if started in the prefix */
                                    assert(newMatchIndex >= dictLimit - 3 && newMatchIndex < dictLimit && !extDict);
                                    matchIndex = dictLimit;
                                }
                            } else {
                                U32 const newMatchIndex = matchCandidateIdx - (U32)backLength;   /* farthest position in current segment, will find a match of length currentSegmentLength + maybe some back */
                                if (!LZ4HC_protectDictEnd(dictLimit, newMatchIndex)) {
                                    assert(newMatchIndex >= dictLimit - 3 && newMatchIndex < dictLimit && !extDict);
                                    matchIndex = dictLimit;
                                } else {
                                    matchIndex = newMatchIndex;
                                    if (lookBackLength==0) {  /* no back possible */
                                        size_t const maxML = MIN(currentSegmentLength, srcPatternLength);
                                        if ((size_t)longest < maxML) {
                                            assert(base + matchIndex != ip);
                                            if ((size_t)(ip - base) - matchIndex > LZ4_DISTANCE_MAX) break;
                                            assert(maxML < 2 GB);
                                            longest = (int)maxML;
                                            *matchpos = base + matchIndex;   /* virtual pos, relative to ip, to retrieve offset */
                                            *startpos = ip;
                                        }
                                        {   U32 const distToNextPattern = DELTANEXTU16(chainTable, matchIndex);
                                            if (distToNextPattern > matchIndex) break;  /* avoid overflow */
                                            matchIndex -= distToNextPattern;
                        }   }   }   }   }
                        continue;
                }   }
        }   }   /* PA optimization */

        /* follow current chain */
        matchIndex -= DELTANEXTU16(chainTable, matchIndex + matchChainPos);

    }  /* while ((matchIndex>=lowestMatchIndex) && (nbAttempts)) */

    if ( dict == usingDictCtxHc
      && nbAttempts > 0
      && ipIndex - lowestMatchIndex < LZ4_DISTANCE_MAX) {
        size_t const dictEndOffset = (size_t)(dictCtx->end - dictCtx->base);
        U32 dictMatchIndex = dictCtx->hashTable[LZ4HC_hashPtr(ip)];
        assert(dictEndOffset <= 1 GB);
        matchIndex = dictMatchIndex + lowestMatchIndex - (U32)dictEndOffset;
        while (ipIndex - matchIndex <= LZ4_DISTANCE_MAX && nbAttempts--) {
            const BYTE* const matchPtr = dictCtx->base + dictMatchIndex;

            if (LZ4_read32(matchPtr) == pattern) {
                int mlt;
                int back = 0;
                const BYTE* vLimit = ip + (dictEndOffset - dictMatchIndex);
                if (vLimit > iHighLimit) vLimit = iHighLimit;
                mlt = (int)LZ4_count(ip+MINMATCH, matchPtr+MINMATCH, vLimit) + MINMATCH;
                back = lookBackLength ? LZ4HC_countBack(ip, matchPtr, iLowLimit, dictCtx->base + dictCtx->dictLimit) : 0;
                mlt -= back;
                if (mlt > longest) {
                    longest = mlt;
                    *matchpos = base + matchIndex + back;
                    *startpos = ip + back;
            }   }

            {   U32 const nextOffset = DELTANEXTU16(dictCtx->chainTable, dictMatchIndex);
                dictMatchIndex -= nextOffset;
                matchIndex -= nextOffset;
    }   }   }

    return longest;
}

LZ4_FORCE_INLINE
int LZ4HC_InsertAndFindBestMatch(LZ4HC_CCtx_internal* const hc4,   /* Index table will be updated */
                                 const BYTE* const ip, const BYTE* const iLimit,
                                 const BYTE** matchpos,
                                 const int maxNbAttempts,
                                 const int patternAnalysis,
                                 const dictCtx_directive dict)
{
    const BYTE* uselessPtr = ip;
    /* note : LZ4HC_InsertAndGetWiderMatch() is able to modify the starting position of a match (*startpos),
     * but this won't be the case here, as we define iLowLimit==ip,
     * so LZ4HC_InsertAndGetWiderMatch() won't be allowed to search past ip */
    return LZ4HC_InsertAndGetWiderMatch(hc4, ip, ip, iLimit, MINMATCH-1, matchpos, &uselessPtr, maxNbAttempts, patternAnalysis, 0 /*chainSwap*/, dict, favorCompressionRatio);
}

/* LZ4HC_encodeSequence() :
 * @return : 0 if ok,
 *           1 if buffer issue detected */
LZ4_FORCE_INLINE int LZ4HC_encodeSequence (
    const BYTE** _ip,
    BYTE** _op,
    const BYTE** _anchor,
    int matchLength,
    const BYTE* const match,
    limitedOutput_directive limit,
    BYTE* oend)
{
#define ip      (*_ip)
#define op      (*_op)
#define anchor  (*_anchor)

    size_t length;
    BYTE* const token = op++;

#if defined(LZ4_DEBUG) && (LZ4_DEBUG >= 6)
    static const BYTE* start = NULL;
    static U32 totalCost = 0;
    U32 const pos = (start==NULL) ? 0 : (U32)(anchor - start);
    U32 const ll = (U32)(ip - anchor);
    U32 const llAdd = (ll>=15) ? ((ll-15) / 255) + 1 : 0;
    U32 const mlAdd = (matchLength>=19) ? ((matchLength-19) / 255) + 1 : 0;
    U32 const cost = 1 + llAdd + ll + 2 + mlAdd;
    if (start==NULL) start = anchor;  /* only works for single segment */
    /* g_debuglog_enable = (pos >= 2228) & (pos <= 2262); */
    DEBUGLOG(6, "pos:%7u -- literals:%4u, match:%4i, offset:%5u, cost:%4u + %5u",
                pos,
                (U32)(ip - anchor), matchLength, (U32)(ip-match),
                cost, totalCost);
    totalCost += cost;
#endif

    /* Encode Literal length */
    length = (size_t)(ip - anchor);
    LZ4_STATIC_ASSERT(notLimited == 0);
    /* Check output limit */
    if (limit && ((op + (length / 255) + length + (2 + 1 + LASTLITERALS)) > oend)) {
        DEBUGLOG(6, "Not enough room to write %i literals (%i bytes remaining)",
                (int)length, (int)(oend - op));
        return 1;
    }
    if (length >= RUN_MASK) {
        size_t len = length - RUN_MASK;
        *token = (RUN_MASK << ML_BITS);
        for(; len >= 255 ; len -= 255) *op++ = 255;
        *op++ = (BYTE)len;
    } else {
        *token = (BYTE)(length << ML_BITS);
    }

    /* Copy Literals */
    LZ4_wildCopy8(op, anchor, op + length);
    op += length;

    /* Encode Offset */
    assert( (ip - match) <= LZ4_DISTANCE_MAX );   /* note : consider providing offset as a value, rather than as a pointer difference */
    LZ4_writeLE16(op, (U16)(ip - match)); op += 2;

    /* Encode MatchLength */
    assert(matchLength >= MINMATCH);
    length = (size_t)matchLength - MINMATCH;
    if (limit && (op + (length / 255) + (1 + LASTLITERALS) > oend)) {
        DEBUGLOG(6, "Not enough room to write match length");
        return 1;   /* Check output limit */
    }
    if (length >= ML_MASK) {
        *token += ML_MASK;
        length -= ML_MASK;
        for(; length >= 510 ; length -= 510) { *op++ = 255; *op++ = 255; }
        if (length >= 255) { length -= 255; *op++ = 255; }
        *op++ = (BYTE)length;
    } else {
        *token += (BYTE)(length);
    }

    /* Prepare next loop */
    ip += matchLength;
    anchor = ip;

    return 0;
}
#undef ip
#undef op
#undef anchor

LZ4_FORCE_INLINE int LZ4HC_compress_hashChain (
    LZ4HC_CCtx_internal* const ctx,
    const char* const source,
    char* const dest,
    int* srcSizePtr,
    int const maxOutputSize,
    int maxNbAttempts,
    const limitedOutput_directive limit,
    const dictCtx_directive dict
    )
{
    const int inputSize = *srcSizePtr;
    const int patternAnalysis = (maxNbAttempts > 128);   /* levels 9+ */

    const BYTE* ip = (const BYTE*) source;
    const BYTE* anchor = ip;
    const BYTE* const iend = ip + inputSize;
    const BYTE* const mflimit = iend - MFLIMIT;
    const BYTE* const matchlimit = (iend - LASTLITERALS);

    BYTE* optr = (BYTE*) dest;
    BYTE* op = (BYTE*) dest;
    BYTE* oend = op + maxOutputSize;

    int   ml0, ml, ml2, ml3;
    const BYTE* start0;
    const BYTE* ref0;
    const BYTE* ref = NULL;
    const BYTE* start2 = NULL;
    const BYTE* ref2 = NULL;
    const BYTE* start3 = NULL;
    const BYTE* ref3 = NULL;

    /* init */
    *srcSizePtr = 0;
    if (limit == fillOutput) oend -= LASTLITERALS;                  /* Hack for support LZ4 format restriction */
    if (inputSize < LZ4_minLength) goto _last_literals;             /* Input too small, no compression (all literals) */

    /* Main Loop */
    while (ip <= mflimit) {
        ml = LZ4HC_InsertAndFindBestMatch(ctx, ip, matchlimit, &ref, maxNbAttempts, patternAnalysis, dict);
        if (ml<MINMATCH) { ip++; continue; }

        /* saved, in case we would skip too much */
        start0 = ip; ref0 = ref; ml0 = ml;

_Search2:
        if (ip+ml <= mflimit) {
            ml2 = LZ4HC_InsertAndGetWiderMatch(ctx,
                            ip + ml - 2, ip + 0, matchlimit, ml, &ref2, &start2,
                            maxNbAttempts, patternAnalysis, 0, dict, favorCompressionRatio);
        } else {
            ml2 = ml;
        }

        if (ml2 == ml) { /* No better match => encode ML1 */
            optr = op;
            if (LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ref, limit, oend)) goto _dest_overflow;
            continue;
        }

        if (start0 < ip) {   /* first match was skipped at least once */
            if (start2 < ip + ml0) {  /* squeezing ML1 between ML0(original ML1) and ML2 */
                ip = start0; ref = ref0; ml = ml0;  /* restore initial ML1 */
        }   }

        /* Here, start0==ip */
        if ((start2 - ip) < 3) {  /* First Match too small : removed */
            ml = ml2;
            ip = start2;
            ref =ref2;
            goto _Search2;
        }

_Search3:
        /* At this stage, we have :
        *  ml2 > ml1, and
        *  ip1+3 <= ip2 (usually < ip1+ml1) */
        if ((start2 - ip) < OPTIMAL_ML) {
            int correction;
            int new_ml = ml;
            if (new_ml > OPTIMAL_ML) new_ml = OPTIMAL_ML;
            if (ip+new_ml > start2 + ml2 - MINMATCH) new_ml = (int)(start2 - ip) + ml2 - MINMATCH;
            correction = new_ml - (int)(start2 - ip);
            if (correction > 0) {
                start2 += correction;
                ref2 += correction;
                ml2 -= correction;
            }
        }
        /* Now, we have start2 = ip+new_ml, with new_ml = min(ml, OPTIMAL_ML=18) */

        if (start2 + ml2 <= mflimit) {
            ml3 = LZ4HC_InsertAndGetWiderMatch(ctx,
                            start2 + ml2 - 3, start2, matchlimit, ml2, &ref3, &start3,
                            maxNbAttempts, patternAnalysis, 0, dict, favorCompressionRatio);
        } else {
            ml3 = ml2;
        }

        if (ml3 == ml2) {  /* No better match => encode ML1 and ML2 */
            /* ip & ref are known; Now for ml */
            if (start2 < ip+ml)  ml = (int)(start2 - ip);
            /* Now, encode 2 sequences */
            optr = op;
            if (LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ref, limit, oend)) goto _dest_overflow;
            ip = start2;
            optr = op;
            if (LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml2, ref2, limit, oend)) {
                ml  = ml2;
                ref = ref2;
                goto _dest_overflow;
            }
            continue;
        }

        if (start3 < ip+ml+3) {  /* Not enough space for match 2 : remove it */
            if (start3 >= (ip+ml)) {  /* can write Seq1 immediately ==> Seq2 is removed, so Seq3 becomes Seq1 */
                if (start2 < ip+ml) {
                    int correction = (int)(ip+ml - start2);
                    start2 += correction;
                    ref2 += correction;
                    ml2 -= correction;
                    if (ml2 < MINMATCH) {
                        start2 = start3;
                        ref2 = ref3;
                        ml2 = ml3;
                    }
                }

                optr = op;
                if (LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ref, limit, oend)) goto _dest_overflow;
                ip  = start3;
                ref = ref3;
                ml  = ml3;

                start0 = start2;
                ref0 = ref2;
                ml0 = ml2;
                goto _Search2;
            }

            start2 = start3;
            ref2 = ref3;
            ml2 = ml3;
            goto _Search3;
        }

        /*
        * OK, now we have 3 ascending matches;
        * let's write the first one ML1.
        * ip & ref are known; Now decide ml.
        */
        if (start2 < ip+ml) {
            if ((start2 - ip) < OPTIMAL_ML) {
                int correction;
                if (ml > OPTIMAL_ML) ml = OPTIMAL_ML;
                if (ip + ml > start2 + ml2 - MINMATCH) ml = (int)(start2 - ip) + ml2 - MINMATCH;
                correction = ml - (int)(start2 - ip);
                if (correction > 0) {
                    start2 += correction;
                    ref2 += correction;
                    ml2 -= correction;
                }
            } else {
                ml = (int)(start2 - ip);
            }
        }
        optr = op;
        if (LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ref, limit, oend)) goto _dest_overflow;

        /* ML2 becomes ML1 */
        ip = start2; ref = ref2; ml = ml2;

        /* ML3 becomes ML2 */
        start2 = start3; ref2 = ref3; ml2 = ml3;

        /* let's find a new ML3 */
        goto _Search3;
    }

_last_literals:
    /* Encode Last Literals */
    {   size_t lastRunSize = (size_t)(iend - anchor);  /* literals */
        size_t llAdd = (lastRunSize + 255 - RUN_MASK) / 255;
        size_t const totalSize = 1 + llAdd + lastRunSize;
        if (limit == fillOutput) oend += LASTLITERALS;  /* restore correct value */
        if (limit && (op + totalSize > oend)) {
            if (limit == limitedOutput) return 0;
            /* adapt lastRunSize to fill 'dest' */
            lastRunSize  = (size_t)(oend - op) - 1 /*token*/;
            llAdd = (lastRunSize + 256 - RUN_MASK) / 256;
            lastRunSize -= llAdd;
        }
        DEBUGLOG(6, "Final literal run : %i literals", (int)lastRunSize);
        ip = anchor + lastRunSize;  /* can be != iend if limit==fillOutput */

        if (lastRunSize >= RUN_MASK) {
            size_t accumulator = lastRunSize - RUN_MASK;
            *op++ = (RUN_MASK << ML_BITS);
            for(; accumulator >= 255 ; accumulator -= 255) *op++ = 255;
            *op++ = (BYTE) accumulator;
        } else {
            *op++ = (BYTE)(lastRunSize << ML_BITS);
        }
        memcpy(op, anchor, lastRunSize);
        op += lastRunSize;
    }

    /* End */
    *srcSizePtr = (int) (((const char*)ip) - source);
    return (int) (((char*)op)-dest);

_dest_overflow:
    if (limit == fillOutput) {
        /* Assumption : ip, anchor, ml and ref must be set correctly */
        size_t const ll = (size_t)(ip - anchor);
        size_t const ll_addbytes = (ll + 240) / 255;
        size_t const ll_totalCost = 1 + ll_addbytes + ll;
        BYTE* const maxLitPos = oend - 3; /* 2 for offset, 1 for token */
        DEBUGLOG(6, "Last sequence overflowing");
        op = optr;  /* restore correct out pointer */
        if (op + ll_totalCost <= maxLitPos) {
            /* ll validated; now adjust match length */
            size_t const bytesLeftForMl = (size_t)(maxLitPos - (op+ll_totalCost));
            size_t const maxMlSize = MINMATCH + (ML_MASK-1) + (bytesLeftForMl * 255);
            assert(maxMlSize < INT_MAX); assert(ml >= 0);
            if ((size_t)ml > maxMlSize) ml = (int)maxMlSize;
            if ((oend + LASTLITERALS) - (op + ll_totalCost + 2) - 1 + ml >= MFLIMIT) {
                LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ref, notLimited, oend);
        }   }
        goto _last_literals;
    }
    /* compression failed */
    return 0;
}


static int LZ4HC_compress_optimal( LZ4HC_CCtx_internal* ctx,
    const char* const source, char* dst,
    int* srcSizePtr, int dstCapacity,
    int const nbSearches, size_t sufficient_len,
    const limitedOutput_directive limit, int const fullUpdate,
    const dictCtx_directive dict,
    const HCfavor_e favorDecSpeed);


LZ4_FORCE_INLINE int LZ4HC_compress_generic_internal (
    LZ4HC_CCtx_internal* const ctx,
    const char* const src,
    char* const dst,
    int* const srcSizePtr,
    int const dstCapacity,
    int cLevel,
    const limitedOutput_directive limit,
    const dictCtx_directive dict
    )
{
    typedef enum { lz4hc, lz4opt } lz4hc_strat_e;
    typedef struct {
        lz4hc_strat_e strat;
        int nbSearches;
        U32 targetLength;
    } cParams_t;
    static const cParams_t clTable[LZ4HC_CLEVEL_MAX+1] = {
        { lz4hc,     2, 16 },  /* 0, unused */
        { lz4hc,     2, 16 },  /* 1, unused */
        { lz4hc,     2, 16 },  /* 2, unused */
        { lz4hc,     4, 16 },  /* 3 */
        { lz4hc,     8, 16 },  /* 4 */
        { lz4hc,    16, 16 },  /* 5 */
        { lz4hc,    32, 16 },  /* 6 */
        { lz4hc,    64, 16 },  /* 7 */
        { lz4hc,   128, 16 },  /* 8 */
        { lz4hc,   256, 16 },  /* 9 */
        { lz4opt,   96, 64 },  /*10==LZ4HC_CLEVEL_OPT_MIN*/
        { lz4opt,  512,128 },  /*11 */
        { lz4opt,16384,LZ4_OPT_NUM },  /* 12==LZ4HC_CLEVEL_MAX */
    };

    DEBUGLOG(4, "LZ4HC_compress_generic(ctx=%p, src=%p, srcSize=%d, limit=%d)",
                ctx, src, *srcSizePtr, limit);

    if (limit == fillOutput && dstCapacity < 1) return 0;   /* Impossible to store anything */
    if ((U32)*srcSizePtr > (U32)LZ4_MAX_INPUT_SIZE) return 0;    /* Unsupported input size (too large or negative) */

    ctx->end += *srcSizePtr;
    if (cLevel < 1) cLevel = LZ4HC_CLEVEL_DEFAULT;   /* note : convention is different from lz4frame, maybe something to review */
    cLevel = MIN(LZ4HC_CLEVEL_MAX, cLevel);
    {   cParams_t const cParam = clTable[cLevel];
        HCfavor_e const favor = ctx->favorDecSpeed ? favorDecompressionSpeed : favorCompressionRatio;
        int result;

        if (cParam.strat == lz4hc) {
            result = LZ4HC_compress_hashChain(ctx,
                                src, dst, srcSizePtr, dstCapacity,
                                cParam.nbSearches, limit, dict);
        } else {
            assert(cParam.strat == lz4opt);
            result = LZ4HC_compress_optimal(ctx,
                                src, dst, srcSizePtr, dstCapacity,
                                cParam.nbSearches, cParam.targetLength, limit,
                                cLevel == LZ4HC_CLEVEL_MAX,   /* ultra mode */
                                dict, favor);
        }
        if (result <= 0) ctx->dirty = 1;
        return result;
    }
}

static void LZ4HC_setExternalDict(LZ4HC_CCtx_internal* ctxPtr, const BYTE* newBlock);

static int
LZ4HC_compress_generic_noDictCtx (
        LZ4HC_CCtx_internal* const ctx,
        const char* const src,
        char* const dst,
        int* const srcSizePtr,
        int const dstCapacity,
        int cLevel,
        limitedOutput_directive limit
        )
{
    assert(ctx->dictCtx == NULL);
    return LZ4HC_compress_generic_internal(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit, noDictCtx);
}

static int
LZ4HC_compress_generic_dictCtx (
        LZ4HC_CCtx_internal* const ctx,
        const char* const src,
        char* const dst,
        int* const srcSizePtr,
        int const dstCapacity,
        int cLevel,
        limitedOutput_directive limit
        )
{
    const size_t position = (size_t)(ctx->end - ctx->base) - ctx->lowLimit;
    assert(ctx->dictCtx != NULL);
    if (position >= 64 KB) {
        ctx->dictCtx = NULL;
        return LZ4HC_compress_generic_noDictCtx(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit);
    } else if (position == 0 && *srcSizePtr > 4 KB) {
        memcpy(ctx, ctx->dictCtx, sizeof(LZ4HC_CCtx_internal));
        LZ4HC_setExternalDict(ctx, (const BYTE *)src);
        ctx->compressionLevel = (short)cLevel;
        return LZ4HC_compress_generic_noDictCtx(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit);
    } else {
        return LZ4HC_compress_generic_internal(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit, usingDictCtxHc);
    }
}

static int
LZ4HC_compress_generic (
        LZ4HC_CCtx_internal* const ctx,
        const char* const src,
        char* const dst,
        int* const srcSizePtr,
        int const dstCapacity,
        int cLevel,
        limitedOutput_directive limit
        )
{
    if (ctx->dictCtx == NULL) {
        return LZ4HC_compress_generic_noDictCtx(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit);
    } else {
        return LZ4HC_compress_generic_dictCtx(ctx, src, dst, srcSizePtr, dstCapacity, cLevel, limit);
    }
}


int LZ4_sizeofStateHC(void) { return (int)sizeof(LZ4_streamHC_t); }

static size_t LZ4_streamHC_t_alignment(void)
{
#if LZ4_ALIGN_TEST
    typedef struct { char c; LZ4_streamHC_t t; } t_a;
    return sizeof(t_a) - sizeof(LZ4_streamHC_t);
#else
    return 1;  /* effectively disabled */
#endif
}

/* state is presumed correctly initialized,
 * in which case its size and alignment have already been validate */
int LZ4_compress_HC_extStateHC_fastReset (void* state, const char* src, char* dst, int srcSize, int dstCapacity, int compressionLevel)
{
    LZ4HC_CCtx_internal* const ctx = &((LZ4_streamHC_t*)state)->internal_donotuse;
    if (!LZ4_isAligned(state, LZ4_streamHC_t_alignment())) return 0;
    LZ4_resetStreamHC_fast((LZ4_streamHC_t*)state, compressionLevel);
    LZ4HC_init_internal (ctx, (const BYTE*)src);
    if (dstCapacity < LZ4_compressBound(srcSize))
        return LZ4HC_compress_generic (ctx, src, dst, &srcSize, dstCapacity, compressionLevel, limitedOutput);
    else
        return LZ4HC_compress_generic (ctx, src, dst, &srcSize, dstCapacity, compressionLevel, notLimited);
}

int LZ4_compress_HC_extStateHC (void* state, const char* src, char* dst, int srcSize, int dstCapacity, int compressionLevel)
{
    LZ4_streamHC_t* const ctx = LZ4_initStreamHC(state, sizeof(*ctx));
    if (ctx==NULL) return 0;   /* init failure */
    return LZ4_compress_HC_extStateHC_fastReset(state, src, dst, srcSize, dstCapacity, compressionLevel);
}

int LZ4_compress_HC(const char* src, char* dst, int srcSize, int dstCapacity, int compressionLevel)
{
#if defined(LZ4HC_HEAPMODE) && LZ4HC_HEAPMODE==1
    LZ4_streamHC_t* const statePtr = (LZ4_streamHC_t*)ALLOC(sizeof(LZ4_streamHC_t));
#else
    LZ4_streamHC_t state;
    LZ4_streamHC_t* const statePtr = &state;
#endif
    int const cSize = LZ4_compress_HC_extStateHC(statePtr, src, dst, srcSize, dstCapacity, compressionLevel);
#if defined(LZ4HC_HEAPMODE) && LZ4HC_HEAPMODE==1
    FREEMEM(statePtr);
#endif
    return cSize;
}

/* state is presumed sized correctly (>= sizeof(LZ4_streamHC_t)) */
int LZ4_compress_HC_destSize(void* state, const char* source, char* dest, int* sourceSizePtr, int targetDestSize, int cLevel)
{
    LZ4_streamHC_t* const ctx = LZ4_initStreamHC(state, sizeof(*ctx));
    if (ctx==NULL) return 0;   /* init failure */
    LZ4HC_init_internal(&ctx->internal_donotuse, (const BYTE*) source);
    LZ4_setCompressionLevel(ctx, cLevel);
    return LZ4HC_compress_generic(&ctx->internal_donotuse, source, dest, sourceSizePtr, targetDestSize, cLevel, fillOutput);
}



/**************************************
*  Streaming Functions
**************************************/
/* allocation */
LZ4_streamHC_t* LZ4_createStreamHC(void)
{
    LZ4_streamHC_t* const state =
        (LZ4_streamHC_t*)ALLOC_AND_ZERO(sizeof(LZ4_streamHC_t));
    if (state == NULL) return NULL;
    LZ4_setCompressionLevel(state, LZ4HC_CLEVEL_DEFAULT);
    return state;
}

int LZ4_freeStreamHC (LZ4_streamHC_t* LZ4_streamHCPtr)
{
    DEBUGLOG(4, "LZ4_freeStreamHC(%p)", LZ4_streamHCPtr);
    if (!LZ4_streamHCPtr) return 0;  /* support free on NULL */
    FREEMEM(LZ4_streamHCPtr);
    return 0;
}


LZ4_streamHC_t* LZ4_initStreamHC (void* buffer, size_t size)
{
    LZ4_streamHC_t* const LZ4_streamHCPtr = (LZ4_streamHC_t*)buffer;
    /* if compilation fails here, LZ4_STREAMHCSIZE must be increased */
    LZ4_STATIC_ASSERT(sizeof(LZ4HC_CCtx_internal) <= LZ4_STREAMHCSIZE);
    DEBUGLOG(4, "LZ4_initStreamHC(%p, %u)", buffer, (unsigned)size);
    /* check conditions */
    if (buffer == NULL) return NULL;
    if (size < sizeof(LZ4_streamHC_t)) return NULL;
    if (!LZ4_isAligned(buffer, LZ4_streamHC_t_alignment())) return NULL;
    /* init */
    { LZ4HC_CCtx_internal* const hcstate = &(LZ4_streamHCPtr->internal_donotuse);
      MEM_INIT(hcstate, 0, sizeof(*hcstate)); }
    LZ4_setCompressionLevel(LZ4_streamHCPtr, LZ4HC_CLEVEL_DEFAULT);
    return LZ4_streamHCPtr;
}

/* just a stub */
void LZ4_resetStreamHC (LZ4_streamHC_t* LZ4_streamHCPtr, int compressionLevel)
{
    LZ4_initStreamHC(LZ4_streamHCPtr, sizeof(*LZ4_streamHCPtr));
    LZ4_setCompressionLevel(LZ4_streamHCPtr, compressionLevel);
}

void LZ4_resetStreamHC_fast (LZ4_streamHC_t* LZ4_streamHCPtr, int compressionLevel)
{
    DEBUGLOG(4, "LZ4_resetStreamHC_fast(%p, %d)", LZ4_streamHCPtr, compressionLevel);
    if (LZ4_streamHCPtr->internal_donotuse.dirty) {
        LZ4_initStreamHC(LZ4_streamHCPtr, sizeof(*LZ4_streamHCPtr));
    } else {
        /* preserve end - base : can trigger clearTable's threshold */
        LZ4_streamHCPtr->internal_donotuse.end -= (uptrval)LZ4_streamHCPtr->internal_donotuse.base;
        LZ4_streamHCPtr->internal_donotuse.base = NULL;
        LZ4_streamHCPtr->internal_donotuse.dictCtx = NULL;
    }
    LZ4_setCompressionLevel(LZ4_streamHCPtr, compressionLevel);
}

void LZ4_setCompressionLevel(LZ4_streamHC_t* LZ4_streamHCPtr, int compressionLevel)
{
    DEBUGLOG(5, "LZ4_setCompressionLevel(%p, %d)", LZ4_streamHCPtr, compressionLevel);
    if (compressionLevel < 1) compressionLevel = LZ4HC_CLEVEL_DEFAULT;
    if (compressionLevel > LZ4HC_CLEVEL_MAX) compressionLevel = LZ4HC_CLEVEL_MAX;
    LZ4_streamHCPtr->internal_donotuse.compressionLevel = (short)compressionLevel;
}

void LZ4_favorDecompressionSpeed(LZ4_streamHC_t* LZ4_streamHCPtr, int favor)
{
    LZ4_streamHCPtr->internal_donotuse.favorDecSpeed = (favor!=0);
}

/* LZ4_loadDictHC() :
 * LZ4_streamHCPtr is presumed properly initialized */
int LZ4_loadDictHC (LZ4_streamHC_t* LZ4_streamHCPtr,
              const char* dictionary, int dictSize)
{
    LZ4HC_CCtx_internal* const ctxPtr = &LZ4_streamHCPtr->internal_donotuse;
    DEBUGLOG(4, "LZ4_loadDictHC(ctx:%p, dict:%p, dictSize:%d)", LZ4_streamHCPtr, dictionary, dictSize);
    assert(LZ4_streamHCPtr != NULL);
    if (dictSize > 64 KB) {
        dictionary += (size_t)dictSize - 64 KB;
        dictSize = 64 KB;
    }
    /* need a full initialization, there are bad side-effects when using resetFast() */
    {   int const cLevel = ctxPtr->compressionLevel;
        LZ4_initStreamHC(LZ4_streamHCPtr, sizeof(*LZ4_streamHCPtr));
        LZ4_setCompressionLevel(LZ4_streamHCPtr, cLevel);
    }
    LZ4HC_init_internal (ctxPtr, (const BYTE*)dictionary);
    ctxPtr->end = (const BYTE*)dictionary + dictSize;
    if (dictSize >= 4) LZ4HC_Insert (ctxPtr, ctxPtr->end-3);
    return dictSize;
}

void LZ4_attach_HC_dictionary(LZ4_streamHC_t *working_stream, const LZ4_streamHC_t *dictionary_stream) {
    working_stream->internal_donotuse.dictCtx = dictionary_stream != NULL ? &(dictionary_stream->internal_donotuse) : NULL;
}

/* compression */

static void LZ4HC_setExternalDict(LZ4HC_CCtx_internal* ctxPtr, const BYTE* newBlock)
{
    DEBUGLOG(4, "LZ4HC_setExternalDict(%p, %p)", ctxPtr, newBlock);
    if (ctxPtr->end >= ctxPtr->base + ctxPtr->dictLimit + 4)
        LZ4HC_Insert (ctxPtr, ctxPtr->end-3);   /* Referencing remaining dictionary content */

    /* Only one memory segment for extDict, so any previous extDict is lost at this stage */
    ctxPtr->lowLimit  = ctxPtr->dictLimit;
    ctxPtr->dictLimit = (U32)(ctxPtr->end - ctxPtr->base);
    ctxPtr->dictBase  = ctxPtr->base;
    ctxPtr->base = newBlock - ctxPtr->dictLimit;
    ctxPtr->end  = newBlock;
    ctxPtr->nextToUpdate = ctxPtr->dictLimit;   /* match referencing will resume from there */

    /* cannot reference an extDict and a dictCtx at the same time */
    ctxPtr->dictCtx = NULL;
}

static int
LZ4_compressHC_continue_generic (LZ4_streamHC_t* LZ4_streamHCPtr,
                                 const char* src, char* dst,
                                 int* srcSizePtr, int dstCapacity,
                                 limitedOutput_directive limit)
{
    LZ4HC_CCtx_internal* const ctxPtr = &LZ4_streamHCPtr->internal_donotuse;
    DEBUGLOG(5, "LZ4_compressHC_continue_generic(ctx=%p, src=%p, srcSize=%d, limit=%d)",
                LZ4_streamHCPtr, src, *srcSizePtr, limit);
    assert(ctxPtr != NULL);
    /* auto-init if forgotten */
    if (ctxPtr->base == NULL) LZ4HC_init_internal (ctxPtr, (const BYTE*) src);

    /* Check overflow */
    if ((size_t)(ctxPtr->end - ctxPtr->base) > 2 GB) {
        size_t dictSize = (size_t)(ctxPtr->end - ctxPtr->base) - ctxPtr->dictLimit;
        if (dictSize > 64 KB) dictSize = 64 KB;
        LZ4_loadDictHC(LZ4_streamHCPtr, (const char*)(ctxPtr->end) - dictSize, (int)dictSize);
    }

    /* Check if blocks follow each other */
    if ((const BYTE*)src != ctxPtr->end)
        LZ4HC_setExternalDict(ctxPtr, (const BYTE*)src);

    /* Check overlapping input/dictionary space */
    {   const BYTE* sourceEnd = (const BYTE*) src + *srcSizePtr;
        const BYTE* const dictBegin = ctxPtr->dictBase + ctxPtr->lowLimit;
        const BYTE* const dictEnd   = ctxPtr->dictBase + ctxPtr->dictLimit;
        if ((sourceEnd > dictBegin) && ((const BYTE*)src < dictEnd)) {
            if (sourceEnd > dictEnd) sourceEnd = dictEnd;
            ctxPtr->lowLimit = (U32)(sourceEnd - ctxPtr->dictBase);
            if (ctxPtr->dictLimit - ctxPtr->lowLimit < 4) ctxPtr->lowLimit = ctxPtr->dictLimit;
    }   }

    return LZ4HC_compress_generic (ctxPtr, src, dst, srcSizePtr, dstCapacity, ctxPtr->compressionLevel, limit);
}

int LZ4_compress_HC_continue (LZ4_streamHC_t* LZ4_streamHCPtr, const char* src, char* dst, int srcSize, int dstCapacity)
{
    if (dstCapacity < LZ4_compressBound(srcSize))
        return LZ4_compressHC_continue_generic (LZ4_streamHCPtr, src, dst, &srcSize, dstCapacity, limitedOutput);
    else
        return LZ4_compressHC_continue_generic (LZ4_streamHCPtr, src, dst, &srcSize, dstCapacity, notLimited);
}

int LZ4_compress_HC_continue_destSize (LZ4_streamHC_t* LZ4_streamHCPtr, const char* src, char* dst, int* srcSizePtr, int targetDestSize)
{
    return LZ4_compressHC_continue_generic(LZ4_streamHCPtr, src, dst, srcSizePtr, targetDestSize, fillOutput);
}



/* LZ4_saveDictHC :
 * save history content
 * into a user-provided buffer
 * which is then used to continue compression
 */
int LZ4_saveDictHC (LZ4_streamHC_t* LZ4_streamHCPtr, char* safeBuffer, int dictSize)
{
    LZ4HC_CCtx_internal* const streamPtr = &LZ4_streamHCPtr->internal_donotuse;
    int const prefixSize = (int)(streamPtr->end - (streamPtr->base + streamPtr->dictLimit));
    DEBUGLOG(5, "LZ4_saveDictHC(%p, %p, %d)", LZ4_streamHCPtr, safeBuffer, dictSize);
    assert(prefixSize >= 0);
    if (dictSize > 64 KB) dictSize = 64 KB;
    if (dictSize < 4) dictSize = 0;
    if (dictSize > prefixSize) dictSize = prefixSize;
    if (safeBuffer == NULL) assert(dictSize == 0);
    if (dictSize > 0)
        memmove(safeBuffer, streamPtr->end - dictSize, dictSize);
    {   U32 const endIndex = (U32)(streamPtr->end - streamPtr->base);
        streamPtr->end = (const BYTE*)safeBuffer + dictSize;
        streamPtr->base = streamPtr->end - endIndex;
        streamPtr->dictLimit = endIndex - (U32)dictSize;
        streamPtr->lowLimit = endIndex - (U32)dictSize;
        if (streamPtr->nextToUpdate < streamPtr->dictLimit)
            streamPtr->nextToUpdate = streamPtr->dictLimit;
    }
    return dictSize;
}


/***************************************************
*  Deprecated Functions
***************************************************/

/* These functions currently generate deprecation warnings */

/* Wrappers for deprecated compression functions */
int LZ4_compressHC(const char* src, char* dst, int srcSize) { return LZ4_compress_HC (src, dst, srcSize, LZ4_compressBound(srcSize), 0); }
int LZ4_compressHC_limitedOutput(const char* src, char* dst, int srcSize, int maxDstSize) { return LZ4_compress_HC(src, dst, srcSize, maxDstSize, 0); }
int LZ4_compressHC2(const char* src, char* dst, int srcSize, int cLevel) { return LZ4_compress_HC (src, dst, srcSize, LZ4_compressBound(srcSize), cLevel); }
int LZ4_compressHC2_limitedOutput(const char* src, char* dst, int srcSize, int maxDstSize, int cLevel) { return LZ4_compress_HC(src, dst, srcSize, maxDstSize, cLevel); }
int LZ4_compressHC_withStateHC (void* state, const char* src, char* dst, int srcSize) { return LZ4_compress_HC_extStateHC (state, src, dst, srcSize, LZ4_compressBound(srcSize), 0); }
int LZ4_compressHC_limitedOutput_withStateHC (void* state, const char* src, char* dst, int srcSize, int maxDstSize) { return LZ4_compress_HC_extStateHC (state, src, dst, srcSize, maxDstSize, 0); }
int LZ4_compressHC2_withStateHC (void* state, const char* src, char* dst, int srcSize, int cLevel) { return LZ4_compress_HC_extStateHC(state, src, dst, srcSize, LZ4_compressBound(srcSize), cLevel); }
int LZ4_compressHC2_limitedOutput_withStateHC (void* state, const char* src, char* dst, int srcSize, int maxDstSize, int cLevel) { return LZ4_compress_HC_extStateHC(state, src, dst, srcSize, maxDstSize, cLevel); }
int LZ4_compressHC_continue (LZ4_streamHC_t* ctx, const char* src, char* dst, int srcSize) { return LZ4_compress_HC_continue (ctx, src, dst, srcSize, LZ4_compressBound(srcSize)); }
int LZ4_compressHC_limitedOutput_continue (LZ4_streamHC_t* ctx, const char* src, char* dst, int srcSize, int maxDstSize) { return LZ4_compress_HC_continue (ctx, src, dst, srcSize, maxDstSize); }


/* Deprecated streaming functions */
int LZ4_sizeofStreamStateHC(void) { return LZ4_STREAMHCSIZE; }

/* state is presumed correctly sized, aka >= sizeof(LZ4_streamHC_t)
 * @return : 0 on success, !=0 if error */
int LZ4_resetStreamStateHC(void* state, char* inputBuffer)
{
    LZ4_streamHC_t* const hc4 = LZ4_initStreamHC(state, sizeof(*hc4));
    if (hc4 == NULL) return 1;   /* init failed */
    LZ4HC_init_internal (&hc4->internal_donotuse, (const BYTE*)inputBuffer);
    return 0;
}

void* LZ4_createHC (const char* inputBuffer)
{
    LZ4_streamHC_t* const hc4 = LZ4_createStreamHC();
    if (hc4 == NULL) return NULL;   /* not enough memory */
    LZ4HC_init_internal (&hc4->internal_donotuse, (const BYTE*)inputBuffer);
    return hc4;
}

int LZ4_freeHC (void* LZ4HC_Data)
{
    if (!LZ4HC_Data) return 0;  /* support free on NULL */
    FREEMEM(LZ4HC_Data);
    return 0;
}

int LZ4_compressHC2_continue (void* LZ4HC_Data, const char* src, char* dst, int srcSize, int cLevel)
{
    return LZ4HC_compress_generic (&((LZ4_streamHC_t*)LZ4HC_Data)->internal_donotuse, src, dst, &srcSize, 0, cLevel, notLimited);
}

int LZ4_compressHC2_limitedOutput_continue (void* LZ4HC_Data, const char* src, char* dst, int srcSize, int dstCapacity, int cLevel)
{
    return LZ4HC_compress_generic (&((LZ4_streamHC_t*)LZ4HC_Data)->internal_donotuse, src, dst, &srcSize, dstCapacity, cLevel, limitedOutput);
}

char* LZ4_slideInputBufferHC(void* LZ4HC_Data)
{
    LZ4_streamHC_t *ctx = (LZ4_streamHC_t*)LZ4HC_Data;
    const BYTE *bufferStart = ctx->internal_donotuse.base + ctx->internal_donotuse.lowLimit;
    LZ4_resetStreamHC_fast(ctx, ctx->internal_donotuse.compressionLevel);
    /* avoid const char * -> char * conversion warning :( */
    return (char *)(uptrval)bufferStart;
}


/* ================================================
 *  LZ4 Optimal parser (levels [LZ4HC_CLEVEL_OPT_MIN - LZ4HC_CLEVEL_MAX])
 * ===============================================*/
typedef struct {
    int price;
    int off;
    int mlen;
    int litlen;
} LZ4HC_optimal_t;

/* price in bytes */
LZ4_FORCE_INLINE int LZ4HC_literalsPrice(int const litlen)
{
    int price = litlen;
    assert(litlen >= 0);
    if (litlen >= (int)RUN_MASK)
        price += 1 + ((litlen-(int)RUN_MASK) / 255);
    return price;
}


/* requires mlen >= MINMATCH */
LZ4_FORCE_INLINE int LZ4HC_sequencePrice(int litlen, int mlen)
{
    int price = 1 + 2 ; /* token + 16-bit offset */
    assert(litlen >= 0);
    assert(mlen >= MINMATCH);

    price += LZ4HC_literalsPrice(litlen);

    if (mlen >= (int)(ML_MASK+MINMATCH))
        price += 1 + ((mlen-(int)(ML_MASK+MINMATCH)) / 255);

    return price;
}


typedef struct {
    int off;
    int len;
} LZ4HC_match_t;

LZ4_FORCE_INLINE LZ4HC_match_t
LZ4HC_FindLongerMatch(LZ4HC_CCtx_internal* const ctx,
                      const BYTE* ip, const BYTE* const iHighLimit,
                      int minLen, int nbSearches,
                      const dictCtx_directive dict,
                      const HCfavor_e favorDecSpeed)
{
    LZ4HC_match_t match = { 0 , 0 };
    const BYTE* matchPtr = NULL;
    /* note : LZ4HC_InsertAndGetWiderMatch() is able to modify the starting position of a match (*startpos),
     * but this won't be the case here, as we define iLowLimit==ip,
     * so LZ4HC_InsertAndGetWiderMatch() won't be allowed to search past ip */
    int matchLength = LZ4HC_InsertAndGetWiderMatch(ctx, ip, ip, iHighLimit, minLen, &matchPtr, &ip, nbSearches, 1 /*patternAnalysis*/, 1 /*chainSwap*/, dict, favorDecSpeed);
    if (matchLength <= minLen) return match;
    if (favorDecSpeed) {
        if ((matchLength>18) & (matchLength<=36)) matchLength=18;   /* favor shortcut */
    }
    match.len = matchLength;
    match.off = (int)(ip-matchPtr);
    return match;
}


static int LZ4HC_compress_optimal ( LZ4HC_CCtx_internal* ctx,
                                    const char* const source,
                                    char* dst,
                                    int* srcSizePtr,
                                    int dstCapacity,
                                    int const nbSearches,
                                    size_t sufficient_len,
                                    const limitedOutput_directive limit,
                                    int const fullUpdate,
                                    const dictCtx_directive dict,
                                    const HCfavor_e favorDecSpeed)
{
    int retval = 0;
#define TRAILING_LITERALS 3
#ifdef LZ4HC_HEAPMODE
    LZ4HC_optimal_t* const opt = (LZ4HC_optimal_t*)ALLOC(sizeof(LZ4HC_optimal_t) * (LZ4_OPT_NUM + TRAILING_LITERALS));
#else
    LZ4HC_optimal_t opt[LZ4_OPT_NUM + TRAILING_LITERALS];   /* ~64 KB, which is a bit large for stack... */
#endif

    const BYTE* ip = (const BYTE*) source;
    const BYTE* anchor = ip;
    const BYTE* const iend = ip + *srcSizePtr;
    const BYTE* const mflimit = iend - MFLIMIT;
    const BYTE* const matchlimit = iend - LASTLITERALS;
    BYTE* op = (BYTE*) dst;
    BYTE* opSaved = (BYTE*) dst;
    BYTE* oend = op + dstCapacity;
    int ovml = MINMATCH;  /* overflow - last sequence */
    const BYTE* ovref = NULL;

    /* init */
#ifdef LZ4HC_HEAPMODE
    if (opt == NULL) goto _return_label;
#endif
    DEBUGLOG(5, "LZ4HC_compress_optimal(dst=%p, dstCapa=%u)", dst, (unsigned)dstCapacity);
    *srcSizePtr = 0;
    if (limit == fillOutput) oend -= LASTLITERALS;   /* Hack for support LZ4 format restriction */
    if (sufficient_len >= LZ4_OPT_NUM) sufficient_len = LZ4_OPT_NUM-1;

    /* Main Loop */
    while (ip <= mflimit) {
         int const llen = (int)(ip - anchor);
         int best_mlen, best_off;
         int cur, last_match_pos = 0;

         LZ4HC_match_t const firstMatch = LZ4HC_FindLongerMatch(ctx, ip, matchlimit, MINMATCH-1, nbSearches, dict, favorDecSpeed);
         if (firstMatch.len==0) { ip++; continue; }

         if ((size_t)firstMatch.len > sufficient_len) {
             /* good enough solution : immediate encoding */
             int const firstML = firstMatch.len;
             const BYTE* const matchPos = ip - firstMatch.off;
             opSaved = op;
             if ( LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), firstML, matchPos, limit, oend) ) {  /* updates ip, op and anchor */
                 ovml = firstML;
                 ovref = matchPos;
                 goto _dest_overflow;
             }
             continue;
         }

         /* set prices for first positions (literals) */
         {   int rPos;
             for (rPos = 0 ; rPos < MINMATCH ; rPos++) {
                 int const cost = LZ4HC_literalsPrice(llen + rPos);
                 opt[rPos].mlen = 1;
                 opt[rPos].off = 0;
                 opt[rPos].litlen = llen + rPos;
                 opt[rPos].price = cost;
                 DEBUGLOG(7, "rPos:%3i => price:%3i (litlen=%i) -- initial setup",
                             rPos, cost, opt[rPos].litlen);
         }   }
         /* set prices using initial match */
         {   int mlen = MINMATCH;
             int const matchML = firstMatch.len;   /* necessarily < sufficient_len < LZ4_OPT_NUM */
             int const offset = firstMatch.off;
             assert(matchML < LZ4_OPT_NUM);
             for ( ; mlen <= matchML ; mlen++) {
                 int const cost = LZ4HC_sequencePrice(llen, mlen);
                 opt[mlen].mlen = mlen;
                 opt[mlen].off = offset;
                 opt[mlen].litlen = llen;
                 opt[mlen].price = cost;
                 DEBUGLOG(7, "rPos:%3i => price:%3i (matchlen=%i) -- initial setup",
                             mlen, cost, mlen);
         }   }
         last_match_pos = firstMatch.len;
         {   int addLit;
             for (addLit = 1; addLit <= TRAILING_LITERALS; addLit ++) {
                 opt[last_match_pos+addLit].mlen = 1; /* literal */
                 opt[last_match_pos+addLit].off = 0;
                 opt[last_match_pos+addLit].litlen = addLit;
                 opt[last_match_pos+addLit].price = opt[last_match_pos].price + LZ4HC_literalsPrice(addLit);
                 DEBUGLOG(7, "rPos:%3i => price:%3i (litlen=%i) -- initial setup",
                             last_match_pos+addLit, opt[last_match_pos+addLit].price, addLit);
         }   }

         /* check further positions */
         for (cur = 1; cur < last_match_pos; cur++) {
             const BYTE* const curPtr = ip + cur;
             LZ4HC_match_t newMatch;

             if (curPtr > mflimit) break;
             DEBUGLOG(7, "rPos:%u[%u] vs [%u]%u",
                     cur, opt[cur].price, opt[cur+1].price, cur+1);
             if (fullUpdate) {
                 /* not useful to search here if next position has same (or lower) cost */
                 if ( (opt[cur+1].price <= opt[cur].price)
                   /* in some cases, next position has same cost, but cost rises sharply after, so a small match would still be beneficial */
                   && (opt[cur+MINMATCH].price < opt[cur].price + 3/*min seq price*/) )
                     continue;
             } else {
                 /* not useful to search here if next position has same (or lower) cost */
                 if (opt[cur+1].price <= opt[cur].price) continue;
             }

             DEBUGLOG(7, "search at rPos:%u", cur);
             if (fullUpdate)
                 newMatch = LZ4HC_FindLongerMatch(ctx, curPtr, matchlimit, MINMATCH-1, nbSearches, dict, favorDecSpeed);
             else
                 /* only test matches of minimum length; slightly faster, but misses a few bytes */
                 newMatch = LZ4HC_FindLongerMatch(ctx, curPtr, matchlimit, last_match_pos - cur, nbSearches, dict, favorDecSpeed);
             if (!newMatch.len) continue;

             if ( ((size_t)newMatch.len > sufficient_len)
               || (newMatch.len + cur >= LZ4_OPT_NUM) ) {
                 /* immediate encoding */
                 best_mlen = newMatch.len;
                 best_off = newMatch.off;
                 last_match_pos = cur + 1;
                 goto encode;
             }

             /* before match : set price with literals at beginning */
             {   int const baseLitlen = opt[cur].litlen;
                 int litlen;
                 for (litlen = 1; litlen < MINMATCH; litlen++) {
                     int const price = opt[cur].price - LZ4HC_literalsPrice(baseLitlen) + LZ4HC_literalsPrice(baseLitlen+litlen);
                     int const pos = cur + litlen;
                     if (price < opt[pos].price) {
                         opt[pos].mlen = 1; /* literal */
                         opt[pos].off = 0;
                         opt[pos].litlen = baseLitlen+litlen;
                         opt[pos].price = price;
                         DEBUGLOG(7, "rPos:%3i => price:%3i (litlen=%i)",
                                     pos, price, opt[pos].litlen);
             }   }   }

             /* set prices using match at position = cur */
             {   int const matchML = newMatch.len;
                 int ml = MINMATCH;

                 assert(cur + newMatch.len < LZ4_OPT_NUM);
                 for ( ; ml <= matchML ; ml++) {
                     int const pos = cur + ml;
                     int const offset = newMatch.off;
                     int price;
                     int ll;
                     DEBUGLOG(7, "testing price rPos %i (last_match_pos=%i)",
                                 pos, last_match_pos);
                     if (opt[cur].mlen == 1) {
                         ll = opt[cur].litlen;
                         price = ((cur > ll) ? opt[cur - ll].price : 0)
                               + LZ4HC_sequencePrice(ll, ml);
                     } else {
                         ll = 0;
                         price = opt[cur].price + LZ4HC_sequencePrice(0, ml);
                     }

                    assert((U32)favorDecSpeed <= 1);
                     if (pos > last_match_pos+TRAILING_LITERALS
                      || price <= opt[pos].price - (int)favorDecSpeed) {
                         DEBUGLOG(7, "rPos:%3i => price:%3i (matchlen=%i)",
                                     pos, price, ml);
                         assert(pos < LZ4_OPT_NUM);
                         if ( (ml == matchML)  /* last pos of last match */
                           && (last_match_pos < pos) )
                             last_match_pos = pos;
                         opt[pos].mlen = ml;
                         opt[pos].off = offset;
                         opt[pos].litlen = ll;
                         opt[pos].price = price;
             }   }   }
             /* complete following positions with literals */
             {   int addLit;
                 for (addLit = 1; addLit <= TRAILING_LITERALS; addLit ++) {
                     opt[last_match_pos+addLit].mlen = 1; /* literal */
                     opt[last_match_pos+addLit].off = 0;
                     opt[last_match_pos+addLit].litlen = addLit;
                     opt[last_match_pos+addLit].price = opt[last_match_pos].price + LZ4HC_literalsPrice(addLit);
                     DEBUGLOG(7, "rPos:%3i => price:%3i (litlen=%i)", last_match_pos+addLit, opt[last_match_pos+addLit].price, addLit);
             }   }
         }  /* for (cur = 1; cur <= last_match_pos; cur++) */

         assert(last_match_pos < LZ4_OPT_NUM + TRAILING_LITERALS);
         best_mlen = opt[last_match_pos].mlen;
         best_off = opt[last_match_pos].off;
         cur = last_match_pos - best_mlen;

encode: /* cur, last_match_pos, best_mlen, best_off must be set */
         assert(cur < LZ4_OPT_NUM);
         assert(last_match_pos >= 1);  /* == 1 when only one candidate */
         DEBUGLOG(6, "reverse traversal, looking for shortest path (last_match_pos=%i)", last_match_pos);
         {   int candidate_pos = cur;
             int selected_matchLength = best_mlen;
             int selected_offset = best_off;
             while (1) {  /* from end to beginning */
                 int const next_matchLength = opt[candidate_pos].mlen;  /* can be 1, means literal */
                 int const next_offset = opt[candidate_pos].off;
                 DEBUGLOG(7, "pos %i: sequence length %i", candidate_pos, selected_matchLength);
                 opt[candidate_pos].mlen = selected_matchLength;
                 opt[candidate_pos].off = selected_offset;
                 selected_matchLength = next_matchLength;
                 selected_offset = next_offset;
                 if (next_matchLength > candidate_pos) break; /* last match elected, first match to encode */
                 assert(next_matchLength > 0);  /* can be 1, means literal */
                 candidate_pos -= next_matchLength;
         }   }

         /* encode all recorded sequences in order */
         {   int rPos = 0;  /* relative position (to ip) */
             while (rPos < last_match_pos) {
                 int const ml = opt[rPos].mlen;
                 int const offset = opt[rPos].off;
                 if (ml == 1) { ip++; rPos++; continue; }  /* literal; note: can end up with several literals, in which case, skip them */
                 rPos += ml;
                 assert(ml >= MINMATCH);
                 assert((offset >= 1) && (offset <= LZ4_DISTANCE_MAX));
                 opSaved = op;
                 if ( LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ml, ip - offset, limit, oend) ) {  /* updates ip, op and anchor */
                     ovml = ml;
                     ovref = ip - offset;
                     goto _dest_overflow;
         }   }   }
     }  /* while (ip <= mflimit) */

_last_literals:
     /* Encode Last Literals */
     {   size_t lastRunSize = (size_t)(iend - anchor);  /* literals */
         size_t llAdd = (lastRunSize + 255 - RUN_MASK) / 255;
         size_t const totalSize = 1 + llAdd + lastRunSize;
         if (limit == fillOutput) oend += LASTLITERALS;  /* restore correct value */
         if (limit && (op + totalSize > oend)) {
             if (limit == limitedOutput) { /* Check output limit */
                retval = 0;
                goto _return_label;
             }
             /* adapt lastRunSize to fill 'dst' */
             lastRunSize  = (size_t)(oend - op) - 1 /*token*/;
             llAdd = (lastRunSize + 256 - RUN_MASK) / 256;
             lastRunSize -= llAdd;
         }
         DEBUGLOG(6, "Final literal run : %i literals", (int)lastRunSize);
         ip = anchor + lastRunSize; /* can be != iend if limit==fillOutput */

         if (lastRunSize >= RUN_MASK) {
             size_t accumulator = lastRunSize - RUN_MASK;
             *op++ = (RUN_MASK << ML_BITS);
             for(; accumulator >= 255 ; accumulator -= 255) *op++ = 255;
             *op++ = (BYTE) accumulator;
         } else {
             *op++ = (BYTE)(lastRunSize << ML_BITS);
         }
         memcpy(op, anchor, lastRunSize);
         op += lastRunSize;
     }

     /* End */
     *srcSizePtr = (int) (((const char*)ip) - source);
     retval = (int) ((char*)op-dst);
     goto _return_label;

_dest_overflow:
if (limit == fillOutput) {
     /* Assumption : ip, anchor, ovml and ovref must be set correctly */
     size_t const ll = (size_t)(ip - anchor);
     size_t const ll_addbytes = (ll + 240) / 255;
     size_t const ll_totalCost = 1 + ll_addbytes + ll;
     BYTE* const maxLitPos = oend - 3; /* 2 for offset, 1 for token */
     DEBUGLOG(6, "Last sequence overflowing (only %i bytes remaining)", (int)(oend-1-opSaved));
     op = opSaved;  /* restore correct out pointer */
     if (op + ll_totalCost <= maxLitPos) {
         /* ll validated; now adjust match length */
         size_t const bytesLeftForMl = (size_t)(maxLitPos - (op+ll_totalCost));
         size_t const maxMlSize = MINMATCH + (ML_MASK-1) + (bytesLeftForMl * 255);
         assert(maxMlSize < INT_MAX); assert(ovml >= 0);
         if ((size_t)ovml > maxMlSize) ovml = (int)maxMlSize;
         if ((oend + LASTLITERALS) - (op + ll_totalCost + 2) - 1 + ovml >= MFLIMIT) {
             DEBUGLOG(6, "Space to end : %i + ml (%i)", (int)((oend + LASTLITERALS) - (op + ll_totalCost + 2) - 1), ovml);
             DEBUGLOG(6, "Before : ip = %p, anchor = %p", ip, anchor);
             LZ4HC_encodeSequence(UPDATABLE(ip, op, anchor), ovml, ovref, notLimited, oend);
             DEBUGLOG(6, "After : ip = %p, anchor = %p", ip, anchor);
     }   }
     goto _last_literals;
}
_return_label:
#ifdef LZ4HC_HEAPMODE
     FREEMEM(opt);
#endif
     return retval;
}
/*
 * LZ4 auto-framing library
 * Copyright (C) 2011-2016, Yann Collet.
 *
 * BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You can contact the author at :
 * - LZ4 homepage : http://www.lz4.org
 * - LZ4 source repository : https://github.com/lz4/lz4
 */

/* LZ4F is a stand-alone API to create LZ4-compressed Frames
 * in full conformance with specification v1.6.1 .
 * This library rely upon memory management capabilities (malloc, free)
 * provided either by <stdlib.h>,
 * or redirected towards another library of user's choice
 * (see Memory Routines below).
 */


/*-************************************
*  Compiler Options
**************************************/
#ifdef _MSC_VER    /* Visual Studio */
#  pragma warning(disable : 4127)        /* disable: C4127: conditional expression is constant */
#endif


/*-************************************
*  Tuning parameters
**************************************/
/*
 * LZ4F_HEAPMODE :
 * Select how default compression functions will allocate memory for their hash table,
 * in memory stack (0:default, fastest), or in memory heap (1:requires malloc()).
 */
#ifndef LZ4F_HEAPMODE
#  define LZ4F_HEAPMODE 0
#endif


/*-************************************
*  Memory routines
**************************************/
/*
 * User may redirect invocations of
 * malloc(), calloc() and free()
 * towards another library or solution of their choice
 * by modifying below section.
 */
#ifndef LZ4_SRC_INCLUDED   /* avoid redefinition when sources are coalesced */
#  include <stdlib.h>   /* malloc, calloc, free */
#  define ALLOC(s)          malloc(s)
#  define ALLOC_AND_ZERO(s) calloc(1,(s))
#  define FREEMEM(p)        free(p)
#endif

#include <string.h>   /* memset, memcpy, memmove */
#ifndef LZ4_SRC_INCLUDED  /* avoid redefinition when sources are coalesced */
#  define MEM_INIT(p,v,s)   memset((p),(v),(s))
#endif


/*-************************************
*  Library declarations
**************************************/
#define LZ4F_STATIC_LINKING_ONLY
#include "lz4frame.h"
#define LZ4_STATIC_LINKING_ONLY
#include "lz4.h"
#define LZ4_HC_STATIC_LINKING_ONLY
#include "lz4hc.h"
#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"


/*-************************************
*  Debug
**************************************/
#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=1)
#  include <assert.h>
#else
#  ifndef assert
#    define assert(condition) ((void)0)
#  endif
#endif

#define LZ4F_STATIC_ASSERT(c)    { enum { LZ4F_static_assert = 1/(int)(!!(c)) }; }   /* use only *after* variable declarations */

#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=2) && !defined(DEBUGLOG)
#  include <stdio.h>
static int g_debuglog_enable = 1;
#  define DEBUGLOG(l, ...) {                                  \
                if ((g_debuglog_enable) && (l<=LZ4_DEBUG)) {  \
                    fprintf(stderr, __FILE__ ": ");           \
                    fprintf(stderr, __VA_ARGS__);             \
                    fprintf(stderr, " \n");                   \
            }   }
#else
#  define DEBUGLOG(l, ...)      {}    /* disabled */
#endif


/*-************************************
*  Basic Types
**************************************/
#if !defined (__VMS) && (defined (__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */) )
# include <stdint.h>
  typedef  uint8_t BYTE;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef  int32_t S32;
  typedef uint64_t U64;
#else
  typedef unsigned char       BYTE;
  typedef unsigned short      U16;
  typedef unsigned int        U32;
  typedef   signed int        S32;
  typedef unsigned long long  U64;
#endif


/* unoptimized version; solves endianess & alignment issues */
static U32 LZ4F_readLE32 (const void* src)
{
    const BYTE* const srcPtr = (const BYTE*)src;
    U32 value32 = srcPtr[0];
    value32 += ((U32)srcPtr[1])<< 8;
    value32 += ((U32)srcPtr[2])<<16;
    value32 += ((U32)srcPtr[3])<<24;
    return value32;
}

static void LZ4F_writeLE32 (void* dst, U32 value32)
{
    BYTE* const dstPtr = (BYTE*)dst;
    dstPtr[0] = (BYTE)value32;
    dstPtr[1] = (BYTE)(value32 >> 8);
    dstPtr[2] = (BYTE)(value32 >> 16);
    dstPtr[3] = (BYTE)(value32 >> 24);
}

static U64 LZ4F_readLE64 (const void* src)
{
    const BYTE* const srcPtr = (const BYTE*)src;
    U64 value64 = srcPtr[0];
    value64 += ((U64)srcPtr[1]<<8);
    value64 += ((U64)srcPtr[2]<<16);
    value64 += ((U64)srcPtr[3]<<24);
    value64 += ((U64)srcPtr[4]<<32);
    value64 += ((U64)srcPtr[5]<<40);
    value64 += ((U64)srcPtr[6]<<48);
    value64 += ((U64)srcPtr[7]<<56);
    return value64;
}

static void LZ4F_writeLE64 (void* dst, U64 value64)
{
    BYTE* const dstPtr = (BYTE*)dst;
    dstPtr[0] = (BYTE)value64;
    dstPtr[1] = (BYTE)(value64 >> 8);
    dstPtr[2] = (BYTE)(value64 >> 16);
    dstPtr[3] = (BYTE)(value64 >> 24);
    dstPtr[4] = (BYTE)(value64 >> 32);
    dstPtr[5] = (BYTE)(value64 >> 40);
    dstPtr[6] = (BYTE)(value64 >> 48);
    dstPtr[7] = (BYTE)(value64 >> 56);
}


/*-************************************
*  Constants
**************************************/
#ifndef LZ4_SRC_INCLUDED   /* avoid double definition */
#  define KB *(1<<10)
#  define MB *(1<<20)
#  define GB *(1<<30)
#endif

#define _1BIT  0x01
#define _2BITS 0x03
#define _3BITS 0x07
#define _4BITS 0x0F
#define _8BITS 0xFF

#define LZ4F_MAGIC_SKIPPABLE_START 0x184D2A50U
#define LZ4F_MAGICNUMBER 0x184D2204U
#define LZ4F_BLOCKUNCOMPRESSED_FLAG 0x80000000U
#define LZ4F_BLOCKSIZEID_DEFAULT LZ4F_max64KB

static const size_t minFHSize = LZ4F_HEADER_SIZE_MIN;   /*  7 */
static const size_t maxFHSize = LZ4F_HEADER_SIZE_MAX;   /* 19 */
static const size_t BHSize = LZ4F_BLOCK_HEADER_SIZE;  /* block header : size, and compress flag */
static const size_t BFSize = LZ4F_BLOCK_CHECKSUM_SIZE;  /* block footer : checksum (optional) */


/*-************************************
*  Structures and local types
**************************************/
typedef struct LZ4F_cctx_s
{
    LZ4F_preferences_t prefs;
    U32    version;
    U32    cStage;
    const LZ4F_CDict* cdict;
    size_t maxBlockSize;
    size_t maxBufferSize;
    BYTE*  tmpBuff;
    BYTE*  tmpIn;
    size_t tmpInSize;
    U64    totalInSize;
    XXH32_state_t xxh;
    void*  lz4CtxPtr;
    U16    lz4CtxAlloc; /* sized for: 0 = none, 1 = lz4 ctx, 2 = lz4hc ctx */
    U16    lz4CtxState; /* in use as: 0 = none, 1 = lz4 ctx, 2 = lz4hc ctx */
} LZ4F_cctx_t;


/*-************************************
*  Error management
**************************************/
#define LZ4F_GENERATE_STRING(STRING) #STRING,
static const char* LZ4F_errorStrings[] = { LZ4F_LIST_ERRORS(LZ4F_GENERATE_STRING) };


unsigned LZ4F_isError(LZ4F_errorCode_t code)
{
    return (code > (LZ4F_errorCode_t)(-LZ4F_ERROR_maxCode));
}

const char* LZ4F_getErrorName(LZ4F_errorCode_t code)
{
    static const char* codeError = "Unspecified error code";
    if (LZ4F_isError(code)) return LZ4F_errorStrings[-(int)(code)];
    return codeError;
}

LZ4F_errorCodes LZ4F_getErrorCode(size_t functionResult)
{
    if (!LZ4F_isError(functionResult)) return LZ4F_OK_NoError;
    return (LZ4F_errorCodes)(-(ptrdiff_t)functionResult);
}

static LZ4F_errorCode_t err0r(LZ4F_errorCodes code)
{
    /* A compilation error here means sizeof(ptrdiff_t) is not large enough */
    LZ4F_STATIC_ASSERT(sizeof(ptrdiff_t) >= sizeof(size_t));
    return (LZ4F_errorCode_t)-(ptrdiff_t)code;
}

unsigned LZ4F_getVersion(void) { return LZ4F_VERSION; }

int LZ4F_compressionLevel_max(void) { return LZ4HC_CLEVEL_MAX; }

size_t LZ4F_getBlockSize(unsigned blockSizeID)
{
    static const size_t blockSizes[4] = { 64 KB, 256 KB, 1 MB, 4 MB };

    if (blockSizeID == 0) blockSizeID = LZ4F_BLOCKSIZEID_DEFAULT;
    if (blockSizeID < LZ4F_max64KB || blockSizeID > LZ4F_max4MB)
        return err0r(LZ4F_ERROR_maxBlockSize_invalid);
    blockSizeID -= LZ4F_max64KB;
    return blockSizes[blockSizeID];
}

/*-************************************
*  Private functions
**************************************/
#define MIN(a,b)   ( (a) < (b) ? (a) : (b) )

static BYTE LZ4F_headerChecksum (const void* header, size_t length)
{
    U32 const xxh = XXH32(header, length, 0);
    return (BYTE)(xxh >> 8);
}


/*-************************************
*  Simple-pass compression functions
**************************************/
static LZ4F_blockSizeID_t LZ4F_optimalBSID(const LZ4F_blockSizeID_t requestedBSID,
                                           const size_t srcSize)
{
    LZ4F_blockSizeID_t proposedBSID = LZ4F_max64KB;
    size_t maxBlockSize = 64 KB;
    while (requestedBSID > proposedBSID) {
        if (srcSize <= maxBlockSize)
            return proposedBSID;
        proposedBSID = (LZ4F_blockSizeID_t)((int)proposedBSID + 1);
        maxBlockSize <<= 2;
    }
    return requestedBSID;
}

/*! LZ4F_compressBound_internal() :
 *  Provides dstCapacity given a srcSize to guarantee operation success in worst case situations.
 *  prefsPtr is optional : if NULL is provided, preferences will be set to cover worst case scenario.
 * @return is always the same for a srcSize and prefsPtr, so it can be relied upon to size reusable buffers.
 *  When srcSize==0, LZ4F_compressBound() provides an upper bound for LZ4F_flush() and LZ4F_compressEnd() operations.
 */
static size_t LZ4F_compressBound_internal(size_t srcSize,
                                    const LZ4F_preferences_t* preferencesPtr,
                                          size_t alreadyBuffered)
{
    LZ4F_preferences_t prefsNull = LZ4F_INIT_PREFERENCES;
    prefsNull.frameInfo.contentChecksumFlag = LZ4F_contentChecksumEnabled;   /* worst case */
    prefsNull.frameInfo.blockChecksumFlag = LZ4F_blockChecksumEnabled;   /* worst case */
    {   const LZ4F_preferences_t* const prefsPtr = (preferencesPtr==NULL) ? &prefsNull : preferencesPtr;
        U32 const flush = prefsPtr->autoFlush | (srcSize==0);
        LZ4F_blockSizeID_t const blockID = prefsPtr->frameInfo.blockSizeID;
        size_t const blockSize = LZ4F_getBlockSize(blockID);
        size_t const maxBuffered = blockSize - 1;
        size_t const bufferedSize = MIN(alreadyBuffered, maxBuffered);
        size_t const maxSrcSize = srcSize + bufferedSize;
        unsigned const nbFullBlocks = (unsigned)(maxSrcSize / blockSize);
        size_t const partialBlockSize = maxSrcSize & (blockSize-1);
        size_t const lastBlockSize = flush ? partialBlockSize : 0;
        unsigned const nbBlocks = nbFullBlocks + (lastBlockSize>0);

        size_t const blockCRCSize = BFSize * prefsPtr->frameInfo.blockChecksumFlag;
        size_t const frameEnd = BHSize + (prefsPtr->frameInfo.contentChecksumFlag*BFSize);

        return ((BHSize + blockCRCSize) * nbBlocks) +
               (blockSize * nbFullBlocks) + lastBlockSize + frameEnd;
    }
}

size_t LZ4F_compressFrameBound(size_t srcSize, const LZ4F_preferences_t* preferencesPtr)
{
    LZ4F_preferences_t prefs;
    size_t const headerSize = maxFHSize;      /* max header size, including optional fields */

    if (preferencesPtr!=NULL) prefs = *preferencesPtr;
    else MEM_INIT(&prefs, 0, sizeof(prefs));
    prefs.autoFlush = 1;

    return headerSize + LZ4F_compressBound_internal(srcSize, &prefs, 0);;
}


/*! LZ4F_compressFrame_usingCDict() :
 *  Compress srcBuffer using a dictionary, in a single step.
 *  cdict can be NULL, in which case, no dictionary is used.
 *  dstBuffer MUST be >= LZ4F_compressFrameBound(srcSize, preferencesPtr).
 *  The LZ4F_preferences_t structure is optional : you may provide NULL as argument,
 *  however, it's the only way to provide a dictID, so it's not recommended.
 * @return : number of bytes written into dstBuffer,
 *           or an error code if it fails (can be tested using LZ4F_isError())
 */
size_t LZ4F_compressFrame_usingCDict(LZ4F_cctx* cctx,
                                     void* dstBuffer, size_t dstCapacity,
                               const void* srcBuffer, size_t srcSize,
                               const LZ4F_CDict* cdict,
                               const LZ4F_preferences_t* preferencesPtr)
{
    LZ4F_preferences_t prefs;
    LZ4F_compressOptions_t options;
    BYTE* const dstStart = (BYTE*) dstBuffer;
    BYTE* dstPtr = dstStart;
    BYTE* const dstEnd = dstStart + dstCapacity;

    if (preferencesPtr!=NULL)
        prefs = *preferencesPtr;
    else
        MEM_INIT(&prefs, 0, sizeof(prefs));
    if (prefs.frameInfo.contentSize != 0)
        prefs.frameInfo.contentSize = (U64)srcSize;   /* auto-correct content size if selected (!=0) */

    prefs.frameInfo.blockSizeID = LZ4F_optimalBSID(prefs.frameInfo.blockSizeID, srcSize);
    prefs.autoFlush = 1;
    if (srcSize <= LZ4F_getBlockSize(prefs.frameInfo.blockSizeID))
        prefs.frameInfo.blockMode = LZ4F_blockIndependent;   /* only one block => no need for inter-block link */

    MEM_INIT(&options, 0, sizeof(options));
    options.stableSrc = 1;

    if (dstCapacity < LZ4F_compressFrameBound(srcSize, &prefs))  /* condition to guarantee success */
        return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);

    { size_t const headerSize = LZ4F_compressBegin_usingCDict(cctx, dstBuffer, dstCapacity, cdict, &prefs);  /* write header */
      if (LZ4F_isError(headerSize)) return headerSize;
      dstPtr += headerSize;   /* header size */ }

    assert(dstEnd >= dstPtr);
    { size_t const cSize = LZ4F_compressUpdate(cctx, dstPtr, (size_t)(dstEnd-dstPtr), srcBuffer, srcSize, &options);
      if (LZ4F_isError(cSize)) return cSize;
      dstPtr += cSize; }

    assert(dstEnd >= dstPtr);
    { size_t const tailSize = LZ4F_compressEnd(cctx, dstPtr, (size_t)(dstEnd-dstPtr), &options);   /* flush last block, and generate suffix */
      if (LZ4F_isError(tailSize)) return tailSize;
      dstPtr += tailSize; }

    assert(dstEnd >= dstStart);
    return (size_t)(dstPtr - dstStart);
}


/*! LZ4F_compressFrame() :
 *  Compress an entire srcBuffer into a valid LZ4 frame, in a single step.
 *  dstBuffer MUST be >= LZ4F_compressFrameBound(srcSize, preferencesPtr).
 *  The LZ4F_preferences_t structure is optional : you can provide NULL as argument. All preferences will be set to default.
 * @return : number of bytes written into dstBuffer.
 *           or an error code if it fails (can be tested using LZ4F_isError())
 */
size_t LZ4F_compressFrame(void* dstBuffer, size_t dstCapacity,
                    const void* srcBuffer, size_t srcSize,
                    const LZ4F_preferences_t* preferencesPtr)
{
    size_t result;
#if (LZ4F_HEAPMODE)
    LZ4F_cctx_t *cctxPtr;
    result = LZ4F_createCompressionContext(&cctxPtr, LZ4F_VERSION);
    if (LZ4F_isError(result)) return result;
#else
    LZ4F_cctx_t cctx;
    LZ4_stream_t lz4ctx;
    LZ4F_cctx_t *cctxPtr = &cctx;

    DEBUGLOG(4, "LZ4F_compressFrame");
    MEM_INIT(&cctx, 0, sizeof(cctx));
    cctx.version = LZ4F_VERSION;
    cctx.maxBufferSize = 5 MB;   /* mess with real buffer size to prevent dynamic allocation; works only because autoflush==1 & stableSrc==1 */
    if (preferencesPtr == NULL ||
        preferencesPtr->compressionLevel < LZ4HC_CLEVEL_MIN)
    {
        LZ4_initStream(&lz4ctx, sizeof(lz4ctx));
        cctxPtr->lz4CtxPtr = &lz4ctx;
        cctxPtr->lz4CtxAlloc = 1;
        cctxPtr->lz4CtxState = 1;
    }
#endif

    result = LZ4F_compressFrame_usingCDict(cctxPtr, dstBuffer, dstCapacity,
                                           srcBuffer, srcSize,
                                           NULL, preferencesPtr);

#if (LZ4F_HEAPMODE)
    LZ4F_freeCompressionContext(cctxPtr);
#else
    if (preferencesPtr != NULL &&
        preferencesPtr->compressionLevel >= LZ4HC_CLEVEL_MIN)
    {
        FREEMEM(cctxPtr->lz4CtxPtr);
    }
#endif
    return result;
}


/*-***************************************************
*   Dictionary compression
*****************************************************/

struct LZ4F_CDict_s {
    void* dictContent;
    LZ4_stream_t* fastCtx;
    LZ4_streamHC_t* HCCtx;
}; /* typedef'd to LZ4F_CDict within lz4frame_static.h */

/*! LZ4F_createCDict() :
 *  When compressing multiple messages / blocks with the same dictionary, it's recommended to load it just once.
 *  LZ4F_createCDict() will create a digested dictionary, ready to start future compression operations without startup delay.
 *  LZ4F_CDict can be created once and shared by multiple threads concurrently, since its usage is read-only.
 * `dictBuffer` can be released after LZ4F_CDict creation, since its content is copied within CDict
 * @return : digested dictionary for compression, or NULL if failed */
LZ4F_CDict* LZ4F_createCDict(const void* dictBuffer, size_t dictSize)
{
    const char* dictStart = (const char*)dictBuffer;
    LZ4F_CDict* cdict = (LZ4F_CDict*) ALLOC(sizeof(*cdict));
    DEBUGLOG(4, "LZ4F_createCDict");
    if (!cdict) return NULL;
    if (dictSize > 64 KB) {
        dictStart += dictSize - 64 KB;
        dictSize = 64 KB;
    }
    cdict->dictContent = ALLOC(dictSize);
    cdict->fastCtx = LZ4_createStream();
    cdict->HCCtx = LZ4_createStreamHC();
    if (!cdict->dictContent || !cdict->fastCtx || !cdict->HCCtx) {
        LZ4F_freeCDict(cdict);
        return NULL;
    }
    memcpy(cdict->dictContent, dictStart, dictSize);
    LZ4_loadDict (cdict->fastCtx, (const char*)cdict->dictContent, (int)dictSize);
    LZ4_setCompressionLevel(cdict->HCCtx, LZ4HC_CLEVEL_DEFAULT);
    LZ4_loadDictHC(cdict->HCCtx, (const char*)cdict->dictContent, (int)dictSize);
    return cdict;
}

void LZ4F_freeCDict(LZ4F_CDict* cdict)
{
    if (cdict==NULL) return;  /* support free on NULL */
    FREEMEM(cdict->dictContent);
    LZ4_freeStream(cdict->fastCtx);
    LZ4_freeStreamHC(cdict->HCCtx);
    FREEMEM(cdict);
}


/*-*********************************
*  Advanced compression functions
***********************************/

/*! LZ4F_createCompressionContext() :
 *  The first thing to do is to create a compressionContext object, which will be used in all compression operations.
 *  This is achieved using LZ4F_createCompressionContext(), which takes as argument a version and an LZ4F_preferences_t structure.
 *  The version provided MUST be LZ4F_VERSION. It is intended to track potential incompatible differences between different binaries.
 *  The function will provide a pointer to an allocated LZ4F_compressionContext_t object.
 *  If the result LZ4F_errorCode_t is not OK_NoError, there was an error during context creation.
 *  Object can release its memory using LZ4F_freeCompressionContext();
 */
LZ4F_errorCode_t LZ4F_createCompressionContext(LZ4F_cctx** LZ4F_compressionContextPtr, unsigned version)
{
    LZ4F_cctx_t* const cctxPtr = (LZ4F_cctx_t*)ALLOC_AND_ZERO(sizeof(LZ4F_cctx_t));
    if (cctxPtr==NULL) return err0r(LZ4F_ERROR_allocation_failed);

    cctxPtr->version = version;
    cctxPtr->cStage = 0;   /* Next stage : init stream */

    *LZ4F_compressionContextPtr = cctxPtr;

    return LZ4F_OK_NoError;
}


LZ4F_errorCode_t LZ4F_freeCompressionContext(LZ4F_cctx* cctxPtr)
{
    if (cctxPtr != NULL) {  /* support free on NULL */
       FREEMEM(cctxPtr->lz4CtxPtr);  /* note: LZ4_streamHC_t and LZ4_stream_t are simple POD types */
       FREEMEM(cctxPtr->tmpBuff);
       FREEMEM(cctxPtr);
    }

    return LZ4F_OK_NoError;
}


/**
 * This function prepares the internal LZ4(HC) stream for a new compression,
 * resetting the context and attaching the dictionary, if there is one.
 *
 * It needs to be called at the beginning of each independent compression
 * stream (i.e., at the beginning of a frame in blockLinked mode, or at the
 * beginning of each block in blockIndependent mode).
 */
static void LZ4F_initStream(void* ctx,
                            const LZ4F_CDict* cdict,
                            int level,
                            LZ4F_blockMode_t blockMode) {
    if (level < LZ4HC_CLEVEL_MIN) {
        if (cdict != NULL || blockMode == LZ4F_blockLinked) {
            /* In these cases, we will call LZ4_compress_fast_continue(),
             * which needs an already reset context. Otherwise, we'll call a
             * one-shot API. The non-continued APIs internally perform their own
             * resets at the beginning of their calls, where they know what
             * tableType they need the context to be in. So in that case this
             * would be misguided / wasted work. */
            LZ4_resetStream_fast((LZ4_stream_t*)ctx);
        }
        LZ4_attach_dictionary((LZ4_stream_t *)ctx, cdict ? cdict->fastCtx : NULL);
    } else {
        LZ4_resetStreamHC_fast((LZ4_streamHC_t*)ctx, level);
        LZ4_attach_HC_dictionary((LZ4_streamHC_t *)ctx, cdict ? cdict->HCCtx : NULL);
    }
}


/*! LZ4F_compressBegin_usingCDict() :
 *  init streaming compression and writes frame header into dstBuffer.
 *  dstBuffer must be >= LZ4F_HEADER_SIZE_MAX bytes.
 * @return : number of bytes written into dstBuffer for the header
 *           or an error code (can be tested using LZ4F_isError())
 */
size_t LZ4F_compressBegin_usingCDict(LZ4F_cctx* cctxPtr,
                          void* dstBuffer, size_t dstCapacity,
                          const LZ4F_CDict* cdict,
                          const LZ4F_preferences_t* preferencesPtr)
{
    LZ4F_preferences_t prefNull;
    BYTE* const dstStart = (BYTE*)dstBuffer;
    BYTE* dstPtr = dstStart;
    BYTE* headerStart;

    if (dstCapacity < maxFHSize) return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);
    MEM_INIT(&prefNull, 0, sizeof(prefNull));
    if (preferencesPtr == NULL) preferencesPtr = &prefNull;
    cctxPtr->prefs = *preferencesPtr;

    /* Ctx Management */
    {   U16 const ctxTypeID = (cctxPtr->prefs.compressionLevel < LZ4HC_CLEVEL_MIN) ? 1 : 2;
        if (cctxPtr->lz4CtxAlloc < ctxTypeID) {
            FREEMEM(cctxPtr->lz4CtxPtr);
            if (cctxPtr->prefs.compressionLevel < LZ4HC_CLEVEL_MIN) {
                cctxPtr->lz4CtxPtr = LZ4_createStream();
            } else {
                cctxPtr->lz4CtxPtr = LZ4_createStreamHC();
            }
            if (cctxPtr->lz4CtxPtr == NULL)
                return err0r(LZ4F_ERROR_allocation_failed);
            cctxPtr->lz4CtxAlloc = ctxTypeID;
            cctxPtr->lz4CtxState = ctxTypeID;
        } else if (cctxPtr->lz4CtxState != ctxTypeID) {
            /* otherwise, a sufficient buffer is allocated, but we need to
             * reset it to the correct context type */
            if (cctxPtr->prefs.compressionLevel < LZ4HC_CLEVEL_MIN) {
                LZ4_initStream((LZ4_stream_t *) cctxPtr->lz4CtxPtr, sizeof (LZ4_stream_t));
            } else {
                LZ4_initStreamHC((LZ4_streamHC_t *) cctxPtr->lz4CtxPtr, sizeof(LZ4_streamHC_t));
                LZ4_setCompressionLevel((LZ4_streamHC_t *) cctxPtr->lz4CtxPtr, cctxPtr->prefs.compressionLevel);
            }
            cctxPtr->lz4CtxState = ctxTypeID;
        }
    }

    /* Buffer Management */
    if (cctxPtr->prefs.frameInfo.blockSizeID == 0)
        cctxPtr->prefs.frameInfo.blockSizeID = LZ4F_BLOCKSIZEID_DEFAULT;
    cctxPtr->maxBlockSize = LZ4F_getBlockSize(cctxPtr->prefs.frameInfo.blockSizeID);

    {   size_t const requiredBuffSize = preferencesPtr->autoFlush ?
                ((cctxPtr->prefs.frameInfo.blockMode == LZ4F_blockLinked) ? 64 KB : 0) :  /* only needs past data up to window size */
                cctxPtr->maxBlockSize + ((cctxPtr->prefs.frameInfo.blockMode == LZ4F_blockLinked) ? 128 KB : 0);

        if (cctxPtr->maxBufferSize < requiredBuffSize) {
            cctxPtr->maxBufferSize = 0;
            FREEMEM(cctxPtr->tmpBuff);
            cctxPtr->tmpBuff = (BYTE*)ALLOC_AND_ZERO(requiredBuffSize);
            if (cctxPtr->tmpBuff == NULL) return err0r(LZ4F_ERROR_allocation_failed);
            cctxPtr->maxBufferSize = requiredBuffSize;
    }   }
    cctxPtr->tmpIn = cctxPtr->tmpBuff;
    cctxPtr->tmpInSize = 0;
    (void)XXH32_reset(&(cctxPtr->xxh), 0);

    /* context init */
    cctxPtr->cdict = cdict;
    if (cctxPtr->prefs.frameInfo.blockMode == LZ4F_blockLinked) {
        /* frame init only for blockLinked : blockIndependent will be init at each block */
        LZ4F_initStream(cctxPtr->lz4CtxPtr, cdict, cctxPtr->prefs.compressionLevel, LZ4F_blockLinked);
    }
    if (preferencesPtr->compressionLevel >= LZ4HC_CLEVEL_MIN) {
        LZ4_favorDecompressionSpeed((LZ4_streamHC_t*)cctxPtr->lz4CtxPtr, (int)preferencesPtr->favorDecSpeed);
    }

    /* Magic Number */
    LZ4F_writeLE32(dstPtr, LZ4F_MAGICNUMBER);
    dstPtr += 4;
    headerStart = dstPtr;

    /* FLG Byte */
    *dstPtr++ = (BYTE)(((1 & _2BITS) << 6)    /* Version('01') */
        + ((cctxPtr->prefs.frameInfo.blockMode & _1BIT ) << 5)
        + ((cctxPtr->prefs.frameInfo.blockChecksumFlag & _1BIT ) << 4)
        + ((unsigned)(cctxPtr->prefs.frameInfo.contentSize > 0) << 3)
        + ((cctxPtr->prefs.frameInfo.contentChecksumFlag & _1BIT ) << 2)
        +  (cctxPtr->prefs.frameInfo.dictID > 0) );
    /* BD Byte */
    *dstPtr++ = (BYTE)((cctxPtr->prefs.frameInfo.blockSizeID & _3BITS) << 4);
    /* Optional Frame content size field */
    if (cctxPtr->prefs.frameInfo.contentSize) {
        LZ4F_writeLE64(dstPtr, cctxPtr->prefs.frameInfo.contentSize);
        dstPtr += 8;
        cctxPtr->totalInSize = 0;
    }
    /* Optional dictionary ID field */
    if (cctxPtr->prefs.frameInfo.dictID) {
        LZ4F_writeLE32(dstPtr, cctxPtr->prefs.frameInfo.dictID);
        dstPtr += 4;
    }
    /* Header CRC Byte */
    *dstPtr = LZ4F_headerChecksum(headerStart, (size_t)(dstPtr - headerStart));
    dstPtr++;

    cctxPtr->cStage = 1;   /* header written, now request input data block */
    return (size_t)(dstPtr - dstStart);
}


/*! LZ4F_compressBegin() :
 *  init streaming compression and writes frame header into dstBuffer.
 *  dstBuffer must be >= LZ4F_HEADER_SIZE_MAX bytes.
 *  preferencesPtr can be NULL, in which case default parameters are selected.
 * @return : number of bytes written into dstBuffer for the header
 *        or an error code (can be tested using LZ4F_isError())
 */
size_t LZ4F_compressBegin(LZ4F_cctx* cctxPtr,
                          void* dstBuffer, size_t dstCapacity,
                          const LZ4F_preferences_t* preferencesPtr)
{
    return LZ4F_compressBegin_usingCDict(cctxPtr, dstBuffer, dstCapacity,
                                         NULL, preferencesPtr);
}


/*  LZ4F_compressBound() :
 * @return minimum capacity of dstBuffer for a given srcSize to handle worst case scenario.
 *  LZ4F_preferences_t structure is optional : if NULL, preferences will be set to cover worst case scenario.
 *  This function cannot fail.
 */
size_t LZ4F_compressBound(size_t srcSize, const LZ4F_preferences_t* preferencesPtr)
{
    if (preferencesPtr && preferencesPtr->autoFlush) {
        return LZ4F_compressBound_internal(srcSize, preferencesPtr, 0);
    }
    return LZ4F_compressBound_internal(srcSize, preferencesPtr, (size_t)-1);
}


typedef int (*compressFunc_t)(void* ctx, const char* src, char* dst, int srcSize, int dstSize, int level, const LZ4F_CDict* cdict);


/*! LZ4F_makeBlock():
 *  compress a single block, add header and optional checksum.
 *  assumption : dst buffer capacity is >= BHSize + srcSize + crcSize
 */
static size_t LZ4F_makeBlock(void* dst,
                       const void* src, size_t srcSize,
                             compressFunc_t compress, void* lz4ctx, int level,
                       const LZ4F_CDict* cdict,
                             LZ4F_blockChecksum_t crcFlag)
{
    BYTE* const cSizePtr = (BYTE*)dst;
    U32 cSize = (U32)compress(lz4ctx, (const char*)src, (char*)(cSizePtr+BHSize),
                                      (int)(srcSize), (int)(srcSize-1),
                                      level, cdict);
    if (cSize == 0) {  /* compression failed */
        DEBUGLOG(5, "LZ4F_makeBlock: compression failed, creating a raw block (size %u)", (U32)srcSize);
        cSize = (U32)srcSize;
        LZ4F_writeLE32(cSizePtr, cSize | LZ4F_BLOCKUNCOMPRESSED_FLAG);
        memcpy(cSizePtr+BHSize, src, srcSize);
    } else {
        LZ4F_writeLE32(cSizePtr, cSize);
    }
    if (crcFlag) {
        U32 const crc32 = XXH32(cSizePtr+BHSize, cSize, 0);  /* checksum of compressed data */
        LZ4F_writeLE32(cSizePtr+BHSize+cSize, crc32);
    }
    return BHSize + cSize + ((U32)crcFlag)*BFSize;
}


static int LZ4F_compressBlock(void* ctx, const char* src, char* dst, int srcSize, int dstCapacity, int level, const LZ4F_CDict* cdict)
{
    int const acceleration = (level < 0) ? -level + 1 : 1;
    LZ4F_initStream(ctx, cdict, level, LZ4F_blockIndependent);
    if (cdict) {
        return LZ4_compress_fast_continue((LZ4_stream_t*)ctx, src, dst, srcSize, dstCapacity, acceleration);
    } else {
        return LZ4_compress_fast_extState_fastReset(ctx, src, dst, srcSize, dstCapacity, acceleration);
    }
}

static int LZ4F_compressBlock_continue(void* ctx, const char* src, char* dst, int srcSize, int dstCapacity, int level, const LZ4F_CDict* cdict)
{
    int const acceleration = (level < 0) ? -level + 1 : 1;
    (void)cdict; /* init once at beginning of frame */
    return LZ4_compress_fast_continue((LZ4_stream_t*)ctx, src, dst, srcSize, dstCapacity, acceleration);
}

static int LZ4F_compressBlockHC(void* ctx, const char* src, char* dst, int srcSize, int dstCapacity, int level, const LZ4F_CDict* cdict)
{
    LZ4F_initStream(ctx, cdict, level, LZ4F_blockIndependent);
    if (cdict) {
        return LZ4_compress_HC_continue((LZ4_streamHC_t*)ctx, src, dst, srcSize, dstCapacity);
    }
    return LZ4_compress_HC_extStateHC_fastReset(ctx, src, dst, srcSize, dstCapacity, level);
}

static int LZ4F_compressBlockHC_continue(void* ctx, const char* src, char* dst, int srcSize, int dstCapacity, int level, const LZ4F_CDict* cdict)
{
    (void)level; (void)cdict; /* init once at beginning of frame */
    return LZ4_compress_HC_continue((LZ4_streamHC_t*)ctx, src, dst, srcSize, dstCapacity);
}

static compressFunc_t LZ4F_selectCompression(LZ4F_blockMode_t blockMode, int level)
{
    if (level < LZ4HC_CLEVEL_MIN) {
        if (blockMode == LZ4F_blockIndependent) return LZ4F_compressBlock;
        return LZ4F_compressBlock_continue;
    }
    if (blockMode == LZ4F_blockIndependent) return LZ4F_compressBlockHC;
    return LZ4F_compressBlockHC_continue;
}

static int LZ4F_localSaveDict(LZ4F_cctx_t* cctxPtr)
{
    if (cctxPtr->prefs.compressionLevel < LZ4HC_CLEVEL_MIN)
        return LZ4_saveDict ((LZ4_stream_t*)(cctxPtr->lz4CtxPtr), (char*)(cctxPtr->tmpBuff), 64 KB);
    return LZ4_saveDictHC ((LZ4_streamHC_t*)(cctxPtr->lz4CtxPtr), (char*)(cctxPtr->tmpBuff), 64 KB);
}

typedef enum { notDone, fromTmpBuffer, fromSrcBuffer } LZ4F_lastBlockStatus;

/*! LZ4F_compressUpdate() :
 *  LZ4F_compressUpdate() can be called repetitively to compress as much data as necessary.
 *  dstBuffer MUST be >= LZ4F_compressBound(srcSize, preferencesPtr).
 *  LZ4F_compressOptions_t structure is optional : you can provide NULL as argument.
 * @return : the number of bytes written into dstBuffer. It can be zero, meaning input data was just buffered.
 *           or an error code if it fails (which can be tested using LZ4F_isError())
 */
size_t LZ4F_compressUpdate(LZ4F_cctx* cctxPtr,
                           void* dstBuffer, size_t dstCapacity,
                     const void* srcBuffer, size_t srcSize,
                     const LZ4F_compressOptions_t* compressOptionsPtr)
{
    LZ4F_compressOptions_t cOptionsNull;
    size_t const blockSize = cctxPtr->maxBlockSize;
    const BYTE* srcPtr = (const BYTE*)srcBuffer;
    const BYTE* const srcEnd = srcPtr + srcSize;
    BYTE* const dstStart = (BYTE*)dstBuffer;
    BYTE* dstPtr = dstStart;
    LZ4F_lastBlockStatus lastBlockCompressed = notDone;
    compressFunc_t const compress = LZ4F_selectCompression(cctxPtr->prefs.frameInfo.blockMode, cctxPtr->prefs.compressionLevel);

    DEBUGLOG(4, "LZ4F_compressUpdate (srcSize=%zu)", srcSize);

    if (cctxPtr->cStage != 1) return err0r(LZ4F_ERROR_GENERIC);
    if (dstCapacity < LZ4F_compressBound_internal(srcSize, &(cctxPtr->prefs), cctxPtr->tmpInSize))
        return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);
    MEM_INIT(&cOptionsNull, 0, sizeof(cOptionsNull));
    if (compressOptionsPtr == NULL) compressOptionsPtr = &cOptionsNull;

    /* complete tmp buffer */
    if (cctxPtr->tmpInSize > 0) {   /* some data already within tmp buffer */
        size_t const sizeToCopy = blockSize - cctxPtr->tmpInSize;
        if (sizeToCopy > srcSize) {
            /* add src to tmpIn buffer */
            memcpy(cctxPtr->tmpIn + cctxPtr->tmpInSize, srcBuffer, srcSize);
            srcPtr = srcEnd;
            cctxPtr->tmpInSize += srcSize;
            /* still needs some CRC */
        } else {
            /* complete tmpIn block and then compress it */
            lastBlockCompressed = fromTmpBuffer;
            memcpy(cctxPtr->tmpIn + cctxPtr->tmpInSize, srcBuffer, sizeToCopy);
            srcPtr += sizeToCopy;

            dstPtr += LZ4F_makeBlock(dstPtr,
                                     cctxPtr->tmpIn, blockSize,
                                     compress, cctxPtr->lz4CtxPtr, cctxPtr->prefs.compressionLevel,
                                     cctxPtr->cdict,
                                     cctxPtr->prefs.frameInfo.blockChecksumFlag);

            if (cctxPtr->prefs.frameInfo.blockMode==LZ4F_blockLinked) cctxPtr->tmpIn += blockSize;
            cctxPtr->tmpInSize = 0;
        }
    }

    while ((size_t)(srcEnd - srcPtr) >= blockSize) {
        /* compress full blocks */
        lastBlockCompressed = fromSrcBuffer;
        dstPtr += LZ4F_makeBlock(dstPtr,
                                 srcPtr, blockSize,
                                 compress, cctxPtr->lz4CtxPtr, cctxPtr->prefs.compressionLevel,
                                 cctxPtr->cdict,
                                 cctxPtr->prefs.frameInfo.blockChecksumFlag);
        srcPtr += blockSize;
    }

    if ((cctxPtr->prefs.autoFlush) && (srcPtr < srcEnd)) {
        /* compress remaining input < blockSize */
        lastBlockCompressed = fromSrcBuffer;
        dstPtr += LZ4F_makeBlock(dstPtr,
                                 srcPtr, (size_t)(srcEnd - srcPtr),
                                 compress, cctxPtr->lz4CtxPtr, cctxPtr->prefs.compressionLevel,
                                 cctxPtr->cdict,
                                 cctxPtr->prefs.frameInfo.blockChecksumFlag);
        srcPtr  = srcEnd;
    }

    /* preserve dictionary if necessary */
    if ((cctxPtr->prefs.frameInfo.blockMode==LZ4F_blockLinked) && (lastBlockCompressed==fromSrcBuffer)) {
        if (compressOptionsPtr->stableSrc) {
            cctxPtr->tmpIn = cctxPtr->tmpBuff;
        } else {
            int const realDictSize = LZ4F_localSaveDict(cctxPtr);
            if (realDictSize==0) return err0r(LZ4F_ERROR_GENERIC);
            cctxPtr->tmpIn = cctxPtr->tmpBuff + realDictSize;
        }
    }

    /* keep tmpIn within limits */
    if ((cctxPtr->tmpIn + blockSize) > (cctxPtr->tmpBuff + cctxPtr->maxBufferSize)   /* necessarily LZ4F_blockLinked && lastBlockCompressed==fromTmpBuffer */
        && !(cctxPtr->prefs.autoFlush))
    {
        int const realDictSize = LZ4F_localSaveDict(cctxPtr);
        cctxPtr->tmpIn = cctxPtr->tmpBuff + realDictSize;
    }

    /* some input data left, necessarily < blockSize */
    if (srcPtr < srcEnd) {
        /* fill tmp buffer */
        size_t const sizeToCopy = (size_t)(srcEnd - srcPtr);
        memcpy(cctxPtr->tmpIn, srcPtr, sizeToCopy);
        cctxPtr->tmpInSize = sizeToCopy;
    }

    if (cctxPtr->prefs.frameInfo.contentChecksumFlag == LZ4F_contentChecksumEnabled)
        (void)XXH32_update(&(cctxPtr->xxh), srcBuffer, srcSize);

    cctxPtr->totalInSize += srcSize;
    return (size_t)(dstPtr - dstStart);
}


/*! LZ4F_flush() :
 *  When compressed data must be sent immediately, without waiting for a block to be filled,
 *  invoke LZ4_flush(), which will immediately compress any remaining data stored within LZ4F_cctx.
 *  The result of the function is the number of bytes written into dstBuffer.
 *  It can be zero, this means there was no data left within LZ4F_cctx.
 *  The function outputs an error code if it fails (can be tested using LZ4F_isError())
 *  LZ4F_compressOptions_t* is optional. NULL is a valid argument.
 */
size_t LZ4F_flush(LZ4F_cctx* cctxPtr,
                  void* dstBuffer, size_t dstCapacity,
            const LZ4F_compressOptions_t* compressOptionsPtr)
{
    BYTE* const dstStart = (BYTE*)dstBuffer;
    BYTE* dstPtr = dstStart;
    compressFunc_t compress;

    if (cctxPtr->tmpInSize == 0) return 0;   /* nothing to flush */
    if (cctxPtr->cStage != 1) return err0r(LZ4F_ERROR_GENERIC);
    if (dstCapacity < (cctxPtr->tmpInSize + BHSize + BFSize))
        return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);
    (void)compressOptionsPtr;   /* not yet useful */

    /* select compression function */
    compress = LZ4F_selectCompression(cctxPtr->prefs.frameInfo.blockMode, cctxPtr->prefs.compressionLevel);

    /* compress tmp buffer */
    dstPtr += LZ4F_makeBlock(dstPtr,
                             cctxPtr->tmpIn, cctxPtr->tmpInSize,
                             compress, cctxPtr->lz4CtxPtr, cctxPtr->prefs.compressionLevel,
                             cctxPtr->cdict,
                             cctxPtr->prefs.frameInfo.blockChecksumFlag);
    assert(((void)"flush overflows dstBuffer!", (size_t)(dstPtr - dstStart) <= dstCapacity));

    if (cctxPtr->prefs.frameInfo.blockMode == LZ4F_blockLinked)
        cctxPtr->tmpIn += cctxPtr->tmpInSize;
    cctxPtr->tmpInSize = 0;

    /* keep tmpIn within limits */
    if ((cctxPtr->tmpIn + cctxPtr->maxBlockSize) > (cctxPtr->tmpBuff + cctxPtr->maxBufferSize)) {  /* necessarily LZ4F_blockLinked */
        int const realDictSize = LZ4F_localSaveDict(cctxPtr);
        cctxPtr->tmpIn = cctxPtr->tmpBuff + realDictSize;
    }

    return (size_t)(dstPtr - dstStart);
}


/*! LZ4F_compressEnd() :
 *  When you want to properly finish the compressed frame, just call LZ4F_compressEnd().
 *  It will flush whatever data remained within compressionContext (like LZ4_flush())
 *  but also properly finalize the frame, with an endMark and an (optional) checksum.
 *  LZ4F_compressOptions_t structure is optional : you can provide NULL as argument.
 * @return: the number of bytes written into dstBuffer (necessarily >= 4 (endMark size))
 *       or an error code if it fails (can be tested using LZ4F_isError())
 *  The context can then be used again to compress a new frame, starting with LZ4F_compressBegin().
 */
size_t LZ4F_compressEnd(LZ4F_cctx* cctxPtr,
                        void* dstBuffer, size_t dstCapacity,
                  const LZ4F_compressOptions_t* compressOptionsPtr)
{
    BYTE* const dstStart = (BYTE*)dstBuffer;
    BYTE* dstPtr = dstStart;

    size_t const flushSize = LZ4F_flush(cctxPtr, dstBuffer, dstCapacity, compressOptionsPtr);
    DEBUGLOG(5,"LZ4F_compressEnd: dstCapacity=%u", (unsigned)dstCapacity);
    if (LZ4F_isError(flushSize)) return flushSize;
    dstPtr += flushSize;

    assert(flushSize <= dstCapacity);
    dstCapacity -= flushSize;

    if (dstCapacity < 4) return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);
    LZ4F_writeLE32(dstPtr, 0);
    dstPtr += 4;   /* endMark */

    if (cctxPtr->prefs.frameInfo.contentChecksumFlag == LZ4F_contentChecksumEnabled) {
        U32 const xxh = XXH32_digest(&(cctxPtr->xxh));
        if (dstCapacity < 8) return err0r(LZ4F_ERROR_dstMaxSize_tooSmall);
        DEBUGLOG(5,"Writing 32-bit content checksum");
        LZ4F_writeLE32(dstPtr, xxh);
        dstPtr+=4;   /* content Checksum */
    }

    cctxPtr->cStage = 0;   /* state is now re-usable (with identical preferences) */
    cctxPtr->maxBufferSize = 0;  /* reuse HC context */

    if (cctxPtr->prefs.frameInfo.contentSize) {
        if (cctxPtr->prefs.frameInfo.contentSize != cctxPtr->totalInSize)
            return err0r(LZ4F_ERROR_frameSize_wrong);
    }

    return (size_t)(dstPtr - dstStart);
}


/*-***************************************************
*   Frame Decompression
*****************************************************/

typedef enum {
    dstage_getFrameHeader=0, dstage_storeFrameHeader,
    dstage_init,
    dstage_getBlockHeader, dstage_storeBlockHeader,
    dstage_copyDirect, dstage_getBlockChecksum,
    dstage_getCBlock, dstage_storeCBlock,
    dstage_flushOut,
    dstage_getSuffix, dstage_storeSuffix,
    dstage_getSFrameSize, dstage_storeSFrameSize,
    dstage_skipSkippable
} dStage_t;

struct LZ4F_dctx_s {
    LZ4F_frameInfo_t frameInfo;
    U32    version;
    dStage_t dStage;
    U64    frameRemainingSize;
    size_t maxBlockSize;
    size_t maxBufferSize;
    BYTE*  tmpIn;
    size_t tmpInSize;
    size_t tmpInTarget;
    BYTE*  tmpOutBuffer;
    const BYTE* dict;
    size_t dictSize;
    BYTE*  tmpOut;
    size_t tmpOutSize;
    size_t tmpOutStart;
    XXH32_state_t xxh;
    XXH32_state_t blockChecksum;
    BYTE   header[LZ4F_HEADER_SIZE_MAX];
};  /* typedef'd to LZ4F_dctx in lz4frame.h */


/*! LZ4F_createDecompressionContext() :
 *  Create a decompressionContext object, which will track all decompression operations.
 *  Provides a pointer to a fully allocated and initialized LZ4F_decompressionContext object.
 *  Object can later be released using LZ4F_freeDecompressionContext().
 * @return : if != 0, there was an error during context creation.
 */
LZ4F_errorCode_t LZ4F_createDecompressionContext(LZ4F_dctx** LZ4F_decompressionContextPtr, unsigned versionNumber)
{
    LZ4F_dctx* const dctx = (LZ4F_dctx*)ALLOC_AND_ZERO(sizeof(LZ4F_dctx));
    if (dctx == NULL) {  /* failed allocation */
        *LZ4F_decompressionContextPtr = NULL;
        return err0r(LZ4F_ERROR_allocation_failed);
    }

    dctx->version = versionNumber;
    *LZ4F_decompressionContextPtr = dctx;
    return LZ4F_OK_NoError;
}

LZ4F_errorCode_t LZ4F_freeDecompressionContext(LZ4F_dctx* dctx)
{
    LZ4F_errorCode_t result = LZ4F_OK_NoError;
    if (dctx != NULL) {   /* can accept NULL input, like free() */
      result = (LZ4F_errorCode_t)dctx->dStage;
      FREEMEM(dctx->tmpIn);
      FREEMEM(dctx->tmpOutBuffer);
      FREEMEM(dctx);
    }
    return result;
}


/*==---   Streaming Decompression operations   ---==*/

void LZ4F_resetDecompressionContext(LZ4F_dctx* dctx)
{
    dctx->dStage = dstage_getFrameHeader;
    dctx->dict = NULL;
    dctx->dictSize = 0;
}


/*! LZ4F_decodeHeader() :
 *  input   : `src` points at the **beginning of the frame**
 *  output  : set internal values of dctx, such as
 *            dctx->frameInfo and dctx->dStage.
 *            Also allocates internal buffers.
 *  @return : nb Bytes read from src (necessarily <= srcSize)
 *            or an error code (testable with LZ4F_isError())
 */
static size_t LZ4F_decodeHeader(LZ4F_dctx* dctx, const void* src, size_t srcSize)
{
    unsigned blockMode, blockChecksumFlag, contentSizeFlag, contentChecksumFlag, dictIDFlag, blockSizeID;
    size_t frameHeaderSize;
    const BYTE* srcPtr = (const BYTE*)src;

    DEBUGLOG(5, "LZ4F_decodeHeader");
    /* need to decode header to get frameInfo */
    if (srcSize < minFHSize) return err0r(LZ4F_ERROR_frameHeader_incomplete);   /* minimal frame header size */
    MEM_INIT(&(dctx->frameInfo), 0, sizeof(dctx->frameInfo));

    /* special case : skippable frames */
    if ((LZ4F_readLE32(srcPtr) & 0xFFFFFFF0U) == LZ4F_MAGIC_SKIPPABLE_START) {
        dctx->frameInfo.frameType = LZ4F_skippableFrame;
        if (src == (void*)(dctx->header)) {
            dctx->tmpInSize = srcSize;
            dctx->tmpInTarget = 8;
            dctx->dStage = dstage_storeSFrameSize;
            return srcSize;
        } else {
            dctx->dStage = dstage_getSFrameSize;
            return 4;
        }
    }

    /* control magic number */
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    if (LZ4F_readLE32(srcPtr) != LZ4F_MAGICNUMBER) {
        DEBUGLOG(4, "frame header error : unknown magic number");
        return err0r(LZ4F_ERROR_frameType_unknown);
    }
#endif
    dctx->frameInfo.frameType = LZ4F_frame;

    /* Flags */
    {   U32 const FLG = srcPtr[4];
        U32 const version = (FLG>>6) & _2BITS;
        blockChecksumFlag = (FLG>>4) & _1BIT;
        blockMode = (FLG>>5) & _1BIT;
        contentSizeFlag = (FLG>>3) & _1BIT;
        contentChecksumFlag = (FLG>>2) & _1BIT;
        dictIDFlag = FLG & _1BIT;
        /* validate */
        if (((FLG>>1)&_1BIT) != 0) return err0r(LZ4F_ERROR_reservedFlag_set); /* Reserved bit */
        if (version != 1) return err0r(LZ4F_ERROR_headerVersion_wrong);        /* Version Number, only supported value */
    }

    /* Frame Header Size */
    frameHeaderSize = minFHSize + (contentSizeFlag?8:0) + (dictIDFlag?4:0);

    if (srcSize < frameHeaderSize) {
        /* not enough input to fully decode frame header */
        if (srcPtr != dctx->header)
            memcpy(dctx->header, srcPtr, srcSize);
        dctx->tmpInSize = srcSize;
        dctx->tmpInTarget = frameHeaderSize;
        dctx->dStage = dstage_storeFrameHeader;
        return srcSize;
    }

    {   U32 const BD = srcPtr[5];
        blockSizeID = (BD>>4) & _3BITS;
        /* validate */
        if (((BD>>7)&_1BIT) != 0) return err0r(LZ4F_ERROR_reservedFlag_set);   /* Reserved bit */
        if (blockSizeID < 4) return err0r(LZ4F_ERROR_maxBlockSize_invalid);    /* 4-7 only supported values for the time being */
        if (((BD>>0)&_4BITS) != 0) return err0r(LZ4F_ERROR_reservedFlag_set);  /* Reserved bits */
    }

    /* check header */
    assert(frameHeaderSize > 5);
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    {   BYTE const HC = LZ4F_headerChecksum(srcPtr+4, frameHeaderSize-5);
        if (HC != srcPtr[frameHeaderSize-1])
            return err0r(LZ4F_ERROR_headerChecksum_invalid);
    }
#endif

    /* save */
    dctx->frameInfo.blockMode = (LZ4F_blockMode_t)blockMode;
    dctx->frameInfo.blockChecksumFlag = (LZ4F_blockChecksum_t)blockChecksumFlag;
    dctx->frameInfo.contentChecksumFlag = (LZ4F_contentChecksum_t)contentChecksumFlag;
    dctx->frameInfo.blockSizeID = (LZ4F_blockSizeID_t)blockSizeID;
    dctx->maxBlockSize = LZ4F_getBlockSize(blockSizeID);
    if (contentSizeFlag)
        dctx->frameRemainingSize =
            dctx->frameInfo.contentSize = LZ4F_readLE64(srcPtr+6);
    if (dictIDFlag)
        dctx->frameInfo.dictID = LZ4F_readLE32(srcPtr + frameHeaderSize - 5);

    dctx->dStage = dstage_init;

    return frameHeaderSize;
}


/*! LZ4F_headerSize() :
 * @return : size of frame header
 *           or an error code, which can be tested using LZ4F_isError()
 */
size_t LZ4F_headerSize(const void* src, size_t srcSize)
{
    if (src == NULL) return err0r(LZ4F_ERROR_srcPtr_wrong);

    /* minimal srcSize to determine header size */
    if (srcSize < LZ4F_MIN_SIZE_TO_KNOW_HEADER_LENGTH)
        return err0r(LZ4F_ERROR_frameHeader_incomplete);

    /* special case : skippable frames */
    if ((LZ4F_readLE32(src) & 0xFFFFFFF0U) == LZ4F_MAGIC_SKIPPABLE_START)
        return 8;

    /* control magic number */
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    if (LZ4F_readLE32(src) != LZ4F_MAGICNUMBER)
        return err0r(LZ4F_ERROR_frameType_unknown);
#endif

    /* Frame Header Size */
    {   BYTE const FLG = ((const BYTE*)src)[4];
        U32 const contentSizeFlag = (FLG>>3) & _1BIT;
        U32 const dictIDFlag = FLG & _1BIT;
        return minFHSize + (contentSizeFlag?8:0) + (dictIDFlag?4:0);
    }
}

/*! LZ4F_getFrameInfo() :
 *  This function extracts frame parameters (max blockSize, frame checksum, etc.).
 *  Usage is optional. Objective is to provide relevant information for allocation purposes.
 *  This function works in 2 situations :
 *   - At the beginning of a new frame, in which case it will decode this information from `srcBuffer`, and start the decoding process.
 *     Amount of input data provided must be large enough to successfully decode the frame header.
 *     A header size is variable, but is guaranteed to be <= LZ4F_HEADER_SIZE_MAX bytes. It's possible to provide more input data than this minimum.
 *   - After decoding has been started. In which case, no input is read, frame parameters are extracted from dctx.
 *  The number of bytes consumed from srcBuffer will be updated within *srcSizePtr (necessarily <= original value).
 *  Decompression must resume from (srcBuffer + *srcSizePtr).
 * @return : an hint about how many srcSize bytes LZ4F_decompress() expects for next call,
 *           or an error code which can be tested using LZ4F_isError()
 *  note 1 : in case of error, dctx is not modified. Decoding operations can resume from where they stopped.
 *  note 2 : frame parameters are *copied into* an already allocated LZ4F_frameInfo_t structure.
 */
LZ4F_errorCode_t LZ4F_getFrameInfo(LZ4F_dctx* dctx,
                                   LZ4F_frameInfo_t* frameInfoPtr,
                             const void* srcBuffer, size_t* srcSizePtr)
{
    LZ4F_STATIC_ASSERT(dstage_getFrameHeader < dstage_storeFrameHeader);
    if (dctx->dStage > dstage_storeFrameHeader) {
        /* frameInfo already decoded */
        size_t o=0, i=0;
        *srcSizePtr = 0;
        *frameInfoPtr = dctx->frameInfo;
        /* returns : recommended nb of bytes for LZ4F_decompress() */
        return LZ4F_decompress(dctx, NULL, &o, NULL, &i, NULL);
    } else {
        if (dctx->dStage == dstage_storeFrameHeader) {
            /* frame decoding already started, in the middle of header => automatic fail */
            *srcSizePtr = 0;
            return err0r(LZ4F_ERROR_frameDecoding_alreadyStarted);
        } else {
            size_t const hSize = LZ4F_headerSize(srcBuffer, *srcSizePtr);
            if (LZ4F_isError(hSize)) { *srcSizePtr=0; return hSize; }
            if (*srcSizePtr < hSize) {
                *srcSizePtr=0;
                return err0r(LZ4F_ERROR_frameHeader_incomplete);
            }

            {   size_t decodeResult = LZ4F_decodeHeader(dctx, srcBuffer, hSize);
                if (LZ4F_isError(decodeResult)) {
                    *srcSizePtr = 0;
                } else {
                    *srcSizePtr = decodeResult;
                    decodeResult = BHSize;   /* block header size */
                }
                *frameInfoPtr = dctx->frameInfo;
                return decodeResult;
    }   }   }
}


/* LZ4F_updateDict() :
 * only used for LZ4F_blockLinked mode
 * Condition : dstPtr != NULL
 */
static void LZ4F_updateDict(LZ4F_dctx* dctx,
                      const BYTE* dstPtr, size_t dstSize, const BYTE* dstBufferStart,
                      unsigned withinTmp)
{
    assert(dstPtr != NULL);
    if (dctx->dictSize==0) {
        dctx->dict = (const BYTE*)dstPtr;   /* priority to prefix mode */
    }
    assert(dctx->dict != NULL);

    if (dctx->dict + dctx->dictSize == dstPtr) {  /* prefix mode, everything within dstBuffer */
        dctx->dictSize += dstSize;
        return;
    }

    assert(dstPtr >= dstBufferStart);
    if ((size_t)(dstPtr - dstBufferStart) + dstSize >= 64 KB) {  /* history in dstBuffer becomes large enough to become dictionary */
        dctx->dict = (const BYTE*)dstBufferStart;
        dctx->dictSize = (size_t)(dstPtr - dstBufferStart) + dstSize;
        return;
    }

    assert(dstSize < 64 KB);   /* if dstSize >= 64 KB, dictionary would be set into dstBuffer directly */

    /* dstBuffer does not contain whole useful history (64 KB), so it must be saved within tmpOutBuffer */
    assert(dctx->tmpOutBuffer != NULL);

    if (withinTmp && (dctx->dict == dctx->tmpOutBuffer)) {   /* continue history within tmpOutBuffer */
        /* withinTmp expectation : content of [dstPtr,dstSize] is same as [dict+dictSize,dstSize], so we just extend it */
        assert(dctx->dict + dctx->dictSize == dctx->tmpOut + dctx->tmpOutStart);
        dctx->dictSize += dstSize;
        return;
    }

    if (withinTmp) { /* copy relevant dict portion in front of tmpOut within tmpOutBuffer */
        size_t const preserveSize = (size_t)(dctx->tmpOut - dctx->tmpOutBuffer);
        size_t copySize = 64 KB - dctx->tmpOutSize;
        const BYTE* const oldDictEnd = dctx->dict + dctx->dictSize - dctx->tmpOutStart;
        if (dctx->tmpOutSize > 64 KB) copySize = 0;
        if (copySize > preserveSize) copySize = preserveSize;

        memcpy(dctx->tmpOutBuffer + preserveSize - copySize, oldDictEnd - copySize, copySize);

        dctx->dict = dctx->tmpOutBuffer;
        dctx->dictSize = preserveSize + dctx->tmpOutStart + dstSize;
        return;
    }

    if (dctx->dict == dctx->tmpOutBuffer) {    /* copy dst into tmp to complete dict */
        if (dctx->dictSize + dstSize > dctx->maxBufferSize) {  /* tmp buffer not large enough */
            size_t const preserveSize = 64 KB - dstSize;
            memcpy(dctx->tmpOutBuffer, dctx->dict + dctx->dictSize - preserveSize, preserveSize);
            dctx->dictSize = preserveSize;
        }
        memcpy(dctx->tmpOutBuffer + dctx->dictSize, dstPtr, dstSize);
        dctx->dictSize += dstSize;
        return;
    }

    /* join dict & dest into tmp */
    {   size_t preserveSize = 64 KB - dstSize;
        if (preserveSize > dctx->dictSize) preserveSize = dctx->dictSize;
        memcpy(dctx->tmpOutBuffer, dctx->dict + dctx->dictSize - preserveSize, preserveSize);
        memcpy(dctx->tmpOutBuffer + preserveSize, dstPtr, dstSize);
        dctx->dict = dctx->tmpOutBuffer;
        dctx->dictSize = preserveSize + dstSize;
    }
}



/*! LZ4F_decompress() :
 *  Call this function repetitively to regenerate compressed data in srcBuffer.
 *  The function will attempt to decode up to *srcSizePtr bytes from srcBuffer
 *  into dstBuffer of capacity *dstSizePtr.
 *
 *  The number of bytes regenerated into dstBuffer will be provided within *dstSizePtr (necessarily <= original value).
 *
 *  The number of bytes effectively read from srcBuffer will be provided within *srcSizePtr (necessarily <= original value).
 *  If number of bytes read is < number of bytes provided, then decompression operation is not complete.
 *  Remaining data will have to be presented again in a subsequent invocation.
 *
 *  The function result is an hint of the better srcSize to use for next call to LZ4F_decompress.
 *  Schematically, it's the size of the current (or remaining) compressed block + header of next block.
 *  Respecting the hint provides a small boost to performance, since it allows less buffer shuffling.
 *  Note that this is just a hint, and it's always possible to any srcSize value.
 *  When a frame is fully decoded, @return will be 0.
 *  If decompression failed, @return is an error code which can be tested using LZ4F_isError().
 */
size_t LZ4F_decompress(LZ4F_dctx* dctx,
                       void* dstBuffer, size_t* dstSizePtr,
                       const void* srcBuffer, size_t* srcSizePtr,
                       const LZ4F_decompressOptions_t* decompressOptionsPtr)
{
    LZ4F_decompressOptions_t optionsNull;
    const BYTE* const srcStart = (const BYTE*)srcBuffer;
    const BYTE* const srcEnd = srcStart + *srcSizePtr;
    const BYTE* srcPtr = srcStart;
    BYTE* const dstStart = (BYTE*)dstBuffer;
    BYTE* const dstEnd = dstStart ? dstStart + *dstSizePtr : NULL;
    BYTE* dstPtr = dstStart;
    const BYTE* selectedIn = NULL;
    unsigned doAnotherStage = 1;
    size_t nextSrcSizeHint = 1;


    DEBUGLOG(5, "LZ4F_decompress : %p,%u => %p,%u",
            srcBuffer, (unsigned)*srcSizePtr, dstBuffer, (unsigned)*dstSizePtr);
    if (dstBuffer == NULL) assert(*dstSizePtr == 0);
    MEM_INIT(&optionsNull, 0, sizeof(optionsNull));
    if (decompressOptionsPtr==NULL) decompressOptionsPtr = &optionsNull;
    *srcSizePtr = 0;
    *dstSizePtr = 0;
    assert(dctx != NULL);

    /* behaves as a state machine */

    while (doAnotherStage) {

        switch(dctx->dStage)
        {

        case dstage_getFrameHeader:
            DEBUGLOG(6, "dstage_getFrameHeader");
            if ((size_t)(srcEnd-srcPtr) >= maxFHSize) {  /* enough to decode - shortcut */
                size_t const hSize = LZ4F_decodeHeader(dctx, srcPtr, (size_t)(srcEnd-srcPtr));  /* will update dStage appropriately */
                if (LZ4F_isError(hSize)) return hSize;
                srcPtr += hSize;
                break;
            }
            dctx->tmpInSize = 0;
            if (srcEnd-srcPtr == 0) return minFHSize;   /* 0-size input */
            dctx->tmpInTarget = minFHSize;   /* minimum size to decode header */
            dctx->dStage = dstage_storeFrameHeader;
            /* fall-through */

        case dstage_storeFrameHeader:
            DEBUGLOG(6, "dstage_storeFrameHeader");
            {   size_t const sizeToCopy = MIN(dctx->tmpInTarget - dctx->tmpInSize, (size_t)(srcEnd - srcPtr));
                memcpy(dctx->header + dctx->tmpInSize, srcPtr, sizeToCopy);
                dctx->tmpInSize += sizeToCopy;
                srcPtr += sizeToCopy;
            }
            if (dctx->tmpInSize < dctx->tmpInTarget) {
                nextSrcSizeHint = (dctx->tmpInTarget - dctx->tmpInSize) + BHSize;   /* rest of header + nextBlockHeader */
                doAnotherStage = 0;   /* not enough src data, ask for some more */
                break;
            }
            {   size_t const hSize = LZ4F_decodeHeader(dctx, dctx->header, dctx->tmpInTarget);  /* will update dStage appropriately */
                if (LZ4F_isError(hSize)) return hSize;
            }
            break;

        case dstage_init:
            DEBUGLOG(6, "dstage_init");
            if (dctx->frameInfo.contentChecksumFlag) (void)XXH32_reset(&(dctx->xxh), 0);
            /* internal buffers allocation */
            {   size_t const bufferNeeded = dctx->maxBlockSize
                    + ((dctx->frameInfo.blockMode==LZ4F_blockLinked) ? 128 KB : 0);
                if (bufferNeeded > dctx->maxBufferSize) {   /* tmp buffers too small */
                    dctx->maxBufferSize = 0;   /* ensure allocation will be re-attempted on next entry*/
                    FREEMEM(dctx->tmpIn);
                    dctx->tmpIn = (BYTE*)ALLOC(dctx->maxBlockSize + BFSize /* block checksum */);
                    if (dctx->tmpIn == NULL)
                        return err0r(LZ4F_ERROR_allocation_failed);
                    FREEMEM(dctx->tmpOutBuffer);
                    dctx->tmpOutBuffer= (BYTE*)ALLOC(bufferNeeded);
                    if (dctx->tmpOutBuffer== NULL)
                        return err0r(LZ4F_ERROR_allocation_failed);
                    dctx->maxBufferSize = bufferNeeded;
            }   }
            dctx->tmpInSize = 0;
            dctx->tmpInTarget = 0;
            dctx->tmpOut = dctx->tmpOutBuffer;
            dctx->tmpOutStart = 0;
            dctx->tmpOutSize = 0;

            dctx->dStage = dstage_getBlockHeader;
            /* fall-through */

        case dstage_getBlockHeader:
            if ((size_t)(srcEnd - srcPtr) >= BHSize) {
                selectedIn = srcPtr;
                srcPtr += BHSize;
            } else {
                /* not enough input to read cBlockSize field */
                dctx->tmpInSize = 0;
                dctx->dStage = dstage_storeBlockHeader;
            }

            if (dctx->dStage == dstage_storeBlockHeader)   /* can be skipped */
        case dstage_storeBlockHeader:
            {   size_t const remainingInput = (size_t)(srcEnd - srcPtr);
                size_t const wantedData = BHSize - dctx->tmpInSize;
                size_t const sizeToCopy = MIN(wantedData, remainingInput);
                memcpy(dctx->tmpIn + dctx->tmpInSize, srcPtr, sizeToCopy);
                srcPtr += sizeToCopy;
                dctx->tmpInSize += sizeToCopy;

                if (dctx->tmpInSize < BHSize) {   /* not enough input for cBlockSize */
                    nextSrcSizeHint = BHSize - dctx->tmpInSize;
                    doAnotherStage  = 0;
                    break;
                }
                selectedIn = dctx->tmpIn;
            }   /* if (dctx->dStage == dstage_storeBlockHeader) */

        /* decode block header */
            {   U32 const blockHeader = LZ4F_readLE32(selectedIn);
                size_t const nextCBlockSize = blockHeader & 0x7FFFFFFFU;
                size_t const crcSize = dctx->frameInfo.blockChecksumFlag * BFSize;
                if (blockHeader==0) {  /* frameEnd signal, no more block */
                    DEBUGLOG(5, "end of frame");
                    dctx->dStage = dstage_getSuffix;
                    break;
                }
                if (nextCBlockSize > dctx->maxBlockSize) {
                    return err0r(LZ4F_ERROR_maxBlockSize_invalid);
                }
                if (blockHeader & LZ4F_BLOCKUNCOMPRESSED_FLAG) {
                    /* next block is uncompressed */
                    dctx->tmpInTarget = nextCBlockSize;
                    DEBUGLOG(5, "next block is uncompressed (size %u)", (U32)nextCBlockSize);
                    if (dctx->frameInfo.blockChecksumFlag) {
                        (void)XXH32_reset(&dctx->blockChecksum, 0);
                    }
                    dctx->dStage = dstage_copyDirect;
                    break;
                }
                /* next block is a compressed block */
                dctx->tmpInTarget = nextCBlockSize + crcSize;
                dctx->dStage = dstage_getCBlock;
                if (dstPtr==dstEnd || srcPtr==srcEnd) {
                    nextSrcSizeHint = BHSize + nextCBlockSize + crcSize;
                    doAnotherStage = 0;
                }
                break;
            }

        case dstage_copyDirect:   /* uncompressed block */
            DEBUGLOG(6, "dstage_copyDirect");
            {   size_t sizeToCopy;
                if (dstPtr == NULL) {
                    sizeToCopy = 0;
                } else {
                    size_t const minBuffSize = MIN((size_t)(srcEnd-srcPtr), (size_t)(dstEnd-dstPtr));
                    sizeToCopy = MIN(dctx->tmpInTarget, minBuffSize);
                    memcpy(dstPtr, srcPtr, sizeToCopy);
                    if (dctx->frameInfo.blockChecksumFlag) {
                        (void)XXH32_update(&dctx->blockChecksum, srcPtr, sizeToCopy);
                    }
                    if (dctx->frameInfo.contentChecksumFlag)
                        (void)XXH32_update(&dctx->xxh, srcPtr, sizeToCopy);
                    if (dctx->frameInfo.contentSize)
                        dctx->frameRemainingSize -= sizeToCopy;

                    /* history management (linked blocks only)*/
                    if (dctx->frameInfo.blockMode == LZ4F_blockLinked) {
                        LZ4F_updateDict(dctx, dstPtr, sizeToCopy, dstStart, 0);
                }   }

                srcPtr += sizeToCopy;
                dstPtr += sizeToCopy;
                if (sizeToCopy == dctx->tmpInTarget) {   /* all done */
                    if (dctx->frameInfo.blockChecksumFlag) {
                        dctx->tmpInSize = 0;
                        dctx->dStage = dstage_getBlockChecksum;
                    } else
                        dctx->dStage = dstage_getBlockHeader;  /* new block */
                    break;
                }
                dctx->tmpInTarget -= sizeToCopy;  /* need to copy more */
            }
            nextSrcSizeHint = dctx->tmpInTarget +
                            +(dctx->frameInfo.blockChecksumFlag ? BFSize : 0)
                            + BHSize /* next header size */;
            doAnotherStage = 0;
            break;

        /* check block checksum for recently transferred uncompressed block */
        case dstage_getBlockChecksum:
            DEBUGLOG(6, "dstage_getBlockChecksum");
            {   const void* crcSrc;
                if ((srcEnd-srcPtr >= 4) && (dctx->tmpInSize==0)) {
                    crcSrc = srcPtr;
                    srcPtr += 4;
                } else {
                    size_t const stillToCopy = 4 - dctx->tmpInSize;
                    size_t const sizeToCopy = MIN(stillToCopy, (size_t)(srcEnd-srcPtr));
                    memcpy(dctx->header + dctx->tmpInSize, srcPtr, sizeToCopy);
                    dctx->tmpInSize += sizeToCopy;
                    srcPtr += sizeToCopy;
                    if (dctx->tmpInSize < 4) {  /* all input consumed */
                        doAnotherStage = 0;
                        break;
                    }
                    crcSrc = dctx->header;
                }
                {   U32 const readCRC = LZ4F_readLE32(crcSrc);
                    U32 const calcCRC = XXH32_digest(&dctx->blockChecksum);
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
                    DEBUGLOG(6, "compare block checksum");
                    if (readCRC != calcCRC) {
                        DEBUGLOG(4, "incorrect block checksum: %08X != %08X",
                                readCRC, calcCRC);
                        return err0r(LZ4F_ERROR_blockChecksum_invalid);
                    }
#else
                    (void)readCRC;
                    (void)calcCRC;
#endif
            }   }
            dctx->dStage = dstage_getBlockHeader;  /* new block */
            break;

        case dstage_getCBlock:
            DEBUGLOG(6, "dstage_getCBlock");
            if ((size_t)(srcEnd-srcPtr) < dctx->tmpInTarget) {
                dctx->tmpInSize = 0;
                dctx->dStage = dstage_storeCBlock;
                break;
            }
            /* input large enough to read full block directly */
            selectedIn = srcPtr;
            srcPtr += dctx->tmpInTarget;

            if (0)  /* always jump over next block */
        case dstage_storeCBlock:
            {   size_t const wantedData = dctx->tmpInTarget - dctx->tmpInSize;
                size_t const inputLeft = (size_t)(srcEnd-srcPtr);
                size_t const sizeToCopy = MIN(wantedData, inputLeft);
                memcpy(dctx->tmpIn + dctx->tmpInSize, srcPtr, sizeToCopy);
                dctx->tmpInSize += sizeToCopy;
                srcPtr += sizeToCopy;
                if (dctx->tmpInSize < dctx->tmpInTarget) { /* need more input */
                    nextSrcSizeHint = (dctx->tmpInTarget - dctx->tmpInSize)
                                    + (dctx->frameInfo.blockChecksumFlag ? BFSize : 0)
                                    + BHSize /* next header size */;
                    doAnotherStage = 0;
                    break;
                }
                selectedIn = dctx->tmpIn;
            }

            /* At this stage, input is large enough to decode a block */
            if (dctx->frameInfo.blockChecksumFlag) {
                dctx->tmpInTarget -= 4;
                assert(selectedIn != NULL);  /* selectedIn is defined at this stage (either srcPtr, or dctx->tmpIn) */
                {   U32 const readBlockCrc = LZ4F_readLE32(selectedIn + dctx->tmpInTarget);
                    U32 const calcBlockCrc = XXH32(selectedIn, dctx->tmpInTarget, 0);
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
                    if (readBlockCrc != calcBlockCrc)
                        return err0r(LZ4F_ERROR_blockChecksum_invalid);
#else
                    (void)readBlockCrc;
                    (void)calcBlockCrc;
#endif
            }   }

            if ((size_t)(dstEnd-dstPtr) >= dctx->maxBlockSize) {
                const char* dict = (const char*)dctx->dict;
                size_t dictSize = dctx->dictSize;
                int decodedSize;
                assert(dstPtr != NULL);
                if (dict && dictSize > 1 GB) {
                    /* the dictSize param is an int, avoid truncation / sign issues */
                    dict += dictSize - 64 KB;
                    dictSize = 64 KB;
                }
                /* enough capacity in `dst` to decompress directly there */
                decodedSize = LZ4_decompress_safe_usingDict(
                        (const char*)selectedIn, (char*)dstPtr,
                        (int)dctx->tmpInTarget, (int)dctx->maxBlockSize,
                        dict, (int)dictSize);
                if (decodedSize < 0) return err0r(LZ4F_ERROR_GENERIC);   /* decompression failed */
                if (dctx->frameInfo.contentChecksumFlag)
                    XXH32_update(&(dctx->xxh), dstPtr, (size_t)decodedSize);
                if (dctx->frameInfo.contentSize)
                    dctx->frameRemainingSize -= (size_t)decodedSize;

                /* dictionary management */
                if (dctx->frameInfo.blockMode==LZ4F_blockLinked) {
                    LZ4F_updateDict(dctx, dstPtr, (size_t)decodedSize, dstStart, 0);
                }

                dstPtr += decodedSize;
                dctx->dStage = dstage_getBlockHeader;
                break;
            }

            /* not enough place into dst : decode into tmpOut */
            /* ensure enough place for tmpOut */
            if (dctx->frameInfo.blockMode == LZ4F_blockLinked) {
                if (dctx->dict == dctx->tmpOutBuffer) {
                    if (dctx->dictSize > 128 KB) {
                        memcpy(dctx->tmpOutBuffer, dctx->dict + dctx->dictSize - 64 KB, 64 KB);
                        dctx->dictSize = 64 KB;
                    }
                    dctx->tmpOut = dctx->tmpOutBuffer + dctx->dictSize;
                } else {  /* dict not within tmp */
                    size_t const reservedDictSpace = MIN(dctx->dictSize, 64 KB);
                    dctx->tmpOut = dctx->tmpOutBuffer + reservedDictSpace;
            }   }

            /* Decode block */
            {   const char* dict = (const char*)dctx->dict;
                size_t dictSize = dctx->dictSize;
                int decodedSize;
                if (dict && dictSize > 1 GB) {
                    /* the dictSize param is an int, avoid truncation / sign issues */
                    dict += dictSize - 64 KB;
                    dictSize = 64 KB;
                }
                decodedSize = LZ4_decompress_safe_usingDict(
                        (const char*)selectedIn, (char*)dctx->tmpOut,
                        (int)dctx->tmpInTarget, (int)dctx->maxBlockSize,
                        dict, (int)dictSize);
                if (decodedSize < 0)  /* decompression failed */
                    return err0r(LZ4F_ERROR_decompressionFailed);
                if (dctx->frameInfo.contentChecksumFlag)
                    XXH32_update(&(dctx->xxh), dctx->tmpOut, (size_t)decodedSize);
                if (dctx->frameInfo.contentSize)
                    dctx->frameRemainingSize -= (size_t)decodedSize;
                dctx->tmpOutSize = (size_t)decodedSize;
                dctx->tmpOutStart = 0;
                dctx->dStage = dstage_flushOut;
            }
            /* fall-through */

        case dstage_flushOut:  /* flush decoded data from tmpOut to dstBuffer */
            DEBUGLOG(6, "dstage_flushOut");
            if (dstPtr != NULL) {
                size_t const sizeToCopy = MIN(dctx->tmpOutSize - dctx->tmpOutStart, (size_t)(dstEnd-dstPtr));
                memcpy(dstPtr, dctx->tmpOut + dctx->tmpOutStart, sizeToCopy);

                /* dictionary management */
                if (dctx->frameInfo.blockMode == LZ4F_blockLinked)
                    LZ4F_updateDict(dctx, dstPtr, sizeToCopy, dstStart, 1 /*withinTmp*/);

                dctx->tmpOutStart += sizeToCopy;
                dstPtr += sizeToCopy;
            }
            if (dctx->tmpOutStart == dctx->tmpOutSize) { /* all flushed */
                dctx->dStage = dstage_getBlockHeader;  /* get next block */
                break;
            }
            /* could not flush everything : stop there, just request a block header */
            doAnotherStage = 0;
            nextSrcSizeHint = BHSize;
            break;

        case dstage_getSuffix:
            if (dctx->frameRemainingSize)
                return err0r(LZ4F_ERROR_frameSize_wrong);   /* incorrect frame size decoded */
            if (!dctx->frameInfo.contentChecksumFlag) {  /* no checksum, frame is completed */
                nextSrcSizeHint = 0;
                LZ4F_resetDecompressionContext(dctx);
                doAnotherStage = 0;
                break;
            }
            if ((srcEnd - srcPtr) < 4) {  /* not enough size for entire CRC */
                dctx->tmpInSize = 0;
                dctx->dStage = dstage_storeSuffix;
            } else {
                selectedIn = srcPtr;
                srcPtr += 4;
            }

            if (dctx->dStage == dstage_storeSuffix)   /* can be skipped */
        case dstage_storeSuffix:
            {   size_t const remainingInput = (size_t)(srcEnd - srcPtr);
                size_t const wantedData = 4 - dctx->tmpInSize;
                size_t const sizeToCopy = MIN(wantedData, remainingInput);
                memcpy(dctx->tmpIn + dctx->tmpInSize, srcPtr, sizeToCopy);
                srcPtr += sizeToCopy;
                dctx->tmpInSize += sizeToCopy;
                if (dctx->tmpInSize < 4) { /* not enough input to read complete suffix */
                    nextSrcSizeHint = 4 - dctx->tmpInSize;
                    doAnotherStage=0;
                    break;
                }
                selectedIn = dctx->tmpIn;
            }   /* if (dctx->dStage == dstage_storeSuffix) */

        /* case dstage_checkSuffix: */   /* no direct entry, avoid initialization risks */
            {   U32 const readCRC = LZ4F_readLE32(selectedIn);
                U32 const resultCRC = XXH32_digest(&(dctx->xxh));
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
                if (readCRC != resultCRC)
                    return err0r(LZ4F_ERROR_contentChecksum_invalid);
#else
                (void)readCRC;
                (void)resultCRC;
#endif
                nextSrcSizeHint = 0;
                LZ4F_resetDecompressionContext(dctx);
                doAnotherStage = 0;
                break;
            }

        case dstage_getSFrameSize:
            if ((srcEnd - srcPtr) >= 4) {
                selectedIn = srcPtr;
                srcPtr += 4;
            } else {
                /* not enough input to read cBlockSize field */
                dctx->tmpInSize = 4;
                dctx->tmpInTarget = 8;
                dctx->dStage = dstage_storeSFrameSize;
            }

            if (dctx->dStage == dstage_storeSFrameSize)
        case dstage_storeSFrameSize:
            {   size_t const sizeToCopy = MIN(dctx->tmpInTarget - dctx->tmpInSize,
                                             (size_t)(srcEnd - srcPtr) );
                memcpy(dctx->header + dctx->tmpInSize, srcPtr, sizeToCopy);
                srcPtr += sizeToCopy;
                dctx->tmpInSize += sizeToCopy;
                if (dctx->tmpInSize < dctx->tmpInTarget) {
                    /* not enough input to get full sBlockSize; wait for more */
                    nextSrcSizeHint = dctx->tmpInTarget - dctx->tmpInSize;
                    doAnotherStage = 0;
                    break;
                }
                selectedIn = dctx->header + 4;
            }   /* if (dctx->dStage == dstage_storeSFrameSize) */

        /* case dstage_decodeSFrameSize: */   /* no direct entry */
            {   size_t const SFrameSize = LZ4F_readLE32(selectedIn);
                dctx->frameInfo.contentSize = SFrameSize;
                dctx->tmpInTarget = SFrameSize;
                dctx->dStage = dstage_skipSkippable;
                break;
            }

        case dstage_skipSkippable:
            {   size_t const skipSize = MIN(dctx->tmpInTarget, (size_t)(srcEnd-srcPtr));
                srcPtr += skipSize;
                dctx->tmpInTarget -= skipSize;
                doAnotherStage = 0;
                nextSrcSizeHint = dctx->tmpInTarget;
                if (nextSrcSizeHint) break;  /* still more to skip */
                /* frame fully skipped : prepare context for a new frame */
                LZ4F_resetDecompressionContext(dctx);
                break;
            }
        }   /* switch (dctx->dStage) */
    }   /* while (doAnotherStage) */

    /* preserve history within tmp whenever necessary */
    LZ4F_STATIC_ASSERT((unsigned)dstage_init == 2);
    if ( (dctx->frameInfo.blockMode==LZ4F_blockLinked)  /* next block will use up to 64KB from previous ones */
      && (dctx->dict != dctx->tmpOutBuffer)             /* dictionary is not already within tmp */
      && (dctx->dict != NULL)                           /* dictionary exists */
      && (!decompressOptionsPtr->stableDst)             /* cannot rely on dst data to remain there for next call */
      && ((unsigned)(dctx->dStage)-2 < (unsigned)(dstage_getSuffix)-2) )  /* valid stages : [init ... getSuffix[ */
    {
        if (dctx->dStage == dstage_flushOut) {
            size_t const preserveSize = (size_t)(dctx->tmpOut - dctx->tmpOutBuffer);
            size_t copySize = 64 KB - dctx->tmpOutSize;
            const BYTE* oldDictEnd = dctx->dict + dctx->dictSize - dctx->tmpOutStart;
            if (dctx->tmpOutSize > 64 KB) copySize = 0;
            if (copySize > preserveSize) copySize = preserveSize;
            assert(dctx->tmpOutBuffer != NULL);

            memcpy(dctx->tmpOutBuffer + preserveSize - copySize, oldDictEnd - copySize, copySize);

            dctx->dict = dctx->tmpOutBuffer;
            dctx->dictSize = preserveSize + dctx->tmpOutStart;
        } else {
            const BYTE* const oldDictEnd = dctx->dict + dctx->dictSize;
            size_t const newDictSize = MIN(dctx->dictSize, 64 KB);

            memcpy(dctx->tmpOutBuffer, oldDictEnd - newDictSize, newDictSize);

            dctx->dict = dctx->tmpOutBuffer;
            dctx->dictSize = newDictSize;
            dctx->tmpOut = dctx->tmpOutBuffer + newDictSize;
        }
    }

    *srcSizePtr = (size_t)(srcPtr - srcStart);
    *dstSizePtr = (size_t)(dstPtr - dstStart);
    return nextSrcSizeHint;
}

/*! LZ4F_decompress_usingDict() :
 *  Same as LZ4F_decompress(), using a predefined dictionary.
 *  Dictionary is used "in place", without any preprocessing.
 *  It must remain accessible throughout the entire frame decoding.
 */
size_t LZ4F_decompress_usingDict(LZ4F_dctx* dctx,
                       void* dstBuffer, size_t* dstSizePtr,
                       const void* srcBuffer, size_t* srcSizePtr,
                       const void* dict, size_t dictSize,
                       const LZ4F_decompressOptions_t* decompressOptionsPtr)
{
    if (dctx->dStage <= dstage_init) {
        dctx->dict = (const BYTE*)dict;
        dctx->dictSize = dictSize;
    }
    return LZ4F_decompress(dctx, dstBuffer, dstSizePtr,
                           srcBuffer, srcSizePtr,
                           decompressOptionsPtr);
}
