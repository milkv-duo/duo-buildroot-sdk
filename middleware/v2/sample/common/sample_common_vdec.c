#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <inttypes.h>
#include "sample_comm.h"

VB_SOURCE_E g_enVdecVBSource = VB_SOURCE_MODULE;

VB_POOL g_ahPicVbPool[VDEC_MAX_CHN_NUM] = { [0 ...(VDEC_MAX_CHN_NUM - 1)] = VB_INVALID_POOLID };
VB_POOL g_ahTmvVbPool[VDEC_MAX_CHN_NUM] = { [0 ...(VDEC_MAX_CHN_NUM - 1)] = VB_INVALID_POOLID };

static inline CVI_VOID PRINTF_VDEC_CHN_STATUS(CVI_S32 Chn, VDEC_CHN_STATUS_S stStatus)
{
	CVI_VDEC_PRNT("\033[0;33m ---------------------------------------");
	CVI_VDEC_PRNT("------------------------------------------------------------\033[0;39m\n");
	CVI_VDEC_PRNT("\033[0;33m chn:%d, Type:%d, bStart:%d, DecodeFrames:%d, LeftPics:%d,",
		Chn, stStatus.enType, stStatus.bStartRecvStream, stStatus.u32DecodeStreamFrames, stStatus.u32LeftPics);
	CVI_VDEC_PRNT("LeftBytes:%d, LeftFrames:%d, RecvFrames:%d  \033[0;39m\n",
		stStatus.u32LeftStreamBytes, stStatus.u32LeftStreamFrames, stStatus.u32RecvStreamFrames);
	CVI_VDEC_PRNT("\033[0;33m FormatErr:%d,	s32PicSizeErrSet:%d,  s32StreamUnsprt:%d,",
		stStatus.stVdecDecErr.s32FormatErr, stStatus.stVdecDecErr.s32PicSizeErrSet,
		stStatus.stVdecDecErr.s32StreamUnsprt);
	CVI_VDEC_PRNT("s32PackErr:%d,  u32PrtclNumErrSet:%d,  s32RefErrSet:%d,  s32PicBufSizeErrSet:%d  \033[0;39m\n",
		stStatus.stVdecDecErr.s32PackErr, stStatus.stVdecDecErr.s32PrtclNumErrSet,
		stStatus.stVdecDecErr.s32RefErrSet, stStatus.stVdecDecErr.s32PicBufSizeErrSet);
	CVI_VDEC_PRNT("\033[0;33m ----------------------------------------");
	CVI_VDEC_PRNT("-----------------------------------------------------------\033[0;39m\n");
}


static inline CVI_S32 SAVE_FILE_NAME(CVI_CHAR *aFileName, CVI_S32 chn, CVI_CHAR *cStreamName,
				      PIXEL_FORMAT_E enPixelFormat)
{
	CVI_CHAR *Postfix;

	if (enPixelFormat == PIXEL_FORMAT_RGB_888) {
		Postfix = "rgb888";
	} else if (enPixelFormat == PIXEL_FORMAT_BGR_888) {
		Postfix = "bgr888";
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Postfix = "argb8888";
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
		Postfix = "argb1555";
	} else if (enPixelFormat == PIXEL_FORMAT_YUV_400 ||
		   enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420 ||
		   enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422 ||
		   enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_444 ||
		   enPixelFormat == PIXEL_FORMAT_NV12 ||
		   enPixelFormat == PIXEL_FORMAT_NV21) {
		Postfix = "yuv";
	} else {
		CVI_VDEC_ERR("[%s]-%d: enPixelFormat type err", __func__, __LINE__);
		Postfix = "unk";
	}
	CVI_VDEC_INFO("aFileName = %s, cStreamName = %s\n", aFileName, cStreamName);
	sprintf(aFileName, "chn%d_%s.%s", chn, cStreamName, Postfix);
	CVI_VDEC_INFO("aFileName = %s\n", aFileName);

	return 0;
}

