#ifndef __U_RGN_UAPI_H__
#define __U_RGN_UAPI_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
#include <linux/time_types.h>
#endif

#define RGN_IOC_MAGIC		'V'
#define RGN_IOC_BASE		0x20

#define RGN_IOC_G_CTRL		_IOWR(RGN_IOC_MAGIC, RGN_IOC_BASE, struct rgn_ext_control)
#define RGN_IOC_S_CTRL		_IOWR(RGN_IOC_MAGIC, RGN_IOC_BASE + 1, struct rgn_ext_control)

enum RNG_IOCTL {
	RGN_IOCTL_SC_SET_RGN,
	RGN_IOCTL_DISP_SET_RGN,
	RGN_IOCTL_SDK_CTRL,
	RGN_IOCTL_MAX,
};

enum RGN_SDK_CTRL {
	RGN_SDK_CREATE,
	RGN_SDK_DESTORY,
	RGN_SDK_GET_ATTR,
	RGN_SDK_SET_ATTR,
	RGN_SDK_SET_BIT_MAP,
	RGN_SDK_ATTACH_TO_CHN,
	RGN_SDK_DETACH_FROM_CHN,
	RGN_SDK_SET_DISPLAY_ATTR,
	RGN_SDK_GET_DISPLAY_ATTR,
	RGN_SDK_GET_CANVAS_INFO,
	RGN_SDK_UPDATE_CANVAS,
	RGN_SDK_INVERT_COLOR,
	RGN_SDK_SET_CHN_PALETTE,
	RGN_SDK_MAX,
};

struct rgn_ext_control {
	__u32 id;
	__u32 sdk_id;
	__u32 handle;
	void *ptr1;
	void *ptr2;
} __attribute__ ((packed));

struct rgn_plane {
	__u64 addr;
};

/*
 * @index:
 * @length: length of planes
 * @planes: to describe buf
 * @reserved
 */
struct rgn_buffer {
	__u32 index;
	__u32 length;
	struct rgn_plane planes[3];
	__u32 reserved;
};

struct rgn_event {
	__u32			dev_id;
	__u32			type;
	__u32			frame_sequence;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
	struct __kernel_timespec   timestamp;
#else
	struct timeval		timestamp;
#endif
};

#ifdef __cplusplus
	}
#endif

#endif /* __U_RGN_UAPI_H__ */
