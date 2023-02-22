#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "cvi_ae.h"
#include "cvi_isp.h"

#include "cvi_scene_inner.h"
#include "cvi_scene_comm.h"
#include "cvi_scene_setparam.h"

#define APPLY_DYNAMIC_PARAMETER_UPDATE

static CVI_SCENE_MODE_S s_stSceneMode;
static SCENE_STATE_S s_stSceneState;
pthread_mutex_t g_mScene_lock = PTHREAD_MUTEX_INITIALIZER;

#define SCENE_CHECK_INIT() \
	do { \
		pthread_mutex_lock(&g_mScene_lock); \
		if (s_stSceneState.bSceneInit == CVI_FALSE) { \
			printf("Please init sceneauto first!\n"); \
			pthread_mutex_unlock(&g_mScene_lock); \
			return CVI_SCENE_ENOTINIT; \
		} \
		pthread_mutex_unlock(&g_mScene_lock); \
	} while (0)

#define SCENE_CHECK_PAUSE() \
	do { \
		if (s_stSceneState.bPause == CVI_TRUE) { \
			return CVI_SUCCESS; \
		} \
	} while (0)

static CVI_S32 SCENE_SetMainPipeState(CVI_VOID)
{
	CVI_U32 i, j;
	CVI_U32 u32MainPipeCnt = 0;

	memset(s_stSceneState.astMainPipe, 0, sizeof(SCENE_MAINPIPE_STATE_S) * CVI_SCENE_PIPE_MAX_NUM);

	for (i = 0; i < CVI_SCENE_PIPE_MAX_NUM; i++) {
		s_stSceneState.astMainPipe[i].bDISEnable = CVI_FALSE;
		s_stSceneState.astMainPipe[i].bFixedAEMatrix = CVI_FALSE;
	}

	for (i = 0; i < CVI_SCENE_PIPE_MAX_NUM; i++) {
		if (s_stSceneMode.astPipeAttr[i].bEnable != CVI_TRUE) {
			continue;
		}

		if (u32MainPipeCnt == 0) {
			s_stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl
				= s_stSceneMode.astPipeAttr[i].MainPipeHdl;
			u32MainPipeCnt++;
			continue;
		}

		for (j = 0; j < u32MainPipeCnt; j++) {
			if (s_stSceneState.astMainPipe[j].MainPipeHdl == s_stSceneMode.astPipeAttr[i].MainPipeHdl) {
				break;
			}
		}

		if (u32MainPipeCnt == j) {
			s_stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl
				= s_stSceneMode.astPipeAttr[i].MainPipeHdl;
			u32MainPipeCnt++;
		}
	}

	// set subpipe in certain mainpipe
	for (i = 0; i < u32MainPipeCnt; i++) {
		CVI_U32 u32SubPipeCnt = 0;

		for (j = 0; j < CVI_SCENE_PIPE_MAX_NUM; j++) {
			if (s_stSceneMode.astPipeAttr[j].bEnable != CVI_TRUE) {
				continue;
			}

			if (s_stSceneState.astMainPipe[i].MainPipeHdl == s_stSceneMode.astPipeAttr[j].MainPipeHdl) {
				s_stSceneState.astMainPipe[i].aSubPipeHdl[u32SubPipeCnt]
					= s_stSceneMode.astPipeAttr[j].VcapPipeHdl;
				u32SubPipeCnt++;
			}
		}

		s_stSceneState.astMainPipe[i].u32SubPipeNum = u32SubPipeCnt;
	}

	s_stSceneState.u32MainPipeNum = u32MainPipeCnt;

//	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
//		printf("The mainpipe is %d\n", s_stSceneState.astMainPipe[i].MainPipeHdl);
//		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
//			printf("The subpipe in mainpipe %d is %d\n",
//				s_stSceneState.astMainPipe[i].MainPipeHdl,
//				s_stSceneState.astMainPipe[i].aSubPipeHdl[j]);
//		}
//		printf("\n");
//	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_CalculateExp(VI_PIPE ViPipe, CVI_U32 *pu32Iso, CVI_U64 *pu64Exposure, CVI_S32 *ps32LV)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U64 u64SysGainValue = 0;

	ISP_EXP_INFO_S stIspExpInfo;
	ISP_PUB_ATTR_S stPubAttr;

	SCENE_CHECK_PAUSE();

	s32Ret = CVI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_ISP_GetPubAttr(ViPipe, &stPubAttr);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	u64SysGainValue = (CVI_U64)stIspExpInfo.u32AGain
						* (CVI_U64)stIspExpInfo.u32DGain
						* (CVI_U64)stIspExpInfo.u32ISPDGain;

	*pu32Iso = (u64SysGainValue * 100) >> 30;

	if (WDR_MODE_4To1_LINE == stPubAttr.enWDRMode) {
		*pu64Exposure = (u64SysGainValue * (CVI_U64)stIspExpInfo.u32LongExpTime) >> 30;
	} else if (WDR_MODE_3To1_LINE == stPubAttr.enWDRMode) {
		*pu64Exposure = (u64SysGainValue * (CVI_U64)stIspExpInfo.u32MedianExpTime) >> 30;
	} else if (WDR_MODE_2To1_LINE == stPubAttr.enWDRMode) {
		*pu64Exposure = (u64SysGainValue * (CVI_U64)stIspExpInfo.u32ShortExpTime) >> 30;
	} else {
		*pu64Exposure = (u64SysGainValue * (CVI_U64)stIspExpInfo.u32ExpTime) >> 30;
	}

	*ps32LV = (CVI_S32)(stIspExpInfo.fLightValue * 100.0);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_CalculateWdrParam(VI_PIPE ViPipe, CVI_U32 *pu32ActRation, CVI_U32 *pu32HDRBrightRatio)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	ISP_INNER_STATE_INFO_S stInnerStateInfo;

	SCENE_CHECK_PAUSE();

	s32Ret = CVI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	*pu32ActRation = stInnerStateInfo.u32WDRExpRatioActual[0];

	UNUSED(*pu32HDRBrightRatio);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_SetMainPipeSpecialParam(VI_PIPE ViPipe, CVI_BOOL bFixedAEMatrix)
{
	CVI_S32 s32Ret;
	CVI_U8 u8Index = s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex;

	s32Ret = CVI_SCENE_SetStaticAE_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticAWB_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticAWBEX_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticAERouteEX_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticCAC_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticSaturation_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticWDRExposure_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticDRC_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticStatisticsCfg_AutoGenerate(ViPipe, u8Index, bFixedAEMatrix);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	s32Ret = CVI_SCENE_SetStaticShading_AutoGenerate(ViPipe, u8Index);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_SetPipeStaticParam(CVI_VOID)
{
	CVI_U32 u32Idx;
	CVI_S32 s32Ret = CVI_SUCCESS;

	for (u32Idx = 0; u32Idx < CVI_SCENE_PIPE_MAX_NUM; ++u32Idx) {
		if (s_stSceneMode.astPipeAttr[u32Idx].bEnable != CVI_TRUE) {
			continue;
		}

		s32Ret = CVI_SCENE_SetStaticDehaze_AutoGenerate(
			s_stSceneMode.astPipeAttr[u32Idx].VcapPipeHdl,
			s_stSceneMode.astPipeAttr[u32Idx].u8PipeParamIndex);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	for (u32Idx = 0; u32Idx < CVI_SCENE_PIPE_MAX_NUM; ++u32Idx) {
		if (s_stSceneMode.astPipeAttr[u32Idx].bEnable != CVI_TRUE) {
			continue;
		}

		s32Ret = CVI_SCENE_SetStaticCCM_AutoGenerate(
			s_stSceneMode.astPipeAttr[u32Idx].VcapPipeHdl,
			s_stSceneMode.astPipeAttr[u32Idx].u8PipeParamIndex);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	for (u32Idx = 0; u32Idx < CVI_SCENE_PIPE_MAX_NUM; ++u32Idx) {
		if (s_stSceneMode.astPipeAttr[u32Idx].bEnable != CVI_TRUE) {
			continue;
		}

		s32Ret = CVI_SCENE_SetStaticDemosaic_AutoGenerate(
			s_stSceneMode.astPipeAttr[u32Idx].VcapPipeHdl,
			s_stSceneMode.astPipeAttr[u32Idx].u8PipeParamIndex);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	for (u32Idx = 0; u32Idx < CVI_SCENE_PIPE_MAX_NUM; ++u32Idx) {
		if (s_stSceneMode.astPipeAttr[u32Idx].bEnable != CVI_TRUE) {
			continue;
		}

		s32Ret = CVI_SCENE_SetStaticSharpen_AutoGenerate(
			s_stSceneMode.astPipeAttr[u32Idx].VcapPipeHdl,
			s_stSceneMode.astPipeAttr[u32Idx].u8PipeParamIndex);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_SetPipeDynamicParam(CVI_VOID)
{
	CVI_U32 i, j;
	CVI_S32 s32Index;
	CVI_U32 u32Iso;
	CVI_U64 u64Exposure, u64LastExposure;
	CVI_U8 u8PipeIndex;
	CVI_S32 s32Ret = CVI_SUCCESS;

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
			u64LastExposure = s_stSceneState.astMainPipe[i].u64LastNormalExposure;

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicShading_AutoGenerate(
				s32Index,
				u64Exposure, u64LastExposure,
				u8PipeIndex);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
			u64LastExposure = s_stSceneState.astMainPipe[i].u64LastNormalExposure;

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicDehaze_AutoGenerate(
				s32Index,
				u64Exposure, u64LastExposure,
				u8PipeIndex);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicCAC_AutoGenerate(
				s32Index,
				u32Iso, s_stSceneState.astMainPipe[i].u32LastLuminanceISO,
				u8PipeIndex);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
			u64LastExposure = s_stSceneState.astMainPipe[i].u64LastNormalExposure;

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicColorTone_AutoGenerate(
				s32Index,
				u64Exposure, u64LastExposure,
				u8PipeIndex);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

			if (s_stSceneState.astMainPipe[i].bDISEnable != CVI_TRUE) {
				continue;
			}

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicDIS_AutoGenerate(
				s32Index,
				u32Iso, s_stSceneState.astMainPipe[i].u32LastNormalIso,
				u8PipeIndex, s_stSceneState.astMainPipe[i].bDISEnable);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
			s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
			u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

			u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

			s32Ret = CVI_SCENE_SetDynamicThreeDNR_AutoGenerate(
				s32Index,
				u32Iso, s_stSceneState.astMainPipe[i].u32LastLuminanceISO,
				u8PipeIndex);
			CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
		}
	}

	for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
		s_stSceneState.astMainPipe[i].u32LastLuminanceISO = s_stSceneState.astMainPipe[i].u32Iso;
	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_SetVIPipeParam(CVI_VOID)
{
	VI_PIPE ViPipe;
	CVI_BOOL bFixedAEMatrix;
	CVI_S32 s32Ret = CVI_SUCCESS;

	for (CVI_U32 u32Idx = 0; u32Idx < s_stSceneState.u32MainPipeNum; ++u32Idx) {
		ViPipe = s_stSceneState.astMainPipe[u32Idx].MainPipeHdl;
		bFixedAEMatrix = s_stSceneState.astMainPipe[u32Idx].bFixedAEMatrix;

		s32Ret = SCENE_SetMainPipeSpecialParam(ViPipe, bFixedAEMatrix);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

		s32Ret = SCENE_CalculateExp(ViPipe,
			&(s_stSceneState.astMainPipe[u32Idx].u32Iso),
			&(s_stSceneState.astMainPipe[u32Idx].u64Exposure),
			&(s_stSceneState.astMainPipe[u32Idx].s32LV));
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	s32Ret = SCENE_SetPipeStaticParam();
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	// s32Ret = SCENE_SetPipeDynamicParam();
	// CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	return CVI_SUCCESS;
}

CVI_VOID *SCENE_Normal_AutoThread(CVI_VOID *pVoid)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VI_PIPE ViPipe;
	CVI_U32 i;

	prctl(PR_SET_NAME, (unsigned long)"CVI_SCENE_NormalThread", 0, 0, 0);

	while (s_stSceneState.stThreadNormal.bThreadFlag == CVI_TRUE) {
		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

			s32Ret = SCENE_CalculateExp(ViPipe,
				&(s_stSceneState.astMainPipe[i].u32Iso),
				&(s_stSceneState.astMainPipe[i].u64Exposure),
				&(s_stSceneState.astMainPipe[i].s32LV));
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
		}

		// printf("NormalAutoThread : ISO:%7u, Exposure:%10lu\n",
		//	s_stSceneState.astMainPipe[0].u32Iso, s_stSceneState.astMainPipe[0].u64Exposure);

#ifdef APPLY_DYNAMIC_PARAMETER_UPDATE
		s32Ret = SCENE_SetPipeDynamicParam();
		CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
#endif

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			s_stSceneState.astMainPipe[i].u64LastNormalExposure = s_stSceneState.astMainPipe[i].u64Exposure;
			s_stSceneState.astMainPipe[i].u32LastNormalIso = s_stSceneState.astMainPipe[i].u32Iso;
			s_stSceneState.astMainPipe[i].s32LastNormalLV = s_stSceneState.astMainPipe[i].s32LV;
		}

		usleep(TIME_NORMAL_THREAD);
	}

	UNUSED(pVoid);

	return CVI_NULL;
}

CVI_VOID *SCENE_Luminance_AutoThread(CVI_VOID *pVoid)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VI_PIPE ViPipe;
	CVI_U32 i, j;

	prctl(PR_SET_NAME, (unsigned long)"CVI_SCENE_LuminanceThread", 0, 0, 0);

	while (s_stSceneState.stThreadLuminance.bThreadFlag == CVI_TRUE) {
		// set AE compersation with exposure calculated by effective ISP dev
		for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++) {
			ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

			s32Ret = SCENE_CalculateExp(ViPipe,
				&(s_stSceneState.astMainPipe[i].u32Iso),
				&(s_stSceneState.astMainPipe[i].u64Exposure),
				&(s_stSceneState.astMainPipe[i].s32LV));
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
		}

		// printf("LuminanceAutoThread : ISO:%7u, Exposure:%10lu\n",
		//	s_stSceneState.astMainPipe[0].u32Iso, s_stSceneState.astMainPipe[0].u64Exposure);

#ifdef APPLY_DYNAMIC_PARAMETER_UPDATE
		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
				CVI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
				CVI_U64 u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
				CVI_U64 u64LastExposure = s_stSceneState.astMainPipe[i].u64LastLuminanceExposure;

				CVI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

				s32Ret = CVI_SCENE_SetDynamicAE_AutoGenerate(
					s32Index,
					u64Exposure, u64LastExposure, u8PipeIndex);
				CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
			}
		}

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
				CVI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
				CVI_U64 u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
				CVI_U64 u64LastExposure = s_stSceneState.astMainPipe[i].u64LastLuminanceExposure;

				CVI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

				s32Ret = CVI_SCENE_SetDynamicVideoGamma_AutoGenerate(
					s32Index,
					u64Exposure, u64LastExposure, u8PipeIndex);
				CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
			}
		}

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++) {
				CVI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
				CVI_U32 u32Iso = s_stSceneState.astMainPipe[i].u32Iso;
				CVI_U32 u32LastISO = s_stSceneState.astMainPipe[i].u32LastLuminanceISO;
				CVI_S32 s32LV = s_stSceneState.astMainPipe[i].s32LV;
				CVI_S32 s32LastLV = s_stSceneState.astMainPipe[i].s32LastLuminanceLV;

				CVI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

				s32Ret = CVI_SCENE_SetDynamicLinearDRC_AutoGenerate(
					s32Index,
					u32Iso, u32LastISO, s32LV, s32LastLV,
					u8PipeIndex);
				CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
			}
		}
