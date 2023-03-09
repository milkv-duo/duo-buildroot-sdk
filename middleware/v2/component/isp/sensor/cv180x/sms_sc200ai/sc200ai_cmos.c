#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>
#ifdef ARCH_CV182X
#include "cvi_type.h"
#include "cvi_comm_video.h"
#include <linux/cvi_vip_snsr.h>
#else
#include <linux/cvi_type.h>
#include <linux/cvi_comm_video.h>
#include <linux/vi_snsr.h>
#endif
#include "cvi_debug.h"
#include "cvi_comm_sns.h"
#include "cvi_sns_ctrl.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_ae.h"
#include "cvi_awb.h"
#include "cvi_isp.h"

#include "sc200ai_cmos_ex.h"
#include "sc200ai_cmos_param.h"

#define DIV_0_TO_1(a)   ((0 == (a)) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))
#define SC200AI_ID 35
#define SENSOR_SC200AI_WIDTH 1920
#define SENSOR_SC200AI_HEIGHT 1080
/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastSC200AI[VI_MAX_PIPE_NUM] = {CVI_NULL};

#define SC200AI_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastSC200AI[dev])
#define SC200AI_SENSOR_SET_CTX(dev, pstCtx)   (g_pastSC200AI[dev] = pstCtx)
#define SC200AI_SENSOR_RESET_CTX(dev)         (g_pastSC200AI[dev] = CVI_NULL)

ISP_SNS_COMMBUS_U g_aunSC200AI_BusInfo[VI_MAX_PIPE_NUM] = {
	[0] = { .s8I2cDev = 0},
	[1 ... VI_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

CVI_U16 g_au16SC200AI_GainMode[VI_MAX_PIPE_NUM] = {0};
CVI_U16 g_au16SC200AI_L2SMode[VI_MAX_PIPE_NUM] = {0};

SC200AI_STATE_S g_astSC200AI_State[VI_MAX_PIPE_NUM] = {{0} };

/****************************************************************************
 * local variables and functions                                                           *
 ****************************************************************************/
static ISP_FSWDR_MODE_E genFSWDRMode[VI_MAX_PIPE_NUM] = {
	[0 ... VI_MAX_PIPE_NUM - 1] = ISP_FSWDR_NORMAL_MODE
};

static CVI_U32 gu32MaxTimeGetCnt[VI_MAX_PIPE_NUM] = {0};
static CVI_U32 g_au32InitExposure[VI_MAX_PIPE_NUM]  = {0};
static CVI_U32 g_au32LinesPer500ms[VI_MAX_PIPE_NUM] = {0};
static CVI_U16 g_au16InitWBGain[VI_MAX_PIPE_NUM][3] = {{0} };
static CVI_U16 g_au16SampleRgain[VI_MAX_PIPE_NUM] = {0};
static CVI_U16 g_au16SampleBgain[VI_MAX_PIPE_NUM] = {0};
static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg);
/*****SC200AI Lines Range*****/
#define SC200AI_FULL_LINES_MAX  (0x7FFF)
#define SC200AI_FULL_LINES_MAX_2TO1_WDR  (0x7FFF)

/*****SC200AI Register Address*****/
#define SC200AI_SHS1_0_ADDR		0x3E00
#define SC200AI_SHS1_1_ADDR		0x3E01
#define SC200AI_SHS1_2_ADDR		0x3E02
#define SC200AI_SHS2_0_ADDR		0x3E22
#define SC200AI_SHS2_1_ADDR		0x3E04
#define SC200AI_SHS2_2_ADDR		0x3E05
#define SC200AI_AGAIN1_ADDR		0x3E08
#define SC200AI_DGAIN1_ADDR		0x3E06
#define SC200AI_AGAIN2_ADDR		0x3E12
#define SC200AI_DGAIN2_ADDR		0x3E10
#define SC200AI_VMAX_ADDR		0x320E
#define SC200AI_MAXSEXP_ADDR		0x3E23
#define SC200AI_TABLE_END		0xFFFF

#define SC200AI_RES_IS_1080P(w, h)      ((w) <= 1920 && (h) <= 1080)

static CVI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
#if 0
	memset(&pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S));
#endif
	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32FlickerFreq = 50 * 256;
	pstAeSnsDft->u32FullLinesMax = SC200AI_FULL_LINES_MAX;
	pstAeSnsDft->u32HmaxTimes = (1000000) / (pstSnsState->u32FLStd * 30);

	pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
	pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

	pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
	pstAeSnsDft->stAgainAccu.f32Accuracy = 1;

	pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
	pstAeSnsDft->stDgainAccu.f32Accuracy = 1;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift;

	if (g_au32LinesPer500ms[ViPipe] == 0)
		pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * 30 / 2;
	else
		pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
	pstAeSnsDft->u32SnsStableFrame = 0;
#if 0
	pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
	pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;

	pstAeSnsDft->bAERouteExValid = CVI_FALSE;
	pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
	pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;
#endif
	switch (pstSnsState->enWDRMode) {
	default:
	case WDR_MODE_NONE:   /*linear mode*/
		pstAeSnsDft->f32Fps = g_astSC200AI_mode[SC200AI_MODE_1080P30].f32MaxFps;
		pstAeSnsDft->f32MinFps = g_astSC200AI_mode[SC200AI_MODE_1080P30].f32MinFps;
		pstAeSnsDft->au8HistThresh[0] = 0xd;
		pstAeSnsDft->au8HistThresh[1] = 0x28;
		pstAeSnsDft->au8HistThresh[2] = 0x60;
		pstAeSnsDft->au8HistThresh[3] = 0x80;

		pstAeSnsDft->u32MaxAgain = 55270;
		pstAeSnsDft->u32MinAgain = 1024;
		pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

		pstAeSnsDft->u32MaxDgain = 32512;
		pstAeSnsDft->u32MinDgain = 1024;
		pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

		pstAeSnsDft->u8AeCompensation = 40;
		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 5;
		pstAeSnsDft->u32AEResponseFrame = 4;
		pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 76151;

		pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 4;
		pstAeSnsDft->u32MinIntTime = 1;
		pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		pstAeSnsDft->u32MinIntTimeTarget = 1;
		break;

	case WDR_MODE_2To1_LINE:
		pstAeSnsDft->f32Fps = g_astSC200AI_mode[SC200AI_MODE_1080P30_WDR].f32MaxFps;
		pstAeSnsDft->f32MinFps = g_astSC200AI_mode[SC200AI_MODE_1080P30_WDR].f32MinFps;
		pstAeSnsDft->au8HistThresh[0] = 0xC;
		pstAeSnsDft->au8HistThresh[1] = 0x18;
		pstAeSnsDft->au8HistThresh[2] = 0x60;
		pstAeSnsDft->au8HistThresh[3] = 0x80;

		pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 5;
		pstAeSnsDft->u32MinIntTime = 1;

		pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

		pstAeSnsDft->u32MaxAgain = 55270;
		pstAeSnsDft->u32MinAgain = 1024;
		pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

		pstAeSnsDft->u32MaxDgain = 32512;
		pstAeSnsDft->u32MinDgain = 1024;
		pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
		pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift;

		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 52000;
		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 5;
		pstAeSnsDft->u32AEResponseFrame = 4;
		if (genFSWDRMode[ViPipe] == ISP_FSWDR_LONG_FRAME_MODE) {
			pstAeSnsDft->u8AeCompensation = 64;
			pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		} else {
			pstAeSnsDft->u8AeCompensation = 40;
			pstAeSnsDft->enAeExpMode = AE_EXP_LOWLIGHT_PRIOR;
			/* [TODO] */
#if 0
			pstAeSnsDft->u16ManRatioEnable = CVI_TRUE;
			pstAeSnsDft->au32Ratio[0] = 0x400;
			pstAeSnsDft->au32Ratio[1] = 0x40;
			pstAeSnsDft->au32Ratio[2] = 0x40;
#endif
		}
		break;
	}

	return CVI_SUCCESS;
}

