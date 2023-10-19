/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_venc.h
 * Description:
 *   Common video encode definitions.
 */

#ifndef __CVI_VENC_H__
#define __CVI_VENC_H__

#include "cvi_comm_video.h"
#include "cvi_comm_vb.h"
#include "cvi_comm_venc.h"
#include "cvi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

CVI_VOID cviGetMask(void);

CVI_S32 CVI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
CVI_S32 CVI_VENC_DestroyChn(VENC_CHN VeChn);
CVI_S32 CVI_VENC_ResetChn(VENC_CHN VeChn);

CVI_S32 CVI_VENC_StartRecvFrame(VENC_CHN VeChn, const VENC_RECV_PIC_PARAM_S *pstRecvParam);
CVI_S32 CVI_VENC_StopRecvFrame(VENC_CHN VeChn);

CVI_S32 CVI_VENC_QueryStatus(VENC_CHN VeChn, VENC_CHN_STATUS_S *pstStatus);

CVI_S32 CVI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstChnAttr);

CVI_S32 CVI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, CVI_S32 S32MilliSec);
CVI_S32 CVI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

CVI_S32 CVI_VENC_InsertUserData(VENC_CHN VeChn, CVI_U8 *pu8Data, CVI_U32 u32Len);

CVI_S32 CVI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, CVI_S32 S32MilliSec);
CVI_S32 CVI_VENC_SendFrameEx(VENC_CHN VeChn, const USER_FRAME_INFO_S *pstFrame, CVI_S32 S32MilliSec);

CVI_S32 CVI_VENC_RequestIDR(VENC_CHN VeChn, CVI_BOOL bInstant);

CVI_S32 CVI_VENC_GetFd(VENC_CHN VeChn);
CVI_S32 CVI_VENC_CloseFd(VENC_CHN VeChn);

CVI_S32 CVI_VENC_SetRoiAttr(VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr);
CVI_S32 CVI_VENC_GetRoiAttr(VENC_CHN VeChn, CVI_U32 u32Index, VENC_ROI_ATTR_S *pstRoiAttr);

CVI_S32 CVI_VENC_GetRoiAttrEx(VENC_CHN VeChn, CVI_U32 u32Index, VENC_ROI_ATTR_EX_S *pstRoiAttrEx);
CVI_S32 CVI_VENC_SetRoiAttrEx(VENC_CHN VeChn, const VENC_ROI_ATTR_EX_S *pstRoiAttrEx);

CVI_S32 CVI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);
CVI_S32 CVI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);

CVI_S32 CVI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_H264_SLICE_SPLIT_S *pstSliceSplit);
CVI_S32 CVI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_H264_SLICE_SPLIT_S *pstSliceSplit);

CVI_S32 CVI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_H264_INTRA_PRED_S *pstH264IntraPred);
CVI_S32 CVI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_H264_INTRA_PRED_S *pstH264IntraPred);

CVI_S32 CVI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_H264_TRANS_S *pstH264Trans);
CVI_S32 CVI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_H264_TRANS_S *pstH264Trans);

CVI_S32 CVI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_H264_ENTROPY_S *pstH264EntropyEnc);
CVI_S32 CVI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_H264_ENTROPY_S *pstH264EntropyEnc);

CVI_S32 CVI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_H264_DBLK_S *pstH264Dblk);
CVI_S32 CVI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_H264_DBLK_S *pstH264Dblk);

CVI_S32 CVI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_H264_VUI_S *pstH264Vui);
CVI_S32 CVI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_H264_VUI_S *pstH264Vui);

CVI_S32 CVI_VENC_SetH265Vui(VENC_CHN VeChn, const VENC_H265_VUI_S *pstH265Vui);
CVI_S32 CVI_VENC_GetH265Vui(VENC_CHN VeChn, VENC_H265_VUI_S *pstH265Vui);

CVI_S32 CVI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_JPEG_PARAM_S *pstJpegParam);
CVI_S32 CVI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_JPEG_PARAM_S *pstJpegParam);

CVI_S32 CVI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_MJPEG_PARAM_S *pstMjpegParam);
CVI_S32 CVI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_MJPEG_PARAM_S *pstMjpegParam);

CVI_S32 CVI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);
CVI_S32 CVI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);

CVI_S32 CVI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_REF_PARAM_S *pstRefParam);
CVI_S32 CVI_VENC_GetRefParam(VENC_CHN VeChn, VENC_REF_PARAM_S *pstRefParam);

CVI_S32 CVI_VENC_SetJpegEncodeMode(VENC_CHN VeChn, const VENC_JPEG_ENCODE_MODE_E enJpegEncodeMode);
CVI_S32 CVI_VENC_GetJpegEncodeMode(VENC_CHN VeChn, VENC_JPEG_ENCODE_MODE_E *penJpegEncodeMode);

CVI_S32 CVI_VENC_EnableIDR(VENC_CHN VeChn, CVI_BOOL bEnableIDR);

CVI_S32 CVI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pstStreamBufInfo);

CVI_S32 CVI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_H265_SLICE_SPLIT_S *pstSliceSplit);
CVI_S32 CVI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_H265_SLICE_SPLIT_S *pstSliceSplit);

