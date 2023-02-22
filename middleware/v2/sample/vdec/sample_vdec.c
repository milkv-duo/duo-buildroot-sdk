#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "sample_vdec_lib.h"
#include "sample_vdec_testcase.h"

#define MAX_FILENAME_LEN	64

#if defined(__GNUC__) && defined(__riscv)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation=" /* Or  "-Wformat-overflow"  */
#endif

static CVI_S32 SAMPLE_VDEC(sampleVdec *psvdec);
static CVI_BOOL isResetChnTestMode(vdecInputCfg *pic);

int main(int argc, char **argv)
{
	sampleVdec sv, *psvdec = &sv;
	vdecInputCfg *pic = &psvdec->inputCfg;
	CVI_S32 s32Ret = CVI_SUCCESS;

	printf("%s\n", argv[0]);

	s32Ret = parseDecArgv(pic, argc, argv);
	if (s32Ret < 0) {
		if (s32Ret == STATUS_HELP)
			return CVI_SUCCESS;

		CVI_VDEC_ERR("parseDecArgv\n");
		return CVI_FAILURE;
	}

	signal(SIGINT, SAMPLE_VDEC_HandleSig);
	signal(SIGTERM, SAMPLE_VDEC_HandleSig);

	if (isResetChnTestMode(pic)) {
		pic->u32NumAllChns = 1;
	}

	if (!isResetChnTestMode(pic) && checkInputConfig(pic) < 0) {
		CVI_VDEC_ERR("checkInputConfig\n");
		printVdecHelp(argv);
		return CVI_FAILURE;
	}

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
	vdecInputCfg *pic = &psvdec->inputCfg;

	s32Ret = SAMPLE_VDEC_INIT_VB(psvdec);
	if (s32Ret == STAT_ERR_VDEC_INIT_ATTR) {
		CVI_VDEC_ERR("STAT_ERR_VDEC_INIT_ATTR\n");
		return s32Ret;
	} else if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("INIT_VB FAIL\n");
		goto END2;
	}

	if (isResetChnTestMode(pic)) {
		vdecChnInputCfg *pTestIc;
		int num_testcase;
		vdecChnInputCfg *pvdcic;
		CVI_U8 u8ChnIdx = 0;

		pTestIc = vdecGetInputCfgTestcase(pic->u32TestMode);
		num_testcase = vdecGetNumTestcase(pic->u32TestMode);
		pvdcic = &pic->chnInCfg[u8ChnIdx];

		for (int idx = 0; idx < num_testcase; idx++) {
			char bitStreamFilename[MAX_FILENAME_LEN];

			memset(pvdcic->codec, 0, sizeof(char) * 64);
			memset(pvdcic->input_path, 0, sizeof(char) * MAX_STRING_LEN);
			memset(pvdcic->output_path, 0, sizeof(char) * MAX_STRING_LEN);
			memcpy(pvdcic, &pTestIc[idx], sizeof(vdecChnInputCfg));

			snprintf(bitStreamFilename, MAX_STRING_LEN, "%s", pvdcic->input_path);

			if (!strcmp(pvdcic->codec, "265")) {
				pvdcic->enType = PT_H265;
			} else if (!strcmp(pvdcic->codec, "264")) {
				pvdcic->enType = PT_H264;
			} else if (!strcmp(pvdcic->codec, "jpg")) {
				pvdcic->enType = PT_JPEG;
			} else if (!strcmp(pvdcic->codec, "mjp")) {
				pvdcic->enType = PT_MJPEG;
			}
			psvdec->chnCtx[u8ChnIdx].stSampleVdecAttr.enType = pvdcic->enType;
			snprintf(pvdcic->input_path, MAX_STRING_LEN, "%s%s",
				 pic->bitStreamFolder, bitStreamFilename);

			s32Ret = checkInputConfig(pic);
			if (s32Ret < 0) {
				CVI_VDEC_ERR("checkInputConfig failure\n");
				return s32Ret;
			}

			s32Ret = SAMPLE_VDEC_START(psvdec);
			if (s32Ret == STAT_ERR_VDEC_COMMON_START) {
				CVI_VDEC_ERR("STAT_ERR_VDEC_INIT_ATTR\n");
				goto END3;
			} else if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("SAMPLE_VDEC_START\n");
				goto END2;
			}

			SAMPLE_VDEC_STOP(psvdec);

			CVI_VDEC_INFO("case idx %d done(total %d)\n", (idx + 1), num_testcase);
		}
		psvdec->chnCtx[u8ChnIdx].bCreateChn = CVI_FALSE;
	} else {
		s32Ret = SAMPLE_VDEC_START(psvdec);

		if (s32Ret == STAT_ERR_VDEC_COMMON_START) {
			CVI_VDEC_ERR("STAT_ERR_VDEC_COMMON_START\n");
			goto END3;
		} else if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_ERR("SAMPLE_VDEC_START\n");
			return s32Ret;
		}
		SAMPLE_VDEC_STOP(psvdec);
	}

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

static CVI_BOOL isResetChnTestMode(vdecInputCfg *pic)
{
	return pic->u32TestMode == H264_CONTI_DECODE_MODE ||
		pic->u32TestMode == H265_CONTI_DECODE_MODE ||
		pic->u32TestMode == JPEG_CONTI_DECODE_MODE;
}

#if defined(__GNUC__) && defined(__riscv)
#pragma GCC diagnostic pop
#endif