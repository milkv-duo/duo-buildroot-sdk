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
	IVE_DST_MEM_INFO_S dst_integ32, dst_integ64;

	CVI_IVE_CreateMemInfo(handle, &dst_integ32,
			      src.u32Stride[0] * src.u32Height *
				      sizeof(CVI_U32));
	CVI_IVE_CreateMemInfo(handle, &dst_integ64,
			      src.u32Stride[0] * src.u32Height *
				      sizeof(CVI_U64));

	// Config Setting.
	IVE_INTEG_CTRL_S pstIntegCtrl;

	// Run IVE
	printf("Run HW IVE Integral COMBINE.\n");
	pstIntegCtrl.enOutCtrl = IVE_INTEG_OUT_CTRL_COMBINE;
	ret |= CVI_IVE_Integ(handle, &src, &dst_integ64, &pstIntegCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteMem(handle, "sample_Integ_Combine.yuv", &dst_integ64);

	printf("Run HW IVE Integral SUM.\n");
	pstIntegCtrl.enOutCtrl = IVE_INTEG_OUT_CTRL_SUM;
	ret |= CVI_IVE_Integ(handle, &src, &dst_integ32, &pstIntegCtrl, 1);
	CVI_IVE_WriteMem(handle, "sample_Integ_Sum.yuv", &dst_integ32);

	printf("Run HW IVE Integral SQSUM.\n");
	pstIntegCtrl.enOutCtrl = IVE_INTEG_OUT_CTRL_SQSUM;
	ret |= CVI_IVE_Integ(handle, &src, &dst_integ64, &pstIntegCtrl, 1);
	CVI_IVE_WriteMem(handle, "sample_Integ_Sqsum.yuv", &dst_integ64);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeM(handle, &dst_integ32);
	CVI_SYS_FreeM(handle, &dst_integ64);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
