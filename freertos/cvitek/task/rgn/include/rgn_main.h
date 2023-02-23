#ifndef __RGN_MAIN_H__
#define __RGN_MAIN_H__

#include <cvi_comm_video.h>
#include "cvi_comm_osdc.h"

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif

#define RGN_CMPR_MAX_OBJ_NUM 10
#define RGN_CMPR_MIN_SIZE 128000

typedef enum _CMDQU_CB_CMD_E {
	CMDQU_CB_RGN_COMPRESS,
	CMDQU_CB_RGN_GET_COMPRESS_SIZE,
	CMDQU_CB_RGN_COMPRESS_DONE,
	CMDQU_CB_MAX
} CMDQU_CB_CMD_E;

typedef enum _RGN_CMPR_FORMAT_E {
	CMPR_ARGB8888 = 0,
	CMPR_ARGB4444 = 4,
	CMPR_ARGB1555 = 5,
	CMPR_LUT8 = 8,
	CMPR_LUT4 = 10,
	NUM_OF_CMPR_FORMAT
} RGN_CMPR_FORMAT_E;

typedef enum _RGN_CMPR_TYPE_E {
	RGN_CMPR_RECT = 0,
	RGN_CMPR_BIT_MAP,
	RGN_CMPR_LINE,
	RGN_CMPR_BUTT
} RGN_CMPR_TYPE_E;

typedef struct _RGN_LINE_ATTR_S {
	POINT_S stPointStart;
	POINT_S stPointEnd;
	CVI_U32 u32Thick;
	CVI_U32 u32Color;
} RGN_LINE_ATTR_S;

typedef struct _RGN_RECT_ATTR_S {
	RECT_S stRect;
	CVI_U32 u32Thick;
	CVI_U32 u32Color;
	CVI_U32 u32IsFill;
} RGN_RECT_ATTR_S;

typedef struct _RGN_BITMAP_ATTR_S {
	RECT_S stRect;
	CVI_U32 u32BitmapPAddr;
} RGN_BITMAP_ATTR_S;

typedef struct _RGN_CMPR_OBJ_ATTR_S {
	RGN_CMPR_TYPE_E enObjType;
	union {
		RGN_LINE_ATTR_S stLine;
		RGN_RECT_ATTR_S stRgnRect;
		RGN_BITMAP_ATTR_S stBitmap;
	};
} RGN_CMPR_OBJ_ATTR_S;

typedef struct _RGN_CANVAS_CMPR_ATTR_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	CVI_U32 u32BgColor;
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32 u32BsSize;
	CVI_U32 u32ObjNum;
} RGN_CANVAS_CMPR_ATTR_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* __RGN_MAIN_H__ */
