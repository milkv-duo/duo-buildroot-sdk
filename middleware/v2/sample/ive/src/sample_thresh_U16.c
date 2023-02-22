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
		printf("Example: %s 352 288 data/00_704x576.u16\n", argv[0]);
		return CVI_FAILURE;
	}
	// 00_704x576.u16
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

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U16C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_THRESH_U16_CTRL_S iveThreshCtrl;

	iveThreshCtrl.enMode = IVE_THRESH_U16_MODE_U16_TO_U8_MIN_MID_MAX;
	iveThreshCtrl.u16LowThr = 41;
	iveThreshCtrl.u16HighThr = 105;
	iveThreshCtrl.u8MinVal = 190;
	iveThreshCtrl.u8MidVal = 132;
	iveThreshCtrl.u8MaxVal = 225;

	// Run HW IVE.
	printf("Run HW IVE Threashold U16 U8_MinMidMax.\n");
	CVI_IVE_Thresh_U16(handle, &src, &dst, &iveThreshCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Thresh_U16_To_U8_MinMidMax.yuv", &dst);

	// Config Setting.
	iveThreshCtrl.enMode = IVE_THRESH_U16_MODE_U16_TO_U8_MIN_ORI_MAX;
	iveThreshCtrl.u16LowThr = 108;
	iveThreshCtrl.u16HighThr = 121;
	iveThreshCtrl.u8MinVal = 174;
	iveThreshCtrl.u8MidVal = 82;
	iveThreshCtrl.u8MaxVal = 144;

	// Run HW IVE.
	printf("Run HW IVE Threashold U16 U8_MinOriMax.\n");
	CVI_IVE_Thresh_U16(handle, &src, &dst, &iveThreshCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Thresh_U16_To_U8_MinOriMax.yuv", &dst);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
