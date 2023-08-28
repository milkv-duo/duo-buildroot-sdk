
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <inttypes.h>

#include <fcntl.h>		/* low-level i/o */
#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"
#include "cvi_comm_sns.h"
#include "cvi_ae.h"
#include "cvi_awb.h"
#include "cvi_isp.h"
#include "cvi_sns_ctrl.h"
#include "sample_comm.h"

#define DELAY_500MS() (usleep(500 * 1000))
#define AAA_LIMIT(var, min, max) ((var) = ((var) < (min)) ? (min) : (((var) > (max)) ? (max) : (var)))
#define AAA_ABS(a) ((a) > 0 ? (a) : -(a))
#define AAA_MIN(a, b) ((a) < (b) ? (a) : (b))
#define AAA_MAX(a, b) ((a) > (b) ? (a) : (b))
#define AAA_DIV_0_TO_1(a) ((0 == (a)) ? 1 : (a))

#define _LOG_NONE       "\033[0m"
#define _LOG_RED        "\033[1;31m"
#define _LOG_YELLOW     "\033[1;33m"
#define _LOG_GREEN      "\033[1;32m"
#define _LOG_DEBUG      "\033[0m"

#define debug_log(fmt, arg...) printf(_LOG_DEBUG fmt _LOG_NONE, ##arg)
#define info_log(fmt, arg...)  printf(_LOG_GREEN fmt _LOG_NONE, ##arg)
#define warn_log(fmt, arg...)  printf(_LOG_YELLOW fmt _LOG_NONE, ##arg)
#define error_log(fmt, arg...) printf(_LOG_RED fmt _LOG_NONE, ##arg)

#ifndef AE_SE
#define AE_SE ISP_CHANNEL_SE
#endif

#ifndef AE_LE
#define AE_LE ISP_CHANNEL_LE
#endif

#ifndef AE_WDR_RATIO_BASE
#define AE_WDR_RATIO_BASE 64
#endif

#ifndef AE_GAIN_BASE
#define AE_GAIN_BASE 1024
#endif

#ifndef MAX_SENSOR_NUM
#define MAX_SENSOR_NUM   2
#endif

static AE_SENSOR_DEFAULT_S stSnsDft[MAX_SENSOR_NUM];

static ISP_SENSOR_EXP_FUNC_S stSensorExpFunc[MAX_SENSOR_NUM];
static AE_SENSOR_EXP_FUNC_S stExpFuncs[MAX_SENSOR_NUM];
static ISP_SNS_OBJ_S *pstSnsObj[MAX_SENSOR_NUM];

static void AE_SetFpsTest(CVI_U8 sID, CVI_U8 fps);

typedef struct __SENSOR_INFO_S {
	bool bWDRMode;

	float fExpLineTime;

	CVI_U32 u32LExpTimeMin;
	CVI_U32 u32LExpTimeMax;

	CVI_U32 u32LExpLineMin;
	CVI_U32 u32LExpLineMax;

	CVI_U32 u32SExpTimeMin;
	CVI_U32 u32SExpTimeMax;

	CVI_U32 u32SExpLineMin;
	CVI_U32 u32SExpLineMax;
} _SENSOR_INFO_S;

static _SENSOR_INFO_S sensorInfo[MAX_SENSOR_NUM];

