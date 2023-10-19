#ifndef __CVI_BIN_H__
#define __CVI_BIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "xil_types.h"
#include "linux/file.h"
//#include "stdio.h"

#include "cvi_type.h"
#include "cvi_comm_video.h"

#define BIN_FILE_LENGTH	256

enum CVI_BIN_SECTION_ID {
	CVI_BIN_ID_MIN = 0,
	CVI_BIN_ID_HEADER = CVI_BIN_ID_MIN,
	CVI_BIN_ID_ISP0,
	CVI_BIN_ID_ISP1,
	CVI_BIN_ID_ISP2,
	CVI_BIN_ID_ISP3,
	CVI_BIN_ID_VPSS,
	CVI_BIN_ID_VDEC,
	CVI_BIN_ID_VENC,
	CVI_BIN_ID_MAX
};

typedef struct {
	CVI_UCHAR Author[32];
	CVI_UCHAR Desc[1024];
	CVI_UCHAR Time[32];
} CVI_BIN_EXTRA_S;

typedef CVI_S32(*CVI_BIN_getBinSize)(CVI_U32 * size);
typedef CVI_S32(*CVI_BIN_getParamFromBin)(CVI_U8 * addr, CVI_U32 size);
typedef CVI_S32(*CVI_BIN_setParamToBin)(FILE * fp);

struct CVI_BIN_REGISTER_PARAM {
	enum CVI_BIN_SECTION_ID id;
	CVI_BIN_getBinSize funcSize;
	CVI_BIN_getParamFromBin funcGetParam;
	CVI_BIN_setParamToBin funcSetParam;
};

CVI_S32 CVI_BIN_SetEncrypt(void);

CVI_S32 CVI_BIN_Register(struct CVI_BIN_REGISTER_PARAM *param);
CVI_S32 CVI_BIN_Unregister(enum CVI_BIN_SECTION_ID id);
CVI_S32 CVI_BIN_GetBinExtraAttr(FILE *fp, CVI_BIN_EXTRA_S *extraInfo);
CVI_S32 CVI_BIN_SaveParamToBin(FILE *fp, CVI_BIN_EXTRA_S *extraInfo);
CVI_S32 CVI_BIN_LoadParamFromBin(enum CVI_BIN_SECTION_ID id, CVI_U8 *buf);

CVI_S32 CVI_BIN_SetBinName(WDR_MODE_E wdrMode, const CVI_CHAR *binName);
CVI_S32 CVI_BIN_GetBinName(CVI_CHAR *binName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


