/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: dwa_uapi.h
 * Description:
 */

#ifndef _U_DWA_UAPI_H_
#define _U_DWA_UAPI_H_

#include <linux/cvi_comm_gdc.h>

#ifdef __cplusplus
extern "C" {
#endif


enum cvi_dwa_op {
	CVI_DWA_OP_NONE,
	CVI_DWA_OP_XY_FLIP,
	CVI_DWA_OP_ROT_90,
	CVI_DWA_OP_ROT_270,
	CVI_DWA_OP_LDC,
	CVI_DWA_OP_MAX,
};

struct cvi_ldc_buffer {
	__u8 pixel_fmt; // 0: Y only, 1: NV21
	__u8 rot;
	__u16 bgcolor; // data outside start/end if used in operation

	__u16 src_width; // src width, including padding
	__u16 src_height; // src height, including padding

	__u32 src_y_base;
	__u32 src_c_base;
	__u32 dst_y_base;
	__u32 dst_c_base;

	__u32 map_base;
};

struct cvi_ldc_rot {
	__u64 handle;

	void *pUsageParam;
	void *vb_in;
	__u32 enPixFormat;
	__u64 mesh_addr;
	__u8 sync_io;
	void *cb;
	void *pcbParam;
	__u32 cbParamSize;
	__u32 enModId;
	__u32 enRotation;
};

struct gdc_handle_data {
	__u64 handle;
};

/*
 * stImgIn: Input picture
 * stImgOut: Output picture
 * au64privateData[4]: RW; Private data of task
 * reserved: RW; Debug information,state of current picture
 */
struct gdc_task_attr {
	__u64 handle;

	struct _VIDEO_FRAME_INFO_S stImgIn;
	struct _VIDEO_FRAME_INFO_S stImgOut;
	__u64 au64privateData[4];
	__u32 enRotation;
	__u64 reserved;
	union {
		FISHEYE_ATTR_S stFishEyeAttr;
		AFFINE_ATTR_S stAffineAttr;
		LDC_ATTR_S stLDCAttr;
	};

	CVI_U64 meshHandle;
	struct _DWA_BUF_WRAP_S stBufWrap;
	CVI_U32 bufWrapDepth;
	CVI_U64 bufWrapPhyAddr;
};

struct dwa_buf_wrap_cfg {
	__u64 handle;
	struct gdc_task_attr stTask;
	struct _DWA_BUF_WRAP_S stBufWrap;
};

#define CVI_DWA_BEGIN_JOB _IOWR('D', 0x00, struct gdc_handle_data)
#define CVI_DWA_END_JOB _IOW('D', 0x01, struct gdc_handle_data)
#define CVI_DWA_CANCEL_JOB _IOW('D', 0x02, unsigned long long)
#define CVI_DWA_ADD_ROT_TASK _IOW('D', 0x03, struct gdc_task_attr)
#define CVI_DWA_ADD_LDC_TASK _IOW('D', 0x04, struct gdc_task_attr)

#define CVI_DWA_SET_BUF_WRAP _IOW('D', 0x10, struct dwa_buf_wrap_cfg)
#define CVI_DWA_GET_BUF_WRAP _IOWR('D', 0x11, struct dwa_buf_wrap_cfg)

#ifdef __cplusplus
}
#endif

#endif /* _U_DWA_UAPI_H_ */
