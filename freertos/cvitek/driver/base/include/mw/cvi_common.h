/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common.h
 * Description: Common video definitions.
 */

#ifndef __CVI_COMMON_H__
#define __CVI_COMMON_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

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

struct MMF_CHN_S {
	MOD_ID_E    enModId;
	int32_t     s32DevId;
	int32_t     s32ChnId;
};

#define VERSION_NAME_MAXLEN 128
struct MMF_VERSION_S {
	char version[VERSION_NAME_MAXLEN];
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMMON_H__ */
