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
	// 00_352x288_y.yuv  00_352x288_SP420.yuv  00_352x288_SP422.yuv
	const char *filename = argv[3];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);

	CVI_S8 arr5by5[25] = {
		1, 2, 3, 2, 1, 2, 5, 6, 5, 2, 3, 6, 8,
		6, 3, 2, 5, 6, 5, 2, 1, 2, 3, 2, 1,
	};

	CVI_S8 arr3by3[25] = {
		0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 2, 4,
		2, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0,
	};

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst_y;

	CVI_IVE_CreateImage(handle, &dst_y, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_FILTER_CTRL_S iveFltCtrl;

	memcpy(iveFltCtrl.as8Mask, arr3by3, 5 * 5 * sizeof(CVI_S8));
	iveFltCtrl.u8Norm = 4;

	// Run HW IVE.
	printf("Run HW IVE Filter 3x3 U8C1.\n");
	ret |= CVI_IVE_Filter(handle, &src, &dst_y, &iveFltCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Filter_Y3x3.yuv", &dst_y);

	memcpy(iveFltCtrl.as8Mask, arr5by5, 5 * 5 * sizeof(CVI_S8));
	iveFltCtrl.u8Norm = 7;

	printf("Run HW IVE Filter 5x5 U8C1.\n");
	ret |= CVI_IVE_Filter(handle, &src, &dst_y, &iveFltCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Filter_Y5x5.yuv", &dst_y);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst_y);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
