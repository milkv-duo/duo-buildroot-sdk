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

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst_bernsen;

	CVI_IVE_CreateImage(handle, &dst_bernsen, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_BERNSEN_CTRL_S iveBernsenCtrl;

	iveBernsenCtrl.u8Thr = 128;
	iveBernsenCtrl.u8ContrastThreshold = 15;
	iveBernsenCtrl.u8WinSize = 5; /* 3x3 or 5x5 */

	// Run HW IVE
	printf("Run HW IVE Bernsen 5x5 NORMAL.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_NORMAL;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Bernsen_5x5.yuv", &dst_bernsen);

	printf("Run HW IVE Bernsen 5x5 THRESH.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_THRESH;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Bernsen_5x5_Thresh.yuv", &dst_bernsen);

	printf("Run HW IVE Bernsen 5x5 PAPER.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_PAPER;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Bernsen_5x5_Paper.yuv", &dst_bernsen);

	iveBernsenCtrl.u8WinSize = 3; /* 3x3 or 5x5 */

	printf("Run HW IVE Bernsen 3x3 NORMAL.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_NORMAL;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Bernsen_3x3.yuv", &dst_bernsen);

	printf("Run HW IVE Bernsen 3x3 THRESH.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_THRESH;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Bernsen_3x3_Thresh.yuv", &dst_bernsen);

	printf("Run HW IVE Bernsen 3x3 PAPER.\n");
	iveBernsenCtrl.enMode = IVE_BERNSEN_MODE_PAPER;
	ret |= CVI_IVE_Bernsen(handle, &src, &dst_bernsen, &iveBernsenCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Bernsen_3x3_Paper.yuv", &dst_bernsen);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst_bernsen);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