static void get_chroma_size_shift_factor(PIXEL_FORMAT_E enPixelFormat, CVI_S32 *w_shift, CVI_S32 *h_shift)
{
	switch (enPixelFormat) {
	case PIXEL_FORMAT_YUV_PLANAR_420:
		*w_shift = 1;
		*h_shift = 1;
		break;
	case PIXEL_FORMAT_YUV_PLANAR_422:
		*w_shift = 1;
		*h_shift = 0;
		break;
	case PIXEL_FORMAT_YUV_PLANAR_444:
		*w_shift = 0;
		*h_shift = 0;
		break;
	case PIXEL_FORMAT_NV12:
	case PIXEL_FORMAT_NV21:
		*w_shift = 0;
		*h_shift = 1;
		break;
	case PIXEL_FORMAT_YUV_400: // no chroma
	default:
		*w_shift = 31;
		*h_shift = 31;
		break;
	}
}

static int write_yuv(FILE *out_f, VIDEO_FRAME_S stVFrame)
{
	CVI_S32 c_w_shift, c_h_shift; // chroma width/height shift
	CVI_U8 *w_ptr;

	CVI_VDEC_INFO("u32Width = %d, u32Height = %d\n",
			stVFrame.u32Width, stVFrame.u32Height);
	CVI_VDEC_INFO("u32Stride[0] = %d, u32Stride[1] = %d, u32Stride[2] = %d\n",
			stVFrame.u32Stride[0], stVFrame.u32Stride[1], stVFrame.u32Stride[2]);
	CVI_VDEC_INFO("u32Length[0] = %d, u32Length[1] = %d, u32Length[2] = %d\n",
			stVFrame.u32Length[0], stVFrame.u32Length[1], stVFrame.u32Length[2]);

	get_chroma_size_shift_factor(stVFrame.enPixelFormat, &c_w_shift, &c_h_shift);

	w_ptr = stVFrame.pu8VirAddr[0];
	for (CVI_U32 i = 0; i < stVFrame.u32Height; i++) {
		fwrite(w_ptr + i * stVFrame.u32Stride[0], 1, stVFrame.u32Width, out_f);
	}

	if (stVFrame.pu8VirAddr[1]) {
		w_ptr = stVFrame.pu8VirAddr[1];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++) {
			fwrite(w_ptr + i * stVFrame.u32Stride[1], 1, stVFrame.u32Width >> c_w_shift, out_f);
		}
	}

	if (stVFrame.pu8VirAddr[2]) {
		w_ptr = stVFrame.pu8VirAddr[2];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++) {
			fwrite(w_ptr + i * stVFrame.u32Stride[2], 1, stVFrame.u32Width >> c_w_shift, out_f);
		}
	}

	return 0;
}

static int md5Sum_update(MD5_CTX *ptMD5Ctx, VIDEO_FRAME_S stVFrame)
{
	CVI_S32 c_w_shift, c_h_shift; // chroma width/height shift
	CVI_U8 *w_ptr;

	CVI_VDEC_INFO("u32Width = %d, u32Height = %d\n",
			stVFrame.u32Width, stVFrame.u32Height);
	CVI_VDEC_INFO("u32Stride[0] = %d, u32Stride[1] = %d, u32Stride[2] = %d\n",
			stVFrame.u32Stride[0], stVFrame.u32Stride[1], stVFrame.u32Stride[2]);
	CVI_VDEC_INFO("u32Length[0] = %d, u32Length[1] = %d, u32Length[2] = %d\n",
			stVFrame.u32Length[0], stVFrame.u32Length[1], stVFrame.u32Length[2]);

	get_chroma_size_shift_factor(stVFrame.enPixelFormat, &c_w_shift, &c_h_shift);

	w_ptr = stVFrame.pu8VirAddr[0];
	for (CVI_U32 i = 0; i < stVFrame.u32Height; i++) {
		MD5_Update(ptMD5Ctx, w_ptr + i * stVFrame.u32Stride[0], stVFrame.u32Width);
	}
	usleep(1000);
	if (stVFrame.pu8VirAddr[1]) {
		w_ptr = stVFrame.pu8VirAddr[1];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++) {
			MD5_Update(ptMD5Ctx, w_ptr + i * stVFrame.u32Stride[1], stVFrame.u32Width >> c_w_shift);
		}
	}
	usleep(1000);
	if (stVFrame.pu8VirAddr[2]) {
		w_ptr = stVFrame.pu8VirAddr[2];
		for (CVI_U32 i = 0; i < (stVFrame.u32Height >> c_h_shift); i++) {
			MD5_Update(ptMD5Ctx, w_ptr + i * stVFrame.u32Stride[2], stVFrame.u32Width >> c_w_shift);
		}
	}
	usleep(1000);
	return 0;
}

