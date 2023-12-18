#ifndef __CVI_WIEGAND_GPIO_H__
#define __CVI_WIEGAND_GPIO_H__

#include <linux/cdev.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/list.h>

#define IOCTL_BASE	'W'
#define IOCTL_WGN_SET_TX_CFG	_IO(IOCTL_BASE, 1)
#define IOCTL_WGN_SET_RX_CFG	_IO(IOCTL_BASE, 2)
#define IOCTL_WGN_TX			_IO(IOCTL_BASE, 3)
#define IOCTL_WGN_RX			_IO(IOCTL_BASE, 4)

#define WDIN0	0
#define WDIN1	1
#define WDOUT0	2
#define WDOUT1	3

#define GPIO_NUM_MIN	404
#define GPIO_NUM_MAX	511

#define BITCOUNT_26	26
#define BITCOUNT_34	34

#define MAX_WIEGAND_BYTES 6

struct wgn_tx_cfg {
	uint32_t tx_lowtime;  //us
	uint32_t tx_hightime; //ms
	uint32_t tx_bitcount; //26 or 34
	uint32_t tx_msb1st;
};

struct wgn_rx_cfg {
	uint32_t rx_idle_timeout; //ms
	uint32_t rx_bitcount; //26 or 34
	uint32_t rx_msb1st;
};

struct cvi_wiegand_device {
	struct device *dev;
	struct cdev cdev;
	spinlock_t close_lock;
	int startParity;
	int endParity;
	char buffer[MAX_WIEGAND_BYTES];
	int currentBit;
	unsigned int FacilityCode;
	unsigned int UserCode;
	int use_count;
	uint64_t tx_data;
};

#endif /* __CVI_WIEGAND_GPIO_H__ */
