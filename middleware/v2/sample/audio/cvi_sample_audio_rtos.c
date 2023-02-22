/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/cvi_sample_audio_rtos.c
 * Description:example for audio api flow
 * such as audio in, audio out, audio trancode flow
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
//add for audio kernel mode sample test case
//headers for cviaduio_core.ko & cvi_rtos_cmdqu.ko
#include "cviaudio_ioctl_cmd.h"
#include "rtos_cmdqu.h"
#include "cviaudio_kernel_mode.h"
#include "cvi_sys.h"

//define macro check
#ifndef SMP_AUD_UNUSED_REF
#define SMP_AUD_UNUSED_REF(X)  ((X) = (X))
#endif

#ifdef AUD_SUPPORT_KERNEL_MODE
static int _console_request(char *printout, int defalut_val);
static int GET_CONSOLE_REQ(int status, char *printout, int default_val);
static CVI_BOOL _update_vqe_setting(AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr);
static CVI_BOOL _update_aec_setting(AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr);
#define _CONSOLE_REQ(X, Y) GET_CONSOLE_REQ(0, (char *)X, (int)Y)

//globacl variable for kernel module fd
CVI_S32 s32AudioCoreKer;

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
	pstAiVqeTalkAttr->u32OpenMask |= (LP_AEC_ENABLE | NLP_AES_ENABLE);
	printf("pstAiVqeTalkAttr:u32OpenMask[0x%x]\n", pstAiVqeTalkAttr->u32OpenMask);
	return CVI_FALSE;
}

static CVI_BOOL _update_vqe_setting(AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr)
{
	if (pstAiVqeTalkAttr == NULL)
		return CVI_FALSE;

	pstAiVqeTalkAttr->u32OpenMask |= (NR_ENABLE|AGC_ENABLE|DCREMOVER_ENABLE);

	AUDIO_AGC_CONFIG_S st_AGC_Setting;
	AUDIO_ANR_CONFIG_S st_ANR_Setting;

	st_AGC_Setting.para_agc_max_gain = 0;
	st_AGC_Setting.para_agc_target_high = 2;
	st_AGC_Setting.para_agc_target_low = 72;
	st_AGC_Setting.para_agc_vad_ena = CVI_TRUE;
	st_ANR_Setting.para_nr_snr_coeff = 15;
	st_ANR_Setting.para_nr_init_sile_time = 0;
	pstAiVqeTalkAttr->stAgcCfg = st_AGC_Setting;
	pstAiVqeTalkAttr->stAnrCfg = st_ANR_Setting;

	pstAiVqeTalkAttr->para_notch_freq = 0;
	printf("pstAiVqeTalkAttr:u32OpenMask[0x%x]\n", pstAiVqeTalkAttr->u32OpenMask);
	return CVI_TRUE;
}

static int _console_request(char *printout, int default_val)
{
	char s_option[128];
	int s32Ret = -1;

	printf("\e[0;32m=====================================\n");
	fflush(stdout);
	fflush(stdin);
	if (printout != NULL) {
		printf(printout);
		fgets(s_option, 10, stdin);
		if (s_option[0] == '\n') {
			s32Ret = default_val;
			printf("input default val[%d]\n", s32Ret);
		} else {
			s32Ret = atoi(s_option);
			printf("input [%d]\n", s32Ret);
		}
	} else
		printf("Error input type[%s][%d]\n", __func__, __LINE__);

	printf("\e[0;32m=====================================\n");
	fflush(stdin);
	return s32Ret;
}

static int GET_CONSOLE_REQ(int audio_status, char *printout, int default_val)
{
	if (audio_status) {
		printf("[Error]Not support [%s][%d]\n", __func__, __LINE__);
		return 0;
	} else {
		return _console_request(printout, default_val);
	}
	return 0;
}

CVI_VOID SAMPLE_AUDIO_Usage(CVI_VOID)
{
	printf("\n\n/Usage:./sample_audio <index>/\n");
	printf("\tindex and its function list below\n");
	printf("\t1:  SAMPLE_AUDIO_RTOS_UNIT_TEST_KERNEL_MODE\n");
	printf("\t2:  SAMPLE_AUDIO_RTOS_UNIT_TEST_BLOCK_MODE\n");
	printf("\t3:  SAMPLE_AUDIO_GET_AUDIO_FRAME_BY_FRAME_RTOS_SSP\n");
	printf("\t4:  For debug purpose\n");

}



