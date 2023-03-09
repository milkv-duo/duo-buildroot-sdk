/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vi.h
 * Description:
 *   MMF Programe Interface for video input management moudle
 */

#ifndef __CVI_VI_H__
#define __CVI_VI_H__

#include <stdio.h>
#include <linux/cvi_comm_vi.h>
#include <linux/cvi_comm_gdc.h>
#include <cvi_comm_vb.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @brief Suspend vi.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_Suspend(void);

/**
 * @brief Resume vi.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_Resume(void);

/* 1 for vi device */
/**
 * @brief Set vi device num.
 *
 * @param devNum(In), Device Num.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetDevNum(CVI_U32 devNum);

/**
 * @brief Get vi device num.
 *
 * @param devNum(Out), Device Num.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetDevNum(CVI_U32 *devNum);

/**
 * @brief Set vi device attribute.
 *
 * @param ViDev(In), Device ID.
 * @param pstDevAttr(In), Device attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);

/**
 * @brief Get vi device attribute.
 *
 * @param ViDev(In), Device ID.
 * @param pstDevAttr(Out), Device Attribute
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

/**
 * @brief Enable vi device.
 *
 * @param ViDev(In), Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_EnableDev(VI_DEV ViDev);

/**
 * @brief Disable vi device.
 *
 * @param ViDev(In), Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_DisableDev(VI_DEV ViDev);

/**
 * @brief Set device timing attribute.
 *
 * @param ViDev(In), Device ID.
 * @param pstTimingAttr(In), Timing Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetDevTimingAttr(VI_DEV ViDev, const VI_DEV_TIMING_ATTR_S *pstTimingAttr);

/**
 * @brief Get device timing attribute.
 *
 * @param ViDev(In), Device ID.
 * @param pstTimingAttr(Out), Timing Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetDevTimingAttr(VI_DEV ViDev, VI_DEV_TIMING_ATTR_S *pstTimingAttr);

/**
 * @brief Create vi pipe.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstPipeAttr(In), Pipe Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);

/**
 * @brief Destroy vi pipe.
 *
 * @param ViPipe(In), Pipe ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_DestroyPipe(VI_PIPE ViPipe);

/**
 * @brief Set vi pipe attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstPipeAttr(In), Pipe Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);

/**
 * @brief Get vi pipe attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstPipeAttr(Out), Pipe Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

/**
 * @brief Start vi pipe.
 *
 * @param ViPipe(In), Pipe ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_StartPipe(VI_PIPE ViPipe);

/**
 * @brief Stop vi pipe.
 *
 * @param ViPipe(In), Pipe ID.
 *
 * @return VI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_StopPipe(VI_PIPE ViPipe);

/**
 * @brief Set pipe crop.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstCropInfo(In), Crop Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetPipeCrop(VI_PIPE ViPipe, const CROP_INFO_S *pstCropInfo);

/**
 * @brief Get pipe crop info.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstCropInfo(Out), Crop Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeCrop(VI_PIPE ViPipe, CROP_INFO_S *pstCropInfo);

/**
 * @brief Set pipe dump attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstDumpAttr(In), Dump Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr);

/**
 * @brief Get pipe dump attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstDumpAttr(Out), Dump Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr);

/**
 * @brief Set Pipe frame source.
 *
 * @param ViPipe(In), Pipe ID.
 * @param enSource(In), Source from Dev or FE or BE.
 *
 * @return  CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetPipeFrameSource(VI_PIPE ViPipe, const VI_PIPE_FRAME_SOURCE_E enSource);

/**
 * @brief Get Pipe frame source.
 *
 * @param ViPipe(In), Pipe ID.
 * @param penSource(In), Source from Dev or FE or BE.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeFrameSource(VI_PIPE ViPipe, VI_PIPE_FRAME_SOURCE_E *penSource);

/**
 * @brief Get vi pipe frame(raw image).
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstVideoFrame(Out), Frame Info.
 * @param s32MilliSec(In), Timeout Unit ms.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);

/**
 * @brief Release vi pipe frame(raw image).
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstVideoFrame(In), Frame Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_ReleasePipeFrame(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);

/**
 * @brief Start vi pipe frame(Continuous).
 *
 * @param pstDumpInfo(In), Continuous dump raw info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_StartSmoothRawDump(const VI_SMOOTH_RAW_DUMP_INFO_S *pstDumpInfo);

/**
 * @brief Stop vi pipe frame(Continuous).
 *
 * @param pstDumpInfo(In), Continuous dump raw info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_StopSmoothRawDump(const VI_SMOOTH_RAW_DUMP_INFO_S *pstDumpInfo);

/**
 * @brief Get vi pipe frame(Continuous).
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstVideoFrame(In), Frame Info.
 * @param s32MilliSec(In), Timeout Unit ms.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetSmoothRawDump(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);

/**
 * @brief Put vi pipe frame(Continuous).
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstVideoFrame(In), Frame Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_PutSmoothRawDump(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);

/**
 * @brief Send raw to pipe.
 *
 * @param u32PipeNum(In), Pipe Num.
 * @param PipeId(In), Pipe ID.
 * @param pstVideoFrame(In), Frame Info.
 * @param s32MilliSec(In), Timeout Unit ms.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SendPipeRaw(CVI_U32 u32PipeNum, VI_PIPE PipeId[], const VIDEO_FRAME_INFO_S *pstVideoFrame[],
					    CVI_S32 s32MilliSec);

/**
 * @brief Query Pipe Status.
 *
 * @param ViPipe(In), Pipe ID.
 * @param pstStatus(In), Pipe Status.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

/**
 * @brief Get pipe file descriptor
 *
 * @param ViPipe(In), Pipe ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetPipeFd(VI_PIPE ViPipe);

/**
 * @brief Dump isp hardware register to file
 *
 * @param ViPipe(In), Pipe ID.
 * @param fp(In), File Descriptor.
 * @param pstRegTbl(In), ISP MlscGainLut.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_DumpHwRegisterToFile(VI_PIPE ViPipe, FILE *fp, VI_DUMP_REGISTER_TABLE_S *pstRegTbl);

/**
 * @brief Vi channel get vb from a specified pool.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param VbPool(In), VB pool ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_AttachVbPool(VI_PIPE ViPipe, VI_CHN ViChn, VB_POOL VbPool);

/**
 * @brief Cancel channel specified pool.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_DetachVbPool(VI_PIPE ViPipe, VI_CHN ViChn);

/**
 * @brief Set vi channel attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstChnAttr(In), Channel Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Get vi channel attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstChnAttr(Out), Channel Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Enable vi channel.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);

/**
 * @brief Disable vi channel.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

/**
 * @brief Set vi channel crop.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstCropInfo(In), Channel Crop Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CROP_INFO_S  *pstCropInfo);

/**
 * @brief Get vi channel crop.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstCropInfo(Out), Channel Crop Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, VI_CROP_INFO_S  *pstCropInfo);

/**
 * @brief Set vi channel rotate.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param enRotation(In), Degree of rotation(0/90/180/270)
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, const ROTATION_E enRotation);

/**
 * @brief Get vi channel rotate.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param enRotation(Out), Degree of rotation
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnRotation(VI_PIPE ViPipe, VI_CHN ViChn, ROTATION_E *penRotation);

/**
 * @brief Set vi channel LDC attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstLDCAttr(In), LDC Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LDC_ATTR_S *pstLDCAttr);

/**
 * @brief Get vi channel LDC attribute.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstLDCAttr(Out), LDC Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnLDCAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LDC_ATTR_S *pstLDCAttr);

/**
 * @brief Get vi frame from a specified channel.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstFrameInfo(Out), Frame Info.
 * @param s32MilliSec(In), Timeout Unit ms.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, CVI_S32 s32MilliSec);

/**
 * @brief release vi channel frame.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstFrameInfo(In), Frame Info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

/**
 * @brief Query vi channel status.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param pstChnStatus(Out), channel status.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

/**
 * @brief Vi close fd.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_CloseFd(void);

/**
 * @brief Register flip mirror callback.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViDev(In), Device ID.
 * @param pvData(In), Point of Sensor Flip Mirror Function.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_RegChnFlipMirrorCallBack(VI_PIPE ViPipe, VI_DEV ViDev, void *pvData);

/**
 * @brief Unregister flip mirror callback.
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViDev(In), Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_UnRegChnFlipMirrorCallBack(VI_PIPE ViPipe, VI_DEV ViDev);

/**
 * @brief Set vi chn flip and mirror
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(In), Channel ID.
 * @param bFlip(In), Flip(0/1).
 * @param bMirror(In), Mirror(0/1).
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_SetChnFlipMirror(VI_PIPE ViPipe, VI_CHN ViChn, CVI_BOOL bFlip, CVI_BOOL bMirror);

/**
 * @brief Set vi chn flip and mirror
 *
 * @param ViPipe(In), Pipe ID.
 * @param ViChn(Out), Channel ID.
 * @param bFlip(Out), Flip(0/1).
 * @param bMirror(In), Mirror(0/1).
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_GetChnFlipMirror(VI_PIPE ViPipe, VI_CHN ViChn, CVI_BOOL *pbFlip, CVI_BOOL *pbMirror);

/**
 * @brief Register Power Management Callback
 *
 * @param ViDev(In), Device ID.
 * @param pstPmOps(In), Power Management operator.
 * @param pvData(In), Point of VI_PM_DATA_S.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_RegPmCallBack(VI_DEV ViDev, VI_PM_OPS_S *pstPmOps, void *pvData);

/**
 * @brief Unregister Power Management Callback.
 *
 * @param ViDev(In), Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VI_UnRegPmCallBack(VI_DEV ViDev);

/**
 * @deprecated
 */
CVI_S32 CVI_VI_Trig_AHD(VI_PIPE ViPipe, CVI_U8 u8AHDSignal);
CVI_S32 CVI_VI_SetDevBindPipe(VI_DEV ViDev, const VI_DEV_BIND_PIPE_S *pstDevBindPipe);
CVI_S32 CVI_VI_GetDevBindPipe(VI_DEV ViDev, VI_DEV_BIND_PIPE_S *pstDevBindPipe);
CVI_S32 CVI_VI_SetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, const FISHEYE_ATTR_S *pstFishEyeAttr);
CVI_S32 CVI_VI_GetExtChnFisheye(VI_PIPE ViPipe, VI_CHN ViChn, FISHEYE_ATTR_S *pstFishEyeAttr);
CVI_S32 CVI_VI_SetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
CVI_S32 CVI_VI_GetExtChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);
// for fpga test
CVI_S32 _CVI_VI_CFG_CTRL_TEST(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__CVI_VI_H__ */
