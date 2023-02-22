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
	IVE_IMAGE_S src1, src2;

	CVI_IVE_ReadRawImage(handle, &src1, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create src2 image.
	CVI_IVE_CreateImage(handle, &src2, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	memset((void *)(uintptr_t)src2.u64VirAddr[0], 255,
	       src1.u32Stride[0] * input_h);
	for (int j = input_h / 10; j < input_h * 9 / 10; j++) {
		for (int i = input_w / 10; i < input_w * 9 / 10; i++) {
			((char *)(uintptr_t)src2
				 .u64VirAddr[0])[i + j * src1.u32Stride[0]] = 0;
		}
	}

	// Create dst image.
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.

	// Run IVE
	printf("Run HW IVE Xor.\n");
	ret = CVI_IVE_Xor(handle, &src1, &src2, &dst, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Xor.yuv", &dst);

	CVI_SYS_FreeI(handle, &src1);
	CVI_SYS_FreeI(handle, &src2);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
