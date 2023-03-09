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

#include "bg0808_cmos_ex.h"
#include "bg0808_cmos_param.h"

#define DIV_0_TO_1(a)   ((0 == (a)) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))
#define BG0808_ID 0x0808
#define BG0808_I2C_ADDR 0x32
#define BG0808_I2C_ADDR_IS_VALID(addr)	((addr) == BG0808_I2C_ADDR)

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastBG0808[VI_MAX_PIPE_NUM] = {CVI_NULL};

#define BG0808_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastBG0808[dev])
#define BG0808_SENSOR_SET_CTX(dev, pstCtx)   (g_pastBG0808[dev] = pstCtx)
#define BG0808_SENSOR_RESET_CTX(dev)         (g_pastBG0808[dev] = CVI_NULL)

ISP_SNS_COMMBUS_U g_aunBG0808_BusInfo[VI_MAX_PIPE_NUM] = {
	[0] = { .s8I2cDev = 2},
	[1 ... VI_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

CVI_U16 g_au16BG0808_GainMode[VI_MAX_PIPE_NUM] = {0};
CVI_U16 g_au16BG0808_L2SMode[VI_MAX_PIPE_NUM] = {0};

ISP_SNS_MIRRORFLIP_TYPE_E g_aeBg0808_MirrorFip[VI_MAX_PIPE_NUM] = {0};

/****************************************************************************
 * local variables and functions                                            *
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
static CVI_U32 g_u32SexpOld;

static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg);
/*****BG0808 Lines Range*****/
#define BG0808_FULL_LINES_MAX  (0xFFFF)

/*****BG0808 Register Address*****/
#define BG0808_EXP_H_ADDR                     (0x000c)
#define BG0808_EXP_L_ADDR                     (0x000d)
#define BG0808_SEXP_H_ADDR                    (0x0022)
#define BG0808_SEXP_L_ADDR                    (0x0023)

#define BG0808_AGAIN_ADDR                     (0x00a2)
#define BG0808_SAGAIN_ADDR                    (0x00a4)
#define BG0808_CLAMP_MODE_ADDR                (0x0073)

#define BG0808_DGAIN_H_ADDR                   (0x00c0)
#define BG0808_DGAIN_L_ADDR                   (0x00c1)
#define BG0808_SDGAIN_H_ADDR                  (0x00c2)
#define BG0808_SDGAIN_L_ADDR                  (0x00c3)

#define BG0808_VMAX_H_ADDR                    (0x0010)
#define BG0808_VMAX_L_ADDR                    (0x0011)
#define BG0808_SHADOW_ADDR                    (0x001d)

#define AGAIN_MAX_IDX                         (64)
#define DGAIN_MAX_IDX                         (83)

#define BG0808_RES_IS_1080P(w, h)      ((w) == 1920 && (h) == 1080)

#define BG0808_EXPACCURACY                    (1)
#define BG0808_SEXP_ADDSPEED_MAX              (264)

static CVI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	const BG0808_MODE_S *pstMode;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstMode = &g_astBG0808_mode[pstSnsState->u8ImgMode];
#if 0
	memset(&pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S));
#endif
	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32FlickerFreq = 50 * 256;
	pstAeSnsDft->u32FullLinesMax = BG0808_FULL_LINES_MAX;
	pstAeSnsDft->u32HmaxTimes = (1000000) / (pstSnsState->u32FLStd * 30);

	pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stIntTimeAccu.f32Accuracy = BG0808_EXPACCURACY;
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
		pstAeSnsDft->f32Fps = pstMode->f32MaxFps;
		pstAeSnsDft->f32MinFps = pstMode->f32MinFps;
		pstAeSnsDft->au8HistThresh[0] = 0xd;
		pstAeSnsDft->au8HistThresh[1] = 0x28;
		pstAeSnsDft->au8HistThresh[2] = 0x60;
		pstAeSnsDft->au8HistThresh[3] = 0x80;

		pstAeSnsDft->u32MaxAgain = pstMode->stAgain[0].u32Max;
		pstAeSnsDft->u32MinAgain = pstMode->stAgain[0].u32Min;
		pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

		pstAeSnsDft->u32MaxDgain = pstMode->stDgain[0].u32Max;
		pstAeSnsDft->u32MinDgain = pstMode->stDgain[0].u32Min;
		pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

		pstAeSnsDft->u8AeCompensation = 40;
		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 5;
		pstAeSnsDft->u32AEResponseFrame = 4;
		pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 76151;

		pstAeSnsDft->u32MaxIntTime = pstMode->stExp[0].u16Max;
		pstAeSnsDft->u32MinIntTime = pstMode->stExp[0].u16Min;
		pstAeSnsDft->u32MaxIntTimeTarget = pstAeSnsDft->u32MaxIntTime;
		pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;
		break;
	case WDR_MODE_2To1_LINE:
		pstAeSnsDft->f32Fps = pstMode->f32MaxFps;
		pstAeSnsDft->f32MinFps = pstMode->f32MinFps;
		pstAeSnsDft->au8HistThresh[0] = 0xC;
		pstAeSnsDft->au8HistThresh[1] = 0x18;
		pstAeSnsDft->au8HistThresh[2] = 0x60;
		pstAeSnsDft->au8HistThresh[3] = 0x80;

		pstAeSnsDft->u32MaxIntTime = pstMode->stExp[0].u16Max;
		pstAeSnsDft->u32MinIntTime = pstMode->stExp[0].u16Min;

		pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

		pstAeSnsDft->u32MaxAgain = pstMode->stAgain[0].u32Max;
		pstAeSnsDft->u32MinAgain = pstMode->stAgain[0].u32Min;
		pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

		pstAeSnsDft->u32MaxDgain = pstMode->stDgain[0].u32Max;
		pstAeSnsDft->u32MinDgain = pstMode->stDgain[0].u32Min;
		pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
		pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift;

		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 52000;
		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 5;
		pstAeSnsDft->u32AEResponseFrame = 3;
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
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32Vts = g_astBG0808_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;
	f32MaxFps = g_astBG0808_mode[pstSnsState->u8ImgMode].f32MaxFps;
	f32MinFps = g_astBG0808_mode[pstSnsState->u8ImgMode].f32MinFps;

	switch (pstSnsState->u8ImgMode) {
	case BG0808_MODE_1920X1080P30:
	case BG0808_MODE_1920X1080P30_WDR:
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Not support Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}
		u32VMAX = (u32VMAX > BG0808_FULL_LINES_MAX) ? BG0808_FULL_LINES_MAX : u32VMAX;
		break;
	default:
		CVI_TRACE_SNS(CVI_DBG_ERR, "Not support sensor mode: %d\n", pstSnsState->u8ImgMode);
		return CVI_FAILURE;
	}

	pstSnsState->u32FLStd = u32VMAX;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstSnsRegsInfo->astI2cData[LINEAR_VMAX_H_ADDR].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_VMAX_L_ADDR].u32Data = (u32VMAX & 0xFF);
	} else {
		pstSnsRegsInfo->astI2cData[WDR_VMAX_H_ADDR].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR_VMAX_L_ADDR].u32Data = (u32VMAX & 0xFF);
	}

	pstAeSnsDft->f32Fps = f32Fps;
	pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 1;
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
	CVI_U32 u32TmpIntTime, u32MinTime, u32MaxTime;
	CVI_U32 SexpAddSpeed = 0;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(u32IntTime);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		/* linear exposure reg range:
		 * min : 1
		 * max : vts - 1
		 * step : 1
		 */
		u32MinTime = 1;
		u32MaxTime = pstSnsState->au32FL[0] - 1;
		u32TmpIntTime = (u32IntTime[0] > u32MaxTime) ? u32MaxTime : u32IntTime[0];
		u32TmpIntTime = (u32TmpIntTime < u32MinTime) ? u32MinTime : u32TmpIntTime;

		pstSnsRegsInfo->astI2cData[LINEAR_EXP_H_ADDR].u32Data = ((u32TmpIntTime & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_EXP_L_ADDR].u32Data = (u32TmpIntTime & 0x00FF);
	} else {
		/*
		 * Long exp + Short exp < VTS
		 * sexp[N+1] - sexp[N] <= 264
		 */
		/* sexp */
		pstSnsState->au32WDRIntTime[0] = u32IntTime[0];
		/* lexp */
		pstSnsState->au32WDRIntTime[1] = u32IntTime[1];
		SexpAddSpeed = pstSnsState->au32WDRIntTime[0] > g_u32SexpOld ?
			(pstSnsState->au32WDRIntTime[0] - g_u32SexpOld) : 0;
		pstSnsState->au32WDRIntTime[0] = SexpAddSpeed >= BG0808_SEXP_ADDSPEED_MAX ?
			BG0808_SEXP_ADDSPEED_MAX : pstSnsState->au32WDRIntTime[0];
		pstSnsRegsInfo->astI2cData[WDR_EXP_H_ADDR].u32Data = ((pstSnsState->au32WDRIntTime[1] & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR_EXP_L_ADDR].u32Data = (pstSnsState->au32WDRIntTime[1] & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR_SEXP_H_ADDR].u32Data = ((pstSnsState->au32WDRIntTime[0] & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR_SEXP_L_ADDR].u32Data = (pstSnsState->au32WDRIntTime[0] & 0xFF);

		g_u32SexpOld = pstSnsState->au32WDRIntTime[0];
		u32IntTime[0] = pstSnsState->au32WDRIntTime[0];
		u32IntTime[1] = pstSnsState->au32WDRIntTime[1];

		/* update isp */
		cmos_get_wdr_size(ViPipe, &pstSnsState->astSyncInfo[0].ispCfg);
	}

	return CVI_SUCCESS;
}

typedef struct again_tbl_info_s {
	CVI_U32	gainMax;
	CVI_U16	idxBase;
	CVI_U8	regGain;
	CVI_U8	regGainFineBase;
	CVI_U8	regGainFineStep;
} again_tbl_info_s;

static CVI_U32 Again_table[AGAIN_MAX_IDX + 1] = {
	1024, 1036, 1050, 1063, 1077, 1092, 1107, 1122, 1137, 1153,
	1170, 1187, 1204, 1222, 1241, 1260, 1280, 1300, 1321, 1342,
	1365, 1388, 1412, 1437, 1462, 1489, 1517, 1545, 1575, 1606,
	1638, 1671, 1706, 1742, 1780, 1820, 1861, 1905, 1950, 1998,
	2048, 2100, 2155, 2214, 2275, 2340, 2409, 2482, 2560, 2642,
	2730, 2824, 2925, 3034, 3150, 3276, 3413, 3561, 3723, 3900,
	4096, 4311, 4551, 4818, 5120
};

static struct again_tbl_info_s AgainInfo = {
		.gainMax = 5120,
		.idxBase = 0,
		.regGainFineBase = 0x7F,
		.regGainFineStep = 1,
};

typedef struct dgain_tbl_info_s {
	CVI_U8	regH[DGAIN_MAX_IDX + 1];
	CVI_U8	regL[DGAIN_MAX_IDX + 1];
} dgain_tbl_info_s;

static CVI_U32 Dgain_table[DGAIN_MAX_IDX + 1] = {
	1024, 1084, 1144, 1204, 1264, 1324, 1384, 1444, 1504, 1564,
	1624, 1684, 1744, 1804, 1864, 2004, 2144, 2284, 2424, 2564,
	2704, 2844, 2984, 3124, 3264, 3404, 3544, 3684, 3824, 3964,
	4104, 4244, 4384, 4524, 4664, 4804, 5044, 5284, 5524, 5764,
	6004, 6244, 6484, 6724, 6964, 7204, 7444, 7684, 7924, 8164,
	8404, 8644, 8884, 9124, 9364, 9604, 9844, 10084, 10324, 10564,
	10804, 11044, 11284, 11524, 11764, 12004, 12244, 12484, 12724, 12964,
	13204, 13444, 13684, 13924, 14164, 14404, 14644, 14884, 15124, 15364,
	15604, 15844, 16084, 16324
};

static struct dgain_tbl_info_s DgainInfo = {
	.regH = {
		0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3,
		0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x5,
		0x5, 0x5, 0x5, 0x6, 0x6, 0x6, 0x6, 0x7, 0x7, 0x7,
		0x8, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0xA, 0xA, 0xB,
		0xB, 0xC, 0xC, 0xD, 0xD, 0xE, 0xE, 0xF, 0xF, 0xF,
		0x10, 0x10, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14,
		0x15, 0x15, 0x16, 0x16, 0x16, 0x17, 0x17, 0x18, 0x18, 0x19,
		0x19, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E,
		0x1E, 0x1E, 0x1F, 0x1F
	},
	.regL = {
		0x0,  0x1E, 0x3C, 0x5A, 0x78, 0x96, 0xB4, 0xD2, 0xF0, 0xE,
		0x2C, 0x4A, 0x68, 0x86, 0xA4, 0xEA, 0x30, 0x76, 0xBC, 0x2,
		0x48, 0x8E, 0xD4, 0x1A, 0x60, 0xA6, 0xEC, 0x32, 0x78, 0xBE,
		0x4,  0x4A, 0x90, 0xD6, 0x1C, 0x62, 0xDA, 0x52, 0xCA, 0x42,
		0xBA, 0x32, 0xAA, 0x22, 0x9A, 0x12, 0x8A, 0x2,  0x7A, 0xF2,
		0x6A, 0xE2, 0x5A, 0xD2, 0x4A, 0xC2, 0x3A, 0xB2, 0x2A, 0xA2,
		0x1A, 0x92, 0xA,  0x82, 0xFA, 0x72, 0xEA, 0x62, 0xDA, 0x52,
		0xCA, 0x42, 0xBA, 0x32, 0xAA, 0x22, 0x9A, 0x12, 0x8A, 0x2,
		0x7A, 0xF2, 0x6A, 0xE2
	},
};

static CVI_S32 cmos_again_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32AgainLin, CVI_U32 *pu32AgainDb)
{
	int i;
	(void) ViPipe;

	CMOS_CHECK_POINTER(pu32AgainLin);
	CMOS_CHECK_POINTER(pu32AgainDb);

	if (*pu32AgainLin >= Again_table[AGAIN_MAX_IDX]) {
		*pu32AgainLin = Again_table[AGAIN_MAX_IDX];
		*pu32AgainDb = AGAIN_MAX_IDX;
		return CVI_SUCCESS;
	}

	for (i = 1; i < AGAIN_MAX_IDX + 1; i++) {
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

	(void) ViPipe;

	CMOS_CHECK_POINTER(pu32DgainLin);
	CMOS_CHECK_POINTER(pu32DgainDb);

	if (*pu32DgainLin >= Dgain_table[DGAIN_MAX_IDX]) {
		*pu32DgainLin = Dgain_table[DGAIN_MAX_IDX];
		*pu32DgainDb = DGAIN_MAX_IDX;
		return CVI_SUCCESS;
	}

	for (i = 1; i < DGAIN_MAX_IDX + 1; i++) {
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
	CVI_U32 u32Again, regAgain;
	CVI_U32 u32Dgain;
	struct again_tbl_info_s *again_info;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pu32Again);
	CMOS_CHECK_POINTER(pu32Dgain);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	u32Again = pu32Again[0];
	u32Dgain = pu32Dgain[0];
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		again_info = &AgainInfo;
		regAgain = again_info->regGainFineBase - u32Again;

		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN_ADDR].u32Data = (regAgain & 0xFF);
		if (regAgain <= 0x4f)
			pstSnsRegsInfo->astI2cData[LINEAR_CLAMP_MODE].u32Data = 0x02;
		else
			pstSnsRegsInfo->astI2cData[LINEAR_CLAMP_MODE].u32Data = 0x00;

		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_H_ADDR].u32Data = (DgainInfo.regH[u32Dgain] & 0xFF);
		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_L_ADDR].u32Data = (DgainInfo.regL[u32Dgain] & 0xFF);
	} else {
		if (g_au16BG0808_GainMode[ViPipe] == SNS_GAIN_MODE_WDR_2F) {
			//SEF Again
			again_info = &AgainInfo;
			regAgain = again_info->regGainFineBase - u32Again;
			pstSnsRegsInfo->astI2cData[WDR_SAGAIN_ADDR].u32Data = (regAgain & 0xFF);
			if (regAgain <= 0x4f)
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x02;
			else
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x00;
			//SEF Dgain
			pstSnsRegsInfo->astI2cData[WDR_SDGAIN_H_ADDR].u32Data = (DgainInfo.regH[u32Dgain] & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR_SDGAIN_L_ADDR].u32Data = (DgainInfo.regL[u32Dgain] & 0xFF);

			//LEF Again
			u32Again = pu32Again[1];
			again_info = &AgainInfo;
			regAgain = again_info->regGainFineBase - u32Again;
			pstSnsRegsInfo->astI2cData[WDR_AGAIN_ADDR].u32Data = (regAgain & 0xFF);
			if (regAgain <= 0x4f)
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x02;
			else
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x00;
			//LEF Dgain
			u32Dgain =  pu32Dgain[1];
			pstSnsRegsInfo->astI2cData[WDR_DGAIN_H_ADDR].u32Data = (DgainInfo.regH[u32Dgain] & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR_DGAIN_L_ADDR].u32Data = (DgainInfo.regL[u32Dgain] & 0xFF);
		} else if (g_au16BG0808_GainMode[ViPipe] == SNS_GAIN_MODE_SHARE) {
				//SEF Again
			again_info = &AgainInfo;
			regAgain = again_info->regGainFineBase - u32Again;
			pstSnsRegsInfo->astI2cData[WDR_SAGAIN_ADDR].u32Data = (regAgain & 0xFF);
			pstSnsRegsInfo->astI2cData[WDR_AGAIN_ADDR].u32Data = (regAgain & 0xFF);
			if (regAgain <= 0x4f)
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x02;
			else
				pstSnsRegsInfo->astI2cData[WDR_CLAMP_MODE].u32Data = 0x00;
		pstSnsRegsInfo->astI2cData[WDR_SDGAIN_H_ADDR].u32Data = (DgainInfo.regH[u32Dgain] & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR_SDGAIN_L_ADDR].u32Data = (DgainInfo.regL[u32Dgain] & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR_DGAIN_H_ADDR].u32Data = (DgainInfo.regH[u32Dgain] & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR_DGAIN_L_ADDR].u32Data = (DgainInfo.regL[u32Dgain] & 0xFF);
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_inttime_max(VI_PIPE ViPipe, CVI_U16 u16ManRatioEnable, CVI_U32 *au32Ratio,
		CVI_U32 *au32IntTimeMax, CVI_U32 *au32IntTimeMin, CVI_U32 *pu32LFMaxIntTime)
{
	CVI_U32 u32IntTimeMaxTmp  = 0, u32IntTimeMaxTmp0 = 0;
	CVI_U32 u32RatioTmp = 0x40;
	CVI_U32 u32ShortTimeMinLimit = 0;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(au32Ratio);
	CMOS_CHECK_POINTER(au32IntTimeMax);
	CMOS_CHECK_POINTER(au32IntTimeMin);
	CMOS_CHECK_POINTER(pu32LFMaxIntTime);
	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32ShortTimeMinLimit = 1;
	/*
	 * Long exp + Short exp < VTS
	 */
	u32IntTimeMaxTmp0 = ((pstSnsState->au32FL[1] - 1 - g_astBG0808_mode[pstSnsState->u8ImgMode].u32IspResTime -
			pstSnsState->au32WDRIntTime[0]) * 0x40) / DIV_0_TO_1(au32Ratio[0]);
	u32IntTimeMaxTmp  = ((pstSnsState->au32FL[0] - 1 - g_astBG0808_mode[pstSnsState->u8ImgMode].u32IspResTime)
			* 0x40) / DIV_0_TO_1(au32Ratio[0] + 0x40);
	u32IntTimeMaxTmp = (u32IntTimeMaxTmp > u32IntTimeMaxTmp0) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
	u32IntTimeMaxTmp  = (!u32IntTimeMaxTmp) ? u32ShortTimeMinLimit : u32IntTimeMaxTmp;

	if (u32IntTimeMaxTmp >= u32ShortTimeMinLimit) {
		if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			au32IntTimeMax[0] = u32IntTimeMaxTmp;
			au32IntTimeMax[1] = au32IntTimeMax[0] * au32Ratio[0] >> 6;
			au32IntTimeMax[2] = au32IntTimeMax[1] * au32Ratio[1] >> 6;
			au32IntTimeMax[3] = au32IntTimeMax[2] * au32Ratio[2] >> 6;
			au32IntTimeMin[0] = u32ShortTimeMinLimit;
			au32IntTimeMin[1] = au32IntTimeMin[0] * au32Ratio[0] >> 6;
			au32IntTimeMin[2] = au32IntTimeMin[1] * au32Ratio[1] >> 6;
			au32IntTimeMin[3] = au32IntTimeMin[2] * au32Ratio[2] >> 6;
		} else {
		}
	} else {
		if (u16ManRatioEnable) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Manaul ExpRatio is too large!\n");
			return CVI_FAILURE;
		}
		u32IntTimeMaxTmp = u32ShortTimeMinLimit;

		if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			u32RatioTmp = 0xFFF;
			au32IntTimeMax[0] = u32IntTimeMaxTmp;
			au32IntTimeMax[1] = au32IntTimeMax[0] * u32RatioTmp >> 6;
		} else {
		}
		au32IntTimeMin[0] = au32IntTimeMax[0];
		au32IntTimeMin[1] = au32IntTimeMax[1];
		au32IntTimeMin[2] = au32IntTimeMax[2];
		au32IntTimeMin[3] = au32IntTimeMax[3];
	}
	CVI_TRACE_SNS(CVI_DBG_DEBUG, "sexp[%d, %d], lexp[%d, %d], ratio:%d\n",
			au32IntTimeMin[0], au32IntTimeMax[0], au32IntTimeMin[1], au32IntTimeMax[1], au32Ratio[0]);

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

	//memcpy(pstDef->stNoiseCalibration.CalibrationCoef,
		//&g_stIspNoiseCalibratio, sizeof(ISP_CMOS_NOISE_CALIBRATION_S));
	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_blc_default(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlc)
{
	(void) ViPipe;
	CMOS_CHECK_POINTER(pstBlc);

	memset(pstBlc, 0, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	memcpy(pstBlc, &g_stIspBlcCalibratio, sizeof(ISP_CMOS_BLACK_LEVEL_S));

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg)
{
	const BG0808_MODE_S *pstMode = CVI_NULL;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstMode = &g_astBG0808_mode[pstSnsState->u8ImgMode];

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

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstSnsState->bSyncInit = CVI_FALSE;

	switch (u8Mode) {
	case WDR_MODE_NONE:
		pstSnsState->u8ImgMode = BG0808_MODE_1920X1080P30;
		pstSnsState->enWDRMode = WDR_MODE_NONE;
		pstSnsState->u32FLStd = g_astBG0808_mode[pstSnsState->u8ImgMode].u32VtsDef;
		CVI_TRACE_SNS(CVI_DBG_DEBUG, "linear mode\n");
		break;
	case WDR_MODE_2To1_LINE:
		if (pstSnsState->u8ImgMode == BG0808_MODE_1920X1080P30)
			pstSnsState->u8ImgMode = BG0808_MODE_1920X1080P30_WDR;
		pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
		pstSnsState->u32FLStd = g_astBG0808_mode[pstSnsState->u8ImgMode].u32VtsDef;
		CVI_TRACE_SNS(CVI_DBG_DEBUG, "2to1 line WDR 1080p mode(60fps->30fps)\n");
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
	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstSnsRegsInfo = &pstSnsSyncInfo->snsCfg;
	pstCfg0 = &pstSnsState->astSyncInfo[0];
	pstCfg1 = &pstSnsState->astSyncInfo[1];
	pstI2c_data = pstCfg0->snsCfg.astI2cData;

	if ((pstSnsState->bSyncInit == CVI_FALSE) || (pstSnsRegsInfo->bConfig == CVI_FALSE)) {
		pstCfg0->snsCfg.enSnsType = SNS_I2C_TYPE;
		pstCfg0->snsCfg.unComBus.s8I2cDev = g_aunBG0808_BusInfo[ViPipe].s8I2cDev;
		pstCfg0->snsCfg.u8Cfg2ValidDelayMax = 0;
		pstCfg0->snsCfg.use_snsr_sram = CVI_TRUE;
		pstCfg0->snsCfg.u32RegNum = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ?
			WDR_REGS_NUM : LINEAR_REGS_NUM;

		for (i = 0; i < pstCfg0->snsCfg.u32RegNum; i++) {
			pstI2c_data[i].bUpdate = CVI_TRUE;
			pstI2c_data[i].u8DevAddr = bg0808_i2c_addr;
			pstI2c_data[i].u32AddrByteNum = bg0808_addr_byte;
			pstI2c_data[i].u32DataByteNum = bg0808_data_byte;
		}

		switch (pstSnsState->enWDRMode) {
		default:
		case WDR_MODE_NONE:
			//Linear Mode Regs
			pstI2c_data[LINEAR_EXP_H_ADDR].u32RegAddr = BG0808_EXP_H_ADDR;
			pstI2c_data[LINEAR_EXP_L_ADDR].u32RegAddr = BG0808_EXP_L_ADDR;
			pstI2c_data[LINEAR_AGAIN_ADDR].u32RegAddr = BG0808_AGAIN_ADDR;
			pstI2c_data[LINEAR_CLAMP_MODE].u32RegAddr = BG0808_CLAMP_MODE_ADDR;
			pstI2c_data[LINEAR_CLAMP_MODE].u32Data = 0x00;
			pstI2c_data[LINEAR_DGAIN_H_ADDR].u32RegAddr = BG0808_DGAIN_H_ADDR;
			pstI2c_data[LINEAR_DGAIN_L_ADDR].u32RegAddr = BG0808_DGAIN_L_ADDR;
			pstI2c_data[LINEAR_VMAX_H_ADDR].u32RegAddr = BG0808_VMAX_H_ADDR;
			pstI2c_data[LINEAR_VMAX_L_ADDR].u32RegAddr = BG0808_VMAX_L_ADDR;
			pstI2c_data[LINEAR_SHADOW_ADDR].u32RegAddr = BG0808_SHADOW_ADDR;
			pstI2c_data[LINEAR_SHADOW_ADDR].u32Data = 0x02;
			break;
		case WDR_MODE_2To1_LINE:
			pstI2c_data[WDR_EXP_H_ADDR].u32RegAddr = BG0808_EXP_H_ADDR;
			pstI2c_data[WDR_EXP_L_ADDR].u32RegAddr = BG0808_EXP_L_ADDR;
			pstI2c_data[WDR_SEXP_H_ADDR].u32RegAddr = BG0808_SEXP_H_ADDR;
			pstI2c_data[WDR_SEXP_L_ADDR].u32RegAddr = BG0808_SEXP_L_ADDR;
			pstI2c_data[WDR_AGAIN_ADDR].u32RegAddr = BG0808_AGAIN_ADDR;
			pstI2c_data[WDR_SAGAIN_ADDR].u32RegAddr = BG0808_SAGAIN_ADDR;
			pstI2c_data[WDR_CLAMP_MODE].u32RegAddr = BG0808_CLAMP_MODE_ADDR;
			pstI2c_data[WDR_CLAMP_MODE].u32Data = 0x00;
			pstI2c_data[WDR_DGAIN_H_ADDR].u32RegAddr = BG0808_DGAIN_H_ADDR;
			pstI2c_data[WDR_DGAIN_L_ADDR].u32RegAddr = BG0808_DGAIN_L_ADDR;
			pstI2c_data[WDR_SDGAIN_H_ADDR].u32RegAddr = BG0808_SDGAIN_H_ADDR;
			pstI2c_data[WDR_SDGAIN_L_ADDR].u32RegAddr = BG0808_SDGAIN_L_ADDR;
			pstI2c_data[WDR_VMAX_H_ADDR].u32RegAddr = BG0808_VMAX_H_ADDR;
			pstI2c_data[WDR_VMAX_L_ADDR].u32RegAddr = BG0808_VMAX_L_ADDR;
			pstI2c_data[WDR_SHADOW_ADDR].u32RegAddr = BG0808_SHADOW_ADDR;
			pstI2c_data[WDR_SHADOW_ADDR].u32Data = 0x02;
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
				pstCfg0->snsCfg.need_update = CVI_TRUE;
				pstCfg0->snsCfg.astI2cData[i].bUpdate = CVI_TRUE;
			}
		}
		pstCfg0->snsCfg.astI2cData[pstCfg0->snsCfg.u32RegNum - 1].u32Data = 0x02;
		pstCfg0->snsCfg.astI2cData[pstCfg0->snsCfg.u32RegNum - 1].bUpdate = CVI_TRUE;

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
	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u8SensorImageMode = pstSnsState->u8ImgMode;
	pstSnsState->bSyncInit = CVI_FALSE;

	if (pstSensorImageMode->f32Fps <= 30) {
		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			if (BG0808_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)) {
				u8SensorImageMode = BG0808_MODE_1920X1080P30;
			} else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				       pstSensorImageMode->u16Width,
				       pstSensorImageMode->u16Height,
				       pstSensorImageMode->f32Fps,
				       pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		} else if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			if (BG0808_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)) {
				u8SensorImageMode = BG0808_MODE_1920X1080P30_WDR;
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
	}

	if ((pstSnsState->bInit == CVI_TRUE) && (u8SensorImageMode == pstSnsState->u8ImgMode)) {
		/* Don't need to switch SensorImageMode */
		return CVI_FAILURE;
	}

	pstSnsState->u8ImgMode = u8SensorImageMode;

	return CVI_SUCCESS;
}

