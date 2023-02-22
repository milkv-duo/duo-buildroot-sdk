/*
 * Copyright (C) Cvitek Co., Ltd. All rights reserved.
 *
 * File Name: cvi_rpc_dnslave.c
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
#include <sys/time.h>
#include <signal.h>
#include "cvi_audio.h"
#include "cvi_type.h"


pthread_t PlayThread;
pthread_t AdecUbindThread;
pthread_t AdecbindThread;

CVI_S32 AoDev;
CVI_S32 AoChn;
CVI_S32 running = 1;
CVI_S32 AdChn;
CVI_S32 mode;
#define ADEC_BIND_AO_TEST 1
#define ADEC_UBIND_AO_TEST 2

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

void *play_audio_file(void *parg)
{
	AIO_ATTR_S *pstAioAttr = (AIO_ATTR_S *)parg;
	CVI_S32 num_readbytes;
	AUDIO_FRAME_S stFrameSend;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32FrameBytes = (pstAioAttr->enSoundmode + 1) * pstAioAttr->u32PtNumPerFrm * 2;
	char *pBuffer = malloc(s32FrameBytes);

	if (!pBuffer || access("record0.raw", 0) < 0) {
		printf("[Error]pBuffer[%p]malloc error/record0.raw file not exist\n", pBuffer);
		return NULL;
	}
	FILE *fp_playfile = fopen("record0.raw", "rb");

	if (!fp_playfile) {
		printf("[error]record0 fopen error\n");
		return NULL;
	}
	memset(pBuffer, 0, s32FrameBytes);
	while (1) {
		num_readbytes = fread(pBuffer, 1, s32FrameBytes, fp_playfile);
		if (num_readbytes > 0) {
			stFrameSend.u64VirAddr[0] = (CVI_U8 *)pBuffer;
			stFrameSend.u32Len = pstAioAttr->u32PtNumPerFrm;//samples size for each channel
			stFrameSend.u64TimeStamp = 0;
			stFrameSend.enSoundmode = pstAioAttr->enSoundmode;
			stFrameSend.enBitwidth = AUDIO_BIT_WIDTH_16;
			s32Ret = CVI_AO_SendFrame(AoDev, AoChn, (const AUDIO_FRAME_S *)&stFrameSend, 1000);
			if (s32Ret != CVI_SUCCESS)
				printf("CVI_AO_SendFrame failed with chn_id[%d]%#x!\n", AoChn, s32Ret);
		} else {
			printf("num_framebytes %d.\n", num_readbytes);
			(CVI_VOID)fseek(fp_playfile, 0, SEEK_SET);/*read file again*/
			printf("audio replay.....loopback file\n");
		}
	}

	if (!pBuffer)
		free(pBuffer);
	if (!fp_playfile)
		fclose(fp_playfile);
	printf("play 0 file thread finished!!!\n");
}

void *adec_unbind_ao(void *parg)
{
	AIO_ATTR_S *pstAioAttr = (AIO_ATTR_S *)parg;
	CVI_S32 s32Ret = -1;
	CVI_S32 num_readbytes = 0;
	AUDIO_STREAM_S stAudioStream;
	AUDIO_FRAME_S stFrame;
	AUDIO_FRAME_INFO_S sDecOutFrm;

	sDecOutFrm.pstFrame = (AUDIO_FRAME_S *)&stFrame;
	CVI_S32 length_bytes = pstAioAttr->u32PtNumPerFrm * (pstAioAttr->enSoundmode + 1) * 2;
	char *pBuffer = malloc(length_bytes);

	if (!pBuffer || access("adec.g711a", 0) < 0) {
		printf("[Error]pBuffer[%p %d]malloc error/adec.g711a file not exist\n",
				pBuffer, length_bytes);
		return NULL;
	}

	memset(pBuffer, 0, length_bytes);
	FILE *fpAdec = fopen("adec.g711a", "rb+");

	if (!fpAdec) {
		printf("fopen error,fuc:%s,line:%d\n", __func__, __LINE__);
		return NULL;
	}

	while (running) {
		stAudioStream.pStream = (CVI_U8 *)pBuffer;
		num_readbytes = fread(stAudioStream.pStream, 1, length_bytes, fpAdec);
		if (num_readbytes <= 0) {
			s32Ret = CVI_ADEC_SendEndOfStream(AdChn, CVI_FALSE);
			if (s32Ret != CVI_SUCCESS)
				printf("[error] CVI_ADEC_SendEndOfStream failed!\n");

			printf("num_framebytes %d.\n", num_readbytes);
			(CVI_VOID)fseek(fpAdec, 0, SEEK_SET);/*read file again*/
			printf("audio replay.....loopback file\n");
		} else {
			stAudioStream.u32Len = num_readbytes; //640 bytes
			s32Ret = CVI_ADEC_SendStream(AdChn, &stAudioStream, CVI_TRUE);
			if (s32Ret != CVI_SUCCESS) {
				printf("[error] adec send stream failed Len[%d]ret[%#x]!\n",
					stAudioStream.u32Len, s32Ret);
				break;
			}

			s32Ret = CVI_ADEC_GetFrame(AdChn, &sDecOutFrm, CVI_TRUE);
			if (s32Ret != CVI_SUCCESS) {
				printf("[cvi_error] adec get stream failed Len[%d]ret[%#x]!\n",
					stAudioStream.u32Len, s32Ret);
				break;
			}

			if (sDecOutFrm.pstFrame->u32Len != 0) {
				s32Ret = CVI_AO_SendFrame(AoDev, AoChn, &stFrame, 5000);
				if (s32Ret != CVI_SUCCESS) {
					printf("%s: AoDev(%d), failed with %#x!\n", __func__, AoDev, s32Ret);
					break;
				}
			} else
				printf("[cvi_error] dec out frame size 0\n");
		}


	}

	if (fpAdec)
		fclose(fpAdec);
	if (pBuffer)
		free(pBuffer);
	return NULL;
}

