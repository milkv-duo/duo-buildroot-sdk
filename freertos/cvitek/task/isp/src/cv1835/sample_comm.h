/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_common.h
 * Description:
 */

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__
//#include <pthread.h>
#include "cvi_common.h"
//#include "cvi_buffer.h"
//#include "cvi_comm_sys.h"
//#include "cvi_comm_vb.h"
#include "cvi_comm_isp.h"
//#include "cvi_comm_3a.h"
#include "cvi_comm_sns.h"
//#include "cvi_comm_vi.h"
//#include "cvi_comm_vpss.h"
//#include "cvi_comm_vo.h"
//#include "cvi_comm_venc.h"
//#include "cvi_comm_vdec.h"
//#include "cvi_comm_region.h"
//#include "cvi_comm_adec.h"
//#include "cvi_comm_aenc.h"
//#include "cvi_comm_ai.h"
//#include "cvi_comm_ao.h"
//#include "cvi_comm_aio.h"
//#include "cvi_audio.h"
#include "cvi_defines.h"
#include "cvi_mipi.h"
//#include "cvi_sys.h"
//#include "cvi_vb.h"
//#include "cvi_vi.h"
//#include "cvi_vpss.h"
//#include "cvi_vo.h"
//#include "cvi_isp.h"
//#include "cvi_venc.h"
//#include "cvi_vdec.h"
//#include "cvi_gdc.h"
//#include "cvi_region.h"
//#include "md5sum.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define FILE_NAME_LEN 128
#define MAX_NUM_INSTANCE 4

#define CHECK_CHN_RET(express, Chn, name)                                                                              \
	do {                                                                                                           \
		CVI_S32 Ret;                                                                                           \
		Ret = express;                                                                                         \
		if (Ret != CVI_SUCCESS) {                                                                              \
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn,          \
			       __func__, __LINE__, Ret);                                                               \
			fflush(stdout);                                                                                \
			return Ret;                                                                                    \
		}                                                                                                      \
	} while (0)

#define CHECK_RET(express, name)                                                                                       \
	do {                                                                                                           \
		CVI_S32 Ret;                                                                                           \
		Ret = express;                                                                                         \
		if (Ret != CVI_SUCCESS) {                                                                              \
			printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __func__,            \
			       __LINE__, Ret);                                                                         \
			return Ret;                                                                                    \
		}                                                                                                      \
	} while (0)
#define SAMPLE_PIXEL_FORMAT PIXEL_FORMAT_YUV_PLANAR_420

#define COLOR_RGB_RED RGB_8BIT(0xFF, 0, 0)
#define COLOR_RGB_GREEN RGB_8BIT(0, 0xFF, 0)
#define COLOR_RGB_BLUE RGB_8BIT(0, 0, 0xFF)
#define COLOR_RGB_BLACK RGB_8BIT(0, 0, 0)
#define COLOR_RGB_YELLOW RGB_8BIT(0xFF, 0xFF, 0)
#define COLOR_RGB_CYN RGB_8BIT(0, 0xFF, 0xFF)
#define COLOR_RGB_WHITE RGB_8BIT(0xFF, 0xFF, 0xFF)

#define COLOR_10_RGB_RED RGB(0x3FF, 0, 0)
#define COLOR_10_RGB_GREEN RGB(0, 0x3FF, 0)
#define COLOR_10_RGB_BLUE RGB(0, 0, 0x3FF)
#define COLOR_10_RGB_BLACK RGB(0, 0, 0)
#define COLOR_10_RGB_YELLOW RGB(0x3FF, 0x3FF, 0)
#define COLOR_10_RGB_CYN RGB(0, 0x3FF, 0x3FF)
#define COLOR_10_RGB_WHITE RGB(0x3FF, 0x3FF, 0x3FF)

#define SAMPLE_VO_DEV_DHD0 0 /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1 /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1
#define SAMPLE_AUDIO_PTNUMPERFRM 480

#define WDR_MAX_PIPE_NUM 4 //need checking by jammy
#define MAX_SENSOR_NUM      2
#define ISP_MAX_DEV_NUM     2

