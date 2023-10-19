#ifndef _CVI_VIP_DEBUG_H_
#define _CVI_VIP_DEBUG_H_
#ifdef __LINUX__
#include <linux/debugfs.h>
#endif
extern int log_level;

#define dprintk(level, fmt, arg...) \
	do { \
		if (log_level & level) { \
			if (level == VIP_ERR) \
				pr_debug("%d:%s(): " fmt, __LINE__, __func__, ## arg); \
			else \
				pr_debug("%d:%s(): " fmt, __LINE__, __func__, ## arg); \
		} \
	} while (0)
#define dpr_cont(level, fmt, arg...) \
	do { \
		if (log_level & level) { \
			if (level == VIP_ERR) \
				pr_debug(fmt, ## arg); \
			else \
				pr_debug(fmt, ## arg); \
		} \
	} while (0)

enum vip_msg_prio {
	VIP_ERR		= 0x0001,
	VIP_WARN	= 0x0002,
	VIP_INFO	= 0x0004,
	VIP_DBG		= 0x0008,
	VIP_VB2		= 0x0010,
	VIP_ISP_IRQ	= 0x0020,
};

#endif
