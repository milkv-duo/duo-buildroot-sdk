/*
 * Copyright ., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: aacdec_interface.c
 * Description: audio transcode function interface as example
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "fdkaac/libAACdec/include/aacdecoder_lib.h"
#include "fdkaac/libAACenc/include/aacenc_lib.h"
#include "cvi_type.h"
#include "cvi_aacdec.h"


#define _VERSION_TAG_  "audio_aac_fdkaac_dec_20210407A"

#ifndef AAC_MAX_NCHANS
#define AAC_MAX_NCHANS 2
#endif
#define AAC_MAX_NSAMPS 1024
#define AAC_MAINBUF_SIZE (768 * AAC_MAX_NCHANS)
/**<according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),6144 bits =  */
 /*  768 bytes per SCE or CCE-I,12288 bits = 1536 bytes per CPE*/

#define AAC_NUM_PROFILES 3
#define AAC_PROFILE_MP 0
#define AAC_PROFILE_LC 1
#define AAC_PROFILE_SSR 2


/** @} */ /** <!-- ==== Macro Definition end ==== */
/*************************** Structure Definition ****************************/
/** \addtogroup      AACDEC */
/** @{ */ /** <!-- [AACDEC] */
/**Defines AACDEC error code*/
/**
 *brief Get version information.
 *attention \n
 *N/A
 *param[in] pVersion    :   version describe struct
 *retval ::CVI_SUCCESS   :   Success
 *retval ::CVI_FAILURE   :   pVersion is NULL, return CVI_FAILURE
 *see \n
 *N/A
 */
CVI_S32 CVI_AACDEC_GetVersion(AACDEC_VERSION_S *pVersion)
{
	strcpy((char *)pVersion->aVersion, (const char *)_VERSION_TAG_);
	printf("[%s][%s]\n", __func__, _VERSION_TAG_);
	return 0;
}



/**
 *brief create and initial decoder device.
 *attention
 *N/A
 *param[in] enTranType   : transport type
 *retval ::CVIAACDecoder   : init success, return non-NULL handle.
 *retval ::NULL          : init failure, return NULL
 *see
 *N/A
 */
CVIAACDecoder AACInitDecoder(AACDECTransportType enTranType)
{
	HANDLE_AACDECODER decoder;
	//AAC_DECODER_ERROR err;

	if (enTranType == AACDEC_ADTS) {
		printf("[%s][%s]\n", __func__, _VERSION_TAG_);
		decoder = aacDecoder_Open(TT_MP4_ADTS, 1);
		printf("ADTS header decoder set\n");
	} else if (enTranType == AACDEC_LOAS)
		decoder = aacDecoder_Open(TT_MP4_LOAS, 1);
	else if (enTranType == AACDEC_LATM_MCP1)
		decoder = aacDecoder_Open(TT_MP4_LATM_MCP1, 1);
	else {
		printf("[Warning]set TransportType[%d] to RawMode\n", (int)enTranType);
		decoder = aacDecoder_Open(TT_MP4_RAW, 1);
		printf("set to TT_MP4_RAW\n");

	}


	if (decoder == NULL) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		return NULL;
	}

	return (void *)decoder;

}


/**
 *brief destroy AAC-Decoder, free the memory.
 *attention \n
 *N/A
 *param[in] CVIAACDecoder  : AAC-Decoder handle
 *see \n
 *N/A
 */
CVI_VOID AACFreeDecoder(CVIAACDecoder CVIAACDecoder)
{
	if (CVIAACDecoder != NULL)
		aacDecoder_Close((HANDLE_AACDECODER)CVIAACDecoder);

}



/**
 *brief set RawMode before decode Raw Format aac bitstream(Reserved API, unused now.)
 *attention \n
 *N/A
 *param[in] CVIAACDecoder  : AAC-Decoder handle
 *param[in] Chans       : inout channels
 *param[in] sampRate     : input sample rate
 *retval ::CVI_FAILURE    : RESERVED API, always return CVI_FAILURE.
 *see \n
 *N/A
 */
