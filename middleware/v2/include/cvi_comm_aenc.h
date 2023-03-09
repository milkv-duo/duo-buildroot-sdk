/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_aenc.h
 * Description: basic audio audio
 * encoder api for application layer
 */
#ifndef __CVI_COMM_AENC_H__
#define __CVI_COMM_AENC_H__

#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include "cvi_comm_aio.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct _AENC_ATTR_G711_S {
	CVI_U32 resv;            /*reserve item*/
} AENC_ATTR_G711_S;

typedef struct _AENC_ATTR_G726_S {
	G726_BPS_E enG726bps;
} AENC_ATTR_G726_S;

typedef struct _AENC_ATTR_ADPCM_S {
	ADPCM_TYPE_E enADPCMType;
} AENC_ATTR_ADPCM_S;

typedef struct _AENC_ATTR_LPCM_S {
	CVI_U32 resv;            /*reserve item*/
} AENC_ATTR_LPCM_S;

typedef struct _AAC_AENC_ENCODER_S {
	PAYLOAD_TYPE_E  enType;
	CVI_U32		u32MaxFrmLen;
	CVI_CHAR	aszName[17];
	/* encoder type,be used to print proc information */
	CVI_S32 (*pfnOpenEncoder)(CVI_VOID *pEncoderAttr, CVI_VOID **ppEncoder);
	/* pEncoder is the handle to control the encoder */
#if 0
	CVI_S32 (*pfnEncodeFrm)(CVI_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
		CVI_U8 *pu8Outbuf, CVI_U32 *pu32OutLen);
#endif
CVI_S32  (*pfnEncodeFrm)(CVI_VOID *pEncoder, CVI_S16 * inputdata, CVI_U8 * pu8Outbuf,
							CVI_S32 s32InputSizeBytes, CVI_U32 *pu32OutLen);
	CVI_S32 (*pfnCloseEncoder)(CVI_VOID *pEncoder);
} AAC_AENC_ENCODER_S;

typedef struct _AENC_CHN_ATTR_S {
	PAYLOAD_TYPE_E      enType;         /*payload type ()*/
	CVI_U32		u32PtNumPerFrm;
	CVI_U32              u32BufSize;      /*buf size [2~CVI_MAX_AUDIO_FRAME_NUM]*/
	/* CVI_VOID ATTRIBUTE   *pValue;  point to attribute of definite audio encoder*/
	CVI_VOID    *pValue;
	CVI_BOOL bFileDbgMode;
} AENC_CHN_ATTR_S;

typedef enum _EN_AENC_ERR_CODE_E {
	AENC_ERR_ENCODER_ERR     = 64,
	AENC_ERR_VQE_ERR        = 65,

} EN_AENC_ERR_CODE_E;


/* invalid device ID */
#define CVI_ERR_AENC_INVALID_DEVID     0xA2000001
/* invalid channel ID */
#define CVI_ERR_AENC_INVALID_CHNID     0xA2000002
/* at least one parameter is illegal ,eg, an illegal enumeration value  */
#define CVI_ERR_AENC_ILLEGAL_PARAM     0xA2000003
/* channel exists */
#define CVI_ERR_AENC_EXIST             0xA2000004
/* channel unexists */
#define CVI_ERR_AENC_UNEXIST           0xA2000005
/* using a NULL point */
#define CVI_ERR_AENC_NULL_PTR         0xA2000006
/* try to enable or initialize system,device or channel, before configing attribute */
#define CVI_ERR_AENC_NOT_CONFIG        0xA2000007
/* operation is not supported by NOW */
#define CVI_ERR_AENC_NOT_SUPPORT       0xA2000008
/* operation is not permitted ,eg, try to change static attribute */
#define CVI_ERR_AENC_NOT_PERM          0xA2000009
/* failure caused by malloc memory */
#define CVI_ERR_AENC_NOMEM            0xA200000A
/* failure caused by malloc buffer */
#define CVI_ERR_AENC_NOBUF             0xA200000B
/* no data in buffer */
#define CVI_ERR_AENC_BUF_EMPTY         0xA200000C
/* no buffer for new data */
#define CVI_ERR_AENC_BUF_FULL          0xA200000D
/* system is not ready,had not initialized or loaded*/
#define CVI_ERR_AENC_SYS_NOTREADY      0xA200000E
/* encoder internal err */
#define CVI_ERR_AENC_ENCODER_ERR       0xA200000F
/* vqe internal err */
#define CVI_ERR_AENC_VQE_ERR       0xA2000010


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __CVI_COMM_AENC_H__*/

