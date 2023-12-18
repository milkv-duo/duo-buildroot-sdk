/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2022. All rights reserved.
 *
 * File Name: include/cvi_common_isp.h
 * Description:
 */

#ifndef __CVI_COMM_ISP_H__
#define __CVI_COMM_ISP_H__

#include <stdint.h>
#include "cvi_comm_inc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CCM_MATRIX_SIZE (9)
#define CSC_MATRIX_SIZE (9)
#define CSC_OFFSET_SIZE (3)
#define AE_MAX_ZONE_ROW (30)
#define AE_MAX_ZONE_COLUMN (34)
#define AE_ZONE_ROW (30)
#define AE_ZONE_COLUMN (34)
#define AE_ZONE_NUM (AE_ZONE_ROW * AE_ZONE_COLUMN)
#define AE_MAX_NUM (1)
#define AWB_ZONE_ORIG_ROW (30)
#define AWB_ZONE_ORIG_COLUMN (34)
#define AWB_ZONE_MAX_COLUMN (34)
#define AWB_ZONE_NUM (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN)
#define AWB_MAX_NUM (1)
#define HIST_BIN_DEPTH (8)
#define MAX_HIST_BINS (1 << HIST_BIN_DEPTH)
#define MAX_EXPOSURE_RATIO (256)
#define MAX_DRC_HIST_BINS (MAX_HIST_BINS * MAX_EXPOSURE_RATIO)
#define DCF_DRSCRIPTION_LENGTH (32)
#define DCF_CAPTURE_TIME_LENGTH (20)
#define ISP_AUTO_ISO_STRENGTH_NUM (16)
#define ISP_AUTO_EXP_RATIO_NUM (16)
#define ISP_AUTO_COLORTEMP_NUM (7)
#define CVI_ISP_LSC_GRID_COL (37)
#define CVI_ISP_LSC_GRID_ROW (37)
#define CVI_ISP_LSC_GRID_POINTS (CVI_ISP_LSC_GRID_COL * CVI_ISP_LSC_GRID_ROW)
#define DCI_BINS_NUM (256)
#define ISP_MAX_SNS_REGS 32
#define ISP_MAX_WDR_FRAME_NUM 2
#define BAYER_PATTERN_NUM 4
#define MG_ZONE_ROW (15)
#define MG_ZONE_COLUMN (17)
#define AF_ZONE_ROW (15)
#define AF_ZONE_COLUMN (17)
#define AF_XOFFSET_MIN (8)
#define AF_YOFFSET_MIN (2)
#define MAX_AWB_LIB_NUM (VI_MAX_PIPE_NUM)
#define MAX_AE_LIB_NUM (VI_MAX_PIPE_NUM)
#define LTM_DARK_CURVE_NODE_NUM (257)
#define LTM_BRIGHT_CURVE_NODE_NUM (513)
#define LTM_GLOBAL_CURVE_NODE_NUM (769)
#define ISP_3ALIB_FIND_FAIL (-1)
#define SHARPEN_LUT_NUM 33
#define MOTION_NR_TABLE_NUM 16
#define HIST_NUM 256
#define ISP_BASE_ADDR 0x0A000000
#define ISP_REG_RANGE 0x80000
#define DEHAZE_LUT_NUM 32
#define DIS_MAX_WINDOW_X_NUM 3
#define DIS_MAX_WINDOW_Y_NUM 3
#define XHIST_LENGTH			255		// 1024 bin
#define YHIST_LENGTH			255		// 512 bin

#define MIN_LV	(-5)
#define MAX_LV	(15)
#define LV_TOTAL_NUM	(MAX_LV - MIN_LV + 1)
#define AE_LV_FACTOR	(100)
#define ISP_AUTO_LV_NUM (LV_TOTAL_NUM)

/*Defines the format of the input Bayer image*/
typedef enum _ISP_BAYER_FORMAT_E {
	BAYER_BGGR,
	BAYER_GBRG,
	BAYER_GRBG,
	BAYER_RGGB,
	//for RGBIR sensor
	BAYER_GRGBI = 8,
	BAYER_RGBGI,
	BAYER_GBGRI,
	BAYER_BGRGI,
	BAYER_IGRGB,
	BAYER_IRGBG,
	BAYER_IBGRG,
	BAYER_IGBGR,
	BAYER_BUTT
} ISP_BAYER_FORMAT_E;

typedef enum _ISP_OP_TYPE_E {
	OP_TYPE_AUTO,
	OP_TYPE_MANUAL,
	OP_TYPE_BUTT
} ISP_OP_TYPE_E;

