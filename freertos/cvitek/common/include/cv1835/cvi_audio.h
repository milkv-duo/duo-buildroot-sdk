/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_audio.h
 * Description: basic audio api for application layer
 */

#ifndef __CVI_AUDIO_H__
#define __CVI_AUDIO_H__

#include "cvi_type.h"
#include "cvi_common.h"
#include "cvi_comm_aio.h"
#include "cvi_comm_aenc.h"
#include "cvi_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc
#ifdef CVI_MODIFIED
CVI_S32 CVI_AUDIO_DEBUG(void);
#endif

/* Audio function api. */
CVI_S32 CVI_AUDIO_INIT(void);
CVI_S32 CVI_AUDIO_DEINIT(void);
CVI_S32 CVI_AUDIO_SetModParam(const AUDIO_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_AUDIO_GetModParam(AUDIO_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_AUDIO_RegisterVQEModule(const AUDIO_VQE_REGISTER_S
					*pstVqeRegister);

CVI_S32  CVI_AUD_SYS_Bind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);
CVI_S32  CVI_AUD_SYS_UnBind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);
/* AI function api. */
CVI_S32 CVI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr);
CVI_S32 CVI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr);

CVI_S32 CVI_AI_Enable(AUDIO_DEV AiDevId);
CVI_S32 CVI_AI_Disable(AUDIO_DEV AiDevId);

CVI_S32 CVI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
CVI_S32 CVI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

CVI_S32 CVI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn,
			    AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, CVI_S32 s32MilliSec);
CVI_S32 CVI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn,
				const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

CVI_S32 CVI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       const AI_CHN_PARAM_S *pstChnParam);
CVI_S32 CVI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       AI_CHN_PARAM_S *pstChnParam);

CVI_S32 CVI_AI_SetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				    const AI_RECORDVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AI_GetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				    AI_RECORDVQE_CONFIG_S *pstVqeConfig);

CVI_S32 CVI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
CVI_S32 CVI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

CVI_S32 CVI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       AUDIO_SAMPLE_RATE_E enOutSampleRate);
CVI_S32 CVI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);

CVI_S32 CVI_AI_SetTrackMode(AUDIO_DEV AiDevId,
				AUDIO_TRACK_MODE_E enTrackMode);
CVI_S32 CVI_AI_GetTrackMode(AUDIO_DEV AiDevId,
				AUDIO_TRACK_MODE_E *penTrackMode);
CVI_S32 CVI_AI_SaveFile(AUDIO_DEV AiDevId, AI_CHN AiChn,
			    const AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);
CVI_S32 CVI_AI_QueryFileStatus(AUDIO_DEV AiDevId, AI_CHN AiChn,
				   AUDIO_FILE_STATUS_S *pstFileStatus);

CVI_S32 CVI_AI_ClrPubAttr(AUDIO_DEV AiDevId);

CVI_S32 CVI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);

CVI_S32 CVI_AI_EnableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn,
				     AUDIO_DEV AoDevId, AO_CHN AoChn);
CVI_S32 CVI_AI_DisableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn);

CVI_S32 CVI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				  AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				  AI_TALKVQE_CONFIG_S *pstVqeConfig);

/* AO function api. */
CVI_S32 CVI_BitOut_Transfer(CVI_CHAR **InPt, CVI_U32 *pu32InSizeByte,
				CVI_CHAR **OutPt,
				CVI_U32 *pu32OutSizeByte, CVI_S32 s32BitOut);
CVI_S32 CVI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S *pstAttr);
CVI_S32 CVI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr);

CVI_S32 CVI_AO_Enable(AUDIO_DEV AoDevId);
CVI_S32 CVI_AO_Disable(AUDIO_DEV AoDevId);

CVI_S32 CVI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
CVI_S32 CVI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

CVI_S32 CVI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn,
			     const AUDIO_FRAME_S *pstData, CVI_S32 s32MilliSec);

CVI_S32 CVI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn,
			       AUDIO_SAMPLE_RATE_E enInSampleRate);
CVI_S32 CVI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn);