/******************************************************************************/
/* function : main*/
/******************************************************************************/
void AUDIO_HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo) {

		//destroy all thread
		printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
	}

	exit(0);
}


CVI_S32 s32RtosCore;
static int devm_rtos_fd = -1;
CVI_S32 SAMPLE_AUDIO_RTOS_UNIT_TEST_BLOCK_MODE(void)
{
	printf("File saving in kernel space after handler\n");
	CVI_CHAR devName[255];
	int ret = 0;
	int counter = 0;
	FILE		*fd_user;
	CVI_S32 s32Ret;
	CVI_U64 u64PhyAddr = 0;
	CVI_VOID *pVirAddr = NULL;
	//char *tmp = NULL;

	//printf("[%s][%d]...\n", __func__, __LINE__);
	CVI_SYS_Init();
	printf("[%s][%d]Using ion buffer for phy/vir addr request...\n", __func__, __LINE__);

	sprintf(devName, "/dev/%s", RTOS_CMDQU_DEV_NAME);
	s32RtosCore = open(devName, O_RDWR | O_DSYNC);
	printf("open device (%s) fd: %d\n", devName, s32RtosCore);
	if (s32RtosCore <= 0) {
		printf("open device (%s) FAILURE: %d\n", devName, __LINE__);
		return CVI_FAILURE;

	} else
		printf("open device (%s) SUCCESS: %d\n", devName, __LINE__);

	if (devm_rtos_fd == -1) {
		printf("open rtos devm for fd\n");
		devm_rtos_fd = open("/dev/mem", O_RDWR | O_SYNC);
		if (devm_rtos_fd == -1) {
			printf("[cviaudio][kernel_mode]cannot open '/dev/mem'\n");
		}
	}

	//step 1: send init command
	cmdqu_t cmdq;

	memset(&cmdq, 0, sizeof(struct cmdqu_t));
	//sys_cache_flush_p(SspRtosUnitTesCfgPhy, sizeof(ST_SSP_RTOS_INIT));
	cmdq.ip_id = IP_AUDIO;
	cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_INIT;
	cmdq.block = 1;
	cmdq.resv.mstime = -1;
	cmdq.param_ptr = 0;

	printf("[%s][%d]...send init\n", __func__, __LINE__);
	ret = ioctl(s32RtosCore, RTOS_CMDQU_SEND_WAIT, &cmdq);
	printf("[%s][%d]...send init out\n", __func__, __LINE__);
	if (ret)
		printf("v][%s][%d]...ioctl error init\n", __func__, __LINE__);

	//step 2: get the frame from file and save
	memset(&cmdq, 0, sizeof(struct cmdqu_t));
	fd_user = fopen("/tmp/user.raw", "wb");
	//sys_cache_flush_p(SspRtosUnitTesCfgPhy, sizeof(ST_SSP_RTOS_INIT));

	s32Ret = CVI_SYS_IonAlloc(&u64PhyAddr, &pVirAddr, "audiortos_unittest", 4*1024);
	if (s32Ret) {
		printf("CVI_SYS_IonAlloc failed >< with %#x\n", s32Ret);
		return 0;

	} else
		printf("CVI_SYS_IonAlloc ok!!!\n");

	while (counter < 2000) {
		counter += 1;
		cmdq.ip_id = IP_AUDIO;
		cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_GET;
		cmdq.param_ptr = u64PhyAddr;//physical address
		cmdq.block = 1;
		cmdq.resv.mstime = 10000;

		ret = ioctl(s32RtosCore, RTOS_CMDQU_SEND_WAIT, &cmdq);

		if (ret) {
			printf("[%s][%d]...ioctl error init\n", __func__, __LINE__);
			break;
		}
		CVI_SYS_IonInvalidateCache(u64PhyAddr, pVirAddr,  4*1024);
		//tmp = (char *)pVirAddr;
		//printf("read out and check[%s][%d]...send init out[0x%x][0x%x][0x%x]\n",
		//		__func__, __LINE__, tmp[0], tmp[1], tmp[2]);
		fwrite(pVirAddr, 1, 320, fd_user);
	}
	fclose(fd_user);
	printf("[%s][%d]finished!!!\n", __func__, __LINE__);
	return 0;

}