//VB_SOURCE_E g_VdecVbSrc = VB_SOURCE_PRIVATE;
//VB_SOURCE_E g_VdecVbSrc = VB_SOURCE_USER;
VB_SOURCE_E g_VdecVbSrc = VB_SOURCE_COMMON;

CVI_S32 SAMPLE_COMM_VDEC_SetVBPool(CVI_S32 Chn, CVI_U32 VBPoolID)
{
	if (Chn >= VDEC_MAX_CHN_NUM) {
		CVI_VDEC_ERR("SAMPLE_COMM_VDEC_SetVBPool Fail\n");
		return CVI_FAILURE;
	}
	g_ahPicVbPool[Chn] = VBPoolID;
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VDEC_GetVBPool(CVI_S32 Chn)
{
	if (Chn >= VDEC_MAX_CHN_NUM) {
		CVI_VDEC_ERR("SAMPLE_COMM_VDEC_GetVBPool Fail\n");
		return CVI_FAILURE;
	}
	return g_ahPicVbPool[Chn];
}

CVI_S32 SAMPLE_COMM_VDEC_InitVBPool(VDEC_CHN ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	SAMPLE_VDEC_BUF astSampleVdecBuf[VDEC_MAX_CHN_NUM];
	VB_POOL_CONFIG_S stVbPoolCfg;

	if (g_VdecVbSrc != VB_SOURCE_USER)
		return s32Ret;

	for (i = 0; i < ChnNum; i++) {
		if (pastSampleVdec[i].enType == PT_JPEG ||
			pastSampleVdec[i].enType == PT_MJPEG) {
			astSampleVdecBuf[i].u32PicBufSize = 0;
		} else {
			astSampleVdecBuf[i].u32PicBufSize =
				VDEC_GetPicBufferSize(pastSampleVdec[i].enType,
						pastSampleVdec[i].u32Width,
						pastSampleVdec[i].u32Height,
						pastSampleVdec[i].enPixelFormat,
						DATA_BITWIDTH_8,
						COMPRESS_MODE_NONE);
		}
	}

	for (i = 0; i < ChnNum; i++) {
		if ((astSampleVdecBuf[i].u32PicBufSize != 0) && (pastSampleVdec[i].u32FrameBufCnt != 0)) {
			memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
			stVbPoolCfg.u32BlkSize	= astSampleVdecBuf[i].u32PicBufSize;
			stVbPoolCfg.u32BlkCnt	= pastSampleVdec[i].u32FrameBufCnt;
			stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;

			g_ahPicVbPool[i] = CVI_VB_CreatePool(&stVbPoolCfg);
			CVI_VDEC_TRACE("CVI_VB_CreatePool : %d, u32BlkSize=0x%x, u32BlkCnt=%d\n",
				g_ahPicVbPool[i], stVbPoolCfg.u32BlkSize, stVbPoolCfg.u32BlkCnt);

			if (g_ahPicVbPool[i] == VB_INVALID_POOLID) {
				CVI_VDEC_ERR("CVI_VB_CreatePool Fail\n");
				return CVI_FAILURE;
			}
		}
	}

	return s32Ret;
}

CVI_VOID SAMPLE_COMM_VDEC_ExitVBPool(CVI_VOID)
{
	CVI_S32 i, m, s32Ret;

	if (g_VdecVbSrc != VB_SOURCE_USER)
		return;

	for (i = 0; i < VDEC_MAX_CHN_NUM; i++) {
		if (g_ahPicVbPool[i] != VB_INVALID_POOLID) {
			CVI_VDEC_TRACE("g_ahPicVbPool[Chn%d]CVI_VB_DestroyPool : %d\n", i, g_ahPicVbPool[i]);

			s32Ret =  CVI_VB_DestroyPool(g_ahPicVbPool[i]);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("g_ahPicVbPool[Chn%d]CVI_VB_DestroyPool : %d fail!\n",
					i, g_ahPicVbPool[i]);
			}
			for (m = i + 1; m < VDEC_MAX_CHN_NUM; m++)
				if (g_ahPicVbPool[m] == g_ahPicVbPool[i])
					g_ahPicVbPool[m] = VB_INVALID_POOLID;
			g_ahPicVbPool[i] = VB_INVALID_POOLID;
		}

		if (g_ahTmvVbPool[i] != VB_INVALID_POOLID) {
			CVI_VDEC_TRACE("g_ahTmvVbPool[Chn%d]CVI_VB_DestroyPool : %d\n", i, g_ahTmvVbPool[i]);

			s32Ret =  CVI_VB_DestroyPool(g_ahTmvVbPool[i]);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("g_ahTmvVbPool[Chn%d]CVI_VB_DestroyPool : %d fail!\n",
					i, g_ahTmvVbPool[i]);
			}
			for (m = i + 1; m < VDEC_MAX_CHN_NUM; m++)
				if (g_ahTmvVbPool[m] == g_ahTmvVbPool[i])
					g_ahTmvVbPool[m] = VB_INVALID_POOLID;
			g_ahTmvVbPool[i] = VB_INVALID_POOLID;
		}
	}
}

