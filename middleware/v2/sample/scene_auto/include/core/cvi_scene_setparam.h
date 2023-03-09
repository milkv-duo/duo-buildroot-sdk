#ifndef __CVI_SCENE_SETPARAM_H__
#define __CVI_SCENE_SETPARAM_H__

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include "cvi_common.h"
#elif defined(__CV181X__) || defined(__CV180X__)
#include <linux/cvi_common.h>
#endif // SOCs

#include "cvi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_ISP_PIPE_NUM							(6)
#define CVI_SCENE_PIPETYPE_NUM						(10)

#define CVI_SCENE_ISO_MAX_COUNT						(16)
#define CVI_SCENE_EXPOSURE_MAX_COUNT				(10)
#define CVI_SCENE_LV_MAX_COUNT						(21)
#define CVI_SCENE_EXPRATION_MAX_COUNT				(5)

#define CVI_SCENE_AE_EXPOSURE_MAX_COUNT				(12)
#define CVI_SCENE_THREEDNR_MAX_COUNT				(15)


typedef struct _SCENE_MODULE_STATE_S {
	CVI_BOOL bStaticAE;
	CVI_BOOL bStaticAWB;
	CVI_BOOL bStaticAWBEx;
	CVI_BOOL bStaticCCM;
	CVI_BOOL bStaticAERouteEx;
	CVI_BOOL bStaticCAC;
	CVI_BOOL bStaticWdrExposure;
	CVI_BOOL bStaticDehaze;
	CVI_BOOL bStaticStatistics;
	CVI_BOOL bStaticSaturation;
	CVI_BOOL bStaticShading;
	CVI_BOOL bDynamicAE;
	CVI_BOOL bDynamicThreeDNR;
	CVI_BOOL bDynamicGamma;
	CVI_BOOL bDynamicCAC;
	CVI_BOOL bDynamicColorTone;
	CVI_BOOL bDynamicShading;
	CVI_BOOL bDynamicDehaze;
	CVI_BOOL bDynamicFSWDR;
	CVI_BOOL bStaticSharpen;
	CVI_BOOL bStaticDemosaic;
	CVI_BOOL bDynamicDIS;
	CVI_BOOL bStaticDrc;
	CVI_BOOL bDynamicLinearDrc;
	CVI_BOOL bDynamicThreadDrc;
} CVI_SCENE_MODULE_STATE_S;

typedef struct _SCENE_STATIC_AE_S {
	CVI_BOOL bAERouteExValid;
	CVI_U8 u8AERunInterval;
	CVI_U32 u32AutoExpTimeMax;
	CVI_U32 u32AutoSysGainMax;
	CVI_U8 u8AutoSpeed;
	CVI_U8 u8AutoTolerance;
	CVI_U16 u16AutoBlackDelayFrame;
	CVI_U16 u16AutoWhiteDelayFrame;
} CVI_SCENE_STATIC_AE_S;

typedef struct _SCENE_STATIC_AWB_S {
} CVI_SCENE_STATIC_AWB_S;

typedef struct _SCENE_STATIC_AWBEX_S {
} CVI_SCENE_STATIC_AWBEX_S;

typedef struct _SCENE_STATIC_CCM_S {
	CVI_BOOL bISOActEn;
	CVI_BOOL bTempActEn;
} CVI_SCENE_STATIC_CCM_S;

typedef struct _SCENE_STATIC_AEROUTE_S {
} CVI_SCENE_STATIC_AEROUTE_S;

typedef struct _SCENE_STATIC_AEROUTEEX_S {
	CVI_U32 u32TotalNum;
	CVI_U32 au32IntTime[ISP_AE_ROUTE_EX_MAX_NODES];
	CVI_U32 au32Again[ISP_AE_ROUTE_EX_MAX_NODES];
	CVI_U32 au32Dgain[ISP_AE_ROUTE_EX_MAX_NODES];
	CVI_U32 au32IspDgain[ISP_AE_ROUTE_EX_MAX_NODES];
} CVI_SCENE_STATIC_AEROUTEEX_S;

typedef struct _SCENE_STATIC_CAC_S {
	CVI_BOOL bEnable;
	CVI_U8 u8OpType;
} CVI_SCENE_STATIC_CAC_S;

typedef struct _SCENE_STATIC_WDREXPOSURE_S {
	CVI_U8 u8ExpRatioType;
	CVI_U32 u32ExpRatioMax;
	CVI_U32 u32ExpRatioMin;
	CVI_U32 au32ExpRatio[WDR_EXP_RATIO_NUM];
} CVI_SCENE_STATIC_WDREXPOSURE_S;

typedef struct _SCENE_STATIC_DEHAZE_S {
	CVI_BOOL bEnable;
	CVI_U8 u8OpType;
} CVI_SCENE_STATIC_DEHAZE_S;

