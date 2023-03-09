/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2022. All rights reserved.
 *
 * File Name: include/cvi_isp.h
 * Description:
 */

#ifndef __CVI_ISP_H__
#define __CVI_ISP_H__

#include "stdio.h"

#include "cvi_comm_isp.h"
#include "cvi_comm_3a.h"
#include "cvi_comm_sns.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

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
/* sensor related register */
CVI_S32 CVI_ISP_SensorRegCallBack(VI_PIPE ViPipe,
				ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo, ISP_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, SENSOR_ID SensorId);

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

CVI_S32 CVI_ISP_SetModuleControl(VI_PIPE ViPipe, const ISP_MODULE_CTRL_U *punModCtrl);
CVI_S32 CVI_ISP_GetModuleControl(VI_PIPE ViPipe, ISP_MODULE_CTRL_U *punModCtrl);
CVI_S32 CVI_ISP_SetRegister(VI_PIPE ViPipe, CVI_U32 u32Addr, CVI_U32 u32Value);
CVI_S32 CVI_ISP_GetRegister(VI_PIPE ViPipe, CVI_U32 u32Addr, CVI_U32 *pu32Value);

//-----------------------------------------------------------------------------
//  Black Level Correction(BLC)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACK_LEVEL_ATTR_S *pstBlackLevelAttr);
CVI_S32 CVI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACK_LEVEL_ATTR_S *pstBlackLevelAttr);

//-----------------------------------------------------------------------------
//  Dead pixel correction(DPC)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetDPDynamicAttr(VI_PIPE ViPipe, const ISP_DP_DYNAMIC_ATTR_S *pstDPCDynamicAttr);
CVI_S32 CVI_ISP_GetDPDynamicAttr(VI_PIPE ViPipe, ISP_DP_DYNAMIC_ATTR_S *pstDPCDynamicAttr);
CVI_S32 CVI_ISP_SetDPStaticAttr(VI_PIPE ViPipe, const ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);
CVI_S32 CVI_ISP_GetDPStaticAttr(VI_PIPE ViPipe, ISP_DP_STATIC_ATTR_S *pstDPStaticAttr);
CVI_S32 CVI_ISP_SetDPCalibrate(VI_PIPE ViPipe, const ISP_DP_CALIB_ATTR_S *pstDPCalibAttr);
CVI_S32 CVI_ISP_GetDPCalibrate(VI_PIPE ViPipe, ISP_DP_CALIB_ATTR_S *pstDPCalibAttr);

//-----------------------------------------------------------------------------
//  Crosstalk
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCrosstalkAttr(VI_PIPE ViPipe, const ISP_CROSSTALK_ATTR_S *pstCrosstalkAttr);
CVI_S32 CVI_ISP_GetCrosstalkAttr(VI_PIPE ViPipe, ISP_CROSSTALK_ATTR_S *pstCrosstalkAttr);

//-----------------------------------------------------------------------------
//  Bayer domain noise reduction (BNR)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetNRAttr(VI_PIPE ViPipe, const ISP_NR_ATTR_S *pstNRAttr);
CVI_S32 CVI_ISP_GetNRAttr(VI_PIPE ViPipe, ISP_NR_ATTR_S *pstNRAttr);
CVI_S32 CVI_ISP_SetNRFilterAttr(VI_PIPE ViPipe, const ISP_NR_FILTER_ATTR_S *pstNRFilterAttr);
CVI_S32 CVI_ISP_GetNRFilterAttr(VI_PIPE ViPipe, ISP_NR_FILTER_ATTR_S *pstNRFilterAttr);

//-----------------------------------------------------------------------------
//  Demosaic
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetDemosaicAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
CVI_S32 CVI_ISP_GetDemosaicAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
CVI_S32 CVI_ISP_SetDemosaicDemoireAttr(VI_PIPE ViPipe, const ISP_DEMOSAIC_DEMOIRE_ATTR_S *pstDemosaicDemoireAttr);
CVI_S32 CVI_ISP_GetDemosaicDemoireAttr(VI_PIPE ViPipe, ISP_DEMOSAIC_DEMOIRE_ATTR_S *pstDemosaicDemoireAttr);

//-----------------------------------------------------------------------------
//  RGBCAC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetRGBCACAttr(VI_PIPE ViPipe, const ISP_RGBCAC_ATTR_S *pstRGBCACAttr);
CVI_S32 CVI_ISP_GetRGBCACAttr(VI_PIPE ViPipe, ISP_RGBCAC_ATTR_S *pstRGBCACAttr);

//-----------------------------------------------------------------------------
//  LCAC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetLCACAttr(VI_PIPE ViPipe, const ISP_LCAC_ATTR_S *pstLCACAttr);
CVI_S32 CVI_ISP_GetLCACAttr(VI_PIPE ViPipe, ISP_LCAC_ATTR_S *pstLCACAttr);

