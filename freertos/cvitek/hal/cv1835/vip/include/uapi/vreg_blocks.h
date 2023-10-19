#ifndef _VREG_BLOCKS_H_
#define _VREG_BLOCKS_H_


/******************************************/
/*          CMDSET Common Define          */
/******************************************/

struct ISPCQ_ADMA_DESC_T {
	union {
		uint64_t    cmdset_addr;
		uint64_t    link_addr;
	};
	uint32_t    cmdset_size;
	union {
		uint32_t _RSV0;
		struct {
			uint32_t END    : 1;
			uint32_t _RSV1  : 1;
			uint32_t _RSV2  : 1;
			uint32_t LINK   : 1;
		} flag;
	};
};

union CMDSET_FIELD {
	uint32_t raw;
	struct {
		uint32_t REG_ADDR           : 20;
		uint32_t BWR_MASK           : 4;
		uint32_t FLAG_END           : 1;
		uint32_t FLAG_INT           : 1;
		uint32_t FLAG_LAST          : 1;
		uint32_t FLAG_RSV           : 1;
		uint32_t ACT                : 4;
	} bits;
};

struct VREG_RESV {
	uint32_t                        resv;
	union CMDSET_FIELD              nop;
};


/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_PRE_RAW_TOP_CTRL {
	union REG_PRE_RAW_TOP_CTRL              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_UP {
	union REG_PRE_RAW_UP                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_DUMMY {
	union REG_PRE_RAW_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_POST_NO_RSPD_CYC {
	union REG_PRE_RAW_POST_NO_RSPD_CYC      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC {
	union REG_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FRAME_VLD {
	union REG_PRE_RAW_FRAME_VLD             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_DEBUG_STATE {
	union REG_PRE_RAW_DEBUG_STATE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_ASYNC_STATE {
	union REG_PRE_RAW_ASYNC_STATE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_RGBMAP_0_ASYNC_STATE {
	union REG_PRE_RAW_RGBMAP_0_ASYNC_STATE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_RGBMAP_1_ASYNC_STATE {
	union REG_PRE_RAW_RGBMAP_1_ASYNC_STATE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_T {
	struct VREG_PRE_RAW_TOP_CTRL             TOP_CTRL;
	struct VREG_PRE_RAW_UP                   REG_UP;
	struct VREG_PRE_RAW_DUMMY                DUMMY;
	struct VREG_PRE_RAW_POST_NO_RSPD_CYC     POST_NO_RSPD_CYC;
	struct VREG_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  POST_RGBMAP_NO_RSPD_CYC;
	struct VREG_PRE_RAW_FRAME_VLD            FRAME_VLD;
	struct VREG_PRE_RAW_DEBUG_STATE          DEBUG_STATE;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_PRE_RAW_ASYNC_STATE          ASYNC_STATE;
	struct VREG_PRE_RAW_RGBMAP_0_ASYNC_STATE  RGBMAP_0_ASYNC_STATE;
	struct VREG_PRE_RAW_RGBMAP_1_ASYNC_STATE  RGBMAP_1_ASYNC_STATE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CSI_BDG_CTRL {
	union REG_ISP_CSI_BDG_CTRL              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_SIZE {
	union REG_ISP_CSI_BDG_SIZE              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_UP {
	union REG_ISP_CSI_BDG_UP                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_PAT_GEN_CTRL {
	union REG_ISP_CSI_BDG_PAT_GEN_CTRL      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_PAT_IDX_CTRL {
	union REG_ISP_CSI_BDG_PAT_IDX_CTRL      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_PAT_COLOR_0 {
	union REG_ISP_CSI_BDG_PAT_COLOR_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_PAT_COLOR_1 {
	union REG_ISP_CSI_BDG_PAT_COLOR_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_BACKGROUND_COLOR_0 {
	union REG_ISP_CSI_BDG_BACKGROUND_COLOR_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_BACKGROUND_COLOR_1 {
	union REG_ISP_CSI_BDG_BACKGROUND_COLOR_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FIX_COLOR_0 {
	union REG_ISP_CSI_BDG_FIX_COLOR_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FIX_COLOR_1 {
	union REG_ISP_CSI_BDG_FIX_COLOR_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_MDE_V_SIZE {
	union REG_ISP_CSI_BDG_MDE_V_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_MDE_H_SIZE {
	union REG_ISP_CSI_BDG_MDE_H_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FDE_V_SIZE {
	union REG_ISP_CSI_BDG_FDE_V_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FDE_H_SIZE {
	union REG_ISP_CSI_BDG_FDE_H_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_HSYNC_CTRL {
	union REG_ISP_CSI_BDG_HSYNC_CTRL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_VSYNC_CTRL {
	union REG_ISP_CSI_BDG_VSYNC_CTRL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_TGEN_TT_SIZE {
	union REG_ISP_CSI_BDG_TGEN_TT_SIZE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_TGEN_ENABLE {
	union REG_ISP_CSI_BDG_TGEN_ENABLE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_RT_DEBUG_INFO_PXL_CNT {
	union REG_ISP_CSI_BDG_RT_DEBUG_INFO_PXL_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_RT_DEBUG_INFO_LINE_CNT {
	union REG_ISP_CSI_BDG_RT_DEBUG_INFO_LINE_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FE_DEBUG_INFO_PXL_CNT {
	union REG_ISP_CSI_BDG_FE_DEBUG_INFO_PXL_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_FE_DEBUG_INFO_LINE_CNT {
	union REG_ISP_CSI_BDG_FE_DEBUG_INFO_LINE_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DEBUG_INFO_FRAME_CNT {
	union REG_ISP_CSI_BDG_DEBUG_INFO_FRAME_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_INTERRUPT_CTRL {
	union REG_ISP_CSI_BDG_INTERRUPT_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS {
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DUMMY {
	union REG_ISP_CSI_BDG_DUMMY             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DEBUG_INFO_VSYNC_CNT {
	union REG_ISP_CSI_BDG_DEBUG_INFO_VSYNC_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DEBUG_INFO_SOF_CNT {
	union REG_ISP_CSI_BDG_DEBUG_INFO_SOF_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_NO_RSPD_CYC_CNT {
	union REG_ISP_CSI_BDG_CH0_NO_RSPD_CYC_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_NO_RSPD_CYC_CNT {
	union REG_ISP_CSI_BDG_CH1_NO_RSPD_CYC_CNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DEBUG_STATE {
	union REG_ISP_CSI_BDG_DEBUG_STATE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DMA_DPCM_MODE {
	union REG_ISP_CSI_BDG_DMA_DPCM_MODE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY {
	union REG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WR_URGENT_CTRL {
	union REG_ISP_CSI_BDG_WR_URGENT_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_RD_URGENT_CTRL {
	union REG_ISP_CSI_BDG_RD_URGENT_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CROP_EN {
	union REG_ISP_CSI_BDG_CROP_EN           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_HORZ_CROP {
	union REG_ISP_CSI_BDG_HORZ_CROP         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_VERT_CROP {
	union REG_ISP_CSI_BDG_VERT_CROP         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT {
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CHANNEL_LINE_COUNT {
	union REG_ISP_CSI_BDG_CHANNEL_LINE_COUNT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_HORZ_CROP_SE {
	union REG_ISP_CSI_BDG_HORZ_CROP_SE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_VERT_CROP_SE {
	union REG_ISP_CSI_BDG_VERT_CROP_SE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_T {
	struct VREG_ISP_CSI_BDG_CTRL             CSI_CTRL;
	struct VREG_ISP_CSI_BDG_SIZE             CSI_SIZE;
	struct VREG_ISP_CSI_BDG_UP               CSI_BDG_UP;
	struct VREG_ISP_CSI_BDG_PAT_GEN_CTRL     CSI_PAT_GEN_CTRL;
	struct VREG_ISP_CSI_BDG_PAT_IDX_CTRL     CSI_PAT_IDX_CTRL;
	struct VREG_ISP_CSI_BDG_PAT_COLOR_0      CSI_PAT_COLOR_0;
	struct VREG_ISP_CSI_BDG_PAT_COLOR_1      CSI_PAT_COLOR_1;
	struct VREG_ISP_CSI_BDG_BACKGROUND_COLOR_0  CSI_BACKGROUND_COLOR_0;
	struct VREG_ISP_CSI_BDG_BACKGROUND_COLOR_1  CSI_BACKGROUND_COLOR_1;
	struct VREG_ISP_CSI_BDG_FIX_COLOR_0      CSI_FIX_COLOR_0;
	struct VREG_ISP_CSI_BDG_FIX_COLOR_1      CSI_FIX_COLOR_1;
	struct VREG_ISP_CSI_BDG_MDE_V_SIZE       CSI_MDE_V_SIZE;
	struct VREG_ISP_CSI_BDG_MDE_H_SIZE       CSI_MDE_H_SIZE;
	struct VREG_ISP_CSI_BDG_FDE_V_SIZE       CSI_FDE_V_SIZE;
	struct VREG_ISP_CSI_BDG_FDE_H_SIZE       CSI_FDE_H_SIZE;
	struct VREG_ISP_CSI_BDG_HSYNC_CTRL       CSI_HSYNC_CTRL;
	struct VREG_ISP_CSI_BDG_VSYNC_CTRL       CSI_VSYNC_CTRL;
	struct VREG_ISP_CSI_BDG_TGEN_TT_SIZE     CSI_TGEN_TT_SIZE;
	struct VREG_ISP_CSI_BDG_TGEN_ENABLE      CSI_TGEN_ENABLE;
	struct VREG_ISP_CSI_BDG_RT_DEBUG_INFO_PXL_CNT  CSI_RT_INFO_PXL_CNT;
	struct VREG_ISP_CSI_BDG_RT_DEBUG_INFO_LINE_CNT  CSI_RT_INFO_LINE_CNT;
	struct VREG_ISP_CSI_BDG_FE_DEBUG_INFO_PXL_CNT  CSI_FE_INFO_PXL_CNT;
	struct VREG_ISP_CSI_BDG_FE_DEBUG_INFO_LINE_CNT  CSI_FE_INFO_LINE_CNT;
	struct VREG_ISP_CSI_BDG_DEBUG_INFO_FRAME_CNT  CSI_INFO_FRAME_CNT;
	struct VREG_ISP_CSI_BDG_INTERRUPT_CTRL   CSI_INTERRUPT_CTRL;
	struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS  CSI_INTERRUPT_STATUS;
	struct VREG_ISP_CSI_BDG_DUMMY            CSI_DUMMY;
	struct VREG_ISP_CSI_BDG_DEBUG_INFO_VSYNC_CNT  CSI_INFO_VSYNC_CNT;
	struct VREG_ISP_CSI_BDG_DEBUG_INFO_SOF_CNT  CSI_INFO_SOF_CNT;
	struct VREG_ISP_CSI_BDG_CH0_NO_RSPD_CYC_CNT  CH0_NO_RSPD_CYC_CNT;
	struct VREG_ISP_CSI_BDG_CH1_NO_RSPD_CYC_CNT  CH1_NO_RSPD_CYC_CNT;
	struct VREG_ISP_CSI_BDG_DEBUG_STATE      CSI_DEBUG_STATE;
	struct VREG_ISP_CSI_BDG_DMA_DPCM_MODE    CSI_DMA_DPCM_MODE;
	struct VREG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	struct VREG_ISP_CSI_BDG_WR_URGENT_CTRL   CSI_WR_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_RD_URGENT_CTRL   CSI_RD_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_CROP_EN          CSI_CROP_EN;
	struct VREG_ISP_CSI_BDG_HORZ_CROP        CSI_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_VERT_CROP        CSI_VERT_CROP;
	struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT  CSI_LINE_INTP_HEIGHT;
	struct VREG_ISP_CSI_BDG_CHANNEL_LINE_COUNT  CSI_CHANNEL_LINE_COUNT;
	struct VREG_ISP_CSI_BDG_HORZ_CROP_SE     CSI_HORZ_CROP_SE;
	struct VREG_ISP_CSI_BDG_VERT_CROP_SE     CSI_VERT_CROP_SE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DMA_0 {
	union REG_ISP_DMA_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_1 {
	union REG_ISP_DMA_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_2 {
	union REG_ISP_DMA_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_3 {
	union REG_ISP_DMA_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_4 {
	union REG_ISP_DMA_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_5 {
	union REG_ISP_DMA_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_6 {
	union REG_ISP_DMA_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_7 {
	union REG_ISP_DMA_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_8 {
	union REG_ISP_DMA_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_9 {
	union REG_ISP_DMA_9                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_10 {
	union REG_ISP_DMA_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_11 {
	union REG_ISP_DMA_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_12 {
	union REG_ISP_DMA_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_13 {
	union REG_ISP_DMA_13                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_14 {
	union REG_ISP_DMA_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_15 {
	union REG_ISP_DMA_15                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_16 {
	union REG_ISP_DMA_16                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_T {
	struct VREG_ISP_DMA_0                    DMA_0;
	struct VREG_ISP_DMA_1                    DMA_1;
	struct VREG_ISP_DMA_2                    DMA_2;
	struct VREG_ISP_DMA_3                    DMA_3;
	struct VREG_ISP_DMA_4                    DMA_4;
	struct VREG_ISP_DMA_5                    DMA_5;
	struct VREG_ISP_DMA_6                    DMA_6;
	struct VREG_ISP_DMA_7                    DMA_7;
	struct VREG_ISP_DMA_8                    DMA_8;
	struct VREG_ISP_DMA_9                    DMA_9;
	struct VREG_ISP_DMA_10                   DMA_10;
	struct VREG_ISP_DMA_11                   DMA_11;
	struct VREG_ISP_DMA_12                   DMA_12;
	struct VREG_ISP_DMA_13                   DMA_13;
	struct VREG_ISP_DMA_14                   DMA_14;
	struct VREG_ISP_DMA_15                   DMA_15;
	struct VREG_ISP_DMA_16                   DMA_16;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CROP_0 {
	union REG_ISP_CROP_0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CROP_1 {
	union REG_ISP_CROP_1                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CROP_2 {
	union REG_ISP_CROP_2                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CROP_DUMMY {
	union REG_ISP_CROP_DUMMY                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CROP_T {
	struct VREG_ISP_CROP_0                   CROP_0;
	struct VREG_ISP_CROP_1                   CROP_1;
	struct VREG_ISP_CROP_2                   CROP_2;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_ISP_CROP_DUMMY               DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_BLC_0 {
	union REG_ISP_BLC_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_1 {
	union REG_ISP_BLC_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_2 {
	union REG_ISP_BLC_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_3 {
	union REG_ISP_BLC_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_4 {
	union REG_ISP_BLC_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_5 {
	union REG_ISP_BLC_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_6 {
	union REG_ISP_BLC_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_7 {
	union REG_ISP_BLC_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_8 {
	union REG_ISP_BLC_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_IMG_BAYERID {
	union REG_ISP_BLC_IMG_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_DUMMY {
	union REG_ISP_BLC_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_LOCATION {
	union REG_ISP_BLC_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_T {
	struct VREG_ISP_BLC_0                    BLC_0;
	struct VREG_ISP_BLC_1                    BLC_1;
	struct VREG_ISP_BLC_2                    BLC_2;
	struct VREG_ISP_BLC_3                    BLC_3;
	struct VREG_ISP_BLC_4                    BLC_4;
	struct VREG_ISP_BLC_5                    BLC_5;
	struct VREG_ISP_BLC_6                    BLC_6;
	struct VREG_ISP_BLC_7                    BLC_7;
	struct VREG_ISP_BLC_8                    BLC_8;
	struct VREG_ISP_BLC_IMG_BAYERID          IMG_BAYERID;
	struct VREG_ISP_BLC_DUMMY                BLC_DUMMY;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_BLC_LOCATION             BLC_LOCATION;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LSCR_SHADOW_RD_SEL {
	union REG_ISP_LSCR_SHADOW_RD_SEL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_EN            {
	union REG_ISP_LSCR_EN                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_INDEX_CLR     {
	union REG_ISP_LSCR_INDEX_CLR            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_HSIZE         {
	union REG_ISP_LSCR_HSIZE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_VSIZE         {
	union REG_ISP_LSCR_VSIZE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_X_CENTER      {
	union REG_ISP_LSCR_X_CENTER             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_Y_CENTER      {
	union REG_ISP_LSCR_Y_CENTER             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_NORM_FACTOR   {
	union REG_ISP_LSCR_NORM_FACTOR          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_LSC_LUT       {
	union REG_ISP_LSCR_LSC_LUT              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_LSC_STRENTH   {
	union REG_ISP_LSCR_LSC_STRENTH          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_DUMMY         {
	union REG_ISP_LSCR_DUMMY                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSCR_T {
	struct VREG_ISP_LSCR_SHADOW_RD_SEL       SHADOW_RD_SEL;
	struct VREG_ISP_LSCR_EN                  EN;
	struct VREG_ISP_LSCR_INDEX_CLR           INDEX_CLR;
	struct VREG_RESV                        _resv_0xc[61];
	struct VREG_ISP_LSCR_HSIZE               HSIZE;
	struct VREG_ISP_LSCR_VSIZE               VSIZE;
	struct VREG_ISP_LSCR_X_CENTER            X_CENTER;
	struct VREG_ISP_LSCR_Y_CENTER            Y_CENTER;
	struct VREG_ISP_LSCR_NORM_FACTOR         NORM_FACTOR;
	struct VREG_ISP_LSCR_LSC_LUT             LSC_LUT;
	struct VREG_ISP_LSCR_LSC_STRENTH         LSC_STRENTH;
	struct VREG_RESV                        _resv_0x11c[952];
	struct VREG_ISP_LSCR_DUMMY               DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_AE_HIST_AE_HIST_STATUS {
	union REG_ISP_AE_HIST_AE_HIST_STATUS    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_GRACE_RESET {
	union REG_ISP_AE_HIST_AE_HIST_GRACE_RESET  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_MONITOR {
	union REG_ISP_AE_HIST_AE_HIST_MONITOR   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_BYPASS {
	union REG_ISP_AE_HIST_AE_HIST_BYPASS    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_KICKOFF {
	union REG_ISP_AE_HIST_AE_KICKOFF        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_ENABLE {
	union REG_ISP_AE_HIST_AE_HIST_ENABLE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_OFFSETX {
	union REG_ISP_AE_HIST_STS_AE_OFFSETX    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_OFFSETY {
	union REG_ISP_AE_HIST_STS_AE_OFFSETY    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_NUMXM1 {
	union REG_ISP_AE_HIST_STS_AE_NUMXM1     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_NUMYM1 {
	union REG_ISP_AE_HIST_STS_AE_NUMYM1     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_WIDTH {
	union REG_ISP_AE_HIST_STS_AE_WIDTH      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_HEIGHT {
	union REG_ISP_AE_HIST_STS_AE_HEIGHT     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE_STS_DIV {
	union REG_ISP_AE_HIST_STS_AE_STS_DIV    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_HIST_MODE {
	union REG_ISP_AE_HIST_STS_HIST_MODE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_MONITOR_SELECT {
	union REG_ISP_AE_HIST_AE_HIST_MONITOR_SELECT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_LOCATION {
	union REG_ISP_AE_HIST_AE_HIST_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_OFFSETX {
	union REG_ISP_AE_HIST_STS_AE1_OFFSETX   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_OFFSETY {
	union REG_ISP_AE_HIST_STS_AE1_OFFSETY   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_NUMXM1 {
	union REG_ISP_AE_HIST_STS_AE1_NUMXM1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_NUMYM1 {
	union REG_ISP_AE_HIST_STS_AE1_NUMYM1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_WIDTH {
	union REG_ISP_AE_HIST_STS_AE1_WIDTH     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_HEIGHT {
	union REG_ISP_AE_HIST_STS_AE1_HEIGHT    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_AE1_STS_DIV {
	union REG_ISP_AE_HIST_STS_AE1_STS_DIV   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_HIST1_MODE {
	union REG_ISP_AE_HIST_STS_HIST1_MODE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_BAYER_STARTING {
	union REG_ISP_AE_HIST_AE_HIST_BAYER_STARTING  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_DUMMY {
	union REG_ISP_AE_HIST_AE_HIST_DUMMY     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_CHECKSUM {
	union REG_ISP_AE_HIST_AE_HIST_CHECKSUM  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_WBG_4 {
	union REG_ISP_AE_HIST_WBG_4             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_WBG_5 {
	union REG_ISP_AE_HIST_WBG_5             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_WBG_6 {
	union REG_ISP_AE_HIST_WBG_6             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_T {
	struct VREG_ISP_AE_HIST_AE_HIST_STATUS   AE_HIST_STATUS;
	struct VREG_ISP_AE_HIST_AE_HIST_GRACE_RESET  AE_HIST_GRACE_RESET;
	struct VREG_ISP_AE_HIST_AE_HIST_MONITOR  AE_HIST_MONITOR;
	struct VREG_ISP_AE_HIST_AE_HIST_BYPASS   AE_HIST_BYPASS;
	struct VREG_ISP_AE_HIST_AE_KICKOFF       AE_KICKOFF;
	struct VREG_ISP_AE_HIST_AE_HIST_ENABLE   AE_HIST_ENABLE;
	struct VREG_ISP_AE_HIST_STS_AE_OFFSETX   STS_AE_OFFSETX;
	struct VREG_ISP_AE_HIST_STS_AE_OFFSETY   STS_AE_OFFSETY;
	struct VREG_ISP_AE_HIST_STS_AE_NUMXM1    STS_AE_NUMXM1;
	struct VREG_ISP_AE_HIST_STS_AE_NUMYM1    STS_AE_NUMYM1;
	struct VREG_ISP_AE_HIST_STS_AE_WIDTH     STS_AE_WIDTH;
	struct VREG_ISP_AE_HIST_STS_AE_HEIGHT    STS_AE_HEIGHT;
	struct VREG_ISP_AE_HIST_STS_AE_STS_DIV   STS_AE_STS_DIV;
	struct VREG_ISP_AE_HIST_STS_HIST_MODE    STS_HIST_MODE;
	struct VREG_RESV                        _resv_0x38[1];
	struct VREG_ISP_AE_HIST_AE_HIST_MONITOR_SELECT  AE_HIST_MONITOR_SELECT;
	struct VREG_ISP_AE_HIST_AE_HIST_LOCATION  AE_HIST_LOCATION;
	struct VREG_RESV                        _resv_0x44[1];
	struct VREG_ISP_AE_HIST_STS_AE1_OFFSETX  STS_AE1_OFFSETX;
	struct VREG_ISP_AE_HIST_STS_AE1_OFFSETY  STS_AE1_OFFSETY;
	struct VREG_ISP_AE_HIST_STS_AE1_NUMXM1   STS_AE1_NUMXM1;
	struct VREG_ISP_AE_HIST_STS_AE1_NUMYM1   STS_AE1_NUMYM1;
	struct VREG_ISP_AE_HIST_STS_AE1_WIDTH    STS_AE1_WIDTH;
	struct VREG_ISP_AE_HIST_STS_AE1_HEIGHT   STS_AE1_HEIGHT;
	struct VREG_ISP_AE_HIST_STS_AE1_STS_DIV  STS_AE1_STS_DIV;
	struct VREG_ISP_AE_HIST_STS_HIST1_MODE   STS_HIST1_MODE;
	struct VREG_ISP_AE_HIST_AE_HIST_BAYER_STARTING  AE_HIST_BAYER_STARTING;
	struct VREG_ISP_AE_HIST_AE_HIST_DUMMY    AE_HIST_DUMMY;
	struct VREG_ISP_AE_HIST_AE_HIST_CHECKSUM  AE_HIST_CHECKSUM;
	struct VREG_ISP_AE_HIST_WBG_4            WBG_4;
	struct VREG_ISP_AE_HIST_WBG_5            WBG_5;
	struct VREG_ISP_AE_HIST_WBG_6            WBG_6;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_AWB_STATUS {
	union REG_ISP_AWB_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_GRACE_RESET {
	union REG_ISP_AWB_GRACE_RESET           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_MONITOR {
	union REG_ISP_AWB_MONITOR               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_SHADOW_SELECT {
	union REG_ISP_AWB_SHADOW_SELECT         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_KICKOFF {
	union REG_ISP_AWB_KICKOFF               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_ENABLE {
	union REG_ISP_AWB_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_MONITOR_SELECT {
	union REG_ISP_AWB_MONITOR_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_OFFSETX {
	union REG_ISP_AWB_STS_OFFSETX           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_OFFSETY {
	union REG_ISP_AWB_STS_OFFSETY           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_NUMXM1 {
	union REG_ISP_AWB_STS_NUMXM1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_NUMYM1 {
	union REG_ISP_AWB_STS_NUMYM1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_WIDTH {
	union REG_ISP_AWB_STS_WIDTH             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_HEIGHT {
	union REG_ISP_AWB_STS_HEIGHT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_SKIPX {
	union REG_ISP_AWB_STS_SKIPX             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_SKIPY {
	union REG_ISP_AWB_STS_SKIPY             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_CORNER_AVG_EN {
	union REG_ISP_AWB_STS_CORNER_AVG_EN     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_CORNER_SIZE {
	union REG_ISP_AWB_STS_CORNER_SIZE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_STS_DIV {
	union REG_ISP_AWB_STS_STS_DIV           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_R_LOTHD {
	union REG_ISP_AWB_STS_R_LOTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_R_UPTHD {
	union REG_ISP_AWB_STS_R_UPTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_G_LOTHD {
	union REG_ISP_AWB_STS_G_LOTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_G_UPTHD {
	union REG_ISP_AWB_STS_G_UPTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_B_LOTHD {
	union REG_ISP_AWB_STS_B_LOTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_STS_B_UPTHD {
	union REG_ISP_AWB_STS_B_UPTHD           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_WBG_4 {
	union REG_ISP_AWB_WBG_4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_WBG_5 {
	union REG_ISP_AWB_WBG_5                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_MEM_SW_MODE {
	union REG_ISP_AWB_MEM_SW_MODE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_MEM_SW_RADDR {
	union REG_ISP_AWB_MEM_SW_RADDR          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_LOCATION {
	union REG_ISP_AWB_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_MEM_SW_RDATA {
	union REG_ISP_AWB_MEM_SW_RDATA          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_BAYER_STARTING {
	union REG_ISP_AWB_BAYER_STARTING        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_DUMMY {
	union REG_ISP_AWB_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AWB_T {
	struct VREG_ISP_AWB_STATUS               STATUS;
	struct VREG_ISP_AWB_GRACE_RESET          GRACE_RESET;
	struct VREG_ISP_AWB_MONITOR              MONITOR;
	struct VREG_ISP_AWB_SHADOW_SELECT        SHADOW_SELECT;
	struct VREG_ISP_AWB_KICKOFF              KICKOFF;
	struct VREG_ISP_AWB_ENABLE               ENABLE;
	struct VREG_RESV                        _resv_0x18[1];
	struct VREG_ISP_AWB_MONITOR_SELECT       MONITOR_SELECT;
	struct VREG_ISP_AWB_STS_OFFSETX          STS_OFFSETX;
	struct VREG_ISP_AWB_STS_OFFSETY          STS_OFFSETY;
	struct VREG_ISP_AWB_STS_NUMXM1           STS_NUMXM1;
	struct VREG_ISP_AWB_STS_NUMYM1           STS_NUMYM1;
	struct VREG_ISP_AWB_STS_WIDTH            STS_WIDTH;
	struct VREG_ISP_AWB_STS_HEIGHT           STS_HEIGHT;
	struct VREG_ISP_AWB_STS_SKIPX            STS_SKIPX;
	struct VREG_ISP_AWB_STS_SKIPY            STS_SKIPY;
	struct VREG_ISP_AWB_STS_CORNER_AVG_EN    STS_CORNER_AVG_EN;
	struct VREG_ISP_AWB_STS_CORNER_SIZE      STS_CORNER_SIZE;
	struct VREG_ISP_AWB_STS_STS_DIV          STS_STS_DIV;
	struct VREG_RESV                        _resv_0x4c[1];
	struct VREG_ISP_AWB_STS_R_LOTHD          STS_R_LOTHD;
	struct VREG_ISP_AWB_STS_R_UPTHD          STS_R_UPTHD;
	struct VREG_ISP_AWB_STS_G_LOTHD          STS_G_LOTHD;
	struct VREG_ISP_AWB_STS_G_UPTHD          STS_G_UPTHD;
	struct VREG_ISP_AWB_STS_B_LOTHD          STS_B_LOTHD;
	struct VREG_ISP_AWB_STS_B_UPTHD          STS_B_UPTHD;
	struct VREG_RESV                        _resv_0x68[3];
	struct VREG_ISP_AWB_WBG_4                WBG_4;
	struct VREG_ISP_AWB_WBG_5                WBG_5;
	struct VREG_RESV                        _resv_0x7c[1];
	struct VREG_ISP_AWB_MEM_SW_MODE          SW_MODE;
	struct VREG_ISP_AWB_MEM_SW_RADDR         SW_RADDR;
	struct VREG_ISP_AWB_LOCATION             LOCATION;
	struct VREG_ISP_AWB_MEM_SW_RDATA         SW_RDATA;
	struct VREG_ISP_AWB_BAYER_STARTING       BAYER_STARTING;
	struct VREG_ISP_AWB_DUMMY                DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_AF_STATUS {
	union REG_ISP_AF_STATUS                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GRACE_RESET {
	union REG_ISP_AF_GRACE_RESET            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MONITOR {
	union REG_ISP_AF_MONITOR                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_BYPASS {
	union REG_ISP_AF_BYPASS                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_KICKOFF {
	union REG_ISP_AF_KICKOFF                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_ENABLES {
	union REG_ISP_AF_ENABLES                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_OFFSET_X {
	union REG_ISP_AF_OFFSET_X               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MXN_IMAGE_WIDTH_M1 {
	union REG_ISP_AF_MXN_IMAGE_WIDTH_M1     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_BLOCK_WIDTH {
	union REG_ISP_AF_BLOCK_WIDTH            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_BLOCK_HEIGHT {
	union REG_ISP_AF_BLOCK_HEIGHT           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_BLOCK_NUM_X {
	union REG_ISP_AF_BLOCK_NUM_X            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_BLOCK_NUM_Y {
	union REG_ISP_AF_BLOCK_NUM_Y            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_CROP_BAYERID {
	union REG_ISP_AF_CROP_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_HOR_LOW_PASS_VALUE_SHIFT {
	union REG_ISP_AF_HOR_LOW_PASS_VALUE_SHIFT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_CORNING_OFFSET_HORIZONTAL_0 {
	union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1 {
	union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_CORNING_OFFSET_VERTICAL {
	union REG_ISP_AF_CORNING_OFFSET_VERTICAL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_HIGH_Y_THRE {
	union REG_ISP_AF_HIGH_Y_THRE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_LOW_PASS_HORIZON {
	union REG_ISP_AF_LOW_PASS_HORIZON       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_LOCATION {
	union REG_ISP_AF_LOCATION               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_HIGH_PASS_HORIZON_0 {
	union REG_ISP_AF_HIGH_PASS_HORIZON_0    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_HIGH_PASS_HORIZON_1 {
	union REG_ISP_AF_HIGH_PASS_HORIZON_1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_HIGH_PASS_VERTICAL_0 {
	union REG_ISP_AF_HIGH_PASS_VERTICAL_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MEM_SW_MODE {
	union REG_ISP_AF_MEM_SW_MODE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MONITOR_SELECT {
	union REG_ISP_AF_MONITOR_SELECT         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_IMAGE_WIDTH {
	union REG_ISP_AF_IMAGE_WIDTH            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_DUMMY {
	union REG_ISP_AF_DUMMY                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MEM_SW_RADDR {
	union REG_ISP_AF_MEM_SW_RADDR           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MEM_SW_RDATA {
	union REG_ISP_AF_MEM_SW_RDATA           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_MXN_BORDER {
	union REG_ISP_AF_MXN_BORDER             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_T {
	struct VREG_ISP_AF_STATUS                STATUS;
	struct VREG_ISP_AF_GRACE_RESET           GRACE_RESET;
	struct VREG_ISP_AF_MONITOR               MONITOR;
	struct VREG_ISP_AF_BYPASS                BYPASS;
	struct VREG_ISP_AF_KICKOFF               KICKOFF;
	struct VREG_ISP_AF_ENABLES               ENABLES;
	struct VREG_ISP_AF_OFFSET_X              OFFSET_X;
	struct VREG_ISP_AF_MXN_IMAGE_WIDTH_M1    MXN_IMAGE_WIDTH_M1;
	struct VREG_ISP_AF_BLOCK_WIDTH           BLOCK_WIDTH;
	struct VREG_ISP_AF_BLOCK_HEIGHT          BLOCK_HEIGHT;
	struct VREG_ISP_AF_BLOCK_NUM_X           BLOCK_NUM_X;
	struct VREG_ISP_AF_BLOCK_NUM_Y           BLOCK_NUM_Y;
	struct VREG_ISP_AF_CROP_BAYERID          CROP_BAYERID;
	struct VREG_ISP_AF_HOR_LOW_PASS_VALUE_SHIFT  HOR_LOW_PASS_VALUE_SHIFT;
	struct VREG_ISP_AF_CORNING_OFFSET_HORIZONTAL_0  OFFSET_HORIZONTAL_0;
	struct VREG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1  OFFSET_HORIZONTAL_1;
	struct VREG_ISP_AF_CORNING_OFFSET_VERTICAL  OFFSET_VERTICAL;
	struct VREG_ISP_AF_HIGH_Y_THRE           HIGH_Y_THRE;
	struct VREG_ISP_AF_LOW_PASS_HORIZON      LOW_PASS_HORIZON;
	struct VREG_ISP_AF_LOCATION              LOCATION;
	struct VREG_ISP_AF_HIGH_PASS_HORIZON_0   HIGH_PASS_HORIZON_0;
	struct VREG_ISP_AF_HIGH_PASS_HORIZON_1   HIGH_PASS_HORIZON_1;
	struct VREG_ISP_AF_HIGH_PASS_VERTICAL_0  HIGH_PASS_VERTICAL_0;
	struct VREG_ISP_AF_MEM_SW_MODE           SW_MODE;
	struct VREG_ISP_AF_MONITOR_SELECT        MONITOR_SELECT;
	struct VREG_RESV                        _resv_0x64[2];
	struct VREG_ISP_AF_IMAGE_WIDTH           IMAGE_WIDTH;
	struct VREG_ISP_AF_DUMMY                 DUMMY;
	struct VREG_ISP_AF_MEM_SW_RADDR          SW_RADDR;
	struct VREG_ISP_AF_MEM_SW_RDATA          SW_RDATA;
	struct VREG_ISP_AF_MXN_BORDER            MXN_BORDER;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_AF_GAMMA_PROG_DATA {
	union REG_ISP_AF_GAMMA_PROG_DATA        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_PROG_CTRL {
	union REG_ISP_AF_GAMMA_PROG_CTRL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_PROG_MAX {
	union REG_ISP_AF_GAMMA_PROG_MAX         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_CTRL {
	union REG_ISP_AF_GAMMA_CTRL             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_MEM_SW_MODE {
	union REG_ISP_AF_GAMMA_MEM_SW_MODE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_MEM_SW_RADDR {
	union REG_ISP_AF_GAMMA_MEM_SW_RADDR     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_MEM_SW_RDATA {
	union REG_ISP_AF_GAMMA_MEM_SW_RDATA     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_DBG {
	union REG_ISP_AF_GAMMA_DBG              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_DMY0 {
	union REG_ISP_AF_GAMMA_DMY0             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_DMY1 {
	union REG_ISP_AF_GAMMA_DMY1             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_DMY_R {
	union REG_ISP_AF_GAMMA_DMY_R            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_PROG_ST_ADDR {
	union REG_ISP_AF_GAMMA_PROG_ST_ADDR     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAMMA_T {
	struct VREG_ISP_AF_GAMMA_PROG_DATA       GAMMA_PROG_DATA;
	struct VREG_ISP_AF_GAMMA_PROG_CTRL       GAMMA_PROG_CTRL;
	struct VREG_ISP_AF_GAMMA_PROG_MAX        GAMMA_PROG_MAX;
	struct VREG_ISP_AF_GAMMA_CTRL            GAMMA_CTRL;
	struct VREG_ISP_AF_GAMMA_MEM_SW_MODE     GAMMA_SW_MODE;
	struct VREG_ISP_AF_GAMMA_MEM_SW_RADDR    GAMMA_SW_RADDR;
	struct VREG_ISP_AF_GAMMA_MEM_SW_RDATA    GAMMA_SW_RDATA;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_AF_GAMMA_DBG             GAMMA_DBG;
	struct VREG_ISP_AF_GAMMA_DMY0            GAMMA_DMY0;
	struct VREG_ISP_AF_GAMMA_DMY1            GAMMA_DMY1;
	struct VREG_ISP_AF_GAMMA_DMY_R           GAMMA_DMY_R;
	struct VREG_ISP_AF_GAMMA_PROG_ST_ADDR    GAMMA_PROG_ST_ADDR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_GMS_STATUS {
	union REG_ISP_GMS_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_GRACE_RESET {
	union REG_ISP_GMS_GRACE_RESET           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_MONITOR {
	union REG_ISP_GMS_MONITOR               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_ENABLE {
	union REG_ISP_GMS_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_IMG_BAYERID {
	union REG_ISP_GMS_IMG_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_FLOW {
	union REG_ISP_GMS_FLOW                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_START_X {
	union REG_ISP_GMS_START_X               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_START_Y {
	union REG_ISP_GMS_START_Y               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_LOCATION {
	union REG_ISP_GMS_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_X_SECTION_SIZE {
	union REG_ISP_GMS_X_SECTION_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_Y_SECTION_SIZE {
	union REG_ISP_GMS_Y_SECTION_SIZE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_X_GAP {
	union REG_ISP_GMS_X_GAP                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_Y_GAP {
	union REG_ISP_GMS_Y_GAP                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_DUMMY {
	union REG_ISP_GMS_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_MEM_SW_MODE {
	union REG_ISP_GMS_MEM_SW_MODE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_MEM_SW_RADDR {
	union REG_ISP_GMS_MEM_SW_RADDR          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_MEM_SW_RDATA {
	union REG_ISP_GMS_MEM_SW_RDATA          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_MONITOR_SELECT {
	union REG_ISP_GMS_MONITOR_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_T {
	struct VREG_ISP_GMS_STATUS               GMS_STATUS;
	struct VREG_ISP_GMS_GRACE_RESET          GMS_GRACE_RESET;
	struct VREG_ISP_GMS_MONITOR              GMS_MONITOR;
	struct VREG_ISP_GMS_ENABLE               GMS_ENABLE;
	struct VREG_ISP_GMS_IMG_BAYERID          IMG_BAYERID;
	struct VREG_ISP_GMS_FLOW                 GMS_FLOW;
	struct VREG_ISP_GMS_START_X              GMS_START_X;
	struct VREG_ISP_GMS_START_Y              GMS_START_Y;
	struct VREG_ISP_GMS_LOCATION             GMS_LOCATION;
	struct VREG_RESV                        _resv_0x24[1];
	struct VREG_ISP_GMS_X_SECTION_SIZE       GMS_X_SECTION_SIZE;
	struct VREG_ISP_GMS_Y_SECTION_SIZE       GMS_Y_SECTION_SIZE;
	struct VREG_ISP_GMS_X_GAP                GMS_X_GAP;
	struct VREG_ISP_GMS_Y_GAP                GMS_Y_GAP;
	struct VREG_ISP_GMS_DUMMY                GMS_DUMMY;
	struct VREG_RESV                        _resv_0x3c[1];
	struct VREG_ISP_GMS_MEM_SW_MODE          GMS_SW_MODE;
	struct VREG_ISP_GMS_MEM_SW_RADDR         GMS_SW_RADDR;
	struct VREG_ISP_GMS_MEM_SW_RDATA         GMS_SW_RDATA;
	struct VREG_ISP_GMS_MONITOR_SELECT       GMS_MONITOR_SELECT;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_WBG_0 {
	union REG_ISP_WBG_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_1 {
	union REG_ISP_WBG_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_2 {
	union REG_ISP_WBG_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_4 {
	union REG_ISP_WBG_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_5 {
	union REG_ISP_WBG_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_6 {
	union REG_ISP_WBG_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_7 {
	union REG_ISP_WBG_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_IMG_BAYERID {
	union REG_ISP_WBG_IMG_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_DUMMY {
	union REG_ISP_WBG_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_LOCATION {
	union REG_ISP_WBG_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_T {
	struct VREG_ISP_WBG_0                    WBG_0;
	struct VREG_ISP_WBG_1                    WBG_1;
	struct VREG_ISP_WBG_2                    WBG_2;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_ISP_WBG_4                    WBG_4;
	struct VREG_ISP_WBG_5                    WBG_5;
	struct VREG_ISP_WBG_6                    WBG_6;
	struct VREG_ISP_WBG_7                    WBG_7;
	struct VREG_RESV                        _resv_0x20[1];
	struct VREG_ISP_WBG_IMG_BAYERID          IMG_BAYERID;
	struct VREG_ISP_WBG_DUMMY                WBG_DUMMY;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_WBG_LOCATION             WBG_LOCATION;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LMAP_LMP_0 {
	union REG_ISP_LMAP_LMP_0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_1 {
	union REG_ISP_LMAP_LMP_1                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_2 {
	union REG_ISP_LMAP_LMP_2                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_DEBUG_0 {
	union REG_ISP_LMAP_LMP_DEBUG_0          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_DEBUG_1 {
	union REG_ISP_LMAP_LMP_DEBUG_1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_DUMMY {
	union REG_ISP_LMAP_DUMMY                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_T {
	struct VREG_ISP_LMAP_LMP_0               LMP_0;
	struct VREG_ISP_LMAP_LMP_1               LMP_1;
	struct VREG_ISP_LMAP_LMP_2               LMP_2;
	struct VREG_ISP_LMAP_LMP_DEBUG_0         LMP_DEBUG_0;
	struct VREG_ISP_LMAP_LMP_DEBUG_1         LMP_DEBUG_1;
	struct VREG_ISP_LMAP_DUMMY               DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGBMAP_0 {
	union REG_ISP_RGBMAP_0                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_1 {
	union REG_ISP_RGBMAP_1                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_DEBUG_0 {
	union REG_ISP_RGBMAP_DEBUG_0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_DEBUG_1 {
	union REG_ISP_RGBMAP_DEBUG_1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_DUMMY {
	union REG_ISP_RGBMAP_DUMMY              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_2 {
	union REG_ISP_RGBMAP_2                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_T {
	struct VREG_ISP_RGBMAP_0                 RGBMAP_0;
	struct VREG_ISP_RGBMAP_1                 RGBMAP_1;
	struct VREG_ISP_RGBMAP_DEBUG_0           RGBMAP_DEBUG_0;
	struct VREG_ISP_RGBMAP_DEBUG_1           RGBMAP_DEBUG_1;
	struct VREG_ISP_RGBMAP_DUMMY             DUMMY;
	struct VREG_ISP_RGBMAP_2                 RGBMAP_2;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_RAW_TOP_RAW_0 {
	union REG_RAW_TOP_RAW_0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_READ_SEL {
	union REG_RAW_TOP_READ_SEL              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RAW_1 {
	union REG_RAW_TOP_RAW_1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_CTRL {
	union REG_RAW_TOP_CTRL                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RAW_2 {
	union REG_RAW_TOP_RAW_2                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_DUMMY {
	union REG_RAW_TOP_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RAW_4 {
	union REG_RAW_TOP_RAW_4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_STATUS {
	union REG_RAW_TOP_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_DEBUG {
	union REG_RAW_TOP_DEBUG                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_DEBUG_SELECT {
	union REG_RAW_TOP_DEBUG_SELECT          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_T {
	struct VREG_RAW_TOP_RAW_0                RAW_0;
	struct VREG_RAW_TOP_READ_SEL             READ_SEL;
	struct VREG_RAW_TOP_RAW_1                RAW_1;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_RAW_TOP_CTRL                 CTRL;
	struct VREG_RESV                        _resv_0x14[1];
	struct VREG_RAW_TOP_RAW_2                RAW_2;
	struct VREG_RAW_TOP_DUMMY                DUMMY;
	struct VREG_RAW_TOP_RAW_4                RAW_4;
	struct VREG_RAW_TOP_STATUS               STATUS;
	struct VREG_RAW_TOP_DEBUG                DEBUG;
	struct VREG_RAW_TOP_DEBUG_SELECT         DEBUG_SELECT;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DPC_0 {
	union REG_ISP_DPC_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_1 {
	union REG_ISP_DPC_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_2 {
	union REG_ISP_DPC_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_3 {
	union REG_ISP_DPC_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_4 {
	union REG_ISP_DPC_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_5 {
	union REG_ISP_DPC_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_6 {
	union REG_ISP_DPC_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_7 {
	union REG_ISP_DPC_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_8 {
	union REG_ISP_DPC_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_9 {
	union REG_ISP_DPC_9                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_10 {
	union REG_ISP_DPC_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_11 {
	union REG_ISP_DPC_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_12 {
	union REG_ISP_DPC_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_13 {
	union REG_ISP_DPC_13                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_14 {
	union REG_ISP_DPC_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_15 {
	union REG_ISP_DPC_15                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_16 {
	union REG_ISP_DPC_16                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_17 {
	union REG_ISP_DPC_17                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_18 {
	union REG_ISP_DPC_18                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_19 {
	union REG_ISP_DPC_19                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_MEM_W0 {
	union REG_ISP_DPC_MEM_W0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_WINDOW {
	union REG_ISP_DPC_WINDOW                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_MEM_ST_ADDR {
	union REG_ISP_DPC_MEM_ST_ADDR           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_CHECKSUM {
	union REG_ISP_DPC_CHECKSUM              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_INT {
	union REG_ISP_DPC_INT                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_20 {
	union REG_ISP_DPC_20                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_21 {
	union REG_ISP_DPC_21                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_22 {
	union REG_ISP_DPC_22                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_23 {
	union REG_ISP_DPC_23                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_T {
	struct VREG_ISP_DPC_0                    DPC_0;
	struct VREG_ISP_DPC_1                    DPC_1;
	struct VREG_ISP_DPC_2                    DPC_2;
	struct VREG_ISP_DPC_3                    DPC_3;
	struct VREG_ISP_DPC_4                    DPC_4;
	struct VREG_ISP_DPC_5                    DPC_5;
	struct VREG_ISP_DPC_6                    DPC_6;
	struct VREG_ISP_DPC_7                    DPC_7;
	struct VREG_ISP_DPC_8                    DPC_8;
	struct VREG_ISP_DPC_9                    DPC_9;
	struct VREG_ISP_DPC_10                   DPC_10;
	struct VREG_ISP_DPC_11                   DPC_11;
	struct VREG_ISP_DPC_12                   DPC_12;
	struct VREG_ISP_DPC_13                   DPC_13;
	struct VREG_ISP_DPC_14                   DPC_14;
	struct VREG_ISP_DPC_15                   DPC_15;
	struct VREG_ISP_DPC_16                   DPC_16;
	struct VREG_ISP_DPC_17                   DPC_17;
	struct VREG_ISP_DPC_18                   DPC_18;
	struct VREG_ISP_DPC_19                   DPC_19;
	struct VREG_ISP_DPC_MEM_W0               DPC_MEM_W0;
	struct VREG_ISP_DPC_WINDOW               DPC_WINDOW;
	struct VREG_ISP_DPC_MEM_ST_ADDR          DPC_MEM_ST_ADDR;
	struct VREG_RESV                        _resv_0x5c[1];
	struct VREG_ISP_DPC_CHECKSUM             DPC_CHECKSUM;
	struct VREG_ISP_DPC_INT                  DPC_INT;
	struct VREG_RESV                        _resv_0x68[2];
	struct VREG_ISP_DPC_20                   DPC_20;
	struct VREG_ISP_DPC_21                   DPC_21;
	struct VREG_ISP_DPC_22                   DPC_22;
	struct VREG_ISP_DPC_23                   DPC_23;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LSC_STATUS {
	union REG_ISP_LSC_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_GRACE_RESET {
	union REG_ISP_LSC_GRACE_RESET           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_MONITOR {
	union REG_ISP_LSC_MONITOR               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_ENABLE {
	union REG_ISP_LSC_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_KICKOFF {
	union REG_ISP_LSC_KICKOFF               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_STRENGTH {
	union REG_ISP_LSC_STRENGTH              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_IMG_BAYERID {
	union REG_ISP_LSC_IMG_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_MONITOR_SELECT {
	union REG_ISP_LSC_MONITOR_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_BLK_NUM_SELECT {
	union REG_ISP_LSC_BLK_NUM_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_GAIN_BASE {
	union REG_ISP_LSC_GAIN_BASE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_XSTEP {
	union REG_ISP_LSC_XSTEP                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_YSTEP {
	union REG_ISP_LSC_YSTEP                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_IMGX0 {
	union REG_ISP_LSC_IMGX0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_IMGY0 {
	union REG_ISP_LSC_IMGY0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_KERNEL_TABLE_WRITE {
	union REG_ISP_LSC_KERNEL_TABLE_WRITE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_KERNEL_TABLE_DATA {
	union REG_ISP_LSC_KERNEL_TABLE_DATA     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_KERNEL_TABLE_CTRL {
	union REG_ISP_LSC_KERNEL_TABLE_CTRL     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_DUMMY {
	union REG_ISP_LSC_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_LOCATION {
	union REG_ISP_LSC_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_1ST_RUNHIT {
	union REG_ISP_LSC_1ST_RUNHIT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_COMPARE_VALUE {
	union REG_ISP_LSC_COMPARE_VALUE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_MEM_SW_MODE {
	union REG_ISP_LSC_MEM_SW_MODE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_MEM_SW_RADDR {
	union REG_ISP_LSC_MEM_SW_RADDR          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_MEM_SW_RDATA {
	union REG_ISP_LSC_MEM_SW_RDATA          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_T {
	struct VREG_ISP_LSC_STATUS               LSC_STATUS;
	struct VREG_ISP_LSC_GRACE_RESET          LSC_GRACE_RESET;
	struct VREG_ISP_LSC_MONITOR              LSC_MONITOR;
	struct VREG_ISP_LSC_ENABLE               LSC_ENABLE;
	struct VREG_ISP_LSC_KICKOFF              LSC_KICKOFF;
	struct VREG_ISP_LSC_STRENGTH             LSC_STRENGTH;
	struct VREG_ISP_LSC_IMG_BAYERID          IMG_BAYERID;
	struct VREG_ISP_LSC_MONITOR_SELECT       LSC_MONITOR_SELECT;
	struct VREG_ISP_LSC_BLK_NUM_SELECT       LSC_BLK_NUM_SELECT;
	struct VREG_RESV                        _resv_0x24[6];
	struct VREG_ISP_LSC_GAIN_BASE            LSC_GAIN_BASE;
	struct VREG_ISP_LSC_XSTEP                LSC_XSTEP;
	struct VREG_ISP_LSC_YSTEP                LSC_YSTEP;
	struct VREG_ISP_LSC_IMGX0                LSC_IMGX0;
	struct VREG_ISP_LSC_IMGY0                LSC_IMGY0;
	struct VREG_RESV                        _resv_0x50[4];
	struct VREG_ISP_LSC_KERNEL_TABLE_WRITE   LSC_KERNEL_TABLE_WRITE;
	struct VREG_ISP_LSC_KERNEL_TABLE_DATA    LSC_KERNEL_TABLE_DATA;
	struct VREG_ISP_LSC_KERNEL_TABLE_CTRL    LSC_KERNEL_TABLE_CTRL;
	struct VREG_ISP_LSC_DUMMY                LSC_DUMMY;
	struct VREG_ISP_LSC_LOCATION             LSC_LOCATION;
	struct VREG_ISP_LSC_1ST_RUNHIT           LSC_1ST_RUNHIT;
	struct VREG_ISP_LSC_COMPARE_VALUE        LSC_COMPARE_VALUE;
	struct VREG_RESV                        _resv_0x7c[1];
	struct VREG_ISP_LSC_MEM_SW_MODE          LSC_SW_MODE;
	struct VREG_ISP_LSC_MEM_SW_RADDR         LSC_SW_RADDR;
	struct VREG_RESV                        _resv_0x88[1];
	struct VREG_ISP_LSC_MEM_SW_RDATA         LSC_SW_RDATA;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_FUSION_FS_CTRL {
	union REG_ISP_FUSION_FS_CTRL            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_FRAME_SIZE {
	union REG_ISP_FUSION_FS_FRAME_SIZE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_SE_GAIN {
	union REG_ISP_FUSION_FS_SE_GAIN         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_LUMA_THD {
	union REG_ISP_FUSION_FS_LUMA_THD        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_WGT {
	union REG_ISP_FUSION_FS_WGT             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_WGT_SLOPE {
	union REG_ISP_FUSION_FS_WGT_SLOPE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_SHDW_READ_SEL {
	union REG_ISP_FUSION_FS_SHDW_READ_SEL   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_FS_UP {
	union REG_ISP_FUSION_FS_UP              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FUSION_T {
	struct VREG_ISP_FUSION_FS_CTRL           FS_CTRL;
	struct VREG_ISP_FUSION_FS_FRAME_SIZE     FS_FRAME_SIZE;
	struct VREG_ISP_FUSION_FS_SE_GAIN        FS_SE_GAIN;
	struct VREG_ISP_FUSION_FS_LUMA_THD       FS_LUMA_THD;
	struct VREG_ISP_FUSION_FS_WGT            FS_WGT;
	struct VREG_ISP_FUSION_FS_WGT_SLOPE      FS_WGT_SLOPE;
	struct VREG_ISP_FUSION_FS_SHDW_READ_SEL  FS_SHDW_READ_SEL;
	struct VREG_ISP_FUSION_FS_UP             FS_UP;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LTM_TOP_CTRL {
	union REG_ISP_LTM_TOP_CTRL              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BLK_SIZE {
	union REG_ISP_LTM_BLK_SIZE              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_FRAME_SIZE {
	union REG_ISP_LTM_FRAME_SIZE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BE_STRTH_CTRL {
	union REG_ISP_LTM_BE_STRTH_CTRL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_STRTH_CTRL {
	union REG_ISP_LTM_DE_STRTH_CTRL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_FILTER_WIN_SIZE_CTRL {
	union REG_ISP_LTM_FILTER_WIN_SIZE_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BGAIN_CTRL_0 {
	union REG_ISP_LTM_BGAIN_CTRL_0          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BGAIN_CTRL_1 {
	union REG_ISP_LTM_BGAIN_CTRL_1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DGAIN_CTRL_0 {
	union REG_ISP_LTM_DGAIN_CTRL_0          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DGAIN_CTRL_1 {
	union REG_ISP_LTM_DGAIN_CTRL_1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_0 {
	union REG_ISP_LTM_BRI_LCE_CTRL_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_1 {
	union REG_ISP_LTM_BRI_LCE_CTRL_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_2 {
	union REG_ISP_LTM_BRI_LCE_CTRL_2        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_3 {
	union REG_ISP_LTM_BRI_LCE_CTRL_3        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_4 {
	union REG_ISP_LTM_BRI_LCE_CTRL_4        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_5 {
	union REG_ISP_LTM_BRI_LCE_CTRL_5        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_6 {
	union REG_ISP_LTM_BRI_LCE_CTRL_6        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_7 {
	union REG_ISP_LTM_BRI_LCE_CTRL_7        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_8 {
	union REG_ISP_LTM_BRI_LCE_CTRL_8        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_9 {
	union REG_ISP_LTM_BRI_LCE_CTRL_9        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_10 {
	union REG_ISP_LTM_BRI_LCE_CTRL_10       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_11 {
	union REG_ISP_LTM_BRI_LCE_CTRL_11       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_12 {
	union REG_ISP_LTM_BRI_LCE_CTRL_12       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BRI_LCE_CTRL_13 {
	union REG_ISP_LTM_BRI_LCE_CTRL_13       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_0 {
	union REG_ISP_LTM_DAR_LCE_CTRL_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_1 {
	union REG_ISP_LTM_DAR_LCE_CTRL_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_2 {
	union REG_ISP_LTM_DAR_LCE_CTRL_2        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_3 {
	union REG_ISP_LTM_DAR_LCE_CTRL_3        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_4 {
	union REG_ISP_LTM_DAR_LCE_CTRL_4        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_5 {
	union REG_ISP_LTM_DAR_LCE_CTRL_5        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DAR_LCE_CTRL_6 {
	union REG_ISP_LTM_DAR_LCE_CTRL_6        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CURVE_QUAN_BIT {
	union REG_ISP_LTM_CURVE_QUAN_BIT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIST_WGT_0 {
	union REG_ISP_LTM_LMAP0_LP_DIST_WGT_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIST_WGT_1 {
	union REG_ISP_LTM_LMAP0_LP_DIST_WGT_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_0 {
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_1 {
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_2 {
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_2   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_3 {
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_3   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_4 {
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_4   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIST_WGT_0 {
	union REG_ISP_LTM_LMAP1_LP_DIST_WGT_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIST_WGT_1 {
	union REG_ISP_LTM_LMAP1_LP_DIST_WGT_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_0 {
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_1 {
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_2 {
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_2   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_3 {
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_3   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_4 {
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_4   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BE_DIST_WGT_0 {
	union REG_ISP_LTM_BE_DIST_WGT_0         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BE_DIST_WGT_1 {
	union REG_ISP_LTM_BE_DIST_WGT_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_DIST_WGT_0 {
	union REG_ISP_LTM_DE_DIST_WGT_0         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_DIST_WGT_1 {
	union REG_ISP_LTM_DE_DIST_WGT_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_LUMA_WGT_0 {
	union REG_ISP_LTM_DE_LUMA_WGT_0         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_LUMA_WGT_1 {
	union REG_ISP_LTM_DE_LUMA_WGT_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_LUMA_WGT_2 {
	union REG_ISP_LTM_DE_LUMA_WGT_2         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_LUMA_WGT_3 {
	union REG_ISP_LTM_DE_LUMA_WGT_3         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DE_LUMA_WGT_4 {
	union REG_ISP_LTM_DE_LUMA_WGT_4         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_PROG_DATA {
	union REG_ISP_LTM_DTONE_CURVE_PROG_DATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_PROG_CTRL {
	union REG_ISP_LTM_DTONE_CURVE_PROG_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_PROG_MAX {
	union REG_ISP_LTM_DTONE_CURVE_PROG_MAX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_CTRL {
	union REG_ISP_LTM_DTONE_CURVE_CTRL      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_MODE {
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_RADDR {
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RADDR  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_RDATA {
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RDATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_PROG_DATA {
	union REG_ISP_LTM_BTONE_CURVE_PROG_DATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_PROG_CTRL {
	union REG_ISP_LTM_BTONE_CURVE_PROG_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_PROG_MAX {
	union REG_ISP_LTM_BTONE_CURVE_PROG_MAX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_CTRL {
	union REG_ISP_LTM_BTONE_CURVE_CTRL      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_MODE {
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_RADDR {
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RADDR  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_RDATA {
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RDATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_DATA {
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_DATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_CTRL {
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_MAX {
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_MAX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_CTRL {
	union REG_ISP_LTM_GLOBAL_CURVE_CTRL     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_MODE {
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RADDR {
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RADDR  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RDATA {
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RDATA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_RESIZE_COEFF_PROG_CTRL {
	union REG_ISP_LTM_RESIZE_COEFF_PROG_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_RESIZE_COEFF_WDATA_0 {
	union REG_ISP_LTM_RESIZE_COEFF_WDATA_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_RESIZE_COEFF_WDATA_1 {
	union REG_ISP_LTM_RESIZE_COEFF_WDATA_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_0 {
	union REG_ISP_LTM_CFA_CTRL_0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_1 {
	union REG_ISP_LTM_CFA_CTRL_1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_2 {
	union REG_ISP_LTM_CFA_CTRL_2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_3 {
	union REG_ISP_LTM_CFA_CTRL_3            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_4 {
	union REG_ISP_LTM_CFA_CTRL_4            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_5 {
	union REG_ISP_LTM_CFA_CTRL_5            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_6 {
	union REG_ISP_LTM_CFA_CTRL_6            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_7 {
	union REG_ISP_LTM_CFA_CTRL_7            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_CFA_CTRL_8 {
	union REG_ISP_LTM_CFA_CTRL_8            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_TILE_MODE_CTRL_0 {
	union REG_ISP_LTM_TILE_MODE_CTRL_0      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_TILE_MODE_CTRL_1 {
	union REG_ISP_LTM_TILE_MODE_CTRL_1      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_TILE_MODE_CTRL_2 {
	union REG_ISP_LTM_TILE_MODE_CTRL_2      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LTM_T {
	struct VREG_ISP_LTM_TOP_CTRL             LTM_TOP_CTRL;
	struct VREG_ISP_LTM_BLK_SIZE             LTM_BLK_SIZE;
	struct VREG_ISP_LTM_FRAME_SIZE           LTM_FRAME_SIZE;
	struct VREG_ISP_LTM_BE_STRTH_CTRL        LTM_BE_STRTH_CTRL;
	struct VREG_ISP_LTM_DE_STRTH_CTRL        LTM_DE_STRTH_CTRL;
	struct VREG_ISP_LTM_FILTER_WIN_SIZE_CTRL  LTM_FILTER_WIN_SIZE_CTRL;
	struct VREG_ISP_LTM_BGAIN_CTRL_0         LTM_BGAIN_CTRL_0;
	struct VREG_ISP_LTM_BGAIN_CTRL_1         LTM_BGAIN_CTRL_1;
	struct VREG_ISP_LTM_DGAIN_CTRL_0         LTM_DGAIN_CTRL_0;
	struct VREG_ISP_LTM_DGAIN_CTRL_1         LTM_DGAIN_CTRL_1;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_0       LTM_BRI_LCE_CTRL_0;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_1       LTM_BRI_LCE_CTRL_1;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_2       LTM_BRI_LCE_CTRL_2;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_3       LTM_BRI_LCE_CTRL_3;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_4       LTM_BRI_LCE_CTRL_4;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_5       LTM_BRI_LCE_CTRL_5;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_6       LTM_BRI_LCE_CTRL_6;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_7       LTM_BRI_LCE_CTRL_7;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_8       LTM_BRI_LCE_CTRL_8;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_9       LTM_BRI_LCE_CTRL_9;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_10      LTM_BRI_LCE_CTRL_10;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_11      LTM_BRI_LCE_CTRL_11;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_12      LTM_BRI_LCE_CTRL_12;
	struct VREG_ISP_LTM_BRI_LCE_CTRL_13      LTM_BRI_LCE_CTRL_13;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_0       LTM_DAR_LCE_CTRL_0;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_1       LTM_DAR_LCE_CTRL_1;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_2       LTM_DAR_LCE_CTRL_2;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_3       LTM_DAR_LCE_CTRL_3;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_4       LTM_DAR_LCE_CTRL_4;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_5       LTM_DAR_LCE_CTRL_5;
	struct VREG_ISP_LTM_DAR_LCE_CTRL_6       LTM_DAR_LCE_CTRL_6;
	struct VREG_ISP_LTM_CURVE_QUAN_BIT       LTM_CURVE_QUAN_BIT;
	struct VREG_ISP_LTM_LMAP0_LP_DIST_WGT_0  LTM_LMAP0_LP_DIST_WGT_0;
	struct VREG_ISP_LTM_LMAP0_LP_DIST_WGT_1  LTM_LMAP0_LP_DIST_WGT_1;
	struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_0  LTM_LMAP0_LP_DIFF_WGT_0;
	struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_1  LTM_LMAP0_LP_DIFF_WGT_1;
	struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_2  LTM_LMAP0_LP_DIFF_WGT_2;
	struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_3  LTM_LMAP0_LP_DIFF_WGT_3;
	struct VREG_ISP_LTM_LMAP0_LP_DIFF_WGT_4  LTM_LMAP0_LP_DIFF_WGT_4;
	struct VREG_ISP_LTM_LMAP1_LP_DIST_WGT_0  LTM_LMAP1_LP_DIST_WGT_0;
	struct VREG_ISP_LTM_LMAP1_LP_DIST_WGT_1  LTM_LMAP1_LP_DIST_WGT_1;
	struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_0  LTM_LMAP1_LP_DIFF_WGT_0;
	struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_1  LTM_LMAP1_LP_DIFF_WGT_1;
	struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_2  LTM_LMAP1_LP_DIFF_WGT_2;
	struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_3  LTM_LMAP1_LP_DIFF_WGT_3;
	struct VREG_ISP_LTM_LMAP1_LP_DIFF_WGT_4  LTM_LMAP1_LP_DIFF_WGT_4;
	struct VREG_ISP_LTM_BE_DIST_WGT_0        LTM_BE_DIST_WGT_0;
	struct VREG_ISP_LTM_BE_DIST_WGT_1        LTM_BE_DIST_WGT_1;
	struct VREG_ISP_LTM_DE_DIST_WGT_0        LTM_DE_DIST_WGT_0;
	struct VREG_ISP_LTM_DE_DIST_WGT_1        LTM_DE_DIST_WGT_1;
	struct VREG_ISP_LTM_DE_LUMA_WGT_0        LTM_DE_LUMA_WGT_0;
	struct VREG_ISP_LTM_DE_LUMA_WGT_1        LTM_DE_LUMA_WGT_1;
	struct VREG_ISP_LTM_DE_LUMA_WGT_2        LTM_DE_LUMA_WGT_2;
	struct VREG_ISP_LTM_DE_LUMA_WGT_3        LTM_DE_LUMA_WGT_3;
	struct VREG_ISP_LTM_DE_LUMA_WGT_4        LTM_DE_LUMA_WGT_4;
	struct VREG_RESV                        _resv_0xdc[1];
	struct VREG_ISP_LTM_DTONE_CURVE_PROG_DATA  DTONE_CURVE_PROG_DATA;
	struct VREG_ISP_LTM_DTONE_CURVE_PROG_CTRL  DTONE_CURVE_PROG_CTRL;
	struct VREG_ISP_LTM_DTONE_CURVE_PROG_MAX  DTONE_CURVE_PROG_MAX;
	struct VREG_ISP_LTM_DTONE_CURVE_CTRL     DTONE_CURVE_CTRL;
	struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_MODE  DTONE_CURVE_SW_MODE;
	struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_RADDR  DTONE_CURVE_SW_RADDR;
	struct VREG_ISP_LTM_DTONE_CURVE_MEM_SW_RDATA  DTONE_CURVE_SW_RDATA;
	struct VREG_RESV                        _resv_0xfc[1];
	struct VREG_ISP_LTM_BTONE_CURVE_PROG_DATA  BTONE_CURVE_PROG_DATA;
	struct VREG_ISP_LTM_BTONE_CURVE_PROG_CTRL  BTONE_CURVE_PROG_CTRL;
	struct VREG_ISP_LTM_BTONE_CURVE_PROG_MAX  BTONE_CURVE_PROG_MAX;
	struct VREG_ISP_LTM_BTONE_CURVE_CTRL     BTONE_CURVE_CTRL;
	struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_MODE  BTONE_CURVE_SW_MODE;
	struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_RADDR  BTONE_CURVE_SW_RADDR;
	struct VREG_ISP_LTM_BTONE_CURVE_MEM_SW_RDATA  BTONE_CURVE_SW_RDATA;
	struct VREG_RESV                        _resv_0x11c[1];
	struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_DATA  GLOBAL_CURVE_PROG_DATA;
	struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_CTRL  GLOBAL_CURVE_PROG_CTRL;
	struct VREG_ISP_LTM_GLOBAL_CURVE_PROG_MAX  GLOBAL_CURVE_PROG_MAX;
	struct VREG_ISP_LTM_GLOBAL_CURVE_CTRL    GLOBAL_CURVE_CTRL;
	struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_MODE  GLOBAL_CURVE_SW_MODE;
	struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RADDR  GLOBAL_CURVE_SW_RADDR;
	struct VREG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RDATA  GLOBAL_CURVE_SW_RDATA;
	struct VREG_RESV                        _resv_0x13c[1];
	struct VREG_ISP_LTM_RESIZE_COEFF_PROG_CTRL  RESIZE_COEFF_PROG_CTRL;
	struct VREG_ISP_LTM_RESIZE_COEFF_WDATA_0  RESIZE_COEFF_WDATA_0;
	struct VREG_ISP_LTM_RESIZE_COEFF_WDATA_1  RESIZE_COEFF_WDATA_1;
	struct VREG_ISP_LTM_CFA_CTRL_0           CFA_CTRL_0;
	struct VREG_ISP_LTM_CFA_CTRL_1           CFA_CTRL_1;
	struct VREG_ISP_LTM_CFA_CTRL_2           CFA_CTRL_2;
	struct VREG_ISP_LTM_CFA_CTRL_3           CFA_CTRL_3;
	struct VREG_ISP_LTM_CFA_CTRL_4           CFA_CTRL_4;
	struct VREG_ISP_LTM_CFA_CTRL_5           CFA_CTRL_5;
	struct VREG_ISP_LTM_CFA_CTRL_6           CFA_CTRL_6;
	struct VREG_ISP_LTM_CFA_CTRL_7           CFA_CTRL_7;
	struct VREG_ISP_LTM_CFA_CTRL_8           CFA_CTRL_8;
	struct VREG_ISP_LTM_TILE_MODE_CTRL_0     TILE_MODE_CTRL_0;
	struct VREG_ISP_LTM_TILE_MODE_CTRL_1     TILE_MODE_CTRL_1;
	struct VREG_ISP_LTM_TILE_MODE_CTRL_2     TILE_MODE_CTRL_2;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_BNR_SHADOW_RD_SEL {
	union REG_ISP_BNR_SHADOW_RD_SEL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_OUT_SEL {
	union REG_ISP_BNR_OUT_SEL               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_INDEX_CLR {
	union REG_ISP_BNR_INDEX_CLR             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_BAYER_TYPE      {
	union REG_ISP_BNR_BAYER_TYPE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_LUMA_TH_R    {
	union REG_ISP_BNR_NS_LUMA_TH_R          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_SLOPE_R      {
	union REG_ISP_BNR_NS_SLOPE_R            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET0_R    {
	union REG_ISP_BNR_NS_OFFSET0_R          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET1_R    {
	union REG_ISP_BNR_NS_OFFSET1_R          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_LUMA_TH_GR   {
	union REG_ISP_BNR_NS_LUMA_TH_GR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_SLOPE_GR     {
	union REG_ISP_BNR_NS_SLOPE_GR           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET0_GR   {
	union REG_ISP_BNR_NS_OFFSET0_GR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET1_GR   {
	union REG_ISP_BNR_NS_OFFSET1_GR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_LUMA_TH_GB   {
	union REG_ISP_BNR_NS_LUMA_TH_GB         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_SLOPE_GB     {
	union REG_ISP_BNR_NS_SLOPE_GB           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET0_GB   {
	union REG_ISP_BNR_NS_OFFSET0_GB         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET1_GB   {
	union REG_ISP_BNR_NS_OFFSET1_GB         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_LUMA_TH_B    {
	union REG_ISP_BNR_NS_LUMA_TH_B          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_SLOPE_B      {
	union REG_ISP_BNR_NS_SLOPE_B            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET0_B    {
	union REG_ISP_BNR_NS_OFFSET0_B          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_OFFSET1_B    {
	union REG_ISP_BNR_NS_OFFSET1_B          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NS_GAIN         {
	union REG_ISP_BNR_NS_GAIN               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_STRENGTH_MODE   {
	union REG_ISP_BNR_STRENGTH_MODE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_INTENSITY_SEL   {
	union REG_ISP_BNR_INTENSITY_SEL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_INTRA_0  {
	union REG_ISP_BNR_WEIGHT_INTRA_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_INTRA_1  {
	union REG_ISP_BNR_WEIGHT_INTRA_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_INTRA_2  {
	union REG_ISP_BNR_WEIGHT_INTRA_2        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_NORM_1   {
	union REG_ISP_BNR_WEIGHT_NORM_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_NORM_2   {
	union REG_ISP_BNR_WEIGHT_NORM_2         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_LSC_RATIO       {
	union REG_ISP_BNR_LSC_RATIO             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_VAR_TH          {
	union REG_ISP_BNR_VAR_TH                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_LUT      {
	union REG_ISP_BNR_WEIGHT_LUT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_SM       {
	union REG_ISP_BNR_WEIGHT_SM             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_V        {
	union REG_ISP_BNR_WEIGHT_V              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_H        {
	union REG_ISP_BNR_WEIGHT_H              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_D45      {
	union REG_ISP_BNR_WEIGHT_D45            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_D135     {
	union REG_ISP_BNR_WEIGHT_D135           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NEIGHBOR_MAX    {
	union REG_ISP_BNR_NEIGHBOR_MAX          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_RES_K_SMOOTH    {
	union REG_ISP_BNR_RES_K_SMOOTH          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_RES_K_TEXTURE   {
	union REG_ISP_BNR_RES_K_TEXTURE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_LSC_EN {
	union REG_ISP_BNR_LSC_EN                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_HSTR    {
	union REG_ISP_BNR_HSTR                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_HSIZE           {
	union REG_ISP_BNR_HSIZE                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_VSIZE           {
	union REG_ISP_BNR_VSIZE                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_X_CENTER        {
	union REG_ISP_BNR_X_CENTER              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_Y_CENTER        {
	union REG_ISP_BNR_Y_CENTER              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_NORM_FACTOR     {
	union REG_ISP_BNR_NORM_FACTOR           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_LSC_LUT         {
	union REG_ISP_BNR_LSC_LUT               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_LSC_STRENTH     {
	union REG_ISP_BNR_LSC_STRENTH           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_DUMMY           {
	union REG_ISP_BNR_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_T {
	struct VREG_ISP_BNR_SHADOW_RD_SEL        SHADOW_RD_SEL;
	struct VREG_ISP_BNR_OUT_SEL              OUT_SEL;
	struct VREG_ISP_BNR_INDEX_CLR            INDEX_CLR;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_ISP_BNR_BAYER_TYPE           BAYER_TYPE;
	struct VREG_RESV                        _resv_0x14[59];
	struct VREG_ISP_BNR_NS_LUMA_TH_R         NS_LUMA_TH_R;
	struct VREG_ISP_BNR_NS_SLOPE_R           NS_SLOPE_R;
	struct VREG_ISP_BNR_NS_OFFSET0_R         NS_OFFSET0_R;
	struct VREG_ISP_BNR_NS_OFFSET1_R         NS_OFFSET1_R;
	struct VREG_ISP_BNR_NS_LUMA_TH_GR        NS_LUMA_TH_GR;
	struct VREG_ISP_BNR_NS_SLOPE_GR          NS_SLOPE_GR;
	struct VREG_ISP_BNR_NS_OFFSET0_GR        NS_OFFSET0_GR;
	struct VREG_ISP_BNR_NS_OFFSET1_GR        NS_OFFSET1_GR;
	struct VREG_ISP_BNR_NS_LUMA_TH_GB        NS_LUMA_TH_GB;
	struct VREG_ISP_BNR_NS_SLOPE_GB          NS_SLOPE_GB;
	struct VREG_ISP_BNR_NS_OFFSET0_GB        NS_OFFSET0_GB;
	struct VREG_ISP_BNR_NS_OFFSET1_GB        NS_OFFSET1_GB;
	struct VREG_ISP_BNR_NS_LUMA_TH_B         NS_LUMA_TH_B;
	struct VREG_ISP_BNR_NS_SLOPE_B           NS_SLOPE_B;
	struct VREG_ISP_BNR_NS_OFFSET0_B         NS_OFFSET0_B;
	struct VREG_ISP_BNR_NS_OFFSET1_B         NS_OFFSET1_B;
	struct VREG_ISP_BNR_NS_GAIN              NS_GAIN;
	struct VREG_ISP_BNR_STRENGTH_MODE        STRENGTH_MODE;
	struct VREG_ISP_BNR_INTENSITY_SEL        INTENSITY_SEL;
	struct VREG_RESV                        _resv_0x14c[45];
	struct VREG_ISP_BNR_WEIGHT_INTRA_0       WEIGHT_INTRA_0;
	struct VREG_ISP_BNR_WEIGHT_INTRA_1       WEIGHT_INTRA_1;
	struct VREG_ISP_BNR_WEIGHT_INTRA_2       WEIGHT_INTRA_2;
	struct VREG_RESV                        _resv_0x20c[1];
	struct VREG_ISP_BNR_WEIGHT_NORM_1        WEIGHT_NORM_1;
	struct VREG_ISP_BNR_WEIGHT_NORM_2        WEIGHT_NORM_2;
	struct VREG_RESV                        _resv_0x218[2];
	struct VREG_ISP_BNR_LSC_RATIO            LSC_RATIO;
	struct VREG_ISP_BNR_VAR_TH               VAR_TH;
	struct VREG_ISP_BNR_WEIGHT_LUT           WEIGHT_LUT;
	struct VREG_ISP_BNR_WEIGHT_SM            WEIGHT_SM;
	struct VREG_ISP_BNR_WEIGHT_V             WEIGHT_V;
	struct VREG_ISP_BNR_WEIGHT_H             WEIGHT_H;
	struct VREG_ISP_BNR_WEIGHT_D45           WEIGHT_D45;
	struct VREG_ISP_BNR_WEIGHT_D135          WEIGHT_D135;
	struct VREG_ISP_BNR_NEIGHBOR_MAX         NEIGHBOR_MAX;
	struct VREG_RESV                        _resv_0x244[3];
	struct VREG_ISP_BNR_RES_K_SMOOTH         RES_K_SMOOTH;
	struct VREG_ISP_BNR_RES_K_TEXTURE        RES_K_TEXTURE;
	struct VREG_RESV                        _resv_0x258[106];
	struct VREG_ISP_BNR_LSC_EN               LSC_EN;
	struct VREG_RESV                        _resv_0x404[2];
	struct VREG_ISP_BNR_HSTR                 HSTR;
	struct VREG_ISP_BNR_HSIZE                HSIZE;
	struct VREG_ISP_BNR_VSIZE                VSIZE;
	struct VREG_ISP_BNR_X_CENTER             X_CENTER;
	struct VREG_ISP_BNR_Y_CENTER             Y_CENTER;
	struct VREG_ISP_BNR_NORM_FACTOR          NORM_FACTOR;
	struct VREG_ISP_BNR_LSC_LUT              LSC_LUT;
	struct VREG_ISP_BNR_LSC_STRENTH          LSC_STRENTH;
	struct VREG_RESV                        _resv_0x42c[756];
	struct VREG_ISP_BNR_DUMMY                DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_MM_00 {
	union REG_ISP_MM_00                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_04 {
	union REG_ISP_MM_04                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_08 {
	union REG_ISP_MM_08                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_0C {
	union REG_ISP_MM_0C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_10 {
	union REG_ISP_MM_10                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_14 {
	union REG_ISP_MM_14                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_18 {
	union REG_ISP_MM_18                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_1C {
	union REG_ISP_MM_1C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_20 {
	union REG_ISP_MM_20                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_24 {
	union REG_ISP_MM_24                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_28 {
	union REG_ISP_MM_28                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_2C {
	union REG_ISP_MM_2C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_30 {
	union REG_ISP_MM_30                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_34 {
	union REG_ISP_MM_34                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_38 {
	union REG_ISP_MM_38                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_3C {
	union REG_ISP_MM_3C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_40 {
	union REG_ISP_MM_40                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_44 {
	union REG_ISP_MM_44                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_48 {
	union REG_ISP_MM_48                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_4C {
	union REG_ISP_MM_4C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_50 {
	union REG_ISP_MM_50                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_54 {
	union REG_ISP_MM_54                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_58 {
	union REG_ISP_MM_58                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_5C {
	union REG_ISP_MM_5C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_60 {
	union REG_ISP_MM_60                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_64 {
	union REG_ISP_MM_64                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_68 {
	union REG_ISP_MM_68                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_6C {
	union REG_ISP_MM_6C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_70 {
	union REG_ISP_MM_70                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_74 {
	union REG_ISP_MM_74                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_78 {
	union REG_ISP_MM_78                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_7C {
	union REG_ISP_MM_7C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_80 {
	union REG_ISP_MM_80                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_84 {
	union REG_ISP_MM_84                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_88 {
	union REG_ISP_MM_88                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_8C {
	union REG_ISP_MM_8C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_90 {
	union REG_ISP_MM_90                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_A0 {
	union REG_ISP_MM_A0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_A4 {
	union REG_ISP_MM_A4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_A8 {
	union REG_ISP_MM_A8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_AC {
	union REG_ISP_MM_AC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_B0 {
	union REG_ISP_MM_B0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_B4 {
	union REG_ISP_MM_B4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_B8 {
	union REG_ISP_MM_B8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_BC {
	union REG_ISP_MM_BC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_C0 {
	union REG_ISP_MM_C0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_C4 {
	union REG_ISP_MM_C4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_C8 {
	union REG_ISP_MM_C8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_CC {
	union REG_ISP_MM_CC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_D0 {
	union REG_ISP_MM_D0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_D4 {
	union REG_ISP_MM_D4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_D8 {
	union REG_ISP_MM_D8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MM_T {
	struct VREG_ISP_MM_00                    REG_00;
	struct VREG_ISP_MM_04                    REG_04;
	struct VREG_ISP_MM_08                    REG_08;
	struct VREG_ISP_MM_0C                    REG_0C;
	struct VREG_ISP_MM_10                    REG_10;
	struct VREG_ISP_MM_14                    REG_14;
	struct VREG_ISP_MM_18                    REG_18;
	struct VREG_ISP_MM_1C                    REG_1C;
	struct VREG_ISP_MM_20                    REG_20;
	struct VREG_ISP_MM_24                    REG_24;
	struct VREG_ISP_MM_28                    REG_28;
	struct VREG_ISP_MM_2C                    REG_2C;
	struct VREG_ISP_MM_30                    REG_30;
	struct VREG_ISP_MM_34                    REG_34;
	struct VREG_ISP_MM_38                    REG_38;
	struct VREG_ISP_MM_3C                    REG_3C;
	struct VREG_ISP_MM_40                    REG_40;
	struct VREG_ISP_MM_44                    REG_44;
	struct VREG_ISP_MM_48                    REG_48;
	struct VREG_ISP_MM_4C                    REG_4C;
	struct VREG_ISP_MM_50                    REG_50;
	struct VREG_ISP_MM_54                    REG_54;
	struct VREG_ISP_MM_58                    REG_58;
	struct VREG_ISP_MM_5C                    REG_5C;
	struct VREG_ISP_MM_60                    REG_60;
	struct VREG_ISP_MM_64                    REG_64;
	struct VREG_ISP_MM_68                    REG_68;
	struct VREG_ISP_MM_6C                    REG_6C;
	struct VREG_ISP_MM_70                    REG_70;
	struct VREG_ISP_MM_74                    REG_74;
	struct VREG_ISP_MM_78                    REG_78;
	struct VREG_ISP_MM_7C                    REG_7C;
	struct VREG_ISP_MM_80                    REG_80;
	struct VREG_ISP_MM_84                    REG_84;
	struct VREG_ISP_MM_88                    REG_88;
	struct VREG_ISP_MM_8C                    REG_8C;
	struct VREG_ISP_MM_90                    REG_90;
	struct VREG_RESV                        _resv_0x94[3];
	struct VREG_ISP_MM_A0                    REG_A0;
	struct VREG_ISP_MM_A4                    REG_A4;
	struct VREG_ISP_MM_A8                    REG_A8;
	struct VREG_ISP_MM_AC                    REG_AC;
	struct VREG_ISP_MM_B0                    REG_B0;
	struct VREG_ISP_MM_B4                    REG_B4;
	struct VREG_ISP_MM_B8                    REG_B8;
	struct VREG_ISP_MM_BC                    REG_BC;
	struct VREG_ISP_MM_C0                    REG_C0;
	struct VREG_ISP_MM_C4                    REG_C4;
	struct VREG_ISP_MM_C8                    REG_C8;
	struct VREG_ISP_MM_CC                    REG_CC;
	struct VREG_ISP_MM_D0                    REG_D0;
	struct VREG_ISP_MM_D4                    REG_D4;
	struct VREG_ISP_MM_D8                    REG_D8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_FPN_STATUS {
	union REG_ISP_FPN_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_GRACE_RESET {
	union REG_ISP_FPN_GRACE_RESET           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MONITOR {
	union REG_ISP_FPN_MONITOR               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_ENABLE {
	union REG_ISP_FPN_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MAP_ENABLE {
	union REG_ISP_FPN_MAP_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_FLOW {
	union REG_ISP_FPN_FLOW                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MONITOR_SELECT {
	union REG_ISP_FPN_MONITOR_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_LOCATION {
	union REG_ISP_FPN_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SELECT {
	union REG_ISP_FPN_MEM_SELECT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_SECTION_INFO_0 {
	union REG_ISP_FPN_SECTION_INFO_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_SECTION_INFO_1 {
	union REG_ISP_FPN_SECTION_INFO_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_SECTION_INFO_2 {
	union REG_ISP_FPN_SECTION_INFO_2        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_SECTION_INFO_3 {
	union REG_ISP_FPN_SECTION_INFO_3        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_SECTION_INFO_4 {
	union REG_ISP_FPN_SECTION_INFO_4        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_DEBUG {
	union REG_ISP_FPN_DEBUG                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_IMG_BAYERID {
	union REG_ISP_FPN_IMG_BAYERID           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_DUMMY {
	union REG_ISP_FPN_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_PROG_DATA {
	union REG_ISP_FPN_PROG_DATA             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_PROG_CTRL {
	union REG_ISP_FPN_PROG_CTRL             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_PROG_MAX {
	union REG_ISP_FPN_PROG_MAX              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_MODE {
	union REG_ISP_FPN_MEM_SW_MODE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_RADDR {
	union REG_ISP_FPN_MEM_SW_RADDR          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_RDATA_OFFSET {
	union REG_ISP_FPN_MEM_SW_RDATA_OFFSET   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_RDATA_OFFSET_BG {
	union REG_ISP_FPN_MEM_SW_RDATA_OFFSET_BG  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_RDATA_GAIN {
	union REG_ISP_FPN_MEM_SW_RDATA_GAIN     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_MEM_SW_RDATA_GAIN_BG {
	union REG_ISP_FPN_MEM_SW_RDATA_GAIN_BG  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_DBG {
	union REG_ISP_FPN_DBG                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_FPN_T {
	struct VREG_ISP_FPN_STATUS               STATUS;
	struct VREG_ISP_FPN_GRACE_RESET          GRACE_RESET;
	struct VREG_ISP_FPN_MONITOR              MONITOR;
	struct VREG_ISP_FPN_ENABLE               ENABLE;
	struct VREG_ISP_FPN_MAP_ENABLE           MAP_ENABLE;
	struct VREG_ISP_FPN_FLOW                 FLOW;
	struct VREG_RESV                        _resv_0x18[1];
	struct VREG_ISP_FPN_MONITOR_SELECT       MONITOR_SELECT;
	struct VREG_ISP_FPN_LOCATION             LOCATION;
	struct VREG_RESV                        _resv_0x24[8];
	struct VREG_ISP_FPN_MEM_SELECT           MEM_SELECT;
	struct VREG_ISP_FPN_SECTION_INFO_0       SECTION_INFO_0;
	struct VREG_ISP_FPN_SECTION_INFO_1       SECTION_INFO_1;
	struct VREG_ISP_FPN_SECTION_INFO_2       SECTION_INFO_2;
	struct VREG_ISP_FPN_SECTION_INFO_3       SECTION_INFO_3;
	struct VREG_RESV                        _resv_0x58[1];
	struct VREG_ISP_FPN_SECTION_INFO_4       SECTION_INFO_4;
	struct VREG_ISP_FPN_DEBUG                DEBUG;
	struct VREG_ISP_FPN_IMG_BAYERID          IMG_BAYERID;
	struct VREG_ISP_FPN_DUMMY                DUMMY;
	struct VREG_RESV                        _resv_0x6c[1];
	struct VREG_ISP_FPN_PROG_DATA            PROG_DATA;
	struct VREG_ISP_FPN_PROG_CTRL            PROG_CTRL;
	struct VREG_ISP_FPN_PROG_MAX             PROG_MAX;
	struct VREG_RESV                        _resv_0x7c[1];
	struct VREG_ISP_FPN_MEM_SW_MODE          SW_MODE;
	struct VREG_ISP_FPN_MEM_SW_RADDR         SW_RADDR;
	struct VREG_ISP_FPN_MEM_SW_RDATA_OFFSET  SW_RDATA_OFFSET;
	struct VREG_ISP_FPN_MEM_SW_RDATA_OFFSET_BG  SW_RDATA_OFFSET_BG;
	struct VREG_ISP_FPN_MEM_SW_RDATA_GAIN    SW_RDATA_GAIN;
	struct VREG_ISP_FPN_MEM_SW_RDATA_GAIN_BG  SW_RDATA_GAIN_BG;
	struct VREG_RESV                        _resv_0x98[2];
	struct VREG_ISP_FPN_DBG                  DBG;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGB_0 {
	union REG_ISP_RGB_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_1 {
	union REG_ISP_RGB_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_2 {
	union REG_ISP_RGB_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_3 {
	union REG_ISP_RGB_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_4 {
	union REG_ISP_RGB_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_5 {
	union REG_ISP_RGB_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_6 {
	union REG_ISP_RGB_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_7 {
	union REG_ISP_RGB_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_T {
	struct VREG_ISP_RGB_0                    REG_0;
	struct VREG_ISP_RGB_1                    REG_1;
	struct VREG_ISP_RGB_2                    REG_2;
	struct VREG_ISP_RGB_3                    REG_3;
	struct VREG_ISP_RGB_4                    REG_4;
	struct VREG_ISP_RGB_5                    REG_5;
	struct VREG_ISP_RGB_6                    REG_6;
	struct VREG_ISP_RGB_7                    REG_7;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CFA_0 {
	union REG_ISP_CFA_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_1 {
	union REG_ISP_CFA_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_2 {
	union REG_ISP_CFA_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_3 {
	union REG_ISP_CFA_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_4 {
	union REG_ISP_CFA_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_4_1 {
	union REG_ISP_CFA_4_1                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_5 {
	union REG_ISP_CFA_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_6 {
	union REG_ISP_CFA_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_7 {
	union REG_ISP_CFA_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_8 {
	union REG_ISP_CFA_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_9 {
	union REG_ISP_CFA_9                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_0 {
	union REG_ISP_CFA_GHP_LUT_0             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_1 {
	union REG_ISP_CFA_GHP_LUT_1             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_2 {
	union REG_ISP_CFA_GHP_LUT_2             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_3 {
	union REG_ISP_CFA_GHP_LUT_3             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_4 {
	union REG_ISP_CFA_GHP_LUT_4             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_5 {
	union REG_ISP_CFA_GHP_LUT_5             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_6 {
	union REG_ISP_CFA_GHP_LUT_6             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_GHP_LUT_7 {
	union REG_ISP_CFA_GHP_LUT_7             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_10 {
	union REG_ISP_CFA_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_11 {
	union REG_ISP_CFA_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_12 {
	union REG_ISP_CFA_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_13 {
	union REG_ISP_CFA_13                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_14 {
	union REG_ISP_CFA_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_15 {
	union REG_ISP_CFA_15                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_16 {
	union REG_ISP_CFA_16                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_17 {
	union REG_ISP_CFA_17                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_18 {
	union REG_ISP_CFA_18                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_19 {
	union REG_ISP_CFA_19                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_20 {
	union REG_ISP_CFA_20                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_21 {
	union REG_ISP_CFA_21                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_T {
	struct VREG_ISP_CFA_0                    REG_0;
	struct VREG_ISP_CFA_1                    REG_1;
	struct VREG_ISP_CFA_2                    REG_2;
	struct VREG_ISP_CFA_3                    REG_3;
	struct VREG_ISP_CFA_4                    REG_4;
	struct VREG_ISP_CFA_4_1                  REG_4_1;
	struct VREG_ISP_CFA_5                    REG_5;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_CFA_6                    REG_6;
	struct VREG_ISP_CFA_7                    REG_7;
	struct VREG_ISP_CFA_8                    REG_8;
	struct VREG_ISP_CFA_9                    REG_9;
	struct VREG_ISP_CFA_GHP_LUT_0            GHP_LUT_0;
	struct VREG_ISP_CFA_GHP_LUT_1            GHP_LUT_1;
	struct VREG_ISP_CFA_GHP_LUT_2            GHP_LUT_2;
	struct VREG_ISP_CFA_GHP_LUT_3            GHP_LUT_3;
	struct VREG_ISP_CFA_GHP_LUT_4            GHP_LUT_4;
	struct VREG_ISP_CFA_GHP_LUT_5            GHP_LUT_5;
	struct VREG_ISP_CFA_GHP_LUT_6            GHP_LUT_6;
	struct VREG_ISP_CFA_GHP_LUT_7            GHP_LUT_7;
	struct VREG_RESV                        _resv_0x50[1];
	struct VREG_ISP_CFA_10                   REG_10;
	struct VREG_ISP_CFA_11                   REG_11;
	struct VREG_ISP_CFA_12                   REG_12;
	struct VREG_ISP_CFA_13                   REG_13;
	struct VREG_ISP_CFA_14                   REG_14;
	struct VREG_ISP_CFA_15                   REG_15;
	struct VREG_ISP_CFA_16                   REG_16;
	struct VREG_ISP_CFA_17                   REG_17;
	struct VREG_ISP_CFA_18                   REG_18;
	struct VREG_ISP_CFA_19                   REG_19;
	struct VREG_ISP_CFA_20                   REG_20;
	struct VREG_ISP_CFA_21                   REG_21;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CCM_00 {
	union REG_ISP_CCM_00                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_01 {
	union REG_ISP_CCM_01                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_02 {
	union REG_ISP_CCM_02                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_10 {
	union REG_ISP_CCM_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_11 {
	union REG_ISP_CCM_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_12 {
	union REG_ISP_CCM_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_20 {
	union REG_ISP_CCM_20                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_21 {
	union REG_ISP_CCM_21                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_22 {
	union REG_ISP_CCM_22                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_CTRL {
	union REG_ISP_CCM_CTRL                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_DBG {
	union REG_ISP_CCM_DBG                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_DMY0 {
	union REG_ISP_CCM_DMY0                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_DMY1 {
	union REG_ISP_CCM_DMY1                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_DMY_R {
	union REG_ISP_CCM_DMY_R                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CCM_T {
	struct VREG_ISP_CCM_00                   CCM_00;
	struct VREG_ISP_CCM_01                   CCM_01;
	struct VREG_ISP_CCM_02                   CCM_02;
	struct VREG_ISP_CCM_10                   CCM_10;
	struct VREG_ISP_CCM_11                   CCM_11;
	struct VREG_ISP_CCM_12                   CCM_12;
	struct VREG_ISP_CCM_20                   CCM_20;
	struct VREG_ISP_CCM_21                   CCM_21;
	struct VREG_ISP_CCM_22                   CCM_22;
	struct VREG_ISP_CCM_CTRL                 CCM_CTRL;
	struct VREG_ISP_CCM_DBG                  CCM_DBG;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_CCM_DMY0                 DMY0;
	struct VREG_ISP_CCM_DMY1                 DMY1;
	struct VREG_ISP_CCM_DMY_R                DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_GAMMA_PROG_DATA {
	union REG_ISP_GAMMA_PROG_DATA           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_CTRL {
	union REG_ISP_GAMMA_PROG_CTRL           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_MAX {
	union REG_ISP_GAMMA_PROG_MAX            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_CTRL {
	union REG_ISP_GAMMA_CTRL                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_MEM_SW_MODE {
	union REG_ISP_GAMMA_MEM_SW_MODE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_MEM_SW_RADDR {
	union REG_ISP_GAMMA_MEM_SW_RADDR        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_MEM_SW_RDATA {
	union REG_ISP_GAMMA_MEM_SW_RDATA        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_MEM_SW_RDATA_BG {
	union REG_ISP_GAMMA_MEM_SW_RDATA_BG     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_DBG {
	union REG_ISP_GAMMA_DBG                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_DMY0 {
	union REG_ISP_GAMMA_DMY0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_DMY1 {
	union REG_ISP_GAMMA_DMY1                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_DMY_R {
	union REG_ISP_GAMMA_DMY_R               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_DATA_G {
	union REG_ISP_GAMMA_PROG_DATA_G         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_CTRL_G {
	union REG_ISP_GAMMA_PROG_CTRL_G         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_MAX_G {
	union REG_ISP_GAMMA_PROG_MAX_G          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_DATA_B {
	union REG_ISP_GAMMA_PROG_DATA_B         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_CTRL_B {
	union REG_ISP_GAMMA_PROG_CTRL_B         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_MAX_B {
	union REG_ISP_GAMMA_PROG_MAX_B          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_ST_ADDR {
	union REG_ISP_GAMMA_PROG_ST_ADDR        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_T {
	struct VREG_ISP_GAMMA_PROG_DATA          GAMMA_PROG_DATA;
	struct VREG_ISP_GAMMA_PROG_CTRL          GAMMA_PROG_CTRL;
	struct VREG_ISP_GAMMA_PROG_MAX           GAMMA_PROG_MAX;
	struct VREG_ISP_GAMMA_CTRL               GAMMA_CTRL;
	struct VREG_ISP_GAMMA_MEM_SW_MODE        GAMMA_SW_MODE;
	struct VREG_ISP_GAMMA_MEM_SW_RADDR       GAMMA_SW_RADDR;
	struct VREG_ISP_GAMMA_MEM_SW_RDATA       GAMMA_SW_RDATA;
	struct VREG_ISP_GAMMA_MEM_SW_RDATA_BG    GAMMA_SW_RDATA_BG;
	struct VREG_ISP_GAMMA_DBG                GAMMA_DBG;
	struct VREG_ISP_GAMMA_DMY0               GAMMA_DMY0;
	struct VREG_ISP_GAMMA_DMY1               GAMMA_DMY1;
	struct VREG_ISP_GAMMA_DMY_R              GAMMA_DMY_R;
	struct VREG_ISP_GAMMA_PROG_DATA_G        GAMMA_PROG_DATA_G;
	struct VREG_ISP_GAMMA_PROG_CTRL_G        GAMMA_PROG_CTRL_G;
	struct VREG_ISP_GAMMA_PROG_MAX_G         GAMMA_PROG_MAX_G;
	struct VREG_RESV                        _resv_0x3c[1];
	struct VREG_ISP_GAMMA_PROG_DATA_B        GAMMA_PROG_DATA_B;
	struct VREG_ISP_GAMMA_PROG_CTRL_B        GAMMA_PROG_CTRL_B;
	struct VREG_ISP_GAMMA_PROG_MAX_B         GAMMA_PROG_MAX_B;
	struct VREG_RESV                        _resv_0x4c[1];
	struct VREG_ISP_GAMMA_PROG_ST_ADDR       GAMMA_PROG_ST_ADDR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_HSV_0 {
	union REG_ISP_HSV_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_DEBUG_0 {
	union REG_ISP_HSV_DEBUG_0               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_DEBUG_1 {
	union REG_ISP_HSV_DEBUG_1               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_1 {
	union REG_ISP_HSV_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_2 {
	union REG_ISP_HSV_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_3 {
	union REG_ISP_HSV_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_4 {
	union REG_ISP_HSV_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_5 {
	union REG_ISP_HSV_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_6 {
	union REG_ISP_HSV_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HSV_T {
	struct VREG_ISP_HSV_0                    HSV_0;
	struct VREG_ISP_HSV_DEBUG_0              HSV_DEBUG_0;
	struct VREG_ISP_HSV_DEBUG_1              HSV_DEBUG_1;
	struct VREG_ISP_HSV_1                    HSV_1;
	struct VREG_ISP_HSV_2                    HSV_2;
	struct VREG_ISP_HSV_3                    HSV_3;
	struct VREG_ISP_HSV_4                    HSV_4;
	struct VREG_ISP_HSV_5                    HSV_5;
	struct VREG_ISP_HSV_6                    HSV_6;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DHZ_DEHAZE_PARA {
	union REG_ISP_DHZ_DEHAZE_PARA           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_BYPASS {
	union REG_ISP_DHZ_BYPASS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_0 {
	union REG_ISP_DHZ_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_1 {
	union REG_ISP_DHZ_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_2 {
	union REG_ISP_DHZ_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_3 {
	union REG_ISP_DHZ_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_4 {
	union REG_ISP_DHZ_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_5 {
	union REG_ISP_DHZ_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_6 {
	union REG_ISP_DHZ_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_7 {
	union REG_ISP_DHZ_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_8 {
	union REG_ISP_DHZ_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DHZ_T {
	struct VREG_ISP_DHZ_DEHAZE_PARA          DEHAZE_PARA;
	struct VREG_RESV                        _resv_0x4[4];
	struct VREG_ISP_DHZ_BYPASS               DHZ_BYPASS;
	struct VREG_ISP_DHZ_0                    REG_0;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_DHZ_1                    REG_1;
	struct VREG_ISP_DHZ_2                    REG_2;
	struct VREG_ISP_DHZ_3                    REG_3;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_DHZ_4                    REG_4;
	struct VREG_ISP_DHZ_5                    REG_5;
	struct VREG_ISP_DHZ_6                    REG_6;
	struct VREG_ISP_DHZ_7                    REG_7;
	struct VREG_ISP_DHZ_8                    REG_8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CSC_0 {
	union REG_ISP_CSC_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_1 {
	union REG_ISP_CSC_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_2 {
	union REG_ISP_CSC_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_3 {
	union REG_ISP_CSC_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_T {
	struct VREG_ISP_CSC_0                    REG_0;
	struct VREG_ISP_CSC_1                    REG_1;
	struct VREG_ISP_CSC_2                    REG_2;
	struct VREG_ISP_CSC_3                    REG_3;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGBDITHER_RGB_DITHER {
	union REG_ISP_RGBDITHER_RGB_DITHER      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBDITHER_RGB_DITHER_DEBUG0 {
	union REG_ISP_RGBDITHER_RGB_DITHER_DEBUG0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBDITHER_T {
	struct VREG_ISP_RGBDITHER_RGB_DITHER     RGB_DITHER;
	struct VREG_ISP_RGBDITHER_RGB_DITHER_DEBUG0  RGB_DITHER_DEBUG0;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGBEE_0 {
	union REG_ISP_RGBEE_0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_1 {
	union REG_ISP_RGBEE_1                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_2 {
	union REG_ISP_RGBEE_2                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_0 {
	union REG_ISP_RGBEE_AC_LUT_0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_1 {
	union REG_ISP_RGBEE_AC_LUT_1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_2 {
	union REG_ISP_RGBEE_AC_LUT_2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_3 {
	union REG_ISP_RGBEE_AC_LUT_3            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_4 {
	union REG_ISP_RGBEE_AC_LUT_4            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_5 {
	union REG_ISP_RGBEE_AC_LUT_5            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_6 {
	union REG_ISP_RGBEE_AC_LUT_6            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_7 {
	union REG_ISP_RGBEE_AC_LUT_7            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_AC_LUT_8 {
	union REG_ISP_RGBEE_AC_LUT_8            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_0 {
	union REG_ISP_RGBEE_EDGE_LUT_0          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_1 {
	union REG_ISP_RGBEE_EDGE_LUT_1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_2 {
	union REG_ISP_RGBEE_EDGE_LUT_2          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_3 {
	union REG_ISP_RGBEE_EDGE_LUT_3          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_4 {
	union REG_ISP_RGBEE_EDGE_LUT_4          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_5 {
	union REG_ISP_RGBEE_EDGE_LUT_5          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_6 {
	union REG_ISP_RGBEE_EDGE_LUT_6          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_7 {
	union REG_ISP_RGBEE_EDGE_LUT_7          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_EDGE_LUT_8 {
	union REG_ISP_RGBEE_EDGE_LUT_8          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_0 {
	union REG_ISP_RGBEE_NP_LUT_0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_1 {
	union REG_ISP_RGBEE_NP_LUT_1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_2 {
	union REG_ISP_RGBEE_NP_LUT_2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_3 {
	union REG_ISP_RGBEE_NP_LUT_3            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_4 {
	union REG_ISP_RGBEE_NP_LUT_4            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_5 {
	union REG_ISP_RGBEE_NP_LUT_5            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_6 {
	union REG_ISP_RGBEE_NP_LUT_6            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_7 {
	union REG_ISP_RGBEE_NP_LUT_7            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_8 {
	union REG_ISP_RGBEE_NP_LUT_8            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_9 {
	union REG_ISP_RGBEE_NP_LUT_9            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_10 {
	union REG_ISP_RGBEE_NP_LUT_10           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_11 {
	union REG_ISP_RGBEE_NP_LUT_11           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_12 {
	union REG_ISP_RGBEE_NP_LUT_12           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_13 {
	union REG_ISP_RGBEE_NP_LUT_13           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_14 {
	union REG_ISP_RGBEE_NP_LUT_14           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_15 {
	union REG_ISP_RGBEE_NP_LUT_15           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_NP_LUT_16 {
	union REG_ISP_RGBEE_NP_LUT_16           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_4 {
	union REG_ISP_RGBEE_4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBEE_T {
	struct VREG_ISP_RGBEE_0                  REG_0;
	struct VREG_ISP_RGBEE_1                  REG_1;
	struct VREG_ISP_RGBEE_2                  REG_2;
	struct VREG_ISP_RGBEE_AC_LUT_0           AC_LUT_0;
	struct VREG_ISP_RGBEE_AC_LUT_1           AC_LUT_1;
	struct VREG_ISP_RGBEE_AC_LUT_2           AC_LUT_2;
	struct VREG_ISP_RGBEE_AC_LUT_3           AC_LUT_3;
	struct VREG_ISP_RGBEE_AC_LUT_4           AC_LUT_4;
	struct VREG_ISP_RGBEE_AC_LUT_5           AC_LUT_5;
	struct VREG_ISP_RGBEE_AC_LUT_6           AC_LUT_6;
	struct VREG_ISP_RGBEE_AC_LUT_7           AC_LUT_7;
	struct VREG_ISP_RGBEE_AC_LUT_8           AC_LUT_8;
	struct VREG_ISP_RGBEE_EDGE_LUT_0         EDGE_LUT_0;
	struct VREG_ISP_RGBEE_EDGE_LUT_1         EDGE_LUT_1;
	struct VREG_ISP_RGBEE_EDGE_LUT_2         EDGE_LUT_2;
	struct VREG_ISP_RGBEE_EDGE_LUT_3         EDGE_LUT_3;
	struct VREG_ISP_RGBEE_EDGE_LUT_4         EDGE_LUT_4;
	struct VREG_ISP_RGBEE_EDGE_LUT_5         EDGE_LUT_5;
	struct VREG_ISP_RGBEE_EDGE_LUT_6         EDGE_LUT_6;
	struct VREG_ISP_RGBEE_EDGE_LUT_7         EDGE_LUT_7;
	struct VREG_ISP_RGBEE_EDGE_LUT_8         EDGE_LUT_8;
	struct VREG_ISP_RGBEE_NP_LUT_0           NP_LUT_0;
	struct VREG_ISP_RGBEE_NP_LUT_1           NP_LUT_1;
	struct VREG_ISP_RGBEE_NP_LUT_2           NP_LUT_2;
	struct VREG_ISP_RGBEE_NP_LUT_3           NP_LUT_3;
	struct VREG_ISP_RGBEE_NP_LUT_4           NP_LUT_4;
	struct VREG_ISP_RGBEE_NP_LUT_5           NP_LUT_5;
	struct VREG_ISP_RGBEE_NP_LUT_6           NP_LUT_6;
	struct VREG_ISP_RGBEE_NP_LUT_7           NP_LUT_7;
	struct VREG_ISP_RGBEE_NP_LUT_8           NP_LUT_8;
	struct VREG_ISP_RGBEE_NP_LUT_9           NP_LUT_9;
	struct VREG_ISP_RGBEE_NP_LUT_10          NP_LUT_10;
	struct VREG_ISP_RGBEE_NP_LUT_11          NP_LUT_11;
	struct VREG_ISP_RGBEE_NP_LUT_12          NP_LUT_12;
	struct VREG_ISP_RGBEE_NP_LUT_13          NP_LUT_13;
	struct VREG_ISP_RGBEE_NP_LUT_14          NP_LUT_14;
	struct VREG_ISP_RGBEE_NP_LUT_15          NP_LUT_15;
	struct VREG_ISP_RGBEE_NP_LUT_16          NP_LUT_16;
	struct VREG_ISP_RGBEE_4                  REG_4;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_YUV_TOP_YUV_0 {
	union REG_YUV_TOP_YUV_0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_1 {
	union REG_YUV_TOP_YUV_1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_2 {
	union REG_YUV_TOP_YUV_2                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_3 {
	union REG_YUV_TOP_YUV_3                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_DEBUG_0 {
	union REG_YUV_TOP_YUV_DEBUG_0           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_4 {
	union REG_YUV_TOP_YUV_4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_DEBUG_STATE {
	union REG_YUV_TOP_YUV_DEBUG_STATE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_YUV_5 {
	union REG_YUV_TOP_YUV_5                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_PROG_SRAM0 {
	union REG_YUV_TOP_HSV_LUT_PROG_SRAM0    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_PROG_SRAM1 {
	union REG_YUV_TOP_HSV_LUT_PROG_SRAM1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_READ_SRAM0 {
	union REG_YUV_TOP_HSV_LUT_READ_SRAM0    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_READ_SRAM1 {
	union REG_YUV_TOP_HSV_LUT_READ_SRAM1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_CONTROL {
	union REG_YUV_TOP_HSV_LUT_CONTROL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_LUT_STATUS {
	union REG_YUV_TOP_HSV_LUT_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_HSV_ENABLE {
	union REG_YUV_TOP_HSV_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_T {
	struct VREG_YUV_TOP_YUV_0                YUV_0;
	struct VREG_YUV_TOP_YUV_1                YUV_1;
	struct VREG_YUV_TOP_YUV_2                YUV_2;
	struct VREG_YUV_TOP_YUV_3                YUV_3;
	struct VREG_YUV_TOP_YUV_DEBUG_0          YUV_DEBUG_0;
	struct VREG_YUV_TOP_YUV_4                YUV_4;
	struct VREG_YUV_TOP_YUV_DEBUG_STATE      YUV_DEBUG_STATE;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_YUV_TOP_YUV_5                YUV_5;
	struct VREG_RESV                        _resv_0x24[7];
	struct VREG_YUV_TOP_HSV_LUT_PROG_SRAM0   HSV_LUT_PROG_SRAM0;
	struct VREG_YUV_TOP_HSV_LUT_PROG_SRAM1   HSV_LUT_PROG_SRAM1;
	struct VREG_YUV_TOP_HSV_LUT_READ_SRAM0   HSV_LUT_READ_SRAM0;
	struct VREG_YUV_TOP_HSV_LUT_READ_SRAM1   HSV_LUT_READ_SRAM1;
	struct VREG_YUV_TOP_HSV_LUT_CONTROL      HSV_LUT_CONTROL;
	struct VREG_RESV                        _resv_0x54[2];
	struct VREG_YUV_TOP_HSV_LUT_STATUS       HSV_LUT_STATUS;
	struct VREG_YUV_TOP_HSV_ENABLE           HSV_ENABLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_444_422_0 {
	union REG_ISP_444_422_0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_1 {
	union REG_ISP_444_422_1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_2 {
	union REG_ISP_444_422_2                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_3 {
	union REG_ISP_444_422_3                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_4 {
	union REG_ISP_444_422_4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_5 {
	union REG_ISP_444_422_5                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_6 {
	union REG_ISP_444_422_6                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_7 {
	union REG_ISP_444_422_7                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_8 {
	union REG_ISP_444_422_8                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_9 {
	union REG_ISP_444_422_9                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_10 {
	union REG_ISP_444_422_10                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_11 {
	union REG_ISP_444_422_11                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_12 {
	union REG_ISP_444_422_12                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_13 {
	union REG_ISP_444_422_13                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_14 {
	union REG_ISP_444_422_14                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_15 {
	union REG_ISP_444_422_15                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_16 {
	union REG_ISP_444_422_16                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_T {
	struct VREG_ISP_444_422_0                REG_0;
	struct VREG_ISP_444_422_1                REG_1;
	struct VREG_ISP_444_422_2                REG_2;
	struct VREG_ISP_444_422_3                REG_3;
	struct VREG_ISP_444_422_4                REG_4;
	struct VREG_ISP_444_422_5                REG_5;
	struct VREG_ISP_444_422_6                REG_6;
	struct VREG_ISP_444_422_7                REG_7;
	struct VREG_ISP_444_422_8                REG_8;
	struct VREG_ISP_444_422_9                REG_9;
	struct VREG_ISP_444_422_10               REG_10;
	struct VREG_ISP_444_422_11               REG_11;
	struct VREG_ISP_444_422_12               REG_12;
	struct VREG_ISP_444_422_13               REG_13;
	struct VREG_ISP_444_422_14               REG_14;
	struct VREG_ISP_444_422_15               REG_15;
	struct VREG_ISP_444_422_16               REG_16;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_YUV_DITHER_Y_DITHER {
	union REG_ISP_YUV_DITHER_Y_DITHER       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YUV_DITHER_UV_DITHER {
	union REG_ISP_YUV_DITHER_UV_DITHER      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YUV_DITHER_DEBUG_00 {
	union REG_ISP_YUV_DITHER_DEBUG_00       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YUV_DITHER_DEBUG_01 {
	union REG_ISP_YUV_DITHER_DEBUG_01       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YUV_DITHER_T {
	struct VREG_ISP_YUV_DITHER_Y_DITHER      Y_DITHER;
	struct VREG_ISP_YUV_DITHER_UV_DITHER     UV_DITHER;
	struct VREG_ISP_YUV_DITHER_DEBUG_00      DEBUG_00;
	struct VREG_ISP_YUV_DITHER_DEBUG_01      DEBUG_01;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_YNR_SHADOW_RD_SEL   {
	union REG_ISP_YNR_SHADOW_RD_SEL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_OUT_SEL         {
	union REG_ISP_YNR_OUT_SEL               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_INDEX_CLR {
	union REG_ISP_YNR_INDEX_CLR             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH     {
	union REG_ISP_YNR_NS0_LUMA_TH           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE       {
	union REG_ISP_YNR_NS0_SLOPE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET      {
	union REG_ISP_YNR_NS0_OFFSET            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH     {
	union REG_ISP_YNR_NS1_LUMA_TH           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE       {
	union REG_ISP_YNR_NS1_SLOPE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET      {
	union REG_ISP_YNR_NS1_OFFSET            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_NS_TH    {
	union REG_ISP_YNR_MOTION_NS_TH          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_POS_GAIN {
	union REG_ISP_YNR_MOTION_POS_GAIN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_NEG_GAIN {
	union REG_ISP_YNR_MOTION_NEG_GAIN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS_GAIN         {
	union REG_ISP_YNR_NS_GAIN               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_STRENGTH_MODE   {
	union REG_ISP_YNR_STRENGTH_MODE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_INTENSITY_SEL   {
	union REG_ISP_YNR_INTENSITY_SEL         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_INTRA_0  {
	union REG_ISP_YNR_WEIGHT_INTRA_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_INTRA_1  {
	union REG_ISP_YNR_WEIGHT_INTRA_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_INTRA_2  {
	union REG_ISP_YNR_WEIGHT_INTRA_2        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_NORM_1   {
	union REG_ISP_YNR_WEIGHT_NORM_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_NORM_2   {
	union REG_ISP_YNR_WEIGHT_NORM_2         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_ALPHA_GAIN      {
	union REG_ISP_YNR_ALPHA_GAIN            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_VAR_TH          {
	union REG_ISP_YNR_VAR_TH                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_LUT      {
	union REG_ISP_YNR_WEIGHT_LUT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_SM       {
	union REG_ISP_YNR_WEIGHT_SM             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_V        {
	union REG_ISP_YNR_WEIGHT_V              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_H        {
	union REG_ISP_YNR_WEIGHT_H              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_D45      {
	union REG_ISP_YNR_WEIGHT_D45            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_D135     {
	union REG_ISP_YNR_WEIGHT_D135           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NEIGHBOR_MAX    {
	union REG_ISP_YNR_NEIGHBOR_MAX          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_K_SMOOTH    {
	union REG_ISP_YNR_RES_K_SMOOTH          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_K_TEXTURE   {
	union REG_ISP_YNR_RES_K_TEXTURE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_DUMMY           {
	union REG_ISP_YNR_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_T {
	struct VREG_ISP_YNR_SHADOW_RD_SEL        SHADOW_RD_SEL;
	struct VREG_ISP_YNR_OUT_SEL              OUT_SEL;
	struct VREG_ISP_YNR_INDEX_CLR            INDEX_CLR;
	struct VREG_RESV                        _resv_0xc[61];
	struct VREG_ISP_YNR_NS0_LUMA_TH          NS0_LUMA_TH;
	struct VREG_ISP_YNR_NS0_SLOPE            NS0_SLOPE;
	struct VREG_ISP_YNR_NS0_OFFSET           NS0_OFFSET;
	struct VREG_RESV                        _resv_0x10c[1];
	struct VREG_ISP_YNR_NS1_LUMA_TH          NS1_LUMA_TH;
	struct VREG_ISP_YNR_NS1_SLOPE            NS1_SLOPE;
	struct VREG_ISP_YNR_NS1_OFFSET           NS1_OFFSET;
	struct VREG_RESV                        _resv_0x11c[1];
	struct VREG_ISP_YNR_MOTION_NS_TH         MOTION_NS_TH;
	struct VREG_ISP_YNR_MOTION_POS_GAIN      MOTION_POS_GAIN;
	struct VREG_ISP_YNR_MOTION_NEG_GAIN      MOTION_NEG_GAIN;
	struct VREG_ISP_YNR_NS_GAIN              NS_GAIN;
	struct VREG_ISP_YNR_STRENGTH_MODE        STRENGTH_MODE;
	struct VREG_ISP_YNR_INTENSITY_SEL        INTENSITY_SEL;
	struct VREG_RESV                        _resv_0x138[50];
	struct VREG_ISP_YNR_WEIGHT_INTRA_0       WEIGHT_INTRA_0;
	struct VREG_ISP_YNR_WEIGHT_INTRA_1       WEIGHT_INTRA_1;
	struct VREG_ISP_YNR_WEIGHT_INTRA_2       WEIGHT_INTRA_2;
	struct VREG_RESV                        _resv_0x20c[1];
	struct VREG_ISP_YNR_WEIGHT_NORM_1        WEIGHT_NORM_1;
	struct VREG_ISP_YNR_WEIGHT_NORM_2        WEIGHT_NORM_2;
	struct VREG_RESV                        _resv_0x218[2];
	struct VREG_ISP_YNR_ALPHA_GAIN           ALPHA_GAIN;
	struct VREG_ISP_YNR_VAR_TH               VAR_TH;
	struct VREG_ISP_YNR_WEIGHT_LUT           WEIGHT_LUT;
	struct VREG_ISP_YNR_WEIGHT_SM            WEIGHT_SM;
	struct VREG_ISP_YNR_WEIGHT_V             WEIGHT_V;
	struct VREG_ISP_YNR_WEIGHT_H             WEIGHT_H;
	struct VREG_ISP_YNR_WEIGHT_D45           WEIGHT_D45;
	struct VREG_ISP_YNR_WEIGHT_D135          WEIGHT_D135;
	struct VREG_ISP_YNR_NEIGHBOR_MAX         NEIGHBOR_MAX;
	struct VREG_RESV                        _resv_0x244[3];
	struct VREG_ISP_YNR_RES_K_SMOOTH         RES_K_SMOOTH;
	struct VREG_ISP_YNR_RES_K_TEXTURE        RES_K_TEXTURE;
	struct VREG_RESV                        _resv_0x258[873];
	struct VREG_ISP_YNR_DUMMY                DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CNR_00 {
	union REG_ISP_CNR_00                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_01 {
	union REG_ISP_CNR_01                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_02 {
	union REG_ISP_CNR_02                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_03 {
	union REG_ISP_CNR_03                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_04 {
	union REG_ISP_CNR_04                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_05 {
	union REG_ISP_CNR_05                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_06 {
	union REG_ISP_CNR_06                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_07 {
	union REG_ISP_CNR_07                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_08 {
	union REG_ISP_CNR_08                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_09 {
	union REG_ISP_CNR_09                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_10 {
	union REG_ISP_CNR_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_11 {
	union REG_ISP_CNR_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_12 {
	union REG_ISP_CNR_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_13 {
	union REG_ISP_CNR_13                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_T {
	struct VREG_ISP_CNR_00                   CNR_00;
	struct VREG_ISP_CNR_01                   CNR_01;
	struct VREG_ISP_CNR_02                   CNR_02;
	struct VREG_ISP_CNR_03                   CNR_03;
	struct VREG_ISP_CNR_04                   CNR_04;
	struct VREG_ISP_CNR_05                   CNR_05;
	struct VREG_ISP_CNR_06                   CNR_06;
	struct VREG_ISP_CNR_07                   CNR_07;
	struct VREG_ISP_CNR_08                   CNR_08;
	struct VREG_ISP_CNR_09                   CNR_09;
	struct VREG_ISP_CNR_10                   CNR_10;
	struct VREG_ISP_CNR_11                   CNR_11;
	struct VREG_ISP_CNR_12                   CNR_12;
	struct VREG_ISP_CNR_13                   CNR_13;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_EE_00 {
	union REG_ISP_EE_00                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_04 {
	union REG_ISP_EE_04                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_08 {
	union REG_ISP_EE_08                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_0C {
	union REG_ISP_EE_0C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_10 {
	union REG_ISP_EE_10                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_14 {
	union REG_ISP_EE_14                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_18 {
	union REG_ISP_EE_18                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1C {
	union REG_ISP_EE_1C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_20 {
	union REG_ISP_EE_20                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_24 {
	union REG_ISP_EE_24                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_28 {
	union REG_ISP_EE_28                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_2C {
	union REG_ISP_EE_2C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_30 {
	union REG_ISP_EE_30                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_34 {
	union REG_ISP_EE_34                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_38 {
	union REG_ISP_EE_38                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_3C {
	union REG_ISP_EE_3C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_40 {
	union REG_ISP_EE_40                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_58 {
	union REG_ISP_EE_58                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_5C {
	union REG_ISP_EE_5C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_60 {
	union REG_ISP_EE_60                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_64 {
	union REG_ISP_EE_64                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_68 {
	union REG_ISP_EE_68                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_6C {
	union REG_ISP_EE_6C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_70 {
	union REG_ISP_EE_70                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_74 {
	union REG_ISP_EE_74                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_78 {
	union REG_ISP_EE_78                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_7C {
	union REG_ISP_EE_7C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_80 {
	union REG_ISP_EE_80                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_84 {
	union REG_ISP_EE_84                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_88 {
	union REG_ISP_EE_88                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_8C {
	union REG_ISP_EE_8C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_90 {
	union REG_ISP_EE_90                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_94 {
	union REG_ISP_EE_94                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_98 {
	union REG_ISP_EE_98                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_9C {
	union REG_ISP_EE_9C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_A4 {
	union REG_ISP_EE_A4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_A8 {
	union REG_ISP_EE_A8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_AC {
	union REG_ISP_EE_AC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_B0 {
	union REG_ISP_EE_B0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_B4 {
	union REG_ISP_EE_B4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_B8 {
	union REG_ISP_EE_B8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_BC {
	union REG_ISP_EE_BC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_C0 {
	union REG_ISP_EE_C0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_C4 {
	union REG_ISP_EE_C4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_C8 {
	union REG_ISP_EE_C8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_CC {
	union REG_ISP_EE_CC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_D0 {
	union REG_ISP_EE_D0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_D4 {
	union REG_ISP_EE_D4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_D8 {
	union REG_ISP_EE_D8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_DC {
	union REG_ISP_EE_DC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_E0 {
	union REG_ISP_EE_E0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_E4 {
	union REG_ISP_EE_E4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_E8 {
	union REG_ISP_EE_E8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_EC {
	union REG_ISP_EE_EC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_F0 {
	union REG_ISP_EE_F0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_F4 {
	union REG_ISP_EE_F4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_F8 {
	union REG_ISP_EE_F8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_FC {
	union REG_ISP_EE_FC                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_100 {
	union REG_ISP_EE_100                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_104 {
	union REG_ISP_EE_104                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_108 {
	union REG_ISP_EE_108                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_10C {
	union REG_ISP_EE_10C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_110 {
	union REG_ISP_EE_110                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_114 {
	union REG_ISP_EE_114                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_118 {
	union REG_ISP_EE_118                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_11C {
	union REG_ISP_EE_11C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_120 {
	union REG_ISP_EE_120                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_124 {
	union REG_ISP_EE_124                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_128 {
	union REG_ISP_EE_128                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_12C {
	union REG_ISP_EE_12C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_130 {
	union REG_ISP_EE_130                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_134 {
	union REG_ISP_EE_134                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_138 {
	union REG_ISP_EE_138                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_13C {
	union REG_ISP_EE_13C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_140 {
	union REG_ISP_EE_140                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_144 {
	union REG_ISP_EE_144                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_148 {
	union REG_ISP_EE_148                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_14C {
	union REG_ISP_EE_14C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_150 {
	union REG_ISP_EE_150                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_154 {
	union REG_ISP_EE_154                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_158 {
	union REG_ISP_EE_158                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_15C {
	union REG_ISP_EE_15C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_160 {
	union REG_ISP_EE_160                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_164 {
	union REG_ISP_EE_164                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_168 {
	union REG_ISP_EE_168                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_16C {
	union REG_ISP_EE_16C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_170 {
	union REG_ISP_EE_170                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_174 {
	union REG_ISP_EE_174                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_178 {
	union REG_ISP_EE_178                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_17C {
	union REG_ISP_EE_17C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_180 {
	union REG_ISP_EE_180                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_184 {
	union REG_ISP_EE_184                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_188 {
	union REG_ISP_EE_188                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_18C {
	union REG_ISP_EE_18C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_190 {
	union REG_ISP_EE_190                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_194 {
	union REG_ISP_EE_194                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_198 {
	union REG_ISP_EE_198                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_19C {
	union REG_ISP_EE_19C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1A0 {
	union REG_ISP_EE_1A0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1A4 {
	union REG_ISP_EE_1A4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1A8 {
	union REG_ISP_EE_1A8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1AC {
	union REG_ISP_EE_1AC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1B0 {
	union REG_ISP_EE_1B0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1B4 {
	union REG_ISP_EE_1B4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1B8 {
	union REG_ISP_EE_1B8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1BC {
	union REG_ISP_EE_1BC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1C0 {
	union REG_ISP_EE_1C0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1C4 {
	union REG_ISP_EE_1C4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_T {
	struct VREG_ISP_EE_00                    REG_00;
	struct VREG_ISP_EE_04                    REG_04;
	struct VREG_ISP_EE_08                    REG_08;
	struct VREG_ISP_EE_0C                    REG_0C;
	struct VREG_ISP_EE_10                    REG_10;
	struct VREG_ISP_EE_14                    REG_14;
	struct VREG_ISP_EE_18                    REG_18;
	struct VREG_ISP_EE_1C                    REG_1C;
	struct VREG_ISP_EE_20                    REG_20;
	struct VREG_ISP_EE_24                    REG_24;
	struct VREG_ISP_EE_28                    REG_28;
	struct VREG_ISP_EE_2C                    REG_2C;
	struct VREG_ISP_EE_30                    REG_30;
	struct VREG_ISP_EE_34                    REG_34;
	struct VREG_ISP_EE_38                    REG_38;
	struct VREG_ISP_EE_3C                    REG_3C;
	struct VREG_ISP_EE_40                    REG_40;
	struct VREG_RESV                        _resv_0x44[5];
	struct VREG_ISP_EE_58                    REG_58;
	struct VREG_ISP_EE_5C                    REG_5C;
	struct VREG_ISP_EE_60                    REG_60;
	struct VREG_ISP_EE_64                    REG_64;
	struct VREG_ISP_EE_68                    REG_68;
	struct VREG_ISP_EE_6C                    REG_6C;
	struct VREG_ISP_EE_70                    REG_70;
	struct VREG_ISP_EE_74                    REG_74;
	struct VREG_ISP_EE_78                    REG_78;
	struct VREG_ISP_EE_7C                    REG_7C;
	struct VREG_ISP_EE_80                    REG_80;
	struct VREG_ISP_EE_84                    REG_84;
	struct VREG_ISP_EE_88                    REG_88;
	struct VREG_ISP_EE_8C                    REG_8C;
	struct VREG_ISP_EE_90                    REG_90;
	struct VREG_ISP_EE_94                    REG_94;
	struct VREG_ISP_EE_98                    REG_98;
	struct VREG_ISP_EE_9C                    REG_9C;
	struct VREG_RESV                        _resv_0xa0[1];
	struct VREG_ISP_EE_A4                    REG_A4;
	struct VREG_ISP_EE_A8                    REG_A8;
	struct VREG_ISP_EE_AC                    REG_AC;
	struct VREG_ISP_EE_B0                    REG_B0;
	struct VREG_ISP_EE_B4                    REG_B4;
	struct VREG_ISP_EE_B8                    REG_B8;
	struct VREG_ISP_EE_BC                    REG_BC;
	struct VREG_ISP_EE_C0                    REG_C0;
	struct VREG_ISP_EE_C4                    REG_C4;
	struct VREG_ISP_EE_C8                    REG_C8;
	struct VREG_ISP_EE_CC                    REG_CC;
	struct VREG_ISP_EE_D0                    REG_D0;
	struct VREG_ISP_EE_D4                    REG_D4;
	struct VREG_ISP_EE_D8                    REG_D8;
	struct VREG_ISP_EE_DC                    REG_DC;
	struct VREG_ISP_EE_E0                    REG_E0;
	struct VREG_ISP_EE_E4                    REG_E4;
	struct VREG_ISP_EE_E8                    REG_E8;
	struct VREG_ISP_EE_EC                    REG_EC;
	struct VREG_ISP_EE_F0                    REG_F0;
	struct VREG_ISP_EE_F4                    REG_F4;
	struct VREG_ISP_EE_F8                    REG_F8;
	struct VREG_ISP_EE_FC                    REG_FC;
	struct VREG_ISP_EE_100                   REG_100;
	struct VREG_ISP_EE_104                   REG_104;
	struct VREG_ISP_EE_108                   REG_108;
	struct VREG_ISP_EE_10C                   REG_10C;
	struct VREG_ISP_EE_110                   REG_110;
	struct VREG_ISP_EE_114                   REG_114;
	struct VREG_ISP_EE_118                   REG_118;
	struct VREG_ISP_EE_11C                   REG_11C;
	struct VREG_ISP_EE_120                   REG_120;
	struct VREG_ISP_EE_124                   REG_124;
	struct VREG_ISP_EE_128                   REG_128;
	struct VREG_ISP_EE_12C                   REG_12C;
	struct VREG_ISP_EE_130                   REG_130;
	struct VREG_ISP_EE_134                   REG_134;
	struct VREG_ISP_EE_138                   REG_138;
	struct VREG_ISP_EE_13C                   REG_13C;
	struct VREG_ISP_EE_140                   REG_140;
	struct VREG_ISP_EE_144                   REG_144;
	struct VREG_ISP_EE_148                   REG_148;
	struct VREG_ISP_EE_14C                   REG_14C;
	struct VREG_ISP_EE_150                   REG_150;
	struct VREG_ISP_EE_154                   REG_154;
	struct VREG_ISP_EE_158                   REG_158;
	struct VREG_ISP_EE_15C                   REG_15C;
	struct VREG_ISP_EE_160                   REG_160;
	struct VREG_ISP_EE_164                   REG_164;
	struct VREG_ISP_EE_168                   REG_168;
	struct VREG_ISP_EE_16C                   REG_16C;
	struct VREG_ISP_EE_170                   REG_170;
	struct VREG_ISP_EE_174                   REG_174;
	struct VREG_ISP_EE_178                   REG_178;
	struct VREG_ISP_EE_17C                   REG_17C;
	struct VREG_ISP_EE_180                   REG_180;
	struct VREG_ISP_EE_184                   REG_184;
	struct VREG_ISP_EE_188                   REG_188;
	struct VREG_ISP_EE_18C                   REG_18C;
	struct VREG_ISP_EE_190                   REG_190;
	struct VREG_ISP_EE_194                   REG_194;
	struct VREG_ISP_EE_198                   REG_198;
	struct VREG_ISP_EE_19C                   REG_19C;
	struct VREG_ISP_EE_1A0                   REG_1A0;
	struct VREG_ISP_EE_1A4                   REG_1A4;
	struct VREG_ISP_EE_1A8                   REG_1A8;
	struct VREG_ISP_EE_1AC                   REG_1AC;
	struct VREG_ISP_EE_1B0                   REG_1B0;
	struct VREG_ISP_EE_1B4                   REG_1B4;
	struct VREG_ISP_EE_1B8                   REG_1B8;
	struct VREG_ISP_EE_1BC                   REG_1BC;
	struct VREG_ISP_EE_1C0                   REG_1C0;
	struct VREG_ISP_EE_1C4                   REG_1C4;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_YCUR_PROG_DATA {
	union REG_ISP_YCUR_PROG_DATA            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_PROG_CTRL {
	union REG_ISP_YCUR_PROG_CTRL            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_PROG_MAX {
	union REG_ISP_YCUR_PROG_MAX             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_CTRL {
	union REG_ISP_YCUR_CTRL                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_MEM_SW_MODE {
	union REG_ISP_YCUR_MEM_SW_MODE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_MEM_SW_RADDR {
	union REG_ISP_YCUR_MEM_SW_RADDR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_MEM_SW_RDATA {
	union REG_ISP_YCUR_MEM_SW_RDATA         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_DBG {
	union REG_ISP_YCUR_DBG                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_DMY0 {
	union REG_ISP_YCUR_DMY0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_DMY1 {
	union REG_ISP_YCUR_DMY1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_DMY_R {
	union REG_ISP_YCUR_DMY_R                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_PROG_ST_ADDR {
	union REG_ISP_YCUR_PROG_ST_ADDR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_RGB_DITHER {
	union REG_ISP_YCUR_RGB_DITHER           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_00 {
	union REG_ISP_YCUR_00                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCUR_T {
	struct VREG_ISP_YCUR_PROG_DATA           YCUR_PROG_DATA;
	struct VREG_ISP_YCUR_PROG_CTRL           YCUR_PROG_CTRL;
	struct VREG_ISP_YCUR_PROG_MAX            YCUR_PROG_MAX;
	struct VREG_ISP_YCUR_CTRL                YCUR_CTRL;
	struct VREG_ISP_YCUR_MEM_SW_MODE         YCUR_SW_MODE;
	struct VREG_ISP_YCUR_MEM_SW_RADDR        YCUR_SW_RADDR;
	struct VREG_ISP_YCUR_MEM_SW_RDATA        YCUR_SW_RDATA;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_YCUR_DBG                 YCUR_DBG;
	struct VREG_ISP_YCUR_DMY0                YCUR_DMY0;
	struct VREG_ISP_YCUR_DMY1                YCUR_DMY1;
	struct VREG_ISP_YCUR_DMY_R               YCUR_DMY_R;
	struct VREG_ISP_YCUR_PROG_ST_ADDR        YCUR_PROG_ST_ADDR;
	struct VREG_ISP_YCUR_RGB_DITHER          RGB_DITHER;
	struct VREG_ISP_YCUR_00                  REG_00;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DCI_STATUS {
	union REG_ISP_DCI_STATUS                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_GRACE_RESET {
	union REG_ISP_DCI_GRACE_RESET           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MONITOR {
	union REG_ISP_DCI_MONITOR               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_ENABLE {
	union REG_ISP_DCI_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MAP_ENABLE {
	union REG_ISP_DCI_MAP_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_FLOW {
	union REG_ISP_DCI_FLOW                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_DEMO_MODE {
	union REG_ISP_DCI_DEMO_MODE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MONITOR_SELECT {
	union REG_ISP_DCI_MONITOR_SELECT        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_LOCATION {
	union REG_ISP_DCI_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_PROG_DATA {
	union REG_ISP_DCI_PROG_DATA             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_PROG_CTRL {
	union REG_ISP_DCI_PROG_CTRL             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_PROG_MAX {
	union REG_ISP_DCI_PROG_MAX              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_CTRL {
	union REG_ISP_DCI_CTRL                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MEM_SW_MODE {
	union REG_ISP_DCI_MEM_SW_MODE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MEM_RADDR {
	union REG_ISP_DCI_MEM_RADDR             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MEM_RDATA {
	union REG_ISP_DCI_MEM_RDATA             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_DEBUG {
	union REG_ISP_DCI_DEBUG                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_DUMMY {
	union REG_ISP_DCI_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_IMG_WIDTHM1 {
	union REG_ISP_DCI_IMG_WIDTHM1           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_LUT_ORDER_SELECT {
	union REG_ISP_DCI_LUT_ORDER_SELECT      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_ROI_START {
	union REG_ISP_DCI_ROI_START             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_ROI_GEO {
	union REG_ISP_DCI_ROI_GEO               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MAPPED_LUT {
	union REG_ISP_DCI_MAPPED_LUT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_T {
	struct VREG_ISP_DCI_STATUS               DCI_STATUS;
	struct VREG_ISP_DCI_GRACE_RESET          DCI_GRACE_RESET;
	struct VREG_ISP_DCI_MONITOR              DCI_MONITOR;
	struct VREG_ISP_DCI_ENABLE               DCI_ENABLE;
	struct VREG_ISP_DCI_MAP_ENABLE           DCI_MAP_ENABLE;
	struct VREG_ISP_DCI_FLOW                 DCI_FLOW;
	struct VREG_ISP_DCI_DEMO_MODE            DCI_DEMO_MODE;
	struct VREG_ISP_DCI_MONITOR_SELECT       DCI_MONITOR_SELECT;
	struct VREG_ISP_DCI_LOCATION             DCI_LOCATION;
	struct VREG_RESV                        _resv_0x24[1];
	struct VREG_ISP_DCI_PROG_DATA            DCI_PROG_DATA;
	struct VREG_ISP_DCI_PROG_CTRL            DCI_PROG_CTRL;
	struct VREG_ISP_DCI_PROG_MAX             DCI_PROG_MAX;
	struct VREG_ISP_DCI_CTRL                 DCI_CTRL;
	struct VREG_ISP_DCI_MEM_SW_MODE          DCI_SW_MODE;
	struct VREG_ISP_DCI_MEM_RADDR            DCI_MEM_RADDR;
	struct VREG_ISP_DCI_MEM_RDATA            DCI_MEM_RDATA;
	struct VREG_ISP_DCI_DEBUG                DCI_DEBUG;
	struct VREG_ISP_DCI_DUMMY                DCI_DUMMY;
	struct VREG_ISP_DCI_IMG_WIDTHM1          IMG_WIDTHM1;
	struct VREG_ISP_DCI_LUT_ORDER_SELECT     DCI_LUT_ORDER_SELECT;
	struct VREG_ISP_DCI_ROI_START            DCI_ROI_START;
	struct VREG_ISP_DCI_ROI_GEO              DCI_ROI_GEO;
	struct VREG_RESV                        _resv_0x5c[41];
	struct VREG_ISP_DCI_MAPPED_LUT           DCI_MAPPED_LUT;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_TOP_0 {
	union REG_ISP_TOP_0                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_1 {
	union REG_ISP_TOP_1                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_2 {
	union REG_ISP_TOP_2                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_3 {
	union REG_ISP_TOP_3                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_4 {
	union REG_ISP_TOP_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_5 {
	union REG_ISP_TOP_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_6 {
	union REG_ISP_TOP_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_7 {
	union REG_ISP_TOP_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_C {
	union REG_ISP_TOP_C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_D {
	union REG_ISP_TOP_D                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_10 {
	union REG_ISP_TOP_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_11 {
	union REG_ISP_TOP_11                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_12 {
	union REG_ISP_TOP_12                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_13 {
	union REG_ISP_TOP_13                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_14 {
	union REG_ISP_TOP_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_15 {
	union REG_ISP_TOP_15                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_T {
	struct VREG_ISP_TOP_0                    REG_0;
	struct VREG_ISP_TOP_1                    REG_1;
	struct VREG_ISP_TOP_2                    REG_2;
	struct VREG_ISP_TOP_3                    REG_3;
	struct VREG_ISP_TOP_4                    REG_4;
	struct VREG_ISP_TOP_5                    REG_5;
	struct VREG_ISP_TOP_6                    REG_6;
	struct VREG_ISP_TOP_7                    REG_7;
	struct VREG_RESV                        _resv_0x20[4];
	struct VREG_ISP_TOP_C                    REG_C;
	struct VREG_ISP_TOP_D                    REG_D;
	struct VREG_RESV                        _resv_0x38[2];
	struct VREG_ISP_TOP_10                   REG_10;
	struct VREG_ISP_TOP_11                   REG_11;
	struct VREG_ISP_TOP_12                   REG_12;
	struct VREG_ISP_TOP_13                   REG_13;
	struct VREG_ISP_TOP_14                   REG_14;
	struct VREG_ISP_TOP_15                   REG_15;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_PCHK_SHADOW_RD_SEL {
	union REG_ISP_PCHK_SHADOW_RD_SEL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_IN_SEL        {
	union REG_ISP_PCHK_IN_SEL               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_RULE_EN       {
	union REG_ISP_PCHK_RULE_EN              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_HSIZE         {
	union REG_ISP_PCHK_HSIZE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_VSIZE         {
	union REG_ISP_PCHK_VSIZE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_NRDY_LIMIT    {
	union REG_ISP_PCHK_NRDY_LIMIT           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_NREQ_LIMIT    {
	union REG_ISP_PCHK_NREQ_LIMIT           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_PFREQ_LIMIT   {
	union REG_ISP_PCHK_PFREQ_LIMIT          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_ERR_BUS       {
	union REG_ISP_PCHK_ERR_BUS              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_ERR_XY        {
	union REG_ISP_PCHK_ERR_XY               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_ERR_CLR       {
	union REG_ISP_PCHK_ERR_CLR              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PCHK_T {
	struct VREG_ISP_PCHK_SHADOW_RD_SEL       SHADOW_RD_SEL;
	struct VREG_ISP_PCHK_IN_SEL              IN_SEL;
	struct VREG_ISP_PCHK_RULE_EN             RULE_EN;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_ISP_PCHK_HSIZE               HSIZE;
	struct VREG_ISP_PCHK_VSIZE               VSIZE;
	struct VREG_ISP_PCHK_NRDY_LIMIT          NRDY_LIMIT;
	struct VREG_ISP_PCHK_NREQ_LIMIT          NREQ_LIMIT;
	struct VREG_ISP_PCHK_PFREQ_LIMIT         PFREQ_LIMIT;
	struct VREG_RESV                        _resv_0x24[55];
	struct VREG_ISP_PCHK_ERR_BUS             ERR_BUS;
	struct VREG_ISP_PCHK_ERR_XY              ERR_XY;
	struct VREG_RESV                        _resv_0x108[62];
	struct VREG_ISP_PCHK_ERR_CLR             ERR_CLR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGB_DITHER_RGB_DITHER {
	union REG_ISP_RGB_DITHER_RGB_DITHER     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0 {
	union REG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_DITHER_T {
	struct VREG_ISP_RGB_DITHER_RGB_DITHER    RGB_DITHER;
	struct VREG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0  RGB_DITHER_DEBUG0;
};

#endif // _VREG_BLOCKS_H_