CVI_S32 CVI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_H265_PU_S *pstPredUnit);
CVI_S32 CVI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_H265_PU_S *pstPredUnit);

CVI_S32 CVI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_H265_TRANS_S *pstH265Trans);
CVI_S32 CVI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_H265_TRANS_S *pstH265Trans);

CVI_S32 CVI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_H265_ENTROPY_S *pstH265Entropy);
CVI_S32 CVI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_H265_ENTROPY_S *pstH265Entropy);

CVI_S32 CVI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_H265_DBLK_S *pstH265Dblk);
CVI_S32 CVI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_H265_DBLK_S *pstH265Dblk);

CVI_S32 CVI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_H265_SAO_S *pstH265Sao);
CVI_S32 CVI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_H265_SAO_S *pstH265Sao);

CVI_S32 CVI_VENC_SetFrameLostStrategy(VENC_CHN VeChn, const VENC_FRAMELOST_S *pstFrmLostParam);
CVI_S32 CVI_VENC_GetFrameLostStrategy(VENC_CHN VeChn, VENC_FRAMELOST_S *pstFrmLostParam);

CVI_S32 CVI_VENC_SetSuperFrameStrategy(VENC_CHN VeChn, const VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);
CVI_S32 CVI_VENC_GetSuperFrameStrategy(VENC_CHN VeChn, VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);

CVI_S32 CVI_VENC_SetIntraRefresh(VENC_CHN VeChn, const VENC_INTRA_REFRESH_S *pstIntraRefresh);
CVI_S32 CVI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_INTRA_REFRESH_S *pstIntraRefresh);

CVI_S32 CVI_VENC_GetSSERegion(VENC_CHN VeChn, CVI_U32 u32Index, VENC_SSE_CFG_S *pstSSECfg);
CVI_S32 CVI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S *pstSSECfg);

CVI_S32 CVI_VENC_SetChnParam(VENC_CHN VeChn, const VENC_CHN_PARAM_S *pstChnParam);
CVI_S32 CVI_VENC_GetChnParam(VENC_CHN VeChn, VENC_CHN_PARAM_S *pstChnParam);

CVI_S32 CVI_VENC_SetModParam(const VENC_PARAM_MOD_S *pstModParam);
CVI_S32 CVI_VENC_GetModParam(VENC_PARAM_MOD_S *pstModParam);

CVI_S32 CVI_VENC_GetForegroundProtect(VENC_CHN VeChn, VENC_FOREGROUND_PROTECT_S *pstForegroundProtect);
CVI_S32 CVI_VENC_SetForegroundProtect(VENC_CHN VeChn, const VENC_FOREGROUND_PROTECT_S *pstForegroundProtect);

CVI_S32 CVI_VENC_SetSceneMode(VENC_CHN VeChn, const VENC_SCENE_MODE_E enSceneMode);
CVI_S32 CVI_VENC_GetSceneMode(VENC_CHN VeChn, VENC_SCENE_MODE_E *penSceneMode);

CVI_S32 CVI_VENC_AttachVbPool(VENC_CHN VeChn, const VENC_CHN_POOL_S *pstPool);
CVI_S32 CVI_VENC_DetachVbPool(VENC_CHN VeChn);

CVI_S32 CVI_VENC_SetCuPrediction(VENC_CHN VeChn, const VENC_CU_PREDICTION_S *pstCuPrediction);
CVI_S32 CVI_VENC_GetCuPrediction(VENC_CHN VeChn, VENC_CU_PREDICTION_S *pstCuPrediction);

CVI_S32 CVI_VENC_SetSkipBias(VENC_CHN VeChn, const VENC_SKIP_BIAS_S *pstSkipBias);
CVI_S32 CVI_VENC_GetSkipBias(VENC_CHN VeChn, VENC_SKIP_BIAS_S *pstSkipBias);

CVI_S32 CVI_VENC_SetDeBreathEffect(VENC_CHN VeChn, const VENC_DEBREATHEFFECT_S *pstDeBreathEffect);
CVI_S32 CVI_VENC_GetDeBreathEffect(VENC_CHN VeChn, VENC_DEBREATHEFFECT_S *pstDeBreathEffect);

CVI_S32 CVI_VENC_SetHierarchicalQp(VENC_CHN VeChn, const VENC_HIERARCHICAL_QP_S *pstHierarchicalQp);
CVI_S32 CVI_VENC_GetHierarchicalQp(VENC_CHN VeChn, VENC_HIERARCHICAL_QP_S *pstHierarchicalQp);

CVI_S32 CVI_VENC_SetRcAdvParam(VENC_CHN VeChn, const VENC_RC_ADVPARAM_S *pstRcAdvParam);
CVI_S32 CVI_VENC_GetRcAdvParam(VENC_CHN VeChn, VENC_RC_ADVPARAM_S *pstRcAdvParam);

#define DEF_STAT_TIME		-1
#define DEF_GOP				30
#define DEF_IQP				32
#define DEF_PQP				32

#define DEF_264_GOP			60
#define DEF_264_MAXIQP		51
#define DEF_264_MINIQP		1
#define DEF_264_MAXQP		51
#define DEF_264_MINQP		1
#define DEF_264_MAXBITRATE	5000

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_VENC_H__ */
