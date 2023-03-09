/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_audio_aac_adp.c
 * Description:
 *   Common audio link lib for AAC codec.
 */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "cvi_audio_aac_adp.h"
#include "cvi_audio_dl_adp.h"
#include "cvi_aacdec.h"
#include "cvi_aacenc.h"

#define _VERSION_TAG_AAC_ADP_  "20220520"


#define CVI_AUDIO_ASSERT(x) {	\
	if (CVI_TRUE != (x)) \
		printf("param is not true\n");  \
}

#define AAC_ENC_LIB_NAME "libaacenc2.so"
#define AAC_DEC_LIB_NAME "libaacdec2.so"
#define SAMPLE_RES_CHECK_NULL_PTR(ptr)\
	do {\
		if (NULL == (CVI_U8 *)ptr)\
			printf("[fatal error]ptr is NULL,fuc:%s,line:%d\n", __func__, __LINE__);\
	} while (0)
#define CVI_ERR_AENC_NOT_SUPPORT       0xA2000008
#define CVI_ERR_AENC_ILLEGAL_PARAM     0xA2000003
#define CVI_ERR_ADEC_NOT_SUPPORT       0xA3000008
#define CVI_ERR_AENC_NULL_PTR         0xA2000006
#define CVI_ERR_AENC_NOMEM            0xA200000A
#define CVI_ERR_ADEC_NOMEM             0xA300000A
#define CVI_ERR_ADEC_DECODER_ERR       0xA300000F
#define CVI_ERR_ADEC_BUF_LACK          0xA3000010
//# aac enc lib
typedef CVI_S32(*pCVI_AACENC_GetVersion_Callback)(AACENC_VERSION_S *pVersion);
typedef CVI_S32(*pAACInitDefaultConfig_Callback)(AACENC_CONFIG *pstConfig);
typedef CVI_S32(*pAACEncoderOpen_Callback)(AAC_ENCODER_S **phAacPlusEnc, AACENC_CONFIG *pstConfig);
typedef CVI_S32(*pAACEncoderFrame_Callback)(AAC_ENCODER_S *hAacPlusEnc,
						CVI_S16 *ps16PcmBuf,
						CVI_U8 *pu8Outbuf,
						CVI_S32 s32InputBytes,
						CVI_S32 *ps32NumOutBytes);

typedef CVI_VOID(*pAACEncoderClose_Callback)(AAC_ENCODER_S *hAacPlusEnc);

//# aac dec lib
typedef int(*pCVI_AACDEC_GetVersion_Callback)(AACDEC_VERSION_S *pVersion);


typedef CVIAACDecoder(*pAACInitDecoder_Callback)(AACDECTransportType enTranType);
typedef CVI_VOID(*pAACFreeDecoder_Callback)(CVIAACDecoder CVIAACDecoder);
typedef CVI_S32(*pAACSetRawMode_Callback)(CVIAACDecoder cviAACDecoder, CVI_S32 nChans, CVI_S32 sampRate);
typedef CVI_S32(*pAACDecodeFindSyncHeader_Callback)(CVIAACDecoder cviAACDecoder,
			CVI_U8 **ppInbufPtr,
			CVI_S32 *pBytesLeft);
typedef CVI_S32(*pAACDecodeFrame_Callback)(CVIAACDecoder cviAACDecoder,
			CVI_U8 **ppInbufPtr,
			CVI_S32 *pBytesLeft, CVI_S16 *pOutPcm);
typedef CVI_S32(*pAACGetLastFrameInfo_Callback)(CVIAACDecoder cviAACDecoder, AACFrameInfo *aacFrameInfo);
typedef CVI_S32(*pAACDecoderSetEosFlag_Callback)(CVIAACDecoder cviAACDecoder, CVI_S32 s32Eosflag);
typedef CVI_S32(*pAACFlushCodec_Callback)(CVIAACDecoder cviAACDecoder);

typedef struct {
	CVI_S32 s32OpenCnt;
	CVI_VOID *pLibHandle;
	pCVI_AACENC_GetVersion_Callback pCVI_AACENC_GetVersion;
	pAACInitDefaultConfig_Callback pAACInitDefaultConfig;
	pAACEncoderOpen_Callback pAACEncoderOpen;
	pAACEncoderFrame_Callback pAACEncoderFrame;
	pAACEncoderClose_Callback pAACEncoderClose;
} AACENC_FUN_S;

typedef struct {
	CVI_S32 s32OpenCnt;
	CVI_VOID *pLibHandle;
	pCVI_AACDEC_GetVersion_Callback pCVI_AACDEC_GetVersion;
	pAACInitDecoder_Callback pAACInitDecoder;
	pAACFreeDecoder_Callback pAACFreeDecoder;
	pAACSetRawMode_Callback pAACSetRawMode;
	pAACDecodeFindSyncHeader_Callback pAACDecodeFindSyncHeader;
	pAACDecodeFrame_Callback pAACDecodeFrame;
	pAACGetLastFrameInfo_Callback pAACGetLastFrameInfo;
	pAACDecoderSetEosFlag_Callback pAACDecoderSetEosFlag;
	pAACFlushCodec_Callback pAACFlushCodec;
} AACDEC_FUN_S;

//#define DUMP_AACENC
#ifdef DUMP_AACENC

FILE * pfdout_ENC = CVI_NULL;
FILE *pfdin_ENC = CVI_NULL;
#endif

//#define DUMP_AACDEC
#ifdef DUMP_AACDEC
FILE *pfdin_DEC = CVI_NULL;
FILE *pfdout_DEC = CVI_NULL;
FILE *pfdout_DEC_L = CVI_NULL;
#endif

int cnt_aenc = 100000;
int cnt_adec = 100000;

static CVI_S32 g_AacEncHandle = -1;
static CVI_S32 g_AacDecHandle = -1;

static AACENC_FUN_S g_stAacEncFunc = {0};
static AACDEC_FUN_S g_stAacDecFunc = {0};
#ifndef UNUSED_PARAM_AVOID
#define UNUSED_PARAM_AVOID(X)   ((X) = (X))
#endif

static CVI_S32 InitAacAencLib(void)
{
	CVI_S32 s32Ret = CVI_FAILURE;

	if (g_stAacEncFunc.s32OpenCnt == 0) {

		AACENC_FUN_S stAacEncFunc;

		memset(&stAacEncFunc, 0, sizeof(AACENC_FUN_S));
#ifdef CVIAUDIO_STATIC
		printf("[cviaudio][%s]Not Using dlopen\n", __func__);
		g_stAacEncFunc.pLibHandle = (CVI_VOID *)1;//not using dlopen, give a default value for handler check
		g_stAacEncFunc.pCVI_AACENC_GetVersion = CVI_AACENC_GetVersion;
		g_stAacEncFunc.pAACInitDefaultConfig = AACInitDefaultConfig;
		g_stAacEncFunc.pAACEncoderOpen = AACEncoderOpen;
		g_stAacEncFunc.pAACEncoderFrame = AACEncoderFrame;
		g_stAacEncFunc.pAACEncoderClose = AACEncoderClose;
		UNUSED_PARAM_AVOID(s32Ret);
#else
		s32Ret = CVI_Audio_Dlopen(&(stAacEncFunc.pLibHandle), AAC_ENC_LIB_NAME);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "load aenc lib fail!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacEncFunc.pCVI_AACENC_GetVersion),
				stAacEncFunc.pLibHandle, "CVI_AACENC_GetVersion");
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacEncFunc.pAACInitDefaultConfig),
			stAacEncFunc.pLibHandle, "AACInitDefaultConfig");
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacEncFunc.pAACEncoderOpen),
		stAacEncFunc.pLibHandle, "AACEncoderOpen");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacEncFunc.pAACEncoderFrame),
		stAacEncFunc.pLibHandle, "AACEncoderFrame");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacEncFunc.pAACEncoderClose),
		stAacEncFunc.pLibHandle, "AACEncoderClose");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_AENC_NOT_SUPPORT;
		}

		memcpy(&g_stAacEncFunc, &stAacEncFunc, sizeof(AACENC_FUN_S));
#endif
	}
	g_stAacEncFunc.s32OpenCnt++;
	return CVI_SUCCESS;
}

