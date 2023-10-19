/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_isp.h
 * Description:
 */

#ifndef __CVI_ISP_H__
#define __CVI_ISP_H__

//#include "cvi_comm_video.h"
//#include "cvi_comm_isp.h"
//#include "cvi_comm_3a.h"
#include "cvi_comm_sns.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#if 0
/* Firmware Main Operation */
CVI_S32 CVI_ISP_Init(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_MemInit(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_Run(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_RunOnce(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_Exit(VI_PIPE ViPipe);

/* Each Param set / get Operation */
CVI_S32 CVI_ISP_SetPubAttr(VI_PIPE ViPipe, const ISP_PUB_ATTR_S *pstPubAttr);
CVI_S32 CVI_ISP_GetPubAttr(VI_PIPE ViPipe, ISP_PUB_ATTR_S *pstPubAttr);
CVI_S32 CVI_ISP_SetModParam(const ISP_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_ISP_GetModParam(ISP_MOD_PARAM_S *pstModParam);
CVI_S32 CVI_ISP_SetCtrlParam(VI_PIPE ViPipe,
				const ISP_CTRL_PARAM_S *pstIspCtrlParam);
CVI_S32 CVI_ISP_GetCtrlParam(VI_PIPE ViPipe, ISP_CTRL_PARAM_S *pstIspCtrlParam);
CVI_S32 CVI_ISP_SetFMWState(VI_PIPE ViPipe, const ISP_FMW_STATE_E enState);
CVI_S32 CVI_ISP_GetFMWState(VI_PIPE ViPipe, ISP_FMW_STATE_E *penState);
#endif
/* sensor related register */
CVI_S32 CVI_ISP_SensorRegCallBack(VI_PIPE ViPipe,
				ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, ISP_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, SENSOR_ID SensorId);
#if 0
/* 3a algo &static related function. */
CVI_S32 CVI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
CVI_S32 CVI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

CVI_S32 CVI_ISP_AELibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib,
				ISP_AE_REGISTER_S *pstRegister);
CVI_S32 CVI_ISP_AELibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

CVI_S32 CVI_ISP_AWBLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib,
				ISP_AWB_REGISTER_S *pstRegister);
CVI_S32 CVI_ISP_AWBLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

CVI_S32 CVI_ISP_AFLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAfLib,
				ISP_AF_REGISTER_S *pstRegister);
CVI_S32 CVI_ISP_AFLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

CVI_S32 CVI_ISP_SetBindAttr(VI_PIPE ViPipe, const ISP_BIND_ATTR_S *pstBindAttr);
CVI_S32 CVI_ISP_GetBindAttr(VI_PIPE ViPipe, ISP_BIND_ATTR_S *pstBindAttr);

CVI_S32 CVI_ISP_SetColorToneAttr(VI_PIPE ViPipe,
				const ISP_COLOR_TONE_ATTR_S *pstWBGAttr);
CVI_S32 CVI_ISP_GetColorToneAttr(VI_PIPE ViPipe, ISP_COLOR_TONE_ATTR_S *pstWBGAttr);

CVI_S32 CVI_ISP_SetDehazeAttr(VI_PIPE ViPipe,
				const ISP_DEHAZE_ATTR_S *pstDehazeAttr);
CVI_S32 CVI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr);

CVI_S32 CVI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_SetGammaCurveType(VI_PIPE ViPipe, const ISP_GAMMA_CURVE_TYPE_E curveType);
CVI_S32 CVI_ISP_SetAutoGammaAttr(VI_PIPE ViPipe, const ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_GetAutoGammaAttr(VI_PIPE ViPipe, ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr);

CVI_S32 CVI_ISP_SetStatisticsConfig(VI_PIPE ViPipe, const ISP_STATISTICS_CFG_S *pstStatCfg);
CVI_S32 CVI_ISP_GetStatisticsConfig(VI_PIPE ViPipe, ISP_STATISTICS_CFG_S *pstStatCfg);	// TODO@CF implement

CVI_S32 CVI_ISP_SetNRAttr(VI_PIPE ViPipe, const ISP_NR_ATTR_S *pstNRAttr);
CVI_S32 CVI_ISP_GetNRAttr(VI_PIPE ViPipe, ISP_NR_ATTR_S *pstNRAttr);
CVI_S32 CVI_ISP_SetNRFilterAttr(VI_PIPE ViPipe, const ISP_NR_FILTER_ATTR_S *pstNRFilterAttr);
CVI_S32 CVI_ISP_GetNRFilterAttr(VI_PIPE ViPipe, ISP_NR_FILTER_ATTR_S *pstNRFilterAttr);
CVI_S32 CVI_ISP_SetRLSCAttr(VI_PIPE ViPipe, const ISP_RLSC_ATTR_S *pstRLSCAttr);
CVI_S32 CVI_ISP_GetRLSCAttr(VI_PIPE ViPipe, ISP_RLSC_ATTR_S *pstRLSCAttr);

CVI_S32 CVI_ISP_SetYNRAttr(VI_PIPE ViPipe, const ISP_YNR_ATTR_S *pstYNRAttr);
CVI_S32 CVI_ISP_GetYNRAttr(VI_PIPE ViPipe, ISP_YNR_ATTR_S *pstYNRAttr);
CVI_S32 CVI_ISP_SetYNRMotionNRAttr(VI_PIPE ViPipe, const ISP_YNR_MOTION_NR_ATTR_S *pstYNRMotionNRAttr);
CVI_S32 CVI_ISP_GetYNRMotionNRAttr(VI_PIPE ViPipe, ISP_YNR_MOTION_NR_ATTR_S *pstYNRMotionNRAttr);
CVI_S32 CVI_ISP_SetYNRFilterAttr(VI_PIPE ViPipe, const ISP_YNR_FILTER_ATTR_S *pstYNRFilterAttr);
CVI_S32 CVI_ISP_GetYNRFilterAttr(VI_PIPE ViPipe, ISP_YNR_FILTER_ATTR_S *pstYNRFilterAttr);

CVI_S32 CVI_ISP_SetCNRAttr(VI_PIPE ViPipe, const ISP_CNR_ATTR_S *pstCNRAttr);
CVI_S32 CVI_ISP_GetCNRAttr(VI_PIPE ViPipe, ISP_CNR_ATTR_S *pstCNRAttr);

CVI_S32 CVI_ISP_SetCACAttr(VI_PIPE ViPipe, const ISP_CAC_ATTR_S *pstCACAttr);
CVI_S32 CVI_ISP_GetCACAttr(VI_PIPE ViPipe, ISP_CAC_ATTR_S *pstCACAttr);

CVI_S32 CVI_ISP_SetDCIAttr(VI_PIPE ViPipe, const ISP_DCI_ATTR_S *pstDCIAttr);
CVI_S32 CVI_ISP_GetDCIAttr(VI_PIPE ViPipe, ISP_DCI_ATTR_S *pstDCIAttr);

CVI_S32 CVI_ISP_SetMeshShadingAttr(VI_PIPE ViPipe, const ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);
CVI_S32 CVI_ISP_GetMeshShadingAttr(VI_PIPE ViPipe, ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);
CVI_S32 CVI_ISP_SetMeshShadingGainLutAttr(VI_PIPE ViPipe,
				const ISP_MESH_SHADING_GAIN_LUT_ATTR_S *pstMeshShadingGainLutAttr);
CVI_S32 CVI_ISP_GetMeshShadingGainLutAttr(VI_PIPE ViPipe,
				ISP_MESH_SHADING_GAIN_LUT_ATTR_S *pstMeshShadingGainLutAttr);

CVI_S32 CVI_ISP_SetRadialShadingAttr(VI_PIPE ViPipe, const ISP_RADIAL_SHADING_ATTR_S *pstRadialShadingAttr);
CVI_S32 CVI_ISP_GetRadialShadingAttr(VI_PIPE ViPipe, ISP_RADIAL_SHADING_ATTR_S *pstRadialShadingAttr);
CVI_S32 CVI_ISP_SetRadialShadingGainLutAttr(VI_PIPE ViPipe,
				const ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S *pstRadialShadingGainLutAttr);
CVI_S32 CVI_ISP_GetRadialShadingGainLutAttr(VI_PIPE ViPipe,
				ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S *pstRadialShadingGainLutAttr);

CVI_S32 CVI_ISP_SetTNRAttr(VI_PIPE ViPipe, const ISP_TNR_ATTR_S *pstTNRAttr);
CVI_S32 CVI_ISP_GetTNRAttr(VI_PIPE ViPipe, ISP_TNR_ATTR_S *pstTNRAttr);
CVI_S32 CVI_ISP_SetTNRNoiseModelAttr(VI_PIPE ViPipe, const ISP_TNR_NOISE_MODEL_ATTR_S *pstTNRNoiseModelAttr);
CVI_S32 CVI_ISP_GetTNRNoiseModelAttr(VI_PIPE ViPipe, ISP_TNR_NOISE_MODEL_ATTR_S *pstTNRNoiseModelAttr);
CVI_S32 CVI_ISP_SetTNRLumaMotionAttr(VI_PIPE ViPipe, const ISP_TNR_LUMA_MOTION_ATTR_S *pstTNRLumaMotionAttr);
CVI_S32 CVI_ISP_GetTNRLumaMotionAttr(VI_PIPE ViPipe, ISP_TNR_LUMA_MOTION_ATTR_S *pstTNRLumaMotionAttr);
CVI_S32 CVI_ISP_SetTNRGhostAttr(VI_PIPE ViPipe, const ISP_TNR_GHOST_ATTR_S *pstTNRGhostAttr);
CVI_S32 CVI_ISP_GetTNRGhostAttr(VI_PIPE ViPipe, ISP_TNR_GHOST_ATTR_S *pstTNRGhostAttr);
CVI_S32 CVI_ISP_SetTNRMtPrtAttr(VI_PIPE ViPipe, const ISP_TNR_MT_PRT_ATTR_S *pstTNRMtPrtAttr);
CVI_S32 CVI_ISP_GetTNRMtPrtAttr(VI_PIPE ViPipe, ISP_TNR_MT_PRT_ATTR_S *pstTNRMtPrtAttr);

CVI_S32 CVI_ISP_SetCLutAttr(VI_PIPE ViPipe, const ISP_CLUT_ATTR_S *pstCLutAttr);
CVI_S32 CVI_ISP_GetCLutAttr(VI_PIPE ViPipe, ISP_CLUT_ATTR_S *pstCLutAttr);

CVI_S32 CVI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACK_LEVEL_ATTR_S *pstBlackLevelAttr);
CVI_S32 CVI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACK_LEVEL_ATTR_S *pstBlackLevelAttr);