void *adec_bind_ao(void *parg)
{
	AIO_ATTR_S *pstAioAttr = (AIO_ATTR_S *)parg;
	CVI_S32 s32Ret = -1;
	CVI_S32 num_readbytes = 0;
	AUDIO_STREAM_S stAudioStream;

	CVI_S32 length_bytes = pstAioAttr->u32PtNumPerFrm * (pstAioAttr->enSoundmode + 1) * 2;
	char *pBuffer = malloc(length_bytes);

	if (!pBuffer || access("adec.g711a", 0) < 0) {
		printf("[Error]pBuffer[%p %d]malloc error/adec.g711a file not exist\n",
				pBuffer, length_bytes);
		return NULL;
	}
	memset(pBuffer, 0, length_bytes);
	FILE *fpAdec = fopen("adec.g711a", "rb+");

	if (!fpAdec) {
		printf("fopen error,fuc:%s,line:%d\n", __func__, __LINE__);
		return NULL;
	}

	while (running) {
		stAudioStream.pStream = (CVI_U8 *)pBuffer;
		num_readbytes = fread(stAudioStream.pStream, 1, length_bytes, fpAdec);
		if (num_readbytes <= 0) {
			s32Ret = CVI_ADEC_SendEndOfStream(AdChn, CVI_FALSE);
			if (s32Ret != CVI_SUCCESS)
				printf("[error] CVI_ADEC_SendEndOfStream failed!\n");

			printf("num_framebytes %d.\n", num_readbytes);
			(CVI_VOID)fseek(fpAdec, 0, SEEK_SET);/*read file again*/
			printf("audio replay.....loopback file\n");
		} else {
			stAudioStream.u32Len = num_readbytes; //640 bytes
			s32Ret = CVI_ADEC_SendStream(AdChn, &stAudioStream, CVI_TRUE);
			if (s32Ret != CVI_SUCCESS) {
				printf("[error] adec send stream failed Len[%d]ret[%#x]!\n",
					stAudioStream.u32Len, s32Ret);
				break;
			}
		}


	}
	if (fpAdec)
		fclose(fpAdec);
	if (pBuffer)
		free(pBuffer);
	return NULL;
}

static int _slave_process_setting(void)
{
	AIO_ATTR_S AudoutAttr;
	int sample_rate = 8000;
	int channels = 2;

	AudoutAttr.u32ChnCnt = 3;//if you want create n channel,you must set it to >= n
	AudoutAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E)sample_rate;
	AudoutAttr.enSoundmode = (channels == 2 ? AUDIO_SOUND_MODE_STEREO :
				 AUDIO_SOUND_MODE_MONO);
	AudoutAttr.enWorkmode	  = AIO_MODE_I2S_MASTER;
	AudoutAttr.u32EXFlag	  = 0;
	AudoutAttr.u32FrmNum	  = 10; /* only use in bind mode */
	AudoutAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	AudoutAttr.u32PtNumPerFrm = 320;
	AudoutAttr.u32ClkSel	  = 0;
	AudoutAttr.enI2sType = AIO_I2STYPE_INNERCODEC;

	if (mode == ADEC_BIND_AO_TEST) {
		pthread_create(&AdecbindThread, 0, adec_bind_ao, &AudoutAttr);
		pthread_detach(AdecbindThread);
		printf("adec_bind_ao thread....!!!\n");
	} else if (mode == ADEC_UBIND_AO_TEST) {
		pthread_create(&AdecUbindThread, 0, adec_unbind_ao, &AudoutAttr);
		pthread_detach(AdecUbindThread);
		printf("adec_unbind_ao thread....!!!\n");
	} else {
		pthread_create(&PlayThread, 0, play_audio_file, &AudoutAttr);
		pthread_detach(PlayThread);
		printf("play_audio_file thread....!!!\n");
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
		printf("usage:%s [0:ao 1:aobindadec 2:aoubindadec]\n", argv[0]);
		return -1;
	}
	mode = atoi(argv[1]);
	_slave_process_setting();
	mode = 0;
	printf("Exit program!!! Second Process\n");
	return 0;
}

