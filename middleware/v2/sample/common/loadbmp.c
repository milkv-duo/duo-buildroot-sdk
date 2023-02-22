#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "loadbmp.h"

/*
 * The compression types supported by BMP are listed below :
 * 0 - no compression
 * 1 - 8 bit run length encoding
 * 2 - 4 bit run length encoding
 * 3 - RGB bitmap with mask
 * Compression encodings for BMP files
 */
#ifndef BI_RGB
#define BI_RGB      0
#define BI_RLE8     1
#define BI_RLE4     2
#define BI_BITFIELDS    3
#endif

OSD_COMP_INFO s_OSDCompInfo[OSD_COLOR_FMT_BUTT] = {
	{ 0, 4, 4, 4 }, /*RGB444*/
	{ 4, 4, 4, 4 }, /*ARGB4444*/
	{ 0, 5, 5, 5 }, /*RGB555*/
	{ 0, 5, 6, 5 }, /*RGB565*/
	{ 1, 5, 5, 5 }, /*ARGB1555*/
	{ 0, 0, 0, 0 }, /*RESERVED*/
	{ 0, 8, 8, 8 }, /*RGB888*/
	{ 8, 8, 8, 8 } /*ARGB8888*/
};

CVI_U16 OSD_MAKECOLOR_U16(CVI_U8 r, CVI_U8 g, CVI_U8 b, OSD_COMP_INFO compinfo)
{
	CVI_U8 r1, g1, b1;
	CVI_U16 pixel = 0;
	CVI_U32 tmp = 15;

	r1 = g1 = b1 = 0;
	r1 = r >> (8 - compinfo.rlen);
	g1 = g >> (8 - compinfo.glen);
	b1 = b >> (8 - compinfo.blen);
	while (compinfo.alen) {
		pixel |= (1 << tmp);
		tmp--;
		compinfo.alen--;
	}

	pixel |= (b1 | (g1 << compinfo.blen) | (r1 << (compinfo.blen + compinfo.glen)));
	return pixel;
}

CVI_U16 OSD_MAKECOLOR_U16_A(CVI_U8 a, CVI_U8 r, CVI_U8 g, CVI_U8 b, OSD_COMP_INFO compinfo)
{
	CVI_U8 a1, r1, g1, b1;
	CVI_U16 pixel = 0;

	a1 = r1 = g1 = b1 = 0;
	a1 = a >> (8 - compinfo.alen);
	r1 = r >> (8 - compinfo.rlen);
	g1 = g >> (8 - compinfo.glen);
	b1 = b >> (8 - compinfo.blen);

	pixel = (b1 | (g1 << compinfo.blen) | (r1 << (compinfo.blen + compinfo.glen))
		| (a1 << (compinfo.rlen + compinfo.glen + compinfo.blen)));
	return pixel;
}

CVI_U8 find_bitshift(CVI_U32 mask)
{
	CVI_U8 j;

	for (j = 0; !((mask >> j) & 1) && j < 32; ++j)
		;

	return j;
}

CVI_S32 GetBmpInfo(const char *filename, OSD_BITMAPFILEHEADER *pBmpFileHeader, OSD_BITMAPINFO *pBmpInfo)
{
	FILE *pFile;
	CVI_U16 bfType;

	if (filename == NULL) {
		printf("OSD_LoadBMP: filename=NULL\n");
		return -1;
	}

	pFile = fopen((char *)filename, "rb");
	if (pFile == NULL) {
		printf("Open file failed:%s!\n", filename);
		return -1;
	}

	fread(&bfType, 1, sizeof(bfType), pFile);
	if (bfType != 0x4d42) {
		printf("not bitmap file\n");
		fclose(pFile);
		return -1;
	}

	/* Bitmap File Header */
	fread(pBmpFileHeader, 1, sizeof(OSD_BITMAPFILEHEADER), pFile);
	/* BITMAPInfo - Defines the dimension and color information for a DIBs */
	fread(pBmpInfo, 1, sizeof(OSD_BITMAPINFO), pFile);
	fclose(pFile);

	printf("bmp width(%d) height(%d) bpp(%d) compression(%d)\n"
		, pBmpInfo->bmiHeader.biWidth, pBmpInfo->bmiHeader.biHeight, pBmpInfo->bmiHeader.biBitCount
		, pBmpInfo->bmiHeader.biCompression);
	if (pBmpInfo->bmiHeader.biCompression == 3)
		printf("bitmask a(%#x) r(%#x) g(%#x) b(%#x)\n", pBmpInfo->bitfield.a_mask
			, pBmpInfo->bitfield.r_mask, pBmpInfo->bitfield.g_mask, pBmpInfo->bitfield.b_mask);
	return 0;
}