CVI_VOID DeInitAacAencLib(CVI_VOID)
{
	if (g_stAacEncFunc.s32OpenCnt != 0)
		g_stAacEncFunc.s32OpenCnt--;

	if (g_stAacEncFunc.s32OpenCnt == 0) {
#ifdef CVIAUDIO_STATIC
		g_stAacEncFunc.pLibHandle = CVI_NULL;
		memset(&g_stAacEncFunc, 0, sizeof(AACENC_FUN_S));
		g_stAacEncFunc.pCVI_AACENC_GetVersion = CVI_NULL;
		g_stAacEncFunc.pAACInitDefaultConfig = CVI_NULL;
		g_stAacEncFunc.pAACEncoderOpen = CVI_NULL;
		g_stAacEncFunc.pAACEncoderFrame = CVI_NULL;
		g_stAacEncFunc.pAACEncoderClose = CVI_NULL;
#else
		if (g_stAacEncFunc.pLibHandle != CVI_NULL)
			CVI_Audio_Dlclose(g_stAacEncFunc.pLibHandle);

		memset(&g_stAacEncFunc, 0, sizeof(AACENC_FUN_S));
#endif
	}

}

CVI_S32 CVI_AACENC_GetVersion_Adp(AACENC_VERSION_S *pVersion)
{
	if (g_stAacEncFunc.pCVI_AACENC_GetVersion == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_AENC_NOT_SUPPORT;
	}
	return g_stAacEncFunc.pCVI_AACENC_GetVersion(pVersion);
}

CVI_S32 AACInitDefaultConfig_Adp(AACENC_CONFIG *pstConfig)
{
	if (g_stAacEncFunc.pAACInitDefaultConfig == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_AENC_NOT_SUPPORT;
	}
	return g_stAacEncFunc.pAACInitDefaultConfig(pstConfig);
}

CVI_S32 AACEncoderOpen_Adp(AAC_ENCODER_S **phAacPlusEnc, AACENC_CONFIG *pstConfig)
{
	if (g_stAacEncFunc.pAACEncoderOpen == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_AENC_NOT_SUPPORT;
	}

	return g_stAacEncFunc.pAACEncoderOpen(phAacPlusEnc, pstConfig);
}

CVI_S32 AACEncoderFrame_Adp(AAC_ENCODER_S *hAacPlusEnc,
	CVI_S16 *ps16PcmBuf,
	CVI_U8 *pu8Outbuf,
	CVI_S32 s32InputBytes,
	CVI_S32 *ps32NumOutBytes)
{
	if (g_stAacEncFunc.pAACEncoderFrame == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_AENC_NOT_SUPPORT;
	}

	return g_stAacEncFunc.pAACEncoderFrame(hAacPlusEnc, ps16PcmBuf, pu8Outbuf, s32InputBytes, ps32NumOutBytes);
}

CVI_VOID AACEncoderClose_Adp(AAC_ENCODER_S *hAacPlusEnc)
{
	if (g_stAacEncFunc.pAACEncoderClose == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return;
	}

	return g_stAacEncFunc.pAACEncoderClose(hAacPlusEnc);
}

static CVI_S32 InitAacAdecLib(void)
{
	CVI_S32 s32Ret = CVI_FAILURE;

	if (g_stAacDecFunc.s32OpenCnt == 0) {
		AACDEC_FUN_S stAacDecFunc;

		memset(&stAacDecFunc, 0, sizeof(AACDEC_FUN_S));
#ifdef CVIAUDIO_STATIC
		printf("[cviaudio][%s]Not Using dlopen\n", __func__);
		g_stAacDecFunc.pLibHandle = (CVI_VOID *)1;//give a none-null value for handle
		g_stAacDecFunc.pCVI_AACDEC_GetVersion = CVI_AACDEC_GetVersion;
		g_stAacDecFunc.pAACInitDecoder = AACInitDecoder;
		g_stAacDecFunc.pAACFreeDecoder = AACFreeDecoder;
		g_stAacDecFunc.pAACSetRawMode = AACSetRawMode;
		g_stAacDecFunc.pAACDecodeFindSyncHeader = AACDecodeFindSyncHeader;
		g_stAacDecFunc.pAACDecodeFrame = AACDecodeFrame;
		g_stAacDecFunc.pAACGetLastFrameInfo = AACGetLastFrameInfo;
		g_stAacDecFunc.pAACDecoderSetEosFlag = AACDecoderSetEosFlag;
		g_stAacDecFunc.pAACFlushCodec = AACFlushCodec;
		if (!g_stAacDecFunc.pAACFlushCodec) {
			s32Ret = CVI_FAILURE;
			printf("[%s][%d]error in function pt\n", __func__, __LINE__);
		}
		UNUSED_PARAM_AVOID(s32Ret);

#else
		s32Ret = CVI_Audio_Dlopen(&(stAacDecFunc.pLibHandle), AAC_DEC_LIB_NAME);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "load aenc lib fail!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pCVI_AACDEC_GetVersion),
				stAacDecFunc.pLibHandle, "CVI_AACDEC_GetVersion");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACInitDecoder),
			stAacDecFunc.pLibHandle, "AACInitDecoder");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACFreeDecoder),
			stAacDecFunc.pLibHandle, "AACFreeDecoder");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACSetRawMode),
			stAacDecFunc.pLibHandle, "AACSetRawMode");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACDecodeFindSyncHeader),
			stAacDecFunc.pLibHandle, "AACDecodeFindSyncHeader");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACDecodeFrame),
			stAacDecFunc.pLibHandle, "AACDecodeFrame");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACGetLastFrameInfo),
			stAacDecFunc.pLibHandle, "AACGetLastFrameInfo");
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACDecoderSetEosFlag),
			stAacDecFunc.pLibHandle, "AACDecoderSetEosFlag");

		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		s32Ret = CVI_Audio_Dlsym((CVI_VOID **)&(stAacDecFunc.pAACFlushCodec),
			stAacDecFunc.pLibHandle, "AACFlushCodec");
		if (s32Ret != CVI_SUCCESS) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "find symbol error!\n");
			return CVI_ERR_ADEC_NOT_SUPPORT;
		}

		memcpy(&g_stAacDecFunc, &stAacDecFunc, sizeof(AACDEC_FUN_S));
#endif
	}
	g_stAacDecFunc.s32OpenCnt++;
	return CVI_SUCCESS;
}

CVI_VOID DeInitAacAdecLib(CVI_VOID)
{
	if (g_stAacDecFunc.s32OpenCnt != 0)
		g_stAacDecFunc.s32OpenCnt--;


	if (g_stAacDecFunc.s32OpenCnt == 0) {
#ifdef CVIAUDIO_STATIC
		g_stAacDecFunc.pLibHandle = CVI_NULL;
		memset(&g_stAacDecFunc, 0, sizeof(AACDEC_FUN_S));
		g_stAacDecFunc.pCVI_AACDEC_GetVersion = CVI_NULL;
		g_stAacDecFunc.pAACInitDecoder = CVI_NULL;
		g_stAacDecFunc.pAACFreeDecoder = CVI_NULL;
		g_stAacDecFunc.pAACSetRawMode = CVI_NULL;
		g_stAacDecFunc.pAACDecodeFindSyncHeader = CVI_NULL;
		g_stAacDecFunc.pAACDecodeFrame = CVI_NULL;
		g_stAacDecFunc.pAACGetLastFrameInfo = CVI_NULL;
		g_stAacDecFunc.pAACDecoderSetEosFlag = CVI_NULL;
		g_stAacDecFunc.pAACFlushCodec = CVI_NULL;

#else
		if (g_stAacDecFunc.pLibHandle != CVI_NULL)
			CVI_Audio_Dlclose(g_stAacDecFunc.pLibHandle);

		memset(&g_stAacDecFunc, 0, sizeof(AACDEC_FUN_S));
#endif
	}

}

