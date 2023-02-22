#include "cvi_gyro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int ret = CVI_SUCCESS;
	int who_ma_i = 0;
	int reset = 0;
	CVI_S16 temp_val;
	CVI_U8 gyro_id;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "check"))
			who_ma_i = 1;
		if (!strcmp(argv[i], "reset"))
			reset = 1;
	}

	ret = CVI_GYRO_Create();
	if (ret != CVI_SUCCESS) {
		printf("gyro instance create failed\n");
		return CVI_FAILURE;
	}

	if (reset) {
		CVI_GYRO_Reset();
	}

	if (who_ma_i) {
		CVI_GYRO_WHO_AM_I(&gyro_id);
		printf("Who am i (0x%02x)\n", gyro_id);
	}

	CVI_GYRO_GET_TEMP(&temp_val);

	printf("T:(%d)\n", temp_val);

	CVI_GYRO_Destroy();
	return ret;
}