static void getSensorInfo(CVI_U8 sID)
{
	ISP_PUB_ATTR_S stPubAttr;

	memset(&stPubAttr, 0, sizeof(stPubAttr));

	CVI_ISP_GetPubAttr(sID, &stPubAttr);

	if (stPubAttr.enWDRMode != WDR_MODE_NONE) {
		sensorInfo[sID].bWDRMode = true;
	} else {
		sensorInfo[sID].bWDRMode = false;
	}

	CVI_U8 fps = stPubAttr.f32FrameRate;

	CVI_U16 manual = 1;
	CVI_U32 ratio[3] = { 256, 64, 64 }; //max 256x
	CVI_U32 IntTimeMax[4], IntTimeMin[4], LFMaxIntTime[4];

	stExpFuncs[sID].pfn_cmos_fps_set(sID, fps, &stSnsDft[sID]);

	sensorInfo[sID].fExpLineTime = 1000000 / (CVI_FLOAT) (stSnsDft[sID].u32FullLinesStd * fps);

	if (stSnsDft[sID].stIntTimeAccu.f32Accuracy < 1) {
		sensorInfo[sID].fExpLineTime = sensorInfo[sID].fExpLineTime *
								stSnsDft[sID].stIntTimeAccu.f32Accuracy;
	}

	info_log("\nsensor: %d, fps: %d\n", sID, fps);

	info_log("sensor frame line: %d, line time: %f, f32Accuracy: %f\n",
		stSnsDft[sID].u32FullLinesStd, sensorInfo[sID].fExpLineTime,
		stSnsDft[sID].stIntTimeAccu.f32Accuracy);

	if (!sensorInfo[sID].bWDRMode) {

		sensorInfo[sID].u32LExpLineMin = stSnsDft[sID].u32MinIntTime;
		sensorInfo[sID].u32LExpLineMax = stSnsDft[sID].u32MaxIntTime;

		sensorInfo[sID].u32LExpTimeMin =  sensorInfo[sID].u32LExpLineMin *
										sensorInfo[sID].fExpLineTime + 1;
		sensorInfo[sID].u32LExpTimeMax =  sensorInfo[sID].u32LExpLineMax *
										sensorInfo[sID].fExpLineTime;

		info_log("sensor exposure time range: %d - %d, line range: %d - %d\n",
			sensorInfo[sID].u32LExpTimeMin, sensorInfo[sID].u32LExpTimeMax,
			sensorInfo[sID].u32LExpLineMin, sensorInfo[sID].u32LExpLineMax);

	} else {
		stExpFuncs[sID].pfn_cmos_get_inttime_max(sID, manual, ratio, IntTimeMax,
			IntTimeMin, LFMaxIntTime);

		sensorInfo[sID].u32LExpLineMin = IntTimeMin[0];
		sensorInfo[sID].u32SExpLineMin = IntTimeMin[0];

		sensorInfo[sID].u32SExpLineMax = IntTimeMax[0];

		sensorInfo[sID].u32LExpLineMax = stSnsDft[sID].u32FullLinesStd - IntTimeMax[0];

		sensorInfo[sID].u32LExpTimeMin = sensorInfo[sID].u32LExpLineMin *
										sensorInfo[sID].fExpLineTime + 1;
		sensorInfo[sID].u32LExpTimeMax = sensorInfo[sID].u32LExpLineMax *
										sensorInfo[sID].fExpLineTime;

		sensorInfo[sID].u32SExpTimeMin = sensorInfo[sID].u32LExpTimeMin;
		sensorInfo[sID].u32SExpTimeMax = sensorInfo[sID].u32SExpLineMax * sensorInfo[sID].fExpLineTime;

		info_log("sensor LE exposure time range: %d - %d, line range: %d - %d\n",
			sensorInfo[sID].u32LExpTimeMin, sensorInfo[sID].u32LExpTimeMax,
			sensorInfo[sID].u32LExpLineMin, sensorInfo[sID].u32LExpLineMax);

		info_log("sensor SE exposure time range: %d - %d, line range: %d - %d\n",
			sensorInfo[sID].u32SExpTimeMin, sensorInfo[sID].u32SExpTimeMax,
			sensorInfo[sID].u32SExpLineMin, sensorInfo[sID].u32SExpLineMax);
	}

	info_log("sensor Again max: %d, Dgain max: %d\n\n",
		stSnsDft[sID].u32MaxAgain, stSnsDft[sID].u32MaxDgain);
}

