#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>

#include "sample_comm.h"
#include "loadbmp.h"

#define OverlayMinHandle 0
#define OverlayExMinHandle 20
#define CoverMinHandle 40
#define CoverExMinHandle 60
#define MosaicMinHandle 80
#define OdecHandle 100

RGN_RGBQUARD_S overlay_palette[256];
CVI_S32 SAMPLE_COMM_REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, CVI_BOOL bFil,
			CVI_U32 u16FilColor, PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;
	CVI_S32 Bpp;
	CVI_U32 nColors;
	CVI_U32 i, u32PdataSize;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return CVI_FAILURE;
	}
	Bpp = bmpInfo.bmiHeader.biBitCount/8;
	nColors = 0;
	if (Bpp == 1) {
		if (bmpInfo.bmiHeader.biClrUsed == 0)
			nColors = 1 << bmpInfo.bmiHeader.biBitCount;
		else
			nColors = bmpInfo.bmiHeader.biClrUsed;

		if (nColors > 256) {
			printf("Number of indexed palette is over 256.");
			return CVI_FAILURE;
		}

		/* Create the palette */
		for (i = 0; i < nColors; i++) {
			overlay_palette[i].argbAlpha = bmpInfo.bmiColors[i].rgbReserved;
			overlay_palette[i].argbRed = bmpInfo.bmiColors[i].rgbRed;
			overlay_palette[i].argbGreen = bmpInfo.bmiColors[i].rgbGreen;
			overlay_palette[i].argbBlue = bmpInfo.bmiColors[i].rgbBlue;
#ifdef _SAMPLE_COMMON_REGION_DEBUG_
			CVI_U32 u32Pixel =
				((overlay_palette[i].argbBlue | overlay_palette[i].argbGreen << 8) |
				(overlay_palette[i].argbRed << 16 | overlay_palette[i].argbAlpha << 24));
			printf("overlay_palette index(%d) (0x%x).\n", i, u32Pixel);
#endif
		}
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	} else if (enPixelFormat == PIXEL_FORMAT_8BIT_MODE) {
		Surface.enColorFmt = OSD_COLOR_FMT_8BIT_MODE;
	} else {
		printf("enPixelFormat err %d\n", enPixelFormat);
		return CVI_FAILURE;
	}

	u32PdataSize = Bpp * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight);
	pstBitmap->pData = malloc(u32PdataSize);
	if (pstBitmap->pData == NULL) {
		printf("malloc osd memory err!\n");
		return CVI_FAILURE;
	}

	CreateSurfaceByBitMap(filename, &Surface, (CVI_U8 *)(pstBitmap->pData));

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	if (bFil) {
		CVI_U32 i, j;
		CVI_U16 *pu16Temp;

		pu16Temp = (CVI_U16 *)pstBitmap->pData;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp) {
					*pu16Temp &= 0x7FFF;
				}

				pu16Temp++;
			}
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_REGION_MST_UpdateCanvas(const char *filename, BITMAP_S *pstBitmap, CVI_BOOL bFil,
				CVI_U32 u16FilColor, SIZE_S *pstSize, CVI_U32 u32Stride, PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return CVI_FAILURE;
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	} else if (enPixelFormat == PIXEL_FORMAT_8BIT_MODE) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	} else {
		printf("Pixel format is not support!\n");
		return CVI_FAILURE;
	}

	if (pstBitmap->pData == NULL) {
		printf("malloc osd memory err!\n");
		return CVI_FAILURE;
	}

	CreateSurfaceByCanvas(filename, &Surface, (CVI_U8 *)(pstBitmap->pData)
			     , pstSize->u32Width, pstSize->u32Height, u32Stride);

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	// if pixel value match color, make it transparent.
	// Only works for ARGB1555
	if (bFil) {
		CVI_U32 i, j;
		CVI_U16 *pu16Temp;

		pu16Temp = (CVI_U16 *)pstBitmap->pData;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp)
					*pu16Temp &= 0x7FFF;

				pu16Temp++;
			}
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType)
{
	CVI_S32 MinHandle;

	switch (enType) {
	case OVERLAY_RGN:
		MinHandle = OverlayMinHandle;
		break;
	case OVERLAYEX_RGN:
		MinHandle = OverlayExMinHandle;
		break;
	case COVER_RGN:
		MinHandle = CoverMinHandle;
		break;
	case COVEREX_RGN:
		MinHandle = CoverExMinHandle;
		break;
	case MOSAIC_RGN:
		MinHandle = MosaicMinHandle;
		break;
	default:
		MinHandle = -1;
		break;
	}
	return MinHandle;
}