#endif

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			s_stSceneState.astMainPipe[i].u64LastLuminanceExposure
				= s_stSceneState.astMainPipe[i].u64Exposure;
			s_stSceneState.astMainPipe[i].u32LastLuminanceISO
				= s_stSceneState.astMainPipe[i].u32Iso;
			s_stSceneState.astMainPipe[i].s32LastLuminanceLV
				= s_stSceneState.astMainPipe[i].s32LV;
		}

		usleep(TIME_LUMINANCE_THREAD);
	}

	UNUSED(pVoid);

	return CVI_NULL;
}

CVI_VOID *SCENE_NotLinear_AutoThread(CVI_VOID *pVoid)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VI_PIPE ViPipe;
	CVI_U32 i;

	prctl(PR_SET_NAME, (unsigned long)"CVI_SCENE_NotLinearThread", 0, 0, 0);

	while (s_stSceneState.stThreadNotLinear.bThreadFlag == CVI_TRUE) {
		if (s_stSceneMode.enPipeMode != CVI_SCENE_PIPE_MODE_WDR) {
			usleep(1000000);
			continue;
		}

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

			s32Ret = SCENE_CalculateExp(ViPipe,
				&(s_stSceneState.astMainPipe[i].u32Iso),
				&(s_stSceneState.astMainPipe[i].u64Exposure),
				&(s_stSceneState.astMainPipe[i].s32LV));
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);

			s32Ret = SCENE_CalculateWdrParam(ViPipe,
				&(s_stSceneState.astMainPipe[i].u32ActRation),
				&(s_stSceneState.astMainPipe[i].u32HDRBrightRatio));
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
		}

		// printf("NotLinearAutoThread (WDR) : ISO:%7u, Exposure:%10lu, ActRation:%5u, BrightRatio:%5u\n",
		//	s_stSceneState.astMainPipe[0].u32Iso, s_stSceneState.astMainPipe[0].u64Exposure,
		//	s_stSceneState.astMainPipe[0].u32ActRation, s_stSceneState.astMainPipe[0].u32HDRBrightRatio);

