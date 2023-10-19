#ifndef __CVI_PQ_BIN_H__
#define __CVI_PQ_BIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "cvi_type.h"

#define BIN_PATH_LENGTH	64
#define BIN_FILE_LENGTH	64

typedef struct {
	CVI_VOID *pIspExtRegAddr;
	CVI_U32 u32IspExtRegSize;
	CVI_VOID *pAeExtRegAddr;
	CVI_U32 u32AeExtRegSize;
	CVI_VOID *pAwbExtRegAddr;
	CVI_U32 u32AwbExtRegSize;
} CVI_PQ_REG_ATTR_S;

typedef enum {
	BIN_VI_OFFLINE_VPSS_OFFLINE = 0,
	BIN_VI_OFFLINE_VPSS_ONLINE,
	BIN_VI_ONLINE_VPSS_OFFLINE,
	BIN_VI_ONLINE_VPSS_ONLINE,
	BIN_VI_PARALLEL_VPSS_OFFLINE,
	BIN_VI_PARALLEL_VPSS_PARALLEL,
	BIN_VI_VPSS_MODE_BUTT
} BIN_VI_VPSS_MODE_E;

typedef struct {
	CVI_UCHAR Author[32];
	CVI_UCHAR Desc[1024];
	CVI_UCHAR Time[32];
} CVI_PQ_BIN_EXTRA_S;

typedef struct {
	CVI_U32 u32ChipId;
	BIN_VI_VPSS_MODE_E enMode;
	CVI_PQ_REG_ATTR_S stIspRegAttr;
} CVI_PQ_BIN_ATTR_S;

CVI_S32 CVI_PQ_BIN_InitEx(const CVI_PQ_BIN_ATTR_S astBinAttr[], CVI_U32 u32IspNum);
CVI_S32 CVI_PQ_BIN_GenerateBinFile(FILE *fp, CVI_PQ_BIN_EXTRA_S *pstBinExtra);
CVI_S32 CVI_PQ_BIN_GetBinExtraAttr(CVI_U8 *pu8FileData, CVI_PQ_BIN_EXTRA_S *pstBinExtraAttr);
CVI_S32 CVI_PQ_BIN_ParseBinData(CVI_U8 *pu8FileData, CVI_U32 u32DataLen);

CVI_S32 CVI_PQ_BIN_SetBinPath(const CVI_CHAR *binPath);
CVI_S32 CVI_PQ_BIN_GetBinPath(CVI_CHAR *binPath);
CVI_S32 CVI_PQ_BIN_SetBinName(const CVI_CHAR *binName);
CVI_S32 CVI_PQ_BIN_GetBinName(CVI_CHAR *binName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


