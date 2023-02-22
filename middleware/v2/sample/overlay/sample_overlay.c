#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <signal.h>
#include <inttypes.h>

#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"

#include "sample_comm.h"

#define MAX_VDEC_NUM 2
#define VDEC_WIDTH 1920
#define VDEC_HEIGHT 1080
#define VDEC_TYPE PT_H264

typedef struct _SAMPLE_VDEC_PARAM_S {
	VDEC_CHN        VdecChn;
	VDEC_CHN_ATTR_S stChnAttr;
	char            decode_file_name[64];
	CVI_BOOL        stop_thread;
	pthread_t       vdec_thread;
	RECT_S          stDispRect;
} SAMPLE_VDEC_PARAM_S;

typedef struct _SAMPLE_VDEC_CONFIG_S {
	CVI_S32 s32ChnNum;
	SAMPLE_VDEC_PARAM_S astVdecParam[MAX_VDEC_NUM];
} SAMPLE_VDEC_CONFIG_S;

static pthread_t send_vo_thread;
static CVI_VOID *s_h264file1;
static CVI_VOID *s_h264file2;

CVI_VOID *thread_vdec_send_stream(CVI_VOID *arg)
{
	FILE *fpStrm = NULL;
	SAMPLE_VDEC_PARAM_S *param = (SAMPLE_VDEC_PARAM_S *)arg;
	CVI_BOOL bEndOfStream = CVI_FALSE;
	CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
	CVI_U8 *pu8Buf = NULL;
	VDEC_STREAM_S stStream;
	CVI_BOOL bFindStart, bFindEnd;
	CVI_U64 u64PTS = 0;
	CVI_U32 u32Len, u32Start;
	CVI_S32 s32Ret, i;
	CVI_S32 bufsize = (param->stChnAttr.u32PicWidth * param->stChnAttr.u32PicHeight * 3) >> 1;

	fpStrm = fopen(param->decode_file_name, "rb");
	if (fpStrm == NULL) {
		printf("open file err, %s\n", param->decode_file_name);
		return (CVI_VOID *)(CVI_FAILURE);
	}
	pu8Buf = malloc(bufsize);
	if (pu8Buf == NULL) {
		printf("chn %d can't alloc %d in send stream thread!\n",
				param->VdecChn,
				bufsize);
		fclose(fpStrm);
		return (CVI_VOID *)(CVI_FAILURE);
	}
	printf("thread_vdec_send_stream %d\n", param->VdecChn);
	u64PTS = 0;
	while (!param->stop_thread) {
		bEndOfStream = CVI_FALSE;
		bFindStart = CVI_FALSE;
		bFindEnd = CVI_FALSE;
		u32Start = 0;
		s32Ret = fseek(fpStrm, s32UsedBytes, SEEK_SET);
		s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);
		if (s32ReadLen == 0) {//file end
			memset(&stStream, 0, sizeof(VDEC_STREAM_S));
			stStream.bEndOfStream = CVI_TRUE;

			CVI_VDEC_SendStream(param->VdecChn, &stStream, -1);

			s32UsedBytes = 0;
			fseek(fpStrm, 0, SEEK_SET);
			s32ReadLen = fread(pu8Buf, 1, bufsize, fpStrm);

		}
		if (param->stChnAttr.enMode == VIDEO_MODE_FRAME &&
				param->stChnAttr.enType == PT_H264) {
			for (i = 0; i < s32ReadLen - 8; i++) {
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
				    (((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
				     (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
					bFindStart = CVI_TRUE;
					i += 8;
					break;
				}
			}

			for (; i < s32ReadLen - 8; i++) {
				int tmp = pu8Buf[i + 3] & 0x1F;

				if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
				    (tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
				     ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
				     (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80))) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}

			if (i > 0)
				s32ReadLen = i;
			if (bFindStart == CVI_FALSE) {
				CVI_VDEC_TRACE("chn 0 can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					    s32ReadLen, s32UsedBytes);
			}
			if (bFindEnd == CVI_FALSE) {
				s32ReadLen = i + 8;
			}

		} else if (param->stChnAttr.enMode == VIDEO_MODE_FRAME &&
				param->stChnAttr.enType == PT_H265) {
			CVI_BOOL bNewPic = CVI_FALSE;

			for (i = 0; i < s32ReadLen - 6; i++) {
				CVI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;

				bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					   (tmp > 0 && tmp <= 21) && ((pu8Buf[i + 5] & 0x80) == 0x80));

				if (bNewPic) {
					bFindStart = CVI_TRUE;
					i += 6;
					break;
				}
			}

			for (; i < s32ReadLen - 6; i++) {
				CVI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;

				bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					   (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 ||
					    ((tmp > 0 && tmp <= 21) && (pu8Buf[i + 5] & 0x80) == 0x80)));

				if (bNewPic) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}
			if (i > 0)
				s32ReadLen = i;

			if (bFindStart == CVI_FALSE) {
				CVI_VDEC_TRACE("chn 0 can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					    s32ReadLen, s32UsedBytes);
			}
			if (bFindEnd == CVI_FALSE) {
				s32ReadLen = i + 6;
			}

		} else if (param->stChnAttr.enType == PT_MJPEG
				|| param->stChnAttr.enType == PT_JPEG) {
			for (i = 0; i < s32ReadLen - 1; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD8) {
					u32Start = i;
					bFindStart = CVI_TRUE;
					i = i + 2;
					break;
				}
			}

			for (; i < s32ReadLen - 3; i++) {
				if ((pu8Buf[i] == 0xFF) && (pu8Buf[i + 1] & 0xF0) == 0xE0) {
					u32Len = (pu8Buf[i + 2] << 8) + pu8Buf[i + 3];
					i += 1 + u32Len;
				} else {
					break;
				}
			}

			for (; i < s32ReadLen - 1; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i + 1] == 0xD9) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}
			s32ReadLen = i + 2;

			if (bFindStart == CVI_FALSE) {
				CVI_VDEC_TRACE("chn 0 can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					    s32ReadLen, s32UsedBytes);
			}
		} else {
			if ((s32ReadLen != 0) && (s32ReadLen < bufsize)) {
				bEndOfStream = CVI_TRUE;
			}
		}

		stStream.u64PTS = u64PTS;
		stStream.pu8Addr = pu8Buf + u32Start;
		stStream.u32Len = s32ReadLen;
		stStream.bEndOfFrame = CVI_TRUE;
		stStream.bEndOfStream = bEndOfStream;
		stStream.bDisplay = 1;
SendAgain:
		s32Ret = CVI_VDEC_SendStream(param->VdecChn,
					&stStream, -1);
		if (s32Ret != CVI_SUCCESS) {
			//printf("%d dec chn CVI_VDEC_SendStream err ret=%d\n",param->vdec_chn,s32Ret);
			usleep(10000);
			goto SendAgain;
		} else {
			bEndOfStream = CVI_FALSE;
			s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
			u64PTS += 0;
		}
		usleep(10000);

	}
	fclose(fpStrm);
	return NULL;
}

