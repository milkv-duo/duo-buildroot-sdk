/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_math.h
 * Description:
 *   mathematical functions.
 */

#ifndef __CVI_MATH_H__
#define __CVI_MATH_H__

#include <cvi_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PI 3.1415926

/******************************************************************************
 * ABS(x)                 absolute value of x
 * SIGN(x)                sign of x
 * CMP(x,y)               0 if x==y; 1 if x>y; -1 if x<y
 *****************************************************************************/
#undef ABS
#define ABS(x) ((x) >= 0 ? (x) : (-(x)))
#undef _SIGN
#define _SIGN(x) ((x) >= 0 ? 1 : -1)
#undef CMP
#define CMP(x, y) (((x) == (y)) ? 0 : (((x) > (y)) ? 1 : -1))

/******************************************************************************
 * MAX2(x,y)              maximum of x and y
 * MIN2(x,y)              minimum of x and y
 * MAX3(x,y,z)            maximum of x, y and z
 * MIN3(x,y,z)            minimun of x, y and z
 * MEDIAN(x,y,z)          median of x,y,z
 * MEAN2(x,y)             mean of x,y
 *****************************************************************************/
#undef MAX2
#define MAX2(x, y) ((x) > (y) ? (x) : (y))
#undef MIN2
#define MIN2(x, y) ((x) < (y) ? (x) : (y))
#undef MAX3
#define MAX3(x, y, z) ((x) > (y) ? MAX2(x, z) : MAX2(y, z))
#undef MIN3
#define MIN3(x, y, z) ((x) < (y) ? MIN2(x, z) : MIN2(y, z))
#undef MEDIAN
#define MEDIAN(x, y, z) (((x) + (y) + (z)-MAX3(x, y, z)) - MIN3(x, y, z))
#undef MEAN2
#define MEAN2(x, y) (((x) + (y)) >> 1)

/******************************************************************************
 * CLIP3(x,min,max)       clip x within [min,max]
 * WRAP_MAX(x,max,min)    wrap to min if x equal max
 * WRAP_MIN(x,min,max)    wrap to max if x equal min
 * VALUE_BETWEEN(x,min.max)   True if x is between [min,max] inclusively.
 *****************************************************************************/
#undef CLIP_MIN
#define CLIP_MIN(x, min) (((x) >= min) ? (x) : min)
#undef CLIP3
#define CLIP3(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#undef CLIP_MAX
#define CLIP_MAX(x, max) ((x) > (max) ? (max) : (x))
#undef WRAP_MAX
#define WRAP_MAX(x, max, min) ((x) >= (max) ? (min) : (x))
#undef WRAP_MIN
#define WRAP_MIN(x, min, max) ((x) <= (min) ? (max) : (x))
#undef VALUE_BETWEEN
#define VALUE_BETWEEN(x, min, max) (((x) >= (min)) && ((x) <= (max)))

/******************************************************************************
 * a is a power of 2 value
 *
 * Example:
 * ALIGN(48,32) = 64
 * ALIGN_DOWN(48,32) = 32
 *****************************************************************************/
#undef IS_ALIGNED
#define IS_ALIGNED(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)
#undef ALIGN
#define ALIGN(x, a)      (((x) + ((a)-1)) & ~((a)-1))
#undef ALIGN_DOWN
#define ALIGN_DOWN(x, a) ((x) & ~((a)-1))
#undef DIV_UP
#define DIV_UP(x, a)     (((x) + ((a)-1)) / a)

/******************************************************************************
 * Get the span between two unsigned number, such as
 * SPAN(CVI_U32, 100, 200) is 200 - 100 = 100
 * SPAN(CVI_U32, 200, 100) is 0xFFFFFFFF - 200 + 100
 * SPAN(CVI_U64, 200, 100) is 0xFFFFFFFFFFFFFFFF - 200 + 100
 *****************************************************************************/
#undef SPAN
#define SPAN(type, begin, end)                                                                                         \
	({                                                                                                             \
		type b = (begin);                                                                                      \
		type e = (end);                                                                                        \
		(type)((b >= e) ? (b - e) : (b + ((~((type)0)) - e)));                                                 \
	})

/******************************************************************************
 * FRACTION32(de,nu)          fraction: nu(minator) / de(nominator).
 * NUMERATOR32(x)              of x(x is fraction)
 * DENOMINATOR32(x)           Denominator of x(x is fraction)

 * represent fraction in 32 bit. LSB 16 is numerator, MSB 16 is denominator
 * It is integer if denominator is 0.
 *****************************************************************************/
#undef FRACTION32
#define FRACTION32(de, nu) (((de) << 16) | (nu))
#undef NUMERATOR32
#define NUMERATOR32(x) ((x)&0xffff)
#undef DENOMINATOR32
#define DENOMINATOR32(x) ((x) >> 16)