CVI_S32 CVI_AACDEC_GetVersion_Adp(AACDEC_VERSION_S *pVersion)
{
	if (g_stAacDecFunc.pCVI_AACDEC_GetVersion == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pCVI_AACDEC_GetVersion(pVersion);
}

CVIAACDecoder AACInitDecoder_Adp(AACDECTransportType enTranType)
{
	if (g_stAacDecFunc.pAACInitDecoder == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_NULL;
	}

	return g_stAacDecFunc.pAACInitDecoder(enTranType);
}

CVI_VOID AACFreeDecoder_Adp(CVIAACDecoder cviAACDecoder)
{
	if (g_stAacDecFunc.pAACFreeDecoder == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return;
	}

	return g_stAacDecFunc.pAACFreeDecoder(cviAACDecoder);
}

CVI_S32 AACSetRawMode_Adp(CVIAACDecoder cviAACDecoder, CVI_S32 nChans, CVI_S32 sampRate)
{
	if (g_stAacDecFunc.pAACSetRawMode == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}


	return g_stAacDecFunc.pAACSetRawMode(cviAACDecoder, nChans, sampRate);
}

CVI_S32 AACDecodeFindSyncHeader_Adp(CVIAACDecoder cviAACDecoder, CVI_U8 **ppInbufPtr, CVI_S32 *pBytesLeft)
{
	if (g_stAacDecFunc.pAACDecodeFindSyncHeader == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pAACDecodeFindSyncHeader(cviAACDecoder, ppInbufPtr, pBytesLeft);
}

CVI_S32 AACDecodeFrame_Adp(CVIAACDecoder cviAACDecoder, CVI_U8 **ppInbufPtr, CVI_S32 *pBytesLeft, CVI_S16 *pOutPcm)
{
	if (g_stAacDecFunc.pAACDecodeFrame == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pAACDecodeFrame(cviAACDecoder, ppInbufPtr, pBytesLeft, pOutPcm);
}

CVI_S32 AACGetLastFrameInfo_Adp(CVIAACDecoder cviAACDecoder, AACFrameInfo *aacFrameInfo)
{
	if (g_stAacDecFunc.pAACGetLastFrameInfo == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pAACGetLastFrameInfo(cviAACDecoder, aacFrameInfo);
}

CVI_S32 AACDecoderSetEosFlag_Adp(CVIAACDecoder cviAACDecoder, CVI_S32 s32Eosflag)
{
	if (g_stAacDecFunc.pAACDecoderSetEosFlag == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pAACDecoderSetEosFlag(cviAACDecoder, s32Eosflag);
}

CVI_S32 AACFlushCodec_Adp(CVIAACDecoder cviAACDecoder)
{
	if (g_stAacDecFunc.pAACFlushCodec == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "call aac function fail!\n");
		return CVI_ERR_ADEC_NOT_SUPPORT;
	}

	return g_stAacDecFunc.pAACFlushCodec(cviAACDecoder);
}

static CVI_S32 AencCheckAACAttr(const AENC_ATTR_AAC_S *pstAACAttr)
{
	if (pstAACAttr->enBitWidth != AUDIO_BIT_WIDTH_16) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "invalid bitwidth for AAC encoder");
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pstAACAttr->enSoundMode >= AUDIO_SOUND_MODE_BUTT) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "invalid sound mode for AAC encoder");
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if ((pstAACAttr->enAACType == AAC_TYPE_EAACPLUS) &&
		(pstAACAttr->enSoundMode != AUDIO_SOUND_MODE_STEREO)) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
			   __func__, __LINE__, "invalid sound mode for AAC encoder");
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pstAACAttr->enTransType == AAC_TRANS_TYPE_ADTS) {
		if ((pstAACAttr->enAACType == AAC_TYPE_AACLD) ||
			(pstAACAttr->enAACType == AAC_TYPE_AACELD)) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
				   __func__, __LINE__, "AACLD or AACELD not support AAC_TRANS_TYPE_ADTS");
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 AencCheckAACLCConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32MinBitRate = 0;
	CVI_S32 s32MaxBitRate = 0;
	CVI_S32 s32RecommendRate = 0;

	if (pconfig->coderFormat == AACLC) {
		if (pconfig->nChannelsOut != pconfig->nChannelsIn) {
			printf("AACLC nChannelsOut(%d) in not equal to nChannelsIn(%d)\n",
			pconfig->nChannelsOut, pconfig->nChannelsIn);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}

		if (pconfig->sampleRate == 32000) {

			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 192000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 32000 Hz bitRate(%d) should be %d ~ %d, recommended %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);

				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 44100) {

			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 265000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 44100 Hz bitRate(%d) should be %d ~ %d, recommended %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 48000) {

			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 288000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 16000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 24000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 8000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 16000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 24000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 144000 : 288000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 22050) {

			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 132000 : 265000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLC 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else {

			printf("AACLC invalid samplerate(%d)\n", pconfig->sampleRate);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	} else {
		/* return erro code*/
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	return CVI_SUCCESS;
}

static CVI_S32 AencCheckEAACConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32MinBitRate = 0;
	CVI_S32 s32MaxBitRate = 0;
	CVI_S32 s32RecommendRate = 0;

	if (pconfig->coderFormat == EAAC) {
		if (pconfig->nChannelsOut != pconfig->nChannelsIn) {
			printf("EAAC nChannelsOut(%d) is not equal to nChannelsIn(%d)\n",
			pconfig->nChannelsOut, pconfig->nChannelsIn);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}

		if (pconfig->sampleRate == 32000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 44100) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 48000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 16000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 24000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 22050) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 24000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAAC 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else {
			printf("EAAC invalid samplerate(%d)\n", pconfig->sampleRate);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	} else {
		/* return error code */
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	return CVI_SUCCESS;
}

static CVI_S32 AencCheckEAACPLUSConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32MinBitRate = 0;
	CVI_S32 s32MaxBitRate = 0;
	CVI_S32 s32RecommendRate = 0;

	if (pconfig->coderFormat == EAACPLUS) {
		if (pconfig->nChannelsOut != 2 || pconfig->nChannelsIn != 2) {
			printf("EAACPLUS nChannelsOut(%d) and nChannelsIn(%d) should be 2\n",
			pconfig->nChannelsOut, pconfig->nChannelsIn);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}

		if (pconfig->sampleRate == 32000) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 64000;
			s32RecommendRate = 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 44100) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 64000;
			s32RecommendRate = 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 48000) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 64000;
			s32RecommendRate = 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 16000) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 48000;
			s32RecommendRate = 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 22050) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 64000;
			s32RecommendRate = 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 24000) {
			s32MinBitRate = 16000;
			s32MaxBitRate = 64000;
			s32RecommendRate = 32000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("EAACPLUS 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else {
			printf("EAACPLUS invalid samplerate(%d)\n", pconfig->sampleRate);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	} else {
		/* return error code */
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	return CVI_SUCCESS;
}

static CVI_S32 AencCheckAACLDConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32MinBitRate = 0;
	CVI_S32 s32MaxBitRate = 0;
	CVI_S32 s32RecommendRate = 0;

	if (pconfig->coderFormat == AACLD) {
		if (pconfig->nChannelsOut != pconfig->nChannelsIn) {
			printf("AACLD nChannelsOut(%d) in not equal to nChannelsIn(%d)\n",
			pconfig->nChannelsOut, pconfig->nChannelsIn);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}

		if (pconfig->sampleRate == 32000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 44100) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 44000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 48000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 64000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 16000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 192000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 8000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 16000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 24000 : 48000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 24000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 48000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 22050) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 48000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACLD 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else {
			printf("AACLD invalid samplerate(%d)\n", pconfig->sampleRate);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	} else {
		/* return error code */
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	return CVI_SUCCESS;
}

static CVI_S32 AencCheckAACELDConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32MinBitRate = 0;
	CVI_S32 s32MaxBitRate = 0;
	CVI_S32 s32RecommendRate = 0;

	if (pconfig->coderFormat == AACELD) {
		if (pconfig->nChannelsOut != pconfig->nChannelsIn) {
			printf("AACELD nChannelsOut(%d) in not equal to nChannelsIn(%d)\n",
			pconfig->nChannelsOut, pconfig->nChannelsIn);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}

		if (pconfig->sampleRate == 32000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 44100) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 48000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 16000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 8000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 24000) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else if (pconfig->sampleRate == 22050) {
			s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
			s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
			s32RecommendRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
			if (pconfig->bitRate < s32MinBitRate || pconfig->bitRate > s32MaxBitRate) {
				printf("AACELD 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",
				pconfig->bitRate, s32MinBitRate, s32MaxBitRate, s32RecommendRate);
				return CVI_ERR_AENC_ILLEGAL_PARAM;
			}
		} else {
			printf("AACELD invalid samplerate(%d)\n", pconfig->sampleRate);
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	} else {
		/* return error code */
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	return CVI_SUCCESS;
}

CVI_S32 AencAACCheckConfig(AACENC_CONFIG *pconfig)
{
	CVI_S32 s32Ret = 0;

	if (pconfig == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
		__func__, __LINE__, "pconfig is null");
		return CVI_ERR_AENC_NULL_PTR;
	}

	if (pconfig->coderFormat != AACLC && pconfig->coderFormat != EAAC && pconfig->coderFormat != EAACPLUS &&
		pconfig->coderFormat != AACLD &&
		pconfig->coderFormat != AACELD) {
		printf("aacenc coderFormat(%d) invalid\n", pconfig->coderFormat);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pconfig->quality != AU_QualityExcellent &&
		pconfig->quality != AU_QualityHigh &&
		pconfig->quality != AU_QualityMedium &&
		pconfig->quality != AU_QualityLow) {
		printf("aacenc quality(%d) invalid\n", pconfig->quality);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pconfig->bitsPerSample != 16) {
		printf("aacenc bitsPerSample(%d) should be 16\n", pconfig->bitsPerSample);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pconfig->transtype < 0 || pconfig->transtype > 2) {
		printf("invalid transtype(%d), not in [0, 2]\n", pconfig->transtype);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pconfig->bandWidth != 0 &&
	(pconfig->bandWidth < 1000 || pconfig->bandWidth > pconfig->sampleRate / 2)) {
		printf("AAC bandWidth(%d) should be 0, or 1000 ~ %d\n",
		pconfig->bandWidth, pconfig->sampleRate / 2);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pconfig->coderFormat == AACLC)
		s32Ret = AencCheckAACLCConfig(pconfig);
	else if (pconfig->coderFormat == EAAC)
		s32Ret = AencCheckEAACConfig(pconfig);
	else if (pconfig->coderFormat == EAACPLUS)
		s32Ret = AencCheckEAACPLUSConfig(pconfig);
	else if (pconfig->coderFormat == AACLD)
		s32Ret = AencCheckAACLDConfig(pconfig);
	else if (pconfig->coderFormat == AACELD)
		s32Ret = AencCheckAACELDConfig(pconfig);

	return s32Ret;
}

