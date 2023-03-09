#ifndef __SAMPLE_SCENE_H__
#define __SAMPLE_SCENE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include "cvi_common.h"
#elif defined(__CV181X__) || defined(__CV180X__)
#include <linux/cvi_common.h>
#endif // SOCs

#include "sample_comm.h"

typedef struct {
	SAMPLE_VI_CONFIG_S  stViConfig;
	SAMPLE_VO_CONFIG_S  stVoConfig;
	VI_DEV              ViDev;
	VI_PIPE             ViPipe;
	VI_CHN              ViChn;
	VPSS_GRP            VpssGrp;
	VPSS_CHN            VpssChn;
	VO_DEV              VoDev;
	VO_CHN              VoChn;
	CVI_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM];
} TStreamInfo;

CVI_S32 SAMPLE_SCENE_VoRotation_Start(TStreamInfo *ptStreamInfo);
CVI_S32 SAMPLE_SCENE_VoRotation_Stop(TStreamInfo *ptStreamInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // __SAMPLE_SCENE_H__