#ifdef APPLY_DYNAMIC_PARAMETER_UPDATE
		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

			s32Ret = CVI_SCENE_SetDynamicFsWdr_AutoGenerate(ViPipe,
				s_stSceneState.astMainPipe[i].u32Iso, s_stSceneState.astMainPipe[i].u32LastNotLinearISO,
				s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex,
				s_stSceneState.astMainPipe[i].u32ActRation);
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
		}

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

			CVI_U32 u32Iso = s_stSceneState.astMainPipe[i].u32Iso;
			CVI_U32 u32LastISO = s_stSceneState.astMainPipe[i].u32LastNotLinearISO;
			CVI_S32 s32LV = s_stSceneState.astMainPipe[i].s32LV;
			CVI_S32 s32LastLV = s_stSceneState.astMainPipe[i].s32LastNotLinearLV;

			s32Ret = CVI_SCENE_SetDynamicThreadDRC_AutoGenerate(ViPipe,
				u32Iso, u32LastISO, s32LV, s32LastLV,
				s_stSceneState.astMainPipe[i].u32ActRation,
				s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
			CVI_SCENECOMM_CHECK(s32Ret, CVI_SCENE_EINTER);
		}
#endif

		for (i = 0; i < s_stSceneState.u32MainPipeNum; i++) {
			s_stSceneState.astMainPipe[i].u64LastNotLinearExposure
				= s_stSceneState.astMainPipe[i].u64Exposure;
			s_stSceneState.astMainPipe[i].u32LastNotLinearISO
				= s_stSceneState.astMainPipe[i].u32Iso;
			s_stSceneState.astMainPipe[i].s32LastNotLinearLV
				= s_stSceneState.astMainPipe[i].s32LV;
		}

		usleep(TIME_NOTLINEAR_THREAD);
	}

	UNUSED(pVoid);

	return CVI_NULL;
}

