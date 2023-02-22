#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "sample_venc_lib.h"
#include "sample_vdec_lib.h"

static CVI_S32 findVenc(int argc, char **argv, CVI_S32 *pVEncIdx, CVI_S32 *pVDecIdx);
static void print_help(char * const *argv);
static CVI_S32 SAMPLE_VCODEC_START_VENC(sampleVenc *psv, int argc, char **argv);
static CVI_S32 SAMPLE_VCODEC_STOP_VENC(sampleVenc *psv);
static CVI_S32 SAMPLE_VCODEC_START_VDEC(sampleVdec *psvdec, int argc, char **argv);
static CVI_S32 SAMPLE_VDEC(sampleVdec *psvdec);
static CVI_S32 SAMPLE_VCODEC_STOP_VDEC(sampleVdec *psvdec);

CVI_S32 main(int argc, char **argv)
{
	sampleVenc sv, *psv = &sv;
	sampleVdec svdec, *psvdec = &svdec;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 vencIdx, vdecIdx;
	CVI_S32 vencEn, vdecEn;
	CVI_S32 vencStat, vdecStat;
	CVI_S32 currArgc;

	s32Ret = findVenc(argc, argv, &vencIdx, &vdecIdx);
	if (s32Ret < 0) {
		print_help(argv);
		return s32Ret;
	}

	vencEn = vencIdx >= 0;
	vdecEn = vdecIdx >= 0;
	vencStat = vencEn;
	vdecStat = vdecEn;

	if (vencEn) {
		if (vdecEn) {
			currArgc = vdecIdx - vencIdx;
		} else {
			currArgc = argc - 1;
		}

		s32Ret = SAMPLE_VCODEC_START_VENC(psv, currArgc, &argv[vencIdx]);
		if (s32Ret < 0) {
			CVI_VENC_ERR("vcodec config failed\n");
			vencStat = 0;
		}
	}

	if (vdecEn) {
		if (vencEn) {
			currArgc = argc - vdecIdx;
		} else {
			currArgc = argc - 1;
		}

		s32Ret = SAMPLE_VCODEC_START_VDEC(psvdec, currArgc, &argv[vdecIdx]);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_ERR("SAMPLE_VCODEC_START_VDEC\n");
			vdecStat = 0;
		}
	}

	if (vencStat)
		SAMPLE_VCODEC_STOP_VENC(psv);

	if (vdecStat)
		SAMPLE_VCODEC_STOP_VDEC(psvdec);

	return 0;
}

static CVI_S32 findVenc(int argc, char **argv, CVI_S32 *pVEncIdx, CVI_S32 *pVDecIdx)
{
	CVI_S32 idx;

	*pVEncIdx = -1;
	*pVDecIdx = -1;
	for (idx = 0; idx < argc; idx++) {
		if (strcmp(argv[idx], "sample_venc") == 0)
			*pVEncIdx = idx;

		if (strcmp(argv[idx], "sample_vdec") == 0)
			*pVDecIdx = idx;
	}

	if (*pVEncIdx >= 1 && *pVDecIdx >= 1) {
		if (*pVEncIdx >= *pVDecIdx) {
			CVI_VENC_ERR("param\n");
			return -1;
		}
	} else if (*pVEncIdx <= 0 && *pVDecIdx <= 0) {
		CVI_VENC_ERR("param\n");
		return -1;
	}

	return 0;
}

static void print_help(char * const *argv)
{
	printf("// ------------------------------------------------\n");
	printf("// %s sample_venc [enc-params ...]\n", argv[0]);
	printf("// %s sample_vdec [dec-params ...]\n", argv[0]);
	printf("// %s sample_venc [enc-params ...] sample_vdec [dec-params ...]\n", argv[0]);
	printf("EX.\n");
	printf("%s sample_venc -c 264 --gop=5 -w 352 -h 288 -i in.yuv -o out_bs\n", argv[0]);
	printf("%s sample_vdec -c 264 -i in.264 -o decoded.yuv\n", argv[0]);
	printf("%s sample_venc -c 264 --gop=5 -w 352 -h 288 -i in.yuv -o out_bs", argv[0]);
	printf("sample_vdec -c 264 -i in.264 -o decoded.yuv\n");
	printf("// ------------------------------------------------\n");
}

