/*
 * Copyright (C) Cvitek Co., Ltd. All rights reserved.
 *
 * File Name: cvi_rpc_upmaster.c
 * Description:example for audio api flow
 * such as audio in,audio trancode flow
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


CVI_S32 AeChn;
CVI_S32 s32DevId;
CVI_S32 running = 1;
CVI_S32 AiChn;
CVI_BOOL bVqeOn;
CVI_BOOL bAecOn;
AENC_CHN_ATTR_S stAencAttr;
MMF_CHN_S stSrcChn, stDestChn;
CVI_S32 mode;

#define AI_BIND_ANEC_TEST 1
#define AI_UBIND_ANEC_TEST 2
#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_32K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

static AAC_TYPE_E     gs_enAacType = AAC_TYPE_AACLC;
static AAC_BPS_E     gs_enAacBps  = AAC_BPS_32K;
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

static CVI_BOOL _update_aec_setting(AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr)
{
	if (pstAiVqeTalkAttr == NULL)
		return CVI_FALSE;

	AI_AEC_CONFIG_S default_AEC_Setting;

	memset(&default_AEC_Setting, 0, sizeof(AI_AEC_CONFIG_S));
	default_AEC_Setting.para_aec_filter_len = 13;
	default_AEC_Setting.para_aes_std_thrd = 37;
	default_AEC_Setting.para_aes_supp_coeff = 60;
	pstAiVqeTalkAttr->stAecCfg = default_AEC_Setting;
	pstAiVqeTalkAttr->u32OpenMask = LP_AEC_ENABLE | NLP_AES_ENABLE |
					NR_ENABLE | AGC_ENABLE;
	printf("pstAiVqeTalkAttr:u32OpenMask[0x%x]\n", pstAiVqeTalkAttr->u32OpenMask);
	return CVI_FALSE;
}


static CVI_BOOL _update_vqe_setting(AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr)
{
	if (pstAiVqeTalkAttr == NULL)
		return CVI_FALSE;

#define CVIAUDIO_ALGO_SSP	0x11
	pstAiVqeTalkAttr->u32OpenMask |= AI_TALKVQE_MASK_ANR;
	pstAiVqeTalkAttr->u32OpenMask |= AI_TALKVQE_MASK_AGC;

	AUDIO_AGC_CONFIG_S st_AGC_Setting;
	AUDIO_ANR_CONFIG_S st_ANR_Setting;

	st_AGC_Setting.para_agc_max_gain = 4;
	st_AGC_Setting.para_agc_target_high = 2;
	st_AGC_Setting.para_agc_target_low = 6;
	//st_AGC_Setting.para_agc_vad_enable = CVI_TRUE;
	//st_AGC_Setting.s32Reserved = 0;
	st_ANR_Setting.para_nr_snr_coeff = 15;
	//st_ANR_Setting.s32Reserved = 0;
	pstAiVqeTalkAttr->stAgcCfg = st_AGC_Setting;
	pstAiVqeTalkAttr->stAnrCfg = st_ANR_Setting;
	pstAiVqeTalkAttr->s32RevMask = CVIAUDIO_ALGO_SSP;
	printf("pstAiVqeTalkAttr:u32OpenMask[0x%x]\n", pstAiVqeTalkAttr->u32OpenMask);

	return CVI_TRUE;
}

static CVI_S32  _update_aenc_params(AENC_CHN_ATTR_S *pAencAttrs,
				    AIO_ATTR_S *pAioAttrs,
				    PAYLOAD_TYPE_E enType)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (!pAencAttrs || !pAioAttrs) {
		printf("[fatal error][%p, %p] ptr is NULL,fuc:%s,line:%d\n",
			pAencAttrs, pAioAttrs, __func__, __LINE__);
		return -1;
	}

	memset(pAencAttrs, 0, sizeof(AENC_CHN_ATTR_S));
	pAencAttrs->enType = enType;
	pAencAttrs->u32BufSize = 30;
	pAencAttrs->u32PtNumPerFrm = pAioAttrs->u32PtNumPerFrm;

	if (pAencAttrs->enType == PT_ADPCMA) {
		AENC_ATTR_ADPCM_S *pstAdpcmAenc = (AENC_ATTR_ADPCM_S *)malloc(sizeof(
				AENC_ATTR_ADPCM_S));

		pstAdpcmAenc->enADPCMType = AUDIO_ADPCM_TYPE;
		pAencAttrs->pValue       = (CVI_VOID *)pstAdpcmAenc;
	} else if (pAencAttrs->enType == PT_G711A || pAencAttrs->enType == PT_G711U) {
		AENC_ATTR_G711_S *pstAencG711 = (AENC_ATTR_G711_S *)malloc(sizeof(
							AENC_ATTR_G711_S));

		pAencAttrs->pValue = (CVI_VOID *)pstAencG711;
	} else if (pAencAttrs->enType == PT_G726) {
		AENC_ATTR_G726_S *pstAencG726 = (AENC_ATTR_G726_S *)malloc(sizeof(
							AENC_ATTR_G726_S));
		pstAencG726->enG726bps = G726_BPS;
		pAencAttrs->pValue = (CVI_VOID *)pstAencG726;

	} else if (pAencAttrs->enType == PT_LPCM) {
		AENC_ATTR_LPCM_S *pstAencLpcm = (AENC_ATTR_LPCM_S *)malloc(sizeof(
							AENC_ATTR_LPCM_S));

		pAencAttrs->pValue = (CVI_VOID *)pstAencLpcm;
	} else if (pAencAttrs->enType == PT_AAC) {
		printf("Need update detail external AAC function params\n");
		//need update AAC if supported
	} else {
		printf("[Error]Not support codec type[%d]\n", enType);
		s32Ret = CVI_FAILURE;
	}

	return s32Ret;
}

static CVI_S32 _update_Aenc_setting(AIO_ATTR_S *pstAioAttr,
			     AENC_CHN_ATTR_S *pstAencAttr,
			     PAYLOAD_TYPE_E enType,
			     int Sample_rate, bool bVqe)
{

	CVI_S32 s32Ret;

	s32Ret = _update_aenc_params(pstAencAttr, pstAioAttr, enType);
	if (s32Ret != CVI_SUCCESS) {
		printf("[Error][%s]failure in params\n", __func__);
		return CVI_FAILURE;
	}

	if (enType == PT_AAC) {
		AENC_ATTR_AAC_S  *pstAencAac = (AENC_ATTR_AAC_S *)malloc(sizeof(AENC_ATTR_AAC_S));

		pstAencAac->enAACType = gs_enAacType;
		pstAencAac->enBitRate = gs_enAacBps;
		pstAencAac->enBitWidth = AUDIO_BIT_WIDTH_16;
		pstAencAac->enSmpRate = Sample_rate;
		printf("[cvi_info] AAC enc[%s][%d]smp-rate[%d]\n",
		       __func__,
		       __LINE__,
		       pstAencAac->enSmpRate);

		pstAencAac->enSoundMode = bVqe ? AUDIO_SOUND_MODE_MONO :
					  pstAioAttr->enSoundmode;
		pstAencAac->enTransType = gs_enAacTransType;
		pstAencAac->s16BandWidth = 0;
		pstAencAttr->pValue = pstAencAac;
		s32Ret = CVI_MPI_AENC_AacInit();
		printf("[cvi_info] CVI_MPI_AENC_AacInit s32Ret:%d\n", s32Ret);
	}

	pstAencAttr->bFileDbgMode = CVI_FALSE;

	return CVI_SUCCESS;
}

static CVI_S32 _destroy_Aenc_setting(AENC_CHN_ATTR_S *pstAencAttr)
{
	if (!pstAencAttr || !pstAencAttr->pValue) {
		printf("[fatal error][%p, %p] ptr is NULL,fuc:%s,line:%d\n",
			pstAencAttr, pstAencAttr->pValue, __func__, __LINE__);
		return -1;
	}
	if (pstAencAttr->enType == PT_AAC)
		CVI_MPI_AENC_AacDeInit();
	if (pstAencAttr->pValue) {
		free(pstAencAttr->pValue);
		pstAencAttr->pValue = NULL;
	}
	memset(pstAencAttr, 0, sizeof(AENC_CHN_ATTR_S));

	return 0;
}


static int _master_process_setting(void)
{
	CVI_S32 s32Ret;
	AIO_ATTR_S	AudinAttr;
	AIO_ATTR_S *pstAioAttr = &AudinAttr;
	CVI_S32 vol = 0;

	//step 1: setup audio in parameters
	AudinAttr.enSamplerate = 8000;
	AudinAttr.u32ChnCnt = 3;
	AudinAttr.enSoundmode = AUDIO_SOUND_MODE_STEREO;
	AudinAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	AudinAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	AudinAttr.u32EXFlag = 0;
	AudinAttr.u32FrmNum = 10;
	AudinAttr.u32PtNumPerFrm = 320;
	AudinAttr.u32ClkSel = 0;
	AudinAttr.enI2sType = AIO_I2STYPE_INNERCODEC;

	AI_TALKVQE_CONFIG_S stAiVqeTalkAttr;

	memset(&stAiVqeTalkAttr, 0, sizeof(AI_TALKVQE_CONFIG_S));
	AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr = (AI_TALKVQE_CONFIG_S *)&stAiVqeTalkAttr;

	if (bVqeOn == CVI_TRUE) {
		if ((AudinAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000) ||
				(AudinAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)) {
			_update_vqe_setting(pstAiVqeTalkAttr);
			pstAiVqeTalkAttr->s32WorkSampleRate = AudinAttr.enSamplerate;
			if (bAecOn == CVI_TRUE) {
				_update_aec_setting(pstAiVqeTalkAttr);
				printf("----------------notice-------------------------------\n");
				printf("AEC will need to setup record in to channel Count = 2\n");
				printf("AEC will output only one single channel with 2 channels in\n");
				printf("-----------------------------------------------------\n");
			}
		}
	} else {
		printf("turn off vqe xxxxx\n");
	}
	s32Ret = CVI_AI_SetPubAttr(s32DevId, &AudinAttr);
	s32Ret = CVI_AI_Enable(s32DevId);
	if (s32Ret == CVI_FAILURE)
		printf("CVI_AI_Enable failure\n");

	s32Ret = CVI_AI_EnableChn(s32DevId, AiChn);
	if (bVqeOn == CVI_TRUE) {
		s32Ret = CVI_AI_SetTalkVqeAttr(
				s32DevId,
				0,
				0,
				0,
				(AI_TALKVQE_CONFIG_S *)pstAiVqeTalkAttr);
		s32Ret = CVI_AI_EnableVqe(s32DevId, AiChn);
	}

	CVI_AI_SetVolume(s32DevId, 7);
	CVI_AI_GetVolume(s32DevId, &vol);
	printf("ai vol:%d\n", vol);

	if (mode == AI_BIND_ANEC_TEST || mode == AI_UBIND_ANEC_TEST) {

		s32Ret = _update_Aenc_setting(pstAioAttr, &stAencAttr, PT_G711A,
					AudinAttr.enSamplerate, bVqeOn);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Error][%s]failure in params\n", __func__);
		}

		s32Ret = CVI_AENC_CreateChn(AeChn, &stAencAttr);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_CreateChn(%d) failed with %#x!\n", __func__,
				AeChn, s32Ret);
		}
	}

	if (mode == AI_BIND_ANEC_TEST) {

		stSrcChn.enModId = CVI_ID_AI;
		stSrcChn.s32DevId = 0;
		stSrcChn.s32ChnId = 0;
		stDestChn.enModId = CVI_ID_AENC;
		stDestChn.s32DevId = 0;
		stDestChn.s32ChnId = 0;
		CVI_AUD_SYS_Bind(&stSrcChn, &stDestChn);
	}

	printf("channel create complete\n");
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

	if (mode == AI_BIND_ANEC_TEST) {
		CVI_AUD_SYS_UnBind(&stSrcChn, &stDestChn);
		CVI_AENC_DestroyChn(AeChn);
		_destroy_Aenc_setting(&stAencAttr);
	} else if (mode == AI_UBIND_ANEC_TEST) {
		CVI_AENC_DestroyChn(AeChn);
		_destroy_Aenc_setting(&stAencAttr);
	}
	mode = 0;
	if (bVqeOn == CVI_TRUE)
		CVI_AI_DisableVqe(s32DevId, AiChn);
	CVI_AI_DisableChn(s32DevId, AiChn);
	CVI_AI_Disable(s32DevId);
	CVI_AUDIO_DEINIT();
	printf("Exit program-->master!!!\n");
	return 0;
}

