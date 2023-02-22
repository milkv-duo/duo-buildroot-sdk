#include <stdio.h>
#include <stdlib.h>

#include "iniparser.h"

#include "cvi_scene_loadparam.h"
#include "cvi_scene_decode.h"

#define SCENE_INIPARAM_INI					"config_cfgaccess.ini"
#define SCENE_INI_GROUP_SCENEMODE			"scene_mode"

#define SCENE_INI_SCENEMODE					"scene_param_"
#define SCENE_INIPARAM_MODULE_NAME_LEN		(64)
#define SCENE_INIPARAM_NODE_NAME_LEN		(128)
#define SCENE_MAX_FILE_PATH_LENGTH			(256)
#define SCENE_MAX_TAG_LENGTH				(64)
#define SCENE_MAX_MODULE_SIZE				(8)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

typedef struct {
	int module_num;
	char module_name[SCENE_MAX_MODULE_SIZE][SCENE_INIPARAM_MODULE_NAME_LEN];
	char module_cfgpath[SCENE_MAX_MODULE_SIZE][SCENE_INIPARAM_MODULE_NAME_LEN];
} CFGModule_Config;

static CVI_S32 SCENE_ParseConfigCFGAccessData(CVI_CHAR *pszFilePath, CFGModule_Config *pstCFGModuleConfig)
{
	CVI_CHAR szTag[SCENE_MAX_TAG_LENGTH];
	dictionary *ini = NULL;
	const char *pszValue = NULL;
	CVI_U32 u32Idx;

	ini = iniparser_load(pszFilePath);
	if (ini == NULL) {
		return CVI_FAILURE;
	}

	pstCFGModuleConfig->module_num = iniparser_getint(ini, "module:module_num", 0);

	for (u32Idx = 0; u32Idx < SCENE_MAX_MODULE_SIZE; ++u32Idx) {
		snprintf(szTag, SCENE_MAX_TAG_LENGTH, "module:module%1d", u32Idx);
		pszValue = iniparser_getstring(ini, szTag, NULL);
		if (pszValue) {
			snprintf(pstCFGModuleConfig->module_name[u32Idx],
				SCENE_INIPARAM_MODULE_NAME_LEN, "%s", pszValue);
		}
	}

	for (u32Idx = 0; u32Idx < SCENE_MAX_MODULE_SIZE; ++u32Idx) {
		if (strlen(pstCFGModuleConfig->module_name[u32Idx]) > 0) {
			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "%s:cfg_filename",
				pstCFGModuleConfig->module_name[u32Idx]);
			pszValue = iniparser_getstring(ini, szTag, NULL);
			if (pszValue) {
				snprintf(pstCFGModuleConfig->module_cfgpath[u32Idx],
					SCENE_INIPARAM_MODULE_NAME_LEN, "%s", pszValue);
			}
		}
	}

	iniparser_freedict(ini);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ParseConfigSceneMode(CVI_CHAR *pszFilePath, CVI_SCENE_VIDEO_MODE_S *pstSceneVideoMode)
{
	CVI_CHAR szTag[SCENE_MAX_TAG_LENGTH];
	CVI_SCENE_MODE_S *pstSceneMode = CVI_NULL;
	CVI_SCENE_PIPE_ATTR_S *pstScenePipeAttr = CVI_NULL;
	dictionary *ini = NULL;
	CVI_U32 u32ModeIdx, u32PipeIdx;

	ini = iniparser_load(pszFilePath);
	if (ini == NULL) {
		return CVI_FAILURE;
	}

	pstSceneMode = pstSceneVideoMode->astVideoMode;
	for (u32ModeIdx = 0; u32ModeIdx < SCENE_MAX_VIDEOMODE; ++u32ModeIdx, pstSceneMode++) {
		snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_comm_%1d:SCENE_MODE", u32ModeIdx);
		pstSceneMode->enPipeMode = iniparser_getint(ini, szTag, 0);
		pstSceneMode->enPipeMode = MIN(CVI_SCENE_PIPE_MODE_WDR, pstSceneMode->enPipeMode);
		pstSceneMode->enPipeMode = MAX(CVI_SCENE_PIPE_MODE_LINEAR, pstSceneMode->enPipeMode);

		pstScenePipeAttr = pstSceneMode->astPipeAttr;
		for (u32PipeIdx = 0; u32PipeIdx < CVI_SCENE_PIPE_MAX_NUM; ++u32PipeIdx, pstScenePipeAttr++) {
			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_%1d_%1d:Enable", u32ModeIdx, u32PipeIdx);
			pstScenePipeAttr->bEnable = (CVI_BOOL)iniparser_getboolean(ini, szTag, 0);

			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_%1d_%1d:MainPipeHdl", u32ModeIdx, u32PipeIdx);
			pstScenePipeAttr->MainPipeHdl = (CVI_U32)iniparser_getint(ini, szTag, 0);

			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_%1d_%1d:VcapPipeHdl", u32ModeIdx, u32PipeIdx);
			pstScenePipeAttr->VcapPipeHdl = (CVI_U32)iniparser_getint(ini, szTag, 0);

			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_%1d_%1d:PipeChnHdl", u32ModeIdx, u32PipeIdx);
			pstScenePipeAttr->PipeChnHdl = (CVI_U32)iniparser_getint(ini, szTag, 0);

			snprintf(szTag, SCENE_MAX_TAG_LENGTH, "pipe_%1d_%1d:PipeParamIndex", u32ModeIdx, u32PipeIdx);
			pstScenePipeAttr->u8PipeParamIndex = (CVI_U8)iniparser_getint(ini, szTag, 0);
		}
	}

	iniparser_freedict(ini);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_ModuleState(const dictionary *ini,
	CVI_SCENE_MODULE_STATE_S *pstData)
{
	pstData->bStaticAE = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticAE", 0);
	pstData->bStaticAWB = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticAWB", 0);
	pstData->bStaticAWBEx = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticAWBEx", 0);
	pstData->bStaticCCM = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticCCM", 0);
	pstData->bStaticAERouteEx = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticAERouteEx", 0);
	pstData->bStaticCAC = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticCAC", 0);
	pstData->bStaticWdrExposure = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticWdrExposure", 0);
	pstData->bStaticDehaze = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticDehaze", 0);
	pstData->bStaticStatistics = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticStatistics", 0);
	pstData->bStaticSaturation = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticSaturation", 0);
	pstData->bStaticShading = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticShading", 0);
	pstData->bDynamicAE = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicAE", 0);
	pstData->bDynamicThreeDNR = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicThreeDNR", 0);
	pstData->bDynamicGamma = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicGamma", 0);
	pstData->bDynamicCAC = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicCAC", 0);
	pstData->bDynamicColorTone = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicColorTone", 0);
	pstData->bDynamicShading = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicShading", 0);
	pstData->bDynamicDehaze = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicDehaze", 0);
	pstData->bDynamicFSWDR = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicFSWDR", 0);
	pstData->bStaticSharpen = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticSharpen", 0);
	pstData->bStaticDemosaic = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticDemosaic", 0);
	pstData->bDynamicDIS = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicDis", 0);
	pstData->bStaticDrc = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bStaticDrc", 0);
	pstData->bDynamicLinearDrc = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicLinearDrc", 0);
	pstData->bDynamicThreadDrc = (CVI_BOOL)iniparser_getboolean(ini, "module_state:bDynamicThreadDrc", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticAE(const dictionary *ini,
	CVI_SCENE_STATIC_AE_S *pstData)
{
	pstData->bAERouteExValid = (CVI_BOOL)iniparser_getboolean(ini, "static_ae:AERouteExValid", 0);
	pstData->u8AERunInterval = (CVI_U8)iniparser_getint(ini, "static_ae:AERunInterval", 0);
	pstData->u8AutoSpeed = (CVI_U8)iniparser_getint(ini, "static_ae:AutoSpeed", 0);
	pstData->u8AutoTolerance = (CVI_U8)iniparser_getint(ini, "static_ae:AutoTolerance", 0);
	pstData->u16AutoBlackDelayFrame = (CVI_U16)iniparser_getint(ini, "static_ae:AutoBlackDelayFrame", 0);
	pstData->u16AutoWhiteDelayFrame = (CVI_U16)iniparser_getint(ini, "static_ae:AutoWhiteDelayFrame", 0);
	pstData->u32AutoSysGainMax = (CVI_U32)iniparser_getint(ini, "static_ae:AutoSysGainMax", 0);
	pstData->u32AutoExpTimeMax = (CVI_U32)iniparser_getint(ini, "static_ae:AutoExpTimeMax", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticAWB(const dictionary *ini,
	CVI_SCENE_STATIC_AWB_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticAWBEX(const dictionary *ini,
	CVI_SCENE_STATIC_AWBEX_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticCCM(const dictionary *ini,
	CVI_SCENE_STATIC_CCM_S *pstData)
{
	pstData->bISOActEn = (CVI_BOOL)iniparser_getboolean(ini, "static_ccm:ISOActEn", 0);
	pstData->bTempActEn = (CVI_BOOL)iniparser_getboolean(ini, "static_ccm:TempActEn", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticAERoute(const dictionary *ini,
	CVI_SCENE_STATIC_AEROUTE_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticAERouteEX(const dictionary *ini,
	CVI_SCENE_STATIC_AEROUTEEX_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32TotalNum = (CVI_U32)iniparser_getint(ini, "static_aerouteex:TotalNum", 0);
	pstData->u32TotalNum = MIN(pstData->u32TotalNum, ISP_AE_ROUTE_EX_MAX_NODES);

	pszValue = iniparser_getstring(ini, "static_aerouteex:RouteEXIntTime", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32IntTime, pstData->u32TotalNum);

	pszValue = iniparser_getstring(ini, "static_aerouteex:RouteEXAGain", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32Again, pstData->u32TotalNum);

	pszValue = iniparser_getstring(ini, "static_aerouteex:RouteEXDGain", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32Dgain, pstData->u32TotalNum);

	pszValue = iniparser_getstring(ini, "static_aerouteex:RouteEXISPDGain", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32IspDgain, pstData->u32TotalNum);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticCAC(const dictionary *ini,
	CVI_SCENE_STATIC_CAC_S *pstData)
{
	pstData->bEnable = (CVI_BOOL)iniparser_getboolean(ini, "static_cac:Enable", 0);
	pstData->u8OpType = (CVI_U8)iniparser_getint(ini, "static_cac:CACOpType", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticWDRExposure(const dictionary *ini,
	CVI_SCENE_STATIC_WDREXPOSURE_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u8ExpRatioType = (CVI_U8)iniparser_getint(ini, "static_wdrexposure:ExpRatioType", 0);
	pstData->u32ExpRatioMax = (CVI_U32)iniparser_getint(ini, "static_wdrexposure:ExpRatioMax", 0);
	pstData->u32ExpRatioMin = (CVI_U32)iniparser_getint(ini, "static_wdrexposure:ExpRatioMin", 0);

	pszValue = iniparser_getstring(ini, "static_wdrexposure:ExpRatio", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32ExpRatio, WDR_EXP_RATIO_NUM);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticDehaze(const dictionary *ini,
	CVI_SCENE_STATIC_DEHAZE_S *pstData)
{
	pstData->bEnable = (CVI_BOOL)iniparser_getboolean(ini, "static_dehaze:Enable", 0);
	pstData->u8OpType = (CVI_U8)iniparser_getint(ini, "static_dehaze:DehazeOpType", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticStatistics(const dictionary *ini,
	CVI_SCENE_STATIC_STATISTICSCFG_S *pstData)
{
	CVI_CHAR szTag[SCENE_MAX_TAG_LENGTH];
	const char *pszValue = NULL;

	for (CVI_U32 u32Idx = 0; u32Idx < AE_WEIGHT_ZONE_ROW; ++u32Idx) {
		snprintf(szTag, SCENE_MAX_TAG_LENGTH, "static_statistics:ExpWeight_%d", u32Idx);

		pszValue = iniparser_getstring(ini, szTag, NULL);
		SCENE_DecodeU8Array(pszValue, pstData->au8AEWeight[u32Idx], AE_WEIGHT_ZONE_COLUMN);
	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticCSC(const dictionary *ini,
	CVI_SCENE_STATIC_CSC_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticSaturation(const dictionary *ini,
	CVI_SCENE_STATIC_SATURATION_S *pstData)
{
	const char *pszValue = NULL;

	pszValue = iniparser_getstring(ini, "static_saturation:AutoSat", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8AutoSat, ISP_AUTO_ISO_STRENGTH_NUM);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticShading(const dictionary *ini,
	CVI_SCENE_STATIC_SHADING_S *pstData)
{
	pstData->bEnable = (CVI_BOOL)iniparser_getboolean(ini, "static_shading:Enable", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_Dynamic3DNR(const dictionary *ini,
	CVI_SCENE_DYNAMIC_THREEDNR_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicAE(const dictionary *ini,
	CVI_SCENE_DYNAMIC_AE_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u8AEExposureCnt = (CVI_U8)iniparser_getint(ini, "dynamic_ae:aeExpCount", 0);
	pstData->u8AEExposureCnt = MIN(pstData->u8AEExposureCnt, CVI_SCENE_AE_EXPOSURE_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_ae:aeExpLtoHThresh", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpLtoHThresh, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:aeExpHtoLThresh", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpHtoLThresh, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:AutoCompesation", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8AutoCompensation, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:AutoHistOffset", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8AutoMaxHistOffset, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:AutoExpRatioMax", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32AutoExpRatioMax, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:AutoExpRatioMin", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32AutoExpRatioMin, pstData->u8AEExposureCnt);

	pszValue = iniparser_getstring(ini, "dynamic_ae:AutoAntiflickerEn", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8AutoAntiflickerEn, pstData->u8AEExposureCnt);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicGamma(const dictionary *ini,
	CVI_SCENE_DYNAMIC_GAMMA_S *pstData)
{
	CVI_CHAR szTag[SCENE_MAX_TAG_LENGTH];
	const char *pszValue = NULL;

	pstData->u32InterVal = (CVI_U32)iniparser_getint(ini, "dynamic_gamma:Interval", 0);
	pstData->u32TotalNum = (CVI_U32)iniparser_getint(ini, "dynamic_gamma:TotalNum", 0);
	pstData->u32TotalNum = MIN(pstData->u32TotalNum, CVI_SCENE_EXPOSURE_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_gamma:gammaExpThreshLtoH", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpThreshLtoH, pstData->u32TotalNum);

	pszValue = iniparser_getstring(ini, "dynamic_gamma:gammaExpThreshHtoL", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpThreshHtoL, pstData->u32TotalNum);

	for (CVI_U32 u32Idx = 0; u32Idx < pstData->u32TotalNum; ++u32Idx) {
		snprintf(szTag, SCENE_MAX_TAG_LENGTH, "dynamic_gamma:Table_%d", u32Idx);

		pszValue = iniparser_getstring(ini, szTag, NULL);
		SCENE_DecodeU16Array(pszValue, pstData->au16Table[u32Idx], GAMMA_NODE_NUM);
	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicCAC(const dictionary *ini,
	CVI_SCENE_DYNAMIC_CAC_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32ISOCount = (CVI_U32)iniparser_getint(ini, "dynamic_cac:IsoCnt", 0);
	pstData->u32ISOCount = MIN(pstData->u32ISOCount, CVI_SCENE_ISO_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_cac:IsoLevel", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32ISOLevel, pstData->u32ISOCount);

	pszValue = iniparser_getstring(ini, "dynamic_cac:PurpleDetRange", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8PurpleDetRange, pstData->u32ISOCount);

	pszValue = iniparser_getstring(ini, "dynamic_cac:ValThr", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16VarThr, pstData->u32ISOCount);

	pszValue = iniparser_getstring(ini, "dynamic_cac:DePurpleStr", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8DePurpleStr, pstData->u32ISOCount);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicColorTone(const dictionary *ini,
	CVI_SCENE_DYNAMIC_COLORTONE_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32ExpThreshCnt = (CVI_U32)iniparser_getint(ini, "dynamic_colortone:ExpThreshCnt", 0);
	pstData->u32ExpThreshCnt = MIN(pstData->u32ExpThreshCnt, CVI_SCENE_EXPOSURE_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_colortone:ExpThreshLtoH", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpThreshLtoH, pstData->u32ExpThreshCnt);

	pszValue = iniparser_getstring(ini, "dynamic_colortone:RedCastGain", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16RedCastGain, pstData->u32ExpThreshCnt);

	pszValue = iniparser_getstring(ini, "dynamic_colortone:GreenCastGain", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16GreenCastGain, pstData->u32ExpThreshCnt);

	pszValue = iniparser_getstring(ini, "dynamic_colortone:BlueCastGain", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16BlueCastGain, pstData->u32ExpThreshCnt);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicShading(const dictionary *ini,
	CVI_SCENE_DYNAMIC_SHADING_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32ExpThreshCnt = (CVI_U32)iniparser_getint(ini, "dynamic_shading:ExpThreshCnt", 0);
	pstData->u32ExpThreshCnt = MIN(pstData->u32ExpThreshCnt, CVI_SCENE_EXPOSURE_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_shading:ExpThreshLtoH", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpThreshLtoH, pstData->u32ExpThreshCnt);

	pszValue = iniparser_getstring(ini, "dynamic_shading:ManualStrength", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16ManualStrength, pstData->u32ExpThreshCnt);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicDehaze(const dictionary *ini,
	CVI_SCENE_DYNAMIC_DEHAZE_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32ExpThreshCnt = (CVI_U32)iniparser_getint(ini, "dynamic_dehaze:ExpThreshCnt", 0);
	pstData->u32ExpThreshCnt = MIN(pstData->u32ExpThreshCnt, CVI_SCENE_EXPOSURE_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_dehaze:ExpThreshLtoH", NULL);
	SCENE_DecodeU64Array(pszValue, pstData->au64ExpThreshLtoH, pstData->u32ExpThreshCnt);

	pszValue = iniparser_getstring(ini, "dynamic_dehaze:ManualDehazeStr", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8ManualStrength, pstData->u32ExpThreshCnt);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicFSWDR(const dictionary *ini,
	CVI_SCENE_DYNAMIC_FSWDR_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32ExpCnt = (CVI_U32)iniparser_getint(ini, "dynamic_fswdr:ExpCnt", 0);
	pstData->u32ExpCnt = MIN(pstData->u32ExpCnt, CVI_SCENE_EXPRATION_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "dynamic_fswdr:ExpRation", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32ExpRation, pstData->u32ExpCnt);

	pszValue = iniparser_getstring(ini, "dynamic_fswdr:MotionComp", NULL);
	SCENE_DecodeU8Array(pszValue, pstData->au8MotionComp, pstData->u32ExpCnt);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticSharpen(const dictionary *ini,
	CVI_SCENE_STATIC_SHARPEN_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticDemosaic(const dictionary *ini,
	CVI_SCENE_STATIC_DEMOSAIC_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicDis(const dictionary *ini,
	CVI_SCENE_DYNAMIC_DIS_S *pstData)
{
	UNUSED(*ini);
	UNUSED(*pstData);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_StaticDrc(const dictionary *ini,
	CVI_SCENE_STATIC_DRC_S *pstData)
{
	pstData->bEnable = (CVI_BOOL)iniparser_getboolean(ini, "static_drc:Enable", 0);
	pstData->u8OpType = (CVI_U8)iniparser_getint(ini, "static_drc:DRCOpType", 0);
#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	pstData->u16LinearStart = (CVI_U16)iniparser_getint(ini, "static_drc:LinearStart", 0);
	pstData->u16LinearEnd = (CVI_U16)iniparser_getint(ini, "static_drc:LinearEnd", 0);
#endif // SOCs
	pstData->u32ToneCurveSmooth = (CVI_U32)iniparser_getint(ini, "static_drc:ToneCurveSmooth", 0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_DynamicLinearDrc(const dictionary *ini,
	CVI_SCENE_DYNAMIC_LINEARDRC_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32LVCount = (CVI_U32)iniparser_getint(ini, "linear_drc:LVCnt", 0);
	pstData->u32LVCount = MIN(pstData->u32LVCount, CVI_SCENE_LV_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "linear_drc:LVLevel", NULL);
	SCENE_DecodeS32Array(pszValue, pstData->as32LVLevel, pstData->u32LVCount);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ScenePipeParamData_Handler_ThreadDrc(const dictionary *ini,
	CVI_SCENE_THREAD_DRC_S *pstData)
{
	const char *pszValue = NULL;

	pstData->u32LVCount = (CVI_U32)iniparser_getint(ini, "thread_drc:LVCnt", 0);
	pstData->u32LVCount = MIN(pstData->u32LVCount, CVI_SCENE_LV_MAX_COUNT);

	pszValue = iniparser_getstring(ini, "thread_drc:LVLevel", NULL);
	SCENE_DecodeS32Array(pszValue, pstData->as32LVLevel, pstData->u32LVCount);

	pszValue = iniparser_getstring(ini, "thread_drc:TargetYScale", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32TargetYScale, pstData->u32LVCount);

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
	pszValue = iniparser_getstring(ini, "thread_drc:DarkOffset", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16DarkOffset, pstData->u32LVCount);

	pszValue = iniparser_getstring(ini, "thread_drc:DarkToneRange", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16DarkToneRange, pstData->u32LVCount);

	pszValue = iniparser_getstring(ini, "thread_drc:LETargetYScale", NULL);
	SCENE_DecodeU32Array(pszValue, pstData->au32LETargetYScale, pstData->u32LVCount);

	pszValue = iniparser_getstring(ini, "thread_drc:ContrastGain", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16ContrastGain, pstData->u32LVCount);
#endif // SOCs

	pszValue = iniparser_getstring(ini, "thread_drc:HdrStrength", NULL);
	SCENE_DecodeU16Array(pszValue, pstData->au16HdrStrength, pstData->u32LVCount);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_ParseConfigScenePipeParam(CVI_CHAR *pszFilePath, CVI_SCENE_PIPE_PARAM_S *pstScenePipeParam)
{
	dictionary *ini = NULL;
	CVI_S32 s32Ret;

	ini = iniparser_load(pszFilePath);
	if (ini == NULL) {
		return CVI_FAILURE;
	}

	// CVI_SCENE_MODULE_STATE_S stModuleState
	s32Ret = SCENE_ScenePipeParamData_Handler_ModuleState(ini, &(pstScenePipeParam->stModuleState));
	// CVI_SCENE_STATIC_AE_S stStaticAe
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticAE(ini, &(pstScenePipeParam->stStaticAe));
	// CVI_SCENE_STATIC_AWB_S stStaticAwb
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticAWB(ini, &(pstScenePipeParam->stStaticAwb));
	// CVI_SCENE_STATIC_AWBEX_S stStaticAwbEx
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticAWBEX(ini, &(pstScenePipeParam->stStaticAwbEx));
	// CVI_SCENE_STATIC_CCM_S stStaticCcm
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticCCM(ini, &(pstScenePipeParam->stStaticCcm));
	// CVI_SCENE_STATIC_AEROUTE_S stStaticAeRoute
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticAERoute(ini, &(pstScenePipeParam->stStaticAeRoute));
	// CVI_SCENE_STATIC_AEROUTEEX_S stStaticAeRouteEx
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticAERouteEX(ini, &(pstScenePipeParam->stStaticAeRouteEx));
	// CVI_SCENE_STATIC_CAC_S stStaticCAC
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticCAC(ini, &(pstScenePipeParam->stStaticCAC));
	// CVI_SCENE_STATIC_WDREXPOSURE_S stStaticWdrExposure
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticWDRExposure(ini, &(pstScenePipeParam->stStaticWdrExposure));
	// CVI_SCENE_STATIC_DEHAZE_S stStaticDehaze
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticDehaze(ini, &(pstScenePipeParam->stStaticDehaze));
	// CVI_SCENE_STATIC_STATISTICSCFG_S stStaticStatistics
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticStatistics(ini, &(pstScenePipeParam->stStaticStatistics));
	// CVI_SCENE_STATIC_CSC_S stStaticCsc;
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticCSC(ini, &(pstScenePipeParam->stStaticCsc));
	// CVI_SCENE_STATIC_SATURATION_S stStaticSaturation;
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticSaturation(ini, &(pstScenePipeParam->stStaticSaturation));
	// CVI_SCENE_STATIC_SHADING_S stStaticShading;
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticShading(ini, &(pstScenePipeParam->stStaticShading));
	// CVI_SCENE_DYNAMIC_THREEDNR_S stDynamicThreeDNR;
	s32Ret = SCENE_ScenePipeParamData_Handler_Dynamic3DNR(ini, &(pstScenePipeParam->stDynamicThreeDNR));
	// CVI_SCENE_DYNAMIC_AE_S stDynamicAe
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicAE(ini, &(pstScenePipeParam->stDynamicAe));
	// CVI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicGamma(ini, &(pstScenePipeParam->stDynamicGamma));
	// CVI_SCENE_DYNAMIC_CAC_S stDynamicCAC
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicCAC(ini, &(pstScenePipeParam->stDynamicCAC));
	// CVI_SCENE_DYNAMIC_COLORTONE_S stDynamicColorTone
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicColorTone(ini, &(pstScenePipeParam->stDynamicColorTone));
	// CVI_SCENE_DYNAMIC_SHADING_S stDynamicShading
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicShading(ini, &(pstScenePipeParam->stDynamicShading));
	// CVI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicDehaze(ini, &(pstScenePipeParam->stDynamicDehaze));
	// CVI_SCENE_DYNAMIC_FSWDR_S stDynamicFSWDR
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicFSWDR(ini, &(pstScenePipeParam->stDynamicFSWDR));
	// CVI_SCENE_STATIC_SHARPEN_S stStaticSharpen
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticSharpen(ini, &(pstScenePipeParam->stStaticSharpen));
	// CVI_SCENE_STATIC_DEMOSAIC_S stStaticDemosaic
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticDemosaic(ini, &(pstScenePipeParam->stStaticDemosaic));
	// CVI_SCENE_DYNAMIC_DIS_S stDynamicDis
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicDis(ini, &(pstScenePipeParam->stDynamicDis));
	// CVI_SCENE_STATIC_DRC_S stStaticDrc
	s32Ret = SCENE_ScenePipeParamData_Handler_StaticDrc(ini, &(pstScenePipeParam->stStaticDrc));
	// CVI_SCENE_DYNAMIC_LINEARDRC_S stDynamicLinearDrc
	s32Ret = SCENE_ScenePipeParamData_Handler_DynamicLinearDrc(ini, &(pstScenePipeParam->stDynamicLinearDrc));
	// CVI_SCENE_THREAD_DRC_S stThreadDrc
	s32Ret = SCENE_ScenePipeParamData_Handler_ThreadDrc(ini, &(pstScenePipeParam->stThreadDrc));

	iniparser_freedict(ini);

	return s32Ret;
}

CVI_S32 CVI_SCENE_CreateParam(CVI_CHAR *pszDirname,
	CVI_SCENE_PARAM_S *pstSceneParam, CVI_SCENE_VIDEO_MODE_S *pstVideoMode)
{
	if (CVI_NULL == pstSceneParam || CVI_NULL == pstVideoMode) {
		printf("NULL Pointer\n");
		return CVI_FAILURE;
	}

	CVI_CHAR szIniPath[SCENE_MAX_FILE_PATH_LENGTH] = {0};
	CVI_CHAR szSceneParamName[32] = {0};
	CFGModule_Config cfgModuleConfig;

	memset(&cfgModuleConfig, 0, sizeof(CFGModule_Config));

	snprintf(szIniPath, SCENE_MAX_FILE_PATH_LENGTH, "%s/%s",
		(pszDirname == NULL) ? "." : pszDirname, SCENE_INIPARAM_INI);

	if (SCENE_ParseConfigCFGAccessData(szIniPath, &cfgModuleConfig) != CVI_SUCCESS) {
		printf("SCENE_ParseConfigCFGAccessData fail\n");
		return CVI_FAILURE;
	}

	memset(pstSceneParam, 0, sizeof(CVI_SCENE_PARAM_S));
	memset(pstVideoMode, 0, sizeof(CVI_SCENE_VIDEO_MODE_S));

	for (CVI_U32 u32Idx = 0; u32Idx < SCENE_MAX_MODULE_SIZE; ++u32Idx) {
		if (strlen(cfgModuleConfig.module_name[u32Idx]) == 0) {
			continue;
		}

		if (strcmp(SCENE_INI_GROUP_SCENEMODE, cfgModuleConfig.module_name[u32Idx]) == 0) {
			// scene_mode
			snprintf(szIniPath, SCENE_MAX_FILE_PATH_LENGTH, "%s/%s",
				(pszDirname == NULL) ? "." : pszDirname, cfgModuleConfig.module_cfgpath[u32Idx]);
			if (SCENE_ParseConfigSceneMode(szIniPath, pstVideoMode) != CVI_SUCCESS) {
				printf("SCENE_ParseConfigSceneMode fail\n");
				return CVI_FAILURE;
			}
		} else {
			// scene_param_X
			for (CVI_U32 u32PipeIdx = 0; u32PipeIdx < CVI_SCENE_PIPETYPE_NUM; ++u32PipeIdx) {
				snprintf(szSceneParamName, 32, "scene_param_%1d", u32PipeIdx);
				if (strcmp(szSceneParamName, cfgModuleConfig.module_name[u32Idx]) == 0) {
					snprintf(szIniPath, SCENE_MAX_FILE_PATH_LENGTH, "%s/%s",
						(pszDirname == NULL) ? "." : pszDirname,
						cfgModuleConfig.module_cfgpath[u32Idx]);
					if (SCENE_ParseConfigScenePipeParam(szIniPath,
						&(pstSceneParam->astPipeParam[u32PipeIdx])) != CVI_SUCCESS) {
						printf("SCENE_ParseConfigScenePipeParam fail\n");
					}
				}
			}
		}
	}

	return CVI_SUCCESS;
}
