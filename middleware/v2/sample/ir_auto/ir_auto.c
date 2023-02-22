
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
#include <signal.h>

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

#define SAMPLE_IR_CALIBRATION_MODE 0
#define SAMPLE_IR_AUTO_MODE        1

static VI_PIPE ViPipe;
static CVI_BOOL g_bEnableRun;

static SAMPLE_VI_CONFIG_S g_stViConfig;
static SAMPLE_INI_CFG_S g_stIniCfg;

static int sys_vi_init(void)
{
	MMF_VERSION_S stVersion;
	SAMPLE_INI_CFG_S stIniCfg;
	SAMPLE_VI_CONFIG_S stViConfig;

	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	CVI_SYS_GetVersion(&stVersion);
	SAMPLE_PRT("MMF Version:%s\n", stVersion.version);

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg)) {
		SAMPLE_PRT("Parse complete\n");
	}

	//Set sensor number
	CVI_VI_SetDevNum(stIniCfg.devNum);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	memcpy(&g_stViConfig, &stViConfig, sizeof(SAMPLE_VI_CONFIG_S));
	memcpy(&g_stIniCfg, &stIniCfg, sizeof(SAMPLE_INI_CFG_S));

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init modules
	 ************************************************/
	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

static void sys_vi_deinit(void)
{
	SAMPLE_COMM_VI_DestroyIsp(&g_stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&g_stViConfig);

	SAMPLE_COMM_SYS_Exit();
}

static void signal_handler(int signo)
{
	if (g_bEnableRun) {
		signal(signo, SIG_IGN);
		g_bEnableRun = CVI_FALSE;
	} else {
		exit(-1);
	}
}

static void print_usage(char *sPrgNm)
{
	printf("Usage : %s <mode> <u32Normal2IrIsoThr> <u32Ir2NormalIsoThr> ", sPrgNm);
	printf("<u32RGMax> <u32RGMin> <u32BGMax> <u32BGMin> <enIrStatus>\n");

	printf("mode:\n");
	printf("\t 0) SAMPLE_IR_CALIBRATION_MODE.\n");
	printf("\t 1) SAMPLE_IR_AUTO_MODE.\n");

	printf("u32Normal2IrIsoThr:\n");
	printf("\t ISO threshold of switching from normal to IR mode.\n");

	printf("u32Ir2NormalIsoThr:\n");
	printf("\t ISO threshold of switching from IR to normal mode.\n");

	printf("u32RGMax/u32RGMin/u32BGMax/u32BGMin:\n");
	printf("\t Maximum(Minimum) value of R/G(B/G) in IR scene.\n");

	printf("enIrStatus:\n");
	printf("\t Current IR status. 0: Normal mode; 1: IR mode.\n");

	printf("e.g : %s 0 (SAMPLE_IR_CALIBRATION_MODE)\n", sPrgNm);
	printf("e.g : %s 1 16000 400 280 190 280 190 0 (SAMPLE_IR_AUTO_MODE, user_define parameters)\n", sPrgNm);
}

static void switch_to_ir(void)
{
	// 1. switch pq BIN
	// 2. switch ir cut

	printf("\nNormal --> IR\n");
}

static void switch_to_normal(void)
{
	// 1. switch pq BIN
	// 2. switch ir cut

	printf("\nIR --> Normal\n");
}

static void get_ae_awb_info(CVI_U32 *u32ISO, CVI_U32 *u32RGgain, CVI_U32 *u32BGgain)
{
#define IR_WB_GAIN_FORMAT	256
#define IR_DIV_0_TO_1(a)	((0 == (a)) ? 1 : (a))

	ISP_EXP_INFO_S aeInfo;
	CVI_U16 grayWorldRgain, grayWorldBgain;

	CVI_ISP_QueryExposureInfo(ViPipe, &aeInfo);
	CVI_ISP_GetGrayWorldAwbInfo(ViPipe, &grayWorldRgain, &grayWorldBgain);

	*u32ISO = aeInfo.u32ISO;
	*u32RGgain = IR_WB_GAIN_FORMAT * 1024 / IR_DIV_0_TO_1(grayWorldRgain);
	*u32BGgain = IR_WB_GAIN_FORMAT * 1024 / IR_DIV_0_TO_1(grayWorldBgain);
}

static int run_calibration(int argc, char **argv)
{
#define GAIN_MAX_COEF 280
#define GAIN_MIN_COEF 190

	CVI_U32 u32ISO;
	CVI_U32 RGgain, BGgain;

	UNUSED(argc);
	UNUSED(argv);

	switch_to_ir();

	while (g_bEnableRun) {

		get_ae_awb_info(&u32ISO, &RGgain, &BGgain);

		printf("\n");
		printf("ISO: %d, RGgain: %d, BGgain: %d\n", u32ISO, RGgain, BGgain);
		printf("Reference range: RGMax: %d, RGMin: %d, BGMax: %d, BGMin: %d\n",
			(RGgain * GAIN_MAX_COEF) >> 8, (RGgain * GAIN_MIN_COEF) >> 8,
			(BGgain * GAIN_MAX_COEF) >> 8, (BGgain * GAIN_MIN_COEF) >> 8);

		DELAY_500MS();
	}

	return CVI_SUCCESS;
}

