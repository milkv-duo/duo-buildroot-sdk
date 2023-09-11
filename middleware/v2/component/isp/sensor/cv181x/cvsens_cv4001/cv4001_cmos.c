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

#include "cv4001_cmos_ex.h"
#include "cv4001_cmos_param.h"

#define DIV_0_TO_1(a)   ((0 == (a)) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))
#define CV4001_ID 4001
#define CV4001_I2C_ADDR_1 0x35
#define CV4001_I2C_ADDR_2 0x36
#define CV4001_I2C_ADDR_IS_VALID(addr)      ((addr) == CV4001_I2C_ADDR_1 || (addr) == CV4001_I2C_ADDR_2)

/****************************************************************************
 * global variables                                                         *
 ***************************************************************************/

ISP_SNS_STATE_S *g_pastCV4001[VI_MAX_PIPE_NUM] = {CVI_NULL};

#define CV4001_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastCV4001[dev])
#define CV4001_SENSOR_SET_CTX(dev, pstCtx)   (g_pastCV4001[dev] = pstCtx)
#define CV4001_SENSOR_RESET_CTX(dev)         (g_pastCV4001[dev] = CVI_NULL)

ISP_SNS_COMMBUS_U g_aunCV4001_BusInfo[VI_MAX_PIPE_NUM] = {
	[0] = { .s8I2cDev = 3},
	[1 ... VI_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

ISP_SNS_MIRRORFLIP_TYPE_E g_aeCV4001_MirrorFip[VI_MAX_PIPE_NUM] = {0};

CVI_U16 g_au16CV4001_GainMode[VI_MAX_PIPE_NUM] = {0};

/****************************************************************************
 * local variables and functions                                            *
 ***************************************************************************/
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
/*****CV4001 Lines Range*****/
#define CV4001_FULL_LINES_MAX  (0xfffff / 2)//liner mode: vts_reg value is double real_vts
#define CV4001_FULL_LINES_MAX_2TO1_WDR  (0xfffff / 4)//wdr mode: four

/*****CV4001 Register Address*****/
#define CV4001_EXP1_ADDR0			0x3062 //bit[19:16]
#define CV4001_EXP1_ADDR1			0x3061
#define CV4001_EXP1_ADDR2			0x3060
#define CV4001_EXP2_ADDR0			0x3066
#define CV4001_EXP2_ADDR1			0x3065
#define CV4001_EXP2_ADDR2			0x3064

#define CV4001_AGAIN1_ADDR			0x3180 //bit[7:0]
#define CV4001_AGAIN2_ADDR			0x3181 //bit[7:0]

#define CV4001_DGAIN1_H_ADDR		0x3179 //bit[15:8]
#define CV4001_DGAIN1_L_ADDR		0x3178 //bit[7:0]
#define CV4001_DGAIN2_H_ADDR		0x317B //bit[15:8]
#define CV4001_DGAIN2_L_ADDR		0x317A //bit[7:0]

#define CV4001_VTS_ADDR0			0x302A //bit[19:16]
#define CV4001_VTS_ADDR1			0x3029
#define CV4001_VTS_ADDR2			0x3028

#define CV4001_FLIP_MIRROR_ADDR		0x3034

#define CV4001_RES_IS_1440P(w, h)      ((w) == 2560 && (h) == 1440)

static CVI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	const CV4001_MODE_S *pstMode;

	CVI_U32 FpsMax;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstAeSnsDft);
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstMode = &g_astCV4001_mode[pstSnsState->u8ImgMode];
	FpsMax = g_astCV4001_mode[pstSnsState->u8ImgMode].f32MaxFps;

	pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
	pstAeSnsDft->u32FlickerFreq = 50 * 256;
	pstAeSnsDft->u32FullLinesMax = (pstSnsState->enWDRMode == WDR_MODE_NONE) ?
			CV4001_FULL_LINES_MAX : CV4001_FULL_LINES_MAX_2TO1_WDR;
	pstAeSnsDft->u32HmaxTimes = (1000000) / (pstSnsState->u32FLStd * FpsMax);

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
		pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * FpsMax / 2;
	else
		pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];

	switch (pstSnsState->enWDRMode) {
	default:
	case WDR_MODE_NONE:   /*linear mode*/
		pstAeSnsDft->f32Fps = pstMode->f32MaxFps;
		pstAeSnsDft->f32MinFps = pstMode->f32MinFps;
		pstAeSnsDft->au8HistThresh[0] = 0xd;
		pstAeSnsDft->au8HistThresh[1] = 0x28;
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
		pstAeSnsDft->u32MinDgain = pstMode->stDgain[0].u32Max;
		pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

		pstAeSnsDft->u8AeCompensation = 40;
		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 2;
		pstAeSnsDft->u32AEResponseFrame = 4;
		pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ?
			g_au32InitExposure[ViPipe] : pstMode->stExp[0].u16Def;

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

		pstAeSnsDft->u32InitAESpeed = 64;
		pstAeSnsDft->u32InitAETolerance = 5;
		pstAeSnsDft->u32AEResponseFrame = 4;
		pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ?
			g_au32InitExposure[ViPipe] : pstMode->stExp[0].u16Def;

		if (genFSWDRMode[ViPipe] == ISP_FSWDR_LONG_FRAME_MODE) {
			pstAeSnsDft->u8AeCompensation = 64;
			pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;
		} else {
			pstAeSnsDft->u8AeCompensation = 40;
			pstAeSnsDft->enAeExpMode = AE_EXP_LOWLIGHT_PRIOR;
		}

		break;
	}
	CVI_TRACE_SNS(CVI_DBG_INFO, "again[%d, %d], dgain[%d, %d]\n",
		pstAeSnsDft->u32MinAgain, pstAeSnsDft->u32MaxAgain, pstAeSnsDft->u32MinDgain, pstAeSnsDft->u32MaxDgain);

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
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;
	u32Vts = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;
	f32MaxFps = g_astCV4001_mode[pstSnsState->u8ImgMode].f32MaxFps;
	f32MinFps = g_astCV4001_mode[pstSnsState->u8ImgMode].f32MinFps;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Unsupport Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}

		u32VMAX = (u32VMAX > CV4001_FULL_LINES_MAX) ?
				CV4001_FULL_LINES_MAX : u32VMAX;
		pstSnsRegsInfo->astI2cData[LINEAR_VTS_0].u32Data = (((u32VMAX * 2) & 0xFF0000) >> 16);
		pstSnsRegsInfo->astI2cData[LINEAR_VTS_1].u32Data = (((u32VMAX * 2) & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_VTS_2].u32Data = ((u32VMAX * 2) & 0xFF);
	} else {
		if ((f32Fps <= f32MaxFps) && (f32Fps >= f32MinFps)) {
			u32VMAX = u32Vts * f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
		} else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Unsupport Fps: %f\n", f32Fps);
			return CVI_FAILURE;
		}

		u32VMAX = (u32VMAX > CV4001_FULL_LINES_MAX_2TO1_WDR) ?
				CV4001_FULL_LINES_MAX_2TO1_WDR : u32VMAX;
		pstSnsRegsInfo->astI2cData[WDR2_VTS_0].u32Data = (((u32VMAX * 4) & 0xFF0000) >> 16);
		pstSnsRegsInfo->astI2cData[WDR2_VTS_1].u32Data = (((u32VMAX * 4) & 0xFF00) >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_VTS_2].u32Data = ((u32VMAX * 4) & 0xFF);
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
	CVI_S32 Reg_IntTime1;
	CVI_S32 Reg_IntTime2;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(u32IntTime);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		Reg_IntTime1 = (pstSnsState->u32FLStd  - u32IntTime[0]) * 2;

		pstSnsRegsInfo->astI2cData[LINEAR_EXP_0].u32Data = ((Reg_IntTime1 >> 16) & 0x0F);
		pstSnsRegsInfo->astI2cData[LINEAR_EXP_1].u32Data = ((Reg_IntTime1 >> 8) & 0xFF);
		pstSnsRegsInfo->astI2cData[LINEAR_EXP_2].u32Data = (Reg_IntTime1 & 0xFF);
	} else {
		/* short exposure */
		pstSnsState->au32WDRIntTime[0] = u32IntTime[0];//?
		/* long exposure */
		pstSnsState->au32WDRIntTime[1] = u32IntTime[1];

		Reg_IntTime1 = (pstSnsState->u32FLStd  - u32IntTime[1]) * 4;//u32IntTime[1] long exposure
		Reg_IntTime2 = (pstSnsState->u32FLStd  - u32IntTime[0]) * 4 + 2;//u32IntTime[0] short exposure

		pstSnsRegsInfo->astI2cData[WDR2_EXP1_0].u32Data = ((Reg_IntTime1 >> 16) & 0x0F);
		pstSnsRegsInfo->astI2cData[WDR2_EXP1_1].u32Data = ((Reg_IntTime1 >> 8) & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_EXP1_2].u32Data = (Reg_IntTime1 & 0xFF);

		pstSnsRegsInfo->astI2cData[WDR2_EXP2_0].u32Data = ((Reg_IntTime2 >> 16) & 0x0F);
		pstSnsRegsInfo->astI2cData[WDR2_EXP2_1].u32Data = ((Reg_IntTime2 >> 8) & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_EXP2_2].u32Data = (Reg_IntTime2 & 0xFF);
	}

	return CVI_SUCCESS;
}

