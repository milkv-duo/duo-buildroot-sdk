/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_sys.c
 * Description:
 *   Common sample code for video input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "sample_comm.h"

/******************************************************************************
 * function : vb init & MMF system init
 ******************************************************************************/
CVI_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig)
{
	CVI_S32 s32Ret = CVI_FAILURE;

	CVI_SYS_Exit();
	CVI_VB_Exit();

	if (pstVbConfig == NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "input parameter is null, it is invaild!\n");
		return CVI_FAILURE;
	}

	s32Ret = CVI_VB_SetConfig(pstVbConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_SetConf failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_VB_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VB_Init failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_SYS_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_Init failed!\n");
		CVI_VB_Exit();
		return s32Ret;
	}

	return CVI_SUCCESS;
}

/******************************************************************************
 * function : vb exit & MMF system exit
 ******************************************************************************/
CVI_VOID SAMPLE_COMM_SYS_Exit(void)
{
	//CVI_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {CVI_TRUE, };

	//for (VPSS_GRP VpssGrp = 0; VpssGrp < VPSS_MAX_GRP_NUM; ++VpssGrp)
		//SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	//SAMPLE_COMM_VO_Exit();
	CVI_SYS_Exit();
	CVI_VB_Exit();
}

CVI_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = VoLayer;
	stDestChn.s32ChnId = VoChn;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VI-VO)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = VoLayer;
	stDestChn.s32ChnId = VoChn;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VI-VO)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VI-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VI-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VI-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VI;
	stSrcChn.s32DevId = ViPipe;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VI-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = VoLayer;
	stDestChn.s32ChnId = VoChn;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VPSS-VO)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VO;
	stDestChn.s32DevId = VoLayer;
	stDestChn.s32ChnId = VoChn;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VPSS-VO)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VPSS-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VPSS-VENC)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VDEC-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VDEC-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_Bind_VPSS(VPSS_GRP VpssGrpSrc, VPSS_CHN VpssChnSrc, VPSS_GRP VpssGrpDst)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrpSrc;
	stSrcChn.s32ChnId = VpssChnSrc;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrpDst;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(VPSS-VPSS)");

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VPSS_UnBind_VPSS(VPSS_GRP VpssGrpSrc, VPSS_CHN VpssChnSrc, VPSS_GRP VpssGrpDst)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stSrcChn.enModId = CVI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrpSrc;
	stSrcChn.s32ChnId = VpssChnSrc;

	stDestChn.enModId = CVI_ID_VPSS;
	stDestChn.s32DevId = VpssGrpDst;
	stDestChn.s32ChnId = 0;

	CHECK_RET(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(VPSS-VPSS)");

	return CVI_SUCCESS;
}

