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

	//CVI_IVE_ReadRawImage(handle, &src, file_name, IVE_IMAGE_TYPE_U8C1, 352, 288);
	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst;

	CVI_IVE_CreateImage(handle, &dst, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Run HW IVE.
	printf("Run HW IVE OrdStaFilter Max.\n");
	IVE_ORD_STAT_FILTER_CTRL_S pstOrdStatFltCtrl;

	pstOrdStatFltCtrl.enMode = IVE_ORD_STAT_FILTER_MODE_MAX;
	CVI_IVE_OrdStatFilter(handle, &src, &dst, &pstOrdStatFltCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_OrdStaFilter_Max.yuv", &dst);

	printf("Run HW IVE OrdStaFilter Min.\n");
	pstOrdStatFltCtrl.enMode = IVE_ORD_STAT_FILTER_MODE_MIN;
	CVI_IVE_OrdStatFilter(handle, &src, &dst, &pstOrdStatFltCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_OrdStaFilter_Min.yuv", &dst);

	printf("Run HW IVE OrdStaFilter Median.\n");
	pstOrdStatFltCtrl.enMode = IVE_ORD_STAT_FILTER_MODE_MEDIAN;
	CVI_IVE_OrdStatFilter(handle, &src, &dst, &pstOrdStatFltCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_OrdStaFilter_Median.yuv", &dst);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