#define __FILENAM__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define CVI_TRACE_LOG(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_LOG, "%s:%d:%s(): " fmt, __FILENAM__, __LINE__, __func__, ##__VA_ARGS__)
#define MAX_STRING_LEN 255

#define PAUSE()                                                                                                        \
	do {                                                                                                           \
		printf("---------------press Enter key to exit!---------------\n");                                    \
		getchar();                                                                                             \
	} while (0)

#define SAMPLE_PRT(fmt...)                                                                                             \
	do {                                                                                                           \
		printf("[%s]-%d: ", __func__, __LINE__);                                                               \
		printf(fmt);                                                                                           \
	} while (0)

#define CHECK_NULL_PTR(ptr)                                                                                            \
	do {                                                                                                           \
		if (ptr == NULL) {                                                                                     \
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);                                 \
			return CVI_FAILURE;                                                                            \
		}                                                                                                      \
	} while (0)

/*******************************************************
 *  enum define
 *******************************************************/
typedef enum _PIC_SIZE_E {
	PIC_CIF,
	PIC_D1_PAL, /* 720 * 576 */
	PIC_D1_NTSC, /* 720 * 480 */
	PIC_720P, /* 1280 * 720  */
	PIC_1080P, /* 1920 * 1080 */
	PIC_1088, /* 1920 * 1088 */
	PIC_1440P, /* 2560 * 1440 */
	PIC_2304x1296,
	PIC_2592x1520,
	PIC_2592x1536,
	PIC_2592x1944,
	PIC_2716x1524,
	PIC_3840x2160,
	PIC_4096x2160,
	PIC_3000x3000,
	PIC_4000x3000,
	PIC_3840x8640,
	PIC_640x480,
	PIC_479P, /* 632 * 479 */
	PIC_288P, /* 384 * 288 */
	PIC_CUSTOMIZE,
	PIC_BUTT
} PIC_SIZE_E;

typedef enum _SAMPLE_SNS_TYPE_E {
	/* ------ LINEAR BEGIN ------*/
	SONY_IMX290_MIPI_1M_30FPS_12BIT,
	SONY_IMX290_MIPI_2M_60FPS_12BIT,
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_MIPI_2M_30FPS_12BIT,
	SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,
	SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT,
	OV_OS08A20_MIPI_8M_30FPS_10BIT,
	OV_OS08A20_MIPI_5M_30FPS_10BIT,
	SOI_F35_MIPI_2M_30FPS_10BIT,
	SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT,
	SOI_H65_MIPI_1M_30FPS_10BIT,
	PICO640_THERMAL_479P,
	PICO384_THERMAL_384X288,
	SONY_IMX327_SUBLVDS_2M_30FPS_12BIT,
	SONY_IMX307_SUBLVDS_2M_30FPS_12BIT,
	VIVO_MCS369Q_4M_30FPS_12BIT,
	VIVO_MM308M2_2M_25FPS_8BIT,
	NEXTCHIP_N5_2M_25FPS_8BIT,
	SMS_SC3335_MIPI_3M_30FPS_10BIT,
	SONY_IMX335_MIPI_5M_30FPS_12BIT,
	SONY_IMX335_MIPI_4M_30FPS_12BIT,
	PIXELPLUS_PR2020_2M_25FPS_8BIT,
	SONY_IMX334_MIPI_8M_30FPS_12BIT,
	SMS_SC8238_MIPI_8M_30FPS_10BIT,
	VIVO_MCS369_2M_30FPS_12BIT,
	SMS_SC4210_MIPI_4M_30FPS_12BIT,
	/* ------ LINEAR END ------*/
	SAMPLE_SNS_TYPE_LINEAR_BUTT,

	/* ------ WDR 2TO1 BEGIN ------*/
	SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1 = SAMPLE_SNS_TYPE_LINEAR_BUTT,
	SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1,
	OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1,
	OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1,
	SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1,
	SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1,
	SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,
	SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,
	SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1,
	SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1,
	SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1,
	/* ------ WDR 2TO1 END ------*/
	SAMPLE_SNS_TYPE_BUTT,
} SAMPLE_SNS_TYPE_E;

