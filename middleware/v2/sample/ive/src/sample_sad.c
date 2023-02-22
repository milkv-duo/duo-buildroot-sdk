#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name1> <file_name2>\n",
		       argv[0]);
		printf("Example: %s 352 288 data/00_352x288_y.yuv data/bin_352x288_y.yuv\n",
		       argv[0]);
		return CVI_FAILURE;
	}
	// 00_352x288_y.yuv bin_352x288_y.yuv
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
	IVE_IMAGE_S src1, src2;

	CVI_IVE_ReadRawImage(handle, &src1, (char *)filename1, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);
	CVI_IVE_ReadRawImage(handle, &src2, (char *)filename2, IVE_IMAGE_TYPE_U8C1,
			  input_w, input_h);

	// Create dst image.
	IVE_DST_IMAGE_S stSad_8, stSad_16, stDst2, stThr;

	CVI_IVE_CreateImage(handle, &stSad_8, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stSad_16, IVE_IMAGE_TYPE_U16C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stDst2, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);
	CVI_IVE_CreateImage(handle, &stThr, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_SAD_CTRL_S iveSadCtrl;

	iveSadCtrl.u16Thr =
		0x800; // Treshold value for thresholding an U16 image into an U8 image.
	iveSadCtrl.u8MinVal = 2;
	iveSadCtrl.u8MaxVal = 30;
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_4X4;

	// Run HW IVE.
	printf("Run HW IVE SAD 16BIT MB_4X4.\n");
	iveSadCtrl.enOutCtrl = IVE_SAD_OUT_CTRL_16BIT_BOTH;
	CVI_IVE_SAD(handle, &src1, &src2, &stSad_16, &stThr, &iveSadCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode0_out0.bin", &stSad_16);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode0_out0.bin", &stThr);

	printf("Run HW IVE SAD 16BIT MB_8X8.\n");
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_8X8;
	CVI_IVE_SAD(handle, &src1, &src2, &stSad_16, &stThr, &iveSadCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode1_out0.bin", &stSad_16);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode1_out0.bin", &stThr);

	printf("Run HW IVE SAD 16BIT MB_16X16.\n");
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_16X16;
	CVI_IVE_SAD(handle, &src1, &src2, &stSad_16, &stThr, &iveSadCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode2_out0.bin", &stSad_16);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode2_out0.bin", &stThr);

	// Config Setting.
	iveSadCtrl.enOutCtrl = IVE_SAD_OUT_CTRL_8BIT_BOTH;
	IVE_DMA_CTRL_S stDMACtrl;

	stDMACtrl.enMode = IVE_DMA_MODE_INTERVAL_COPY;
	stDMACtrl.u8ElemSize = 1;
	stDMACtrl.u8HorSegSize = 2;
	stDMACtrl.u8VerSegRows = 1;
	stDMACtrl.u64Val = 0;

	IVE_DATA_S stdmaSrc1;

	stdmaSrc1.u64VirAddr = stDst2.u64VirAddr[0];
	stdmaSrc1.u64PhyAddr = stDst2.u64PhyAddr[0];
	stdmaSrc1.u32Stride = stDst2.u32Stride[0];
	stdmaSrc1.u32Width = stDst2.u32Width;
	stdmaSrc1.u32Height = stDst2.u32Height;

	IVE_DATA_S stdmaDst;

	stdmaDst.u64VirAddr = stSad_8.u64VirAddr[0];
	stdmaDst.u64PhyAddr = stSad_8.u64PhyAddr[0];
	stdmaDst.u32Stride = stSad_8.u32Stride[0] / 2;
	stdmaDst.u32Width = stSad_8.u32Width;
	stdmaDst.u32Height = stSad_8.u32Height;

	printf("Run HW IVE SAD 8BIT MB_4X4.\n");
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_4X4;
	CVI_IVE_SAD(handle, &src1, &src2, &stDst2, &stThr, &iveSadCtrl, 1);
	CVI_IVE_DMA(handle, &stdmaSrc1, &stdmaDst, &stDMACtrl, CVI_TRUE);
	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode0_out1.bin", &stSad_8);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode0_out1.bin", &stThr);

	printf("Run HW IVE SAD 8BIT MB_8X8.\n");
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_8X8;
	CVI_IVE_SAD(handle, &src1, &src2, &stDst2, &stThr, &iveSadCtrl, 1);
	CVI_IVE_DMA(handle, &stdmaSrc1, &stdmaDst, &stDMACtrl, CVI_TRUE);
	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode1_out1.bin", &stSad_8);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode1_out1.bin", &stThr);

	printf("Run HW IVE SAD 8BIT MB_16X16.\n");
	iveSadCtrl.enMode = IVE_SAD_MODE_MB_16X16;
	CVI_IVE_SAD(handle, &src1, &src2, &stDst2, &stThr, &iveSadCtrl, 1);
	CVI_IVE_DMA(handle, &stdmaSrc1, &stdmaDst, &stDMACtrl, CVI_TRUE);
	CVI_IVE_WriteImg(handle, "sample_Sad_sad_mode2_out1.bin", &stSad_8);
	CVI_IVE_WriteImg(handle, "sample_Sad_thr_mode2_out1.bin", &stThr);

	CVI_SYS_FreeI(handle, &src1);
	CVI_SYS_FreeI(handle, &src2);
	CVI_SYS_FreeI(handle, &stSad_8);
	CVI_SYS_FreeI(handle, &stSad_16);
	CVI_SYS_FreeI(handle, &stDst2);
	CVI_SYS_FreeI(handle, &stThr);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
