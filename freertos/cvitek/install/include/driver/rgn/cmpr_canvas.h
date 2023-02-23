#ifndef __CMPR_CANVAS__
#define __CMPR_CANVAS__

#include <float.h>
#include <string.h>
#include <math.h>
#include "osd_cmpr.h"
#include "cvi_list.h"

#define BUF_GUARD_SIZE (1 << 12)
#define OSDEC_RL_BD (6)
#define OSDEC_PAL_BD (3)
#define OSDEC_MAX_RL (1 << OSDEC_RL_BD)

#define MIN_THICKNESS (1)
#define MAX_THICKNESS (32)

#define BG_COLOR_CODE (2)
#define CMPR_CANVAS_DBG (0)

typedef enum {
	RECT = 0,
	STROKE_RECT,
	BIT_MAP,
	LINE,
	CMPR_BIT_MAP,
	NUM_OF_DRAW_OBJ
} DRAW_OBJ_TYPE;

typedef struct {
	int width;
	int height;
	OSD_FORMAT format;
	uint32_t bg_color_code;
} Canvas_Attr;

typedef union _COLOR {
	uint32_t code;
	uint8_t *buf;
} COLOR;

typedef struct {
	int x0; // start position
	int x1; // end position
	uint16_t obj_id;
} OBJ_SLICE;

typedef struct {
	OBJ_SLICE slice;
	int num;
	dlist_t item;
} SLICE_LIST;

typedef struct {
	bool is_const;
	bool is_cmpr;
	uint16_t width;
	union {
		uint32_t stride;
		uint16_t *bs_len;
	};
	COLOR color;
	uint16_t id;
} SEGMENT;

typedef struct {
	SEGMENT segment;
	int num;
	dlist_t item;
} SEGMENT_LIST;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int thickness;
} RECT_ATTR;

typedef struct {
	RECT_ATTR rect;
	union {
		uint32_t stride;
		uint32_t bs_offset;
	};
} BITMAP_ATTR;

typedef struct {
	float _mx; // slope of two end-point vector
	float _bx[2];
	float _by[2];
	float _ex[2];
	float _ey[2];
	float ts_h; // thickness proj. on horizontal slice
} LINE_ATTR;

typedef struct {
	DRAW_OBJ_TYPE type;
	union {
		RECT_ATTR rect;
		LINE_ATTR line;
		BITMAP_ATTR bitmap;
	};
	COLOR color;
	int _max_y;
	int _min_y;
} DRAW_OBJ;

typedef struct {
	OSDCmpr_Ctrl osdCmpr_ctrl;
	StreamBuffer bitstream;
	RGBA last_color;
	int rl_cnt;
	MODE_TYPE md;
	CODE code;
} Cmpr_Canvas_Ctrl;

uint32_t est_cmpr_canvas_size(Canvas_Attr *canvas, DRAW_OBJ *objs, uint32_t obj_num);

int draw_cmpr_canvas(Canvas_Attr *canvas, DRAW_OBJ *objs, uint32_t obj_num,
			uint8_t *obuf, int buf_size, uint32_t *p_osize);

void set_rect_obj_attr(DRAW_OBJ *obj, Canvas_Attr *canvas, uint32_t color_code,
			   int pt_x, int pt_y, int width, int height,
			   bool is_filled, int thickness);
void set_bitmap_obj_attr(DRAW_OBJ *obj_attr, Canvas_Attr *canvas, uint8_t *buf,
			 int pt_x, int pt_y, int width, int height,
			 bool is_cmpr);
void set_line_obj_attr(DRAW_OBJ *obj, Canvas_Attr *canvas, uint32_t color_code,
			   int pt_x0, int pt_y0, int pt_x1, int pt_y1,
			   int thickness);
int cmpr_bitmap(Canvas_Attr *canvas, uint8_t *ibuf, uint8_t *obuf, int width,
		int height, int buf_size, uint32_t *p_osize);

#if (CMPR_CANVAS_DBG)
int draw_canvas_raw_buffer(Canvas_Attr *canvas, DRAW_OBJ *obj_vec,
			   uint8_t *obuf);
int draw_canvas_raw_buffer2(Canvas_Attr *canvas, DRAW_OBJ *obj_vec,
				uint8_t *obuf);
#endif

//==============================================================================================
//CVI interface
#ifdef __cplusplus
extern "C"
{
#endif

uint32_t CVI_OSDC_est_cmpr_canvas_size(Canvas_Attr *canvas, DRAW_OBJ *objs, uint32_t obj_num);

int CVI_OSDC_draw_cmpr_canvas(Canvas_Attr *canvas, DRAW_OBJ *objs, uint32_t obj_num,
				uint8_t *obuf, uint32_t buf_size, uint32_t *p_osize);

void CVI_OSDC_set_rect_obj_attr(Canvas_Attr *canvas, DRAW_OBJ *obj, uint32_t color_code,
				int pt_x, int pt_y, int width, int height, bool is_filled, int thickness);

void CVI_OSDC_set_bitmap_obj_attr(Canvas_Attr *canvas, DRAW_OBJ *obj_attr, uint8_t *buf,
				  int pt_x, int pt_y, int width, int height, bool is_cmpr);

void CVI_OSDC_set_line_obj_attr(Canvas_Attr *canvas, DRAW_OBJ *obj, uint32_t color_code,
				int pt_x0, int pt_y0, int pt_x1, int pt_y1, int thickness);

int CVI_OSDC_cmpr_bitmap(Canvas_Attr *canvas, uint8_t *ibuf, uint8_t *obuf, int width, int height,
				int buf_size, uint32_t *p_osize);

#if (CMPR_CANVAS_DBG)
int CVI_OSDC_draw_canvas_raw_buffer(Canvas_Attr *canvas, DRAW_OBJ *obj_vec, uint8_t *obuf);
int CVI_OSDC_draw_canvas_raw_buffer2(Canvas_Attr *canvas, DRAW_OBJ *obj_vec, uint8_t *obuf);
#endif


#ifdef __cplusplus
}
#endif


#endif /* __CMPR_CANVAS__ */