typedef struct _SAMPLE_SENSOR_INFO_S {
	SAMPLE_SNS_TYPE_E enSnsType;
	CVI_S32 s32SnsId;
	CVI_S32 s32BusId;
	combo_dev_t MipiDev;
	CVI_S16 as16LaneId[5];
	CVI_S8  as8PNSwap[5];
	CVI_U8  u8HwSync;
} SAMPLE_SENSOR_INFO_S;

#if 0
typedef enum _SAMPLE_VO_MODE_E {
	VO_MODE_1MUX,
	VO_MODE_2MUX,
	VO_MODE_4MUX,
	VO_MODE_8MUX,
	VO_MODE_9MUX,
	VO_MODE_16MUX,
	VO_MODE_25MUX,
	VO_MODE_36MUX,
	VO_MODE_49MUX,
	VO_MODE_64MUX,
	VO_MODE_2X4,
	VO_MODE_BUTT
} SAMPLE_VO_MODE_E;

typedef enum _SAMPLE_RC_E {
	SAMPLE_RC_CBR = 0,
	SAMPLE_RC_VBR,
	SAMPLE_RC_AVBR,
	SAMPLE_RC_QVBR,
	SAMPLE_RC_FIXQP,
	SAMPLE_RC_QPMAP,
	SAMPLE_RC_MAX
} SAMPLE_RC_E;

#endif
/*******************************************************
 *   structure define
 *******************************************************/
#if 0
typedef struct _SAMPLE_SNAP_INFO_S {
	bool bSnap;
	bool bDoublePipe;
	VI_PIPE VideoPipe;
	VI_PIPE SnapPipe;
	//VI_VPSS_MODE_E enVideoPipeMode;
	//VI_VPSS_MODE_E enSnapPipeMode;
} SAMPLE_SNAP_INFO_S;
#endif
typedef struct _SAMPLE_DEV_INFO_S {
	VI_DEV ViDev;
	WDR_MODE_E enWDRMode;
} SAMPLE_DEV_INFO_S;

typedef struct _SAMPLE_PIPE_INFO_S {
	VI_PIPE aPipe[WDR_MAX_PIPE_NUM];
	//VI_VPSS_MODE_E enMastPipeMode;
	bool bMultiPipe;
	bool bVcNumCfged;
	bool bIspBypass;
	PIXEL_FORMAT_E enPixFmt;
	CVI_U32 u32VCNum[WDR_MAX_PIPE_NUM];
} SAMPLE_PIPE_INFO_S;
#if 0
typedef struct _SAMPLE_CHN_INFO_S {
	VI_CHN ViChn;
	PIXEL_FORMAT_E enPixFormat;
	DYNAMIC_RANGE_E enDynamicRange;
	VIDEO_FORMAT_E enVideoFormat;
	COMPRESS_MODE_E enCompressMode;
} SAMPLE_CHN_INFO_S;
#endif
typedef struct _SAMPLE_VI_INFO_S {
	SAMPLE_SENSOR_INFO_S stSnsInfo;
	SAMPLE_DEV_INFO_S stDevInfo;
	SAMPLE_PIPE_INFO_S stPipeInfo;
	//SAMPLE_CHN_INFO_S stChnInfo;
	//SAMPLE_SNAP_INFO_S stSnapInfo;
} SAMPLE_VI_INFO_S;

typedef struct _SAMPLE_VI_CONFIG_S {
	SAMPLE_VI_INFO_S astViInfo[VI_MAX_DEV_NUM];
	CVI_S32 as32WorkingViId[VI_MAX_DEV_NUM];
	CVI_S32 s32WorkingViNum;
	CVI_BOOL bViRotation;
} SAMPLE_VI_CONFIG_S;

#if 0
typedef struct _SAMPLE_VI_FRAME_CONFIG_S {
	CVI_U32                 u32Width;
	CVI_U32                 u32Height;
	CVI_U32                 u32ByteAlign;
	PIXEL_FORMAT_E          enPixelFormat;
	VIDEO_FORMAT_E          enVideoFormat;
	COMPRESS_MODE_E         enCompressMode;
	DYNAMIC_RANGE_E         enDynamicRange;
} SAMPLE_VI_FRAME_CONFIG_S;

