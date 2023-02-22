/*
 * Copyright ., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: aacenc_interface.c
 * Description: audio transcode function interface as example
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "fdkaac/libAACenc/include/aacenc_lib.h"
#include "cvi_type.h"
#include "cvi_aacenc.h"


#define MAX_CHANNELS 2
#define AACENC_BLOCKSIZE 1024
#define _VERSION_TAG_  "audio_aac_fdkaac_enc_20210407"
#define AACLC_ENC_AOT 2
#define HEAAC_ENC_AOT 5
#define HEAAC_PLUS_ENC_AOT 29
#define AACLD_ENC_AOT 23
#define AACELD_ENC_AOT 39

int CodecArray[5] = {
	AACLC_ENC_AOT,
	HEAAC_ENC_AOT,
	HEAAC_PLUS_ENC_AOT,
	AACLD_ENC_AOT,
	AACELD_ENC_AOT
};

int TransportTable[3] = {
	TT_MP4_ADTS,
	TT_MP4_LOAS,
	TT_MP4_LATM_MCP1
};



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
CVI_S32  CVI_AACENC_GetVersion(AACENC_VERSION_S *pVersion)
{
	printf("[%s][%s]\n", __func__, _VERSION_TAG_);
	strcpy((char *)pVersion->aVersion, (const char *)_VERSION_TAG_);
	return 0;
}

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
int  AACInitDefaultConfig(AACENC_CONFIG *pstConfig)
{

#if 0
typedef struct {
	AuQuality       quality;
	AuEncoderFormat coderFormat;
	short          bitsPerSample;
	int          sampleRate;    /**<audio file sample rate */
	int          bitRate;       /**<encoder bit rate in bits/sec */
	short          nChannelsIn;   /**<number of channels on input (1,2) */
	short          nChannelsOut;  /**<number of channels on output (1,2) */
	short          bandWidth;     /**<targeted audio bandwidth in Hz */
	AACENCTransportType transtype;
} AACENC_CONFIG;
#endif

	pstConfig->quality = AU_QualityHigh;
	pstConfig->coderFormat = AACLC;
	pstConfig->bitsPerSample = 16;
	pstConfig->sampleRate = 32000;
	//for bitrate, user should check below graph:
	//http://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Recommended_Sampling_Rate_and_Bitrate_Combinations

	pstConfig->bitRate = 32000;
	pstConfig->nChannelsIn = 1;
	pstConfig->nChannelsOut = 1;
	pstConfig->bandWidth = 16000;
	pstConfig->transtype =AACENC_ADTS;

	return 0;
}

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
int  AACEncoderOpen(AAC_ENCODER_S **phAacPlusEnc,
							AACENC_CONFIG *pstConfig)
{
	HANDLE_AACENCODER handle;
	int variable_bitrate = 0;
	int afterburner = 1;
	AACENC_InfoStruct info = { 0 };

	if (aacEncOpen(&handle, 0, pstConfig->nChannelsIn) != AACENC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncoder_SetParam(handle, AACENC_AOT, CodecArray[pstConfig->coderFormat]) != AACENC_OK) {
		printf("[Error][%s][%d]format[%d][%d]\n", __func__, __LINE__,
				pstConfig->coderFormat,
		 		CodecArray[pstConfig->coderFormat]);
		return (-1);
	}

	if (CodecArray[pstConfig->coderFormat] == AACELD_ENC_AOT) {
		if (aacEncoder_SetParam(handle, AACENC_SBR_MODE, 1) != AACENC_OK) {
			printf("[Error]Unable to set SBR mode for ELD[%s][%d]\n", __func__, __LINE__);
			return (-1);
		}
	}

	if (aacEncoder_SetParam(handle, AACENC_SAMPLERATE, pstConfig->sampleRate) != AACENC_OK) {
		printf("[Error]Unable to set the AOT[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	CHANNEL_MODE mode;

	switch (pstConfig->nChannelsIn) {
	case 1: mode = MODE_1;       break;
	case 2: mode = MODE_2;       break;
	case 3: mode = MODE_1_2;     break;
	case 4: mode = MODE_1_2_1;   break;
	case 5: mode = MODE_1_2_2;   break;
	case 6: mode = MODE_1_2_2_1; break;
	default:
		fprintf(stderr, "[Error]Unsupported WAV channels %d\n", pstConfig->nChannelsIn);
		return (-1);
	}

	if (aacEncoder_SetParam(handle, AACENC_CHANNELMODE, mode) != AACENC_OK) {
		printf("[Error]Unable to set the channel mode[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
		printf("[Error]Unable to set the wav channel order[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (variable_bitrate) {
		if (aacEncoder_SetParam(handle, AACENC_BITRATEMODE, variable_bitrate) != AACENC_OK) {
			printf("[Error]Unable to set the VBR bitrate mode[%s][%d]\n", __func__, __LINE__);
			return (-1);
		}
	} else {
		if (aacEncoder_SetParam(handle, AACENC_BITRATE, pstConfig->bitRate) != AACENC_OK) {
			printf("[Error]Unable to set the bitrate[%s][%d]\n", __func__, __LINE__);
			return (-1);
		}
	}

	if (aacEncoder_SetParam(handle, AACENC_TRANSMUX, TransportTable[pstConfig->transtype]) != AACENC_OK) {
		printf("[Error]Unable to set the ADTS transmux[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (pstConfig->quality <= AU_QualityHigh)
		afterburner = 1;//high quality
	else
		afterburner = 0;//medium, low quality

	if (aacEncoder_SetParam(handle, AACENC_AFTERBURNER, afterburner) != AACENC_OK) {
		printf("[Error]Unable to set the afterburner mode[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncEncode(handle, NULL, NULL, NULL, NULL) != AACENC_OK) {
		printf("[Error]Unable to initialize the encoder[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncInfo(handle, &info) != AACENC_OK) {
		printf("[Error]Unable to get the encoder info[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	printf("[%s]print out config size[%d] buffer[0x%x] [0x%x] [0x%x] [0x%x] [0x%x]\n",
			__func__, info.confSize,
			info.confBuf[0], info.confBuf[1], info.confBuf[2], info.confBuf[3], info.confBuf[4]);

	printf("AAC encoder handle open success framelength[%d]\n", info.frameLength);
	pstConfig->s32SuggestFrameSize =  info.frameLength;

	*phAacPlusEnc = (void *)handle;

	return 0;
}

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
CVI_S32  AACEncoderFrame(AAC_ENCODER_S * hAacPlusEnc,
						CVI_S16 * ps16PcmBuf,
						CVI_U8 *pu8Outbuf,
						CVI_S32 s32InputBytes,
						CVI_S32 * ps32NumOutBytes)
{
	AACENC_InArgs in_args = { 0 };
	AACENC_OutArgs out_args = { 0 };
	AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
	uint8_t outbuf[20480];
	int in_identifier = IN_AUDIO_DATA;
	int out_identifier = OUT_BITSTREAM_DATA;
	AACENC_ERROR err;
	void *in_ptr, *out_ptr;
	int in_size, in_elem_size;
	int out_size, out_elem_size;

	in_ptr = ps16PcmBuf;
	out_ptr = pu8Outbuf;
	in_size = s32InputBytes;
	 in_elem_size = 2;
	//setup input argument--------------
	in_args.numInSamples = s32InputBytes/2;
	in_buf.numBufs = 1;
	in_buf.bufs = &in_ptr;
	in_buf.bufferIdentifiers = &in_identifier;
	in_buf.bufSizes = &in_size;
	in_buf.bufElSizes = &in_elem_size;

	//setup output argument-----------
	out_ptr = outbuf;
	out_size = sizeof(outbuf);
	out_elem_size = 1;
	out_buf.numBufs = 1;
	out_buf.bufs = &out_ptr;
	out_buf.bufferIdentifiers = &out_identifier;
	out_buf.bufSizes = &out_size;
	out_buf.bufElSizes = &out_elem_size;
	//printf("in2[0x%x] [0x%x] [0x%x] \n", ps16PcmBuf[0], ps16PcmBuf[1], ps16PcmBuf[2]);
	if ((err = aacEncEncode((HANDLE_AACENCODER)hAacPlusEnc, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
		if (err == AACENC_ENCODE_EOF) {
			printf("[%s][%d]Encoding EOF[%d]\n", __func__, __LINE__, err);
			return (-1);
		} else
			printf("out[0x%x] [0x%x] [0x%x] \n", outbuf[0], outbuf[1], outbuf[2]);
	}

	if (err ==AACENC_OK){
		if (out_args.numOutBytes != 0) {
			*ps32NumOutBytes = out_args.numOutBytes;
			memcpy(pu8Outbuf, outbuf, out_args.numOutBytes);
		} else
			printf("return 0 size[%s][%d]\n", __func__, __LINE__);
	} else
		printf("[%s]exception error\n", __func__);


	return 0;
}

/**
 *brief close encoder device.
 *attention \n
 *N/A
 *param[in] hAacPlusEnc    pointer to an configuration information structure
 *retval N/A
 *see \n
 *N/A
 */
CVI_VOID AACEncoderClose(AAC_ENCODER_S *hAacPlusEnc)
{
	//TODO: double check the input handle valid
	printf("AACEncoderClose ...version20201130\n");
	aacEncClose((HANDLE_AACENCODER *)&hAacPlusEnc);
	//return 0;
}