CVI_S32 AACSetRawMode(CVIAACDecoder CVIAACDecoder,
							CVI_S32 nChans,
							CVI_S32 sampRate)
{
	UCHAR conf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	UCHAR* conf_array[1] = { conf };
	UINT length = 4;
	//Step1: Find the config data
	HANDLE_AACENCODER tmp_enchandle;
	AACENC_InfoStruct tmp_info = { 0 };
	if (aacEncOpen(&tmp_enchandle, 0, nChans) != AACENC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (nChans == 1) {
	if (aacEncoder_SetParam(tmp_enchandle, AACENC_CHANNELMODE, MODE_1) != AACENC_OK) {
		printf("[Error]Unable to set the channel mode[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}
	} else {
	if (aacEncoder_SetParam(tmp_enchandle, AACENC_CHANNELMODE, MODE_2) != AACENC_OK) {
		printf("[Error]Unable to set the channel mode[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}
	}

	if (aacEncoder_SetParam(tmp_enchandle, AACENC_AOT, 2) != AACENC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncoder_SetParam(tmp_enchandle, AACENC_SAMPLERATE, sampRate) != AACENC_OK) {
	printf("[Error]Unable to set the AOT[%s][%d]\n", __func__, __LINE__);
	return (-1);
	}

	if (aacEncoder_SetParam(tmp_enchandle, AACENC_AFTERBURNER, 1) != AACENC_OK) {
		printf("[Error]Unable to set the afterburner mode[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncEncode(tmp_enchandle, NULL, NULL, NULL, NULL) != AACENC_OK) {
		printf("[Error]Unable to initialize the encoder[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	if (aacEncInfo(tmp_enchandle, &tmp_info) != AACENC_OK) {
		printf("[Error]Unable to get the encoder info[%s][%d]\n", __func__, __LINE__);
		return (-1);
	}

	memcpy(conf, tmp_info.confBuf, 4);
	conf_array[1] = { conf };

	printf("[%s]print out config size[%d] buffer[0x%x] [0x%x] [0x%x] [0x%x] [0x%x]\n",
			__func__, tmp_info.confSize,
			conf[0],conf[1], conf[2], conf[3], conf[4]);

	//Step2 : Update the config data
	AAC_DECODER_ERROR err = AAC_DEC_OK;

	err = aacDecoder_ConfigRaw((HANDLE_AACDECODER)CVIAACDecoder,
    		conf_array,
		&length);

	if (err != AAC_DEC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		printf("err = [0x%x]\n", err);
	}

	return 0;
}

int get_one_ADTS_frame(unsigned char *buffer, size_t buf_size, size_t *data_size)
{
	size_t size = 0;
	size_t save_buf_size = buf_size;

	if (!buffer || !data_size)
		return -1;

	while (1) {
	if (buf_size  < 7)
		return -1;

	if ((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0)) {
		size |= ((buffer[3] & 0x03) << 11);     //high 2 bit
		size |= buffer[4]<<3;                //middle 8 bit
		size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
		//printf("adts-->header[0x%x][0x%x][0x%x][0x%x]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
		break;
	}

	--buf_size;
	++buffer;
	}

	if (buf_size < size) {
		printf("[%s] no header...\n", __func__);
		return -1;
	}
	//memcpy(data, buffer, size);
	//printf("aft getOneFrame[0x%x][0x%x][0x%x][0x%x]\n", data[0], data[1], data[2], data[3]);
	*data_size = size;//parsing size from header
	//printf("[%s]require size[%d] left check[%d]\n", __func__, size, (2048 - size));
	return (save_buf_size - size); //return bytes left
}

/**
 *brief look for valid AAC sync header
 *attention \n
 *N/A
 *param[in] CVIAACDecoder      : AAC-Decoder handle
 *param[in/out] ppInbufPtr   : address of the pointer of start-point of the bitstream
 *param[in/out] pBytesLeft   : pointer to BytesLeft that indicates bitstream numbers at input buffer
 *retval ::<0                : err, always return ERR_AAC_INDATA_UNDERFLOW
 *retval ::other             : Success, return number bytes of current frame
 *see \n
 *N/A
 */
CVI_S32 AACDecodeFindSyncHeader(CVIAACDecoder CVIAACDecoder,
										CVI_U8 **ppInbufPtr,
										CVI_S32 *pBytesLeft)
{
#if 0
1024 990 sample for AAC_LC
2048 or 1920 for HE-AAC (v2)
512 or 480 for AAC-LD and AAC-ELD
768, 1024, 2048 or 4096 for USAC
#endif

	AAC_DECODER_ERROR err = AAC_DEC_OK;
	UINT bytes_valid;
	unsigned int input_sizebytes = *pBytesLeft;
	int current_index = 0;

	unsigned char *inBufferArray[1];
	unsigned int inBuffReaded[1];

	//step 1: wrap the input data to array
	inBufferArray[0] = (unsigned char *)(*ppInbufPtr);
	inBuffReaded[0] = input_sizebytes;
	bytes_valid = input_sizebytes;


	//step2 : fill the aacDecoder_Fill and check the return valid_bytes
	err = aacDecoder_Fill((HANDLE_AACDECODER)CVIAACDecoder,
							inBufferArray,
							inBuffReaded,
							&bytes_valid);

	printf("aacDecoder_Fill->bytes_valid[%d]\n", bytes_valid);

	if (err == AAC_DEC_OK)
		printf("aacDecoder_Fill return ok\n");
	else {
		printf("err=[%d]\n", (int)err);
		current_index = input_sizebytes - bytes_valid;
		*pBytesLeft = bytes_valid;
	}

	return current_index;
}



/**
 *brief decoding AAC frame and output 1024(LC) OR
 *2048(HEAAC/eAAC/eAAC+) 16bit PCM samples per channel.
 *attention \n
 *param[in] CVIAACDecoder       : AAC-Decoder handle
 *param[in] ppInbufPtr        : address of the pointer of start-point of the bitstream
 *param[in/out] pBytesLeft    : pointer to BytesLeft that indicates
 *bitstream numbers at input buffer,indicates the left bytes
 *param[in] pOutPcm           : the address of the out pcm buffer,
 *pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
 *retval :: SUCCESS           : Success
 *retval :: ERROR_CODE        : FAILURE, return error_code.
 *see \n
 */
CVI_S32 AACDecodeFrame(CVIAACDecoder CVIAACDecoder,
							CVI_U8 **ppInbufPtr,
							CVI_S32 *pBytesLeft,
							CVI_S16 *pOutPcm)
{
	AAC_DECODER_ERROR err = AAC_DEC_OK;
	CStreamInfo *theInfo = NULL;
	unsigned char *inBufferArray[1];
	unsigned int inBuffReaded[1];
	unsigned int byte_left;

	//CVI_U8 *tmpcheck;
	//tmpcheck = (CVI_U8 *)(*ppInbufPtr);
	//printf("in [0x%x] [0x%x] [0x%x] inbytes[%d] \n", tmpcheck[0], tmpcheck[1], tmpcheck[2], *pBytesLeft);

	inBufferArray[0] = (unsigned char *)(*ppInbufPtr);
	inBuffReaded[0] = *pBytesLeft;
	byte_left =  *pBytesLeft;
#if 0
	unsigned char fake_inputdata[10240] = {0};
	unsigned char fake_outputdata[10240] = {0};
	int inputbytes = byte_left;

	memcpy(fake_inputdata, inBufferArray[0] , inputbytes);
#endif

	err  = aacDecoder_Fill((HANDLE_AACDECODER)CVIAACDecoder,
							inBufferArray,
							inBuffReaded,
							&byte_left);

	if (err != AAC_DEC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		printf("err = [0x%x]\n", err);
	} else {
		//printf("[xxxx]aacDecoder_Fill after byte left[%d]\n", byte_left);
		//sleep(1);
	}

#if 0//AAC ADTS header check

	int fake = 0;
	int fake_byteleft = 0;
	printf("fake_inputdata[0x%x] [0x%x]\n", fake_inputdata[0], fake_inputdata[1]);
	fake_byteleft = get_one_ADTS_frame(
		fake_inputdata,
		inputbytes,
		(size_t *)&fake);

	if (fake_byteleft < 0)
		printf("[xxxxx]fake_byteleft no header\n");
	else
		printf("[xxxxxx]fake_byteleft[%d] data size[%d]\n", fake_byteleft, fake);
#else
	//printf("no self check\n");
#endif

	err = aacDecoder_DecodeFrame((HANDLE_AACDECODER)CVIAACDecoder,
							(INT_PCM *) pOutPcm,
							(1024 * 30), 0);


	if (err != AAC_DEC_OK) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		printf("err = [0x%x]\n", err);
		if (err == AAC_DEC_NOT_ENOUGH_BITS)
			printf("err[%s][%d]AAC_DEC_NOT_ENOUGH_BITS\n", __func__, __LINE__);
		if (err == AAC_DEC_TRANSPORT_ERROR)
			printf("err[%s][%d]AAC_DEC_TRANSPORT_ERROR\n", __func__, __LINE__);

		return err;
	} else {
		//for debug usage
		//printf("out [0x%x] [0x%x] [0x%x]\n", pOutPcm[0], pOutPcm[1], pOutPcm[2]);
	}



	theInfo = aacDecoder_GetStreamInfo((HANDLE_AACDECODER)CVIAACDecoder);
	if (theInfo == NULL) {
		printf("[Error]Get null info[%s][%d]\n", __func__, __LINE__);
		//sleep(1);
	}
	//printf("aacDecoder_DecodeFrame theInfo->frameSize[%d]\n",
	//	theInfo->frameSize);
	*pBytesLeft = byte_left;

	return 0;
}


/**
 *brief get the frame information.
 *attention \n
 *param[in] CVIAACDecoder       : AAC-Decoder handle
 *param[out] aacFrameInfo     : frame information
 *retval :: CVI_SUCCESS        : Success
 *retval :: ERROR_CODE        : FAILURE, return error_code.
 *see \n
 *N/A
 */
CVI_S32 AACGetLastFrameInfo(CVIAACDecoder CVIAACDecoder,
								AACFrameInfo *aacFrameInfo)
{
	HANDLE_AACDECODER decoder = (HANDLE_AACDECODER)CVIAACDecoder;

	CStreamInfo* info = aacDecoder_GetStreamInfo(decoder);

	aacFrameInfo->bitRate = info->bitRate;
	aacFrameInfo->nChans = info->numChannels;
	aacFrameInfo->sampRateCore = info->aacSampleRate;
	aacFrameInfo->sampRateOut = info->sampleRate;
	aacFrameInfo->bitsPerSample = 16;
	aacFrameInfo->outputSamps = info->frameSize;
	aacFrameInfo->profile = info->profile;
	aacFrameInfo->tnsUsed = 0;
	aacFrameInfo->pnsUsed = 0;
	aacFrameInfo->bytespassDec = info->numTotalBytes;

	// printf("aacDecoder_GetStreamInfo br[%d] ch[%d] sr[%d] total bytes pass[%ld]aacsr[%d]\n",
	// 		info->bitRate,
	// 		info->numChannels,
	// 		info->sampleRate,
	// 		info->numTotalBytes,
	// 		info->aacSampleRate);

	return 0;
}


/**
 *brief set eosflag.
 *attention \n
 *param[in] CVIAACDecoder       : AAC-Decoder handle
 *param[in] s32Eosflag        : end flag
 *retval :: CVI_SUCCESS        : Success
 *retval :: ERROR_CODE        : FAILURE, return error_code.
 *see \n
 *N/A
 */

CVI_S32 AACDecoderSetEosFlag(CVIAACDecoder CVIAACDecoder,
								CVI_S32 s32Eosflag)
{
	//TODO:
	return 0;
}



/**
 *brief flush internal codec state (after seeking, for example)
 *attention \n
 *param[in] CVIAACDecoder       : AAC-Decoder handle
 *retval :: CVI_SUCCESS        : Success
 *retval :: ERROR_CODE        : FAILURE, return error_code.
 *see \n
 *N/A
 */
CVI_S32 AACFlushCodec(CVIAACDecoder CVIAACDecoder)
{
	//TODO:
	return 0;
}