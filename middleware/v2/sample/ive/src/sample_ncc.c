#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name1> <file_name2>\n",
		       argv[0]);
		printf("Example: %s 352 288 data/00_352x288_y.yuv data/01_352x288_y.yuv\n",
		       argv[0]);
		return CVI_FAILURE;
	}
	// 00_352x288_y.yuv 01_352x288_y.yuv
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

	// Create src image.
	IVE_IMAGE_S src, src1;

	CVI_IVE_ReadRawImage(handle, &src, filename1, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);
	CVI_IVE_ReadRawImage(handle, &src1, filename2, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_MEM_INFO_S dstNCC;

	CVI_IVE_CreateMemInfo(handle, &dstNCC, sizeof(IVE_NCC_DST_MEM_S));

	// Run HW IVE.
	printf("Run HW IVE NCC.\n");
	CVI_IVE_NCC(handle, &src, &src1, &dstNCC, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteMem(handle, "sample_NCC_Mem.bin", &dstNCC);

	CVI_U64 *numerator = (CVI_U64 *)(uintptr_t)dstNCC.u64VirAddr;
	CVI_U64 *quadSum1 = (CVI_U64 *)(uintptr_t)(dstNCC.u64VirAddr + sizeof(CVI_U64));
	CVI_U64 *quadSum2 = quadSum1 + 1;
	CVI_FLOAT fR = (CVI_FLOAT)(
		(CVI_DOUBLE)*numerator /
		(sqrt((CVI_DOUBLE)*quadSum1) * sqrt((CVI_DOUBLE)*quadSum2)));

	printf("NCC value is %f.\n", fR);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &src1);
	CVI_SYS_FreeM(handle, &dstNCC);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
