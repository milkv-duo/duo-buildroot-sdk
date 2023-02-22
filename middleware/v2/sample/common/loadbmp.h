#ifndef __LOAD_BMP_H__
#define __LOAD_BMP_H__

#include <linux/cvi_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* the color format OSD supported */
typedef enum _OSD_COLOR_FMT_E {
	OSD_COLOR_FMT_RGB444 = 0,
	OSD_COLOR_FMT_RGB4444 = 1,
	OSD_COLOR_FMT_RGB555 = 2,
	OSD_COLOR_FMT_RGB565 = 3,
	OSD_COLOR_FMT_RGB1555 = 4,
	OSD_COLOR_FMT_8BIT_MODE = 5,
	OSD_COLOR_FMT_RGB888 = 6,
	OSD_COLOR_FMT_RGB8888 = 7,
	OSD_COLOR_FMT_BUTT
} OSD_COLOR_FMT_E;

typedef struct _OSD_RGB_S {
	CVI_U8 u8B;
	CVI_U8 u8G;
	CVI_U8 u8R;
	CVI_U8 u8Reserved;
} OSD_RGB_S;

typedef struct _OSD_SURFACE_S {
	OSD_COLOR_FMT_E enColorFmt; /* color format */
	CVI_U16 u16Height; /* operation height */
	CVI_U16 u16Width; /* operation width */
	CVI_U16 u16Stride; /* surface stride */
	CVI_U16 u16Reserved;
} OSD_SURFACE_S;

typedef struct _Logo {
	CVI_U32 width; /* out */
	CVI_U32 height; /* out */
	CVI_U32 stride; /* in */
	CVI_U8 *pRGBBuffer; /* in/out */
} OSD_LOGO_T;

typedef struct _BITMAPFILEHEADER {
	CVI_U32 bfSize; /* The size of the BMP file in bytes */
	CVI_U16 bfReserved1; /* Reserved; actual value depends on the application that creates the image */
	CVI_U16 bfReserved2; /* Reserved; actual value depends on the application that creates the image */
	/* The offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found. */
	CVI_U32 bfOffBits;
} OSD_BITMAPFILEHEADER;

/* Windows BITMAPINFOHEADER */
typedef struct _BITMAPINFOHEADER {
	CVI_U32 biSize; /* the size of this header, in bytes (40) */
	CVI_U32 biWidth; /*	the bitmap width in pixels (signed integer) */
	CVI_S32 biHeight; /* the bitmap height in pixels (signed integer)  */
	CVI_U16 biPlanes; /* the number of color planes (must be 1) */

	/* the number of bits per pixel, which is the color depth of the image. */
	/* Typical values are 1, 4, 8, 16, 24 and 32. */
	CVI_U16 biBitCount;

	/* the compression method being used. See the next table for a list of possible values */
	CVI_U32 biCompression;

	/* the image size. This is the size of the raw bitmap data; */
	/* a dummy 0 can be given for BI_RGB bitmaps. */
	CVI_U32 biSizeImage;

	/* the horizontal resolution of the image. (pixel per metre, signed integer) */
	CVI_U32 biXPelsPerMeter;

	/* the vertical resolution of the image. (pixel per metre, signed integer) */
	CVI_U32 biYPelsPerMeter;

	/* the number of colors in the color palette, or 0 to default to 2^n. */
	/* n is color depth of the image */
	CVI_U32 biClrUsed;

	/* the number of important colors used, or 0 when every color is important; generally ignored */
	CVI_U32 biClrImportant;
} OSD_BITMAPINFOHEADER;

typedef struct _RGBQUAD {
	CVI_U8 rgbBlue;
	CVI_U8 rgbGreen;
	CVI_U8 rgbRed;
	CVI_U8 rgbReserved;
} OSD_RGBQUAD;

typedef struct _BITFIELD {
	CVI_U32 r_mask;
	CVI_U32 g_mask;
	CVI_U32 b_mask;
	CVI_U32 a_mask;
} OSD_BITFIELD;

typedef struct _BITMAPINFO {
	OSD_BITMAPINFOHEADER bmiHeader;
	union {
		OSD_RGBQUAD bmiColors[256]; /* Color palette */
		OSD_BITFIELD bitfield;
	};
} OSD_BITMAPINFO;

typedef struct _OSD_COMPONENT_INFO_S {
	int alen;
	int rlen;
	int glen;
	int blen;
} OSD_COMP_INFO;

CVI_S32 CreateSurfaceByBitMap(const CVI_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, CVI_U8 *pu8Virt);
CVI_S32 CreateSurfaceByCanvas(const CVI_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, CVI_U8 *pu8Virt, CVI_U32 u32Width,
			     CVI_U32 u32Height, CVI_U32 u32Stride);
CVI_S32 GetBmpInfo(const CVI_CHAR *filename, OSD_BITMAPFILEHEADER *pBmpFileHeader, OSD_BITMAPINFO *pBmpInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __LOAD_BMP_H__*/