CVI_S32 CVI_ISP_SetDemosaicAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
CVI_S32 CVI_ISP_GetDemosaicAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
CVI_S32 CVI_ISP_SetDemosaicDemoireAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_DEMOIRE_ATTR_S *pstDemosaicDemoireAttr);
CVI_S32 CVI_ISP_GetDemosaicDemoireAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_DEMOIRE_ATTR_S *pstDemosaicDemoireAttr);
CVI_S32 CVI_ISP_SetDemosaicFilterAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_FILTER_ATTR_S *pstDemosaicFilterAttr);
CVI_S32 CVI_ISP_GetDemosaicFilterAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_FILTER_ATTR_S *pstDemosaicFilterAttr);
CVI_S32 CVI_ISP_SetDemosaicEEAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_EE_ATTR_S *pstDemosaicEEAttr);
CVI_S32 CVI_ISP_GetDemosaicEEAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_EE_ATTR_S *pstDemosaicEEAttr);

CVI_S32 CVI_ISP_SetSaturationAttr(VI_PIPE ViPipe, const ISP_SATURATION_ATTR_S *pstSaturationAttr);
CVI_S32 CVI_ISP_GetSaturationAttr(VI_PIPE ViPipe, ISP_SATURATION_ATTR_S *pstSaturationAttr);
CVI_S32 CVI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_CCM_ATTR_S *pstCCMAttr);
CVI_S32 CVI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_CCM_ATTR_S *pstCCMAttr);
CVI_S32 CVI_ISP_SetHSVAttr(VI_PIPE ViPipe, const ISP_HSV_ATTR_S *pstHSVAttr);
CVI_S32 CVI_ISP_GetHSVAttr(VI_PIPE ViPipe, ISP_HSV_ATTR_S *pstHSVAttr);