/******************************************************************************
 * RGB(r,g,b)    assemble the r,g,b to 30bit color
 * RGB_R(c)      get RED   from 30bit color
 * RGB_G(c)      get GREEN from 30bit color
 * RGB_B(c)      get BLUE  from 30bit color
 *****************************************************************************/
#undef RGB
#define RGB(r, g, b) ((((r)&0x3ff) << 20) | (((g)&0x3ff) << 10) | ((b)&0x3ff))
#undef RGB_R
#define RGB_R(c) (((c)&0x3ff00000) >> 20)
#undef RGB_G
#define RGB_G(c) (((c)&0x000ffc00) >> 10)
#undef RGB_B
#define RGB_B(c) ((c)&0x000003ff)

/******************************************************************************
 * RGB(r,g,b)    assemble the r,g,b to 24bit color
 * RGB_R(c)      get RED   from 24bit color
 * RGB_G(c)      get GREEN from 24bit color
 * RGB_B(c)      get BLUE  from 24bit color
 *****************************************************************************/
#undef RGB_8BIT
#define RGB_8BIT(r, g, b) ((((r)&0xff) << 16) | (((g)&0xff) << 8) | ((b)&0xff))
#undef RGB_8BIT_R
#define RGB_8BIT_R(c) (((c)&0xff0000) >> 16)
#undef RGB_8BIT_G
#define RGB_8BIT_G(c) (((c)&0xff00) >> 8)
#undef RGB_8BIT_B
#define RGB_8BIT_B(c) ((c)&0xff)

/******************************************************************************
 * YUV(y,u,v)    assemble the y,u,v to 30bit color
 * YUV_Y(c)      get Y from 30bit color
 * YUV_U(c)      get U from 30bit color
 * YUV_V(c)      get V from 30bit color
 *****************************************************************************/
#undef YUV
#define YUV(y, u, v) ((((y)&0x03ff) << 20) | (((u)&0x03ff) << 10) | ((v)&0x03ff))
#undef YUV_Y
#define YUV_Y(c) (((c)&0x3ff00000) >> 20)
#undef YUV_U
#define YUV_U(c) (((c)&0x000ffc00) >> 10)
#undef YUV_V
#define YUV_V(c) ((c)&0x000003ff)

/******************************************************************************
 * YUV_8BIT(y,u,v)    assemble the y,u,v to 24bit color
 * YUV_8BIT_Y(c)      get Y from 24bit color
 * YUV_8BIT_U(c)      get U from 24bit color
 * YUV_8BIT_V(c)      get V from 24bit color
 *****************************************************************************/
#undef YUV_8BIT
#define YUV_8BIT(y, u, v) ((((y)&0xff) << 16) | (((u)&0xff) << 8) | ((v)&0xff))
#undef YUV_8BIT_Y
#define YUV_8BIT_Y(c) (((c)&0xff0000) >> 16)
#undef YUV_8BIT_U
#define YUV_8BIT_U(c) (((c)&0xff00) >> 8)
#undef YUV_8BIT_V
#define YUV_8BIT_V(c) ((c)&0xff)

/******************************************************************************
 * Rgb2Yc(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
 * Rgb2Yuv(rgb)             convert rgb to yuv
 *****************************************************************************/
static inline CVI_VOID Rgb2Yc(CVI_U16 r, CVI_U16 g, CVI_U16 b, CVI_U16 *py, CVI_U16 *pcb, CVI_U16 *pcr)
{
	/* Y */
	*py = (CVI_U16)((((r * 66 + g * 129 + b * 25) >> 8) + 16) << 2);

	/* Cb */
	*pcb = (CVI_U16)(((((b * 112 - r * 38) - g * 74) >> 8) + 128) << 2);

	/* Cr */
	*pcr = (CVI_U16)(((((r * 112 - g * 94) - b * 18) >> 8) + 128) << 2);
}

static inline CVI_U32 Rgb2Yuv(CVI_U32 u32Rgb)
{
	CVI_U16 y, u, v;

	Rgb2Yc(RGB_R(u32Rgb), RGB_G(u32Rgb), RGB_B(u32Rgb), &y, &u, &v);

	return YUV(y, u, v);
}

static inline CVI_VOID Rgb2Yc_full(CVI_U16 r, CVI_U16 g, CVI_U16 b, CVI_U16 *py, CVI_U16 *pcb, CVI_U16 *pcr)
{
	CVI_U16 py_temp, pcb_temp, pcr_temp;

	py_temp = (CVI_U16)(((r * 76 + g * 150 + b * 29) >> 8) * 4);
	pcb_temp = (CVI_U16)(CLIP_MIN(((((b * 130 - r * 44) - g * 86) >> 8) + 128), 0) * 4);
	pcr_temp = (CVI_U16)(CLIP_MIN(((((r * 130 - g * 109) - b * 21) >> 8) + 128), 0) * 4);

	*py = MAX2(MIN2(py_temp, 1023), 0);
	*pcb = MAX2(MIN2(pcb_temp, 1023), 0);
	*pcr = MAX2(MIN2(pcr_temp, 1023), 0);
}

