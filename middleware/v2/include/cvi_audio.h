/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_audio.h
 * Description: basic audio api for application layer
 */

#ifndef __CVI_AUDIO_H__
#define __CVI_AUDIO_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include <string.h>

#include "cvi_comm_aio.h"
#include "cvi_comm_aenc.h"
#include "cvi_comm_adec.h"
#include <stdlib.h>

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
//define print level -------------------------------------[start]
#define CVI_AUD_MASK_ERR	(0x00)
#define CVI_AUD_MASK_INFO	(0x01)
#define CVI_AUD_MASK_DBG	(0x02)

/* =====Audio function api===== */

/**
 * @brief Audio init
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AUDIO_INIT(void);

/**
 * @brief Audio deinit
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AUDIO_DEINIT(void);

/**
 * @brief Set the binding properties of the audio
 * @param pstSrcChn Source information of bind mode
 * @param pstDestChn Target information of bind mode
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32  CVI_AUD_SYS_Bind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);

/**
 * @brief Unregister the binding mode of the system
 * @param pstSrcChn Source information of bind mode
 * @param pstDestChn Target information of bind mode
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32  CVI_AUD_SYS_UnBind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);
/* AI function api.  */

/**
 * @brief Set the parameter to audio
 * @param AiDevId the number of audio device
 * @param pstAttr pointer that describes an ai property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr);

/**
 * @brief Get the parameter to audio
 * @param AiDevId the number of audio device
 * @param pstAttr pointer that describes an ai property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr);

/**
 * @brief Create and initial ai device.
 * @param AiDevId the number of audio device
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_Enable(AUDIO_DEV AiDevId);

/**
 * @brief Destroy ai device
 * @param AiDevId the number of audio device
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_Disable(AUDIO_DEV AiDevId);

/**
 * @brief Create and initial ai channel.
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

/**
 * @brief Destroy ai channels
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

/**
 * @brief Record and get audio data
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param pstFrm the starting address of the obtained audio data
 * @param pstAecFrm the starting address of the obtained Reference signal
 * @param s32MilliSec -1:block mode/0:non-blocking/s32MilliSec
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn,
			    AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, CVI_S32 s32MilliSec);

/**
 * @brief Release record frame
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param pstFrm the starting address of the obtained audio data
 * @param pstAecFrm the starting address of the obtained Reference signal
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn,
			const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

/**
 * @brief Sets the frame depth of audio data
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param pstChnParam frame depth struct
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       const AI_CHN_PARAM_S *pstChnParam);
/**
 * @brief Get the frame depth of audio data
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param pstChnParam frame depth struct
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       AI_CHN_PARAM_S *pstChnParam);
/**
 * @brief Set record volume
 * @param AiDevId the number of audio device
 * @param s32VolumeStep input gain
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetVolume(AUDIO_DEV AiDevId, CVI_S32 s32VolumeStep);

/**
 * @brief Get record volume
 * @param AiDevId the number of audio device
 * @param ps32VolumeStep pointer to s32VolumeStep that indicates ai gain
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetVolume(AUDIO_DEV AiDevId, CVI_S32 *ps32VolumeStep);
/**
 * @brief Set record volume
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param s32VolumeDb input gain [0-100]
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, CVI_S32 s32VolumeDb);
/**
 * @brief Get record volume
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param ps32VolumeDb pointer to s32VolumeStep that indicates ai gain
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, CVI_S32 *ps32VolumeDb);
/**
 * @brief Start Vqe(AGC+ANR+AES+AEC)
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

/**
 * @brief Destroy Vqe(AGC+ANR+AES+AEC)
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

/**
 * @brief Config Vqe(AGC+ANR+AES+AEC)
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param u32Mask Select mask for vqe function
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_VqeFunConfig(AUDIO_DEV AiDevId, AI_CHN AiChn, int u32Mask);
/**
 * @brief Start resample
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param enOutSampleRate target sample rate
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn,
			       AUDIO_SAMPLE_RATE_E enOutSampleRate);
/**
 * @brief Destroy resample
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);

/**
 * @brief Set audio input track status
 * @param AiDevId the number of audio device
 * @param enTrackMode the mode of track
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetTrackMode(AUDIO_DEV AiDevId,
				AUDIO_TRACK_MODE_E enTrackMode);
/**
 * @brief Get audio input track status
 * @param AiDevId the number of audio device
 * @param penTrackMode the mode of track
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetTrackMode(AUDIO_DEV AiDevId,
				AUDIO_TRACK_MODE_E *penTrackMode);


/**
 * @brief Empty the parameters
 * @param AiDevId the number of audio device
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_ClrPubAttr(AUDIO_DEV AiDevId);

/**
 * @brief Set vqe attr and enable talk vqe
 * @param AiDevId the number of input device
 * @param AiChn the number of input channels
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @param pstVqeConfig the config of vqe
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				  AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig);

/**
 * @brief Get vqe attr
 * @param AiDevId the number of audio device
 * @param AiChn the number of audio channels
 * @param pstVqeConfig the config of vqe
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn,
				  AI_TALKVQE_CONFIG_S *pstVqeConfig);

/* AO function api.  */