CVI_S32 OpenAACEncoder(CVI_VOID *pEncoderAttr, CVI_VOID **ppEncoder)
{
	AENC_AAC_ENCODER_S *pstEncoder = CVI_NULL;
	AENC_ATTR_AAC_S *pstAttr = CVI_NULL;
	CVI_S32 s32Ret;
	AACENC_CONFIG config;

	CVI_AUDIO_ASSERT(pEncoderAttr != CVI_NULL);
	CVI_AUDIO_ASSERT(ppEncoder != CVI_NULL);

	/* check attribute of encoder */
	pstAttr = (AENC_ATTR_AAC_S *)pEncoderAttr;
	s32Ret = AencCheckAACAttr(pstAttr);
	if (s32Ret) {
		printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n",
		__func__, __LINE__, s32Ret);
		return s32Ret;
	}

	/* allocate memory for encoder */
	pstEncoder = (AENC_AAC_ENCODER_S *)malloc(sizeof(AENC_AAC_ENCODER_S));

	if (pstEncoder == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n",
		__func__, __LINE__, "no memory");
		return CVI_ERR_AENC_NOMEM;
	}
	memset(pstEncoder, 0, sizeof(AENC_AAC_ENCODER_S));
	*ppEncoder = (CVI_VOID *)pstEncoder;

	/* set default config to encoder */
	s32Ret = AACInitDefaultConfig_Adp(&config);
	if (s32Ret) {
		free(pstEncoder);
		printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __func__, __LINE__, s32Ret);
		return s32Ret;
	}

	config.coderFormat = (AuEncoderFormat)pstAttr->enAACType;
	config.bitRate = pstAttr->enBitRate;
	config.bitsPerSample = 8 * (1 << (pstAttr->enBitWidth));
	config.sampleRate = pstAttr->enSmpRate;
	config.bandWidth = pstAttr->s16BandWidth; //config.sampleRate/2;
	config.transtype = (AACENCTransportType)pstAttr->enTransType;
	printf("[OpenAACEncoder_][%d][%d][%d][%d][%d][%d]\n",
		config.coderFormat,
		config.bitRate,
		config.bitsPerSample,
		config.sampleRate,
		config.bandWidth,
		config.transtype);
	if (pstAttr->enSoundMode ==  AUDIO_SOUND_MODE_MONO &&
	pstAttr->enAACType != AAC_TYPE_EAACPLUS) {
		config.nChannelsIn = 1;
		config.nChannelsOut = 1;
	} else {
		config.nChannelsIn = 2;
		config.nChannelsOut = 2;
	}

	config.quality = AU_QualityHigh;


	s32Ret = AencAACCheckConfig(&config);
	if (s32Ret) {
		free(pstEncoder);
		printf("[Func]:%s [Line]:%d #########\n", __func__, __LINE__);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	/* create encoder */
	s32Ret = AACEncoderOpen_Adp(&pstEncoder->pstAACState, &config);
	if (s32Ret) {
		free(pstEncoder);
		printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __func__, __LINE__, s32Ret);
		return s32Ret;
	}

	memcpy(&pstEncoder->stAACAttr, pstAttr, sizeof(AENC_ATTR_AAC_S));

#ifdef DUMP_AACENC
	CVI_CHAR NAMEin[256];
	CVI_CHAR NAMEout[256];

	snprintf(NAMEin, 256, "AACENC_sin_T%d_t%d.raw", config.coderFormat, config.transtype);
	snprintf(NAMEout, 256, "AACENC_sout_T%d_t%d.aac", config.coderFormat, config.transtype);
	pfdin_ENC = fopen(NAMEin, "w+");
	pfdout_ENC = fopen(NAMEout, "w+");
	cnt_aenc = 10000;
#endif
	printf("entering .....[%s][%d]\n", __func__, __LINE__);
	return CVI_SUCCESS;
}

#ifdef CVI_MODIFIED
CVI_S32 EncodeAACFrm(CVI_VOID *pEncoder, CVI_S16 *inputdata,
							CVI_U8 *pu8Outbuf,
							CVI_S32 s32InputSizeBytes, CVI_U32 *pu32OutLen)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	AENC_AAC_ENCODER_S *pstEncoder = CVI_NULL;
	CVI_U32 u32PtNums; //samples number
	//CVI_S32 i;
	CVI_U32 u32WaterLine;

	CVI_AUDIO_ASSERT(pEncoder != CVI_NULL);
	CVI_AUDIO_ASSERT(inputdata != CVI_NULL);
	CVI_AUDIO_ASSERT(pu8Outbuf != CVI_NULL);
	CVI_AUDIO_ASSERT(pu32OutLen != CVI_NULL);

	pstEncoder = (AENC_AAC_ENCODER_S *)pEncoder;


	/*WaterLine, equals to the frame sample frame of protocol*/
	if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLC)
		u32WaterLine = AACLC_SAMPLES_PER_FRAME;
	else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAAC ||
	pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAACPLUS)
		u32WaterLine = AACPLUS_SAMPLES_PER_FRAME;
	else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLD ||
		pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACELD)
		u32WaterLine = AACLD_SAMPLES_PER_FRAME;
	else {
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "invalid AAC coder type");
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}
	/* calculate point number */
	u32PtNums = s32InputSizeBytes / 2;

#ifdef DUMP_AACENC
	//if (cnt_aenc > 0)
	{
		fwrite((CVI_U8 *)aData, 1,
		(pstEncoder->stAACAttr.enSoundMode == AUDIO_SOUND_MODE_STEREO) ? u32count * 2 : u32count, pfdin_ENC);
	}
#endif

	//CVI_S32 s32InputBytes = u32WaterLine * 2;  //input bytes per frame
	s32Ret = AACEncoderFrame_Adp(pstEncoder->pstAACState, inputdata, pu8Outbuf, s32InputSizeBytes,
			(CVI_S32 *)pu32OutLen);

	if (s32Ret != CVI_SUCCESS)
		printf("[Func]:%s [Line]:%d [Info]:%su32WaterLine[%d] u32PtNums[%d]\n",
				__func__,
				__LINE__,
				"AAC encode failed",
				u32WaterLine,
				u32PtNums);

#ifdef DUMP_AACENC
	CVI_S16 aData[AACENC_BLOCKSIZE * 2 * MAX_CHANNELS];
	//printf("~~~~~~~~~strlen:%d~~~~~~\n", *((CVI_S32*)pu32OutLen));
	//if (cnt_aenc > 0)
	{
		CVI_U8 *pu8TestNum = pu8Outbuf + 1;


		fwrite((CVI_U8 *)pu8Outbuf, 1, *((CVI_S32 *)pu32OutLen), pfdout_ENC);
		cnt_aenc--;
	}