static void apply_sensor_default_blc(CVI_U8 sID)
{
	ISP_CMOS_BLACK_LEVEL_S stBlc;

	memset(&stBlc, 0, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	stSensorExpFunc[sID].pfn_cmos_get_isp_black_level(sID, &stBlc);
	CVI_ISP_SetBlackLevelAttr(sID, &stBlc.blcAttr);

	debug_log("apply sensor default blc enOpType:%d ISO = 100 R:%d Gr:%d Gb:%d B:%d\n",
		stBlc.blcAttr.enOpType,
		stBlc.blcAttr.stAuto.OffsetR[0],
		stBlc.blcAttr.stAuto.OffsetGr[0],
		stBlc.blcAttr.stAuto.OffsetGb[0],
		stBlc.blcAttr.stAuto.OffsetB[0]);
}

static void init_sensor_info(void)
{
	for (int i = 0; i < MAX_SENSOR_NUM; i++) {
		pstSnsObj[i] = SAMPLE_COMM_ISP_GetSnsObj(i);
		if (pstSnsObj[i] != CVI_NULL) {
			pstSnsObj[i]->pfnExpSensorCb(&stSensorExpFunc[i]);
			apply_sensor_default_blc(i);
			pstSnsObj[i]->pfnExpAeCb(&stExpFuncs[i]);
			stExpFuncs[i].pfn_cmos_get_ae_default(i, &stSnsDft[i]);
			getSensorInfo(i);
		}
	}
}

static CVI_U32 calcExpLine(CVI_U8 sID, CVI_S32 expTime)
{
	CVI_U32 expLine = 0;

	expLine = expTime / sensorInfo[sID].fExpLineTime;
	AAA_LIMIT(expLine, sensorInfo[sID].u32LExpLineMin, sensorInfo[sID].u32LExpLineMax);

	return expLine;
}

static void calcCenterG(CVI_U8 sID, CVI_U16 *LE, CVI_U16 *SE)
{
	CVI_U16 row, column, i;
	CVI_U16 RValue, GValue, BValue, maxValue;
	CVI_U8 centerRowStart, centerRowEnd, centerColumnStart, centerColumnEnd;
	CVI_U32 centerLuma[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_U16 centerCnt[ISP_CHANNEL_MAX_NUM] = {0, 0};

	ISP_AE_STATISTICS_S stAeStat;

	memset(&stAeStat, 0, sizeof(ISP_AE_STATISTICS_S));

	CVI_ISP_GetAEStatistics(sID, &stAeStat);

	centerRowStart = AE_ZONE_ROW / 2 - AE_ZONE_ROW / 4;
	centerRowEnd = AE_ZONE_ROW / 2 + AE_ZONE_ROW / 4;
	centerColumnStart = AE_ZONE_COLUMN / 2 - AE_ZONE_COLUMN / 4;
	centerColumnEnd = AE_ZONE_COLUMN / 2 + AE_ZONE_COLUMN / 4;

	for (i = 0; i < ISP_CHANNEL_MAX_NUM; i++) {
		for (row = 0; row < AE_ZONE_ROW; row++) {
			for (column = 0; column < AE_ZONE_COLUMN; column++) {

				if ((row >= centerRowStart && row <= centerRowEnd) &&
					(column >= centerColumnStart && column <= centerColumnEnd)) {

					RValue = stAeStat.au16FEZoneAvg[i][0][row][column][ISP_BAYER_CHN_R];
					GValue = (stAeStat.au16FEZoneAvg[i][0][row][column][ISP_BAYER_CHN_GR] +
						stAeStat.au16FEZoneAvg[i][0][row][column][ISP_BAYER_CHN_GB]) / 2;
					BValue = stAeStat.au16FEZoneAvg[i][0][row][column][ISP_BAYER_CHN_B];

					maxValue = AAA_MAX(RValue, GValue);
					maxValue = AAA_MAX(maxValue, BValue);
					centerCnt[i]++;
					centerLuma[i] += maxValue;
				}

			}
		}
	}

	*LE = centerLuma[ISP_CHANNEL_LE] / centerCnt[ISP_CHANNEL_LE];
	*SE = centerLuma[ISP_CHANNEL_SE] / centerCnt[ISP_CHANNEL_SE];
}

static void _print_ae_info(CVI_U8 sID)
{
	ISP_EXP_INFO_S stExpInfo;

	memset(&stExpInfo, 0, sizeof(stExpInfo));
	CVI_ISP_QueryExposureInfo(sID, &stExpInfo);

	CVI_U16 le, se;

	calcCenterG(sID, &le, &se);

	if (!sensorInfo[sID].bWDRMode) {
		info_log("\ntime: %u, iso: %u, AeL: %u\n", stExpInfo.u32ExpTime,
			stExpInfo.u32ISO, le);
		info_log("sensor LEexpT: %u, LEexpL: %u AG: %u, DG: %u, IG: %u\n\n",
			stExpInfo.u32ExpTime, calcExpLine(sID, stExpInfo.u32ExpTime),
			stExpInfo.u32AGain, stExpInfo.u32DGain, stExpInfo.u32ISPDGain);
	} else {
		info_log("\ntime: %u, iso: %u, AeL: %u, AeS: %u\n", stExpInfo.u32ExpTime,
			stExpInfo.u32ISO, le, se);
		info_log("sensor LEexpT: %u, LEexpL: %u, SEexpT: %u, SEexpL: %u, AG: %u, DG: %u, IG: %u\n\n",
			stExpInfo.u32ExpTime, calcExpLine(sID, stExpInfo.u32ExpTime),
			stExpInfo.u32ShortExpTime, calcExpLine(sID, stExpInfo.u32ShortExpTime),
			stExpInfo.u32AGain, stExpInfo.u32DGain, stExpInfo.u32ISPDGain);
	}
}

static void AE_SetManualExposureTest(CVI_U8 sID, CVI_U8 mode, CVI_S32 expTime, CVI_S32 ISONum)
{
	ISP_EXPOSURE_ATTR_S stExpAttr;

	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.u8DebugMode = 0;

	if (mode == 0) {
		stExpAttr.bByPass = 1;
		printf("AE byPass!\n");
	} else if (mode == 1) {
		stExpAttr.bByPass = 0;
		stExpAttr.enOpType = OP_TYPE_AUTO;
		stExpAttr.stManual.enExpTimeOpType = OP_TYPE_AUTO;
		stExpAttr.stManual.enISONumOpType = OP_TYPE_AUTO;
		printf("AE Auto!\n");
	} else if (mode == 2) {
		stExpAttr.bByPass = 0;
		stExpAttr.enOpType = OP_TYPE_MANUAL;
		stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
		stExpAttr.stManual.enISONumOpType = OP_TYPE_MANUAL;
		stExpAttr.stManual.u32ExpTime = expTime;
		stExpAttr.stManual.enGainType = AE_TYPE_ISO;
		stExpAttr.stManual.u32ISONum = ISONum;
		printf("AE Manual!\n");
	}

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);

	DELAY_500MS();

	_print_ae_info(sID);
}

static void AE_SetDebugMode(CVI_U8 sID, CVI_U8 item)
{
	ISP_EXPOSURE_ATTR_S stExpAttr;

	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.u8DebugMode = item;

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);
}