CVI_S32 set_vpss_AspectRatio(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, RECT_S *rect)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VPSS_CHN_ATTR_S stChnAttr;

	s32Ret = CVI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnAttr is fail\n");
		return s32Ret;
	}
	stChnAttr.stAspectRatio.stVideoRect.s32X = rect->s32X;
	stChnAttr.stAspectRatio.stVideoRect.s32Y = rect->s32Y;
	stChnAttr.stAspectRatio.stVideoRect.u32Width = rect->u32Width;
	stChnAttr.stAspectRatio.stVideoRect.u32Height = rect->u32Height;
	stChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_MANUAL;
	stChnAttr.stAspectRatio.bEnableBgColor = CVI_FALSE;
	s32Ret = CVI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VPSS_SetChnAttr grp %d failed with %#x\n", VpssGrp, s32Ret);
	}

	return s32Ret;
}

CVI_VOID *thread_send_vo(CVI_VOID *arg)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_BOOL bFirstFrame = CVI_FALSE;
	SAMPLE_VDEC_CONFIG_S *pstVdecCfg = (SAMPLE_VDEC_CONFIG_S *)arg;
	SAMPLE_VDEC_PARAM_S *pstVdecChn;
	VIDEO_FRAME_INFO_S stViFrame = {0};
	VIDEO_FRAME_INFO_S stVdecFrame = {0};
	VIDEO_FRAME_INFO_S stOverlayFrame = {0};
	VPSS_GRP VpssGrp0 = 0;
	VPSS_GRP VpssGrp1 = 1;
	VPSS_CHN VpssChn = 0;
	VO_LAYER VoLayer = 0;
	VO_CHN VoChn = 0;

	while (!pstVdecCfg->astVdecParam[0].stop_thread) {
		//vi frame
		s32Ret = CVI_VPSS_GetChnFrame(VpssGrp0, VpssChn, &stViFrame, 1000);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_VPSS_GetChnFrame faile, grp:%d\n", VpssGrp0);
			continue;
		}
		s32Ret = CVI_VPSS_SendChnFrame(VpssGrp1, VpssChn, &stViFrame, 1000);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VPSS_ReleaseChnFrame(VpssGrp0, VpssChn, &stViFrame);
			continue;
		}
		CVI_VPSS_ReleaseChnFrame(VpssGrp0, VpssChn, &stViFrame);
		bFirstFrame = CVI_TRUE;

		//vdec frame
		for (CVI_S32 i = 0; i < pstVdecCfg->s32ChnNum; i++) {
			pstVdecChn = &pstVdecCfg->astVdecParam[i];

			if (!bFirstFrame) {
				s32Ret = CVI_VPSS_SendChnFrame(VpssGrp1, VpssChn, &stOverlayFrame, 1000);
				if (s32Ret != CVI_SUCCESS) {
					CVI_VPSS_ReleaseChnFrame(VpssGrp1, VpssChn, &stOverlayFrame);
					continue;
				}
				CVI_VPSS_ReleaseChnFrame(VpssGrp1, VpssChn, &stOverlayFrame);
			}
RETRY_GET_FRAME:
			s32Ret = CVI_VDEC_GetFrame(pstVdecChn->VdecChn, &stVdecFrame, 1000);
			if (s32Ret != CVI_SUCCESS) {
				printf("CVI_VDEC_GetFrame failure\n");
				if (s32Ret == CVI_ERR_VDEC_BUSY) {
					printf("get frame timeout ..in overlay ..retry\n");
					goto RETRY_GET_FRAME;
				}
				continue;
			}

			set_vpss_AspectRatio(VpssGrp1, VpssChn, &pstVdecChn->stDispRect);
			s32Ret = CVI_VPSS_SendFrame(VpssGrp1, &stVdecFrame, 1000);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ReleaseFrame(pstVdecChn->VdecChn, &stVdecFrame);
				continue;
			}

			s32Ret = CVI_VPSS_GetChnFrame(VpssGrp1, VpssChn, &stOverlayFrame, 1000);
			CVI_VDEC_ReleaseFrame(pstVdecChn->VdecChn, &stVdecFrame);
			if (s32Ret != CVI_SUCCESS) {
				printf("CVI_VPSS_GetChnFrame faile, grp:%d\n", VpssGrp1);
				continue;
			}
			bFirstFrame = CVI_FALSE;
		}

		s32Ret = CVI_VO_SendFrame(VoLayer, VoChn, &stOverlayFrame, 1000);
		if (s32Ret != CVI_SUCCESS) {
			printf("CVI_VO_SendFrame faile\n");
		}
		CVI_VPSS_ReleaseChnFrame(VpssGrp1, VpssChn, &stOverlayFrame);
	}
	return NULL;
}

