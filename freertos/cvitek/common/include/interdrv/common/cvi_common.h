/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common.h
 * Description: Common video definitions.
 */

#ifndef __CVI_COMMON_H__
#define __CVI_COMMON_H__

#include <cvi_type.h>
#include <cvi_defines.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#undef ALIGN
#define ALIGN(x, a)      (((x) + ((a)-1)) & ~((a)-1))

#define ATTRIBUTE  __attribute__((aligned(ALIGN_NUM)))

#define CVI_INVALID_CHN (-1)
#define CVI_INVALID_LAYER (-1)
#define CVI_INVALID_DEV (-1)
#define CVI_INVALID_HANDLE (-1)
#define CVI_INVALID_VALUE (-1)
#define CVI_INVALID_TYPE (-1)

typedef CVI_S32 VI_DEV;
typedef CVI_S32 VI_PIPE;
typedef CVI_S32 VI_CHN;
typedef CVI_S32 ISP_DEV;
typedef CVI_S32 SENSOR_ID;

#define FOREACH_MOD(MOD) {\
	MOD(BASE)   \
	MOD(VB)	    \
	MOD(SYS)    \
	MOD(RGN)    \
	MOD(CHNL)   \
	MOD(VDEC)   \
	MOD(VPSS)   \
	MOD(VENC)   \
	MOD(H264E)  \
	MOD(JPEGE)  \
	MOD(MPEG4E) \
	MOD(H265E)  \
	MOD(JPEGD)  \
	MOD(VO)	    \
	MOD(VI)	    \
	MOD(DIS)    \
	MOD(RC)	    \
	MOD(AIO)    \
	MOD(AI)	    \
	MOD(AO)	    \
	MOD(AENC)   \
	MOD(ADEC)   \
	MOD(AUD)   \
	MOD(VPU)    \
	MOD(ISP)    \
	MOD(IVE)    \
	MOD(USER)   \
	MOD(PROC)   \
	MOD(LOG)    \
	MOD(H264D)  \
	MOD(GDC)    \
	MOD(PHOTO)  \
	MOD(FB)	    \
	MOD(BUTT)   \
}

#define GENERATE_ENUM(ENUM) CVI_ID_ ## ENUM,

typedef enum _MOD_ID_E FOREACH_MOD(GENERATE_ENUM) MOD_ID_E;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMMON_H__ */