CVI_S32 SAMPLE_AUDIO_DEBUG(void)
{
	CVI_CHAR devName[255];
	int ret = 0;
#ifndef CVIAUDIO_CEO_DEV_NAME
#define CVIAUDIO_CEO_DEV_NAME			"cviaudio_core"
#endif

	sprintf(devName, "/dev/%s", CVIAUDIO_CEO_DEV_NAME);
	s32AudioCoreKer = open(devName, O_RDWR | O_DSYNC);
	printf("open device (%s) fd: %d\n", devName, s32AudioCoreKer);
	if (s32AudioCoreKer <= 0) {
		printf("open device (%s) FAILURE: %d\n", devName, __LINE__);
		return CVI_FAILURE;

	} else
		printf("open device (%s) SUCCESS: %d\n", devName, __LINE__);

	ret = ioctl(s32AudioCoreKer, CVIAUDIO_IOCTL_SSP_DEBUG, NULL);
	if (ret != 0) {
		printf("[Error]ioctl[%d]  CVIAUDIO_IOCTL_SSP_DEBUG fail with %d\n",
			CVIAUDIO_IOCTL_SSP_DEBUG, ret);
	} else
		printf("[cviaudio]CVIAUDIO_IOCTL_SSP_DEBUG success!!!\n");

	return CVI_SUCCESS;
}
CVI_S32 SAMPLE_AUDIO_RTOS_UNIT_TEST_KERNEL_MODE(void)
{
	CVI_CHAR devName[255];
	int ret = 0;
#ifndef CVIAUDIO_CEO_DEV_NAME
#define CVIAUDIO_CEO_DEV_NAME			"cviaudio_core"
#endif

	sprintf(devName, "/dev/%s", CVIAUDIO_CEO_DEV_NAME);
	s32AudioCoreKer = open(devName, O_RDWR | O_DSYNC);
	printf("open device (%s) fd: %d\n", devName, s32AudioCoreKer);
	if (s32AudioCoreKer <= 0) {
		printf("open device (%s) FAILURE: %d\n", devName, __LINE__);
		return CVI_FAILURE;

	} else
		printf("open device (%s) SUCCESS: %d\n", devName, __LINE__);



	ret = ioctl(s32AudioCoreKer, CVIAUDIO_IOCTL_SSP_UNIT_TEST, NULL);
	if (ret != 0) {
		printf("[Error]ioctl[%d]  CVIAUDIO_INIT_CHECK fail with %d\n",
			CVIAUDIO_IOCTL_SSP_UNIT_TEST, ret);
		goto KERNEL_MODE_ERR;
	} else {
		printf("[cviaudio][kernel_mode]Init Counter\n");
		//s32Ret = CVI_SUCCESS;
	}
	printf("user space keep running ...!\n");
	while (1) {
		usleep(500);
	}
KERNEL_MODE_ERR:
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_AUDIO_GET_AUDIO_FRAME_BY_FRAME_RTOS_SSP(CVI_VOID)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32SampleRate = 0;
	CVI_S32 s32PeriodSize = 0;
	CVI_S32 s32RecSeconds = 0;
	CVI_S32 s32RecLoops = 0;
	AUDIO_FRAME_S stFrame;
	CVI_BOOL bVqeOn = CVI_FALSE;
	CVI_BOOL bAecOn = CVI_FALSE;
	AI_TALKVQE_CONFIG_S stAiVqeTalkAttr;
	AI_TALKVQE_CONFIG_S *pstAiVqeTalkAttr = (AI_TALKVQE_CONFIG_S *)&stAiVqeTalkAttr;
	FILE *fp_rec = CVI_NULL;
	//CVI_U64 u64PhyAddr = 0;
	//CVI_VOID *pVirAddr = NULL;

	memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
	memset(&stAiVqeTalkAttr, 0, sizeof(AI_TALKVQE_CONFIG_S));
	s32SampleRate = _CONSOLE_REQ("Enter sample rate(only support 8000/16000)\n",
					8000);
	s32PeriodSize = _CONSOLE_REQ("Enter period_size(in samples), ex 160/320/640\n",
					160);
	bVqeOn = (CVI_BOOL)_CONSOLE_REQ("bVqeOn? 0:No  1:Yes (default:0)\n", 0);

	if (bVqeOn == CVI_FALSE)
		pstAiVqeTalkAttr = NULL;
	else {
		if ((s32SampleRate == 8000) ||
		    (s32SampleRate == 16000)) {
			_update_vqe_setting(pstAiVqeTalkAttr);
			pstAiVqeTalkAttr->s32WorkSampleRate = s32SampleRate;

			printf("Turn On AEC?\n");
			bAecOn = (CVI_BOOL)_CONSOLE_REQ("bAecOn 0:No  1:Yes (default:0)\n", 0);
			if (bAecOn == CVI_TRUE) {
				_update_aec_setting(pstAiVqeTalkAttr);
				printf("----------------notice-------------------------------\n");
				printf("AEC will need to setup record in to channel Count = 2\n");
				printf("AEC will output only one single channel with 2 channels in\n");
				printf("-----------------------------------------------------\n");
			}
		} else {
			//not support VQE aside of 8k/16k input
			printf("VQE only support on 8k/16k sample rate\n");
			bVqeOn = CVI_FALSE;
			bAecOn = CVI_FALSE;
		}
	}
	if (pstAiVqeTalkAttr)
		printf("[%s]open_mask[0x%x]\n", __func__, pstAiVqeTalkAttr->u32OpenMask);
	s32Ret = CVI_AI_EnableExtSSp(s32SampleRate, s32PeriodSize, pstAiVqeTalkAttr);
	if (s32Ret != CVI_SUCCESS) {
		printf("[Error][%s][%d]\n", __func__, __LINE__);
		goto EXIT_TEST;
		return CVI_FAILURE;
	}

	s32RecSeconds = 15;
	s32RecLoops = (s32RecSeconds * s32SampleRate) / s32PeriodSize;
	fp_rec = fopen("/tmp/sample_record.raw", "wb");
	if (!fp_rec)
		return CVI_FAILURE;

	printf("force rec 15 seconds ---------------------->\n");
	while (s32RecLoops--) {
		s32Ret = CVI_AI_GetFrameExtSsp(&stFrame);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Error][%s][%d]\n", __func__, __LINE__);
			goto EXIT_TEST;

		} else {
			fwrite(stFrame.u64VirAddr[0], 1,
				(stFrame.u32Len * 2), fp_rec);
		}

	}

	if (fp_rec)
		fclose(fp_rec);
	//if (pVirAddr)
	//	CVI_SYS_IonFree(u64PhyAddr, pVirAddr);
	s32Ret = CVI_AI_DisableExtSsp();
	if (s32Ret != CVI_SUCCESS)
		printf("[Error][%s][%d]\n", __func__, __LINE__);

	printf("[%s]test...finished s32RecLoops[%d]!!!!!!!!!!!!!!!!!!!!\n", __func__, s32RecLoops);
	return CVI_SUCCESS;
