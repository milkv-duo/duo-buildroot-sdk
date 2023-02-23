/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_osdc.h
 * Description:
 *   The common data type defination for osdc module.
 */

#ifndef __CVI_COMM_OSDC_H__
#define __CVI_COMM_OSDC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum _OSDC_OSD_FORMAT_E {
	OSD_ARGB8888 = 0,
	OSD_ARGB4444 = 4,
	OSD_ARGB1555 = 5,
	OSD_LUT8 = 8,
	OSD_LUT4 = 10,
	NUM_OF_FORMAT
} OSDC_OSD_FORMAT_E;

typedef enum _OSDC_DRAW_OBJ_TYPE_E {
	RECT = 0,
	STROKE_RECT,
	BIT_MAP,
	LINE,
	CMPR_BIT_MAP,
	NUM_OF_DRAW_OBJ
} OSDC_DRAW_OBJ_TYPE_E;

typedef struct _OSDC_Canvas_Attr_S {
	int width;
	int height;
	OSDC_OSD_FORMAT_E format;
	uint32_t bg_color_code;
} OSDC_Canvas_Attr_S;

typedef struct _OSDC_RECT_ATTR_S {
	int x;
	int y;
	int width;
	int height;
	int thickness;
} OSDC_RECT_ATTR_S;

typedef struct _OSDC_LINE_ATTR_S {
	float _mx; // slope of two end-point vector
	float _bx[2];
	float _by[2];
	float _ex[2];
	float _ey[2];
	float ts_h; // thickness proj. on horizontal slice
} OSDC_LINE_ATTR_S;

typedef struct _OSDC_BITMAP_ATTR_S {
	OSDC_RECT_ATTR_S rect;
	union {
		uint32_t stride;
		uint32_t bs_offset;
	};
} OSDC_BITMAP_ATTR_S;

typedef union _OSDC_COLOR_S {
	uint32_t code;
	uint8_t *buf;
} OSDC_COLOR_S;

typedef struct _OSDC_DRAW_OBJ_S {
	OSDC_DRAW_OBJ_TYPE_E type;
	union {
		OSDC_RECT_ATTR_S rect;
		OSDC_LINE_ATTR_S line;
		OSDC_BITMAP_ATTR_S bitmap;
	};
	OSDC_COLOR_S color;
	int _max_y;
	int _min_y;
} OSDC_DRAW_OBJ_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_OSDC_H__ */