static void AE_SetManualGainTest(CVI_U8 sID, CVI_U32 again, CVI_U32 dgain, CVI_U32 ispDgain)
{
	ISP_EXPOSURE_ATTR_S stExpAttr;

	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.bByPass = 0;
	stExpAttr.u8DebugMode = 0;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enGainType = AE_TYPE_GAIN;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = again;
	stExpAttr.stManual.u32DGain = dgain;
	stExpAttr.stManual.u32ISPDGain = ispDgain;

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);

	DELAY_500MS();

	_print_ae_info(sID);
}

static void AE_SetFpsTest(CVI_U8 sID, CVI_U8 fps)
{
	ISP_PUB_ATTR_S stPubAttr;

	memset(&stPubAttr, 0, sizeof(stPubAttr));

	CVI_ISP_GetPubAttr(sID, &stPubAttr);

	stPubAttr.f32FrameRate = fps;

	info_log("\nset pipe: %d, fps: %d\n", sID, fps);

	CVI_ISP_SetPubAttr(sID, &stPubAttr);

	DELAY_500MS();

	getSensorInfo(sID);
}

static void AE_SetLSC(CVI_U8 sID, CVI_BOOL enableLSC)
{
	VI_PIPE ViPipe = sID;

#ifdef ARCH_CV182X
	ISP_MESH_SHADING_ATTR_S plscAttr;

	CVI_ISP_GetMeshShadingAttr(ViPipe, &plscAttr);
	plscAttr.Enable = enableLSC;
	CVI_ISP_SetMeshShadingAttr(ViPipe, &plscAttr);
#else
	ISP_DRC_ATTR_S pDRCAttr;

#define AE_LSCR_ENABLE  3
#define AWB_LSCR_ENABLE 4
	CVI_ISP_GetDRCAttr(ViPipe, &pDRCAttr);
	pDRCAttr.DRCMu[AE_LSCR_ENABLE] = enableLSC;
	pDRCAttr.DRCMu[AWB_LSCR_ENABLE] = enableLSC;
	CVI_ISP_SetDRCAttr(ViPipe, &pDRCAttr);
#endif

	if (enableLSC) {
		info_log("sID:%d LSC enalbe!\n", sID);
	} else {
		info_log("sID:%d LSC disable!\n", sID);
	}
}

static void AE_SetWDRManualRatio(CVI_U8 sID, CVI_U16 ratio)
{
	ISP_WDR_EXPOSURE_ATTR_S stWDRExpAttr;

	memset(&stWDRExpAttr, 0, sizeof(ISP_WDR_EXPOSURE_ATTR_S));

	CVI_ISP_GetWDRExposureAttr(sID, &stWDRExpAttr);

	stWDRExpAttr.enExpRatioType = OP_TYPE_MANUAL;
	stWDRExpAttr.au32ExpRatio[0] = ratio < 4 ? (4 * AE_WDR_RATIO_BASE) : (ratio * AE_WDR_RATIO_BASE);

	CVI_ISP_SetWDRExposureAttr(sID, &stWDRExpAttr);

	info_log("set WDR manual ratio: %d\n", stWDRExpAttr.au32ExpRatio[0]);

	if (ratio == 0) {

		printf("set max SE shutter time: %d\n", sensorInfo[sID].u32SExpTimeMax);

		ISP_EXP_INFO_S stExpInfo;

		memset(&stExpInfo, 0, sizeof(stExpInfo));
		CVI_ISP_QueryExposureInfo(sID, &stExpInfo);

		AE_SetManualExposureTest(sID, 2, 100000, stExpInfo.u32ISO);
	}
}