CVI_VOID *SAMPLE_COMM_VDEC_SendStream(CVI_VOID *pArgs)
{
	VDEC_THREAD_PARAM_S *pstVdecThreadParam = (VDEC_THREAD_PARAM_S *)pArgs;
	CVI_BOOL bEndOfStream = CVI_FALSE;
	CVI_S32 s32UsedBytes = 0, s32ReadLen = 0;
	FILE *fpStrm = NULL;
	CVI_U8 *pu8Buf = NULL;
	VDEC_STREAM_S stStream;
	CVI_BOOL bFindStart, bFindEnd;
	CVI_U64 u64PTS = 0;
	CVI_U32 u32Len, u32Start;
	CVI_S32 s32Ret, i;
	CVI_CHAR cStreamFile[256];

	CVI_VDEC_TRACE("\n");

	prctl(PR_SET_NAME, "VideoSendStream", 0, 0, 0);

	snprintf(cStreamFile, sizeof(cStreamFile), "%s", pstVdecThreadParam->cFileName);
	if (cStreamFile != 0) {
		fpStrm = fopen(cStreamFile, "rb");
		CVI_VDEC_INFO("cStreamFile = %s\n", cStreamFile);

		if (fpStrm == NULL) {
			CVI_VDEC_ERR("chn %d can't open file %s in send stream thread!\n",
					pstVdecThreadParam->s32ChnId, cStreamFile);
			pstVdecThreadParam->bFileEnd = CVI_TRUE;
			return (CVI_VOID *)(CVI_FAILURE);
		}
	}

	CVI_VDEC_TRACE("\n \033[0;36m chn %d, stream file:%s, userbufsize: %d \033[0;39m\n",
			pstVdecThreadParam->s32ChnId,
			pstVdecThreadParam->cFileName,
			pstVdecThreadParam->s32MinBufSize);

	pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
	if (pu8Buf == NULL) {
		CVI_VDEC_ERR("chn %d can't alloc %d in send stream thread!\n",
				pstVdecThreadParam->s32ChnId,
				pstVdecThreadParam->s32MinBufSize);
		fclose(fpStrm);
		pstVdecThreadParam->bFileEnd = CVI_TRUE;
		return (CVI_VOID *)(CVI_FAILURE);
	}
	fflush(stdout);

	u64PTS = pstVdecThreadParam->u64PtsInit;
	while (1) {
		if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP) {
			CVI_VDEC_TRACE("eThreadCtrl = THREAD_CTRL_STOP\n");
			break;
		} else if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_PAUSE) {
			sleep(1);
			continue;
		}

		bEndOfStream = CVI_FALSE;
		bFindStart = CVI_FALSE;
		bFindEnd = CVI_FALSE;
		u32Start = 0;
		s32Ret = fseek(fpStrm, s32UsedBytes, SEEK_SET);
		s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
		if (s32ReadLen == 0) {
			if (pstVdecThreadParam->bCircleSend == CVI_TRUE) {
				memset(&stStream, 0, sizeof(VDEC_STREAM_S));
				stStream.bEndOfStream = CVI_TRUE;
SendAgainFlag:
				s32Ret = CVI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);
				if (s32Ret == CVI_ERR_VDEC_BUSY) {
					CVI_VDEC_TRACE("timeout in vdec sendstream\n");
					goto SendAgainFlag;
				}

				s32UsedBytes = 0;
				fseek(fpStrm, 0, SEEK_SET);
				s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
			} else {
				CVI_VDEC_TRACE("break\n");
				break;
			}
		}

		if (pstVdecThreadParam->s32StreamMode == VIDEO_MODE_FRAME &&
				pstVdecThreadParam->enType == PT_H264) {
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
				CVI_VDEC_TRACE("chn %d can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					   pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
			}
			if (bFindEnd == CVI_FALSE) {
				s32ReadLen = i + 8;
			}

		} else if (pstVdecThreadParam->s32StreamMode == VIDEO_MODE_FRAME &&
				pstVdecThreadParam->enType == PT_H265) {
			CVI_BOOL bNewPic = CVI_FALSE;

			for (i = 0; i < s32ReadLen - 6; i++) {
				CVI_U32 tmp = (pu8Buf[i + 3] & 0x7E) >> 1;

				bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
					   (tmp <= 21) && ((pu8Buf[i + 5] & 0x80) == 0x80));

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
					    ((tmp <= 21) && (pu8Buf[i + 5] & 0x80) == 0x80)));

				if (bNewPic) {
					bFindEnd = CVI_TRUE;
					break;
				}
			}
			if (i > 0)
				s32ReadLen = i;

			if (bFindStart == CVI_FALSE) {
				CVI_VDEC_TRACE("chn %d can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					   pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
			}
			if (bFindEnd == CVI_FALSE) {
				s32ReadLen = i + 6;
			}

		} else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG) {
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
				CVI_VDEC_TRACE("chn %d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
					   pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
			}
		} else {
			if ((s32ReadLen != 0) && (s32ReadLen < pstVdecThreadParam->s32MinBufSize)) {
				bEndOfStream = CVI_TRUE;
			}
		}

		stStream.u64PTS = u64PTS;
		stStream.pu8Addr = pu8Buf + u32Start;
		stStream.u32Len = s32ReadLen;
		stStream.bEndOfFrame = (pstVdecThreadParam->s32StreamMode == VIDEO_MODE_FRAME) ? CVI_TRUE : CVI_FALSE;
		stStream.bEndOfStream = bEndOfStream;
		stStream.bDisplay = 1;