/* the function of sensor set fps */
static CVI_S32 cmos_fps_set(VI_PIPE ViPipe, CVI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	CVI_U32 u32VMAX;
	CVI_FLOAT f32MaxFps = 0;
	CVI_FLOAT f32MinFps = 0;
	CVI_U32 u32Vts = 0;
	CVI_U16 u16MaxSexpReg;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32Vts = g_astSC200AI_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;
	f32MaxFps = g_astSC200AI_mode[pstSnsState->u8ImgMode].f32MaxFps;
	f32MinFps = g_astSC200AI_mode[pstSnsState->u8ImgMode].f32MinFps;
	u16MaxSexpReg = g_astSC200AI_mode[pstSnsState->u8ImgMode].u16SexpMaxReg;

	switch (pstSnsState->u8ImgMode) {
	case SC200AI_MODE_1080P30_WDR:
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
			u16MaxSexpReg = u16MaxSexpReg * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Not support Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}
		u32VMAX = (u32VMAX > SC200AI_FULL_LINES_MAX_2TO1_WDR) ? SC200AI_FULL_LINES_MAX_2TO1_WDR : u32VMAX;
		break;

	case SC200AI_MODE_1080P30:
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Not support Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}
		u32VMAX = (u32VMAX > SC200AI_FULL_LINES_MAX) ? SC200AI_FULL_LINES_MAX : u32VMAX;
		break;
	default:
		CVI_TRACE_SNS(CVI_DBG_ERR, "Not support sensor mode: %d\n", pstSnsState->u8ImgMode);
		return CVI_FAILURE;
	}

	pstSnsState->u32FLStd = u32VMAX;

	if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) {
		g_astSC200AI_State[ViPipe].u32Sexp_MAX = u16MaxSexpReg - 5;
	}
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstSnsRegsInfo->astI2cData[LINEAR_VMAX_0_ADDR].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_VMAX_1_ADDR].u32Data = (u32VMAX & 0xFF);
	} else {
		pstSnsRegsInfo->astI2cData[WDR2_VMAX_0_ADDR].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_VMAX_1_ADDR].u32Data = (u32VMAX & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_MAXSEXP_0_ADDR].u32Data = ((u16MaxSexpReg & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_MAXSEXP_1_ADDR].u32Data = u16MaxSexpReg & 0xFF;
	}

	pstAeSnsDft->f32Fps = f32Fps;
	pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 5;
	pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
	pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
	pstAeSnsDft->u32HmaxTimes = (1000000) / (pstSnsState->u32FLStd * DIV_0_TO_1_FLOAT(f32Fps));

	return CVI_SUCCESS;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static CVI_S32 cmos_inttime_update(VI_PIPE ViPipe, CVI_U32 *u32IntTime)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(u32IntTime);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) {
		CVI_U32 u32ShortIntTime = u32IntTime[0];
		CVI_U32 u32LongIntTime = u32IntTime[1];
		CVI_U16 u16SexpReg, u16LexpReg;
		CVI_U32 u32MaxLExp;

		/* short exposure reg range:
		 * min : 1
		 * max : 2 * reg_sexp_max - 10
		 * step : 4
		 */
		pstSnsState->au32WDRIntTime[0] = (u32ShortIntTime > g_astSC200AI_State[ViPipe].u32Sexp_MAX) ?
			g_astSC200AI_State[ViPipe].u32Sexp_MAX : u32ShortIntTime;
		if (!pstSnsState->au32WDRIntTime[0])
			pstSnsState->au32WDRIntTime[0] = 1;
		/* short exp = SexpReg / 2 */
		u16SexpReg = (CVI_U16)(pstSnsState->au32WDRIntTime[0] * 2 - 1);
		u32IntTime[0] = pstSnsState->au32WDRIntTime[0];

		/* long exposure reg range:
		 * min : 1
		 * max : 2 * (vts - max sexp)
		 * step : 4
		 */
		u32MaxLExp = pstSnsState->au32FL[0] - g_astSC200AI_State[ViPipe].u32Sexp_MAX;
		pstSnsState->au32WDRIntTime[1] = (u32LongIntTime > u32MaxLExp) ? u32MaxLExp : u32LongIntTime;
		if (!pstSnsState->au32WDRIntTime[1])
			pstSnsState->au32WDRIntTime[1] = 1;
		u16LexpReg = (CVI_U16)(pstSnsState->au32WDRIntTime[1] * 2 - 1);
		u32IntTime[1] = pstSnsState->au32WDRIntTime[1];

		pstSnsRegsInfo->astI2cData[WDR2_SHS1_0_ADDR].u32Data = ((u16LexpReg & 0xF000) >> 12);
		pstSnsRegsInfo->astI2cData[WDR2_SHS1_1_ADDR].u32Data = ((u16LexpReg & 0x0FF0) >> 4);
		pstSnsRegsInfo->astI2cData[WDR2_SHS1_2_ADDR].u32Data = (u16LexpReg & 0xF) << 4;

		pstSnsRegsInfo->astI2cData[WDR2_SHS2_0_ADDR].u32Data = ((u16SexpReg & 0xF000) >> 12);
		pstSnsRegsInfo->astI2cData[WDR2_SHS2_1_ADDR].u32Data = ((u16SexpReg & 0x0FF0) >> 4);
		pstSnsRegsInfo->astI2cData[WDR2_SHS2_2_ADDR].u32Data = (u16SexpReg & 0xF) << 4;
		/* update isp */
		cmos_get_wdr_size(ViPipe, &pstSnsState->astSyncInfo[0].ispCfg);
	} else {
		CVI_U32 u32TmpIntTime = u32IntTime[0];
		/* linear exposure reg range:
		 * min : 1
		 * max : 2 * (vts - 8)
		 * step : 1
		 */
		u32TmpIntTime = (u32TmpIntTime > (pstSnsState->au32FL[0] - 4)) ?
				(pstSnsState->au32FL[0] - 4) : u32TmpIntTime;
		u32TmpIntTime = u32TmpIntTime << 1;
		if (!u32TmpIntTime)
			u32TmpIntTime = 1;

		pstSnsRegsInfo->astI2cData[LINEAR_SHS1_0_ADDR].u32Data = ((u32TmpIntTime & 0xF000) >> 12);
		pstSnsRegsInfo->astI2cData[LINEAR_SHS1_1_ADDR].u32Data = ((u32TmpIntTime & 0x0FF0) >> 4);
		pstSnsRegsInfo->astI2cData[LINEAR_SHS1_2_ADDR].u32Data = (u32TmpIntTime & 0xF) << 4;
	}

	return CVI_SUCCESS;

}