typedef struct _SAMPLE_VI_FRAME_INFO_S {
	VB_BLK             VbBlk;
	CVI_U32            u32Size;
	VIDEO_FRAME_INFO_S stVideoFrameInfo;
} SAMPLE_VI_FRAME_INFO_S;

typedef struct _SAMPLE_VI_FPN_CALIBRATE_INFO_S {
	CVI_U32                 u32Threshold;
	CVI_U32                 u32FrameNum;
	//ISP_FPN_TYPE_E          enFpnType;
	PIXEL_FORMAT_E          enPixelFormat;
	COMPRESS_MODE_E         enCompressMode;
} SAMPLE_VI_FPN_CALIBRATE_INFO_S;

typedef struct _SAMPLE_VI_FPN_CORRECTION_INFO_S {
	ISP_OP_TYPE_E           enOpType;
	//ISP_FPN_TYPE_E          enFpnType;
	CVI_U32                 u32Strength;
	PIXEL_FORMAT_E          enPixelFormat;
	COMPRESS_MODE_E         enCompressMode;
	SAMPLE_VI_FRAME_INFO_S  stViFrameInfo;
} SAMPLE_VI_FPN_CORRECTION_INFO_S;

typedef struct _SAMPLE_COMM_VO_LAYER_CONFIG_S {
	/* for layer */
	VO_LAYER VoLayer;
	VO_INTF_SYNC_E enIntfSync;
	RECT_S stDispRect;
	SIZE_S stImageSize;
	PIXEL_FORMAT_E enPixFormat;

	CVI_U32 u32DisBufLen;

	/* for chn */
	SAMPLE_VO_MODE_E enVoMode;
} SAMPLE_COMM_VO_LAYER_CONFIG_S;

typedef struct _SAMPLE_VO_CONFIG_S {
	/* for device */
	VO_DEV VoDev;
	VO_INTF_TYPE_E enVoIntfType;
	VO_INTF_SYNC_E enIntfSync;
	VO_SYNC_INFO_S stSyncInfo;
	PIC_SIZE_E enPicSize;
	CVI_U32 u32BgColor;

	/* for layer */
	PIXEL_FORMAT_E enPixFormat;
	RECT_S stDispRect;
	SIZE_S stImageSize;

	CVI_U32 u32DisBufLen;

	/* for channel */
	SAMPLE_VO_MODE_E enVoMode;
} SAMPLE_VO_CONFIG_S;

typedef enum _THREAD_CONTRL_E {
	THREAD_CTRL_START,
	THREAD_CTRL_PAUSE,
	THREAD_CTRL_STOP,
} THREAD_CONTRL_E;

typedef struct _VDEC_THREAD_PARAM_S {
	CVI_S32 s32ChnId;
	PAYLOAD_TYPE_E enType;
	CVI_CHAR cFilePath[128];
	CVI_CHAR cFileName[128];
	CVI_S32 s32StreamMode;
	CVI_S32 s32MilliSec;
	CVI_S32 s32MinBufSize;
	CVI_S32 s32IntervalTime;
	THREAD_CONTRL_E eThreadCtrl;
	CVI_U64 u64PtsInit;
	CVI_U64 u64PtsIncrease;
	CVI_BOOL bCircleSend;
	CVI_BOOL bFileEnd;
	CVI_BOOL bDumpYUV;
	MD5_CTX tMD5Ctx;
} VDEC_THREAD_PARAM_S;

typedef struct _SAMPLE_VDEC_BUF {
	CVI_U32  u32PicBufSize;
	CVI_U32  u32TmvBufSize;
	CVI_BOOL bPicBufAlloc;
	CVI_BOOL bTmvBufAlloc;
} SAMPLE_VDEC_BUF;