#endif
	return s32Ret;
}
#else  //original  setup
CVI_S32 EncodeAACFrm(CVI_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
			CVI_U8 *pu8Outbuf, CVI_U32 *pu32OutLen)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	AENC_AAC_ENCODER_S *pstEncoder = CVI_NULL;
	CVI_U32 u32PtNums;
	CVI_S32 i;
	CVI_S16 aData[AACENC_BLOCKSIZE * 2 * MAX_CHANNELS];
	CVI_S16 s16Len = 0;

	CVI_U32 u32WaterLine;

	CVI_AUDIO_ASSERT(pEncoder != CVI_NULL);
	CVI_AUDIO_ASSERT(pstData != CVI_NULL);
	CVI_AUDIO_ASSERT(pu8Outbuf != CVI_NULL);
	CVI_AUDIO_ASSERT(pu32OutLen != CVI_NULL);

	pstEncoder = (AENC_AAC_ENCODER_S *)pEncoder;

	if (pstEncoder->stAACAttr.enSoundMode == AUDIO_SOUND_MODE_STEREO) {
		/* whether the sound mode of frame and channel is match  */
		if (pstData->enSoundmode != AUDIO_SOUND_MODE_STEREO) {
			printf("[Func]:%s [Line]:%d [Info]:%s\n",
			__func__, __LINE__,
			"AAC encode receive a frame which not match its Soundmode");
			return CVI_ERR_AENC_ILLEGAL_PARAM;
		}
	}

	/*WaterLine, equals to the frame sample frame of protocol*/
	if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLC)
		u32WaterLine = AACLC_SAMPLES_PER_FRAME;
	else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAAC ||
	pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAACPLUS)
		u32WaterLine = AACPLUS_SAMPLES_PER_FRAME;
	else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLD ||
		pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACELD)
		u32WaterLine = AACLD_SAMPLES_PER_FRAME;
	else {
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "invalid AAC coder type");
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}
	/* calculate point number */
	u32PtNums = pstData->u32Len / (pstData->enBitwidth + 1);

	/*if frame sample larger than protocol sample, reject to receive, or buffer will be overflow*/
	if (u32PtNums != u32WaterLine) {
		printf("[Func]:%s [Line]:%d [Info]:invalid u32PtNums%d for AACType:%d\n",
			   __func__, __LINE__, u32PtNums, pstEncoder->stAACAttr.enAACType);
		return CVI_ERR_AENC_ILLEGAL_PARAM;
	}

	/* AAC encoder need interleaved data,here change LLLRRR to LRLRLR */
	/* AACLC will encode 1024*2 point, and AACplus encode 2048*2 point*/
	if (pstEncoder->stAACAttr.enSoundMode == AUDIO_SOUND_MODE_STEREO) {
		s16Len = u32WaterLine;
		for (i = s16Len - 1; i >= 0; i--) {
			aData[2 * i] = *((CVI_S16 *)pstData->u64VirAddr[0] + i);
			aData[2 * i + 1] = *((CVI_S16 *)pstData->u64VirAddr[1] + i);
		}
	} else {
		CVI_S16 *temp = (CVI_S16 *)pstData->u64VirAddr[0];

		s16Len = u32WaterLine;
		for (i = s16Len - 1; i >= 0; i--)
			aData[i] = *(temp + i);

	}

#ifdef DUMP_AACENC
	//if (cnt_aenc > 0)
	{
		fwrite((CVI_U8 *)aData, 1,
		(pstEncoder->stAACAttr.enSoundMode == AUDIO_SOUND_MODE_STEREO) ? u32count * 2 : u32count, pfdin_ENC);
	}
#endif

	CVI_S32 s32InputBytes = u32WaterLine * 2;  //input bytes per frame

	s32Ret = AACEncoderFrame_Adp(pstEncoder->pstAACState, aData, pu8Outbuf, s32InputBytes,
			(CVI_S32 *)pu32OutLen);
	if (s32Ret != CVI_SUCCESS)
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "AAC encode failed");

#ifdef DUMP_AACENC
	//printf("~~~~~~~~~strlen:%d~~~~~~\n", *((CVI_S32*)pu32OutLen));
	//if (cnt_aenc > 0)
	{
		CVI_U8 *pu8TestNum = pu8Outbuf + 1;


		fwrite((CVI_U8 *)pu8Outbuf, 1, *((CVI_S32 *)pu32OutLen), pfdout_ENC);
		cnt_aenc--;
	}
#endif
	return s32Ret;
}
#endif

CVI_S32 CloseAACEncoder(CVI_VOID *pEncoder)
{
	AENC_AAC_ENCODER_S *pstEncoder = CVI_NULL;

	CVI_AUDIO_ASSERT(pEncoder != CVI_NULL);
	pstEncoder = (AENC_AAC_ENCODER_S *)pEncoder;

	AACEncoderClose_Adp(pstEncoder->pstAACState);

	free(pstEncoder);

#ifdef DUMP_AACENC
	if (pfdin_ENC) {
		fclose(pfdin_ENC);
		pfdin_ENC = CVI_NULL;
	}

	if (pfdout_ENC) {
		fclose(pfdout_ENC);
		pfdout_ENC = CVI_NULL;
	}
#endif
	return CVI_SUCCESS;
}

CVI_S32 OpenAACDecoder(CVI_VOID *pDecoderAttr, CVI_VOID **ppDecoder)
{
	ADEC_AAC_DECODER_S *pstDecoder = CVI_NULL;
	ADEC_ATTR_AAC_S *pstAttr = CVI_NULL;
	CVI_BOOL bRawMode = CVI_FALSE;
	// if bRawMode is CVI_TRUE, means there are no any headers in stream.

	CVI_AUDIO_ASSERT(pDecoderAttr != CVI_NULL);
	CVI_AUDIO_ASSERT(ppDecoder != CVI_NULL);

	pstAttr = (ADEC_ATTR_AAC_S *)pDecoderAttr;

	/* allocate memory for decoder */
	pstDecoder = (ADEC_AAC_DECODER_S *)malloc(sizeof(ADEC_AAC_DECODER_S));
	if (pstDecoder == CVI_NULL) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "no memory");
		return CVI_ERR_ADEC_NOMEM;
	}
	memset(pstDecoder, 0, sizeof(ADEC_AAC_DECODER_S));
	*ppDecoder = (CVI_VOID *)pstDecoder;

	/* create decoder */

	if (bRawMode == CVI_FALSE) {
		printf("~~~~~~~~ADEC trans type:%d ~~~~~~~~~\n", pstAttr->enTransType);
		pstDecoder->pstAACState = AACInitDecoder_Adp((AACDECTransportType)pstAttr->enTransType);
	if (!pstDecoder->pstAACState) {
		free(pstDecoder);
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "AACInitDecoder failed");
		return CVI_ERR_ADEC_DECODER_ERR;
	}
	} else {
		printf("~~~~~~~~ADEC trans type: No header / Raw Mode~~~~~~~~~\n");
		pstDecoder->pstAACState = AACInitDecoder_Adp((AACDECTransportType)AAC_TRANS_TYPE_BUTT);
		if (!pstDecoder->pstAACState) {
			free(pstDecoder);
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "AACInitDecoder failed");
			return CVI_ERR_ADEC_DECODER_ERR;
		}

		AACSetRawMode_Adp(pstDecoder->pstAACState,
								(CVI_S32)(pstAttr->enSoundMode + 1),
								(CVI_S32)pstAttr->enSmpRate);

	}



	memcpy(&pstDecoder->stAACAttr, pstAttr, sizeof(ADEC_ATTR_AAC_S));

#ifdef DUMP_AACDEC
	CVI_CHAR NAMEin[256];
	CVI_CHAR NAMEout[256];
	CVI_CHAR NAMEout_L[256];

	snprintf(NAMEin, 256, "AACDEC_sin_t%d.aac", pstAttr->enTransType);
	snprintf(NAMEout, 256, "AACDEC_sout_t%d.raw", pstAttr->enTransType);
	snprintf(NAMEout_L, 256, "AACDEC_sout_t%d_L.raw", pstAttr->enTransType);

	pfdin_DEC = fopen(NAMEin, "w+");
	pfdout_DEC = fopen(NAMEout, "w+");
	pfdout_DEC_L = fopen(NAMEout_L, "w+");
	cnt_adec = 10000;
#endif
	return CVI_SUCCESS;
}

CVI_S32 DecodeAACFrm(CVI_VOID *pDecoder, CVI_U8 **pu8Inbuf, CVI_S32 *ps32LeftByte,
			CVI_U16 *pu16Outbuf, CVI_U32 *pu32OutLen, CVI_U32 *pu32Chns)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	ADEC_AAC_DECODER_S *pstDecoder = CVI_NULL;
	CVI_S32 s32Samples, s32SampleBytes;
	AACFrameInfo aacFrameInfo;

	CVI_AUDIO_ASSERT(pDecoder != CVI_NULL);
	CVI_AUDIO_ASSERT(pu8Inbuf != CVI_NULL);
	CVI_AUDIO_ASSERT(ps32LeftByte != CVI_NULL);
	CVI_AUDIO_ASSERT(pu16Outbuf != CVI_NULL);
	CVI_AUDIO_ASSERT(pu32OutLen != CVI_NULL);
	CVI_AUDIO_ASSERT(pu32Chns != CVI_NULL);

	pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

#ifdef DUMP_AACDEC
	CVI_U32 u32PreLen = *ps32LeftByte;
