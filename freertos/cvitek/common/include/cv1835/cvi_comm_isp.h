/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common_isp.h
 * Description:
 */

#ifndef __CVI_COMM_ISP_H__
#define __CVI_COMM_ISP_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_defines.h"
#include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ISP_CHECK_POINTER(addr)                                                                                        \
	do {                                                                                                           \
		if (addr == NULL)                                                                                      \
			return CVI_FAILURE;                                                                            \
	} while (0)

#define CVI_TRACE_ISP(level, fmt, ...)  \
	CVI_TRACE(level, CVI_ID_ISP, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CCM_MATRIX_SIZE (9)
#define AE_MAX_ZONE_ROW (24)
#define AE_MAX_ZONE_COLUMN (32)
#define AE_ZONE_ROW (15)
#define AE_ZONE_COLUMN (17)
#define AE_MAX_NUM (2)
#define ISP_SENSOR_MAX_DELAY_NUM (5)
#define AWB_ZONE_ORIG_ROW (48)
#define AWB_ZONE_ORIG_COLUMN (64)
#define AWB_ZONE_NUM (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN)
#define AWB_MAX_NUM (1)
#define HIST_BIN_DEPTH (8)
#define MAX_HIST_BINS (1 << HIST_BIN_DEPTH)
#define MAX_EXPOSURE_RATIO 256
#define MAX_DRC_HIST_BINS (MAX_HIST_BINS * MAX_EXPOSURE_RATIO)
#define DCF_DRSCRIPTION_LENGTH 32
#define DCF_CAPTURE_TIME_LENGTH 20
#define ISP_AUTO_ISO_STRENGTH_NUM 16
#define ISP_AUTO_EXP_RATIO_NUM 16
#define ISP_AUTO_COLORTEMP_NUM 7
#define ISP_AUTO_LV_NUM ISP_AUTO_ISO_STRENGTH_NUM
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
#define ISP_3ALIB_FIND_FAIL (-1)

#define HIST_NUM 256



/*Defines the format of the input Bayer image*/
typedef enum _ISP_BAYER_FORMAT_E {
	BAYER_BGGR,
	BAYER_GBRG,
	BAYER_GRBG,
	BAYER_RGGB,
	BAYER_BUTT
} ISP_BAYER_FORMAT_E;

typedef enum bmISP_OP_TYPE_E {
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

typedef union _ISP_SNS_COMMBUS_U {
	CVI_S8 s8I2cDev;
	struct {
		CVI_S8 bit4SspDev : 4;
		CVI_S8 bit4SspCs : 4;
	} s8SspDev;
} ISP_SNS_COMMBUS_U;

typedef struct _ISP_I2C_DATA_S {
	CVI_BOOL bUpdate;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the sensor register*/
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
	CVI_U32 u32StatIntvl;
	CVI_U32 u32ProcParam;
	CVI_U32 u32UpdatePos;
	CVI_U32 u32IntTimeOut;
	CVI_U32 u32PwmNumber;
	CVI_U32 u32PortIntDelay;
} ISP_CTRL_PARAM_S;

typedef union _ISP_MODULE_CTRL_U {
	CVI_U64 u64Key;
	struct {
		CVI_U64 bitBypassISPDGain : 1; /* RW;[0] */
		CVI_U64 bitBypassAntiFC : 1; /* RW;[1] */
		CVI_U64 bitBypassCrosstalkR : 1; /* RW;[2] */
		CVI_U64 bitBypassDPC : 1; /* RW;[3] */
		CVI_U64 bitBypassNR : 1; /* RW;[4] */
		CVI_U64 bitBypassDehaze : 1; /* RW;[5] */
		CVI_U64 bitBypassWBGain : 1; /* RW;[6] */
		CVI_U64 bitBypassMeshShading : 1; /* RW;[7] */
		CVI_U64 bitBypassDRC : 1; /* RW;[8] */
		CVI_U64 bitBypassDemosaic : 1; /* RW;[9] */
		CVI_U64 bitBypassColorMatrix : 1; /* RW;[10] */
		CVI_U64 bitBypassGamma : 1; /* RW;[11] */
		CVI_U64 bitBypassFSWDR : 1; /* RW;[12] */
		CVI_U64 bitBypassCA : 1; /* RW;[13] */
		CVI_U64 bitBypassCsConv : 1; /* RW;[14] */
		CVI_U64 bitBypassRadialCrop : 1; /* RW;[15] */
		CVI_U64 bitBypassSharpen : 1; /* RW;[16] */
		CVI_U64 bitBypassLCAC : 1; /* RW;[17] */
		CVI_U64 bitBypassGCAC : 1; /* RW;[18] */
		CVI_U64 bit2ChnSelect : 2; /* RW;[19:20] */
		CVI_U64 bitBypassLdci : 1; /* RW;[21] */
		CVI_U64 bitBypassPreGamma : 1; /* RW;[22] */
		CVI_U64 bitBypassRadialShading : 1; /* RW;[23] */
		CVI_U64 bitBypassAEStatFE : 1; /* RW;[24] */
		CVI_U64 bitBypassAEStatBE : 1; /* RW;[25] */
		CVI_U64 bitBypassMGStat : 1; /* RW;[26] */
		CVI_U64 bitBypassDE : 1; /* RW;[27] */
		CVI_U64 bitBypassAFStatFE : 1; /* RW;[28] */
		CVI_U64 bitBypassAFStatBE : 1; /* RW;[29] */
		CVI_U64 bitBypassAWBStat : 1; /* RW;[30] */
		CVI_U64 bitBypassCLUT : 1; /* RW;[31] */
		CVI_U64 bitBypassHLC : 1; /* RW;[32] */
		CVI_U64 bitBypassEdgeMark : 1; /* RW;[33] */
		CVI_U64 bitBypassRGBIR : 1; /* RW;[34] */
		CVI_U64 bitBypassHSV : 1; /* RW;[35] */
		CVI_U64 bitBypassRgbDither : 1; /* RW;[36] */
		CVI_U64 bitBypassYuvDither : 1; /* RW;[37] */
		CVI_U64 bitBypassYnr : 1; /* RW;[38] */
		CVI_U64 bitBypassCnr : 1; /* RW;[39] */
		CVI_U64 bitBypassRgbEE : 1; /* RW;[40] */
		CVI_U64 bitBypass3DNR : 1; /* RW;[41] */
		CVI_U64 bitRsv21 : 21; /* H  ; [42:63] */
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
	AE_METER_FISHEYE,
} ISP_AE_METER_MODE_E;

typedef enum _ISP_AE_IR_CUT_FORCE_STATUS {
	AE_IR_CUT_FORCE_AUTO = 0,
	AE_IR_CUT_FORCE_ON,
	AE_IR_CUT_FORCE_OFF,
} ISP_AE_IR_CUT_FORCE_STATUS;

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
	CVI_BOOL bEnable;
	CVI_U16 u16X;
	CVI_U16 u16Y;
	CVI_U16 u16W;
	CVI_U16 u16H;
} ISP_AE_CROP_S;

typedef struct _ISP_AE_STATISTICS_CFG_S {
	ISP_AE_SWITCH_E enAESwitch; /*RW; Range:[0,2]; */
	ISP_AE_HIST_CONFIG_S stHistConfig;
	ISP_AE_FOUR_PLANE_MODE_E enFourPlaneMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enHistMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enAverMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enMaxGainMode; /*RW; Range:[0,1]; */
	ISP_AE_CROP_S stCrop[AE_MAX_NUM];
	CVI_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*RW; Range:[0x0, 0xF]; */
} ISP_AE_STATISTICS_CFG_S;

typedef struct _ISP_AE_WIN_STATISTICS_CFG_S {
	CVI_BOOL	bHisStatisticsEnable;
	CVI_BOOL	bCropEnable;
	CVI_U16		ZoneOffsetX;
	CVI_U16		ZoneOffsetY;
	CVI_U16		ZoneWidth;
	CVI_U16		ZoneHeight;
	CVI_U8		StatisticsShiftBits;
	CVI_U16		RGain;
	CVI_U16		GGain;
	CVI_U16		BGain;
	CVI_U8		au8WinWeight[AE_ZONE_ROW][AE_ZONE_COLUMN];
} ISP_AE_WIN_STATISTICS_CFG_S;

typedef enum _ISP_IRIS_TYPE_E {
	ISP_IRIS_DC_TYPE = 0,
	ISP_IRIS_P_TYPE,
	ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

//Defines the F value of the ISP iris
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

#define ISP_AE_ROUTE_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_NODE_S {
	CVI_U32 u32IntTime;
	CVI_U32 u32SysGain;
	ISP_IRIS_F_NO_E enIrisFNO;
	CVI_U32 u32IrisFNOLin;
} ISP_AE_ROUTE_NODE_S;

typedef struct _ISP_AE_ROUTE_S {
	CVI_U32 u32TotalNum;
	ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_EX_NODE_S {
	CVI_U32 u32IntTime;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	CVI_U32 u32IspDgain;
	ISP_IRIS_F_NO_E enIrisFNO;
	CVI_U32 u32IrisFNOLin;
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct _ISP_AE_ROUTE_EX_S {
	CVI_U32 u32TotalNum;
	ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

typedef struct _ISP_EXP_INFO_S {
	CVI_U32 u32ExpTime;
	CVI_U32 u32ShortExpTime;
	CVI_U32 u32MedianExpTime;
	CVI_U32 u32LongExpTime;
	CVI_U32 u32AGain;
	CVI_U32 u32DGain;
	CVI_U32 u32ISPDGain;
	CVI_U32 u32Exposure;
	CVI_BOOL bExposureIsMAX;
	CVI_S16 s16HistError;
	CVI_U32 au32AE_Hist1024Value[HIST_NUM];

	CVI_U8 u8AveLum;
	CVI_U32 u32LinesPer500ms;
	CVI_U32 u32PirisFNO;
	CVI_U32 u32Fps;
	CVI_U32 u32ISO;
	CVI_U32 u32ISOCalibrate;
	CVI_U32 u32RefExpRatio;
	CVI_U32 u32FirstStableTime;
	ISP_AE_ROUTE_S stAERoute;
	ISP_AE_ROUTE_EX_S stAERouteEx;
	CVI_U8 u8WDRShortAveLuma;
	CVI_U32 u32WDRExpRatio;
	CVI_U8 u8LEFrameAvgLuma;
	CVI_U8 u8SEFrameAvgLuma;
	CVI_FLOAT fLightValue;
} ISP_EXP_INFO_S;

#define WDR_EXP_RATIO_NUM       (3)
#define WDR_LV_TARGET_NUM		(16)
typedef struct _ISP_WDR_EXPOSURE_ATTR_S {
	ISP_OP_TYPE_E enExpRatioType;
	CVI_U32 au32ExpRatio[WDR_EXP_RATIO_NUM];
	CVI_U32 u32ExpRatioMax;
	CVI_U32 u32ExpRatioMin;
	CVI_U16 u16Tolerance;
	CVI_U16 u16Speed;
	CVI_U16 u16RatioBias;
	CVI_U8 u8SECompensation;
	CVI_U16 u16SEHisThr;
	CVI_U16 u16SEHisCntRatio1;
	CVI_U16 u16SEHisCntRatio2;
	CVI_U32 u16SEHis255CntThr1;
	CVI_U32 u16SEHis255CntThr2;
	CVI_U8 au8LEAdjustTargetMin[WDR_LV_TARGET_NUM];
	CVI_U8 au8LEAdjustTargetMax[WDR_LV_TARGET_NUM];
	CVI_U8 au8SEAdjustTargetMin[WDR_LV_TARGET_NUM];
	CVI_U8 au8SEAdjustTargetMax[WDR_LV_TARGET_NUM];
	CVI_U8 u8AdjustTargetDetectFrmNum;
	CVI_U32 u32DiffPixelNum;
	CVI_U16 u16LELowBinThr;
	CVI_U16 u16LEHighBinThr;
	CVI_U16 u16SELowBinThr;
	CVI_U16 u16SEHighBinThr;
	CVI_U8 au8FrameAvgLumaMin[WDR_LV_TARGET_NUM];
	CVI_U8 au8FrameAvgLumaMax[WDR_LV_TARGET_NUM];
} ISP_WDR_EXPOSURE_ATTR_S;

typedef struct _ISP_AWB_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X;
	CVI_U16 u16Y;
	CVI_U16 u16W;
	CVI_U16 u16H;
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
	CVI_U16 u16ZoneRow;
	CVI_U16 u16ZoneCol;
	CVI_U16 u16ZoneBin;
	CVI_U16 au16HistBinThresh[4];
	CVI_U16 u16WhiteLevel;
	CVI_U16 u16BlackLevel;
	CVI_U16 u16CbMax;
	CVI_U16 u16CbMin;
	CVI_U16 u16CrMax;
	CVI_U16 u16CrMin;
	ISP_AWB_CROP_S stCrop;
} ISP_WB_STATISTICS_CFG_S;

typedef struct _ISP_WB_WIN_STATISTICS_CFG_S {
	CVI_BOOL	bStatisticsEnable;
	ISP_AWB_SWITCH_E		enAWBSwitch;
	CVI_BOOL	bCropEnable;
	CVI_U16		ZoneOffsetX;
	CVI_U16		ZoneOffsetY;
	CVI_U16		ZoneWidth;
	CVI_U16		ZoneHeight;
	CVI_U16		u16BlackLevel;
	CVI_U16		u16WhiteLevel;
	CVI_U8		u8ZoneRow;
	CVI_U8		u8ZoneColumn;
	CVI_BOOL	CornerAvgEnable;
	CVI_U8		CornerSize;
	CVI_U8		StatisticsShiftBits;
} ISP_WB_WIN_STATISTICS_CFG_S;

#define FIR_H_GAIN_NUM 5
#define FIR_V_GAIN_NUM 3
#define AF_GAMMA_NUM 256
// AF window X region must between [0x8 ~ image xsize - 8].
// Y region must between [0x2 ~ image ysize - 2].
typedef struct _ISP_AF_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X;	// Range: must more than 8.
	CVI_U16 u16Y;	// Range: must more than 2.
	CVI_U16 u16W;
	CVI_U16 u16H;
} ISP_AF_CROP_S;

typedef struct _ISP_AF_RAW_CFG_S {
	CVI_U8 PreGammaEn;
	CVI_U8 PreGammaTable[AF_GAMMA_NUM];
} ISP_AF_RAW_CFG_S;

typedef struct _ISP_AF_PRE_FILTER_CFG_S {
	CVI_BOOL PreFltEn;
} ISP_AF_PRE_FILTER_CFG_S;

typedef struct _ISP_AF_CFG_S {
	CVI_BOOL bEnable;
	CVI_U16 u16Hwnd;
	CVI_U16 u16Vwnd;
	CVI_U8 u8HFltShift;
	CVI_S8 s8HVFltLpCoeff[FIR_H_GAIN_NUM];
	ISP_AF_RAW_CFG_S stRawCfg;
	ISP_AF_PRE_FILTER_CFG_S stPreFltCfg;
	ISP_AF_CROP_S stCrop;
	CVI_U16 u16HighLumaTh;
} ISP_AF_CFG_S;

typedef struct _ISP_AF_H_PARAM_S {
	CVI_S8 s8HFltHpCoeff[FIR_H_GAIN_NUM];
} ISP_AF_H_PARAM_S;

typedef struct _ISP_AF_V_PARAM_S {
	CVI_S8 s8VFltHpCoeff[FIR_V_GAIN_NUM];
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
	CVI_U16 u16GlobalR;
	CVI_U16 u16GlobalG;
	CVI_U16 u16GlobalB;
	CVI_U16 u16CountAll;
	CVI_U16 au16ZoneAvgR[AWB_ZONE_NUM];
	CVI_U16 au16ZoneAvgG[AWB_ZONE_NUM];
	CVI_U16 au16ZoneAvgB[AWB_ZONE_NUM];
	CVI_U16 au16ZoneCountAll[AWB_ZONE_NUM];
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

#define AWB_CALIB_PTS_NUM (3)
typedef struct _ISP_AWB_Calibration_Gain_S {
	CVI_U16 u16AvgRgain[AWB_CALIB_PTS_NUM];
	CVI_U16 u16AvgBgain[AWB_CALIB_PTS_NUM];
	CVI_U16 u16ColorTemperature[AWB_CALIB_PTS_NUM];
} ISP_AWB_Calibration_Gain_S;

#define AWB_LUM_HIST_NUM (6)
typedef struct _ISP_AWB_LUM_HISTGRAM_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 au8HistThresh[AWB_LUM_HIST_NUM];
	CVI_U16 au16HistWt[AWB_LUM_HIST_NUM];
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct _ISP_AWB_CT_LIMIT_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U16 u16HighRgLimit;
	CVI_U16 u16HighBgLimit;
	CVI_U16 u16LowRgLimit;
	CVI_U16 u16LowBgLimit;
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct _ISP_AWB_CBCR_TRACK_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U16 au16CrMax[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16CrMin[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16CbMax[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 au16CbMin[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_AWB_CBCR_TRACK_ATTR_S;

#define AWB_CURVE_PARA_NUM  (6)
#define AWB_CURVE_BOUND_NUM	(8)
typedef struct _ISP_AWB_ATTR_S {
	CVI_BOOL bEnable;

	CVI_U16 u16RefColorTemp;
	CVI_U16 au16StaticWB[ISP_BAYER_CHN_NUM];
	CVI_S32 as32CurvePara[AWB_CURVE_PARA_NUM];

	ISP_AWB_ALG_TYPE_E enAlgType;

	CVI_U8 u8RGStrength;
	CVI_U8 u8BGStrength;
	CVI_U16 u16Speed;
	CVI_U16 u16ZoneSel;
	CVI_U16 u16HighColorTemp;
	CVI_U16 u16LowColorTemp;
	ISP_AWB_CT_LIMIT_ATTR_S stCTLimit;
	CVI_BOOL bShiftLimitEn;
	CVI_U8 u8ShiftLimit;
	CVI_BOOL bGainNormEn;
	CVI_BOOL bNaturalCastEn;
	CVI_U8 u8RGainBias;
	CVI_U8 u8BGainBias;

	ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;
	ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
	CVI_BOOL bAWBZoneWtEn;
	CVI_U8 au8ZoneWt[AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN];
	CVI_U16 u16ShiftLimit[AWB_CURVE_BOUND_NUM];
} ISP_AWB_ATTR_S;

typedef struct _ISP_MWB_ATTR_S {
	CVI_U16 u16Rgain;
	CVI_U16 u16Grgain;
	CVI_U16 u16Gbgain;
	CVI_U16 u16Bgain;
} ISP_MWB_ATTR_S;

typedef enum _ISP_AWB_ALG_E {
	ALG_AWB,
	ALG_AWB_SPEC,
	ALG_BUTT
} ISP_AWB_ALG_E;

typedef struct _ISP_WB_ATTR_S {
	CVI_BOOL bByPass;
	CVI_U8 u8AWBRunInterval;
	ISP_OP_TYPE_E enOpType;
	ISP_MWB_ATTR_S stManual;
	ISP_AWB_ATTR_S stAuto;
	ISP_AWB_ALG_E enAlgType;
	CVI_U8	u8DebugMode;
} ISP_WB_ATTR_S;

typedef struct _ISP_WB_INFO_S {
	CVI_U16 u16Rgain;
	CVI_U16 u16Grgain;
	CVI_U16 u16Gbgain;
	CVI_U16 u16Bgain;
	CVI_U16 u16Saturation;
	CVI_U16 u16ColorTemp;
	CVI_U16 au16CCM[CCM_MATRIX_SIZE];
	CVI_U16 u16LS0CT;
	CVI_U16 u16LS1CT;
	CVI_U16 u16LS0Area;
	CVI_U16 u16LS1Area;
	CVI_U8  u8MultiDegree;
	CVI_U16 u16ActiveShift;
	CVI_U32 u32FirstStableTime;
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus;
	CVI_S16	s16Bv;
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
	CVI_U8 lsc_enable;
	CVI_U8 lsc_debug;
	CVI_U8 lsc_xblknumm1;
	CVI_U8 reg_lsc_yblknumm1;
	CVI_U16 reg_lsc_xstep;
	CVI_U16 reg_lsc_ystep;
	CVI_U32 reg_lsc_imgx0;
	CVI_U32 reg_lsc_imgy0;
	CVI_U8 reg_lsc_gain_base;
	CVI_U16 reg_lsc_scaler[256];
	CVI_U8 lscr_enable;
	CVI_U16 lscr_centerx;
	CVI_U16 lscr_centery;
	CVI_U16 lscr_norm;
	ISP_OP_TYPE_E enOpType;
	ISP_MESHSHADING_MANUAL_ATTR_S stManual;
	ISP_MESHSHADING_AUTO_ATTR_S stAuto;
} ISP_MESHSHADING_ATTR_S;

//-----------------------------------------------------------------------------
//	YNR
//-----------------------------------------------------------------------------
#define YNR_CORING_NUM 6
typedef struct _ISP_YNR_MANUAL_ATTR_S {
	CVI_U8 WindowType;
	CVI_U8 DetailSmoothMode;
	CVI_U8 NoiseSuppressStr;
	CVI_U8 FilterType;
	CVI_U8 NoiseSuppressStrMode;
	CVI_U8 NoiseCoringBaseLuma[YNR_CORING_NUM];
	CVI_U8 NoiseCoringBaseOffset[YNR_CORING_NUM];
	CVI_U8 NoiseCoringAdvLuma[YNR_CORING_NUM];
	CVI_U8 NoiseCoringAdvOffset[YNR_CORING_NUM];
} ISP_YNR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStrMode[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseCoringBaseLuma[YNR_CORING_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseCoringBaseOffset[YNR_CORING_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseCoringAdvLuma[YNR_CORING_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseCoringAdvOffset[YNR_CORING_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_YNR_AUTO_ATTR_S;

typedef struct _ISP_YNR_ATTR_S {
	CVI_BOOL Enable;
	CVI_BOOL CoringParamEnable;
	CVI_U8 TuningMode;
	ISP_OP_TYPE_E enOpType;
	ISP_YNR_MANUAL_ATTR_S stManual;
	ISP_YNR_AUTO_ATTR_S stAuto;
} ISP_YNR_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_MANUAL_ATTR_S {
	CVI_U8 MotionThr;
	CVI_U8 MotionNrPosGain;
	CVI_U8 MotionNrNegGain;
} ISP_YNR_MOTION_NR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_AUTO_ATTR_S {
	CVI_U8 MotionThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MotionNrPosGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MotionNrNegGain[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_YNR_MOTION_NR_AUTO_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_YNR_MOTION_NR_MANUAL_ATTR_S stManual;
	ISP_YNR_MOTION_NR_AUTO_ATTR_S stAuto;
} ISP_YNR_MOTION_NR_ATTR_S;

typedef struct _ISP_YNR_FILTER_MANUAL_ATTR_S {
	CVI_U8 LumaStr[8];
	CVI_U8 VarThr;
	CVI_U16 CoringWgtLF;
	CVI_U16 CoringWgtHF;
	CVI_U8 NonDirFiltStr;
	CVI_U8 VhDirFiltStr;
	CVI_U8 AaDirFiltStr;
	CVI_U16 FilterMode;
} ISP_YNR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_YNR_FILTER_AUTO_ATTR_S {
	CVI_U8 LumaStr[8][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 VarThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 FilterMode[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_YNR_FILTER_AUTO_ATTR_S;

typedef struct _ISP_YNR_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_YNR_FILTER_MANUAL_ATTR_S stManual;
	ISP_YNR_FILTER_AUTO_ATTR_S stAuto;
} ISP_YNR_FILTER_ATTR_S;

typedef struct _ISP_DEHAZE_MANUAL_ATTR_S {
	CVI_U8 Strength;
} ISP_DEHAZE_MANUAL_ATTR_S;

typedef struct _ISP_DEHAZE_AUTO_ATTR_S {
	CVI_U8 Strength[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DEHAZE_AUTO_ATTR_S;

typedef struct _ISP_DEHAZE_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	CVI_U16 CumulativeThr;
	CVI_U16 MinTransMapValue;
	ISP_DEHAZE_MANUAL_ATTR_S stManual;
	ISP_DEHAZE_AUTO_ATTR_S stAuto;
} ISP_DEHAZE_ATTR_S;

//-----------------------------------------------------------------------------
//  CNR
//-----------------------------------------------------------------------------
typedef struct _ISP_CNR_MANUAL_ATTR_S {
	CVI_U8 CnrStr;
	CVI_U8 NoiseSuppressStr;
	CVI_U8 NoiseSuppressGain;
	CVI_U8 FilterType;
	CVI_U8 MotionNrStr;
	CVI_U8 LumaWgt;
	CVI_U8 DetailSmoothMode;
} ISP_CNR_MANUAL_ATTR_S;

typedef struct _ISP_CNR_AUTO_ATTR_S {
	CVI_U8 CnrStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MotionNrStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaWgt[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CNR_AUTO_ATTR_S;

typedef struct _ISP_CNR_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_CNR_MANUAL_ATTR_S stManual;
	ISP_CNR_AUTO_ATTR_S stAuto;
} ISP_CNR_ATTR_S;

//-----------------------------------------------------------------------------
//  CAC
//-----------------------------------------------------------------------------
typedef struct _ISP_CAC_MANUAL_ATTR_S {
	CVI_U8 DePurpleStr;
} ISP_CAC_MANUAL_ATTR_S;

typedef struct _ISP_CAC_AUTO_ATTR_S {
	CVI_U8 DePurpleStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CAC_AUTO_ATTR_S;

typedef struct _ISP_CAC_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	CVI_U16 VarThr;
	CVI_U8 PurpleDetRange;
	CVI_U8 PurpleCb;
	CVI_U8 PurpleCr;
	CVI_U8 GreenCb;
	CVI_U8 GreenCr;
	CVI_U8 TuningMode;
	ISP_CAC_MANUAL_ATTR_S stManual;
	ISP_CAC_AUTO_ATTR_S stAuto;
} ISP_CAC_ATTR_S;

#define GAMMA_NODE_NUM 256
#define GAMMA_MAX_INTERPOLATION_NUM 5
typedef enum _ISP_GAMMA_CURVE_TYPE_E {
	ISP_GAMMA_CURVE_DEFAULT,
	ISP_GAMMA_CURVE_SRGB,
	ISP_GAMMA_CURVE_USER_DEFINE,
	ISP_GAMMA_CURVE_MAX
} ISP_GAMMA_CURVE_TYPE_E;

typedef struct _ISP_GAMMA_ATTR_S {
	CVI_BOOL Enable;
	CVI_U16 Table[GAMMA_NODE_NUM];

	ISP_GAMMA_CURVE_TYPE_E enCurveType;
} ISP_GAMMA_ATTR_S;

typedef struct _ISP_GAMMA_CURVE_ATTR_S {
	CVI_S16 Lv;
	CVI_U16 Tbl[GAMMA_NODE_NUM];
} ISP_GAMMA_CURVE_ATTR_S;

typedef struct _ISP_AUTO_GAMMA_ATTR_S {
	CVI_BOOL Enable;
	CVI_U8 GammaTabNum;
	ISP_GAMMA_CURVE_ATTR_S GammaTab[GAMMA_MAX_INTERPOLATION_NUM];
} ISP_AUTO_GAMMA_ATTR_S;

typedef struct _ISP_NR_MANUAL_ATTR_S {
	CVI_U8 WindowType;
	CVI_U8 DetailSmoothMode;
	CVI_U8 NoiseSuppressStr;
	CVI_U8 FilterType;
	CVI_U8 NoiseSuppressStrMode;
} ISP_NR_MANUAL_ATTR_S;

typedef struct _ISP_NR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStrMode[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_NR_AUTO_ATTR_S;

typedef struct _ISP_NR_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_NR_MANUAL_ATTR_S stManual;
	ISP_NR_AUTO_ATTR_S stAuto;
} ISP_NR_ATTR_S;

typedef struct _ISP_NR_FILTER_MANUAL_ATTR_S {
	CVI_U8 NrLscRatio;
	CVI_U8 LumaStr[8];
	CVI_U16 VarThr;
	CVI_U16 CoringWgtLF;
	CVI_U16 CoringWgtHF;
	CVI_U8 NonDirFiltStr;
	CVI_U8 VhDirFiltStr;
	CVI_U8 AaDirFiltStr;
} ISP_NR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_NR_FILTER_AUTO_ATTR_S {
	CVI_U8 NrLscRatio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaStr[8][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 VarThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_NR_FILTER_AUTO_ATTR_S;

typedef struct _ISP_NR_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	CVI_U8 TuningMode;
	ISP_NR_FILTER_MANUAL_ATTR_S stManual;
	ISP_NR_FILTER_AUTO_ATTR_S stAuto;
} ISP_NR_FILTER_ATTR_S;

typedef struct _ISP_RLSC_MANUAL_ATTR_S {
	CVI_U16 RadialStr;
} ISP_RLSC_MANUAL_ATTR_S;

typedef struct _ISP_RLSC_AUTO_ATTR_S {
	CVI_U16 RadialStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_RLSC_AUTO_ATTR_S;

typedef struct _ISP_RLSC_ATTR_S {
	CVI_BOOL RlscEnable;
	CVI_U16 RlscCenterX;
	CVI_U16 RlscCenterY;
	ISP_OP_TYPE_E enOpType;
	ISP_RLSC_MANUAL_ATTR_S stManual;
	ISP_RLSC_AUTO_ATTR_S stAuto;
} ISP_RLSC_ATTR_S;

typedef struct _ISP_DPC_MANUAL_ATTR_S {
	CVI_BOOL dpc_enable;
	CVI_U8 dpc_staticdpc_enable;
	CVI_U8 dpc_cluster_size;
	CVI_U16 dpc_r_bright_pixel_ratio;
	CVI_U16 dpc_g_bright_pixel_ratio;
	CVI_U16 dpc_b_bright_pixel_ratio;
	CVI_U16 dpc_r_dark_pixel_ratio;
	CVI_U16 dpc_g_dark_pixel_ratio;
	CVI_U16 dpc_b_dark_pixel_ratio;
	CVI_U8 dpc_r_dark_pixel_mindiff;
	CVI_U8 dpc_g_dark_pixel_mindiff;
	CVI_U8 dpc_b_dark_pixel_mindiff;
	CVI_U8 dpc_flat_thre_r;
	CVI_U8 dpc_flat_thre_g;
	CVI_U8 dpc_flat_thre_b;
	CVI_U8 dpc_flat_thre_min_g;
	CVI_U8 dpc_flat_thre_min_rb;
} ISP_DPC_MANUAL_ATTR_S;

typedef struct _ISP_DPC_AUTO_ATTR_S {
	CVI_U8 dpc_enable[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_staticdpc_enable[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_cluster_size[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_r_bright_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_g_bright_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_b_bright_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_r_dark_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_g_dark_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 dpc_b_dark_pixel_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_r_dark_pixel_mindiff[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_g_dark_pixel_mindiff[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_b_dark_pixel_mindiff[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_flat_thre_r[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_flat_thre_g[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_flat_thre_b[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_flat_thre_min_g[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 dpc_flat_thre_min_rb[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DPC_AUTO_ATTR_S;

typedef struct _ISP_DPC_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DPC_MANUAL_ATTR_S stManual;
	ISP_DPC_AUTO_ATTR_S stAuto;
} ISP_DPC_ATTR_S;

//-----------------------------------------------------------------------------
//  Sharpen
//-----------------------------------------------------------------------------
#define SHARPEN_LUT_NUM 33
typedef struct _ISP_SHARPEN_MANUAL_ATTR_S {
	CVI_U8 EdgeFreq;
	CVI_U8 TextureFreq;
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM];
	CVI_U8 GlobalGain;
	CVI_U8 OverShootThr;
	CVI_U8 UnderShootThr;
	CVI_U8 YNoiseLevel;
	CVI_U16 LumaAdpCoring[SHARPEN_LUT_NUM];
} ISP_SHARPEN_MANUAL_ATTR_S;

typedef struct _ISP_SHARPEN_AUTO_ATTR_S {
	CVI_U8 EdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 TextureFreq[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 OverShootThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 UnderShootThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 YNoiseLevel[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 LumaAdpCoring[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct _ISP_SHARPEN_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;

	CVI_U8 TuningMode;
	CVI_U8 TuningModeLevelShift;
	CVI_U8 EdgeGain;
	CVI_U16 TextureGain;
	CVI_U8 EdgeThr;
	CVI_U8 TextureThr;
	CVI_BOOL LumaAdpCoringEn;
	CVI_BOOL LumaAdpGainEn;
	CVI_BOOL DeltaAdpGainEn;
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM];
	CVI_BOOL WdrCoringCompensationEn;
	CVI_U8 WdrCoringCompensationMode;
	CVI_U16 WdrCoringToleranceLevel;
	CVI_U8 WdrCoringHighThr;
	CVI_U8 WdrCoringLowThr;

	ISP_SHARPEN_MANUAL_ATTR_S stManual;
	ISP_SHARPEN_AUTO_ATTR_S stAuto;
} ISP_SHARPEN_ATTR_S;

typedef struct _ISP_TNR_MANUAL_ATTR_S {
	CVI_U8 MergeMode;
	CVI_U8 MergeModeAlpha;
	CVI_U8 TnrStrength0;
	CVI_U8 MapThdLow0;
	CVI_U8 MapThdHigh0;
	CVI_U8 TnrStrength1;
	CVI_U8 MapThdLow1;
	CVI_U8 MapThdHigh1;
	CVI_S16 BrightnessNoiseLevelLE;
	CVI_S16 BrightnessNoiseLevelSE;
} ISP_TNR_MANUAL_ATTR_S;

typedef struct _ISP_TNR_AUTO_ATTR_S {
	CVI_U8 MergeMode[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MergeModeAlpha[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 TnrStrength0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MapThdLow0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MapThdHigh0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 TnrStrength1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MapThdLow1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MapThdHigh1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_S16 BrightnessNoiseLevelLE[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_S16 BrightnessNoiseLevelSE[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_TNR_AUTO_ATTR_S;

typedef struct _ISP_TNR_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	CVI_BOOL TuningMode;
	CVI_U8 DeflickerMode;
	CVI_U16 DeflickerToleranceLevel;
	ISP_TNR_MANUAL_ATTR_S stManual;
	ISP_TNR_AUTO_ATTR_S stAuto;
} ISP_TNR_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S {
	CVI_U8 RNoiseLevel0;
	CVI_U8 GNoiseLevel0;
	CVI_U8 BNoiseLevel0;
	CVI_U8 RNoiseLevel1;
	CVI_U8 GNoiseLevel1;
	CVI_U8 BNoiseLevel1;
	CVI_U8 RNoiseHiLevel0;
	CVI_U8 GNoiseHiLevel0;
	CVI_U8 BNoiseHiLevel0;
	CVI_U8 RNoiseHiLevel1;
	CVI_U8 GNoiseHiLevel1;
	CVI_U8 BNoiseHiLevel1;
} ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_AUTO_ATTR_S {
	CVI_U8 RNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 RNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 RNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 RNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_TNR_NOISE_MODEL_AUTO_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S stManual;
	ISP_TNR_NOISE_MODEL_AUTO_ATTR_S stAuto;
} ISP_TNR_NOISE_MODEL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S {
	CVI_U16 L2mIn0[4];
	CVI_U8 L2mOut0[4];
	CVI_U16 L2mIn1[4];
	CVI_U8 L2mOut1[4];
} ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_AUTO_ATTR_S {
	CVI_U16 L2mIn0[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 L2mOut0[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 L2mIn1[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 L2mOut1[4][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_TNR_LUMA_MOTION_AUTO_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S stManual;
	ISP_TNR_LUMA_MOTION_AUTO_ATTR_S stAuto;
} ISP_TNR_LUMA_MOTION_ATTR_S;

typedef struct _ISP_TNR_GHOST_MANUAL_ATTR_S {
	CVI_U8 PrtctIn0[4];
	CVI_U8 PrtctOut0[4];
	CVI_U8 PrtctIn1[4];
	CVI_U8 PrtctOut1[4];
} ISP_TNR_GHOST_MANUAL_ATTR_S;

typedef struct _ISP_TNR_GHOST_AUTO_ATTR_S {
	CVI_U8 PrtctIn0[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 PrtctOut0[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 PrtctIn1[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 PrtctOut1[4][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_TNR_GHOST_AUTO_ATTR_S;

typedef struct _ISP_TNR_GHOST_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_GHOST_MANUAL_ATTR_S stManual;
	ISP_TNR_GHOST_AUTO_ATTR_S stAuto;
} ISP_TNR_GHOST_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_MANUAL_ATTR_S {
	CVI_U8 LowMtPrtLevel;
	CVI_U8 LowMtPrtIn[4];
	CVI_U8 LowMtPrtOut[4];
	CVI_S16 LowMtPrtSlope0[3];
} ISP_TNR_MT_PRT_MANUAL_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_AUTO_ATTR_S {
	CVI_U8 LowMtPrtLevel[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LowMtPrtIn[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LowMtPrtOut[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_S16 LowMtPrtSlope0[3][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_TNR_MT_PRT_AUTO_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_ATTR_S {
	CVI_U8 LowMtPrtEn;
	ISP_TNR_MT_PRT_MANUAL_ATTR_S stManual;
	ISP_TNR_MT_PRT_AUTO_ATTR_S stAuto;
} ISP_TNR_MT_PRT_ATTR_S;

typedef struct _ISP_CLUT_MANUAL_ATTR_S {
} ISP_CLUT_MANUAL_ATTR_S;

typedef struct _ISP_CLUT_AUTO_ATTR_S {
} ISP_CLUT_AUTO_ATTR_S;

typedef struct _ISP_CLUT_ATTR_S {
	CVI_U8 hsv3dlut_enable;
	CVI_U8 hsv3dlut_h_clamp_wrap_opt;
} ISP_CLUT_ATTR_S;

//-----------------------------------------------------------------------------
//  BlackLevel
//-----------------------------------------------------------------------------
typedef struct _ISP_BLACK_LEVEL_MANUAL_ATTR_S {
	CVI_U16 OffsetR;
	CVI_U16 OffsetGr;
	CVI_U16 OffsetGb;
	CVI_U16 OffsetB;
	CVI_U16 GainR;
	CVI_U16 GainGr;
	CVI_U16 GainGb;
	CVI_U16 GainB;
} ISP_BLACK_LEVEL_MANUAL_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_AUTO_ATTR_S {
	CVI_U16 OffsetR[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 OffsetGr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 OffsetGb[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 OffsetB[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 GainR[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 GainGr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 GainGb[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 GainB[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_BLACK_LEVEL_AUTO_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_ATTR_S {
	CVI_U8 Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_BLACK_LEVEL_MANUAL_ATTR_S stManual;
	ISP_BLACK_LEVEL_AUTO_ATTR_S stAuto;
} ISP_BLACK_LEVEL_ATTR_S;

typedef struct _ISP_COLOR_TONE_MANUAL_ATTR_S {
} ISP_COLOR_TONE_MANUAL_ATTR_S;

typedef struct _ISP_COLOR_TONE_AUTO_ATTR_S {
} ISP_COLOR_TONE_AUTO_ATTR_S;

typedef struct _ISP_COLOR_TONE_ATTR_S {
	CVI_BOOL wbg_enable; // TODO@Kidd remove this
	CVI_U16 u16RedCastGain;
	CVI_U16 u16GreenCastGain;
	CVI_U16 u16BlueCastGain;
} ISP_COLOR_TONE_ATTR_S;

//-----------------------------------------------------------------------------
//  DCI
//-----------------------------------------------------------------------------
typedef struct _ISP_DCI_MANUAL_ATTR_S {
	CVI_U16 ContrastGain;
	CVI_U8 BlcThr;
	CVI_U8 WhtThr;
	CVI_U16 BlcCtrl;
	CVI_U16 WhtCtrl;
} ISP_DCI_MANUAL_ATTR_S;

typedef struct _ISP_DCI_AUTO_ATTR_S {
	CVI_U16 ContrastGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BlcThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 WhtThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 WhtCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DCI_AUTO_ATTR_S;

typedef struct _ISP_DCI_ATTR_S {
	CVI_BOOL Enable;
	CVI_BOOL TuningMode;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 Method;
	CVI_U32 Speed;
	CVI_U16 DciStrength;
	ISP_DCI_MANUAL_ATTR_S stManual;
	ISP_DCI_AUTO_ATTR_S stAuto;
} ISP_DCI_ATTR_S;

//-----------------------------------------------------------------------------
//  MeshShading
//-----------------------------------------------------------------------------
typedef enum _ISP_MLSC_COLOR_TEMPERATURE {
	ISP_MLSC_COLOR_TEMPERATURE_0,
	ISP_MLSC_COLOR_TEMPERATURE_1,
	ISP_MLSC_COLOR_TEMPERATURE_2,
	ISP_MLSC_COLOR_TEMPERATURE_3,
	ISP_MLSC_COLOR_TEMPERATURE_4,
	ISP_MLSC_COLOR_TEMPERATURE_5,
	ISP_MLSC_COLOR_TEMPERATURE_6,
	ISP_MLSC_COLOR_TEMPERATURE_SIZE,
} ISP_MLSC_COLOR_TEMPERATURE;

typedef struct _ISP_MESH_SHADING_MANUAL_ATTR_S {
	CVI_U16 MeshStr;
} ISP_MESH_SHADING_MANUAL_ATTR_S;

typedef struct _ISP_MESH_SHADING_AUTO_ATTR_S {
	CVI_U16 MeshStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_MESH_SHADING_AUTO_ATTR_S;

typedef struct _ISP_MESH_SHADING_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_MESH_SHADING_MANUAL_ATTR_S stManual;
	ISP_MESH_SHADING_AUTO_ATTR_S stAuto;
} ISP_MESH_SHADING_ATTR_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_S {
	CVI_U16 RGain[CVI_ISP_LSC_GRID_POINTS];
	CVI_U16 GGain[CVI_ISP_LSC_GRID_POINTS];
	CVI_U16 BGain[CVI_ISP_LSC_GRID_POINTS];
} ISP_MESH_SHADING_GAIN_LUT_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_ATTR_S {
	CVI_U8 Size;
	CVI_U16 ColorTemperature[ISP_MLSC_COLOR_TEMPERATURE_SIZE];
	ISP_MESH_SHADING_GAIN_LUT_S LscGainLut[ISP_MLSC_COLOR_TEMPERATURE_SIZE];
} ISP_MESH_SHADING_GAIN_LUT_ATTR_S;

//-----------------------------------------------------------------------------
//  RadialShading
//-----------------------------------------------------------------------------
typedef enum _ISP_RLSC_COLOR_TEMPERATURE {
	ISP_RLSC_COLOR_TEMPERATURE_LOW,
	ISP_RLSC_COLOR_TEMPERATURE_HIGH,
	ISP_RLSC_COLOR_TEMPERATURE_SIZE,
} ISP_RLSC_COLOR_TEMPERATURE;

#define ISP_RLSC_WINDOW_SIZE 32
typedef struct _ISP_RADIAL_SHADING_ATTR_S {
	CVI_BOOL Enable;
	CVI_U16 CenterX;
	CVI_U16 CenterY;
} ISP_RADIAL_SHADING_ATTR_S;

typedef struct _ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S {
	CVI_U16 RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_SIZE][ISP_RLSC_WINDOW_SIZE];
} ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S;

//-----------------------------------------------------------------------------
//  Demosaic
//-----------------------------------------------------------------------------
typedef struct _ISP_DEMOSAIC_MANUAL_ATTR_S {
	CVI_U16 CoarseEdgeThr;
	CVI_U16 CoarseStr;
	CVI_U16 FineEdgeThr;
	CVI_U16 FineStr;
} ISP_DEMOSAIC_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_AUTO_ATTR_S {
	CVI_U16 CoarseEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 CoarseStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 FineEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 FineStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DEMOSAIC_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_ATTR_S {
	CVI_BOOL Enable;
	CVI_BOOL TuningMode;
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S {
	CVI_BOOL DetailSmoothEnable;
	CVI_U8 DetailSmoothStr;
	CVI_U8 EdgeWgtStr;
} ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S {
	CVI_BOOL DetailSmoothEnable[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailSmoothStr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 EdgeWgtStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_DEMOIRE_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S {
	CVI_U16 DetailWgtThr;
	CVI_U8 DetailWgtSlope;
	CVI_U8 DetailWgtMin;
	CVI_U8 DetailWgtMax;
	CVI_U16 LumaWgtThr;
	CVI_U8 LumaWgtSlope;
	CVI_U8 LumaWgtMin;
	CVI_U8 LumaWgtMax;
} ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_AUTO_ATTR_S {
	CVI_U16 DetailWgtThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailWgtMin[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DetailWgtMax[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 LumaWgtThr[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaWgtMin[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 LumaWgtMax[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DEMOSAIC_FILTER_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_FILTER_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_FILTER_ATTR_S;

typedef struct _ISP_DEMOSAIC_EE_MANUAL_ATTR_S {
	CVI_BOOL EdgeEnhanceEnable;
	CVI_U16 OverShtGain;
	CVI_U16 UnderShtGain;
	CVI_U8 HFGainTable[33];
	CVI_U8 GainTable[33];
	CVI_U8 NoiseSuppressStr;
} ISP_DEMOSAIC_EE_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_EE_AUTO_ATTR_S {
	CVI_BOOL EdgeEnhanceEnable[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 OverShtGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 UnderShtGain[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 HFGainTable[33][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 GainTable[33][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DEMOSAIC_EE_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_EE_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_EE_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_EE_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_EE_ATTR_S;

//-----------------------------------------------------------------------------
//  CCM
//-----------------------------------------------------------------------------
// Saturation
typedef struct _ISP_SATURATION_MANUAL_ATTR_S {
	CVI_U8 Saturation;
} ISP_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_SATURATION_AUTO_ATTR_S {
	CVI_U8 Saturation[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_SATURATION_ATTR_SS {
	ISP_OP_TYPE_E enOpType;
	ISP_SATURATION_AUTO_ATTR_S stAuto;
	ISP_SATURATION_MANUAL_ATTR_S stManual;
} ISP_SATURATION_ATTR_S;

// CCM
typedef struct _ISP_COLORMATRIX_ATTR_S {
	CVI_U16 ColorTemp;
	CVI_S16 CCM[9];
} ISP_COLORMATRIX_ATTR_S;

typedef struct _ISP_CCM_MANUAL_ATTR_S {
	CVI_U8 SatEnable;
	CVI_S16 CCM[9];
} ISP_CCM_MANUAL_ATTR_S;

typedef struct _ISP_CCM_AUTO_ATTR_S {
	CVI_U8 ISOActEnable;
	CVI_U8 TempActEnable;
	CVI_U8 CCMTabNum;
	ISP_COLORMATRIX_ATTR_S CCMTab[7];
} ISP_CCM_AUTO_ATTR_S;

typedef struct _ISP_CCM_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_CCM_AUTO_ATTR_S stAuto;
	ISP_CCM_MANUAL_ATTR_S stManual;
} ISP_CCM_ATTR_S;

//-----------------------------------------------------------------------------
//  HSV
//-----------------------------------------------------------------------------
typedef struct _ISP_HSV_MANUAL_ATTR_S {
	CVI_U8 AllSatStrength;
	CVI_U16 AllSatCoring;
	CVI_U16 SatCoringLinearTh;
	CVI_U16 SatCoringLinearLmt;
} ISP_HSV_MANUAL_ATTR_S;

typedef struct _ISP_HSV_AUTO_ATTR_S {
	CVI_U8 AllSatStrength[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 AllSatCoring[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 SatCoringLinearTh[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 SatCoringLinearLmt[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_HSV_AUTO_ATTR_S;

typedef struct _ISP_HSV_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_HSV_AUTO_ATTR_S stAuto;
	ISP_HSV_MANUAL_ATTR_S stManual;
} ISP_HSV_ATTR_S;

//-----------------------------------------------------------------------------
//  DPC
//-----------------------------------------------------------------------------
typedef struct _ISP_DP_DYNAMIC_MANUAL_ATTR_S {
	CVI_U8 ClusterSize;
	CVI_U8 BrightDefectToNormalPixRatio;
	CVI_U8 DarkDefectToNormalPixRatio;
	CVI_U8 FlatThreR;
	CVI_U8 FlatThreG;
	CVI_U8 FlatThreB;
	CVI_U8 FlatThreMinG;
	CVI_U8 FlatThreMinRB;
} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_AUTO_ATTR_S {
	CVI_U8 ClusterSize[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 BrightDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 DarkDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FlatThreR[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FlatThreG[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FlatThreB[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FlatThreMinG[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 FlatThreMinRB[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_ATTR_S {
	CVI_BOOL Enable;
	// CVI_U32 StaticDpcEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
	ISP_DP_DYNAMIC_AUTO_ATTR_S stAuto;
} ISP_DP_DYNAMIC_ATTR_S;

#define STATIC_DP_COUNT_MAX 4096
typedef struct _ISP_DP_STATIC_ATTR_S {
	CVI_BOOL Enable;
	CVI_U16 BrightCount;
	CVI_U16 DarkCount;
	CVI_U32 BrightTable[STATIC_DP_COUNT_MAX];
	CVI_U32 DarkTable[STATIC_DP_COUNT_MAX];
	CVI_BOOL Show;	// not support yet
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
	CVI_U8 StartThresh;
	CVI_U16 CountMax;
	CVI_U16 CountMin;
	CVI_U16 TimeLimit;
	CVI_BOOL saveFileEn;

	// read only
	CVI_U32 Table[STATIC_DP_COUNT_MAX];
	CVI_U8 FinishThresh;
	CVI_U16 Count;
	ISP_STATUS_E Status;
} ISP_DP_CALIB_ATTR_S;

//-----------------------------------------------------------------------------
//  Crosstalk
//-----------------------------------------------------------------------------
typedef struct _ISP_CROSSTALK_MANUAL_ATTR_S {
	CVI_U16 Strength;
} ISP_CROSSTALK_MANUAL_ATTR_S;

typedef struct _ISP_CROSSTALK_AUTO_ATTR_S {
	CVI_U16 Strength[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CROSSTALK_AUTO_ATTR_S;

typedef struct _ISP_CROSSTALK_ATTR_S {
	CVI_BOOL Enable;
	CVI_U16 GrGbDiffThreSec[4];
	CVI_U16 FlatThre[4];
	ISP_OP_TYPE_E enOpType;
	ISP_CROSSTALK_MANUAL_ATTR_S stManual;
	ISP_CROSSTALK_AUTO_ATTR_S stAuto;
} ISP_CROSSTALK_ATTR_S;

//-----------------------------------------------------------------------------
//  FSWDR
//-----------------------------------------------------------------------------
typedef struct _ISP_FSWDR_MANUAL_ATTR_S {
	CVI_U8 MergeMode;
	CVI_U8 MergeModeAlpha;
	CVI_U16 WDRCombineShortThr;
	CVI_U16 WDRCombineLongThr;
	CVI_U8 WDRCombineMinWeight;
	CVI_U8 WDRCombineMaxWeight;
} ISP_FSWDR_MANUAL_ATTR_S;

typedef struct _ISP_FSWDR_AUTO_ATTR_S {
	CVI_U8 MergeMode[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U8 MergeModeAlpha[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 WDRCombineShortThr[ISP_AUTO_LV_NUM];
	CVI_U16 WDRCombineLongThr[ISP_AUTO_LV_NUM];
	CVI_U8 WDRCombineMinWeight[ISP_AUTO_LV_NUM];
	CVI_U8 WDRCombineMaxWeight[ISP_AUTO_LV_NUM];
} ISP_FSWDR_AUTO_ATTR_S;

typedef struct _ISP_FSWDR_ATTR_S {
	CVI_BOOL Enable;
	CVI_BOOL MotionCompEnable;
	CVI_BOOL WDRCombineSNRAwareEn;
	CVI_U16 WDRCombineSNRAwareSmoothLevel;
	CVI_U16 WDRCombineSNRAwareLowThr;
	CVI_U16 WDRCombineSNRAwareHighThr;
	CVI_U8 WDRCombineSNRAwareToleranceLevel;
	CVI_U8 TuningMode;
	ISP_OP_TYPE_E enOpType;
	ISP_FSWDR_MANUAL_ATTR_S stManual;
	ISP_FSWDR_AUTO_ATTR_S stAuto;
} ISP_FSWDR_ATTR_S;

//-----------------------------------------------------------------------------
//  DRC
//-----------------------------------------------------------------------------
typedef struct _ISP_DRC_MANUAL_ATTR_S {
	CVI_U16 NLDetailDarkGain[4];
	CVI_S32 NLDetailDarkAmplitude[4];
	CVI_U16 NLDetailBrightGain[4];
	CVI_S32 NLDetailBrightAmplitude[4];
	CVI_U32 TargetYScale;
	CVI_U32 SETargetYScale;
	CVI_U32 LETargetYScale;
	CVI_U16 ContrastDarkMinThrd;
	CVI_U16 ContrastDarkMaxThrd;
	CVI_U8 ContrastDarkMinWeight;
	CVI_U8 ContrastDarkMaxWeight;
	CVI_U16 ContrastBrightMinThrd;
	CVI_U16 ContrastBrightMaxThrd;
	CVI_U8 ContrastBrightMinWeight;
	CVI_U8 ContrastBrightMaxWeight;
} ISP_DRC_MANUAL_ATTR_S;

typedef struct _ISP_DRC_AUTO_ATTR_S {
	CVI_U16 NLDetailDarkGain[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_S32 NLDetailDarkAmplitude[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 NLDetailBrightGain[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_S32 NLDetailBrightAmplitude[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U32 TargetYScale[ISP_AUTO_LV_NUM];
	CVI_U32 SETargetYScale[ISP_AUTO_LV_NUM];
	CVI_U32 LETargetYScale[ISP_AUTO_LV_NUM];
	CVI_U16 ContrastDarkMinThrd[ISP_AUTO_LV_NUM];
	CVI_U16 ContrastDarkMaxThrd[ISP_AUTO_LV_NUM];
	CVI_U8 ContrastDarkMinWeight[ISP_AUTO_LV_NUM];
	CVI_U8 ContrastDarkMaxWeight[ISP_AUTO_LV_NUM];
	CVI_U16 ContrastBrightMinThrd[ISP_AUTO_LV_NUM];
	CVI_U16 ContrastBrightMaxThrd[ISP_AUTO_LV_NUM];
	CVI_U8 ContrastBrightMinWeight[ISP_AUTO_LV_NUM];
	CVI_U8 ContrastBrightMaxWeight[ISP_AUTO_LV_NUM];
} ISP_DRC_AUTO_ATTR_S;

typedef struct _ISP_DRC_ATTR_S {
	CVI_BOOL Enable;
	CVI_BOOL NLDetailEnhanceEn;
	CVI_U8 TuningMode;
	CVI_U8 ToneCurveSelect;
	CVI_U8 CoarseFltScale;
	CVI_U16 CurveUserDefine[769];
	CVI_U16 BrightUserDefine[513];
	CVI_U16 DarkUserDefine[257];
	CVI_U32 DRCMu[32];
	CVI_U8 DRangeFltScale;
	CVI_U8 DRangeFltCoef;
	CVI_U8 SpatialFltScale;
	CVI_U8 SpatialFltCoef;
	CVI_U32 TargetYLowBound;
	CVI_U32 TargetYHighBound;
	CVI_U32 SETargetYLowBound;
	CVI_U32 SETargetYHighBound;
	CVI_U32 LETargetYLowBound;
	CVI_U32 LETargetYHighBound;
	CVI_U32 LocalToneEn;
	CVI_U32 ToneCurveSmooth;
	CVI_U16 HdrStrength;
	CVI_U8 PwlEn;
	CVI_U16 LinearStart;
	CVI_U16 LinearEnd;
	CVI_U16 DarkOffset;
	ISP_OP_TYPE_E enOpType;
	ISP_DRC_MANUAL_ATTR_S stManual;
	ISP_DRC_AUTO_ATTR_S stAuto;
} ISP_DRC_ATTR_S;

typedef struct _ISP_MONO_ATTR_S {
	CVI_BOOL Enable;
} ISP_MONO_ATTR_S;

#define MAX_VPSS_ADJUSTMENT_GRP 4
typedef struct _VPSS_ATTR_S {
	CVI_S32 brightness[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 contrast[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 saturation[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 hue[MAX_VPSS_ADJUSTMENT_GRP];
} VPSS_ADJUSTMENT_ATTR_S;

typedef struct _ISP_INNER_STATE_INFO_S {
	CVI_U32 wdrHistBinNum;
	CVI_U32 blcOffsetR;
	CVI_U32 blcOffsetGr;
	CVI_U32 blcOffsetGb;
	CVI_U32 blcOffsetB;
	CVI_U32 blcGainR;
	CVI_U32 blcGainGr;
	CVI_U32 blcGainGb;
	CVI_U32 blcGainB;
	CVI_S32 ccm[9];
	CVI_U32 wdrHistogramBefore[MAX_HIST_BINS * MAX_HIST_BINS];
	CVI_U32 wdrHistogramAfter[MAX_HIST_BINS * MAX_HIST_BINS];
	CVI_U32 *pWdrHistogram;
	CVI_U32 *pWdrHistogramResult;
	CVI_U32 drcDarkTone[LTM_DARK_CURVE_NODE_NUM];
	CVI_U32 drcBrightTone[LTM_BRIGHT_CURVE_NODE_NUM];
} ISP_INNER_STATE_INFO_S;

// Backward Compatible
#include "cvi_isp_backwardcompatible.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_COMM_ISP_H__ */
