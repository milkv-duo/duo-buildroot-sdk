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

	/* 5 by 5*/
	CVI_S8 arr5by5[25] = { -1, -2, 0,  2,  1, -4, -8, 0,  8,  4, -6, -12, 0,
			       12, 6,  -4, -8, 0, 8,  4,  -1, -2, 0, 2,	 1 };
	/* 3 by 3*/
	CVI_S8 arr3by3[25] = { 0, 0, 0, 0,  0, 0, -1, 0, 1, 0, 0, -2, 0,
			       2, 0, 0, -1, 0, 1, 0,  0, 0, 0, 0, 0 };
	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, (char *)filename, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst_h, dst_v;

	CVI_IVE_CreateImage(handle, &dst_h, IVE_IMAGE_TYPE_S16C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &dst_v, IVE_IMAGE_TYPE_S16C1, input_w,
			    input_h);

	// Config Setting.
	IVE_SOBEL_CTRL_S iveSblCtrl;

	memcpy(iveSblCtrl.as8Mask, arr5by5, 5 * 5 * sizeof(CVI_S8));

	// Run HW IVE.
	printf("Run HW IVE Sobel 5x5 Both.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_BOTH;
	CVI_IVE_Sobel(handle, &src, &dst_h, &dst_v, &iveSblCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Sobel_Hor5x5.yuv", &dst_h);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Ver5x5.yuv", &dst_v);

	printf("Run HW IVE Sobel 5x5 Ver.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_VER;
	CVI_IVE_Sobel(handle, &src, CVI_NULL, &dst_v, &iveSblCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Ver5x5.yuv", &dst_v);

	printf("Run HW IVE Sobel 5x5 Hor.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_HOR;
	CVI_IVE_Sobel(handle, &src, &dst_h, CVI_NULL, &iveSblCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Hor5x5.yuv", &dst_h);

	memcpy(iveSblCtrl.as8Mask, arr3by3, 5 * 5 * sizeof(CVI_S8));

	printf("Run HW IVE Sobel 3x3 Both.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_BOTH;
	CVI_IVE_Sobel(handle, &src, &dst_h, &dst_v, &iveSblCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Hor3x3.yuv", &dst_h);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Ver3x3.yuv", &dst_v);

	printf("Run HW IVE Sobel 3x3 Ver.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_VER;
	CVI_IVE_Sobel(handle, &src, CVI_NULL, &dst_v, &iveSblCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Ver3x3.yuv", &dst_v);

	printf("Run HW IVE Sobel 3x3 Hor.\n");
	iveSblCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_HOR;
	CVI_IVE_Sobel(handle, &src, &dst_h, CVI_NULL, &iveSblCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sobel_Hor3x3.yuv", &dst_h);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst_h);
	CVI_SYS_FreeI(handle, &dst_v);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