int LoadBMP(const char *filename, OSD_LOGO_T *pVideoLogo, OSD_COLOR_FMT_E enFmt, CVI_BOOL update_logo)
{
	FILE *pFile;
	CVI_U16 i, j;

	CVI_U32 w, h;
	CVI_U16 Bpp;

	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;

	CVI_U8 *pOrigBMPBuf;
	CVI_U8 *pRGBBuf;
	CVI_U32 stride;
	CVI_U8 r, g, b, a;
	CVI_U8 *pStart;
	CVI_U16 *pDst;
	CVI_U32 pxl;
	CVI_U8 color_shifts[4] = { 0 };

	if (filename == NULL) {
		printf("OSD_LoadBMP: filename=NULL\n");
		return -1;
	}

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		return -1;
	}

	Bpp = bmpInfo.bmiHeader.biBitCount / 8;

	switch (bmpInfo.bmiHeader.biCompression) {
	case BI_RGB:
		printf("Non-compressed bitmap file!\n");
		break;
	case BI_RLE4:
	case BI_RLE8:
		printf("only support non-compressed or bitfile bitmap file!\n");
		return -1;
	case BI_BITFIELDS:
		printf("Bitfile bitmap file!\n");

		if ((Bpp != 2) && (Bpp != 4)) {
			printf("Compression(3):RGB bitmap bitfiled format bpp(%d) not supported!\n", Bpp);
			return -1;
		}

		printf("bitmask a(%#x) r(%#x) g(%#x) b(%#x)\n", bmpInfo.bitfield.a_mask
				, bmpInfo.bitfield.r_mask, bmpInfo.bitfield.g_mask, bmpInfo.bitfield.b_mask);

		color_shifts[0] = find_bitshift(bmpInfo.bitfield.a_mask);
		color_shifts[1] = find_bitshift(bmpInfo.bitfield.r_mask);
		color_shifts[2] = find_bitshift(bmpInfo.bitfield.g_mask);
		color_shifts[3] = find_bitshift(bmpInfo.bitfield.b_mask);
	}

	if (bmpInfo.bmiHeader.biHeight < 0) {
		printf("bmpInfo.bmiHeader.biHeight < 0\n");
		return -1;
	}
	if (bmpInfo.bmiHeader.biHeight < 0) {
		printf("bmpInfo.bmiHeader.biHeight < 0\n");
		return -1;
	}

	pFile = fopen((char *)filename, "rb");
	if (pFile == NULL) {
		printf("Open file failed:%s!\n", filename);
		return -1;
	}

	w = bmpInfo.bmiHeader.biWidth;
	h = ((bmpInfo.bmiHeader.biHeight > 0) ? bmpInfo.bmiHeader.biHeight : (-bmpInfo.bmiHeader.biHeight));

	stride = w * Bpp;
	/* ALIGN for 32. */
	if (stride % 4) {
		stride = (stride & 0xfffc) + 4;
	}

	if (update_logo) {
		pVideoLogo->width = w;
		pVideoLogo->height = h;
		pVideoLogo->stride = (enFmt == OSD_COLOR_FMT_8BIT_MODE) ? w :
				(enFmt >= OSD_COLOR_FMT_RGB888) ? w << 2 : w << 1;
	}

	/* RGB8888 or RGB1555 or 8BIT_MODE_index */
	pOrigBMPBuf = (CVI_U8 *)malloc(h * stride);
	if (pOrigBMPBuf == NULL) {
		printf("not enough memory to malloc!\n");
		fclose(pFile);
		return -1;
	}

	pRGBBuf = pVideoLogo->pRGBBuffer;

	if (h > pVideoLogo->height) {
		printf("Bitmap's height(%d) is bigger than canvas's height(%d). Load bitmap error!\n", h,
		       pVideoLogo->height);
		free(pOrigBMPBuf);
		fclose(pFile);
		return -1;
	}

	if (w > pVideoLogo->width) {
		printf("Bitmap's width(%d) is bigger than canvas's width(%d). Load bitmap error!\n", w,
		       pVideoLogo->width);
		free(pOrigBMPBuf);
		fclose(pFile);
		return -1;
	}

	fseek(pFile, bmpFileHeader.bfOffBits, 0);
	if (fread(pOrigBMPBuf, 1, h * stride, pFile) != (h * stride)) {
		printf("fread (%d*%d)error!line:%d\n", h, stride, __LINE__);
		perror("fread:");
	}

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if (Bpp == 3) {
				switch (enFmt) {
				case OSD_COLOR_FMT_RGB444:
				case OSD_COLOR_FMT_RGB555:
				case OSD_COLOR_FMT_RGB565:
				case OSD_COLOR_FMT_RGB1555:
				case OSD_COLOR_FMT_RGB4444:
					/* start color convert */
					pStart = pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp;
					pDst = (CVI_U16 *)(pRGBBuf + i * pVideoLogo->stride + j * 2);
					b = *(pStart);
					g = *(pStart + 1);
					r = *(pStart + 2);
					*pDst = OSD_MAKECOLOR_U16(r, g, b, s_OSDCompInfo[enFmt]);

					break;

				case OSD_COLOR_FMT_RGB888:
				case OSD_COLOR_FMT_RGB8888:
					memcpy(pRGBBuf + i * pVideoLogo->stride + j * 4,
					       pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp, Bpp);
					*(pRGBBuf + i * pVideoLogo->stride + j * 4 + 3) = 0xff; /*alpha*/
					break;

				default:
					printf("file(%s), line(%d), no such format!\n", __FILE__, __LINE__);
					break;
				}
			} else if ((Bpp == 2) || (Bpp == 4)) {
				pStart = pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp;
				pDst = (CVI_U16 *)(pRGBBuf + i * pVideoLogo->stride + j * 2);
				pxl = *(CVI_U32 *)pStart;
				a = (pxl & bmpInfo.bitfield.a_mask) >> color_shifts[0];
				r = (pxl & bmpInfo.bitfield.r_mask) >> color_shifts[1];
				g = (pxl & bmpInfo.bitfield.g_mask) >> color_shifts[2];
				b = (pxl & bmpInfo.bitfield.b_mask) >> color_shifts[3];
				if (i == 0 && j == 0) {
					printf("Func: %s, line:%d.\n", __func__, __LINE__);
					printf("Bpp: %d, bmp stride: %d, Canvas stride: %d, h:%d, w:%d.\n",
					       Bpp, stride, pVideoLogo->stride, i, j);
				}
				*pDst = OSD_MAKECOLOR_U16_A(a, r, g, b, s_OSDCompInfo[enFmt]);
			} else if (Bpp == 1) {
				//For OSD_COLOR_FMT_8BIT_MODE case
				memcpy(pRGBBuf + i * pVideoLogo->stride + j * 1,
				       pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp, Bpp);
			} else
				printf("Error Bpp: %d in Func: %s, line:%d\n",	Bpp, __func__, __LINE__);
		}
	}

	free(pOrigBMPBuf);
	pOrigBMPBuf = NULL;

	fclose(pFile);
	return 0;
}

