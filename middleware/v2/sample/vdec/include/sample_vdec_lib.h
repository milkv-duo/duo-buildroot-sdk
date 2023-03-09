/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_common.h
 * Description:
 */

#ifndef __SAMPLE_VDEC_LIB_H__
#define __SAMPLE_VDEC_LIB_H__

#include "sample_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define STATUS_HELP	-2

enum _STAT_VDEC_ {
	STAT_ERR_VDEC_INIT_ATTR = -20,
	STAT_ERR_VDEC_INIT_VBPOOL,
	STAT_ERR_VDEC_COMMON_START,
};

typedef struct _vdecChnInputCfg_ {
	char input_path[MAX_STRING_LEN];
	char output_path[MAX_STRING_LEN];
	char codec[64];
	CVI_BOOL bDumpYUV;
	PAYLOAD_TYPE_E enType;
	CVI_U32 u32BufWidth;
	CVI_U32 u32BufHeight;
	CVI_U32 u32MaxFrameBuffer;
	CVI_S32 s32sendstream_timeout;
	CVI_S32 s32getframe_timeout;
	CVI_S32 s32PixelFormat;
} vdecChnInputCfg;

typedef struct _vdecInputCfg_ {
	vdecChnInputCfg chnInCfg[VDEC_MAX_CHN_NUM];
	CVI_U32 u32NumAllChns;
	CVI_U32 u32BindMode;
	int InitedVb;
	CVI_U32 u32TestMode;
	CVI_CHAR bitStreamFolder[MAX_STRING_LEN];
} vdecInputCfg;

typedef struct _sampleVdec_ {
	vdecInputCfg inputCfg;
	vdecChnCtx chnCtx[VDEC_MAX_CHN_NUM];
	CVI_U32 u32VdecNumAllChns;
} sampleVdec;

CVI_S32 SAMPLE_VDEC_START(sampleVdec *psvdec);
CVI_VOID SAMPLE_VDEC_STOP(sampleVdec *psvdec);
CVI_S32 SAMPLE_VDEC_INIT_VB(sampleVdec *psvdec);
CVI_S32 parseDecArgv(vdecInputCfg *pic, CVI_S32 argc, char **argv);
CVI_VOID SAMPLE_VDEC_HandleSig(CVI_S32 signo);
void printVdecHelp(char **argv);
int checkInputConfig(vdecInputCfg *pic);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