static CVI_U32 gain_table[242] = {
	1024, 1024, 1024, 1024, 1040, 1040, 1040, 1040, 1056, 1056,
	1056, 1056, 1072, 1072, 1072, 1072, 1088, 1088, 1088, 1104,
	1104, 1104, 1120, 1120, 1120, 1120, 1136, 1136, 1136, 1152,
	1152, 1152, 1168, 1168, 1168, 1184, 1184, 1184, 1200, 1200,
	1200, 1216, 1216, 1216, 1232, 1232, 1248, 1248, 1248, 1264,
	1264, 1264, 1280, 1280, 1296, 1296, 1296, 1312, 1312, 1328,
	1328, 1344, 1344, 1344, 1360, 1360, 1376, 1376, 1392, 1392,
	1408, 1408, 1424, 1424, 1440, 1440, 1456, 1456, 1472, 1472,
	1488, 1488, 1504, 1504, 1520, 1520, 1536, 1536, 1552, 1568,
	1568, 1584, 1584, 1600, 1616, 1616, 1632, 1648, 1648, 1664,
	1680, 1680, 1696, 1712, 1712, 1728, 1744, 1744, 1760, 1776,
	1792, 1808, 1808, 1824, 1840, 1856, 1872, 1872, 1888, 1904,
	1920, 1936, 1952, 1968, 1984, 2000, 2016, 2016, 2048, 2064,
	2080, 2096, 2112, 2128, 2144, 2160, 2176, 2192, 2208, 2240,
	2256, 2272, 2288, 2304, 2336, 2352, 2368, 2400, 2416, 2448,
	2464, 2496, 2512, 2544, 2560, 2592, 2608, 2640, 2672, 2688,
	2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008,
	3040, 3072, 3120, 3152, 3184, 3232, 3264, 3312, 3360, 3392,
	3440, 3488, 3536, 3584, 3632, 3680, 3744, 3792, 3840, 3904,
	3968, 4032, 4096, 4160, 4224, 4288, 4368, 4432, 4512, 4592,
	4672, 4752, 4848, 4944, 5040, 5136, 5232, 5344, 5456, 5568,
	5696, 5824, 5952, 6080, 6240, 6384, 6544, 6720, 6896, 7072,
	7280, 7488, 7696, 7936, 8192, 8448, 8736, 9024, 9360, 9696,
	10080, 10480, 10912, 11392, 11904, 12480, 13104, 13792, 14560, 15408,
	16384, 0x3FFFF,
};

