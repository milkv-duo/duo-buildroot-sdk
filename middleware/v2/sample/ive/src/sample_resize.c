#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct IVE_IMAGE_FILL_U8C3_PLANAR_S {
	CVI_U8 u8Type;
	CVI_U32 u32SrcPhyAddr[3];
	CVI_U32 u32DstPhyAddr[3];
	CVI_U16 u16SrcStride[3];
	CVI_U16 u16DstStride[3];
	CVI_U16 u16SrcWidth;
	CVI_U16 u16SrcHeight;
	CVI_U16 u16DstWidth;
	CVI_U16 u16DstHeight;
	CVI_U16 u16XScale;
	CVI_U16 u16YScale;
} IVE_IMAGE_FILL_U8C3_PLANAR_S;

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name>\n",
		       argv[0]);
		printf("Example: %s 352 288 data/campus_352x288.rgb\n", argv[0]);
		return CVI_FAILURE;
	}
	// campus_352x288.rgb
	const char *filename = argv[3];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);

	IVE_DST_IMAGE_S astDst[2];
	IVE_SRC_IMAGE_S astSrc[2];
	CVI_U16 au16ResizeWidth[2];
	CVI_U16 au16ResizeHeight[2];

	au16ResizeWidth[0] = 176;
	au16ResizeHeight[0] = 144;

	au16ResizeWidth[1] = 320;
	au16ResizeHeight[1] = 240;

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	CVI_IVE_ReadRawImage(handle, &astSrc[0], filename,
			  IVE_IMAGE_TYPE_U8C3_PLANAR, input_w, input_h);
	CVI_IVE_ReadRawImage(handle, &astSrc[1], filename,
			  IVE_IMAGE_TYPE_U8C3_PLANAR, input_w, input_h);

	// Create dst image.
	CVI_IVE_CreateImage(handle, &astDst[0], IVE_IMAGE_TYPE_U8C3_PLANAR,
			    au16ResizeWidth[0], au16ResizeHeight[0]);
	CVI_IVE_CreateImage(handle, &astDst[1], IVE_IMAGE_TYPE_U8C3_PLANAR,
			    au16ResizeWidth[1], au16ResizeHeight[1]);

	// Config Setting.
	IVE_RESIZE_CTRL_S stCtrl;

	stCtrl.u16Num = 2;
	CVI_IVE_CreateMemInfo(handle, &(stCtrl.stMem),
			      stCtrl.u16Num *
				      sizeof(IVE_IMAGE_FILL_U8C3_PLANAR_S) * 2);

	// Run HW IVE.
	printf("Run HW IVE Resize LINEAR.\n");
	stCtrl.enMode = IVE_RESIZE_MODE_LINEAR;
	CVI_IVE_Resize(handle, astSrc, astDst, &stCtrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Resize_Bilinear_rgb.rgb", &astDst[0]);
	CVI_IVE_WriteImg(handle, "sample_Resize_Bilinear_240p.rgb", &astDst[1]);

	printf("Run HW IVE Resize AREA.\n");
	stCtrl.enMode = IVE_RESIZE_MODE_AREA;
	CVI_IVE_Resize(handle, astSrc, astDst, &stCtrl, 1);
	CVI_IVE_WriteImg(handle, "sample_Resize_Area_rgb.rgb", &astDst[0]);
	CVI_IVE_WriteImg(handle, "sample_Resize_Area_240p.rgb", &astDst[1]);

	CVI_SYS_FreeI(handle, &astSrc[0]);
	CVI_SYS_FreeI(handle, &astSrc[1]);
	CVI_SYS_FreeI(handle, &astDst[0]);
	CVI_SYS_FreeI(handle, &astDst[1]);
	CVI_SYS_FreeM(handle, &(stCtrl.stMem));
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
