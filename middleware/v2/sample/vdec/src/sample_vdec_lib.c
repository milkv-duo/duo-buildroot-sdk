
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>

#include "sample_vdec_lib.h"
#include "sample_vdec_testcase.h"
#include "../../venc/include/sample_venc_lib.h"

//#define VDEC_BIND_MODE

#define SAMPLE_STREAM_PATH "./"

#define MAX_VDEC_OPTIONS	128

#define VB_HEIGHT		2304
#define VB_WIDTH		4096
#define VB_MAX_FRM_NUM	32

typedef enum _ARG_TYPE_ {
	ARG_INT = 0,
	ARG_UINT,
	ARG_STRING,
} ARG_TYPE;

typedef struct _optionExt_ {
	struct option opt;
	int type;
	int64_t min;
	int64_t max;
	const char *help;
} optionExt;

typedef union {
	CVI_S32 ival;
	CVI_U32 uval;
} SAMPLE_ARG;

static CVI_S32 vdecInitAttr(
		SAMPLE_VDEC_ATTR * psvdattr,
		vdecChnInputCfg *pvdcic);
static CVI_S32 vdecInitVBPool(sampleVdec *psvdec);
static CVI_S32 vdecInitVBPoolH26X(sampleVdec *psvdec);
static void initVdecThreadParam(
		vdecChnCtx * pvdchnCtx,
		VDEC_THREAD_PARAM_S *pvtp,
		char *path,
		CVI_S32 s32MilliSec);
static CVI_S32 checkArg(CVI_S32 entryIdx, SAMPLE_ARG *pArg);
static void outputMD5Sum(VDEC_THREAD_PARAM_S *pvdtpg);

static optionExt long_option_ext[] = {
	{{"numChn",    optional_argument, NULL, 0},   ARG_UINT,    1,   VDEC_MAX_CHN_NUM,
		"number of channels to decode"},
	{{"chn",       optional_argument, NULL, 0},   ARG_UINT,    0,   VDEC_MAX_CHN_NUM - 1,
		"set channel-id to configure the following parameters"},
	{{"codec",     optional_argument, NULL, 'c'}, ARG_STRING, 0,   0,
		"264 = h.264, 265 = h.265, jpg = jpeg, mjp = motion jpeg" },
	{{"input",     optional_argument, NULL, 'i'}, ARG_STRING, 0,   0,
		"source bitstream"},
	{{"output",    optional_argument, NULL, 'o'}, ARG_STRING, 0,   0,
		"output yuv file"},
	{{"dump",      optional_argument, NULL, 'd'}, ARG_INT,    0,   1,
		"dump yuv for md5sum check"},
	{{"bufwidth",     optional_argument, NULL, 0}, ARG_UINT,      0,   VB_WIDTH,
		"set max width for alloc frame buffer"},
	{{"bufheight",    optional_argument, NULL, 0}, ARG_UINT,      0,   VB_HEIGHT,
		"set max height for alloc frame buffer"},
	{{"maxframe",    optional_argument, NULL, 0}, ARG_UINT,      0,   VB_MAX_FRM_NUM,
		"set max frame buffer number"},
	{{"vbMode",    optional_argument, NULL, 0}, ARG_INT,      0,   VB_SOURCE_BUTT - 1,
		"set vb mode for alloc frame buffer"},
	{{"testMode", optional_argument, NULL, 0},    ARG_UINT,	  0,   (SAMPLE_VDEC_TEST_MODE_MAX - 1),
		"samele_vdec test mode"},
	{{"bitStreamFolder", optional_argument, NULL, 0},   ARG_STRING, 0,   256,
		"bitstream files folder"},
	{{"getframe-timeout", optional_argument, NULL, 0},    ARG_INT,	  -1,   100000,
		"samele_vdec getframe-timeout   -1:block mode, 0:try_once, >0 timeout in ms"},
	{{"sendstream-timeout", optional_argument, NULL, 0},    ARG_INT,	  -1,   100000,
		"samele_vdec sendstream-timeout   -1:block mode, 0:try_once, >0 timeout in ms"},
	{{"bindmode", optional_argument, NULL, 0},	ARG_UINT,	  0,   2,
		"bind mode"},
	{{"pixel_format", optional_argument, NULL, 0},	ARG_INT,	  0,   2,
		"output pixel format. 0: do not specify, 1: NV12, 2: NV21"},
	{{"help",      no_argument, NULL, 'h'},       ARG_STRING, 0,   0,
		"help"},
	{{NULL, 0, NULL, 0}, ARG_INT, 0, 0, ""}
};

