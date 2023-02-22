#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

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

	CVI_U32 u32FrameNumMax = 32;
	CVI_U32 s32FrmCnt = 0;

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_CreateImage(handle, &src, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	IVE_DST_MEM_INFO_S stInput;

	CVI_IVE_ReadMem(handle, &stInput, filename,
			input_w * input_h * u32FrameNumMax);

	// Create dst image.
	IVE_DST_IMAGE_S dst_bg, dst_fg;

	CVI_IVE_CreateImage(handle, &dst_bg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &dst_fg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_GMM_CTRL_S stGMMCtrl;

	stGMMCtrl.u0q16BgRatio = 45875;
	stGMMCtrl.u0q16InitWeight = 3277;
	stGMMCtrl.u22q10NoiseVar = 225 * 1024;
	stGMMCtrl.u22q10MaxVar = 2000 * 1024;
	stGMMCtrl.u22q10MinVar = 200 * 1024;
	stGMMCtrl.u8q8VarThr = (CVI_U16)(256 * 6.25);
	stGMMCtrl.u8ModelNum = 3;

	IVE_MEM_INFO_S stModel;

	CVI_IVE_CreateMemInfo(handle, &stModel,
			      stGMMCtrl.u8ModelNum * 8 * input_w * input_h);

	// Run IVE
	printf("Run HW IVE GMM.\n");
	for (s32FrmCnt = 0; s32FrmCnt < u32FrameNumMax; s32FrmCnt++) {
		memcpy((void *)(uintptr_t)src.u64VirAddr[0],
		       (void *)(uintptr_t)(stInput.u64VirAddr +
				(s32FrmCnt * input_w * input_h)),
		       input_w * input_h);

		if (s32FrmCnt >= 500) {
			stGMMCtrl.u0q16LearnRate = 131; //0.02
		} else {
			stGMMCtrl.u0q16LearnRate = 65535 / (s32FrmCnt + 1);
		}
		CVI_IVE_GMM(handle, &src, &dst_fg, &dst_bg, &stModel,
			    &stGMMCtrl, 1);
	}
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu / u32FrameNumMax);

	CVI_IVE_WriteImg(handle, "sample_GMM_U8C1_fg_31.yuv", &dst_fg);
	CVI_IVE_WriteImg(handle, "sample_GMM_U8C1_bg_31.yuv", &dst_bg);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst_bg);
	CVI_SYS_FreeI(handle, &dst_fg);
	CVI_SYS_FreeM(handle, &stInput);
	CVI_SYS_FreeM(handle, &stModel);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
