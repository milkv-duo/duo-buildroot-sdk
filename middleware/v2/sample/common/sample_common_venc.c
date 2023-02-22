#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "sample_comm.h"
#include "cvi_venc.h"

//test cases for ROI H264/H265
const VENC_ROI_ATTR_S RoiAttr_testcase[2][4] = {
	// H264 4 ROI test cases.
	{
		{
			.u32Index = 0,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_TRUE,
			.s32Qp = 10,
			.stRect.s32X = 64,
			.stRect.s32Y = 64,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 1,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -10,
			.stRect.s32X = 256,
			.stRect.s32Y = 64,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 2,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_TRUE,
			.s32Qp = 5,
			.stRect.s32X = 64,
			.stRect.s32Y = 256,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 3,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -5,
			.stRect.s32X = 256,
			.stRect.s32Y = 256,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
	},
	// H265 4 ROI test cases.
	{
		{
			.u32Index = 0,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -8,
			.stRect.s32X = 64,
			.stRect.s32Y = 64,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 1,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -5,
			.stRect.s32X = 256,
			.stRect.s32Y = 64,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 2,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -8,
			.stRect.s32X = 64,
			.stRect.s32Y = 256,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
		{
			.u32Index = 3,
			.bEnable = CVI_TRUE,
			.bAbsQp = CVI_FALSE,
			.s32Qp = -5,
			.stRect.s32X = 256,
			.stRect.s32Y = 256,
			.stRect.u32Width = 64,
			.stRect.u32Height = 64,
		},
	},
};


VB_POOL gVencPicVbPool[VB_MAX_COMM_POOLS] = { [0 ...(VB_MAX_COMM_POOLS - 1)] = VB_INVALID_POOLID };
VB_POOL gVencPicInfoVbPool[VB_MAX_COMM_POOLS] = { [0 ...(VB_MAX_COMM_POOLS - 1)] = VB_INVALID_POOLID };

typedef struct _SAMPLE_COMM_VENC_GET_STREAM_ {
	SAMPLE_VENC_GETSTREAM_PARA_S *pstPara;
	VENC_CHN_ATTR_S stVencChnAttr;
	PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
	CVI_CHAR file_ext[16];
	CVI_CHAR fileName[VENC_MAX_CHN_NUM][MAX_STRING_LEN];
	CVI_S32 maxfd;
	FILE *pFile[VENC_MAX_CHN_NUM];
	CVI_S32 VencFd[VENC_MAX_CHN_NUM];
	CVI_U32 u32PictureCnt[VENC_MAX_CHN_NUM];
} SAMPLE_COMM_VENC_GET_STREAM;

enum _SAMPLE_COMM_VENC_STAT_ {
	SCV_STAT_CONTINUE = 1,
	SCV_STAT_BREAK,
} SAMPLE_COMM_VENC_STAT;

pthread_t gs_VencTask[VENC_MAX_CHN_NUM];

static CVI_S32 SAMPLE_COMM_VENC_GetDataType(PAYLOAD_TYPE_E enType, VENC_PACK_S *ppack);
static CVI_S32 SAMPLE_COMM_VENC_SetChnAttr(
		chnInputCfg * pIc,
		VENC_CHN_ATTR_S *pstVencChnAttr,
		PAYLOAD_TYPE_E enType,
		PIC_SIZE_E enSize,
		SAMPLE_RC_E enRcMode,
		CVI_U32 u32Profile,
		VENC_GOP_ATTR_S *pstGopAttr,
		CVI_BOOL bRcnRefShareBuf);
static CVI_S32	SAMPLE_COMM_VENC_SetRcParam(
		chnInputCfg * pIc,
		VENC_CHN VencChn);
static CVI_S32	SAMPLE_COMM_VENC_SetRefParam(
		chnInputCfg * pIc,
		VENC_CHN VencChn);
static CVI_S32 SAMPLE_COMM_VENC_SetFrameLost(
		chnInputCfg * pIc,
		VENC_CHN VencChn);
static CVI_S32 SAMPLE_COMM_VENC_SetSuperFrame(
		chnInputCfg * pIc,
		VENC_CHN VencChn);
static CVI_S32 SAMPLE_COMM_VENC_SetCuPrediction(
		chnInputCfg * pIc,
		VENC_CHN VencChn);
static CVI_S32 SAMPLE_COMM_VENC_DetachVbPool(VENC_CHN VencChn);

CVI_VOID SAMPLE_COMM_VENC_InitCommonInputCfg(commonInputCfg *pCic)
{
	if (!pCic) {
		CVI_VENC_ERR("pCic = NULL\n");
		return;
	}

	memset(pCic, 0, sizeof(commonInputCfg));
	pCic->ifInitVb = 1;
	pCic->vbMode = VB_SOURCE_COMMON;
	pCic->bSingleEsBuf_jpege = 0;
	pCic->bSingleEsBuf_h264e = 0;
	pCic->bSingleEsBuf_h265e = 0;
	pCic->singleEsBufSize_jpege = 0;
	pCic->singleEsBufSize_h264e = 0;
	pCic->singleEsBufSize_h265e = 0;
	pCic->h265RefreshType = 0;
	pCic->jpegMarkerOrder = 0;
	pCic->bThreadDisable = 0;
}

CVI_VOID SAMPLE_COMM_VENC_InitChnInputCfg(chnInputCfg *pIc)
{
	if (!pIc) {
		CVI_VENC_ERR("pIc = NULL\n");
		return;
	}

	memset(pIc, 0, sizeof(chnInputCfg));
	pIc->u32Profile = CVI_H264_PROFILE_DEFAULT;
	pIc->rcMode = -1;
	pIc->iqp = -1;
	pIc->pqp = -1;
	pIc->gop = CVI_H26X_GOP_DEFAULT;
	pIc->gopMode = CVI_H26X_GOP_MODE_DEFAULT;
	pIc->bitrate = -1;
	pIc->firstFrmstartQp = -1;
	pIc->num_frames = -1;
	pIc->framerate = 30;
	pIc->bVariFpsEn = 0;
	pIc->maxIprop = CVI_H26X_MAX_I_PROP_DEFAULT;
	pIc->minIprop = CVI_H26X_MIN_I_PROP_DEFAULT;
	pIc->maxQp = -1;
	pIc->minQp = -1;
	pIc->maxIqp = -1;
	pIc->minIqp = -1;
	pIc->quality = -1;
	pIc->maxbitrate = -1;
	pIc->statTime = -1;
	pIc->bind_mode = VENC_BIND_DISABLE;
	pIc->pixel_format = 0;
	pIc->bitstreamBufSize = 0;
	pIc->single_LumaBuf = 0;
	pIc->single_core = 0;
	pIc->forceIdr = -1;
	pIc->chgNum = -1;
	pIc->tempLayer = 0;
	pIc->bgInterval = CVI_H26X_SMARTP_BG_INTERVAL_DEFAULT;
	pIc->frameLost = -1;
	pIc->frameLostBspThr = -1;
	pIc->frameLostGap = -1;
	pIc->MCUPerECS = 0;
	pIc->sendframe_timeout = 20000;
	pIc->getstream_timeout = -1;
	pIc->s32IPQpDelta = CVI_H26X_NORMALP_IP_QP_DELTA_DEFAULT;
	pIc->s32BgQpDelta = CVI_H26X_SMARTP_BG_QP_DELTA_DEFAULT;
	pIc->s32ViQpDelta = CVI_H26X_SMARTP_VI_QP_DELTA_DEFAULT;
	pIc->initialDelay = CVI_INITIAL_DELAY_DEFAULT;
	pIc->h264EntropyMode = H264E_ENTROPY_CABAC;
	pIc->h264ChromaQpOffset = 0;
	pIc->h265CbQpOffset = 0;
	pIc->h265CrQpOffset = 0;
	pIc->u32RowQpDelta = CVI_H26X_ROW_QP_DELTA_DEFAULT;
	pIc->enSuperFrmMode = CVI_H26X_SUPER_FRM_MODE_DEFAULT;
	pIc->u32SuperIFrmBitsThr = CVI_H26X_SUPER_I_BITS_THR_DEFAULT;
	pIc->u32SuperPFrmBitsThr = CVI_H26X_SUPER_P_BITS_THR_DEFAULT;
	pIc->s32MaxReEncodeTimes = CVI_H26X_MAX_RE_ENCODE_DEFAULT;

	pIc->aspectRatioInfoPresentFlag = CVI_H26X_ASPECT_RATIO_INFO_PRESENT_FLAG_DEFAULT;
	pIc->aspectRatioIdc = CVI_H26X_ASPECT_RATIO_IDC_DEFAULT;
	pIc->overscanInfoPresentFlag = CVI_H26X_OVERSCAN_INFO_PRESENT_FLAG_DEFAULT;
	pIc->overscanAppropriateFlag = CVI_H26X_OVERSCAN_APPROPRIATE_FLAG_DEFAULT;
	pIc->sarWidth = CVI_H26X_SAR_WIDTH_DEFAULT;
	pIc->sarHeight = CVI_H26X_SAR_HEIGHT_DEFAULT;

	pIc->timingInfoPresentFlag = CVI_H26X_TIMING_INFO_PRESENT_FLAG_DEFAULT;
	pIc->fixedFrameRateFlag = CVI_H264_FIXED_FRAME_RATE_FLAG_DEFAULT;
	pIc->numUnitsInTick = CVI_H26X_NUM_UNITS_IN_TICK_DEFAULT;
	pIc->timeScale = CVI_H26X_TIME_SCALE_DEFAULT;

	pIc->videoSignalTypePresentFlag = CVI_H26X_VIDEO_SIGNAL_TYPE_PRESENT_FLAG_DEFAULT;
	pIc->videoFormat = CVI_H26X_VIDEO_FORMAT_DEFAULT;
	pIc->videoFullRangeFlag = CVI_H26X_VIDEO_FULL_RANGE_FLAG_DEFAULT;
	pIc->colourDescriptionPresentFlag = CVI_H26X_COLOUR_DESCRIPTION_PRESENT_FLAG_DEFAULT;
	pIc->colourPrimaries = CVI_H26X_COLOUR_PRIMARIES_DEFAULT;
	pIc->transferCharacteristics = CVI_H26X_TRANSFER_CHARACTERISTICS_DEFAULT;
	pIc->matrixCoefficients = CVI_H26X_MATRIX_COEFFICIENTS_DEFAULT;

	pIc->u32FrameQp = CVI_H26X_FRAME_QP_DEFAULT;
	pIc->bTestUbrEn = CVI_H26X_TEST_UBR_EN_DEFAULT;
	pIc->bEsBufQueueEn = CVI_H26X_ES_BUFFER_QUEUE_DEFAULT;
	pIc->bIsoSendFrmEn = CVI_H26X_ISO_SEND_FRAME_DEFAUL;
	pIc->bSensorEn = CVI_H26X_SENSOR_EN_DEFAULT;

	pIc->u32SliceCnt = 1;
	pIc->bIntraPred = 0;
}

CVI_S32 SAMPLE_COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType,
		FILE *pFd, VENC_STREAM_S *pstStream)
{
	VENC_PACK_S *ppack;
	CVI_S32 dataType;

	if (!pFd) {
		CVI_VENC_ERR("pFd = NULL\n");
		return CVI_FAILURE;
	}

	CVI_VENC_TRACE("u32PackCount = %d\n", pstStream->u32PackCount);

	for (CVI_U32 i = 0; i < pstStream->u32PackCount; i++) {
		ppack = &pstStream->pstPack[i];
		fwrite(ppack->pu8Addr + ppack->u32Offset,
				ppack->u32Len - ppack->u32Offset, 1, pFd);

		dataType = SAMPLE_COMM_VENC_GetDataType(enType, ppack);
		if (dataType < 0) {
			CVI_VENC_ERR("dataType = %d\n", dataType);
			return CVI_FAILURE;
		}

		CVI_VENC_BS("pack[%d], PTS = %"PRId64", DataType = %d\n",
				i, ppack->u64PTS, dataType);
		CVI_VENC_BS("Addr = %p, Len = 0x%X, Offset = 0x%X\n",
				ppack->pu8Addr, ppack->u32Len, ppack->u32Offset);
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_SaveChannelStream(vencChnCtx *pvecc)
{
	VENC_CHN_STATUS_S stStat;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_STREAM_S stStream;
	VENC_CHN VencChn = pvecc->VencChn;
	chnInputCfg *pIc = &pvecc->chnIc;
	CVI_S32 s32Ret;

	do {
		s32Ret = CVI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_GetChnAttr, VencChn = %d, s32Ret = %d\n",
					VencChn, s32Ret);
			return s32Ret;
		}

		s32Ret = CVI_VENC_QueryStatus(VencChn, &stStat);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_QueryStatus, Vench = %d, s32Ret = %d\n",
					VencChn, s32Ret);
			return s32Ret;
		}

		if (!stStat.u32CurPacks) {
			CVI_VENC_ERR("u32CurPacks = NULL!\n");
			return s32Ret;
		}

		stStream.pstPack =
			(VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
		if (stStream.pstPack == NULL) {
			CVI_VENC_ERR("malloc memory failed!\n");
			return s32Ret;
		}
RETRY_GET_STREAM:
		s32Ret = CVI_VENC_GetStream(VencChn, &stStream, pIc->getstream_timeout);
		if (s32Ret != CVI_SUCCESS) {
			if (s32Ret == CVI_ERR_VENC_BUSY) {
				CVI_VENC_WARN("CVI_VENC_GetStream, VencChn Retry= %d,s32Ret = 0x%X\n",
				VencChn, s32Ret);
				goto RETRY_GET_STREAM;
			} else {
				CVI_VENC_WARN("CVI_VENC_GetStream, VencChn = %d, s32Ret = 0x%X\n",
					VencChn, s32Ret);
				break;
			}
		}

		CVI_VENC_BIND("CVI_VENC_GetStream, VencChn = %d\n", VencChn);

		s32Ret = SAMPLE_COMM_VENC_SaveStream(
				stVencChnAttr.stVencAttr.enType,
				pvecc->pFile,
				&stStream);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SaveStream, s32Ret = %d\n", s32Ret);
			break;
		}

		s32Ret = CVI_VENC_QueryStatus(VencChn, &stStat);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_QueryStatus, Vench = %d, s32Ret = %d\n",
					VencChn, s32Ret);
			return s32Ret;
		}
		CVI_VENC_BS("MeanQp = %d\n", stStat.stVencStrmInfo.u32MeanQp);

		s32Ret = CVI_VENC_ReleaseStream(VencChn, &stStream);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_ReleaseStream, s32Ret = %d\n", s32Ret);
			break;
		}
	} while (CVI_FALSE);

	free(stStream.pstPack);
	stStream.pstPack = NULL;

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_GetDataType(PAYLOAD_TYPE_E enType, VENC_PACK_S *ppack)
{
	if (enType == PT_H264)
		return ppack->DataType.enH264EType;
	else if (enType == PT_H265)
		return ppack->DataType.enH265EType;
	else if (enType == PT_JPEG || enType == PT_MJPEG)
		return ppack->DataType.enJPEGEType;

	CVI_VENC_ERR("enType = %d\n", enType);
	return CVI_FAILURE;
}

