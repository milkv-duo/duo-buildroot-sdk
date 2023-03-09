/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_adec.h
 * Description: basic audio decoder  api for application layer
 */

#ifndef __CVI_COMM_ADEC_H__
#define __CVI_COMM_ADEC_H__


#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include "cvi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct _ADEC_ATTR_G711_S {
	CVI_U32 resv;
} ADEC_ATTR_G711_S;

typedef struct _ADEC_ATTR_G726_S {
	G726_BPS_E enG726bps;
} ADEC_ATTR_G726_S;

typedef struct _ADEC_ATTR_ADPCM_S {
	ADPCM_TYPE_E enADPCMType;
} ADEC_ATTR_ADPCM_S;

typedef struct _ADEC_ATTR_LPCM_S {
	CVI_U32 resv;
} ADEC_ATTR_LPCM_S;

typedef enum _ADEC_MODE_E {
	ADEC_MODE_PACK = 0,
	/*require input is valid dec pack(a		*/
	/*complete frame encode result),*/
	/*e.g.the stream get from AENC is a*/
	/*valid dec pack, the stream know actually*/
	/*pack len from file is also a dec pack.*/
	/*this mode is high-performative*/
	ADEC_MODE_STREAM,
	/*input is stream,low-performative,		*/
	/*if you couldn't find out whether a stream is*/
	/*valid dec pack,you could use*/
	/*this mode*/
	ADEC_MODE_BUTT
} ADEC_MODE_E;

typedef struct _ADEC_CH_ATTR_S {
	PAYLOAD_TYPE_E enType;
	CVI_U32         u32BufSize; /*buf size[2~CVI_MAX_AUDIO_FRAME_NUM]*/
	ADEC_MODE_E	enMode;/*decode mode*/
	/* CVI_VOID ATTRIBUTE      *pValue;*/
	CVI_VOID *pValue;
	CVI_BOOL bFileDbgMode;
	//------user should update these information if
	//ao not enable
	CVI_S32 s32BytesPerSample;
	CVI_S32 s32frame_size; //in samples
	CVI_S32 s32ChannelNums; // 1 or 2
	CVI_S32 s32Sample_rate;
} ADEC_CHN_ATTR_S;

typedef struct _ADEC_CHN_STATE_S {
	CVI_BOOL bEndOfStream;             /* EOS flag */
	CVI_U32 u32BufferFrmNum;           /* total number of channel buffer */
	CVI_U32 u32BufferFreeNum;          /* free number of channel buffer */
	CVI_U32 u32BufferBusyNum;          /* busy number of channel buffer */
} ADEC_CHN_STATE_S;

typedef struct _ADEC_DECODER_S {
	PAYLOAD_TYPE_E  enType;
	CVI_CHAR	aszName[17];

	CVI_S32 (*pfnOpenDecoder)(CVI_VOID *pDecoderAttr, CVI_VOID **ppDecoder);
	CVI_S32 (*pfnDecodeFrm)(CVI_VOID *pDecoder, CVI_U8 **pu8Inbuf,
				CVI_S32 *ps32LeftByte,
				CVI_U16 *pu16Outbuf, CVI_U32 *pu32OutLen, CVI_U32 *pu32Chns);
	CVI_S32 (*pfnGetFrmInfo)(CVI_VOID *pDecoder, CVI_VOID *pInfo);
	CVI_S32 (*pfnCloseDecoder)(CVI_VOID *pDecoder);
	CVI_S32 (*pfnResetDecoder)(CVI_VOID *pDecoder);
} ADEC_DECODER_S;

/* invalid device ID */
#define CVI_ERR_ADEC_INVALID_DEVID     0xA3000001
/* invalid channel ID */
#define CVI_ERR_ADEC_INVALID_CHNID     0xA3000002
/* at least one parameter is illegal ,eg, an illegal enumeration value  */
#define CVI_ERR_ADEC_ILLEGAL_PARAM     0xA3000003
/* channel exists */
#define CVI_ERR_ADEC_EXIST            0xA3000004
/* channel unexists */
#define CVI_ERR_ADEC_UNEXIST           0xA3000005
/* using a NULL point */
#define CVI_ERR_ADEC_NULL_PTR          0xA3000006
/* try to enable or initialize system,device or channel, before configing attribute */
#define CVI_ERR_ADEC_NOT_CONFIG        0xA3000007
/* operation is not supported by NOW */
#define CVI_ERR_ADEC_NOT_SUPPORT       0xA3000008
/* operation is not permitted ,eg, try to change stati attribute */
#define CVI_ERR_ADEC_NOT_PERM          0xA3000009
/* failure caused by malloc memory */
#define CVI_ERR_ADEC_NOMEM             0xA300000A
/* failure caused by malloc buffer */
#define CVI_ERR_ADEC_NOBUF             0xA300000B
/* no data in buffer */
#define CVI_ERR_ADEC_BUF_EMPTY         0xA300000C
/* no buffer for new data */
#define CVI_ERR_ADEC_BUF_FULL          0xA300000D
/* system is not ready,had not initialized or loaded*/
#define CVI_ERR_ADEC_SYS_NOTREADY      0xA300000E
/* decoder internal err */
#define CVI_ERR_ADEC_DECODER_ERR       0xA300000F
/* input buffer not enough to decode one frame */
#define CVI_ERR_ADEC_BUF_LACK          0xA3000010




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __CVI_COMM_ADEC_H__*/