static void AE_GainLinearTest(CVI_U8 sID, CVI_S32 expTime, CVI_U32 StartISONum, CVI_U32 EndISONum)
{
#define RATIO_ERROR_DIFF	3

	CVI_U16 leLuma, seLuma;

	CVI_U32 tempGain = 0;

	CVI_U32 iso, gain, again, dgain, againDb = 0, dgainDb = 0,
			preAgain = 0, preDgain = 0;
	CVI_U32	isoTable[] = {100, 200, 400, 800, 1600, 3200, 6400,
		12800, 25600, 51200, 102400, 204800, 409600, 819200};
	CVI_U16 isoStep = 1, curLuma, preLuma = 0, isoTblSize;
	CVI_U16 i, lumaRatio, gainRatio;

	ISP_EXPOSURE_ATTR_S expAttr = { 0 };
	VI_PIPE ViPipe = sID;

	CVI_ISP_GetExposureAttr(sID, &expAttr);

	isoTblSize = sizeof(isoTable) / sizeof(CVI_U32);
	expAttr.bByPass = 0;
	expAttr.u8DebugMode = 0;
	expAttr.enOpType = OP_TYPE_MANUAL;
	expAttr.stManual.enGainType = AE_TYPE_GAIN;
	expAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	expAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	expAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	expAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	expAttr.stManual.u32ExpTime = expTime;

	if (EndISONum < StartISONum)
		EndISONum = StartISONum;

	StartISONum = AAA_MAX(StartISONum, 100);

	for (iso = StartISONum; iso <= EndISONum; iso += isoStep) {
		for (i = 1 ; i < isoTblSize; ++i) {
			if (iso < isoTable[i]) {
				isoStep = (isoTable[i] - isoTable[i-1]) / 100;
				break;
			}
		}

		gain = (CVI_U32) ((CVI_U64) iso * (CVI_U64) AE_GAIN_BASE) / 100;

		if (gain > stSnsDft[sID].u32MaxAgain && preAgain == stSnsDft[sID].u32MaxAgain) {
			again = stSnsDft[sID].u32MaxAgain;
			dgain = (CVI_U64)gain * AE_GAIN_BASE / AAA_DIV_0_TO_1(again);
			tempGain = dgain;
			stExpFuncs[sID].pfn_cmos_dgain_calc_table(ViPipe, &dgain, &dgainDb);
			if (dgain > tempGain) {
				error_log("\n\nWARN: The output Dgain(%d) can not bigger than", dgain);
				error_log(" the input Dgain(%d)!!!\n\n", tempGain);
			}
		} else {
			again = gain;
			dgain = AE_GAIN_BASE;
			again = AAA_MIN(again, stSnsDft[sID].u32MaxAgain);
			tempGain = again;
			stExpFuncs[sID].pfn_cmos_again_calc_table(ViPipe, &again, &againDb);
			if (again > tempGain) {
				error_log("\n\nWARN: The output Again(%d) can not bigger than", again);
				error_log(" the input Again(%d)!!!\n\n", tempGain);
			}
		}

		if (again != preAgain || dgain != preDgain) {
			expAttr.stManual.u32AGain = again;
			expAttr.stManual.u32DGain = dgain;
			expAttr.stManual.u32ISPDGain = AE_GAIN_BASE;
			CVI_ISP_SetExposureAttr(sID, &expAttr);
			DELAY_500MS();
			calcCenterG(sID, &leLuma, &seLuma);
			curLuma = leLuma;
			lumaRatio = curLuma * 100 / AAA_DIV_0_TO_1(preLuma);
			gainRatio = (CVI_U64)again * dgain * 100 / AAA_DIV_0_TO_1((CVI_U64)preAgain * preDgain);
			if (AAA_ABS(lumaRatio - gainRatio) > RATIO_ERROR_DIFF)
				error_log("AG(%d):%u DG(%d):%u L:%u LR:%d GR:%d\n", againDb, again,
					dgainDb, dgain, curLuma, lumaRatio, gainRatio);
			else
				info_log("AG(%d):%u DG(%d):%u L:%u LR:%d GR:%d\n", againDb, again,
					dgainDb, dgain, curLuma, lumaRatio, gainRatio);
			preAgain = again;
			preDgain = dgain;
			preLuma = curLuma;
			if (stSnsDft[sID].u32MaxDgain > 1024 &&
				dgain >= stSnsDft[sID].u32MaxDgain) {
				break;
			} else if (stSnsDft[sID].u32MaxDgain == 1024 &&
				again >= stSnsDft[sID].u32MaxAgain) {
				break;
			}
		}
	}
}