static CVI_S32 SAMPLE_COMM_VENC_DetachVbPool(VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VB_SOURCE_E eVbSource;
	VENC_CHN_ATTR_S stChnAttr;
	VENC_PARAM_MOD_S stModParam;

	s32Ret = CVI_VENC_GetChnAttr(VencChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetChnAttr vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}

	if (stChnAttr.stVencAttr.enType == PT_H264) {
		stModParam.enVencModType = MODTYPE_H264E;
	} else if (stChnAttr.stVencAttr.enType == PT_H265) {
		stModParam.enVencModType = MODTYPE_H265E;
	} else {
		return s32Ret;
	}

	s32Ret = CVI_VENC_GetModParam(&stModParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetModParam vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}

	if (stChnAttr.stVencAttr.enType == PT_H264)
		eVbSource = stModParam.stH264eModParam.enH264eVBSource;
	else if (stChnAttr.stVencAttr.enType == PT_H265)
		eVbSource = stModParam.stH265eModParam.enH265eVBSource;
	else {
		return s32Ret;
	}
	//get_modparam	-> user mode
	CVI_VENC_CFG("eVbSource[%d]\n", eVbSource);
	if (eVbSource == VB_SOURCE_USER) {
		s32Ret = CVI_VENC_DetachVbPool(VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_DetachVbPool vechn[%d] failed with %#x!\n",
					VencChn, s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn)
{
	CVI_S32 s32Ret;

	if (gs_VencTask[VencChn] != 0) {
		pthread_join(gs_VencTask[VencChn], CVI_NULL);
		CVI_VENC_SYNC("GetVencStreamProc done\n");

		gs_VencTask[VencChn] = 0;
	}

	s32Ret = CVI_VENC_StopRecvFrame(VencChn);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VENC_ResetChn(VencChn);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_ResetChn vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = SAMPLE_COMM_VENC_DetachVbPool(VencChn);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SAMPLE_COMM_VENC_DetachVbPool vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VENC_DestroyChn(VencChn);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_DestroyChn vechn[%d] failed with %#x!\n",
				VencChn, s32Ret);
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix)
{
	if (enPayload == PT_H264)
		strcpy(szFilePostfix, ".h264");
	else if (enPayload == PT_H265)
		strcpy(szFilePostfix, ".h265");
	else if (enPayload == PT_JPEG)
		strcpy(szFilePostfix, ".jpg");
	else if (enPayload == PT_MJPEG)
		strcpy(szFilePostfix, ".mjp");
	else {
		CVI_VENC_ERR("payload type err!\n");
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode, VENC_GOP_ATTR_S *pstGopAttr)
{
	switch (enGopMode) {
	case VENC_GOPMODE_NORMALP:
		pstGopAttr->enGopMode = VENC_GOPMODE_NORMALP;
		pstGopAttr->stNormalP.s32IPQpDelta = CVI_H26X_NORMALP_IP_QP_DELTA_DEFAULT;
		CVI_VENC_CFG("s32IPQpDelta = %d\n", pstGopAttr->stNormalP.s32IPQpDelta);
		break;

	case VENC_GOPMODE_SMARTP:
		pstGopAttr->enGopMode = VENC_GOPMODE_SMARTP;
		pstGopAttr->stSmartP.s32BgQpDelta = CVI_H26X_SMARTP_BG_QP_DELTA_DEFAULT;
		pstGopAttr->stSmartP.s32ViQpDelta = CVI_H26X_SMARTP_VI_QP_DELTA_DEFAULT;
		pstGopAttr->stSmartP.u32BgInterval = CVI_H26X_SMARTP_BG_INTERVAL_DEFAULT;
		CVI_VENC_CFG("u32BgInterval %d, s32BgQpDelta %d, s32ViQpDelta %d\n",
				pstGopAttr->stSmartP.u32BgInterval,
				pstGopAttr->stSmartP.s32BgQpDelta,
				pstGopAttr->stSmartP.s32ViQpDelta);
		break;

	default:
		CVI_VENC_ERR("not support the gop mode %d!\n", enGopMode);
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_CloseReEncode(VENC_CHN VencChn)
{
	CVI_S32 s32Ret;
	VENC_RC_PARAM_S stRcParam;
	VENC_CHN_ATTR_S stChnAttr;

	s32Ret = CVI_VENC_GetChnAttr(VencChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetChnAttr failed!\n");
		return CVI_FAILURE;
	}

	s32Ret = CVI_VENC_GetRcParam(VencChn, &stRcParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetRcParam failed!\n");
		return CVI_FAILURE;
	}

	if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR)
		stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
	else if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR)
		stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
	else if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
		stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
	else if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
		stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
	else
		return CVI_SUCCESS;

	s32Ret = CVI_VENC_SetRcParam(VencChn, &stRcParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetRcParam failed!\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_SetRoiAttr(VENC_CHN VencChn, PAYLOAD_TYPE_E enType)
{
	CVI_S32 s32Ret;
	VENC_ROI_ATTR_S RoiAttr;
	int type = 0;
	int i;

	if (enType == PT_H264) {
		type = 0;
	} else if (enType == PT_H265) {
		type = 1;
	} else {
		CVI_VENC_ERR("SetRoiAttr only support H264/H265!\n");
		return CVI_FAILURE;
	}

	for (i = 0; i < 4; i++) {
		s32Ret = CVI_VENC_GetRoiAttr(VencChn, i, &RoiAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("GetRoiAttr failed!\n");
			return CVI_FAILURE;
		}
		RoiAttr.bEnable = RoiAttr_testcase[type][i].bEnable;
		RoiAttr.bAbsQp = RoiAttr_testcase[type][i].bAbsQp;
		RoiAttr.s32Qp = RoiAttr_testcase[type][i].s32Qp;
		RoiAttr.stRect.s32X = RoiAttr_testcase[type][i].stRect.s32X;
		RoiAttr.stRect.s32Y = RoiAttr_testcase[type][i].stRect.s32Y;
		RoiAttr.stRect.u32Width = RoiAttr_testcase[type][i].stRect.u32Width;
		RoiAttr.stRect.u32Height = RoiAttr_testcase[type][i].stRect.u32Height;

		s32Ret = CVI_VENC_SetRoiAttr(VencChn, &RoiAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SetRoiAttr failed!\n");
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetQpMapByCfgFile(VENC_CHN VencChn,
		SAMPLE_COMM_VENC_ROI *vencRoi, CVI_U32 frame_idx,
		CVI_U8 *pu8QpMap, CVI_BOOL *pbQpMapValid,
		CVI_U32 u32Width, CVI_U32 u32Height)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	SAMPLE_COMM_VENC_ROI *pVencRoi;
	VENC_ROI_ATTR_S *pRoiAttr;
	(void) VencChn;
	CVI_U32 s32Left, s32Right;
	CVI_U32 s32Top, s32Bottom;
	CVI_U32 i, row, col;
	CVI_U32 ctbHeight = ((u32Height + 63) & ~63) >> 6;
	CVI_U32 ctbStride = ((u32Width + 63) & ~63) >> 6;

	*pbQpMapValid = CVI_FALSE;
	for (i = 0; i < MAX_NUM_ROI; ++i) {
		pVencRoi = &vencRoi[i];
		pRoiAttr = &pVencRoi->stVencRoiAttr;

		CVI_VENC_TRACE("roi[%d]\n", i);

		if (frame_idx >= pVencRoi->u32FrameStart &&
		    frame_idx <= pVencRoi->u32FrameEnd) {
			pRoiAttr->bEnable = true;
			*pbQpMapValid = CVI_TRUE;
		} else {
			pRoiAttr->bEnable = false;
			continue;
		}

		s32Left = pRoiAttr->stRect.s32X >> 6;
		s32Right = (pRoiAttr->stRect.s32X + pRoiAttr->stRect.u32Width - 1) >> 6;
		if (s32Left > s32Right || s32Right >= ctbStride) {
			CVI_VENC_ERR("s32Left = %d, s32Right = %d, ctbStride = %d\n",
					s32Left, s32Right, ctbStride);
			return CVI_FAILURE;
		}

		s32Top = pRoiAttr->stRect.s32Y >> 6;
		s32Bottom = (pRoiAttr->stRect.s32Y + pRoiAttr->stRect.u32Height - 1) >> 6;
		if (s32Top > s32Bottom || s32Bottom >= ctbHeight) {
			CVI_VENC_ERR("s32Top = %d, s32Bottom = %d, ctbHeight = %d\n",
					s32Top, s32Bottom, ctbHeight);
			return CVI_FAILURE;
		}

		CVI_VENC_TRACE("s32Left = %d, s32Right = %d, s32Top = %d, s32Bottom = %d\n",
				s32Left, s32Right, s32Top, s32Bottom);

		for (row = s32Top; row <= s32Bottom; row++) {
			for (col = s32Left; col <= s32Right; col++) {
				pu8QpMap[ctbStride * row + col] = pRoiAttr->s32Qp;
			}
		}
	}

	CVI_VENC_TRACE("ctbStride = %d, ctbHeight = %d, QpMap = %p\n",
			ctbStride, ctbHeight, pu8QpMap);

	if (vencDbg.currMask & CVI_VENC_MASK_TRACE) {
		for (row = 0; row < ctbHeight; row++) {
			for (col = 0; col < ctbStride; col++) {
				printf("%3d ", pu8QpMap[ctbStride * row + col]);
			}
			printf("\n");
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetRoiAttrByCfgFile(VENC_CHN VencChn, SAMPLE_COMM_VENC_ROI *vencRoi, CVI_U32 frame_idx)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_ROI_ATTR_S RoiAttr;
	int i;

	for (i = 0; i < MAX_NUM_ROI; ++i) {
		if (frame_idx >= vencRoi[i].u32FrameStart &&
		    frame_idx <= vencRoi[i].u32FrameEnd) {
			vencRoi[i].stVencRoiAttr.bEnable = true;
		} else {
			vencRoi[i].stVencRoiAttr.bEnable = false;
		}

		s32Ret = CVI_VENC_GetRoiAttr(VencChn, i, &RoiAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("GetRoiAttr failed!\n");
			return CVI_FAILURE;
		}

		RoiAttr.bEnable = vencRoi[i].stVencRoiAttr.bEnable;
		RoiAttr.bAbsQp = vencRoi[i].stVencRoiAttr.bAbsQp;
		RoiAttr.s32Qp = vencRoi[i].stVencRoiAttr.s32Qp;
		RoiAttr.stRect.s32X = vencRoi[i].stVencRoiAttr.stRect.s32X;
		RoiAttr.stRect.s32Y = vencRoi[i].stVencRoiAttr.stRect.s32Y;
		RoiAttr.stRect.u32Width = vencRoi[i].stVencRoiAttr.stRect.u32Width;
		RoiAttr.stRect.u32Height = vencRoi[i].stVencRoiAttr.stRect.u32Height;

		s32Ret = CVI_VENC_SetRoiAttr(VencChn, &RoiAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SetRoiAttr failed!\n");
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_LoadRoiCfgFile(SAMPLE_COMM_VENC_ROI *vencRoi, CVI_CHAR *cfgFileName)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	FILE *cfgFile = NULL;
	char line[256] = { 0 };

	cfgFile = fopen(cfgFileName, "r");
	if (cfgFile == NULL) {
		CVI_VENC_ERR("Missing ROI config file, %s\n", cfgFileName);
		return CVI_FAILURE_ILLEGAL_PARAM;
	}

	while (fgets(line, 256, cfgFile) != NULL) {
		SAMPLE_COMM_VENC_ROI _roi;

		if ((line[0] == '#') || (line[0] == ';') || (line[0] == ':'))
			continue;

		if (sscanf(line, "%u %d %d %d %d %u %u %u %u",
				&_roi.stVencRoiAttr.u32Index,
				(int *)(&_roi.stVencRoiAttr.bAbsQp),
				&_roi.stVencRoiAttr.s32Qp,
				&_roi.stVencRoiAttr.stRect.s32X,
				&_roi.stVencRoiAttr.stRect.s32Y,
				&_roi.stVencRoiAttr.stRect.u32Width,
				&_roi.stVencRoiAttr.stRect.u32Height,
				&_roi.u32FrameStart,
				&_roi.u32FrameEnd) == 0) {
			CVI_VENC_ERR("Failed to parse roi cfg file, %s\n", cfgFileName);
			s32Ret = CVI_FAILURE_ILLEGAL_PARAM;
			break;
		}

		if (_roi.stVencRoiAttr.u32Index >= MAX_NUM_ROI) {
			CVI_VENC_ERR("Incorrect index %d in roi cfg file, %s\n",
					_roi.stVencRoiAttr.u32Index, cfgFileName);
			s32Ret = CVI_FAILURE_ILLEGAL_PARAM;
			break;
		}

		memcpy(&vencRoi[_roi.stVencRoiAttr.u32Index], &_roi, sizeof(SAMPLE_COMM_VENC_ROI));

		CVI_VENC_CFG("roi index %d, bEnable %d, bAbsQp %d, s32Qp %d\n",
				_roi.stVencRoiAttr.u32Index, _roi.stVencRoiAttr.bEnable,
				_roi.stVencRoiAttr.bAbsQp, _roi.stVencRoiAttr.s32Qp);
		CVI_VENC_CFG("roi s32X %d, s32Y %d, u32Width %d, u32Height %d\n",
				_roi.stVencRoiAttr.stRect.s32X, _roi.stVencRoiAttr.stRect.s32Y,
				_roi.stVencRoiAttr.stRect.u32Width, _roi.stVencRoiAttr.stRect.u32Height);
		CVI_VENC_CFG("roi frameStart %d ,frameEnd %d\n",
				_roi.u32FrameStart, _roi.u32FrameEnd);
	}

	fclose(cfgFile);
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_Create(
		chnInputCfg * pIc,
		VENC_CHN VencChn,
		PAYLOAD_TYPE_E enType,
		PIC_SIZE_E enSize,
		SAMPLE_RC_E enRcMode,
		CVI_U32 u32Profile,
		CVI_BOOL bRcnRefShareBuf,
		VENC_GOP_ATTR_S *pstGopAttr)
{
	CVI_S32 s32Ret;
	VENC_CHN_ATTR_S stVencChnAttr, *pstVencChnAttr = &stVencChnAttr;

	s32Ret = SAMPLE_COMM_VENC_SetChnAttr(
			pIc,
			pstVencChnAttr,
			enType,
			enSize,
			enRcMode,
			u32Profile,
			pstGopAttr,
			bRcnRefShareBuf);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("Get picture size failed!\n");
		return CVI_FAILURE;
	}

	if (pIc->bCreateChn == CVI_FALSE) {
		s32Ret = CVI_VENC_CreateChn(VencChn, pstVencChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_CreateChn [%d] failed with %d\n", VencChn, s32Ret);
			return s32Ret;
		}
		pIc->bCreateChn = CVI_TRUE;
	}

	if (enType != PT_JPEG) {
		s32Ret = SAMPLE_COMM_VENC_SetRcParam(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetRcParam, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetRefParam(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetRefParam, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetCuPrediction(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetCuPrediction, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetFrameLost(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetFrameLost, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetSuperFrame(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetSuperFrame, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}
	}

	if (enType != PT_JPEG && enType != PT_MJPEG) {
		VB_SOURCE_E eVbSource;
		VENC_PARAM_MOD_S stModParam;

		if (enType == PT_H264)
			stModParam.enVencModType = MODTYPE_H264E;
		else if (enType == PT_H265)
			stModParam.enVencModType = MODTYPE_H265E;
		else {
			CVI_VENC_ERR("Unsupport type[%d]\n", enType);
			eVbSource = VB_SOURCE_MODULE;
			return CVI_FAILURE;
		}

		s32Ret = CVI_VENC_GetModParam(&stModParam);
		if (enType == PT_H264)
			eVbSource = stModParam.stH264eModParam.enH264eVBSource;
		else if (enType == PT_H265)
			eVbSource = stModParam.stH265eModParam.enH265eVBSource;
		else {
			CVI_VENC_CFG("Unknown type[%d]\n", enType);
			eVbSource = VB_SOURCE_MODULE;
		}
		//get_modparam  -> user mode
		CVI_VENC_CFG("eVbSource[%d]\n", eVbSource);
		if (eVbSource == VB_SOURCE_USER) {
			//attachvbpool
			VENC_CHN_POOL_S stPool;

			stPool.hPicVbPool = gVencPicVbPool[VencChn];
			stPool.hPicInfoVbPool = gVencPicInfoVbPool[VencChn];
			CVI_VENC_CFG("CVI_VENC_AttachVbPool chn[%d]\n",  VencChn);
			s32Ret = CVI_VENC_AttachVbPool(VencChn, &stPool);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VENC_ERR("CVI_VENC_AttachVbPool, %d\n", s32Ret);
				goto ERR_SAMPLE_COMM_VENC_CREATE;
			}
		}
	}

	if (enType == PT_H264) {
		VENC_H264_ENTROPY_S h264Entropy = { 0 };

		switch (pIc->h264EntropyMode) {
		case 0:
			h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CAVLC;
			h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CAVLC;
			break;
		case 1:
			h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CABAC;
			h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CABAC;
			break;
		default:
			h264Entropy.u32EntropyEncModeI = H264E_ENTROPY_CABAC;
			h264Entropy.u32EntropyEncModeP = H264E_ENTROPY_CABAC;
			break;
		}

		s32Ret = CVI_VENC_SetH264Entropy(VencChn, &h264Entropy);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_SetH264Entropy, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH264Trans(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Trans, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH264Vui(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Vui, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH264SliceSplit(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264SliceSplit, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH264Dblk(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264Dblk, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH264IntraPred(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH264IntraPred, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}
	}

	if (enType == PT_H265) {
		s32Ret = SAMPLE_COMM_VENC_SetH265Trans(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH265Trans, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH265Vui(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH265Vui, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH265SliceSplit(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH265SliceSplit, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}

		s32Ret = SAMPLE_COMM_VENC_SetH265Dblk(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetH265Dblk, %d\n", s32Ret);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}
	}

	s32Ret = SAMPLE_COMM_VENC_SetChnParam(pIc, VencChn);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SAMPLE_COMM_VENC_SetChnParam, %d\n", s32Ret);
		CVI_VENC_DestroyChn(VencChn);
		goto ERR_SAMPLE_COMM_VENC_CREATE;
	}

	if (enType == PT_JPEG) {
		s32Ret = SAMPLE_COMM_VENC_SetJpegParam(pIc, VencChn);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetJpegParam, %d\n", s32Ret);
			CVI_VENC_DestroyChn(VencChn);
			goto ERR_SAMPLE_COMM_VENC_CREATE;
		}
	}

	return s32Ret;
ERR_SAMPLE_COMM_VENC_CREATE:
	CVI_VENC_DestroyChn(VencChn);

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetChnAttr(
		chnInputCfg * pIc,
		VENC_CHN_ATTR_S *pstVencChnAttr,
		PAYLOAD_TYPE_E enType,
		PIC_SIZE_E enSize,
		SAMPLE_RC_E enRcMode,
		CVI_U32 u32Profile,
		VENC_GOP_ATTR_S *pstGopAttr,
		CVI_BOOL bRcnRefShareBuf)
{
	SIZE_S stPicSize;
	CVI_U32 u32StatTime = 0;
	CVI_U32 u32Gop = 30;
	CVI_U32 u32FrameRate = pIc->framerate;
	CVI_U32 u32SrcFrameRate = pIc->srcFramerate;
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (enSize == PIC_CUSTOMIZE) {
		stPicSize.u32Width = pIc->width;
		stPicSize.u32Height = pIc->height;
	} else {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stPicSize);
	}

	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("Get picture size failed!\n");
		return CVI_FAILURE;
	}

	memset(pstVencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

	pstVencChnAttr->stVencAttr.enType = enType;
	pstVencChnAttr->stVencAttr.u32MaxPicWidth = stPicSize.u32Width;
	pstVencChnAttr->stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
	pstVencChnAttr->stVencAttr.u32PicWidth = stPicSize.u32Width;
	pstVencChnAttr->stVencAttr.u32PicHeight = stPicSize.u32Height;
	pstVencChnAttr->stVencAttr.u32BufSize = pIc->bitstreamBufSize;
	pstVencChnAttr->stVencAttr.bEsBufQueueEn = pIc->bEsBufQueueEn;
	pstVencChnAttr->stVencAttr.bIsoSendFrmEn = pIc->bIsoSendFrmEn;
	pstVencChnAttr->stVencAttr.u32Profile = u32Profile;
	pstVencChnAttr->stVencAttr.bByFrame = CVI_TRUE; // get stream mode is slice mode or
													// frame mode ?

	if (pstGopAttr->enGopMode == VENC_GOPMODE_NORMALP) {
		pstGopAttr->stNormalP.s32IPQpDelta = pIc->s32IPQpDelta;
		CVI_VENC_CFG("s32IPQpDelta = %d\n", pstGopAttr->stNormalP.s32IPQpDelta);
		u32StatTime = pIc->statTime;
	} else if (pstGopAttr->enGopMode == VENC_GOPMODE_SMARTP) {
		pstGopAttr->stSmartP.u32BgInterval = pIc->bgInterval;
		u32StatTime = pstGopAttr->stSmartP.u32BgInterval / u32Gop;

		pstGopAttr->stSmartP.s32BgQpDelta = pIc->s32BgQpDelta;
		pstGopAttr->stSmartP.s32ViQpDelta = pIc->s32ViQpDelta;
		CVI_VENC_CFG("s32BgQpDelta = %d\n", pstGopAttr->stSmartP.s32BgQpDelta);
		CVI_VENC_CFG("s32ViQpDelta = %d\n", pstGopAttr->stSmartP.s32ViQpDelta);
	} else {
		u32StatTime = pIc->statTime;
	}

	switch (enType) {
	case PT_H265: {
		if (enRcMode == SAMPLE_RC_CBR) {
			VENC_H265_CBR_S *pstH265Cbr = &pstVencChnAttr->stRcAttr.stH265Cbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
			pstH265Cbr->u32Gop = pIc->gop;
			pstH265Cbr->u32StatTime = u32StatTime;
			pstH265Cbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Cbr->fr32DstFrameRate = u32FrameRate;
			pstH265Cbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Cbr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH265Cbr->u32BitRate);
		} else if (enRcMode == SAMPLE_RC_FIXQP) {
			VENC_H265_FIXQP_S *pstH265FixQp = &pstVencChnAttr->stRcAttr.stH265FixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
			pstH265FixQp->u32Gop = pIc->gop;
			pstH265FixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstH265FixQp->fr32DstFrameRate = u32FrameRate;
			pstH265FixQp->bVariFpsEn = pIc->bVariFpsEn;
			pstH265FixQp->u32IQp = pIc->iqp;
			pstH265FixQp->u32PQp = pIc->pqp;
			CVI_VENC_CFG("u32Gop = %d, u32IQp = %d, u32PQp = %d\n",
					pstH265FixQp->u32Gop,
					pstH265FixQp->u32IQp,
					pstH265FixQp->u32PQp);
		} else if (enRcMode == SAMPLE_RC_VBR) {
			VENC_H265_VBR_S *pstH265Vbr = &pstVencChnAttr->stRcAttr.stH265Vbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
			pstH265Vbr->u32Gop = pIc->gop;
			pstH265Vbr->u32StatTime = u32StatTime;
			pstH265Vbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Vbr->fr32DstFrameRate = u32FrameRate;
			pstH265Vbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Vbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
					pstH265Vbr->u32StatTime,
					pstH265Vbr->u32Gop,
					pstH265Vbr->u32MaxBitRate);
		} else if (enRcMode == SAMPLE_RC_AVBR) {
			VENC_H265_AVBR_S *pstH265AVbr = &pstVencChnAttr->stRcAttr.stH265AVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
			pstH265AVbr->u32Gop = pIc->gop;
			pstH265AVbr->u32StatTime = u32StatTime;
			pstH265AVbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265AVbr->fr32DstFrameRate = u32FrameRate;
			pstH265AVbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265AVbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
					pstH265AVbr->u32StatTime,
					pstH265AVbr->u32Gop,
					pstH265AVbr->u32MaxBitRate);
		} else if (enRcMode == SAMPLE_RC_QVBR) {
			VENC_H265_QVBR_S stH265QVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
			stH265QVbr.u32Gop = u32Gop;
			stH265QVbr.u32StatTime = u32StatTime;
			stH265QVbr.u32SrcFrameRate = u32SrcFrameRate;
			stH265QVbr.fr32DstFrameRate = u32FrameRate;

			switch (enSize) {
			case PIC_720P:
				stH265QVbr.u32TargetBitRate = 1024 * 2 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stH265QVbr.u32TargetBitRate = 1024 * 5 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120 * u32FrameRate / 30;
				break;
			default:
				stH265QVbr.u32TargetBitRate = 1024 * 15 + 2048 * u32FrameRate / 30;
				break;
			}
			memcpy(&pstVencChnAttr->stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
		} else if (enRcMode == SAMPLE_RC_QPMAP) {
			VENC_H265_QPMAP_S *pstH265QpMap = &pstVencChnAttr->stRcAttr.stH265QpMap;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
			pstH265QpMap->u32Gop = pIc->gop;
			pstH265QpMap->u32StatTime = u32StatTime;
			pstH265QpMap->u32SrcFrameRate = u32SrcFrameRate;
			pstH265QpMap->fr32DstFrameRate = u32FrameRate;
			pstH265QpMap->bVariFpsEn = pIc->bVariFpsEn;
			pstH265QpMap->enQpMapMode = VENC_RC_QPMAP_MODE_MEANQP;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d\n",
					pstH265QpMap->u32StatTime,
					pstH265QpMap->u32Gop);
		} else if (enRcMode == SAMPLE_RC_UBR) {
			VENC_H265_UBR_S *pstH265Ubr = &pstVencChnAttr->stRcAttr.stH265Ubr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265UBR;
			pstH265Ubr->u32Gop = pIc->gop;
			pstH265Ubr->u32StatTime = u32StatTime;
			pstH265Ubr->u32SrcFrameRate = u32SrcFrameRate;
			pstH265Ubr->fr32DstFrameRate = u32FrameRate;
			pstH265Ubr->bVariFpsEn = pIc->bVariFpsEn;
			pstH265Ubr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH265Ubr->u32BitRate);
		} else {
			CVI_VENC_ERR("enRcMode(%d) not support\n", enRcMode);
			return CVI_FAILURE;
		}
		pstVencChnAttr->stVencAttr.stAttrH265e.bRcnRefShareBuf = bRcnRefShareBuf;
	} break;
	case PT_H264: {
		if (enRcMode == SAMPLE_RC_CBR) {
			VENC_H264_CBR_S *pstH264Cbr = &pstVencChnAttr->stRcAttr.stH264Cbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
			pstH264Cbr->u32Gop = pIc->gop;
			pstH264Cbr->u32StatTime = u32StatTime;
			pstH264Cbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Cbr->fr32DstFrameRate = u32FrameRate;
			pstH264Cbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Cbr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("bitrate = %d, u32BitRate = %d\n",
					pIc->bitrate,
					pstH264Cbr->u32BitRate);
		} else if (enRcMode == SAMPLE_RC_FIXQP) {
			VENC_H264_FIXQP_S *pstH264FixQp = &pstVencChnAttr->stRcAttr.stH264FixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
			pstH264FixQp->u32Gop = pIc->gop;
			pstH264FixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstH264FixQp->fr32DstFrameRate = u32FrameRate;
			pstH264FixQp->bVariFpsEn = pIc->bVariFpsEn;
			pstH264FixQp->u32IQp = pIc->iqp;
			pstH264FixQp->u32PQp = pIc->pqp;
			CVI_VENC_CFG("u32Gop = %d, u32IQp = %d, u32PQp = %d\n",
					pstH264FixQp->u32Gop,
					pstH264FixQp->u32IQp,
					pstH264FixQp->u32PQp);
		} else if (enRcMode == SAMPLE_RC_VBR) {
			VENC_H264_VBR_S *pstH264Vbr = &pstVencChnAttr->stRcAttr.stH264Vbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
			pstH264Vbr->u32Gop = pIc->gop;
			pstH264Vbr->u32StatTime = u32StatTime;
			pstH264Vbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Vbr->fr32DstFrameRate = u32FrameRate;
			pstH264Vbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Vbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
					pstH264Vbr->u32StatTime,
					pstH264Vbr->u32Gop,
					pstH264Vbr->u32MaxBitRate);
		} else if (enRcMode == SAMPLE_RC_AVBR) {
			VENC_H264_AVBR_S *pstH264AVbr = &pstVencChnAttr->stRcAttr.stH264AVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
			pstH264AVbr->u32Gop = pIc->gop;
			pstH264AVbr->u32StatTime = u32StatTime;
			pstH264AVbr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264AVbr->fr32DstFrameRate = u32FrameRate;
			pstH264AVbr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264AVbr->u32MaxBitRate = pIc->maxbitrate;
			CVI_VENC_CFG("u32StatTime = %d, u32Gop = %d, u32MaxBitRate = %d\n",
					pstH264AVbr->u32StatTime,
					pstH264AVbr->u32Gop,
					pstH264AVbr->u32MaxBitRate);
		} else if (enRcMode == SAMPLE_RC_QVBR) {
			VENC_H264_QVBR_S stH264QVbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
			stH264QVbr.u32Gop = u32Gop;
			stH264QVbr.u32StatTime = u32StatTime;
			stH264QVbr.u32SrcFrameRate = u32SrcFrameRate;
			stH264QVbr.fr32DstFrameRate = u32FrameRate;
			switch (enSize) {
			case PIC_720P:
				stH264QVbr.u32TargetBitRate = 1024 * 2 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stH264QVbr.u32TargetBitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stH264QVbr.u32TargetBitRate = 1024 * 3 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stH264QVbr.u32TargetBitRate = 1024 * 5 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stH264QVbr.u32TargetBitRate = 1024 * 10 + 5120 * u32FrameRate / 30;
				break;
			default:
				stH264QVbr.u32TargetBitRate = 1024 * 15 + 2048 * u32FrameRate / 30;
				break;
			}
			memcpy(&pstVencChnAttr->stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
		} else if (enRcMode == SAMPLE_RC_UBR) {
			VENC_H264_UBR_S *pstH264Ubr = &pstVencChnAttr->stRcAttr.stH264Ubr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264UBR;
			pstH264Ubr->u32Gop = pIc->gop;
			pstH264Ubr->u32StatTime = u32StatTime;
			pstH264Ubr->u32SrcFrameRate = u32SrcFrameRate;
			pstH264Ubr->fr32DstFrameRate = u32FrameRate;
			pstH264Ubr->bVariFpsEn = pIc->bVariFpsEn;
			pstH264Ubr->u32BitRate = pIc->bitrate;
			CVI_VENC_CFG("u32BitRate = %d\n", pstH264Ubr->u32BitRate);
		} else {
			CVI_VENC_ERR("H.264 enRcMode(%d) not support\n", enRcMode);
			return CVI_FAILURE;
		}
		pstVencChnAttr->stVencAttr.stAttrH264e.bRcnRefShareBuf = bRcnRefShareBuf;
		pstVencChnAttr->stVencAttr.stAttrH264e.bSingleLumaBuf = pIc->single_LumaBuf;
	} break;
	case PT_MJPEG: {
		if (enRcMode == SAMPLE_RC_FIXQP) {
			VENC_MJPEG_FIXQP_S *pstMjpegeFixQp = &pstVencChnAttr->stRcAttr.stMjpegFixQp;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;

			// 0 use old q-table for forward compatible.
			pstMjpegeFixQp->u32Qfactor = (pIc->quality > 0) ? pIc->quality : 0;
			pstMjpegeFixQp->u32SrcFrameRate = u32SrcFrameRate;
			pstMjpegeFixQp->fr32DstFrameRate = u32FrameRate;
			pstMjpegeFixQp->bVariFpsEn = pIc->bVariFpsEn;
		} else if (enRcMode == SAMPLE_RC_CBR) {
			VENC_MJPEG_CBR_S *pstMjpegeCbr = &pstVencChnAttr->stRcAttr.stMjpegCbr;

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
			pstMjpegeCbr->u32StatTime = u32StatTime;
			pstMjpegeCbr->u32SrcFrameRate = u32SrcFrameRate;
			pstMjpegeCbr->fr32DstFrameRate = u32FrameRate;
			pstMjpegeCbr->bVariFpsEn = pIc->bVariFpsEn;
			pstMjpegeCbr->u32BitRate = pIc->bitrate;
		} else if ((enRcMode == SAMPLE_RC_VBR) || (enRcMode == SAMPLE_RC_AVBR) ||
			   (enRcMode == SAMPLE_RC_QVBR)) {
			VENC_MJPEG_VBR_S stMjpegVbr;

			if (enRcMode == SAMPLE_RC_AVBR)
				CVI_VENC_INFO("Mjpege not support AVBR, so change rcmode to VBR!\n");

			pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
			stMjpegVbr.u32StatTime = u32StatTime;
			stMjpegVbr.u32SrcFrameRate = u32SrcFrameRate;
			stMjpegVbr.fr32DstFrameRate = 5;

			switch (enSize) {
			case PIC_720P:
				stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024 * u32FrameRate / 30;
				break;
			case PIC_1080P:
				stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048 * u32FrameRate / 30;
				break;
			case PIC_2592x1944:
				stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072 * u32FrameRate / 30;
				break;
			case PIC_3840x2160:
				stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120 * u32FrameRate / 30;
				break;
			case PIC_4000x3000:
				stMjpegVbr.u32MaxBitRate = 1024 * 30 + 5120 * u32FrameRate / 30;
				break;
			default:
				stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048 * u32FrameRate / 30;
				break;
			}

			memcpy(&pstVencChnAttr->stRcAttr.stMjpegVbr, &stMjpegVbr, sizeof(VENC_MJPEG_VBR_S));
		} else {
			CVI_VENC_ERR("cann't support other mode(%d) in this version!\n", enRcMode);
			return CVI_FAILURE;
		}
	} break;

	case PT_JPEG: {
		VENC_ATTR_JPEG_S *pstJpegAttr = &pstVencChnAttr->stVencAttr.stAttrJpege;

		pstJpegAttr->bSupportDCF = CVI_FALSE;
		pstJpegAttr->stMPFCfg.u8LargeThumbNailNum = 0;
		pstJpegAttr->enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
	} break;

	default:
		CVI_VENC_ERR("cann't support this enType (%d) in this version!\n", enType);
		return CVI_ERR_VENC_NOT_SUPPORT;
	}

	if (PT_MJPEG == enType || PT_JPEG == enType) {
		pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
		pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta = 0;
	} else {
		memcpy(&pstVencChnAttr->stGopAttr, pstGopAttr, sizeof(VENC_GOP_ATTR_S));

		if ((pstGopAttr->enGopMode == VENC_GOPMODE_BIPREDB) && (enType == PT_H264)) {
			if (pstVencChnAttr->stVencAttr.u32Profile == 0) {
				pstVencChnAttr->stVencAttr.u32Profile = 1;

				CVI_VENC_INFO("H.264 base not support BIPREDB, change to main\n");
			}
		}

		if ((pstVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264QPMAP) ||
			(pstVencChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265QPMAP)) {
			if (pstGopAttr->enGopMode == VENC_GOPMODE_ADVSMARTP) {
				pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

				CVI_VENC_INFO("advsmartp not support QPMAP, so change gopmode to smartp!\n");
			}
		}
	}

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetRefParam(
		chnInputCfg * pIc,
		VENC_CHN VencChn)
{
	CVI_S32 s32Ret;
	VENC_REF_PARAM_S stRefParam, *pstRefParam = &stRefParam;

	s32Ret = CVI_VENC_GetRefParam(VencChn, pstRefParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetRefParam failed!\n");
		return CVI_FAILURE;
	}

	if (pIc->tempLayer == 2) {
		pstRefParam->u32Base = 1;
		pstRefParam->u32Enhance = 1;
		pstRefParam->bEnablePred = CVI_TRUE;
	} else if (pIc->tempLayer == 3) {
		pstRefParam->u32Base = 2;
		pstRefParam->u32Enhance = 1;
		pstRefParam->bEnablePred = CVI_TRUE;
	} else {
		pstRefParam->u32Base = 0;
		pstRefParam->u32Enhance = 0;
		pstRefParam->bEnablePred = CVI_TRUE;
	}

	s32Ret = CVI_VENC_SetRefParam(VencChn, pstRefParam);

	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetRefParam failed!\n");
		return CVI_FAILURE;
	}
	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetCuPrediction(
		chnInputCfg * pIc,
		VENC_CHN VencChn)
{
	CVI_S32 s32Ret;
	VENC_CU_PREDICTION_S stCuPrediction, *pstCuPrediction = &stCuPrediction;

	s32Ret = CVI_VENC_GetCuPrediction(VencChn, pstCuPrediction);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetCuPrediction, 0x%X\n", s32Ret);
		return s32Ret;
	}

	pstCuPrediction->u32IntraCost = pIc->u32IntraCost;

	s32Ret = CVI_VENC_SetCuPrediction(VencChn, pstCuPrediction);

	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetCuPrediction, 0x%X\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetRcParam(
		chnInputCfg * pIc,
		VENC_CHN VencChn)
{
	CVI_S32 s32Ret;
	VENC_RC_PARAM_S stRcParam, *pstRcParam = &stRcParam;

	s32Ret = CVI_VENC_GetRcParam(VencChn, pstRcParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetRcParam, 0x%X\n", s32Ret);
		return s32Ret;
	}

	pstRcParam->u32ThrdLv = pIc->u32ThrdLv;
	pstRcParam->bBgEnhanceEn = pIc->bBgEnhanceEn;
	pstRcParam->s32BgDeltaQp = pIc->s32BgDeltaQp;
	pstRcParam->u32RowQpDelta = pIc->u32RowQpDelta;
	pstRcParam->s32FirstFrameStartQp = pIc->firstFrmstartQp;
	pstRcParam->s32InitialDelay = pIc->initialDelay;

	if (!strcmp(pIc->codec, "264") && pIc->rcMode == SAMPLE_RC_CBR) {
		pstRcParam->stParamH264Cbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH264Cbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH264Cbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH264Cbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH264Cbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH264Cbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH264Cbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	} else if (!strcmp(pIc->codec, "265") && pIc->rcMode == SAMPLE_RC_CBR) {
		pstRcParam->stParamH265Cbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH265Cbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH265Cbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH265Cbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH265Cbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH265Cbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH265Cbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	}  else if (!strcmp(pIc->codec, "264") && pIc->rcMode == SAMPLE_RC_VBR) {
		pstRcParam->stParamH264Vbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH264Vbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH264Vbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH264Vbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH264Vbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH264Vbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH264Vbr.s32ChangePos = pIc->s32ChangePos;
		pstRcParam->stParamH264Vbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	}  else if (!strcmp(pIc->codec, "265") && pIc->rcMode == SAMPLE_RC_VBR) {
		pstRcParam->stParamH265Vbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH265Vbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH265Vbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH265Vbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH265Vbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH265Vbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH265Vbr.s32ChangePos = pIc->s32ChangePos;
		pstRcParam->stParamH265Vbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	}  else if (!strcmp(pIc->codec, "264") && pIc->rcMode == SAMPLE_RC_AVBR) {
		pstRcParam->stParamH264AVbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH264AVbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH264AVbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH264AVbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH264AVbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH264AVbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH264AVbr.s32ChangePos = pIc->s32ChangePos;
		pstRcParam->stParamH264AVbr.s32MinStillPercent = pIc->s32MinStillPercent;
		pstRcParam->stParamH264AVbr.u32MaxStillQP = pIc->u32MaxStillQP;
		pstRcParam->stParamH264AVbr.u32MotionSensitivity = pIc->u32MotionSensitivity;
		pstRcParam->stParamH264AVbr.s32AvbrFrmLostOpen = pIc->s32AvbrFrmLostOpen;
		pstRcParam->stParamH264AVbr.s32AvbrFrmGap = pIc->s32AvbrFrmGap;
		pstRcParam->stParamH264AVbr.s32AvbrPureStillThr = pIc->s32AvbrPureStillThr;
		pstRcParam->stParamH264AVbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	}  else if (!strcmp(pIc->codec, "265") && pIc->rcMode == SAMPLE_RC_AVBR) {
		pstRcParam->stParamH265AVbr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH265AVbr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH265AVbr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH265AVbr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH265AVbr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH265AVbr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH265AVbr.s32ChangePos = pIc->s32ChangePos;
		pstRcParam->stParamH265AVbr.s32MinStillPercent = pIc->s32MinStillPercent;
		pstRcParam->stParamH265AVbr.u32MaxStillQP = pIc->u32MaxStillQP;
		pstRcParam->stParamH265AVbr.u32MotionSensitivity = pIc->u32MotionSensitivity;
		pstRcParam->stParamH265AVbr.s32AvbrFrmLostOpen = pIc->s32AvbrFrmLostOpen;
		pstRcParam->stParamH265AVbr.s32AvbrFrmGap = pIc->s32AvbrFrmGap;
		pstRcParam->stParamH265AVbr.s32AvbrPureStillThr = pIc->s32AvbrPureStillThr;
		pstRcParam->stParamH265AVbr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	} else if (!strcmp(pIc->codec, "264") && pIc->rcMode == SAMPLE_RC_UBR) {
		pstRcParam->stParamH264Ubr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH264Ubr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH264Ubr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH264Ubr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH264Ubr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH264Ubr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH264Ubr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	} else if (!strcmp(pIc->codec, "265") && pIc->rcMode == SAMPLE_RC_UBR) {
		pstRcParam->stParamH265Ubr.u32MaxIprop = pIc->maxIprop;
		pstRcParam->stParamH265Ubr.u32MinIprop = pIc->minIprop;
		pstRcParam->stParamH265Ubr.u32MaxIQp = pIc->maxIqp;
		pstRcParam->stParamH265Ubr.u32MinIQp = pIc->minIqp;
		pstRcParam->stParamH265Ubr.u32MaxQp = pIc->maxQp;
		pstRcParam->stParamH265Ubr.u32MinQp = pIc->minQp;
		pstRcParam->stParamH265Ubr.s32MaxReEncodeTimes = pIc->s32MaxReEncodeTimes;
	}

	s32Ret = CVI_VENC_SetRcParam(VencChn, pstRcParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetRcParam, 0x%X\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetFrameLost(
	chnInputCfg * pIc,
	VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_FRAMELOST_S stFL, *pstFL = &stFL;

	s32Ret = CVI_VENC_GetFrameLostStrategy(VencChn, pstFL);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetFrameLostStrategy failed!\n");
		return CVI_FAILURE;
	}

	pstFL->bFrmLostOpen = (pIc->frameLost) == 1 ? CVI_TRUE : CVI_FALSE;
	pstFL->enFrmLostMode = FRMLOST_PSKIP;
	pstFL->u32EncFrmGaps = pIc->frameLostGap;
	pstFL->u32FrmLostBpsThr = pIc->frameLostBspThr;

	s32Ret = CVI_VENC_SetFrameLostStrategy(VencChn, pstFL);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetFrameLostStrategy failed!\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VENC_SetSuperFrame(
	chnInputCfg * pIc,
	VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_SUPERFRAME_CFG_S stsf, *pstsf = &stsf;

	s32Ret = CVI_VENC_GetSuperFrameStrategy(VencChn, pstsf);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetSuperFrameStrategy failed!\n");
		return CVI_FAILURE;
	}

	pstsf->enSuperFrmMode = (pIc->enSuperFrmMode)
		? SUPERFRM_REENCODE_IDR
		: SUPERFRM_NONE;
	pstsf->u32SuperIFrmBitsThr = pIc->u32SuperIFrmBitsThr;
	pstsf->u32SuperPFrmBitsThr = pIc->u32SuperPFrmBitsThr;

	s32Ret = CVI_VENC_SetSuperFrameStrategy(VencChn, pstsf);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetSuperFrameStrategy failed!\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetModParam(const commonInputCfg *pCic)
{
	CVI_S32 s32Ret;
	VB_SOURCE_E eVbSource = pCic->vbMode;

	switch (eVbSource) {
	case VB_SOURCE_COMMON:
		CVI_VENC_TRACE("vbMode VB_SOURCE_COMMON\n");
		break;
	case VB_SOURCE_MODULE:
		CVI_VENC_TRACE("vbMode VB_SOURCE_MODULE\n");
		break;
	case VB_SOURCE_PRIVATE:
		CVI_VENC_TRACE("vbMode VB_SOURCE_PRIVATE\n");
		break;
	case VB_SOURCE_USER:
		CVI_VENC_TRACE("vbMode VB_SOURCE_USER\n");
		break;
	default:
		CVI_VENC_WARN("Invalid VB mode %d. Force to Priavte Mode.\n", eVbSource);
		eVbSource = VB_SOURCE_PRIVATE;
		CVI_VENC_TRACE("vbMode VB_SOURCE_PRIVATE\n");
		break;
	}

	//vb buffer mode only support non jpeg mode
	for (VENC_MODTYPE_E modtype = MODTYPE_H264E; modtype <= MODTYPE_JPEGE; modtype++) {
		VENC_PARAM_MOD_S stModParam;

		stModParam.enVencModType = modtype;
		s32Ret = CVI_VENC_GetModParam(&stModParam);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_GetModParam type %d failure\n", modtype);
			return CVI_FAILURE;
		}

		switch (modtype) {
		case MODTYPE_H264E:
			stModParam.stH264eModParam.enH264eVBSource = eVbSource;
			stModParam.stH264eModParam.bSingleEsBuf = (pCic->bSingleEsBuf_h264e ? true : false);
			stModParam.stH264eModParam.u32SingleEsBufSize = pCic->singleEsBufSize_h264e;
			break;
		case MODTYPE_H265E:
			stModParam.stH265eModParam.enH265eVBSource = eVbSource;
			stModParam.stH265eModParam.u32UserDataMaxLen = 3072;
			stModParam.stH265eModParam.bSingleEsBuf = (pCic->bSingleEsBuf_h265e ? true : false);
			stModParam.stH265eModParam.u32SingleEsBufSize = pCic->singleEsBufSize_h265e;
			stModParam.stH265eModParam.enRefreshType = pCic->h265RefreshType;
			break;
		case MODTYPE_JPEGE:
			stModParam.stJpegeModParam.bSingleEsBuf = (pCic->bSingleEsBuf_jpege ? true : false);
			stModParam.stJpegeModParam.u32SingleEsBufSize = pCic->singleEsBufSize_jpege;
			switch (pCic->jpegMarkerOrder) {
			case 2:
				stModParam.stJpegeModParam.enJpegeFormat = JPEGE_FORMAT_CUSTOM;
				stModParam.stJpegeModParam.JpegMarkerOrder[0] = JPEGE_MARKER_SOI;
				stModParam.stJpegeModParam.JpegMarkerOrder[1] = JPEGE_MARKER_JFIF;
				stModParam.stJpegeModParam.JpegMarkerOrder[2] = JPEGE_MARKER_FRAME_INDEX;
				stModParam.stJpegeModParam.JpegMarkerOrder[3] = JPEGE_MARKER_USER_DATA;
				stModParam.stJpegeModParam.JpegMarkerOrder[4] = JPEGE_MARKER_DRI_OPT;
				stModParam.stJpegeModParam.JpegMarkerOrder[5] = JPEGE_MARKER_DQT;
				stModParam.stJpegeModParam.JpegMarkerOrder[6] = JPEGE_MARKER_DHT;
				stModParam.stJpegeModParam.JpegMarkerOrder[7] = JPEGE_MARKER_SOF0;
				stModParam.stJpegeModParam.JpegMarkerOrder[8] = JPEGE_MARKER_BUTT;
				break;
			case 1:
				stModParam.stJpegeModParam.enJpegeFormat = JPEGE_FORMAT_TYPE_1;
				break;
			case 0:
			default:
				stModParam.stJpegeModParam.enJpegeFormat = JPEGE_FORMAT_DEFAULT;
				break;
			}
			break;
		default:
			CVI_VENC_ERR("SAMPLE_COMM_VENC_SetModParam invalid type %d failure\n", modtype);
			return CVI_FAILURE;
		}

		s32Ret = CVI_VENC_SetModParam(&stModParam);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VENC_ERR("CVI_VENC_SetModParam type %d failure\n", modtype);
			return CVI_FAILURE;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_SetJpegParam(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_JPEG_PARAM_S stJpegParam, *pstJpegParam = &stJpegParam;
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VENC_GetJpegParam(VencChn, pstJpegParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetJpegParam\n");
		return CVI_FAILURE;
	}

	pstJpegParam->u32Qfactor = pIc->quality;
	pstJpegParam->u32MCUPerECS = pIc->MCUPerECS;

	s32Ret = CVI_VENC_SetJpegParam(VencChn, pstJpegParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetJpegParam fail\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_InitVBPool(vencChnCtx *pvecc, VENC_CHN VencChnIdx)
{

	VB_POOL_CONFIG_S stVbPoolCfg;
	CVI_U32 u32BlkSize;
	SIZE_S stSize;

	if (pvecc->stSize.u32Width || pvecc->stSize.u32Height) {
		stSize.u32Width = pvecc->stSize.u32Width;
		stSize.u32Height = pvecc->stSize.u32Height;
	} else {
		CVI_VENC_ERR("Invalid width or height[%d][%d]\n",
										pvecc->stSize.u32Width,
										pvecc->stSize.u32Height);
		return CVI_FAILURE;
	}

	//create venc own vbpool
	memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
	u32BlkSize = COMMON_GetVencFrameBufferSize(pvecc->enPayLoad,
					stSize.u32Width,
					stSize.u32Height);
	if (u32BlkSize == 0) {
		CVI_VENC_ERR("Invalid type for VENC to calculate frame buffer size\n");
		return CVI_FAILURE;
	}

	stVbPoolCfg.u32BlkSize	= u32BlkSize;
	stVbPoolCfg.u32BlkCnt	= 3;
	stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
	gVencPicVbPool[VencChnIdx] = CVI_VB_CreatePool(&stVbPoolCfg);
	if (gVencPicVbPool[VencChnIdx] == VB_INVALID_POOLID) {
		CVI_VENC_ERR("VencChnIdx[%d]\n", VencChnIdx);
		return CVI_FAILURE;
	}

	CVI_VENC_TRACE("CVI_VB_CreatePool : id:%d, u32BlkSize=0x%x, u32BlkCnt=%d chn[%d]\n",
		gVencPicVbPool[VencChnIdx], stVbPoolCfg.u32BlkSize, stVbPoolCfg.u32BlkCnt, VencChnIdx);



	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VENC_SetH264Trans(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H264_TRANS_S h264Trans = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH264Trans(VencChn, &h264Trans);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH264Trans, %d\n", s32Ret);
		return s32Ret;
	}

	h264Trans.chroma_qp_index_offset = pIc->h264ChromaQpOffset;
	s32Ret = CVI_VENC_SetH264Trans(VencChn, &h264Trans);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH264Trans, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH265Trans(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H265_TRANS_S h265Trans = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH265Trans(VencChn, &h265Trans);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH265Trans, %d\n", s32Ret);
		return s32Ret;
	}

	h265Trans.cb_qp_offset = pIc->h265CbQpOffset;
	h265Trans.cr_qp_offset = pIc->h265CrQpOffset;
	s32Ret = CVI_VENC_SetH265Trans(VencChn, &h265Trans);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH265Trans, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH264Vui(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H264_VUI_S h264Vui = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH264Vui(VencChn, &h264Vui);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH264Vui, %d\n", s32Ret);
		return s32Ret;
	}

	h264Vui.stVuiAspectRatio.aspect_ratio_info_present_flag = pIc->aspectRatioInfoPresentFlag;
	if (h264Vui.stVuiAspectRatio.aspect_ratio_info_present_flag) {
		h264Vui.stVuiAspectRatio.aspect_ratio_idc = pIc->aspectRatioIdc;
		h264Vui.stVuiAspectRatio.sar_width = pIc->sarWidth;
		h264Vui.stVuiAspectRatio.sar_height = pIc->sarHeight;
	}

	h264Vui.stVuiAspectRatio.overscan_info_present_flag = pIc->overscanInfoPresentFlag;
	if (h264Vui.stVuiAspectRatio.overscan_info_present_flag) {
		h264Vui.stVuiAspectRatio.overscan_appropriate_flag = pIc->overscanAppropriateFlag;
	}

	h264Vui.stVuiTimeInfo.timing_info_present_flag = pIc->timingInfoPresentFlag;
	if (h264Vui.stVuiTimeInfo.timing_info_present_flag) {
		h264Vui.stVuiTimeInfo.fixed_frame_rate_flag = pIc->fixedFrameRateFlag;
		h264Vui.stVuiTimeInfo.num_units_in_tick = pIc->numUnitsInTick;
		h264Vui.stVuiTimeInfo.time_scale = pIc->timeScale;
	}

	h264Vui.stVuiVideoSignal.video_signal_type_present_flag = pIc->videoSignalTypePresentFlag;
	if (h264Vui.stVuiVideoSignal.video_signal_type_present_flag) {
		h264Vui.stVuiVideoSignal.video_format = pIc->videoFormat;
		h264Vui.stVuiVideoSignal.video_full_range_flag = pIc->videoFullRangeFlag;
		h264Vui.stVuiVideoSignal.colour_description_present_flag = pIc->colourDescriptionPresentFlag;
		if (h264Vui.stVuiVideoSignal.colour_description_present_flag) {
			h264Vui.stVuiVideoSignal.colour_primaries = pIc->colourPrimaries;
			h264Vui.stVuiVideoSignal.transfer_characteristics = pIc->transferCharacteristics;
			h264Vui.stVuiVideoSignal.matrix_coefficients = pIc->matrixCoefficients;
		}
	}

	s32Ret = CVI_VENC_SetH264Vui(VencChn, &h264Vui);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH264Vui, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH265Vui(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H265_VUI_S h265Vui = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH265Vui(VencChn, &h265Vui);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH265Vui, %d\n", s32Ret);
		return s32Ret;
	}

	h265Vui.stVuiAspectRatio.aspect_ratio_info_present_flag = pIc->aspectRatioInfoPresentFlag;
	if (h265Vui.stVuiAspectRatio.aspect_ratio_info_present_flag) {
		h265Vui.stVuiAspectRatio.aspect_ratio_idc = pIc->aspectRatioIdc;
		h265Vui.stVuiAspectRatio.sar_width = pIc->sarWidth;
		h265Vui.stVuiAspectRatio.sar_height = pIc->sarHeight;
	}

	h265Vui.stVuiAspectRatio.overscan_info_present_flag = pIc->overscanInfoPresentFlag;
	if (h265Vui.stVuiAspectRatio.overscan_info_present_flag) {
		h265Vui.stVuiAspectRatio.overscan_appropriate_flag = pIc->overscanAppropriateFlag;
	}

	h265Vui.stVuiTimeInfo.timing_info_present_flag = pIc->timingInfoPresentFlag;
	if (h265Vui.stVuiTimeInfo.timing_info_present_flag) {
		h265Vui.stVuiTimeInfo.num_units_in_tick = pIc->numUnitsInTick;
		h265Vui.stVuiTimeInfo.time_scale = pIc->timeScale;
	}

	h265Vui.stVuiVideoSignal.video_signal_type_present_flag = pIc->videoSignalTypePresentFlag;
	if (h265Vui.stVuiVideoSignal.video_signal_type_present_flag) {
		h265Vui.stVuiVideoSignal.video_format = pIc->videoFormat;
		h265Vui.stVuiVideoSignal.video_full_range_flag = pIc->videoFullRangeFlag;
		h265Vui.stVuiVideoSignal.colour_description_present_flag = pIc->colourDescriptionPresentFlag;
		if (h265Vui.stVuiVideoSignal.colour_description_present_flag) {
			h265Vui.stVuiVideoSignal.colour_primaries = pIc->colourPrimaries;
			h265Vui.stVuiVideoSignal.transfer_characteristics = pIc->transferCharacteristics;
			h265Vui.stVuiVideoSignal.matrix_coefficients = pIc->matrixCoefficients;
		}
	}

	s32Ret = CVI_VENC_SetH265Vui(VencChn, &h265Vui);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH265Vui, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetChnParam(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_CHN_PARAM_S stChnParam, *pstChnParam = &stChnParam;
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VENC_GetChnParam(VencChn, pstChnParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetJpegParam\n");
		return CVI_FAILURE;
	}

	pstChnParam->stCropCfg.bEnable = (pIc->posX || pIc->posY);
	pstChnParam->stCropCfg.stRect.s32X = pIc->posX;
	pstChnParam->stCropCfg.stRect.s32Y = pIc->posY;
	pstChnParam->stCropCfg.stRect.u32Width = pIc->width;
	pstChnParam->stCropCfg.stRect.u32Height = pIc->height;
	CVI_VENC_TRACE("s32X = %d, s32Y = %d\n",
			pstChnParam->stCropCfg.stRect.s32X,
			pstChnParam->stCropCfg.stRect.s32Y);

	pstChnParam->stFrameRate.s32SrcFrmRate = pIc->srcFramerate;
	pstChnParam->stFrameRate.s32DstFrmRate = pIc->framerate;

	s32Ret = CVI_VENC_SetChnParam(VencChn, pstChnParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetChnParam fail\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH264SliceSplit(
	chnInputCfg *pIc,
	VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_H264_SLICE_SPLIT_S stH264Split, *pstH264Split = &stH264Split;

	s32Ret = CVI_VENC_GetH264SliceSplit(VencChn, pstH264Split);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetH264SliceSplit failed!\n");
		return CVI_FAILURE;
	}

	if (pIc->u32SliceCnt > 1) {
		pstH264Split->bSplitEnable = 1;
		pstH264Split->u32MbLineNum = ((pIc->height + 15) / 16 + (pIc->u32SliceCnt - 1)) / pIc->u32SliceCnt;
	} else {
		pstH264Split->bSplitEnable = 0;
	}

	s32Ret = CVI_VENC_SetH264SliceSplit(VencChn, pstH264Split);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetH264SliceSplit failed!\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH265SliceSplit(
	chnInputCfg *pIc,
	VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_H265_SLICE_SPLIT_S stH265Split, *pstH265Split = &stH265Split;

	s32Ret = CVI_VENC_GetH265SliceSplit(VencChn, pstH265Split);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetH265SliceSplit failed!\n");
		return CVI_FAILURE;
	}

	if (pIc->u32SliceCnt > 1) {
		pstH265Split->bSplitEnable = 1;
		pstH265Split->u32LcuLineNum = ((pIc->height + 63) / 64 + (pIc->u32SliceCnt - 1)) / pIc->u32SliceCnt;
	} else {
		pstH265Split->bSplitEnable = 0;
	}

	s32Ret = CVI_VENC_SetH265SliceSplit(VencChn, pstH265Split);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetH265SliceSplit failed!\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH264Dblk(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H264_DBLK_S h264Dblk = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH264Dblk(VencChn, &h264Dblk);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH264Dblk, %d\n", s32Ret);
		return s32Ret;
	}

	h264Dblk.disable_deblocking_filter_idc = pIc->bDisableDeblk;
	h264Dblk.slice_alpha_c0_offset_div2 = pIc->alphaOffset;
	h264Dblk.slice_beta_offset_div2 = pIc->betaOffset;

	s32Ret = CVI_VENC_SetH264Dblk(VencChn, &h264Dblk);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH264Dblk, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH265Dblk(chnInputCfg *pIc, VENC_CHN VencChn)
{
	VENC_H265_DBLK_S h265Dblk = { 0 };
	CVI_S32 s32Ret = CVI_FAILURE;

	s32Ret = CVI_VENC_GetH265Dblk(VencChn, &h265Dblk);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_GetH265Dblk, %d\n", s32Ret);
		return s32Ret;
	}

	h265Dblk.slice_deblocking_filter_disabled_flag = pIc->bDisableDeblk;
	h265Dblk.slice_beta_offset_div2 = pIc->betaOffset;
	h265Dblk.slice_tc_offset_div2 = pIc->alphaOffset;

	s32Ret = CVI_VENC_SetH265Dblk(VencChn, &h265Dblk);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_SetH265Dblk, %d\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_SetH264IntraPred(
	chnInputCfg *pIc,
	VENC_CHN VencChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VENC_H264_INTRA_PRED_S stH264IntraPred, *pstH264IntraPred = &stH264IntraPred;

	s32Ret = CVI_VENC_GetH264IntraPred(VencChn, pstH264IntraPred);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("GetH264IntraPred failed!\n");
		return CVI_FAILURE;
	}

	pstH264IntraPred->constrained_intra_pred_flag = pIc->bIntraPred;

	s32Ret = CVI_VENC_SetH264IntraPred(VencChn, pstH264IntraPred);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SetH264IntraPred failed!\n");
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VENC_Start(
		chnInputCfg * pIc,
		VENC_CHN VencChn,
		PAYLOAD_TYPE_E enType,
		PIC_SIZE_E enSize,
		SAMPLE_RC_E enRcMode,
		CVI_U32 u32Profile,
		CVI_BOOL bRcnRefShareBuf,
		VENC_GOP_ATTR_S *pstGopAttr)
{
	CVI_S32 s32Ret;
	VENC_RECV_PIC_PARAM_S stRecvParam;

	s32Ret = SAMPLE_COMM_VENC_Create(
			pIc, VencChn, enType, enSize, enRcMode,
			u32Profile, bRcnRefShareBuf, pstGopAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("SAMPLE_COMM_VENC_Create failed with %d\n", s32Ret);
		return CVI_FAILURE;
	}

	if (pIc->bind_mode == VENC_BIND_VI) {
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;

		CVI_VENC_BIND("VI_Bind_VENC, VencChn = %d\n", VencChn);
		SAMPLE_COMM_VI_Bind_VENC(ViPipe, ViChn, VencChn);
	} else if (pIc->bind_mode == VENC_BIND_VPSS) {
		CVI_VENC_BIND("VPSS_Bind_VENC, vpss Grp = %d, Chn = %d, VencChn = %d\n",
				pIc->vpssGrp, pIc->vpssChn, VencChn);
		SAMPLE_COMM_VPSS_Bind_VENC(pIc->vpssGrp, pIc->vpssChn, VencChn);
	}

	stRecvParam.s32RecvPicNum = pIc->num_frames;
	s32Ret = CVI_VENC_StartRecvFrame(VencChn, &stRecvParam);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VENC_ERR("CVI_VENC_StartRecvPic failed with %d\n", s32Ret);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}