#define DEFAULT_MAX_FRM_BUF 4

sampleVenc psv = {0};

CVI_S32 vencStartBindVpss(sampleVdec *psvdec)
{
	commonInputCfg *pcic = &psv.commonIc;
	chnInputCfg *pIc = NULL;
	int ret = 0;
	unsigned int idx = 0;
	unsigned int width, height;
	vdecChnInputCfg *pvdcic = NULL;
	vdecInputCfg *pic = &psvdec->inputCfg;

	pcic->numChn = psvdec->u32VdecNumAllChns;
	psv.commonIc.ifInitVb = 0;

	for (idx = 0; idx < psvdec->u32VdecNumAllChns; idx++) {
		pIc = &psv.chnCtx[idx].chnIc;

		pvdcic = &pic->chnInCfg[idx];

		initInputCfg(pcic, pIc);

		pIc->bsMode = 0;
		pIc->bind_mode = VENC_BIND_VPSS;
		pIc->vpssGrp = idx;
		pIc->vpssChn = 0;

		// VPSS output wxh == 1280x720
		width = 1280;
		height = 720;

		pIc->width = width;
		pIc->height = height;
		SAMPLE_PRT("size = %d x %d\n", width, height);

		sprintf(pIc->output_path, "chn%d_%s_720p", idx, pvdcic->output_path);
		SAMPLE_PRT("output_path = %s\n", pIc->output_path);

		sprintf(pIc->codec, "%s", "mjp"); // now only support mjp
		pIc->rcMode = 4;
		pIc->num_frames = 10;  //decoder stream must be 10 frames
	}

	pcic->ifInitVb = 0;

	ret = SAMPLE_VENC_START(&psv);
	if (ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_VENC_START\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

#define VDEC_BIND_VPSS_VENC_DELAY 60000

CVI_S32 SAMPLE_VDEC_START(sampleVdec *psvdec)
{
	vdecInputCfg *pic = &psvdec->inputCfg;
	vdecChnCtx *pvdchnCtx;
	vdecChnInputCfg *pvdcic = NULL;
	VDEC_THREAD_PARAM_S *pvdtps, *pvdtpg;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 i;

	if (pic->u32BindMode == VDEC_BIND_VPSS_VENC) {
		// Bind mode - Start venc
		s32Ret = vencStartBindVpss(psvdec);
		if (s32Ret != CVI_SUCCESS)
			return CVI_FAILURE;
	}

	for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
		pvdchnCtx = &psvdec->chnCtx[i];
		pvdcic = &pic->chnInCfg[i];
		pvdtps = &pvdchnCtx->stVdecThreadParamSend;
		pvdtpg = &pvdchnCtx->stVdecThreadParamGet;

		s32Ret = SAMPLE_COMM_VDEC_Start(pvdchnCtx);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
			return STAT_ERR_VDEC_COMMON_START;
		}

		if (pic->u32BindMode != VDEC_BIND_DISABLE) {
			SIZE_S stSizeIn, stSizeOut;
			// Bind mode - Init VPSS & VO, Bind VDEC & VPSS & VO

			stSizeIn.u32Width = 1920;
			stSizeIn.u32Height = 1080;
			stSizeOut.u32Width = 1280;
			stSizeOut.u32Height = 720;

			s32Ret = SAMPLE_PLAT_VPSS_INIT(i, stSizeIn, stSizeOut);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("SAMPLE_PLAT_VPSS_INIT fail for %#x!\n", s32Ret);
				return s32Ret;
			}

			if (pic->u32BindMode == VDEC_BIND_VPSS_VENC) {
				SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
			} else {
				s32Ret = SAMPLE_PLAT_VO_INIT();
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("SAMPLE_PLAT_VO_INIT fail for %#x!\n", s32Ret);
					return s32Ret;
				}

				// For VO, now only support channel 0
				CVI_VO_SetChnRotation(0, 0, ROTATION_90);
				SAMPLE_COMM_VPSS_Bind_VO(0, 0, 0, 0);
				SAMPLE_COMM_VDEC_Bind_VPSS(0, 0);
			}
		}

		initVdecThreadParam(
				pvdchnCtx,
				pvdtps,
				pvdcic->input_path,
				pvdcic->s32sendstream_timeout);

		if (pic->u32BindMode == VDEC_BIND_VPSS_VENC) {
			pvdtps->s32IntervalTime = VDEC_BIND_VPSS_VENC_DELAY;
		}

		SAMPLE_COMM_VDEC_StartSendStream(pvdtps, &pvdchnCtx->vdecThreadSend);

		CVI_VDEC_TRACE("i = %d, output_path = %s\n", i, pvdcic->output_path);
		initVdecThreadParam(
				pvdchnCtx,
				pvdtpg,
				pvdcic->output_path,
				pvdcic->s32getframe_timeout);

		pvdtpg->bDumpYUV = pvdcic->bDumpYUV;
		if (pic->u32BindMode == VDEC_BIND_DISABLE) {
			if (pvdcic->bDumpYUV == 0) {
				MD5_Init(&pvdtpg->tMD5Ctx);
			}

			SAMPLE_COMM_VDEC_StartGetPic(pvdtpg, &pvdchnCtx->vdecThreadGet);
		}
	}

	return s32Ret;
}