typedef struct _SAMPLE_VDEC_VIDEO_ATTR {
	VIDEO_DEC_MODE_E enDecMode;
	CVI_U32              u32RefFrameNum;
	DATA_BITWIDTH_E  enBitWidth;
} SAMPLE_VDEC_VIDEO_ATTR;

typedef struct _SAMPLE_VDEC_PICTURE_ATTR {
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32         u32Alpha;
} SAMPLE_VDEC_PICTURE_ATTR;


typedef struct _SAMPLE_VDEC_ATTR {
	PAYLOAD_TYPE_E enType;
	VIDEO_MODE_E   enMode;
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	CVI_U32 u32FrameBufCnt;
	CVI_U32 u32DisplayFrameNum;
	union {
		SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;      /* structure with video ( h265/h264) */
		SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
	};
} SAMPLE_VDEC_ATTR;

typedef struct _vdecChnCtx_ {
	VDEC_THREAD_PARAM_S stVdecThreadParamSend;
	VDEC_THREAD_PARAM_S stVdecThreadParamGet;
	SAMPLE_VDEC_ATTR stSampleVdecAttr;
	pthread_t vdecThreadSend;
	pthread_t vdecThreadGet;
	VDEC_CHN VdecChn;
} vdecChnCtx;
#endif

extern SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM];

#if 0
typedef struct SAMPLE_VENC_GETSTREAM_PARA_S {
	CVI_BOOL bThreadStart;
	VENC_CHN VeChn[VENC_MAX_CHN_NUM];
	CVI_S32  s32Cnt;
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct _chnInputCfg_ {
	char codec[64];
	unsigned int width;
	unsigned int height;
	char input_path[MAX_STRING_LEN];
	char output_path[MAX_STRING_LEN];
	char outputFileName[MAX_STRING_LEN];
	CVI_S32 num_frames;
	CVI_S32 bsMode;
	CVI_S32 rcMode;
	CVI_S32 iqp;
	CVI_S32 pqp;
	CVI_S32 gop;
	CVI_S32 bitrate;
	CVI_S32 firstFrmstartQp;
	CVI_S32 minIqp;
	CVI_S32 maxIqp;
	CVI_S32 minQp;
	CVI_S32 maxQp;
	CVI_S32 framerate;
	CVI_S32 quality;
	CVI_S32 maxbitrate;
	CVI_S32 statTime;
	CVI_S32 bind_mode;
	CVI_S32 pixel_format;
	CVI_S32 posX;
	CVI_S32 posY;
	CVI_S32 inWidth;
	CVI_S32 inHeight;
	CVI_S32 srcFramerate;
	CVI_U32 bitstreamBufSize;
	CVI_S32 single_LumaBuf;
	CVI_S32 single_core;
	CVI_S32 vpssGrp;
	CVI_S32 vpssChn;
	CVI_S32 forceIdr;
	CVI_S32 chgNum;
	CVI_S32 chgBitrate;
	CVI_S32 chgFramerate;
	CVI_S32 tempLayer;
	CVI_S32 testRoi;
	CVI_S32 bgInterval;
	CVI_S32 reserved[11];
} chnInputCfg;

typedef enum _CHN_STATE_ {
	CHN_STAT_NONE = 0,
	CHN_STAT_START,
	CHN_STAT_STOP,
} CHN_STATE;

typedef enum _BS_MODE_ {
	BS_MODE_QUERY_STAT = 0,
	BS_MODE_PROC,
} BS_MODE;

typedef struct _vencChnCtx_ {
	VENC_CHN VencChn;
	PIC_SIZE_E enSize;
	SIZE_S stSize;
	VIDEO_FRAME_INFO_S *pstFrameInfo;
	VIDEO_FRAME_S *pstVFrame;
	CVI_U32 u32LumaSize;
	CVI_U32 u32ChrmSize;
	CVI_U32 u32FrameSize;
	CVI_U32 num_frames;
	CVI_S32 s32ChnNum;
	CVI_U32 s32FbCnt;
	CVI_U32 u32Profile;
	PAYLOAD_TYPE_E enPayLoad;
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E enRcMode;
	FILE *fpSrc;
	long file_size;
	FILE *pFile;
	chnInputCfg chnIc;
	PIXEL_FORMAT_E pixel_format;
	CHN_STATE chnStat;
	CHN_STATE nextChnStat;
} vencChnCtx;

typedef struct _SAMPLE_INI_CFG_S {
	VI_PIPE_FRAME_SOURCE_E	enSource;
	CVI_U8			devNum;
	SAMPLE_SNS_TYPE_E	enSnsType;
	WDR_MODE_E		enWDRMode;
	CVI_S32			s32BusId;
	combo_dev_t		MipiDev;
	CVI_S16			as16LaneId[5];
	CVI_S8			as8PNSwap[5];
	CVI_U8			u8HwSync;
	SAMPLE_SNS_TYPE_E	enSns2Type;
	WDR_MODE_E		enSns2WDRMode;
	CVI_S32			s32Sns2BusId;
	combo_dev_t		Sns2MipiDev;
	CVI_S16			as16Sns2LaneId[5];
	CVI_S8			as8Sns2PNSwap[5];
	CVI_U8			u8Sns2HwSync;
	CVI_U8			u8UseDualSns;
} SAMPLE_INI_CFG_S;

CVI_S32 ae0_register_callback(int ViPipe);
CVI_S32 ae0_unregister_callback(int ViPipe);
CVI_S32 ae1_register_callback(int ViPipe);
CVI_S32 ae1_unregister_callback(int ViPipe);
CVI_S32 ae2_register_callback(int ViPipe);
CVI_S32 ae2_unregister_callback(int ViPipe);

/********************************************************
 *     function announce
 ********************************************************/

CVI_VOID *SAMPLE_SYS_IOMmap(CVI_U64 u64PhyAddr, CVI_U32 u32Size);
CVI_S32 SAMPLE_SYS_Munmap(CVI_VOID *pVirAddr, CVI_U32 u32Size);
CVI_S32 SAMPLE_SYS_SetReg(CVI_U64 u64Addr, CVI_U32 u32Value);
CVI_S32 SAMPLE_SYS_GetReg(CVI_U64 u64Addr, CVI_U32 *pu32Value);

CVI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize);
CVI_S32 SAMPLE_COMM_SYS_MemConfig(void);
CVI_VOID SAMPLE_COMM_SYS_Exit(void);
CVI_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig);
CVI_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S *pstVbConf, CVI_U32 u32SupplementConfig);

