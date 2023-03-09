/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_gyro_ioctl.h
 * Description:
 */

#ifndef __CVI_GYRO_IOCTL_H1__
#define __CVI_GYRO_IOCTL_H__

#include "cvi_errno.h"

struct cvi_gy_regval {
	uint8_t addr;
	uint16_t val;
};

struct cvi_gy_regval_6byte {
	uint8_t addr;
	uint16_t x_val;
	uint16_t y_val;
	uint16_t z_val;
};

#define CVI_GYRO_IOC_MAGIC      'g'
#define CVI_GYRO_IOC_CHECK      _IOR(CVI_GYRO_IOC_MAGIC, 0x00, unsigned long long)
#define CVI_GYRO_IOC_READ       _IOWR(CVI_GYRO_IOC_MAGIC, 0x01, unsigned long long)
#define CVI_GYRO_IOC_READ_2BYTE _IOWR(CVI_GYRO_IOC_MAGIC, 0x02, unsigned long long)
#define CVI_GYRO_IOC_READ_6BYTE _IOWR(CVI_GYRO_IOC_MAGIC, 0x03, unsigned long long)
#define CVI_GYRO_IOC_WRITE      _IOW(CVI_GYRO_IOC_MAGIC, 0x04, unsigned long long)
#define CVI_GYRO_IOC_WRITE_OR   _IOW(CVI_GYRO_IOC_MAGIC, 0x05, unsigned long long)
#define CVI_GYRO_IOC_WRITE_AND  _IOW(CVI_GYRO_IOC_MAGIC, 0x06, unsigned long long)

#define CVI_GYRO_IOC_ADJUST     _IO(CVI_GYRO_IOC_MAGIC, 0x10)
#define CVI_GYRO_IOC_ACC_ADJUST _IO(CVI_GYRO_IOC_MAGIC, 0x11)
#endif /* __CVI_GYRO_IOCTL_H__ */
