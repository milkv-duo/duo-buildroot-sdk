/*usage from C std lib */
#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifndef PLATFORM_RTOS
#include <errno.h>
#endif
/* usage from cviaudio header */
#include "struct.h"
#include "cvi_comm_aio.h"
#include "cvi_ssp_interface.h"
#define  _AUDIO_ALGO_INTERFACE_VERSION_TAG_ "Aud_AlgoInterface_ver20220128"
#define _AUDIO_ALGO_INTERNAL_VERSION_ "CVITEK_AEC_Algo_20220128"
//turn on all the mask to user config, do not wrap or simply to 3 function AGC/ANR/AEC
#define CVIAUDIO_ALGO_FUNCTION_AGC 0x01
#define CVIAUDIO_ALGO_FUNCTION_ANR 0x02
#define CVIAUDIO_ALGO_FUNCTION_AEC  0x04

//TODO: step 1 add include header for specific algo
/* porting layer for specific algorithm */
//#include "cvi_aec_interface_v2.h"
//#include "cvi_3a_internal.h"

#define AEC_FRAME_LENGTH (160)
#ifndef CHECK_NULL_PTR
#define CHECK_NULL_PTR(ptr)                                                                                            \
	do {                                                                                                           \
		if (!(ptr)) {                                                                                     \
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);                                 \
			return -1;                                                                            \
		}                                                                                                      \
	} while (0)
#endif

int notch_dbglevel = 2;

