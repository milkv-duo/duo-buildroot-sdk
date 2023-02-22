#include "cvi_gyro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int ret = CVI_SUCCESS;
	int calibration = 0;
	int who_ma_i = 0;
	int reset = 0;
	uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
	CVI_S16 acc_val[3];
	CVI_U8 gyro_id;
	float ax, ay, az;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "cal"))
			calibration = 1;
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

	if (calibration) {
		CVI_GYRO_ACC_CALIBRATION();
	}

	//CVI_GYRO_GET_AX(&acc_val[0]);
	//CVI_GYRO_GET_AY(&acc_val[1]);
	//CVI_GYRO_GET_AZ(&acc_val[2]);

	// or one time ioctl;
	CVI_GYRO_GET_A_XYZ(&acc_val[0], &acc_val[1], &acc_val[2]);

	ax = (float)acc_val[0] / (float)accelsensitivity;
	ay = (float)acc_val[1] / (float)accelsensitivity;
	az = (float)acc_val[2] / (float)accelsensitivity;

	printf("A:(x:%f y:%f z:%f)\n", ax, ay, az);

	CVI_GYRO_Destroy();

	return ret;
}