static CVI_S32 vdecInitAttr(
		SAMPLE_VDEC_ATTR *psvdattr,
		vdecChnInputCfg *pvdcic)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32MaxWidth = 0, u32MaxHeight = 0;
	PAYLOAD_TYPE_E enType = pvdcic->enType;
	PIXEL_FORMAT_E enPixelFormat = (enType == PT_JPEG || enType == PT_MJPEG)
				     ? PIXEL_FORMAT_YUV_PLANAR_444
				     : PIXEL_FORMAT_YUV_PLANAR_420;

	if (pvdcic->s32PixelFormat == 1) {
		enPixelFormat = PIXEL_FORMAT_NV12;
	} else if (pvdcic->s32PixelFormat == 2) {
		enPixelFormat = PIXEL_FORMAT_NV21;
	}

	if (enType == PT_JPEG || enType == PT_MJPEG) {
#if defined(FPGA_PORTING)
		u32MaxWidth = 3840;
		u32MaxHeight = 2160;
#else
		u32MaxWidth = JPEGD_MAX_WIDTH;
		u32MaxHeight = JPEGD_MAX_HEIGHT;
#endif
	} else if (enType == PT_H264) {
		u32MaxWidth = VEDU_H264D_MAX_WIDTH;
		u32MaxHeight = VEDU_H264D_MAX_HEIGHT;
	} else if (enType == PT_H265) {
		u32MaxWidth = VEDU_H265D_MAX_WIDTH;
		u32MaxHeight = VEDU_H265D_MAX_HEIGHT;
	}

	if (pvdcic->u32BufWidth != 0)
		u32MaxWidth = pvdcic->u32BufWidth;
	if (pvdcic->u32BufHeight != 0)
		u32MaxHeight = pvdcic->u32BufHeight;

	psvdattr->enType = enType;
	psvdattr->u32Width = u32MaxWidth;
	psvdattr->u32Height = u32MaxHeight;
	psvdattr->enMode = VIDEO_MODE_FRAME;
	psvdattr->enPixelFormat = enPixelFormat;
	if (enType == PT_JPEG || enType == PT_MJPEG) {
		psvdattr->stSampleVdecPicture.u32Alpha = 255;
	}

	if (enType == PT_JPEG) {
		psvdattr->u32DisplayFrameNum = 0;
		psvdattr->u32FrameBufCnt = 1;
	} else {
		psvdattr->u32DisplayFrameNum = 2;
		psvdattr->u32FrameBufCnt =
			psvdattr->u32DisplayFrameNum + 1;
	}

	return s32Ret;
}

