/*
 * Copyright (C) Cvitek Co., Ltd. All rights reserved.
 *
 * File Name: cvi_rpc_upslave.c
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
#include <sys/time.h>
#include <signal.h>
#include "cvi_audio.h"
#include "cvi_type.h"

CVI_S32 s32DevId;
CVI_S32 AiChn;
CVI_S32 AeChn;
CVI_S32 running = 1;
CVI_S32 mode;

#define AI_BIND_ANEC_TEST 1
#define AI_UNBIND_AENC_TEST 2
pthread_t *pThread = (pthread_t *)(CVI_NULL);

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

CVI_VOID *AencGetThreadProc(CVI_VOID *arg)
{
	CVI_S32 s32Ret;
	AUDIO_STREAM_S stStream;

	printf("[%s][%d][%p]\n", __func__, __LINE__, arg);
	FILE *pfd_aenc = fopen("aenc_get.g711a", "wb");

	if (!pfd_aenc) {
		printf("[%s][%d]fopen error\n", __func__, __LINE__);
		return NULL;
	}

	while (running) {
		s32Ret = CVI_AENC_GetStream(AeChn, &stStream, CVI_FALSE);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_GetStream(%d), failed with %#x!\n",
			 __func__, AeChn, s32Ret);
			return NULL;
		}

		if (stStream.u32Len != 0)
			(CVI_VOID)fwrite(stStream.pStream, 1, stStream.u32Len, pfd_aenc);

		s32Ret = CVI_AENC_ReleaseStream(AeChn, &stStream);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_ReleaseStream(%d), failed with %#x!\n",
			       __func__, AeChn, s32Ret);
			return NULL;
		}
	}
	fclose(pfd_aenc);
	return 0;
}

CVI_VOID *AiGetThreadProc(CVI_VOID *arg)
{
	AUDIO_FRAME_S stFrame;
	AEC_FRAME_S   stAecFrm;
	CVI_S32 s32Ret;
	CVI_S32 s32ChnCnt = 2;

	printf("[%s][%d][%p]\n", __func__, __LINE__, arg);
	FILE *pfd_out = fopen("ai_getframe.raw", "wb");

	if (!pfd_out) {
		printf("[%s][%d]fopen error\n", __func__, __LINE__);
		return NULL;
	}

	while (running) {

		s32Ret = CVI_AI_GetFrame(s32DevId, AiChn,
						&stFrame,
						&stAecFrm,
						-1);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Error]CVI_AI_GetFrame none!!\n");
			break;
		}

		if (stFrame.u32Len != 0) {
			fwrite(stFrame.u64VirAddr[0], 1, (stFrame.u32Len * s32ChnCnt * 2), pfd_out);
		} else
			printf("[%s][%d]...size[%d]\n", __func__, __LINE__, stFrame.u32Len);
	}
	fclose(pfd_out);
	return 0;
}


CVI_VOID *AencUbindThreadProc(CVI_VOID *arg)
{
	CVI_S32 s32Ret;
	AUDIO_FRAME_S stFrame;
	AEC_FRAME_S   stAecFrm;
	AUDIO_STREAM_S stStream;

	printf("[%s][%d][%p]\n", __func__, __LINE__, arg);
	FILE *pfd_ubaenc = fopen("aenc_ubind.g711a", "wb");

	if (!pfd_ubaenc) {
		printf("[%s][%d]fopen error\n", __func__, __LINE__);
		return NULL;
	}

	while (running) {

		s32Ret = CVI_AI_GetFrame(s32DevId, AiChn, &stFrame, &stAecFrm, -1);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_GetStream(%d), failed with %#x!\n",
			 __func__, AeChn, s32Ret);
			return NULL;
		}

		if (stFrame.u32Len == 0)
			printf("ai get len is 0\n");

		s32Ret = CVI_AENC_SendFrame(AeChn, &stFrame, &stAecFrm);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_GetStream(%d), failed with %#x!\n",
			 __func__, AeChn, s32Ret);
			return NULL;
		}

		s32Ret = CVI_AENC_GetStream(AeChn, &stStream, CVI_FALSE);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_GetStream(%d), failed with %#x!\n",
			 __func__, AeChn, s32Ret);
			return NULL;
		}

		if (stStream.u32Len != 0) {
		/* save audio stream to file */
			(CVI_VOID)fwrite(stStream.pStream, 1, stStream.u32Len, pfd_ubaenc);
		}

		s32Ret = CVI_AENC_ReleaseStream(AeChn, &stStream);
		if (s32Ret != CVI_SUCCESS) {
			printf("%s: CVI_AENC_ReleaseStream(%d), failed with %#x!\n",
			       __func__, AeChn, s32Ret);
			return NULL;
		}
	}
	fclose(pfd_ubaenc);
	return 0;
}


static int _slave_process_setting(void)
{

if (mode == AI_BIND_ANEC_TEST) {
	pThread = (pthread_t *)malloc(sizeof(pthread_t));
	printf("ai bind aenc thread start---->\n");
	pthread_create(pThread,
			0,
			(CVI_VOID * (*)(CVI_VOID *))AencGetThreadProc,
			(CVI_VOID *)CVI_NULL);
	pthread_detach(*pThread);
} else if (mode == AI_UNBIND_AENC_TEST) {
	pThread = (pthread_t *)malloc(sizeof(pthread_t));
	printf("ai unbind aenc thread start---->\n");

	pthread_create(pThread,
			0,
			(CVI_VOID * (*)(CVI_VOID *))AencUbindThreadProc,
			(CVI_VOID *)CVI_NULL);
	pthread_detach(*pThread);
} else {
	printf("ai thread start---->\n");
	pThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(pThread,
			0,
			(CVI_VOID * (*)(CVI_VOID *))AiGetThreadProc,
			(CVI_VOID *)CVI_NULL);
	pthread_detach(*pThread);

}
	while (running) {
		sleep(5);
	}

	return CVI_SUCCESS;
}

int main(int argc, char *argv[])
{

	printf("Second process get data\n");
	register_inthandler();
	if (argc != 2 || atoi(argv[1]) >  2) {
		printf("usage:%s [0:ai 1:aibindaenc 2:aiubindaenc]\n", argv[0]);
		return -1;
	}
	mode = atoi(argv[1]);
	_slave_process_setting();
	mode = 0;
	free(pThread);
	printf("Exit program!!! Second Process\n");
	return 0;
}