SendAgain:
		s32Ret = CVI_VDEC_SendStream(pstVdecThreadParam->s32ChnId,
				&stStream, pstVdecThreadParam->s32MilliSec);
		if ((s32Ret != CVI_SUCCESS) &&
			(pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_START)) {
			usleep(pstVdecThreadParam->s32IntervalTime);

			if (s32Ret == CVI_ERR_VDEC_BUSY)
				CVI_VDEC_TRACE("timeout in vdec sendstream\n");

			goto SendAgain;
		} else {
			bEndOfStream = CVI_FALSE;
			s32UsedBytes = s32UsedBytes + s32ReadLen + u32Start;
			u64PTS += pstVdecThreadParam->u64PtsIncrease;
		}
		usleep(pstVdecThreadParam->s32IntervalTime);
	}

	/* send the flag of stream end */
	memset(&stStream, 0, sizeof(VDEC_STREAM_S));
	stStream.bEndOfStream = CVI_TRUE;
	CVI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

	CVI_VDEC_TRACE("\033[0;35m chn %d send steam thread return ...  \033[0;39m\n",
			pstVdecThreadParam->s32ChnId);

	pstVdecThreadParam->bFileEnd = CVI_TRUE;
	CVI_VDEC_TRACE("File end in chn[%d]\n", pstVdecThreadParam->s32ChnId);

	fflush(stdout);
	if (pu8Buf != CVI_NULL) {
		free(pu8Buf);
	}
	fclose(fpStrm);

	return (CVI_VOID *)CVI_SUCCESS;
}

