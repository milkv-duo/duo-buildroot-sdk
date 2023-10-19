/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_vo.h
 * Description:
 *   The common data type defination for VO module.
 */

#ifndef __CVI_COMM_VO_H__
#define __CVI_COMM_VO_H__

#include "cvi_type.h"
#include "cvi_common.h"
#include "cvi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CVI_TRACE_VO(level, fmt, ...)  \
	CVI_TRACE(level, CVI_ID_VO, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

typedef enum _EN_VOU_ERR_CODE_E {
	EN_ERR_VO_DEV_NOT_CONFIG = 0x40,
	EN_ERR_VO_DEV_NOT_ENABLED = 0x41,
	EN_ERR_VO_DEV_HAS_ENABLED = 0x42,

	EN_ERR_VO_LAYER_NOT_ENABLED = 0x45,
	EN_ERR_VO_LAYER_NOT_DISABLED = 0x46,
	EN_ERR_VO_LAYER_NOT_CONFIG = 0x47,

	EN_ERR_VO_CHN_NOT_DISABLED = 0x48,
	EN_ERR_VO_CHN_NOT_ENABLED = 0x49,
	EN_ERR_VO_CHN_NOT_CONFIG = 0x4a,

	EN_ERR_VO_WAIT_TIMEOUT = 0x4e,
	EN_ERR_VO_INVALID_VFRAME = 0x4f,
	EN_ERR_VO_INVALID_RECT_PARA = 0x50,

	EN_ERR_VO_CHN_AREA_OVERLAP = 0x65,

	EN_ERR_VO_INVALID_LAYERID = 0x66,


	/* new added */
	ERR_VO_BUTT

} EN_VOU_ERR_CODE_E;

/* System define error code */
#define CVI_ERR_VO_BUSY CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define CVI_ERR_VO_NO_MEM CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define CVI_ERR_VO_NULL_PTR CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define CVI_ERR_VO_SYS_NOTREADY CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define CVI_ERR_VO_INVALID_DEVID CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define CVI_ERR_VO_INVALID_CHNID CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define CVI_ERR_VO_ILLEGAL_PARAM CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define CVI_ERR_VO_NOT_SUPPORT CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define CVI_ERR_VO_NOT_PERMIT CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define CVI_ERR_VO_INVALID_LAYERID CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_INVALID_LAYERID)

/* Device relative error code */
#define CVI_ERR_VO_DEV_NOT_CONFIG CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define CVI_ERR_VO_DEV_NOT_ENABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLED)
#define CVI_ERR_VO_DEV_HAS_ENABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)

/* Video layer relative error code */
#define CVI_ERR_VO_VIDEO_NOT_ENABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_LAYER_NOT_ENABLED)
#define CVI_ERR_VO_VIDEO_NOT_DISABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_LAYER_NOT_DISABLED)
#define CVI_ERR_VO_VIDEO_NOT_CONFIG CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_LAYER_NOT_CONFIG)

/* Channel Relative error code */
#define CVI_ERR_VO_CHN_NOT_DISABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_DISABLED)
#define CVI_ERR_VO_CHN_NOT_ENABLED CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_ENABLED)
#define CVI_ERR_VO_CHN_NOT_CONFIG CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_CONFIG)
#define CVI_ERR_VO_CHN_AREA_OVERLAP CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_AREA_OVERLAP)

/* MISCellaneous error code*/
#define CVI_ERR_VO_WAIT_TIMEOUT CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WAIT_TIMEOUT)
#define CVI_ERR_VO_INVALID_VFRAME CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_INVALID_VFRAME)
#define CVI_ERR_VO_INVALID_RECT_PARA CVI_DEF_ERR(CVI_ID_VO, EN_ERR_LEVEL_ERROR, EN_ERR_VO_INVALID_RECT_PARA)


/* VO video output interface type */
#define VO_INTF_CVBS (0x01L << 0)
#define VO_INTF_YPBPR (0x01L << 1)
#define VO_INTF_VGA (0x01L << 2)
#define VO_INTF_BT656 (0x01L << 3)
#define VO_INTF_BT1120 (0x01L << 6)
#define VO_INTF_LCD (0x01L << 7)
#define VO_INTF_LCD_18BIT (0x01L << 10)
#define VO_INTF_LCD_24BIT (0x01L << 11)
#define VO_INTF_LCD_30BIT (0x01L << 12)
#define VO_INTF_MIPI (0x01L << 13)
#define VO_INTF_MIPI_SLAVE (0x01L << 14)
#define VO_INTF_HDMI (0x01L << 15)
#define VO_INTF_I80 (0x01L << 16)

typedef CVI_U32 VO_INTF_TYPE_E;

typedef enum _VO_INTF_SYNC_E {
	VO_OUTPUT_PAL = 0, /* PAL standard*/
	VO_OUTPUT_NTSC, /* NTSC standard */

	VO_OUTPUT_1080P24, /* 1920 x 1080 at 24 Hz. */
	VO_OUTPUT_1080P25, /* 1920 x 1080 at 25 Hz. */
	VO_OUTPUT_1080P30, /* 1920 x 1080 at 30 Hz. */

	VO_OUTPUT_720P50, /* 1280 x  720 at 50 Hz. */
	VO_OUTPUT_720P60, /* 1280 x  720 at 60 Hz. */
	VO_OUTPUT_1080P50, /* 1920 x 1080 at 50 Hz. */
	VO_OUTPUT_1080P60, /* 1920 x 1080 at 60 Hz. */

	VO_OUTPUT_576P50, /* 720  x  576 at 50 Hz. */
	VO_OUTPUT_480P60, /* 720  x  480 at 60 Hz. */

	VO_OUTPUT_800x600_60, /* VESA 800 x 600 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1024x768_60, /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1280x1024_60, /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1366x768_60, /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1440x900_60, /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
	VO_OUTPUT_1280x800_60, /* 1280*800@60Hz VGA@60Hz*/
	VO_OUTPUT_1600x1200_60, /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1680x1050_60, /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1920x1200_60, /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
	VO_OUTPUT_640x480_60, /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
	VO_OUTPUT_720x1280_60, /* For MIPI DSI Tx 720 x1280 at 60 Hz */
	VO_OUTPUT_1080x1920_60, /* For MIPI DSI Tx 1080x1920 at 60 Hz */
	VO_OUTPUT_USER, /* User timing. */

	VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

typedef enum _VO_CSC_MATRIX_E {
	VO_CSC_MATRIX_IDENTITY = 0,
	VO_CSC_MATRIX_BT601_TO_BT709,
	VO_CSC_MATRIX_BT709_TO_BT601,

	VO_CSC_MATRIX_BT601_TO_RGB_PC,
	VO_CSC_MATRIX_BT709_TO_RGB_PC,

	VO_CSC_MATRIX_RGB_TO_BT601_PC,
	VO_CSC_MATRIX_RGB_TO_BT709_PC,

	VO_CSC_MATRIX_RGB_TO_BT2020_PC,
	VO_CSC_MATRIX_BT2020_TO_RGB_PC,

	VO_CSC_MATRIX_RGB_TO_BT601_TV,
	VO_CSC_MATRIX_RGB_TO_BT709_TV,

	VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef enum _VO_I80_FORMAT {
	VO_I80_FORMAT_RGB444 = 0,
	VO_I80_FORMAT_RGB565,
	VO_I80_FORMAT_RGB666,
	VO_I80_FORMAT_MAX
} VO_I80_FORMAT;

typedef enum _VO_LVDS_LAND_ID {
	VO_LVDS_LANE_CLK = 0,
	VO_LVDS_LANE_0,
	VO_LVDS_LANE_1,
	VO_LVDS_LANE_2,
	VO_LVDS_LANE_3,
	VO_LVDS_LANE_MAX,
} VO_LVDS_LAND_ID;

/*
 * u32Priority: Video out overlay priority.
 * stRect: Rectangle of video output channel.
 */
typedef struct _VO_CHN_ATTR_S {
	CVI_U32 u32Priority;
	RECT_S stRect;
} VO_CHN_ATTR_S;

/*
 * u32ChnBufUsed: Channel buffer that been occupied.
 */
typedef struct _VO_QUERY_STATUS_S {
	CVI_U32 u32ChnBufUsed;
} VO_QUERY_STATUS_S;

/*
 * bSynm: sync mode(0:timing,as BT.656; 1:signal,as LCD)
 * bIop: interlaced or progressive display(0:i; 1:p)
 * u16FrameRate: frame-rate
 * u16Vact: vertical active area
 * u16Vbb: vertical back blank porch
 * u16Vfb: vertical front blank porch
 * u16Hact: horizontal active area
 * u16Hbb: horizontal back blank porch
 * u16Hfb: horizontal front blank porch
 * u16Hpw: horizontal pulse width
 * u16Vpw: vertical pulse width
 * bIdv: inverse data valid of output
 * bIhs: inverse horizontal synch signal
 * bIvs: inverse vertical synch signal
 */
typedef struct _VO_SYNC_INFO_S {
	CVI_BOOL bSynm;
	CVI_BOOL bIop;
	CVI_U16 u16FrameRate;

	CVI_U16 u16Vact;
	CVI_U16 u16Vbb;
	CVI_U16 u16Vfb;

	CVI_U16 u16Hact;
	CVI_U16 u16Hbb;
	CVI_U16 u16Hfb;

	CVI_U16 u16Hpw;
	CVI_U16 u16Vpw;

	CVI_BOOL bIdv;
	CVI_BOOL bIhs;
	CVI_BOOL bIvs;
} VO_SYNC_INFO_S;

/* Define I80's lane (0~3)
 *
 * CS: Chip Select
 * RS(DCX): Data/Command
 * WR: MCU Write to bus
 * RD: MCU Read from bus
 */
typedef struct _VO_I80_LANE_S {
	CVI_U8 CS;
	CVI_U8 RS;
	CVI_U8 WR;
	CVI_U8 RD;
} VO_I80_LANE_S;

/* Define I80's config
 *
 * lane_s: lane mapping
 * fmt: format of data
 * cycle_time: cycle time of WR/RD, unit ns, max 250
 */
typedef struct _VO_I80_CFG_S {
	VO_I80_LANE_S lane_s;
	VO_I80_FORMAT fmt;
	CVI_U16 cycle_time;
} VO_I80_CFG_S;

/* Define I80's cmd
 *
 * delay: ms to delay after instr
 * data_type: Data(1)/Command(0)
 * data: data to send
 */
typedef struct _VO_I80_INSTR_S {
	CVI_U8	delay;
	CVI_U8  data_type;
	CVI_U8	data;
} VO_I80_INSTR_S;

/* Define LVDS's config
 *
 * lvds_vesa_mode: true for VESA mode; false for JEIDA mode
 * data_big_endian: true for big endian; true for little endian
 * lane_id: lane mapping, -1 no used
 * lane_pn_swap: lane pn-swap if true
 */
typedef struct _VO_LVDS_ATTR_S {
	bool lvds_vesa_mode;
	bool data_big_endian;
	VO_LVDS_LAND_ID lane_id[VO_LVDS_LANE_MAX];
	bool lane_pn_swap[VO_LVDS_LANE_MAX];
} VO_LVDS_ATTR_S;

/*
 * u32BgColor: Background color of a device, in RGB format.
 * enIntfType: Type of a VO interface.
 * enIntfSync: Type of a VO interface timing.
 * stSyncInfo: Information about VO interface timings if customed type.
 * sti80Cfg: attritube for i80 interface if IntfType is i80
 * stLvdsAttr: attritube for lvds interface if IntfType is lvds
 */
typedef struct _VO_PUB_ATTR_S {
	CVI_U32 u32BgColor;
	VO_INTF_TYPE_E enIntfType;
	VO_INTF_SYNC_E enIntfSync;
	VO_SYNC_INFO_S stSyncInfo;
	union {
		VO_I80_CFG_S sti80Cfg;
		VO_LVDS_ATTR_S stLvdsAttr;
	};
} VO_PUB_ATTR_S;

/*
 * stDispRect: Display resolution
 * stImageSize: Original ImageSize.
 *              Only useful if vo support scaling, otherwise, it should be the same width stDispRect.
 * u32DispFrmRt: frame rate.
 * enPixFormat: Pixel format of the video layer
 */
typedef struct _VO_VIDEO_LAYER_ATTR_S {
	RECT_S stDispRect;
	SIZE_S stImageSize;
	CVI_U32 u32DispFrmRt;
	PIXEL_FORMAT_E enPixFormat;
} VO_VIDEO_LAYER_ATTR_S;

/*
 * enCscMatrix: CSC matrix
 */
typedef struct _VO_CSC_S {
	VO_CSC_MATRIX_E enCscMatrix;
} VO_CSC_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __CVI_COMM_VO_H__ */
