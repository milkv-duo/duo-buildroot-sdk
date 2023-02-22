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
		printf("Example: %s 352 288 data/00_352x288_SP420.yuv\n", argv[0]);
		return CVI_FAILURE;
	}
	// 00_352x288_SP420.yuv
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
	IVE_IMAGE_S src_420;

	CVI_IVE_ReadRawImage(handle, &src_420, filename, IVE_IMAGE_TYPE_YUV420SP,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C3_PLANAR, input_w,
			    input_h);

	// Config Setting.
	IVE_FILTER_AND_CSC_CTRL_S stCtrl;

	// Run HW IVE.
	printf("Run HW IVE FilterAndCSC YUV2RGB.\n");
	stCtrl.enMode = IVE_CSC_MODE_VIDEO_BT601_YUV2RGB;
	stCtrl.u8Norm = 4;
	memcpy(stCtrl.as8Mask, arr3by3, sizeof(CVI_S8) * 25);
	CVI_IVE_FilterAndCSC(handle, &src_420, &dst, &stCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle,
			 "sample_FilterAndCSC_420SPToVideoPlanar3x3.yuv", &dst);

	printf("Run HW IVE FilterAndCSC YUV2RGB.\n");
	stCtrl.enMode = IVE_CSC_MODE_VIDEO_BT601_YUV2RGB;
	stCtrl.u8Norm = 7;
	memcpy(stCtrl.as8Mask, arr5by5, sizeof(CVI_S8) * 25);
	CVI_IVE_FilterAndCSC(handle, &src_420, &dst, &stCtrl, 1);
	CVI_IVE_WriteImg(handle,
			 "sample_FilterAndCSC_420SPToVideoPlanar5x5.yuv", &dst);

	CVI_SYS_FreeI(handle, &src_420);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
