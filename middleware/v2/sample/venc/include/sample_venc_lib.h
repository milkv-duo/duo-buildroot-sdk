/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_common.h
 * Description:
 */

#ifndef __SAMPLE_VENC_LIB_H__
#define __SAMPLE_VENC_LIB_H__

#include "sample_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct _sampleVenc_ {
	commonInputCfg commonIc;
	vencChnCtx chnCtx[VENC_MAX_CHN_NUM];
} sampleVenc;

CVI_S32 venc_main(int argc, char **argv);
CVI_S32 SAMPLE_VENC_INIT_CFG(sampleVenc *psv, int argc, char **argv);
CVI_VOID initInputCfg(commonInputCfg *pcic, chnInputCfg *pIc);
CVI_S32 parseEncArgv(sampleVenc *psv, chnInputCfg *pIc, CVI_S32 argc, char **argv);
CVI_S32 SAMPLE_VENC_START(sampleVenc *psv);
CVI_S32 SAMPLE_VENC_STOP(sampleVenc *psv);
CVI_S32 SAMPLE_VENC_MOVE_TO_STOP_STATE(sampleVenc *psv);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