static CVI_S32 vdecInitVBPool(sampleVdec *psvdec)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32BlkSize, i;
	VB_CONFIG_S stVbConf;
	SAMPLE_VDEC_ATTR *psvdattr;
	VB_POOL chnPoolGroup[VDEC_MAX_CHN_NUM] = { [0 ...(VDEC_MAX_CHN_NUM - 1)] = VB_INVALID_POOLID };
	vdecInputCfg *pic = &psvdec->inputCfg;

	i = 0;
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	if (SAMPLE_COMM_VDEC_GetVbMode() == VB_SOURCE_USER && (!psvdec->inputCfg.InitedVb)) {
		if (pic->u32BindMode != VDEC_BIND_DISABLE) {
			// Bind Mode should alloc common pool for VPSS and VO
			CVI_U32 u32BlkSize;
			COMPRESS_MODE_E    enCompressMode	= COMPRESS_MODE_NONE;

			u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, SAMPLE_PIXEL_FORMAT,
				DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);

			stVbConf.u32MaxPoolCnt			 = 1;
			stVbConf.astCommPool[0].u32BlkCnt  = 5;
			stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
			stVbConf.astCommPool[0].enRemapMode = VB_REMAP_MODE_CACHED;
		}

		if (stVbConf.u32MaxPoolCnt == 0 && stVbConf.astCommPool[0].u32BlkSize == 0) {
			stVbConf.u32MaxPoolCnt = 1;
			stVbConf.astCommPool[0].u32BlkCnt = 1;
			 stVbConf.astCommPool[0].u32BlkSize = 1024;
			 printf("force toogle vb init\n");
		}

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_ERR("SAMPLE_COMM_SYS_Init, %d\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
		CVI_BOOL isFind = CVI_FALSE;

		psvdattr = &psvdec->chnCtx[i].stSampleVdecAttr;

		if (psvdattr->enType == PT_JPEG || psvdattr->enType == PT_MJPEG) {
			u32BlkSize =
				VDEC_GetPicBufferSize(psvdattr->enType, psvdattr->u32Width,
						psvdattr->u32Height,
						psvdattr->enPixelFormat, DATA_BITWIDTH_8,
						COMPRESS_MODE_NONE);

			for (CVI_U32 j = 0; j < stVbConf.u32MaxPoolCnt; j++) {
				if (stVbConf.astCommPool[j].u32BlkSize == u32BlkSize) {
					stVbConf.astCommPool[j].u32BlkCnt += psvdattr->u32FrameBufCnt;
					isFind = CVI_TRUE;
					chnPoolGroup[i] = j;
					break;
				}
			}
			if (!isFind) {
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = u32BlkSize;
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = psvdattr->u32FrameBufCnt;
				chnPoolGroup[i] = stVbConf.u32MaxPoolCnt;
				stVbConf.u32MaxPoolCnt++;
			}
			CVI_VDEC_INFO("chnPoolGroup[%d] = %d\n", i, chnPoolGroup[i]);
		}
	}

	for (i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
		CVI_VDEC_INFO("VDec Init Pool[%d], u32BlkSize = %d, u32BlkCnt = %d\n",
				i, stVbConf.astCommPool[i].u32BlkSize,
				stVbConf.astCommPool[i].u32BlkCnt);
	}

	if (SAMPLE_COMM_VDEC_GetVbMode() != VB_SOURCE_USER) {
		if (stVbConf.u32MaxPoolCnt == 0 && stVbConf.astCommPool[0].u32BlkSize == 0) {
			CVI_SYS_Exit();
			s32Ret = CVI_SYS_Init();
		} else {
			s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		}


		if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_ERR("SAMPLE_COMM_SYS_Init, %d\n", s32Ret);
			return CVI_FAILURE;
		}
	} else {
		for (i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
			CVI_U32 PoolId = CVI_VB_CreatePool(&stVbConf.astCommPool[i]);

			if (PoolId == VB_INVALID_POOLID) {
				CVI_VDEC_ERR("CVI_VB_CreatePool Fail\n");
				return CVI_FAILURE;
			}
			for (CVI_U32 chn = 0; chn < psvdec->u32VdecNumAllChns; chn++) {
				if (chnPoolGroup[chn] == i)
					SAMPLE_COMM_VDEC_SetVBPool(chn, PoolId);
			}
		}
		for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
			CVI_VDEC_INFO("Chn[%d], poolID %d\n", i, SAMPLE_COMM_VDEC_GetVBPool(i));
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 vdecInitVBPoolH26X(sampleVdec *psvdec)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 i;
	SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];

	////////////////////////////////////////////////////
	// init user VB(for VDEC)
	////////////////////////////////////////////////////
	for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
		SAMPLE_VDEC_ATTR *psvdattr = &psvdec->chnCtx[i].stSampleVdecAttr;
		vdecChnInputCfg *pvdcic = &psvdec->inputCfg.chnInCfg[i];

		astSampleVdec[i].enType = psvdattr->enType;
		astSampleVdec[i].u32Width = psvdattr->u32Width;
		astSampleVdec[i].u32Height = psvdattr->u32Height;
		astSampleVdec[i].enMode = VIDEO_MODE_FRAME;
		astSampleVdec[i].stSampleVdecVideo.enDecMode = VIDEO_DEC_MODE_IP;
		astSampleVdec[i].stSampleVdecVideo.enBitWidth = DATA_BITWIDTH_8;
		astSampleVdec[i].stSampleVdecVideo.u32RefFrameNum = 2;
		astSampleVdec[i].u32DisplayFrameNum = 2;
		astSampleVdec[i].u32FrameBufCnt = pvdcic->u32MaxFrameBuffer;
		astSampleVdec[i].enPixelFormat = psvdattr->enPixelFormat;
	}

	s32Ret = SAMPLE_COMM_VDEC_InitVBPool(psvdec->u32VdecNumAllChns, &astSampleVdec[0]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("SAMPLE_COMM_VDEC_InitVBPool fail\n");
	}

	return s32Ret;
}


