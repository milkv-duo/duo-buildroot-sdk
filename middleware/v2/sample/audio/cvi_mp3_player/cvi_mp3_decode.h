/*
 *	cvi_audio_uac.h  --  USB Audio Class Gadget API
 *
 */

#ifndef _CVI_MP3_DECODE_H_
#define _CVI_MP3_DECODE_H_
#include <stdio.h>
#include "cvi_type.h"
/* ------------------------------------------------------------------------
 * Debugging, printing and logging
 */
#define CVI_MP3_DEC_CB_STATUS_INIT 0
#define CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO  0x31
#define CVI_MP3_DEC_CB_STATUS_INFO_CHG 0x32
#define CVI_MP3_DEC_CB_STATUS_BIT_RATE_CHG 0x33
#define CVI_MP3_DEC_CB_STATUS_SMP_RATE_CHG 0x34
#define CVI_MP3_DEC_CB_STATUS_STABLE 0x30

typedef struct _st_mp3_dec_info {
	int channel_num;
	int sample_rate;
	int bit_rate;
	int frame_cnt;
	int cbState;
} ST_MP3_DEC_INFO;

typedef int (*Decode_Cb)(void *, ST_MP3_DEC_INFO *, char *, int);

CVI_S32 CVI_MP3_Decode(CVI_VOID *inst, CVI_VOID *pInputBuf,
	CVI_VOID *pOutputBUf, CVI_S32 s32InputLen, CVI_S32 *s32OutLen);

CVI_VOID *CVI_MP3_Decode_Init(CVI_VOID *inst);

CVI_S32 CVI_MP3_Decode_DeInit(CVI_VOID *inst);

CVI_S32 CVI_MP3_Decode_InstallCb(CVI_VOID *inst, Decode_Cb pCbFunc);

#endif /* _CVI_AUDIO_UAC_H_ */

