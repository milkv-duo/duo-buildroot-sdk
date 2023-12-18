/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_venc.h
 * Description:
 *   Common video encode definitions.
 */
#ifndef __CVI_COMM_VENC_H__
#define __CVI_COMM_VENC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef USE_KERNEL_MODE
#include <pthread.h>
#endif
#include <linux/cvi_comm_video.h>
#include <linux/cvi_type.h>
#include <linux/cvi_comm_rc.h>
#include <linux/cvi_common.h>

#define JPEG_MARKER_ORDER_CNT	16

enum VENC_BIND_MODE_E {
	VENC_BIND_DISABLE  = 0,
	VENC_BIND_VI,
	VENC_BIND_VPSS,
};

#define CVI_VENC_STR_LEN		255
#define CVI_VENC_MASK_ERR		0x1
#define CVI_VENC_MASK_WARN		0x2
#define CVI_VENC_MASK_INFO		0x4
#define CVI_VENC_MASK_FLOW		0x8
#define CVI_VENC_MASK_DBG		0x10
#define CVI_VENC_MASK_BS		0x100
#define CVI_VENC_MASK_SRC		0x200
#define CVI_VENC_MASK_API		0x400
#define CVI_VENC_MASK_SYNC		0x800
#define CVI_VENC_MASK_PERF		0x1000
#define CVI_VENC_MASK_CFG		0x2000
#define CVI_VENC_MASK_FRC		0x4000
#define CVI_VENC_MASK_BIND		0x8000
#define CVI_VENC_MASK_TRACE		0x10000
#define CVI_VENC_MASK_DUMP_YUV	0x100000
#define CVI_VENC_MASK_DUMP_BS	0x200000
#define CVI_VENC_MASK_DEBUG     0x400000

#define CVI_VENC_MASK_CURR		(CVI_VENC_MASK_ERR)

typedef struct _venc_dbg_ {
	CVI_S32 startFn;
	CVI_S32 endFn;
	CVI_S32 dbgMask;
	CVI_S32 currMask;
	CVI_CHAR dbgDir[CVI_VENC_STR_LEN];
	#ifndef USE_KERNEL_MODE
	FILE *dumpYuv;
	FILE *dumpBs;
	#endif
	CVI_U32 noDataTimeout;
} venc_dbg;

extern venc_dbg vencDbg;
#ifndef USE_KERNEL_MODE
extern pthread_t gs_VencTask[VENC_MAX_CHN_NUM];
#endif

//#define ENABLE_SYSLOG
#if defined(CVI_FUNC_COND)
#undef CVI_FUNC_COND
#endif
#define CVI_FUNC_COND(FLAG, FUNC)			\
	do {									\
		if (vencDbg.currMask & (FLAG)) {	\
			FUNC;							\
		}									\
	} while (0)

