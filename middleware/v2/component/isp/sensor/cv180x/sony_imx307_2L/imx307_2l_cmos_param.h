#ifndef __IMX307_2L_CMOS_PARAM_H_
#define __IMX307_2L_CMOS_PARAM_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ARCH_CV182X
#include <linux/cvi_vip_cif.h>
#include <linux/cvi_vip_snsr.h>
#include "cvi_type.h"
#else
#include <linux/cif_uapi.h>
#include <linux/vi_snsr.h>
#include <linux/cvi_type.h>
#endif
#include "cvi_sns_ctrl.h"
#include "imx307_2l_cmos_ex.h"

static const IMX307_2L_MODE_S g_astImx307_2l_mode[IMX307_2L_MODE_NUM] = {
	[IMX307_2L_MODE_1080P30] = {
		.name = "1080p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.13, /* 1125 * 30 / 0x3FFFF */
		.u32HtsDef = 0x1130,
		.u32VtsDef = 1125,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1123,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 62416,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 38485,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.u16RHS1 = 11,
		.u16BRL = 1109,
		.u16OpbSize = 10,
		.u16MarginVtop = 8,
		.u16MarginVbot = 9,
	},
	[IMX307_2L_MODE_1080P30_WDR] = {
		.name = "1080p30wdr",
		/* sef */
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
		},
		/* lef */
		.astImg[1] = {
			.stSnsSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1948,
				.u32Height = 1097,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.13, /* 1125 * 30 / 0x3FFFF */
		.u32HtsDef = 0x0898,
		.u32VtsDef = 1125,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 8,
			.u16Def = 8,
			.u16Step = 1,
		},
		.stExp[1] = {
			.u16Min = 1,
			.u16Max = 2236,
			.u16Def = 828,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 62416,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stAgain[1] = {
			.u16Min = 1024,
			.u16Max = 62416,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 38485,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[1] = {
			.u16Min = 1024,
			.u16Max = 38485,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.u16RHS1 = 11,
		.u16BRL = 1109,
		.u16OpbSize = 7,
		.u16MarginVtop = 8,
		.u16MarginVbot = 9,
	},
};

static ISP_CMOS_NOISE_CALIBRATION_S g_stIspNoiseCalibratio = {.CalibrationCoef = {
	{	//iso  100
		{0.03396590426564216614,	2.31280159950256347656}, //B: slope, intercept
		{0.02877708896994590759,	1.99502599239349365234}, //Gb: slope, intercept
		{0.02853375114500522614,	1.53825533390045166016}, //Gr: slope, intercept
		{0.02940983884036540985,	2.40369534492492675781}, //R: slope, intercept
	},
	{	//iso  200
		{0.03635194525122642517,	5.70069074630737304688}, //B: slope, intercept
		{0.02990435808897018433,	6.70210981369018554688}, //Gb: slope, intercept
		{0.03183263540267944336,	4.92136955261230468750}, //Gr: slope, intercept
		{0.03145531564950942993,	5.97894525527954101563}, //R: slope, intercept
	},
	{	//iso  400
		{0.04329251870512962341,	7.09708356857299804688}, //B: slope, intercept
		{0.03147880360484123230,	11.36076831817626953125}, //Gb: slope, intercept
		{0.03299136087298393250,	8.49763488769531250000}, //Gr: slope, intercept
		{0.03585162013769149780,	8.31188583374023437500}, //R: slope, intercept
	},
	{	//iso  800
		{0.05740678682923316956,	10.93907737731933593750}, //B: slope, intercept
		{0.03538244962692260742,	21.53132820129394531250}, //Gb: slope, intercept
		{0.04014955088496208191,	14.56391811370849609375}, //Gr: slope, intercept
		{0.03907874971628189087,	15.16305541992187500000}, //R: slope, intercept
	},
	{	//iso  1600
		{0.05417122691869735718,	23.33716773986816406250}, //B: slope, intercept
		{0.02816017158329486847,	55.33533859252929687500}, //Gb: slope, intercept
		{0.03809726238250732422,	30.80285072326660156250}, //Gr: slope, intercept
		{0.04028835147619247437,	31.24934768676757812500}, //R: slope, intercept
	},
	{	//iso  3200
		{0.07438381761312484741,	33.81318664550781250000}, //B: slope, intercept
		{0.02690842747688293457,	95.65101623535156250000}, //Gb: slope, intercept
		{0.05132644250988960266,	46.27661132812500000000}, //Gr: slope, intercept
		{0.05343631654977798462,	44.67097473144531250000}, //R: slope, intercept
	},
	{	//iso  6400
		{0.10831451416015625000,	49.42535018920898437500}, //B: slope, intercept
		{0.04577258601784706116,	122.51580810546875000000}, //Gb: slope, intercept
		{0.06677398085594177246,	69.54801177978515625000}, //Gr: slope, intercept
		{0.07670628279447555542,	63.47140884399414062500}, //R: slope, intercept
	},
	{	//iso  12800
		{0.15062870085239410400,	68.36373138427734375000}, //B: slope, intercept
		{0.07762257009744644165,	146.30807495117187500000}, //Gb: slope, intercept
		{0.09608269482851028442,	96.59751892089843750000}, //Gr: slope, intercept
		{0.10346080362796783447,	96.64923095703125000000}, //R: slope, intercept
	},
	{	//iso  25600
		{0.21278673410415649414,	107.37140655517578125000}, //B: slope, intercept
		{0.12505164742469787598,	179.24717712402343750000}, //Gb: slope, intercept
		{0.14927712082862854004,	140.71289062500000000000}, //Gr: slope, intercept
		{0.15530782938003540039,	139.79985046386718750000}, //R: slope, intercept
	},
	{	//iso  51200
		{0.32940942049026489258,	149.26779174804687500000}, //B: slope, intercept
		{0.18958723545074462891,	247.18806457519531250000}, //Gb: slope, intercept
		{0.19027391076087951660,	230.56108093261718750000}, //Gr: slope, intercept
		{0.23455394804477691650,	192.10685729980468750000}, //R: slope, intercept
	},
	{	//iso  102400
		{0.48793542385101318359,	210.41285705566406250000}, //B: slope, intercept
		{0.24973315000534057617,	372.87121582031250000000}, //Gb: slope, intercept
		{0.23015110194683074951,	402.12283325195312500000}, //Gr: slope, intercept
		{0.35159105062484741211,	294.27154541015625000000}, //R: slope, intercept
	},
	{	//iso  204800
		{0.60629695653915405273,	340.77212524414062500000}, //B: slope, intercept
		{0.39248645305633544922,	481.49472045898437500000}, //Gb: slope, intercept
		{0.33751612901687622070,	544.22698974609375000000}, //Gr: slope, intercept
		{0.43583938479423522949,	458.90490722656250000000}, //R: slope, intercept
	},
	{	//iso  409600
		{0.71151763200759887695,	544.37280273437500000000}, //B: slope, intercept
		{0.47628879547119140625,	697.04498291015625000000}, //Gb: slope, intercept
		{0.38568580150604248047,	794.64263916015625000000}, //Gr: slope, intercept
		{0.54425776004791259766,	658.77343750000000000000}, //R: slope, intercept
	},
	{	//iso  819200
		{0.61085152626037597656,	821.67352294921875000000}, //B: slope, intercept
		{0.38012877106666564941,	1050.61950683593750000000}, //Gb: slope, intercept
		{0.38677954673767089844,	1049.79968261718750000000}, //Gr: slope, intercept
		{0.54386067390441894531,	875.86968994140625000000}, //R: slope, intercept
	},
	{	//iso  1638400
		{0.88969016075134277344,	635.22283935546875000000}, //B: slope, intercept
		{0.71133875846862792969,	754.16796875000000000000}, //Gb: slope, intercept
		{0.66038286685943603516,	765.11163330078125000000}, //Gr: slope, intercept
		{0.61520493030548095703,	817.94128417968750000000}, //R: slope, intercept
	},
	{	//iso  3276800
		{1.43259191513061523438,	263.03683471679687500000}, //B: slope, intercept
		{0.87281060218811035156,	624.04595947265625000000}, //Gb: slope, intercept
		{0.81407910585403442383,	666.59527587890625000000}, //Gr: slope, intercept
		{0.87593811750411987305,	608.70275878906250000000}, //R: slope, intercept
	},
} };

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {239, 239, 239, 239, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1087, 1087, 1087, 1087
#endif
		},
		.stAuto = {
			{239, 240, 240, 241, 241, 241, 241, 244, /*8*/244, 252, 290, 395, 683, 1023, 1023, 1023},
			{239, 240, 240, 241, 241, 241, 241, 244, /*8*/244, 252, 290, 395, 683, 1023, 1023, 1023},
			{239, 240, 240, 241, 241, 241, 241, 244, /*8*/244, 252, 290, 395, 683, 1023, 1023, 1023},
			{239, 240, 240, 241, 241, 241, 241, 244, /*8*/243, 252, 290, 395, 681, 1023, 1023, 1023},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1087, 1088, 1088, 1088, 1088, 1088, 1088, 1089,
				/*8*/1089, 1091, 1102, 1133, 1229, 1365, 1365, 1365},
			{1087, 1088, 1088, 1088, 1088, 1088, 1088, 1089,
				/*8*/1089, 1091, 1102, 1133, 1229, 1365, 1365, 1365},
			{1087, 1088, 1088, 1088, 1088, 1088, 1088, 1089,
				/*8*/1089, 1091, 1102, 1133, 1229, 1365, 1365, 1365},
			{1087, 1088, 1088, 1088, 1088, 1088, 1088, 1089,
				/*8*/1089, 1091, 1102, 1133, 1228, 1365, 1365, 1365},
#endif
		},
	},
};

struct combo_dev_attr_s imx307_2l_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_12BIT,
		.lane_id = {2, 1, 0, -1, -1},
		.pn_swap = {1, 1, 1, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_DOL,
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_37P125M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __IMX307_2L_CMOS_PARAM_H_ */