struct gain_tbl_info_s {
	CVI_U16	gainMax;
	CVI_U16	idxBase;
	CVI_U8	regGain;
	CVI_U8	regGainFineBase;
	CVI_U8	regGainFineStep;
};

static struct gain_tbl_info_s AgainInfo[6] = {
	{
		.gainMax = 2031,
		.idxBase = 0,
		.regGain = 0x03,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
	{
		.gainMax = 3456,
		.idxBase = 64,
		.regGain = 0x07,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
	{
		.gainMax = 6908,
		.idxBase = 109,
		.regGain = 0x23,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
	{
		.gainMax = 13817,
		.idxBase = 173,
		.regGain = 0x27,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
	{
		.gainMax = 27635,
		.idxBase = 237,
		.regGain = 0x2f,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
	{
		.gainMax = 55270,
		.idxBase = 301,
		.regGain = 0x3f,
		.regGainFineBase = 0x40,
		.regGainFineStep = 1,
	},
};

static CVI_U32 Again_table[365] = {
	1024, 1040, 1055, 1072, 1088, 1103, 1120, 1135, 1152, 1168, 1183, 1200, 1216, 1231, 1248, 1263, 1280,
	1296, 1311, 1328, 1344, 1359, 1376, 1391, 1408, 1424, 1439, 1456, 1472, 1487, 1504, 1519, 1536, 1552,
	1567, 1584, 1600, 1615, 1632, 1647, 1664, 1680, 1695, 1712, 1728, 1743, 1760, 1775, 1792, 1808, 1823,
	1840, 1856, 1871, 1888, 1903, 1920, 1936, 1951, 1968, 1984, 1999, 2016, 2031, 2048, 2079, 2112, 2144,
	2176, 2207, 2240, 2272, 2304, 2335, 2368, 2400, 2432, 2463, 2496, 2528, 2560, 2591, 2624, 2656, 2688,
	2719, 2752, 2784, 2816, 2847, 2880, 2912, 2944, 2975, 3008, 3040, 3072, 3103, 3136, 3168, 3200, 3231,
	3264, 3296, 3328, 3359, 3392, 3424, 3456, 3481, 3535, 3590, 3644, 3699, 3753, 3808, 3862, 3916, 3971,
	4025, 4079, 4134, 4189, 4243, 4297, 4352, 4406, 4460, 4514, 4570, 4624, 4678, 4732, 4787, 4841, 4895,
	4950, 5005, 5059, 5113, 5168, 5222, 5276, 5330, 5385, 5440, 5494, 5549, 5603, 5657, 5711, 5766, 5820,
	5875, 5929, 5984, 6038, 6092, 6147, 6201, 6255, 6310, 6365, 6419, 6473, 6528, 6582, 6636, 6690, 6746,
	6800, 6854, 6908, 6963, 7071, 7181, 7289, 7398, 7506, 7616, 7725, 7833, 7942, 8051, 8160, 8268, 8377,
	8486, 8595, 8704, 8812, 8922, 9030, 9139, 9247, 9357, 9465, 9574, 9682, 9792, 9901, 10009, 10118, 10227,
	10336, 10444, 10553, 10662, 10771, 10880, 10988, 11098, 11206, 11315, 11423, 11533, 11641, 11750, 11858,
	11968, 12077, 12185, 12294, 12403, 12512, 12620, 12729, 12838, 12947, 13056, 13164, 13274, 13382, 13491,
	13599, 13709, 13817, 13926, 14144, 14361, 14579, 14796, 15014, 15232, 15450, 15667, 15885, 16102, 16320,
	16537, 16755, 16972, 17190, 17408, 17626, 17843, 18061, 18278, 18496, 18713, 18931, 19148, 19366, 19584,
	19802, 20019, 20237, 20454, 20672, 20889, 21107, 21324, 21542, 21760, 21978, 22195, 22413, 22630, 22848,
	23065, 23283, 23500, 23718, 23936, 24154, 24371, 24589, 24806, 25024, 25241, 25459, 25676, 25894, 26112,
	26330, 26547, 26765, 26982, 27200, 27417, 27635, 27852, 28288, 28723, 29158, 29593, 30028, 30464, 30899,
	31334, 31769, 32204, 32640, 33075, 33510, 33945, 34380, 34816, 35251, 35686, 36121, 36556, 36992, 37427,
	37862, 38297, 38732, 39168, 39603, 40038, 40473, 40908, 41344, 41779, 42214, 42649, 43084, 43520, 43955,
	44390, 44825, 45260, 45696, 46131, 46566, 47001, 47436, 47872, 48307, 48742, 49177, 49612, 50048, 50483,
	50918, 51353, 51788, 52224, 52659, 53094, 53529, 53964, 54400, 54835, 55270
};

static struct gain_tbl_info_s DgainInfo[5] = {
	{
		.gainMax = 2031,
		.idxBase = 0,
		.regGain = 0x00,
		.regGainFineBase = 0x80,
		.regGainFineStep = 2,
	},
	{
		.gainMax = 4064,
		.idxBase = 64,
		.regGain = 0x01,
		.regGainFineBase = 0x80,
		.regGainFineStep = 2,
	},
	{
		.gainMax = 8128,
		.idxBase = 128,
		.regGain = 0x03,
		.regGainFineBase = 0x80,
		.regGainFineStep = 2,
	},
	{
		.gainMax = 16256,
		.idxBase = 192,
		.regGain = 0x07,
		.regGainFineBase = 0x80,
		.regGainFineStep = 2,
	},
	{
		.gainMax = 32512,
		.idxBase = 256,
		.regGain = 0x0f,
		.regGainFineBase = 0x80,
		.regGainFineStep = 2,
	},
};

static CVI_U32 Dgain_table[320] = {
	1024, 1040, 1055, 1072, 1088, 1103, 1120, 1135, 1152, 1168, 1183, 1200, 1216, 1231, 1248,
	1263, 1280, 1296, 1311, 1328, 1344, 1359, 1376, 1391, 1408, 1424, 1439, 1456, 1472, 1487,
	1504, 1519, 1536, 1552, 1567, 1584, 1600, 1615, 1632, 1647, 1664, 1680, 1695, 1712, 1728,
	1743, 1760, 1775, 1792, 1808, 1823, 1840, 1856, 1871, 1888, 1903, 1920, 1936, 1951, 1968,
	1984, 1999, 2016, 2031, 2048, 2079, 2112, 2144, 2176, 2207, 2240, 2272, 2304, 2335, 2368,
	2400, 2432, 2463, 2496, 2528, 2560, 2591, 2624, 2656, 2688, 2719, 2752, 2784, 2816, 2847,
	2880, 2912, 2944, 2975, 3008, 3040, 3072, 3103, 3136, 3168, 3200, 3231, 3264, 3296, 3328,
	3359, 3392, 3424, 3456, 3487, 3520, 3552, 3584, 3615, 3648, 3680, 3712, 3743, 3776, 3808,
	3840, 3871, 3904, 3936, 3968, 3999, 4032, 4064, 4096, 4160, 4224, 4288, 4352, 4416, 4480,
	4544, 4608, 4672, 4736, 4800, 4864, 4928, 4992, 5056, 5120, 5184, 5248, 5312, 5376, 5440,
	5504, 5568, 5632, 5696, 5760, 5824, 5888, 5952, 6016, 6080, 6144, 6208, 6272, 6336, 6400,
	6464, 6528, 6592, 6656, 6720, 6784, 6848, 6912, 6976, 7040, 7104, 7168, 7232, 7296, 7360,
	7424, 7488, 7552, 7616, 7680, 7744, 7808, 7872, 7936, 8000, 8064, 8128, 8192, 8320, 8448,
	8576, 8704, 8832, 8960, 9088, 9216, 9344, 9472, 9600, 9728, 9856, 9984, 10112, 10240, 10368,
	10496, 10624, 10752, 10880, 11008, 11136, 11264, 11392, 11520, 11648, 11776, 11904, 12032,
	12160, 12288, 12416, 12544, 12672, 12800, 12928, 13056, 13184, 13312, 13440, 13568, 13696,
	13824, 13952, 14080, 14208, 14336, 14464, 14592, 14720, 14848, 14976, 15104, 15232, 15360,
	15488, 15616, 15744, 15872, 16000, 16128, 16256, 16384, 16640, 16896, 17152, 17408, 17664,
	17920, 18176, 18432, 18688, 18944, 19200, 19456, 19712, 19968, 20224, 20480, 20736, 20992,
	21248, 21504, 21760, 22016, 22272, 22528, 22784, 23040, 23296, 23552, 23808, 24064, 24320,
	24576, 24832, 25088, 25344, 25600, 25856, 26112, 26368, 26624, 26880, 27136, 27392, 27648,
	27904, 28160, 28416, 28672, 28928, 29184, 29440, 29696, 29952, 30208, 30464, 30720, 30976,
	31232, 31488, 31744, 32000, 32256, 32512
};

static CVI_S32 cmos_again_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32AgainLin, CVI_U32 *pu32AgainDb)
{
	int i;

	(void)ViPipe;

	CMOS_CHECK_POINTER(pu32AgainLin);
	CMOS_CHECK_POINTER(pu32AgainDb);

	if (*pu32AgainLin >= Again_table[364]) {
		*pu32AgainLin = Again_table[364];
		*pu32AgainDb = 364;
		return CVI_SUCCESS;
	}

	for (i = 1; i < 365; i++) {
		if (*pu32AgainLin < Again_table[i]) {
			*pu32AgainLin = Again_table[i - 1];
			*pu32AgainDb = i - 1;
			break;
		}
	}
	return CVI_SUCCESS;
}

static CVI_S32 cmos_dgain_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32DgainLin, CVI_U32 *pu32DgainDb)
{
	int i;

	(void)ViPipe;

	CMOS_CHECK_POINTER(pu32DgainLin);
	CMOS_CHECK_POINTER(pu32DgainDb);

	if (*pu32DgainLin >= Dgain_table[319]) {
		*pu32DgainLin = Dgain_table[319];
		*pu32DgainDb = 319;
		return CVI_SUCCESS;
	}

	for (i = 1; i < 320; i++) {
		if (*pu32DgainLin < Dgain_table[i]) {
			*pu32DgainLin = Dgain_table[i - 1];
			*pu32DgainDb = i - 1;
			break;
		}
	}
	return CVI_SUCCESS;
}

static CVI_S32 cmos_gains_update(VI_PIPE ViPipe, CVI_U32 *pu32Again, CVI_U32 *pu32Dgain)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	CVI_U32 u16Mode = g_au16SC200AI_GainMode[ViPipe];
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	struct gain_tbl_info_s *info;
	int i, tbl_num;
	static bool bGainLogicChanged[VI_MAX_PIPE_NUM] = {true};

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pu32Again);
	CMOS_CHECK_POINTER(pu32Dgain);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	u32Again = pu32Again[0];
	u32Dgain = pu32Dgain[0];