typedef enum _ISP_FMW_STATE_E {
	ISP_FMW_STATE_RUN,
	ISP_FMW_STATE_FREEZE,
	ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef enum _ISP_CHANNEL_LIST_E {
	ISP_CHANNEL_LE,
	ISP_CHANNEL_SE,
	ISP_CHANNEL_MAX_NUM,
} ISP_CHANNEL_LIST_E;

typedef enum _ISP_BAYER_CHANNEL_E {
	ISP_BAYER_CHN_R,
	ISP_BAYER_CHN_GR,
	ISP_BAYER_CHN_GB,
	ISP_BAYER_CHN_B,
	ISP_BAYER_CHN_NUM,
} ISP_BAYER_CHANNEL_E;

typedef enum _ISP_SNS_TYPE_E {
	SNS_I2C_TYPE,
	SNS_SSP_TYPE,
	SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

typedef enum _ISP_WDR_FRAME_IDX_E {
	ISP_WDR_FRAME_IDX_1,
	ISP_WDR_FRAME_IDX_2,
	ISP_WDR_FRAME_IDX_3,
	ISP_WDR_FRAME_IDX_4,
	ISP_WDR_FRAME_IDX_SIZE
} ISP_WDR_FRAME_IDX_E;

typedef union _ISP_SNS_COMMBUS_U {
	CVI_S8 s8I2cDev;
	struct {
		CVI_S8 bit4SspDev : 4;
		CVI_S8 bit4SspCs : 4;
	} s8SspDev;
} ISP_SNS_COMMBUS_U;

typedef struct _ISP_I2C_DATA_S {
	CVI_BOOL bUpdate;
	CVI_BOOL bDropFrm;
	CVI_BOOL bvblankUpdate;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the sensor register*/
	CVI_U8 u8DropFrmNum; /*RW; Number of frame to drop*/
	CVI_U8 u8IntPos; /*RW;Position where the configuration of the sensor register takes effect */
	CVI_U8 u8DevAddr; /*RW;Sensor device address*/
	CVI_U32 u32RegAddr; /*RW;Sensor register address*/
	CVI_U32 u32AddrByteNum; /*RW;Bit width of the sensor register address*/
	CVI_U32 u32Data; /*RW;Sensor register data*/
	CVI_U32 u32DataByteNum; /*RW;Bit width of sensor register data*/
} ISP_I2C_DATA_S;

typedef struct _ISP_SSP_DATA_S {
	CVI_BOOL bUpdate;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the sensor register*/
	CVI_U8 u8IntPos; /*RW;Position where the configuration of the sensor register takes effect */
	CVI_U32 u32DevAddr; /*RW;Sensor device address*/
	CVI_U32 u32DevAddrByteNum; /*RW;Bit width of the sensor device address*/
	CVI_U32 u32RegAddr; /*RW;Sensor register address*/
	CVI_U32 u32RegAddrByteNum; /*RW;Bit width of the sensor register address*/
	CVI_U32 u32Data; /*RW;Sensor register data*/
	CVI_U32 u32DataByteNum; /*RW;Bit width of sensor register data*/
} ISP_SSP_DATA_S;

typedef struct _ISP_SNS_REGS_INFO_S {
	ISP_SNS_TYPE_E enSnsType;
	CVI_U32 u32RegNum;
	CVI_U8 u8Cfg2ValidDelayMax;
	ISP_SNS_COMMBUS_U unComBus;
	union {
		ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
		ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
	};

	struct {
		CVI_BOOL bUpdate;
		CVI_U8 u8DelayFrmNum;
		CVI_U32 u32SlaveVsTime; /* RW;time of vsync. Unit: inck clock cycle */
		CVI_U32 u32SlaveBindDev;
	} stSlvSync;

	CVI_BOOL bConfig;
	CVI_U8 use_snsr_sram;
	CVI_U8 need_update;
} ISP_SNS_REGS_INFO_S;

typedef enum _ISP_VD_TYPE_E {
	ISP_VD_FE_START = 0,
	ISP_VD_FE_END,
	ISP_VD_BE_END,
	ISP_VD_MAX
} ISP_VD_TYPE_E;

typedef struct _ISP_MANUAL_WDR_ATTR_S {
	CVI_S32 devno;
	CVI_S32 manual_en;
	CVI_S16 l2s_distance;
	CVI_S16 lsef_length;
	CVI_S32 discard_padding_lines;
	CVI_S32 update;
} ISP_MANUAL_WDR_ATTR_S;

typedef struct _ISP_SNS_CIF_INFO_S {
	ISP_MANUAL_WDR_ATTR_S wdr_manual;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the cif setting */
	CVI_U8 need_update;
} ISP_SNS_CIF_INFO_S;

typedef struct _ISP_WDR_SIZE_S {
	RECT_S stWndRect;
	SIZE_S stSnsSize;
	SIZE_S stMaxSize;
} ISP_WDR_SIZE_S;

typedef struct _ISP_SNS_ISP_INFO_S {
	CVI_U32 frm_num;
	ISP_WDR_SIZE_S img_size[ISP_MAX_WDR_FRAME_NUM];
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the isp setting */
	CVI_U8 need_update;
} ISP_SNS_ISP_INFO_S;

typedef struct _ISP_SNS_SYNC_INFO_S {
	ISP_SNS_REGS_INFO_S snsCfg;
	ISP_SNS_ISP_INFO_S ispCfg;
	ISP_SNS_CIF_INFO_S cifCfg;
} ISP_SNS_SYNC_INFO_S;

typedef struct _ISP_PUB_ATTR_S {
	RECT_S stWndRect;
	SIZE_S stSnsSize;
	CVI_FLOAT f32FrameRate;
	ISP_BAYER_FORMAT_E enBayer;
	WDR_MODE_E enWDRMode;
	CVI_U8 u8SnsMode;
} ISP_PUB_ATTR_S;

typedef struct _ISP_CTRL_PARAM_S {
	CVI_U32 u32AEStatIntvl;
	CVI_U32 u32AWBStatIntvl;
	CVI_U32 u32AFStatIntvl;
	CVI_U32 u32ProcParam;
	CVI_U32 u32ProcLevel;
	CVI_U32 u32UpdatePos;
	CVI_U32 u32IntTimeOut;
	CVI_U32 u32PwmNumber;
	CVI_U32 u32PortIntDelay;
} ISP_CTRL_PARAM_S;

typedef union _ISP_MODULE_CTRL_U {
	CVI_U64 u64Key;
	struct {
		CVI_U64 bitBypassBlc : 1;		/*RW:[0]*/
		CVI_U64 bitBypassRlsc : 1;		/*RW:[1]*/
		CVI_U64 bitBypassFpn : 1;		/*RW:[2]*/
		CVI_U64 bitBypassDpc : 1;		/*RW:[3]*/
		CVI_U64 bitBypassCrosstalk : 1;	/*RW:[4]*/
		CVI_U64 bitBypassWBGain : 1;	/*RW:[5]*/
		CVI_U64 bitBypassDis : 1;		/*RW:[6]*/
		CVI_U64 bitBypassBnr : 1;		/*RW:[7]*/
		CVI_U64 bitBypassDemosaic : 1;	/*RW:[8]*/
		CVI_U64 bitBypassRgbcac : 1;	/*RW:[9]*/
		CVI_U64 bitBypassLcac : 1;		/*RW:[10]*/
		CVI_U64 bitBypassMlsc : 1;		/*RW:[11]*/
		CVI_U64 bitBypassCcm : 1;		/*RW:[12]*/
		CVI_U64 bitBypassFusion : 1;	/*RW:[13]*/
		CVI_U64 bitBypassDrc : 1;		/*RW:[14]*/
		CVI_U64 bitBypassGamma : 1;		/*RW:[15]*/
		CVI_U64 bitBypassDehaze : 1;	/*RW:[16]*/
		CVI_U64 bitBypassClut : 1;		/*RW:[17]*/
		CVI_U64 bitBypassCsc : 1;		/*RW:[18]*/
		CVI_U64 bitBypassDci : 1;		/*RW:[19]*/
		CVI_U64 bitBypassLdci : 1;		/*RW:[20]*/
		CVI_U64 bitBypassCa : 1;		/*RW:[21]*/
		CVI_U64 bitBypassPreyee : 1;	/*RW:[22]*/
		CVI_U64 bitBypassMotion : 1;	/*RW:[23]*/
		CVI_U64 bitBypass3dnr : 1;		/*RW:[24]*/
		CVI_U64 bitBypassYnr : 1;		/*RW:[25]*/
		CVI_U64 bitBypassCnr : 1;		/*RW:[26]*/
		CVI_U64 bitBypassCac : 1;		/*RW:[27]*/
		CVI_U64 bitBypassCa2 : 1;		/*RW:[28]*/
		CVI_U64 bitBypassYee : 1;		/*RW:[29]*/
		CVI_U64 bitBypassYcontrast : 1;	/*RW:[30]*/
		CVI_U64 bitBypassMono : 1;		/*RW:[31]*/
		CVI_U64 bitRsv : 32;			/*H; [32:63] */
	};
} ISP_MODULE_CTRL_U;

typedef union _ISP_STATISTICS_CTRL_U {
	CVI_U64 u64Key;
	struct {
		CVI_U64 bit1FEAeGloStat : 1; /* [0] */
		CVI_U64 bit1FEAeLocStat : 1; /* [1] */
		CVI_U64 bit1AwbStat1 : 1; /* [2] Awb Stat1 means global awb data. */
		CVI_U64 bit1AwbStat2 : 1; /* [3] Awb Stat2 means local awb data. */
		CVI_U64 bit1FEAfStat : 1; /* [4] */
		CVI_U64 bit14Rsv : 59; /* [5:63] */
	};
} ISP_STATISTICS_CTRL_U;

typedef enum _ISP_AE_STAT_MODE_E {
	ISP_AE_MODE_BUTT,
} ISP_AE_STAT_MODE_E;

typedef enum _ISP_AE_SWITCH_E {
	ISP_AE_SWITCH_BUTT,
} ISP_AE_SWITCH_E;

typedef enum _ISP_AE_FOUR_PLANE_MODE_E {
	ISP_AE_FOUR_PLANE_MODE_BUTT,
} ISP_AE_FOUR_PLANE_MODE_E;

typedef enum _ISP_AE_HIST_SKIP_E {
	ISP_AE_HIST_SKIP_BUTT,
} ISP_AE_HIST_SKIP_E;

typedef enum _ISP_AE_HIST_OFFSET_X_E {
	ISP_AE_HIST_OFFSET_X_BUTT,
} ISP_AE_HIST_OFFSET_X_E;

typedef enum _ISP_AE_HIST_OFFSET_Y_E {
	ISP_AE_HIST_OFFSET_Y_BUTT,
} ISP_AE_HIST_OFFSET_Y_E;

typedef enum _ISP_AE_ANTIFLICKER_FREQUENCE_E {
	AE_FREQUENCE_60HZ = 0,
	AE_FREQUENCE_50HZ,
} ISP_AE_ANTIFLICKER_FREQUENCE_E;

typedef enum _ISP_AE_METER_MODE_E {
	AE_METER_MULTI = 0,
	AE_METER_AVERAGE,
	AE_METER_HIGHLIGHT_PRIORITY,
	AE_METER_FISHEYE,
} ISP_AE_METER_MODE_E;

typedef enum _ISP_AE_IR_CUT_FORCE_STATUS {
	AE_IR_CUT_FORCE_AUTO = 0,
	AE_IR_CUT_FORCE_ON,
	AE_IR_CUT_FORCE_OFF,
} ISP_AE_IR_CUT_FORCE_STATUS;

typedef enum _ISP_AE_GAIN_TYPE_E {
	AE_TYPE_GAIN = 0,
	AE_TYPE_ISO = 1,
	AE_TYPE_BUTT
} ISP_AE_GAIN_TYPE_E;

typedef struct _ISP_AE_HIST_CONFIG_S {
	ISP_AE_HIST_SKIP_E enHistSkipX;
	ISP_AE_HIST_SKIP_E enHistSkipY;
	ISP_AE_HIST_OFFSET_X_E enHistOffsetX;
	ISP_AE_HIST_OFFSET_Y_E enHistOffsetY;
} ISP_AE_HIST_CONFIG_S;

typedef struct _ISP_3AWIN_CONFIG_S {
	CVI_U16 winWidth;
	CVI_U16 winHeight;
	CVI_U16 winXOffset;
	CVI_U16 winYOffset;
	CVI_U8 winXNum;
	CVI_U8 winYNum;
} ISP_3AWIN_CONFIG_S;

typedef struct _ISP_AE_CROP_S {
	CVI_BOOL bEnable; /*RW; Range:[0x0,0x1]*/
	CVI_U16 u16X; /*RW; Range:[0x00,0x1FFF]*/
	CVI_U16 u16Y; /*RW; Range:[0x00,0x1FFF]*/
	CVI_U16 u16W; /*RW; Range:[0x00,0x1FFF]*/
	CVI_U16 u16H; /*RW; Range:[0x00,0x1FFF]*/
} ISP_AE_CROP_S;

typedef struct _ISP_AE_FACE_CROP_S {
	CVI_BOOL bEnable; /*RW; Range:[0x0,0x1]*/
	CVI_U16 u16X; /*RW; Range:[0x00,0x1FFF]*/
	CVI_U16 u16Y; /*RW; Range:[0x00,0x1FFF]*/
	CVI_U8 u16W; /*RW; Range:[0x00,0xFF]*/
	CVI_U8 u16H; /*RW; Range:[0x00,0xFF]*/
} ISP_AE_FACE_CROP_S;

#define AE_WEIGHT_ZONE_ROW	15
#define AE_WEIGHT_ZONE_COLUMN	17
#define FACE_WIN_NUM 4
typedef struct _ISP_AE_STATISTICS_CFG_S {
	CVI_BOOL bHisStatisticsEnable; /*RW; Range:[0x0,0x1]*/
	ISP_AE_CROP_S stCrop[AE_MAX_NUM];
	ISP_AE_FACE_CROP_S stFaceCrop[FACE_WIN_NUM];
	CVI_BOOL fast2A_ena; /*RW; Range:[0x0,0x1]*/
	CVI_U8 fast2A_ae_low; /*RW; Range:[0x0,0xFF]*/
	CVI_U8 fast2A_ae_high; /*RW; Range:[0x0,0xFF]*/
	CVI_U16 fast2A_awb_top; /*RW; Range:[0x0,0xFFF]*/
	CVI_U16 fast2A_awb_bot; /*RW; Range:[0x0,0xFFF]*/
	CVI_U16 over_exp_thr; /*RW; Range:[0x0,0x3FF]*/
	CVI_U8 au8Weight[AE_WEIGHT_ZONE_ROW][AE_WEIGHT_ZONE_COLUMN]; /*RW; Range:[0x0, 0xF]*/
} ISP_AE_STATISTICS_CFG_S;

//-----------------------------------------------------------------------------
//  IRIS
//-----------------------------------------------------------------------------
typedef enum _ISP_IRIS_TYPE_E {
	ISP_IRIS_DC_TYPE = 0,
	ISP_IRIS_P_TYPE,
	ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

typedef enum _ISP_IRIS_F_NO_E {
	ISP_IRIS_F_NO_32_0 = 0,
	ISP_IRIS_F_NO_22_0,
	ISP_IRIS_F_NO_16_0,
	ISP_IRIS_F_NO_11_0,
	ISP_IRIS_F_NO_8_0,
	ISP_IRIS_F_NO_5_6,
	ISP_IRIS_F_NO_4_0,
	ISP_IRIS_F_NO_2_8,
	ISP_IRIS_F_NO_2_0,
	ISP_IRIS_F_NO_1_4,
	ISP_IRIS_F_NO_1_0,
	ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

typedef struct _ISP_DCIRIS_ATTR_S {
	CVI_S32 s32Kp; /*RW; Range:[0x0, 0x186A0]*/
	CVI_S32 s32Ki; /*RW; Range:[0x0, 0x3E8]*/
	CVI_S32 s32Kd; /*RW; Range:[0x0, 0x186A0]*/
	CVI_U32 u32MinPwmDuty; /*RW; Range:[0x0, 0x3E8]*/
	CVI_U32 u32MaxPwmDuty; /*RW; Range:[0x0, 0x3E8]*/
	CVI_U32 u32OpenPwmDuty; /*RW; Range:[0x0, 0x3E8]*/
} ISP_DCIRIS_ATTR_S;

#define AI_MAX_STEP_FNO_NUM (1024)
typedef struct _ISP_PIRIS_ATTR_S {
	CVI_BOOL bStepFNOTableChange;
	CVI_BOOL bZeroIsMax;
	CVI_U16 u16TotalStep;
	CVI_U16 u16StepCount;
	CVI_U16 au16StepFNOTable[AI_MAX_STEP_FNO_NUM];
	ISP_IRIS_F_NO_E enMaxIrisFNOTarget;
	ISP_IRIS_F_NO_E enMinIrisFNOTarget;
	CVI_BOOL bFNOExValid;
	CVI_U32 u32MaxIrisFNOTarget;
	CVI_U32 u32MinIrisFNOTarget;
} ISP_PIRIS_ATTR_S;

typedef struct _ISP_MI_ATTR_S {
	CVI_U32 u32HoldValue; /*RW; Range:[0x0, 0x3E8]*/
	ISP_IRIS_F_NO_E enIrisFNO; /*RW; Range:[0x0, 0xA]*/
} ISP_MI_ATTR_S;

typedef enum _ISP_IRIS_STATUS_E {
	ISP_IRIS_KEEP  = 0,
	ISP_IRIS_OPEN  = 1,
	ISP_IRIS_CLOSE = 2,
	ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

typedef struct _ISP_IRIS_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_IRIS_TYPE_E enIrisType;
	ISP_IRIS_STATUS_E enIrisStatus;
	ISP_MI_ATTR_S stMIAttr;
} ISP_IRIS_ATTR_S;

//-----------------------------------------------------------------------------
//  AE Route
//-----------------------------------------------------------------------------
#define ISP_AE_ROUTE_MIN_NODES (1)
#define ISP_AE_ROUTE_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_NODE_S {
	CVI_U32 u32IntTime; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32SysGain; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	ISP_IRIS_F_NO_E enIrisFNO; /*RW; Range:[0x0, 0xA]*/
	CVI_U32 u32IrisFNOLin; /*RW; Range:[0x1, 0x400]*/
} ISP_AE_ROUTE_NODE_S;

typedef struct _ISP_AE_ROUTE_S {
	CVI_U32 u32TotalNum; /*RW; Range:[0x1, 0x10]*/
	ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MIN_NODES (1)
#define ISP_AE_ROUTE_EX_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_EX_NODE_S {
	CVI_U32 u32IntTime; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32Again; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32Dgain; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32IspDgain; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	ISP_IRIS_F_NO_E enIrisFNO; /*RW; Range:[0x0, 0xA]*/
	CVI_U32 u32IrisFNOLin; /*RW; Range:[0x0, 0x400]*/
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct _ISP_AE_ROUTE_EX_S {
	CVI_U32 u32TotalNum; /*RW; Range:[0x1, 0x10]*/
	ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

//-----------------------------------------------------------------------------
//  Exposure Info
//-----------------------------------------------------------------------------
typedef struct _ISP_EXP_INFO_S {
	CVI_U32 u32ExpTime; /*R;*/
	CVI_U32 u32ShortExpTime; /*R;*/
	CVI_U32 u32MedianExpTime; /*R;*/
	CVI_U32 u32LongExpTime; /*R;*/
	CVI_U32 u32AGain; /*R;*/
	CVI_U32 u32DGain; /*R;*/
	CVI_U32 u32ISPDGain; /*R;*/
	CVI_U32 u32Exposure; /*R;*/
	CVI_BOOL bExposureIsMAX; /*R;*/
	CVI_S16 s16HistError; /*R;*/
	CVI_U32 au32AE_Hist256Value[HIST_NUM]; /*R;*/
	CVI_U8 u8AveLum; /*R;*/
	CVI_U32 u32LinesPer500ms; /*R;*/
	CVI_U32 u32PirisFNO; /*R;*/
	CVI_U32 u32Fps; /*R;*/
	CVI_U32 u32ISO; /*R;*/
	CVI_U32 u32ISOCalibrate; /*R;*/
	CVI_U32 u32RefExpRatio; /*R;*/
	CVI_U32 u32FirstStableTime; /*R;*/
	ISP_AE_ROUTE_S stAERoute; /*R;*/
	ISP_AE_ROUTE_EX_S stAERouteEx; /*R;*/
	CVI_U8 u8WDRShortAveLuma; /*R;*/
	CVI_U32 u32WDRExpRatio; /*R;*/
	CVI_U8 u8LEFrameAvgLuma; /*R;*/
	CVI_U8 u8SEFrameAvgLuma; /*R;*/
	CVI_FLOAT fLightValue; /*R;*/
	CVI_U32 u32AGainSF; /*R;*/
	CVI_U32 u32DGainSF; /*R;*/
	CVI_U32 u32ISPDGainSF; /*R;*/
	CVI_U32 u32ISOSF; /*R;*/
	ISP_AE_ROUTE_S stAERouteSF; /*R;*/
	ISP_AE_ROUTE_EX_S stAERouteSFEx; /*R;*/
	CVI_BOOL bGainSepStatus; /*R;*/
} ISP_EXP_INFO_S;

#define SMART_MAX_NUM (3)
typedef struct _ISP_SMART_ROI_S {
	CVI_BOOL bEnable;
	CVI_BOOL bAvailable;
	CVI_U8 u8Luma;
	CVI_U8 u8Num;
	CVI_U16 u16PosX[SMART_MAX_NUM];
	CVI_U16 u16PosY[SMART_MAX_NUM];
	CVI_U16 u16Width[SMART_MAX_NUM];
	CVI_U16 u16Height[SMART_MAX_NUM];
	CVI_U16 u16FrameWidth;
	CVI_U16 u16FrameHeight;
} ISP_SMART_ROI_S;

#define SMART_CLASS_MAX (2)
typedef struct _ISP_SMART_INFO_S {
	ISP_SMART_ROI_S stROI[SMART_CLASS_MAX];
} ISP_SMART_INFO_S;

//-----------------------------------------------------------------------------
//  WDR Exposure Attr
//-----------------------------------------------------------------------------
#define WDR_EXP_RATIO_NUM (3)
typedef struct _ISP_WDR_EXPOSURE_ATTR_S {
	ISP_OP_TYPE_E enExpRatioType;
	CVI_U32 au32ExpRatio[WDR_EXP_RATIO_NUM]; /*RW; Range:[0x40, 0x4000]*/
	CVI_U32 u32ExpRatioMax; /*RW; Range:[0x40, 0x4000]*/
	CVI_U32 u32ExpRatioMin; /*RW; Range:[0x40, 0x4000]*/
	CVI_U16 u16Tolerance; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16Speed; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16RatioBias; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U8 u8SECompensation; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16SEHisThr; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16SEHisCntRatio1; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16SEHisCntRatio2; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16SEHis255CntThr1; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U16 u16SEHis255CntThr2; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U8 au8LEAdjustTargetMin[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 au8LEAdjustTargetMax[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 au8SEAdjustTargetMin[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 au8SEAdjustTargetMax[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 u8AdjustTargetDetectFrmNum; /*RW; Range:[0x0, 0xFF]*/
	CVI_U32 u32DiffPixelNum; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U16 u16LELowBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16LEHighBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16SELowBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16SEHighBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 au8FrameAvgLumaMin[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 au8FrameAvgLumaMax[LV_TOTAL_NUM]; /*RW; Range:[0x0, 0xFF]*/
} ISP_WDR_EXPOSURE_ATTR_S;

//-----------------------------------------------------------------------------
//  Smart Exposure Attr
//-----------------------------------------------------------------------------
typedef struct _ISP_SMART_EXPOSURE_ATTR_S {
	CVI_BOOL bEnable;
	CVI_BOOL bIRMode;
	ISP_OP_TYPE_E enSmartExpType;
	CVI_U8 u8LumaTarget; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16ExpCoef; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16ExpCoefMax; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16ExpCoefMin; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U8 u8SmartInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 u8SmartSpeed; /*RW; Range:[0x1, 0xFF]*/
	CVI_U16 u16SmartDelayNum; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 u8Weight; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 u8NarrowRatio; /*RW; Range:[0x0, 0x64]*/
} ISP_SMART_EXPOSURE_ATTR_S;

typedef enum _ISP_IR_STATUS_E {
	ISP_IR_STATUS_NORMAL = 0,
	ISP_IR_STATUS_IR = 1,
	ISP_IR_BUTT
} ISP_IR_STATUS_E;

typedef enum _ISP_IR_SWITCH_STATUS_E {
	ISP_IR_SWITCH_NONE = 0,
	ISP_IR_SWITCH_TO_NORMAL = 1,
	ISP_IR_SWITCH_TO_IR = 2,
	ISP_IR_SWITCH_BUTT
} ISP_IR_SWITCH_STATUS_E;

typedef struct _ISP_IR_AUTO_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U32 u32Normal2IrIsoThr;
	CVI_U32 u32Ir2NormalIsoThr;
	CVI_U32 u32RGMax;
	CVI_U32 u32RGMin;
	CVI_U32 u32BGMax;
	CVI_U32 u32BGMin;
	ISP_IR_STATUS_E enIrStatus;
	ISP_IR_SWITCH_STATUS_E enIrSwitch;
} ISP_IR_AUTO_ATTR_S;

typedef struct _ISP_AWB_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X; /*RW; Range:[0x0, 0x1000]*/
	CVI_U16 u16Y; /*RW; Range:[0x0, 0x1000]*/
	CVI_U16 u16W; /*RW; Range:[0x0, 0x1000]*/
	CVI_U16 u16H; /*RW; Range:[0x0, 0x1000]*/
} ISP_AWB_CROP_S;

typedef enum _ISP_AWB_SWITCH_E {
	ISP_AWB_AFTER_DG,
	ISP_AWB_AFTER_DRC,
	ISP_AWB_SWITCH_BUTT,
} ISP_AWB_SWITCH_E;

typedef enum _ISP_AWB_ALG_TYPE_E {
	AWB_ALG_LOWCOST,
	AWB_ALG_ADVANCE,
	AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

typedef enum _ISP_AWB_MULTI_LS_TYPE_E {
	AWB_MULTI_LS_SAT,
	AWB_MULTI_LS_CCM,
	AWB_MULTI_LS_BUTT
} ISP_AWB_MULTI_LS_TYPE_E;

typedef enum _ISP_AWB_INDOOR_OUTDOOR_STATUS_E {
	AWB_INDOOR_MODE,
	AWB_OUTDOOR_MODE,
	AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

typedef struct _ISP_WB_STATISTICS_CFG_S {
	ISP_AWB_SWITCH_E enAWBSwitch;
	CVI_U16 u16ZoneRow; /*RW; Range:[0x0, AWB_ZONE_ORIG_ROW]*/
	CVI_U16 u16ZoneCol; /*RW; Range:[0x0, AWB_ZONE_ORIG_COLUMN]*/
	CVI_U16 u16ZoneBin;
	CVI_U16 au16HistBinThresh[4];
	CVI_U16 u16WhiteLevel; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 u16BlackLevel; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 u16CbMax;
	CVI_U16 u16CbMin;
	CVI_U16 u16CrMax;
	CVI_U16 u16CrMin;
	ISP_AWB_CROP_S stCrop;
} ISP_WB_STATISTICS_CFG_S;

#define FIR_H_GAIN_NUM (5)
#define FIR_V_GAIN_NUM (3)
#define AF_GAMMA_NUM (256)
// AF window X region must between [0x8 ~ image xsize - 8].
// Y region must between [0x2 ~ image ysize - 2].
typedef struct _ISP_AF_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X; /*RW; Range:[0x8, 0xFFF]*/
	CVI_U16 u16Y; /*RW; Range:[0x2, 0xFFF]*/
	CVI_U16 u16W; /*RW; Range:[0x110, 0xFFF]*/
	CVI_U16 u16H; /*RW; Range:[0xF0, 0xFFF]*/
} ISP_AF_CROP_S;

typedef struct _ISP_AF_RAW_CFG_S {
	CVI_BOOL PreGammaEn;
	CVI_U8 PreGammaTable[AF_GAMMA_NUM]; /*RW; Range:[0x0, 0xFF]*/
} ISP_AF_RAW_CFG_S;

typedef struct _ISP_AF_PRE_FILTER_CFG_S {
	CVI_BOOL PreFltEn;
} ISP_AF_PRE_FILTER_CFG_S;

typedef struct _ISP_AF_CFG_S {
	CVI_BOOL bEnable;
	CVI_U16 u16Hwnd; /*RW; Range:[0x2, 0x11]*/
	CVI_U16 u16Vwnd; /*RW; Range:[0x2, 0xF]*/
	CVI_U8 u8HFltShift; /*RW; Range:[0x0, 0xF]*/
	CVI_S8 s8HVFltLpCoeff[FIR_H_GAIN_NUM]; /*RW; Range:[0x0, 0x1F]*/
	ISP_AF_RAW_CFG_S stRawCfg;
	ISP_AF_PRE_FILTER_CFG_S stPreFltCfg;
	ISP_AF_CROP_S stCrop;
	CVI_U8 H0FltCoring; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 H1FltCoring; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 V0FltCoring; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16HighLumaTh; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 u8ThLow;
	CVI_U8 u8ThHigh;
	CVI_U8 u8GainLow; /*RW; Range:[0x0, 0xFE]*/
	CVI_U8 u8GainHigh; /*RW; Range:[0x0, 0xFE]*/
	CVI_U8 u8SlopLow; /*RW; Range:[0x0, 0xF]*/
	CVI_U8 u8SlopHigh; /*RW; Range:[0x0, 0xF]*/
} ISP_AF_CFG_S;

typedef struct _ISP_AF_H_PARAM_S {
	CVI_S8 s8HFltHpCoeff[FIR_H_GAIN_NUM]; /*RW; Range:[0x0, 0x1F]*/
} ISP_AF_H_PARAM_S;

typedef struct _ISP_AF_V_PARAM_S {
	CVI_S8 s8VFltHpCoeff[FIR_V_GAIN_NUM]; /*RW; Range:[0x0, 0x1F]*/
} ISP_AF_V_PARAM_S;

typedef struct _ISP_FOCUS_STATISTICS_CFG_S {
	ISP_AF_CFG_S stConfig;
	ISP_AF_H_PARAM_S stHParam_FIR0;
	ISP_AF_H_PARAM_S stHParam_FIR1;
	ISP_AF_V_PARAM_S stVParam_FIR;
} ISP_FOCUS_STATISTICS_CFG_S;

typedef struct _ISP_STATISTICS_CFG_S {
	ISP_STATISTICS_CTRL_U unKey;
	ISP_AE_STATISTICS_CFG_S stAECfg;
	ISP_WB_STATISTICS_CFG_S stWBCfg;
	ISP_FOCUS_STATISTICS_CFG_S stFocusCfg;
} ISP_STATISTICS_CFG_S;

typedef struct _ISP_AE_GRID_INFO_S {
	CVI_U16 au16GridYPos[AE_ZONE_ROW + 1];
	CVI_U16 au16GridXPos[AE_ZONE_COLUMN + 1];
	CVI_U8 u8Status;
} ISP_AE_GRID_INFO_S;

typedef struct _ISP_AE_STATISTICS_S {
	CVI_U32 au32FEHist1024Value[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][MAX_HIST_BINS];
	CVI_U16 au16FEGlobalAvg[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][BAYER_PATTERN_NUM];
	CVI_U16 au16FEZoneAvg[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];
	CVI_U32 au32BEHist1024Value[MAX_HIST_BINS];
	CVI_U16 au16BEGlobalAvg[BAYER_PATTERN_NUM];
	CVI_U16 au16BEZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];
	ISP_AE_GRID_INFO_S stFEGridInfo;
} ISP_AE_STATISTICS_S;

typedef struct _ISP_AWB_GRID_INFO_S {
	CVI_U16 au16GridYPos[AWB_ZONE_ORIG_ROW + 1];
	CVI_U16 au16GridXPos[AWB_ZONE_ORIG_COLUMN + 1];
	CVI_U8 u8Status;
} ISP_AWB_GRID_INFO_S;

typedef struct _ISP_WB_STATISTICS_S {
	CVI_U16 u16GlobalR; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16GlobalG; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16GlobalB; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16CountAll; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 au16ZoneAvgR[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneAvgG[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneAvgB[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneCountAll[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0xFFFF]*/
	ISP_AWB_GRID_INFO_S stGridInfo;
} ISP_WB_STATISTICS_S;

typedef struct _ISP_FOCUS_ZONE_S {
	CVI_U16 u16HlCnt;
	CVI_U64 u64h0;
	CVI_U64 u64h1;
	CVI_U32 u32v0;
} ISP_FOCUS_ZONE_S;

typedef struct _ISP_FE_FOCUS_STATISTICS_S {
	ISP_FOCUS_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_FE_FOCUS_STATISTICS_S;

typedef struct _ISP_AF_STATISTICS_S {
	ISP_FE_FOCUS_STATISTICS_S stFEAFStat;
} ISP_AF_STATISTICS_S;

typedef struct _ISP_MG_STATISTICS_S {
	CVI_U16 au16ZoneAvg[MG_ZONE_ROW][MG_ZONE_COLUMN][BAYER_PATTERN_NUM];
} ISP_MG_STATISTICS_S;

//-----------------------------------------------------------------------------
//  WB Attr
//-----------------------------------------------------------------------------
#define AWB_CALIB_PTS_NUM (3)
#define AWB_CT_BIN_NUM (8)

typedef struct _ISP_AWB_LightBox_Gain_S {
	CVI_U16 u16AvgRgain;
	CVI_U16 u16AvgBgain;
} ISP_AWB_LightBox_Gain_S;

typedef struct _ISP_AWB_Calibration_Gain_S {
	CVI_U16 u16AvgRgain[AWB_CALIB_PTS_NUM]; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16AvgBgain[AWB_CALIB_PTS_NUM]; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16ColorTemperature[AWB_CALIB_PTS_NUM]; /*RW; Range:[0x1F4, 0x7530]*/
} ISP_AWB_Calibration_Gain_S;

#define AWB_CALIB_PTS_NUM_EX (10)
typedef struct _ISP_AWB_Calibration_Gain_S_EX {
	CVI_U16 u16AvgRgain[AWB_CALIB_PTS_NUM_EX]; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16AvgBgain[AWB_CALIB_PTS_NUM_EX]; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16ColorTemperature[AWB_CALIB_PTS_NUM_EX]; /*RW; Range:[0x1F4, 0x7530]*/
	CVI_U8 u8Weight[AWB_CALIB_PTS_NUM_EX];
} ISP_AWB_Calibration_Gain_S_EX;

#define AWB_LUM_HIST_NUM (6)
typedef struct _ISP_AWB_LUM_HISTGRAM_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 au8HistThresh[AWB_LUM_HIST_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 au16HistWt[AWB_LUM_HIST_NUM]; /*RW; Range:[0x0, 0x200]*/
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct _ISP_AWB_CT_LIMIT_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U16 u16HighRgLimit; /*RW; Range:[0x0, 0x3FFF] H temp,Rgain max*/
	CVI_U16 u16HighBgLimit; /*RW; Range:[0x0, 0x3FFF] H temp,Bgain max*/
	CVI_U16 u16LowRgLimit; /*RW; Range:[0x0, 0x3FFF] Low temp,Rgain min*/
	CVI_U16 u16LowBgLimit; /*RW; Range:[0x0, 0x3FFF] Low temp,Bgain min*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct _ISP_AWB_CBCR_TRACK_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U16 au16CrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FFF] R/G at L tmp*/
	CVI_U16 au16CrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FFF] R/G at H tmp*/
	CVI_U16 au16CbMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FFF] B/G at H tmp*/
	CVI_U16 au16CbMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FFF] B/G at L tmp*/
} ISP_AWB_CBCR_TRACK_ATTR_S;

typedef struct _ISP_AWB_IN_OUT_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enOutdoorStatus;
	CVI_U32 u32OutThresh; /*RW; Range:[0x0, 0x14]*/
	CVI_U16 u16LowStart; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16LowStop; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16HighStart; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16HighStop; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_BOOL bGreenEnhanceEn;
	CVI_U8 u8OutShiftLimit;
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct _ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S {
	CVI_U16 u16WhiteRgain; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16WhiteBgain; /*RW; Range:[0x0, 0x3FFF]*/
	CVI_U16 u16ExpQuant; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 u8LightStatus; /*RW; Range:[0x0, 0x2]*/
	CVI_U8 u8Radius; /*RW; Range:[0x1, 0xFF]*/
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

struct ST_ISP_AWB_INTERFERENCE_S {
	CVI_U8 u8Mode;	/*RW; Range:[0x0, 0x1]*/
	CVI_U8 u8Limit; /*RW; Range:[0x32, 0x64]*/
	CVI_U8 u8Radius; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 u8Ratio; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 u8Distance; /*RW; Range:[0x1, 0xFF]*/
};

struct ST_ISP_AWB_SKIN_S {
	CVI_U8 u8Mode;
	CVI_U16 u16RgainDiff;
	CVI_U16 u16BgainDiff;
	CVI_U8 u8Radius;
};

struct ST_ISP_AWB_SKY_S {
	CVI_U8 u8Mode;
	CVI_U8 u8ThrLv;
	CVI_U16 u16Rgain;
	CVI_U16 u16Bgain;
	CVI_U16 u16MapRgain;
	CVI_U16 u16MapBgain;
	CVI_U8 u8Radius;
};

struct ST_ISP_AWB_GRASS_S {
	CVI_U8 u8Mode;
	CVI_U8 u8ThrLv;
	CVI_U16 u16Rgain;
	CVI_U16 u16Bgain;
	CVI_U16 u16MapRgain;
	CVI_U16 u16MapBgain;
	CVI_U8 u8Radius;
};

enum ISP_AWB_TEMP_E {
	ISP_AWB_COLORTEMP_LOW,
	ISP_AWB_COLORTEMP_HIGH,
	ISP_AWB_COLORTEMP_NUM
};

struct ST_ISP_AWB_SHIFT_LV_S {
	CVI_U8 u8LowLvMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 u16LowLvCT[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16LowLvThr[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x0, 0x5DC]*/
	CVI_U16 u16LowLvRatio[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x64, 0x3E8]*/
	CVI_U8 u8HighLvMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 u16HighLvCT[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16HighLvThr[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x0, 0x5DC]*/
	CVI_U16 u16HighLvRatio[ISP_AWB_COLORTEMP_NUM]; /*RW; Range:[0x64, 0x3E8]*/
};

struct ST_ISP_AWB_REGION_S {
	CVI_U16 u16Region1; /*RW; Range:[0x1, 0xFFFF]*/
	CVI_U16 u16Region2; /*RW; Range:[0x1, 0xFFFF]*/
	CVI_U16 u16Region3; /*RW; Range:[0x1, 0xFFFF]*/
};

#define AWB_CT_LV_NUM (4)
struct ST_ISP_AWB_CT_WGT_S {
	CVI_BOOL bEnable;
	CVI_U16 au16MultiCTBin[AWB_CT_BIN_NUM];//low to High ColorTemp
	CVI_S8 s8ThrLv[AWB_CT_LV_NUM];//low to high light
	CVI_U16 au16MultiCTWt[AWB_CT_LV_NUM][AWB_CT_BIN_NUM];//0~1024
};

#define AWB_CURVE_PARA_NUM (6)
#define AWB_CURVE_BOUND_NUM (8)
#define AWB_LS_NUM (4)
#define AWB_ZONE_WT_W (32)
#define AWB_ZONE_WT_H (32)
#define AWB_ZONE_WT_NUM (AWB_ZONE_WT_W * AWB_ZONE_WT_H)

typedef struct _ISP_AWB_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U16 u16RefColorTemp; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 au16StaticWB[ISP_BAYER_CHN_NUM]; /*RW; Range:[0x0, 0xFFFF] G/R*/
	CVI_S32 as32CurvePara[AWB_CURVE_PARA_NUM];
	ISP_AWB_ALG_TYPE_E enAlgType;
	CVI_U8 u8RGStrength;
	CVI_U8 u8BGStrength;
	CVI_U16 u16Speed; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 u16ZoneSel; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 u16HighColorTemp; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 u16LowColorTemp; /*RW; Range:[0x0, 0xFFFF]*/
	ISP_AWB_CT_LIMIT_ATTR_S stCTLimit;
	CVI_BOOL bShiftLimitEn;
	CVI_U16 u16ShiftLimit[AWB_CURVE_BOUND_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_BOOL bGainNormEn;
	CVI_BOOL bNaturalCastEn;
	ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;// R/G
	ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
	CVI_BOOL bAWBZoneWtEn;
	CVI_U8 au8ZoneWt[AWB_ZONE_WT_NUM];
} ISP_AWB_ATTR_S;

typedef struct _ISP_AWB_ATTR_EX_S {
	CVI_U8 u8Tolerance;
	CVI_U8 u8ZoneRadius;
	CVI_U16 u16CurveLLimit; /*RW; Range:[0x0, 0x400]*/
	CVI_U16 u16CurveRLimit; /*RW; Range:[0x200, 0x3FFF]*/
	CVI_BOOL bExtraLightEn;
	ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S stLightInfo[AWB_LS_NUM];
	ISP_AWB_IN_OUT_ATTR_S stInOrOut;
	CVI_BOOL bMultiLightSourceEn;
	ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;
	CVI_U16 u16MultiLSScaler; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16MultiLSThr; /*RW; Range:[0x20, 0x200]*/
	CVI_U16 u16CALumaDiff; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16CAAdjustRatio; /*RW; Range:[0x0, 0x10]*/
	CVI_U16 au16MultiCTBin[AWB_CT_BIN_NUM]; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 au16MultiCTWt[AWB_CT_BIN_NUM]; /*RW; Range:[0x0, 0x400]*/
	CVI_BOOL bFineTunEn;
	CVI_U8 u8FineTunStrength;
	//AWB Algo 6
	struct ST_ISP_AWB_INTERFERENCE_S stInterference;
	struct ST_ISP_AWB_SKIN_S stSkin;
	struct ST_ISP_AWB_SKY_S stSky;
	struct ST_ISP_AWB_GRASS_S stGrass;
	struct ST_ISP_AWB_CT_WGT_S stCtLv;
	struct ST_ISP_AWB_SHIFT_LV_S stShiftLv;
	struct ST_ISP_AWB_REGION_S stRegion;
	CVI_U8 adjBgainMode;
} ISP_AWB_ATTR_EX_S;//keep size to 512 bytes

typedef struct _ISP_MWB_ATTR_S {
	CVI_U16 u16Rgain; /*RW; Range:[0x1, 0x3FFF]*/
	CVI_U16 u16Grgain; /*RW; Range:[0x1, 0x3FFF]*/
	CVI_U16 u16Gbgain; /*RW; Range:[0x1, 0x3FFF]*/
	CVI_U16 u16Bgain; /*RW; Range:[0x1, 0x3FFF]*/
} ISP_MWB_ATTR_S;

typedef enum _ISP_AWB_ALG_E {
	ALG_AWB,
	ALG_AWB_SPEC,
	ALG_BUTT
} ISP_AWB_ALG_E;

typedef struct _ISP_WB_ATTR_S {
	CVI_BOOL bByPass;
	CVI_U8 u8AWBRunInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_OP_TYPE_E enOpType;
	ISP_MWB_ATTR_S stManual;
	ISP_AWB_ATTR_S stAuto;
	ISP_AWB_ALG_E enAlgType;
	CVI_U8 u8DebugMode;
} ISP_WB_ATTR_S;

//-----------------------------------------------------------------------------
//  WB Info
//-----------------------------------------------------------------------------
typedef struct _ISP_WB_INFO_S {
	CVI_U16 u16Rgain; /*R; Range:[0x0, 0xFFF]*/
	CVI_U16 u16Grgain; /*R; Range:[0x0, 0xFFF]*/
	CVI_U16 u16Gbgain; /*R; Range:[0x0, 0xFFF]*/
	CVI_U16 u16Bgain; /*R; Range:[0x0, 0xFFF]*/
	CVI_U16 u16Saturation; /*R; Range:[0x0, 0xFF]*/
	CVI_U16 u16ColorTemp; /*R;*/
	CVI_U16 au16CCM[CCM_MATRIX_SIZE]; /*R;*/
	CVI_U16 u16LS0CT; /*R;*/
	CVI_U16 u16LS1CT; /*R;*/
	CVI_U16 u16LS0Area; /*R;*/
	CVI_U16 u16LS1Area; /*R;*/
	CVI_U8 u8MultiDegree; /*R;*/
	CVI_U16 u16ActiveShift; /*R;*/
	CVI_U32 u32FirstStableTime; /*R;*/
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus; /*R;*/
	CVI_S16 s16Bv; /*R;*/
} ISP_WB_INFO_S;

typedef struct _ISP_DCF_CONST_INFO_S {
	CVI_U8 au8ImageDescription[DCF_DRSCRIPTION_LENGTH]; /*Describes image*/
	CVI_U8 au8Make[DCF_DRSCRIPTION_LENGTH]; /*Shows manufacturer of digital cameras*/
	CVI_U8 au8Model[DCF_DRSCRIPTION_LENGTH]; /*Shows model number of digital cameras*/
	CVI_U8 au8Software[DCF_DRSCRIPTION_LENGTH];
	/*Shows firmware (internal software of digital cameras) version number*/
	CVI_U8 u8LightSource;
	CVI_U32 u32FocalLength; /*Focal length of lens used to take image. Unit is millimeter*/
	CVI_U8 u8SceneType;
	/*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
	CVI_U8 u8CustomRendered;
	CVI_U8 u8FocalLengthIn35mmFilm; /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
	CVI_U8 u8SceneCaptureType;
	CVI_U8 u8GainControl;
	CVI_U8 u8Contrast;
	CVI_U8 u8Saturation;
	CVI_U8 u8Sharpness;
	CVI_U8 u8MeteringMode;
} ISP_DCF_CONST_INFO_S;

typedef struct _ISP_DCF_UPDATE_INFO_S {
	CVI_U32 u32ISOSpeedRatings; /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
	CVI_U32 u32ExposureTime; /*Exposure time (reciprocal of shutter speed).*/
	CVI_U32 u32ExposureBiasValue; /*Exposure bias (compensation) value of taking picture*/
	CVI_U8 u8ExposureProgram;
	CVI_U32 u32FNumber; /*The actual F-number (F-stop) of lens when the image was taken*/
	CVI_U32 u32MaxApertureValue; /*Maximum aperture value of lens.*/
	CVI_U8 u8ExposureMode;
	CVI_U8 u8WhiteBalance;
} ISP_DCF_UPDATE_INFO_S;

typedef struct _ISP_DCF_INFO_S {
	ISP_DCF_CONST_INFO_S stIspDCFConstInfo;
	ISP_DCF_UPDATE_INFO_S stIspDCFUpdateInfo;
} ISP_DCF_INFO_S;

typedef struct _ISP_MOD_PARAM_S {
	CVI_U32 u32IntBotHalf;
} ISP_MOD_PARAM_S;
#if 0
typedef struct _ISP_MESHSHADING_MANUAL_ATTR_S {
	CVI_U16 lsc_strength;
	CVI_U16 lscr_strnth;
	CVI_U16 lscr_gain_lut[32];
} ISP_MESHSHADING_MANUAL_ATTR_S;

typedef struct _ISP_MESHSHADING_AUTO_ATTR_S {
	CVI_U16 lsc_strength[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 lscr_strnth[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 lscr_gain_lut[ISP_AUTO_ISO_STRENGTH_NUM][32];
} ISP_MESHSHADING_AUTO_ATTR_S;

typedef struct _ISP_MESHSHADING_ATTR_S {
	CVI_BOOL lsc_enable;
	CVI_U8 lsc_debug;
	CVI_U8 lsc_xblknumm1;
	CVI_U8 reg_lsc_yblknumm1;
	CVI_U16 reg_lsc_xstep;
	CVI_U16 reg_lsc_ystep;
	CVI_U32 reg_lsc_imgx0;
	CVI_U32 reg_lsc_imgy0;
	CVI_U8 reg_lsc_gain_base;
	CVI_U16 reg_lsc_scaler[256];
	CVI_BOOL lscr_enable;
	CVI_U16 lscr_centerx;
	CVI_U16 lscr_centery;
	CVI_U16 lscr_norm;
	ISP_OP_TYPE_E enOpType;
	ISP_MESHSHADING_MANUAL_ATTR_S stManual;
	ISP_MESHSHADING_AUTO_ATTR_S stAuto;
} ISP_MESHSHADING_ATTR_S;
#endif

//-----------------------------------------------------------------------------
//  Black Level Correction(BLC)
//-----------------------------------------------------------------------------
typedef struct _ISP_BLACK_LEVEL_MANUAL_ATTR_S {
	CVI_U16 OffsetR; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetR2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB2; /*RW; Range:[0x0, 0xfff]*/
} ISP_BLACK_LEVEL_MANUAL_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_AUTO_ATTR_S {
	CVI_U16 OffsetR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetR2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_BLACK_LEVEL_AUTO_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_BLACK_LEVEL_MANUAL_ATTR_S stManual;
	ISP_BLACK_LEVEL_AUTO_ATTR_S stAuto;
} ISP_BLACK_LEVEL_ATTR_S;

//-----------------------------------------------------------------------------
//  Dead pixel correction(DPC)
//-----------------------------------------------------------------------------
typedef struct _ISP_DP_DYNAMIC_MANUAL_ATTR_S {
	CVI_U8 ClusterSize; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 BrightDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DarkDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreR; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreG; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinG; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinRB; /*RW; Range:[0x0, 0xff]*/
} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_AUTO_ATTR_S {
	CVI_U8 ClusterSize[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 BrightDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DarkDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreG[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinG[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinRB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_U32 DynamicDPCEnable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
	ISP_DP_DYNAMIC_AUTO_ATTR_S stAuto;
} ISP_DP_DYNAMIC_ATTR_S;

#define STATIC_DP_COUNT_MAX (4096)
typedef struct _ISP_DP_STATIC_ATTR_S {
	CVI_BOOL Enable;
	CVI_U16 BrightCount; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DarkCount; /*RW; Range:[0x0, 0xfff]*/
	CVI_U32 BrightTable[STATIC_DP_COUNT_MAX]; /*RW; Range:[0x0, 0x1fff1fff]*/
	CVI_U32 DarkTable[STATIC_DP_COUNT_MAX]; /*RW; Range:[0x0, 0x1fff1fff]*/
	CVI_BOOL Show; // not support yet
} ISP_DP_STATIC_ATTR_S;

typedef enum _CVI_STATIC_DP_TYPE_E {
	ISP_STATIC_DP_BRIGHT,
	ISP_STATIC_DP_DARK,
} CVI_STATIC_DP_TYPE_E;

typedef enum _ISP_STATUS_E {
	ISP_STATUS_INIT,
	ISP_STATUS_SUCCESS,
	ISP_STATUS_TIMEOUT,
	ISP_STATUS_SIZE,
} ISP_STATUS_E;

typedef struct _ISP_DP_CALIB_ATTR_S {
	CVI_BOOL EnableDetect;
	CVI_STATIC_DP_TYPE_E StaticDPType;
	CVI_U8 StartThresh; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CountMax; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CountMin; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 TimeLimit; /*RW; Range:[0x0, 0x640]*/
	CVI_BOOL saveFileEn;
	// read only
	CVI_U32 Table[STATIC_DP_COUNT_MAX]; /*R; Range:[0x0, 0x1fff1fff]*/
	CVI_U8 FinishThresh; /*R; Range:[0x0, 0xff]*/
	CVI_U16 Count; /*R; Range:[0x0, 0xfff]*/
	ISP_STATUS_E Status; /*R; Range:[0x0, 0x2]*/
} ISP_DP_CALIB_ATTR_S;

//-----------------------------------------------------------------------------
//  Crosstalk
//-----------------------------------------------------------------------------
typedef struct _ISP_CROSSTALK_MANUAL_ATTR_S {
	CVI_U16 Strength; /*RW; Range:[0x0, 0x100]*/
} ISP_CROSSTALK_MANUAL_ATTR_S;

typedef struct _ISP_CROSSTALK_AUTO_ATTR_S {
	CVI_U16 Strength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_CROSSTALK_AUTO_ATTR_S;

typedef struct _ISP_CROSSTALK_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U16 GrGbDiffThreSec[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FlatThre[4]; /*RW; Range:[0x0, 0xfff]*/
	ISP_CROSSTALK_MANUAL_ATTR_S stManual;
	ISP_CROSSTALK_AUTO_ATTR_S stAuto;
} ISP_CROSSTALK_ATTR_S;

//-----------------------------------------------------------------------------
//  Bayer domain noise reduction (BNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_NR_MANUAL_ATTR_S {
	CVI_U8 WindowType; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStrMode; /*RW; Range:[0x0, 0xff]*/
} ISP_NR_MANUAL_ATTR_S;

typedef struct _ISP_NR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStrMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_NR_AUTO_ATTR_S;

typedef struct _ISP_NR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL CoringParamEnable;
	ISP_NR_MANUAL_ATTR_S stManual;
	ISP_NR_AUTO_ATTR_S stAuto;
} ISP_NR_ATTR_S;

typedef struct _ISP_NR_FILTER_MANUAL_ATTR_S {
	CVI_U8 LumaStr[8]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U16 NpSlopeR; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeGr; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeGb; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeB; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrR; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrGr; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrGb; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrB; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetR; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetGr; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetGb; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetB; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetR; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetGr; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetGb; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetB; /*RW; Range:[0x0, 0x3ff]*/
} ISP_NR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_NR_FILTER_AUTO_ATTR_S {
	CVI_U8 LumaStr[8][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U16 NpSlopeR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpSlopeB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLumaThrB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpLowOffsetB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 NpHighOffsetB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
} ISP_NR_FILTER_AUTO_ATTR_S;

typedef enum _ISP_NR_TUNING_MODE_E {
	NR_RESULT = 8,
	NR_EDGE_SMOOTH_REGION = 11,
} ISP_NR_TUNING_MODE_E;

typedef struct _ISP_NR_FILTER_ATTR_S {
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xf]*/
	ISP_NR_FILTER_MANUAL_ATTR_S stManual;
	ISP_NR_FILTER_AUTO_ATTR_S stAuto;
} ISP_NR_FILTER_ATTR_S;

typedef struct _ISP_RLSC_MANUAL_ATTR_S {
	CVI_U16 RadialStr; /*RW; Range:[0x0, 0xfff]*/
} ISP_RLSC_MANUAL_ATTR_S;

typedef struct _ISP_RLSC_AUTO_ATTR_S {
	CVI_U16 RadialStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_RLSC_AUTO_ATTR_S;

typedef struct _ISP_RLSC_ATTR_S {
	CVI_BOOL RlscEnable; /*RW; Range:[0, 1]*/
	CVI_U16 RlscCenterX; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RlscCenterY; /*RW; Range:[0x0, 0xfff]*/
	ISP_RLSC_MANUAL_ATTR_S stManual;
	ISP_RLSC_AUTO_ATTR_S stAuto;
} ISP_RLSC_ATTR_S;

//-----------------------------------------------------------------------------
//  Demosaic
//-----------------------------------------------------------------------------
typedef struct _ISP_DEMOSAIC_MANUAL_ATTR_S {
	CVI_U16 CoarseEdgeThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CoarseStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineEdgeThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RbSigLumaThd; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 FilterMode; /*RW; Range:[0, 1]*/
} ISP_DEMOSAIC_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_AUTO_ATTR_S {
	CVI_U16 CoarseEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CoarseStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RbSigLumaThd[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 FilterMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0, 1]*/
} ISP_DEMOSAIC_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	CVI_BOOL RbVtEnable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_DEMOSAIC_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S {
	CVI_U8 AntiFalseColorStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 SatGainIn[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 SatGainOut[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ProtectColorGainIn[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ProtectColorGainOut[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor1; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor3; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 EdgeGainIn[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 EdgeGainOut[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailGainIn[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailGaintOut[2]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailDetectLumaStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 DetailSmoothStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DetailWgtMin; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailWgtMax; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailWgtSlope; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 EdgeWgtNp; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 EdgeWgtMin; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 EdgeWgtMax; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 EdgeWgtSlope; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 DetailSmoothMapTh; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DetailSmoothMapMin; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailSmoothMapMax; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailSmoothMapSlope; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 LumaWgt; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S {
	CVI_U8 AntiFalseColorStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 SatGainIn[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 SatGainOut[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ProtectColorGainIn[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ProtectColorGainOut[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 UserDefineProtectColor3[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 EdgeGainIn[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 EdgeGainOut[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailGainIn[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailGaintOut[2][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DetailDetectLumaStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 DetailSmoothStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DetailWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 EdgeWgtNp[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 EdgeWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 EdgeWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 EdgeWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 DetailSmoothMapTh[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DetailSmoothMapMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailSmoothMapMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DetailSmoothMapSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x400]*/
	CVI_U8 LumaWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_ATTR_S {
	CVI_BOOL AntiFalseColorEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL ProtectColorEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL DetailDetectLumaEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL DetailSmoothEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL DetailMode; /*RW; Range:[0, 1]*/
	ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_DEMOIRE_ATTR_S;

//-----------------------------------------------------------------------------
//  RGBCAC
//-----------------------------------------------------------------------------
typedef struct _ISP_RGBCAC_MANUAL_ATTR_S {
	CVI_U8 DePurpleStr0; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStr1; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 EdgeCoring; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 DePurpleCrStr0; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCbStr0; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCrStr1; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCbStr1; /*RW; Range:[0x0, 0x10]*/
} ISP_RGBCAC_MANUAL_ATTR_S;

typedef struct _ISP_RGBCAC_AUTO_ATTR_S {
	CVI_U8 DePurpleStr0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStr1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 EdgeCoring[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 DePurpleCrStr0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCbStr0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCrStr1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleCbStr1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x10]*/
} ISP_RGBCAC_AUTO_ATTR_S;

typedef struct _ISP_RGBCAC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 PurpleDetRange0; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 PurpleDetRange1; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 DePurpleStrMax0; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStrMin0; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStrMax1; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStrMin1; /*RW; Range:[0x0, 0xFF]*/
	CVI_U16 EdgeGlobalGain; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 EdgeGainIn[3]; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 EdgeGainOut[3]; /*RW; Range:[0x0, 0x20]*/
	CVI_U16 LumaScale; /*RW; Range:[0x0, 0x7FF]*/
	CVI_U16 UserDefineLuma; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 LumaBlendWgt; /*RW; Range:[0x0, 0x20]*/
	CVI_U8 LumaBlendWgt2; /*RW; Range:[0x0, 0x20]*/
	CVI_U8 LumaBlendWgt3; /*RW; Range:[0x0, 0x20]*/
	CVI_U16 PurpleCb; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 PurpleCr; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 PurpleCb2; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 PurpleCr2; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 PurpleCb3; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 PurpleCr3; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 GreenCb; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 GreenCr; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x2]*/
	ISP_RGBCAC_MANUAL_ATTR_S stManual;
	ISP_RGBCAC_AUTO_ATTR_S stAuto;
} ISP_RGBCAC_ATTR_S;

//-----------------------------------------------------------------------------
//  LCAC
//-----------------------------------------------------------------------------
typedef struct _ISP_LCAC_GAUSS_COEF_ATTR_S {
	CVI_U8 Wgt; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 Sigma; /*RW; Range:[0x1, 0xFF]*/
} ISP_LCAC_GAUSS_COEF_ATTR_S;

typedef struct _ISP_LCAC_MANUAL_ATTR_S {
	CVI_U16 DePurpleCrGain; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 DePurpleCbGain; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 DePurepleCrWgt0; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCbWgt0; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCrWgt1; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCbWgt1; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 EdgeCoringBase; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeCoringAdv; /*RW; Range:[0x0, 0xFF]*/
} ISP_LCAC_MANUAL_ATTR_S;

typedef struct _ISP_LCAC_AUTO_ATTR_S {
	CVI_U16 DePurpleCrGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 DePurpleCbGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U8 DePurepleCrWgt0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCbWgt0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCrWgt1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurepleCbWgt1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 EdgeCoringBase[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeCoringAdv[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
} ISP_LCAC_AUTO_ATTR_S;

typedef struct _ISP_LCAC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x6]*/
	CVI_U8 DePurpleCrStr0; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurpleCbStr0; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurpleCrStr1; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurpleCbStr1; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 FilterTypeBase; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 EdgeGainBase0; /*RW; Range:[0x0, 0x1C]*/
	CVI_U8 EdgeGainBase1; /*RW; Range:[0x0, 0x23]*/
	CVI_U8 EdgeStrWgtBase; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleStrMaxBase; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 DePurpleStrMinBase; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 FilterScaleAdv; /*RW; Range:[0x0, 0xF]*/
	CVI_U8 LumaWgt; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 FilterTypeAdv; /*RW; Range:[0x0, 0x5]*/
	CVI_U8 EdgeGainAdv0; /*RW; Range:[0x0, 0x1C]*/
	CVI_U8 EdgeGainAdv1; /*RW; Range:[0x0, 0x23]*/
	CVI_U8 EdgeStrWgtAdvG; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 DePurpleStrMaxAdv; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleStrMinAdv; /*RW; Range:[0x0, 0xFF]*/
	ISP_LCAC_GAUSS_COEF_ATTR_S EdgeWgtBase;
	ISP_LCAC_GAUSS_COEF_ATTR_S EdgeWgtAdv;
	ISP_LCAC_MANUAL_ATTR_S stManual;
	ISP_LCAC_AUTO_ATTR_S stAuto;
} ISP_LCAC_ATTR_S;

//-----------------------------------------------------------------------------
//  Mesh lens shading correction (MLSC)
//-----------------------------------------------------------------------------
#define ISP_MLSC_COLOR_TEMPERATURE_SIZE (7)

typedef struct _ISP_MESH_SHADING_MANUAL_ATTR_S {
	CVI_U16 MeshStr; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_MANUAL_ATTR_S;

typedef struct _ISP_MESH_SHADING_AUTO_ATTR_S {
	CVI_U16 MeshStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_AUTO_ATTR_S;

typedef struct _ISP_MESH_SHADING_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL OverflowProtection;
	ISP_MESH_SHADING_MANUAL_ATTR_S stManual;
	ISP_MESH_SHADING_AUTO_ATTR_S stAuto;
} ISP_MESH_SHADING_ATTR_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_S {
	CVI_U16 ColorTemperature; /*RW; Range:[0x0, 0x7530]*/
	CVI_U16 RGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_GAIN_LUT_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_ATTR_S {
	CVI_U8 Size; /*RW; Range:[0x1, 0x7]*/
	ISP_MESH_SHADING_GAIN_LUT_S LscGainLut[ISP_MLSC_COLOR_TEMPERATURE_SIZE];
} ISP_MESH_SHADING_GAIN_LUT_ATTR_S;

//-----------------------------------------------------------------------------
//  CCM
//-----------------------------------------------------------------------------
// Saturation
typedef struct _ISP_SATURATION_MANUAL_ATTR_S {
	CVI_U8 Saturation; /*RW; Range:[0x0, 0xff]*/
} ISP_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_SATURATION_AUTO_ATTR_S {
	CVI_U8 Saturation[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_SATURATION_ATTR_SS {
	ISP_OP_TYPE_E enOpType;
	ISP_SATURATION_MANUAL_ATTR_S stManual;
	ISP_SATURATION_AUTO_ATTR_S stAuto;
} ISP_SATURATION_ATTR_S;

// CCM Saturation
typedef struct _ISP_CCM_SATURATION_MANUAL_ATTR_S {
	CVI_U8 SaturationLE; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SaturationSE; /*RW; Range:[0x0, 0xff]*/
} ISP_CCM_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_CCM_SATURATION_AUTO_ATTR_S {
	CVI_U8 SaturationLE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SaturationSE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CCM_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_CCM_SATURATION_ATTR_SS {
	ISP_CCM_SATURATION_MANUAL_ATTR_S stManual;
	ISP_CCM_SATURATION_AUTO_ATTR_S stAuto;
} ISP_CCM_SATURATION_ATTR_S;

// CCM
typedef struct _ISP_COLORMATRIX_ATTR_S {
	CVI_U16 ColorTemp; /*RW; Range:[0x1f4, 0x7530]*/
	CVI_S16 CCM[9]; /*RW; Range:[-0x2000, 0x1FFF]*/
} ISP_COLORMATRIX_ATTR_S;

typedef struct _ISP_CCM_MANUAL_ATTR_S {
	CVI_BOOL SatEnable; /*RW; Range:[0, 1]*/
	CVI_S16 CCM[9]; /*RW; Range:[-0x2000, 0x1FFF]*/
} ISP_CCM_MANUAL_ATTR_S;

typedef struct _ISP_CCM_AUTO_ATTR_S {
	CVI_BOOL ISOActEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL TempActEnable; /*RW; Range:[0, 1]*/
	CVI_U8 CCMTabNum; /*RW; Range:[0x3, 0x7]*/
	ISP_COLORMATRIX_ATTR_S CCMTab[7];
} ISP_CCM_AUTO_ATTR_S;

typedef struct _ISP_CCM_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_CCM_MANUAL_ATTR_S stManual;
	ISP_CCM_AUTO_ATTR_S stAuto;
} ISP_CCM_ATTR_S;

//-----------------------------------------------------------------------------
//  CSC
//-----------------------------------------------------------------------------
typedef enum _ISP_CSC_COLORGAMUT {
	ISP_CSC_COLORGAMUT_BT601,
	ISP_CSC_COLORGAMUT_BT709,
	ISP_CSC_COLORGAMUT_BT2020,
	ISP_CSC_COLORGAMUT_USER,
	ISP_CSC_COLORGAMUT_NUM
} ISP_CSC_COLORGAMUT;

typedef struct _ISP_CSC_MATRX_S {
	CVI_S16 userCscCoef[CSC_MATRIX_SIZE]; /*RW; Range:[-0x2000, 0x1FFF]*/
	CVI_S16 userCscOffset[CSC_OFFSET_SIZE]; /*RW; Range:[-0x100, 0xFF]*/
} ISP_CSC_MATRX_S;

typedef struct _ISP_CSC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_CSC_COLORGAMUT enColorGamut;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 Hue; /*RW; Range:[0, 0x64]*/
	CVI_U8 Luma; /*RW; Range:[0, 0x64]*/
	CVI_U8 Contrast; /*RW; Range:[0, 0x64]*/
	CVI_U8 Saturation; /*RW; Range:[0, 0x64]*/
	ISP_CSC_MATRX_S stUserMatrx;
} ISP_CSC_ATTR_S;

//-----------------------------------------------------------------------------
//  Color tone
//-----------------------------------------------------------------------------
typedef struct _ISP_COLOR_TONE_ATTR_S {
	CVI_U16 u16RedCastGain; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 u16GreenCastGain; /*RW; Range:[0x0, 0xFFF]*/
	CVI_U16 u16BlueCastGain; /*RW; Range:[0x0, 0xFFF]*/
} ISP_COLOR_TONE_ATTR_S;

//-----------------------------------------------------------------------------
//  FSWDR
//-----------------------------------------------------------------------------
typedef struct _ISP_FSWDR_MANUAL_ATTR_S {
	CVI_U16 WDRCombineLongThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineShortThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineMaxWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRCombineMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRMtIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMtOut[4]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRLongWgt; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRCombineSNRAwareToleranceLevel; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MergeModeAlpha; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMotionCombineLongThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineShortThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineMaxWeight; /*RW; Range:[0x0, 0x100]*/
} ISP_FSWDR_MANUAL_ATTR_S;

typedef struct _ISP_FSWDR_AUTO_ATTR_S {
	CVI_U16 WDRCombineLongThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineShortThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRCombineMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRMtIn[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMtOut[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRLongWgt[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRCombineSNRAwareToleranceLevel[ISP_AUTO_LV_NUM];
	CVI_U8 MergeModeAlpha[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMotionCombineLongThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineShortThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_FSWDR_AUTO_ATTR_S;

typedef struct _ISP_FSWDR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType; /*RW; Range:[0, 1]*/
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL MotionCompEnable; /*RW; Range:[0, 1]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x9]*/
	CVI_BOOL WDRDCMode;; /*RW; Range:[0, 1]*/
	CVI_BOOL WDRLumaMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 WDRType; /*RW; Range:[0x0, 0x2]*/
	CVI_BOOL WDRCombineSNRAwareEn; /*RW; Range:[0, 1]*/
	CVI_U16 WDRCombineSNRAwareLowThr; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 WDRCombineSNRAwareHighThr; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 WDRCombineSNRAwareSmoothLevel; /*RW; Range:[0x1, 0xbb8]*/
	CVI_BOOL LocalToneRefinedDCMode; /*RW; Range:[0, 1]*/
	CVI_BOOL LocalToneRefinedLumaMode; /*RW; Range:[0, 1]*/
	CVI_U16 DarkToneRefinedThrL; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DarkToneRefinedThrH; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DarkToneRefinedMaxWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DarkToneRefinedMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 BrightToneRefinedThrL; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BrightToneRefinedThrH; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BrightToneRefinedMaxWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 BrightToneRefinedMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRMotionFusionMode; /*RW; Range:[0x0, 0x3]*/
	CVI_BOOL MtMode; /*RW; Range:[0, 1]*/

	ISP_FSWDR_MANUAL_ATTR_S stManual;
	ISP_FSWDR_AUTO_ATTR_S stAuto;
} ISP_FSWDR_ATTR_S;

//-----------------------------------------------------------------------------
//  DRC
//-----------------------------------------------------------------------------
#define DRC_DARK_USER_DEFINE_NUM (257)
#define DRC_BRIGHT_USER_DEFINE_NUM (513)
#define DRC_GLOBAL_USER_DEFINE_NUM (1025)

typedef struct _ISP_DRC_MANUAL_ATTR_S {
	CVI_U32 TargetYScale; /*RW; Range:[0x0, 0x800]*/
	CVI_U16 HdrStrength; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 DEAdaptPercentile; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 DEAdaptTargetGain; /*RW; Range:[0x1, 0x60]*/
	CVI_U8 DEAdaptGainUB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 DEAdaptGainLB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 BritInflectPtLuma; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrTargetY; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SdrTargetYGain; /*RW; Range:[0x20, 0x80]*/
	CVI_U16 SdrGlobalToneStr; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 SdrDEAdaptPercentile; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 SdrDEAdaptTargetGain; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 SdrDEAdaptGainLB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptGainUB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrBritInflectPtLuma; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 TotalGain; /*RW; Range:[0x0, 0xff]*/
} ISP_DRC_MANUAL_ATTR_S;

typedef struct _ISP_DRC_AUTO_ATTR_S {
	CVI_U32 TargetYScale[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x800]*/
	CVI_U16 HdrStrength[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 DEAdaptPercentile[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 DEAdaptTargetGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0x60]*/
	CVI_U8 DEAdaptGainUB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 DEAdaptGainLB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 BritInflectPtLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrTargetY[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SdrTargetYGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x20, 0x80]*/
	CVI_U16 SdrGlobalToneStr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 SdrDEAdaptPercentile[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 SdrDEAdaptTargetGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 SdrDEAdaptGainLB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptGainUB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrBritInflectPtLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 TotalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DRC_AUTO_ATTR_S;

typedef struct _ISP_DRC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x7]*/
	CVI_BOOL LocalToneEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LocalToneRefineEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 ToneCurveSelect; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 CurveUserDefine[DRC_GLOBAL_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 DarkUserDefine[DRC_DARK_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 BrightUserDefine[DRC_BRIGHT_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U32 ToneCurveSmooth; /*RW; Range:[0x0, 0x1f4]*/
	CVI_U8 CoarseFltScale; /*RW; Range:[0x3, 0x6]*/
	CVI_U8 SdrTargetYGainMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DetailEnhanceEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 LumaGain[33]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailEnhanceMtIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DetailEnhanceMtOut[4]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 OverShootThd; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThd; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 UnderShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 OverShootThdMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThdMin; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL SoftClampEnable; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 SoftClampUB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SoftClampLB; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL dbg_182x_sim_enable; /*RW; Range:[0, 1]*/
	CVI_U8 DarkMapStr; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 BritMapStr; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 SdrDarkMapStr; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 SdrBritMapStr; /*RW; Range:[0x0, 0x80]*/
	CVI_U32 DRCMu[32]; /*RW; Range:[0x0, 0x7fffffff]*/
	ISP_DRC_MANUAL_ATTR_S stManual;
	ISP_DRC_AUTO_ATTR_S stAuto;
} ISP_DRC_ATTR_S;

//-----------------------------------------------------------------------------
//  Gamma
//-----------------------------------------------------------------------------
#define GAMMA_NODE_NUM (256)
#define GAMMA_MAX_INTERPOLATION_NUM (5)
typedef enum _ISP_GAMMA_CURVE_TYPE_E {
	ISP_GAMMA_CURVE_DEFAULT,
	ISP_GAMMA_CURVE_SRGB,
	ISP_GAMMA_CURVE_USER_DEFINE,
	ISP_GAMMA_CURVE_AUTO,
	ISP_GAMMA_CURVE_MAX
} ISP_GAMMA_CURVE_TYPE_E;

typedef struct _ISP_GAMMA_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U16 Table[GAMMA_NODE_NUM]; /*RW; Range:[0x0, 0xfff]*/
	ISP_GAMMA_CURVE_TYPE_E enCurveType;
} ISP_GAMMA_ATTR_S;

typedef struct _ISP_GAMMA_CURVE_ATTR_S {
	CVI_S16 Lv; /*RW; Range:[-0x1f4, 0x5dc]*/
	CVI_U16 Tbl[GAMMA_NODE_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_GAMMA_CURVE_ATTR_S;

typedef struct _ISP_AUTO_GAMMA_ATTR_S {
	CVI_U8 GammaTabNum; /*RW; Range:[1, GAMMA_MAX_INTERPOLATION_NUM]*/
	ISP_GAMMA_CURVE_ATTR_S GammaTab[GAMMA_MAX_INTERPOLATION_NUM];
} ISP_AUTO_GAMMA_ATTR_S;

//-----------------------------------------------------------------------------
//  Dehaze
//-----------------------------------------------------------------------------
typedef struct _ISP_DEHAZE_MANUAL_ATTR_S {
	CVI_U8 Strength; /*RW; Range:[0x0, 0x64]*/
} ISP_DEHAZE_MANUAL_ATTR_S;

typedef struct _ISP_DEHAZE_AUTO_ATTR_S {
	CVI_U8 Strength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
} ISP_DEHAZE_AUTO_ATTR_S;

typedef struct _ISP_DEHAZE_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U16 CumulativeThr; /*RW; Range:[0x0, 0x3fff]*/
	CVI_U16 MinTransMapValue; /*RW; Range:[0x0, 0x1fff]*/
	CVI_BOOL DehazeLumaEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL DehazeSkinEnable; /*RW; Range:[0, 1]*/
	CVI_U8 AirLightMixWgt; /*RW; Range:[0x0, 0x20]*/
	CVI_U8 DehazeWgt; /*RW; Range:[0x0, 0x20]*/
	CVI_U8 TransMapScale; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AirlightDiffWgt; /*RW; Range:[0x0, 0x10]*/
	CVI_U16 AirLightMax; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 AirLightMin; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 SkinCb; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SkinCr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 DehazeLumaCOEFFI; /*RW; Range:[0x0, 0x7d0]*/
	CVI_U16 DehazeSkinCOEFFI; /*RW; Range:[0x0, 0x7d0]*/
	CVI_U8 TransMapWgtWgt; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 TransMapWgtSigma; /*RW; Range:[0x0, 0xff]*/
	ISP_DEHAZE_MANUAL_ATTR_S stManual;
	ISP_DEHAZE_AUTO_ATTR_S stAuto;
} ISP_DEHAZE_ATTR_S;

//-----------------------------------------------------------------------------
//  CLUT
//-----------------------------------------------------------------------------
#define ISP_CLUT_LUT_LENGTH (4913)
typedef struct _ISP_CLUT_ATTR_S {
	CVI_BOOL Enable;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U16 ClutR[ISP_CLUT_LUT_LENGTH]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 ClutG[ISP_CLUT_LUT_LENGTH]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 ClutB[ISP_CLUT_LUT_LENGTH]; /*RW; Range:[0x0, 0x3FF]*/
} ISP_CLUT_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_MANUAL_ATTR_S {
	CVI_U16 SatIn[4]; /*RW; Range:[0x0, 0x2000]*/
	CVI_U16 SatOut[4]; /*RW; Range:[0x0, 0x2000]*/
} ISP_CLUT_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_AUTO_ATTR_S {
	CVI_U16 SatIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x2000]*/
	CVI_U16 SatOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x2000]*/
} ISP_CLUT_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_CLUT_SATURATION_MANUAL_ATTR_S stManual;
	ISP_CLUT_SATURATION_AUTO_ATTR_S stAuto;
} ISP_CLUT_SATURATION_ATTR_S;

//-----------------------------------------------------------------------------
//  DCI
//-----------------------------------------------------------------------------
typedef struct _ISP_DCI_MANUAL_ATTR_S {
	CVI_U16 ContrastGain; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 BlcThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WhtThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 BlcCtrl; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 WhtCtrl; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 DciGainMax; /*RW; Range:[0x0, 0x100]*/
} ISP_DCI_MANUAL_ATTR_S;

typedef struct _ISP_DCI_AUTO_ATTR_S {
	CVI_U16 ContrastGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 BlcThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WhtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 WhtCtrl[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 DciGainMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_DCI_AUTO_ATTR_S;

typedef struct _ISP_DCI_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 Method; /*RW; Range:[0x0, 0x1]*/
	CVI_U32 Speed; /*RW; Range:[0x0, 0x1f4]*/
	CVI_U16 DciStrength; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DciGamma; /*RW; Range:[0x64, 0x320]*/
	CVI_U8 DciOffset; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ToleranceY; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 Sensitivity; /*RW; Range:[0x0, 0xff]*/
	ISP_DCI_MANUAL_ATTR_S stManual;
	ISP_DCI_AUTO_ATTR_S stAuto;
} ISP_DCI_ATTR_S;

//-----------------------------------------------------------------------------
//  LDCI
//-----------------------------------------------------------------------------
typedef struct _ISP_LDCI_GAUSS_COEF_ATTR_S {
	CVI_U8 Wgt; /*RW; Range:[0x1, 0x80]*/
	CVI_U8 Sigma; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 Mean; /*RW; Range:[0x1, 0xFF]*/
} ISP_LDCI_GAUSS_COEF_ATTR_S;

typedef struct _ISP_LDCI_MANUAL_ATTR_S {
	CVI_U16 LdciStrength; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 LdciRange; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 TprCoef; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U8 EdgeCoring; /*RW; Range:[0x0, 0x0]*/
	CVI_U8 LumaWgtMax; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 LumaWgtMin; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 VarMapMax; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 VarMapMin; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 UvGainMax; /*RW; Range:[0x0, 0x7F]*/
	CVI_U8 UvGainMin; /*RW; Range:[0x0, 0x7F]*/
	CVI_U8 BrightContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BrightContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DarkContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DarkContrastLow; /*RW; Range:[0x0, 0x64]*/
	ISP_LDCI_GAUSS_COEF_ATTR_S LumaPosWgt;
} ISP_LDCI_MANUAL_ATTR_S;

typedef struct _ISP_LDCI_AUTO_ATTR_S {
	CVI_U16 LdciStrength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 LdciRange[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 TprCoef[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U8 EdgeCoring[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x0]*/
	CVI_U8 LumaWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 LumaWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 VarMapMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 VarMapMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 UvGainMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7F]*/
	CVI_U8 UvGainMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7F]*/
	CVI_U8 BrightContrastHigh[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BrightContrastLow[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DarkContrastHigh[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DarkContrastLow[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
	ISP_LDCI_GAUSS_COEF_ATTR_S LumaPosWgt[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_LDCI_AUTO_ATTR_S;

typedef struct _ISP_LDCI_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 GaussLPFSigma; /*RW; Range:[0x0, 0xFF]*/
	ISP_LDCI_MANUAL_ATTR_S stManual;
	ISP_LDCI_AUTO_ATTR_S stAuto;
} ISP_LDCI_ATTR_S;

//-----------------------------------------------------------------------------
//  CA (CA/CP)
//-----------------------------------------------------------------------------
#define CA_LUT_NUM (256)

typedef struct _ISP_CA_MANUAL_ATTR_S {
	CVI_U16 ISORatio; /*RW; Range:[0x0, 0x7FF]*/
	CVI_U16 YRatioLut[CA_LUT_NUM]; /*RW; Range:[0x0, 0x7FF]*/
} ISP_CA_MANUAL_ATTR_S;

typedef struct _ISP_CA_AUTO_ATTR_S {
	CVI_U16 ISORatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7FF]*/
	CVI_U16 YRatioLut[CA_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7FF]*/
} ISP_CA_AUTO_ATTR_S;

typedef struct _ISP_CA_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL CaCpMode;
	CVI_U8 CPLutY[CA_LUT_NUM];
	CVI_U8 CPLutU[CA_LUT_NUM];
	CVI_U8 CPLutV[CA_LUT_NUM];
	ISP_CA_MANUAL_ATTR_S stManual;
	ISP_CA_AUTO_ATTR_S stAuto;
} ISP_CA_ATTR_S;

//-----------------------------------------------------------------------------
//  CA2
//-----------------------------------------------------------------------------
#define CA_LITE_NODE (6)

typedef struct _ISP_CA2_MANUAL_ATTR_S {
	CVI_U8 Ca2In[CA_LITE_NODE]; /*RW; Range:[0x0, 0xC0]*/
	CVI_U16 Ca2Out[CA_LITE_NODE]; /*RW; Range:[0x0, 0x7FF]*/
} ISP_CA2_MANUAL_ATTR_S;

typedef struct _ISP_CA2_AUTO_ATTR_S {
	CVI_U8 Ca2In[CA_LITE_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xC0]*/
	CVI_U16 Ca2Out[CA_LITE_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7FF]*/
} ISP_CA2_AUTO_ATTR_S;

typedef struct _ISP_CA2_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_CA2_MANUAL_ATTR_S stManual;
	ISP_CA2_AUTO_ATTR_S stAuto;
} ISP_CA2_ATTR_S;

//-----------------------------------------------------------------------------
//  PreSharpen
//-----------------------------------------------------------------------------
#define EE_LUT_NODE (4)
typedef struct _ISP_PRESHARPEN_MANUAL_ATTR_S {
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 LumaCorLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaCorLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorWgtLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorWgtLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 GlobalGain; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 UnderShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 HFBlendWgt; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MFBlendWgt; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 HueShtCtrl[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 SatShtGainIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatShtGainOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
} ISP_PRESHARPEN_MANUAL_ATTR_S;

typedef struct _ISP_PRESHARPEN_AUTO_ATTR_S {
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 LumaCorLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaCorLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorWgtLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorWgtLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 UnderShootGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 HFBlendWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MFBlendWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 HueShtCtrl[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 SatShtGainIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatShtGainOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
} ISP_PRESHARPEN_AUTO_ATTR_S;

typedef struct _ISP_PRESHARPEN_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xb]*/
	CVI_BOOL LumaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DeltaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL NoiseSuppressEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL SatShtCtrlEn; /*RW; Range:[0, 1]*/
	CVI_BOOL SoftClampEnable; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 SoftClampUB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SoftClampLB; /*RW; Range:[0x0, 0xff]*/
	ISP_PRESHARPEN_MANUAL_ATTR_S stManual;
	ISP_PRESHARPEN_AUTO_ATTR_S stAuto;
} ISP_PRESHARPEN_ATTR_S;

//-----------------------------------------------------------------------------
//  Time-domain noise reduction (TNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_TNR_MANUAL_ATTR_S {
	CVI_U8 TnrStrength0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdLow0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdHigh0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MtDetectUnit; /*RW; Range:[0x3, 0x5]*/
	CVI_S16 BrightnessNoiseLevelLE; /*RW; Range:[0x1, 0x3ff]*/
	CVI_S16 BrightnessNoiseLevelSE; /*RW; Range:[0x1, 0x3ff]*/
	CVI_BOOL MtFiltMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 MtFiltWgt; /*RW; Range:[0x0, 0x100]*/
} ISP_TNR_MANUAL_ATTR_S;

typedef struct _ISP_TNR_AUTO_ATTR_S {
	CVI_U8 TnrStrength0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdLow0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdHigh0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MtDetectUnit[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x3, 0x5]*/
	CVI_S16 BrightnessNoiseLevelLE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x3ff]*/
	CVI_S16 BrightnessNoiseLevelSE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x3ff]*/
	CVI_BOOL MtFiltMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 MtFiltWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_TNR_AUTO_ATTR_S;

typedef struct _ISP_TNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	CVI_BOOL TnrMtMode; /*RW; Range:[0, 1]*/
	CVI_BOOL YnrCnrSharpenMtMode; /*RW; Range:[0, 1]*/
	CVI_BOOL PreSharpenMtMode; /*RW; Range:[0, 1]*/
	CVI_U8 ChromaScalingDownMode; /*RW; Range:[0, 3]*/
	CVI_BOOL CompGainEnable; /*RW; Range:[0x0, 0x1]*/
	ISP_TNR_MANUAL_ATTR_S stManual;
	ISP_TNR_AUTO_ATTR_S stAuto;
} ISP_TNR_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S {
	CVI_U8 RNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_AUTO_ATTR_S {
	CVI_U8 RNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_NOISE_MODEL_AUTO_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_ATTR_S {
	ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S stManual;
	ISP_TNR_NOISE_MODEL_AUTO_ATTR_S stAuto;
} ISP_TNR_NOISE_MODEL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S {
	CVI_U16 L2mIn0[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut0[4]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 L2mIn1[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut1[4]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL MtLumaMode; /*RW; Range:[0x0, 0x1]*/
} ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_AUTO_ATTR_S {
	CVI_U16 L2mIn0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 L2mIn1[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut1[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL MtLumaMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
} ISP_TNR_LUMA_MOTION_AUTO_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_ATTR_S {
	ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S stManual;
	ISP_TNR_LUMA_MOTION_AUTO_ATTR_S stAuto;
} ISP_TNR_LUMA_MOTION_ATTR_S;

typedef struct _ISP_TNR_GHOST_MANUAL_ATTR_S {
	CVI_U8 PrvMotion0[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PrtctWgt0[4]; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionHistoryStr; /*RW; Range:[0x0, 0xf]*/
} ISP_TNR_GHOST_MANUAL_ATTR_S;

typedef struct _ISP_TNR_GHOST_AUTO_ATTR_S {
	CVI_U8 PrvMotion0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PrtctWgt0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionHistoryStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xf]*/
} ISP_TNR_GHOST_AUTO_ATTR_S;

typedef struct _ISP_TNR_GHOST_ATTR_S {
	ISP_TNR_GHOST_MANUAL_ATTR_S stManual;
	ISP_TNR_GHOST_AUTO_ATTR_S stAuto;
} ISP_TNR_GHOST_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_MANUAL_ATTR_S {
	CVI_U8 LowMtPrtLevelY; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelU; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelV; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInY[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInU[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInV[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutY[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutU[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutV[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtAdvIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtAdvOut[4]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MT_PRT_MANUAL_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_AUTO_ATTR_S {
	CVI_U8 LowMtPrtLevelY[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelU[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelV[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInY[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInU[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInV[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutY[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutU[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutV[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtAdvIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtAdvOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MT_PRT_AUTO_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_ATTR_S {
	CVI_BOOL LowMtPrtEn; /*RW; Range:[0, 1]*/
	CVI_BOOL LowMtLowPassEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL LowMtPrtAdvLumaEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL LowMtPrtAdvMode; /*RW; Range:[0, 1]*/
	CVI_U8 LowMtPrtAdvMax; /*RW; Range:[0, 0xff]*/
	CVI_BOOL LowMtPrtAdvDebugMode; /*RW; Range:[0, 1]*/
	CVI_U8 LowMtPrtAdvDebugIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtAdvDebugOut[4]; /*RW; Range:[0x0, 0xff]*/
	ISP_TNR_MT_PRT_MANUAL_ATTR_S stManual;
	ISP_TNR_MT_PRT_AUTO_ATTR_S stAuto;
} ISP_TNR_MT_PRT_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S {
	CVI_U8 AdaptNrLumaStrIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrLumaStrOut[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrOut[4]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S {
	CVI_U8 AdaptNrLumaStrIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrLumaStrOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAP_ATTR_S {
	ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S stManual;
	ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S stAuto;
} ISP_TNR_MOTION_ADAPT_ATTR_S;

//-----------------------------------------------------------------------------
//  Y domain noise reduction (YNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_YNR_MANUAL_ATTR_S {
	CVI_U8 WindowType; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringBase; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringAdv; /*RW; Range:[0x0, 0xff]*/
} ISP_YNR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringBase[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringAdv[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_YNR_AUTO_ATTR_S;

typedef enum _ISP_YNR_TUNING_MODE_E {
	YNR_RESULT = 8,
	YNR_EDGE_SMOOTH_REGION = 11,
} ISP_YNR_TUNING_MODE_E;

typedef struct _ISP_YNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL CoringParamEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL FiltModeEnable; /*RW; Range:[0, 1]*/
	CVI_U16 FiltMode; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xf]*/
	ISP_YNR_MANUAL_ATTR_S stManual;
	ISP_YNR_AUTO_ATTR_S stAuto;
} ISP_YNR_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_MANUAL_ATTR_S {
	CVI_U8 MotionCoringWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionYnrLut[16]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionCoringWgt[16]; /*RW; Range:[0x0, 0x100]*/
} ISP_YNR_MOTION_NR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_AUTO_ATTR_S {
	CVI_U8 MotionCoringWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionYnrLut[16][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionCoringWgt[16][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_YNR_MOTION_NR_AUTO_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_ATTR_S {
	ISP_YNR_MOTION_NR_MANUAL_ATTR_S stManual;
	ISP_YNR_MOTION_NR_AUTO_ATTR_S stAuto;
} ISP_YNR_MOTION_NR_ATTR_S;

typedef struct _ISP_YNR_FILTER_MANUAL_ATTR_S {
	CVI_U8 VarThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 CoringWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 FilterMode; /*RW; Range:[0x0, 0x3ff]*/
} ISP_YNR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_YNR_FILTER_AUTO_ATTR_S {
	CVI_U8 VarThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 CoringWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 FilterMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
} ISP_YNR_FILTER_AUTO_ATTR_S;

typedef struct _ISP_YNR_FILTER_ATTR_S {
	ISP_YNR_FILTER_MANUAL_ATTR_S stManual;
	ISP_YNR_FILTER_AUTO_ATTR_S stAuto;
} ISP_YNR_FILTER_ATTR_S;

//-----------------------------------------------------------------------------
//  UV domain noise reduction (CNR)
//-----------------------------------------------------------------------------
#define CNR_MOTION_LUT_NUM (16)

typedef struct _ISP_CNR_MANUAL_ATTR_S {
	CVI_U8 CnrStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressGain; /*RW; Range:[0x1, 0x8]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionNrStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgt; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
} ISP_CNR_MANUAL_ATTR_S;

typedef struct _ISP_CNR_AUTO_ATTR_S {
	CVI_U8 CnrStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x8]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionNrStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
} ISP_CNR_AUTO_ATTR_S;

typedef struct _ISP_CNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_CNR_MANUAL_ATTR_S stManual;
	ISP_CNR_AUTO_ATTR_S stAuto;
} ISP_CNR_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_MANUAL_ATTR_S {
	CVI_U8 MotionCnrCoringLut[CNR_MOTION_LUT_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCnrStrLut[CNR_MOTION_LUT_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CNR_MOTION_NR_MANUAL_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_AUTO_ATTR_S {
	CVI_U8 MotionCnrCoringLut[CNR_MOTION_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCnrStrLut[CNR_MOTION_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CNR_MOTION_NR_AUTO_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_ATTR_S {
	CVI_BOOL MotionCnrEnable; /*RW; Range:[0, 1]*/
	ISP_CNR_MOTION_NR_MANUAL_ATTR_S stManual;
	ISP_CNR_MOTION_NR_AUTO_ATTR_S stAuto;
} ISP_CNR_MOTION_NR_ATTR_S;

//-----------------------------------------------------------------------------
//  CAC
//-----------------------------------------------------------------------------
typedef struct _ISP_CAC_MANUAL_ATTR_S {
	CVI_U8 DePurpleStr; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeGlobalGain; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeCoring; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeStrMin; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeStrMax; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleCbStr; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DePurpleCrStr; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DePurpleStrMaxRatio; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurpleStrMinRatio; /*RW; Range:[0x0, 0x40]*/
} ISP_CAC_MANUAL_ATTR_S;

typedef struct _ISP_CAC_AUTO_ATTR_S {
	CVI_U8 DePurpleStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeGlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeCoring[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeStrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 EdgeStrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 DePurpleCbStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DePurpleCrStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DePurpleStrMaxRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 DePurpleStrMinRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
} ISP_CAC_AUTO_ATTR_S;

typedef struct _ISP_CAC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 PurpleDetRange; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 PurpleCb; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 PurpleCr; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 PurpleCb2; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 PurpleCr2; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 PurpleCb3; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 PurpleCr3; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 GreenCb; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 GreenCr; /*RW; Range:[0x0, 0xFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x2]*/
	CVI_U8 EdgeGainIn[3]; /*RW; Range:[0x0, 0x10]*/
	CVI_U8 EdgeGainOut[3]; /*RW; Range:[0x0, 0x20]*/
	ISP_CAC_MANUAL_ATTR_S stManual;
	ISP_CAC_AUTO_ATTR_S stAuto;
} ISP_CAC_ATTR_S;

//-----------------------------------------------------------------------------
//  Sharpen
//-----------------------------------------------------------------------------
#define EE_LUT_NODE (4)

typedef struct _ISP_SHARPEN_MANUAL_ATTR_S {
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 LumaCorLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaCorLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorWgtLutIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorWgtLutOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 GlobalGain; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 UnderShootGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 HFBlendWgt; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MFBlendWgt; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 HueShtCtrl[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 SatShtGainIn[EE_LUT_NODE]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatShtGainOut[EE_LUT_NODE]; /*RW; Range:[0x0, 0x80]*/
} ISP_SHARPEN_MANUAL_ATTR_S;

typedef struct _ISP_SHARPEN_AUTO_ATTR_S {
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 LumaCorLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaCorLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionCorWgtLutIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionCorWgtLutOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 UnderShootGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 HFBlendWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MFBlendWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 HueShtCtrl[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 SatShtGainIn[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatShtGainOut[EE_LUT_NODE][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x80]*/
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct _ISP_SHARPEN_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xb]*/
	CVI_BOOL LumaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DeltaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL NoiseSuppressEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL SatShtCtrlEn; /*RW; Range:[0, 1]*/
	CVI_BOOL SoftClampEnable; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 SoftClampUB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SoftClampLB; /*RW; Range:[0x0, 0xff]*/
	ISP_SHARPEN_MANUAL_ATTR_S stManual;
	ISP_SHARPEN_AUTO_ATTR_S stAuto;
} ISP_SHARPEN_ATTR_S;

//-----------------------------------------------------------------------------
//  Y Contrast
//-----------------------------------------------------------------------------
typedef struct _ISP_YCONTRAST_MANUAL_ATTR_S {
	CVI_U8 ContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 ContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 CenterLuma; /*RW; Range:[0x0, 0x40]*/
} ISP_YCONTRAST_MANUAL_ATTR_S;

typedef struct _ISP_YCONTRAST_AUTO_ATTR_S {
	CVI_U8 ContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 ContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 CenterLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x40]*/
} ISP_YCONTRAST_AUTO_ATTR_S;

typedef struct _ISP_YCONTRAST_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	ISP_YCONTRAST_MANUAL_ATTR_S stManual;
	ISP_YCONTRAST_AUTO_ATTR_S stAuto;
} ISP_YCONTRAST_ATTR_S;

//-----------------------------------------------------------------------------
//  Mono
//-----------------------------------------------------------------------------
typedef struct _ISP_MONO_ATTR_S {
	CVI_BOOL Enable;
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
} ISP_MONO_ATTR_S;

//-----------------------------------------------------------------------------
//  DIS
//-----------------------------------------------------------------------------
typedef enum _ISP_DIS_MODE_E {
	DIS_MODE_2_DOF_GME = 0, /* Only use with GME in 2 dof */
	DIS_MODE_DEBUG,
	DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

typedef enum _ISP_DIS_MOTION_LEVEL_E {
	DIS_MOTION_LEVEL_NORMAL = 1,
	DIS_MOTION_LEVEL_BUTT
} DIS_MOTION_LEVEL_E;

typedef struct _ISP_DIS_CONFIG_S {
	DIS_MODE_E mode;
	DIS_MOTION_LEVEL_E motionLevel;
	CVI_U32 cropRatio; /*RW; Range:[0x32, 0x62] */
} ISP_DIS_CONFIG_S;

typedef struct _ISP_DIS_ATTR_S {
	CVI_BOOL enable;
	CVI_U32 movingSubjectLevel; /*RW; Range:[0x0, 0x6] */
	CVI_U32 horizontalLimit; /*RW; Range:[0x0, 0x3E8] */
	CVI_U32 verticalLimit; /*RW; Range:[0x0, 0x3E8] */
	CVI_BOOL stillCrop;
} ISP_DIS_ATTR_S;

typedef struct _ISP_INNER_STATE_INFO_S {
	CVI_U32 blcOffsetR;
	CVI_U32 blcOffsetGr;
	CVI_U32 blcOffsetGb;
	CVI_U32 blcOffsetB;
	CVI_U32 blcGainR;
	CVI_U32 blcGainGr;
	CVI_U32 blcGainGb;
	CVI_U32 blcGainB;
	CVI_S32 ccm[9];
	CVI_U16 drcGlobalToneBinNum;
	CVI_U16 drcGlobalToneBinSEStep;
	CVI_U32 drcGlobalTone[LTM_GLOBAL_CURVE_NODE_NUM];
	CVI_U32 drcDarkTone[LTM_DARK_CURVE_NODE_NUM];
	CVI_U32 drcBrightTone[LTM_BRIGHT_CURVE_NODE_NUM];
	CVI_BOOL bWDRSwitchFinish;
	// For 2TO1/3TO1/4TO1 use.
	CVI_U32 u32WDRExpRatioActual[ISP_WDR_FRAME_IDX_SIZE];
	ISP_MESH_SHADING_GAIN_LUT_S mlscGainTable;
} ISP_INNER_STATE_INFO_S;

typedef struct _ISP_VC_ATTR_S {
	CVI_U8 UpdateInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 MotionThreshold[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff] */
} ISP_VC_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_COMM_ISP_H__ */