static CVI_S32 SCENE_StartAutoThread(CVI_VOID)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	pthread_attr_init(&(s_stSceneState.stThreadNormalAttr));
	pthread_attr_setdetachstate(&(s_stSceneState.stThreadNormalAttr), PTHREAD_CREATE_DETACHED);

	pthread_attr_init(&(s_stSceneState.stThreadLuminanceAttr));
	pthread_attr_setdetachstate(&(s_stSceneState.stThreadLuminanceAttr), PTHREAD_CREATE_DETACHED);

	pthread_attr_init(&(s_stSceneState.stThreadNotLinearAttr));
	pthread_attr_setdetachstate(&(s_stSceneState.stThreadNotLinearAttr), PTHREAD_CREATE_DETACHED);

	if (s_stSceneState.stThreadNormal.bThreadFlag == CVI_FALSE) {
		s_stSceneState.stThreadNormal.bThreadFlag = CVI_TRUE;
		s32Ret = pthread_create(&(s_stSceneState.stThreadNormal.pThread),
			&(s_stSceneState.stThreadNormalAttr),
			SCENE_Normal_AutoThread, NULL);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	if (s_stSceneState.stThreadLuminance.bThreadFlag == CVI_FALSE) {
		s_stSceneState.stThreadLuminance.bThreadFlag = CVI_TRUE;
		s32Ret = pthread_create(&(s_stSceneState.stThreadLuminance.pThread),
			&(s_stSceneState.stThreadLuminanceAttr),
			SCENE_Luminance_AutoThread, NULL);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	if ((s_stSceneState.stThreadNotLinear.bThreadFlag == CVI_FALSE)
		&& (s_stSceneMode.enPipeMode == CVI_SCENE_PIPE_MODE_WDR)) {
		s_stSceneState.stThreadNotLinear.bThreadFlag = CVI_TRUE;
		s32Ret = pthread_create(&(s_stSceneState.stThreadNotLinear.pThread),
			&(s_stSceneState.stThreadNotLinearAttr),
			SCENE_NotLinear_AutoThread, NULL);
		CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);
	}

	return CVI_SUCCESS;
}