static CVI_S32 cmos_again_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32AgainLin, CVI_U32 *pu32AgainDb)
{
	int i, total;
	CVI_U32 pregain;

	CMOS_CHECK_POINTER(pu32AgainLin);
	CMOS_CHECK_POINTER(pu32AgainDb);
	UNUSED(ViPipe);
	total = sizeof(gain_table) / sizeof(CVI_U32);

	if (*pu32AgainLin >= gain_table[total - 1]) {
		*pu32AgainLin = *pu32AgainDb = gain_table[total - 1];
		return CVI_SUCCESS;
	}

	for (i = 1; i < total; i++) {
		if (*pu32AgainLin < gain_table[i])
			break;
	}
	i--;
	// find the pregain
	pregain = *pu32AgainLin * 64 / gain_table[i];
	// set the Db as the AE algo gain, we need this to do gain update
	*pu32AgainDb = *pu32AgainLin;
	// set the Lin as the closest sensor gain for AE algo reference
	*pu32AgainLin = pregain * gain_table[i] / 64;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_dgain_calc_table(VI_PIPE ViPipe, CVI_U32 *pu32DgainLin, CVI_U32 *pu32DgainDb)
{
	CVI_U32 pregain;

	CMOS_CHECK_POINTER(pu32DgainLin);
	CMOS_CHECK_POINTER(pu32DgainDb);
	UNUSED(ViPipe);
	// find the pregain
	pregain = *pu32DgainLin * 64 / 1024;
	// set the Db as the AE algo gain, we need this to do gain update
	*pu32DgainDb = *pu32DgainLin;
	// set the Lin as the closest sensor gain for AE algo reference
	*pu32DgainLin = pregain * 16;

	return CVI_SUCCESS;
}

static CVI_S32 cmos_gains_update(VI_PIPE ViPipe, CVI_U32 *pu32Again, CVI_U32 *pu32Dgain)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	int i, total;

	total = sizeof(gain_table) / sizeof(CVI_U32);

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pu32Again);
	CMOS_CHECK_POINTER(pu32Dgain);
	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;

	/* only surpport linear mode */
	for (i = 0; i < total; i++) {
		if ((gain_table[i] <= pu32Again[0]) && (gain_table[i+1] >= pu32Again[0])) {
			break;
		}
	}

	u32Again = i;
	u32Dgain = pu32Dgain[0] / 16;
	if (u32Dgain < 64) {
		u32Dgain = 64;
	}
	if (u32Dgain > 1024) {
		u32Dgain = 1024;
	}

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstSnsRegsInfo->astI2cData[LINEAR_AGAIN].u32Data = u32Again;

		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_H].u32Data = (u32Dgain >> 8);
		pstSnsRegsInfo->astI2cData[LINEAR_DGAIN_L].u32Data = (u32Dgain & 0xFF);
	} else {
		pstSnsRegsInfo->astI2cData[WDR2_AGAIN1].u32Data = u32Again;
		pstSnsRegsInfo->astI2cData[WDR2_AGAIN2].u32Data = u32Again;

		pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_H].u32Data = (u32Dgain >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN1_L].u32Data = (u32Dgain & 0xFF);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_H].u32Data = (u32Dgain >> 8);
		pstSnsRegsInfo->astI2cData[WDR2_DGAIN2_L].u32Data = (u32Dgain & 0xFF);
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
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u32ShortTimeMinLimit = 1;
	/*
	 * Long exp + Short exp < VTS
	 */
	u32IntTimeMaxTmp0 = ((pstSnsState->au32FL[1] - 1 - g_astCV4001_mode[pstSnsState->u8ImgMode].u32IspResTime -
			pstSnsState->au32WDRIntTime[0]) * 0x40) / DIV_0_TO_1(au32Ratio[0]);
	u32IntTimeMaxTmp  = ((pstSnsState->au32FL[0] - 1 - g_astCV4001_mode[pstSnsState->u8ImgMode].u32IspResTime)
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
	UNUSED(ViPipe);
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
	UNUSED(ViPipe);

	memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_blc_default(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlc)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	UNUSED(ViPipe);
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstBlc);

	memset(pstBlc, 0, sizeof(ISP_CMOS_BLACK_LEVEL_S));

	if (pstSnsState->enWDRMode == WDR_MODE_NONE)
		memcpy(pstBlc,
			&g_stIspBlcCalibratio, sizeof(ISP_CMOS_BLACK_LEVEL_S));
	else
		memcpy(pstBlc,
			&g_stIspBlcCalibratio_wdr, sizeof(ISP_CMOS_BLACK_LEVEL_S));

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg)
{
	const CV4001_MODE_S *pstMode = CVI_NULL;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstMode = &g_astCV4001_mode[pstSnsState->u8ImgMode];

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstIspCfg->frm_num = 1;
		memcpy(&pstIspCfg->img_size[0], &pstMode->astImg[0], sizeof(ISP_WDR_SIZE_S));
	} else {
		pstIspCfg->frm_num = 2;
		memcpy(&pstIspCfg->img_size[0], &pstMode->astImg[0], sizeof(ISP_WDR_SIZE_S));
		memcpy(&pstIspCfg->img_size[1], &pstMode->astImg[1], sizeof(ISP_WDR_SIZE_S));
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, CVI_U8 u8Mode)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstSnsState->bSyncInit = CVI_FALSE;

	switch (u8Mode) {
	case WDR_MODE_NONE:
		if (pstSnsState->u8ImgMode == CV4001_MODE_2560X1440P15_WDR)
			pstSnsState->u8ImgMode = CV4001_MODE_2560X1440P25;

		pstSnsState->enWDRMode = WDR_MODE_NONE;
		pstSnsState->u32FLStd = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;
		syslog(LOG_INFO, "WDR_MODE_NONE\n");
		break;

	case WDR_MODE_2To1_LINE:
		if (pstSnsState->u8ImgMode == CV4001_MODE_2560X1440P25) {
			pstSnsState->u8ImgMode = CV4001_MODE_2560X1440P15_WDR;
		}

		pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
		pstSnsState->u32FLStd = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;
		syslog(LOG_INFO, "WDR_MODE_2To1_LINE 1440p mode(60fps->30fps)\n");
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
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	pstSnsRegsInfo = &pstSnsSyncInfo->snsCfg;
	pstCfg0 = &pstSnsState->astSyncInfo[0];
	pstCfg1 = &pstSnsState->astSyncInfo[1];
	pstI2c_data = pstCfg0->snsCfg.astI2cData;

	if ((pstSnsState->bSyncInit == CVI_FALSE) || (pstSnsRegsInfo->bConfig == CVI_FALSE)) {
		pstCfg0->snsCfg.enSnsType = SNS_I2C_TYPE;
		pstCfg0->snsCfg.unComBus.s8I2cDev = g_aunCV4001_BusInfo[ViPipe].s8I2cDev;
		pstCfg0->snsCfg.u8Cfg2ValidDelayMax = 0;
		pstCfg0->snsCfg.use_snsr_sram = CVI_TRUE;
		pstCfg0->snsCfg.u32RegNum = (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) ?
					WDR2_REGS_NUM : LINEAR_REGS_NUM;

		for (i = 0; i < pstCfg0->snsCfg.u32RegNum; i++) {
			pstI2c_data[i].bUpdate = CVI_TRUE;
			pstI2c_data[i].u8DevAddr = cv4001_i2c_addr;
			pstI2c_data[i].u32AddrByteNum = cv4001_addr_byte;
			pstI2c_data[i].u32DataByteNum = cv4001_data_byte;
		}

		switch (pstSnsState->enWDRMode) {
		case WDR_MODE_2To1_LINE:
			pstI2c_data[WDR2_EXP1_0].u32RegAddr = CV4001_EXP1_ADDR0;
			pstI2c_data[WDR2_EXP1_1].u32RegAddr = CV4001_EXP1_ADDR1;
			pstI2c_data[WDR2_EXP1_2].u32RegAddr = CV4001_EXP1_ADDR2;
			pstI2c_data[WDR2_EXP2_0].u32RegAddr = CV4001_EXP2_ADDR0;
			pstI2c_data[WDR2_EXP2_1].u32RegAddr = CV4001_EXP2_ADDR1;
			pstI2c_data[WDR2_EXP2_2].u32RegAddr = CV4001_EXP2_ADDR2;
			pstI2c_data[WDR2_AGAIN1].u32RegAddr = CV4001_AGAIN1_ADDR;
			pstI2c_data[WDR2_AGAIN2].u32RegAddr = CV4001_AGAIN2_ADDR;
			pstI2c_data[WDR2_DGAIN1_H].u32RegAddr = CV4001_DGAIN1_H_ADDR;
			pstI2c_data[WDR2_DGAIN1_L].u32RegAddr = CV4001_DGAIN1_L_ADDR;
			pstI2c_data[WDR2_DGAIN2_H].u32RegAddr = CV4001_DGAIN2_H_ADDR;
			pstI2c_data[WDR2_DGAIN2_L].u32RegAddr = CV4001_DGAIN2_L_ADDR;
			pstI2c_data[WDR2_VTS_0].u32RegAddr = CV4001_VTS_ADDR0;
			pstI2c_data[WDR2_VTS_1].u32RegAddr = CV4001_VTS_ADDR1;
			pstI2c_data[WDR2_VTS_2].u32RegAddr = CV4001_VTS_ADDR2;
			pstI2c_data[WDR2_FLIP_MIRROR].u32RegAddr = CV4001_FLIP_MIRROR_ADDR;
			break;
		default:
			pstI2c_data[LINEAR_EXP_0].u32RegAddr = CV4001_EXP1_ADDR0;
			pstI2c_data[LINEAR_EXP_1].u32RegAddr = CV4001_EXP1_ADDR1;
			pstI2c_data[LINEAR_EXP_2].u32RegAddr = CV4001_EXP1_ADDR2;
			pstI2c_data[LINEAR_AGAIN].u32RegAddr = CV4001_AGAIN1_ADDR;
			pstI2c_data[LINEAR_DGAIN_H].u32RegAddr = CV4001_DGAIN1_H_ADDR;
			pstI2c_data[LINEAR_DGAIN_L].u32RegAddr = CV4001_DGAIN1_L_ADDR;
			pstI2c_data[LINEAR_VTS_0].u32RegAddr = CV4001_VTS_ADDR0;
			pstI2c_data[LINEAR_VTS_1].u32RegAddr = CV4001_VTS_ADDR1;
			pstI2c_data[LINEAR_VTS_2].u32RegAddr = CV4001_VTS_ADDR2;
			pstI2c_data[LINEAR_FLIP_MIRROR].u32RegAddr = CV4001_FLIP_MIRROR_ADDR;
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
	}

	pstSnsRegsInfo->bConfig = CVI_FALSE;
	memcpy(pstSnsSyncInfo, &pstSnsState->astSyncInfo[0], sizeof(ISP_SNS_SYNC_INFO_S));
	memcpy(&pstSnsState->astSyncInfo[1], &pstSnsState->astSyncInfo[0], sizeof(ISP_SNS_SYNC_INFO_S));
	pstSnsState->au32FL[1] = pstSnsState->au32FL[0];

	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstCfg0->snsCfg.astI2cData[LINEAR_FLIP_MIRROR].bDropFrm = CVI_FALSE;
	} else {
		pstCfg0->snsCfg.astI2cData[WDR2_FLIP_MIRROR].bDropFrm = CVI_FALSE;
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	CVI_U8 u8SensorImageMode = 0;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstSensorImageMode);
	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u8SensorImageMode = pstSnsState->u8ImgMode;
	pstSnsState->bSyncInit = CVI_FALSE;

	if (pstSensorImageMode->f32Fps <= 30) {
		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			if (CV4001_RES_IS_1440P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
				u8SensorImageMode = CV4001_MODE_2560X1440P25;
			else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
					      pstSensorImageMode->u16Width,
					      pstSensorImageMode->u16Height,
					      pstSensorImageMode->f32Fps,
					      pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		} else if (pstSnsState->enWDRMode == WDR_MODE_2To1_LINE) {
			if (CV4001_RES_IS_1440P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)) {
				u8SensorImageMode = CV4001_MODE_2560X1440P15_WDR;
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
		CVI_TRACE_SNS(CVI_DBG_ERR, "Not support this Fps:%f\n", pstSensorImageMode->f32Fps);
		return CVI_FAILURE;
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
	CVI_U8 value = 0x0;
	CVI_U8 start_x = 4;
	CVI_U8 start_y = 4;

	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_REGS_INFO_S *pstSnsRegsInfo = CVI_NULL;
	ISP_SNS_ISP_INFO_S *pstIspCfg0 = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsRegsInfo = &pstSnsState->astSyncInfo[0].snsCfg;
	pstIspCfg0 = &pstSnsState->astSyncInfo[0].ispCfg;

	/* Apply the setting on the fly  */
	if (pstSnsState->bInit == CVI_TRUE && g_aeCV4001_MirrorFip[ViPipe] != eSnsMirrorFlip) {
		switch (eSnsMirrorFlip) {
		case ISP_SNS_NORMAL:
			value = 0x0;
			start_x = 4;
			start_y = 4;
			break;
		case ISP_SNS_MIRROR:
			value = 0x1;
			start_x = 5;
			start_y = 4;
			break;
		case ISP_SNS_FLIP:
			value = 0x2;
			start_x = 4;
			start_y = 5;
			break;
		case ISP_SNS_MIRROR_FLIP:
			value = 0x3;
			start_x = 5;
			start_y = 5;
			break;
		default:
			return;
		}

		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			pstSnsRegsInfo->astI2cData[LINEAR_FLIP_MIRROR].u32Data = value;
			pstSnsRegsInfo->astI2cData[LINEAR_FLIP_MIRROR].bDropFrm = 1;
			pstSnsRegsInfo->astI2cData[LINEAR_FLIP_MIRROR].u8DropFrmNum = 2;
		} else {
			start_x = 0;
			start_y = 0;
			pstSnsRegsInfo->astI2cData[WDR2_FLIP_MIRROR].u32Data = value;
			pstSnsRegsInfo->astI2cData[WDR2_FLIP_MIRROR].bDropFrm = 1;
			pstSnsRegsInfo->astI2cData[WDR2_FLIP_MIRROR].u8DropFrmNum = 2;
		}
		g_aeCV4001_MirrorFip[ViPipe] = eSnsMirrorFlip;
		pstIspCfg0->img_size[0].stWndRect.s32X = start_x;
		pstIspCfg0->img_size[0].stWndRect.s32Y = start_y;

	}
}

static CVI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsState->bInit = CVI_FALSE;
	pstSnsState->bSyncInit = CVI_FALSE;
	pstSnsState->u8ImgMode = CV4001_MODE_2560X1440P25;
	pstSnsState->enWDRMode = WDR_MODE_NONE;
	pstSnsState->u32FLStd  = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsState->au32FL[0] = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;
	pstSnsState->au32FL[1] = g_astCV4001_mode[pstSnsState->u8ImgMode].u32VtsDef;

	memset(&pstSnsState->astSyncInfo[0], 0, sizeof(ISP_SNS_SYNC_INFO_S));
	memset(&pstSnsState->astSyncInfo[1], 0, sizeof(ISP_SNS_SYNC_INFO_S));
}

