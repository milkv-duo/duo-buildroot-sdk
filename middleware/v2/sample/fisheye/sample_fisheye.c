#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/param.h>
#include <inttypes.h>
#include <math.h>

#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"

#include "sample_comm.h"

#if 0
#define FILENAME "fisheye_ceil_560.yuv"
#define FILE_WIDTH    560
#define FILE_HEIGHT   560
#define FILE_MOUNT    FISHEYE_CEILING_MOUNT
#endif
#if 0
#define FILENAME "fisheye_wall_8M.yuv"
#define FILE_WIDTH    4002
#define FILE_HEIGHT   2654
#define FILE_MOUNT    FISHEYE_WALL_MOUNT
#endif
#if 0
#define FILENAME "fisheye_ceil_2M.yuv"
#define FILE_WIDTH    1414
#define FILE_HEIGHT   1402
#define FILE_MOUNT    FISHEYE_CEILING_MOUNT
#endif
#if 1
#define FILENAME "fisheye_floor_1M.yuv"
#define FILE_WIDTH    1024
#define FILE_HEIGHT   1024
#define FILE_MOUNT    FISHEYE_DESKTOP_MOUNT
#endif


CVI_S32 _prepare_frame(VIDEO_FRAME_INFO_S *pstVideoFrame, char *filename)
{
	FILE *fp;
	VB_BLK blk;
	SIZE_S size = { .u32Width = pstVideoFrame->stVFrame.u32Width, .u32Height = pstVideoFrame->stVFrame.u32Height };
	CVI_S32 ret = CVI_SUCCESS;

	if (SAMPLE_COMM_PrepareFrame(size, pstVideoFrame->stVFrame.enPixelFormat, pstVideoFrame) != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_PrepareFrame failed\n");
		return CVI_FAILURE;
	}
	blk = CVI_VB_PhysAddr2Handle(pstVideoFrame->stVFrame.u64PhyAddr[0]);

	pstVideoFrame->stVFrame.pu8VirAddr[0]
		= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[0], pstVideoFrame->stVFrame.u32Length[0]);
	pstVideoFrame->stVFrame.pu8VirAddr[1]
		= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[1], pstVideoFrame->stVFrame.u32Length[1]);
	pstVideoFrame->stVFrame.pu8VirAddr[2]
		= CVI_SYS_Mmap(pstVideoFrame->stVFrame.u64PhyAddr[2], pstVideoFrame->stVFrame.u32Length[2]);

	SAMPLE_PRT("phy addr(%#"PRIx64", %#"PRIx64", %#"PRIx64"\n", pstVideoFrame->stVFrame.u64PhyAddr[0]
		, pstVideoFrame->stVFrame.u64PhyAddr[1], pstVideoFrame->stVFrame.u64PhyAddr[2]);
	SAMPLE_PRT("vir addr(%p, %p, %p\n", pstVideoFrame->stVFrame.pu8VirAddr[0]
		, pstVideoFrame->stVFrame.pu8VirAddr[1], pstVideoFrame->stVFrame.pu8VirAddr[2]);

	if (filename) {
		fp = fopen(filename, "r");
		if (fp == CVI_NULL) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "fopen fail\n");
			CVI_VB_ReleaseBlock(blk);
			ret = CVI_FAILURE;
			goto OPEN_FAIL;
		}
		for (int i = 0; i < 3; i++)
			fread((void *)pstVideoFrame->stVFrame.pu8VirAddr[i]
			     , pstVideoFrame->stVFrame.u32Length[i], 1, fp);
		fclose(fp);
	}

OPEN_FAIL:
	CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[0], pstVideoFrame->stVFrame.u32Length[0]);
	CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[1], pstVideoFrame->stVFrame.u32Length[1]);
	CVI_SYS_Munmap(pstVideoFrame->stVFrame.pu8VirAddr[2], pstVideoFrame->stVFrame.u32Length[2]);
	return ret;
}