//-----------------------------------------------------------------------------
//  Mesh lens shading correction (MLSC)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetMeshShadingAttr(VI_PIPE ViPipe, const ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);
CVI_S32 CVI_ISP_GetMeshShadingAttr(VI_PIPE ViPipe, ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);
CVI_S32 CVI_ISP_SetMeshShadingGainLutAttr(VI_PIPE ViPipe,
				const ISP_MESH_SHADING_GAIN_LUT_ATTR_S *pstMeshShadingGainLutAttr);
CVI_S32 CVI_ISP_GetMeshShadingGainLutAttr(VI_PIPE ViPipe,
				ISP_MESH_SHADING_GAIN_LUT_ATTR_S *pstMeshShadingGainLutAttr);

//-----------------------------------------------------------------------------
//  CCM
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetSaturationAttr(VI_PIPE ViPipe, const ISP_SATURATION_ATTR_S *pstSaturationAttr);
CVI_S32 CVI_ISP_GetSaturationAttr(VI_PIPE ViPipe, ISP_SATURATION_ATTR_S *pstSaturationAttr);
CVI_S32 CVI_ISP_SetCCMSaturationAttr(VI_PIPE ViPipe, const ISP_CCM_SATURATION_ATTR_S *pstCCMSaturationAttr);
CVI_S32 CVI_ISP_GetCCMSaturationAttr(VI_PIPE ViPipe, ISP_CCM_SATURATION_ATTR_S *pstCCMSaturationAttr);
CVI_S32 CVI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_CCM_ATTR_S *pstCCMAttr);
CVI_S32 CVI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_CCM_ATTR_S *pstCCMAttr);

//-----------------------------------------------------------------------------
//  CSC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCSCAttr(VI_PIPE ViPipe, const ISP_CSC_ATTR_S *pstCSCAttr);
CVI_S32 CVI_ISP_GetCSCAttr(VI_PIPE ViPipe, ISP_CSC_ATTR_S *pstCSCAttr);

//-----------------------------------------------------------------------------
//  Color tone
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetColorToneAttr(VI_PIPE ViPipe,
				const ISP_COLOR_TONE_ATTR_S *pstColorToneAttr);
CVI_S32 CVI_ISP_GetColorToneAttr(VI_PIPE ViPipe, ISP_COLOR_TONE_ATTR_S *pstColorToneAttr);

//-----------------------------------------------------------------------------
//  FSWDR
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetFSWDRAttr(VI_PIPE ViPipe, const ISP_FSWDR_ATTR_S *pstFSWDRAttr);
CVI_S32 CVI_ISP_GetFSWDRAttr(VI_PIPE ViPipe, ISP_FSWDR_ATTR_S *pstFSWDRAttr);

//-----------------------------------------------------------------------------
//  DRC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetDRCAttr(VI_PIPE ViPipe, const ISP_DRC_ATTR_S *pstDRCAttr);
CVI_S32 CVI_ISP_GetDRCAttr(VI_PIPE ViPipe, ISP_DRC_ATTR_S *pstDRCAttr);

//-----------------------------------------------------------------------------
//  Gamma
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_GetGammaCurveByType(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr,
				const ISP_GAMMA_CURVE_TYPE_E curveType);
CVI_S32 CVI_ISP_SetAutoGammaAttr(VI_PIPE ViPipe, const ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr);
CVI_S32 CVI_ISP_GetAutoGammaAttr(VI_PIPE ViPipe, ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr);

//-----------------------------------------------------------------------------
//  Dehaze
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetDehazeAttr(VI_PIPE ViPipe,
				const ISP_DEHAZE_ATTR_S *pstDehazeAttr);
CVI_S32 CVI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr);

//-----------------------------------------------------------------------------
//  CLUT
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetClutAttr(VI_PIPE ViPipe, const ISP_CLUT_ATTR_S *pstClutAttr);
CVI_S32 CVI_ISP_GetClutAttr(VI_PIPE ViPipe, ISP_CLUT_ATTR_S *pstClutAttr);
CVI_S32 CVI_ISP_SetClutSaturationAttr(VI_PIPE ViPipe, const ISP_CLUT_SATURATION_ATTR_S *pstClutSaturationAttr);
CVI_S32 CVI_ISP_GetClutSaturationAttr(VI_PIPE ViPipe, ISP_CLUT_SATURATION_ATTR_S *pstClutSaturationAttr);

//-----------------------------------------------------------------------------
//  DCI
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetDCIAttr(VI_PIPE ViPipe, const ISP_DCI_ATTR_S *pstDCIAttr);
CVI_S32 CVI_ISP_GetDCIAttr(VI_PIPE ViPipe, ISP_DCI_ATTR_S *pstDCIAttr);