static CVI_VOID sensor_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);
	if (pstSnsState->bInit == CVI_TRUE && g_aeBg0808_MirrorFip[ViPipe] != eSnsMirrorFlip) {
		bg0808_mirror_flip(ViPipe, eSnsMirrorFlip);
		g_aeBg0808_MirrorFip[ViPipe] = eSnsMirrorFlip;
	}
}

static CVI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	const BG0808_MODE_S *pstMode = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsState->bInit = CVI_FALSE;
	pstSnsState->bSyncInit = CVI_FALSE;
	pstSnsState->u8ImgMode = BG0808_MODE_1920X1080P30;
	pstSnsState->enWDRMode = WDR_MODE_NONE;
	pstMode = &g_astBG0808_mode[pstSnsState->u8ImgMode];
	pstSnsState->u32FLStd  = pstMode->u32VtsDef;
	pstSnsState->au32FL[0] = pstMode->u32VtsDef;
	pstSnsState->au32FL[1] = pstMode->u32VtsDef;

	memset(&pstSnsState->astSyncInfo[0], 0, sizeof(ISP_SNS_SYNC_INFO_S));
	memset(&pstSnsState->astSyncInfo[1], 0, sizeof(ISP_SNS_SYNC_INFO_S));
}

static CVI_S32 sensor_rx_attr(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *pstRxAttr)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstRxAttr);

	memcpy(pstRxAttr, &bg0808_rx_attr, sizeof(*pstRxAttr));

	pstRxAttr->img_size.width = g_astBG0808_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Width;
	pstRxAttr->img_size.height = g_astBG0808_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height;
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstRxAttr->mipi_attr.wdr_mode = CVI_MIPI_WDR_MODE_NONE;
	} else if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
		pstRxAttr->mclk.freq = CAMPLL_FREQ_27M;
	}

	return CVI_SUCCESS;
}