CVI_S32 SAMPLE_REGION_CreateOverLay(CVI_S32 HandleNum, PIXEL_FORMAT_E pixelFormat)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAY_RGN;
	if (pixelFormat ==  PIXEL_FORMAT_8BIT_MODE)
		stRegion.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_8BIT_MODE;
	else
		stRegion.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_ARGB_1555;

	stRegion.unAttr.stOverlay.stSize.u32Height = 200;
	stRegion.unAttr.stOverlay.stSize.u32Width = 300;
	stRegion.unAttr.stOverlay.u32BgColor = 0x00000000; // ARGB1555 transparent
	stRegion.unAttr.stOverlay.u32CanvasNum = 2;
	stRegion.unAttr.stOverlay.stCompressInfo.enOSDCompressMode = OSD_COMPRESS_MODE_NONE;
	for (i = OverlayMinHandle; i < OverlayMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateOverLayEx(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAYEX_RGN;
	stRegion.unAttr.stOverlayEx.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	stRegion.unAttr.stOverlayEx.stSize.u32Height = 200;
	stRegion.unAttr.stOverlayEx.stSize.u32Width = 300;
	stRegion.unAttr.stOverlayEx.u32BgColor = 0x00000000; // ARGB1555 transparent
	stRegion.unAttr.stOverlayEx.u32CanvasNum = 2;
	stRegion.unAttr.stOverlayEx.stCompressInfo.enOSDCompressMode = OSD_COMPRESS_MODE_NONE;
	for (i = OverlayExMinHandle; i < OverlayExMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateCover(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVER_RGN;

	for (i = CoverMinHandle; i < CoverMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateCoverEx(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVEREX_RGN;

	for (i = CoverExMinHandle; i < CoverExMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateMosaic(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAIC_RGN;

	for (i = MosaicMinHandle; i < MosaicMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_Destroy(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_AttachToChn(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_AttachToChn(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle, MMF_CHN_S *pstChn)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_DetachFromChn(Handle, pstChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_SetDisplayAttr(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_SetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_SetDisplayAttr failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_GetDisplayAttr(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_GetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetDisplayAttr failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_SetBitMap(Handle, pstBitmap);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_GetUpCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_GetCanvasInfo(Handle, pstCanvasInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_RGN_UpdateCanvas(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_Create(CVI_S32 HandleNum, RGN_TYPE_E enType, PIXEL_FORMAT_E pixelFormat)
{
	CVI_S32 s32Ret;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAY_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
		s32Ret = SAMPLE_REGION_CreateOverLay(HandleNum, pixelFormat);
		break;
	case OVERLAYEX_RGN:
		s32Ret = SAMPLE_REGION_CreateOverLayEx(HandleNum);
		break;
	case COVER_RGN:
		s32Ret = SAMPLE_REGION_CreateCover(HandleNum);
		break;
	case COVEREX_RGN:
		s32Ret = SAMPLE_REGION_CreateCoverEx(HandleNum);
		break;
	case MOSAIC_RGN:
		s32Ret = SAMPLE_REGION_CreateMosaic(HandleNum);
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed! HandleNum%d,entype:%d!\n", HandleNum, enType);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_Destroy(CVI_S32 HandleNum, RGN_TYPE_E enType)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHandle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAY_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
		MinHandle = OverlayMinHandle;
		break;
	case OVERLAYEX_RGN:
		MinHandle = OverlayExMinHandle;
		break;
	case COVER_RGN:
		MinHandle = CoverMinHandle;
		break;
	case COVEREX_RGN:
		MinHandle = CoverExMinHandle;
		break;
	case MOSAIC_RGN:
		MinHandle = MosaicMinHandle;
		break;
	default:
		return CVI_FAILURE;
	}
	for (i = MinHandle; i < MinHandle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_Destroy(i);
		if (s32Ret != CVI_SUCCESS)
			SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_AttachToChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHadle;
	RGN_CHN_ATTR_S stChnAttr;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAY_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}
	memset(&stChnAttr, 0, sizeof(stChnAttr));

	/*set the chn config*/
	stChnAttr.bShow = CVI_TRUE;
	switch (enType) {
	case OVERLAY_RGN:
		MinHadle = OverlayMinHandle;
		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = OVERLAY_RGN;
		stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_FALSE;
		break;
	case OVERLAYEX_RGN:
		MinHadle = OverlayExMinHandle;
		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = OVERLAYEX_RGN;
		stChnAttr.unChnAttr.stOverlayExChn.stInvertColor.bInvColEn = CVI_FALSE;
		break;
	case COVER_RGN:
		MinHadle = CoverMinHandle;

		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = COVER_RGN;
		stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

		stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 100;
		stChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 100;

		stChnAttr.unChnAttr.stCoverChn.u32Color = 0x0000ffff;

		stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
		break;
	case COVEREX_RGN:
		MinHadle = CoverExMinHandle;

		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = COVEREX_RGN;
		stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

		stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 100;
		stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width = 100;

		stChnAttr.unChnAttr.stCoverExChn.u32Color = 0x0000ffff;
		break;
	case MOSAIC_RGN:
		MinHadle = MosaicMinHandle;
		stChnAttr.enType = MOSAIC_RGN;
		stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_8;
		stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 96; // 8 pixel align
		stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width = 96;
		break;
	default:
		return CVI_FAILURE;
	}
	/*attach to Chn*/
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		if (enType == OVERLAY_RGN) {
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 200 * (i - OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 200 * (i - OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.u32Layer = (i - OverlayMinHandle);
		}
		if (enType == OVERLAYEX_RGN) {
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200 * (i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200 * (i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
		}
		if (enType == COVER_RGN) {
			stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200 * (i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200 * (i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.u32Layer = (i - CoverMinHandle);
		}
		if (enType == COVEREX_RGN) {
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 20 + 200 * (i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200 * (i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
		}
		if (enType == MOSAIC_RGN) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 20 + 200 * (i - MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200 * (i - MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i - MosaicMinHandle;
		}
		s32Ret = SAMPLE_REGION_AttachToChn(i, pstChn, &stChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
			break;
		}
	}
	/*detach region from chn */
	if (s32Ret != CVI_SUCCESS && i > 0) {
		i--;
		for (; i >= MinHadle; i--)
			s32Ret = SAMPLE_REGION_DetachFromChn(i, pstChn);
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_DetachFrmChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAY_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
		MinHadle = OverlayMinHandle;
		break;
	case OVERLAYEX_RGN:
		MinHadle = OverlayExMinHandle;
		break;
	case COVER_RGN:
		MinHadle = CoverMinHandle;
		break;
	case COVEREX_RGN:
		MinHadle = CoverExMinHandle;
		break;
	case MOSAIC_RGN:
		MinHadle = MosaicMinHandle;
		break;
	default:
		return CVI_FAILURE;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_DetachFromChn(i, pstChn);
		if (s32Ret != CVI_SUCCESS)
			SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", i);
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, const char *filename,
		PIXEL_FORMAT_E pixelFormat, CVI_BOOL bCompressed)
{
	CVI_S32 s32Ret, u32FileSize;
	BITMAP_S stBitmap;
	RGN_CANVAS_INFO_S stCanvasInfo;
	FILE *pFile;

	s32Ret = CVI_RGN_GetCanvasInfo(Handle, &stCanvasInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	s32Ret = CVI_RGN_UpdateCanvas(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	if (bCompressed) {
		if (filename == NULL) {
			printf("OSD_LoadBMP: filename=NULL\n");
			return -1;
		}

		pFile = fopen((char *)filename, "rb");
		fseek(pFile, 0, SEEK_END); // seek to end of file
		u32FileSize = ftell(pFile); // get current file pointer
		fseek(pFile, 0, SEEK_SET); // seek back to beginning of file

		stBitmap.pData = malloc(u32FileSize);
		if (stBitmap.pData == NULL) {
			printf("malloc osd memory err!\n");
			return CVI_FAILURE;
		}

		stBitmap.enPixelFormat = pixelFormat;
		stBitmap.u32Width = stCanvasInfo.stSize.u32Width;
		stBitmap.u32Height = stCanvasInfo.stSize.u32Height;

		fread(stBitmap.pData, 1, u32FileSize, pFile);
		fclose(pFile);
	} else
		SAMPLE_COMM_REGION_MST_LoadBmp(filename, &stBitmap, CVI_FALSE, 0, pixelFormat);

	s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_SetBitMap failed!Handle:%d\n", Handle);
	free(stBitmap.pData);
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle, const char *filename)
{
	CVI_S32 s32Ret;
	SIZE_S stSize;
	BITMAP_S stBitmap;
	RGN_CANVAS_INFO_S stCanvasInfo;
	FILE *pFile;

	s32Ret = CVI_RGN_GetCanvasInfo(Handle, &stCanvasInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	stBitmap.pData = stCanvasInfo.pu8VirtAddr;
	stSize.u32Width = stCanvasInfo.stSize.u32Width;
	stSize.u32Height = stCanvasInfo.stSize.u32Height;

	if (stCanvasInfo.enOSDCompressMode == OSD_COMPRESS_MODE_SW) {
		pFile = fopen((char *)filename, "rb");

		if (pFile == NULL) {
			printf("Open file failed:%s!\n", filename);
			return -1;
		}

		fread(stBitmap.pData, 1, stCanvasInfo.u32CompressedSize, pFile);
		fclose(pFile);
	} else {
		if (stCanvasInfo.enPixelFormat == PIXEL_FORMAT_8BIT_MODE)
			SAMPLE_COMM_REGION_MST_UpdateCanvas(filename, &stBitmap, CVI_FALSE, 0, &stSize,
						stCanvasInfo.u32Stride, PIXEL_FORMAT_8BIT_MODE);
		else
			SAMPLE_COMM_REGION_MST_UpdateCanvas(filename, &stBitmap, CVI_FALSE, 0, &stSize,
						stCanvasInfo.u32Stride, PIXEL_FORMAT_ARGB_1555);
	}

	s32Ret = CVI_RGN_UpdateCanvas(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ODEC_REGION_Create(CVI_U32 u32FileSize, SIZE_S *stSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	RGN_ATTR_S stRegion;

	memset(&stRegion, 0, sizeof(stRegion));
	stRegion.enType = OVERLAY_RGN;
	stRegion.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_ARGB_8888;
	stRegion.unAttr.stOverlay.stSize.u32Height = stSize->u32Height;
	stRegion.unAttr.stOverlay.stSize.u32Width = stSize->u32Width;
	stRegion.unAttr.stOverlay.u32BgColor = 0x00000000; // ARGB1555 transparent
	stRegion.unAttr.stOverlay.u32CanvasNum = 2;
	stRegion.unAttr.stOverlay.stCompressInfo.enOSDCompressMode = OSD_COMPRESS_MODE_SW;
	stRegion.unAttr.stOverlay.stCompressInfo.u32EstCompressedSize = u32FileSize;
	s32Ret = CVI_RGN_Create(OdecHandle, &stRegion);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("CVI_RGN_Create failed with %#x, hdl(%d)\n", s32Ret, OdecHandle);

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ODEC_REGION_Destroy(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_REGION_Destroy(OdecHandle);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_Destroy failed with %#x, hdl(%d)\n",
			s32Ret, OdecHandle);
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ODEC_REGION_AttachToChn(MMF_CHN_S *pstChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	RGN_CHN_ATTR_S stChnAttr;

	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}

	/*set the chn config*/
	memset(&stChnAttr, 0, sizeof(stChnAttr));
	stChnAttr.bShow = CVI_TRUE;
	stChnAttr.enType = OVERLAY_RGN;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_FALSE;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 0;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 0;
	stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
	s32Ret = SAMPLE_REGION_AttachToChn(OdecHandle, pstChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed with %#x, hdl(%d), chn(%d %d %d)\n",
			s32Ret, OdecHandle, pstChn->enModId, pstChn->s32DevId, pstChn->s32ChnId);

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ODEC_REGION_DetachFrmChn(MMF_CHN_S *pstChn)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}

	s32Ret = SAMPLE_REGION_DetachFromChn(OdecHandle, pstChn);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failedwith %#x, hdl(%d), chn(%d %d %d)\n",
			s32Ret, OdecHandle, pstChn->enModId, pstChn->s32DevId, pstChn->s32ChnId);
	return s32Ret;
}

