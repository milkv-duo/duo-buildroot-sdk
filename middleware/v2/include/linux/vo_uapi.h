#ifndef __U_VO_UAPI_H__
#define __U_VO_UAPI_H__

#include <linux/cvi_comm_vpss.h>
#include <linux/cvi_comm_vo.h>
#ifdef __cplusplus
	extern "C" {
#endif

#define VO_IOC_MAGIC		'o'
#define VO_IOC_BASE		0x20

#define VO_IOC_G_CTRL		_IOWR(VO_IOC_MAGIC, VO_IOC_BASE, struct vo_ext_control)
#define VO_IOC_S_CTRL		_IOWR(VO_IOC_MAGIC, VO_IOC_BASE + 1, struct vo_ext_control)

enum VO_IOCTL {
	//VO_IOC_S_CTRL
	VO_IOCTL_BASE = 0,
	VO_IOCTL_VB_DONE,
	VO_IOCTL_INTR,
	VO_IOCTL_OUT_CSC,
	VO_IOCTL_PATTERN,
	VO_IOCTL_FRAME_BGCOLOR,
	VO_IOCTL_WINDOW_BGCOLOR,
	VO_IOCTL_ONLINE,
	VO_IOCTL_INTF,
	VO_IOCTL_ENABLE_WIN_BGCOLOR,
	VO_IOCTL_SET_ALIGN = 10,
	VO_IOCTL_SET_RGN,
	VO_IOCTL_SET_CUSTOM_CSC,
	VO_IOCTL_I80_SW_MODE,
	VO_IOCTL_I80_CMD,
	VO_IOCTL_SET_CLK,
	VO_IOCTL_GAMMA_LUT_UPDATE,

	//VO_IOC_G_CTRL
	VO_IOCTL_GET_VLAYER_SIZE,
	VO_IOCTL_GET_PANEL_STATUS,
	VO_IOCTL_GET_INTF_TYPE,
	VO_IOCTL_GAMMA_LUT_READ = 20,

	//VO_IOC_S_SELECTION
	VO_IOCTL_SEL_TGT_COMPOSE,
	VO_IOCTL_SEL_TGT_CROP,

	//VO_IOC_S_DV_TIMINGS
	VO_IOCTL_SET_DV_TIMINGS,
	VO_IOCTL_GET_DV_TIMINGS,
	VO_IOCTL_SET_FMT,

	VO_IOCTL_START_STREAMING,
	VO_IOCTL_STOP_STREAMING,
	VO_IOCTL_ENQ_BUF,
	VO_IOCTL_ENQ_WAITQ,
	VO_IOCTL_SDK_CTRL,
	VO_IOCTL_MAX,
};

enum VO_SDK_CTRL {
	VO_SDK_SET_DEVATTR,//0
	VO_SDK_GET_DEVATTR,
	VO_SDK_ENABLE_DEV,
	VO_SDK_DISABLE_DEV,
	VO_SDK_SET_PIPEATTR,
	VO_SDK_GET_PIPEATTR,

	//CHN CTRL
	VO_SDK_SET_CHNATTR,
	VO_SDK_GET_CHNATTR,
	VO_SDK_ENABLE_CHN,
	VO_SDK_DISABLE_CHN,
	VO_SDK_ENABLE,//10
	VO_SDK_DISABLE,
	VO_SDK_SHOW_CHN,
	VO_SDK_SET_DISPLAYBUFLEN,
	VO_SDK_GET_DISPLAYBUFLEN,
	VO_SDK_HIDE_CHN,
	VO_SDK_RESUME_CHN,
	VO_SDK_PAUSE_CHN,
	VO_SDK_SEND_FRAME,
	VO_SDK_SET_PUBATTR,
	VO_SDK_GET_PUBATTR,//20
	VO_SDK_GET_PANELSTATUE,
	VO_SDK_SET_VIDEOLAYERATTR,
	VO_SDK_GET_VIDEOLAYERATTR,
	VO_SDK_ENABLE_VIDEOLAYER,
	VO_SDK_DISABLE_VIDEOLAYER,
	VO_SDK_SET_CHNROTATION,
	VO_SDK_GET_CHNROTATION,
	VO_SDK_SUSPEND,
	VO_SDK_RESUME,
	VO_SDK_CLEAR_CHNBUF,
};

struct vo_bt_timings {
	__u32 width;
	__u32 height;
	__u32 interlaced;
	__u32 polarities;
	__u64 pixelclock;
	__u32 hfrontporch;
	__u32 hsync;
	__u32 hbackporch;
	__u32 vfrontporch;
	__u32 vsync;
	__u32 vbackporch;
	__u32 il_vfrontporch;
	__u32 il_vsync;
	__u32 il_vbackporch;
	__u32 standards;
	__u32 flags;
	__u32 reservedd[14];
};

struct vo_dv_timings {
	__u32 type;
	struct vo_bt_timings bt;
};

struct vo_ext_control {
	__u32 id;
	__u32 sdk_id;
	__u32 size;
	__u32 reserved[1];
	union {
		__s32 value;
		__s64 value64;
		void *ptr;
	};
};

struct vo_capability {
	__u8 driver[16];
	__u8 card[32];
	__u8 bus_info[32];
	__u32 version;
	__u32 capabilities;
	__u32 vdevice_caps;
	__u32 reserved[3];

};

struct vo_rect {
	__u32 left;
	__u32 top;
	__u32 width;
	__u32 height;
};

struct vo_plane_pix_format {
	__u32 sizeimage;
	__u16 bytesperline;
	__u16 reserved[7];
};

struct vo_pix_format_mplane {
	__u32 width;
	__u32 height;
	__u32 pixelformat;
	__u32 field;
	__u32 colorspace;
	struct vo_plane_pix_format plane_fmt[3];
	__u8 num_planes;
	__u8 reserved[11];
};
struct vo_selection {
	__u32 type;
	__u32 target;
	__u32 flags;
	struct vo_rect r;
	__u32 reserved[9];
};

struct vo_plane {
	__u32 length;
	__u64 addr;
	__u32 bytesused;
	union {
	__u32 offset;
	__u64 userptr;
	} m;
};

enum vo_dv_pos_pol {
	VO_DV_VSYNC_POS_POL = 1,
	VO_DV_HSYNC_POS_POL,
};

/*
 * @index:
 * @length: length of planes
 * @planes: to describe buf
 * @reserved
 */
struct vo_buffer {
	__u32 index;
	__u32 length;
	struct vo_plane planes[3];
	__u32 reserved;
};

enum vo_colorspace {
	VO_COLORSPACE_SRGB = 0,
	VO_COLORSPACE_SMPTE170M,
	VO_COLORSPACE_MAX,
};

struct vo_test {
	__u32 type;
	__u32 target;
	__u32 flags;
	struct vo_rect r;
	__u32 reserved[9];
};

struct vo_fmt {
	__u32 fourcc;
	__u8 fmt;
	__u8 buffers;
	__u32 bit_depth[3];
	__u8 plane_sub_h;
	__u8 plane_sub_v;
};

//vo sdk layer config
struct vo_video_layer_cfg {
	__u8 VoLayer;
};

struct vo_video_layer_attr_cfg {
	__u8 VoLayer;
	VO_VIDEO_LAYER_ATTR_S pstLayerAttr;
};

struct vo_clear_chn_buf_cfg {
	__u8 VoLayer;
	__u8 VoChn;
	__u8 bClrAll;
};

struct vo_snd_frm_cfg {
	__u8 VoLayer;
	__u8 VoChn;
	VIDEO_FRAME_INFO_S stVideoFrame;
	__s32 s32MilliSec;
};

struct vo_gamma_info_cfg {
	VO_GAMMA_INFO_S *pinfo;
};

struct vo_display_buflen_cfg {
	__u8 VoLayer;
	__u32 u32BufLen;
};

struct vo_chn_attr_cfg {
	__u8 VoLayer;
	__u8 VoChn;
	VO_CHN_ATTR_S pstChnAttr;
};

struct vo_chn_cfg {
	__u8 VoLayer;
	__u8 VoChn;
};

struct vo_chn_rotation_cfg {
	__u8 VoLayer;
	__u8 VoChn;
	ROTATION_E enRotation;
};

struct vo_panel_status_cfg {
	__u8 VoLayer;
	__u8 VoChn;
	__u32 is_init;
};

struct vo_dev_cfg {
	__u8 VoDev;
};

struct vo_pub_attr_cfg {
	__u8 VoDev;
	VO_PUB_ATTR_S pstPubAttr;
};

struct vo_video_plane_pix_format {
	__u32 sizeimage;
	__u16 bytesperline;
	__u16 reserved[7];
};

struct vo_video_pix_format {
	__u32 width;
	__u32 height;
	__u32 pixelformat;
	__u32 field;
	__u32 bytesperline;
	__u32 sizeimage;
	__u32 colorspace;
	__u32 priv;
};

struct vo_video_format_mplane {
	__u32 width;
	__u32 height;
	__u32 pixelformat;
	__u32 field;
	__u32 colorspace;
	struct vo_video_plane_pix_format plane_fmt[3];
	__u8 num_planes;
	__u8 reserved[11];
};

struct vo_video_format {
	__u32 type;
	union {
		struct vo_video_pix_format pix;
		struct vo_video_format_mplane pix_mp;
		__u8 raw_data[200];
	} fmt;
};

#ifdef __cplusplus
	}
#endif

#endif /* __U_VO_UAPI_H__ */

