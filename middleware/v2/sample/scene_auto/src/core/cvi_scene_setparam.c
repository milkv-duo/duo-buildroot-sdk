#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cvi_ae.h"
#include "cvi_awb.h"
#include "cvi_isp.h"
#include "cvi_scene_comm.h"

#include "cvi_scene_setparam.h"

#define SCENE_DIV_0TO1(a)	((0 == (a)) ? 1 : (a))

// #define DEVELOP_DEBUG
#define APPLY_PARAMETER_TO_SYSTEM

#ifdef DEVELOP_DEBUG
#define PRINT_FUNCTION()	printf("%s\n", __func__)
#else
#define PRINT_FUNCTION()
#endif // DEVELOP_DEBUG

#define GAMMA_UPDATE_TIME_GAP_PER_STEP		(30000)

CVI_BOOL g_bISPPause;
CVI_SCENE_PIPE_PARAM_S g_astScenePipeParam[CVI_SCENE_PIPETYPE_NUM];

#define CHECK_SCENE_PAUSE() \
	do { \
		if (g_bISPPause == CVI_TRUE) { \
			return CVI_SUCCESS; \
		} \
	} while (0)

#define CHECK_SCENE_RET(s32Ret) \
	do { \
		if (s32Ret != CVI_SUCCESS) { \
			printf("Fail at %s: LINE: %d with %#x\n", __func__, __LINE__, s32Ret); \
		} \
	} while (0)

static CVI_U32 SCENE_GetLevelLtoH(CVI_U64 u64Value, CVI_U32 u32Count, CVI_U64 *pu64Thresh)
{
	CVI_U32 u32Level = 0;

	for (u32Level = 0; u32Level < u32Count; u32Level++) {
		if (u64Value <= pu64Thresh[u32Level]) {
			break;
		}
	}

	if (u32Level == u32Count) {
		u32Level = u32Count - 1;
	}

	return u32Level;
}

static CVI_U32 SCENE_GetLevelHtoL(CVI_U64 u64Value, CVI_U32 u32Count, CVI_U64 *pu64Thresh)
{
	CVI_U32 u32Level = 0;

	for (u32Level = u32Count; u32Level > 0; u32Level--) {
		if (u64Value > pu64Thresh[u32Level - 1]) {
			break;
		}
	}

	if (u32Level > 0) {
		u32Level = u32Level - 1;
	}

	return u32Level;
}

static CVI_U32 SCENE_GetLevelLtoH_U32(CVI_U32 u32Value, CVI_U32 u32Count, CVI_U32 *pu32Thresh)
{
	CVI_U32 u32Level = 0;

	for (u32Level = 0; u32Level < u32Count; u32Level++) {
		if (u32Value <= pu32Thresh[u32Level]) {
			break;
		}
	}

	if (u32Level == u32Count) {
		u32Level = u32Count - 1;
	}

	return u32Level;
}

static CVI_U32 SCENE_GetLevelHtoL_U32(CVI_U32 u32Value, CVI_U32 u32Count, CVI_U32 *pu32Thresh)
{
	CVI_U32 u32Level = 0;

	for (u32Level = u32Count; u32Level > 0; u32Level--) {
		if (u32Value > pu32Thresh[u32Level - 1]) {
			break;
		}
	}

	if (u32Level > 0) {
		u32Level = u32Level - 1;
	}

	return u32Level;
}

static CVI_U32 SCENE_GetLevelLtoH_S32(CVI_S32 s32Value, CVI_U32 u32Count, CVI_S32 *ps32Thresh)
{
	CVI_U32 u32Level = 0;

	for (u32Level = 0; u32Level < u32Count; u32Level++) {
		if (s32Value <= ps32Thresh[u32Level]) {
			break;
		}
	}

	if (u32Level == u32Count) {
		u32Level = u32Count - 1;
	}

	return u32Level;
}

static CVI_U32 SCENE_Interpulate(CVI_U32 u32Mid,
	CVI_U32 u32Left, CVI_U32 u32LValue, CVI_U32 u32Right, CVI_U32 u32RValue)
{
	CVI_U32 u32Value = 0;
	CVI_U32 k = 0;

	if (u32Mid <= u32Left) {
		u32Value = u32LValue;
		return u32Value;
	}

	if (u32Mid >= u32Right) {
		u32Value = u32RValue;
		return u32Value;
	}

	k = (u32Right - u32Left);
	u32Value = (((u32Right - u32Mid) * u32LValue + (u32Mid - u32Left) * u32RValue + (k >> 1)) / k);

	return u32Value;
}