static CVI_S32 SCENE_StopAutoThread(CVI_VOID)
{
	if (s_stSceneState.stThreadNormal.bThreadFlag == CVI_TRUE) {
		s_stSceneState.stThreadNormal.bThreadFlag = CVI_FALSE;
		pthread_attr_destroy(&(s_stSceneState.stThreadNormalAttr));
	}

	if (s_stSceneState.stThreadLuminance.bThreadFlag == CVI_TRUE) {
		s_stSceneState.stThreadLuminance.bThreadFlag = CVI_FALSE;
		pthread_attr_destroy(&(s_stSceneState.stThreadLuminanceAttr));
	}

	if (s_stSceneState.stThreadNotLinear.bThreadFlag == CVI_TRUE) {
		s_stSceneState.stThreadNotLinear.bThreadFlag = CVI_FALSE;
		pthread_attr_destroy(&(s_stSceneState.stThreadNotLinearAttr));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_Init(const CVI_SCENE_PARAM_S *pstSceneParam)
{
	CVI_SCENECOMM_CHECK_POINTER(pstSceneParam, CVI_SCENE_ENONPTR);

	if (s_stSceneState.bSceneInit == CVI_TRUE) {
		printf("SCENE module has already been inited\n");
		return CVI_SCENE_EINITIALIZED;
	}

	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_SCENE_SetPipeParam_AutoGenerate(pstSceneParam->astPipeParam, CVI_SCENE_PIPETYPE_NUM);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	memset(&s_stSceneMode, 0, sizeof(CVI_SCENE_MODE_S));
	memset(&s_stSceneState, 0, sizeof(SCENE_STATE_S));

	s_stSceneState.bSceneInit = CVI_TRUE;

	printf("SCENE module has been inited successfully\n");

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_SetSceneMode(const CVI_SCENE_MODE_S *pstSceneMode)
{
	SCENE_CHECK_INIT();
	CVI_SCENECOMM_CHECK_POINTER(pstSceneMode, CVI_SCENE_ENONPTR);

	CVI_S32 s32Ret = CVI_SUCCESS;

	CVI_MUTEX_LOCK(g_mScene_lock);

	if ((pstSceneMode->enPipeMode != CVI_SCENE_PIPE_MODE_LINEAR) &&
		(pstSceneMode->enPipeMode != CVI_SCENE_PIPE_MODE_WDR)) {
		printf("The pipe mode must be LINEAR or WDR\n");
		CVI_MUTEX_UNLOCK(g_mScene_lock);
		return CVI_SCENE_EINVAL;
	}

	for (CVI_U32 i = 0; i < CVI_SCENE_PIPE_MAX_NUM; i++) {
		if (pstSceneMode->astPipeAttr[i].bEnable != CVI_TRUE) {
			continue;
		}

		if (i != pstSceneMode->astPipeAttr[i].VcapPipeHdl) {
			printf("The value of pipe in scene pipe array must be equal to the index of array\n");
			CVI_MUTEX_UNLOCK(g_mScene_lock);
			return CVI_SCENE_EINVAL;
		}

		if (pstSceneMode->astPipeAttr[i].u8PipeParamIndex >= CVI_SCENE_PIPETYPE_NUM) {
			printf("Pipe param index is out of range\n");
			CVI_MUTEX_UNLOCK(g_mScene_lock);
			return CVI_SCENE_EOUTOFRANGE;
		}
	}

	memcpy(&s_stSceneMode, pstSceneMode, sizeof(CVI_SCENE_MODE_S));

	s32Ret = SCENE_SetMainPipeState();
	if (s32Ret != CVI_SUCCESS) {
		printf("SCENE_SetMainIspState failed\n");
		CVI_MUTEX_UNLOCK(g_mScene_lock);
		return CVI_FAILURE;
	}

	s32Ret = SCENE_SetVIPipeParam();
	if (s32Ret != CVI_SUCCESS) {
		printf("SCENE_SetIspParam failed\n");
		CVI_MUTEX_UNLOCK(g_mScene_lock);
		return CVI_FAILURE;
	}

	s32Ret = SCENE_StartAutoThread();
	if (s32Ret != CVI_SUCCESS) {
		printf("SCENE_StartThread failed\n");
		CVI_MUTEX_UNLOCK(g_mScene_lock);
		return CVI_FAILURE;
	}

	CVI_MUTEX_UNLOCK(g_mScene_lock);

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_Deinit(void)
{
	SCENE_CHECK_INIT();

	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SCENE_StopAutoThread();
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	pthread_mutex_lock(&g_mScene_lock);
	s_stSceneState.bSceneInit = CVI_FALSE;
	pthread_mutex_unlock(&g_mScene_lock);

	printf("SCENE Module has been deinited successfully!\n");

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_GetSceneMode(CVI_SCENE_MODE_S *pstSceneMode)
{
	SCENE_CHECK_INIT();
	CVI_SCENECOMM_CHECK_POINTER(pstSceneMode, CVI_SCENE_ENONPTR);

	memcpy(pstSceneMode, &s_stSceneMode, sizeof(CVI_SCENE_MODE_S));

	return CVI_SUCCESS;
}

CVI_S32 CVI_SCENE_Pause(CVI_BOOL bEnable)
{
	SCENE_CHECK_INIT();

	CVI_S32 s32Ret = CVI_SUCCESS;

	s_stSceneState.bPause = bEnable;

	s32Ret = CVI_SCENE_SetPause_AutoGenerate(bEnable);
	CVI_SCENECOMM_CHECK_RETURN(s32Ret, CVI_SCENE_EINTER);

	return CVI_SUCCESS;
}
