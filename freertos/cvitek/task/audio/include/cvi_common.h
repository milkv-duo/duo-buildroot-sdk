/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common.h
 * Description: Common video definitions.
 */

#ifndef __CVI_COMMON_H__
#define __CVI_COMMON_H__

#include <string.h>

#include "cvi_type.h"
#include "cvi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef VER_X
#define VER_X 1
#endif

#ifndef VER_Y
#define VER_Y 0
#endif

#ifndef VER_Z
#define VER_Z 0
#endif

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef CVI_DEBUG
#define VER_D " Debug"
#else
#define VER_D " Release"
#endif


#define ATTRIBUTE  __attribute__((aligned(ALIGN_NUM)))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define MK_VERSION(x, y, z) STR(x) "." STR(y) "." STR(z)



typedef CVI_S32 AI_CHN;
typedef CVI_S32 AO_CHN;
typedef CVI_S32 AENC_CHN;
typedef CVI_S32 ADEC_CHN;
typedef CVI_S32 AUDIO_DEV;
typedef CVI_S32 VI_DEV;
typedef CVI_S32 VI_PIPE;
typedef CVI_S32 VI_CHN;
typedef CVI_S32 VO_DEV;
typedef CVI_S32 VO_LAYER;
typedef CVI_S32 VO_CHN;
typedef CVI_S32 GRAPHIC_LAYER;
typedef CVI_S32 VENC_CHN;
typedef CVI_S32 VDEC_CHN;
typedef CVI_S32 ISP_DEV;
typedef CVI_S32 SENSOR_ID;
typedef CVI_S32 MIPI_DEV;
typedef CVI_S32 SLAVE_DEV;
typedef CVI_S32 VPSS_GRP;
typedef CVI_S32 VPSS_CHN;

#define CVI_INVALID_CHN (-1)
#define CVI_INVALID_LAYER (-1)
#define CVI_INVALID_DEV (-1)
#define CVI_INVALID_HANDLE (-1)
#define CVI_INVALID_VALUE (-1)
#define CVI_INVALID_TYPE (-1)


#define CCM_MATRIX_SIZE             (9)
#define CCM_MATRIX_NUM              (7)


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

typedef struct _MMF_CHN_S {
	MOD_ID_E    enModId;
	CVI_S32     s32DevId;
	CVI_S32     s32ChnId;
} MMF_CHN_S;


/* We just copy this value of payload type from RTP/RTSP definition */
typedef enum {
	PT_PCMU          = 0,
	PT_1016          = 1,
	PT_G721          = 2,
	PT_GSM           = 3,
	PT_G723          = 4,
	PT_DVI4_8K       = 5,
	PT_DVI4_16K      = 6,
	PT_LPC           = 7,
	PT_PCMA          = 8,
	PT_G722          = 9,
	PT_S16BE_STEREO  = 10,
	PT_S16BE_MONO    = 11,
	PT_QCELP         = 12,
	PT_CN            = 13,
	PT_MPEGAUDIO     = 14,
	PT_G728          = 15,
	PT_DVI4_3        = 16,
	PT_DVI4_4        = 17,
	PT_G729          = 18,
	PT_G711A         = 19,
	PT_G711U         = 20,
	PT_G726          = 21,
	PT_G729A         = 22,
	PT_LPCM          = 23,
	PT_CelB          = 25,
	PT_JPEG          = 26,
	PT_CUSM          = 27,
	PT_NV            = 28,
	PT_PICW          = 29,
	PT_CPV           = 30,
	PT_H261          = 31,
	PT_MPEGVIDEO     = 32,
	PT_MPEG2TS       = 33,
	PT_H263          = 34,
	PT_SPEG          = 35,
	PT_MPEG2VIDEO    = 36,
	PT_AAC           = 37,
	PT_WMA9STD       = 38,
	PT_HEAAC         = 39,
	PT_PCM_VOICE     = 40,
	PT_PCM_AUDIO     = 41,
	PT_MP3           = 43,
	PT_ADPCMA        = 49,
	PT_AEC           = 50,
	PT_X_LD          = 95,
	PT_H264          = 96,
	PT_D_GSM_HR      = 200,
	PT_D_GSM_EFR     = 201,
	PT_D_L8          = 202,
	PT_D_RED         = 203,
	PT_D_VDVI        = 204,
	PT_D_BT656       = 220,
	PT_D_H263_1998   = 221,
	PT_D_MP1S        = 222,
	PT_D_MP2P        = 223,
	PT_D_BMPEG       = 224,
	PT_MP4VIDEO      = 230,
	PT_MP4AUDIO      = 237,
	PT_VC1           = 238,
	PT_JVC_ASF       = 255,
	PT_D_AVI         = 256,
	PT_DIVX3         = 257,
	PT_AVS             = 258,
	PT_REAL8         = 259,
	PT_REAL9         = 260,
	PT_VP6             = 261,
	PT_VP6F             = 262,
	PT_VP6A             = 263,
	PT_SORENSON          = 264,
	PT_H265          = 265,
	PT_VP8             = 266,
	PT_MVC             = 267,
	PT_PNG           = 268,
	/* add by ourselves */
	PT_AMR           = 1001,
	PT_MJPEG         = 1002,
	PT_BUTT
} PAYLOAD_TYPE_E;

#define VERSION_NAME_MAXLEN 128
typedef struct _MMF_VERSION_S {
	char version[VERSION_NAME_MAXLEN];
} MMF_VERSION_S;

typedef enum {
	GPIO_GRPA,
	GPIO_GRPB,
	GPIO_GRPC,
	GPIO_GRPD,
	GPIO_GRPE,
	GPIO_GRO_BUFF
} GPIO_GRP_E;

typedef enum {
	GPIO_INDEX_0,
	GPIO_INDEX_1,
	GPIO_INDEX_2,
	GPIO_INDEX_3,
	GPIO_INDEX_4,
	GPIO_INDEX_5,
	GPIO_INDEX_6,
	GPIO_INDEX_7,
	GPIO_INDEX_8,
	GPIO_INDEX_9,
	GPIO_INDEX_10,
	GPIO_INDEX_11,
	GPIO_INDEX_12,
	GPIO_INDEX_13,
	GPIO_INDEX_14,
	GPIO_INDEX_15,
	GPIO_INDEX_16,
	GPIO_INDEX_17,
	GPIO_INDEX_18,
	GPIO_INDEX_19,
	GPIO_INDEX_20,
	GPIO_INDEX_21,
	GPIO_INDEX_22,
	GPIO_INDEX_23,
	GPIO_INDEX_24,
	GPIO_INDEX_25,
	GPIO_INDEX_26,
	GPIO_INDEX_27,
	GPIO_INDEX_28,
	GPIO_INDEX_29,
	GPIO_INDEX_30,
	GPIO_INDEX_31,
	GPIO_INDEX_BUFF
} GPIO_INDEX_E;

typedef enum {
	GPIO_ACTIVE_LOW,
	GPIO_ACTIVE_HIGH,
	GPIO_ACTIVE_BUFF
} GPIO_ACTIVE_E;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMMON_H__ */
