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
	IVE_DATA_S src_data;

	src_data.u64PhyAddr = src.u64PhyAddr[0];
	src_data.u64VirAddr = src.u64VirAddr[0];
	src_data.u32Stride = src.u32Stride[0];
	src_data.u32Width = src.u32Width;
	src_data.u32Height = src.u32Height;

	// Create dst image.
	IVE_DST_DATA_S dst_data;

	CVI_IVE_CreateDataInfo(handle, &dst_data, input_w, input_h);

	// Config Setting.
	IVE_DMA_CTRL_S iveDmaCtrl;

	iveDmaCtrl.enMode = IVE_DMA_MODE_DIRECT_COPY;

	// Run HW IVE.
	printf("Run HW IVE DMA Direct Copy.\n");
	ret |= CVI_IVE_DMA(handle, &src_data, &dst_data, &iveDmaCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteData(handle, "sample_DMA_Direct.bin", &dst_data);

	IVE_DST_DATA_S dst_data1;

	printf("Run HW IVE DMA Interval Copy.\n");
	iveDmaCtrl.enMode = IVE_DMA_MODE_INTERVAL_COPY;
	iveDmaCtrl.u8ElemSize = 1;
	iveDmaCtrl.u8HorSegSize = 4;
	iveDmaCtrl.u8VerSegRows = 1;
	int IntervalWidth = (src_data.u32Width / iveDmaCtrl.u8HorSegSize) *
			    iveDmaCtrl.u8ElemSize;
	int IntervalHeight = src_data.u32Height / iveDmaCtrl.u8VerSegRows;

	CVI_IVE_CreateDataInfo(handle, &dst_data1, IntervalWidth,
			       IntervalHeight);
	ret |= CVI_IVE_DMA(handle, &src_data, &dst_data1, &iveDmaCtrl, 1);
	CVI_IVE_WriteData(handle, "sample_DMA_Interval.bin", &dst_data1);

	printf("Run HW IVE DMA 3BYTE Copy.\n");
	iveDmaCtrl.enMode = IVE_DMA_MODE_SET_3BYTE;
	iveDmaCtrl.u64Val = 0x123456789abcdef;
	iveDmaCtrl.u8HorSegSize = 1;
	ret |= CVI_IVE_DMA(handle, &src_data, &dst_data, &iveDmaCtrl, 1);
	CVI_IVE_WriteData(handle, "sample_DMA_Set3Byte.bin", &dst_data);

	printf("Run HW IVE DMA 8BYTE Copy. ken new\n");
	iveDmaCtrl.enMode = IVE_DMA_MODE_SET_8BYTE;
	iveDmaCtrl.u64Val = 0x123456789abcdef;
	ret |= CVI_IVE_DMA(handle, &src_data, &dst_data, &iveDmaCtrl, 1);
	CVI_IVE_WriteData(handle, "sample_DMA_Set8Byte.bin", &dst_data);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeD(handle, &dst_data);
	CVI_SYS_FreeD(handle, &dst_data1);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