typedef struct _SCENE_STATIC_STATISTICSCFG_S {
	CVI_U8 au8AEWeight[AE_WEIGHT_ZONE_ROW][AE_WEIGHT_ZONE_COLUMN];
} CVI_SCENE_STATIC_STATISTICSCFG_S;

typedef struct _SCENE_STATIC_CSC_S {
} CVI_SCENE_STATIC_CSC_S;

typedef struct _SCENE_STATIC_SATURATION_S {
	CVI_U8 au8AutoSat[ISP_AUTO_ISO_STRENGTH_NUM];
} CVI_SCENE_STATIC_SATURATION_S;

typedef struct _SCENE_STATIC_SHADING_S {
	CVI_BOOL bEnable;
} CVI_SCENE_STATIC_SHADING_S;

typedef struct _SCENE_DYNAMIC_THREEDNR_S {
} CVI_SCENE_DYNAMIC_THREEDNR_S;

typedef struct _SCENE_DYNAMIC_AE_S {
	CVI_U8 u8AEExposureCnt;
	CVI_U64 au64ExpLtoHThresh[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U64 au64ExpHtoLThresh[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U8 au8AutoCompensation[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U8 au8AutoMaxHistOffset[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U32 au32AutoExpRatioMax[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U32 au32AutoExpRatioMin[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
	CVI_U8 au8AutoAntiflickerEn[CVI_SCENE_AE_EXPOSURE_MAX_COUNT];
} CVI_SCENE_DYNAMIC_AE_S;

typedef struct _SCENE_DYNAMIC_GAMMA_S {
	CVI_U32 u32InterVal;
	CVI_U32 u32TotalNum;
	CVI_U64 au64ExpThreshLtoH[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U64 au64ExpThreshHtoL[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U16 au16Table[CVI_SCENE_EXPOSURE_MAX_COUNT][GAMMA_NODE_NUM];
} CVI_SCENE_DYNAMIC_GAMMA_S;

typedef struct _SCENE_DYNAMIC_CAC_S {
	CVI_U32 u32ISOCount;
	CVI_U32 au32ISOLevel[CVI_SCENE_ISO_MAX_COUNT];
	CVI_U8 au8PurpleDetRange[CVI_SCENE_ISO_MAX_COUNT];
	CVI_U16 au16VarThr[CVI_SCENE_ISO_MAX_COUNT];
	CVI_U8 au8DePurpleStr[CVI_SCENE_ISO_MAX_COUNT];
} CVI_SCENE_DYNAMIC_CAC_S;

typedef struct _SCENE_DYNAMIC_COLORTONE_S {
	CVI_U32 u32ExpThreshCnt;
	CVI_U64 au64ExpThreshLtoH[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U16 au16RedCastGain[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U16 au16GreenCastGain[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U16 au16BlueCastGain[CVI_SCENE_EXPOSURE_MAX_COUNT];
} CVI_SCENE_DYNAMIC_COLORTONE_S;

typedef struct _SCENE_DYNAMIC_SHADING_S {
	CVI_U32 u32ExpThreshCnt;
	CVI_U64 au64ExpThreshLtoH[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U16 au16ManualStrength[CVI_SCENE_EXPOSURE_MAX_COUNT];
} CVI_SCENE_DYNAMIC_SHADING_S;

typedef struct _SCENE_DYNAMIC_DEHAZE_S {
	CVI_U32 u32ExpThreshCnt;
	CVI_U64 au64ExpThreshLtoH[CVI_SCENE_EXPOSURE_MAX_COUNT];
	CVI_U8 au8ManualStrength[CVI_SCENE_EXPOSURE_MAX_COUNT];
} CVI_SCENE_DYNAMIC_DEHAZE_S;

typedef struct _SCENE_DYNAMIC_FSWDR_S {
	CVI_U32 u32ExpCnt;
	CVI_U32 au32ExpRation[CVI_SCENE_EXPRATION_MAX_COUNT];
	CVI_U8 au8MotionComp[CVI_SCENE_EXPRATION_MAX_COUNT];
} CVI_SCENE_DYNAMIC_FSWDR_S;

typedef struct _SCENE_STATIC_SHARPEN_S {
} CVI_SCENE_STATIC_SHARPEN_S;

typedef struct _SCENE_STATIC_DEMOSAIC_S {
} CVI_SCENE_STATIC_DEMOSAIC_S;

typedef struct _SCENE_DYNAMIC_DIS_S {
} CVI_SCENE_DYNAMIC_DIS_S;

typedef struct _SCENE_STATIC_DRC_S {
	CVI_BOOL bEnable;
	CVI_U8 u8OpType;
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	CVI_U16 u16LinearStart;
	CVI_U16 u16LinearEnd;
#endif // SOCs
	CVI_U32 u32ToneCurveSmooth;
} CVI_SCENE_STATIC_DRC_S;

typedef struct _SCENE_DYNAMIC_LINEARDRC_S {
	CVI_U32 u32LVCount;
	CVI_S32 as32LVLevel[CVI_SCENE_LV_MAX_COUNT];
	CVI_U8 au8SdrGlobalTargetYScale[CVI_SCENE_LV_MAX_COUNT];
	CVI_U8 au8SdrDarkTargetYScale[CVI_SCENE_LV_MAX_COUNT];
} CVI_SCENE_DYNAMIC_LINEARDRC_S;

typedef struct _SCENE_THREAD_DRC_S {
	CVI_U32 u32LVCount;
	CVI_S32 as32LVLevel[CVI_SCENE_LV_MAX_COUNT];
	CVI_U32 au32TargetYScale[CVI_SCENE_LV_MAX_COUNT];
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	CVI_U16 au16DarkOffset[CVI_SCENE_LV_MAX_COUNT];
	CVI_U16 au16DarkToneRange[CVI_SCENE_LV_MAX_COUNT];
	CVI_U32 au32LETargetYScale[CVI_SCENE_LV_MAX_COUNT];
#endif // SOCs
	CVI_U16 au16HdrStrength[CVI_SCENE_LV_MAX_COUNT];
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	CVI_U16 au16ContrastGain[CVI_SCENE_LV_MAX_COUNT];
#endif // SOCs
} CVI_SCENE_THREAD_DRC_S;

typedef struct _SCENE_PIPE_PARAM_S {
	CVI_SCENE_STATIC_AE_S stStaticAe;
	CVI_SCENE_STATIC_AEROUTE_S stStaticAeRoute;
	CVI_SCENE_STATIC_AEROUTEEX_S stStaticAeRouteEx;
	CVI_SCENE_STATIC_CAC_S stStaticCAC;
	CVI_SCENE_STATIC_WDREXPOSURE_S stStaticWdrExposure;
	CVI_SCENE_STATIC_DRC_S stStaticDrc;
	CVI_SCENE_STATIC_DEHAZE_S stStaticDehaze;
	CVI_SCENE_STATIC_STATISTICSCFG_S stStaticStatistics;
	CVI_SCENE_STATIC_AWB_S stStaticAwb;
	CVI_SCENE_STATIC_AWBEX_S stStaticAwbEx;
	CVI_SCENE_STATIC_CCM_S stStaticCcm;
	CVI_SCENE_STATIC_CSC_S stStaticCsc;
	CVI_SCENE_STATIC_SHADING_S stStaticShading;
	CVI_SCENE_STATIC_SATURATION_S stStaticSaturation;
	CVI_SCENE_STATIC_SHARPEN_S stStaticSharpen;
	CVI_SCENE_STATIC_DEMOSAIC_S stStaticDemosaic;
	CVI_SCENE_DYNAMIC_THREEDNR_S stDynamicThreeDNR;
	CVI_SCENE_DYNAMIC_AE_S stDynamicAe;
	CVI_SCENE_DYNAMIC_SHADING_S stDynamicShading;
	CVI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze;
	CVI_SCENE_DYNAMIC_FSWDR_S stDynamicFSWDR;
	CVI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma;
	CVI_SCENE_DYNAMIC_CAC_S stDynamicCAC;
	CVI_SCENE_DYNAMIC_COLORTONE_S stDynamicColorTone;
	CVI_SCENE_DYNAMIC_DIS_S stDynamicDis;
	CVI_SCENE_DYNAMIC_LINEARDRC_S stDynamicLinearDrc;
	CVI_SCENE_THREAD_DRC_S stThreadDrc;
	CVI_SCENE_MODULE_STATE_S stModuleState;
} CVI_SCENE_PIPE_PARAM_S;

CVI_S32 CVI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticAERouteEX_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticCAC_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticWDRExposure_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticDRC_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticDehaze_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticAWB_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticAWBEX_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticSaturation_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticCCM_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticStatisticsCfg_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index,
	CVI_BOOL bFixedAEMatrix);
CVI_S32 CVI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticSharpen_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetStaticDemosaic_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index);

CVI_S32 CVI_SCENE_SetDynamicCAC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicThreeDNR_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicVideoGamma_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicShading_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicColorTone_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicDehaze_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicFsWdr_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index, CVI_U32 u32ActRation);
CVI_S32 CVI_SCENE_SetDynamicBayernr_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_U8 u8Index, CVI_BOOL bEnable);
CVI_S32 CVI_SCENE_SetDynamicLinearDRC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_S32 s32LV, CVI_S32 s32LastLV,
	CVI_U8 u8Index);
CVI_S32 CVI_SCENE_SetDynamicThreadDRC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_S32 s32LV, CVI_S32 s32LastLV,
	CVI_U32 u32ActRation, CVI_U8 u8Index);

CVI_S32 CVI_SCENE_SetPipeParam_AutoGenerate(
	const CVI_SCENE_PIPE_PARAM_S * pstScenePipeParam, CVI_U32 u32Num);
CVI_S32 CVI_SCENE_SetPause_AutoGenerate(CVI_BOOL bPause);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENE_SETPARAM_H__
