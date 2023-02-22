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

	/* 5 by 5*/
	CVI_S8 arr5by5[25] = { -1, -2, 0,  2,  1, -4, -8, 0,  8,  4, -6, -12, 0,
			       12, 6,  -4, -8, 0, 8,  4,  -1, -2, 0, 2,	 1 };
	/* 3 by 3*/
	CVI_S8 arr3by3[25] = { 0, 0, 0, 0,  0, 0, -1, 0, 1, 0, 0, -2, 0,
			       2, 0, 0, -1, 0, 1, 0,  0, 0, 0, 0, 0 };

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S stEdge;

	CVI_IVE_CreateImage(handle, &stEdge, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	IVE_MEM_INFO_S stStack;

	CVI_IVE_CreateMemInfo(handle, &stStack,
			      input_w * input_h * (sizeof(IVE_POINT_U16_S)) +
				      sizeof(IVE_CANNY_STACK_SIZE_S));

	// Config Setting.
	IVE_CANNY_HYS_EDGE_CTRL_S stCannyHysEdgeCtrl;

	CVI_IVE_CreateMemInfo(handle, &(stCannyHysEdgeCtrl.stMem),
			      input_w * input_h * 4 *
				      (sizeof(CVI_U16) + sizeof(CVI_U8)));
	stCannyHysEdgeCtrl.u16LowThr = 42;
	stCannyHysEdgeCtrl.u16HighThr = 3 * stCannyHysEdgeCtrl.u16LowThr;
	memset((CVI_U8 *)(uintptr_t)((IVE_MEM_INFO_S)stCannyHysEdgeCtrl.stMem).u64VirAddr,
	       0, ((IVE_MEM_INFO_S)stCannyHysEdgeCtrl.stMem).u32Size);
	memcpy(stCannyHysEdgeCtrl.as8Mask, arr3by3, 5 * 5 * sizeof(CVI_S8));

	// Run HW IVE.
	printf("Run HW IVE CannyEdge 3x3.\n");
	gettimeofday(&t0, NULL);
	//ret |= CVI_IVE_CannyEdge(handle, &src, &stEdge, &stCannyHysEdgeCtrl, 1);
	ret |= CVI_IVE_CannyHysEdge(handle, &src, &stEdge, &stStack,
				    &stCannyHysEdgeCtrl, 1);
	ret |= CVI_IVE_CannyEdge(&stEdge, &stStack);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_CannyEdge_3x3.yuv", &stEdge);

	printf("Run HW IVE CannyEdge 5x5.\n");
	stCannyHysEdgeCtrl.u16LowThr = 108;
	stCannyHysEdgeCtrl.u16HighThr = 3 * stCannyHysEdgeCtrl.u16LowThr;
	memcpy(stCannyHysEdgeCtrl.as8Mask, arr5by5, 5 * 5 * sizeof(CVI_S8));
	//ret |= CVI_IVE_CannyEdge(handle, &src, &stEdge, &stCannyHysEdgeCtrl, 1);
	ret |= CVI_IVE_CannyHysEdge(handle, &src, &stEdge, &stStack,
				    &stCannyHysEdgeCtrl, 1);
	ret |= CVI_IVE_CannyEdge(&stEdge, &stStack);
	CVI_IVE_WriteImg(handle, "sample_CannyEdge_5x5.yuv", &stEdge);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &stEdge);
	CVI_SYS_FreeM(handle, &stStack);
	CVI_SYS_FreeM(handle, &stCannyHysEdgeCtrl.stMem);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