#define ENABLE_RUN_IR_AUTO_DEBUG

#ifdef ENABLE_RUN_IR_AUTO_DEBUG
static void print_ae_awb_info(void)
{
	CVI_U32 u32ISO;
	CVI_U32 RGgain, BGgain;

	get_ae_awb_info(&u32ISO, &RGgain, &BGgain);
	printf("Current, ISO: %d, RGgain: %d, BGgain: %d\n", u32ISO, RGgain, BGgain);
}
#endif

static int run_ir_auto(int argc, char **argv)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

#define STABLE_COUNT_THR   3
	CVI_U8 u8StableCount = 0;

	ISP_IR_AUTO_ATTR_S stIrAttr;

	stIrAttr.bEnable = CVI_TRUE;

	if (argc > 7) {
		stIrAttr.u32Normal2IrIsoThr = atoi(argv[2]);
		stIrAttr.u32Ir2NormalIsoThr = atoi(argv[3]);
		stIrAttr.u32RGMax = atoi(argv[4]);
		stIrAttr.u32RGMin = atoi(argv[5]);
		stIrAttr.u32BGMax = atoi(argv[6]);
		stIrAttr.u32BGMin = atoi(argv[7]);
		stIrAttr.enIrStatus = atoi(argv[8]);

		if (stIrAttr.enIrStatus != ISP_IR_STATUS_NORMAL &&
			stIrAttr.enIrStatus != ISP_IR_STATUS_IR) {
			printf("the mode is invalid!\n");
			goto exit;
		}
	} else {
		printf("Invalid parameter!\n");
		goto exit;
	}

	while (g_bEnableRun) {

#ifdef ENABLE_RUN_IR_AUTO_DEBUG
		printf("\n");
		printf("input, u32Normal2IrIsoThr: %d, u32Ir2NormalIsoThr: %d, ",
			stIrAttr.u32Normal2IrIsoThr,
			stIrAttr.u32Ir2NormalIsoThr);
		printf("RG: %d - %d, BG: %d - %d, enIrStatus: %d\n",
			stIrAttr.u32RGMax,
			stIrAttr.u32RGMin,
			stIrAttr.u32BGMax,
			stIrAttr.u32BGMin,
			stIrAttr.enIrStatus);
		print_ae_awb_info();
#endif

		s32Ret = CVI_ISP_IrAutoRunOnce(ViPipe, &stIrAttr);

#ifdef ENABLE_RUN_IR_AUTO_DEBUG
		printf("enIrSwitch: %d, u8StableCount: %d\n", stIrAttr.enIrSwitch, u8StableCount);
#endif

		if (stIrAttr.enIrSwitch == ISP_IR_SWITCH_TO_IR &&
			u8StableCount++ > STABLE_COUNT_THR) {

			switch_to_ir();

			stIrAttr.enIrStatus = ISP_IR_STATUS_IR;

		} else if (stIrAttr.enIrSwitch == ISP_IR_SWITCH_TO_NORMAL &&
			u8StableCount++ > STABLE_COUNT_THR) {

			switch_to_normal();

			stIrAttr.enIrStatus = ISP_IR_STATUS_NORMAL;

		}

		if (stIrAttr.enIrSwitch == ISP_IR_SWITCH_NONE) {
			u8StableCount = 0;
		}

		DELAY_500MS();
	}

	return s32Ret;

exit:
	print_usage(argv[0]);
	return CVI_FAILURE;
}

int main(int argc, char **argv)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32Mode = SAMPLE_IR_CALIBRATION_MODE;

	if (argc < 2) {
		print_usage(argv[0]);
		return CVI_FAILURE;
	}

	u32Mode = atoi(argv[1]);

	if (u32Mode != SAMPLE_IR_CALIBRATION_MODE &&
		u32Mode != SAMPLE_IR_AUTO_MODE) {
		printf("the mode is invalid!\n");
		print_usage(argv[0]);
		return CVI_FAILURE;
	}

	s32Ret = sys_vi_init();
	if (s32Ret != CVI_SUCCESS) {
		printf("sys vi init failed!\n");
		return s32Ret;
	}

	ViPipe = 0;
	g_bEnableRun = CVI_TRUE;

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	if (u32Mode == SAMPLE_IR_CALIBRATION_MODE) {
		s32Ret = run_calibration(argc, argv);
	} else if (u32Mode == SAMPLE_IR_AUTO_MODE) {
		s32Ret = run_ir_auto(argc, argv);
	}

	sys_vi_deinit();

	return s32Ret;
}