#endif

//CVI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S *pstViConfig);
//CVI_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
//CVI_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
//CVI_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
//CVI_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
//CVI_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
//CVI_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
//CVI_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
//CVI_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
//CVI_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
//CVI_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
//CVI_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
//CVI_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
//CVI_S32 SAMPLE_COMM_VPSS_Bind_VPSS(VPSS_GRP VpssGrpSrc, VPSS_CHN VpssChnSrc, VPSS_GRP VpssGrpDst);
//CVI_S32 SAMPLE_COMM_VPSS_UnBind_VPSS(VPSS_GRP VpssGrpSrc, VPSS_CHN VpssChnSrc, VPSS_GRP VpssGrpDst);
//CVI_S32 SAMPLE_COMM_ISP_Run(CVI_U8 IspDev);
//CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr);
//CVI_VOID SAMPLE_COMM_ISP_Stop(CVI_U8 IspDev);
//CVI_VOID SAMPLE_COMM_All_ISP_Stop(void);
//CVI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev);
//CVI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev);
//CVI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev);
CVI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, CVI_U32 u32SnsId, CVI_S32 s32BusId);
//CVI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev);
//CVI_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev);
//CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr);
CVI_S32 SAMPLE_COMM_ISP_SetSensorMode(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_ISP_SetSnsObj(CVI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType);
CVI_S32 SAMPLE_COMM_ISP_SetSnsInit(CVI_U32 u32SnsId, CVI_U8 u8HwSync);
CVI_S32 SAMPLE_COMM_ISP_PatchSnsObj(CVI_U32 u32SnsId, SAMPLE_SENSOR_INFO_S *pstSnsInfo);
CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj(CVI_U32 u32SnsId);

#if 0
CVI_S32 SAMPLE_AUDIO_DEBUG(void);
CVI_S32 SAMPLE_AUDIO_DEBUG_LEVEL(ST_AudioUnitTestCfg *testCfg);

CVI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr);
void SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize);
CVI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S *pstChnAttr);
CVI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S *pstViInfo);
CVI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo);
CVI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S *pstViInfo);
CVI_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S *pstViInfo);
CVI_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S *pstViInfo);
CVI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_VI_OPEN(void);
CVI_S32 SAMPLE_COMM_VI_CLOSE(void);
CVI_S32 SAMPLE_COMM_VI_StartSensor(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_COMM_VI_ParseIni(SAMPLE_INI_CFG_S *pstIniCfg);

CVI_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
			       VPSS_CHN_ATTR_S *pastVpssChnAttr);