#endif

	/*Notes: pInbuf will updated*/
	s32Ret = AACDecodeFrame_Adp(pstDecoder->pstAACState, pu8Inbuf, ps32LeftByte,
		(CVI_S16 *)pu16Outbuf);


	if (s32Ret) {
		printf("aac decoder failed!, s32Ret:0x%x\n", s32Ret);
		return s32Ret;
	}

	AACGetLastFrameInfo_Adp(pstDecoder->pstAACState, &aacFrameInfo);
	aacFrameInfo.nChans = ((aacFrameInfo.nChans != 0) ? aacFrameInfo.nChans : 1);
	/* samples per frame of one sound track*/
	s32Samples = aacFrameInfo.outputSamps;

	if ((s32Samples != AACLC_SAMPLES_PER_FRAME) &&
	(s32Samples != AACPLUS_SAMPLES_PER_FRAME) &&
	(s32Samples != AACLD_SAMPLES_PER_FRAME)) {
		printf("aac decoder failed! [%d]\n", s32Samples);
		return CVI_ERR_ADEC_DECODER_ERR;
	}

	s32SampleBytes = s32Samples * sizeof(CVI_U16) * aacFrameInfo.nChans;
	*pu32Chns = aacFrameInfo.nChans;
	//printf("s32Samples[%d] pu32Chns[%d]\n", s32Samples, *pu32Chns);
	*pu32OutLen = s32SampleBytes;

#ifdef DUMP_AACDEC
	//if (cnt_adec > 0) {
		//printf("Dec: ~~~~~~~~(%d, %d)~~~~~~~\n", aacFrameInfo.outputSamps, *pu32OutLen);
		fwrite((CVI_U8 *)pu8BaseBuf, 1, (u32PreLen - (*ps32LeftByte)), pfdin_DEC);
		fwrite((CVI_U8 *)pu16Outbuf, 1, (*pu32OutLen) * (*pu32Chns), pfdout_DEC);
		fwrite((CVI_U8 *)pu16Outbuf, 1, (*pu32OutLen), pfdout_DEC_L);
		cnt_adec--;
	}
#endif

	/* NOTICE: our audio frame format is same as AAC decoder L/L/L/... R/R/R/...*/
	return s32Ret;
}

CVI_S32 GetAACFrmInfo(CVI_VOID *pDecoder, CVI_VOID *pInfo)
{
	ADEC_AAC_DECODER_S *pstDecoder = CVI_NULL;
	AACFrameInfo aacFrameInfo;
	AAC_FRAME_INFO_S *pstAacFrm = CVI_NULL;

	CVI_AUDIO_ASSERT(pDecoder != CVI_NULL);
	CVI_AUDIO_ASSERT(pInfo != CVI_NULL);

	pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;
	pstAacFrm = (AAC_FRAME_INFO_S *)pInfo;

	AACGetLastFrameInfo_Adp(pstDecoder->pstAACState, &aacFrameInfo);

	pstAacFrm->s32Samplerate = aacFrameInfo.sampRateOut;
	pstAacFrm->s32BitRate = aacFrameInfo.bitRate;
	pstAacFrm->s32Profile = aacFrameInfo.profile;
	pstAacFrm->s32TnsUsed = aacFrameInfo.tnsUsed;
	pstAacFrm->s32PnsUsed = aacFrameInfo.pnsUsed;

	return aacFrameInfo.bytespassDec;
}

CVI_S32 CloseAACDecoder(CVI_VOID *pDecoder)
{
	ADEC_AAC_DECODER_S *pstDecoder = CVI_NULL;

	CVI_AUDIO_ASSERT(pDecoder != CVI_NULL);
	pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

	AACFreeDecoder_Adp(pstDecoder->pstAACState);

	free(pstDecoder);

#ifdef DUMP_AACDEC
	if (pfdin_DEC) {
		fclose(pfdin_DEC);
		pfdin_DEC = CVI_NULL;
	}

	if (pfdout_DEC) {
		fclose(pfdout_DEC);
		pfdout_DEC = CVI_NULL;
	}

	if (pfdout_DEC_L) {
		fclose(pfdout_DEC_L);
		pfdout_DEC_L = CVI_NULL;
	}
#endif

	return CVI_SUCCESS;
}

