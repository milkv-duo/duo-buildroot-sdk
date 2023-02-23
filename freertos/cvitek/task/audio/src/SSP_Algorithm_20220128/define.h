
#ifndef SHARON_DEFINE_H
#define SHARON_DEFINE_H

/* Include files */
#include <math.h>


/*  Commonly Used Macro */
#define IN_RANGE(n, lo, hi)   ((lo) <= (n) && (n) <= (hi))
#define ABS(t)                              (((t) < 0 ) ? -(t) : (t))
#define MAX(x, y)                       (((x) < (y)) ? (y) : (x))
#define MIN(x, y)                        (((x) > (y)) ? (y) : (x))
#define ROUND_POS(x)           (short) (((float)(x)) + 0.5F)
#define ROUND_NEG(x)           (short) (((float)(x)) - 0.5F)
#define SQR(x)                              ((x)*(x))

/* Fixed Definitions */
#define PLUS_1dB                       (int)(1.1220f * 32768)     // Q.15 format
#define MINUS_quarterdB      (int)(0.9716f * 32768)     // Q.15 format
#define MINUS_halfdB             (int)(0.944f * 32768)        // Q.15 format
#define MINUS_1dB                   (int)(0.8912f * 32768)     // Q.15 format
#define MINUS_2dB                   (int)(0.7943f * 32768)     // Q.15 format

/* Math Approximation */
#define math_sqrt sqrt
#define math_acos acos
#define math_exp exp
#define math_cos_norm(x) (cos((.5f*M_PI)*(x)))
#define math_atan atan
#ifndef M_PI
#define M_PI           3.14159265358979323846    /* pi */
#endif

#define WORD2INT(x) ((x) < -32767.5f ? -32768 : ((x) > 32766.5f ? 32767 : floor(.5+(x))))
#define toBARK(n)   (13.1f*atan(.00074f*(n))+2.24f*atan((n)*(n)*1.85e-8f)+1e-4f*(n))
#define toMEL(n)    (2595.f*log10(1.f+(n)/700.f))

/* Floating Point */
#define FLOAT_ZERO 0.f
#define FLOAT_ONE 1.f
#define FLOAT_HALF 0.5f
#define FP_VERY_SMALL 1e-15f
#define FP_VERY_LARGE 1e15f
#define AES_SNR_SCALING 1.f
#define AES_FRAC_SCALING 1.f
#define AES_EXPIN_SCALING 1.f

/* We don't support visibility on Win32 */
#define EXPORT

/* Microsoft version of 'inline' */
#ifdef PLATFORM_RTOS
#ifndef inline
#define inline __inline
#endif
#endif


#ifndef NULL
#define NULL 0
#endif

#if defined(_WIN32)
     /* MSVC/Borland */
     typedef __int32 int32;
     typedef unsigned __int32 uint32;
     typedef __int16 int16;
     typedef unsigned __int16 uint16;
#else
    typedef int int32;
    typedef unsigned int uint32;
    typedef short int16;
    typedef unsigned short uint16;
#endif

#endif