static void initVdecThreadParam(
		vdecChnCtx *pvdchnCtx,
		VDEC_THREAD_PARAM_S *pvtp,
		char *path,
		CVI_S32 s32MilliSec)
{
	SAMPLE_VDEC_ATTR *psvdattr = &pvdchnCtx->stSampleVdecAttr;

	snprintf(pvtp->cFileName,
			sizeof(pvtp->cFileName), path);
	snprintf(pvtp->cFilePath,
			sizeof(pvtp->cFilePath), "%s",
			SAMPLE_STREAM_PATH);

	pvtp->enType = psvdattr->enType;
	pvtp->s32StreamMode = psvdattr->enMode;
	pvtp->s32ChnId = pvdchnCtx->VdecChn;
	pvtp->s32IntervalTime = 10000;
	pvtp->u64PtsInit = 0;
	pvtp->u64PtsIncrease = 0;
	pvtp->eThreadCtrl = THREAD_CTRL_START;
	pvtp->bCircleSend = CVI_FALSE;
	pvtp->s32MilliSec = s32MilliSec; // block mode
	pvtp->s32MinBufSize = (psvdattr->u32Width * psvdattr->u32Height * 3) >> 1;
	pvtp->bFileEnd = CVI_FALSE;
}

CVI_S32 SAMPLE_VDEC_INIT_VB(sampleVdec *psvdec)
{
	vdecInputCfg *pic = &psvdec->inputCfg;
	vdecChnCtx *pvdchnCtx;
	vdecChnInputCfg *pvdcic = NULL;
	SAMPLE_VDEC_ATTR *psvdattr;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 i;

	psvdec->u32VdecNumAllChns = pic->u32NumAllChns;

	for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
		pvdchnCtx = &psvdec->chnCtx[i];
		pvdcic = &pic->chnInCfg[i];
		psvdattr = &pvdchnCtx->stSampleVdecAttr;

		pvdchnCtx->VdecChn = i;
		s32Ret = vdecInitAttr(psvdattr, pvdcic);
		if (s32Ret != CVI_SUCCESS) {
			CVI_VDEC_ERR("vdecInitAttr, %#x!\n", s32Ret);
			return STAT_ERR_VDEC_INIT_ATTR;
		}
		CVI_VDEC_INFO("[ch%d]BufWidth %d, BufHeight %d, MaxFrame %d\n",
			i, psvdattr->u32Width, psvdattr->u32Height, psvdattr->u32FrameBufCnt);

		if (pvdcic->u32MaxFrameBuffer == 0) {
			if (psvdattr->enType == PT_H264)
				pvdcic->u32MaxFrameBuffer = DEFAULT_MAX_FRM_BUF;
			else if (psvdattr->enType == PT_H265)
				pvdcic->u32MaxFrameBuffer = DEFAULT_MAX_FRM_BUF << 1;
		}
	}

	s32Ret = vdecInitVBPool(psvdec);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("init mod common vb fail for %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = vdecInitVBPoolH26X(psvdec);
	if (s32Ret != CVI_SUCCESS) {
		CVI_VDEC_ERR("init vb fail\n");
		return s32Ret;
	}

	return s32Ret;
}

