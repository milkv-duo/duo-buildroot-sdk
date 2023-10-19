/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_base.h
 * Description:
 */


#ifndef _U_CVI_BASE_H_
#define _U_CVI_BASE_H_

#define IOCTL_BASE_BASE		's'
#define IOCTL_READ_CHIP_ID	_IOR(IOCTL_BASE_BASE, 1, unsigned int)
#define IOCTL_READ_CHIP_VERSION	_IOR(IOCTL_BASE_BASE, 2, unsigned int)

/* chip ID list */
enum ENUM_CHIP_ID {
	E_CHIPID_CV1822 = 0,	//0
	E_CHIPID_CV1832,	//1
	E_CHIPID_CV1835,	//2
	E_CHIPID_CV1838,	//3
	E_CHIPID_CV1829,
	E_CHIPID_CV1826
};

/* chip version list */
enum ENUM_CHIP_VERSION {
	E_CHIPVERSION_U01 = 1,	//1
	E_CHIPVERSION_U02,	//2
};

#endif // _U_CVI_BASE_H_
