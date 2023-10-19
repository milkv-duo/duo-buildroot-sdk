#ifndef __CV183X_BASE_H__
#define __CV183X_BASE_H__

#include <linux/types.h>

#ifdef __LINUX__
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#endif

#include <linux/miscdevice.h>
#include "uapi/cvi_base.h"

/* bit operations */
#define BIT0 (0x0001)
#define BIT1 (0x0002)
#define BIT2 (0x0004)
#define BIT3 (0x0008)
#define BIT4 (0x0010)
#define BIT5 (0x0020)
#define BIT6 (0x0040)
#define BIT7 (0x0080)
#define BIT8 (0x0100)
#define BIT9 (0x0200)
#define BIT10 (0x0400)
#define BIT11 (0x0800)
#define BIT12 (0x1000)
#define BIT13 (0x2000)
#define BIT14 (0x4000)
#define BIT15 (0x8000)

/* register bank */
#define TOP_BASE 0x03000000
#define TOP_REG_BANK_SIZE 0x10000
#define GP_REG3_OFFSET 0x8C
#define GP_REG_CHIP_ID_MASK 0xFFFF

struct base_device {
	struct device *dev;
	struct miscdevice miscdev;
	void *shared_mem;
	u16 mmap_count;
};

//int vip_sys_cif_cb(unsigned int cmd, void *arg);

#endif /* __CV183X_BASE_H__ */