#if 0
static CVI_U8 SCENE_Interpulate_S32_U8(CVI_S32 s32Mid,
	CVI_S32 s32Left, CVI_U8 u8LValue, CVI_S32 s32Right, CVI_U8 u8RValue)
{
	CVI_U32 u32LeftWeight, u32RightWeight;
	CVI_U32 u32Length = 0;
	CVI_U32 u32Value = 0;
	CVI_U8 u8Value = 0;

	if (s32Mid <= s32Left) {
		u8Value = u8LValue;
		return u8Value;
	}

	if (s32Mid >= s32Right) {
		u8Value = u8RValue;
		return u8Value;
	}

	u32Length = s32Right - s32Left;
	u32LeftWeight = s32Right - s32Mid;
	u32RightWeight = s32Mid - s32Left;

	u32Value = ((u32LeftWeight * (CVI_U32)u8LValue) + (u32RightWeight * (CVI_U32)u8RValue) + (u32Length / 2))
		/ u32Length;
	u8Value = (u32Value > 255) ? 255 : (CVI_U8)u32Value;

	return u8Value;
}
#endif //

static CVI_U32 SCENE_Interpulate_S32_U32(CVI_S32 s32Mid,
	CVI_S32 s32Left, CVI_U32 u32LValue, CVI_S32 s32Right, CVI_U32 u32RValue)
{
	CVI_U32 u32LeftWeight, u32RightWeight;
	CVI_U32 u32Length = 0;
	CVI_U32 u32Value = 0;

	if (s32Mid <= s32Left) {
		u32Value = u32LValue;
		return u32Value;
	}

	if (s32Mid >= s32Right) {
		u32Value = u32RValue;
		return u32Value;
	}

	u32Length = s32Right - s32Left;
	u32LeftWeight = s32Right - s32Mid;
	u32RightWeight = s32Mid - s32Left;

	u32Value = ((u32LeftWeight * u32LValue) + (u32RightWeight * u32RValue) + (u32Length / 2)) / u32Length;

	return u32Value;
}

static CVI_U32 SCENE_TimeFilter(CVI_U32 u32Para0, CVI_U32 u32Para1, CVI_U32 u32TimeCnt, CVI_U32 u32Index)
{
	CVI_U64 u64Temp = 0;
	CVI_U32 u32Value = 0;

	if (u32Para0 > u32Para1) {
		u64Temp = (CVI_U64)(u32Para0 - u32Para1) << 8;
		u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
		u32Value = u32Para0 - (CVI_U32)u64Temp - 1;
	} else if (u32Para0 < u32Para1) {
		u64Temp = (CVI_U64)(u32Para1 - u32Para0) << 8;
		u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
		u32Value = u32Para0 + (CVI_U32)u64Temp + 1;
	} else {
		u32Value = u32Para0;
	}

	return u32Value;
}