CVI_S32 CVI_ISP_SetDPDynamicAttr(VI_PIPE ViPipe, const ISP_DP_DYNAMIC_ATTR_S *pstDPCDynamicAttr);
CVI_S32 CVI_ISP_GetDPDynamicAttr(VI_PIPE ViPipe, ISP_DP_DYNAMIC_ATTR_S *pstDPCDynamicAttr);
CVI_S32 CVI_ISP_SetDPStaticAttr(VI_PIPE ViPipe, const ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);
CVI_S32 CVI_ISP_GetDPStaticAttr(VI_PIPE ViPipe, ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);
CVI_S32 CVI_ISP_SetDPCalibrate(VI_PIPE ViPipe, const ISP_DP_CALIB_ATTR_S *pstDPCalibAttr);
CVI_S32 CVI_ISP_GetDPCalibrate(VI_PIPE ViPipe, ISP_DP_CALIB_ATTR_S *pstDPCalibAttr);

CVI_S32 CVI_ISP_SetCrosstalkAttr(VI_PIPE ViPipe, const ISP_CROSSTALK_ATTR_S *pstCrosstalkAttr);
CVI_S32 CVI_ISP_GetCrosstalkAttr(VI_PIPE ViPipe, ISP_CROSSTALK_ATTR_S *pstCrosstalkAttr);

