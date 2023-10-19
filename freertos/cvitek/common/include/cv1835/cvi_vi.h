/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vi.h
 * Description:
 *   MMF Programe Interface for video input management moudle
 */

#ifndef __CVI_VI_H__
#define __CVI_VI_H__

#include "cvi_comm_video.h"
#include "cvi_comm_vi.h"
#include "cvi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* 1 for vi device */
CVI_S32 CVI_VI_SetDevNum(CVI_U32 devNum);
CVI_S32 CVI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
CVI_S32 CVI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

CVI_S32 CVI_VI_SetDevAttrEx(VI_DEV ViDev, const VI_DEV_ATTR_EX_S *pstDevAttrEx);
CVI_S32 CVI_VI_GetDevAttrEx(VI_DEV ViDev, VI_DEV_ATTR_EX_S *pstDevAttrEx);

CVI_S32 CVI_VI_SetVSSignalAttr(VI_DEV ViDev, const VI_VS_SIGNAL_ATTR_S *pstVSSignalAttr);
CVI_S32 CVI_VI_GetVSSignalAttr(VI_DEV ViDev, VI_VS_SIGNAL_ATTR_S *pstVSSignalAttr);
CVI_S32 CVI_VI_TriggerVSSignal(VI_DEV ViDev, CVI_BOOL bEnable);

CVI_S32 CVI_VI_EnableDev(VI_DEV ViDev);
CVI_S32 CVI_VI_DisableDev(VI_DEV ViDev);

CVI_S32 CVI_VI_SetMipiBindDev(VI_DEV ViDev, MIPI_DEV MipiDev);
CVI_S32 CVI_VI_GetMipiBindDev(VI_DEV ViDev, MIPI_DEV *pMipiDev);

CVI_S32 CVI_VI_SetDevBindPipe(VI_DEV ViDev, const VI_DEV_BIND_PIPE_S *pstDevBindPipe);
CVI_S32 CVI_VI_GetDevBindPipe(VI_DEV ViDev, VI_DEV_BIND_PIPE_S *pstDevBindPipe);

CVI_S32 CVI_VI_SetDevTimingAttr(VI_DEV ViDev, const VI_DEV_TIMING_ATTR_S *pstTimingAttr);
CVI_S32 CVI_VI_GetDevTimingAttr(VI_DEV ViDev, VI_DEV_TIMING_ATTR_S *pstTimingAttr);


/* 2 for vi pipe */
CVI_S32 CVI_VI_GetPipeCmpParam(VI_PIPE ViPipe, VI_CMP_PARAM_S *pCmpParam);

CVI_S32 CVI_VI_SetUserPic(VI_PIPE ViPipe, const VI_USERPIC_ATTR_S *pstUsrPic);
CVI_S32 CVI_VI_EnableUserPic(VI_PIPE ViPipe);
CVI_S32 CVI_VI_DisableUserPic(VI_PIPE ViPipe);

CVI_S32 CVI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
CVI_S32 CVI_VI_DestroyPipe(VI_PIPE ViPipe);

CVI_S32 CVI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
CVI_S32 CVI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

CVI_S32 CVI_VI_StartPipe(VI_PIPE ViPipe);
CVI_S32 CVI_VI_StopPipe(VI_PIPE ViPipe);

CVI_S32 CVI_VI_SetPipeCrop(VI_PIPE ViPipe, const CROP_INFO_S *pstCropInfo);
CVI_S32 CVI_VI_GetPipeCrop(VI_PIPE ViPipe, CROP_INFO_S *pstCropInfo);

CVI_S32 CVI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr);
CVI_S32 CVI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr);

CVI_S32 CVI_VI_SetPipeFrameSource(VI_PIPE ViPipe, const VI_PIPE_FRAME_SOURCE_E enSource);
CVI_S32 CVI_VI_GetPipeFrameSource(VI_PIPE ViPipe, VI_PIPE_FRAME_SOURCE_E *penSource);

CVI_S32 CVI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);
CVI_S32 CVI_VI_ReleasePipeFrame(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);

CVI_S32 CVI_VI_SendPipeYUV(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);
CVI_S32 CVI_VI_SendPipeRaw(CVI_U32 u32PipeNum, VI_PIPE PipeId[], const VIDEO_FRAME_INFO_S *pstVideoFrame[],
					    CVI_S32 s32MilliSec);

CVI_S32 CVI_VI_SetPipeNRXParam(VI_PIPE ViPipe, const VI_PIPE_NRX_PARAM_S *pstNrXParam);
CVI_S32 CVI_VI_GetPipeNRXParam(VI_PIPE ViPipe, VI_PIPE_NRX_PARAM_S *pstNrXParam);