static CVI_S32 sensor_rx_attr(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *pstRxAttr)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstRxAttr);

	memcpy(pstRxAttr, &cv4001_rx_attr, sizeof(*pstRxAttr));

	pstRxAttr->img_size.width = g_astCV4001_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Width;
	pstRxAttr->img_size.height = g_astCV4001_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height;
	if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
		pstRxAttr->mipi_attr.wdr_mode = CVI_MIPI_WDR_MODE_NONE;
	} else {
		pstRxAttr->mac_clk = RX_MAC_CLK_400M;
	}

	return CVI_SUCCESS;

}

static CVI_S32 sensor_patch_rx_attr(RX_INIT_ATTR_S *pstRxInitAttr)
{
	SNS_COMBO_DEV_ATTR_S *pstRxAttr = &cv4001_rx_attr;
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

	pstSensorExpFunc->pfn_cmos_sensor_init = cv4001_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit = cv4001_exit;
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
	if (CV4001_I2C_ADDR_IS_VALID(s32I2cAddr))
		cv4001_i2c_addr = s32I2cAddr;
}

static CVI_S32 cv4001_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunCV4001_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

	if (pastSnsStateCtx == CVI_NULL) {
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
		if (pastSnsStateCtx == CVI_NULL) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
			return -ENOMEM;
		}
	}

	memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

	CV4001_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

	return CVI_SUCCESS;
}