	// control sensor DPC to fix high-gain noise
	if (Again_table[u32Again] >= 30720 && bGainLogicChanged[ViPipe]) {
		sc200ai_write_register(ViPipe, 0x5799, 0x7);
		bGainLogicChanged[ViPipe] = false;
	} else if (Again_table[u32Again] <= 20480 && !bGainLogicChanged[ViPipe]) {
		sc200ai_write_register(ViPipe, 0x5799, 0x0);
		bGainLogicChanged[ViPipe] = true;
	}

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		/* linear mode */

		/* find Again register setting. */
		tbl_num = sizeof(AgainInfo)/sizeof(struct gain_tbl_info_s);
		for (i = tbl_num - 1; i >= 0; i--) {
			info = &AgainInfo[i];

			if (u32Again >= info->idxBase)
				break;
		}

		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN_0_ADDR].u32Data = (info->regGain & 0xFF);
		u32Again = info->regGainFineBase + (u32Again - info->idxBase) * info->regGainFineStep;
		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN_1_ADDR].u32Data = (u32Again & 0xFF);

		/* find Dgain register setting. */
		tbl_num = sizeof(DgainInfo)/sizeof(struct gain_tbl_info_s);
		for (i = tbl_num - 1; i >= 0; i--) {
			info = &DgainInfo[i];

			if (u32Dgain >= info->idxBase)
				break;
		}

		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_0_ADDR].u32Data = (info->regGain & 0xFF);
		u32Dgain = info->regGainFineBase + (u32Dgain - info->idxBase) * info->regGainFineStep;
		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_1_ADDR].u32Data = (u32Dgain & 0xFF);
	} else {
		/* DOL mode */
		if (u16Mode == SNS_GAIN_MODE_WDR_2F) {
			/* find SEF Again register setting. */
			tbl_num = sizeof(AgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &AgainInfo[i];

				if (u32Again >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Again = info->regGainFineBase + (u32Again - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_1_ADDR].u32Data = (u32Again & 0xFF);

			/* find SEF Dgain register setting. */
			tbl_num = sizeof(DgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &DgainInfo[i];

				if (u32Dgain >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Dgain = info->regGainFineBase + (u32Dgain - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_1_ADDR].u32Data = (u32Dgain & 0xFF);

			u32Again = pu32Again[1];
			u32Dgain = pu32Dgain[1];

			/* find LEF Again register setting. */
			tbl_num = sizeof(AgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &AgainInfo[i];

				if (u32Again >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Again = info->regGainFineBase + (u32Again - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_1_ADDR].u32Data = (u32Again & 0xFF);

			/* find LEF Dgain register setting. */
			tbl_num = sizeof(DgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &DgainInfo[i];

				if (u32Dgain >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Dgain = info->regGainFineBase + (u32Dgain - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_1_ADDR].u32Data = (u32Dgain & 0xFF);
		} else if (u16Mode == SNS_GAIN_MODE_SHARE) {
			/* find Again register setting. */
			tbl_num = sizeof(AgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &AgainInfo[i];

				if (u32Again >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_0_ADDR].u32Data = (info->regGain & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Again = info->regGainFineBase + (u32Again - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_1_ADDR].u32Data = (u32Again & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_1_ADDR].u32Data = (u32Again & 0xFF);

			/* find Dgain register setting. */
			tbl_num = sizeof(DgainInfo)/sizeof(struct gain_tbl_info_s);
			for (i = tbl_num - 1; i >= 0; i--) {
				info = &DgainInfo[i];

				if (u32Dgain >= info->idxBase)
					break;
			}

			pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_0_ADDR].u32Data = (info->regGain & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_0_ADDR].u32Data = (info->regGain & 0xFF);
			u32Dgain = info->regGainFineBase + (u32Dgain - info->idxBase) * info->regGainFineStep;
			pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_1_ADDR].u32Data = (u32Dgain & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_1_ADDR].u32Data = (u32Dgain & 0xFF);
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_inttime_max(VI_PIPE ViPipe, CVI_U16 u16ManRatioEnable, CVI_U32 *au32Ratio,
		CVI_U32 *au32IntTimeMax, CVI_U32 *au32IntTimeMin, CVI_U32 *pu32LFMaxIntTime)
{
	CVI_U32 u32IntTimeMaxTmp  = 0;
	CVI_U32 u32ShortTimeMinLimit = 1;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	(void) u16ManRatioEnable;

	CMOS_CHECK_POINTER(au32Ratio);
	CMOS_CHECK_POINTER(au32IntTimeMax);
	CMOS_CHECK_POINTER(au32IntTimeMin);
	CMOS_CHECK_POINTER(pu32LFMaxIntTime);
	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	/* short exposure reg range:
	 * min : 1
	 * max : 2 * (max sexp - 5)
	 * step : 4
	 * long exposure reg range:
	 * min : 1
	 * max : 2 * (vts - max sexp - 5)
	 * step : 4
	 */
	u32IntTimeMaxTmp = ((2 * pstSnsState->au32FL[0] - 10) * 0x40) / (au32Ratio[0] + 0x40) / 4 * 4;
	u32IntTimeMaxTmp = (u32IntTimeMaxTmp > (g_astSC200AI_State[ViPipe].u32Sexp_MAX * 2)) ?
				(g_astSC200AI_State[ViPipe].u32Sexp_MAX * 2) : u32IntTimeMaxTmp;
	u32IntTimeMaxTmp  = (!u32IntTimeMaxTmp) ? u32ShortTimeMinLimit : u32IntTimeMaxTmp;

	if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
		/* [TODO] Convert to 1-line unit */
		u32IntTimeMaxTmp = (u32IntTimeMaxTmp - 1) / 2;
		u32ShortTimeMinLimit = (u32ShortTimeMinLimit + 1) / 2;
		au32IntTimeMax[0] = u32IntTimeMaxTmp;
		au32IntTimeMax[1] = au32IntTimeMax[0] * au32Ratio[0] >> 6;
		au32IntTimeMax[2] = au32IntTimeMax[1] * au32Ratio[1] >> 6;
		au32IntTimeMax[3] = au32IntTimeMax[2] * au32Ratio[2] >> 6;
		au32IntTimeMin[0] = u32ShortTimeMinLimit;
		au32IntTimeMin[1] = au32IntTimeMin[0] * au32Ratio[0] >> 6;
		au32IntTimeMin[2] = au32IntTimeMin[1] * au32Ratio[1] >> 6;
		au32IntTimeMin[3] = au32IntTimeMin[2] * au32Ratio[2] >> 6;
		syslog(LOG_DEBUG, "ViPipe = %d ratio = %d, (%d, %d)\n", ViPipe, au32Ratio[0],
				u32IntTimeMaxTmp, u32ShortTimeMinLimit);
	}

	return CVI_SUCCESS;
}

/* Only used in LINE_WDR mode */
static CVI_S32 cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
	CMOS_CHECK_POINTER(pstAeFSWDRAttr);

	genFSWDRMode[ViPipe] = pstAeFSWDRAttr->enFSWDRMode;
	gu32MaxTimeGetCnt[ViPipe] = 0;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	CMOS_CHECK_POINTER(pstExpFuncs);

	memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

	pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	//pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
	pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
	pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	(void) ViPipe;

	CMOS_CHECK_POINTER(pstAwbSnsDft);

	memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

	pstAwbSnsDft->u16InitGgain = 1024;
	pstAwbSnsDft->u8AWBRunInterval = 1;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	CMOS_CHECK_POINTER(pstExpFuncs);

	memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
	(void) ViPipe;

	memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

	memcpy(pstDef->stNoiseCalibration.CalibrationCoef,
		&g_stIspNoiseCalibratio, sizeof(ISP_CMOS_NOISE_CALIBRATION_S));
	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_blc_default(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlc)
{
	(void) ViPipe;

	CMOS_CHECK_POINTER(pstBlc);

	memset(pstBlc, 0, sizeof(ISP_CMOS_BLACK_LEVEL_S));

	memcpy(pstBlc,
		&g_stIspBlcCalibratio, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg)
{
	const SC200AI_MODE_S *pstMode = CVI_NULL;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstMode = &g_astSC200AI_mode[pstSnsState->u8ImgMode];

	if (pstSnsState->enWDRMode != WDR_MODE_NONE) {
		pstIspCfg->frm_num = 2;
		memcpy(&pstIspCfg->img_size[0], &pstMode->astImg[0], sizeof(ISP_WDR_SIZE_S));
		memcpy(&pstIspCfg->img_size[1], &pstMode->astImg[1], sizeof(ISP_WDR_SIZE_S));
	} else {
		pstIspCfg->frm_num = 1;
		memcpy(&pstIspCfg->img_size[0], &pstMode->astImg[0], sizeof(ISP_WDR_SIZE_S));
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, CVI_U8 u8Mode)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstSnsState->bSyncInit = CVI_FALSE;

	switch (u8Mode) {
	case WDR_MODE_NONE:
		if (pstSnsState->u8ImgMode == SC200AI_MODE_1080P30_WDR)
			pstSnsState->u8ImgMode = SC200AI_MODE_1080P30;
		pstSnsState->enWDRMode = WDR_MODE_NONE;
		pstSnsState->u32FLStd = g_astSC200AI_mode[pstSnsState->u8ImgMode].u32VtsDef;
		syslog(LOG_INFO, "linear mode\n");
		break;

	case WDR_MODE_2To1_LINE:
		if (pstSnsState->u8ImgMode == SC200AI_MODE_1080P30)
			pstSnsState->u8ImgMode = SC200AI_MODE_1080P30_WDR;
		pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
		pstSnsState->u32FLStd = g_astSC200AI_mode[pstSnsState->u8ImgMode].u32VtsDef;
		syslog(LOG_INFO, "2to1 line WDR 1080p mode(60fps->30fps)\n");
		break;
	default:
		CVI_TRACE_SNS(CVI_DBG_ERR, "NOT support this mode!\n");
		return CVI_FAILURE;
	}

	pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
	pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
	memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));

	return CVI_SUCCESS;
}

static CVI_U32 sensor_cmp_wdr_size(ISP_SNS_ISP_INFO_S *pstWdr1, ISP_SNS_ISP_INFO_S *pstWdr2)
{
	CVI_U32 i;

	if (pstWdr1->frm_num != pstWdr2->frm_num)
		goto _mismatch;
	for (i = 0; i < 2; i++) {
		if (pstWdr1->img_size[i].stSnsSize.u32Width != pstWdr2->img_size[i].stSnsSize.u32Width)
			goto _mismatch;
		if (pstWdr1->img_size[i].stSnsSize.u32Height != pstWdr2->img_size[i].stSnsSize.u32Height)
			goto _mismatch;
		if (pstWdr1->img_size[i].stWndRect.s32X != pstWdr2->img_size[i].stWndRect.s32X)
			goto _mismatch;
		if (pstWdr1->img_size[i].stWndRect.s32Y != pstWdr2->img_size[i].stWndRect.s32Y)
			goto _mismatch;
		if (pstWdr1->img_size[i].stWndRect.u32Width != pstWdr2->img_size[i].stWndRect.u32Width)
			goto _mismatch;
		if (pstWdr1->img_size[i].stWndRect.u32Height != pstWdr2->img_size[i].stWndRect.u32Height)
			goto _mismatch;
	}

	return 0;
_mismatch:
	return 1;
}

static CVI_U32 sensor_cmp_cif_wdr(ISP_SNS_CIF_INFO_S *pstWdr1, ISP_SNS_CIF_INFO_S *pstWdr2)
{
	if (pstWdr1->wdr_manual.l2s_distance != pstWdr2->wdr_manual.l2s_distance)
		goto _mismatch;
	if (pstWdr1->wdr_manual.lsef_length != pstWdr2->wdr_manual.lsef_length)
		goto _mismatch;

	return 0;
_mismatch:
	return 1;
}

static CVI_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsSyncInfo)
{
	CVI_U32 i;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	ISP_SNS_SYNC_INFO_S *pstCfg0 = CVI_NULL;
	ISP_SNS_SYNC_INFO_S *pstCfg1 = CVI_NULL;
	ISP_I2C_DATA_S *pstI2c_data = CVI_NULL;

	CMOS_CHECK_POINTER(pstSnsSyncInfo);
	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstSnsRegsInfo = &pstSnsSyncInfo->snsCfg;
	pstCfg0 = &pstSnsState->astSyncInfo[0];
	pstCfg1 = &pstSnsState->astSyncInfo[1];
	pstI2c_data = pstCfg0->snsCfg.astI2cData;

	if ((pstSnsState->bSyncInit == CVI_FALSE) || (pstSnsRegsInfo->bConfig == CVI_FALSE)) {
		pstCfg0->snsCfg.enSnsType = SNS_I2C_TYPE;
		pstCfg0->snsCfg.unComBus.s8I2cDev = g_aunSC200AI_BusInfo[ViPipe].s8I2cDev;
		pstCfg0->snsCfg.u8Cfg2ValidDelayMax = 0;
		pstCfg0->snsCfg.use_snsr_sram = CVI_TRUE;
		pstCfg0->snsCfg.u32RegNum = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ?
					WDR2_REGS_NUM : LINEAR_REGS_NUM;

		for (i = 0; i < pstCfg0->snsCfg.u32RegNum; i++) {
			pstI2c_data[i].bUpdate = CVI_TRUE;
			pstI2c_data[i].u8DevAddr = sc200ai_i2c_addr;
			pstI2c_data[i].u32AddrByteNum = sc200ai_addr_byte;
			pstI2c_data[i].u32DataByteNum = sc200ai_data_byte;
		}

		//DOL 2t1 Mode Regs
		switch (pstSnsState->enWDRMode) {
		case WDR_MODE_2To1_LINE:
			//WDR Mode Regs
			pstI2c_data[WDR2_SHS1_0_ADDR].u32RegAddr = SC200AI_SHS1_0_ADDR;
			pstI2c_data[WDR2_SHS1_1_ADDR].u32RegAddr = SC200AI_SHS1_1_ADDR;
			pstI2c_data[WDR2_SHS1_2_ADDR].u32RegAddr = SC200AI_SHS1_2_ADDR;
			pstI2c_data[WDR2_SHS2_0_ADDR].u32RegAddr = SC200AI_SHS2_0_ADDR;
			pstI2c_data[WDR2_SHS2_1_ADDR].u32RegAddr = SC200AI_SHS2_1_ADDR;
			pstI2c_data[WDR2_SHS2_2_ADDR].u32RegAddr = SC200AI_SHS2_2_ADDR;
			pstI2c_data[WDR2_AGAIN1_0_ADDR].u32RegAddr = SC200AI_AGAIN1_ADDR;
			pstI2c_data[WDR2_AGAIN1_1_ADDR].u32RegAddr = SC200AI_AGAIN1_ADDR + 1;
			pstI2c_data[WDR2_DGAIN1_0_ADDR].u32RegAddr = SC200AI_DGAIN1_ADDR;
			pstI2c_data[WDR2_DGAIN1_1_ADDR].u32RegAddr = SC200AI_DGAIN1_ADDR + 1;
			pstI2c_data[WDR2_AGAIN2_0_ADDR].u32RegAddr = SC200AI_AGAIN2_ADDR;
			pstI2c_data[WDR2_AGAIN2_1_ADDR].u32RegAddr = SC200AI_AGAIN2_ADDR + 1;
			pstI2c_data[WDR2_DGAIN2_0_ADDR].u32RegAddr = SC200AI_DGAIN2_ADDR;
			pstI2c_data[WDR2_DGAIN2_1_ADDR].u32RegAddr = SC200AI_DGAIN2_ADDR + 1;
			pstI2c_data[WDR2_VMAX_0_ADDR].u32RegAddr = SC200AI_VMAX_ADDR;
			pstI2c_data[WDR2_VMAX_1_ADDR].u32RegAddr = SC200AI_VMAX_ADDR + 1;
			pstI2c_data[WDR2_MAXSEXP_0_ADDR].u32RegAddr = SC200AI_MAXSEXP_ADDR;
			pstI2c_data[WDR2_MAXSEXP_1_ADDR].u32RegAddr = SC200AI_MAXSEXP_ADDR + 1;

			break;
		default:
			//Linear Mode Regs
			pstI2c_data[LINEAR_SHS1_0_ADDR].u32RegAddr = SC200AI_SHS1_0_ADDR;
			pstI2c_data[LINEAR_SHS1_1_ADDR].u32RegAddr = SC200AI_SHS1_1_ADDR;
			pstI2c_data[LINEAR_SHS1_2_ADDR].u32RegAddr = SC200AI_SHS1_2_ADDR;
			pstI2c_data[LINEAR_AGAIN_0_ADDR].u32RegAddr = SC200AI_AGAIN1_ADDR;
			pstI2c_data[LINEAR_AGAIN_1_ADDR].u32RegAddr = SC200AI_AGAIN1_ADDR + 1;
			pstI2c_data[LINEAR_DGAIN_0_ADDR].u32RegAddr = SC200AI_DGAIN1_ADDR;
			pstI2c_data[LINEAR_DGAIN_1_ADDR].u32RegAddr = SC200AI_DGAIN1_ADDR + 1;
			pstI2c_data[LINEAR_VMAX_0_ADDR].u32RegAddr = SC200AI_VMAX_ADDR;
			pstI2c_data[LINEAR_VMAX_1_ADDR].u32RegAddr = SC200AI_VMAX_ADDR + 1;

			break;
		}
		pstSnsState->bSyncInit = CVI_TRUE;
		pstCfg0->snsCfg.need_update = CVI_TRUE;
		/* recalcualte WDR size */
		cmos_get_wdr_size(ViPipe, &pstCfg0->ispCfg);
		pstCfg0->ispCfg.need_update = CVI_TRUE;
	} else {
		pstCfg0->snsCfg.need_update = CVI_FALSE;
		for (i = 0; i < pstCfg0->snsCfg.u32RegNum; i++) {
			if (pstCfg0->snsCfg.astI2cData[i].u32Data == pstCfg1->snsCfg.astI2cData[i].u32Data) {
				pstCfg0->snsCfg.astI2cData[i].bUpdate = CVI_FALSE;
			} else {
				pstCfg0->snsCfg.astI2cData[i].bUpdate = CVI_TRUE;
				pstCfg0->snsCfg.need_update = CVI_TRUE;
			}
		}
		/* check update isp crop or not */
		pstCfg0->ispCfg.need_update = (sensor_cmp_wdr_size(&pstCfg0->ispCfg, &pstCfg1->ispCfg) ?
				CVI_TRUE : CVI_FALSE);

		/* check update cif wdr manual or not */
		pstCfg0->cifCfg.need_update = (sensor_cmp_cif_wdr(&pstCfg0->cifCfg, &pstCfg1->cifCfg) ?
				CVI_TRUE : CVI_FALSE);
	}

	pstSnsRegsInfo->bConfig = CVI_FALSE;
	memcpy(pstSnsSyncInfo, &pstSnsState->astSyncInfo[0], sizeof(ISP_SNS_SYNC_INFO_S));
	memcpy(&pstSnsState->astSyncInfo[1], &pstSnsState->astSyncInfo[0], sizeof(ISP_SNS_SYNC_INFO_S));
	pstSnsState->au32FL[1] = pstSnsState->au32FL[0];

	return CVI_SUCCESS;
}

static CVI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	CVI_U8 u8SensorImageMode = 0;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstSensorImageMode);
	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u8SensorImageMode = pstSnsState->u8ImgMode;
	pstSnsState->bSyncInit = CVI_FALSE;

	if (pstSensorImageMode->f32Fps <= 30) {
		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			if (SC200AI_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)) {
				u8SensorImageMode = SC200AI_MODE_1080P30;
			} else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				       pstSensorImageMode->u16Width,
				       pstSensorImageMode->u16Height,
				       pstSensorImageMode->f32Fps,
				       pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		} else if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			if (SC200AI_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)) {
				u8SensorImageMode = SC200AI_MODE_1080P30_WDR;
			} else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				       pstSensorImageMode->u16Width,
				       pstSensorImageMode->u16Height,
				       pstSensorImageMode->f32Fps,
				       pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
			       pstSensorImageMode->u16Width,
			       pstSensorImageMode->u16Height,
			       pstSensorImageMode->f32Fps,
			       pstSnsState->enWDRMode);
			return CVI_FAILURE;
		}
	} else {
	}

	if ((pstSnsState->bInit == CVI_TRUE) && (u8SensorImageMode == pstSnsState->u8ImgMode)) {
		/* Don't need to switch SensorImageMode */
		return CVI_FAILURE;
	}

	pstSnsState->u8ImgMode = u8SensorImageMode;

	return CVI_SUCCESS;
}

