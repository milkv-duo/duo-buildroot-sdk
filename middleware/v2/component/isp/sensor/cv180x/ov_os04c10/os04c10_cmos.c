#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>
#include <math.h>
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

#include "os04c10_cmos_ex.h"
#include "os04c10_cmos_param.h"

#define EPS 1e-7
#define DIV_0_TO_1(a)   ((0 == (a)) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))
#define OS04C10_ID 0x530443
#define OS04C10_I2C_ADDR_1 0x10
#define OS04C10_I2C_ADDR_2 0x36
#define OS04C10_I2C_ADDR_IS_VALID(addr)      ((addr) == OS04C10_I2C_ADDR_1 || (addr) == OS04C10_I2C_ADDR_2)
/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastOs04c10[VI_MAX_PIPE_NUM] = {CVI_NULL};

#define OS04C10_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastOs04c10[dev])
#define OS04C10_SENSOR_SET_CTX(dev, pstCtx)   (g_pastOs04c10[dev] = pstCtx)
#define OS04C10_SENSOR_RESET_CTX(dev)         (g_pastOs04c10[dev] = CVI_NULL)

ISP_SNS_COMMBUS_U g_aunOs04c10_BusInfo[VI_MAX_PIPE_NUM] = {
	[0] = { .s8I2cDev = 0},
	[1 ... VI_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

CVI_U16 g_au16Os04c10_GainMode[VI_MAX_PIPE_NUM] = {0};
CVI_U16 g_au16Os04c10_UseHwSync[VI_MAX_PIPE_NUM] = {0};

OS04C10_STATE_S g_astOs04c10_State[VI_MAX_PIPE_NUM] = {{0} };
ISP_SNS_MIRRORFLIP_TYPE_E g_aeOs04c10_MirrorFip[VI_MAX_PIPE_NUM] = {0};

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
static CVI_FLOAT OTP_rate;
static CVI_BOOL HCG_EN;
/*****Os04c10 Lines Range*****/
#define OS04C10_FULL_LINES_MAX  (0xFFFF)
#define OS04C10_FULL_LINES_MAX_2TO1_WDR  (0xFFFF)

/*****Os04c10 Register Address*****/
#define OS04C10_HOLD_3208		0x3208
#define OS04C10_HOLD_320D		0x320D
#define OS04C10_EXP1_ADDR		0x3501
#define OS04C10_EXP2_ADDR		0x3511
#define OS04C10_AGAIN1_ADDR		0x3508
#define OS04C10_DGAIN1_ADDR		0x350A
#define OS04C10_AGAIN2_ADDR		0x350C
#define OS04C10_DGAIN2_ADDR		0x350E
#define OS04C10_L2S_ADDR		0x3798
#define OS04C10_VTS_ADDR		0x380E
#define OS04C10_LINEAR_HCG_ADDR		0x3798
#define OS04C10_GGAIN_ADDR		0x5142
#define OS04C10_TABLE_END		0xffff

#define OS04C10_RES_IS_1520P(w, h)      ((w) == 2688 && (h) == 1520)
#define OS04C10_RES_IS_1440P(w, h)      ((w) == 2560 && (h) == 1440)

static CVI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	const OS04C10_MODE_S *pstMode;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstMode = &g_astOs04c10_mode[pstSnsState->u8ImgMode];

	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32FlickerFreq = 50 * 256;
	pstAeSnsDft->u32FullLinesMax = OS04C10_FULL_LINES_MAX;
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
	/* OV sensor cannot update new setting before the old setting takes effect */
	pstAeSnsDft->u8AERunInterval = 4;

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
		pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		pstAeSnsDft->u32MinIntTimeTarget = 1;
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
		pstAeSnsDft->u32MinIntTimeTarget = 1;

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
		pstAeSnsDft->u32AEResponseFrame = 4;
		if (genFSWDRMode[ViPipe] == ISP_FSWDR_LONG_FRAME_MODE) {
			pstAeSnsDft->u8AeCompensation = 64;
			pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		} else {
			pstAeSnsDft->u8AeCompensation = 40;
			pstAeSnsDft->enAeExpMode = AE_EXP_LOWLIGHT_PRIOR;
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
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32Vts = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;
	f32MaxFps = g_astOs04c10_mode[pstSnsState->u8ImgMode].f32MaxFps;
	f32MinFps = g_astOs04c10_mode[pstSnsState->u8ImgMode].f32MinFps;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Unsupport Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}
		u32VMAX = (u32VMAX > OS04C10_FULL_LINES_MAX) ? OS04C10_FULL_LINES_MAX : u32VMAX;

		pstSnsRegsInfo->astI2cData[LINEAR_VTS_0].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_VTS_1].u32Data = (u32VMAX & 0xFF);
	} else {
		CVI_U32 isp_res = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32IspResTime;
		CVI_U32 l2s_offset = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32L2S_offset;
		CVI_U32 margin = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32HdrMargin;
		CVI_U32 vblank = 0;

		/* In auto mode,
		 * L2S_distane = Sexp_max + L2S offset
		 * Vblank = Vtotal - Vactive - margin
		 * if Vblank > isp_res
		 * Sexp_max + L2S offset <= Vblank - isp_res
		 * else if Vblank < isp_res
		 * Sexp_max + L2S offset = Vblank
		 */
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Unsupport Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}
		u32VMAX = (u32VMAX > OS04C10_FULL_LINES_MAX_2TO1_WDR) ?
				OS04C10_FULL_LINES_MAX_2TO1_WDR : u32VMAX;

		vblank = u32VMAX - g_astOs04c10_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height
			- margin;
		if (vblank > isp_res)
			vblank -= isp_res;
		else
			CVI_TRACE_SNS(CVI_DBG_WARN, "cannot reserve 1ms for isp delay %d\n", vblank);

		g_astOs04c10_State[ViPipe].u32Sexp_MAX = vblank - l2s_offset;
		pstSnsRegsInfo->astI2cData[WDR2_VTS_0].u32Data = ((u32VMAX & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_VTS_1].u32Data = (u32VMAX & 0xFF);
	}

	pstSnsState->u32FLStd = u32VMAX;

	pstAeSnsDft->f32Fps = f32Fps;
	pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 8;
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

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(u32IntTime);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) {
		/* short exposure */
		pstSnsState->au32WDRIntTime[0] = u32IntTime[0];
		/* long exposure */
		pstSnsState->au32WDRIntTime[1] = u32IntTime[1];

		pstSnsRegsInfo->astI2cData[WDR2_EXP1_0].u32Data = ((pstSnsState->au32WDRIntTime[1] & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_EXP1_1].u32Data = (pstSnsState->au32WDRIntTime[1] & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_EXP2_0].u32Data = ((pstSnsState->au32WDRIntTime[0] & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_EXP2_1].u32Data = (pstSnsState->au32WDRIntTime[0] & 0xFF);
		/* update isp */
		cmos_get_wdr_size(ViPipe, &pstSnsState->astSyncInfo[0].ispCfg);

		//When the expLine is very small, the exposure time of R G B has a great error
		//use sensor internal Ggain to fix the gap
		//sync patch from ov fae: clyde.liu
		CVI_FLOAT ratio1 = (CVI_FLOAT)((u32IntTime[1] + 0.25) * (u32IntTime[0] + 0.75));
		CVI_FLOAT ratio2 = (CVI_FLOAT)((u32IntTime[1] + 0.75) * (u32IntTime[0] + 0.25));
		CVI_FLOAT ratio = ratio1 / ratio2;
		CVI_U16 fixGGain = (CVI_U16)(ratio * 1024);

		pstSnsRegsInfo->astI2cData[WDR2_GGAIN_0].u32Data = (fixGGain & 0xFF00) >> 8;
		pstSnsRegsInfo->astI2cData[WDR2_GGAIN_1].u32Data = (fixGGain & 0xFF);

	} else {
		pstSnsRegsInfo->astI2cData[LINEAR_EXP_0].u32Data = ((u32IntTime[0] & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_EXP_1].u32Data = (u32IntTime[0] & 0xFF);
	}

	return CVI_SUCCESS;

}

/*
 * GainReg {0x3508, 0x3509}	Real Gain
 * 0x0080~0x00FF		INT(GainReg/8)/16
 * 0x0100~0x01FF		INT(GainReg/16)/8
 * 0x0200~0x03FF		INT(GainReg/32)/4
 * 0x0400~0x07FF		INT(GainReg/64)/2
 */

static CVI_U32 gain_table[64] = {
	1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792,
	1856, 1920, 1984, 2048, 2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200,
	3328, 3456, 3584, 3712, 3840, 3968, 4096, 4352, 4608, 4864, 5120, 5376, 5632,
	5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192, 8704, 9216, 9728,
	10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360,
	15872
};

static CVI_S32 cmos_again_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32AgainLin, CVI_U32 *pu32AgainDb)
{
	int i;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	(void) ViPipe;
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);

	CMOS_CHECK_POINTER(pu32AgainLin);
	CMOS_CHECK_POINTER(pu32AgainDb);
	CMOS_CHECK_POINTER(pstSnsState);
	float rate;

	if (OTP_rate < 1.0)
		OTP_rate = (((os04c10_read_register(ViPipe, 0x71e0) * 256) + os04c10_read_register(ViPipe, 0x71e1))
			   * 1.0) / 1024;
	if (pstSnsState->enWDRMode == WDR_MODE_NONE && *pu32AgainLin > 15900) {
		rate = OTP_rate;
		HCG_EN = CVI_TRUE;
	} else {
		rate = 1;
		HCG_EN = CVI_FALSE;
	}

	if (*pu32AgainLin >= (unsigned int)(gain_table[63] * rate)) {
		*pu32AgainLin = (unsigned int)(gain_table[63] * rate);
		*pu32AgainDb = (63 - 48) * 64 + 1024;
		return CVI_SUCCESS;
	}

	for (i = 1; i < 64; i++) {
		if (*pu32AgainLin < (unsigned int)(gain_table[i] * rate)) {
			*pu32AgainLin = (unsigned int)(gain_table[i - 1] * rate);
			i--;
			if (i < 16)
				*pu32AgainDb = i * 8 + 128;
			else if (i < 32)
				*pu32AgainDb = (i - 16) * 16 + 256;
			else if (i < 48)
				*pu32AgainDb = (i - 32) * 32 + 512;
			else
				*pu32AgainDb = (i - 48) * 64 + 1024;

			break;
		}
	}

	if (*pu32AgainLin < 15872 && fabs(rate - OTP_rate) <= EPS)
		*pu32AgainLin = 15872;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_dgain_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32DgainLin, CVI_U32 *pu32DgainDb)
{
	(void) ViPipe;

	CMOS_CHECK_POINTER(pu32DgainLin);
	CMOS_CHECK_POINTER(pu32DgainDb);

	if (*pu32DgainLin <= 1024) {
		*pu32DgainLin = 1024;
	} else if (*pu32DgainLin > 16383) {
		*pu32DgainLin = 16383;
	}
	*pu32DgainDb = *pu32DgainLin;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_gains_update(VI_PIPE ViPipe, CVI_U32 *pu32Again, CVI_U32 *pu32Dgain)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pu32Again);
	CMOS_CHECK_POINTER(pu32Dgain);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		if (HCG_EN) {
			pstSnsRegsInfo->astI2cData[LINEAR_HCG].u32Data = 0x40;
		} else {
			pstSnsRegsInfo->astI2cData[LINEAR_HCG].u32Data = 0xc0;
		}
		/* linear mode */
		u32Again = pu32Again[0];
		u32Dgain = pu32Dgain[0];

		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN_0].u32Data = ((u32Again & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN_1].u32Data = (u32Again & 0xFF);
		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_0].u32Data = ((u32Dgain & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_1].u32Data = (u32Dgain & 0xFF);
	} else {
		/* DOL mode */
		u32Again = pu32Again[1];
		u32Dgain = pu32Dgain[1];

		pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_0].u32Data = ((u32Again & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_AGAIN1_1].u32Data = (u32Again & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_0].u32Data = ((u32Dgain & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_1].u32Data = (u32Dgain & 0xFF);

		if (g_au16Os04c10_GainMode[ViPipe] == SNS_GAIN_MODE_SHARE) {
			u32Again = pu32Again[1];
			u32Dgain = pu32Dgain[1];
		} else if (g_au16Os04c10_GainMode[ViPipe] == SNS_GAIN_MODE_WDR_2F) {
			u32Again = pu32Again[0];
			u32Dgain = pu32Dgain[0];
		} else {
			return CVI_SUCCESS;
		}

		pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_0].u32Data = ((u32Again & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_AGAIN2_1].u32Data = (u32Again & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_0].u32Data = ((u32Dgain & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_1].u32Data = (u32Dgain & 0xFF);

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
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32ShortTimeMinLimit = 2;
	/*
	 * Max Lexp = VTS - 8
	 * Max Sexp = VTS - Lexp - 12
	 * Long exp + Short exp < VTS - 12
	 */
	u32IntTimeMaxTmp0 = ((pstSnsState->au32FL[1] - 12 - pstSnsState->au32WDRIntTime[0]) * 0x40) /
					DIV_0_TO_1(au32Ratio[0]);
	u32IntTimeMaxTmp  = ((pstSnsState->au32FL[0] - 12) * 0x40)  / DIV_0_TO_1(au32Ratio[0] + 0x40);
	u32IntTimeMaxTmp = (u32IntTimeMaxTmp > u32IntTimeMaxTmp0) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;

	u32IntTimeMaxTmp  = (g_astOs04c10_State[ViPipe].u32Sexp_MAX < u32IntTimeMaxTmp) ?
					g_astOs04c10_State[ViPipe].u32Sexp_MAX : u32IntTimeMaxTmp;
	u32IntTimeMaxTmp  = (!u32IntTimeMaxTmp) ? 1 : u32IntTimeMaxTmp;
	syslog(LOG_DEBUG, "Max inttime0 = %u\n", u32IntTimeMaxTmp);


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

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_blc_default(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlc)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstBlc);

	memset(pstBlc, 0, sizeof(ISP_CMOS_BLACK_LEVEL_S));

	if (pstSnsState->enWDRMode == WDR_MODE_NONE)
		memcpy(pstBlc,
			&g_stIspBlcCalibratio, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	else
		memcpy(pstBlc,
			&g_stIspBlcCalibratio10Bit, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg)
{
	const OS04C10_MODE_S *pstMode = CVI_NULL;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstMode = &g_astOs04c10_mode[pstSnsState->u8ImgMode];

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

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstSnsState->bSyncInit = CVI_FALSE;

	switch (u8Mode) {
	case WDR_MODE_NONE:
		if (pstSnsState->u8ImgMode == OS04C10_MODE_2688X1520P30_WDR)
			pstSnsState->u8ImgMode = OS04C10_MODE_2688X1520P30;
		else if (pstSnsState->u8ImgMode == OS04C10_MODE_2560X1440P30_WDR)
			pstSnsState->u8ImgMode = OS04C10_MODE_2560X1440P30;
		else {
		}

		pstSnsState->enWDRMode = WDR_MODE_NONE;
		pstSnsState->u32FLStd = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;
		syslog(LOG_INFO, "WDR_MODE_NONE\n");
		break;

	case WDR_MODE_2To1_LINE:
		if (pstSnsState->u8ImgMode == OS04C10_MODE_2688X1520P30) {
			pstSnsState->u8ImgMode = OS04C10_MODE_2688X1520P30_WDR;
			syslog(LOG_INFO, "WDR_MODE_2To1_LINE 1520p mode(60fps->30fps)\n");
		} else if (pstSnsState->u8ImgMode == OS04C10_MODE_2560X1440P30) {
			pstSnsState->u8ImgMode = OS04C10_MODE_2560X1440P30_WDR;
		} else {
		}

		pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
		pstSnsState->u32FLStd = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;
		break;
	default:
		CVI_TRACE_SNS(CVI_DBG_ERR, "Unsupport sensor mode!\n");
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

static CVI_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsSyncInfo)
{
	CVI_U32 i;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	ISP_SNS_SYNC_INFO_S *pstCfg0 = CVI_NULL;
	ISP_SNS_SYNC_INFO_S *pstCfg1 = CVI_NULL;
	ISP_I2C_DATA_S *pstI2c_data = CVI_NULL;

	CMOS_CHECK_POINTER(pstSnsSyncInfo);
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstSnsRegsInfo = &pstSnsSyncInfo->snsCfg;
	pstCfg0 = &pstSnsState->astSyncInfo[0];
	pstCfg1 = &pstSnsState->astSyncInfo[1];
	pstI2c_data = pstCfg0->snsCfg.astI2cData;

	if ((pstSnsState->bSyncInit == CVI_FALSE) || (pstSnsRegsInfo->bConfig == CVI_FALSE)) {
		pstCfg0->snsCfg.enSnsType = SNS_I2C_TYPE;
		pstCfg0->snsCfg.unComBus.s8I2cDev = g_aunOs04c10_BusInfo[ViPipe].s8I2cDev;
		pstCfg0->snsCfg.u8Cfg2ValidDelayMax = 0;
		pstCfg0->snsCfg.use_snsr_sram = CVI_TRUE;
		pstCfg0->snsCfg.u32RegNum = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ?
					WDR2_REGS_NUM : LINEAR_REGS_NUM;

		for (i = 0; i < pstCfg0->snsCfg.u32RegNum; i++) {
			pstI2c_data[i].bUpdate = CVI_TRUE;
			pstI2c_data[i].u8DevAddr = os04c10_i2c_addr;
			pstI2c_data[i].u32AddrByteNum = os04c10_addr_byte;
			pstI2c_data[i].u32DataByteNum = os04c10_data_byte;
			pstI2c_data[i].bvblankUpdate = CVI_FALSE;
		}

		switch (pstSnsState->enWDRMode) {
		case WDR_MODE_2To1_LINE:
			pstI2c_data[WDR2_HOLD_START].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[WDR2_HOLD_START].u32Data = 0x00;
			pstI2c_data[WDR2_EXP1_0].u32RegAddr = OS04C10_EXP1_ADDR;
			pstI2c_data[WDR2_EXP1_1].u32RegAddr = OS04C10_EXP1_ADDR + 1;
			pstI2c_data[WDR2_EXP2_0].u32RegAddr = OS04C10_EXP2_ADDR;
			pstI2c_data[WDR2_EXP2_1].u32RegAddr = OS04C10_EXP2_ADDR + 1;
			pstI2c_data[WDR2_AGAIN1_0].u32RegAddr = OS04C10_AGAIN1_ADDR;
			pstI2c_data[WDR2_AGAIN1_1].u32RegAddr = OS04C10_AGAIN1_ADDR + 1;
			pstI2c_data[WDR2_AGAIN2_0].u32RegAddr = OS04C10_AGAIN2_ADDR;
			pstI2c_data[WDR2_AGAIN2_1].u32RegAddr = OS04C10_AGAIN2_ADDR + 1;
			pstI2c_data[WDR2_DGAIN1_0].u32RegAddr = OS04C10_DGAIN1_ADDR;
			pstI2c_data[WDR2_DGAIN1_1].u32RegAddr = OS04C10_DGAIN1_ADDR + 1;
			pstI2c_data[WDR2_DGAIN2_0].u32RegAddr = OS04C10_DGAIN2_ADDR;
			pstI2c_data[WDR2_DGAIN2_1].u32RegAddr = OS04C10_DGAIN2_ADDR + 1;
			pstI2c_data[WDR2_VTS_0].u32RegAddr = OS04C10_VTS_ADDR;
			pstI2c_data[WDR2_VTS_1].u32RegAddr = OS04C10_VTS_ADDR + 1;
			pstI2c_data[WDR2_HOLD_END].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[WDR2_HOLD_END].u32Data = 0x10;
			pstI2c_data[WDR2_LAUNCH_0].u32RegAddr = OS04C10_HOLD_320D;
			pstI2c_data[WDR2_LAUNCH_0].u32Data = 0x00;
			pstI2c_data[WDR2_LAUNCH_0].u8DelayFrmNum = 0;
			pstI2c_data[WDR2_LAUNCH_1].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[WDR2_LAUNCH_1].u32Data = 0xA0;
			pstI2c_data[WDR2_LAUNCH_1].u8DelayFrmNum = 0;
			pstI2c_data[WDR2_GGAIN_0].u32RegAddr = OS04C10_GGAIN_ADDR;
			pstI2c_data[WDR2_GGAIN_1].u32RegAddr = OS04C10_GGAIN_ADDR + 1;
			break;
		default:
			pstI2c_data[LINEAR_HOLD_START].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[LINEAR_HOLD_START].u32Data = 0x00;
			pstI2c_data[LINEAR_EXP_0].u32RegAddr = OS04C10_EXP1_ADDR;
			pstI2c_data[LINEAR_EXP_1].u32RegAddr = OS04C10_EXP1_ADDR + 1;
			pstI2c_data[LINEAR_AGAIN_0].u32RegAddr = OS04C10_AGAIN1_ADDR;
			pstI2c_data[LINEAR_AGAIN_1].u32RegAddr = OS04C10_AGAIN1_ADDR + 1;
			pstI2c_data[LINEAR_DGAIN_0].u32RegAddr = OS04C10_DGAIN1_ADDR;
			pstI2c_data[LINEAR_DGAIN_1].u32RegAddr = OS04C10_DGAIN1_ADDR + 1;
			pstI2c_data[LINEAR_VTS_0].u32RegAddr = OS04C10_VTS_ADDR;
			pstI2c_data[LINEAR_VTS_1].u32RegAddr = OS04C10_VTS_ADDR + 1;
			pstI2c_data[LINEAR_HOLD_END].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[LINEAR_HOLD_END].u32Data = 0x10;
			pstI2c_data[LINEAR_LAUNCH_0].u32RegAddr = OS04C10_HOLD_320D;
			pstI2c_data[LINEAR_LAUNCH_0].u32Data = 0x00;
			pstI2c_data[LINEAR_LAUNCH_0].u8DelayFrmNum = 0;
			pstI2c_data[LINEAR_LAUNCH_1].u32RegAddr = OS04C10_HOLD_3208;
			pstI2c_data[LINEAR_LAUNCH_1].u32Data = 0xA0;
			pstI2c_data[LINEAR_LAUNCH_1].u8DelayFrmNum = 0;
			pstI2c_data[LINEAR_HCG].u32RegAddr = OS04C10_LINEAR_HCG_ADDR;
			pstI2c_data[LINEAR_HCG].bvblankUpdate = CVI_TRUE;
			pstI2c_data[LINEAR_HCG].u8DelayFrmNum = 3;
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
		if (pstCfg0->snsCfg.need_update == CVI_TRUE) {
			if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
				pstI2c_data[WDR2_HOLD_START].bUpdate = CVI_TRUE;
				pstI2c_data[WDR2_HOLD_END].bUpdate = CVI_TRUE;
				pstI2c_data[WDR2_LAUNCH_0].bUpdate = CVI_TRUE;
				pstI2c_data[WDR2_LAUNCH_1].bUpdate = CVI_TRUE;
			} else {
				pstI2c_data[LINEAR_HOLD_START].bUpdate = CVI_TRUE;
				pstI2c_data[LINEAR_HOLD_END].bUpdate = CVI_TRUE;
				pstI2c_data[LINEAR_LAUNCH_0].bUpdate = CVI_TRUE;
				pstI2c_data[LINEAR_LAUNCH_1].bUpdate = CVI_TRUE;
			}
		}
		/* check update isp crop or not */
		pstCfg0->ispCfg.need_update = (sensor_cmp_wdr_size(&pstCfg0->ispCfg, &pstCfg1->ispCfg) ?
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
	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u8SensorImageMode = pstSnsState->u8ImgMode;
	pstSnsState->bSyncInit = CVI_FALSE;
	if (pstSensorImageMode->f32Fps <= 30) {
		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			if (OS04C10_RES_IS_1520P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
				u8SensorImageMode = OS04C10_MODE_2688X1520P30;
			else if (OS04C10_RES_IS_1440P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
				u8SensorImageMode = OS04C10_MODE_2560X1440P30;
			else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				       pstSensorImageMode->u16Width,
				       pstSensorImageMode->u16Height,
				       pstSensorImageMode->f32Fps,
				       pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		} else if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			if (OS04C10_RES_IS_1520P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
				u8SensorImageMode = OS04C10_MODE_2688X1520P30_WDR;
			else if (OS04C10_RES_IS_1440P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
				u8SensorImageMode = OS04C10_MODE_2560X1440P30_WDR;
			else {
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

static CVI_VOID sensor_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);
	if (pstSnsState->bInit == CVI_TRUE && g_aeOs04c10_MirrorFip[ViPipe] != eSnsMirrorFlip) {
		os04c10_mirror_flip(ViPipe, eSnsMirrorFlip);
		g_aeOs04c10_MirrorFip[ViPipe] = eSnsMirrorFlip;
	}
}

static CVI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsState->bInit = CVI_FALSE;
	pstSnsState->bSyncInit = CVI_FALSE;
	pstSnsState->u8ImgMode = OS04C10_MODE_2560X1440P30;
	pstSnsState->enWDRMode = WDR_MODE_NONE;
	pstSnsState->u32FLStd  = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsState->au32FL[0] = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsState->au32FL[1] = g_astOs04c10_mode[pstSnsState->u8ImgMode].u32VtsDef;

	memset(&pstSnsState->astSyncInfo[0], 0, sizeof(ISP_SNS_SYNC_INFO_S));
	memset(&pstSnsState->astSyncInfo[1], 0, sizeof(ISP_SNS_SYNC_INFO_S));
}

static CVI_S32 sensor_rx_attr(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *pstRxAttr)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstRxAttr);

	memcpy(pstRxAttr, &os04c10_rx_attr, sizeof(*pstRxAttr));

	pstRxAttr->img_size.width = g_astOs04c10_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Width;
	pstRxAttr->img_size.height = g_astOs04c10_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height;
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstRxAttr->mipi_attr.wdr_mode = CVI_MIPI_WDR_MODE_NONE;
	} else {
		pstRxAttr->mac_clk = RX_MAC_CLK_400M;
		pstRxAttr->mipi_attr.raw_data_type = RAW_DATA_10BIT;
	}
	return CVI_SUCCESS;
}

static CVI_S32 sensor_patch_rx_attr(RX_INIT_ATTR_S *pstRxInitAttr)
{
	SNS_COMBO_DEV_ATTR_S *pstRxAttr = &os04c10_rx_attr;
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

	pstSensorExpFunc->pfn_cmos_sensor_init = os04c10_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit = os04c10_exit;
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
	if (OS04C10_I2C_ADDR_IS_VALID(s32I2cAddr))
		os04c10_i2c_addr = s32I2cAddr;
}

static CVI_S32 os04c10_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunOs04c10_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

	if (pastSnsStateCtx == CVI_NULL) {
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
		if (pastSnsStateCtx == CVI_NULL) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
			return -ENOMEM;
		}
	}

	memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

	OS04C10_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

	return CVI_SUCCESS;
}

static CVI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	OS04C10_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
	SENSOR_FREE(pastSnsStateCtx);
	OS04C10_SENSOR_RESET_CTX(ViPipe);
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

	stSnsAttrInfo.eSensorId = OS04C10_ID;

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

	s32Ret = CVI_ISP_SensorUnRegCallBack(ViPipe, OS04C10_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, OS04C10_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, OS04C10_ID);
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
	g_au16Os04c10_GainMode[ViPipe] = pstInitAttr->enGainMode;
	g_au16Os04c10_UseHwSync[ViPipe] = pstInitAttr->u16UseHwSync;

	return CVI_SUCCESS;
}
static CVI_S32 sensor_probe(VI_PIPE ViPipe)
{
	return os04c10_probe(ViPipe);
}

ISP_SNS_OBJ_S stSnsOs04c10_Obj = {
	.pfnRegisterCallback    = sensor_register_callback,
	.pfnUnRegisterCallback  = sensor_unregister_callback,
	.pfnStandby             = os04c10_standby,
	.pfnRestart             = os04c10_restart,
	.pfnMirrorFlip          = sensor_mirror_flip,
	.pfnWriteReg            = os04c10_write_register,
	.pfnReadReg             = os04c10_read_register,
	.pfnSetBusInfo          = os04c10_set_bus_info,
	.pfnSetInit             = sensor_set_init,
	.pfnPatchRxAttr		= sensor_patch_rx_attr,
	.pfnPatchI2cAddr	= sensor_patch_i2c_addr,
	.pfnGetRxAttr		= sensor_rx_attr,
	.pfnExpSensorCb		= cmos_init_sensor_exp_function,
	.pfnExpAeCb		= cmos_init_ae_exp_function,
	.pfnSnsProbe		= sensor_probe,
};

