#ifndef __CVI_AAC_DEC_H__
#define __CVI_AAC_DEC_H__


/* ------------------------------------------------------------------------
 * Debugging, printing and logging
 */
typedef struct _st_aac_dec_info {
	int channel_num;
	int sample_rate;
	int frame_byte;
	int cbState;
} ST_AAC_DEC_INFO;

typedef signed short INT_PCM;
typedef enum {
	AACDEC_ADTS = 0,
	AACDEC_LOAS = 1,
	AACDEC_LATM_MCP1 = 2,
	AACDEC_RAW = 3,
} AACDECTransportType;
#define CVI_DECODE_AAC_TYPE AACDEC_ADTS
typedef int (*pDecode_Cb)(void *, ST_AAC_DEC_INFO *, char *, int);

//int get_aac_sample_rate(int sampling_frequency_index);
//int get_aac_head_info(void *paac_handle);

void *CVI_AAC_Decode_Init(AACDECTransportType enTranType, FILE *intput_file_fd);
int CVI_AAC_Decode(void *paac_handle, unsigned char *pInputBuf,
		   INT_PCM *pOutBuf, int byte_left, int *frame_size);
int CVI_AAC_Decode_Dinit(void *paac_handle);
int CVI_AAC_Decode_InstallCb(void *paac_handle, pDecode_Cb pCbFunc);


#endif
