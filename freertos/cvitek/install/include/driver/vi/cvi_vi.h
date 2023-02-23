#ifndef __CVI_VI_H__
#define __CVI_VI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <cvi_comm_vi.h>

CVI_S32 CVI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
CVI_S32 CVI_VI_Start(CVI_U32 addr, CVI_U32 size);
CVI_S32 CVI_VI_Stop(CVI_VOID);
CVI_VOID CVI_VI_UT_MAIN(CVI_U32 addr, CVI_U32 size);
QueueHandle_t CVI_VI_GetQueHandle(CVI_VOID);
CVI_VOID CVI_VI_QBUF(CVI_VOID);

#ifdef __cplusplus
}
#endif

#endif /* __CVI_VI_H__ */
