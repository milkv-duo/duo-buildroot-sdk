/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_region.h
 * Description:
 *   region Interface
 */

#ifndef __CVI_REGION_H__
#define __CVI_REGION_H__

#include <linux/cvi_comm_region.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Create RGN.
 *
 * @param Handle(In), RGN ID.
 * @param pstRegion(In), region attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

/**
 * @brief Destroy RGN.
 *
 * @param Handle(In), RGN ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_Destroy(RGN_HANDLE Handle);

/**
 * @brief Get region attribute.
 *
 * @param Handle(In), RGN ID.
 * @param pstRegion(Out), region attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);

/**
 * @brief Set region attribute.
 *
 * @param Handle(In), RGN ID.
 * @param pstRegion(In), region attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

/**
 * @brief Set bitmap.
 *
 * @param Handle(In), RGN ID.
 * @param pstBitmap(In), bitmap of overlay.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

/**
 * @brief region apply to chn.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @param pstChnAttr(In), chn attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_AttachToChn(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);

/**
 * @brief cancel region on chn.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_DetachFromChn(RGN_HANDLE Handle, const MMF_CHN_S *pstChn);

/**
 * @brief Set display attribute to chn.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @param pstChnAttr(In), chn attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Get display attribute from chn.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @param pstChnAttr(Out), chn attribute.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

/**
 * @brief Get canvas info.
 *
 * @param Handle(In), RGN ID.
 * @param pstCanvasInfo(In), canvas info.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);

/**
 * @brief Update canvas.
 *
 * @param Handle(In), RGN ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_UpdateCanvas(RGN_HANDLE Handle);

/**
 * @brief Get display attribute from chn.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @param pu32Color(In), color.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_Invert_Color(RGN_HANDLE Handle, MMF_CHN_S *pstChn, CVI_U32 *pu32Color);


/**
 * @brief Set Palette for LUT.
 *
 * @param Handle(In), RGN ID.
 * @param pstChn(In), module chn.
 * @param pstPalette(In), Palette info.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_RGN_SetChnPalette(RGN_HANDLE Handle, const MMF_CHN_S *pstChn, RGN_PALETTE_S *pstPalette);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __CVI_REGION_H__ */