static CVI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	const SC200AI_MODE_S *pstMode = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsState->bInit = CVI_FALSE;
	pstSnsState->bSyncInit = CVI_FALSE;
	pstSnsState->u8ImgMode = SC200AI_MODE_1080P30;
	pstSnsState->enWDRMode = WDR_MODE_NONE;
	pstMode = &g_astSC200AI_mode[pstSnsState->u8ImgMode];
	pstSnsState->u32FLStd  = pstMode->u32VtsDef;
	pstSnsState->au32FL[0] = pstMode->u32VtsDef;
	pstSnsState->au32FL[1] = pstMode->u32VtsDef;

	memset(&pstSnsState->astSyncInfo[0], 0, sizeof(ISP_SNS_SYNC_INFO_S));
	memset(&pstSnsState->astSyncInfo[1], 0, sizeof(ISP_SNS_SYNC_INFO_S));
}

static CVI_S32 sensor_rx_attr(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *pstRxAttr)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstRxAttr);

	memcpy(pstRxAttr, &sc200ai_rx_attr, sizeof(*pstRxAttr));

	pstRxAttr->img_size.width = g_astSC200AI_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Width;
	pstRxAttr->img_size.height = g_astSC200AI_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height;
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstRxAttr->mipi_attr.wdr_mode = CVI_MIPI_WDR_MODE_NONE;
	}

	return CVI_SUCCESS;

}