static CVI_S32 SAMPLE_VCODEC_START_VENC(sampleVenc *psv, int argc, char **argv)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_VENC_INIT_CFG(psv, argc, argv);
	if (s32Ret < 0) {
		CVI_VENC_ERR("SAMPLE_VENC_INIT_CFG\n");
		return s32Ret;
	}

	s32Ret = SAMPLE_VENC_START(psv);
	if (s32Ret < 0) {
		CVI_VENC_ERR("SAMPLE_VENC_START\n");
		return s32Ret;
	}

	return s32Ret;
}

static CVI_S32 SAMPLE_VCODEC_STOP_VENC(sampleVenc *psv)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_VENC_STOP(psv);
	if (s32Ret < 0) {
		CVI_VENC_ERR("SAMPLE_VENC_STOP\n");
		return s32Ret;
	}

	return s32Ret;
}

static CVI_S32 SAMPLE_VCODEC_START_VDEC(sampleVdec *psvdec, int argc, char **argv)
{
	vdecInputCfg *pic = &psvdec->inputCfg;
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = parseDecArgv(pic, argc, argv);
	if (s32Ret < 0) {
		if (s32Ret == STATUS_HELP)
			return CVI_SUCCESS;

		CVI_VDEC_ERR("parseDecArgv\n");
		return CVI_FAILURE;
	}

	signal(SIGINT, SAMPLE_VDEC_HandleSig);
	signal(SIGTERM, SAMPLE_VDEC_HandleSig);

	if (checkInputConfig(pic) < 0) {
		CVI_VDEC_ERR("checkInputConfig\n");
		printVdecHelp(argv);
		return CVI_FAILURE;
	}

	psvdec->inputCfg.InitedVb = 1;
	s32Ret = SAMPLE_VDEC(psvdec);
	if (s32Ret == CVI_SUCCESS) {
		CVI_VDEC_TRACE("program exit normally!\n");
	} else {
		CVI_VDEC_TRACE("program exit abnormally!\n");
	}
	return s32Ret;
}

static CVI_S32 SAMPLE_VDEC(sampleVdec *psvdec)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_VDEC_INIT_VB(psvdec);
	if (s32Ret == STAT_ERR_VDEC_INIT_ATTR) {
		CVI_VDEC_ERR("STAT_ERR_VDEC_INIT_ATTR\n");
		return s32Ret;
	} else if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("INIT_VB FAIL\n");
		goto END2;
	}

	s32Ret = SAMPLE_VDEC_START(psvdec);
	if (s32Ret == STAT_ERR_VDEC_COMMON_START) {
		CVI_VDEC_ERR("STAT_ERR_VDEC_COMMON_START\n");
		goto END3;
	} else if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("SAMPLE_VDEC_START\n");
		return s32Ret;
	}

	return s32Ret;

END3:
	SAMPLE_COMM_VDEC_Stop(psvdec->u32VdecNumAllChns);

END2:
	SAMPLE_COMM_VDEC_ExitVBPool();
#ifdef VDEC_BIND_MODE
END1:
#endif
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

static CVI_S32 SAMPLE_VCODEC_STOP_VDEC(sampleVdec *psvdec)
{
	SAMPLE_VDEC_STOP(psvdec);
	SAMPLE_COMM_VDEC_Stop(psvdec->u32VdecNumAllChns);
	SAMPLE_COMM_VDEC_ExitVBPool();
	SAMPLE_COMM_SYS_Exit();
	return CVI_SUCCESS;
}
