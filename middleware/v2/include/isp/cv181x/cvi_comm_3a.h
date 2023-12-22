/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: include/cvi_common_3a.h
 * Description:
 */

#ifndef __CVI_COMM_3A_H__
#define __CVI_COMM_3A_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdbool.h>
#include "cvi_comm_isp.h"
#include "cvi_comm_sns.h"

#define ALG_LIB_NAME_SIZE_MAX (20)
#define MAX_REGISTER_ALG_LIB_NUM 4

#define HIST_THRESH_NUM (4)

#define MAX_AE_W (32)
#define MAX_AE_H (24)
#define MAX_AE_SIZE (MAX_AE_W * MAX_AE_H)
#define MAX_AE_WIN (2)

#define ENABLE_AF_LIB	(0)

typedef enum _ISP_FSWDR_MODE_E {
	ISP_FSWDR_NORMAL_MODE = 0x0,
	ISP_FSWDR_LONG_FRAME_MODE = 0x1,
	ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,
	ISP_FSWDR_MODE_BUTT
} ISP_FSWDR_MODE_E;

typedef struct _ISP_AE_RANGE_S {
	CVI_U32 u32Max; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32Min; /*RW; Range:[0x0, 0x7FFFFFFF]*/
} ISP_AE_RANGE_S;

typedef enum _ISP_AE_STRATEGY_E {
	AE_EXP_HIGHLIGHT_PRIOR = 0,
	AE_EXP_LOWLIGHT_PRIOR = 1,
	AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef enum _ISP_AE_MODE_E {
	AE_MODE_SLOW_SHUTTER = 0,
	AE_MODE_FIX_FRAME_RATE = 1,
	AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum _ISP_ANTIFLICKER_MODE_E {
	ISP_ANTIFLICKER_NORMAL_MODE = 0,
	ISP_ANTIFLICKER_AUTO_MODE = 1,
	ISP_ANTIFLICKER_MODE_BUTT
} ISP_ANTIFLICKER_MODE_E;


typedef struct _ISP_ANTIFLICKER_S {
	CVI_BOOL bEnable;
	ISP_AE_ANTIFLICKER_FREQUENCE_E enFrequency;
	ISP_ANTIFLICKER_MODE_E enMode;
} ISP_ANTIFLICKER_S;

typedef struct _ISP_SUBFLICKER_S {
	CVI_BOOL bEnable;
	CVI_U8 u8LumaDiff; /*RW; Range:[0x0, 0x64]*/
} ISP_SUBFLICKER_S;

typedef struct _ISP_AE_DELAY_S {
	CVI_U16 u16BlackDelayFrame;
	CVI_U16 u16WhiteDelayFrame;
} ISP_AE_DELAY_S;

typedef struct _ISP_ME_ATTR_S {
	ISP_OP_TYPE_E enExpTimeOpType;
	ISP_OP_TYPE_E enAGainOpType;
	ISP_OP_TYPE_E enDGainOpType;
	ISP_OP_TYPE_E enISPDGainOpType;
	CVI_U32 u32ExpTime; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U32 u32AGain; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	CVI_U32 u32DGain; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	CVI_U32 u32ISPDGain; /*RW; Range:[0x400, 0x40000]*/
	ISP_OP_TYPE_E enISONumOpType;
	ISP_AE_GAIN_TYPE_E enGainType;
	CVI_U32 u32ISONum; /*RW; Range:[0x64, 0x7FFFFFFF]*/
} ISP_ME_ATTR_S;

#define RESERVE_SIZE	10
typedef struct _ISP_AE_ATTR_S {
	ISP_AE_RANGE_S stExpTimeRange;
	ISP_AE_RANGE_S stAGainRange; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	ISP_AE_RANGE_S stDGainRange; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	ISP_AE_RANGE_S stISPDGainRange; /*RW; Range:[0x400, 0x40000]*/
	ISP_AE_RANGE_S stSysGainRange; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	CVI_U32 u32GainThreshold; /*RW; Range:[0x400, 0x7FFFFFFF]*/
	CVI_U8 u8Speed; // 1 means 0.1 ev
	CVI_U16 u16BlackSpeedBias;
	CVI_U8 u8Tolerance;
	CVI_U8 u8Compensation;
	CVI_U16 u16EVBias;
	ISP_AE_STRATEGY_E enAEStrategyMode;
	CVI_U16 u16HistRatioSlope;
	CVI_U8 u8MaxHistOffset;
	ISP_AE_MODE_E enAEMode;
	ISP_ANTIFLICKER_S stAntiflicker;
	ISP_SUBFLICKER_S stSubflicker;
	ISP_AE_DELAY_S stAEDelayAttr;
	CVI_BOOL bManualExpValue;
	CVI_U32 u32ExpValue; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	ISP_FSWDR_MODE_E enFSWDRMode;
	CVI_BOOL bWDRQuick;
	CVI_U16 u16ISOCalCoef;
	ISP_AE_GAIN_TYPE_E enGainType;
	ISP_AE_RANGE_S stISONumRange; /*RW; Range:[0x64, 0x7FFFFFFF]*/
	CVI_S16 s16IRCutOnLv;
	CVI_S16	s16IRCutOffLv;
	ISP_AE_IR_CUT_FORCE_STATUS enIRCutStatus;
	CVI_U8 au8AdjustTargetMin[LV_TOTAL_NUM];
	CVI_U8 au8AdjustTargetMax[LV_TOTAL_NUM];
	CVI_U16 u16LowBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 u16HighBinThr; /*RW; Range:[0x0, 0x100]*/
	CVI_BOOL bEnableFaceAE;
	CVI_U8 u8FaceTargetLuma; /*RW; Range:[0x1, 0xFF]*/
	CVI_U8 u8FaceWeight; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 u8GridBvWeight; /*RW; Range:[0x0, 0x64]*/
	CVI_U32	au32Reserve[RESERVE_SIZE]; /*RW; Range:[0x0, 0x7FFFFFFF]*/
	CVI_U8 u8HighLightLumaThr;
	CVI_U8 u8HighLightBufLumaThr;
	CVI_U8 u8LowLightLumaThr;
	CVI_U8 u8LowLightBufLumaThr;
	CVI_BOOL bHistogramAssist;
} ISP_AE_ATTR_S;


typedef struct _ISP_EXPOSURE_ATTR_S {
	CVI_BOOL bByPass;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 u8AERunInterval; /*RW; Range:[0x1, 0xFF]*/
	CVI_BOOL bHistStatAdjust;
	CVI_BOOL bAERouteExValid;
	ISP_ME_ATTR_S stManual;
	ISP_AE_ATTR_S stAuto;
	CVI_U8	u8DebugMode;
	ISP_AE_METER_MODE_E	enMeterMode;
	CVI_BOOL	bAEGainSepCfg;
} ISP_EXPOSURE_ATTR_S;

typedef struct ISP_STITCH_ATTR_T {
	CVI_BOOL enable;
} ISP_STITCH_ATTR_S;

typedef struct _ALG_LIB_S {
	CVI_S32 s32Id;
	CVI_CHAR acLibName[ALG_LIB_NAME_SIZE_MAX];
} ALG_LIB_S;

/* the init param of awb alg */
typedef struct _ISP_AE_PARAM_S {
	SENSOR_ID SensorId;
	CVI_U8 u8WDRMode;
	CVI_U8 u8HDRMode;
	CVI_U16 u16BlackLevel;
	CVI_FLOAT f32Fps;
	ISP_BAYER_FORMAT_E enBayer;
	ISP_STITCH_ATTR_S stStitchAttr;
	CVI_S32 s32Rsv;
	ISP_3AWIN_CONFIG_S aeLEWinConfig[AE_MAX_NUM];
	ISP_3AWIN_CONFIG_S aeSEWinConfig;
} ISP_AE_PARAM_S;

typedef struct _ISP_FE_AE_STAT_1_S {
	CVI_U32 u32PixelCount[ISP_CHANNEL_MAX_NUM];
	CVI_U32 u32PixelWeight[ISP_CHANNEL_MAX_NUM];
	CVI_U32 au32HistogramMemArray[ISP_CHANNEL_MAX_NUM][MAX_HIST_BINS];
} ISP_FE_AE_STAT_1_S;

typedef struct _ISP_FE_AE_STAT_2_S {
	CVI_U16 u16GlobalAvgR[ISP_CHANNEL_MAX_NUM];
	CVI_U16 u16GlobalAvgGr[ISP_CHANNEL_MAX_NUM];
	CVI_U16 u16GlobalAvgGb[ISP_CHANNEL_MAX_NUM];
	CVI_U16 u16GlobalAvgB[ISP_CHANNEL_MAX_NUM];
} ISP_FE_AE_STAT_2_S;

typedef struct _ISP_FE_AE_STAT_3_S {
	CVI_U16 au16ZoneAvg[ISP_CHANNEL_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STAT_3_S;

typedef struct _ISP_FE_AE_STITCH_STAT_3_S {
	CVI_U16 au16ZoneAvg[ISP_CHANNEL_MAX_NUM][ISP_CHANNEL_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STITCH_STAT_3_S;

typedef struct _ISP_AE_STAT_ATTR_S {
	CVI_BOOL bChange;

	CVI_BOOL bHistAdjust;
	CVI_U8 u8AEBESel;
	CVI_U8 u8FourPlaneMode;
	CVI_U8 u8HistOffsetX;
	CVI_U8 u8HistOffsetY;
	CVI_U8 u8HistSkipX;
	CVI_U8 u8HistSkipY;

	CVI_BOOL bModeUpdate;
	CVI_U8 u8HistMode;
	CVI_U8 u8AverMode;
	CVI_U8 u8MaxGainMode;

	CVI_BOOL bWightTableUpdate;
	CVI_U8 au8WeightTable[VI_MAX_PIPE_NUM][AE_WEIGHT_ZONE_ROW][AE_WEIGHT_ZONE_COLUMN];
} ISP_AE_STAT_ATTR_S;

typedef struct _ISP_AE_STAT_RESULT_S {
	CVI_U32 u32IntTime[VI_MAX_PIPE_NUM];
	CVI_U32 u32IspDgain;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	CVI_U32 u32Iso;
	CVI_U8 u8AERunInterval;

	CVI_BOOL bPirisValid;
	CVI_S32 s32PirisPos;
	CVI_U32 u32PirisGain;

	ISP_FSWDR_MODE_E enFSWDRMode;
	CVI_U32 au32WDRGain[4];
	CVI_U32 u32HmaxTimes; /* unit is ns */

	ISP_AE_STAT_ATTR_S stStatAttr;
	ISP_DCF_UPDATE_INFO_S stUpdateInfo;
	CVI_U32	u32ExpRatio;
	CVI_S16	s16CurrentLV;
	CVI_U32	u32AvgLuma;
	CVI_U8	u8MeterFramePeriod;
	CVI_BOOL	bStable;
	CVI_FLOAT	fBvStep;
	CVI_U32		u32BlcIso;

	CVI_U32 u32IspDgainSF;
	CVI_U32 u32AgainSF;
	CVI_U32 u32DgainSF;
	CVI_U32 u32IsoSF;
	CVI_U32		u32BlcIsoSF;
	CVI_FLOAT	fEvRatio[2];
} ISP_AE_RESULT_S;

typedef struct _ISP_BE_AE_STAT_1_S {
	CVI_BOOL bStable;
} ISP_BE_AE_STAT_1_S;

typedef struct _ISP_BE_AE_STAT_2_S {
	CVI_BOOL bStable;
} ISP_BE_AE_STAT_2_S;

typedef struct _ISP_BE_AE_STAT_3_S {
	CVI_BOOL bStable;
} ISP_BE_AE_STAT_3_S;

typedef struct _ISP_BE_AE_STITCH_STAT_3_S {
	CVI_BOOL bStable;
} ISP_BE_AE_STITCH_STAT_3_S;

typedef struct _ISP_AE_INFO_S {
	CVI_U32 u32FrameCnt; /* the counting of frame */
	ISP_SMART_ROI_S	stSmartInfo;
	ISP_FE_AE_STAT_1_S * pstFEAeStat1[AE_MAX_NUM];
	ISP_FE_AE_STAT_2_S * pstFEAeStat2[AE_MAX_NUM];
	ISP_FE_AE_STAT_3_S * pstFEAeStat3[AE_MAX_NUM];
	ISP_FE_AE_STITCH_STAT_3_S *pstFEAeStiStat;
	ISP_BE_AE_STAT_1_S *pstBEAeStat1;
	ISP_BE_AE_STAT_2_S *pstBEAeStat2;
	ISP_BE_AE_STAT_3_S *pstBEAeStat3;
	ISP_BE_AE_STITCH_STAT_3_S *pstBEAeStiStat;
	ISP_3AWIN_CONFIG_S aeLEWinConfig[AE_MAX_NUM];
	ISP_3AWIN_CONFIG_S aeSEWinConfig;
} ISP_AE_INFO_S;

typedef struct bmISP_DCI_INFO_S {
	CVI_U32 u32FrameCnt; /* the counting of frame */

	CVI_U16 pStat[DCI_BINS_NUM];
} ISP_DCI_INFO_S;


/* the init param of awb alg */
typedef struct _ISP_AWB_PARAM_S {
	SENSOR_ID SensorId;
	CVI_U8 u8WDRMode;
	CVI_U8 u8AWBZoneRow;
	CVI_U8 u8AWBZoneCol;
	CVI_U8 u8AWBZoneBin;
	ISP_STITCH_ATTR_S stStitchAttr;
	CVI_U16 u16AWBWidth;
	CVI_U16 u16AWBHeight;
	CVI_S8 s8Rsv;
} ISP_AWB_PARAM_S;

typedef struct _ISP_AWB_RAW_STAT_ATTR_S {
	bool bStatCfgUpdate;

	CVI_U16 u16MeteringWhiteLevelAwb;
	CVI_U16 u16MeteringBlackLevelAwb;
	CVI_U16 u16MeteringCrRefMaxAwb;
	CVI_U16 u16MeteringCbRefMaxAwb;
	CVI_U16 u16MeteringCrRefMinAwb;
	CVI_U16 u16MeteringCbRefMinAwb;
} ISP_AWB_RAW_STAT_ATTR_S;

typedef struct _ISP_AWB_RESULT_S {
	CVI_U32 au32WhiteBalanceGain[ISP_BAYER_CHN_NUM];
	CVI_U16 au16ColorMatrix[CCM_MATRIX_SIZE];
	CVI_U32 u32ColorTemp;
	CVI_U8 u8Saturation[4];
	ISP_AWB_RAW_STAT_ATTR_S stRawStatAttr;
	CVI_BOOL bStable;
	CVI_U8 u8AdjCASaturation;
	CVI_U8 u8AdjCASatLuma;
} ISP_AWB_RESULT_S;

typedef struct _ISP_AWB_STAT_1_S {
	CVI_U16 u16MeteringAwbAvgR;
	CVI_U16 u16MeteringAwbAvgG;
	CVI_U16 u16MeteringAwbAvgB;
	CVI_U16 u16MeteringAwbCountAll;
} ISP_AWB_STAT_1_S;

typedef struct _ISP_AWB_STAT_RESULT_S {
	CVI_U16 *pau16ZoneAvgR;
	CVI_U16 *pau16ZoneAvgG;
	CVI_U16 *pau16ZoneAvgB;
	CVI_U16 *pau16ZoneCount;
} ISP_AWB_STAT_RESULT_S;

typedef struct _ISP_AWB_INFO_S {
	CVI_U32 u32FrameCnt;
	ISP_SMART_ROI_S	stSmartInfo;
	ISP_AWB_STAT_1_S * pstAwbStat1[ISP_CHANNEL_MAX_NUM];
	ISP_AWB_STAT_RESULT_S stAwbStat2[ISP_CHANNEL_MAX_NUM];
	CVI_U8 u8AwbGainSwitch;
	CVI_U32 au32WDRWBGain[ISP_BAYER_CHN_NUM];
	CVI_U32 u32IsoNum;
	CVI_S16 s16LVx100;
	CVI_FLOAT fBVstep;
} ISP_AWB_INFO_S;

// AF related structure
typedef struct _ISP_AF_PARAM_S {
	SENSOR_ID SensorId;
	CVI_U8 u8WDRMode;
	CVI_S32 s32Rsv;
} ISP_AF_PARAM_S;

typedef struct _ISP_AF_INFO_S {
	CVI_U32 u32FrameCnt;
	ISP_AF_STATISTICS_S *pstAfStat;
} ISP_AF_INFO_S;

typedef struct _ISP_AF_RESULT_S {
	CVI_S32 s32Rsv;
} ISP_AF_RESULT_S;

typedef struct _ISP_AE_STITCH_STATISTICS_S {
	CVI_U32 enable;
} ISP_AE_STITCH_STATISTICS_S;

typedef struct _ISP_BIND_ATTR_S {
	SENSOR_ID sensorId;
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAfLib;
	ALG_LIB_S stAwbLib;
} ISP_BIND_ATTR_S;

typedef struct _ISP_AE_EXP_FUNC_S {
	CVI_S32 (*pfn_ae_init)(VI_PIPE ViPipe, const ISP_AE_PARAM_S *pstAeParam);
	CVI_S32 (*pfn_ae_run)(VI_PIPE ViPipe, const ISP_AE_INFO_S *pstAeInfo,
			ISP_AE_RESULT_S *pstAeResult, CVI_S32 s32Rsv);
	CVI_S32 (*pfn_ae_ctrl)(VI_PIPE ViPipe, CVI_U32 u32Cmd, void *pValue);
	CVI_S32 (*pfn_ae_exit)(VI_PIPE ViPipe);
} ISP_AE_EXP_FUNC_S;

typedef struct _ISP_AWB_EXP_FUNC_S {
	CVI_S32 (*pfn_awb_init)(VI_PIPE ViPipe, const ISP_AWB_PARAM_S *pstAwbParam);
	CVI_S32 (*pfn_awb_run)(VI_PIPE ViPipe, const ISP_AWB_INFO_S *pstAwbInfo, ISP_AWB_RESULT_S *pstAwbResult,
			CVI_S32 s32Rsv);
	CVI_S32 (*pfn_awb_ctrl)(VI_PIPE ViPipe, CVI_U32 u32Cmd, CVI_VOID *pValue);
	CVI_S32 (*pfn_awb_exit)(VI_PIPE ViPipe);
} ISP_AWB_EXP_FUNC_S;

typedef struct _ISP_AF_EXP_FUNC_S {
	CVI_S32 (*pfn_af_init)(VI_PIPE ViPipe, const ISP_AF_PARAM_S *pstAfParam);
	CVI_S32 (*pfn_af_run)(VI_PIPE ViPipe, const ISP_AF_INFO_S *pstAfInfo, ISP_AF_RESULT_S *pstAfResult,
			CVI_S32 s32Rsv);
	CVI_S32 (*pfn_af_ctrl)(VI_PIPE ViPipe, CVI_U32 u32Cmd, CVI_VOID *pValue);
	CVI_S32 (*pfn_af_exit)(VI_PIPE ViPipe);
} ISP_AF_EXP_FUNC_S;

typedef struct _ISP_AE_REGISTER_S {
	ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;

typedef struct _ISP_AWB_REGISTER_S {
	ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

typedef struct _ISP_AF_REGISTER_S {
	ISP_AF_EXP_FUNC_S stAfExpFunc;
} ISP_AF_REGISTER_S;

typedef enum _ISP_CTRL_CMD_E {
	ISP_WDR_MODE_SET = 0,

	ISP_CTRL_CMD_BUTT_MAX,
} ISP_CTRL_CMD_E;


#define AWB_DBG_VER	(11)
#define AWB_SENSOR_NUM (VI_MAX_PIPE_NUM)
#define AWB_DBG_W	(64)
#define AWB_DBG_H	(32)
#define AWB_SHIFT_REGION_NUM	(4)
typedef struct _AWB_DBG_S {
	//Don't modify ==>
	CVI_U32 u32Date;
	CVI_U16 u16AlgoVer;
	CVI_U16 u16DbgVer;
	CVI_U16 u16MaxFrameNum;
	CVI_U16 u16SensorId;
	CVI_U32 u32BinSize;
	CVI_U16 u16WinWnum;
	CVI_U16 u16WinHnum;
	CVI_U16 u16WinOffX;
	CVI_U16 u16WinOffY;
	CVI_U16 u16WinWsize;
	CVI_U16 u16WinHsize;
	CVI_U16 u16P_R[2][AWB_DBG_W*AWB_DBG_H];//keep Max Size
	CVI_U16 u16P_G[2][AWB_DBG_W*AWB_DBG_H];//keep Max Size
	CVI_U16 u16P_B[2][AWB_DBG_W*AWB_DBG_H];//keep Max Size
	CVI_U16 u16P_type[2][AWB_DBG_W*AWB_DBG_H];//keep Max Size
	CVI_U16 u16GrayCnt;
	CVI_U16 u16BalanceR;
	CVI_U16 u16BalanceB;
	CVI_U16 u16FinalR;
	CVI_U16 u16FinalB;
	CVI_U16 u16CurrentR;
	CVI_U16 u16CurrentB;
	CVI_U16 u16IdealR;
	CVI_U16 u16IdealB;
	CVI_U16 CalibRgain[AWB_CALIB_PTS_NUM];
	CVI_U16 CalibBgain[AWB_CALIB_PTS_NUM];
	CVI_U16 CalibTemp[AWB_CALIB_PTS_NUM];
	CVI_U16 u16CurveR[256];//4096/16
	CVI_U16 u16CurveB[256];
	CVI_U16 u16CurveB_Top[256];
	CVI_U16 u16CurveB_Bot[256];
	CVI_S16 s16LvX100;
	CVI_U32 u32ISONum;
	ISP_WB_ATTR_S dbgMPIAttr[AWB_SENSOR_NUM];
	ISP_WB_ATTR_S dbgInfoAttr[AWB_SENSOR_NUM];
	ISP_AWB_ATTR_EX_S dbgMPIAttrEx[AWB_SENSOR_NUM];
	ISP_AWB_ATTR_EX_S dbgInfoAttrEx[AWB_SENSOR_NUM];
	//Don't modify <==
	CVI_U8 calib_sts;
	ISP_AWB_Calibration_Gain_S_EX calib_ex;
	ISP_SMART_ROI_S stFace;

	// fix size to 128 byte from here
	CVI_U16 u16Region_R[AWB_SHIFT_REGION_NUM];
	CVI_U8 u8Reserve[120];
} s_AWB_DBG_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_COMM_3A_H__ */
