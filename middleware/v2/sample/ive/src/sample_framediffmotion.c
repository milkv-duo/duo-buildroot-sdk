#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name_1> <file_name_2>\n",
		       argv[0]);
		printf("Example: %s 480 480 data/md1_480x480.yuv data/md2_480x480.yuv\n",
		       argv[0]);
		return CVI_FAILURE;
	}
	// md1_480x480.yuv md2_480x480.yuv
	const char *filename1 = argv[3];
	const char *filename2 = argv[4];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Read image from file. CVI_IVE_ReadImage will do the flush for you.
	IVE_IMAGE_S src1, src2;

	CVI_IVE_ReadRawImage(handle, &src1, (char *)filename1, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);
	CVI_IVE_ReadRawImage(handle, &src2, (char *)filename2, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting (Sub -> threshold -> erode -> dilate)
	IVE_FRAME_DIFF_MOTION_CTRL_S iveMDCtrl;

	iveMDCtrl.enSubMode = IVE_SUB_MODE_ABS;
	iveMDCtrl.enThrMode = IVE_THRESH_MODE_BINARY;
	iveMDCtrl.u8ThrMinVal = 0;
	iveMDCtrl.u8ThrMaxVal = 255;
	iveMDCtrl.u8ThrLow = 30;
	CVI_U8 arr[] = { 0, 0,	 255, 0,   0,	0,   0, 255, 0,
			 0, 255, 255, 255, 255, 255, 0, 0,   255,
			 0, 0,	 0,   0,   255, 0,   0 };
	memcpy(iveMDCtrl.au8ErodeMask, arr, 25 * sizeof(CVI_U8));
	memcpy(iveMDCtrl.au8DilateMask, arr, 25 * sizeof(CVI_U8));

	// Run IVE
	printf("Run HW IVE FrameDiffMotion.\n");
	ret |= CVI_IVE_FrameDiffMotion(handle, &src1, &src2, &dst, &iveMDCtrl,
				       1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_FrameDiffMotion.yuv", &dst);

	CVI_SYS_FreeI(handle, &src1);
	CVI_SYS_FreeI(handle, &src2);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