CVI_S32 ResetAACDecoder(CVI_VOID *pDecoder)
{
	ADEC_AAC_DECODER_S *pstDecoder = CVI_NULL;

	CVI_AUDIO_ASSERT(pDecoder != CVI_NULL);
	pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

	AACFreeDecoder_Adp(pstDecoder->pstAACState);

	/* create decoder */
	pstDecoder->pstAACState = AACInitDecoder_Adp((AACDECTransportType)pstDecoder->stAACAttr.enTransType);
	if (!pstDecoder->pstAACState) {
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __func__, __LINE__, "AACResetDecoder failed");
		return CVI_ERR_ADEC_DECODER_ERR;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_MPI_AENC_AacInit(CVI_VOID)
{
	CVI_S32 s32Handle, s32Ret;
#ifdef SUPPORT_EXTERNAL_AAC
	AAC_AENC_ENCODER_S stAacEnc;

	s32Ret = InitAacAencLib();
	if (s32Ret != CVI_SUCCESS) {
		printf("Error in AAC encode[%d]\n", __LINE__);
		return 0;
	}

	stAacEnc.enType = PT_AAC;
	snprintf(stAacEnc.aszName, sizeof(stAacEnc.aszName), "Aac");
	//printf("start register AAC encoder xxxxxxxxxxxxxxx\n");
	//printf("PT_AAC[%d]\n", (int)PT_AAC);
	stAacEnc.u32MaxFrmLen = 100;//MAX_AAC_MAINBUF_SIZE;
	stAacEnc.pfnOpenEncoder = OpenAACEncoder;
	stAacEnc.pfnEncodeFrm = EncodeAACFrm;
	stAacEnc.pfnCloseEncoder = CloseAACEncoder;
	s32Ret = CVI_AENC_RegisterExternalEncoder(&s32Handle, &stAacEnc);
	if (s32Ret)
		return s32Ret;

	printf("start register AAC encoder end\n");
	g_AacEncHandle = s32Handle;
	return CVI_SUCCESS;
#else
	printf("[Warning]Not loading external AAC lib to cvi_aenc\n");
	return CVI_FAILURE;

#endif

}

CVI_S32 CVI_MPI_AENC_AacDeInit(CVI_VOID)
{
	CVI_S32 s32Ret;
#ifdef SUPPORT_EXTERNAL_AAC
	s32Ret = CVI_AENC_UnRegisterExternalEncoder(g_AacEncHandle);
	if (s32Ret)
		return s32Ret;

	DeInitAacAencLib();
#endif
	return CVI_SUCCESS;
}

CVI_S32 CVI_MPI_ADEC_AacInit(CVI_VOID)
{
	CVI_S32 s32Handle, s32Ret;
#ifdef SUPPORT_EXTERNAL_AAC
	ADEC_DECODER_S stAacDec;

	s32Ret = InitAacAdecLib();
	if (s32Ret)
		return s32Ret;

	stAacDec.enType = PT_AAC;
	snprintf(stAacDec.aszName, sizeof(stAacDec.aszName), "Aac");
	//printf("start register AAC encoder xxxxxxxxxxxxxxx\n");
	//printf("PT_AAC[%d]\n", (int)PT_AAC);
	stAacDec.pfnOpenDecoder = OpenAACDecoder;
	stAacDec.pfnDecodeFrm = DecodeAACFrm;
	stAacDec.pfnGetFrmInfo = GetAACFrmInfo;
	stAacDec.pfnCloseDecoder = CloseAACDecoder;
	stAacDec.pfnResetDecoder = ResetAACDecoder;
	s32Ret = CVI_ADEC_RegisterExternalDecoder(&s32Handle, &stAacDec);
	if (s32Ret)
		return s32Ret;

	printf("start register AAC decoder end\n");
	g_AacDecHandle = s32Handle;
	return CVI_SUCCESS;
#else
	printf("[Warning]Not loading external AAC lib to cvi_adec\n");
	return CVI_FAILURE;

#endif

}

CVI_S32 CVI_MPI_ADEC_AacDeInit(CVI_VOID)
{
	CVI_S32 s32Ret;
#ifdef SUPPORT_EXTERNAL_AAC
	s32Ret = CVI_ADEC_UnRegisterExternalDecoder(g_AacDecHandle);
	if (s32Ret)
		return s32Ret;

	DeInitAacAdecLib();
#endif
	return CVI_SUCCESS;
}

//#if 1//def CVI_MODIFIED
#define FILE_NAME_LEN	128
#define AUDIO_BUFFER_MAX  (50 * 60 * 1024)
char *input_filename;
char output_filename[FILE_NAME_LEN] = {0};
bool bEncodeAAC;
bool bDecodeAAC;
AENC_ATTR_AAC_S aac_config;
ADEC_ATTR_AAC_S aacdec_config;

#ifndef SUPPORT_EXTERNAL_AAC
static CVI_BOOL _cvi_checkname_iswav(char *infilename)
{
	CVI_S32 s32InputFileLen = 0;

	s32InputFileLen = strlen(infilename);

	if (s32InputFileLen == 0) {
		printf("No Input File Name..force return\n");
		return 0;
	}

	if (infilename[s32InputFileLen-4] == '.' &&
	(infilename[s32InputFileLen-3] == 'W' || infilename[s32InputFileLen-3] == 'w') &&
	(infilename[s32InputFileLen-2] == 'A' || infilename[s32InputFileLen-2] == 'a') &&
	(infilename[s32InputFileLen-1] == 'V' || infilename[s32InputFileLen-1] == 'v')) {
		printf("Enter wav file\n");
		return CVI_TRUE;
	} else
		return CVI_FALSE;

}
//#endif
#ifndef SUPPORT_EXTERNAL_AAC
//flag on : user can build the code in sample layer(Verdor code layer)
//flag off : user can test this in aac_sample by aac_sample/Makefile
static int _aac_transcode_option(void)
{
	char getcharlocal[100];
	int getvalue;
	//config struct recommend to
	// follow the AENC_ATTR_AAC_S  in cvi_audio_aac_adp.h
	printf("\n");
	printf("Enter Header Type:\n");
	printf("0:TYPE_ADTS\n");
	printf("1:TYPE_LOAS\n");
	printf("2:TYPE_LATM_MCP1\n");
	printf("3:[DECODE]No Header Raw Mode\n");
	fgets(getcharlocal, 10, stdin);
	getvalue = atoi(getcharlocal);
	if (getvalue == 3) {
		printf("Warning ...this only support decode raw mode\n");
		printf("AAC encode not support raw mode\n");
		if (bEncodeAAC) {
			printf("[Error]Encode not support Raw mode\n");
			return (-1);
		}
	}
	if (getvalue > AAC_TRANS_TYPE_BUTT) {
		printf("[Error] Wrong input option[%d]\n", getvalue);
		printf("\n");
		return (-1);
	}
	printf("\n");
	aac_config.enTransType = (AAC_TRANS_TYPE_E)getvalue;
	aacdec_config.enTransType = (AAC_TRANS_TYPE_E)getvalue;

	if (bEncodeAAC) {
		printf("Enter sample rate:\n");
		fgets(getcharlocal, 10, stdin);
		getvalue = atoi(getcharlocal);

		//#endif
		aac_config.enSmpRate = (AAC_BPS_E)getvalue;
		printf("\n");

		printf("Enter Channel numbers(sound mode)  1 or 2\n");
		fgets(getcharlocal, 10, stdin);
		getvalue = atoi(getcharlocal);
		if ((getvalue < 1) || (getvalue > 2)) {
			printf("[Error]Channel numbers not support[%d]\n", getvalue);
			return (-1);
		} else if (getvalue == 1)
			aac_config.enSoundMode = AUDIO_SOUND_MODE_MONO;
		else if (getvalue == 2)
			aac_config.enSoundMode = AUDIO_SOUND_MODE_STEREO;
		printf("\n");

		printf("Enter bitwidth[8/16/24/32]\n");
		printf("Only support 16 bits for now..\n");
		aac_config.enBitWidth = AUDIO_BIT_WIDTH_16;

		printf("Enter Bit Rate ....\n");
		printf("8000\n");
		printf("16000\n");
		printf("22000\n");
		printf("24000\n");
		printf("32000\n");
		printf("48000\n");
		printf("64000\n");
		printf("96000\n");
		printf("128000\n");
		printf("256000\n");
		printf("320000\n");
		printf("----------suggestion----------------------\n");
		printf("AAC bitrate (LC:16000~32000, EAAC:24000~128000,\n");
		printf("EAAC+:16000~64000, AACLD:16000~320000, AACELD:32000~320000)\n");
		printf("Please enter the numbers:\t");
		fgets(getcharlocal, 10, stdin);
		getvalue = atoi(getcharlocal);
		if (getvalue > AAC_BPS_BUTT  || getvalue < AAC_BPS_8K) {
			printf("Not support bit rate[%d]\n", getvalue);
			return  (-1);
		}

		aac_config.enBitRate = (AAC_BPS_E)getvalue;
		printf("\n");

		printf("Enter AAC type:\n");
		printf("0:AAC_TYPE_AACLC\n");
		printf("1:AAC_TYPE_EAAC\n");
		printf("2:AAC_TYPE_EAACPLUS\n");
		printf("3:AAC_TYPE_AACLD\n");
		printf("4:AAC_TYPE_AACELD\n");
		printf("Enter 0~4\t");
		fgets(getcharlocal, 10, stdin);
		getvalue = atoi(getcharlocal);
		if (getvalue < 0 || getvalue >= AAC_TYPE_BUTT) {
			printf("[Error]Not valid input [%d]\n", getvalue);
			return (-1);
			//force return
		} else { //output_filename
			switch (getvalue) {
			case 0:
				snprintf(output_filename, 128, "%s.aaclc", input_filename);
				break;
			case 1:
				snprintf(output_filename, 128, "%s.eaac", input_filename);
				break;
			case 2:
				snprintf(output_filename, 128, "%s.eaacplus", input_filename);
				break;
			case 3:
				snprintf(output_filename, 128, "%s.aacld", input_filename);
				break;
			case 4:
				snprintf(output_filename, 128, "%s.aaceld", input_filename);
				break;
			default:
				snprintf(output_filename, 128, "%s.aac_type", input_filename);
				break;
			}
		}
		aac_config.enAACType = (AAC_TYPE_E)getvalue;
		printf("\n");
		aac_config.s16BandWidth = (int)aac_config.enSmpRate / 2;
	} else if (bDecodeAAC) {
		printf("\n");
		printf("Enter Channel numbers(sound mode)  1 or 2\n");
		fgets(getcharlocal, 10, stdin);
		getvalue = atoi(getcharlocal);
		if ((getvalue < 1) || (getvalue > 2)) {
			printf("[Error]Channel numbers not support[%d]\n", getvalue);
			return (-1);
		} else if (getvalue == 1)
			aacdec_config.enSoundMode = AUDIO_SOUND_MODE_MONO;
		else if (getvalue == 2)
			aacdec_config.enSoundMode = AUDIO_SOUND_MODE_STEREO;
		printf("\n");

		if (aacdec_config.enTransType == AAC_TRANS_TYPE_BUTT) {
			printf("Enter AAC decoder Raw mode\n");
			printf("Enter sample rate:\n");
			fgets(getcharlocal, 10, stdin);
			getvalue = atoi(getcharlocal);
			aacdec_config.enSmpRate = (AAC_BPS_E)getvalue;
		}

		snprintf(output_filename, 128, "%s_decout.raw", input_filename);

		} else {
			printf("[Error]Unknown type for AAC ...force exit\n");
			return (-1);
	}

	return 0;
}
#endif

#ifndef SUPPORT_EXTERNAL_AAC
int main(int argc, char **argv)
{
	FILE *pfd_in = NULL;
	FILE *pfd_out = NULL;
	char s_option[100];
	int s32Ret = (-1);
	int cmd = (-1);
	CVI_BOOL bIsWav = CVI_FALSE;

	bEncodeAAC = false;
	bDecodeAAC = false;
	memset(&aac_config, 0, sizeof(AENC_ATTR_AAC_S));
	memset(&aacdec_config, 0, sizeof(ADEC_ATTR_AAC_S));


	do {
		printf("====================================\n");
		printf("sample_audio_aac usage :\n");
		printf("sample_audio_aac  [input_file_name.wav/.raw]\n");
		printf("Enter(Choose) the following option displayed on the terminal\n");
		printf("The program will show result filename after AAC encode or Decode\n");
		printf("====================================\n");
	} while (0);

	if (argc > 2)
		printf("Enter command debug mode\n");

	if (argc < 2) {
		printf("[Error]Not enough input\n");
		return 0;
	}

	input_filename = argv[1];
	if (access(input_filename, 0) <  0) {
		printf("[Error]check file[%s] not exist!....error\n", input_filename);
		return (-1);
	}

	bIsWav = _cvi_checkname_iswav(input_filename);
	if (bIsWav == CVI_TRUE) {
		printf("[Error] The test unit only supprot encoded file type or raw format\n");
		return (-1);
	}
	pfd_in = fopen(input_filename, "rb");

	do {
		printf("Choose 1:Encode or 2:Decode :");
		fgets(s_option, 10, stdin);
		cmd = atoi(s_option);
		printf("select[%d]\n", cmd);
	} while ((cmd != 1) && (cmd != 2));

	printf("\n");
	if (cmd == 1) {
		bEncodeAAC = true;
		bDecodeAAC = false;
	} else if (cmd == 2) {
		bDecodeAAC = true;
		bEncodeAAC = false;
	} else {
		printf("[Error]Option failure..force return\n");
		return 0;
	}

	s32Ret = _aac_transcode_option();
	if (s32Ret != CVI_SUCCESS) {
		printf("[Error]setting AAC transcode option abnormal...!\n");
		return 0;
	}

	//AAC encode sample code ----------------------start
	CVI_CHAR *audio_buffer =  (CVI_CHAR *)malloc(AUDIO_BUFFER_MAX);
	CVI_CHAR *pOutputBUf =  (CVI_CHAR *)malloc(AUDIO_BUFFER_MAX);

	if (bEncodeAAC == true) {
		CVI_VOID *aacenc_handle;
		CVI_S32 s32framesize = 0;
		CVI_S32 s32Cnt = 0;
		CVI_S32  s32OutLen = 0;

		pfd_in = fopen(input_filename, "rb");
		pfd_out = fopen(output_filename, "wb");
		if (((pfd_in) == NULL) || ((pfd_out) == NULL)) {
			printf(" [Error]Cannot open input / output file\n");
			return 0;
		}

		s32Ret = InitAacAencLib();
		if (s32Ret != CVI_SUCCESS) {
			printf("Error in AAC encode[%d]\n", __LINE__);
			return 0;
		}

		s32Ret = OpenAACEncoder((CVI_VOID *)(&aac_config), (CVI_VOID **)(&aacenc_handle));
		if (s32Ret != CVI_SUCCESS) {
			printf("Error in AAC encode[%d]\n", __LINE__);
			return (-1);

		} else {
			printf("aacenc_handle[0x%x]\n", aacenc_handle);
		}

		if (aac_config.enAACType == AAC_TYPE_AACLC)
			s32framesize = AACLC_SAMPLES_PER_FRAME;
		else if (aac_config.enAACType == AAC_TYPE_EAAC ||
				aac_config.enAACType == AAC_TYPE_EAACPLUS)
			s32framesize = AACPLUS_SAMPLES_PER_FRAME;
		else if (aac_config.enAACType == AAC_TYPE_AACLD ||
				aac_config.enAACType == AAC_TYPE_AACELD)
			s32framesize = AACLD_SAMPLES_PER_FRAME;

		if (aac_config.enSoundMode ==  AUDIO_SOUND_MODE_STEREO)
			s32framesize = s32framesize * 2;

		printf("Encode each frame with samples size[%d]\n", s32framesize);
		//Encode bytes = suggest_sample_size x channels x bytes_per_sample
		//			= suggest_sample_size x (1 or 2) x 2
		for (;;) {
			s32Cnt++;

			if (fread(audio_buffer,
					sizeof(short),
					s32framesize, pfd_in) != s32framesize) {
				printf("leaving AAC encode...LINE[%d] s32framesize[%d]\n",
					__LINE__,
					s32framesize);
				//printf("cnt 1[%d]\n", s32Cnt);
				goto Pattern_EOF;
			} else {
				//do the aac encode frame by frame
				s32Ret = EncodeAACFrm(aacenc_handle, (CVI_S16 *)audio_buffer,
								(CVI_U8 *)pOutputBUf, s32framesize * 2, &s32OutLen);
				//printf("cnt 2[%d]\n", s32Cnt);
				if (s32Ret != CVI_SUCCESS) {
					printf("[Error]AAC encode error[%d]\n", __LINE__);
					goto Pattern_EOF_1;
				} else {
					//write to file
					printf("fwrite s32OutLen[%d] line[%d]Cnt[%d]\n", s32OutLen, __LINE__, s32Cnt);
					fwrite(pOutputBUf, 1, s32OutLen, pfd_out);
				}
			}
		}

		CloseAACEncoder(aacenc_handle);

	}
	//AAC encode sample code ----------------------end


	//AAC decode sample code ----------------------start
	CVI_CHAR *pDecOutputBuf =  (CVI_CHAR *)malloc(AUDIO_BUFFER_MAX);
	//This decode test assume that the receiving data have adts header insides streams.
	if (bDecodeAAC == true) {
		CVI_VOID *aacdec_handle = NULL;
		char *pCurr;
		char *pInput;
		char *pEndPivot;
		AACDECTransportType enTranType = AACDEC_ADTS;
		int s32totalFrameSize;
		int s32totalFrameSizeBytes;
		int input_samples = 1024;
		int output_bytes = 0;
		int byte_left = 0;
		int channel_num = 0;
		int consume_bytes = 0;
		int save_outputbytes = 0;
		int totalbytepass = 0;

		AAC_FRAME_INFO_S  AACInfo;

		pfd_in = fopen(input_filename, "rb");
		pfd_out = fopen(output_filename, "wb");
		if (((pfd_in) == NULL) || ((pfd_out) == NULL)) {
			printf(" [Error]Cannot open input / output file\n");
			return 0;
		}

		if (aac_config.enSoundMode ==  AUDIO_SOUND_MODE_STEREO)
			channel_num = 2;
		else
			channel_num = 1;


		InitAacAdecLib();
		s32Ret = OpenAACDecoder((CVI_VOID *)(&aacdec_config), (CVI_VOID **)(&aacdec_handle));
		if (s32Ret != CVI_SUCCESS) {
			printf("Error in AAC decoder[%d]\n", __LINE__);
			return (-1);
		}

		fseek(pfd_in, 0, SEEK_END);
		uint64_t fsize = ftell(pfd_in);

		fseek(pfd_in, 0, SEEK_SET);
		s32totalFrameSize = fsize/2;
		s32totalFrameSizeBytes =  sizeof(short)*s32totalFrameSize;//input bytes
		printf("AAC decode input size total_sample[%d] total_bytes[%d]\n",
				s32totalFrameSize, s32totalFrameSizeBytes);

		if (s32totalFrameSizeBytes > AUDIO_BUFFER_MAX) {
			printf("Error .. the AAC input file size too large(over 1M)\n");
			printf("Please choose shorter files\n");
			goto Pattern_EOF;
		}

		if (fread(audio_buffer,
			sizeof(short),
			s32totalFrameSize, pfd_in) != s32totalFrameSize) {
			printf("Read file buffer failure ...leaving aac decode [%d]\n", __LINE__);
			goto Pattern_EOF_1;
		}

		pCurr = audio_buffer;
		pEndPivot = pCurr + s32totalFrameSizeBytes;
		printf("version tag[%s]\n", _VERSION_TAG_AAC_ADP_);


		//while (pCurr < pEndPivot) {
		while (totalbytepass < s32totalFrameSizeBytes) {
		//while(1) {
			pInput = pCurr;
			byte_left = input_samples * 2;  //enter bytes each time
			s32Ret =  DecodeAACFrm(aacdec_handle,
									(CVI_U8 **)(&pInput),
									(CVI_S32 *)&byte_left,
									(CVI_S16 *)pDecOutputBuf,
									(CVI_U32 *)&output_bytes,
									(CVI_U32 *)&channel_num);
			if (s32Ret != CVI_SUCCESS)
				printf("[Error][%s][%d]\n", __func__, __LINE__);

			if (byte_left == input_samples * 2) {
				printf("[Error]Did not consume any bytes...try larger the input size\n");
				return (-1);
			}

			consume_bytes =  (input_samples * 2) - byte_left;
			pCurr += consume_bytes;

			save_outputbytes = output_bytes;

			if (byte_left == 0)
				printf("=========================================\n");

			fwrite(pDecOutputBuf, 1, save_outputbytes, pfd_out);

			totalbytepass =  GetAACFrmInfo(aacdec_handle, (CVI_VOID *)(&AACInfo));

			printf("AAC dec consume_bytes[%d] byte_left[%d]output_bytes[%d] totalpass bytes[%d]\n",
					consume_bytes, byte_left, output_bytes, totalbytepass);

		}
		printf("AAC decode end version\n");

	}
	//AAC decode sample code ----------------------end
Pattern_EOF_1:
	free(audio_buffer);
	free(pOutputBUf);
Pattern_EOF:
	if (bEncodeAAC)
		printf("Encode finished\n");
	if (bDecodeAAC)
		printf("Decode finished\n");

	fclose(pfd_in);
	fclose(pfd_out);
}
#endif
#endif

