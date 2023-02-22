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

	// Create dst mem info.
	IVE_DST_MEM_INFO_S dst_hist;

	CVI_U32 dstHistSize = 256 * sizeof(CVI_U32);
	CVI_IVE_CreateMemInfo(handle, &dst_hist, dstHistSize);

	// Config Setting.

	// Run IVE
	printf("Run HW IVE Hist.\n");
	ret |= CVI_IVE_Hist(handle, &src, &dst_hist, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteMem(handle, "sample_Hist.bin", &dst_hist);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeM(handle, &dst_hist);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}