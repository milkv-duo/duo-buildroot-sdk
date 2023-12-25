#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cvi_buffer.h"

#include "cvi_sys.h"

int main(int argc, char **argv)
{
	CVI_U32 id = 0;
	CVI_S32 ret = CVI_SUCCESS;

	UNUSED(argc);
	UNUSED(argv);

	ret = CVI_SYS_GetChipId(&id);

	switch (id) {
	case E_CHIPID_CV1800B:
		printf("cv1800b\n");
		break;
	case E_CHIPID_CV1812C:
		printf("cv1812c\n");
		break;
	case E_CHIPID_CV1813H:
		printf("cv1813h\n");
		break;
	default:
		printf("unsupported chip id\n");
		break;
	}

	return ret;
}

