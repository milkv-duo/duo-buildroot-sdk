#ifndef __CVI_PQBIN_H__
#define __CVI_PQBIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "cvi_comm_isp.h"
#include "cvi_comm_video.h"

CVI_S32 CVI_PQBIN_SetEncrypt(void);
CVI_S32 CVI_PQBIN_SetSnsWDRMode(const WDR_MODE_E wdrMode);
CVI_S32 CVI_PQBIN_SetBinPath(const CVI_CHAR *binPath);
CVI_S32 CVI_PQBIN_SetBinName(const CVI_CHAR *binName);
CVI_S32 CVI_PQBIN_GetVpssAdjustmentAttr(VPSS_ADJUSTMENT_ATTR_S *pstVpssAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