CVI_S32 SAMPLE_COMM_VPSS_Init(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
			      VPSS_CHN_ATTR_S *pastVpssChnAttr);

CVI_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, CVI_BOOL *pabChnEnable);
CVI_S32 SAMPLE_COMM_VPSS_SendFrame(VPSS_GRP VpssGrp, SIZE_S *stSize, PIXEL_FORMAT_E enPixelFormat, CVI_CHAR *filename);
CVI_S32 SAMPLE_COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType,
		FILE *pFd, VENC_STREAM_S *pstStream);
CVI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
CVI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode, VENC_GOP_ATTR_S *pstGopAttr);
CVI_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix);
CVI_S32 SAMPLE_COMM_VENC_CloseReEncode(VENC_CHN VencChn);
CVI_S32 SAMPLE_COMM_VENC_SetJpegParam(chnInputCfg *pIc, VENC_CHN VencChn);
CVI_S32 SAMPLE_COMM_VENC_SetRoiAttr(VENC_CHN VencChn, PAYLOAD_TYPE_E enType);
CVI_S32 SAMPLE_COMM_VENC_SetChnParam(chnInputCfg *pIc, VENC_CHN VencChn);
CVI_S32 SAMPLE_COMM_VENC_Start(
		chnInputCfg * pIc,
		VENC_CHN VencChn,
		PAYLOAD_TYPE_E enType,
		PIC_SIZE_E enSize,
		SAMPLE_RC_E enRcMode,
		CVI_U32 u32Profile,
		CVI_BOOL bRcnRefShareBuf,
		VENC_GOP_ATTR_S *pstGopAttr);
CVI_S32 SAMPLE_COMM_VDEC_InitVBPool(VDEC_CHN VdecChn, SAMPLE_VDEC_ATTR *pastSampleVdec);
CVI_VOID SAMPLE_COMM_VDEC_StartSendStream(VDEC_THREAD_PARAM_S *pstVdecSend,
		pthread_t *pVdecThread);
CVI_S32 SAMPLE_COMM_VDEC_Start(vdecChnCtx *pvdchnCtx);
CVI_VOID SAMPLE_COMM_VDEC_CmdCtrl(VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
CVI_VOID SAMPLE_COMM_VDEC_StopSendStream(VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
CVI_S32 SAMPLE_COMM_VDEC_Stop(CVI_S32 s32ChnNum);
CVI_VOID SAMPLE_COMM_VDEC_ExitVBPool(void);
CVI_VOID SAMPLE_COMM_VDEC_StartGetPic(VDEC_THREAD_PARAM_S *pstVdecGet,
		pthread_t *pVdecThread);
CVI_VOID SAMPLE_COMM_VDEC_StopGetPic(VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);

CVI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, CVI_U32 *pu32W, CVI_U32 *pu32H, CVI_U32 *pu32Frm);
CVI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
CVI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
CVI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
CVI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
CVI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
CVI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
CVI_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
CVI_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
CVI_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
CVI_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
CVI_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig);
CVI_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
CVI_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
CVI_S32 SAMPLE_COMM_VO_StopPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
CVI_S32 SAMPLE_COMM_VO_StartPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
CVI_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
CVI_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
CVI_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
CVI_VOID SAMPLE_COMM_VO_Exit(void);

