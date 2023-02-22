/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/aaacdec.h
 * Description:
 *   Common audio encoder definitions for AAC format.
 */
/*
 * \file
 * \brief Describes the information about AACENC.
 */

#ifndef _CVI_AACENC_H
#define _CVI_AACENC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "cvi_type.h"

/********************************Macro Definition********************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!-- [AACENC] */

#ifdef MONO_ONLY
#define MAX_CHANNELS 1 /**<aacenc encoder channels*/
#else
#define MAX_CHANNELS 2
#endif

#define AACENC_BLOCKSIZE 1024 /**<aacenc blocksize*/
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/


typedef enum {
	/*!< No error happened. All fine. */
	CVI_AACENC_OK                     = 0x0000,
	/*!< Handle passed to function call was invalid. */
	CVI_AACENC_INVALID_HANDLE         = 0x0020,
	/*!< Memory allocation failed. */
	CVI_AACENC_MEMORY_ERROR           = 0x0021,
	/*!< Parameter not available. */
	CVI_AACENC_UNSUPPORTED_PARAMETER  = 0x0022,
	/*!< Configuration not provided. */
	CVI_AACENC_INVALID_CONFIG         = 0x0023,
	/*!< General initialization error. */
	CVI_AACENC_INIT_ERROR             = 0x0040,
	/*!< AAC library initialization error. */
	CVI_AACENC_INIT_AAC_ERROR         = 0x0041,
	/*!< SBR library initialization error. */
	CVI_AACENC_INIT_SBR_ERROR         = 0x0042,
	/*!< Transport library initialization error. */
	CVI_AACENC_INIT_TP_ERROR          = 0x0043,
	/*!< Meta data library initialization error. */
	CVI_AACENC_INIT_META_ERROR        = 0x0044,
	/*!< The encoding process was interrupted by an unexpected error. */
	CVI_AACENC_ENCODE_ERROR           = 0x0060,
	/*!< End of file reached. */
	CVI_AACENC_ENCODE_EOF             = 0x0080
} CVI_AACENC_ERROR_E;

/**Defines AACENC quality*/
typedef enum {
	AU_QualityExcellent = 0,
	AU_QualityHigh   = 1,
	AU_QualityMedium = 2,
	AU_QualityLow = 3,
} AuQuality;

/**Defines AACENC format*/
typedef enum {
	AACLC = 0,/**<AAC-LC format*/
	EAAC = 1,/**<HEAAC or AAC+  or aacPlusV1*/
	EAACPLUS = 2,/**<AAC++ or aacPlusV2*/
	AACLD = 3,/**<AAC LD(Low Delay)*/
	AACELD = 4,/**<AAC ELD(Low Delay)*/
} AuEncoderFormat;

/**Defines AACENC container*/
typedef enum {
	AACENC_ADTS = 0,
	AACENC_LOAS = 1,
	AACENC_LATM_MCP1 = 2,
} AACENCTransportType;

/**Defines AACENC configuration*/
typedef struct {
	AuQuality       quality;
	AuEncoderFormat coderFormat;
	CVI_S16          bitsPerSample;
	CVI_S32          sampleRate;    /**<audio file sample rate */
	CVI_S32          bitRate;       /**<encoder bit rate in bits/sec */
	CVI_S16          nChannelsIn;   /**<number of channels on input (1,2) */
	CVI_S16          nChannelsOut;  /**<number of channels on output (1,2) */
	CVI_S16          bandWidth;     /**<targeted audio bandwidth in Hz */
	AACENCTransportType transtype;
	CVI_S32		s32SuggestFrameSize;
} AACENC_CONFIG;

/**Defines AACENC version*/
typedef struct cviAACENC_VERSION_S {
	CVI_U8 aVersion[64];
} AACENC_VERSION_S;

typedef void  AAC_ENCODER_S;
/******************************* API declaration *****************************/

/**
 *brief Get version information.
 *attention \n
 *N/A
 *param[in] pVersion       version describe struct
 *retval ::CVI_SUCCESS   : Success
 *retval ::CVI_FAILURE          : FAILURE
 *see \n
 *N/A
 */
CVI_S32  CVI_AACENC_GetVersion(AACENC_VERSION_S *pVersion);

/**
 *brief get reasonable default configuration.
 *attention \n
 *N/A
 *param[in] pstConfig    pointer to an configuration information structure
 *retval ::CVI_SUCCESS   : Success
 *retval ::CVI_FAILURE          : FAILURE
 *see \n
 *N/A
 */
CVI_S32  AACInitDefaultConfig(AACENC_CONFIG *pstConfig);

/**
 *brief allocate and initialize a new encoder instance.
 *attention \n
 *N/A
 *param[in] phAacPlusEnc    pointer to an configuration information structure
 *param[in] pstConfig    pointer to an configuration information structure
 *retval ::CVI_SUCCESS   : Success
 *retval ::CVI_FAILURE   : FAILURE
 *see \n
 *N/A
 */
CVI_S32  AACEncoderOpen(AAC_ENCODER_S **phAacPlusEnc,
							AACENC_CONFIG *pstConfig);

/**
 *brief allocate and initialize a new encoder instance
 *attention \n
 *N/A
 *param[in] hAacPlusEnc    pointer to an configuration information structure
 *param[in] ps16PcmBuf    BLOCKSIZE*nChannels audio samples,interleaved
 *param[in] pu8Outbuf    pointer to output buffer,(must be 6144/8*MAX_CHANNELS bytes large)
 *param[in] ps32NumOutBytes    number of bytes in output buffer after processing
 *retval ::CVI_SUCCESS   : Success
 *retval ::CVI_FAILURE   : FAILURE
 *see \n
 *N/A
 */
CVI_S32  AACEncoderFrame(AAC_ENCODER_S *hAacPlusEnc,
						CVI_S16 *ps16PcmBuf,
						CVI_U8 *pu8Outbuf, CVI_S32 s32InputBytes, CVI_S32 *ps32NumOutBytes);

/**
 *brief close encoder device.
 *attention \n
 *N/A
 *param[in] hAacPlusEnc    pointer to an configuration information structure
 *retval N/A
 *see \n
 *N/A
 */
CVI_VOID AACEncoderClose(AAC_ENCODER_S *hAacPlusEnc);
/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* _CVI_AACENC_H */
