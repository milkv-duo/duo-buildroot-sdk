#ifndef _CVI_DWA_CFG_H_
#define _CVI_DWA_CFG_H_

#define DWA_INTR_NUM 157

// coordinate in S COOR_N.COOR_M
#define DEWARP_COORD_MBITS 13
#define DEWARP_COORD_NBITS 18
#define NUMBER_Y_LINE_A_TILE 4
#define DIV_0_BIT_LSHIFT 0			// default 16
#define DIV_1_BIT_LSHIFT 18
#define INTERPOLATION_PRECISION_BITS 5
#define COEFFICIENT_PHASE_NUM (1 << INTERPOLATION_PRECISION_BITS)
#define INTERPOLATION_COEF_BITS 8		// SN
#define INTERPOLATION_COEF_FBITS 6

#define MESH_ID_FFS	0xfffa	// frame start
#define MESH_ID_FSS 0xfffb	// slice start
#define MESH_ID_FTS	0xfffc	// tile start
#define MESH_ID_FTE 0xfffd	// tile end
#define MESH_ID_FSE 0xfffe	// slice end
#define MESH_ID_FFE 0xffff	// frame end

#define CACHE_LINE_SIZE 16	// 16 bytes
#define CACHE_DATA_SIZE 128 // 128 bytes i.e., 16 bytes with 8 continue bursts,
#define CACHE_SET_NUM	16	// 16 sets
#define CACHE_WAY_NUM	4	// 4 ways
#define CACHE_X_ADDR	1	// 4 3 2 1
#define CACHE_Y_ADDR	3	// 0 1 2 3
#define CACHE_REPLACE_FIFO
//#define CACHE_REPLACE_LRU

#define YUV400		2
#define YUV420p		0
#define RGB888p		1

#define HTILE_MODE

struct dwa_buf {
	u32 addrl;
	u32 addrh;
	u32 pitch;
	u16 offset_x;
	u16 offset_y;
};

struct dwa_cfg {
	u8 pix_fmt;         // 0: YUV420, 1: RGB, 2: Y only
	u8 output_target;   // 0: to scaler, 1: to DRAM

	u32 bgcolor;        // R[0:7], G[15:8], B[23:16]

	int src_width;
	int src_height;
	struct dwa_buf src_buf[3];
	int dst_width;
	int dst_height;
	struct dwa_buf dst_buf[3];
	u64 mesh_id;
};

#endif // _CVI_DWA_CFG_H_
