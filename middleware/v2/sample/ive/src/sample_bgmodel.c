#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct _IVE__BGMDL1_PIX_S {
	CVI_U16 u16ShortKeepTime; /*Candidate background short hold time*/
	CVI_U8 u8ChgCond; /*Time condition for candidate background into the changing state*/
	CVI_U8 u8PotenBgLife; /*Potential background cumulative access time */
	CVI_U8 u8WorkBgLife[3]; /*1# ~ 3# background vitality */
	CVI_U8 u8CandiBgLife; /*Candidate background vitality */
} IVE_BGMDL1_PIX_S;

CVI_S32 InitMatchBgModel(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstCurImg,
			 IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag,
			 IVE_DST_IMAGE_S *pstDiffFg,
			 IVE_DST_MEM_INFO_S *pstStatData,
			 IVE_MATCH_BG_MODEL_CTRL_S *pstMatchBgModelCtrl,
			 CVI_U32 u32Width, CVI_U32 u32Height)
{
	//malloc pstCurImg
	CVI_IVE_CreateImage(pIveHandle, pstCurImg, IVE_IMAGE_TYPE_U8C1,
			    u32Width, u32Height);

	//malloc pstBgModel
	//CVI_IVE_ReadDataArray(pIveHandle, pstBgModel, (char*) data_campus_stBgModel,
	// u32Width * sizeof(IVE_BG_MODEL_PIX_S), u32Height);
	CVI_IVE_CreateDataInfo(pIveHandle, pstBgModel,
			       u32Width * sizeof(IVE_BG_MODEL_PIX_S),
			       u32Height);

	//malloc pstFgFlag
	//CVI_IVE_ReadImageArray(pIveHandle, pstFgFlag, (char*) data_campus_stFgFlag,
	//IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
	CVI_IVE_CreateImage(pIveHandle, pstFgFlag, IVE_IMAGE_TYPE_U8C1,
			    u32Width, u32Height);

	//malloc pstMatchFg
	CVI_IVE_CreateImage(pIveHandle, pstDiffFg, IVE_IMAGE_TYPE_S16C1,
			    u32Width, u32Height);

	//malloc pstStatData
	CVI_IVE_CreateMemInfo(pIveHandle, pstStatData,
			      sizeof(IVE_BG_STAT_DATA_S));

	//init ctrl info
	pstMatchBgModelCtrl->u32CurFrmNum = 0;
	pstMatchBgModelCtrl->u32PreFrmNum = 0;
	pstMatchBgModelCtrl->u16TimeThr = 20;
	pstMatchBgModelCtrl->u8DiffThrCrlCoef = 0;
	pstMatchBgModelCtrl->u8DiffMaxThr = 10;
	pstMatchBgModelCtrl->u8DiffMinThr = 10;
	pstMatchBgModelCtrl->u8DiffThrInc = 0;
	pstMatchBgModelCtrl->u8FastLearnRate = 4;
	pstMatchBgModelCtrl->u8DetChgRegion = 1;

	return CVI_SUCCESS;
}

CVI_S32 InitUpdateBgModel(IVE_HANDLE pIveHandle, IVE_DST_IMAGE_S *pstBgImg,
			  IVE_DST_IMAGE_S *pstChgSta,
			  IVE_UPDATE_BG_MODEL_CTRL_S *pstUpdateBgModelCtrl,
			  CVI_U16 u32Width, CVI_U16 u32Height)
{
	//malloc pstBgImg
	CVI_IVE_CreateImage(pIveHandle, pstBgImg, IVE_IMAGE_TYPE_U8C1, u32Width,
			    u32Height);

	//malloc pstChgSta
	CVI_IVE_CreateImage(pIveHandle, pstChgSta, IVE_IMAGE_TYPE_U32C1,
			    u32Width, u32Height);

	//init ctrl info
	pstUpdateBgModelCtrl->u32CurFrmNum = 0;
	pstUpdateBgModelCtrl->u32PreChkTime = 0;
	pstUpdateBgModelCtrl->u32FrmChkPeriod = 30;
	pstUpdateBgModelCtrl->u32InitMinTime = 25;
	pstUpdateBgModelCtrl->u32StyBgMinBlendTime = 100;
	pstUpdateBgModelCtrl->u32StyBgMaxBlendTime = 1500;
	pstUpdateBgModelCtrl->u32DynBgMinBlendTime = 0;
	pstUpdateBgModelCtrl->u32StaticDetMinTime = 80;
	pstUpdateBgModelCtrl->u16FgMaxFadeTime = 15;
	pstUpdateBgModelCtrl->u16BgMaxFadeTime = 60;
	pstUpdateBgModelCtrl->u8StyBgAccTimeRateThr = 80;
	pstUpdateBgModelCtrl->u8ChgBgAccTimeRateThr = 60;
	pstUpdateBgModelCtrl->u8DynBgAccTimeThr = 0;
	pstUpdateBgModelCtrl->u8DynBgDepth = 3;
	pstUpdateBgModelCtrl->u8BgEffStaRateThr = 90;
	pstUpdateBgModelCtrl->u8AcceBgLearn = 0;
	pstUpdateBgModelCtrl->u8DetChgRegion = 1;

	return CVI_SUCCESS;
}

