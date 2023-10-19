/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vo.h
 * Description:
 *   MMF Programe Interface for video output management moudle
 */

#ifndef __CVI_VO_H__
#define __CVI_VO_H__

#include "cvi_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* Device Relative Settings */

CVI_S32 CVI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
CVI_S32 CVI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
/* CVI_VO_I80Init: init i80 per instructions
 *
 * @param VoDev: the chn which has vb to be released
 * @param pi80Instr: i80 init instructions
 * @param size: number of pi80Instr
 * @return: status of operation. CVI_SUCCESS if OK.
 */
CVI_S32 CVI_VO_I80Init(VO_DEV VoDev, const VO_I80_INSTR_S *pi80Instr, CVI_U8 size);

CVI_S32 CVI_VO_Enable(VO_DEV VoDev);
CVI_S32 CVI_VO_Disable(VO_DEV VoDev);

CVI_S32 CVI_VO_CloseFd(void);
/* Video Relative Settings */
CVI_S32 CVI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
CVI_S32 CVI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

CVI_S32 CVI_VO_EnableVideoLayer(VO_LAYER VoLayer);
CVI_S32 CVI_VO_DisableVideoLayer(VO_LAYER VoLayer);

CVI_S32 CVI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, CVI_U32 u32Priority);
CVI_S32 CVI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, CVI_U32 *pu32Priority);

CVI_S32 CVI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
CVI_S32 CVI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

CVI_S32 CVI_VO_GetLayerProcAmpCtrl(VO_LAYER VoLayer, PROC_AMP_E type, PROC_AMP_CTRL_S *ctrl);
CVI_S32 CVI_VO_GetLayerProcAmp(VO_LAYER VoLayer, PROC_AMP_E type, CVI_S32 *value);
CVI_S32 CVI_VO_SetLayerProcAmp(VO_LAYER VoLayer, PROC_AMP_E type, CVI_S32 value);

CVI_S32 CVI_VO_BatchBegin(VO_LAYER VoLayer);
CVI_S32 CVI_VO_BatchEnd(VO_LAYER VoLayer);


/* Display relative operations */
CVI_S32 CVI_VO_SetPlayToleration(VO_LAYER VoLayer, CVI_U32 u32Toleration);
CVI_S32 CVI_VO_GetPlayToleration(VO_LAYER VoLayer, CVI_U32 *pu32Toleration);

CVI_S32 CVI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);
CVI_S32 CVI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, const VIDEO_FRAME_INFO_S *pstVideoFrame);

CVI_S32 CVI_VO_SetDisplayBufLen(VO_LAYER VoLayer, CVI_U32 u32BufLen);
CVI_S32 CVI_VO_GetDisplayBufLen(VO_LAYER VoLayer, CVI_U32 *pu32BufLen);

/* Channel Relative Operations */
CVI_S32 CVI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

CVI_S32 CVI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);
CVI_S32 CVI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

CVI_S32 CVI_VO_SetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);
CVI_S32 CVI_VO_GetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);


CVI_S32 CVI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, CVI_S32 s32ChnFrmRate);
CVI_S32 CVI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, CVI_S32 *ps32ChnFrmRate);

CVI_S32 CVI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);
CVI_S32 CVI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstVideoFrame);

CVI_S32 CVI_VO_PauseChn(VO_LAYER VoLayer, VO_CHN VoChn);
CVI_S32 CVI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);

CVI_S32 CVI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
CVI_S32 CVI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

CVI_S32 CVI_VO_GetChnPTS(VO_LAYER VoLayer, VO_CHN VoChn, CVI_U64 *pu64ChnPTS);
CVI_S32 CVI_VO_QueryChnStatus(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

CVI_S32 CVI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);

CVI_S32 CVI_VO_ClearChnBuf(VO_LAYER VoLayer, VO_CHN VoChn, CVI_BOOL bClrAll);

CVI_S32 CVI_VO_SetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, CVI_U32 u32Threshold);
CVI_S32 CVI_VO_GetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, CVI_U32 *pu32Threshold);

CVI_S32 CVI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E enRotation);
CVI_S32 CVI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);

CVI_S32 CVI_VO_SetDevFrameRate(VO_DEV VoDev, CVI_U32 u32FrameRate);
CVI_S32 CVI_VO_GetDevFrameRate(VO_DEV VoDev, CVI_U32 *pu32FrameRate);

/* Module Parameter Settings */
CVI_S32 CVI_VO_SetVtth(VO_DEV VoDev, CVI_U32 u32Vtth);
CVI_S32 CVI_VO_GetVtth(VO_DEV VoDev, CVI_U32 *pu32Vtth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_VO_H__