CVI_VOID SAMPLE_COMM_VDEC_CmdCtrl(VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
	CVI_S32 s32Ret;
	VDEC_CHN_STATUS_S stStatus;
	char c = 0;

	if (pstVdecSend->bCircleSend == CVI_TRUE) {
		goto WHILE;
	}

	CVI_VDEC_TRACE("decoding..............\n");

	if (*pVdecThread != 0) {
		s32Ret = pthread_join(*pVdecThread, CVI_NULL);
		if (s32Ret == 0) {
			*pVdecThread = 0;
		}
	}
	*pVdecThread = 0;

	while (1) {
		s32Ret = CVI_VDEC_QueryStatus(pstVdecSend->s32ChnId, &stStatus);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_WARN("chn %d CVI_MPI_VDEC_QueryStatus fail!!!\n", s32Ret);
			return;
		}
		if ((stStatus.u32LeftStreamBytes == 0) && (stStatus.u32LeftPics == 0) &&
			(pstVdecSend->bFileEnd == CVI_TRUE)) {
			PRINTF_VDEC_CHN_STATUS(pstVdecSend->s32ChnId, stStatus);
			break;
		}
		usleep(1000);
	}

	CVI_VDEC_TRACE("end!\n");
	return;

WHILE:
	while (1) {
		printf("\nSAMPLE_TEST:press 'e' to exit; 'q' to query;\n");
		c = getchar();

		if (c == 'e')
			break;
		else if (c == 'q') {
			CVI_VDEC_QueryStatus(pstVdecSend->s32ChnId, &stStatus);
			PRINTF_VDEC_CHN_STATUS(pstVdecSend->s32ChnId, stStatus);
		}
	}
}

CVI_VOID SAMPLE_COMM_VDEC_StartSendStream(VDEC_THREAD_PARAM_S *pstVdecSend,
		pthread_t *pVdecThread)
{
	struct sched_param param;
	pthread_attr_t attr;

	param.sched_priority = 80;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_create(pVdecThread, &attr, SAMPLE_COMM_VDEC_SendStream, (CVI_VOID *)pstVdecSend);
}

CVI_VOID SAMPLE_COMM_VDEC_StopSendStream(VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
	pstVdecSend->eThreadCtrl = THREAD_CTRL_STOP;
	if (*pVdecThread != 0) {
		pthread_join(*pVdecThread, CVI_NULL);
		*pVdecThread = 0;
	}
}