CVI_VOID SAMPLE_VDEC_STOP(sampleVdec *psvdec)
{
	vdecInputCfg *pic = &psvdec->inputCfg;
	vdecChnCtx *pvdchnCtx;
	vdecChnInputCfg *pvdcic;
	VDEC_THREAD_PARAM_S *pvdtps, *pvdtpg;
	CVI_U32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	SAMPLE_VO_CONFIG_S stVoConfig;
	CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {CVI_TRUE, };

	for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {
		pvdchnCtx = &psvdec->chnCtx[i];
		pvdcic = &pic->chnInCfg[i];
		pvdtps = &pvdchnCtx->stVdecThreadParamSend;
		pvdtpg = &pvdchnCtx->stVdecThreadParamGet;

		SAMPLE_COMM_VDEC_CmdCtrl(pvdtps, &pvdchnCtx->vdecThreadSend);
		SAMPLE_COMM_VDEC_StopSendStream(pvdtps, &pvdchnCtx->vdecThreadSend);

		if (pic->u32BindMode != VDEC_BIND_DISABLE) {
			// Bind mode - Unbind VPSS & VO
			CVI_VDEC_StopRecvStream(i);
			if (pic->u32BindMode == VDEC_BIND_VPSS_VO) {
				SAMPLE_COMM_VPSS_UnBind_VO(0, 0, 0, 0);
			}

			SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
		} else {
			SAMPLE_COMM_VDEC_StopGetPic(pvdtpg, &pvdchnCtx->vdecThreadGet);
			CVI_VDEC_StopRecvStream(i);

			if (pvdcic->bDumpYUV == 0) {
				outputMD5Sum(pvdtpg);
			}

			CVI_VDEC_ResetChn(i);
		}
	}

	if (pic->u32BindMode == VDEC_BIND_VPSS_VENC) {
		// Bind mode - Stop venc
		SAMPLE_VENC_STOP(&psv);
	}

	if (pic->u32BindMode != VDEC_BIND_DISABLE) {
		// Bind mode - Stop VPSS & VO
		for (i = 0; i < psvdec->u32VdecNumAllChns; i++) {

			if (pic->u32BindMode == VDEC_BIND_VPSS_VO) {
				s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
				if (s32Ret != CVI_SUCCESS) {
					CVI_VDEC_ERR("SAMPLE_COMM_VO_GetDefConfig fail\n");
				}

				s32Ret = SAMPLE_COMM_VO_StopVO(&stVoConfig);
				if (s32Ret != CVI_SUCCESS) {
					CVI_VDEC_ERR("SAMPLE_COMM_VO_StopVO fail\n");
				}
			}

			s32Ret = SAMPLE_COMM_VPSS_Stop(i, abChnEnable);
			if (s32Ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("SAMPLE_COMM_VPSS_Stop fail\n");
			}
		}
	}
}

