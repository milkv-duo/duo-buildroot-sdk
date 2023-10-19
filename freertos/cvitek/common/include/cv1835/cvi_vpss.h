/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vpss.h
 * Description:
 *   MMF Programe Interface for video processing moudle
 */

#ifndef __CVI_VPSS_H__
#define __CVI_VPSS_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_comm_vb.h"
#include "cvi_comm_vpss.h"
#include "cvi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Group Settings */
CVI_S32 CVI_VPSS_CreateGrp(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);
CVI_S32 CVI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);
VPSS_GRP CVI_VPSS_GetAvailableGrp(void);

CVI_S32 CVI_VPSS_StartGrp(VPSS_GRP VpssGrp);
CVI_S32 CVI_VPSS_StopGrp(VPSS_GRP VpssGrp);

CVI_S32 CVI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

CVI_S32 CVI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
CVI_S32 CVI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S *pstGrpAttr);

CVI_S32 CVI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp, const VPSS_CROP_INFO_S *pstCropInfo);
CVI_S32 CVI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S *pstCropInfo);

CVI_S32 CVI_VPSS_SendFrame(VPSS_GRP VpssGrp, const VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);
CVI_S32 CVI_VPSS_SendChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn
	, const VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);

CVI_S32 CVI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVideoFrame);
CVI_S32 CVI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp, const VIDEO_FRAME_INFO_S *pstVideoFrame);

CVI_S32 CVI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
CVI_S32 CVI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

CVI_S32 CVI_VPSS_SetGrpDelay(VPSS_GRP VpssGrp, CVI_U32 u32Delay);
CVI_S32 CVI_VPSS_GetGrpDelay(VPSS_GRP VpssGrp, CVI_U32 *pu32Delay);

CVI_S32 CVI_VPSS_EnableUserFrameRateCtrl(VPSS_GRP VpssGrp);
CVI_S32 CVI_VPSS_DisableUserFrameRateCtrl(VPSS_GRP VpssGrp);

CVI_S32 CVI_VPSS_GetGrpProcAmpCtrl(VPSS_GRP VpssGrp, PROC_AMP_E type, PROC_AMP_CTRL_S *ctrl);
CVI_S32 CVI_VPSS_GetGrpProcAmp(VPSS_GRP VpssGrp, PROC_AMP_E type, CVI_S32 *value);
CVI_S32 CVI_VPSS_SetGrpProcAmp(VPSS_GRP VpssGrp, PROC_AMP_E type, CVI_S32 value);

/* CVI_VPSS_SetGrpParamfromBin: Apply the settings of scene from bin
 *
 * @param VpssGrp: the vpss grp to apply
 * @param scene: the scene of settings stored in bin to use
 * @return: result of the API
 */
CVI_S32 CVI_VPSS_SetGrpParamfromBin(VPSS_GRP VpssGrp, CVI_U8 scene);

/* Chn Settings */
CVI_S32 CVI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

CVI_S32 CVI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
CVI_S32 CVI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

CVI_S32 CVI_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CROP_INFO_S *pstCropInfo);
CVI_S32 CVI_VPSS_GetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CROP_INFO_S *pstCropInfo);

CVI_S32 CVI_VPSS_SetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E enRotation);
CVI_S32 CVI_VPSS_GetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E *penRotation);

CVI_S32 CVI_VPSS_SetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_LDC_ATTR_S *pstLDCAttr);
CVI_S32 CVI_VPSS_GetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LDC_ATTR_S *pstLDCAttr);

CVI_S32 CVI_VPSS_SetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const SPREAD_ATTR_S *pstSpreadAttr);
CVI_S32 CVI_VPSS_GetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SPREAD_ATTR_S *pstSpreadAttr);

CVI_S32 CVI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVideoFrame,
				 CVI_S32 s32MilliSec);
CVI_S32 CVI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_FRAME_INFO_S *pstVideoFrame);

CVI_S32 CVI_VPSS_TriggerSnapFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_U32 u32FrameCnt);

CVI_S32 CVI_VPSS_AttachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VB_POOL hVbPool);
CVI_S32 CVI_VPSS_DetachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

CVI_S32 CVI_VPSS_SetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_U32 u32Align);
CVI_S32 CVI_VPSS_GetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_U32 *pu32Align);

CVI_S32 CVI_VPSS_SetChnYRatio(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_FLOAT YRatio);
CVI_S32 CVI_VPSS_GetChnYRatio(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_FLOAT *pYRatio);

/* Set/get Scale coefficient level for VPSS*/
CVI_S32 CVI_VPSS_SetChnScaleCoefLevel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_SCALE_COEF_E enCoef);
CVI_S32 CVI_VPSS_GetChnScaleCoefLevel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_SCALE_COEF_E *penCoef);

/* ExtChn Settings */
CVI_S32 CVI_VPSS_SetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const FISHEYE_ATTR_S *pstFishEyeAttr);
CVI_S32 CVI_VPSS_GetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, FISHEYE_ATTR_S *pstFishEyeAttr);

CVI_S32 CVI_VPSS_SetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_U32 u32Align);
CVI_S32 CVI_VPSS_GetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CVI_U32 *pu32Align);

/* Module Param Settings */
CVI_S32 CVI_VPSS_SetModParam(const VPSS_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_VPSS_GetModParam(VPSS_MOD_PARAM_S *pstModParam);

CVI_S32 CVI_VPSS_GetChnFd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
CVI_S32 CVI_VPSS_CloseFd(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_VPSS_H__ */
