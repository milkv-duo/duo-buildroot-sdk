/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_vi.h
 * Description:
 *   Common video input definitions.
 */

#ifndef __CVI_COMM_VI_H__
#define __CVI_COMM_VI_H__

#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>
//#include "cvi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define VI_MAX_ADCHN_NUM (4UL)

#define VI_COMPMASK_NUM (2UL)
#define VI_PRO_MAX_FRAME_NUM (8UL)
#define VI_SHARPEN_GAIN_NUM 32
#define VI_AUTO_ISO_STRENGTH_NUM 16

#define VI_INVALID_FRMRATE (-1)
#define VI_CHN0 0
#define VI_CHN1 1
#define VI_CHN2 2
#define VI_CHN3 3
#define VI_INVALID_CHN -1

#define VI_MAX_VC_NUM 4

typedef struct _VI_LOW_DELAY_INFO_S {
	CVI_BOOL bEnable; /* RW; Low delay enable. */
	CVI_U32 u32LineCnt; /* RW; Range: [32, 16384]; Low delay shoreline. */
} VI_LOW_DELAY_INFO_S;

/* Information of raw data cmpresss param */
typedef struct _VI_CMP_PARAM_S {
	CVI_U8 au8CmpParam[VI_CMP_PARAM_SIZE];
} VI_CMP_PARAM_S;

