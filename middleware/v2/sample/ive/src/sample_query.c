#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	IVE_HANDLE handle = CVI_IVE_CreateHandle();
	CVI_BOOL block = CVI_FALSE, bFinish = CVI_FALSE;

	UNUSED(argc);
	UNUSED(argv);

	printf("!!!!!!bFinish = %d\n", bFinish);
	int ret = CVI_IVE_QUERY(handle, &bFinish, block);

	printf("!!!!!!bFinish = %d\n", bFinish);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