//-----------------------------------------------------------------------------
//  LDCI
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetLDCIAttr(VI_PIPE ViPipe, const ISP_LDCI_ATTR_S *pstLDCIAttr);
CVI_S32 CVI_ISP_GetLDCIAttr(VI_PIPE ViPipe, ISP_LDCI_ATTR_S *pstLDCIAttr);

//-----------------------------------------------------------------------------
//  CA (CA/CP)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCAAttr(VI_PIPE ViPipe, const ISP_CA_ATTR_S *pstCAAttr);
CVI_S32 CVI_ISP_GetCAAttr(VI_PIPE ViPipe, ISP_CA_ATTR_S *pstCAAttr);

//-----------------------------------------------------------------------------
//  CA2
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCA2Attr(VI_PIPE ViPipe, const ISP_CA2_ATTR_S *pstCA2Attr);
CVI_S32 CVI_ISP_GetCA2Attr(VI_PIPE ViPipe, ISP_CA2_ATTR_S *pstCA2Attr);

//-----------------------------------------------------------------------------
//  PreSharpen
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetPreSharpenAttr(VI_PIPE ViPipe, const ISP_PRESHARPEN_ATTR_S *pstPreSharpenAttr);
CVI_S32 CVI_ISP_GetPreSharpenAttr(VI_PIPE ViPipe, ISP_PRESHARPEN_ATTR_S *pstPreSharpenAttr);

//-----------------------------------------------------------------------------
//  Time-domain noise reduction (TNR)
//-----------------------------------------------------------------------------
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
CVI_S32 CVI_ISP_SetTNRMotionAdaptAttr(VI_PIPE ViPipe, const ISP_TNR_MOTION_ADAPT_ATTR_S *pstTNRMotionAdaptAttr);
CVI_S32 CVI_ISP_GetTNRMotionAdaptAttr(VI_PIPE ViPipe, ISP_TNR_MOTION_ADAPT_ATTR_S *pstTNRMotionAdaptAttr);

//-----------------------------------------------------------------------------
//  Y domain noise reduction (YNR)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetYNRAttr(VI_PIPE ViPipe, const ISP_YNR_ATTR_S *pstYNRAttr);
CVI_S32 CVI_ISP_GetYNRAttr(VI_PIPE ViPipe, ISP_YNR_ATTR_S *pstYNRAttr);
CVI_S32 CVI_ISP_SetYNRMotionNRAttr(VI_PIPE ViPipe, const ISP_YNR_MOTION_NR_ATTR_S *pstYNRMotionNRAttr);
CVI_S32 CVI_ISP_GetYNRMotionNRAttr(VI_PIPE ViPipe, ISP_YNR_MOTION_NR_ATTR_S *pstYNRMotionNRAttr);
CVI_S32 CVI_ISP_SetYNRFilterAttr(VI_PIPE ViPipe, const ISP_YNR_FILTER_ATTR_S *pstYNRFilterAttr);
CVI_S32 CVI_ISP_GetYNRFilterAttr(VI_PIPE ViPipe, ISP_YNR_FILTER_ATTR_S *pstYNRFilterAttr);

//-----------------------------------------------------------------------------
//  UV domain noise reduction (CNR)
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCNRAttr(VI_PIPE ViPipe, const ISP_CNR_ATTR_S *pstCNRAttr);
CVI_S32 CVI_ISP_GetCNRAttr(VI_PIPE ViPipe, ISP_CNR_ATTR_S *pstCNRAttr);
CVI_S32 CVI_ISP_SetCNRMotionNRAttr(VI_PIPE ViPipe, const ISP_CNR_MOTION_NR_ATTR_S *pstCNRMotionNRAttr);
CVI_S32 CVI_ISP_GetCNRMotionNRAttr(VI_PIPE ViPipe, ISP_CNR_MOTION_NR_ATTR_S *pstCNRMotionNRAttr);

//-----------------------------------------------------------------------------
//  CAC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCACAttr(VI_PIPE ViPipe, const ISP_CAC_ATTR_S *pstCACAttr);
CVI_S32 CVI_ISP_GetCACAttr(VI_PIPE ViPipe, ISP_CAC_ATTR_S *pstCACAttr);

//-----------------------------------------------------------------------------
//  Sharpen
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetSharpenAttr(VI_PIPE ViPipe, const ISP_SHARPEN_ATTR_S *pstSharpenAttr);
CVI_S32 CVI_ISP_GetSharpenAttr(VI_PIPE ViPipe, ISP_SHARPEN_ATTR_S *pstSharpenAttr);

