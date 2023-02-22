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
	//(gyro_raw_data * 250) / 32768 = gyro_raw_data / 131;
	uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
	CVI_S16 gyro_val[3];
	CVI_U8 gyro_id;
	float gx, gy, gz;

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
		CVI_GYRO_CALIBRATION();
	}

	//CVI_GYRO_GET_GX(&gyro_val[0]);
	//CVI_GYRO_GET_GY(&gyro_val[1]);
	//CVI_GYRO_GET_GZ(&gyro_val[2]);

	// or one time ioctl;
	CVI_GYRO_GET_G_XYZ(&gyro_val[0], &gyro_val[1], &gyro_val[2]);

	gx = (float) gyro_val[0] / gyrosensitivity;
	gy = (float) gyro_val[1] / gyrosensitivity;
	gz = (float) gyro_val[2] / gyrosensitivity;

	printf("G:(x:%f y:%f z:%f)\n", gx, gy, gz);

	CVI_GYRO_Destroy();
	return ret;
}