CVI_S32 parseDecArgv(vdecInputCfg *pic, CVI_S32 argc, char **argv)
{
	struct option long_options[MAX_VDEC_OPTIONS + 1];
	vdecChnInputCfg *pvdcic = &pic->chnInCfg[0];
	CVI_S32 ch, idx, ret;
	SAMPLE_ARG arg = { 0 };

	CVI_VDEC_TRACE("\n");

	memset((void *)long_options, 0, sizeof(long_options));
	memset(pic, 0x0, sizeof(vdecInputCfg));

	pic->u32NumAllChns = 1;
	pvdcic->s32getframe_timeout = -1;//blcok mode
	pvdcic->s32sendstream_timeout = -1;//blcok mode

	for (idx = 0; idx < MAX_VDEC_OPTIONS; idx++) {
		if (long_option_ext[idx].opt.name == NULL)
			break;

		if (idx >= MAX_VDEC_OPTIONS) {
			CVI_VDEC_ERR("too many options\n");
			return -1;
		}

		memcpy(&long_options[idx], &long_option_ext[idx].opt, sizeof(struct option));
	}

	optind = 0;
	CVI_VDEC_TRACE("optind = %d\n", optind);
	while ((ch = getopt_long(argc, argv, "c:i:o:d:h", long_options, &idx)) != -1) {
		CVI_VDEC_TRACE("ch = %c, idx = %d, optind = %d\n", ch, idx, optind);

		switch (ch) {
		case 'c':
			strcpy(pvdcic->codec, optarg);
			CVI_VDEC_TRACE("codec = %s\n", pvdcic->codec);

			if (!strcmp(pvdcic->codec, "265"))
				pvdcic->enType = PT_H265;
			else if (!strcmp(pvdcic->codec, "264"))
				pvdcic->enType = PT_H264;
			else if (!strcmp(pvdcic->codec, "jpg"))
				pvdcic->enType = PT_JPEG;
			else if (!strcmp(pvdcic->codec, "mjp"))
				pvdcic->enType = PT_MJPEG;
			else {
				CVI_VDEC_ERR("codec, %s\n", pvdcic->codec);
				return -1;
			}
			break;
		case 'i':
			strcpy(pvdcic->input_path, optarg);
			CVI_VDEC_TRACE("input_path = %s\n", pvdcic->input_path);
			break;
		case 'o':
			strcpy(pvdcic->output_path, optarg);
			pvdcic->bDumpYUV = 1;	//default to dump YUV for decoder
			CVI_VDEC_TRACE("output_path = %s\n", pvdcic->output_path);
			break;
		case 'd':
			pvdcic->bDumpYUV = (CVI_BOOL)atoi(optarg);
			CVI_VDEC_TRACE("bDumpYUV = %d\n", pvdcic->bDumpYUV);
			break;
		case 'h':
			printVdecHelp(argv);
			return STATUS_HELP;
		case 0:
			ret = checkArg(idx, &arg);
			if (ret != CVI_SUCCESS) {
				CVI_VDEC_ERR("checkArg, %d\n", ret);
				printVdecHelp(argv);
				return ret;
			}

			if (!strcmp(long_options[idx].name, "numChn")) {
				pic->u32NumAllChns = arg.uval;
				CVI_VDEC_TRACE("u32NumAllChns = %d\n", pic->u32NumAllChns);
			} else if (!strcmp(long_options[idx].name, "chn")) {
				CVI_VDEC_TRACE("chn = %ld\n", (long)arg.uval);
				pvdcic = &pic->chnInCfg[arg.uval];
			} else if (!strcmp(long_options[idx].name, "bufwidth")) {
				pvdcic->u32BufWidth = arg.uval;
			} else if (!strcmp(long_options[idx].name, "bufheight")) {
				pvdcic->u32BufHeight = arg.uval;
			} else if (!strcmp(long_options[idx].name, "maxframe")) {
				pvdcic->u32MaxFrameBuffer = arg.uval;
			} else if (!strcmp(long_options[idx].name, "vbMode")) {
				SAMPLE_COMM_VDEC_SetVbMode(arg.ival);
			} else if (!strcmp(long_options[idx].name, "testMode")) {
				pic->u32TestMode = arg.uval;
			} else if (!strcmp(long_options[idx].name, "bitStreamFolder")) {
				strcpy(pic->bitStreamFolder, optarg);
			} else if (!strcmp(long_options[idx].name, "getframe-timeout")) {
				pvdcic->s32getframe_timeout = arg.ival;
			} else if (!strcmp(long_options[idx].name, "sendstream-timeout")) {
				pvdcic->s32sendstream_timeout = arg.ival;
			} else if (!strcmp(long_options[idx].name, "bindmode")) {
				pic->u32BindMode = arg.uval;
			} else if (!strcmp(long_options[idx].name, "pixel_format")) {
				pvdcic->s32PixelFormat = arg.ival;
			} else {
				CVI_VDEC_TRACE("not exist name = %s\n", long_options[idx].name);
				printVdecHelp(argv);
				return -1;
			}
			break;
		default:
			CVI_VDEC_TRACE("ch = %c\n", ch);
			printVdecHelp(argv);
			break;
		}
	}

	if (optind < argc)
		printVdecHelp(argv);

	return 0;
}

CVI_VOID SAMPLE_VDEC_HandleSig(CVI_S32 signo)
{
	if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo) {
		// SAMPLE_COMM_VO_HdmiStop();
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
	}

	exit(0);
}

void printVdecHelp(char **argv)
{
	CVI_U32 idx;

	printf("// ------------------------------------------------\n");
	printf("%s --numChn=num-all-chnnels --chn=currChnId -c codec -i bs -o out.yuv\n", argv[0]);
	printf("EX.\n");
	printf(" %s -h\n", argv[0]);
	printf(" %s --numChn=1 --chn=0 -c 264 -i in.264 -o out.yuv\n", argv[0]);
	printf(" %s --numChn=2 --chn=0 -c 264 -i in0.264 -o out0.yuv --chn=1 -c 264 -i in1.264 -o out1.yuv\n", argv[0]);
	printf("// ------------------------------------------------\n");

	for (idx = 0; idx < sizeof(long_option_ext) / sizeof(optionExt); idx++) {
		if (long_option_ext[idx].opt.name == NULL)
			break;

		printf("--%s\n", long_option_ext[idx].opt.name);
		printf("    %s\n", long_option_ext[idx].help);
	}
}