/* SAMPLE_COMM_FRAME_SaveToFile:
 *   Save videoframe to the file
 *
 * [in]filename: char string of the file to save data.
 * [IN]pstVideoFrame: the videoframe whose data will be saved to file.
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_FRAME_SaveToFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame)
{
	FILE *fp;
	CVI_U32 u32len, u32DataLen;

	fp = fopen(filename, "w");
	if (fp == CVI_NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "open data file error\n");
		return CVI_FAILURE;
	}
	for (int i = 0; i < 3; ++i) {
		u32DataLen = pstVideoFrame->stVFrame.u32Stride[i] * pstVideoFrame->stVFrame.u32Height;
		if (u32DataLen == 0)
			continue;
		if (i > 0 && ((pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) ||
			(pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12) ||
			(pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21)))
			u32DataLen >>= 1;

		pstVideoFrame->stVFrame.pu8VirAddr[i]
			= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[i], pstVideoFrame->stVFrame.u32Length[i]);

		CVI_TRACE_LOG(CVI_DBG_INFO, "plane(%d): paddr(%#"PRIx64") vaddr(%p) stride(%d)\n",
			   i, pstVideoFrame->stVFrame.u64PhyAddr[i],
			   pstVideoFrame->stVFrame.pu8VirAddr[i],
			   pstVideoFrame->stVFrame.u32Stride[i]);
		CVI_TRACE_LOG(CVI_DBG_INFO, " data_len(%d) plane_len(%d)\n",
			      u32DataLen, pstVideoFrame->stVFrame.u32Length[i]);
		u32len = fwrite(pstVideoFrame->stVFrame.pu8VirAddr[i], u32DataLen, 1, fp);
		if (u32len <= 0) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "fwrite data(%d) error\n", i);
			break;
		}
		CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[i], pstVideoFrame->stVFrame.u32Length[i]);
	}

	fclose(fp);
	return CVI_SUCCESS;
}

/* SAMPLE_COMM_PrepareFrame:
 *   Prepare videoframe per size & format.
 *
 * [in]stSize: the size of videoframe
 * [in]enPixelFormat: pixel format of videoframe
 * [Out]pstVideoFrame: the videoframe generated.
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_PrepareFrame(SIZE_S stSize, PIXEL_FORMAT_E enPixelFormat, VIDEO_FRAME_INFO_S *pstVideoFrame)
{
	VB_BLK blk;
	VB_CAL_CONFIG_S stVbCalConfig;

	if (pstVideoFrame == CVI_NULL) {
		SAMPLE_PRT("Null pointer!\n");
		return CVI_FAILURE;
	}

	COMMON_GetPicBufferConfig(stSize.u32Width, stSize.u32Height, enPixelFormat, DATA_BITWIDTH_8
		, COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbCalConfig);

	memset(pstVideoFrame, 0, sizeof(*pstVideoFrame));
	pstVideoFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pstVideoFrame->stVFrame.enPixelFormat = enPixelFormat;
	pstVideoFrame->stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	pstVideoFrame->stVFrame.enColorGamut = COLOR_GAMUT_BT601;
	pstVideoFrame->stVFrame.u32Width = stSize.u32Width;
	pstVideoFrame->stVFrame.u32Height = stSize.u32Height;
	pstVideoFrame->stVFrame.u32Stride[0] = stVbCalConfig.u32MainStride;
	pstVideoFrame->stVFrame.u32Stride[1] = stVbCalConfig.u32CStride;
	pstVideoFrame->stVFrame.u32TimeRef = 0;
	pstVideoFrame->stVFrame.u64PTS = 0;
	pstVideoFrame->stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;

	blk = CVI_VB_GetBlock(VB_INVALID_POOLID, stVbCalConfig.u32VBSize);
	if (blk == VB_INVALID_HANDLE) {
		SAMPLE_PRT("Can't acquire vb block\n");
		return CVI_FAILURE;
	}

	pstVideoFrame->u32PoolId = CVI_VB_Handle2PoolId(blk);
	pstVideoFrame->stVFrame.u32Length[0] = stVbCalConfig.u32MainYSize;
	pstVideoFrame->stVFrame.u32Length[1] = stVbCalConfig.u32MainCSize;
	pstVideoFrame->stVFrame.u64PhyAddr[0] = CVI_VB_Handle2PhysAddr(blk);
	pstVideoFrame->stVFrame.u64PhyAddr[1] = pstVideoFrame->stVFrame.u64PhyAddr[0]
		+ ALIGN(stVbCalConfig.u32MainYSize, stVbCalConfig.u16AddrAlign);
	if (stVbCalConfig.plane_num == 3) {
		pstVideoFrame->stVFrame.u32Stride[2] = stVbCalConfig.u32CStride;
		pstVideoFrame->stVFrame.u32Length[2] = stVbCalConfig.u32MainCSize;
		pstVideoFrame->stVFrame.u64PhyAddr[2] = pstVideoFrame->stVFrame.u64PhyAddr[1]
			+ ALIGN(stVbCalConfig.u32MainCSize, stVbCalConfig.u16AddrAlign);
	}

	return CVI_SUCCESS;
}

/* SAMPLE_COMM_FRAME_CompareWithFile:
 *   Compare data with frame.
 *
 * [in]filename: file to read.
 * [in]pstVideoFrame: the video-frame to store data from file.
 * return: True if match; False if mismatch.
 */
