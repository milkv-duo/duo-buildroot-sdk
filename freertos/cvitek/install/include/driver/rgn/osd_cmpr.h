#ifndef __OSD_CMPR_H__
#define __OSD_CMPR_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/param.h>

// DataType-free color field copy
#define CPY_C(in, out)                                                         \
	{                                                                      \
		out.r = in.r;                                                  \
		out.g = in.g;                                                  \
		out.b = in.b;                                                  \
		out.a = in.a;                                                  \
	}

#define HDR_SZ (8)

typedef enum {
	OSD_ARGB8888 = 0,
	OSD_ARGB4444 = 4,
	OSD_ARGB1555 = 5,
	OSD_LUT8 = 8,
	OSD_LUT4 = 10,
	NUM_OF_FORMAT
} OSD_FORMAT;

typedef struct {
	int img_width;
	int img_height;
	bool palette_mode_en;
	bool zeroize_by_alpha;
	int rgb_trunc_bit;
	int alpha_trunc_bit;
	int run_len_bd;
	int palette_idx_bd;
	OSD_FORMAT osd_format;
	bool hdr_en;
} OSDCmpr_Cfg;

typedef struct {
	uint8_t *stream; // stream buffer pointer
	int bit_pos; // current pointer (in bit)
	int buf_size; // in byte
	int status;
} StreamBuffer;

int clip(int data, int min, int max);
void init_stream(StreamBuffer *bs, const uint8_t *buf, int buf_size,
		 bool read_only);
void write_stream(StreamBuffer *bs, uint8_t *src, int bit_len);
void parse_stream(StreamBuffer *bs, uint8_t *dest, int bit_len, bool read_only);
void move_stream_ptr(StreamBuffer *bs, int bit_len);

uint8_t get_bit_val(uint8_t *buf, int byte_idx, int bit_idx);

typedef union {
	struct {
		uint8_t g;
		uint8_t b;
		uint8_t r;
		uint8_t a;
	};
	uint32_t code;
} RGBA;

typedef union {
	struct {
		uint16_t g : 4;
		uint16_t b : 4;
		uint16_t r : 4;
		uint16_t a : 4;
	};
	uint16_t code;
} ARGB4444;

typedef union {
	struct {
		uint16_t g : 5;
		uint16_t b : 5;
		uint16_t r : 5;
		uint16_t a : 1;
	};
	uint16_t code;
} ARGB1555;

typedef union {
	RGBA color;
	int palette_idx;
} CODE;

typedef enum {
	Literal = 0,
	Palette,
	Literal_RL,
	Palette_RL,
	NUM_OF_MODE
} MODE_TYPE;

typedef struct {
	RGBA *color;
	int num;
} PaletteRGBA;

typedef struct {
	int reg_image_width;
	int reg_image_height;
	bool reg_zeroize_by_alpha;
	int reg_rgb_trunc_bit;
	int reg_alpha_trunc_bit;
	bool reg_palette_mode_en;
	int reg_run_len_bd;
	int reg_palette_idx_bd;
	OSD_FORMAT reg_osd_format;
	int pel_sz;
	PaletteRGBA palette_cache;
	int bs_buf_size;
	uint8_t *bsbuf; // intermediate bitstream buffer
} OSDCmpr_Ctrl;

// RGBA get_color(uint8_t *ptr, OSD_FORMAT format = OSD_ARGB8888);
// void set_color(uint8_t *ptr, RGBA color, OSD_FORMAT format = OSD_ARGB8888);
RGBA get_color(uint8_t *ptr, OSD_FORMAT format);
void set_color(uint8_t *ptr, RGBA color, OSD_FORMAT format);

void osd_cmpr_frame_init(OSDCmpr_Ctrl *p_ctrl);
int osd_cmpr_enc_one_frame(uint8_t *ibuf, uint8_t *obs, OSDCmpr_Ctrl *p_ctrl);
void osd_cmpr_dec_one_frame(uint8_t *bsbuf, size_t bs_size, uint8_t *obuf,
			    OSDCmpr_Ctrl *p_ctrl);
void osd_cmpr_enc_header(uint8_t *hdrbuf, OSDCmpr_Ctrl *p_ctrl);
void osd_cmpr_dec_header(uint8_t *hdrbuf, OSDCmpr_Ctrl *p_ctrl);

void osd_cmpr_setup(OSDCmpr_Ctrl *p_ctrl, OSDCmpr_Cfg *p_cfg);
void osd_cmpr_enc_const_pixel(RGBA cur_c, RGBA *last_c, int *rl_cnt,
			      MODE_TYPE *md, CODE *code, uint16_t *length,
			      bool is_force_new_run, uint16_t max_run_len,
			      OSDCmpr_Ctrl *p_ctrl, StreamBuffer *bitstream);
void osd_cmpr_enc_followed_run(RGBA cur_c, int *rl_cnt, MODE_TYPE *md,
			       CODE *code, uint16_t *length, uint16_t max_run_len,
			       OSDCmpr_Ctrl *p_ctrl, StreamBuffer *bitstream);

size_t osd_cmpr_get_pixel_sz(OSD_FORMAT format);
size_t osd_cmpr_get_bs_buf_max_sz(int pel_num, int pel_sz);
size_t osd_cmpr_get_header_sz(void);

void osd_cmpr_debug_frame_compare(OSDCmpr_Ctrl *p_ctrl, uint8_t *buf0,
				  uint8_t *buf1);

void palette_cache_init(PaletteRGBA *cache, int cache_sz);
int palette_cache_lookup_color(PaletteRGBA *cache, RGBA color);
void palette_cache_lru_update(PaletteRGBA *cache, int index);
void palette_cache_push_color(PaletteRGBA *cache, RGBA color);
#endif /* __OSD_CMPR_H__ */