typedef enum _VI_USERPIC_MODE_E {
	VI_USERPIC_MODE_PIC = 0, /* YUV picture */
	VI_USERPIC_MODE_BGC, /* Background picture only with a color */
	VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

typedef struct _VI_USERPIC_BGC_S {
	CVI_U32 u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct _VI_USERPIC_ATTR_S {
	VI_USERPIC_MODE_E enUsrPicMode; /* User picture mode */
	union {
		VIDEO_FRAME_INFO_S stUsrPicFrm; /* Information about a YUV picture */
		VI_USERPIC_BGC_S stUsrPicBg; /* Information about a background picture only with a color */
	} unUsrPic;
} VI_USERPIC_ATTR_S;

/* interface mode of video input */
typedef enum _VI_INTF_MODE_E {
	VI_MODE_BT656 = 0, /* ITU-R BT.656 YUV4:2:2 */
	VI_MODE_BT601, /* ITU-R BT.601 YUV4:2:2 */
	VI_MODE_DIGITAL_CAMERA, /* digatal camera mode */
	VI_MODE_BT1120_STANDARD, /* BT.1120 progressive mode */
	VI_MODE_BT1120_INTERLEAVED, /* BT.1120 interstage mode */
	VI_MODE_MIPI, /* MIPI RAW mode */
	VI_MODE_MIPI_YUV420_NORMAL, /* MIPI YUV420 normal mode */
	VI_MODE_MIPI_YUV420_LEGACY, /* MIPI YUV420 legacy mode */
	VI_MODE_MIPI_YUV422, /* MIPI YUV422 mode */
	VI_MODE_LVDS, /* LVDS mode */
	VI_MODE_HISPI, /* HiSPi mode */
	VI_MODE_SLVS, /* SLVS mode */

	VI_MODE_BUTT
} VI_INTF_MODE_E;

/* Input mode */
typedef enum _VI_INPUT_MODE_E {
	VI_INPUT_MODE_BT656 = 0, /* ITU-R BT.656 YUV4:2:2 */
	VI_INPUT_MODE_BT601, /* ITU-R BT.601 YUV4:2:2 */
	VI_INPUT_MODE_DIGITAL_CAMERA, /* digatal camera mode */
	VI_INPUT_MODE_INTERLEAVED, /* interstage mode */
	VI_INPUT_MODE_MIPI, /* MIPI mode */
	VI_INPUT_MODE_LVDS, /* LVDS mode */
	VI_INPUT_MODE_HISPI, /* HiSPi mode */
	VI_INPUT_MODE_SLVS, /* SLVS mode */

	VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;

/* Work mode */
typedef enum _VI_WORK_MODE_E {
	VI_WORK_MODE_1Multiplex = 0, /* 1 Multiplex mode */
	VI_WORK_MODE_2Multiplex, /* 2 Multiplex mode */
	VI_WORK_MODE_3Multiplex, /* 3 Multiplex mode */
	VI_WORK_MODE_4Multiplex, /* 4 Multiplex mode */

	VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;

/* whether an input picture is interlaced or progressive */
typedef enum _VI_SCAN_MODE_E {
	VI_SCAN_INTERLACED = 0, /* interlaced mode */
	VI_SCAN_PROGRESSIVE, /* progressive mode */

	VI_SCAN_BUTT
} VI_SCAN_MODE_E;

/* Sequence of YUV data
 *
 * VI_DATA_SEQ_VUVU: The input sequence of the second component(only contains u and v) in BT.1120 mode is VUVU
 * VI_DATA_SEQ_UVUV: The input sequence of the second component(only contains u and v) in BT.1120 mode is UVUV
 */
typedef enum _VI_YUV_DATA_SEQ_E {
	VI_DATA_SEQ_VUVU = 0,
	VI_DATA_SEQ_UVUV,

	VI_DATA_SEQ_UYVY, /* The input sequence of YUV is UYVY */
	VI_DATA_SEQ_VYUY, /* The input sequence of YUV is VYUY */
	VI_DATA_SEQ_YUYV, /* The input sequence of YUV is YUYV */
	VI_DATA_SEQ_YVYU, /* The input sequence of YUV is YVYU */

	VI_DATA_SEQ_BUTT
} VI_YUV_DATA_SEQ_E;

/* Clock edge mode */
typedef enum _VI_CLK_EDGE_E {
	VI_CLK_EDGE_SINGLE_UP = 0, /* single-edge mode and in rising edge */
	VI_CLK_EDGE_SINGLE_DOWN, /* single-edge mode and in falling edge */

	VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

/* Component mode */
typedef enum _VI_COMPONENT_MODE_E {
	VI_COMPONENT_MODE_SINGLE = 0, /* single component mode */
	VI_COMPONENT_MODE_DOUBLE, /* double component mode */

	VI_COMPONENT_MODE_BUTT
} VI_COMPONENT_MODE_E;

/* Y/C composite or separation mode */
typedef enum _VI_COMBINE_MODE_E {
	VI_COMBINE_COMPOSITE = 0, /* Composite mode */
	VI_COMBINE_SEPARATE, /* Separate mode */

	VI_COMBINE_BUTT
} VI_COMBINE_MODE_E;

/* Attribute of the vertical synchronization signal */
typedef enum _VI_VSYNC_E {
	VI_VSYNC_FIELD = 0, /* Field/toggle mode:a signal reversal means a new frame or a field */
	VI_VSYNC_PULSE, /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */

	VI_VSYNC_BUTT
} VI_VSYNC_E;

/* Polarity of the vertical synchronization signal
 *
 * VI_VSYNC_NEG_HIGH: if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the v-sync signal of even field is high-level,
 *		      if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the v-sync pulse is positive pulse.
 * VI_VSYNC_NEG_LOW: if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the v-sync signal of even field is low-level,
 *		     if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the v-sync pulse is negative pulse.
 */
typedef enum _VI_VSYNC_NEG_E {
	VI_VSYNC_NEG_HIGH = 0,
	VI_VSYNC_NEG_LOW,
	VI_VSYNC_NEG_BUTT
} VI_VSYNC_NEG_E;

/* Attribute of the horizontal synchronization signal */
typedef enum _VI_HSYNC_E {
	VI_HSYNC_VALID_SINGNAL = 0, /* the h-sync is valid signal mode */
	VI_HSYNC_PULSE, /* the h-sync is pulse mode, a new pulse means the beginning of a new line */

	VI_HSYNC_BUTT
} VI_HSYNC_E;

/* Polarity of the horizontal synchronization signal
 *
 * VI_HSYNC_NEG_HIGH: if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid h-sync signal is high-level;
 *		    if VI_HSYNC_E = VI_HSYNC_PULSE,then the h-sync pulse is positive pulse.
 * VI_HSYNC_NEG_LOW: if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid h-sync signal is low-level;
 *		    if VI_HSYNC_E = VI_HSYNC_PULSE,then the h-sync pulse is negative pulse
 */
typedef enum _VI_HSYNC_NEG_E {
	VI_HSYNC_NEG_HIGH = 0,
	VI_HSYNC_NEG_LOW,
	VI_HSYNC_NEG_BUTT
} VI_HSYNC_NEG_E;

/* Attribute of the valid vertical synchronization signal
 *
 * VI_VSYNC_NORM_PULSE: the v-sync is pusle mode, a pusle means a new frame or field
 * VI_VSYNC_VALID_SIGNAL: the v-sync is effective mode, a effective signal means a new frame or field
 */
typedef enum _VI_VSYNC_VALID_E {
	VI_VSYNC_NORM_PULSE = 0,
	VI_VSYNC_VALID_SIGNAL,

	VI_VSYNC_VALID_BUTT
} VI_VSYNC_VALID_E;

/* Polarity of the valid vertical synchronization signal
 *
 * VI_VSYNC_VALID_NEG_HIGH: if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a positive pulse means v-sync pulse;
 *			    if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SIGNAL,the valid v-sync signal is high-level
 * VI_VSYNC_VALID_NEG_LOW: if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a negative pulse means v-sync pulse
 *			   if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SIGNAL,the valid v-sync signal is low-level
 */
typedef enum _VI_VSYNC_VALID_NEG_E {
	VI_VSYNC_VALID_NEG_HIGH = 0,
	VI_VSYNC_VALID_NEG_LOW,
	VI_VSYNC_VALID_NEG_BUTT
} VI_VSYNC_VALID_NEG_E;

typedef enum _VI_STATE {
	VI_RUNNING,
	VI_SUSPEND,
	VI_MAX,
} VI_STATE_E;

/* Blank information of the input timing
 *
 * u32VsyncVfb: RW;Vertical front blanking height of one frame or odd-field frame picture
 * u32VsyncVact: RW;Vertical effetive width of one frame or odd-field frame picture
 * u32VsyncVbb: RW;Vertical back blanking height of one frame or odd-field frame picture
 * u32VsyncVbfb: RW;Even-field vertical front blanking height when input mode is interlace
 *		(invalid when progressive input mode)
 * u32VsyncVbact: RW;Even-field vertical effetive width when input mode is interlace
 *		(invalid when progressive input mode)
 * u32VsyncVbbb: RW;Even-field vertical back blanking height when input mode is interlace
 *		(invalid when progressive input mode)
 */
typedef struct _VI_TIMING_BLANK_S {
	CVI_U32 u32HsyncHfb; /* RW;Horizontal front blanking width */
	CVI_U32 u32HsyncAct; /* RW;Horizontal effetive width */
	CVI_U32 u32HsyncHbb; /* RW;Horizontal back blanking width */
	CVI_U32 u32VsyncVfb;
	CVI_U32 u32VsyncVact;
	CVI_U32 u32VsyncVbb;
	CVI_U32 u32VsyncVbfb;
	CVI_U32 u32VsyncVbact;
	CVI_U32 u32VsyncVbbb;
} VI_TIMING_BLANK_S;

/* synchronization information about the BT.601 or DC timing */
typedef struct _VI_SYNC_CFG_S {
	VI_VSYNC_E enVsync;
	VI_VSYNC_NEG_E enVsyncNeg;
	VI_HSYNC_E enHsync;
	VI_HSYNC_NEG_E enHsyncNeg;
	VI_VSYNC_VALID_E enVsyncValid;
	VI_VSYNC_VALID_NEG_E enVsyncValidNeg;
	VI_TIMING_BLANK_S stTimingBlank;
} VI_SYNC_CFG_S;

/* the highest bit of the BT.656 timing reference code */
typedef enum _VI_BT656_FIXCODE_E {
	VI_BT656_FIXCODE_1 = 0, /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 1. */
	VI_BT656_FIXCODE_0, /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 0. */

	VI_BT656_FIXCODE_BUTT
} VI_BT656_FIXCODE_E;

/* Polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum _VI_BT656_FIELD_POLAR_E {
	VI_BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1 */
	VI_BT656_FIELD_POLAR_NSTD, /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0 */

	VI_BT656_FIELD_POLAR_BUTT
} VI_BT656_FIELD_POLAR_E;

/* synchronization information about the BT.656 */
typedef struct _VI_BT656_SYNC_CFG_S {
	VI_BT656_FIXCODE_E enFixCode;
	VI_BT656_FIELD_POLAR_E enFieldPolar;
} VI_BT656_SYNC_CFG_S;

/* Input data type */
typedef enum _VI_DATA_TYPE_E {
	VI_DATA_TYPE_YUV = 0,
	VI_DATA_TYPE_RGB,
	VI_DATA_TYPE_YUV_EARLY,

	VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

/* Attribute of wdr */
typedef struct _VI_WDR_ATTR_S {
	WDR_MODE_E enWDRMode; /* RW; WDR mode.*/
	CVI_U32 u32CacheLine; /* RW; WDR cache line.*/
	CVI_BOOL bSyntheticWDR; /* RW; Synthetic WDR mode.*/
} VI_WDR_ATTR_S;

/* the extended attributes of VI device
 *
 * enInputDataType: RW;RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable.
 */
typedef struct _VI_DEV_ATTR_EX_S {
	VI_INPUT_MODE_E enInputMode; /* RW;Input mode */
	VI_WORK_MODE_E enWorkMode; /* RW; Work mode */

	VI_COMBINE_MODE_E enCombineMode; /* RW;Y/C composite or separation mode */
	VI_COMPONENT_MODE_E enComponentMode; /* RW;Component mode (single-component or dual-component) */
	VI_CLK_EDGE_E enClkEdge; /* RW;Clock edge mode (sampling on the rising or falling edge) */

	CVI_U32 au32ComponentMask[VI_COMPMASK_NUM]; /* RW;Component mask */

	VI_SCAN_MODE_E enScanMode; /* RW;Input scanning mode (progressive or interlaced) */
	CVI_S32 as32AdChnId[VI_MAX_ADCHN_NUM]; /* RW;AD channel ID. Typically, the default value -1 is recommended */

	VI_YUV_DATA_SEQ_E enDataSeq; /* RW;Input data sequence (only the YUV format is supported) */
	VI_SYNC_CFG_S stSynCfg; /* RW;Sync timing. This member must be configured in BT.601 mode or DC mode */

	VI_BT656_SYNC_CFG_S stBT656SynCfg; /* RW;Sync timing. This member must be configured in BT.656 mode */

	VI_DATA_TYPE_E enInputDataType;

	CVI_BOOL bDataReverse; /* RW;Data reverse */

	SIZE_S stSize; /* RW;Input size */

	VI_WDR_ATTR_S stWDRAttr; /* RW;Attribute of WDR */
} VI_DEV_ATTR_EX_S;

/* The attributes of a VI device
 *
 * enInputDataType: RW;RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable.
 */
typedef struct _VI_DEV_ATTR_S {
	VI_INTF_MODE_E enIntfMode; /* RW;Interface mode */
	VI_WORK_MODE_E enWorkMode; /* RW;Work mode */

	VI_SCAN_MODE_E enScanMode; /* RW;Input scanning mode (progressive or interlaced) */
	CVI_S32 as32AdChnId[VI_MAX_ADCHN_NUM]; /* RW;AD channel ID. Typically, the default value -1 is recommended */

	/* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
	VI_YUV_DATA_SEQ_E enDataSeq; /* RW;Input data sequence (only the YUV format is supported) */
	VI_SYNC_CFG_S stSynCfg; /* RW;Sync timing. This member must be configured in BT.601 mode or DC mode */

	VI_DATA_TYPE_E enInputDataType;

	SIZE_S stSize; /* RW;Input size */

	VI_WDR_ATTR_S stWDRAttr; /* RW;Attribute of WDR */

	BAYER_FORMAT_E enBayerFormat; /* RW;Bayer format of Device */

	CVI_U32 chn_num; /* R; total chnannels sended from dev */

	CVI_U32 snrFps; /* R; snr init fps from isp pub attr */

	CVI_BOOL isMux; /* multi sensor use same dev*/

	CVI_U32 switchGpioPin; /*switch pin*/

	CVI_U8 switchGPioPol; /*switch pol*/
} VI_DEV_ATTR_S;

/* Information of pipe binded to device */
typedef struct _VI_DEV_BIND_PIPE_S {
	CVI_U32 u32Num; /* RW;Range [1,VI_MAX_PIPE_NUM] */
	VI_PIPE PipeId[VI_MAX_PIPE_NUM]; /* RW;Array of pipe ID */
} VI_DEV_BIND_PIPE_S;

/* Source of 3DNR reference frame */
typedef enum _VI_NR_REF_SOURCE_E {
	VI_NR_REF_FROM_RFR = 0, /* Reference frame from reconstruction frame */
	VI_NR_REF_FROM_CHN0, /* Reference frame from CHN0's frame */

	VI_NR_REF_FROM_BUTT
} VI_NR_REF_SOURCE_E;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _VI_PIPE_BYPASS_MODE_E {
	VI_PIPE_BYPASS_NONE,
	VI_PIPE_BYPASS_FE,
	VI_PIPE_BYPASS_BE,

	VI_PIPE_BYPASS_BUTT
} VI_PIPE_BYPASS_MODE_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

/* The attributes of 3DNR */
typedef struct _VI_NR_ATTR_S {
	PIXEL_FORMAT_E enPixFmt; /* RW;Pixel format of reference frame */
	DATA_BITWIDTH_E enBitWidth; /* RW;Bit Width of reference frame */
	VI_NR_REF_SOURCE_E enNrRefSource; /* RW;Source of 3DNR reference frame */
	COMPRESS_MODE_E enCompressMode; /* RW;Reference frame compress mode */
} VI_NR_ATTR_S;

/* The attributes of pipe
 *
 * bDiscardProPic: RW;when professional mode snap, whether to discard long exposure picture in the video pipe.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _VI_PIPE_ATTR_S {
	VI_PIPE_BYPASS_MODE_E enPipeBypassMode;
	CVI_BOOL bYuvSkip; /* RW;YUV skip enable */
	CVI_BOOL bIspBypass; /* RW;ISP bypass enable */
	CVI_U32 u32MaxW; /* RW;Range[VI_PIPE_MIN_WIDTH,VI_PIPE_MAX_WIDTH];Maximum width */
	CVI_U32 u32MaxH; /* RW;Range[VI_PIPE_MIN_HEIGHT,VI_PIPE_MAX_HEIGHT];Maximum height */
	PIXEL_FORMAT_E enPixFmt; /* RW;Pixel format */
	COMPRESS_MODE_E enCompressMode; /* RW;Compress mode.*/
	DATA_BITWIDTH_E enBitWidth; /* RW;Bit width*/
	CVI_BOOL bNrEn; /* RW;3DNR enable */
	CVI_BOOL bSharpenEn; /* RW;Sharpen enable*/
	FRAME_RATE_CTRL_S stFrameRate; /* RW;Frame rate */
	CVI_BOOL bDiscardProPic;
	CVI_BOOL bYuvBypassPath; /* RW;ISP YUV bypass enable */
} VI_PIPE_ATTR_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

/*
 * au16TextureStr: RW; range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement.
 * au16EdgeStr: RW; range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement.
 * u16TextureFreq: RW; range: [0, 4095]; Format:6.6; Texture frequency adjustment.
 *		   Texture and detail will be finer when it increase.
 * u16EdgeFreq: RW; range: [0, 4095]; Format:6.6; Edge frequency adjustment.
 *		Edge will be narrower and thiner when it increase.
 * u8ShootSupStr: RW; range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength,
 *		  the amplitude and width of shoot will be decrease when shootSupSt increase.
 */
typedef struct _VI_PIPE_SHARPEN_MANUAL_ATTR_S {
	CVI_U16 au16TextureStr[VI_SHARPEN_GAIN_NUM];
	CVI_U16 au16EdgeStr[VI_SHARPEN_GAIN_NUM];
	CVI_U16 u16TextureFreq;
	CVI_U16 u16EdgeFreq;
	CVI_U8 u8OverShoot; /* RW; range: [0, 127];  Format:7.0;u8OvershootAmt*/
	CVI_U8 u8UnderShoot; /* RW; range: [0, 127];  Format:7.0;u8UndershootAmt*/
	CVI_U8 u8ShootSupStr;

} VI_PIPE_SHARPEN_MANUAL_ATTR_S;

/*
 * au16TextureStr: RW; range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement.
 * au16EdgeStr:  RW; range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement
 * au16TextureFreq: RW; range: [0, 4095]; Format:6.6;Texture frequency adjustment.
 *		    Texture and detail will be finer when it increase
 * au16EdgeFreq: RW; range: [0, 4095]; Format:6.6;Edge frequency adjustment.
 *		 Edge will be narrower and thiner when it increase
 * au8ShootSupStr: RW; range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength,
 *		   the amplitude and width of shoot will be decrease when shootSupSt increase
 */
typedef struct _VI_PIPE_SHARPEN_AUTO_ATTR_S {
	CVI_U16 au16TextureStr[VI_SHARPEN_GAIN_NUM][VI_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16EdgeStr[VI_SHARPEN_GAIN_NUM][VI_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16TextureFreq[VI_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16EdgeFreq[VI_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 au8OverShoot[VI_AUTO_ISO_STRENGTH_NUM]; /* RW; range: [0, 127];  Format:7.0;u8OvershootAmt*/
	CVI_U8 au8UnderShoot[VI_AUTO_ISO_STRENGTH_NUM]; /* RW; range: [0, 127];  Format:7.0;u8UndershootAmt*/
	CVI_U8 au8ShootSupStr[VI_AUTO_ISO_STRENGTH_NUM];

} VI_PIPE_SHARPEN_AUTO_ATTR_S;

typedef struct _VI_PIPE_SHARPEN_ATTR_S {
	OPERATION_MODE_E enOpType;
	CVI_U8 au8LumaWgt[VI_SHARPEN_GAIN_NUM]; /* RW; range: [0, 127];  Format:7.0;*/
	VI_PIPE_SHARPEN_MANUAL_ATTR_S stSharpenManualAttr;
	VI_PIPE_SHARPEN_AUTO_ATTR_S stSharpenAutoAttr;
} VI_PIPE_SHARPEN_ATTR_S;

typedef enum _VI_PIPE_REPEAT_MODE_E {
	VI_PIPE_REPEAT_NONE = 0,
	VI_PIPE_REPEAT_ONCE = 1,
	VI_PIPE_REPEAT_BUTT
} VI_PIPE_REPEAT_MODE_E;

/*
 * IES: RW; Range:[0,255];Format 8.0;the absolute strength of image enhancement for edge
 * IESS:RW; Range:[0,255];Format 8.0;the absolute strength of image enhancement for texture and shadow
 * IEDZ: RW; Range:[0,8192];Format 14.0;the threshold of image enhancement for controlling noise
 */
typedef struct {
	CVI_U8 IES;
	CVI_U8 IESS;
	CVI_U16 IEDZ;
} tV59aIEy;

/*
 * SBF: RW; Range:[0,3];Format 2.0;the band type of spatial filter,
 *	notice: SBF0,SBF1 range is [2,3];SBF2,SBF3 range is [0,3]
 * STR: RW; Range:[0,13];Format 4.0;the relative strength of spatial filter refer to the previous frame
 * STHp: RW; Range:[0,2];Format 2.0;Not recommended for debugging
 * SFT: RW; Range:[0,31];Format 5.0;Not recommended for debugging
 * kPro: RW; Range:[0,7];Format 3.0;notice: the kPro of SFy2 range is [0,7],the kPro of SFy3 range is [0,4]
 * STH: RW; Range:[0,999];Format 10.0;the edge-preserve threshold for spatial filter
 * SBS: RW; Range:[0,9999];Format 14.0;the noise reduction strength of spatial filter for the relative bright pixel
 * SDS: RW; Range:[0,9999];Format 14.0;the noise reduction strength of spatial filter for the relative dark pixel
 */
typedef struct {
	CVI_U8 SBF : 2;
	CVI_U8 STR : 4;
	CVI_U8 STHp : 2;
	CVI_U8 SFT : 5;
	CVI_U8 kPro : 3;

	CVI_U16 STH[3];
	CVI_U16 SBS[3];
	CVI_U16 SDS[3];

} tV59aSFy;

typedef struct {
	CVI_U16 MATH : 10; /* RW; Range:[0,1023];Format 10.0;the motion detection threshold for temporal filter */
	CVI_U16 MATE : 4; /* RW; Range:[0,11];Format 4.0;the motion detection index of flat area for temporal filter */
	CVI_U16 MATW : 2; /* RW; Range:[0,3];Format 2.0;the index of suppressing trailing for temporal filter */
	CVI_U8 MASW : 4; /* RW; Range:[0,12];Format 4.0;the index of suppressing raindrop noise for temporal filter */
	CVI_U8 MABW : 3; /* RW; Range:[0,4];Format 3.0;the window of motion detection for temporal filter */
	CVI_U8 MAXN : 1; /* RW; Range:[0,1];Format 1.0;Not recommended for debugging */

} tV59aMDy;

typedef struct {
	CVI_U8 TFR[4]; /* RW; Range:[0,255];Format 8.0;the relative strength of temporal filter for the static area */
	CVI_U16 TDZ : 14; /* RW; Range:[0,999];Format 10.0;the threshold of dead-area of temporal filter */
	CVI_U16 TDX : 2; /* RW; Range:[0,2];Format 2.0;Not recommended for debugging */
	CVI_U16 TFS : 6; /* RW; Range:[0,63];Format 6.0;the absolute strength of temporal filter  */
} tV59aTFy;

typedef struct {
	CVI_U16 SFC : 10; /* RW; Range:[0,1023];Format 10.0;the strength of spatial filter for NRC0 */
	CVI_U16 TFC : 6; /* RW; Range:[0,63];Format 6.0;the strength of temporal filter for NRC0 */
	CVI_U16 CSFS : 14; /* RW; Range:[0,999];Format 10.0;the strength of spatial filter for NRC1 */
	CVI_U16 CSFk : 2; /* RW; Range:[0,3];Format 2.0;Not recommended for debugging */
	CVI_U16 CTFS : 4; /* RW; Range:[0,15];Format 4.0;the strength of temporal filter for NRC1 */
	CVI_U16 CIIR : 1; /* RW; Range:[0,1];Format 1.0;the mode of spatial filter for NRC1 */
	CVI_U16 CTFR : 11; /* RW; Range:[0,999];Format 10.0;the relative strength of temporal filter for NRC1 */

} tV59aNRc;

/* 3DNR Spatial Filter: SFy0,SFy1,SFy2,SFy3; Temporal Filter:TFy0,TFy1;Chroma Noise Reduction: NRC0,NRC1
 *
 * HdgType: RW; Range:[0,1];Format 1.0;the type of complexed mixed spatial filter whether is SFi or SFk
 * BriType: RW; Range:[0,1];Format 1.0;the mode decide SFy3 whether is SFk type or SFi type
 * HdgMode: RW; Range:[0,3];Format 2.0;the mode decide complexed mixed spatial filter band for flat area
 * kTab2: RW; Range:[0,1];Format 1.0;the parameter decide SFy2 whether or not based on the image absolute luminance
 * HdgWnd: RW; Range:[0,1];Format 1.0;the sampling window of complexed mixed spatial filter for noise detection
 * kTab3: RW; Range:[0,1];Format 1.0;the parameter decide SFy3 whether or not based on the image absolute luminance
 * HdgSFR: RW; Range:[0,13];Format 4.0;the trend of the noise reduction of complexed mixed spatial filter for flat area
 * nOut: RW; Range:[0,27];Format 5.0;the parameter for output intermediate result of SFy3
 * HdgIES: RW; Range:[0,255];Format 8.0;the strength of image enhancement for complexed mixed spatial filter
 * nRef: RW; Range:[0,1];Format 1.0;Not recommended for debugging
 *
 * SFRi: RW; Range:[0,255];Format 8.0;the relative strength of SFy3 when the filter type is SFi
 * SFRk: RW; Range:[0,255];Format 8.0;the relative strength of SFy3 when the filter type is SFk
 * SBSk2: RW; Range:[0,9999];Format 14.0;the noise reduction strength of SFy2 for the relative bright pixel
 *	  based on the image absolute luminance
 * SBSk3: RW; Range:[0,9999];Format 14.0;the noise reduction strength of SFy3 for the relative bright pixel
 *	  based on the image absolute luminance
 * SDSk2: RW; Range:[0,9999];Format 14.0;the noise reduction strength of SFy2 for the relative dark pixel
 *	  based on the image absolute luminance
 * SDSk3: RW; Range:[0,9999];Format 14.0;the noise reduction strength of SFy3 for the relative dark pixel
 *	  based on the image absolute luminance
 * BriThr: RW; Range:[0,1024];Format 11.0;the threshold decide SFy3 choose the SFi type filter or SFk type filter
 *	  in dark and bright area
 *
 */
typedef struct {
	tV59aIEy IEy;
	tV59aSFy SFy[5];
	tV59aMDy MDy[2];
	tV59aTFy TFy[2];

	CVI_U16 HdgType : 1;
	CVI_U16 BriType : 1;
	CVI_U16 HdgMode : 2;
	CVI_U16 kTab2 : 1;
	CVI_U16 HdgWnd : 1;
	CVI_U16 kTab3 : 1;
	CVI_U16 HdgSFR : 4;
	CVI_U16 nOut : 5;
	CVI_U8 HdgIES;
	CVI_U8 nRef : 1;

	CVI_U8 IEyMode : 1;
	CVI_U8 IEyEx[4];

	CVI_U8 SFRi[4];
	CVI_U8 SFRk[4];
	CVI_U16 SBSk2[32];
	CVI_U16 SBSk3[32];
	CVI_U16 SDSk2[32];
	CVI_U16 SDSk3[32];
	CVI_U16 BriThr[16];

	tV59aNRc NRc;
} VI_PIPE_NRX_PARAM_V1_S;

typedef enum _VI_NR_VERSION_E { VI_NR_V1 = 1, VI_NR_V2 = 2, VI_NR_V3 = 3, VI_NR_V4 = 4, VI_NR_BUTT } VI_NR_VERSION_E;

typedef struct _NRX_PARAM_MANUAL_V1_S {
	VI_PIPE_NRX_PARAM_V1_S stNRXParamV1;
} NRX_PARAM_MANUAL_V1_S;

typedef struct _NRX_PARAM_AUTO_V1_S {
	CVI_U32 u32ParamNum;

	CVI_U32 *ATTRIBUTE pau32ISO;
	VI_PIPE_NRX_PARAM_V1_S *ATTRIBUTE pastNRXParamV1;
} NRX_PARAM_AUTO_V1_S;

typedef struct _NRX_PARAM_V1_S {
	OPERATION_MODE_E enOptMode; /* RW;Adaptive NR */
	NRX_PARAM_MANUAL_V1_S stNRXManualV1; /* RW;NRX V1 param for manual */
	NRX_PARAM_AUTO_V1_S stNRXAutoV1; /* RW;NRX V1 param for auto */
} NRX_PARAM_V1_S;

typedef struct {
	CVI_U8 IES0, IES1, IES2, IES3;
	CVI_U16 IEDZ : 10, _rb_ : 6;
} tV500_VI_IEy;

typedef struct {
	CVI_U8 SPN6 : 3, SFR : 5;
	CVI_U8 SBN6 : 3, PBR6 : 5;
	CVI_U16 SRT0 : 5, SRT1 : 5, JMODE : 3, DeIdx : 3;
	CVI_U8 DeRate, SFR6[3];

	CVI_U8 SFS1, SFT1, SBR1;
	CVI_U8 SFS2, SFT2, SBR2;
	CVI_U8 SFS4, SFT4, SBR4;

	CVI_U16 STH1 : 9, SFN1 : 3, NRyEn : 1, SFN0 : 3;
	CVI_U16 STH2 : 9, SFN2 : 3, BWSF4 : 1, kMode : 3;
	CVI_U16 STH3 : 9, SFN3 : 3, TriTh : 1, _rb0_ : 3;
} tV500_VI_SFy;

typedef struct {
	tV500_VI_IEy IEy;
	tV500_VI_SFy SFy;
} VI_PIPE_NRX_PARAM_V2_S;

typedef struct _NRX_PARAM_MANUAL_V2_S {
	VI_PIPE_NRX_PARAM_V2_S stNRXParamV2;
} NRX_PARAM_MANUAL_V2_S;

typedef struct _NRX_PARAM_AUTO_V2_S {
	CVI_U32 u32ParamNum;

	CVI_U32 *ATTRIBUTE pau32ISO;
	VI_PIPE_NRX_PARAM_V2_S *ATTRIBUTE pastNRXParamV2;

} NRX_PARAM_AUTO_V2_S;

typedef struct _NRX_PARAM_V2_S {
	OPERATION_MODE_E enOptMode; /* RW;Adaptive NR */
	NRX_PARAM_MANUAL_V2_S stNRXManualV2; /* RW;NRX V2 param for manual */
	NRX_PARAM_AUTO_V2_S stNRXAutoV2; /* RW;NRX V2 param for auto */
} NRX_PARAM_V2_S;

typedef struct _VI_PIPE_NRX_PARAM_S {
	VI_NR_VERSION_E enNRVersion; /* RW;3DNR Version*/
	union {
		NRX_PARAM_V1_S stNRXParamV1; /* RW;3DNR X param version 1 */
		NRX_PARAM_V2_S stNRXParamV2; /* RW;3DNR X param version 2 */
	};
} VI_PIPE_NRX_PARAM_S;

/* The attributes of channel */
typedef struct _VI_CHN_ATTR_S {
	SIZE_S stSize; /* RW;Channel out put size */
	PIXEL_FORMAT_E enPixelFormat; /* RW;Pixel format */
	DYNAMIC_RANGE_E enDynamicRange; /* RW;Dynamic Range */
	VIDEO_FORMAT_E enVideoFormat; /* RW;Video format */
	COMPRESS_MODE_E enCompressMode; /* RW;256B Segment compress or no compress. */
	CVI_BOOL bMirror; /* RW;Mirror enable */
	CVI_BOOL bFlip; /* RW;Flip enable */
	CVI_U32 u32Depth; /* RW;Range [0,8];Depth */
	FRAME_RATE_CTRL_S stFrameRate; /* RW;Frame rate */
	CVI_U32 u32BindVbPool; /*chn bind vb*/
} VI_CHN_ATTR_S;

/* The status of pipe */
typedef struct _VI_PIPE_STATUS_S {
	CVI_BOOL bEnable; /* RO;Whether this pipe is enabled */
	CVI_U32 u32IntCnt; /* RO;The video frame interrupt count */
	CVI_U32 u32FrameRate; /* RO;Current frame rate */
	CVI_U32 u32LostFrame; /* RO;Lost frame count */
	CVI_U32 u32VbFail; /* RO;Video buffer malloc failure */
	SIZE_S stSize; /* RO;Current pipe output size */
} VI_PIPE_STATUS_S;

/* VS signal output mode */
typedef enum _VI_VS_SIGNAL_MODE_E {
	VI_VS_SIGNAL_ONCE = 0, /* output one time */
	VI_VS_SIGNAL_FREQ, /* output frequently */

	VI_VS_SIGNAL_MODE_BUTT
} VI_VS_SIGNAL_MODE_E;

/* The attributes of VS signal */
typedef struct _VI_VS_SIGNAL_ATTR_S {
	VI_VS_SIGNAL_MODE_E enMode; /* RW;output one time, output frequently*/
	CVI_U32 u32StartTime; /* RW;output start time,unit: sensor pix clk.*/
	CVI_U32 u32Duration; /* RW;output high duration, unit: sensor pix clk.*/
	CVI_U32 u32CapFrmIndex; /* RW;VS signal will be output after trigger by which vframe, default is 0. */
	CVI_U32 u32Interval; /* RW;output frequently interval, unit: frame*/
} VI_VS_SIGNAL_ATTR_S;

typedef enum _VI_EXT_CHN_SOURCE_E {
	VI_EXT_CHN_SOURCE_TAIL,
	VI_EXT_CHN_SOURCE_HEAD,

	VI_EXT_CHN_SOURCE_BUTT
} VI_EXT_CHN_SOURCE_E;

typedef struct _VI_EXT_CHN_ATTR_S {
	VI_EXT_CHN_SOURCE_E enSource;
	VI_CHN s32BindChn; /* RW;Range [VI_CHN0, VI_MAX_PHY_CHN_NUM);The channel num which extend channel will bind to*/
	SIZE_S stSize; /* RW;Channel out put size */
	PIXEL_FORMAT_E enPixelFormat; /* RW;Pixel format */
	CVI_U32 u32Depth; /* RW;Range [0,8];Depth */
	FRAME_RATE_CTRL_S stFrameRate; /* RW;Frame rate */
} VI_EXT_CHN_ATTR_S;

typedef enum _VI_CROP_COORDINATE_E {
	VI_CROP_RATIO_COOR = 0, /* Ratio coordinate */
	VI_CROP_ABS_COOR, /* Absolute coordinate */
	VI_CROP_BUTT
} VI_CROP_COORDINATE_E;

/* Information of chn crop */
typedef struct _VI_CROP_INFO_S {
	CVI_BOOL bEnable; /* RW;CROP enable*/
	VI_CROP_COORDINATE_E enCropCoordinate; /* RW;Coordinate mode of the crop start point*/
	RECT_S stCropRect; /* RW;CROP rectangular*/
} VI_CROP_INFO_S;

/* The attributes of LDC */
typedef struct _VI_LDC_ATTR_S {
	CVI_BOOL bEnable; /* RW;Whether LDC is enbale */
	LDC_ATTR_S stAttr;
} VI_LDC_ATTR_S;

/* The status of chn */
typedef struct _VI_CHN_STATUS_S {
	CVI_BOOL bEnable; /* RO;Whether this channel is enabled */
	CVI_U32 u32FrameRate; /* RO;current frame rate */
	CVI_U64 u64PrevTime; // latest time (us)
	CVI_U32 u32FrameNum;  //The number of Frame in one second
	CVI_U32 u32LostFrame; /* RO;Lost frame count */
	CVI_U32 u32VbFail; /* RO;Video buffer malloc failure */
	CVI_U32 u32IntCnt; /* RO;Receive frame int count */
	CVI_U32 u32RecvPic; /* RO;Receive frame count */
	CVI_U32 u32TotalMemByte; /* RO;VI buffer malloc failure */
	SIZE_S stSize; /* RO;chn output size */

} VI_CHN_STATUS_S;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _VI_DUMP_TYPE_E {
	VI_DUMP_TYPE_RAW = 0,
	VI_DUMP_TYPE_YUV = 1,
	VI_DUMP_TYPE_IR = 2,
	VI_DUMP_TYPE_BUTT
} VI_DUMP_TYPE_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _VI_DUMP_ATTR_S {
	CVI_BOOL bEnable; /* RW;Whether dump is enable */
	CVI_U32 u32Depth; /* RW;Range [0,8];Depth */
	VI_DUMP_TYPE_E enDumpType;
} VI_DUMP_ATTR_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

typedef enum _VI_PIPE_FRAME_SOURCE_E {
	VI_PIPE_FRAME_SOURCE_DEV = 0, /* RW;Source from dev */
	VI_PIPE_FRAME_SOURCE_USER_FE, /* RW;User send to FE */
	VI_PIPE_FRAME_SOURCE_USER_BE, /* RW;User send to BE */

	VI_PIPE_FRAME_SOURCE_BUTT
} VI_PIPE_FRAME_SOURCE_E;

typedef struct _VI_RAW_INFO_S {
	VIDEO_FRAME_INFO_S stVideoFrame;
	ISP_CONFIG_INFO_S stIspInfo;
} VI_RAW_INFO_S;

/* module params */
typedef struct _VI_MOD_PARAM_S {
	CVI_S32 s32DetectErrFrame;
	CVI_U32 u32DropErrFrame;
} VI_MOD_PARAM_S;

typedef struct _VI_DEV_TIMING_ATTR_S {
	CVI_BOOL bEnable; /* RW;Whether enable VI generate timing */
	CVI_S32 s32FrmRate; /* RW;Generate timing Frame rate*/
} VI_DEV_TIMING_ATTR_S;

typedef struct _VI_EARLY_INTERRUPT_S {
	CVI_BOOL bEnable;
	CVI_U32 u32LineCnt;
} VI_EARLY_INTERRUPT_S;

/* VI dump register table */
typedef struct _MLSC_GAIN_LUT_S {
	CVI_U16 *RGain;
	CVI_U16 *GGain;
	CVI_U16 *BGain;
} MLSC_GAIN_LUT_S;

typedef struct _VI_DUMP_REGISTER_TABLE_S {
	MLSC_GAIN_LUT_S MlscGainLut;
} VI_DUMP_REGISTER_TABLE_S;

typedef int (*pfnViDevPmOps)(void *pvData);

typedef struct _VI_PM_OPS_S {
	pfnViDevPmOps pfnSnsSuspend;
	pfnViDevPmOps pfnSnsResume;
	pfnViDevPmOps pfnMipiSuspend;
	pfnViDevPmOps pfnMipiResume;
} VI_PM_OPS_S;

typedef struct _VI_SMOOTH_RAW_DUMP_INFO_S {
	VI_PIPE ViPipe;
	CVI_U8  u8BlkCnt;	// ring buffer number
	CVI_U64 *phy_addr_list;	// ring buffer addr
	RECT_S  stCropRect;
} VI_SMOOTH_RAW_DUMP_INFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__CVI_COMM_VIDEO_IN_H__ */
