#ifndef __IMX327_FPGA_CMOS_PARAM_FPGA_H_
#define __IMX327_FPGA_CMOS_PARAM_FPGA_H_

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
#include "imx327_fpga_cmos_ex.h"

static const IMX327_FPGA_MODE_S g_astImx327_fpga_mode[IMX327_FPGA_MODE_NUM] = {
	[IMX327_FPGA_MODE_720P30] = {
		.name = "720p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stMaxSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
		},
		.f32MaxFps = 10,
		.f32MinFps = 0.09, /* 750 *30 / 0x3FFFF */
		.u32HtsDef = 0x4D58,
		.u32VtsDef = 750,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 750,
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
		.u16RHS1 = 9,
		.u16BRL = 735,
		.u16OpbSize = 4,
		.u16MarginVtop = 4,
		.u16MarginVbot = 5,
	},
	[IMX327_FPGA_MODE_720P30_WDR] = {
		.name = "720p30wdr",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stMaxSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
		},
		.astImg[1] = {
			.stSnsSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
			.stWndRect = {
				.s32X = 12,
				.s32Y = 8,
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stMaxSize = {
				.u32Width = 1308,
				.u32Height = 729,
			},
		},
		.f32MaxFps = 5,
		.f32MinFps = 0.09, /* 750 *30 / 0x3FFFF */
		.u32HtsDef = 0x4D58,
		.u32VtsDef = 750,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 750,
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
		.stExp[1] = {
			.u16Min = 1,
			.u16Max = 750,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[1] = {
			.u16Min = 1024,
			.u16Max = 62416,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[1] = {
			.u16Min = 1024,
			.u16Max = 38485,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.u16RHS1 = 9,
		.u16BRL = 735,
		.u16OpbSize = 4,
		.u16MarginVtop = 4,
		.u16MarginVbot = 5,
	},
	[IMX327_FPGA_MODE_1080P30] = {
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
	[IMX327_FPGA_MODE_1080P30_WDR] = {
		.name = "1080p30wdr",
		/* sef */
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1952,
				.u32Height = 1114,
			},
			.stWndRect = {
				.s32X = 16,
				.s32Y = 20,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1952,
				.u32Height = 1114,
			},
		},
		/* lef */
		.astImg[1] = {
			.stSnsSize = {
				.u32Width = 1952,
				.u32Height = 1114,
			},
			.stWndRect = {
				.s32X = 16,
				.s32Y = 15,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1952,
				.u32Height = 1114,
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

// F1.8 Lens
#ifdef F18
static ISP_CMOS_NOISE_CALIBRATION_S g_stIspNoiseCalibratio = {
    /*************Calibration LUT Table*************/
	.CalibrationCoef = {
		{	//iso 100
			{0.03243722766637802,	3.855583906173706},	//B: slope, intercept
			{0.024103811010718346,	3.9538209438323975},	//Gb: slope, intercept
			{0.022307759150862694,	4.970882892608643},	//Gr: slope, intercept
			{0.02794046327471733,	3.1342978477478027},	//R: slope, intercept
		},
		{	//iso 200
			{0.03799639642238617,	5.526010990142822},	//B: slope, intercept
			{0.02662081830203533,	7.61380672454834},	//Gb: slope, intercept
			{0.025447502732276917,	8.231551170349121},	//Gr: slope, intercept
			{0.032776281237602234,	5.149646282196045},	//R: slope, intercept
		},
		{	//iso 400
			{0.050715841352939606,	7.763713836669922},	//B: slope, intercept
			{0.032093651592731476,	12.182195663452148},	//Gb: slope, intercept
			{0.030688125640153885,	12.43420696258545},	//Gr: slope, intercept
			{0.040235333144664764,	8.544824600219727},	//R: slope, intercept
		},
		{	//iso 800
			{0.06888508796691895,	10.970534324645996},	//B: slope, intercept
			{0.03813415393233299,	19.28471565246582},	//Gb: slope, intercept
			{0.03805641457438469,	19.625152587890625},	//Gr: slope, intercept
			{0.04966627061367035,	12.69924545288086},	//R: slope, intercept
		},
		{	//iso 1600
			{0.08884920179843903,	15.890214920043945},	//B: slope, intercept
			{0.046779610216617584,	30.772066116333008},	//Gb: slope, intercept
			{0.045214589685201645,	31.489530563354492},	//Gr: slope, intercept
			{0.06512749195098877,	20.34792137145996},	//R: slope, intercept
		},
		{	//iso 3200
			{0.1241341084241867,	20.70075225830078},	//B: slope, intercept
			{0.05977431312203407,	45.98811721801758},	//Gb: slope, intercept
			{0.05858884006738663,	47.11114501953125},	//Gr: slope, intercept
			{0.08636551350355148,	30.561227798461914},	//R: slope, intercept
		},
		{	//iso 6400
			{0.1791478991508484,	30.803356170654297},	//B: slope, intercept
			{0.09205856174230576,	65.60118103027344},	//Gb: slope, intercept
			{0.08692053705453873,	69.85540771484375},	//Gr: slope, intercept
			{0.12406758964061737,	45.617835998535156},	//R: slope, intercept
		},
		{	//iso 12800
			{0.2464703917503357,	44.136966705322266},	//B: slope, intercept
			{0.11594483256340027,	97.8382797241211},	//Gb: slope, intercept
			{0.11075824499130249,	101.47943115234375},	//Gr: slope, intercept
			{0.17024046182632446,	66.47021484375},	//R: slope, intercept
		},
		{	//iso 25600
			{0.3376912772655487,	68.80254364013672},	//B: slope, intercept
			{0.15025299787521362,	145.10032653808594},	//Gb: slope, intercept
			{0.14386454224586487,	148.1698455810547},	//Gr: slope, intercept
			{0.23965470492839813,	96.42337036132812},	//R: slope, intercept
		},
		{	//iso 51200
			{0.4279725253582001,	117.4070816040039},	//B: slope, intercept
			{0.17134547233581543,	243.6494598388672},	//Gb: slope, intercept
			{0.16668814420700073,	241.21603393554688},	//Gr: slope, intercept
			{0.29862311482429504,	156.05364990234375},	//R: slope, intercept
		},
		{	//iso 102400
			{0.534967303276062,	181.45118713378906},	//B: slope, intercept
			{0.2024478018283844,	365.00177001953125},	//Gb: slope, intercept
			{0.20520392060279846,	362.1013488769531},	//Gr: slope, intercept
			{0.40533769130706787,	241.33485412597656},	//R: slope, intercept
		},
		{	//iso 204800
			{0.6578512191772461,	286.16363525390625},	//B: slope, intercept
			{0.223756805062294,	551.998779296875},	//Gb: slope, intercept
			{0.22110669314861298,	562.9724731445312},	//Gr: slope, intercept
			{0.4937806725502014,	371.5768737792969},	//R: slope, intercept
		},
		{	//iso 409600
			{0.5731459259986877,	525.5570678710938},	//B: slope, intercept
			{0.11877097189426422,	962.0527954101562},	//Gb: slope, intercept
			{0.13799689710140228,	933.2637329101562},	//Gr: slope, intercept
			{0.3682960867881775,	696.024658203125},	//R: slope, intercept
		},
		//-------------------------------------------------------------
		{	//iso 819200
			{0.5731459259986877,	525.5570678710938},	//B: slope, intercept
			{0.11877097189426422,	962.0527954101562},	//Gb: slope, intercept
			{0.13799689710140228,	933.2637329101562},	//Gr: slope, intercept
			{0.3682960867881775,	696.024658203125},	//R: slope, intercept
		},
		{	//iso 1638400
			{0.5731459259986877,	525.5570678710938},	//B: slope, intercept
			{0.11877097189426422,	962.0527954101562},	//Gb: slope, intercept
			{0.13799689710140228,	933.2637329101562},	//Gr: slope, intercept
			{0.3682960867881775,	696.024658203125},	//R: slope, intercept
		},
		{	//iso 3276800
			{0.5731459259986877,	525.5570678710938},	//B: slope, intercept
			{0.11877097189426422,	962.0527954101562},	//Gb: slope, intercept
			{0.13799689710140228,	933.2637329101562},	//Gr: slope, intercept
			{0.3682960867881775,	696.024658203125},	//R: slope, intercept
		},
	}
    /*********************************************/
};
#endif

// F1.0 Lens
#ifdef F10
static ISP_CMOS_NOISE_CALIBRATION_S g_stIspNoiseCalibratio = {.CalibrationCoef = {
	{	//iso  100
		{0.038478557020425796517,	2.14561891555786132813}, //B: slope, intercept
		{0.03633839637041091919,	0.17788629233837127686}, //Gb: slope, intercept
		{0.03592018783092498779,	0.21645727753639221191}, //Gr: slope, intercept
		{0.03592239692807197571,	1.48806631565093994141}, //R: slope, intercept
	},
	{	//iso  200
		{0.04168356582522392273,	4.38879251480102539063}, //B: slope, intercept
		{0.03754633292555809021,	3.21502065658569335938}, //Gb: slope, intercept
		{0.03759334236383438110,	3.48370814323425292969}, //Gr: slope, intercept
		{0.03719408065080642700,	4.63629007339477539063}, //R: slope, intercept
	},
	{	//iso  400
		{0.05050259083509445190,	6.85188436508178710938}, //B: slope, intercept
		{0.04007886722683906555,	8.49916744232177734375}, //Gb: slope, intercept
		{0.03934165090322494507,	8.68585968017578125000}, //Gr: slope, intercept
		{0.04134147986769676208,	8.89877605438232421875}, //R: slope, intercept
	},
	{	//iso  800
		{0.04306267201900482178,	15.41853904724121093750}, //B: slope, intercept
		{0.03570587188005447388,	18.56583213806152343750}, //Gb: slope, intercept
		{0.03533876314759254456,	19.57685661315917968750}, //Gr: slope, intercept
		{0.03832129389047622681,	17.90320968627929687500}, //R: slope, intercept
	},
	{	//iso  1600
		{0.06225715205073356628,	21.87150573730468750000}, //B: slope, intercept
		{0.04437549784779548645,	30.87298965454101562500}, //Gb: slope, intercept
		{0.04517432302236557007,	29.32084465026855468750}, //Gr: slope, intercept
		{0.05255207046866416931,	26.26001358032226562500}, //R: slope, intercept
	},
	{	//iso  3200
		{0.07859147340059280396,	32.48978424072265625000}, //B: slope, intercept
		{0.05918205901980400085,	45.08999633789062500000}, //Gb: slope, intercept
		{0.05711782723665237427,	48.86173248291015625000}, //Gr: slope, intercept
		{0.07310666143894195557,	36.88227081298828125000}, //R: slope, intercept
	},
	{	//iso  6400
		{0.11759266257286071777,	45.81345367431640625000}, //B: slope, intercept
		{0.07679814100265502930,	69.04325866699218750000}, //Gb: slope, intercept
		{0.07774948328733444214,	66.52905273437500000000}, //Gr: slope, intercept
		{0.10286796092987060547,	51.73817443847656250000}, //R: slope, intercept
	},
	{	//iso  12800
		{0.15860086679458618164,	64.15329742431640625000}, //B: slope, intercept
		{0.10511043667793273926,	91.36695098876953125000}, //Gb: slope, intercept
		{0.10040879994630813599,	92.05632781982421875000}, //Gr: slope, intercept
		{0.13499946892261505127,	73.36162567138671875000}, //R: slope, intercept
	},
	{	//iso  25600
		{0.22810073196887969971,	100.01741027832031250000}, //B: slope, intercept
		{0.14736327528953552246,	146.62678527832031250000}, //Gb: slope, intercept
		{0.15503610670566558838,	136.06079101562500000000}, //Gr: slope, intercept
		{0.20250190794467926025,	112.74269104003906250000}, //R: slope, intercept
	},
	{	//iso  51200
		{0.29010805487632751465,	155.05882263183593750000}, //B: slope, intercept
		{0.20221179723739624023,	210.97210693359375000000}, //Gb: slope, intercept
		{0.21603405475616455078,	201.40393066406250000000}, //Gr: slope, intercept
		{0.27819159626960754395,	165.48959350585937500000}, //R: slope, intercept
	},
	{	//iso  102400
		{0.40437409281730651855,	245.11883544921875000000}, //B: slope, intercept
		{0.26703724265098571777,	345.62698364257812500000}, //Gb: slope, intercept
		{0.27536261081695556641,	318.75701904296875000000}, //Gr: slope, intercept
		{0.44441288709640502930,	222.47651672363281250000}, //R: slope, intercept
	},
	{	//iso  204800
		{0.48959052562713623047,	383.21560668945312500000}, //B: slope, intercept
		{0.37694901227951049805,	442.55813598632812500000}, //Gb: slope, intercept
		{0.38724529743194580078,	449.72384643554687500000}, //Gr: slope, intercept
		{0.60220617055892944336,	318.92291259765625000000}, //R: slope, intercept
	},
	{	//iso  409600
		{0.59899300336837768555,	580.43414306640625000000}, //B: slope, intercept
		{0.43681395053863525391,	700.22204589843750000000}, //Gb: slope, intercept
		{0.50216537714004516602,	633.89794921875000000000}, //Gr: slope, intercept
		{0.77917146682739257813,	453.13076782226562500000}, //R: slope, intercept
	},
	{	//iso  819200
		{0.54956322908401489258,	871.06585693359375000000}, //B: slope, intercept
		{0.38088488578796386719,	1040.72192382812500000000}, //Gb: slope, intercept
		{0.47245877981185913086,	937.37445068359375000000}, //Gr: slope, intercept
		{0.70387065410614013672,	730.45373535156250000000}, //R: slope, intercept
	},
	{	//iso  1638400
		{0.67512661218643188477,	797.11108398437500000000}, //B: slope, intercept
		{0.50457936525344848633,	962.40740966796875000000}, //Gb: slope, intercept
		{0.59857457876205444336,	856.90533447265625000000}, //Gr: slope, intercept
		{0.74725526571273803711,	740.04290771484375000000}, //R: slope, intercept
	},
	{	//iso  3276800
		{0.79927802085876464844,	687.07183837890625000000}, //B: slope, intercept
		{0.81719654798507690430,	687.01580810546875000000}, //Gb: slope, intercept
		{0.83395677804946899414,	658.91857910156250000000}, //Gr: slope, intercept
		{0.72803622484207153320,	746.19427490234375000000}, //R: slope, intercept
	},
} };
#endif

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {240, 240, 240, 240, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1088, 1088, 1088, 1088
#endif
		},
		.stAuto = {
			{241, 241, 241, 241, 239, 239, 239, 238, /*8*/229, 225, 273, 355, 611, 1023, 1023, 1023},
			{241, 241, 241, 241, 239, 239, 239, 238, /*8*/229, 225, 273, 355, 611, 1023, 1023, 1023},
			{241, 241, 241, 241, 239, 239, 239, 238, /*8*/229, 225, 273, 355, 611, 1023, 1023, 1023},
			{241, 241, 241, 241, 239, 239, 239, 238, /*8*/229, 225, 273, 355, 611, 1023, 1023, 1023},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1089, 1089, 1089, 1089, 1088, 1088, 1088, 1088,
				/*8*/1085, 1084, 1098, 1122, 1204, 1365, 1365, 1365},
			{1089, 1089, 1089, 1089, 1088, 1088, 1088, 1088,
				/*8*/1085, 1084, 1098, 1122, 1204, 1365, 1365, 1365},
			{1089, 1089, 1089, 1089, 1088, 1088, 1088, 1088,
				/*8*/1085, 1084, 1098, 1122, 1204, 1365, 1365, 1365},
			{1089, 1089, 1089, 1089, 1088, 1088, 1088, 1088,
				/*8*/1085, 1084, 1098, 1122, 1204, 1365, 1365, 1365},
#endif
		},
	},
};


struct combo_dev_attr_s imx327_fpga_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {0, 1, 2, -1, -1},
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


#endif /* __IMX327_FPGA_CMOS_PARAM_H_ */
