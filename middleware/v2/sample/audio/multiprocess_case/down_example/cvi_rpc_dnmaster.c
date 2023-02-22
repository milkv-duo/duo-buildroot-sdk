/*
 * Copyright (C) Cvitek Co., Ltd. All rights reserved.
 *
 * File Name: cvi_rpc_dnmaster.c
 * Description:example for audio api flow
 * such as audio out,audio trancode flow
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include "cvi_audio.h"
#include "cvi_type.h"
#include "cvi_audio_aac_adp.h"

CVI_S32 AdChn;
CVI_S32 s32DevId;
CVI_S32 AoChn_0;
CVI_S32 running = 1;
CVI_S32 mode;
ADEC_CHN_ATTR_S stAdecAttr;
MMF_CHN_S stSrcChn, stDestChn;

#define ADEC_BIND_AO_TEST 1
#define ADEC_UBIND_AO_TEST 2
#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_32K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

static AAC_TRANS_TYPE_E gs_enAacTransType = AAC_TRANS_TYPE_ADTS;

void sigint_handler_sample(int signal)
{
	printf("signal:%d\n", signal);
	running = 0;
}

void register_inthandler(void)
{
	signal(SIGINT, sigint_handler_sample);
	signal(SIGHUP, sigint_handler_sample);
	signal(SIGTERM, sigint_handler_sample);
}

CVI_S32 _updata_Adec_setting(ADEC_CHN_ATTR_S *pAdecAttr)
{
	if (!pAdecAttr) {
		printf("[fatal error] ptr is NULL,fuc:%s,line:%d\n", __func__, __LINE__);
		return -1;
	}
	pAdecAttr->u32BufSize = 20;
	pAdecAttr->enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */
	pAdecAttr->bFileDbgMode = CVI_FALSE;

	if (pAdecAttr->enType == PT_ADPCMA) {
		ADEC_ATTR_ADPCM_S *pstAdpcm = malloc(sizeof(ADEC_ATTR_ADPCM_S));

		pAdecAttr->pValue = pstAdpcm;
		pstAdpcm->enADPCMType = AUDIO_ADPCM_TYPE;
	} else if (pAdecAttr->enType == PT_G711A || pAdecAttr->enType == PT_G711U) {
		ADEC_ATTR_G711_S *pstAdecG711 = malloc(sizeof(ADEC_ATTR_G711_S));

		pAdecAttr->pValue = pstAdecG711;
	} else if (pAdecAttr->enType == PT_G726) {
		ADEC_ATTR_G726_S *pstAdecG726 = malloc(sizeof(ADEC_ATTR_G726_S));

		pAdecAttr->pValue = pstAdecG726;
		pstAdecG726->enG726bps = G726_BPS;
	} else if (pAdecAttr->enType == PT_LPCM) {
		ADEC_ATTR_LPCM_S *pstAdecLpcm = malloc(sizeof(ADEC_ATTR_LPCM_S));

		pAdecAttr->pValue = pstAdecLpcm;
		pAdecAttr->enMode = ADEC_MODE_PACK;/* lpcm must use pack mode */
	}

	if (pAdecAttr->enType == PT_AAC) {
		CVI_MPI_ADEC_AacInit();
		ADEC_ATTR_AAC_S *pstAdecAac = malloc(sizeof(ADEC_ATTR_AAC_S));

		pstAdecAac->enTransType = gs_enAacTransType;
		pstAdecAac->enSoundMode = (pAdecAttr->s32ChannelNums == 2 ?
					   AUDIO_SOUND_MODE_STEREO : AUDIO_SOUND_MODE_MONO);
		pstAdecAac->enSmpRate = pAdecAttr->s32Sample_rate;
		pAdecAttr->pValue = pstAdecAac;
		pAdecAttr->enMode = ADEC_MODE_STREAM;   /* aac should be stream mode */
		pAdecAttr->s32frame_size = 1024;
	}

	return 0;
}

int _destroy_Adec_setting(ADEC_CHN_ATTR_S *pAdecAttr)
{
	if (!pAdecAttr || !pAdecAttr->pValue) {
		printf("[fatal error][%p, %p] ptr is NULL,fuc:%s,line:%d\n",
			pAdecAttr, pAdecAttr->pValue, __func__, __LINE__);
		return -1;
	}
	if (pAdecAttr->enType == PT_AAC)
		CVI_MPI_ADEC_AacDeInit();

	if (pAdecAttr->pValue) {
		free(pAdecAttr->pValue);
		pAdecAttr->pValue = NULL;
	}
	memset(pAdecAttr, 0, sizeof(ADEC_CHN_ATTR_S));

	return 0;
}