static void AE_ShutterLinearTest(CVI_U8 sID, CVI_U8 fid, CVI_U32 startExpTime, CVI_U32 endExpTime)
{
	ISP_EXP_INFO_S stExpInfo;
	ISP_EXPOSURE_ATTR_S stExpAttr;
	ISP_WDR_EXPOSURE_ATTR_S stWDRExpAttr;

	memset(&stExpInfo, 0, sizeof(stExpInfo));
	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));
	memset(&stWDRExpAttr, 0, sizeof(ISP_WDR_EXPOSURE_ATTR_S));

	CVI_U32 tmpExpTime[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_U16 curLuma[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_U16 preLuma[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_U32 curExpLine[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_U32 preExpLine[ISP_CHANNEL_MAX_NUM] = {0, 0};

	CVI_S16 lumaRatio[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_S16 expLineRatio[ISP_CHANNEL_MAX_NUM] = {0, 0};
	CVI_S16 ratioDiff[ISP_CHANNEL_MAX_NUM] = {0, 0};

	bool loop = true;

	CVI_U16 leLuma, seLuma;

#define RATIO_ERROR_DIFF  3
#define WDR_TEST_RATIO    4

	startExpTime = AAA_MAX(sensorInfo[sID].u32LExpTimeMin, startExpTime);
	endExpTime = AAA_MIN(sensorInfo[sID].u32LExpTimeMax, endExpTime);

	info_log("LE T: %d - %d, L: %d - %d\n",
		sensorInfo[sID].u32LExpTimeMin,
		sensorInfo[sID].u32LExpTimeMax,
		sensorInfo[sID].u32LExpLineMin,
		sensorInfo[sID].u32LExpLineMax);

	if (sensorInfo[sID].bWDRMode) {

		info_log("SE T: %d - %d, L: %d - %d\n",
			sensorInfo[sID].u32SExpTimeMin,
			sensorInfo[sID].u32SExpTimeMax,
			sensorInfo[sID].u32SExpLineMin,
			sensorInfo[sID].u32SExpLineMax);
	}

	fid = (fid == 1 ? AE_SE : AE_LE);

	tmpExpTime[AE_LE] = tmpExpTime[AE_SE] = startExpTime;

	CVI_ISP_GetWDRExposureAttr(sID, &stWDRExpAttr);

	if (fid == AE_SE) {

		if (!sensorInfo[sID].bWDRMode) {
			error_log("not WDR mode...\n");
			return;
		}

		stWDRExpAttr.enExpRatioType = OP_TYPE_MANUAL;
		stWDRExpAttr.au32ExpRatio[0] = WDR_TEST_RATIO * AE_WDR_RATIO_BASE;

		tmpExpTime[AE_LE] = tmpExpTime[AE_SE] * WDR_TEST_RATIO;
		endExpTime = AAA_MIN(sensorInfo[sID].u32SExpTimeMax, endExpTime);

	} else {
		stWDRExpAttr.enExpRatioType = OP_TYPE_AUTO;
	}

	if (endExpTime < startExpTime) {
		error_log("test fail, endExpTime: %d < startExpTime: %d\n", endExpTime, startExpTime);
		return;
	}

	CVI_ISP_SetWDRExposureAttr(sID, &stWDRExpAttr);

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.bByPass = 0;
	stExpAttr.u8DebugMode = 0;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enGainType = AE_TYPE_GAIN;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32ExpTime = tmpExpTime[AE_LE];

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);

	DELAY_500MS();

	CVI_ISP_QueryExposureInfo(sID, &stExpInfo);
	calcCenterG(sID, &leLuma, &seLuma);

	curExpLine[AE_LE] = calcExpLine(sID, stExpInfo.u32ExpTime);
	preExpLine[AE_LE] = curExpLine[AE_LE];
	curLuma[AE_LE] = leLuma;
	preLuma[AE_LE] = curLuma[AE_LE];

	if (sensorInfo[sID].bWDRMode) {
		curExpLine[AE_SE] = calcExpLine(sID, stExpInfo.u32ShortExpTime);
		preExpLine[AE_SE] = curExpLine[AE_SE];
		curLuma[AE_SE] = seLuma;
		preLuma[AE_SE] = curLuma[AE_SE];
	}

	tmpExpTime[fid] = startExpTime;

	while (loop) {

		if (tmpExpTime[fid] > endExpTime) {
			tmpExpTime[fid] = endExpTime;
			loop = false;
		}

		if (fid == AE_SE) {
			tmpExpTime[AE_LE] = tmpExpTime[AE_SE] * WDR_TEST_RATIO;
		}

		stExpAttr.stManual.u32ExpTime = tmpExpTime[AE_LE];
		CVI_ISP_SetExposureAttr(sID, &stExpAttr);

		DELAY_500MS();

		CVI_ISP_QueryExposureInfo(sID, &stExpInfo);
		calcCenterG(sID, &leLuma, &seLuma);

		//curExpLine[AE_LE] = calcExpLine(sID, stExpInfo.u32ExpTime);
		curExpLine[AE_LE] = calcExpLine(sID, tmpExpTime[AE_LE]);
		curLuma[AE_LE] = leLuma;

		lumaRatio[AE_LE] = curLuma[AE_LE] * 100 / AAA_DIV_0_TO_1(preLuma[AE_LE]);
		expLineRatio[AE_LE] = curExpLine[AE_LE] * 100 / AAA_DIV_0_TO_1(preExpLine[AE_LE]);

		ratioDiff[AE_LE] = abs(lumaRatio[AE_LE] - expLineRatio[AE_LE]);

		if (sensorInfo[sID].bWDRMode) {
			//curExpLine[AE_SE] = calcExpLine(sID, stExpInfo.u32ShortExpTime);
			curExpLine[AE_SE] = calcExpLine(sID, tmpExpTime[AE_SE]);
			curLuma[AE_SE] = seLuma;

			lumaRatio[AE_SE] = curLuma[AE_SE] * 100 / AAA_DIV_0_TO_1(preLuma[AE_SE]);
			expLineRatio[AE_SE] = curExpLine[AE_SE] * 100 / AAA_DIV_0_TO_1(preExpLine[AE_SE]);

			ratioDiff[AE_SE] = abs(lumaRatio[AE_SE] - expLineRatio[AE_SE]);
		}

		if (ratioDiff[AE_LE] >= RATIO_ERROR_DIFF
			|| ratioDiff[AE_SE] >= RATIO_ERROR_DIFF) {
			error_log("\nWARN: Not linear item:\n");
		}

		preLuma[AE_LE] = curLuma[AE_LE];
		preExpLine[AE_LE] = curExpLine[AE_LE];

		info_log("LE, L: %d, T: %d, E: %d, LR: %d, ER: %d\n",
			curLuma[AE_LE],
			stExpInfo.u32ExpTime,
			curExpLine[AE_LE],
			lumaRatio[AE_LE],
			expLineRatio[AE_LE]);

		if (sensorInfo[sID].bWDRMode) {
			preLuma[AE_SE] = curLuma[AE_SE];
			preExpLine[AE_SE] = curExpLine[AE_SE];

			info_log("SE, L: %d, T: %d, E: %d, LR: %d, ER: %d\n",
				curLuma[AE_SE],
				stExpInfo.u32ShortExpTime,
				curExpLine[AE_SE],
				lumaRatio[AE_SE],
				expLineRatio[AE_SE]);
		}

		do {
			tmpExpTime[fid] = tmpExpTime[fid] * 105 / 100; // 5%

			curExpLine[fid] = calcExpLine(sID, tmpExpTime[fid]);

		} while (curExpLine[fid] == preExpLine[fid]);
	}
}

static CVI_U32 gainLookup(CVI_U8 sID, CVI_U8 type, CVI_U32 index)
{
	CVI_U32 maxGain = 1024;
	CVI_U32 minGain = 1024;

	CVI_U32 tempGain = 0;

	CVI_U32 tempIndex;

	if (index == 0) {
		return 1024;
	}

	if (type == 0) {
		maxGain = stSnsDft[sID].u32MaxAgain;
		stExpFuncs[sID].pfn_cmos_again_calc_table(sID, &maxGain, &tempIndex);
	} else {
		maxGain = stSnsDft[sID].u32MaxDgain;
		stExpFuncs[sID].pfn_cmos_dgain_calc_table(sID, &maxGain, &tempIndex);
	}

	if (index >= tempIndex) {
		return maxGain;
	}

	while (1) {

		tempGain = (maxGain + minGain) / 2;

		if (type == 0) {
			stExpFuncs[sID].pfn_cmos_again_calc_table(sID, &tempGain, &tempIndex);
		} else {
			stExpFuncs[sID].pfn_cmos_dgain_calc_table(sID, &tempGain, &tempIndex);
		}

		if (tempIndex == index) {
			return tempGain;
		} else if (tempIndex > index) {
			maxGain = tempGain;
		} else {
			minGain = tempGain;
		}
	}
}

static void AE_GainTableLinearTest(CVI_U8 sID, CVI_U8 type, CVI_U32 startIndex, CVI_U32 endIndex)
{
#define RATIO_ERROR_DIFF	3

	CVI_U16 leLuma, seLuma;

	CVI_U32 preGain = 0, gain = AE_GAIN_BASE;
	CVI_U16 curLuma = 0, preLuma = 0;
	CVI_S32 lumaRatio, gainRatio;

	ISP_EXP_INFO_S stExpInfo;
	ISP_EXPOSURE_ATTR_S stExpAttr;

	memset(&stExpInfo, 0, sizeof(ISP_EXP_INFO_S));
	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.bByPass = 0;
	stExpAttr.u8DebugMode = 0;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enGainType = AE_TYPE_GAIN;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;

	stExpAttr.stManual.u32ISPDGain = AE_GAIN_BASE;

	gain = gainLookup(sID, type, startIndex);

	if (type == 0) {
		stExpAttr.stManual.u32DGain = AE_GAIN_BASE;
		stExpAttr.stManual.u32AGain = gain;
	} else {
		stExpAttr.stManual.u32AGain = stSnsDft[sID].u32MaxAgain;
		stExpAttr.stManual.u32DGain = gain;
	}

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);

	DELAY_500MS();

	CVI_ISP_QueryExposureInfo(sID, &stExpInfo);
	calcCenterG(sID, &leLuma, &seLuma);

	curLuma = leLuma;

	if (type == 0) {
		gain = stExpInfo.u32AGain;
	} else {
		gain = stExpInfo.u32DGain;
	}

	printf("start index: %d, gain: %d, luma: %d\n", startIndex, gain, curLuma);

	for (CVI_U32 i = startIndex + 1; i <= endIndex; i++) {

		preGain = gain;
		preLuma = curLuma;

		gain = gainLookup(sID, type, i);

		if (type == 0) {
			stExpAttr.stManual.u32DGain = AE_GAIN_BASE;
			stExpAttr.stManual.u32AGain = gain;
		} else {
			stExpAttr.stManual.u32AGain = stSnsDft[sID].u32MaxAgain;
			stExpAttr.stManual.u32DGain = gain;
		}

		CVI_ISP_SetExposureAttr(sID, &stExpAttr);

		DELAY_500MS();

		CVI_ISP_QueryExposureInfo(sID, &stExpInfo);
		calcCenterG(sID, &leLuma, &seLuma);

		curLuma = leLuma;

		if (type == 0) {
			gain = stExpInfo.u32AGain;
		} else {
			gain = stExpInfo.u32DGain;
		}

		lumaRatio = curLuma * 100 / AAA_DIV_0_TO_1(preLuma);
		gainRatio = gain * 100 / AAA_DIV_0_TO_1(preGain);

		if (abs(lumaRatio - gainRatio) >= RATIO_ERROR_DIFF ||
			curLuma <= preLuma) {
			error_log("Index: %u, AG:%u DG:%u L:%u LR:%d GR:%d\n", i, stExpInfo.u32AGain,
				stExpInfo.u32DGain, curLuma, lumaRatio, gainRatio);
		} else {
			info_log("Index: %u, AG:%u DG:%u L:%u LR:%d GR:%d\n", i, stExpInfo.u32AGain,
				stExpInfo.u32DGain, curLuma, lumaRatio, gainRatio);
		}
	}
}

static void AE_WorkFrameCheck(CVI_U8 sID, CVI_U32 expTime, CVI_U32 ISONum1, CVI_U32 ISONum2)
{
	CVI_U32 u32CheckCnt = 110;
	ISP_EXPOSURE_ATTR_S stExpAttr;

	memset(&stExpAttr, 0, sizeof(ISP_EXPOSURE_ATTR_S));

	CVI_ISP_GetExposureAttr(sID, &stExpAttr);

	stExpAttr.u8DebugMode = 82;

	stExpAttr.bByPass = 0;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISONumOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32ExpTime = expTime;
	stExpAttr.stManual.enGainType = AE_TYPE_ISO;
	stExpAttr.stManual.u32ISONum = ISONum1;

	CVI_ISP_SetExposureAttr(sID, &stExpAttr);

	for (CVI_U32 i = 0; i < u32CheckCnt; i++) {

		if (i % 20 == 0x00) {

			if (stExpAttr.stManual.u32ISONum == ISONum1) {
				stExpAttr.stManual.u32ISONum = ISONum2;
			} else {
				stExpAttr.stManual.u32ISONum = ISONum1;
			}

			CVI_ISP_SetExposureAttr(sID, &stExpAttr);
		}

		CVI_ISP_GetVDTimeOut(sID, ISP_VD_BE_END, 200);
	}

	stExpAttr.enOpType = OP_TYPE_AUTO;
	stExpAttr.u8DebugMode = 0;
	CVI_ISP_SetExposureAttr(sID, &stExpAttr);
}

static void sensor_ae_test_init(void)
{
	memset(&stSnsDft, 0, sizeof(AE_SENSOR_DEFAULT_S) * MAX_SENSOR_NUM);
	memset(&sensorInfo, 0, sizeof(_SENSOR_INFO_S) * MAX_SENSOR_NUM);

	init_sensor_info();
}

CVI_S32 sensor_ae_test(void)
{
	CVI_S32 sID = 0, item = 0, para1 = 0, para2 = 0, para3 = 0;
	CVI_S32 s32Ret = CVI_SUCCESS;

	sensor_ae_test_init();

	SAMPLE_PRT("\n1:AE_SetManualExposureTest(sID, 0:bypss 1:auto 2:manu, time, iso)\n");
	SAMPLE_PRT("2:AE_SetDebugMode(sID, item)\n");
	SAMPLE_PRT("3:AE_SetManualGainTest(sID, AG, DG, IG)\n");
	SAMPLE_PRT("4:AE_SetFpsTest(sID, fps)\n");
	SAMPLE_PRT("5:AE_SetLSC(sID, enable)\n");
	SAMPLE_PRT("6:AE_SetWDRManualRatio(sid, ratio), ratio: 4 - 256, 0: set SE max shutter time.\n");
	SAMPLE_PRT("7:AE_GainLinearTest(sID, time, startISO, endISO)\n");
	SAMPLE_PRT("8:AE_ShutterLinearTest(sID, fid 0: LE 1: SE, startExpTime, endExpTime)\n");
	SAMPLE_PRT("9:AE_GainTableLinearTest(sID, type: again 0 dgain 1, startIndex, endIndex)\n");
	SAMPLE_PRT("10:AE_WorkFrameCheck(sID, time, ISO 1, ISO 2)\n");
	SAMPLE_PRT("Item/sID/para1/para2/para3\n\n");

	scanf("%d %d %d %d %d", &item, &sID, &para1, &para2, &para3);

	if (sID >= MAX_SENSOR_NUM) {
		error_log("sID out of range...\n");
		s32Ret = CVI_FAILURE;
		return s32Ret;
	}

	switch (item) {
	case 1:
		AE_SetManualExposureTest(sID, para1, para2, para3);
		break;
	case 2:
		AE_SetDebugMode(sID, para1);
		break;
	case 3:
		AE_SetManualGainTest(sID, para1, para2, para3);
		break;
	case 4:
		AE_SetFpsTest(sID, para1);
		break;
	case 5:
		AE_SetLSC(sID, para1);
		break;
	case 6:
		AE_SetWDRManualRatio(sID, para1);
		break;
	case 7:
		AE_GainLinearTest(sID, para1, para2, para3);
		break;
	case 8:
		AE_ShutterLinearTest(sID, para1, para2, para3);
		break;
	case 9:
		AE_GainTableLinearTest(sID, para1, para2, para3);
		break;
	case 10:
		AE_WorkFrameCheck(sID, para1, para2, para3);
	default:
		break;
	}

	return s32Ret;
}