char *GetExtName(char *filename)
{
	char *pret = NULL;
	CVI_U32 fnLen;

	if (filename == NULL) {
		printf("filename can't be null!");
		return NULL;
	}

	fnLen = strlen(filename);
	while (fnLen) {
		pret = filename + fnLen;
		if (*pret == '.')
			return (pret + 1);

		fnLen--;
	}

	return pret;
}

int LoadImageEx(const char *filename, OSD_LOGO_T *pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
	char *ext = GetExtName((char *)filename);

	if (ext == CVI_NULL) {
		printf("LoadImageEx error!\n");
		return -1;
	}

	if (strcmp(ext, "bmp") == 0) {
		if (LoadBMP(filename, pVideoLogo, enFmt, CVI_TRUE) != 0) {
			printf("OSD_LoadBMP error!\n");
			return -1;
		}
	} else {
		printf("not supported image file!\n");
		return -1;
	}

	return 0;
}

int LoadCanvasEx(const char *filename, OSD_LOGO_T *pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
	char *ext = GetExtName((char *)filename);

	if (ext == CVI_NULL) {
		printf("LoadCanvasEx error!\n");
		return -1;
	}

	if (strcmp(ext, "bmp") == 0) {
		if (LoadBMP(filename, pVideoLogo, enFmt, CVI_FALSE) != 0) {
			printf("OSD_LoadBMP error!\n");
			return -1;
		}
	} else {
		printf("not supported image file!\n");
		return -1;
	}

	return 0;
}

CVI_S32 CreateSurfaceByBitMap(const CVI_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, CVI_U8 *pu8Virt)
{
	OSD_LOGO_T stLogo;

	stLogo.pRGBBuffer = pu8Virt;
	if (LoadImageEx(pszFileName, &stLogo, pstSurface->enColorFmt) < 0) {
		printf("load bmp error!\n");
		return -1;
	}

	pstSurface->u16Height = stLogo.height;
	pstSurface->u16Width = stLogo.width;
	pstSurface->u16Stride = stLogo.stride;

	return 0;
}

CVI_S32 CreateSurfaceByCanvas(const CVI_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, CVI_U8 *pu8Virt
			     , CVI_U32 u32Width, CVI_U32 u32Height, CVI_U32 u32Stride)
{
	OSD_LOGO_T stLogo;

	stLogo.pRGBBuffer = pu8Virt;
	stLogo.width = u32Width;
	stLogo.height = u32Height;
	stLogo.stride = u32Stride;
	if (LoadCanvasEx(pszFileName, &stLogo, pstSurface->enColorFmt) < 0) {
		printf("load bmp error!\n");
		return -1;
	}

	pstSurface->u16Height = u32Height;
	pstSurface->u16Width = u32Width;
	pstSurface->u16Stride = u32Stride;

	return 0;
}