CVI_S32 CVI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat);
CVI_S32 CVI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstWBStat);
CVI_S32 CVI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat);

CVI_S32 CVI_ISP_SetFSWDRAttr(VI_PIPE ViPipe, const ISP_FSWDR_ATTR_S *pstFSWDRAttr);
CVI_S32 CVI_ISP_GetFSWDRAttr(VI_PIPE ViPipe, ISP_FSWDR_ATTR_S *pstFSWDRAttr);


CVI_S32 CVI_ISP_SetDRCAttr(VI_PIPE ViPipe, const ISP_DRC_ATTR_S *pstDRCAttr);
CVI_S32 CVI_ISP_GetDRCAttr(VI_PIPE ViPipe, ISP_DRC_ATTR_S *pstDRCAttr);

CVI_S32 CVI_ISP_SetSharpenAttr(VI_PIPE ViPipe, const ISP_SHARPEN_ATTR_S *pstDRCAttr);
CVI_S32 CVI_ISP_GetSharpenAttr(VI_PIPE ViPipe, ISP_SHARPEN_ATTR_S *pstDRCAttr);

CVI_S32 CVI_ISP_SetNoiseProfileAttr(VI_PIPE ViPipe, const ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr);
CVI_S32 CVI_ISP_GetNoiseProfileAttr(VI_PIPE ViPipe, ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr);
#endif
/* MIPI related function. */
CVI_S32 CVI_MIPI_SetMipiReset(CVI_S32 devno, CVI_U32 reset);
CVI_S32 CVI_MIPI_SetSensorClock(CVI_S32 devno, CVI_U32 enable);
CVI_S32 CVI_MIPI_SetSensorReset(CVI_S32 devno, CVI_U32 reset);
CVI_S32 CVI_MIPI_SetMipiAttr(CVI_S32 ViPipe, const CVI_VOID *devAttr);
CVI_S32 CVI_MIPI_SetClkEdge(CVI_S32 devno, CVI_U32 is_up);

//CVI_S32 CVI_ISP_QueryInnerStateInfo(VI_PIPE ViPipe, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);
//CVI_S32 CVI_ISP_GetAEStitchStatistics(VI_PIPE ViPipe, ISP_AE_STITCH_STATISTICS_S *pstStitchStat);
//CVI_S32 CVI_ISP_GetMGStatistics(VI_PIPE ViPipe, ISP_MG_STATISTICS_S *pstMgStat);
//CVI_S32 CVI_ISP_GetWBStitchStatistics(VI_PIPE ViPipe, ISP_WB_STITCH_STATISTICS_S *pstStitchWBStat);
//CVI_S32 CVI_ISP_SetMonoAttr(VI_PIPE ViPipe, const ISP_MONO_ATTR_S *pstMonoAttr);
//CVI_S32 CVI_ISP_GetMonoAttr(VI_PIPE ViPipe, ISP_MONO_ATTR_S *pstMonoAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_ISP_H__ */