static int _master_process_setting(void)
{
	//step 1 setup ao enable and turn on ao channel
	//turn 3 channel ao_chn 0, ao_chn 1, ao_chn 2
	//simplify the condition
	//period size : 320 , setting play file all 16k 2chn no vqe playout
	AIO_ATTR_S AudoutAttr;
	CVI_S32 s32Ret = 0;
	CVI_S32 channels = 2;
	CVI_S32 frame_size = 320;
	CVI_S32 sample_rate = 8000;
	CVI_S32 vol = 0;
	PAYLOAD_TYPE_E enType = PT_G711A;


	AudoutAttr.u32ChnCnt = 3;//if you want create n channel,you must set it to >= n
	AudoutAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E)sample_rate;
	AudoutAttr.enSoundmode = (channels == 2 ? AUDIO_SOUND_MODE_STEREO :
				 AUDIO_SOUND_MODE_MONO);
	AudoutAttr.enWorkmode	  = AIO_MODE_I2S_MASTER;
	AudoutAttr.u32EXFlag	  = 0;
	AudoutAttr.u32FrmNum	  = 10; /* only use in bind mode */
	AudoutAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	AudoutAttr.u32PtNumPerFrm = frame_size;
	AudoutAttr.u32ClkSel	  = 0;
	AudoutAttr.enI2sType = AIO_I2STYPE_INNERCODEC;

	s32Ret |= CVI_AO_SetPubAttr(s32DevId, &AudoutAttr);
	s32Ret |= CVI_AO_Enable(s32DevId);
	s32Ret |= CVI_AO_EnableChn(s32DevId, AoChn_0);
	if (s32Ret == CVI_FAILURE) {
		printf("[Error]in downlink audio setup\n");
	}

	CVI_AO_SetVolume(s32DevId, 20);
	CVI_AO_GetVolume(s32DevId, &vol);
	printf("ao vol:%d\n", vol);

	if (mode == ADEC_BIND_AO_TEST || mode == ADEC_UBIND_AO_TEST) {
		memset(&stAdecAttr, 0, sizeof(ADEC_CHN_ATTR_S));
		stAdecAttr.s32Sample_rate = sample_rate;
		stAdecAttr.s32ChannelNums = channels;
		stAdecAttr.s32BytesPerSample = 2;
		stAdecAttr.s32frame_size = frame_size;
		stAdecAttr.enType = enType;
		if (stAdecAttr.enType == PT_AAC)
			stAdecAttr.s32frame_size = 1024;

		_updata_Adec_setting(&stAdecAttr);
		s32Ret = CVI_ADEC_CreateChn(AdChn, &stAdecAttr);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_ADEC_CreateChn(%d) failed with %#x!\n", __func__,
				AdChn, s32Ret);
		}
	}

	if (mode == ADEC_BIND_AO_TEST) {
		stSrcChn.enModId = CVI_ID_ADEC;
		stSrcChn.s32DevId = 0;
		stSrcChn.s32ChnId = AdChn;
		stDestChn.enModId = CVI_ID_AO;
		stDestChn.s32DevId = s32DevId;
		stDestChn.s32ChnId = AoChn_0;

		CVI_AUD_SYS_Bind(&stSrcChn, &stDestChn);
	}

	printf("master create ao enable , create chn 0/1/2\n");
	return CVI_SUCCESS;
}


int main(int argc, char *argv[])
{
	printf("Master set channel\n");
	CVI_AUDIO_INIT();
	register_inthandler();
	if (argc != 2 || atoi(argv[1]) >  2) {
		printf("usage:%s [0:ai 1:aibindaenc 2:aiubindaenc]\n", argv[0]);
		return -1;
	}
	mode = atoi(argv[1]);
	_master_process_setting();
	printf("press ctrl+c to leave....xxx\n");
	while (running) {
		//do nothing
		//do nothing
		sleep(5);
	}

	if (mode == ADEC_BIND_AO_TEST) {
		CVI_AUD_SYS_UnBind(&stSrcChn, &stDestChn);
		CVI_ADEC_DestroyChn(AdChn);
		_destroy_Adec_setting(&stAdecAttr);
	} else if (mode == ADEC_UBIND_AO_TEST) {
		CVI_ADEC_DestroyChn(AdChn);
		_destroy_Adec_setting(&stAdecAttr);
	}

	mode = 0;
	CVI_AO_DisableChn(s32DevId, AoChn_0);
	CVI_AO_Disable(s32DevId);
	CVI_AUDIO_DEINIT();
	printf("Exit program-->master!!!\n");
	return 0;
}