CVI_S32 CVI_VI_SetPipeRepeatMode(VI_PIPE ViPipe, const VI_PIPE_REPEAT_MODE_E enPepeatMode);
CVI_S32 CVI_VI_GetPipeRepeatMode(VI_PIPE ViPipe, VI_PIPE_REPEAT_MODE_E *penPepeatMode);

CVI_S32 CVI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

CVI_S32 CVI_VI_EnablePipeInterrupt(VI_PIPE ViPipe);
CVI_S32 CVI_VI_DisablePipeInterrupt(VI_PIPE ViPipe);

CVI_S32 CVI_VI_SetPipeVCNumber(VI_PIPE ViPipe, CVI_U32 u32VCNumber);
CVI_S32 CVI_VI_GetPipeVCNumber(VI_PIPE ViPipe, CVI_U32 *pu32VCNumber);

CVI_S32 CVI_VI_GetPipeFd(VI_PIPE ViPipe);


/* 3 for vi chn */
CVI_S32 CVI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

CVI_S32 CVI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);
CVI_S32 CVI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

CVI_S32 CVI_VI_SetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CROP_INFO_S  *pstCropInfo);
CVI_S32 CVI_VI_GetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, VI_CROP_INFO_S  *pstCropInfo);

CVI_S32 CVI_VI_SetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, const ROTATION_E enRotation);
CVI_S32 CVI_VI_GetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, ROTATION_E *penRotation);

CVI_S32 CVI_VI_SetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LDC_ATTR_S *pstLDCAttr);
CVI_S32 CVI_VI_GetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LDC_ATTR_S *pstLDCAttr);

CVI_S32 CVI_VI_SetChnSpreadAttr(VI_PIPE ViPipe, VI_CHN ViChn, const SPREAD_ATTR_S *pstSpreadAttr);
CVI_S32 CVI_VI_GetChnSpreadAttr(VI_PIPE ViPipe, VI_CHN ViChn, SPREAD_ATTR_S *pstSpreadAttr);

CVI_S32 CVI_VI_SetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LOW_DELAY_INFO_S *pstLowDelayInfo);
CVI_S32 CVI_VI_GetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LOW_DELAY_INFO_S *pstLowDelayInfo);

CVI_S32 CVI_VI_GetChnRegionLuma(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_REGION_INFO_S *pstRegionInfo,
				    CVI_U64 *pu64LumaData, CVI_S32 s32MilliSec);

CVI_S32 CVI_VI_SetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, const FISHEYE_ATTR_S *pstFishEyeAttr);
CVI_S32 CVI_VI_GetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, FISHEYE_ATTR_S *pstFishEyeAttr);

CVI_S32 CVI_VI_SetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
CVI_S32 CVI_VI_GetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);

CVI_S32 CVI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, CVI_S32 s32MilliSec);
CVI_S32 CVI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

CVI_S32 CVI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

CVI_S32 CVI_VI_GetChnFd(VI_PIPE ViPipe, VI_CHN ViChn);
CVI_S32 CVI_VI_Trig_AHD(VI_PIPE ViPipe, CVI_U8 u8AHDSignal);

/* 4 for vi module */
CVI_S32 CVI_VI_CloseFd(void);

CVI_S32 CVI_VI_SetModParam(const VI_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_VI_GetModParam(VI_MOD_PARAM_S *pstModParam);

CVI_S32 CVI_VI_SetChnAlign(VI_PIPE ViPipe, VI_CHN ViChn, CVI_U32 u32Align);
CVI_S32 CVI_VI_GetChnAlign(VI_PIPE ViPipe, VI_CHN ViChn, CVI_U32 *pu32Align);
CVI_S32 CVI_VI_RegChnFlipMirrorCallBack(VI_PIPE ViPipe, VI_CHN ViChn, void *pvData);
CVI_S32 CVI_VI_UnRegChnFlipMirrorCallBack(VI_PIPE ViPipe, VI_CHN ViChn);
CVI_S32 CVI_VI_SetChnFlipMirror(VI_PIPE ViPipe, VI_CHN ViChn, CVI_BOOL bFlip, CVI_BOOL bMirror);
CVI_S32 CVI_VI_GetChnFlipMirror(VI_PIPE ViPipe, VI_CHN ViChn, CVI_BOOL *pbFlip, CVI_BOOL *pbMirror);

CVI_S32 CVI_VI_SetChnEarlyInterrupt(VI_PIPE ViPipe, VI_CHN ViChn, const VI_EARLY_INTERRUPT_S *pstEarlyInterrupt);
CVI_S32 CVI_VI_GetChnEarlyInterrupt(VI_PIPE ViPipe, VI_CHN ViChn, VI_EARLY_INTERRUPT_S *pstEarlyInterrupt);

// for fpga test
CVI_S32 _CVI_VI_CFG_CTRL_TEST(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__CVI_VI_H__ */