CVI_BOOL SAMPLE_COMM_FRAME_CompareWithFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame)
{
	FILE *fp;
	CVI_U32 u32len, plane_len, data_len;
	CVI_U32 u32LumaData, u32ChromaData = 0, data_height;
	bool result = true;
	VB_CAL_CONFIG_S stVbCalConfig;

	u32LumaData = pstVideoFrame->stVFrame.u32Width;
	data_height = pstVideoFrame->stVFrame.u32Height;

	COMMON_GetPicBufferConfig(pstVideoFrame->stVFrame.u32Width, pstVideoFrame->stVFrame.u32Height,
		pstVideoFrame->stVFrame.enPixelFormat, DATA_BITWIDTH_8,
		COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbCalConfig);

	if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR ||
	    pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_BGR_888_PLANAR ||
	    pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_444) {
		u32ChromaData = u32LumaData;
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422) {
		u32ChromaData =  (pstVideoFrame->stVFrame.u32Width / 2);
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) {
		u32ChromaData =  (pstVideoFrame->stVFrame.u32Width / 2);
		data_height = pstVideoFrame->stVFrame.u32Height / 2;
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12 ||
		   pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21) {
		u32ChromaData = u32LumaData;
		data_height = pstVideoFrame->stVFrame.u32Height / 2;
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV16 ||
		   pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV61) {
		u32ChromaData = u32LumaData;
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUYV ||
		   pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_UYVY ||
		   pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YVYU ||
		   pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_VYUY) {
		u32LumaData *= 2;
		u32ChromaData = 0;
	} else if (pstVideoFrame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_400) {
		u32ChromaData = 0;
	}

	CVI_TRACE_LOG(CVI_DBG_INFO, "u32LumaSize(%d): u32ChromaSize(%d)\n",
		stVbCalConfig.u32MainYSize, stVbCalConfig.u32MainCSize);
	CVI_TRACE_LOG(CVI_DBG_INFO, "u32LumaData(%d): u32ChromaData(%d)\n", u32LumaData, u32ChromaData);
	fp = fopen(filename, "r");
	if (fp == CVI_NULL) {
		SAMPLE_PRT("open data file, %s, error\n", filename);
		return false;
	}

	CVI_U8 buffer[stVbCalConfig.u32MainYSize];
	CVI_U32 offset = 0;

	for (int i = 0; i < stVbCalConfig.plane_num; ++i) {
		plane_len = (i == 0) ? stVbCalConfig.u32MainYSize : stVbCalConfig.u32MainCSize;
		if (plane_len == 0)
			continue;
		data_len = (i == 0) ? u32LumaData : u32ChromaData;
		offset = 0;

		pstVideoFrame->stVFrame.pu8VirAddr[i]
			= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[i], pstVideoFrame->stVFrame.u32Length[i]);

		u32len = fread(buffer, plane_len, 1, fp);
		if (u32len <= 0) {
			SAMPLE_PRT("fread data(%d) error\n", i);
			result = false;
			break;
		}
		// line by line check to avoid padding data mismatch problem.
		for (CVI_U32 line = 0; line < data_height; ++line) {
			if (memcmp(buffer + offset, pstVideoFrame->stVFrame.pu8VirAddr[i] + offset, data_len) != 0) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "plane(%d) line(%d) offset(%d) data mismatch:\n",
					      i, line, offset);
				CVI_TRACE_LOG(CVI_DBG_ERR, " paddr(%#"PRIx64") vaddr(%p) stride(%d)\n",
					      pstVideoFrame->stVFrame.u64PhyAddr[i],
					      pstVideoFrame->stVFrame.pu8VirAddr[i],
					      pstVideoFrame->stVFrame.u32Stride[i]);

				result = false;
				break;
			}
			offset += pstVideoFrame->stVFrame.u32Stride[i];
		}
		CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[i], pstVideoFrame->stVFrame.u32Length[i]);
	}

	fclose(fp);
	return result;
}

