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
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_THRESH_CTRL_S iveThreshCtrl;

	iveThreshCtrl.enMode = IVE_THRESH_MODE_BINARY;
	iveThreshCtrl.u8LowThr = 41;
	iveThreshCtrl.u8HighThr = 105;
	iveThreshCtrl.u8MinVal = 190;
	iveThreshCtrl.u8MidVal = 132;
	iveThreshCtrl.u8MaxVal = 225;

	// Run HW IVE.
	printf("Run HW IVE Threashold BINARY.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Thresh_Binary.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_TRUNC;
	iveThreshCtrl.u8LowThr = 169;
	iveThreshCtrl.u8HighThr = 210;
	iveThreshCtrl.u8MinVal = 174;
	iveThreshCtrl.u8MidVal = 82;
	iveThreshCtrl.u8MaxVal = 144;

	// Run HW IVE.
	printf("Run HW IVE Threashold TRUNC.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_Trunc.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_TO_MINVAL;
	iveThreshCtrl.u8LowThr = 95;
	iveThreshCtrl.u8HighThr = 241;
	iveThreshCtrl.u8MinVal = 241;
	iveThreshCtrl.u8MidVal = 187;
	iveThreshCtrl.u8MaxVal = 233;

	// Run HW IVE.
	printf("Run HW IVE Threashold TO_MINVAL.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_ToMinVal.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_MIN_MID_MAX;
	iveThreshCtrl.u8LowThr = 236;
	iveThreshCtrl.u8HighThr = 249;
	iveThreshCtrl.u8MinVal = 166;
	iveThreshCtrl.u8MidVal = 219;
	iveThreshCtrl.u8MaxVal = 60;

	// Run HW IVE.
	printf("Run HW IVE Threashold MIN_MID_MAX.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_MinMidMax.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_ORI_MID_MAX;
	iveThreshCtrl.u8LowThr = 6;
	iveThreshCtrl.u8HighThr = 169;
	iveThreshCtrl.u8MinVal = 62;
	iveThreshCtrl.u8MidVal = 153;
	iveThreshCtrl.u8MaxVal = 36;

	// Run HW IVE.
	printf("Run HW IVE Threashold ORI_MID_MAX.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_OriMidMax.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_MIN_MID_ORI;
	iveThreshCtrl.u8LowThr = 142;
	iveThreshCtrl.u8HighThr = 162;
	iveThreshCtrl.u8MinVal = 28;
	iveThreshCtrl.u8MidVal = 6;
	iveThreshCtrl.u8MaxVal = 183;

	// Run HW IVE.
	printf("Run HW IVE Threashold MIN_MID_ORI.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_MinMidOri.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_MIN_ORI_MAX;
	iveThreshCtrl.u8LowThr = 92;
	iveThreshCtrl.u8HighThr = 140;
	iveThreshCtrl.u8MinVal = 179;
	iveThreshCtrl.u8MidVal = 18;
	iveThreshCtrl.u8MaxVal = 77;

	// Run HW IVE.
	printf("Run HW IVE Threashold MIN_ORI_MAX.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_MinOriMax.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_MODE_ORI_MID_ORI;
	iveThreshCtrl.u8LowThr = 22;
	iveThreshCtrl.u8HighThr = 60;
	iveThreshCtrl.u8MinVal = 187;
	iveThreshCtrl.u8MidVal = 139;
	iveThreshCtrl.u8MaxVal = 166;

	// Run HW IVE.
	printf("Run HW IVE Threashold ORI_MID_ORI.\n");
	CVI_IVE_Thresh(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_OriMidOri.yuv", &dst);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