EXIT_TEST:
	if (fp_rec)
		fclose(fp_rec);
	printf("[Error][%s]test...failure\n", __func__);
	return CVI_FAILURE;
}
#endif

CVI_S32 main(int argc, char *argv[])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32Index = 0;

#ifndef AUD_SUPPORT_KERNEL_MODE

	printf("audio not suppot kernel mode in this chip!!\n");

	SMP_AUD_UNUSED_REF(argc);
	SMP_AUD_UNUSED_REF(argv);
	SMP_AUD_UNUSED_REF(s32Ret);
	SMP_AUD_UNUSED_REF(u32Index);
	return CVI_FAILURE;
#else
	printf("auido support kernel mode with cviaudio_core.ko insert first\n");



	if (argc  <  2) {
		SAMPLE_AUDIO_Usage();
		return CVI_FAILURE;
	}

	u32Index = atoi(argv[1]);

	if (u32Index > 6) {
		printf("Wrong option[%d]...\n", u32Index);
		SAMPLE_AUDIO_Usage();
		return CVI_FAILURE;
	}

	signal(SIGINT, AUDIO_HandleSig);
	signal(SIGTERM, AUDIO_HandleSig);

	printf("sample_audio_rtos:Enter command id =[%d]\n", u32Index);


	switch (u32Index) {
	case 1: {
		printf("test ssp unit test in cviaudio_core.ko and freeRtos\n");
		SAMPLE_AUDIO_RTOS_UNIT_TEST_KERNEL_MODE();
		break;
	}
	case 2: {
		printf("test ssp unit test in cvi_rtos_cmd.ko and freeRtos\n");
		SAMPLE_AUDIO_RTOS_UNIT_TEST_BLOCK_MODE();
		break;
	}
	case 3: {
		printf("[sample code]recording frame by frame with external RTOS SSP\n");
		SAMPLE_AUDIO_GET_AUDIO_FRAME_BY_FRAME_RTOS_SSP();
		break;
	}
	case 4: {
		SAMPLE_AUDIO_DEBUG();
		break;
	}
	default: {
		printf("Invalid input command[%d]\n", u32Index);
		break;
	}
	}

	return s32Ret;
#endif
}