CVI_S32 start_vdec(SAMPLE_VDEC_PARAM_S *pVdecParam)
{
	VDEC_CHN_PARAM_S stChnParam = {0};
	CVI_S32 s32Ret = CVI_SUCCESS;
	VDEC_CHN VdecChn = pVdecParam->VdecChn;

	s32Ret = CVI_VDEC_CreateChn(VdecChn, &pVdecParam->stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_CreateChn chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VDEC_GetChnParam(VdecChn, &stChnParam);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_GetChnParam chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}
	stChnParam.u32DisplayFrameNum = 3;
	s32Ret = CVI_VDEC_SetChnParam(VdecChn, &stChnParam);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_SetChnParam chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VDEC_StartRecvStream(VdecChn);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_StartRecvStream chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}


CVI_VOID stop_vdec(SAMPLE_VDEC_PARAM_S *pVdecParam)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VDEC_StopRecvStream(pVdecParam->VdecChn);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_StopRecvStream chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
	}
	s32Ret = CVI_VDEC_DestroyChn(pVdecParam->VdecChn);
	if (s32Ret != CVI_SUCCESS) {
		printf("CVI_VDEC_DestroyChn chn[%d] failed for %#x!\n", pVdecParam->VdecChn, s32Ret);
	}
}