/**
 * @brief Transfer data bitdepth
 * @param InPt input buffer addr
 * @param pu32InSizeByte input buffer size
 * @param OutPt output buffer addr
 * @param pu32OutSizeByte output buffer size
 * @param s32BitOut target bitdepth to transfer only support 24 bit / 32 bit
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_BitOut_Transfer(CVI_CHAR **InPt, CVI_U32 *pu32InSizeByte,
				CVI_CHAR **OutPt,
				CVI_U32 *pu32OutSizeByte, CVI_S32 s32BitOut);
/**
 * @brief Set the parameter to audio output
 * @param AoDevId the number of output device
 * @param pstAttr pointer that describes an ao property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S *pstAttr);

/**
 * @brief Get the parameter to audio output
 * @param AoDevId the number of output device
 * @param pstAttr pointer that describes an ao property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr);

/**
 * @brief Create and initial ao device.
 * @param AoDevId the number of output device
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_Enable(AUDIO_DEV AoDevId);
/**
 * @brief Destroy ao device
 * @param AoDevId the number of output device
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_Disable(AUDIO_DEV AoDevId);
/**
 * @brief Create and initial ao channel.
 * @param AoDevId the number of ao device
 * @param AoChn the number of audio channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
 * @brief Destroy ao channels
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

/**
 * @brief aplay and send audio data
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @param pstData the address of data that you want to send
 * @param s32MilliSec -1:block mode/0:non-blocking/s32MilliSec
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn,
			     const AUDIO_FRAME_S *pstData, CVI_S32 s32MilliSec);

/**
 * @brief Start resample
 * @param AoDevId the number of output device
 * @param AoDevId the number of output channels
 * @param enInSampleRate input sample rate
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn,
			       AUDIO_SAMPLE_RATE_E enInSampleRate);
/**
 * @brief Destroy resample
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
 * @brief Clear channel data
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_ClearChnBuf(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
 * @brief Gets the current status of the AO channel
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @param pstStatus structure that describes the state of a channel
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_QueryChnStat(AUDIO_DEV AoDevId, AO_CHN AoChn,
				AO_CHN_STATE_S *pstStatus);
/**
 * @brief Pause send data
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
 * @brief Recovery of transmitted data
 * @param AoDevId the number of output device
 * @param AoChn the number of output channels
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
 * @brief Set aplay volume
 * @param AoDevId the number of output device
 * @param s32VolumeDb output gain
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_SetVolume(AUDIO_DEV AoDevId, CVI_S32 s32VolumeDb);
/**
 * @brief Get aplay volume
 * @param AoDevId the number of output device
 * @param ps32VolumeDb pointer to s32VolumeDb that indicates ao gain
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_GetVolume(AUDIO_DEV AoDevId, CVI_S32 *ps32VolumeDb);
/**
 * @brief Set ao mute
 * @param AoDevId the number of output device
 * @param bEnable Select whether to enable mute mode
 * @param pstFade structure that describes fadein and fadeout
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_SetMute(AUDIO_DEV AoDevId, CVI_BOOL bEnable,
			   const AUDIO_FADE_S *pstFade);
/**
 * @brief Get ao mute status
 * @param AoDevId the number of output device
 * @param bEnable Select whether to enable mute mode
 * @param pstFade structure that describes fadein and fadeout
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_GetMute(AUDIO_DEV AoDevId, CVI_BOOL *pbEnable,
			   AUDIO_FADE_S *pstFade);
/**
 * @brief Set audio output track status
 * @param AoDevId the number of output device
 * @param enTrackMode the mode of track
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_SetTrackMode(AUDIO_DEV AoDevId,
				AUDIO_TRACK_MODE_E enTrackMode);
/**
 * @brief Get audio output track status
 * @param AoDevId the number of output device
 * @param penTrackMode the mode of track
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AO_GetTrackMode(AUDIO_DEV AoDevId,
				AUDIO_TRACK_MODE_E *penTrackMode);

/* AENC function api.  */
/**
 * @brief Enable and create channel to encode
 * @param AeChn the number of aenc channel
 * @param pstAttr pointer that describes an aenc property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
/**
 * @brief Destroy aenc channel
 * @param AeChn the number of aenc channel
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_DestroyChn(AENC_CHN AeChn);
/**
 * @brief Send audio data to channel
 * @param AeChn the number of aenc channel
 * @param pstFrm data that you want to send
 * @param pstAecFrm Reference signal
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm,
			       const AEC_FRAME_S *pstAecFrm);
/**
 * @brief Encode pcm and get data
 * @param AeChn the number of aenc channel
 * @param pstStream data that you get
 * @param s32MilliSec -1:block mode/0:non-blocking/s32MilliSec
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream,
			       CVI_S32 s32MilliSec);
/**
 * @brief Release aenc stream
 * @param AeChn the number of aenc channel
 * @param pstStream the information obtained by calling CVI_AENC_GetStream
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_ReleaseStream(AENC_CHN AeChn,
				   const AUDIO_STREAM_S *pstStream);


/**
 * @brief Get aenc channel data len and EncInBuff
 * @param AeChn the number of aenc channel
 * @param pu64PhysAddr the address of EncInBuff
 * @param pu32Size data len in aenc channel
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_GetStreamBufInfo(AENC_CHN AeChn, CVI_U64 *pu64PhysAddr,
				      CVI_U32 *pu32Size);

/**
 * @brief Registered AAC encoder
 *        Please using api:CVI_MPI_AENC_AacInit instead
 * @param ps32Handle aenc handle
 * @param pstEncoder Registered encode api
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_RegisterExternalEncoder(CVI_S32 *ps32Handle, const AAC_AENC_ENCODER_S *pstEncoder);

/**
 * @brief Unregistered AAC encoder
 *        Please using api:CVI_MPI_AENC_AacDeInit instead
 * @param ps32Handle aenc handle
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_AENC_UnRegisterExternalEncoder(CVI_S32 s32Handle);

/* ADEC function api. */
/**
 * @brief Enable and create channel to decode
 * @param AdChn the number of adec channel
 * @param pstAttr pointer that describes an adec property
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr);
/**
 * @brief Destroy adec channel
 * @param AdChn the number of adec channel
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_DestroyChn(ADEC_CHN AdChn);

/**
 * @brief Send audio data to channel
 * @param AdChn the number of adec channel
 * @param pstStream data that you want to send
 * @param bBlock Select whether to block
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream,
				CVI_BOOL bBlock);

/**
 * @brief Clear channel data
 * @param AdChn the number of adec channel
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_ClearChnBuf(ADEC_CHN AdChn);

/**
 * @brief Registered AAC decoder
 *        Please using api:CVI_MPI_ADEC_AacInit instead
 * @param ps32Handle adec handle
 * @param pstDecoder Registered decoder api
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_RegisterExternalDecoder(CVI_S32 *ps32Handle,
				     const ADEC_DECODER_S *pstDecoder);
/**
 * @brief Unregistered AAC decoder
 *        Please using api:CVI_MPI_ADEC_AacDeInit instead
 * @param s32Handle adec handle
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_UnRegisterExternalDecoder(CVI_S32 s32Handle);
/**
 * @brief Decode pcm and get data
 * @param AdChn the number of adec channel
 * @param pstFrmInfo data that you get
 * @param bBlock Select whether to block
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_INFO_S *pstFrmInfo,
			      CVI_BOOL bBlock);

/**
 * @brief Clear channel buffer
 * @param pstFrmInfo data that you get
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_ReleaseFrame(ADEC_CHN AdChn, const AUDIO_FRAME_INFO_S *pstFrmInfo);

/**
 * @brief Send end flag to Decoder
 * @param AdChn the number of adec channel
 * @param bInstant end flag
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_ADEC_SendEndOfStream(ADEC_CHN AdChn, CVI_BOOL bInstant);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_AUDIO_H__ */