static inline CVI_U32 Rgb2Yuv_full(CVI_U32 u32Rgb)
{
	CVI_U16 y, u, v;

	Rgb2Yc_full(RGB_R(u32Rgb), RGB_G(u32Rgb), RGB_B(u32Rgb), &y, &u, &v);

	return YUV(y, u, v);
}

/******************************************************************************
 * Rgb2Yc_8BIT(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
 * Rgb2Yuv_8BIT(rgb)                   convert rgb to yuv
 *****************************************************************************/
static inline CVI_VOID Rgb2Yc_8BIT(CVI_U8 r, CVI_U8 g, CVI_U8 b, CVI_U8 *py, CVI_U8 *pcb, CVI_U8 *pcr)
{
	/* Y */
	*py = (CVI_U8)(((r * 66 + g * 129 + b * 25) >> 8) + 16);

	/* Cb */
	*pcb = (CVI_U8)((((b * 112 - r * 38) - g * 74) >> 8) + 128);

	/* Cr */
	*pcr = (CVI_U8)((((r * 112 - g * 94) - b * 18) >> 8) + 128);
}

static inline CVI_U32 Rgb2Yuv_8BIT(CVI_U32 u32Rgb)
{
	CVI_U8 y, u, v;

	Rgb2Yc_8BIT(RGB_8BIT_R(u32Rgb), RGB_8BIT_G(u32Rgb), RGB_8BIT_B(u32Rgb), &y, &u, &v);

	return YUV_8BIT(y, u, v);
}

static inline CVI_VOID Rgb2Yc_full_8BIT(CVI_U8 r, CVI_U8 g, CVI_U8 b, CVI_U8 *py, CVI_U8 *pcb, CVI_U8 *pcr)
{
	CVI_S16 py_temp, pcb_temp, pcr_temp;

	py_temp = (r * 76 + g * 150 + b * 29) >> 8;
	pcb_temp = (((b * 130 - r * 44) - g * 86) >> 8) + 128;
	pcr_temp = (((r * 130 - g * 109) - b * 21) >> 8) + 128;

	*py = MAX2(MIN2(py_temp, 255), 0);
	*pcb = MAX2(MIN2(pcb_temp, 255), 0);
	*pcr = MAX2(MIN2(pcr_temp, 255), 0);
}

static inline CVI_U32 Rgb2Yuv_full_8BIT(CVI_U32 u32Rgb)
{
	CVI_U8 y, u, v;

	Rgb2Yc_full_8BIT(RGB_8BIT_R(u32Rgb), RGB_8BIT_G(u32Rgb), RGB_8BIT_B(u32Rgb), &y, &u, &v);

	return YUV_8BIT(y, u, v);
}

/*******************************************************************************
 * FpsControl Useing Sample:
 *  FPS_CTRL_S g_stFpsCtrl;
 *
 *  Take 12 frame uniform in 25.
 *  InitFps(&g_stFpsCtrl, 25, 12);
 *
 *  {
 *       if(FpsControl(&g_stFpsCtrl)) printf("Yes, this frmae should be token");
 *  }
 *
 ******************************************************************************/
typedef struct _FPS_CTRL_S {
	CVI_U32 u32Ffps; /* Full frame rate    */
	CVI_U32 u32Tfps; /* Target frame rate  */
	CVI_U32 u32FrmKey; /* update key frame   */
} FPS_CTRL_S;

static inline CVI_VOID InitFps(FPS_CTRL_S *pFrmCtrl, CVI_U32 u32FullFps, CVI_U32 u32TagFps)
{
	pFrmCtrl->u32Ffps = u32FullFps;
	pFrmCtrl->u32Tfps = u32TagFps;
	pFrmCtrl->u32FrmKey = 0;
}

static inline CVI_BOOL FpsControl(FPS_CTRL_S *pFrmCtrl)
{
	CVI_BOOL bReturn = CVI_FALSE;

	pFrmCtrl->u32FrmKey += pFrmCtrl->u32Tfps;
	if (pFrmCtrl->u32FrmKey >= pFrmCtrl->u32Ffps) {
		pFrmCtrl->u32FrmKey -= pFrmCtrl->u32Ffps;
		bReturn = CVI_TRUE;
	}

	return bReturn;
}

static inline CVI_U32 GetLowAddr(CVI_U64 u64Phyaddr)
{
	return (CVI_U32)u64Phyaddr;
}

static inline CVI_U32 GetHighAddr(CVI_U64 u64Phyaddr)
{
	return (CVI_U32)(u64Phyaddr >> 32);
}

#define CVI_usleep(usec) usleep(usec)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_MATH_H__ */