#define CVIAUD_NOTCH_ERR_PRINTF(fmt, args...) \
	do { \
		if (notch_dbglevel > 0) \
			printf("[cviaudio][error][%s][%d] "fmt, __func__, __LINE__, ##args);\
	} while (0)

#define CVIAUD_NOTCH_DBG_PRINTF(fmt, args...) \
	do { \
		if (notch_dbglevel > 1) \
			printf("[cviaudio][info] "fmt, ##args);\
	} while (0)

#define CVIAUD_NOTCH_TRA_PRINTF(fmt, args...) \
	do { \
		if (notch_dbglevel > 2) \
			printf("[cvitrace][%s][%d] "fmt, __func__, __LINE__, ##args);\
	} while (0)

#ifndef SSP_UNUSED_REF
#define SSP_UNUSED_REF(X)  ((X) = (X))
#endif

#ifndef DEFAULT_BYTES_PER_SAMPLE
#define DEFAULT_BYTES_PER_SAMPLE 2
#endif

#ifndef CVI_AEC_FRAMES_LEN
#define CVI_AEC_FRAMES_LEN 160
#endif
//#define IBUFFSIZE 160/* Input buffer size */
#define MAX_IBUFFSIZE 4096
/* sample code from ffmpeg resampling_audio.c -----------end */
bool gv_bEnableAEC;//TOD: this should not exist if aec, agc use the same handle


/**************************************************************************************
 * Function:    CviAud_Algo_Init
 *
 * Description: allocate and init for audio algorithm
 *
 * Inputs:      s32FunctMask: CVIAUDIO_ALGO_FUNCTION_AGC
 * 						/CVIAUDIO_ALGO_FUNCTION_AGC
 * 						/CVIAUDIO_ALGO_FUNCTION_AGC
 * param_info:input info needed for specific algorithm lib
 * Outputs:  none
 *
 * Return: algo handle
 **************************************************************************************/
void *CviAud_Algo_Init(int s32FunctMask, void *param_info)
{
	int s32Ret;
	void *_handle = NULL;
#ifdef FAST_MATH_CALC
	printf("[cviaudio][SSP]Using Fast Math[replace atan/logarithm]\n");
#endif
	const AI_TALKVQE_CONFIG_S *pstVqeConfig = (const AI_TALKVQE_CONFIG_S *)param_info;
	ssp_para_struct ssp_para_info;
	ssp_para_struct *p_ssp_para_info = &ssp_para_info;

	s32FunctMask = s32FunctMask;
	memset(p_ssp_para_info, 0, sizeof(ssp_para_struct));
	// set to default value first ....
	ssp_para_info.para_fun_config = 15;
	ssp_para_info.para_delay_sample = 1;
	ssp_para_info.para_dg_target = 1;
	ssp_para_info.para_aec_init_filter_len = 2;
	ssp_para_info.para_aec_filter_len = 13;
	ssp_para_info.para_aes_std_thrd = 37;
	ssp_para_info.para_aes_supp_coeff = 60;
	ssp_para_info.para_nr_init_sile_time = 0;
	ssp_para_info.para_nr_snr_coeff = 15;
	ssp_para_info.para_agc_max_gain = 0;
	ssp_para_info.para_agc_target_high = 2;
	ssp_para_info.para_agc_target_low = 72;
	ssp_para_info.para_agc_vad_ena = 1;
	ssp_para_info.para_notch_freq = 0;
	ssp_para_info.para_spk_fun_config = 0;
	ssp_para_info.para_spk_agc_max_gain = 0;
	ssp_para_info.para_spk_agc_target_high = 8;
	ssp_para_info.para_spk_agc_target_low = 72;


	//update value from use layer
	//ssp_para_info.para_sample_rate = pstVqeConfig->s32WorkSampleRate;//only support 8k and 16k
	ssp_para_info.para_client_config = pstVqeConfig->para_client_config;
	ssp_para_info.para_fun_config = pstVqeConfig->u32OpenMask;
	//update aec config
	ssp_para_info.para_aec_filter_len = pstVqeConfig->stAecCfg.para_aec_filter_len;
	ssp_para_info.para_aes_std_thrd = pstVqeConfig->stAecCfg.para_aes_std_thrd;
	ssp_para_info.para_aes_supp_coeff = pstVqeConfig->stAecCfg.para_aes_supp_coeff;
	//update nr config
	ssp_para_info.para_nr_init_sile_time = pstVqeConfig->stAnrCfg.para_nr_init_sile_time;
	ssp_para_info.para_nr_snr_coeff = pstVqeConfig->stAnrCfg.para_nr_snr_coeff;;
	//update agc config
	ssp_para_info.para_agc_max_gain = pstVqeConfig->stAgcCfg.para_agc_max_gain;
	ssp_para_info.para_agc_target_high = pstVqeConfig->stAgcCfg.para_agc_target_high;;
	ssp_para_info.para_agc_target_low = pstVqeConfig->stAgcCfg.para_agc_target_low;
	ssp_para_info.para_agc_vad_ena =  pstVqeConfig->stAgcCfg.para_agc_vad_ena;
	//udpate sw aec config
	ssp_para_info.para_aec_init_filter_len = pstVqeConfig->stAecDelayCfg.para_aec_init_filter_len;
	ssp_para_info.para_dg_target = pstVqeConfig->stAecDelayCfg.para_dg_target;
	ssp_para_info.para_delay_sample = pstVqeConfig->stAecDelayCfg.para_delay_sample;
	//update notch filter setting for specific customer
	ssp_para_info.para_notch_freq = pstVqeConfig->para_notch_freq;//only support 8k and 16k
	#if 0 //leave the spk setting to CviAud_SpkAlgo_Init
	ssp_para_info.para_spk_fun_config = pstVqeConfig->stSpkCfg.para_spk_fun_config;
	ssp_para_info.para_spk_agc_max_gain = pstVqeConfig->stSpkCfg.para_spk_agc_max_gain;
	ssp_para_info.para_spk_agc_target_high = pstVqeConfig->stSpkCfg.para_spk_agc_target_high;
	ssp_para_info.para_spk_agc_target_low = pstVqeConfig->stSpkCfg.para_spk_agc_target_low;
	#endif


	_handle = (void *)audio_ssp_init((void *)p_ssp_para_info, pstVqeConfig->s32WorkSampleRate);


	return (void *)_handle;


}


/**************************************************************************************
 * Function:    CviAud_Algo_Process
 *
 * Description: Process the audio algorithm through interface api CviAud_Algo_Process
 *
 * Inputs:      pHandle: Create by CviAud_Algo_Init
 *              mic_in:  input data address for entering audio algo
 *              ref_in: reference data for audio AEC algorithm
 * 		iLength: input samples count for each process function call
 *
 * Outputs:     out:  pointer to outputbuf
 *
 * Return:    output samples
 * Notes:       sure insamps < MAXFRAMESIZE
 **************************************************************************************/
int CviAud_Algo_Process(void *pHandle,  uint64_t mic_in,
			uint64_t ref_in, uint64_t out, int iLength)
{
//TODO: step 3[ok]  implement the function acquired
	SSP_UNUSED_REF(iLength);
	int ret = 0;
	int s32RetTotalSamples = 0;

	if (pHandle == NULL) {
		printf("Null input [%s][%d]\n", __func__, __LINE__);
		return -1;
	}

	if (iLength != AEC_FRAME_LENGTH) {
		printf("input length only support 160 samples[%s][%d]\n", __func__, __LINE__);
		return -1;
	}

	ret = audio_ssp_process(pHandle, mic_in, ref_in, out, AEC_FRAME_LENGTH);
	if (ret != 0) {
		printf("audio_ssp_process error ret(%d).\n", ret);
		return -1;
	}
	s32RetTotalSamples = AEC_FRAME_LENGTH;

	return s32RetTotalSamples;

}

/**************************************************************************************
 * Function:    CviAud_Algo_Fun_Config
 *
 * Description: free platform-specific data allocated by CviAud_Algo_Init
 *
 * Inputs:      instance pointer (pHandle)
 * Outputs:	none
 *
 * Return:      CVI_SUCCESS / CVI_FAILURE
 **************************************************************************************/
int CviAud_Algo_Fun_Config(void *pHandle, int u32OpenMask)
{
	int ret = 0;

	if (pHandle == NULL) {
		printf("Null input [%s][%d]\n", __func__, __LINE__);
		return -1;
	}

	ret = audio_ssp_fun_config(pHandle, u32OpenMask);
	if (ret != 0) {
		printf("audio_ssp_process error ret(%d).\n", ret);
		return -1;
	}

	return ret;

}

/**************************************************************************************
 * Function:    CviAud_Algo_DeInit
 *
 * Description: free platform-specific data allocated by CviAud_Algo_Init
 *
 * Inputs:      instance pointer (pHandle)
 * Outputs:	none
 *
 * Return:      CVI_SUCCESS / CVI_FAILURE
 **************************************************************************************/
void CviAud_Algo_DeInit(void *pHandle)
{

	//VQE_UNUSED_REF(pHandle);
	//struct st3AInfo *pst3Ainfo = (struct st3AInfo *)pHandle;

	if (audio_ssp_deinit(pHandle) != 0) {
		CVIAUD_NOTCH_ERR_PRINTF("3a deinit err\n");
		return;
	}

}



/**************************************************************************************
 * Function:    CviAud_Algo_GetVersion
 *
 * Description: Get version info: algorithrm source , date  through this api
 *
 * Inputs:      None
 * Outputs:	None
 *
 * Return:     Version info in string type
 **************************************************************************************/
void  CviAud_Algo_GetVersion(char *pstrVersion)
{
	CVIAUD_NOTCH_DBG_PRINTF("CviAud algo interface[%s]\n", _AUDIO_ALGO_INTERFACE_VERSION_TAG_);
	CVIAUD_NOTCH_DBG_PRINTF("CviAud  algo lib[%s]\n", _AUDIO_ALGO_INTERNAL_VERSION_);
	sprintf(pstrVersion, _AUDIO_ALGO_INTERNAL_VERSION_);
	return;
}

void *CviAud_SpkAlgo_Init(int s32FunctMask, void *param_info)
{

	int s32Ret;
	void *_handle = NULL;

	s32FunctMask = s32FunctMask;
	const AO_VQE_CONFIG_S *pstVqeConfig = (const AO_VQE_CONFIG_S *)param_info;
	ssp_para_struct ssp_para_info;
	ssp_para_struct *p_ssp_para_info = &ssp_para_info;

	memset(p_ssp_para_info, 0, sizeof(ssp_para_struct));
	// set to default value first ....
	//ignore the mic in param, since we only use for SPK
	ssp_para_info.para_fun_config = 0;
	ssp_para_info.para_aec_filter_len = 0;
	ssp_para_info.para_aes_std_thrd = 0;
	ssp_para_info.para_aes_supp_coeff = 0;
	ssp_para_info.para_nr_init_sile_time = 0;
	ssp_para_info.para_nr_snr_coeff = 0;
	ssp_para_info.para_agc_max_gain = 0;
	ssp_para_info.para_agc_target_high = 0;
	ssp_para_info.para_agc_target_low = 0;
	ssp_para_info.para_agc_vad_ena = 0;
	ssp_para_info.para_notch_freq = 0;
	//Focus on the spk config
	ssp_para_info.para_spk_agc_max_gain = 0;
	ssp_para_info.para_spk_agc_target_high = 8;
	ssp_para_info.para_spk_agc_target_low = 72;


	//update value from use layer
	//ssp_para_info.para_sample_rate = pstVqeConfig->s32WorkSampleRate;//only support 8k and 16k
	ssp_para_info.para_spk_fun_config = pstVqeConfig->u32OpenMask;
	ssp_para_info.para_spk_agc_max_gain = pstVqeConfig->stAgcCfg.para_agc_max_gain;
	ssp_para_info.para_spk_agc_target_high = pstVqeConfig->stAgcCfg.para_agc_target_high;
	ssp_para_info.para_spk_agc_target_low = pstVqeConfig->stAgcCfg.para_agc_target_low;

	ssp_para_info.para_spk_eq_nband = pstVqeConfig->stEqCfg.para_spk_eq_nband;
	ssp_para_info.para_spk_eq_freq[0] = pstVqeConfig->stEqCfg.para_spk_eq_freq[0];
	ssp_para_info.para_spk_eq_gain[0] = pstVqeConfig->stEqCfg.para_spk_eq_gain[0];
	ssp_para_info.para_spk_eq_qfactor[0] = pstVqeConfig->stEqCfg.para_spk_eq_qfactor[0];
	ssp_para_info.para_spk_eq_freq[1] = pstVqeConfig->stEqCfg.para_spk_eq_freq[1];
	ssp_para_info.para_spk_eq_gain[1] = pstVqeConfig->stEqCfg.para_spk_eq_gain[1];
	ssp_para_info.para_spk_eq_qfactor[1] = pstVqeConfig->stEqCfg.para_spk_eq_qfactor[1];
	ssp_para_info.para_spk_eq_freq[2] = pstVqeConfig->stEqCfg.para_spk_eq_freq[2];
	ssp_para_info.para_spk_eq_gain[2] = pstVqeConfig->stEqCfg.para_spk_eq_gain[2];
	ssp_para_info.para_spk_eq_qfactor[2] = pstVqeConfig->stEqCfg.para_spk_eq_qfactor[2];
	ssp_para_info.para_spk_eq_freq[3] = pstVqeConfig->stEqCfg.para_spk_eq_freq[3];
	ssp_para_info.para_spk_eq_gain[3] = pstVqeConfig->stEqCfg.para_spk_eq_gain[3];
	ssp_para_info.para_spk_eq_qfactor[3] = pstVqeConfig->stEqCfg.para_spk_eq_qfactor[3];
	ssp_para_info.para_spk_eq_freq[4] = pstVqeConfig->stEqCfg.para_spk_eq_freq[4];
	ssp_para_info.para_spk_eq_gain[4] = pstVqeConfig->stEqCfg.para_spk_eq_gain[4];
	ssp_para_info.para_spk_eq_qfactor[4] = pstVqeConfig->stEqCfg.para_spk_eq_qfactor[4];


	_handle = (void *)audio_ssp_spk_init((void *)p_ssp_para_info, pstVqeConfig->s32WorkSampleRate);


	return (void *)_handle;


}

int CviAud_SpkAlgo_Process(void *pHandle,  short *spk_in,
			short *spk_out, int iLength)
{
	SSP_UNUSED_REF(iLength);
	int ret = 0;
	int s32RetTotalSamples = 0;

	if (pHandle == NULL) {
		printf("Null input [%s][%d]\n", __func__, __LINE__);
		return -1;
	}

	if (iLength != AEC_FRAME_LENGTH) {
		printf("input length only support 160 samples[%s][%d]\n", __func__, __LINE__);
		return -1;
	}

	ret = audio_ssp_spk_process(pHandle, spk_in, spk_out, AEC_FRAME_LENGTH);
	if (ret != 0) {
		printf("audio_ssp_spk_process error ret(%d).\n", ret);
		return -1;
	}
	s32RetTotalSamples = AEC_FRAME_LENGTH;

	return s32RetTotalSamples;

}

void CviAud_SpkAlgo_DeInit(void *pHandle)
{

	//VQE_UNUSED_REF(pHandle);
	//struct st3AInfo *pst3Ainfo = (struct st3AInfo *)pHandle;

	if (audio_ssp_spk_deinit(pHandle) != 0) {
		CVIAUD_NOTCH_ERR_PRINTF("3a deinit err\n");
		return;
	}

}
