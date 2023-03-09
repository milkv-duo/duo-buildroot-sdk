#ifndef __F_CVG_H
#define __F_CVG_H

#include <linux/types.h>

struct cvg_uurb32 {
	__s32 type;
	__s32 status;
	__u32 flags;
	__u32 buffer;
	__s32 buffer_length;
	__s32 actual_length;
	__s32 error_count;
	__u32 signr;
	__u32 usercontext; /* unused */
};

struct cvg_connectsignal32 {
	__s32 signr;
	__u32 context;
};

struct cvg_ion_queue32 {
	__s32 fd;
	__u32 vbase;
};

#define CVG_XFER_MAX		(4<<20)

#define CVG_UURB_DIR_MASK	1u
#define CVG_UURB_DIR_OUT	0
#define CVG_UURB_DIR_IN		1

#define CVG_UURB_APPEND_ZERO	BIT(1)

struct cvg_uurb {
	int type;
	int status;
	unsigned int flags;
	void *buffer;
	int buffer_length;
	int actual_length;
	int error_count;
	/* signal to be sent on completion,
	 * or 0 if none should be sent.
	 */
	unsigned int signr;
	void *usercontext;
};

struct cvg_connectsignal {
	unsigned int signr;
	void *context;
};

struct cvg_ion_queue {
	int fd;
	void *vbase;
};

#define CVG_IOCTL_CONNSIG	_IOR('U', 6, struct cvg_connectsignal)
#define CVG_IOCTL_CONNSIG32	_IOR('U', 7, struct cvg_connectsignal32)
#define CVG_IOCTL_SUBMITURB32   _IOR('U', 10, struct cvg_uurb32)
#define CVG_IOCTL_SUBMITURB	_IOR('U', 10, struct cvg_uurb)
#define CVG_IOCTL_DISCARDURB    _IOR('U', 11, struct cvg_uurb)
#define CVG_IOCTL_REAPTXURB32   _IOW('U', 13, __u32)
#define CVG_IOCTL_REAPTXURB	_IOW('U', 13, void *)
#define CVG_IOCTL_REAPRXURB32   _IOW('U', 14, __u32)
#define CVG_IOCTL_REAPRXURB	_IOW('U', 14, void *)
#define CVG_IOCTL_QUEUEION	_IOR('U', 15, struct cvg_ion_queue)
#define CVG_IOCTL_QUEUEION32	_IOR('U', 15, struct cvg_ion_queue32)

#define CVG_UURB_TYPE_ISO		0
#define CVG_UURB_TYPE_INTERRUPT		1
#define CVG_UURB_TYPE_CONTROL		2
#define CVG_UURB_TYPE_BULK		3

#define CVG_DEF_OUT_EP_NUM		1
#define CVG_DEF_IN_EP_NUM		1

#endif
