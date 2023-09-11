/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vo.h
 * Description:
 *   MMF Programe Interface for video output management moudle
 */

#ifndef __CVI_VO_H__
#define __CVI_VO_H__

#include <linux/cvi_comm_vo.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* Device Relative Settings */

/**
 * @brief Set vo device attribute.
 *
 * @param VoDev(In), Device ID.
 * @param pstPubAttr(In), Device attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);

/**
 * @brief Get vo device attribute.
 *
 * @param VoDev(In), Device ID.
 * @param pstPubAttr(Out), Device Attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

/**
 * @brief Init i80 per instructions
 *
 * @param VoDev(In), Device ID.
 * @param pi80Instr(In), I80 init instructions
 * @param size(In), Number of pi80Instr
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_I80Init(VO_DEV VoDev, const VO_I80_INSTR_S *pi80Instr, CVI_U8 size);

/**
 * @brief Get vo device status.
 *
 * @param VoDev(In): Device ID.
 *
 * @return CVI_BOOL Return status of VoDev.
 */
CVI_BOOL CVI_VO_IsEnabled(VO_DEV VoDev);

/**
 * @brief Enable vo device.
 *
 * @param VoDev(In): Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_Enable(VO_DEV VoDev);

/**
 * @brief Disable vo device.
 *
 * @param VoDev(In): Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_Disable(VO_DEV VoDev);

/**
 * @brief Close vo device file descriptor.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_CloseFd(void);

/* Video Relative Settings */
/**
 * @brief Set vo videolayer attribute.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param pstLayerAttr(In), Videolayer attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
 * @brief Get vo videolayer attribute.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param pstLayerAttr(Out), Videolayer attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
 * @brief Enable vo videolayer.
 *
 * @param VoLayer(In): Videolayer ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_EnableVideoLayer(VO_LAYER VoLayer);

/**
 * @brief Disable vo videolayer.
 *
 * @param VoLayer(In): Videolayer ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_DisableVideoLayer(VO_LAYER VoLayer);

/**
 * @brief Get videolayer image quality control.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param type(In), Brightness/contrast/saturation/hue.
 * @param ctrl(out), Control info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetLayerProcAmpCtrl(VO_LAYER VoLayer, PROC_AMP_E type, PROC_AMP_CTRL_S *ctrl);

/**
 * @brief Get videolayer image brightness/contrast/saturation/hue value.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param type(In), Brightness/contrast/saturation/hue.
 * @param value(out), Value.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetLayerProcAmp(VO_LAYER VoLayer, PROC_AMP_E type, CVI_S32 *value);

/**
 * @brief Set videolayer image brightness/contrast/saturation/hue value.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param type(In), Brightness/contrast/saturation/hue.
 * @param value(In), Value.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetLayerProcAmp(VO_LAYER VoLayer, PROC_AMP_E type, CVI_S32 value);

/* Display relative operations */
/**
 * @brief Set videolayer buffer length.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param u32BufLen(In), Buffer length.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetDisplayBufLen(VO_LAYER VoLayer, CVI_U32 u32BufLen);

/**
 * @brief Get videolayer buffer length.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param pu32BufLen(Out), Buffer length.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetDisplayBufLen(VO_LAYER VoLayer, CVI_U32 *pu32BufLen);

/* Channel Relative Operations */
/**
 * @brief Set vo channel attribute.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param pstChnAttr(In), Channel attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Get vo channel attribute.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param pstChnAttr(Out), Channel attribute.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Enable vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Disable vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Pause vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_PauseChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Resume vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Show vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Hide vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
 * @brief Show vo Pattern.
 *
 * @param VoDev(In), VoDev ID.
 * @param PatternId(In), PatternId ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_ShowPattern(VO_DEV VoDev, enum VO_PATTERN_MODE PatternId);

/**
 * @brief Send frame to vo channel.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param pstVideoFrame(In), Frame info.
 * @param s32MilliSec(In), Timeout.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVideoFrame, CVI_S32 s32MilliSec);

/**
 * @brief Clear vo channel buffer.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param bClrAll(In), Clear all buffer or not.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_ClearChnBuf(VO_LAYER VoLayer, VO_CHN VoChn, CVI_BOOL bClrAll);

/**
 * @brief Set vo channel rotation.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param enRotation(In), Rotation.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E enRotation);

/**
 * @brief Get vo channel rotation.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param penRotation(Out), Rotation.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);


/* Module Parameter Settings */
/**
 * @brief Get panel status.
 *
 * @param VoLayer(In), Videolayer ID.
 * @param VoChn(In), Channel ID.
 * @param is_init(Out), Initialised or not.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_Get_Panel_Status(VO_LAYER VoLayer, VO_CHN VoChn, CVI_U32 *is_init);

/**
 * @brief Register Power Management Callback.
 *
 * @param VoDev(In), Device ID.
 * @param pstPmOps(In), Power Management operator.
 * @param pvData(In), Pointer of VO_PM_OPS_S.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_RegPmCallBack(VO_DEV VoDev, VO_PM_OPS_S *pstPmOps, void *pvData);

/**
 * @brief Unegister Power Management Callback.
 *
 * @param VoDev(In), Device ID.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_UnRegPmCallBack(VO_DEV VoDev);

/**
 * @brief Set vo gamma info.
 *
 * @param pinfo(In), gamma info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_SetGammaInfo(VO_GAMMA_INFO_S *pinfo);

/**
 * @brief Get vo gamma info.
 *
 * @param pinfo(Out), gamma info.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VO_GetGammaInfo(VO_GAMMA_INFO_S *pinfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_VO_H__