static CVI_S32 sensor_patch_rx_attr(RX_INIT_ATTR_S *pstRxInitAttr)
{
	SNS_COMBO_DEV_ATTR_S *pstRxAttr = &sc200ai_rx_attr;
	int i;

	CMOS_CHECK_POINTER(pstRxInitAttr);

	if (pstRxInitAttr->stMclkAttr.bMclkEn)
		pstRxAttr->mclk.cam = pstRxInitAttr->stMclkAttr.u8Mclk;

	if (pstRxInitAttr->MipiDev >= VI_MAX_DEV_NUM)
		return CVI_SUCCESS;

	pstRxAttr->devno = pstRxInitAttr->MipiDev;

	if (pstRxAttr->input_mode == INPUT_MODE_MIPI) {
		struct mipi_dev_attr_s *attr = &pstRxAttr->mipi_attr;

		for (i = 0; i < MIPI_LANE_NUM + 1; i++) {
			attr->lane_id[i] = pstRxInitAttr->as16LaneId[i];
			attr->pn_swap[i] = pstRxInitAttr->as8PNSwap[i];
		}
	} else {
		struct lvds_dev_attr_s *attr = &pstRxAttr->lvds_attr;

		for (i = 0; i < MIPI_LANE_NUM + 1; i++) {
			attr->lane_id[i] = pstRxInitAttr->as16LaneId[i];
			attr->pn_swap[i] = pstRxInitAttr->as8PNSwap[i];
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	CMOS_CHECK_POINTER(pstSensorExpFunc);

	memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

	pstSensorExpFunc->pfn_cmos_sensor_init = sc200ai_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit = sc200ai_exit;
	pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
	pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
	pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;

	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_blc_default;
	pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

	return CVI_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static CVI_S32 sc200ai_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunSC200AI_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

	if (pastSnsStateCtx == CVI_NULL) {
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
		if (pastSnsStateCtx == CVI_NULL) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
			return -ENOMEM;
		}
	}

	memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

	SC200AI_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

	return CVI_SUCCESS;
}

static CVI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	SC200AI_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
	SENSOR_FREE(pastSnsStateCtx);
	SC200AI_SENSOR_RESET_CTX(ViPipe);
}