//-----------------------------------------------------------------------------
//  Y Contrast
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetYContrastAttr(VI_PIPE ViPipe, const ISP_YCONTRAST_ATTR_S *pstYContrastAttr);
CVI_S32 CVI_ISP_GetYContrastAttr(VI_PIPE ViPipe, ISP_YCONTRAST_ATTR_S *pstYContrastAttr);

//-----------------------------------------------------------------------------
//  Mono
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetMonoAttr(VI_PIPE ViPipe, const ISP_MONO_ATTR_S *pstMonoAttr);
CVI_S32 CVI_ISP_GetMonoAttr(VI_PIPE ViPipe, ISP_MONO_ATTR_S *pstMonoAttr);


CVI_S32 CVI_ISP_SetStatisticsConfig(VI_PIPE ViPipe, const ISP_STATISTICS_CFG_S *pstStatCfg);
CVI_S32 CVI_ISP_GetStatisticsConfig(VI_PIPE ViPipe, ISP_STATISTICS_CFG_S *pstStatCfg);
CVI_S32 CVI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat);
CVI_S32 CVI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstWBStat);
CVI_S32 CVI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat);
CVI_S32 CVI_ISP_GetLightboxGain(VI_PIPE ViPipe, ISP_AWB_LightBox_Gain_S *pstAWBLightBoxGain);

CVI_S32 CVI_ISP_SetNoiseProfileAttr(VI_PIPE ViPipe, const ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr);
CVI_S32 CVI_ISP_GetNoiseProfileAttr(VI_PIPE ViPipe, ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr);

/* MIPI related function. */
CVI_S32 CVI_MIPI_SetMipiReset(CVI_S32 devno, CVI_U32 reset);
CVI_S32 CVI_MIPI_SetSensorClock(CVI_S32 devno, CVI_U32 enable);
CVI_S32 CVI_MIPI_SetSensorReset(CVI_S32 devno, CVI_U32 reset);
CVI_S32 CVI_MIPI_SetMipiAttr(CVI_S32 ViPipe, const CVI_VOID *devAttr);
CVI_S32 CVI_MIPI_SetClkEdge(CVI_S32 devno, CVI_U32 is_up);
CVI_S32 CVI_MIPI_SetSnsMclk(SNS_MCLK_S *mclk);

CVI_S32 CVI_ISP_QueryInnerStateInfo(VI_PIPE ViPipe, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);
CVI_S32 CVI_ISP_GetVDTimeOut(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, CVI_U32 u32MilliSec);
//CVI_S32 CVI_ISP_GetAEStitchStatistics(VI_PIPE ViPipe, ISP_AE_STITCH_STATISTICS_S *pstStitchStat);
//CVI_S32 CVI_ISP_GetMGStatistics(VI_PIPE ViPipe, ISP_MG_STATISTICS_S *pstMgStat);
//CVI_S32 CVI_ISP_GetWBStitchStatistics(VI_PIPE ViPipe, ISP_WB_STITCH_STATISTICS_S *pstStitchWBStat);

CVI_S32 CVI_ISP_SetDisAttr(VI_PIPE ViPipe, const ISP_DIS_ATTR_S *pstDisAttr);
CVI_S32 CVI_ISP_GetDisAttr(VI_PIPE ViPipe, ISP_DIS_ATTR_S *pstDisAttr);
CVI_S32 CVI_ISP_SetDisConfig(VI_PIPE ViPipe, const ISP_DIS_CONFIG_S *pstDisConfig);
CVI_S32 CVI_ISP_GetDisConfig(VI_PIPE ViPipe, ISP_DIS_CONFIG_S *pstDisConfig);

CVI_S32 CVI_ISP_SetVCAttr(VI_PIPE ViPipe, const ISP_VC_ATTR_S *pstVCAttr);
CVI_S32 CVI_ISP_GetVCAttr(VI_PIPE ViPipe, ISP_VC_ATTR_S *pstVCAttr);

CVI_S32 CVI_ISP_DumpHwRegisterToFile(VI_PIPE ViPipe, FILE *fp);
CVI_S32 CVI_ISP_DumpFrameRawInfoToFile(VI_PIPE ViPipe, FILE *fp);

CVI_S32 CVI_ISP_IrAutoRunOnce(ISP_DEV IspDev, ISP_IR_AUTO_ATTR_S *pstIrAttr);

CVI_S32 CVI_ISP_SetSmartInfo(VI_PIPE ViPipe, const ISP_SMART_INFO_S *pstSmartInfo, CVI_U8 TimeOut);
CVI_S32 CVI_ISP_GetSmartInfo(VI_PIPE ViPipe, ISP_SMART_INFO_S *pstSmartInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_ISP_H__ */