CVI_S32 start_thread(SAMPLE_VDEC_CONFIG_S *pstVdecCfg)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	for (int i = 0; i < pstVdecCfg->s32ChnNum; i++) {
		s32Ret = pthread_create(&pstVdecCfg->astVdecParam[i].vdec_thread, NULL,
			thread_vdec_send_stream, (void *)&pstVdecCfg->astVdecParam[i]);
		if (s32Ret != 0) {
			return CVI_FAILURE;
		}
	}
	sleep(1);
	s32Ret = pthread_create(&send_vo_thread, NULL, thread_send_vo, (void *)pstVdecCfg);
	if (s32Ret != 0) {
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_VOID stop_thread(SAMPLE_VDEC_CONFIG_S *pstVdecCfg)
{
	pstVdecCfg->astVdecParam[0].stop_thread = CVI_TRUE;
	if (send_vo_thread != 0)
		pthread_join(send_vo_thread, NULL);

	for (int i = 0; i < pstVdecCfg->s32ChnNum; i++) {
		pstVdecCfg->astVdecParam[i].stop_thread = CVI_TRUE;
		if (pstVdecCfg->astVdecParam[i].vdec_thread != 0)
			pthread_join(pstVdecCfg->astVdecParam[i].vdec_thread, NULL);
	}

}

CVI_S32 SAMPLE_VPSS_Overlay_vi_2vdec(CVI_VOID)
{
	SAMPLE_SNS_TYPE_E  enSnsType        = SENSOR0_TYPE;
	WDR_MODE_E         enWDRMode        = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat      = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E     enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	       u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 3;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					    , enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 15;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Height, stSize.u32Width, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					    , enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 5;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	u32BlkSize = VDEC_GetPicBufferSize(VDEC_TYPE, VDEC_WIDTH, VDEC_HEIGHT,
				SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE);
	stVbConf.astCommPool[2].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[2].u32BlkCnt	= 3;
	SAMPLE_PRT("common pool[2] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stPipeAttr.bYuvSkip = CVI_FALSE;
	stPipeAttr.u32MaxW = stSize.u32Width;
	stPipeAttr.u32MaxH = stSize.u32Height;
	stPipeAttr.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
	stPipeAttr.enBitWidth = DATA_BITWIDTH_12;
	stPipeAttr.stFrameRate.s32SrcFrameRate = -1;
	stPipeAttr.stFrameRate.s32DstFrameRate = -1;
	stPipeAttr.bNrEn = CVI_TRUE;
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp0	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp0, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp0, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/*overlay vpss grp*/
	VPSS_GRP	VpssGrp1	= 1;

	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp1, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp1, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev		 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode		 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);

	SAMPLE_VDEC_CONFIG_S stVdecCfg;
	SAMPLE_VDEC_PARAM_S *pVdecChn0 = &stVdecCfg.astVdecParam[0];
	SAMPLE_VDEC_PARAM_S *pVdecChn1 = &stVdecCfg.astVdecParam[1];

	stVdecCfg.s32ChnNum = 2;

	pVdecChn0->VdecChn = 0;
	pVdecChn0->stop_thread = CVI_FALSE;
	snprintf(pVdecChn0->decode_file_name, 63, "%s", (char *)s_h264file1);
	pVdecChn0->stChnAttr.enType = VDEC_TYPE;
	pVdecChn0->stChnAttr.enMode = VIDEO_MODE_FRAME;
	pVdecChn0->stChnAttr.u32PicWidth = VDEC_WIDTH;
	pVdecChn0->stChnAttr.u32PicHeight = VDEC_HEIGHT;
	pVdecChn0->stChnAttr.u32FrameBufCnt = 3;
	pVdecChn0->stChnAttr.u32StreamBufSize = 1920*1080;
	pVdecChn0->stDispRect.s32X = 0;
	pVdecChn0->stDispRect.s32Y = 0;
	pVdecChn0->stDispRect.u32Width = 320;
	pVdecChn0->stDispRect.u32Height = 180;
	start_vdec(pVdecChn0);
	printf("start vdec 0\n");

	memcpy(pVdecChn1, pVdecChn0, sizeof(SAMPLE_VDEC_PARAM_S));
	pVdecChn1->VdecChn = 1;
	snprintf(pVdecChn1->decode_file_name, 63, "%s", (char *)s_h264file2);
	pVdecChn1->stDispRect.s32X = 960;
	pVdecChn1->stDispRect.s32Y = 0;
	pVdecChn1->stDispRect.u32Width = 320;
	pVdecChn1->stDispRect.u32Height = 180;
	start_vdec(pVdecChn1);
	printf("start vdec 1\n");

	start_thread(&stVdecCfg);

	PAUSE();

	stop_thread(&stVdecCfg);
	printf("stop thread\n");
	stop_vdec(pVdecChn1);
	stop_vdec(pVdecChn0);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp0);

	SAMPLE_COMM_VPSS_Stop(VpssGrp0, abChnEnable);
	SAMPLE_COMM_VPSS_Stop(VpssGrp1, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_VOID SAMPLE_VIO_HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo) {
		//todo for release
		SAMPLE_PRT("Program termination abnormally\n");
	}
	exit(-1);
}

CVI_VOID SAMPLE_VIO_Usage(CVI_CHAR *sPrgNm)
{
	printf("Usage : %s <index> <h264file1> <h264file2>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)vi + 2vdec, Multi-channel display.\n");
}

int main(int argc, char *argv[])
{
	CVI_S32 s32Ret = CVI_FAILURE;
	CVI_S32 s32Index;

	if (argc < 4) {
		SAMPLE_VIO_Usage(argv[0]);
		return CVI_FAILURE;
	}
	s32Index = atoi(argv[1]);
	s_h264file1 = argv[2];
	s_h264file2 = argv[3];

	signal(SIGINT, SAMPLE_VIO_HandleSig);
	signal(SIGTERM, SAMPLE_VIO_HandleSig);

	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_VPSS_Overlay_vi_2vdec();
		break;

	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_VIO_Usage(argv[0]);
		return CVI_FAILURE;
	}

	if (s32Ret == CVI_SUCCESS)
		SAMPLE_PRT("sample_vio exit success!\n");
	else
		SAMPLE_PRT("sample_vio exit abnormally!\n");

	return s32Ret;
}