CVI_S32 CVI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticAE != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_STATIC_AE_S *pstStaticAe = &(g_astScenePipeParam[u8Index].stStaticAe);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_EXPOSURE_ATTR_S stExposureAttr;

	s32Ret = CVI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
	CHECK_SCENE_RET(s32Ret);

	// stExposureAttr.bAERouteExValid = pstStaticAe->bAERouteExValid;			// segment fault
	stExposureAttr.u8AERunInterval = pstStaticAe->u8AERunInterval;
	stExposureAttr.stAuto.stExpTimeRange.u32Max = pstStaticAe->u32AutoExpTimeMax;
	stExposureAttr.stAuto.stSysGainRange.u32Max = pstStaticAe->u32AutoSysGainMax;
	stExposureAttr.stAuto.u8Speed = pstStaticAe->u8AutoSpeed;
	stExposureAttr.stAuto.u8Tolerance = pstStaticAe->u8AutoTolerance;
	stExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = pstStaticAe->u16AutoBlackDelayFrame;
	stExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = pstStaticAe->u16AutoWhiteDelayFrame;

	s32Ret = CVI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticAWB_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticAWB != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_WB_ATTR_S stWbAttr;

	s32Ret = CVI_ISP_GetWBAttr(ViPipe, &stWbAttr);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	s32Ret = CVI_ISP_SetWBAttr(ViPipe, &stWbAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticAWBEX_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticAWBEx != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_AWB_ATTR_EX_S stAwbAttrEx;

	s32Ret = CVI_ISP_GetAWBAttrEx(ViPipe, &stAwbAttrEx);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	s32Ret = CVI_ISP_SetAWBAttrEx(ViPipe, &stAwbAttrEx);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticAERouteEX_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticAERouteEx != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_STATIC_AEROUTEEX_S *pstStaticAeRouteEx = &(g_astScenePipeParam[u8Index].stStaticAeRouteEx);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_AE_ROUTE_EX_S stAeRouteEx;

	s32Ret = CVI_ISP_GetAERouteAttrEx(ViPipe, &stAeRouteEx);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stAeRouteEx.u32TotalNum = pstStaticAeRouteEx->u32TotalNum;

	for (CVI_U32 i = 0; i < stAeRouteEx.u32TotalNum; i++) {
		stAeRouteEx.astRouteExNode[i].u32IntTime = pstStaticAeRouteEx->au32IntTime[i];
		stAeRouteEx.astRouteExNode[i].u32Again = pstStaticAeRouteEx->au32Again[i];
		stAeRouteEx.astRouteExNode[i].u32Dgain = pstStaticAeRouteEx->au32Dgain[i];
		stAeRouteEx.astRouteExNode[i].u32IspDgain = pstStaticAeRouteEx->au32IspDgain[i];
	}
#endif

	s32Ret = CVI_ISP_SetAERouteAttrEx(ViPipe, &stAeRouteEx);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticCAC_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticCAC != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_CAC_ATTR_S stCACAttr;

	s32Ret = CVI_ISP_GetCACAttr(ViPipe, &stCACAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stCACAttr.Enable = g_astScenePipeParam[u8Index].stStaticCAC.bEnable;
	stCACAttr.enOpType = g_astScenePipeParam[u8Index].stStaticCAC.u8OpType;
#endif

	s32Ret = CVI_ISP_SetCACAttr(ViPipe, &stCACAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticWDRExposure_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticWdrExposure != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_STATIC_WDREXPOSURE_S *pstStaticWdrExposure = &(g_astScenePipeParam[u8Index].stStaticWdrExposure);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;

	s32Ret = CVI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)pstStaticWdrExposure->u8ExpRatioType;
	stWdrExposureAttr.u32ExpRatioMax = pstStaticWdrExposure->u32ExpRatioMax;
	stWdrExposureAttr.u32ExpRatioMin = pstStaticWdrExposure->u32ExpRatioMin;

	for (CVI_U32 i = 0; i < WDR_EXP_RATIO_NUM; i++) {
		stWdrExposureAttr.au32ExpRatio[i] = pstStaticWdrExposure->au32ExpRatio[i];
	}
#endif

	s32Ret = CVI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticDRC_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticDrc != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_STATIC_DRC_S *pstStaticDrc = &(g_astScenePipeParam[u8Index].stStaticDrc);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_DRC_ATTR_S stDrcAttr;

	s32Ret = CVI_ISP_GetDRCAttr(ViPipe, &stDrcAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stDrcAttr.Enable = pstStaticDrc->bEnable;
	stDrcAttr.enOpType = (ISP_OP_TYPE_E)pstStaticDrc->u8OpType;
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	stDrcAttr.LinearStart = pstStaticDrc->u16LinearStart;
	stDrcAttr.LinearEnd = pstStaticDrc->u16LinearEnd;
#endif // SOCs
	stDrcAttr.ToneCurveSmooth = pstStaticDrc->u32ToneCurveSmooth;
#endif

	s32Ret = CVI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticDehaze_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticDehaze != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_DEHAZE_ATTR_S stDehazeAttr;

	s32Ret = CVI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stDehazeAttr.Enable = g_astScenePipeParam[u8Index].stStaticDehaze.bEnable;
	stDehazeAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticDehaze.u8OpType;
#endif

	s32Ret = CVI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticStatisticsCfg_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index, CVI_BOOL bFixedAEMatrix)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticStatistics != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (bFixedAEMatrix == CVI_TRUE) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_STATIC_STATISTICSCFG_S *pstStaticStatistics = &(g_astScenePipeParam[u8Index].stStaticStatistics);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_STATISTICS_CFG_S stStatisticsCfg;

	s32Ret = CVI_ISP_GetStatisticsConfig(ViPipe, &stStatisticsCfg);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	for (CVI_U32 i = 0; i < AE_WEIGHT_ZONE_ROW; i++) {
		for (CVI_U32 j = 0; j < AE_WEIGHT_ZONE_COLUMN; j++) {
			stStatisticsCfg.stAECfg.au8Weight[i][j] = pstStaticStatistics->au8AEWeight[i][j];
		}
	}
#endif

	s32Ret = CVI_ISP_SetStatisticsConfig(ViPipe, &stStatisticsCfg);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticSaturation_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticSaturation != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_STATIC_SATURATION_S *pstStaticSaturation = &(g_astScenePipeParam[u8Index].stStaticSaturation);
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_SATURATION_ATTR_S stSaturationAttr;

	s32Ret = CVI_ISP_GetSaturationAttr(ViPipe, &stSaturationAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		stSaturationAttr.stAuto.Saturation[i] = pstStaticSaturation->au8AutoSat[i];
	}
#endif

	s32Ret = CVI_ISP_SetSaturationAttr(ViPipe, &stSaturationAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticCCM_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticCCM != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_CCM_ATTR_S stCCMAttr;

	s32Ret = CVI_ISP_GetCCMAttr(ViPipe, &stCCMAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stCCMAttr.stAuto.ISOActEnable = g_astScenePipeParam[u8Index].stStaticCcm.bISOActEn;
	stCCMAttr.stAuto.TempActEnable = g_astScenePipeParam[u8Index].stStaticCcm.bTempActEn;
#endif

	s32Ret = CVI_ISP_SetCCMAttr(ViPipe, &stCCMAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticShading != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_MESH_SHADING_ATTR_S stMeshShadingAttr;

	s32Ret = CVI_ISP_GetMeshShadingAttr(ViPipe, &stMeshShadingAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stMeshShadingAttr.Enable = g_astScenePipeParam[u8Index].stStaticShading.bEnable;
#endif

	s32Ret = CVI_ISP_SetMeshShadingAttr(ViPipe, &stMeshShadingAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicVideoGamma_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u64Exposure == u64LastExposure) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_GAMMA_S *pstDynamicGamma = &(g_astScenePipeParam[u8Index].stDynamicGamma);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpLevel = 0;
	static CVI_U32 u32LastExpLevel;

	ISP_GAMMA_ATTR_S stIspGammaAttr;

	if (u64Exposure > u64LastExposure) {
		u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure,
			pstDynamicGamma->u32TotalNum, pstDynamicGamma->au64ExpThreshLtoH);
	} else {
		u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure,
			pstDynamicGamma->u32TotalNum, pstDynamicGamma->au64ExpThreshHtoL);
	}

#ifdef APPLY_PARAMETER_TO_SYSTEM
	for (CVI_U32 u32IntervalIdx = 0; u32IntervalIdx < pstDynamicGamma->u32InterVal; ++u32IntervalIdx) {
		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_GetGammaAttr(ViPipe, &stIspGammaAttr);
		CHECK_SCENE_RET(s32Ret);

		for (CVI_U32 u32NodeIdx = 0; u32NodeIdx < GAMMA_NODE_NUM; ++u32NodeIdx) {
			stIspGammaAttr.Table[u32NodeIdx] = SCENE_TimeFilter(
				pstDynamicGamma->au16Table[u32LastExpLevel][u32NodeIdx],
				pstDynamicGamma->au16Table[u32ExpLevel][u32NodeIdx],
				pstDynamicGamma->u32InterVal, u32IntervalIdx);
		}

		stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;

		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_SetGammaAttr(ViPipe, &stIspGammaAttr);
		CHECK_SCENE_RET(s32Ret);

		usleep(GAMMA_UPDATE_TIME_GAP_PER_STEP);
	}
#endif

	u32LastExpLevel = u32ExpLevel;

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicThreeDNR_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicThreeDNR != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u32ISO == u32LastISO) {
		return CVI_SUCCESS;
	}

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_TNR_ATTR_S stTNRAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetTNRAttr(ViPipe, &stTNRAttr);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetTNRAttr(ViPipe, &stTNRAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicCAC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicCAC != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u32ISO == u32LastISO) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_CAC_S *pstDynamicCAC = &(g_astScenePipeParam[u8Index].stDynamicCAC);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32IsoLevel, u32NextIsoLevel;

	ISP_CAC_ATTR_S stCACAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetCACAttr(ViPipe, &stCACAttr);
	CHECK_SCENE_RET(s32Ret);

	u32IsoLevel = SCENE_GetLevelHtoL_U32(u32ISO,
		pstDynamicCAC->u32ISOCount, pstDynamicCAC->au32ISOLevel);

	if ((pstDynamicCAC->u32ISOCount - 1) == u32IsoLevel) {
		u32NextIsoLevel = u32IsoLevel;
	} else {
		u32NextIsoLevel = u32IsoLevel + 1;
	}

#ifdef APPLY_PARAMETER_TO_SYSTEM
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	stCACAttr.VarThr = SCENE_Interpulate(u32ISO,
		pstDynamicCAC->au32ISOLevel[u32IsoLevel],
		pstDynamicCAC->au16VarThr[u32IsoLevel],
		pstDynamicCAC->au32ISOLevel[u32NextIsoLevel],
		pstDynamicCAC->au16VarThr[u32NextIsoLevel]);
#endif // SOCs
	stCACAttr.PurpleDetRange = SCENE_Interpulate(u32ISO,
		pstDynamicCAC->au32ISOLevel[u32IsoLevel],
		pstDynamicCAC->au8PurpleDetRange[u32IsoLevel],
		pstDynamicCAC->au32ISOLevel[u32NextIsoLevel],
		pstDynamicCAC->au8PurpleDetRange[u32NextIsoLevel]);

	stCACAttr.stManual.DePurpleStr = SCENE_Interpulate(u32ISO,
		pstDynamicCAC->au32ISOLevel[u32IsoLevel],
		pstDynamicCAC->au8DePurpleStr[u32IsoLevel],
		pstDynamicCAC->au32ISOLevel[u32NextIsoLevel],
		pstDynamicCAC->au8DePurpleStr[u32NextIsoLevel]);
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetCACAttr(ViPipe, &stCACAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicAE != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u64Exposure == u64LastExposure) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_AE_S *pstDynamicAe = &(g_astScenePipeParam[u8Index].stDynamicAe);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpLevel = 0;

	ISP_EXPOSURE_ATTR_S stExposureAttr;
	ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;

	if (u64Exposure > u64LastExposure) {
		u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure,
			pstDynamicAe->u8AEExposureCnt, pstDynamicAe->au64ExpLtoHThresh);
	} else {
		u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure,
			pstDynamicAe->u8AEExposureCnt, pstDynamicAe->au64ExpHtoLThresh);
	}

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	if (u32ExpLevel == (CVI_U32)(pstDynamicAe->u8AEExposureCnt - 1)) {
		stExposureAttr.stAuto.u8Compensation = pstDynamicAe->au8AutoCompensation[u32ExpLevel];
		stExposureAttr.stAuto.u8MaxHistOffset = pstDynamicAe->au8AutoMaxHistOffset[u32ExpLevel];
		stExposureAttr.stAuto.stAntiflicker.bEnable = (CVI_BOOL)pstDynamicAe->au8AutoAntiflickerEn[u32ExpLevel];
	} else {
		stExposureAttr.stAuto.u8Compensation = SCENE_Interpulate(u64Exposure,
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel],
			pstDynamicAe->au8AutoCompensation[u32ExpLevel],
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel + 1],
			pstDynamicAe->au8AutoCompensation[u32ExpLevel + 1]);
		stExposureAttr.stAuto.u8MaxHistOffset = SCENE_Interpulate(u64Exposure,
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel],
			pstDynamicAe->au8AutoMaxHistOffset[u32ExpLevel],
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel + 1],
			pstDynamicAe->au8AutoMaxHistOffset[u32ExpLevel + 1]);
		stExposureAttr.stAuto.stAntiflicker.bEnable = (CVI_BOOL)SCENE_Interpulate(u64Exposure,
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel],
			pstDynamicAe->au8AutoAntiflickerEn[u32ExpLevel],
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel + 1],
			pstDynamicAe->au8AutoAntiflickerEn[u32ExpLevel + 1]);
	}
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
	CHECK_SCENE_RET(s32Ret);

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
	CHECK_SCENE_RET(s32Ret);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	if (u32ExpLevel == (CVI_U32)(pstDynamicAe->u8AEExposureCnt - 1)) {
		stWdrExposureAttr.u32ExpRatioMax = pstDynamicAe->au32AutoExpRatioMax[u32ExpLevel];
		stWdrExposureAttr.u32ExpRatioMin = pstDynamicAe->au32AutoExpRatioMin[u32ExpLevel];
	} else {
		stWdrExposureAttr.u32ExpRatioMax = SCENE_Interpulate(u64Exposure,
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel],
			pstDynamicAe->au32AutoExpRatioMax[u32ExpLevel],
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel + 1],
			pstDynamicAe->au32AutoExpRatioMax[u32ExpLevel + 1]);
		stWdrExposureAttr.u32ExpRatioMin = SCENE_Interpulate(u64Exposure,
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel],
			pstDynamicAe->au32AutoExpRatioMin[u32ExpLevel],
			pstDynamicAe->au64ExpLtoHThresh[u32ExpLevel + 1],
			pstDynamicAe->au32AutoExpRatioMin[u32ExpLevel + 1]);
	}
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicShading_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicShading != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u64Exposure == u64LastExposure) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_SHADING_S *pstDynamicShading = &(g_astScenePipeParam[u8Index].stDynamicShading);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpLevel = 0;

	ISP_MESH_SHADING_ATTR_S stMeshShadingAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetMeshShadingAttr(ViPipe, &stMeshShadingAttr);
	CHECK_SCENE_RET(s32Ret);

	u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure,
		pstDynamicShading->u32ExpThreshCnt, pstDynamicShading->au64ExpThreshLtoH);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	if ((u32ExpLevel == 0) || (u32ExpLevel >= (pstDynamicShading->u32ExpThreshCnt - 1))) {
		stMeshShadingAttr.stManual.MeshStr = pstDynamicShading->au16ManualStrength[u32ExpLevel];
	} else {
		stMeshShadingAttr.stManual.MeshStr = SCENE_Interpulate(u64Exposure,
			pstDynamicShading->au64ExpThreshLtoH[u32ExpLevel - 1],
			pstDynamicShading->au16ManualStrength[u32ExpLevel - 1],
			pstDynamicShading->au64ExpThreshLtoH[u32ExpLevel],
			pstDynamicShading->au16ManualStrength[u32ExpLevel]);
	}
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetMeshShadingAttr(ViPipe, &stMeshShadingAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicColorTone_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicColorTone != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u64Exposure == u64LastExposure) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_COLORTONE_S *pstDynamicColorTone = &(g_astScenePipeParam[u8Index].stDynamicColorTone);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpLevel = 0;

	ISP_COLOR_TONE_ATTR_S stColorToneAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetColorToneAttr(ViPipe, &stColorToneAttr);
	CHECK_SCENE_RET(s32Ret);

	u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure,
		pstDynamicColorTone->u32ExpThreshCnt, pstDynamicColorTone->au64ExpThreshLtoH);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	if ((u32ExpLevel == 0) || (u32ExpLevel >= (pstDynamicColorTone->u32ExpThreshCnt - 1))) {
		stColorToneAttr.u16RedCastGain = pstDynamicColorTone->au16RedCastGain[u32ExpLevel];
		stColorToneAttr.u16GreenCastGain = pstDynamicColorTone->au16GreenCastGain[u32ExpLevel];
		stColorToneAttr.u16BlueCastGain = pstDynamicColorTone->au16BlueCastGain[u32ExpLevel];
	} else {
		stColorToneAttr.u16RedCastGain = SCENE_Interpulate(u64Exposure,
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel - 1],
			pstDynamicColorTone->au16RedCastGain[u32ExpLevel - 1],
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel],
			pstDynamicColorTone->au16RedCastGain[u32ExpLevel]);

		stColorToneAttr.u16GreenCastGain = SCENE_Interpulate(u64Exposure,
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel - 1],
			pstDynamicColorTone->au16GreenCastGain[u32ExpLevel - 1],
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel],
			pstDynamicColorTone->au16GreenCastGain[u32ExpLevel]);

		stColorToneAttr.u16BlueCastGain = SCENE_Interpulate(u64Exposure,
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel - 1],
			pstDynamicColorTone->au16BlueCastGain[u32ExpLevel - 1],
			pstDynamicColorTone->au64ExpThreshLtoH[u32ExpLevel],
			pstDynamicColorTone->au16BlueCastGain[u32ExpLevel]);
	}
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetColorToneAttr(ViPipe, &stColorToneAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicDehaze_AutoGenerate(VI_PIPE ViPipe,
	CVI_U64 u64Exposure, CVI_U64 u64LastExposure, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicDehaze != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u64Exposure == u64LastExposure) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_DEHAZE_S *pstDynamicDehaze = &(g_astScenePipeParam[u8Index].stDynamicDehaze);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpLevel = 0;

	ISP_DEHAZE_ATTR_S stDehazeAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
	CHECK_SCENE_RET(s32Ret);

	u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure,
		pstDynamicDehaze->u32ExpThreshCnt, pstDynamicDehaze->au64ExpThreshLtoH);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	if ((u32ExpLevel == 0) || (u32ExpLevel >= (pstDynamicDehaze->u32ExpThreshCnt - 1))) {
		stDehazeAttr.stManual.Strength = pstDynamicDehaze->au8ManualStrength[u32ExpLevel];
	} else {
		stDehazeAttr.stManual.Strength = SCENE_Interpulate(u64Exposure,
			pstDynamicDehaze->au64ExpThreshLtoH[u32ExpLevel - 1],
			pstDynamicDehaze->au8ManualStrength[u32ExpLevel - 1],
			pstDynamicDehaze->au64ExpThreshLtoH[u32ExpLevel],
			pstDynamicDehaze->au8ManualStrength[u32ExpLevel]);
	}
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicFsWdr_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32ISO, CVI_U32 u32LastISO, CVI_U8 u8Index, CVI_U32 u32ActRation)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicFSWDR != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (u32ISO == u32LastISO) {
		return CVI_SUCCESS;
	}

	CVI_SCENE_DYNAMIC_FSWDR_S *pstDynamicFSWDR = &(g_astScenePipeParam[u8Index].stDynamicFSWDR);
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32ExpRationLevel = 0;

	ISP_FSWDR_ATTR_S stFSWDRAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetFSWDRAttr(ViPipe, &stFSWDRAttr);
	CHECK_SCENE_RET(s32Ret);

	u32ExpRationLevel = SCENE_GetLevelLtoH_U32(u32ActRation,
		pstDynamicFSWDR->u32ExpCnt, pstDynamicFSWDR->au32ExpRation);