#ifndef USE_KERNEL_MODE
#define CVI_VENC_PRNT(msg, ...)	\
			printf(msg, ##__VA_ARGS__)
#else
#define CVI_VENC_PRNT(msg, ...)	\
			pr_info(msg, ##__VA_ARGS__)
#endif

#ifndef USE_KERNEL_MODE
#define CVI_VENC_ERR(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_ERR) \
		CVI_TRACE(CVI_DBG_ERR, CVI_ID_VENC, "%s:%d:%s()[ERR]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
		printf("[ERR] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_WARN(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_WARN) \
		CVI_TRACE(CVI_DBG_WARN, CVI_ID_VENC, "%s:%d:%s()[WARN]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_VENC_BS(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_BS) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[BS]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_SRC(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_SRC) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[SRC]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_PERF(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_PERF) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[PERF]: chn = %d, "msg, \
		__FILENAME__, __LINE__, __func__, pChnHandle->VeChn, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_CFG(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_CFG) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[CFG]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_FRC(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_FRC) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[FRC]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_BIND(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_BIND) \
		CVI_TRACE(CVI_DBG_NOTICE, CVI_ID_VENC, "%s:%d:%s()[BIND]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_VENC_INFO(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_INFO) \
		CVI_TRACE(CVI_DBG_INFO, CVI_ID_VENC, "%s:%d:%s()[INFO]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_FLOW(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_FLOW) \
		CVI_TRACE(CVI_DBG_INFO, CVI_ID_VENC, "%s:%d:%s()[FLOW]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_API(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_API) \
		CVI_TRACE(CVI_DBG_INFO, CVI_ID_VENC, "%s:%d:%s()[API]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_VENC_DBG(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DBG) \
		CVI_TRACE(CVI_DBG_DEBUG, CVI_ID_VENC, "%s:%d:%s()[DBG]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_SYNC(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_SYNC) \
		CVI_TRACE(CVI_DBG_DEBUG, CVI_ID_VENC, "%s:%d:%s()[SYNC]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)
#define CVI_VENC_TRACE(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_TRACE) \
		CVI_TRACE(CVI_DBG_DEBUG, CVI_ID_VENC, "%s:%d:%s()[TRACE]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_VENC_DUMP_YUV(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DUMP_YUV) \
		CVI_TRACE(CVI_DBG_DEBUG, CVI_ID_VENC, "%s:%d:%s()[DYUV]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_VENC_DUMP_BS(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DUMP_BS) \
		CVI_TRACE(CVI_DBG_DEBUG, CVI_ID_VENC, "%s:%d:%s()[DBS]: "msg, \
		__FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define CVI_TRACE_VENC(level, fmt, ...)                                           \
	CVI_TRACE(level, CVI_ID_VENC, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#else
#ifndef VC_DEBUG_BASIC_LEVEL
#define CVI_VENC_DEBUG(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DEBUG) { \
			struct timespec64 ts;	\
			ktime_get_ts64(&ts);	\
			pr_info("[DEBUG][%llu] %s = %d," msg, ts.tv_sec * 1000 + ts.tv_nsec / 1000000, __func__, \
			__LINE__, ## __VA_ARGS__); \
		} \
	} while (0)
#define CVI_VENC_ERR(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_ERR) \
		pr_err("[ERR] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_WARN(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_WARN) \
		pr_warn("[WARN] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_BS(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_BS) \
		pr_notice("[BS] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_SRC(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_SRC) \
		pr_notice("[SRC] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_PERF(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_PERF) \
		pr_notice("[PERF] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_CFG(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_CFG) \
		pr_notice("[CFG] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_FRC(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_FRC) \
		pr_notice("[FRC] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_BIND(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_BIND) \
		pr_notice("[BIND] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_INFO(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_INFO) \
		pr_info("[INFO] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_FLOW(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_FLOW) \
		pr_info("[FLOW] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_API(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_API) \
		pr_info("[API] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_DBG(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DBG) \
		pr_debug("[DBG] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_SYNC(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_SYNC) \
		pr_debug("[SYNC] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_TRACE(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_TRACE) \
		pr_debug("[TRACE] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_DUMP_YUV(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DUMP_YUV) \
		pr_debug("[YUV] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_DUMP_BS(msg, ...)	\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DUMP_BS) \
		pr_debug("[BS] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)


#define CVI_TRACE_VENC(level, fmt, ...)                                           \
	pr_debug("%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#else
#define CVI_VENC_DEBUG(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_DEBUG) { \
			struct timespec64 ts;	\
			ktime_get_ts64(&ts);	\
			pr_info("[DEBUG][%llu] %s = %d," msg, ts.tv_sec * 1000 + ts.tv_nsec / 1000000, __func__, \
			__LINE__, ## __VA_ARGS__); \
		} \
	} while (0)
#define CVI_VENC_ERR(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_ERR) \
		pr_err("[ERR] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)
#define CVI_VENC_WARN(msg, ...)		\
	do { \
		if (vencDbg.currMask & CVI_VENC_MASK_WARN) \
		pr_warn("[WARN] %s = %d, "msg, __func__, __LINE__, ## __VA_ARGS__); \
	} while (0)

#define CVI_VENC_BS(msg, ...)
#define CVI_VENC_SRC(msg, ...)
#define CVI_VENC_PERF(msg, ...)
#define CVI_VENC_CFG(msg, ...)
#define CVI_VENC_FRC(msg, ...)
#define CVI_VENC_BIND(msg, ...)
#define CVI_VENC_INFO(msg, ...)
#define CVI_VENC_FLOW(msg, ...)
#define CVI_VENC_API(msg, ...)
#define CVI_VENC_DBG(msg, ...)
#define CVI_VENC_SYNC(msg, ...)
#define CVI_VENC_TRACE(msg, ...)
#define CVI_VENC_DUMP_YUV(msg, ...)
#define CVI_VENC_DUMP_BS(msg, ...)
#define CVI_TRACE_VENC(level, fmt, ...)

#endif
#endif

// TODO: refinememt for hardcode
#define ENUM_ERR_VENC_INVALID_CHNID \
	((CVI_S32)(0xC0000000L | ((CVI_ID_VENC) << 16) | ((4) << 13) | (2)))
#define ENUM_ERR_VENC_VENC_INIT \
	((CVI_S32)(0xC0000000L | ((CVI_ID_VENC) << 16) | ((4) << 13) | (64)))

typedef enum {
	CVI_ERR_VENC_INVALID_CHNID = ENUM_ERR_VENC_INVALID_CHNID,
	CVI_ERR_VENC_ILLEGAL_PARAM,
	CVI_ERR_VENC_EXIST,
	CVI_ERR_VENC_UNEXIST,
	CVI_ERR_VENC_NULL_PTR,
	CVI_ERR_VENC_NOT_CONFIG,
	CVI_ERR_VENC_NOT_SUPPORT,
	CVI_ERR_VENC_NOT_PERM,
	CVI_ERR_VENC_INVALID_PIPEID,
	CVI_ERR_VENC_INVALID_GRPID,
	CVI_ERR_VENC_NOMEM,
	CVI_ERR_VENC_NOBUF,
	CVI_ERR_VENC_BUF_EMPTY,
	CVI_ERR_VENC_BUF_FULL,
	CVI_ERR_VENC_SYS_NOTREADY,
	CVI_ERR_VENC_BADADDR,
	CVI_ERR_VENC_BUSY,
	CVI_ERR_VENC_SIZE_NOT_ENOUGH,
	CVI_ERR_VENC_INVALID_VB,
    ///========//
	CVI_ERR_VENC_INIT = ENUM_ERR_VENC_VENC_INIT,
	CVI_ERR_VENC_FRC_NO_ENC,
	CVI_ERR_VENC_STAT_VFPS_CHANGE,
	CVI_ERR_VENC_EMPTY_STREAM_FRAME,
	CVI_ERR_VENC_EMPTY_PACK,
	CVI_ERR_VENC_JPEG_MARKER_ORDER,
	CVI_ERR_VENC_CU_PREDICTION,
	CVI_ERR_VENC_RC_PARAM,
	CVI_ERR_VENC_H264_ENTROPY,
	CVI_ERR_VENC_H264_TRANS,
	CVI_ERR_VENC_H265_TRANS,
	CVI_ERR_VENC_MUTEX_ERROR,
	CVI_ERR_VENC_INVALILD_RET,
	CVI_ERR_VENC_H264_VUI,
	CVI_ERR_VENC_H265_VUI,
	CVI_ERR_VENC_GOP_ATTR,
	CVI_ERR_VENC_FRAME_PARAM,
	CVI_ERR_VENC_H264_SPLIT,
	CVI_ERR_VENC_H265_SPLIT,
	CVI_ERR_VENC_H264_INTRA_PRED,
	CVI_ERR_VENC_BUTT
} VENC_RECODE_E_ERRTYPE;

/*the nalu type of H264E*/
typedef enum _H264E_NALU_TYPE_E {
	H264E_NALU_BSLICE = 0, /*B SLICE types*/
	H264E_NALU_PSLICE = 1, /*P SLICE types*/
	H264E_NALU_ISLICE = 2, /*I SLICE types*/
	H264E_NALU_IDRSLICE = 5, /*IDR SLICE types*/
	H264E_NALU_SEI = 6, /*SEI types*/
	H264E_NALU_SPS = 7, /*SPS types*/
	H264E_NALU_PPS = 8, /*PPS types*/
	H264E_NALU_BUTT
} H264E_NALU_TYPE_E;

/*the nalu type of H265E*/
typedef enum _H265E_NALU_TYPE_E {
	H265E_NALU_BSLICE = 0, /*B SLICE types*/
	H265E_NALU_PSLICE = 1, /*P SLICE types*/
	H265E_NALU_ISLICE = 2, /*I SLICE types*/
	H265E_NALU_IDRSLICE = 19, /*IDR SLICE types*/
	H265E_NALU_VPS = 32, /*VPS types*/
	H265E_NALU_SPS = 33, /*SPS types*/
	H265E_NALU_PPS = 34, /*PPS types*/
	H265E_NALU_SEI = 39, /*SEI types*/

	H265E_NALU_BUTT
} H265E_NALU_TYPE_E;

/*h265 decoding refresh type*/
typedef enum _H265E_REFERSH_TYPE_E {
	H265E_REFRESH_IDR = 0, /*Instantaneous decoding refresh picture*/
	H265E_REFRESH_CRA = 1, /*Clean random access picture*/
	H265E_REFRESH_BUTT
} H265E_REFRESH_TYPE_E;

/*the reference type of H264E slice*/
typedef enum _H264E_REFSLICE_TYPE_E {
	H264E_REFSLICE_FOR_1X = 1, /*Reference slice for H264E_REF_MODE_1X*/
	H264E_REFSLICE_FOR_2X = 2, /*Reference slice for H264E_REF_MODE_2X*/
	H264E_REFSLICE_FOR_4X = 5, /*Reference slice for H264E_REF_MODE_4X*/
	H264E_REFSLICE_FOR_BUTT /* slice not for reference*/
} H264E_REFSLICE_TYPE_E;

/* the entropy mode of H264E */
typedef enum _H264E_ENTROPY {
	H264E_ENTROPY_CAVLC = 0,
	H264E_ENTROPY_CABAC,
} H264E_ENTROPY;

/* the profile of H264E */
typedef enum _H264E_PROFILE {
	H264E_PROFILE_BASELINE = 0,
	H264E_PROFILE_MAIN,
	H264E_PROFILE_HIGH,
	H264E_PROFILE_BUTT
} H264E_PROFILE;

/*the pack type of JPEGE*/
typedef enum _JPEGE_PACK_TYPE_E {
	JPEGE_PACK_ECS = 5, /*ECS types*/
	JPEGE_PACK_APP = 6, /*APP types*/
	JPEGE_PACK_VDO = 7, /*VDO types*/
	JPEGE_PACK_PIC = 8, /*PIC types*/
	JPEGE_PACK_DCF = 9, /*DCF types*/
	JPEGE_PACK_DCF_PIC = 10, /*DCF PIC types*/
	JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/*the marker type of JPEGE*/
typedef enum _JPEGE_MARKER_TYPE_E {
	JPEGE_MARKER_SOI = 1,           /*SOI*/
	JPEGE_MARKER_DQT = 2,           /*DQT*/
	JPEGE_MARKER_DQT_MERGE = 3,     /*DQT containing multiple tables*/
	JPEGE_MARKER_DHT = 4,           /*DHT*/
	JPEGE_MARKER_DHT_MERGE = 5,     /*DHT containing multiple tables*/
	JPEGE_MARKER_DRI = 6,           /*DRI*/
	JPEGE_MARKER_DRI_OPT = 7,       /*DRI inserted only when restart interval is not 0*/
	JPEGE_MARKER_SOF0 = 8,          /*SOF0*/
	JPEGE_MARKER_JFIF = 9,          /*JFIF tags as APP0*/
	JPEGE_MARKER_FRAME_INDEX = 10,  /*frame index as APP9*/
	JPEGE_MARKER_ADOBE = 11,        /*ADOBE tags as APP14*/
	JPEGE_MARKER_USER_DATA = 12,    /*user data as APP15*/
	JPEGE_MARKER_BUTT
} JPEGE_MARKER_TYPE_E;

typedef enum _JPEGE_FORMAT_E {
	JPEGE_FORMAT_DEFAULT = 0,       /* SOI, FRAME_INDEX, USER_DATA, DRI_OPT, DQT, DHT, SOF0 */
	JPEGE_FORMAT_TYPE_1 = 1,        /* SOI, JFIF, DQT_MERGE, SOF0, DHT_MERGE, DRI */
	JPEGE_FORMAT_CUSTOM = 0xFF,     /* custom marker order specified by JpegMarkerOrder */
} JPEGE_FORMAT_E;

/*the pack type of PRORES*/
typedef enum _PRORES_PACK_TYPE_E {
	PRORES_PACK_PIC = 1, /*PIC types*/
	PRORES_PACK_BUTT
} PRORES_PACK_TYPE_E;

/*the data type of VENC*/
typedef union cviVENC_DATA_TYPE_U {
	H264E_NALU_TYPE_E enH264EType; /* R; H264E NALU types*/
	JPEGE_PACK_TYPE_E enJPEGEType; /* R; JPEGE pack types*/
	H265E_NALU_TYPE_E enH265EType; /* R; H264E NALU types*/
	PRORES_PACK_TYPE_E enPRORESType;
} VENC_DATA_TYPE_U;

/*the pack info of VENC*/
typedef struct _VENC_PACK_INFO_S {
	VENC_DATA_TYPE_U u32PackType; /* R; the pack type*/
	CVI_U32 u32PackOffset;
	CVI_U32 u32PackLength;
} VENC_PACK_INFO_S;

/*Defines a stream packet*/
typedef struct _VENC_PACK_S {
	CVI_U64 u64PhyAddr; /* R; the physics address of stream */

	CVI_U8 ATTRIBUTE *pu8Addr; /* R; the virtual address of stream */
#ifdef __arm__
	__u32 u32AddrPadding;
#endif
	CVI_U32 ATTRIBUTE u32Len; /* R; the length of stream */

	CVI_U64 u64PTS; /* R; PTS */
	CVI_BOOL bFrameEnd; /* R; frame end */

	VENC_DATA_TYPE_U DataType; /* R; the type of stream */
	CVI_U32 u32Offset; /* R; the offset between the Valid data and the start address*/
	CVI_U32 u32DataNum; /* R; the  stream packets num */
	VENC_PACK_INFO_S stPackInfo[8]; /* R; the stream packet Information */
} VENC_PACK_S;

/*Defines the frame type and reference attributes of the H.264 frame skipping reference streams*/
typedef enum _H264E_REF_TYPE_E {
	BASE_IDRSLICE = 0, /* the Idr frame at Base layer*/
	BASE_PSLICE_REFTOIDR, // the P frame at Base layer,
	// referenced by other frames at Base layer and reference to Idr frame
	BASE_PSLICE_REFBYBASE, /* the P frame at Base layer, referenced by other frames at Base layer */
	BASE_PSLICE_REFBYENHANCE, /* the P frame at Base layer, referenced by other frames at Enhance layer */
	ENHANCE_PSLICE_REFBYENHANCE, /* the P frame at Enhance layer, referenced by other frames at Enhance layer */
	ENHANCE_PSLICE_NOTFORREF, /* the P frame at Enhance layer ,not referenced */
	ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum _H264E_REF_TYPE_E H265E_REF_TYPE_E;

/*Defines the features of an H.264 stream*/
typedef struct _VENC_STREAM_INFO_H264_S {
	CVI_U32 u32PicBytesNum; /* R; the coded picture stream byte number */
	CVI_U32 u32Inter16x16MbNum; /* R; the inter16x16 macroblock num */
	CVI_U32 u32Inter8x8MbNum; /* R; the inter8x8 macroblock num */
	CVI_U32 u32Intra16MbNum; /* R; the intra16x16 macroblock num */
	CVI_U32 u32Intra8MbNum; /* R; the intra8x8 macroblock num */
	CVI_U32 u32Intra4MbNum; /* R; the inter4x4 macroblock num */

	H264E_REF_TYPE_E enRefType; /* R; Type of encoded frames in advanced frame skipping reference mode */
	CVI_U32 u32UpdateAttrCnt; // R; Number of times that channel attributes
	// or parameters (including RC parameters) are set
	CVI_U32 u32StartQp; /* R; the start Qp of encoded frames*/
	CVI_U32 u32MeanQp; /* R; the mean Qp of encoded frames*/
	CVI_BOOL bPSkip;
} VENC_STREAM_INFO_H264_S;

/*Defines the features of an H.265 stream*/
typedef struct _VENC_STREAM_INFO_H265_S {
	CVI_U32 u32PicBytesNum; /* R; the coded picture stream byte number */
	CVI_U32 u32Inter64x64CuNum; /* R; the inter64x64 cu num  */
	CVI_U32 u32Inter32x32CuNum; /* R; the inter32x32 cu num  */
	CVI_U32 u32Inter16x16CuNum; /* R; the inter16x16 cu num  */
	CVI_U32 u32Inter8x8CuNum; /* R; the inter8x8   cu num  */
	CVI_U32 u32Intra32x32CuNum; /* R; the Intra32x32 cu num  */
	CVI_U32 u32Intra16x16CuNum; /* R; the Intra16x16 cu num  */
	CVI_U32 u32Intra8x8CuNum; /* R; the Intra8x8   cu num  */
	CVI_U32 u32Intra4x4CuNum; /* R; the Intra4x4   cu num  */
	H265E_REF_TYPE_E enRefType; /* R; Type of encoded frames in advanced frame skipping reference mode*/

	CVI_U32 u32UpdateAttrCnt; // R; Number of times that channel
	// attributes/parameters (including RC parameters) are set
	CVI_U32 u32StartQp; /* R; the start Qp of encoded frames*/
	CVI_U32 u32MeanQp; /* R; the mean Qp of encoded frames*/
	CVI_BOOL bPSkip;
} VENC_STREAM_INFO_H265_S;

/* the sse info*/
typedef struct _VENC_SSE_INFO_S {
	CVI_BOOL bSSEEn; /* RW; Range:[0,1]; Region SSE enable */
	CVI_U32 u32SSEVal; /* R; Region SSE value */
} VENC_SSE_INFO_S;

/* the advance information of the h264e */
typedef struct _VENC_STREAM_ADVANCE_INFO_H264_S {
	CVI_U32 u32ResidualBitNum; /* R; the residual num */
	CVI_U32 u32HeadBitNum; /* R; the head bit num */
	CVI_U32 u32MadiVal; /* R; the madi value */
	CVI_U32 u32MadpVal; /* R; the madp value */
	double dPSNRVal; /* R; the PSNR value */
	CVI_U32 u32MseLcuCnt; /* R; the lcu cnt of the mse */
	CVI_U32 u32MseSum; /* R; the sum of the mse */
	VENC_SSE_INFO_S stSSEInfo[8]; /* R; the information of the sse */
	CVI_U32 u32QpHstgrm[52]; /* R; the Qp histogram value */
	CVI_U32 u32MoveScene16x16Num; /* R; the 16x16 cu num of the move scene*/
	CVI_U32 u32MoveSceneBits; /* R; the stream bit num of the move scene */
} VENC_STREAM_ADVANCE_INFO_H264_S;

/* the advance information of the Jpege */
typedef struct _VENC_STREAM_ADVANCE_INFO_JPEG_S {
	// CVI_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_JPEG_S;

/* the advance information of the Prores */
typedef struct _VENC_STREAM_ADVANCE_INFO_PRORES_S {
	// CVI_U32 u32Reserved;
} VENC_STREAM_ADVANCE_INFO_PRORES_S;

/* the advance information of the h265e */
typedef struct _VENC_STREAM_ADVANCE_INFO_H265_S {
	CVI_U32 u32ResidualBitNum; /* R; the residual num */
	CVI_U32 u32HeadBitNum; /* R; the head bit num */
	CVI_U32 u32MadiVal; /* R; the madi value */
	CVI_U32 u32MadpVal; /* R; the madp value */
	double dPSNRVal; /* R; the PSNR value */
	CVI_U32 u32MseLcuCnt; /* R; the lcu cnt of the mse */
	CVI_U32 u32MseSum; /* R; the sum of the mse */
	VENC_SSE_INFO_S stSSEInfo[8]; /* R; the information of the sse */
	CVI_U32 u32QpHstgrm[52]; /* R; the Qp histogram value */
	CVI_U32 u32MoveScene32x32Num; /* R; the 32x32 cu num of the move scene*/
	CVI_U32 u32MoveSceneBits; /* R; the stream bit num of the move scene */
} VENC_STREAM_ADVANCE_INFO_H265_S;

/*Defines the features of an jpege stream*/
typedef struct _VENC_STREAM_INFO_PRORES_S {
	CVI_U32 u32PicBytesNum;
	CVI_U32 u32UpdateAttrCnt;
} VENC_STREAM_INFO_PRORES_S;

/*Defines the features of an jpege stream*/
typedef struct _VENC_STREAM_INFO_JPEG_S {
	CVI_U32 u32PicBytesNum; /* R; the coded picture stream byte number */
	CVI_U32 u32UpdateAttrCnt; // R; Number of times that channel attributes
	// or parameters (including RC parameters) are set
	CVI_U32 u32Qfactor; /* R; image quality */
} VENC_STREAM_INFO_JPEG_S;

/**
 * @brief Define the attributes of encoded bitstream
 *
 */
typedef struct _VENC_STREAM_S {
	VENC_PACK_S ATTRIBUTE *pstPack;	///< Encoded bitstream packet
#ifdef __arm__
	__u32 u32stPackPadding;
#endif
	CVI_U32 ATTRIBUTE u32PackCount; ///< Number of bitstream packets
	CVI_U32 u32Seq;	///< TODO VENC

	union {
		VENC_STREAM_INFO_H264_S stH264Info;		///< TODO VENC
		VENC_STREAM_INFO_JPEG_S stJpegInfo;		///< The information of JPEG bitstream
		VENC_STREAM_INFO_H265_S stH265Info;		///< TODO VENC
		VENC_STREAM_INFO_PRORES_S stProresInfo;	///< TODO VENC
	};

	union {
		VENC_STREAM_ADVANCE_INFO_H264_S
		stAdvanceH264Info;	///< TODO VENC
		VENC_STREAM_ADVANCE_INFO_JPEG_S
		stAdvanceJpegInfo;	///< TODO VENC
		VENC_STREAM_ADVANCE_INFO_H265_S
		stAdvanceH265Info;	///< TODO VENC
		VENC_STREAM_ADVANCE_INFO_PRORES_S
		stAdvanceProresInfo;///< TODO VENC
	};
} VENC_STREAM_S;

typedef struct _VENC_STREAM_INFO_S {
	H265E_REF_TYPE_E enRefType; /*Type of encoded frames in advanced frame skipping reference mode */

	CVI_U32 u32PicBytesNum; /* the coded picture stream byte number */
	CVI_U32 u32PicCnt; /*Number of times that channel attributes or parameters (including RC parameters) are set */
	CVI_U32 u32StartQp; /*the start Qp of encoded frames*/
	CVI_U32 u32MeanQp; /*the mean Qp of encoded frames*/
	CVI_BOOL bPSkip;

	CVI_U32 u32ResidualBitNum; // residual
	CVI_U32 u32HeadBitNum; // head information
	CVI_U32 u32MadiVal; // madi
	CVI_U32 u32MadpVal; // madp
	CVI_U32 u32MseSum; /* Sum of MSE value */
	CVI_U32 u32MseLcuCnt; /* Sum of LCU number */
	double dPSNRVal; // PSNR
} VENC_STREAM_INFO_S;

/*the size of array is 2,that is the maximum*/
typedef struct _VENC_MPF_CFG_S {
	CVI_U8 u8LargeThumbNailNum; /* RW; Range:[0,2]; the large thumbnail pic num of the MPF */
	SIZE_S astLargeThumbNailSize[2]; /* RW; The resolution of large ThumbNail*/
} VENC_MPF_CFG_S;

typedef enum _VENC_PIC_RECEIVE_MODE_E {
	VENC_PIC_RECEIVE_SINGLE = 0,
	VENC_PIC_RECEIVE_MULTI,
	VENC_PIC_RECEIVE_BUTT
} VENC_PIC_RECEIVE_MODE_E;

/**
 * @brief Define the attributes of JPEG Encoder.
 *
 */
typedef struct _VENC_ATTR_JPEG_S {
	CVI_BOOL bSupportDCF;		///< TODO VENC
	VENC_MPF_CFG_S stMPFCfg;	///< TODO VENC
	VENC_PIC_RECEIVE_MODE_E enReceiveMode;	///< TODO VENC
} VENC_ATTR_JPEG_S;

/*the attribute of h264e*/
typedef struct _VENC_ATTR_H264_S {
	CVI_BOOL bRcnRefShareBuf; /* RW; Range:[0, 1]; Whether to enable the Share Buf of Rcn and Ref .*/
	CVI_BOOL bSingleLumaBuf; /* Use single luma buffer*/
	// reserved
} VENC_ATTR_H264_S;

/*the attribute of h265e*/
typedef struct _VENC_ATTR_H265_S {
	CVI_BOOL bRcnRefShareBuf; /* RW; Range:[0, 1]; Whether to enable the Share Buf of Rcn and Ref .*/
	// reserved
} VENC_ATTR_H265_S;

/*the frame rate of PRORES*/
typedef enum _PRORES_FRAMERATE {
	PRORES_FR_UNKNOWN = 0,
	PRORES_FR_23_976,
	PRORES_FR_24,
	PRORES_FR_25,
	PRORES_FR_29_97,
	PRORES_FR_30,
	PRORES_FR_50,
	PRORES_FR_59_94,
	PRORES_FR_60,
	PRORES_FR_100,
	PRORES_FR_119_88,
	PRORES_FR_120,
	PRORES_FR_BUTT
} PRORES_FRAMERATE;

/*the aspect ratio of PRORES*/
typedef enum _PRORES_ASPECT_RATIO {
	PRORES_ASPECT_RATIO_UNKNOWN = 0,
	PRORES_ASPECT_RATIO_SQUARE,
	PRORES_ASPECT_RATIO_4_3,
	PRORES_ASPECT_RATIO_16_9,
	PRORES_ASPECT_RATIO_BUTT
} PRORES_ASPECT_RATIO;

/*the attribute of PRORES*/
typedef struct _VENC_ATTR_PRORES_S {
	char cIdentifier[4];
	PRORES_FRAMERATE enFrameRateCode;
	PRORES_ASPECT_RATIO enAspectRatio;
} VENC_ATTR_PRORES_S;

/**
 * @brief Define the attributes of the venc.
 *
 */
typedef struct _VENC_ATTR_S {
	PAYLOAD_TYPE_E enType;		///< the type of payload

	CVI_U32 u32MaxPicWidth;		///< maximum width of a picture to be encoded
	CVI_U32 u32MaxPicHeight;	///< maximum height of a picture to be encoded

	CVI_U32 u32BufSize;			///< size of encoded bitstream buffer
	CVI_U32 u32Profile;			///< TODO VENC
	CVI_BOOL bByFrame;			///< mode of collecting encoded bitstream\n
								///< CVI_TRUE: frame-based\n CVI_FALSE: packet-based
	CVI_U32 u32PicWidth;		///< width of a picture to be encoded
	CVI_U32 u32PicHeight;		///< height of a picture to be encoded
	CVI_BOOL bSingleCore;		///< Use single HW core
	CVI_BOOL bEsBufQueueEn;		///< Use es buffer queue
	CVI_BOOL bIsoSendFrmEn;		///< Isolating SendFrame/GetStream pairing
	union {
		VENC_ATTR_H264_S stAttrH264e;	///< TODO VENC
		VENC_ATTR_H265_S stAttrH265e;	///< TODO VENC
		VENC_ATTR_JPEG_S stAttrJpege;	///< the attibute of JPEG encoder
		VENC_ATTR_PRORES_S stAttrProres;///< TODO VENC
	};
} VENC_ATTR_S;

/* the gop mode */
typedef enum _VENC_GOP_MODE_E {
	VENC_GOPMODE_NORMALP = 0, /* NORMALP */
	VENC_GOPMODE_DUALP = 1, /* DUALP */
	VENC_GOPMODE_SMARTP = 2, /* SMARTP */
	VENC_GOPMODE_ADVSMARTP = 3, /* ADVSMARTP */
	VENC_GOPMODE_BIPREDB = 4, /* BIPREDB */
	VENC_GOPMODE_LOWDELAYB = 5, /* LOWDELAYB */
	VENC_GOPMODE_BUTT,
} VENC_GOP_MODE_E;

/* the attribute of the normalp*/
typedef struct _VENC_GOP_NORMALP_S {
	CVI_S32 s32IPQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and I frame */
} VENC_GOP_NORMALP_S;

/* the attribute of the dualp*/
typedef struct _VENC_GOP_DUALP_S {
	CVI_U32 u32SPInterval; /* RW; Range:[0, 1)U(1, U32Gop -1]; Interval of the special P frames */
	CVI_S32 s32SPQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and special P frame */
	CVI_S32 s32IPQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and I frame */
} VENC_GOP_DUALP_S;

/* the attribute of the smartp*/
typedef struct _VENC_GOP_SMARTP_S {
	CVI_U32 u32BgInterval; /* RW; Range:[U32Gop, 65536] ;Interval of the long-term reference frame*/
	CVI_S32 s32BgQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and Bg frame */
	CVI_S32 s32ViQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and virtual I  frame */
} VENC_GOP_SMARTP_S;

/* the attribute of the advsmartp*/
typedef struct _VENC_GOP_ADVSMARTP_S {
	CVI_U32 u32BgInterval; /* RW; Range:[U32Gop, 65536] ;Interval of the long-term reference frame*/
	CVI_S32 s32BgQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and Bg frame */
	CVI_S32 s32ViQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and virtual I  frame */
} VENC_GOP_ADVSMARTP_S;

/* the attribute of the bipredb*/
typedef struct _VENC_GOP_BIPREDB_S {
	CVI_U32 u32BFrmNum; /* RW; Range:[1,3]; Number of B frames */
	CVI_S32 s32BQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and B frame */
	CVI_S32 s32IPQpDelta; /* RW; Range:[-10,30]; QP variance between P frame and I frame */
} VENC_GOP_BIPREDB_S;

/* the attribute of the gop*/
typedef struct _VENC_GOP_ATTR_S {
	VENC_GOP_MODE_E enGopMode; /* RW; Encoding GOP type */
	union {
		VENC_GOP_NORMALP_S stNormalP; /*attributes of normal P*/
		VENC_GOP_DUALP_S stDualP; /*attributes of dual   P*/
		VENC_GOP_SMARTP_S stSmartP; /*attributes of Smart P*/
		VENC_GOP_ADVSMARTP_S stAdvSmartP; /*attributes of AdvSmart P*/
		VENC_GOP_BIPREDB_S stBipredB; /*attributes of b */
	};

} VENC_GOP_ATTR_S;

/**
 * @brief Define the attributes of the venc channel.
 *
 */
typedef struct _VENC_CHN_ATTR_S {
	VENC_ATTR_S stVencAttr;		///< The attribute of video encoder
	VENC_RC_ATTR_S stRcAttr;	///< The attribute of bitrate control
	VENC_GOP_ATTR_S stGopAttr;	///< TODO VENC
} VENC_CHN_ATTR_S;

/* the param of receive picture */
typedef struct _VENC_RECV_PIC_PARAM_S {
	CVI_S32 s32RecvPicNum; // RW; Range:[-1,0)U(0 2147483647]; Number of frames
	// received and encoded by the encoding channel
} VENC_RECV_PIC_PARAM_S;

/**
 * @brief Define the current channel status of encoder
 *
 */
typedef struct _VENC_CHN_STATUS_S {
	CVI_U32 u32LeftPics;				///< Number of frames left to be encoded TODO VENC
	CVI_U32 u32LeftStreamBytes;			///< Number of stream bytes left in the stream buffer TODO VENC
	CVI_U32 u32LeftStreamFrames;		///< Number of encoded frame in the stream buffer TODO VENC
	CVI_U32 u32CurPacks;				///< Number of packets in current frame
	CVI_U32 u32LeftRecvPics;			///< Number of frames to be received TODO VENC
	CVI_U32 u32LeftEncPics;				///< Number of frames to be encoded. TODO VENC
	CVI_BOOL bJpegSnapEnd;				///< if the process of JPEG captureThe is finished. TODO VENC
	VENC_STREAM_INFO_S stVencStrmInfo;	///< the stream information of encoder TODO VENC
} VENC_CHN_STATUS_S;

/* the param of the h264e slice split*/
typedef struct _VENC_H264_SLICE_SPLIT_S {
	CVI_BOOL bSplitEnable;
	// RW; Range:[0,1]; slice split enable, CVI_TRUE:enable, CVI_FALSE:disable, default value:CVI_FALSE
	CVI_U32 u32MbLineNum;
	// RW; Range:[1,(Picture height + 15)/16]; this value presents the mb line number of one slice
} VENC_H264_SLICE_SPLIT_S;

/* the param of the h264e intra pred*/
typedef struct _VENC_H264_INTRA_PRED_S {
	CVI_U32 constrained_intra_pred_flag;
	// RW; Range:[0,1];default: CVI_FALSE, see the H.264 protocol for the meaning
} VENC_H264_INTRA_PRED_S;

/* the param of the h264e trans*/
typedef struct _VENC_H264_TRANS_S {
	CVI_U32 u32IntraTransMode; // RW; Range:[0,2]; Conversion mode for
	// intra-prediction,0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8
	CVI_U32 u32InterTransMode; // RW; Range:[0,2]; Conversion mode for
	// inter-prediction,0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8

	CVI_BOOL bScalingListValid; /* RW; Range:[0,1]; enable Scaling,default: CVI_FALSE  */
	CVI_U8 InterScalingList8X8[64]; /* RW; Range:[1,255]; A quantization table for 8x8 inter-prediction*/
	CVI_U8 IntraScalingList8X8[64]; /* RW; Range:[1,255]; A quantization table for 8x8 intra-prediction*/

	CVI_S32 chroma_qp_index_offset; /* RW; Range:[-12,12];default value: 0, see the H.264 protocol for the meaning*/
} VENC_H264_TRANS_S;

/* the param of the h264e entropy*/
typedef struct _VENC_H264_ENTROPY_S {
	CVI_U32 u32EntropyEncModeI; /* RW; Range:[0,1]; Entropy encoding mode for the I frame, 0:cavlc, 1:cabac */
	CVI_U32 u32EntropyEncModeP; /* RW; Range:[0,1]; Entropy encoding mode for the P frame, 0:cavlc, 1:cabac */
	CVI_U32 u32EntropyEncModeB; /* RW; Range:[0,1]; Entropy encoding mode for the B frame, 0:cavlc, 1:cabac */
	CVI_U32 cabac_init_idc; /* RW; Range:[0,2]; see the H.264 protocol for the meaning */
} VENC_H264_ENTROPY_S;

/* the config of the h264e poc*/
typedef struct _VENC_H264_POC_S {
	CVI_U32 pic_order_cnt_type; /* RW; Range:[0,2]; see the H.264 protocol for the meaning */

} VENC_H264_POC_S;

/* the param of the h264e deblocking*/
typedef struct _VENC_H264_DBLK_S {
	CVI_U32 disable_deblocking_filter_idc; /*  RW; Range:[0,2]; see the H.264 protocol for the meaning */
	CVI_S32 slice_alpha_c0_offset_div2; /*  RW; Range:[-6,+6]; see the H.264 protocol for the meaning */
	CVI_S32 slice_beta_offset_div2; /*  RW; Range:[-6,+6]; see the H.264 protocol for the meaning */
} VENC_H264_DBLK_S;

/* the param of the h264e vui timing info*/
typedef struct _VENC_H264_VUI_TIME_INFO_S {
	CVI_U8 timing_info_present_flag; /* RW; Range:[0,1]; If 1, timing info belows will be encoded into vui.*/
	CVI_U8 fixed_frame_rate_flag; /* RW; Range:[0,1]; see the H.264 protocol for the meaning. */
	CVI_U32 num_units_in_tick; /* RW; Range:(0,4294967295]; see the H.264 protocol for the meaning */
	CVI_U32 time_scale; /* RW; Range:(0,4294967295]; see the H.264 protocol for the meaning */
} VENC_VUI_H264_TIME_INFO_S;

/* the param of the vui aspct ratio*/
typedef struct _VENC_VUI_ASPECT_RATIO_S {
	CVI_U8 aspect_ratio_info_present_flag;
	// RW; Range:[0,1]; If 1, aspectratio info belows will be encoded into vui
	CVI_U8 aspect_ratio_idc; /* RW; Range:[0,255]; 17~254 is reserved,see the protocol for the meaning.*/
	CVI_U8 overscan_info_present_flag; // RW; Range:[0,1]; If 1, oversacan info belows will be encoded into vui.
	CVI_U8 overscan_appropriate_flag; /* RW; Range:[0,1]; see the protocol for the meaning. */
	CVI_U16 sar_width; /* RW; Range:(0, 65535]; see the protocol for the meaning. */
	CVI_U16 sar_height; // RW; Range:(0, 65535]; see the protocol for the meaning.
	// notes: sar_width and sar_height  shall  be  relatively prime.
} VENC_VUI_ASPECT_RATIO_S;

/* the param of the vui video signal*/
typedef struct _VENC_VUI_VIDEO_SIGNAL_S {
	CVI_U8 video_signal_type_present_flag; /* RW; Range:[0,1]; If 1, video singnal info will be encoded into vui. */
	CVI_U8 video_format; /* RW; H.264e Range:[0,7], H.265e Range:[0,5]; see the protocol for the meaning. */
	CVI_U8 video_full_range_flag; /* RW; Range: {0,1}; see the protocol for the meaning.*/
	CVI_U8 colour_description_present_flag; /* RO; Range: {0,1}; see the protocol for the meaning.*/
	CVI_U8 colour_primaries; /* RO; Range: [0,255]; see the protocol for the meaning.*/
	CVI_U8 transfer_characteristics; /* RO; Range: [0,255]; see the protocol for the meaning. */
	CVI_U8 matrix_coefficients; /* RO; Range:[0,255]; see the protocol for themeaning. */
} VENC_VUI_VIDEO_SIGNAL_S;

/* the param of the vui video signal*/
typedef struct _VENC_VUI_BITSTREAM_RESTRIC_S {
	CVI_U8 bitstream_restriction_flag; /* RW; Range: {0,1}; see the protocol for the meaning.*/
} VENC_VUI_BITSTREAM_RESTRIC_S;

/* the param of the h264e vui */
typedef struct _VENC_H264_VUI_S {
	VENC_VUI_ASPECT_RATIO_S stVuiAspectRatio;
	VENC_VUI_H264_TIME_INFO_S stVuiTimeInfo;
	VENC_VUI_VIDEO_SIGNAL_S stVuiVideoSignal;
	VENC_VUI_BITSTREAM_RESTRIC_S stVuiBitstreamRestric;
} VENC_H264_VUI_S;

/* the param of the h265e vui timing info*/
typedef struct _VENC_VUI_H265_TIME_INFO_S {
	CVI_U32 timing_info_present_flag; /* RW; Range:[0,1]; If 1, timing info belows will be encoded into vui.*/
	CVI_U32 num_units_in_tick; /* RW; Range:[0,4294967295]; see the H.265 protocol for the meaning. */
	CVI_U32 time_scale; /* RW; Range:(0,4294967295]; see the H.265 protocol for the meaning */
	CVI_U32 num_ticks_poc_diff_one_minus1; /* RW; Range:(0,4294967294]; see the H.265 protocol for the meaning */
} VENC_VUI_H265_TIME_INFO_S;

/* the param of the h265e vui */
typedef struct _VENC_H265_VUI_S {
	VENC_VUI_ASPECT_RATIO_S stVuiAspectRatio;
	VENC_VUI_H265_TIME_INFO_S stVuiTimeInfo;
	VENC_VUI_VIDEO_SIGNAL_S stVuiVideoSignal;
	VENC_VUI_BITSTREAM_RESTRIC_S stVuiBitstreamRestric;
} VENC_H265_VUI_S;

/* the param of the jpege */
typedef struct _VENC_JPEG_PARAM_S {
	CVI_U32 u32Qfactor; /* RW; Range:[1,99]; Qfactor value, 50 = user q-table */
	CVI_U8 u8YQt[64]; /* RW; Range:[1, 255]; Y quantization table */
	CVI_U8 u8CbQt[64]; /* RW; Range:[1, 255]; Cb quantization table */
	CVI_U8 u8CrQt[64]; /* RW; Range:[1, 255]; Cr quantization table */
	CVI_U32 u32MCUPerECS; // RW; Range:[0, (picwidth + 15) >> 4 x (picheight + 15) >> 4 x 2]; MCU number of one ECS
} VENC_JPEG_PARAM_S;

/* the param of the mjpege */
typedef struct _VENC_MJPEG_PARAM_S {
	CVI_U8 u8YQt[64]; /* RW; Range:[1, 255]; Y quantization table */
	CVI_U8 u8CbQt[64]; /* RW; Range:[1, 255]; Cb quantization table */
	CVI_U8 u8CrQt[64]; /* RW; Range:[1, 255]; Cr quantization table */
	CVI_U32 u32MCUPerECS; // RW; Range:[0, (picwidth + 15) >> 4 x (picheight + 15) >> 4 x 2]; MCU number of one ECS
} VENC_MJPEG_PARAM_S;

/* the param of the ProRes */
typedef struct _VENC_PRORES_PARAM_S {
	CVI_U8 u8LumaQt[64]; /* RW; Range:[1, 255]; Luma quantization table */
	CVI_U8 u8ChromaQt[64]; /* RW; Range:[1, 255]; Chroma quantization table */
	char encoder_identifier[4]; // RW:  identifies the encoder vendor or product that generated the compressed frame
} VENC_PRORES_PARAM_S;

/* the attribute of the roi */
typedef struct _VENC_ROI_ATTR_S {
	CVI_U32 u32Index; /* RW; Range:[0, 7]; Index of an ROI. The system supports indexes ranging from 0 to 7 */
	CVI_BOOL bEnable; /* RW; Range:[0, 1]; Whether to enable this ROI */
	CVI_BOOL bAbsQp; // RW; Range:[0, 1]; QP mode of an ROI. CVI_FALSE: relative QP. CVI_TRUE: absolute QP
	CVI_S32 s32Qp; /* RW; Range:[-51, 51]; QP value,only relative mode can QP value less than 0. */
	RECT_S stRect; /* RW;Region of an ROI*/
} VENC_ROI_ATTR_S;

/* ROI struct */
typedef struct _VENC_ROI_ATTR_EX_S {
	CVI_U32 u32Index; /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
	CVI_BOOL bEnable[3]; // Subscript of array   0: I Frame; 1: P/B Frame; 2: VI Frame; other params are the same
	CVI_BOOL bAbsQp[3]; // QP mode of an ROI.CVI_FALSE: relative QP. CVI_TRUE: absolute QP
	CVI_S32 s32Qp[3]; /* QP value. */
	RECT_S stRect[3]; /* Region of an ROI*/
} VENC_ROI_ATTR_EX_S;

/* the param of the roibg frame rate */
typedef struct _VENC_ROIBG_FRAME_RATE_S {
	CVI_S32 s32SrcFrmRate; /* RW; Range:[-1,0)U(0 2147483647];Source frame rate of a non-ROI*/
	CVI_S32 s32DstFrmRate; /* RW; Range:[-1, s32SrcFrmRate]; Target frame rate of a non-ROI  */
} VENC_ROIBG_FRAME_RATE_S;

/* the param of the roibg frame rate */
typedef struct _VENC_REF_PARAM_S {
	CVI_U32 u32Base; /* RW; Range:[0,4294967295]; Base layer period*/
	CVI_U32 u32Enhance; /* RW; Range:[0,255]; Enhance layer period*/
	CVI_BOOL bEnablePred; // RW; Range:[0, 1]; Whether some frames at the base
	// layer are referenced by other frames at the base layer. When bEnablePred is CVI_FALSE,
	// all frames at the base layer reference IDR frames.
} VENC_REF_PARAM_S;

/* Jpeg snap mode */
typedef enum _VENC_JPEG_ENCODE_MODE_E {
	JPEG_ENCODE_ALL = 0, /* Jpeg channel snap all the pictures when started. */
	JPEG_ENCODE_SNAP = 1, /* Jpeg channel snap the flashed pictures when started. */
	JPEG_ENCODE_BUTT,
} VENC_JPEG_ENCODE_MODE_E;

/* the information of the stream */
typedef struct _VENC_STREAM_BUF_INFO_S {
	CVI_U64 u64PhyAddr[MAX_TILE_NUM]; /* R; Start physical address for a stream buffer */

	CVI_VOID ATTRIBUTE *pUserAddr[MAX_TILE_NUM]; /* R; Start virtual address for a stream buffer */
	CVI_U64 ATTRIBUTE u64BufSize[MAX_TILE_NUM]; /* R; Stream buffer size */
} VENC_STREAM_BUF_INFO_S;

/* the param of the h265e slice split */
typedef struct _VENC_H265_SLICE_SPLIT_S {
	CVI_BOOL bSplitEnable; // RW; Range:[0,1]; slice split enable, CVI_TRUE:enable,
	// CVI_FALSE:disable, default value:CVI_FALSE
	CVI_U32 u32LcuLineNum; // RW; Range:(Picture height + lcu size minus one)/lcu
	// size;this value presents lcu line number
} VENC_H265_SLICE_SPLIT_S;

/* the param of the h265e pu */
typedef struct _VENC_H265_PU_S {
	CVI_U32 constrained_intra_pred_flag; /* RW; Range:[0,1]; see the H.265 protocol for the meaning. */
	CVI_U32 strong_intra_smoothing_enabled_flag; /* RW; Range:[0,1]; see the H.265 protocol for the meaning. */
} VENC_H265_PU_S;

/* the param of the h265e trans */
typedef struct _VENC_H265_TRANS_S {
	CVI_S32 cb_qp_offset; /* RW; Range:[-12,12]; see the H.265 protocol for the meaning. */
	CVI_S32 cr_qp_offset; /* RW; Range:[-12,12]; see the H.265 protocol for the meaning. */

	CVI_BOOL bScalingListEnabled; /* RW; Range:[0,1]; If 1, specifies that a scaling list is used.*/

	CVI_BOOL bScalingListTu4Valid; /* RW; Range:[0,1]; If 1, ScalingList4X4 belows will be encoded.*/
	CVI_U8 InterScalingList4X4[2][16]; /* RW; Range:[1,255]; Scaling List for inter 4X4 block.*/
	CVI_U8 IntraScalingList4X4[2][16]; /* RW; Range:[1,255]; Scaling List for intra 4X4 block.*/

	CVI_BOOL bScalingListTu8Valid; /* RW; Range:[0,1]; If 1, ScalingList8X8 belows will be encoded.*/
	CVI_U8 InterScalingList8X8[2][64]; /* RW; Range:[1,255]; Scaling List for inter 8X8 block.*/
	CVI_U8 IntraScalingList8X8[2][64]; /* RW; Range:[1,255]; Scaling List for intra 8X8 block.*/

	CVI_BOOL bScalingListTu16Valid; /* RW; Range:[0,1]; If 1, ScalingList16X16 belows will be encoded.*/
	CVI_U8 InterScalingList16X16[2][64]; /* RW; Range:[1,255]; Scaling List for inter 16X16 block..*/
	CVI_U8 IntraScalingList16X16[2][64]; /* RW; Range:[1,255]; Scaling List for inter 16X16 block.*/

	CVI_BOOL bScalingListTU32Valid; /* RW; Range:[0,1]; If 1, ScalingList32X32 belows will be encoded.*/
	CVI_U8 InterScalingList32X32[64]; /* RW; Range:[1,255]; Scaling List for inter 32X32 block..*/
	CVI_U8 IntraScalingList32X32[64]; /* RW; Range:[1,255]; Scaling List for inter 32X32 block.*/

} VENC_H265_TRANS_S;

/* the param of the h265e entroy */
typedef struct _VENC_H265_ENTROPY_S {
	CVI_U32 cabac_init_flag; /* RW; Range:[0,1]; see the H.265 protocol for the meaning. */
} VENC_H265_ENTROPY_S;

/* the param of the h265e deblocking */
typedef struct _VENC_H265_DBLK_S {
	CVI_U32 slice_deblocking_filter_disabled_flag; /* RW; Range:[0,1]; see the H.265 protocol for the meaning. */
	CVI_S32 slice_beta_offset_div2; /* RW; Range:[-6,6]; see the H.265 protocol for the meaning. */
	CVI_S32 slice_tc_offset_div2; /* RW; Range:[-6,6]; see the H.265 protocol for the meaning. */
} VENC_H265_DBLK_S;

/* the param of the h265e sao */
typedef struct _VENC_H265_SAO_S {
	CVI_U32 slice_sao_luma_flag; // RW; Range:[0,1]; Indicates whether SAO filtering is
	// performed on the luminance component of the current slice.
	CVI_U32 slice_sao_chroma_flag; // RW; Range:[0,1]; Indicates whether SAO filtering
	// is performed on the chrominance component of the current slice
} VENC_H265_SAO_S;

/* venc mode type */
typedef enum _VENC_INTRA_REFRESH_MODE_E {
	INTRA_REFRESH_ROW = 0, /* Line mode */
	INTRA_REFRESH_COLUMN, /* Column mode */
	INTRA_REFRESH_BUTT
} VENC_INTRA_REFRESH_MODE_E;

/* the param of the intra refresh */
typedef struct _VENC_INTRA_REFRESH_S {
	CVI_BOOL bRefreshEnable; // RW; Range:[0,1]; intra refresh enable, CVI_TRUE:enable,
    // CVI_FALSE:disable, default value:CVI_FALSE
	VENC_INTRA_REFRESH_MODE_E
	enIntraRefreshMode; /*RW;Range:INTRA_REFRESH_ROW or INTRA_REFRESH_COLUMN*/
	CVI_U32 u32RefreshNum; /* RW; Number of rows/column to be refreshed during each I macroblock refresh*/
	CVI_U32 u32ReqIQp; /* RW; Range:[0,51]; QP value of the I frame*/
} VENC_INTRA_REFRESH_S;

/* venc mode type */
typedef enum _VENC_MODTYPE_E {
	MODTYPE_VENC = 1, /* VENC */
	MODTYPE_H264E, /* H264e */
	MODTYPE_H265E, /* H265e */
	MODTYPE_JPEGE, /* Jpege */
	MODTYPE_RC, /* Rc */
	MODTYPE_BUTT
} VENC_MODTYPE_E;

/* the param of the h264e mod */
typedef struct _VENC_MOD_H264E_S {
	CVI_U32 u32OneStreamBuffer; /* RW; Range:[0,1]; one stream buffer*/
	CVI_U32 u32H264eMiniBufMode; /* RW; Range:[0,1]; H264e MiniBufMode*/
	CVI_U32 u32H264ePowerSaveEn; /* RW; Range:[0,1]; H264e PowerSaveEn*/
	VB_SOURCE_E enH264eVBSource; /* RW; Range:VB_SOURCE_PRIVATE,VB_SOURCE_USER; H264e VBSource*/
	CVI_BOOL bQpHstgrmEn; /* RW; Range:[0,1]*/
	CVI_U32 u32UserDataMaxLen;   /* RW; Range:[0,65536]; maximum number of bytes of a user data segment */
	CVI_BOOL bSingleEsBuf;       /* RW; Range[0,1]; use single output es buffer in n-way encode */
	CVI_U32 u32SingleEsBufSize;  /* RW; size of single es buffer in n-way encode */
} VENC_MOD_H264E_S;

/* the param of the h265e mod */
typedef struct _VENC_MOD_H265E_S {
	CVI_U32 u32OneStreamBuffer; /* RW; Range:[0,1]; one stream buffer*/
	CVI_U32 u32H265eMiniBufMode; /* RW; Range:[0,1]; H265e MiniBufMode*/
	CVI_U32 u32H265ePowerSaveEn; /* RW; Range:[0,2]; H265e PowerSaveEn*/
	VB_SOURCE_E enH265eVBSource; /* RW; Range:VB_SOURCE_PRIVATE,VB_SOURCE_USER; H265e VBSource*/
	CVI_BOOL bQpHstgrmEn; /* RW; Range:[0,1]*/
	CVI_U32 u32UserDataMaxLen;   /* RW; Range:[0,65536]; maximum number of bytes of a user data segment */
	CVI_BOOL bSingleEsBuf;       /* RW; Range[0,1]; use single output es buffer in n-way encode */
	CVI_U32 u32SingleEsBufSize;  /* RW; size of single es buffer in n-way encode */
	H265E_REFRESH_TYPE_E enRefreshType; /* RW; Range:H265E_REFRESH_IDR,H265E_REFRESH_CRA; decoding refresh type */
} VENC_MOD_H265E_S;

/* the param of the jpege mod */
typedef struct _VENC_MOD_JPEGE_S {
	CVI_U32 u32OneStreamBuffer; /* RW; Range:[0,1]; one stream buffer*/
	CVI_U32 u32JpegeMiniBufMode; /* RW; Range:[0,1]; Jpege MiniBufMode*/
	CVI_U32 u32JpegClearStreamBuf; /* RW; Range:[0,1]; JpegClearStreamBuf*/
	CVI_BOOL bSingleEsBuf;       /* RW; Range[0,1]; use single output es buffer in n-way encode */
	CVI_U32 u32SingleEsBufSize;  /* RW; size of single es buffer in n-way encode */
	JPEGE_FORMAT_E enJpegeFormat; /* RW; Range[0,255]; Jpege format with different marker order */
	JPEGE_MARKER_TYPE_E JpegMarkerOrder[JPEG_MARKER_ORDER_CNT];  /* RW: array specifying JPEG marker order*/
} VENC_MOD_JPEGE_S;

typedef struct _VENC_MOD_RC_S {
	CVI_U32 u32ClrStatAfterSetBr;
} VENC_MOD_RC_S;
/* the param of the venc mod */
typedef struct _VENC_MOD_VENC_S {
	CVI_U32 u32VencBufferCache; /* RW; Range:[0,1]; VencBufferCache*/
	CVI_U32 u32FrameBufRecycle; /* RW; Range:[0,1]; FrameBufRecycle*/
} VENC_MOD_VENC_S;

/* the param of the mod */
typedef struct _VENC_MODPARAM_S {
	VENC_MODTYPE_E enVencModType; /* RW; VencModType*/
	union {
		VENC_MOD_VENC_S stVencModParam;
		VENC_MOD_H264E_S stH264eModParam;
		VENC_MOD_H265E_S stH265eModParam;
		VENC_MOD_JPEGE_S stJpegeModParam;
		VENC_MOD_RC_S stRcModParam;
	};
} VENC_PARAM_MOD_S;

typedef enum _VENC_FRAME_TYPE_E {
	VENC_FRAME_TYPE_NONE = 1,
	VENC_FRAME_TYPE_IDR,
	VENC_FRAME_TYPE_BUTT
} VENC_FRAME_TYPE_E;

/* the information of the user rc*/
typedef struct _USER_RC_INFO_S {
	CVI_BOOL bQpMapValid; /*RW; Range:[0,1]; Indicates whether the QpMap mode is valid for the current frame*/
	CVI_BOOL bSkipWeightValid; // RW; Range:[0,1]; Indicates whether
	// the Skip Weight mode is valid for the current frame
	CVI_U32 u32BlkStartQp; /* RW; Range:[0,51];QP value of the first 16 x 16 block in QpMap mode */
	CVI_U64 u64QpMapPhyAddr; /* RW; Physical address of the QP table in QpMap mode*/
	CVI_U64 u64SkipWeightPhyAddr; /* RW; Physical address of the SkipWeight table in QpMap mode*/
	VENC_FRAME_TYPE_E enFrameType;
} USER_RC_INFO_S;

/* the information of the user frame*/
typedef struct _USER_FRAME_INFO_S {
	VIDEO_FRAME_INFO_S stUserFrame;
	USER_RC_INFO_S stUserRcInfo;
} USER_FRAME_INFO_S;

/* the config of the sse*/
typedef struct _VENC_SSE_CFG_S {
	CVI_U32 u32Index; /* RW; Range:[0, 7]; Index of an SSE. The system supports indexes ranging from 0 to 7 */
	CVI_BOOL bEnable; /* RW; Range:[0, 1]; Whether to enable SSE */
	RECT_S stRect; /* RW; */
} VENC_SSE_CFG_S;

/* the param of the crop */
typedef struct _VENC_CROP_INFO_S {
	CVI_BOOL bEnable; /* RW; Range:[0, 1]; Crop region enable */
	RECT_S stRect; /* RW;  Crop region, note: s32X must be multi of 16 */
} VENC_CROP_INFO_S;

/* the param of the venc frame rate */
typedef struct _VENC_FRAME_RATE_S {
	CVI_S32 s32SrcFrmRate; /* RW; Range:[0, 240]; Input frame rate of a channel*/
	CVI_S32 s32DstFrmRate; /* RW; Range:[0, 240]; Output frame rate of a channel*/
} VENC_FRAME_RATE_S;

/* the param of the venc encode chnl */
typedef struct _VENC_CHN_PARAM_S {
	CVI_BOOL bColor2Grey; /* RW; Range:[0, 1]; Whether to enable Color2Grey.*/
	CVI_U32 u32Priority; /* RW; Range:[0, 1]; The priority of the coding chnl.*/
	CVI_U32 u32MaxStrmCnt; /* RW: Range:[0,4294967295]; Maximum number of frames in a stream buffer*/
	CVI_U32 u32PollWakeUpFrmCnt; /* RW: Range:(0,4294967295]; the frame num needed to wake up  obtaining streams */
	VENC_CROP_INFO_S stCropCfg;
	VENC_FRAME_RATE_S stFrameRate;
} VENC_CHN_PARAM_S;

/*the ground protect of FOREGROUND*/
typedef struct _VENC_FOREGROUND_PROTECT_S {
	CVI_BOOL bForegroundCuRcEn;
	CVI_U32 u32ForegroundDirectionThresh;
	// RW; Range:[0, 16]; The direction for controlling the macroblock-level bit rate
	CVI_U32 u32ForegroundThreshGain; /*RW; Range:[0, 15]; The gain of the thresh*/
	CVI_U32 u32ForegroundThreshOffset; /*RW; Range:[0, 255]; The offset of the thresh*/
	CVI_U32 u32ForegroundThreshP[RC_TEXTURE_THR_SIZE];
	/*RW; Range:[0, 255]; Mad threshold for controlling the foreground macroblock-level bit rate of P frames */
	CVI_U32 u32ForegroundThreshB[RC_TEXTURE_THR_SIZE]; // RW; Range:[0, 255]; Mad threshold for controlling
	// the foreground macroblock-level bit rate of B frames
} VENC_FOREGROUND_PROTECT_S;

/* the scene mode of the venc encode chnl */
typedef enum _VENC_SCENE_MODE_E {
	SCENE_0 = 0, /*RW;A scene in which the camera does not move or periodically moves continuously*/
	SCENE_1 = 1, /*RW;Motion scene at high bit rate*/
	SCENE_2 = 2, // RW;It has regular continuous motion at medium bit rate and the
	// encoding pressure is relatively large
	SCENE_BUTT
} VENC_SCENE_MODE_E;

typedef struct _VENC_DEBREATHEFFECT_S {
	CVI_BOOL bEnable; /*RW; Range:[0,1];default: 0, DeBreathEffect enable */
	CVI_S32 s32Strength0; /*RW; Range:[0,35];The Strength0 of DeBreathEffect.*/
	CVI_S32 s32Strength1; /*RW; Range:[0,35];The Strength1 of DeBreathEffect.*/
} VENC_DEBREATHEFFECT_S;

typedef struct _VENC_CU_PREDICTION_S {
	OPERATION_MODE_E enPredMode;

	CVI_U32 u32IntraCost;
	CVI_U32 u32Intra32Cost;
	CVI_U32 u32Intra16Cost;
	CVI_U32 u32Intra8Cost;
	CVI_U32 u32Intra4Cost;

	CVI_U32 u32Inter64Cost;
	CVI_U32 u32Inter32Cost;
	CVI_U32 u32Inter16Cost;
	CVI_U32 u32Inter8Cost;
} VENC_CU_PREDICTION_S;

typedef struct _VENC_SKIP_BIAS_S {
	CVI_BOOL bSkipBiasEn;
	CVI_U32 u32SkipThreshGain;
	CVI_U32 u32SkipThreshOffset;
	CVI_U32 u32SkipBackgroundCost;
	CVI_U32 u32SkipForegroundCost;
} VENC_SKIP_BIAS_S;

typedef struct _VENC_HIERARCHICAL_QP_S {
	CVI_BOOL bHierarchicalQpEn;
	CVI_S32 s32HierarchicalQpDelta[4];
	CVI_S32 s32HierarchicalFrameNum[4];
} VENC_HIERARCHICAL_QP_S;

typedef struct _VENC_CHN_POOL_S {
	VB_POOL hPicVbPool; /* RW;  vb pool id for pic buffer */
	VB_POOL hPicInfoVbPool; /* RW;  vb pool id for pic info buffer */
} VENC_CHN_POOL_S;

typedef struct _VENC_RC_ADVPARAM_S {
	CVI_U32 u32ClearStatAfterSetAttr;
} VENC_RC_ADVPARAM_S;

typedef struct _VENC_FRAME_PARAM_S {
	CVI_U32 u32FrameQp;
	CVI_U32 u32FrameBits;
} VENC_FRAME_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_VENC_H__ */