static CVI_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	CVI_S32 s32Ret;
	ISP_SENSOR_REGISTER_S stIspRegister;
	AE_SENSOR_REGISTER_S  stAeRegister;
	AWB_SENSOR_REGISTER_S stAwbRegister;
	ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

	CMOS_CHECK_POINTER(pstAeLib);
	CMOS_CHECK_POINTER(pstAwbLib);

	s32Ret = sensor_ctx_init(ViPipe);

	if (s32Ret != CVI_SUCCESS)
		return CVI_FAILURE;

	stSnsAttrInfo.eSensorId = SC200AI_ID;

	s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
	s32Ret |= CVI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor register callback function failed!\n");
		return s32Ret;
	}

	s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stAeExp);
	s32Ret |= CVI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor register callback function to ae lib failed!\n");
		return s32Ret;
	}

	s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stAwbExp);
	s32Ret |= CVI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor register callback function to awb lib failed!\n");
		return s32Ret;
	}

	return CVI_SUCCESS;
}

static CVI_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	CVI_S32 s32Ret;

	CMOS_CHECK_POINTER(pstAeLib);
	CMOS_CHECK_POINTER(pstAwbLib);

	s32Ret = CVI_ISP_SensorUnRegCallBack(ViPipe, SC200AI_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, SC200AI_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, SC200AI_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
		return s32Ret;
	}

	sensor_ctx_exit(ViPipe);

	return CVI_SUCCESS;
}