static CVI_S32 _handle_file_op(USAGE_MODE mode, VPSS_GRP VpssGrp)
{
	GDC_HANDLE hHandle;
	FISHEYE_ATTR_S stFisheyeAttr;
	GDC_TASK_ATTR_S stTask;
	VB_BLK blk;
	VPSS_CHN_ATTR_S stChnAttr;

	memset(&stTask, 0, sizeof(stTask));

	// prepare the in/out image info of the gdc task.
	stTask.stImgIn.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	stTask.stImgIn.stVFrame.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	stTask.stImgIn.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	stTask.stImgIn.stVFrame.u32Width = FILE_WIDTH;
	stTask.stImgIn.stVFrame.u32Height = FILE_HEIGHT;
	if (_prepare_frame(&stTask.stImgIn, FILENAME) != CVI_SUCCESS) {
		SAMPLE_PRT("prepare input from file failed.\n");
		return CVI_ERR_GDC_NOBUF;
	}

	stTask.stImgOut.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	stTask.stImgOut.stVFrame.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	stTask.stImgOut.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	stTask.stImgOut.stVFrame.u32Width = 1280;
	stTask.stImgOut.stVFrame.u32Height = 720;
	if (_prepare_frame(&stTask.stImgOut, CVI_NULL) != CVI_SUCCESS) {
		SAMPLE_PRT("prepare output buffer failed.\n");
		return CVI_ERR_GDC_NOBUF;
	}


	if (mode == MODE_01_1O) {
		CVI_VPSS_GetChnAttr(VpssGrp, 0, &stChnAttr);
		stChnAttr.stAspectRatio.enMode	      = ASPECT_RATIO_AUTO;
		stChnAttr.stAspectRatio.bEnableBgColor = CVI_TRUE;
		stChnAttr.stAspectRatio.u32BgColor = COLOR_RGB_BLACK;
		CVI_VPSS_SetChnAttr(VpssGrp, 0, &stChnAttr);
		CVI_VPSS_SendFrame(VpssGrp, &stTask.stImgIn, -1);
	} else {
		CVI_GDC_BeginJob(&hHandle);

		stFisheyeAttr.bEnable = CVI_TRUE;
		stFisheyeAttr.bBgColor = CVI_TRUE;
		stFisheyeAttr.u32BgColor = YUV_8BIT(0, 128, 128);
		stFisheyeAttr.s32HorOffset = FILE_WIDTH / 2;
		stFisheyeAttr.s32VerOffset = FILE_HEIGHT / 2;
		stFisheyeAttr.enMountMode = FILE_MOUNT;
		stFisheyeAttr.enUseMode = mode;
		stFisheyeAttr.u32RegionNum = 1;
		CVI_GDC_AddCorrectionTask(hHandle, &stTask, &stFisheyeAttr);

		if (CVI_GDC_EndJob(hHandle) != CVI_SUCCESS) {
			SAMPLE_PRT("GDC Job failed.\n");
			return CVI_FAILURE;
		}

		if (mode == MODE_02_1O4R) {
			CVI_VPSS_GetChnAttr(VpssGrp, 0, &stChnAttr);
			stChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_MANUAL;
			stChnAttr.stAspectRatio.bEnableBgColor = CVI_FALSE;
			stChnAttr.stAspectRatio.stVideoRect.s32X = 960;
			stChnAttr.stAspectRatio.stVideoRect.s32Y = 200;
			stChnAttr.stAspectRatio.stVideoRect.u32Width = 320;
			stChnAttr.stAspectRatio.stVideoRect.u32Height = 320;
			CVI_VPSS_SetChnAttr(VpssGrp, 0, &stChnAttr);

			CVI_VPSS_SendChnFrame(VpssGrp, 0, &stTask.stImgOut, -1);
			CVI_VPSS_SendFrame(VpssGrp, &stTask.stImgIn, -1);
		} else {
			CVI_VO_SendFrame(0, 0, &stTask.stImgOut, -1);
		}
	}

	blk = CVI_VB_PhysAddr2Handle(stTask.stImgIn.stVFrame.u64PhyAddr[0]);
	if (blk != VB_INVALID_HANDLE)
		CVI_VB_ReleaseBlock(blk);
	blk = CVI_VB_PhysAddr2Handle(stTask.stImgOut.stVFrame.u64PhyAddr[0]);
	if (blk != VB_INVALID_HANDLE)
		CVI_VB_ReleaseBlock(blk);
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_FISHEYE_GDC_Fisheye_VoRotation(void)
{
	VB_CONFIG_S        stVbConf;
	CVI_U32	           u32BlkSize;
	SIZE_S stSize = { .u32Width = FILE_WIDTH, .u32Height = FILE_HEIGHT };
	CVI_S32 s32Ret = CVI_SUCCESS;

	/************************************************
	 * step1:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 2;

	// for VI
	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_888_PLANAR,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 3;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	// for vpss/vo
	u32BlkSize = COMMON_GetPicBufferSize(736, 1280, PIXEL_FORMAT_RGB_888_PLANAR,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 6;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step2:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = FILE_WIDTH;
	stVpssGrpAttr.u32MaxH                        = FILE_HEIGHT;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_MANUAL;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32X = 960;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32Y = 200;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Width = 320;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Height = 320;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	/************************************************
	 * step5:  Init GDC
	 ************************************************/
	int op = 0;
	#if 0
	LOG_LEVEL_CONF_S log = { .enModId = CVI_ID_GDC, .s32Level = 7 };

	CVI_LOG_SetLevelConf(&log);
	#endif

	do {
		printf("---------------select the fisheye mode!---------------\n");
		printf("PANORAMA_360 = 1, PANORAMA_180 = 2,\n");
		printf("MODE_01_1O   = 3, MODE_02_1O4R = 4,\n");
		printf("MODE_03_4R   = 5, MODE_04_1P2R = 6,\n");
		printf("MODE_05_1P2R = 7, MODE_06_1P   = 8,\n");
		printf("MODE_07_2P   = 9,\n");
		printf("others to exit\n");
		scanf("%d", &op);
		if (op < MODE_MAX) {
			if (_handle_file_op(op, VpssGrp) != CVI_SUCCESS)
				break;
		} else
			break;
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

RECT_S keep_ratio_resize(SIZE_S in, SIZE_S out)
{
	RECT_S rect;
	float ratio = MIN2((float)out.u32Width / in.u32Width, (float)out.u32Height / in.u32Height);

	rect.u32Height = (float)in.u32Height * ratio + 0.5;
	rect.u32Width = (float)in.u32Width * ratio + 0.5;
	rect.s32X = (out.u32Width - rect.u32Width) >> 1;
	rect.s32Y = (out.u32Height - rect.u32Height) >> 1;
	return rect;
}

static CVI_S32 _vi_get_chn_frame(CVI_U8 chn)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, chn, &stVideoFrame, 1000) == 0) {
		FILE *output;
		size_t image_size = stVideoFrame.stVFrame.u32Length[0] + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char    img_name[128] = {0, };

		SAMPLE_PRT("width: %d, height: %d, total_buf_length: %zu\n",
			   stVideoFrame.stVFrame.u32Width,
			   stVideoFrame.stVFrame.u32Height, image_size);

		snprintf(img_name, sizeof(img_name), "sample_%d.yuv", chn);

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_%d.yuv", chn);
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, chn, &stVideoFrame);
				SAMPLE_PRT("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height / 2;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			SAMPLE_PRT("plane(%d): paddr(%#"PRIx64") vaddr(0x%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);

		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, chn, &stVideoFrame) != 0)
			SAMPLE_PRT("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}
	CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

static CVI_S32 _handle_sns_op(int mode, SIZE_S stSize, FISHEYE_ATTR_S	*pstFisheyeAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	pstFisheyeAttr->enUseMode = 0;
	if (mode == 0) {
		pstFisheyeAttr->u32RegionNum	  = 1;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_NORMAL;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 20;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height;

		printf("Pan(0~360): ");
		scanf("%d", &pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan);
		printf("Tilt(0~360): ");
		scanf("%d", &pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt);
		printf("HorZoom(0~4095): ");
		scanf("%d", &pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom);
		printf("u32VerZoom(0~4095): ");
		scanf("%d", &pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom);
	} else if (mode == MODE_PANORAMA_360) {
		SIZE_S in, out;
		RECT_S rect;

		in.u32Height = MIN2(pstFisheyeAttr->s32HorOffset, pstFisheyeAttr->s32VerOffset);
		in.u32Width = M_PI * in.u32Height;
		out = stSize;
		rect = keep_ratio_resize(in, out);

		pstFisheyeAttr->u32RegionNum	  = 1;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = rect.s32X;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = rect.s32Y;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = rect.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = rect.u32Height;
	} else if (mode == MODE_PANORAMA_180) {
		pstFisheyeAttr->u32RegionNum	  = 1;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_180_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height;
	} else if (mode == MODE_02_1O4R) {
		double width_sec = 1280 / 40;
		double height_sec = 720 / 40;

		pstFisheyeAttr->u32RegionNum	  = 4;

		for (int i = 0; i < 4; ++i) {
			pstFisheyeAttr->astFishEyeRegionAttr[i].enViewMode	    = FISHEYE_VIEW_NORMAL;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32InRadius	    = 0;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32OutRadius	    = 4095;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Pan		    = 90 * i;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Tilt		    = 20;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32HorZoom	    = stSize.u32Width * 2048 / 1280;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32VerZoom	    = stSize.u32Height * 2048 / 720;
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32X	    = (width_sec * 15) * (i % 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32Y	    = (height_sec * 20) * (i / 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Width    = (width_sec * 15);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Height   = (height_sec * 20);
		}
	} else if (mode == MODE_03_4R) {
		pstFisheyeAttr->u32RegionNum	  = 4;

		for (int i = 0; i < 4; ++i) {
			pstFisheyeAttr->astFishEyeRegionAttr[i].enViewMode	    = FISHEYE_VIEW_NORMAL;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32InRadius	    = 0;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32OutRadius	    = 4095;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Pan		    = 90 * i;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Tilt		    = 20;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32HorZoom	    = 2048;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32VerZoom	    = 2048;
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32X	    = (stSize.u32Width >> 1) * (i % 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32Y	    = (stSize.u32Height >> 1) * (i / 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Width    = stSize.u32Width >> 1;
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Height   = stSize.u32Height >> 1;
		}
	} else if (mode == MODE_04_1P2R) {
		pstFisheyeAttr->u32RegionNum	  = 3;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height >> 1;

		pstFisheyeAttr->astFishEyeRegionAttr[1].enViewMode	    = FISHEYE_VIEW_NORMAL;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Tilt		    = 20;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32VerZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32Y	    = (stSize.u32Height >> 1);
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Width    = stSize.u32Width >> 1;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Height   = stSize.u32Height >> 1;

		pstFisheyeAttr->astFishEyeRegionAttr[2].enViewMode	    = FISHEYE_VIEW_NORMAL;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32Pan		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32Tilt		    = 20;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32VerZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.s32X	    = (stSize.u32Width >> 1);
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.s32Y	    = (stSize.u32Height >> 1);
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.u32Width    = stSize.u32Width >> 1;
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.u32Height   = stSize.u32Height >> 1;
	} else if (mode == MODE_05_1P2R) {
		double width_sec = stSize.u32Width / 40;
		double height_sec = stSize.u32Height / 40;

		pstFisheyeAttr->u32RegionNum	  = 3;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width  = (width_sec * 27);
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height = (height_sec * 40);

		pstFisheyeAttr->astFishEyeRegionAttr[1].enViewMode	    = FISHEYE_VIEW_NORMAL;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32OutRadius	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Tilt		    = 20;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32VerZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32X	    = (width_sec * 27);
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Width    = (width_sec * 13);
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Height   = (height_sec * 20);

		pstFisheyeAttr->astFishEyeRegionAttr[2].enViewMode	    = FISHEYE_VIEW_NORMAL;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32InRadius	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32OutRadius	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32Pan		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32Tilt		    = 20;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[2].u32VerZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.s32X	    = (width_sec * 27);
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.s32Y	    = (height_sec * 20);
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.u32Width    = (width_sec * 13);
		pstFisheyeAttr->astFishEyeRegionAttr[2].stOutRect.u32Height   = (height_sec * 20);
	} else if (mode == MODE_07_2P) {
		pstFisheyeAttr->u32RegionNum	  = 2;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 1024;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height >> 1;

		pstFisheyeAttr->astFishEyeRegionAttr[1].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Pan		    = 240;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32Tilt		    = 200;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32HorZoom	    = 1024;
		pstFisheyeAttr->astFishEyeRegionAttr[1].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.s32Y	    = stSize.u32Height >> 1;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[1].stOutRect.u32Height   = stSize.u32Height >> 1;
	} else if (mode == 10) {
		pstFisheyeAttr->u32RegionNum	  = 1;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 1024;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height;
	} else if (mode == 11) {
		pstFisheyeAttr->u32RegionNum	  = 1;

		pstFisheyeAttr->astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32InRadius	    = 768;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32OutRadius	    = 3800;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Pan		    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32Tilt		    = 180;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32HorZoom	    = 2048;
		pstFisheyeAttr->astFishEyeRegionAttr[0].u32VerZoom	    = 4095;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
		pstFisheyeAttr->astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height;
	} else if (mode == 12) {
		pstFisheyeAttr->u32RegionNum	  = 4;

		for (int i = 0; i < 4; ++i) {
			pstFisheyeAttr->astFishEyeRegionAttr[i].enViewMode	    = FISHEYE_VIEW_360_PANORAMA;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32InRadius	    = 768;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32OutRadius	    = 3800;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Pan		    = 90 * i;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32Tilt		    = 180;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32HorZoom	    = 1024;
			pstFisheyeAttr->astFishEyeRegionAttr[i].u32VerZoom	    = 4095;
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32X	    = (stSize.u32Width >> 1) * (i % 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.s32Y	    = (stSize.u32Height >> 1) * (i / 2);
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Width    = stSize.u32Width >> 1;
			pstFisheyeAttr->astFishEyeRegionAttr[i].stOutRect.u32Height   = stSize.u32Height >> 1;
		}
	}


	return s32Ret;
}

CVI_BOOL thread_run = CVI_FALSE;

CVI_VOID *fishey_1O4R_handler(CVI_VOID *data)
{
	VIDEO_FRAME_INFO_S stVideoFrame, stVideoFrameFisheye;
	VI_CHN   ViChn = 0;
	VI_CHN   ViExtChn = VI_EXT_CHN_START;
	VPSS_GRP VpssGrp = 0;
	UNUSED(data);

	while (thread_run) {
		if (CVI_VI_GetChnFrame(0, ViChn, &stVideoFrame, 500) != CVI_SUCCESS)
			continue;
		if (CVI_VI_GetChnFrame(0, ViExtChn, &stVideoFrameFisheye, 500) != CVI_SUCCESS) {
			CVI_VI_ReleaseChnFrame(0, ViChn, &stVideoFrame);
			continue;
		}

		CVI_VPSS_SendChnFrame(VpssGrp, 0, &stVideoFrameFisheye, -1);
		CVI_VPSS_SendFrame(VpssGrp, &stVideoFrame, -1);

		CVI_VI_ReleaseChnFrame(0, ViChn, &stVideoFrame);
		CVI_VI_ReleaseChnFrame(0, ViExtChn, &stVideoFrameFisheye);
	}
	pthread_exit(NULL);
}

CVI_S32 SAMPLE_FISHEYE_VI_Fisheye_VoRotation(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SENSOR0_TYPE;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	           u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 2;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 9;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(736, 1280, SAMPLE_PIXEL_FORMAT,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 4;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

#if 0
	LOG_LEVEL_CONF_S log = { .enModId = CVI_ID_GDC, .s32Level = 7 };

	CVI_LOG_SetLevelConf(&log);
#endif
	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stPipeAttr.bYuvSkip = CVI_FALSE;
	stPipeAttr.u32MaxW = stSize.u32Width;
	stPipeAttr.u32MaxH = stSize.u32Height;
	stPipeAttr.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
	stPipeAttr.enBitWidth = DATA_BITWIDTH_12;
	stPipeAttr.stFrameRate.s32SrcFrameRate = -1;
	stPipeAttr.stFrameRate.s32DstFrameRate = -1;
	stPipeAttr.bNrEn = CVI_TRUE;
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
	SIZE_S             stSizeVpss = { .u32Width = 1280, .u32Height = 720 };

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeVpss.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeVpss.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_AUTO;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor = CVI_FALSE;
	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	/************************************************
	 * step7:  Setup VI Ext Chn
	 ************************************************/
	VI_CHN_ATTR_S	    stChnAttr;
	VI_EXT_CHN_ATTR_S   stExtChnAttr;
	VI_CHN              ViExtChn       = VI_EXT_CHN_START;

	s32Ret = CVI_VI_GetChnAttr(ViPipe, ViChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("get vi chn:%d attr failed with:0x%x!\n", ViChn, s32Ret);
		return s32Ret;
	}

	stExtChnAttr.s32BindChn			  = ViChn;
	stExtChnAttr.enPixelFormat		  = stChnAttr.enPixelFormat;
	stExtChnAttr.stFrameRate.s32SrcFrameRate  = stChnAttr.stFrameRate.s32SrcFrameRate;
	stExtChnAttr.stFrameRate.s32DstFrameRate  = stChnAttr.stFrameRate.s32DstFrameRate;
	stExtChnAttr.stSize.u32Width		  = stChnAttr.stSize.u32Width;
	stExtChnAttr.stSize.u32Height		  = stChnAttr.stSize.u32Height;
	stExtChnAttr.u32Depth			  = 0;
	stExtChnAttr.enSource			  = VI_EXT_CHN_SOURCE_TAIL;

	/* start vi dev extern chn */
	s32Ret = CVI_VI_SetExtChnAttr(ViPipe, ViExtChn, &stExtChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("set vi extern chn attr failed with: 0x%x.!\n", s32Ret);
		return s32Ret;
	}

	/*Enable ext-channel*/
	s32Ret = CVI_VI_EnableChn(ViPipe, ViExtChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("enable vi extern chn failed with: 0x%x.!\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step8:  Setup Fisheye
	 ************************************************/
	FISHEYE_ATTR_S	   stFisheyeAttr;

	stFisheyeAttr.bEnable		  = CVI_TRUE;
	stFisheyeAttr.bBgColor		  = CVI_TRUE;
	stFisheyeAttr.u32BgColor	  = YUV_8BIT(0, 128, 128);
	stFisheyeAttr.s32HorOffset	  = stSize.u32Width >> 1;
	stFisheyeAttr.s32VerOffset	  = stSize.u32Height >> 1;
	stFisheyeAttr.u32TrapezoidCoef	  = 2;
	stFisheyeAttr.s32FanStrength	  = 0;
	stFisheyeAttr.enMountMode	  = FISHEYE_DESKTOP_MOUNT;
	stFisheyeAttr.u32RegionNum	  = 1;
	stFisheyeAttr.enUseMode = 0;

	stFisheyeAttr.astFishEyeRegionAttr[0].enViewMode	    = FISHEYE_VIEW_NORMAL;
	stFisheyeAttr.astFishEyeRegionAttr[0].u32InRadius	    = 0;
	stFisheyeAttr.astFishEyeRegionAttr[0].u32OutRadius
		= MIN2(stFisheyeAttr.s32HorOffset, stFisheyeAttr.s32VerOffset);
	stFisheyeAttr.astFishEyeRegionAttr[0].u32Pan		    = 180;
	stFisheyeAttr.astFishEyeRegionAttr[0].u32Tilt		    = 20;
	stFisheyeAttr.astFishEyeRegionAttr[0].u32HorZoom	    = 2048;
	stFisheyeAttr.astFishEyeRegionAttr[0].u32VerZoom	    = 2048;
	stFisheyeAttr.astFishEyeRegionAttr[0].stOutRect.s32X	    = 0;
	stFisheyeAttr.astFishEyeRegionAttr[0].stOutRect.s32Y	    = 0;
	stFisheyeAttr.astFishEyeRegionAttr[0].stOutRect.u32Width    = stSize.u32Width;
	stFisheyeAttr.astFishEyeRegionAttr[0].stOutRect.u32Height   = stSize.u32Height;

	s32Ret =  CVI_VI_SetExtChnFisheye(ViPipe, ViExtChn, &stFisheyeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("set fisheye attr failed with s32Ret:0x%x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViExtChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vo failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	int op = 0;
	pthread_t thread;

	do {
		printf("---------------select the fisheye mode!---------------\n");
		printf("NORMAL = 0,\n");
		printf("PANORAMA_360 = 1, PANORAMA_180 = 2,\n");
		printf("MODE_01_1O   = 3, MODE_02_1O4R = 4,\n");
		printf("MODE_03_4R   = 5, MODE_04_1P2R = 6,\n");
		printf("MODE_05_1P2R = 7, MODE_06_1P   = 8,\n");
		printf("MODE_07_2P   = 9,\n");

		printf("--following are all Panorama360 mode--\n");
		printf("1P(90)       = 10, 1P(180) = 11,\n");
		printf("4P(90/90/90/90) = 12\n");
		printf("25 capture,\n");
		printf("others to exit\n");
		scanf("%d", &op);
		if (op <= 12) {
			if (_handle_sns_op(op, stSize, &stFisheyeAttr) != CVI_SUCCESS)
				break;

			s32Ret = CVI_VI_DisableChn(ViPipe, ViExtChn);

			if (op == MODE_02_1O4R) {
				stExtChnAttr.stSize.u32Width		  = stSizeVpss.u32Width;
				stExtChnAttr.stSize.u32Height		  = stSizeVpss.u32Height;
				s32Ret = CVI_VI_SetExtChnAttr(ViPipe, ViExtChn, &stExtChnAttr);
			} else {
				stExtChnAttr.stSize.u32Width		  = stChnAttr.stSize.u32Width;
				stExtChnAttr.stSize.u32Height		  = stChnAttr.stSize.u32Height;
				s32Ret = CVI_VI_SetExtChnAttr(ViPipe, ViExtChn, &stExtChnAttr);
			}
			stFisheyeAttr.bEnable = !(op == MODE_01_1O);
			s32Ret =  CVI_VI_SetExtChnFisheye(ViPipe, ViExtChn, &stFisheyeAttr);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("set fisheye attr failed with s32Ret:0x%x!\n", s32Ret);
				return s32Ret;
			}

			if (op == MODE_02_1O4R) {
				if (!thread_run) {
					VPSS_CHN_ATTR_S stVpssChnAttr;

					thread_run = CVI_TRUE;
					SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViExtChn, VpssGrp);

					CVI_VPSS_GetChnAttr(VpssGrp, 0, &stVpssChnAttr);
					stVpssChnAttr.stAspectRatio.enMode	     = ASPECT_RATIO_MANUAL;
					stVpssChnAttr.stAspectRatio.bEnableBgColor   = CVI_FALSE;
					stVpssChnAttr.stAspectRatio.stVideoRect.s32X = 960;
					stVpssChnAttr.stAspectRatio.stVideoRect.s32Y = 200;
					stVpssChnAttr.stAspectRatio.stVideoRect.u32Width = 320;
					stVpssChnAttr.stAspectRatio.stVideoRect.u32Height = 320;
					CVI_VPSS_SetChnAttr(VpssGrp, 0, &stVpssChnAttr);

					pthread_create(&thread, NULL, fishey_1O4R_handler, NULL);
				}
			} else {
				if (thread_run) {
					VPSS_CHN_ATTR_S stVpssChnAttr;

					CVI_VPSS_GetChnAttr(VpssGrp, 0, &stVpssChnAttr);
					stVpssChnAttr.stAspectRatio.enMode = ASPECT_RATIO_AUTO;
					CVI_VPSS_SetChnAttr(VpssGrp, 0, &stVpssChnAttr);

					thread_run = CVI_FALSE;
					pthread_join(thread, NULL);
					SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViExtChn, VpssGrp);
				}
			}
			s32Ret = CVI_VI_EnableChn(ViPipe, ViExtChn);
		} else if (op == 25) {
			_vi_get_chn_frame(2);
		} else
			break;
	} while (1);


	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViExtChn, VpssGrp);

	CVI_VI_DisableChn(ViPipe, ViChn);
	CVI_VI_DisableChn(ViPipe, ViExtChn);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

void SAMPLE_FISHEYE_HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo) {
		SAMPLE_COMM_ISP_Stop(0);
		SAMPLE_COMM_ISP_Stop(1);
		SAMPLE_COMM_SYS_Exit();
		SAMPLE_PRT("Program termination abnormally\n");
	}
	exit(-1);
}

void SAMPLE_FISHEYE_Usage(char *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)FISHEYE from file - VO.\n");
	printf("\t 1)FISHEYE from sensor - VO.\n");
}

int main(int argc, char *argv[])
{
	CVI_S32 ret = CVI_FAILURE;
	CVI_S32 s32Index;

	if (argc < 2) {
		SAMPLE_FISHEYE_Usage(argv[0]);
		return CVI_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_FISHEYE_Usage(argv[0]);
		return CVI_SUCCESS;
	}

	signal(SIGINT, SAMPLE_FISHEYE_HandleSig);
	signal(SIGTERM, SAMPLE_FISHEYE_HandleSig);

	s32Index = atoi(argv[1]);
	switch (s32Index) {
	case 0:
		SAMPLE_FISHEYE_GDC_Fisheye_VoRotation();
		break;
	case 1:
		SAMPLE_FISHEYE_VI_Fisheye_VoRotation();
		break;
	}

	return ret;
}
