/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_region.h
 * Description:
 *   region Interface
 */

#ifndef __CVI_REGION_H__
#define __CVI_REGION_H__

#include "cvi_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

CVI_S32 CVI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
CVI_S32 CVI_RGN_Destroy(RGN_HANDLE Handle);

CVI_S32 CVI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
CVI_S32 CVI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

CVI_S32 CVI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

CVI_S32 CVI_RGN_AttachToChn(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_RGN_DetachFromChn(RGN_HANDLE Handle, const MMF_CHN_S *pstChn);

CVI_S32 CVI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
CVI_S32 CVI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

CVI_S32 CVI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);
CVI_S32 CVI_RGN_UpdateCanvas(RGN_HANDLE Handle);
CVI_S32 CVI_RGN_Invert_Color(RGN_HANDLE Handle, MMF_CHN_S *pstChn, CVI_U32 *pu32Color);

CVI_S32 CVI_RGN_BatchBegin(RGN_HANDLEGROUP *pu32Group, CVI_U32 u32Num, const RGN_HANDLE handle[]);
CVI_S32 CVI_RGN_BatchEnd(RGN_HANDLEGROUP u32Group);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __CVI_REGION_H__ */