CVI_VOID *SAMPLE_COMM_VDEC_GetPic(CVI_VOID *pArgs)
{
	VDEC_THREAD_PARAM_S *pstVdecThreadParam = (VDEC_THREAD_PARAM_S *)pArgs;
	FILE *fp = CVI_NULL;
	CVI_S32 s32Ret, s32Cnt = 0;
	VDEC_CHN_ATTR_S stAttr;
	VIDEO_FRAME_INFO_S stVFrame;
	VIDEO_FRAME_S *pstVFrame = &stVFrame.stVFrame;
	CVI_CHAR cSaveFile[256];

	CVI_VDEC_TRACE("\n");

	prctl(PR_SET_NAME, "VdecGetPic", 0, 0, 0);

	s32Ret = CVI_VDEC_GetChnAttr(pstVdecThreadParam->s32ChnId, &stAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("chn %d get chn attr fail for %#x!\n",
				pstVdecThreadParam->s32ChnId,
				s32Ret);
		return (CVI_VOID *)(CVI_FAILURE);
	}

	if (stAttr.enType != PT_JPEG &&
	    stAttr.enType != PT_H264 &&
	    stAttr.enType != PT_H265 &&
	    stAttr.enType != PT_MJPEG) {
		CVI_VDEC_ERR("chn %d enType %d do not support save file!\n",
				pstVdecThreadParam->s32ChnId,
				stAttr.enType);
		return (CVI_VOID *)(CVI_FAILURE);
	}

	while (1) {
RETRY_GET_FRAME:
		if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
			break;

		s32Ret = CVI_VDEC_GetFrame(
				pstVdecThreadParam->s32ChnId,
				&stVFrame,
				pstVdecThreadParam->s32MilliSec);
		if (s32Ret == CVI_SUCCESS) {
			CVI_VDEC_DISP("PTS = %"PRId64", u32TimeRef = %d\n",
					pstVFrame->u64PTS, pstVFrame->u32TimeRef);

			if (pstVdecThreadParam->bDumpYUV == 1) {
				if (s32Cnt == 0) {
					SAVE_FILE_NAME(cSaveFile, pstVdecThreadParam->s32ChnId,
							pstVdecThreadParam->cFileName, stVFrame.stVFrame.enPixelFormat);
					CVI_VDEC_TRACE("SAVE_FILE_NAME %s\n\n", cSaveFile);

					fp = fopen(cSaveFile, "wb");
					if (fp == NULL) {
						CVI_VDEC_ERR("chn %d can't open file %s\n",
							   pstVdecThreadParam->s32ChnId, cSaveFile);
						return (CVI_VOID *)(CVI_FAILURE);
					}
					CVI_VDEC_TRACE("\033[0;34m chn %d saving yuv file:%s \033[0;39m\n",
						   pstVdecThreadParam->s32ChnId, cSaveFile);
				}

				write_yuv(fp, stVFrame.stVFrame);
			} else {
				md5Sum_update(&pstVdecThreadParam->tMD5Ctx, stVFrame.stVFrame);
			}

			s32Cnt++;

			s32Ret = CVI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("chn %d CVI_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n",
					   pstVdecThreadParam->s32ChnId, s32Ret);
			}
		} else {
			if (s32Ret == CVI_ERR_VDEC_BUSY) {
				CVI_VDEC_TRACE("vdec getframe timeout...retry\n");
				goto RETRY_GET_FRAME;
			}
			usleep(pstVdecThreadParam->s32IntervalTime);
		}
	}

	if (pstVdecThreadParam->bDumpYUV == 1) {
		if (fp != CVI_NULL)
			fclose(fp);
	}

	CVI_VDEC_TRACE("\033[0;35m chn %d get pic thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);

	return (CVI_VOID *)CVI_SUCCESS;
}

CVI_VOID SAMPLE_COMM_VDEC_StartGetPic(VDEC_THREAD_PARAM_S *pstVdecGet,
		pthread_t *pVdecThread)
{
	struct sched_param param;
	pthread_attr_t attr;

	param.sched_priority = 80;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_create(pVdecThread, &attr, SAMPLE_COMM_VDEC_GetPic, (CVI_VOID *)pstVdecGet);
}

CVI_VOID SAMPLE_COMM_VDEC_StopGetPic(VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
	pstVdecGet->eThreadCtrl = THREAD_CTRL_STOP;
	if (*pVdecThread != 0) {
		pthread_join(*pVdecThread, CVI_NULL);
		*pVdecThread = 0;
	}
}