static CVI_S32 checkArg(CVI_S32 entryIdx, SAMPLE_ARG *pArg)
{
	CVI_VDEC_TRACE("entryIdx = %d\n", entryIdx);

	if (long_option_ext[entryIdx].type == ARG_INT) {
		pArg->ival = strtoimax(optarg, NULL, 10);
		if ((int64_t)(pArg->ival) < long_option_ext[entryIdx].min ||
		    (int64_t)(pArg->ival) > long_option_ext[entryIdx].max) {
			CVI_VDEC_ERR("%s = %d, min = %"PRId64", max = %"PRId64"\n",
					long_option_ext[entryIdx].opt.name,
					pArg->ival,
					long_option_ext[entryIdx].min,
					long_option_ext[entryIdx].max);
			return CVI_FAILURE;
		}
	} else if (long_option_ext[entryIdx].type == ARG_UINT) {
		pArg->uval = strtoumax(optarg, NULL, 10);
		if ((int64_t)(pArg->uval) < long_option_ext[entryIdx].min ||
		    (int64_t)(pArg->uval) > long_option_ext[entryIdx].max) {
			CVI_VDEC_ERR("%s = %u, min = %"PRId64", max = %"PRId64"\n",
					long_option_ext[entryIdx].opt.name,
					pArg->uval,
					long_option_ext[entryIdx].min,
					long_option_ext[entryIdx].max);
			return CVI_FAILURE;
		}
	} else if (long_option_ext[entryIdx].type == ARG_STRING) {
		if (optarg == NULL) {
			CVI_VDEC_ERR("%s = NULL\n", long_option_ext[entryIdx].opt.name);
			return CVI_FAILURE;
		}
	}

	return CVI_SUCCESS;
}

static void outputMD5Sum(VDEC_THREAD_PARAM_S *pvdtpg)
{
	FILE *fp = CVI_NULL;
	CVI_CHAR cSaveFile[256];
	CVI_U8 checksum[MD5_DIGEST_LENGTH];
	CVI_CHAR checksum_buf[MD5_DIGEST_LENGTH*2 + 1];

	MD5_Final(checksum, &pvdtpg->tMD5Ctx);

	sprintf(cSaveFile, "chn%d_%s.%s", pvdtpg->s32ChnId, pvdtpg->cFileName, "md5");
	CVI_VDEC_TRACE("SAVE_FILE_NAME %s\n\n", cSaveFile);

	if (cSaveFile != 0) {
		fp = fopen(cSaveFile, "wb");
		if (fp == NULL) {
			CVI_VDEC_ERR("chn %d can't open file %s\n",
				   pvdtpg->s32ChnId, cSaveFile);
			return;
		}
		CVI_VDEC_TRACE("\033[0;34m chn %d saving md5 file:%s \033[0;39m\n",
			   pvdtpg->s32ChnId, cSaveFile);
	}
	for (int i = 0, j = 0; i < MD5_DIGEST_LENGTH; i++, j += 2)
		sprintf(checksum_buf+j, "%02x", checksum[i]);
	checksum_buf[MD5_DIGEST_LENGTH*2] = 0;
	CVI_VDEC_TRACE("checksum_buf: %s\n", checksum_buf);

	if (fp != CVI_NULL) {
		fwrite(checksum_buf, 1, MD5_DIGEST_LENGTH*2, fp);
		fclose(fp);
	}
}

int checkInputConfig(vdecInputCfg *pic)
{
	vdecChnInputCfg *pvdcic;
	CVI_S32 len;
	CVI_U32 idx;

	if (pic->u32NumAllChns <= 0) {
		CVI_VDEC_ERR("u32NumAllChns = %d\n", pic->u32NumAllChns);
		return -1;
	}

	for (idx = 0; idx < pic->u32NumAllChns; idx++) {
		pvdcic = &pic->chnInCfg[idx];
		pvdcic->s32getframe_timeout = pic->chnInCfg[0].s32getframe_timeout;
		pvdcic->s32sendstream_timeout = pic->chnInCfg[0].s32sendstream_timeout;
		if (!(pvdcic->enType == PT_H265 ||
			pvdcic->enType == PT_H264 ||
			pvdcic->enType == PT_JPEG ||
			pvdcic->enType == PT_MJPEG)) {
			CVI_VDEC_ERR("enType = %d\n", pvdcic->enType);
			return -1;
		}

		len = strlen(pvdcic->input_path);
		if (len <= 0) {
			CVI_VDEC_ERR("len, input_path = %s\n", pvdcic->input_path);
			return -1;
		}

		len = strlen(pvdcic->output_path);
		if (len <= 0) {
			CVI_VDEC_ERR("len, output_path = %s\n", pvdcic->output_path);
			return -1;
		}
	}

	return 0;
}