CVI_VOID UninitMatchBgModel(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstCurImg,
			    IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag,
			    IVE_DST_IMAGE_S *pstMatchFg,
			    IVE_DST_MEM_INFO_S *pstStatData)
{
	CVI_SYS_FreeI(pIveHandle, pstCurImg);
	CVI_SYS_FreeD(pIveHandle, pstBgModel);
	CVI_SYS_FreeI(pIveHandle, pstFgFlag);
	CVI_SYS_FreeI(pIveHandle, pstMatchFg);
	CVI_SYS_FreeM(pIveHandle, pstStatData);
}

CVI_VOID UninitUpdateBgModel(IVE_HANDLE pIveHandle, IVE_DST_IMAGE_S *pstBgImg,
			     IVE_DST_IMAGE_S *pstChgSta)
{
	CVI_SYS_FreeI(pIveHandle, pstBgImg);
	CVI_SYS_FreeI(pIveHandle, pstChgSta);
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name>\n",
		       argv[0]);
		printf("Example: %s 352 288 data/campus.u8c1.1_100.raw\n", argv[0]);
		return CVI_FAILURE;
	}
	// campus.u8c1.1_100.raw
	const char *filename = argv[3];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);

	CVI_BOOL bInstant = CVI_TRUE;

	CVI_U32 u32FrameNum;
	CVI_S32 u32FrameNumMax = 100;
	CVI_S32 s32FrmCnt = 0;
	CVI_U32 u32UpdCnt = 5;
	CVI_U32 u32PreUpdTime = 0;
	CVI_U32 u32PreChkTime = 0;
	CVI_U32 u32FrmUpdPeriod = 10;
	CVI_U32 u32FrmChkPeriod = 30;

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_SRC_IMAGE_S src;
	IVE_DATA_S stBgModel;
	IVE_IMAGE_S stFgFlag;
	IVE_DST_IMAGE_S stDiffFg;
	IVE_DST_MEM_INFO_S stStatData;
	IVE_MATCH_BG_MODEL_CTRL_S stMatchBgModelCtrl;

	InitMatchBgModel(handle, &src, &stBgModel, &stFgFlag, &stDiffFg,
			 &stStatData, &stMatchBgModelCtrl, input_w, input_h);

	IVE_MEM_INFO_S stInput;

	CVI_IVE_ReadMem(handle, &stInput, filename,
			input_w * input_h * u32FrameNumMax);

	// Create dst image.
	IVE_DST_IMAGE_S stBgImg;
	IVE_DST_IMAGE_S stChgSta;
	IVE_UPDATE_BG_MODEL_CTRL_S stUpdateBgModelCtrl;

	InitUpdateBgModel(handle, &stBgImg, &stChgSta, &stUpdateBgModelCtrl,
			  input_w, input_h);

	// Config Setting.
	stMatchBgModelCtrl.u32CurFrmNum = s32FrmCnt;

	// Run IVE
	printf("Run HW IVE BgModel.\n");
	for (s32FrmCnt = 0; s32FrmCnt < u32FrameNumMax; s32FrmCnt++) {
		u32FrameNum = s32FrmCnt + 1;
		memcpy((void *)(uintptr_t)src.u64VirAddr[0],
		       (void *)(uintptr_t)(stInput.u64VirAddr +
				(s32FrmCnt * input_w * input_h)),
		       input_w * input_h);

		IVE_BG_STAT_DATA_S *stat =
			(IVE_BG_STAT_DATA_S *)(uintptr_t)stStatData.u64VirAddr;
		stMatchBgModelCtrl.u32PreFrmNum =
			stMatchBgModelCtrl.u32CurFrmNum;
		stMatchBgModelCtrl.u32CurFrmNum = u32FrameNum;

		CVI_IVE_MatchBgModel(handle, &src, &stBgModel, &stFgFlag,
				     &stDiffFg, &stStatData,
				     &stMatchBgModelCtrl, bInstant);
		printf("CVI_IVE_MatchBgModel u32UpdCnt %d, u32FrameNum %d, ",
		       u32UpdCnt, u32FrameNum);
		printf("frm %d, stat u32PixNum = %d, u32SumLum = %d\n",
		       s32FrmCnt, stat->u32PixNum, stat->u32SumLum);
		if ((u32UpdCnt == 0 ||
		     u32FrameNum >= u32PreUpdTime + u32FrmUpdPeriod)) {
			u32UpdCnt++;
			u32PreUpdTime = u32FrameNum;
			stUpdateBgModelCtrl.u32CurFrmNum = u32FrameNum;
			stUpdateBgModelCtrl.u32PreChkTime = u32PreChkTime;
			stUpdateBgModelCtrl.u32FrmChkPeriod = 0;
			if (u32FrameNum >= u32PreChkTime + u32FrmChkPeriod) {
				stUpdateBgModelCtrl.u32FrmChkPeriod =
					u32FrmChkPeriod;
				u32PreChkTime = u32FrameNum;
			}

			CVI_IVE_UpdateBgModel(handle, &stBgModel, &stFgFlag,
					      &stBgImg, &stChgSta, &stStatData,
					      &stUpdateBgModelCtrl, bInstant);
			printf("CVI_IVE_UpdateBgModel frm %d, stat u32PixNum = %d, u32SumLum = %d\n",
			       s32FrmCnt, stat->u32PixNum, stat->u32SumLum);
		}
	}
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu / 110);

	CVI_IVE_WriteData(handle, "sample_BgModelSample2_BgMdl.bin",
			  &stBgModel);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_DiffFg.bin", &stDiffFg);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_ChgSta.bin", &stChgSta);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_FgFlag.bin", &stFgFlag);

	IVE_DST_IMAGE_S stBgDiffFg, stFrmDiffFg;

	CVI_IVE_CreateImage(handle, &stBgDiffFg, IVE_IMAGE_TYPE_S8C1,
			    stDiffFg.u32Width, stDiffFg.u32Height);
	CVI_IVE_CreateImage(handle, &stFrmDiffFg, IVE_IMAGE_TYPE_S8C1,
			    stDiffFg.u32Width, stDiffFg.u32Height);
	CVI_IVE_DiffFg_Split(handle, &stDiffFg, &stBgDiffFg, &stFrmDiffFg);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_BgDiffFg.yuv",
			 &stBgDiffFg);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_FrmDiffFg.yuv",
			 &stFrmDiffFg);

	IVE_DST_IMAGE_S stChgStaImg, stChgStaFg, stChStaLift;

	CVI_IVE_CreateImage(handle, &stChgStaImg, IVE_IMAGE_TYPE_U8C1,
			    stChgSta.u32Width, stChgSta.u32Height);
	CVI_IVE_CreateImage(handle, &stChgStaFg, IVE_IMAGE_TYPE_U8C1,
			    stChgSta.u32Width, stChgSta.u32Height);
	CVI_IVE_CreateImage(handle, &stChStaLift, IVE_IMAGE_TYPE_U16C1,
			    stChgSta.u32Width, stChgSta.u32Height);
	CVI_IVE_ChgSta_Split(handle, &stChgSta, &stChgStaImg, &stChgStaFg,
			     &stChStaLift);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_ChgStaImg.yuv",
			 &stChgStaImg);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_ChgStaFg.yuv",
			 &stChgStaFg);
	CVI_IVE_WriteImg(handle, "sample_BgModelSample2_ChStaLift.yuv",
			 &stChStaLift);

	UninitMatchBgModel(handle, &src, &stBgModel, &stFgFlag, &stDiffFg,
			   &stStatData);
	UninitUpdateBgModel(handle, &stBgImg, &stChgSta);
	CVI_SYS_FreeM(handle, &stInput);
	CVI_SYS_FreeI(handle, &stBgDiffFg);
	CVI_SYS_FreeI(handle, &stFrmDiffFg);
	CVI_SYS_FreeI(handle, &stChgStaImg);
	CVI_SYS_FreeI(handle, &stChgStaFg);
	CVI_SYS_FreeI(handle, &stChStaLift);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