static CVI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	CV4001_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
	SENSOR_FREE(pastSnsStateCtx);
	CV4001_SENSOR_RESET_CTX(ViPipe);
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

	stSnsAttrInfo.eSensorId = CV4001_ID;

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

	s32Ret = CVI_ISP_SensorUnRegCallBack(ViPipe, CV4001_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, CV4001_ID);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
		return s32Ret;
	}

	s32Ret = CVI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, CV4001_ID);
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
	g_au16CV4001_GainMode[ViPipe] = pstInitAttr->enGainMode;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_probe(VI_PIPE ViPipe)
{
	return cv4001_probe(ViPipe);
}

ISP_SNS_OBJ_S stSnsCV4001_Obj = {
	.pfnRegisterCallback    = sensor_register_callback,
	.pfnUnRegisterCallback  = sensor_unregister_callback,
	.pfnStandby             = cv4001_standby,
	.pfnRestart             = cv4001_restart,
	.pfnWriteReg            = cv4001_write_register,
	.pfnReadReg             = cv4001_read_register,
	.pfnSetBusInfo          = cv4001_set_bus_info,
	.pfnSetInit             = sensor_set_init,
	.pfnMirrorFlip          = sensor_mirror_flip,
	.pfnPatchRxAttr         = sensor_patch_rx_attr,
	.pfnPatchI2cAddr        = sensor_patch_i2c_addr,
	.pfnGetRxAttr           = sensor_rx_attr,
	.pfnExpSensorCb         = cmos_init_sensor_exp_function,
	.pfnExpAeCb             = cmos_init_ae_exp_function,
	.pfnSnsProbe            = sensor_probe,
};