static CVI_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{
	CMOS_CHECK_POINTER(pstInitAttr);

	g_au32InitExposure[ViPipe] = pstInitAttr->u32Exposure;
	g_au32LinesPer500ms[ViPipe] = pstInitAttr->u32LinesPer500ms;
	g_au16InitWBGain[ViPipe][0] = pstInitAttr->u16WBRgain;
	g_au16InitWBGain[ViPipe][1] = pstInitAttr->u16WBGgain;
	g_au16InitWBGain[ViPipe][2] = pstInitAttr->u16WBBgain;
	g_au16SampleRgain[ViPipe] = pstInitAttr->u16SampleRgain;
	g_au16SampleBgain[ViPipe] = pstInitAttr->u16SampleBgain;
	g_au16SC200AI_GainMode[ViPipe] = pstInitAttr->enGainMode;
	g_au16SC200AI_L2SMode[ViPipe] = pstInitAttr->enL2SMode;

	return CVI_SUCCESS;
}

ISP_SNS_OBJ_S stSnsSC200AI_Obj = {
	.pfnRegisterCallback    = sensor_register_callback,
	.pfnUnRegisterCallback  = sensor_unregister_callback,
	.pfnStandby             = sc200ai_standby,
	.pfnRestart             = sc200ai_restart,
	.pfnMirrorFlip          = sc200ai_mirror_flip,
	.pfnWriteReg            = sc200ai_write_register,
	.pfnReadReg             = sc200ai_read_register,
	.pfnSetBusInfo          = sc200ai_set_bus_info,
	.pfnSetInit             = sensor_set_init,
	.pfnPatchRxAttr		= sensor_patch_rx_attr,
	.pfnPatchI2cAddr	= CVI_NULL,
	.pfnGetRxAttr		= sensor_rx_attr,
	.pfnExpSensorCb		= cmos_init_sensor_exp_function,
	.pfnExpAeCb		= cmos_init_ae_exp_function,
	.pfnSnsProbe		= sc200ai_probe,
};