CVI_S32 SAMPLE_COMM_REGION_Create(CVI_S32 HandleNum, RGN_TYPE_E enType);
CVI_S32 SAMPLE_COMM_REGION_Destroy(CVI_S32 HandleNum, RGN_TYPE_E enType);
CVI_S32 SAMPLE_COMM_REGION_AttachToChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn);
CVI_S32 SAMPLE_COMM_REGION_DetachFrmChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn);
CVI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, const char *filename);
CVI_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle, const char *filename);
CVI_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType);

CVI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
CVI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
CVI_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
CVI_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd);
CVI_S32 SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AoDev);
CVI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
CVI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
CVI_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
CVI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AoDev);
CVI_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd(void);
CVI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
CVI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
CVI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
CVI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
CVI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
CVI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
CVI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S *pstAioAttr);
CVI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, CVI_S32 s32AiChnCnt, AIO_ATTR_S *pstAioAttr,
				  AUDIO_SAMPLE_RATE_E enOutSampleRate, CVI_BOOL bResampleEn, CVI_VOID *pstAiVqeAttr,
				  CVI_U32 u32AiVqeType);
CVI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, CVI_S32 s32AiChnCnt, CVI_BOOL bResampleEn, CVI_BOOL bVqeEn);
CVI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, CVI_S32 s32AoChnCnt, AIO_ATTR_S *pstAioAttr,
				  AUDIO_SAMPLE_RATE_E enInSampleRate, CVI_BOOL bResampleEn);
CVI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, CVI_S32 s32AoChnCnt, CVI_BOOL bResampleEn);
CVI_S32 SAMPLE_COMM_AUDIO_StartAenc(CVI_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
CVI_S32 SAMPLE_COMM_AUDIO_StopAenc(CVI_S32 s32AencChnCnt);
CVI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
CVI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);

CVI_S32 SAMPLE_PLAT_SYS_INIT(SIZE_S stSize);
CVI_S32 SAMPLE_PLAT_VI_INIT(SAMPLE_VI_CONFIG_S *pstViConfig);
CVI_S32 SAMPLE_PLAT_VPSS_INIT(VPSS_GRP VpssGrp, SIZE_S stSizeIn, SIZE_S stSizeOut);
CVI_S32 SAMPLE_PLAT_VO_INIT(void);

/* SAMPLE_COMM_FRAME_SaveToFile:
 *   Save videoframe to the file
 *
 * [in]filename: char string of the file to save data.
 * [IN]pstVideoFrame: the videoframe whose data will be saved to file.
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_FRAME_SaveToFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame);

/* SAMPLE_COMM_PrepareFrame:
 *   Prepare videoframe per size & format.
 *
 * [in]stSize: the size of videoframe
 * [in]enPixelFormat: pixel format of videoframe
 * [Out]pstVideoFrame: the videoframe generated.
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_PrepareFrame(SIZE_S stSize, PIXEL_FORMAT_E enPixelFormat, VIDEO_FRAME_INFO_S *pstVideoFrame);

/* SAMPLE_COMM_FRAME_CompareWithFile:
 *   Compare data with frame.
 *
 * [in]filename: file to read.
 * [in]pstVideoFrame: the video-frame to store data from file.
 * return: True if match; False if mismatch.
 */
CVI_BOOL SAMPLE_COMM_FRAME_CompareWithFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame);

/* SAMPLE_COMM_FRAME_LoadFromFile:
 *   Load data to frame, whose data loaded from given filename.
 *
 * [in]filename: file to read.
 * [in]pstVideoFrame: the video-frame to store data from file.
 * [in]stSize: size of image.
 * [in]enPixelFormat: format of image
 * return: CVI_SUCCESS if no problem.
 */
CVI_S32 SAMPLE_COMM_FRAME_LoadFromFile(const CVI_CHAR *filename, VIDEO_FRAME_INFO_S *pstVideoFrame,
	SIZE_S *stSize, PIXEL_FORMAT_E enPixelFormat);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
