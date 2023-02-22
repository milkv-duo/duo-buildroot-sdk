#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 5) {
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

	CVI_BOOL pixelctrl = CVI_FALSE;
	CVI_U32 u32FrameNumMax = 32;
	CVI_U32 s32FrmCnt = 0;
	CVI_U32 u32FrmNum;

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_MEM_INFO_S stInput;
	IVE_SRC_IMAGE_S stIveImg, stFactor;

	CVI_IVE_ReadMem(handle, &stInput, filename,
			input_w * input_h * u32FrameNumMax);
	CVI_IVE_CreateImage(handle, &stIveImg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stFactor, IVE_IMAGE_TYPE_U16C1, input_w,
			    input_h);

	// Config Setting.
	IVE_GMM2_CTRL_S stGmm2Ctrl;

	stGmm2Ctrl.u16VarRate = 1;
	stGmm2Ctrl.u8ModelNum = atoi(argv[4]);
	printf("ModelNum = %d\n", stGmm2Ctrl.u8ModelNum);
	stGmm2Ctrl.u9q7MaxVar = (16 * 16) << 7;
	stGmm2Ctrl.u9q7MinVar = (8 * 8) << 7;
	stGmm2Ctrl.u8GlbSnsFactor = 8;
	stGmm2Ctrl.enSnsFactorMode = (pixelctrl) ?
					     IVE_GMM2_SNS_FACTOR_MODE_PIX :
					     IVE_GMM2_SNS_FACTOR_MODE_GLB;
	stGmm2Ctrl.u16FreqThr = 12000;
	stGmm2Ctrl.u16FreqInitVal = 20000;
	stGmm2Ctrl.u16FreqAddFactor = 0xEF;
	stGmm2Ctrl.u16FreqReduFactor = 0xFF00;
	stGmm2Ctrl.u16LifeThr = 5000;
	stGmm2Ctrl.enLifeUpdateFactorMode =
		IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLB;

	// Create dst image.
	IVE_MEM_INFO_S stModel;
	IVE_DST_IMAGE_S stIveFg, stIveBg, stMatchModelInfo;

	CVI_IVE_CreateImage(handle, &stIveFg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stIveBg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stMatchModelInfo, IVE_IMAGE_TYPE_U8C1,
			    input_w, input_h);
	CVI_IVE_CreateMemInfo(handle, &stModel,
			      stGmm2Ctrl.u8ModelNum * 8 * input_w * input_h);

	// Run IVE
	printf("Run HW IVE GMM2.\n");
	for (s32FrmCnt = 0; s32FrmCnt < u32FrameNumMax; s32FrmCnt++) {
		memcpy((void *)(uintptr_t)stIveImg.u64VirAddr[0],
		       (void *)(uintptr_t)(stInput.u64VirAddr +
				(s32FrmCnt * input_w * input_h)),
		       input_w * input_h);
		u32FrmNum = s32FrmCnt + 1;
		if (stGmm2Ctrl.u8ModelNum == 1) {
			//If the parameter u8ModelNum is set to 1, the parameter u16FreqReduFactor
			//is usually set to a small value at the first N frames. Here, N = 500.
			stGmm2Ctrl.u16FreqReduFactor =
				(u32FrmNum >= 500) ? 0xFFA0 : 0xFC00;
		} else {
			//If the parameter u8ModelNum is more than 1, the global
			// life mode should be used at the first N frames,
			//and the parameter u16GlbLifeUpdateFactor is usually
			// set to a big value. Here, N = 500.
			stGmm2Ctrl.u16GlbLifeUpdateFactor =
				(u32FrmNum >= 500) ? 4 : 0xFFFF / u32FrmNum;
		}
		if (pixelctrl && u32FrmNum > 16)
			stGmm2Ctrl.enLifeUpdateFactorMode =
				IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIX;

		CVI_IVE_GMM2(handle, &stIveImg, &stFactor, &stIveFg, &stIveBg,
			     &stMatchModelInfo, &stModel, &stGmm2Ctrl, 1);
	}
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu / u32FrameNumMax);

	if (pixelctrl) {
		CVI_IVE_WriteImg(handle, "sample_GMM2_U8C1_fg_31.yuv",
				 &stIveFg);
		CVI_IVE_WriteImg(handle, "sample_GMM2_U8C1_bg_31.yuv",
				 &stIveBg);
		CVI_IVE_WriteImg(handle,
				 "sample_GMM2_U8C1_PixelCtrl_match_31.yuv",
				 &stMatchModelInfo);
	} else {
		CVI_IVE_WriteImg(handle, "sample_GMM2_U8C1_fg_31.yuv",
				 &stIveFg);
		CVI_IVE_WriteImg(handle, "sample_GMM2_U8C1_bg_31.yuv",
				 &stIveBg);
	}

	CVI_SYS_FreeI(handle, &stIveImg);
	CVI_SYS_FreeI(handle, &stFactor);
	CVI_SYS_FreeI(handle, &stIveFg);
	CVI_SYS_FreeI(handle, &stIveBg);
	CVI_SYS_FreeI(handle, &stMatchModelInfo);
	CVI_SYS_FreeM(handle, &stInput);
	CVI_SYS_FreeM(handle, &stModel);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