/* SAMPLE_COMM_FRAME_LoadFromFile:
 *   Load data to frame, whose data loaded from given filename.
 *
 * [in]filename: file to read.
 * [in]pstVideoFrame: the video-frame to store data from file.
 * [in]stSize: size of image.
 * [in]enPixelFormat: format of image
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_FRAME_LoadFromFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame,
	SIZE_S *stSize, PIXEL_FORMAT_E enPixelFormat)
{
	VB_BLK blk;
	FILE *fp;
	CVI_U32 u32len;
	VB_CAL_CONFIG_S stVbCalConfig;

	COMMON_GetPicBufferConfig(stSize->u32Width, stSize->u32Height, enPixelFormat, DATA_BITWIDTH_8
		, COMPRESS_MODE_NONE, DEFAULT_ALIGN, &stVbCalConfig);

	memset(pstVideoFrame, 0, sizeof(*pstVideoFrame));
	pstVideoFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pstVideoFrame->stVFrame.enPixelFormat = enPixelFormat;
	pstVideoFrame->stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	pstVideoFrame->stVFrame.enColorGamut = COLOR_GAMUT_BT601;
	pstVideoFrame->stVFrame.u32Width = stSize->u32Width;
	pstVideoFrame->stVFrame.u32Height = stSize->u32Height;
	pstVideoFrame->stVFrame.u32Stride[0] = stVbCalConfig.u32MainStride;
	pstVideoFrame->stVFrame.u32Stride[1] = stVbCalConfig.u32CStride;
	pstVideoFrame->stVFrame.u32TimeRef = 0;
	pstVideoFrame->stVFrame.u64PTS = 0;
	pstVideoFrame->stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;

	blk = CVI_VB_GetBlock(VB_INVALID_POOLID, stVbCalConfig.u32VBSize);
	if (blk == VB_INVALID_HANDLE) {
		SAMPLE_PRT("Can't acquire vb block\n");
		return CVI_FAILURE;
	}

	//open data file & fread into the mmap address
	fp = fopen(filename, "r");
	if (fp == CVI_NULL) {
		SAMPLE_PRT("open data file error\n");
		return CVI_FAILURE;
	}

	pstVideoFrame->u32PoolId = CVI_VB_Handle2PoolId(blk);
	pstVideoFrame->stVFrame.u32Length[0] = stVbCalConfig.u32MainYSize;
	pstVideoFrame->stVFrame.u32Length[1] = stVbCalConfig.u32MainCSize;
	pstVideoFrame->stVFrame.u64PhyAddr[0] = CVI_VB_Handle2PhysAddr(blk);
	pstVideoFrame->stVFrame.u64PhyAddr[1] = pstVideoFrame->stVFrame.u64PhyAddr[0]
		+ ALIGN(stVbCalConfig.u32MainYSize, stVbCalConfig.u16AddrAlign);
	if (stVbCalConfig.plane_num == 3) {
		pstVideoFrame->stVFrame.u32Stride[2] = stVbCalConfig.u32CStride;
		pstVideoFrame->stVFrame.u32Length[2] = stVbCalConfig.u32MainCSize;
		pstVideoFrame->stVFrame.u64PhyAddr[2] = pstVideoFrame->stVFrame.u64PhyAddr[1]
			+ ALIGN(stVbCalConfig.u32MainCSize, stVbCalConfig.u16AddrAlign);
	}

	SAMPLE_PRT("length of buffer(%d, %d, %d)\n", pstVideoFrame->stVFrame.u32Length[0]
		, pstVideoFrame->stVFrame.u32Length[1], pstVideoFrame->stVFrame.u32Length[2]);
	SAMPLE_PRT("phy addr(%#"PRIx64", %#"PRIx64", %#"PRIx64")\n", pstVideoFrame->stVFrame.u64PhyAddr[0]
		, pstVideoFrame->stVFrame.u64PhyAddr[1], pstVideoFrame->stVFrame.u64PhyAddr[2]);

	for (int i = 0; i < stVbCalConfig.plane_num; ++i) {
		if (pstVideoFrame->stVFrame.u32Length[i] == 0)
			continue;
		pstVideoFrame->stVFrame.pu8VirAddr[i]
			= CVI_SYS_MmapCache(pstVideoFrame->stVFrame.u64PhyAddr[i],
					    pstVideoFrame->stVFrame.u32Length[i]);
		if (pstVideoFrame->stVFrame.pu8VirAddr[i] == CVI_NULL) {
			SAMPLE_PRT("mmap plane%d error\n", i);
			return CVI_FAILURE;
		}

		u32len = fread(pstVideoFrame->stVFrame.pu8VirAddr[i], pstVideoFrame->stVFrame.u32Length[i], 1, fp);
		if (u32len <= 0) {
			SAMPLE_PRT("fread plane%d error\n", i);
			return CVI_FAILURE;
		}
		CVI_SYS_IonInvalidateCache(pstVideoFrame->stVFrame.u64PhyAddr[i],
					   pstVideoFrame->stVFrame.pu8VirAddr[i],
					   pstVideoFrame->stVFrame.u32Length[i]);
		CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[i], pstVideoFrame->stVFrame.u32Length[i]);
	}

	fclose(fp);

	SAMPLE_PRT("read file done and send out frame.\n");

	return CVI_SUCCESS;
}

