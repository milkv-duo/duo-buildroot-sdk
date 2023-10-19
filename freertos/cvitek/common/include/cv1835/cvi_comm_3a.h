/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_common_3a.h
 * Description:
 */

#ifndef _CVI_COMM_3A_H_
#define _CVI_COMM_3A_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <stdbool.h>
#include "cvi_type.h"
#include "cvi_comm_isp.h"
#include "cvi_comm_sns.h"

#define ALG_LIB_NAME_SIZE_MAX (20)
#define MAX_REGISTER_ALG_LIB_NUM 4

#define HIST_THRESH_NUM (4)

#define MAX_AE_W (32)
#define MAX_AE_H (24)
#define MAX_AE_SIZE (MAX_AE_W * MAX_AE_H)
#define MAX_AE_WIN (2)

typedef enum _ISP_FSWDR_MODE_E {
	ISP_FSWDR_NORMAL_MODE = 0x0,
	ISP_FSWDR_LONG_FRAME_MODE = 0x1,
	ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,
	ISP_FSWDR_MODE_BUTT
} ISP_FSWDR_MODE_E;

typedef struct _ISP_AE_RANGE_S {
	CVI_U32 u32Max;
	CVI_U32 u32Min;
} ISP_AE_RANGE_S;

typedef enum _ISP_AE_STRATEGY_E {
	AE_EXP_HIGHLIGHT_PRIOR = 0,
	AE_EXP_LOWLIGHT_PRIOR = 1,
	AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef enum _ISP_AE_MODE_E { AE_MODE_SLOW_SHUTTER = 0, AE_MODE_FIX_FRAME_RATE = 1, AE_MODE_BUTT } ISP_AE_MODE_E;

typedef enum _ISP_ANTIFLICKER_MODE_E {
	ISP_ANTIFLICKER_NORMAL_MODE = 0x0,
	ISP_ANTIFLICKER_AUTO_MODE = 0x1,
	ISP_ANTIFLICKER_MODE_BUTT
} ISP_ANTIFLICKER_MODE_E;

typedef struct _ISP_ANTIFLICKER_S {
	CVI_BOOL bEnable;
	CVI_U8 u8Frequency;
	ISP_ANTIFLICKER_MODE_E enMode;
} ISP_ANTIFLICKER_S;

typedef struct hiISP_SUBFLICKER_S {
	CVI_BOOL bEnable;
	CVI_U8 u8LumaDiff;
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
	CVI_U32 u32ExpTime;
	CVI_U32 u32AGain;
	CVI_U32 u32DGain;
	CVI_U32 u32ISPDGain;

	ISP_OP_TYPE_E enISONumOpType;
	CVI_BOOL bUseISONum;
	CVI_U32 u32ISONum;
} ISP_ME_ATTR_S;

#define LINEAR_LV_TARGET_NUM	(16)
typedef struct _ISP_AE_ATTR_S {
	ISP_AE_RANGE_S stExpTimeRange;
	ISP_AE_RANGE_S stAGainRange;
	ISP_AE_RANGE_S stDGainRange;
	ISP_AE_RANGE_S stISPDGainRange;
	ISP_AE_RANGE_S stSysGainRange;

	CVI_U32 u32GainThreshold;
	CVI_S32 u8Speed; // 1 means 0.1 ev
	CVI_U16 u16BlackSpeedBias;
	CVI_S32 u8Tolerance;
	CVI_S32 u8Compensation;
	CVI_U16 u16EVBias;
	ISP_AE_STRATEGY_E enAEStrategyMode;
	CVI_U16 u16HistRatioSlope;
	CVI_U8 u8MaxHistOffset;
	ISP_AE_MODE_E enAEMode;
	ISP_ANTIFLICKER_S stAntiflicker;
	ISP_SUBFLICKER_S stSubflicker;
	ISP_AE_DELAY_S stAEDelayAttr;
	CVI_BOOL bManualExpValue;
	CVI_U32 u32ExpValue;
	ISP_FSWDR_MODE_E enFSWDRMode;
	CVI_BOOL bWDRQuick;
	CVI_U16 u16ISOCalCoef;

	CVI_BOOL bUseISONum;
	ISP_AE_RANGE_S stISONumRange;
	CVI_S16		s16IRCutOnLv;
	CVI_S16		s16IRCutOffLv;
	ISP_AE_IR_CUT_FORCE_STATUS enIRCutStatus;
	CVI_U8 au8AdjustTargetMin[LINEAR_LV_TARGET_NUM];
	CVI_U8 au8AdjustTargetMax[LINEAR_LV_TARGET_NUM];
	CVI_U16 u16LowBinThr;
	CVI_U16 u16HighBinThr;
} ISP_AE_ATTR_S;


typedef struct _ISP_EXPOSURE_ATTR_S {
	CVI_BOOL bByPass;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 u8AERunInterval;
	CVI_BOOL bHistStatAdjust;
	CVI_BOOL bAERouteExValid;
	ISP_ME_ATTR_S stManual;
	ISP_AE_ATTR_S stAuto;
	CVI_U8	u8DebugMode;
	CVI_U8	u8MeterMode;
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
	CVI_S32 SensorId;
	CVI_U8 u8WDRMode;
	CVI_U8 u8HDRMode;
	CVI_U16 u16BlackLevel;
	CVI_FLOAT f32Fps;
	ISP_BAYER_FORMAT_E enBayer;
	ISP_STITCH_ATTR_S stStitchAttr;
	CVI_S32 s32Rsv;
	ISP_EXPOSURE_ATTR_S stExpAttr;
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
	CVI_U8 au8WeightTable[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN];
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
	CVI_FLOAT	fWDRIndex;
	CVI_U32	u32ExpRatio;
	CVI_S16	s16CurrentLV;
	CVI_U32	u32AvgLuma;
	CVI_U8	u8MeterFramePeriod;
	CVI_FLOAT fSEExpGainUpRatio;
	CVI_BOOL	bStable;
} ISP_AE_RESULT_S;

typedef struct _ISP_BE_AE_STAT_1_S {
} ISP_BE_AE_STAT_1_S;

typedef struct _ISP_BE_AE_STAT_2_S {
} ISP_BE_AE_STAT_2_S;

typedef struct _ISP_BE_AE_STAT_3_S {
} ISP_BE_AE_STAT_3_S;

typedef struct _ISP_BE_AE_STITCH_STAT_3_S {
} ISP_BE_AE_STITCH_STAT_3_S;

typedef struct _ISP_FE_HIST_STAT_S {
	CVI_U32 histR[ISP_CHANNEL_MAX_NUM][MAX_HIST_BINS];
	CVI_U32 histG[ISP_CHANNEL_MAX_NUM][MAX_HIST_BINS];
	CVI_U32 histB[ISP_CHANNEL_MAX_NUM][MAX_HIST_BINS];
} ISP_FE_HIST_STAT_S;

typedef struct _ISP_HIST_INFO_S {
	CVI_U32 u32FrameCnt; /* the counting of frame */
	ISP_FE_HIST_STAT_S *pstFEHistStat;
} ISP_HIST_INFO_S;

typedef struct _ISP_AE_INFO_S {
	CVI_U32 u32FrameCnt; /* the counting of frame */

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
	CVI_S32 SensorId;
	CVI_U8 u8WDRMode;
	CVI_U8 u8AWBZoneRow;
	CVI_U8 u8AWBZoneCol;
	CVI_U8 u8AWBZoneBin;
	ISP_STITCH_ATTR_S stStitchAttr;
	CVI_U16 u16AWBWidth;
	CVI_U16 u16AWBHeight;
	CVI_S8 s8Rsv;
} ISP_AWB_PARAM_S;

typedef struct hiISP_AWB_RAW_STAT_ATTR_S {
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
	CVI_U8 u8Saturation;
	ISP_AWB_RAW_STAT_ATTR_S stRawStatAttr;
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

	ISP_AWB_STAT_1_S * pstAwbStat1[ISP_CHANNEL_MAX_NUM];
	ISP_AWB_STAT_RESULT_S stAwbStat2[ISP_CHANNEL_MAX_NUM];
	CVI_U8 u8AwbGainSwitch;
	CVI_U32 au32WDRWBGain[ISP_BAYER_CHN_NUM];
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
	CVI_S32 sensorId;
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAfLib;
	ALG_LIB_S stAwbLib;
} ISP_BIND_ATTR_S;

typedef struct _ISP_AE_EXP_FUNC_S {
	CVI_S32(*pfn_ae_init)(VI_PIPE ViPipe, const ISP_AE_PARAM_S * pstAeParam);
	CVI_S32(*pfn_ae_run)(VI_PIPE ViPipe, const ISP_AE_INFO_S * pstAeInfo,
			ISP_AE_RESULT_S * pstAeResult, CVI_S32 s32Rsv);
	CVI_S32(*pfn_ae_ctrl)(VI_PIPE ViPipe, CVI_U32 u32Cmd, void *pValue);
	CVI_S32(*pfn_ae_exit)(VI_PIPE ViPipe);
} ISP_AE_EXP_FUNC_S;

typedef struct _ISP_AWB_EXP_FUNC_S {
	CVI_S32(*pfn_awb_init)(VI_PIPE ViPipe, const ISP_AWB_PARAM_S * pstAwbParam);
	CVI_S32(*pfn_awb_run)(VI_PIPE ViPipe, const ISP_AWB_INFO_S * pstAwbInfo, ISP_AWB_RESULT_S * pstAwbResult,
			CVI_S32 s32Rsv);
	CVI_S32(*pfn_awb_ctrl)(VI_PIPE ViPipe, CVI_U32 u32Cmd, CVI_VOID * pValue);
	CVI_S32(*pfn_awb_exit)(VI_PIPE ViPipe);
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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _CVI_COMM_3A_H_ */
