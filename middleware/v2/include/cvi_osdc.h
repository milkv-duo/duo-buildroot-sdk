#ifndef __CVI_OSDC_H__
#define __CVI_OSDC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <cvi_comm_osdc.h>

uint32_t CVI_OSDC_EstCmprCanvasSize(OSDC_Canvas_Attr_S * canvas, OSDC_DRAW_OBJ_S * objs, uint32_t obj_num);

int CVI_OSDC_DrawCmprCanvas(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *objs, uint32_t obj_num,
				uint8_t *obuf, uint32_t buf_size, uint32_t *p_osize);

void CVI_OSDC_SetRectObjAttr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				int pt_x, int pt_y, int width, int height, bool is_filled, int thickness);

void CVI_OSDC_SetRectObjAttrEx(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				OSDC_RECT_ATTR_S *rects, int num, bool is_filled);

void CVI_OSDC_SetBitmapObjAttr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj_attr, uint8_t *buf,
				  int pt_x, int pt_y, int width, int height, bool is_cmpr);

void CVI_OSDC_SetLineObjAttr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				int pt_x0, int pt_y0, int pt_x1, int pt_y1, int thickness);

void CVI_OSDC_SetLineObjAttrEx(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				OSDC_POINT_ATTR_S *points, int num, int thickness);

int CVI_OSDC_CmprBitmap(OSDC_Canvas_Attr_S *canvas, uint8_t *ibuf, uint8_t *obuf, int width, int height,
				int buf_size, uint32_t *p_osize);

extern uint32_t CVI_OSDC_est_cmpr_canvas_size(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *objs, uint32_t obj_num);

extern int CVI_OSDC_draw_cmpr_canvas(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *objs, uint32_t obj_num,
			      uint8_t *obuf, uint32_t buf_size, uint32_t *p_osize);

extern void CVI_OSDC_set_rect_obj_attr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				int pt_x, int pt_y, int width, int height, bool is_filled, int thickness);

extern void CVI_OSDC_set_bitmap_obj_attr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj_attr, uint8_t *buf,
				  int pt_x, int pt_y, int width, int height, bool is_cmpr);

extern void CVI_OSDC_set_line_obj_attr(OSDC_Canvas_Attr_S *canvas, OSDC_DRAW_OBJ_S *obj, uint32_t color_code,
				int pt_x0, int pt_y0, int pt_x1, int pt_y1, int thickness);

extern int CVI_OSDC_cmpr_bitmap(OSDC_Canvas_Attr_S *canvas, uint8_t *ibuf, uint8_t *obuf, int width, int height,
			 int buf_size, uint32_t *p_osize);
#if (CMPR_CANVAS_DBG)
int CVI_OSDC_draw_canvas_raw_buffer(OSDC_Canvas_Attr_S &canvas, vector<DRAW_OBJ> obj_vec, uint8_t *obuf);
int CVI_OSDC_draw_canvas_raw_buffer2(OSDC_Canvas_Attr_S &canvas, vector<DRAW_OBJ> obj_vec, uint8_t *obuf);
#endif


#ifdef __cplusplus
}
#endif

#endif /*__CVI_OSDC_H__ */
