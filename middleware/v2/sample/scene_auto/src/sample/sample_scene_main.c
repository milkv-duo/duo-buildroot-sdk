#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "sample_scene.h"
#include "cvi_scene.h"
#include "cvi_scene_loadparam.h"

static TStreamInfo g_stStreamInfo;

#define DEFAULT_VIDEO_MODE				(1)
#define DEFAULT_PARAMETER_FOLDER_PATH	"/mnt/data/param"

void SAMPLE_SCENE_HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo) {
		SAMPLE_PRT("Program termination abnormally\n");
		CVI_SCENE_Deinit();
		SAMPLE_SCENE_VoRotation_Stop(&g_stStreamInfo);
	}
	exit(-1);
}

CVI_S32 SAMPLE_SCENE_HandleCommand(CVI_SCENE_VIDEO_MODE_S *pstSceneVideMode, CVI_BOOL *pbExit)
{
	CVI_S32 s32Key, s32Choice;
	CVI_S32 s32Ret = CVI_FAILURE;

	*pbExit = CVI_FALSE;

	printf("0.   Change new media type\n");
	printf("1.   Scene Pause\n");
	printf("2.   Scene Resume\n");
	printf("255. Exit\n");
	scanf("%d", &s32Key);

	switch (s32Key) {
	case 0:
		printf("Enter a valid videomode\n");
		s32Choice = -1;
		scanf("%d", &s32Choice);
		printf("Videomode change to %d\n", s32Choice);

		if (s32Choice < 0) {
			SAMPLE_PRT("Invalid input\n");
			return CVI_FAILURE;
		}

		s32Ret = CVI_SCENE_Pause(CVI_TRUE);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_SCENE_Pause fail\n");
			return s32Ret;
		}

		s32Ret = CVI_SCENE_SetSceneMode(&(pstSceneVideMode->astVideoMode[s32Choice]));
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_SCENE_SetSceneMode fail\n");
			return s32Ret;
		}

		s32Ret = CVI_SCENE_Pause(CVI_FALSE);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_SCENE_Resume fail\n");
			return s32Ret;
		}

		SAMPLE_PRT("Videomode change to %d - success\n", s32Choice);
		break;

	case 1:
		s32Ret = CVI_SCENE_Pause(CVI_TRUE);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_SCENE_Pause fail\n");
		} else {
			SAMPLE_PRT("The Scene Auto is Pause\n");
		}
		break;

	case 2:
		s32Ret = CVI_SCENE_Pause(CVI_FALSE);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_SCENE_Resume fail\n");
		} else {
			SAMPLE_PRT("The Scene Auto is Resume\n");
		}
		break;

	case 255:
		*pbExit = CVI_TRUE;
		break;

	default:
		break;
	}

	return CVI_SUCCESS;
}

int main(void)
{
	CVI_SCENE_VIDEO_MODE_S stSceneVideMode;
	CVI_SCENE_PARAM_S stSceneParam;
	CVI_S32 s32Ret = CVI_FAILURE;
	CVI_U8 u8SetSceneRetryTime;
	CVI_BOOL bLoopExit;

	signal(SIGINT, SAMPLE_SCENE_HandleSig);
	signal(SIGTERM, SAMPLE_SCENE_HandleSig);

	memset(&g_stStreamInfo, 0, sizeof(TStreamInfo));

	g_stStreamInfo.ViDev		= 0;
	g_stStreamInfo.ViPipe		= 0;
	g_stStreamInfo.ViChn		= 0;
	g_stStreamInfo.VpssGrp		= 0;
	g_stStreamInfo.VpssChn		= 0;
	g_stStreamInfo.VoDev		= 0;
	g_stStreamInfo.VoChn		= 0;

	s32Ret = SAMPLE_SCENE_VoRotation_Start(&g_stStreamInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("VO Rotate Streaming start fail, 0x%x\n", s32Ret);
		return s32Ret;
	}

	// Auto scene module pre-process (parse parameter)
	s32Ret = CVI_SCENE_CreateParam(DEFAULT_PARAMETER_FOLDER_PATH, &stSceneParam, &stSceneVideMode);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_SCENE_CreateParam fail\n");
		goto SCENE_INIT_FAIL;
	}

	// Auto scene module initial
	s32Ret = CVI_SCENE_Init(&stSceneParam);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_SCENE_Init fail\n");
		goto SCENE_INIT_FAIL;
	}

	// Wait for AE ready
	sleep(1);

	// Auto scene module run
	u8SetSceneRetryTime = 0;
	s32Ret = CVI_SCENE_SetSceneMode(&(stSceneVideMode.astVideoMode[DEFAULT_VIDEO_MODE]));
	while ((s32Ret != CVI_SUCCESS) && (u8SetSceneRetryTime < 3)) {
		sleep(1);
		s32Ret = CVI_SCENE_SetSceneMode(&(stSceneVideMode.astVideoMode[DEFAULT_VIDEO_MODE]));
		u8SetSceneRetryTime++;
	}
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_SCENE_SetSceneMode fail\n");
		goto SCENE_INIT_FAIL;
	}

	bLoopExit = CVI_FALSE;
	do {
		// Auto scene module (change scene mode)
		SAMPLE_SCENE_HandleCommand(&stSceneVideMode, &bLoopExit);
	} while (!bLoopExit);

	// Auto scene module de-initial
	s32Ret = CVI_SCENE_Deinit();
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_SCENE_Deinit fail\n");
		goto SCENE_INIT_FAIL;
	}

	s32Ret = SAMPLE_SCENE_VoRotation_Stop(&g_stStreamInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("VO Rotate Streaming stop fail, 0x%x\n", s32Ret);
		return s32Ret;
	}

	if (s32Ret == CVI_SUCCESS)
		SAMPLE_PRT("sample_scene exit success!\n");
	else
		SAMPLE_PRT("sample_scene exit abnormally!\n");

	return s32Ret;

SCENE_INIT_FAIL:
	SAMPLE_SCENE_VoRotation_Stop(&g_stStreamInfo);
	return -1;
}
