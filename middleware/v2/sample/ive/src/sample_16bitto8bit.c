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
		printf("Example: %s 352 288 data/00_704x576.s16.yuv\n", argv[0]);
		return CVI_FAILURE;
	}
	// 00_704x576.s16
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
	IVE_IMAGE_S src_s16;

	CVI_IVE_ReadRawImage(handle, &src_s16, filename, IVE_IMAGE_TYPE_S16C1,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst_u8, dst_s8;

	CVI_IVE_CreateImage(handle, &dst_u8, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &dst_s8, IVE_IMAGE_TYPE_S8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_16BIT_TO_8BIT_CTRL_S ctrl;

	// Run HW IVE.
	printf("Run HW IVE S16 to S8.\n");
	ctrl.enMode = IVE_16BIT_TO_8BIT_MODE_S16_TO_S8;
	ctrl.u8Numerator = 41; //255
	ctrl.u16Denominator = 18508; //256
	ctrl.s8Bias = 0;
	CVI_IVE_16BitTo8Bit(handle, &src_s16, &dst_s8, &ctrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_16BitTo8Bit_S16ToS8.yuv", &dst_s8);

	printf("Run HW IVE S16 to U8 ABS.\n");
	ctrl.enMode = IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS;
	ctrl.u8Numerator = 190; //255
	ctrl.u16Denominator = 26690; //256
	ctrl.s8Bias = 0;
	CVI_IVE_16BitTo8Bit(handle, &src_s16, &dst_u8, &ctrl, 1);
	CVI_IVE_WriteImg(handle, "sample_16BitTo8Bit_Abs.yuv", &dst_u8);

	printf("Run HW IVE S16 to U8 BIAS.\n");
	ctrl.enMode = IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS;
	ctrl.u8Numerator = 225; //255
	ctrl.u16Denominator = 15949; //256
	ctrl.s8Bias = -42;
	CVI_IVE_16BitTo8Bit(handle, &src_s16, &dst_u8, &ctrl, 1);
	CVI_IVE_WriteImg(handle, "sample_16BitTo8Bit_Shift.yuv", &dst_u8);

	CVI_SYS_FreeI(handle, &src_s16);
	CVI_SYS_FreeI(handle, &dst_u8);
	CVI_SYS_FreeI(handle, &dst_s8);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
