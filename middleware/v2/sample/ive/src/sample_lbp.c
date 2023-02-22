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
	IVE_LBP_CTRL_S ctrl;

	ctrl.enMode = IVE_LBP_CMP_MODE_NORMAL;
	ctrl.un8BitThr.s8Val = (IVE_LBP_CMP_MODE_ABS == ctrl.enMode ? 35 : 41);

	// Run HW IVE
	printf("Run HW IVE LBP Normal.\n");
	CVI_IVE_LBP(handle, &src, &dst, &ctrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_LBP_Normal.yuv", &dst);

	ctrl.enMode = IVE_LBP_CMP_MODE_ABS;
	ctrl.un8BitThr.s8Val = (IVE_LBP_CMP_MODE_ABS == ctrl.enMode ? 35 : 41);
	printf("Run HW IVE LBP ABS.\n");
	CVI_IVE_LBP(handle, &src, &dst, &ctrl, 1);
	CVI_IVE_WriteImg(handle, "sample_LBP_Abs.yuv", &dst);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