CVI_S32 SAMPLE_COMM_VDEC_SetVbMode(CVI_S32 VdecVbSrc)
{
	if (VdecVbSrc == 0)
		g_VdecVbSrc = VB_SOURCE_COMMON;
	else if (VdecVbSrc == 3)
		g_VdecVbSrc = VB_SOURCE_USER;
	else {
		CVI_VDEC_ERR("set source mode %d error\n", VdecVbSrc);
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VDEC_GetVbMode(CVI_VOID)
{
	return g_VdecVbSrc;
}

CVI_S32 SAMPLE_COMM_VDEC_Start(vdecChnCtx *pvdchnCtx)
{
	VDEC_CHN_ATTR_S stChnAttr, *pstChnAttr = &stChnAttr;
	VDEC_CHN VdecChn = pvdchnCtx->VdecChn;
	SAMPLE_VDEC_ATTR *psvdattr = &pvdchnCtx->stSampleVdecAttr;
	VDEC_CHN_PARAM_S stChnParam;
	VDEC_MOD_PARAM_S stModParam;

	pstChnAttr->enType = psvdattr->enType;
	pstChnAttr->enMode = psvdattr->enMode;
	pstChnAttr->u32PicWidth = psvdattr->u32Width;
	pstChnAttr->u32PicHeight = psvdattr->u32Height;
	pstChnAttr->u32StreamBufSize = ALIGN(psvdattr->u32Width * psvdattr->u32Height, 0x4000);
	CVI_VDEC_MEM("u32StreamBufSize = 0x%X\n", pstChnAttr->u32StreamBufSize);
	pstChnAttr->u32FrameBufCnt = psvdattr->u32FrameBufCnt;

	if (psvdattr->enType == PT_JPEG || psvdattr->enType == PT_MJPEG) {
		pstChnAttr->enMode = VIDEO_MODE_FRAME;
		pstChnAttr->u32FrameBufSize = VDEC_GetPicBufferSize(
				pstChnAttr->enType, psvdattr->u32Width, psvdattr->u32Height,
				psvdattr->enPixelFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE);
	}

	CVI_VDEC_TRACE("VdecChn = %d\n", VdecChn);

	if (g_VdecVbSrc != VB_SOURCE_COMMON) {
		CVI_VDEC_GetModParam(&stModParam);
		stModParam.enVdecVBSource = g_VdecVbSrc;
		CVI_VDEC_SetModParam(&stModParam);
	}

	if (pvdchnCtx->bCreateChn == CVI_FALSE) {
		CHECK_CHN_RET(CVI_VDEC_CreateChn(VdecChn, pstChnAttr), VdecChn, "CVI_VDEC_CreateChn");
		pvdchnCtx->bCreateChn = CVI_TRUE;
	} else {
		CHECK_CHN_RET(CVI_VDEC_SetChnAttr(VdecChn, pstChnAttr), VdecChn, "CVI_VDEC_SetChnAttr");
	}

	if (g_VdecVbSrc == VB_SOURCE_USER) {
		VDEC_CHN_POOL_S stPool;

		stPool.hPicVbPool = g_ahPicVbPool[VdecChn];
		stPool.hTmvVbPool = g_ahTmvVbPool[VdecChn];

		CVI_VDEC_AttachVbPool(VdecChn, &stPool);
	}

	CHECK_CHN_RET(CVI_VDEC_GetChnParam(VdecChn, &stChnParam), VdecChn, "CVI_VDEC_GetChnParam");

	if (psvdattr->enType == PT_H264 || psvdattr->enType == PT_H265) {
#if 0
		stChnParam.stVdecVideoParam.enDecMode = psvdattr->stSampleVdecVideo.enDecMode;
		stChnParam.stVdecVideoParam.enCompressMode = COMPRESS_MODE_NONE;
		stChnParam.stVdecVideoParam.enVideoFormat = VIDEO_FORMAT_TILE_64x16;
		if (stChnParam.stVdecVideoParam.enDecMode == VIDEO_DEC_MODE_IPB) {
			stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DISP;
		} else {
			stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DEC;
		}
#endif
	} else {
		stChnParam.stVdecPictureParam.u32Alpha =
			psvdattr->stSampleVdecPicture.u32Alpha;
	}
	stChnParam.enPixelFormat = psvdattr->enPixelFormat;
	stChnParam.u32DisplayFrameNum = psvdattr->u32DisplayFrameNum;

	CHECK_CHN_RET(CVI_VDEC_SetChnParam(VdecChn, &stChnParam), VdecChn, "CVI_MPI_VDEC_GetChnParam");

	CHECK_CHN_RET(CVI_VDEC_StartRecvStream(VdecChn), VdecChn, "CVI_MPI_VDEC_StartRecvStream");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VDEC_Stop(CVI_S32 s32ChnNum)
{
	CVI_S32 i;

	for (i = 0; i < s32ChnNum; i++) {
		CHECK_CHN_RET(CVI_VDEC_StopRecvStream(i), i, "CVI_MPI_VDEC_StopRecvStream");
		if (g_VdecVbSrc == VB_SOURCE_USER) {
			CVI_VDEC_TRACE("detach in user mode\n");
			CHECK_CHN_RET(CVI_VDEC_DetachVbPool(i), i, "CVI_VDEC_DetachVbPool");
		}
		CHECK_CHN_RET(CVI_VDEC_ResetChn(i), i, "CVI_MPI_VDEC_ResetChn");
		CHECK_CHN_RET(CVI_VDEC_DestroyChn(i), i, "CVI_MPI_VDEC_DestroyChn");
	}

	return CVI_SUCCESS;
}
