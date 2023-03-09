/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_base.h
 * Description:
 */


#ifndef _U_CVI_BASE_H_
#define _U_CVI_BASE_H_

#include <linux/types.h>

struct base_statesignal32 {
	unsigned int signr;
	unsigned int context;
};

struct base_statesignal {
	unsigned int signr;
	void *context;
};

enum base_state_e {
	BASE_STATE_NORMAL = 0,		/* init state or after user space resume is complete. */
	BASE_STATE_SUSPEND_PREPARE,	/* enter when suspend prepare is notified. */
	BASE_STATE_SUSPEND,		/* enter after user space suspend is done. */
	BASE_STATE_RESUME,		/* enter when suspend post is notified. */
	BASE_STATE_NUM
};

/* chip ID list */
enum ENUM_CHIP_ID {
	E_CHIPID_CV1822 = 0,		//0
	E_CHIPID_CV1832,		//1
	E_CHIPID_CV1835,		//2
	E_CHIPID_CV1838,		//3
	E_CHIPID_CV1829,		//4
	E_CHIPID_CV1826,		//5
	E_CHIPID_CV1821,		//6
	E_CHIPID_CV1820,		//7
	E_CHIPID_CV1823,		//8
	E_CHIPID_CV1825,		//9
// cv181 chips
	E_CHIPID_CV1820A,		//10
	E_CHIPID_CV1821A,		//11
	E_CHIPID_CV1822A,		//12
	E_CHIPID_CV1823A,		//13
	E_CHIPID_CV1825A,		//14
	E_CHIPID_CV1826A,		//15
	E_CHIPID_CV1810C,		//16
	E_CHIPID_CV1811C,		//17
	E_CHIPID_CV1812C,		//18
	E_CHIPID_CV1811H,		//19
	E_CHIPID_CV1812H,		//20
	E_CHIPID_CV1813H,		//21
// cv180 chips
	E_CHIPID_CV1800B,		//22
	E_CHIPID_CV1801B,		//23
	E_CHIPID_CV1800C,		//24
	E_CHIPID_CV1801C,		//25
};

/* chip version list */
enum ENUM_CHIP_VERSION {
	E_CHIPVERSION_U01 = 1,	//1
	E_CHIPVERSION_U02,	//2
};

/* chip power on reason list */
enum ENUM_CHIP_PWR_ON_REASON {
	E_CHIP_PWR_ON_COLDBOOT = 1,	//1
	E_CHIP_PWR_ON_WDT,	//2
	E_CHIP_PWR_ON_SUSPEND,	//3
	E_CHIP_PWR_ON_WARM_RST,	//4
};

struct vb_ext_control {
	__u32 id;
	__u32 reserved[1];
	union {
		__s32 value;
		__s64 value64;
		void *ptr;
	};
} __attribute__ ((packed));

#define IOCTL_BASE_MAGIC	's'
#define IOCTL_READ_CHIP_ID		_IOR(IOCTL_BASE_MAGIC, 1, unsigned int)
#define IOCTL_READ_CHIP_VERSION	_IOR(IOCTL_BASE_MAGIC, 2, unsigned int)
#define IOCTL_STATESIG		_IOW(IOCTL_BASE_MAGIC, 3, struct base_statesignal)
#define IOCTL_STATESIG32	_IOW(IOCTL_BASE_MAGIC, 3, struct base_statesignal32)
#define IOCTL_READ_STATE	_IOR(IOCTL_BASE_MAGIC, 4, unsigned int)
#define IOCTL_USER_SUSPEND_DONE	_IOR(IOCTL_BASE_MAGIC, 5, unsigned int)
#define IOCTL_USER_RESUME_DONE	_IOR(IOCTL_BASE_MAGIC, 6, unsigned int)
#define IOCTL_READ_CHIP_PWR_ON_REASON	_IOR(IOCTL_BASE_MAGIC, 7, unsigned int)
#define IOCTL_VB_CMD		_IOWR(IOCTL_BASE_MAGIC, 8, struct vb_ext_control)

#endif // _U_CVI_BASE_H_