static CVI_S32 sensor_patch_rx_attr(RX_INIT_ATTR_S *pstRxInitAttr)
{
	SNS_COMBO_DEV_ATTR_S *pstRxAttr = &bg0808_rx_attr;
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

	pstSensorExpFunc->pfn_cmos_sensor_init = bg0808_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit = bg0808_exit;
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
static CVI_VOID sensor_patch_i2c_addr(CVI_S32 s32I2cAddr)
{
	if (BG0808_I2C_ADDR_IS_VALID(s32I2cAddr))
		bg0808_i2c_addr = s32I2cAddr;
}

static CVI_S32 bg0808_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunBG0808_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

	if (pastSnsStateCtx == CVI_NULL) {
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
		if (pastSnsStateCtx == CVI_NULL) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
			return -ENOMEM;
		}
	}

	memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

	BG0808_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

	return CVI_SUCCESS;
}

static CVI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	BG0808_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
	SENSOR_FREE(pastSnsStateCtx);
	BG0808_SENSOR_RESET_CTX(ViPipe);
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

	stSnsAttrInfo.eSensorId = BG0808_ID;

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

	s32Ret = CVI_ISP_SensorUnRegCallBack(ViPipe, BG0808_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, BG0808_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, BG0808_ID);
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
	g_au16BG0808_GainMode[ViPipe] = pstInitAttr->enGainMode;
	g_au16BG0808_L2SMode[ViPipe] = pstInitAttr->enL2SMode;

	return CVI_SUCCESS;
}

ISP_SNS_OBJ_S stSnsBG0808_Obj = {
	.pfnRegisterCallback	= sensor_register_callback,
	.pfnUnRegisterCallback	= sensor_unregister_callback,
	.pfnStandby		= bg0808_standby,
	.pfnRestart		= bg0808_restart,
	.pfnMirrorFlip		= sensor_mirror_flip,
	.pfnWriteReg		= bg0808_write_register,
	.pfnReadReg		= bg0808_read_register,
	.pfnSetBusInfo		= bg0808_set_bus_info,
	.pfnSetInit		= sensor_set_init,
	.pfnPatchRxAttr		= sensor_patch_rx_attr,
	.pfnPatchI2cAddr	= sensor_patch_i2c_addr,
	.pfnGetRxAttr		= sensor_rx_attr,
	.pfnExpSensorCb		= cmos_init_sensor_exp_function,
	.pfnExpAeCb		= cmos_init_ae_exp_function,
	.pfnSnsProbe		= bg0808_probe,
};