CVI_S32 CVI_AO_ClearChnBuf(AUDIO_DEV AoDevId, AO_CHN AoChn);
CVI_S32 CVI_AO_QueryChnStat(AUDIO_DEV AoDevId, AO_CHN AoChn,
				AO_CHN_STATE_S *pstStatus);

CVI_S32 CVI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
CVI_S32 CVI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

CVI_S32 CVI_AO_SetVolume(AUDIO_DEV AoDevId, CVI_S32 s32VolumeDb);
CVI_S32 CVI_AO_GetVolume(AUDIO_DEV AoDevId, CVI_S32 *ps32VolumeDb);

CVI_S32 CVI_AO_SetMute(AUDIO_DEV AoDevId, CVI_BOOL bEnable,
			   const AUDIO_FADE_S *pstFade);
CVI_S32 CVI_AO_GetMute(AUDIO_DEV AoDevId, CVI_BOOL *pbEnable,
			   AUDIO_FADE_S *pstFade);

CVI_S32 CVI_AO_SetTrackMode(AUDIO_DEV AoDevId,
				AUDIO_TRACK_MODE_E enTrackMode);
CVI_S32 CVI_AO_GetTrackMode(AUDIO_DEV AoDevId,
				AUDIO_TRACK_MODE_E *penTrackMode);

CVI_S32 CVI_AO_GetFd(AUDIO_DEV AoDevId, AO_CHN AoChn);

CVI_S32 CVI_AO_ClrPubAttr(AUDIO_DEV AoDevId);

CVI_S32 CVI_AO_SetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn,
			      const AO_VQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AO_GetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn,
			      AO_VQE_CONFIG_S *pstVqeConfig);

CVI_S32 CVI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
CVI_S32 CVI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);

/* AENC function api. */
CVI_S32 CVI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
CVI_S32 CVI_AENC_DestroyChn(AENC_CHN AeChn);

CVI_S32 CVI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm,
			       const AEC_FRAME_S *pstAecFrm);

CVI_S32 CVI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream,
			       CVI_S32 s32MilliSec);
CVI_S32 CVI_AENC_ReleaseStream(AENC_CHN AeChn,
				   const AUDIO_STREAM_S *pstStream);

CVI_S32 CVI_AENC_GetFd(AENC_CHN AeChn);

CVI_S32 CVI_AENC_GetStreamBufInfo(AENC_CHN AeChn, CVI_U64 *pu64PhysAddr,
				      CVI_U32 *pu32Size);

CVI_S32 CVI_AENC_SetMute(AENC_CHN AeChn, CVI_BOOL bEnable);
CVI_S32 CVI_AENC_GetMute(AENC_CHN AeChn, CVI_BOOL *pbEnable);
CVI_S32 CVI_AENC_RegisterExternalEncoder(CVI_S32 *ps32Handle, const AAC_AENC_ENCODER_S *pstEncoder);
CVI_S32 CVI_AENC_UnRegisterExternalEncoder(CVI_S32 s32Handle);
/* ADEC function api. */
CVI_S32 CVI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr);
CVI_S32 CVI_ADEC_DestroyChn(ADEC_CHN AdChn);

CVI_S32 CVI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream,
				CVI_BOOL bBlock);

CVI_S32 CVI_ADEC_ClearChnBuf(ADEC_CHN AdChn);

CVI_S32 CVI_ADEC_RegisterExternalDecoder(CVI_S32 *ps32Handle,
				     const ADEC_DECODER_S *pstDecoder);
CVI_S32 CVI_ADEC_UnRegisterExternalDecoder(CVI_S32 s32Handle);

CVI_S32 CVI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_INFO_S *pstFrmInfo,
			      CVI_BOOL bBlock);
CVI_S32 CVI_ADEC_ReleaseFrame(ADEC_CHN AdChn,
				  const AUDIO_FRAME_INFO_S *pstFrmInfo);
CVI_S32 CVI_ADEC_SendEndOfStream(ADEC_CHN AdChn, CVI_BOOL bInstant);
CVI_S32 CVI_ADEC_QueryChnStat(ADEC_CHN AdChn,
				  ADEC_CHN_STATE_S *pstBufferStatus);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_AUDIO_H__ */