#ifdef APPLY_PARAMETER_TO_SYSTEM
	stFSWDRAttr.MotionCompEnable = pstDynamicFSWDR->au8MotionComp[u32ExpRationLevel];
#endif

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticDemosaic_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticDemosaic != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_DEMOSAIC_ATTR_S stDemosaicAttr;

	s32Ret = CVI_ISP_GetDemosaicAttr(ViPipe, &stDemosaicAttr);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	s32Ret = CVI_ISP_SetDemosaicAttr(ViPipe, &stDemosaicAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetStaticSharpen_AutoGenerate(VI_PIPE ViPipe, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bStaticSharpen != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_SHARPEN_ATTR_S stSharpenAttr;

	s32Ret = CVI_ISP_GetSharpenAttr(ViPipe, &stSharpenAttr);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	s32Ret = CVI_ISP_SetSharpenAttr(ViPipe, &stSharpenAttr);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_U8 u8Index, CVI_BOOL bEnable)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicDIS != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	if (bEnable != CVI_TRUE) {
		return CVI_SUCCESS;
	}

	if (u32Iso == u32LastIso) {
		return CVI_SUCCESS;
	}

	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_DIS_CONFIG_S stDISConfig;
	ISP_DIS_ATTR_S stDISAttr;

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetDisAttr(ViPipe, &stDISAttr);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetDisAttr(ViPipe, &stDISAttr);
	CHECK_SCENE_RET(s32Ret);


	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_GetDisConfig(ViPipe, &stDISConfig);
	CHECK_SCENE_RET(s32Ret);

	// Apply parameter to system here

	CHECK_SCENE_PAUSE();
	s32Ret = CVI_ISP_SetDisConfig(ViPipe, &stDISConfig);
	CHECK_SCENE_RET(s32Ret);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicLinearDRC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_S32 s32LV, CVI_S32 s32LastLV,
	CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicLinearDrc != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_DYNAMIC_LINEARDRC_S *pstDynamicLDrc = &(g_astScenePipeParam[u8Index].stDynamicLinearDrc);
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (s32LV != s32LastLV) {
		ISP_DRC_ATTR_S stDRCAttr;
		CVI_U32 u32LVLevel = 0;

		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_GetDRCAttr(ViPipe, &stDRCAttr);
		CHECK_SCENE_RET(s32Ret);

		u32LVLevel = SCENE_GetLevelLtoH_S32(s32LV,
			pstDynamicLDrc->u32LVCount, pstDynamicLDrc->as32LVLevel);

#ifdef APPLY_PARAMETER_TO_SYSTEM
		UNUSED(u32LVLevel);
#endif

		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_SetDRCAttr(ViPipe, &stDRCAttr);
		CHECK_SCENE_RET(s32Ret);
	}

	UNUSED(u32Iso);
	UNUSED(u32LastIso);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetDynamicThreadDRC_AutoGenerate(VI_PIPE ViPipe,
	CVI_U32 u32Iso, CVI_U32 u32LastIso, CVI_S32 s32LV, CVI_S32 s32LastLV,
	CVI_U32 u32ActRation, CVI_U8 u8Index)
{
	if (g_astScenePipeParam[u8Index].stModuleState.bDynamicThreadDrc != CVI_TRUE) {
		return CVI_SUCCESS;
	}
	PRINT_FUNCTION();

	CVI_SCENE_THREAD_DRC_S *pstThreadDrc = &(g_astScenePipeParam[u8Index].stThreadDrc);
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (s32LV != s32LastLV) {
		ISP_DRC_ATTR_S stDRCAttr;
		CVI_U32 u32LVLevel = 0;

		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_GetDRCAttr(ViPipe, &stDRCAttr);
		CHECK_SCENE_RET(s32Ret);

		u32LVLevel = SCENE_GetLevelLtoH_S32(s32LV,
			pstThreadDrc->u32LVCount, pstThreadDrc->as32LVLevel);

#ifdef APPLY_PARAMETER_TO_SYSTEM
		if ((u32LVLevel == 0) || (u32LVLevel >= (pstThreadDrc->u32LVCount - 1))) {
			stDRCAttr.stManual.TargetYScale = pstThreadDrc->au32TargetYScale[u32LVLevel];
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
			stDRCAttr.stManual.LETargetYScale = pstThreadDrc->au32LETargetYScale[u32LVLevel];
#endif // SOCs
			stDRCAttr.stManual.HdrStrength = pstThreadDrc->au16HdrStrength[u32LVLevel];
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
			stDRCAttr.stManual.DarkOffset = pstThreadDrc->au16DarkOffset[u32LVLevel];
			stDRCAttr.stManual.ContrastGain = pstThreadDrc->au16ContrastGain[u32LVLevel];
			stDRCAttr.stManual.DarkToneRange = pstThreadDrc->au16DarkToneRange[u32LVLevel];
#endif // SOCs
		} else {
			stDRCAttr.stManual.TargetYScale = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				pstThreadDrc->au32TargetYScale[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				pstThreadDrc->au32TargetYScale[u32LVLevel]);

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
			stDRCAttr.stManual.LETargetYScale = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				pstThreadDrc->au32LETargetYScale[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				pstThreadDrc->au32LETargetYScale[u32LVLevel]);
#endif // SOCs

			stDRCAttr.stManual.HdrStrength = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				(CVI_U32)pstThreadDrc->au16HdrStrength[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				(CVI_U32)pstThreadDrc->au16HdrStrength[u32LVLevel]);

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
			stDRCAttr.stManual.DarkOffset = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				(CVI_U32)pstThreadDrc->au16DarkOffset[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				(CVI_U32)pstThreadDrc->au16DarkOffset[u32LVLevel]);

			stDRCAttr.stManual.ContrastGain = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				(CVI_U32)pstThreadDrc->au16ContrastGain[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				(CVI_U32)pstThreadDrc->au16ContrastGain[u32LVLevel]);

			stDRCAttr.stManual.DarkToneRange = SCENE_Interpulate_S32_U32(s32LV,
				pstThreadDrc->as32LVLevel[u32LVLevel - 1],
				(CVI_U32)pstThreadDrc->au16DarkToneRange[u32LVLevel - 1],
				pstThreadDrc->as32LVLevel[u32LVLevel],
				(CVI_U32)pstThreadDrc->au16DarkToneRange[u32LVLevel]);
#endif // SOCs
		}
#endif

		CHECK_SCENE_PAUSE();
		s32Ret = CVI_ISP_SetDRCAttr(ViPipe, &stDRCAttr);
		CHECK_SCENE_RET(s32Ret);
	}

	UNUSED(u32Iso);
	UNUSED(u32LastIso);
	UNUSED(u32ActRation);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetPipeParam_AutoGenerate(const CVI_SCENE_PIPE_PARAM_S *pstScenePipeParam, CVI_U32 u32Num)
{
	if (!pstScenePipeParam) {
		printf("NULL Pointer\n");
		return CVI_FAILURE;
	}

	memcpy(g_astScenePipeParam, pstScenePipeParam, sizeof(CVI_SCENE_PIPE_PARAM_S) * u32Num);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetPause_AutoGenerate(CVI_BOOL bPause)
{
	g_bISPPause = bPause;

	return CVI_SUCCESS;
}
