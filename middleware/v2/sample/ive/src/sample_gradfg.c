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
		printf("Example: %s 352 288 data/00_352x288_y.yuv\n", argv[0]);
		return CVI_FAILURE;
	}
	// 00_352x288_y.yuv
	const char *filename = argv[3];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);
	/* 3 by 3*/
	CVI_S8 arr3by3[25] = {
		0, 0, 0, 0,  0, 0, -1, 0, 1, 0, 0, -2, 0,
		2, 0, 0, -1, 0, 1, 0,  0, 0, 0, 0, 0,
	};

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S stCurGrad, stBgGrad, stGradFg;

	CVI_IVE_CreateImage(handle, &stCurGrad, IVE_IMAGE_TYPE_U16C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stBgGrad, IVE_IMAGE_TYPE_U16C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stGradFg, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_GRAD_FG_CTRL_S stCtrl;

	stCtrl.enMode = IVE_GRAD_FG_MODE_FIND_MIN_GRAD;
	stCtrl.u16EdwFactor = 1000;
	stCtrl.u8CrlCoefThr = 80;
	stCtrl.u8MagCrlThr = 4;
	stCtrl.u8MinMagDiff = 2;
	stCtrl.u8NoiseVal = 1;
	stCtrl.u8EdwDark = 1;
	IVE_NORM_GRAD_OUT_CTRL_E enOutCtrl = IVE_NORM_GRAD_OUT_CTRL_COMBINE;
	IVE_NORM_GRAD_CTRL_S stNormGradCtrl;

	stNormGradCtrl.enOutCtrl = enOutCtrl;
	memcpy(stNormGradCtrl.as8Mask, arr3by3, sizeof(CVI_S8) * 25);
	stNormGradCtrl.u8Norm = 8;

	// Create Fake Data.
	ret |= CVI_IVE_NormGrad(handle, &src, NULL, NULL, &stCurGrad,
				&stNormGradCtrl, 1);
	stNormGradCtrl.u8Norm = 2;
	ret |= CVI_IVE_NormGrad(handle, &src, NULL, NULL, &stBgGrad,
				&stNormGradCtrl, 1);

	// Run HW IVE.
	printf("Run HW IVE GradFg FIND_MIN_GRAD.\n");
	ret |= CVI_IVE_GradFg(handle, &src, &stCurGrad, &stBgGrad, &stGradFg,
			      &stCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_GradFg_FIND_MIN_GRAD.out", &stGradFg);

	printf("Run HW IVE GradFg USE_CUR_GRAD.\n");
	stCtrl.enMode = IVE_GRAD_FG_MODE_USE_CUR_GRAD;
	ret |= CVI_IVE_GradFg(handle, &src, &stCurGrad, &stBgGrad, &stGradFg,
			      &stCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_GradFg_USE_CUR_GRAD.out", &stGradFg);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &stCurGrad);
	CVI_SYS_FreeI(handle, &stBgGrad);
	CVI_SYS_FreeI(handle, &stGradFg);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
