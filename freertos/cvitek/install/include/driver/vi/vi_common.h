#ifndef __VI_COMMON_H__
#define __VI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <linux/clk.h>
//#include <linux/debugfs.h>
//#include <linux/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <types.h>
#include <time.h>
#include "FreeRTOS.h"
#include <fifo.h>

#ifndef NULL
#define NULL 0
#endif

#define NONE             0   /* No errors */
#define ENOENT           2   /* No such file or directory */
#define EINTR            4   /* Interrupted system call */
#define EIO              5   /* I/O error */
#define ENXIO            6   /* No such device or address */
#define EBADF            9   /* Bad file number */
#define EAGAIN           11  /* No more processes */
#define EWOULDBLOCK      11  /* Operation would block */
#define ENOMEM           12  /* Not enough memory */
#define EACCES           13  /* Permission denied */
#define EFAULT           14  /* Bad address */
#define EBUSY            16  /* Mount device busy */
#define EEXIST           17  /* File exists */
#define EXDEV            18  /* Cross-device link */
#define ENODEV           19  /* No such device */
#define ENOTDIR          20  /* Not a directory */
#define EISDIR           21  /* Is a directory */
#define EINVAL           22  /* Invalid argument */
#define ENOSPC           28  /* No space left on device */
#define ESPIPE           29  /* Illegal seek */
#define EROFS            30  /* Read only file system */
#define EUNATCH          42  /* Protocol driver not attached */
#define EBADE            50  /* Invalid exchange */
#define EFTYPE           79  /* Inappropriate file type or format */
#define ENMFILE          89  /* No more files */
#define ENOTEMPTY        90  /* Directory not empty */
#define ENAMETOOLONG     91  /* File or path name too long */
#define EOPNOTSUPP       95  /* Operation not supported on transport endpoint */
#define ENOBUFS          105 /* No buffer space available */
#define ENOPROTOOPT      109 /* Protocol not available */
#define EADDRINUSE       112 /* Address already in use */
#define ETIMEDOUT        116 /* Connection timed out */
#define EINPROGRESS      119 /* Connection already in progress */
#define EALREADY         120 /* Socket already connected */
#define EADDRNOTAVAIL    125 /* Address not available */
#define EISCONN          127 /* Socket is already connected */
#define ENOTCONN         128 /* Socket is not connected */
#define ENOMEDIUM        135 /* No medium inserted */
#define EILSEQ           138 /* An invalid UTF-16 sequence was encountered. */
#define ECANCELED        140 /* Operation canceled. */

// #define MIN(a, b) (((a) < (b))?(a):(b))
// #define MAX(a, b) (((a) > (b))?(a):(b))
#define VI_64_ALIGN(x) (((x) + 0x3F) & ~0x3F)   // for 64byte alignment
#define VI_256_ALIGN(x) (((x) + 0xFF) & ~0xFF)   // for 256byte alignment
#define VI_ALIGN(x) (((x) + 0xF) & ~0xF)   // for 16byte alignment
#define VI_256_ALIGN(x) (((x) + 0xFF) & ~0xFF)   // for 256byte alignment
#define ISP_ALIGN(x, y) (((x) + (y - 1)) & ~(y - 1))   // for any bytes alignment
#define UPPER(x, y) (((x) + ((1 << (y)) - 1)) >> (y))   // for alignment
#define CEIL(x, y) (((x) + ((1 << (y)))) >> (y))   // for alignment

extern u32 vi_log_lv;

#define vi_pr(level, fmt, arg...) \
do { \
	if (vi_log_lv & level) { \
		if (level == VI_ERR) \
			printf("%s:%d(): " fmt, __func__, __LINE__, ## arg); \
		else if (level == VI_WARN) \
			printf("%s:%d(): " fmt, __func__, __LINE__, ## arg); \
		else if (level == VI_NOTICE) \
			printf("%s:%d(): " fmt, __func__, __LINE__, ## arg); \
		else if (level == VI_INFO) \
			printf("%s:%d(): " fmt, __func__, __LINE__, ## arg); \
		else if (level == VI_DBG) \
			printf("%s:%d(): " fmt, __func__, __LINE__, ## arg); \
	} \
} while (0)

enum vi_msg_pri {
	VI_ERR		= 0x1,
	VI_WARN		= 0x2,
	VI_NOTICE	= 0x4,
	VI_INFO		= 0x8,
	VI_DBG		= 0x10,
};

struct vi_rect {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};


void _reg_write_mask(uintptr_t addr, u32 mask, u32 data);
int vip_sys_cif_cb(unsigned int cmd, void *arg);
int vip_sys_cmm_cb_i2c(unsigned int cmd, void *arg);
void vip_sys_reg_write_mask(uintptr_t addr, u32 mask, u32 data);
//extern bool __clk_is_enabled(struct clk *clk);

#ifdef __cplusplus
}
#endif

#endif /* __VI_COMMON_H__ */
