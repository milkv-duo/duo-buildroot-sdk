/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vdec.h
 * Description:
 *   Common video decode definitions.
 */

#ifndef __CVI_VDEC_H__
#define __CVI_VDEC_H__

#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>
#include "cvi_comm_vb.h"
#include <linux/cvi_comm_vdec.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

CVI_S32 CVI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
CVI_S32 CVI_VDEC_DestroyChn(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);
CVI_S32 CVI_VDEC_SetChnAttr(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);

CVI_S32 CVI_VDEC_StartRecvStream(VDEC_CHN VdChn);
CVI_S32 CVI_VDEC_StopRecvStream(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_QueryStatus(VDEC_CHN VdChn, VDEC_CHN_STATUS_S *pstStatus);

CVI_S32 CVI_VDEC_GetFd(VDEC_CHN VdChn);
CVI_S32 CVI_VDEC_CloseFd(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_ResetChn(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_SetChnParam(VDEC_CHN VdChn, const VDEC_CHN_PARAM_S *pstParam);
CVI_S32 CVI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S *pstParam);

CVI_S32 CVI_VDEC_SetProtocolParam(VDEC_CHN VdChn, const VDEC_PRTCL_PARAM_S *pstParam);
CVI_S32 CVI_VDEC_GetProtocolParam(VDEC_CHN VdChn, VDEC_PRTCL_PARAM_S *pstParam);

/* s32MilliSec: -1 is block,0 is no block,other positive number is timeout */
CVI_S32 CVI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, CVI_S32 s32MilliSec);

CVI_S32 CVI_VDEC_GetFrame(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo, CVI_S32 s32MilliSec);
CVI_S32 CVI_VDEC_ReleaseFrame(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

CVI_S32 CVI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, CVI_S32 s32MilliSec);
CVI_S32 CVI_VDEC_ReleaseUserData(VDEC_CHN VdChn, const VDEC_USERDATA_S *pstUserData);

CVI_S32 CVI_VDEC_SetUserPic(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstUsrPic);
CVI_S32 CVI_VDEC_EnableUserPic(VDEC_CHN VdChn, CVI_BOOL bInstant);
CVI_S32 CVI_VDEC_DisableUserPic(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode);
CVI_S32 CVI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode);

CVI_S32 CVI_VDEC_SetRotation(VDEC_CHN VdChn, ROTATION_E enRotation);
CVI_S32 CVI_VDEC_GetRotation(VDEC_CHN VdChn, ROTATION_E *penRotation);

CVI_S32 CVI_VDEC_AttachVbPool(VDEC_CHN VdChn, const VDEC_CHN_POOL_S *pstPool);
CVI_S32 CVI_VDEC_DetachVbPool(VDEC_CHN VdChn);

CVI_S32 CVI_VDEC_SetUserDataAttr(VDEC_CHN VdChn, const VDEC_USER_DATA_ATTR_S *pstUserDataAttr);
CVI_S32 CVI_VDEC_GetUserDataAttr(VDEC_CHN VdChn, VDEC_USER_DATA_ATTR_S *pstUserDataAttr);

CVI_S32 CVI_VDEC_SetModParam(const VDEC_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __CVI_VDEC_H__ */


