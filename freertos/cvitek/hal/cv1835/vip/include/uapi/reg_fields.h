#ifndef _REG_FIELDS_H_
#define _REG_FIELDS_H_

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_PRE_RAW_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t CSI_BDG_OUT_MODE                : 2;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t LE_RGBMAP_INPUT_SEL             : 1;
		uint32_t SE_RGBMAP_INPUT_SEL             : 1;
	} bits;
};

union REG_PRE_RAW_UP {
	uint32_t raw;
	struct {
		uint32_t PRE_RAW_UP_REG                  : 1;
	} bits;
};

union REG_PRE_RAW_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};

union REG_PRE_RAW_POST_NO_RSPD_CYC {
	uint32_t raw;
	struct {
		uint32_t POST_NO_RSPD_CYC                : 32;
	} bits;
};

union REG_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC {
	uint32_t raw;
	struct {
		uint32_t POST_RGBMAP_NO_RSPD_CYC         : 32;
	} bits;
};

union REG_PRE_RAW_FRAME_VLD {
	uint32_t raw;
	struct {
		uint32_t PRE_RAW_FRAME_VLD               : 1;
	} bits;
};

union REG_PRE_RAW_DEBUG_STATE {
	uint32_t raw;
	struct {
		uint32_t POST_RAW_IDLE                   : 1;
		uint32_t PRE_RAW_IDLE                    : 1;
		uint32_t AE_0_DMA_IDLE                   : 1;
		uint32_t AE_1_DMA_IDLE                   : 1;
		uint32_t AWB_0_DMA_IDLE                  : 1;
		uint32_t AWB_1_DMA_IDLE                  : 1;
		uint32_t AF_DMA_IDLE                     : 1;
		uint32_t GMS_DMA_IDLE                    : 1;
		uint32_t RGBMAP_0_DMA_IDLE               : 1;
		uint32_t RGBMAP_1_DMA_IDLE               : 1;
		uint32_t LMAP_0_DMA_IDLE                 : 1;
		uint32_t LMAP_1_DMA_IDLE                 : 1;
		uint32_t ASYNC_FIFO_IDLE                 : 1;
		uint32_t RGBMAP_0_ASYNC_FIFO_IDLE        : 1;
		uint32_t RGBMAP_1_ASYNC_FIFO_IDLE        : 1;
	} bits;
};

union REG_PRE_RAW_ASYNC_STATE {
	uint32_t raw;
	struct {
		uint32_t ASYNC_SRC_HDK_EN                : 1;
		uint32_t ASYNC_DST_HDK_EN                : 1;
		uint32_t ASYNC_S2IP_RDY                  : 1;
		uint32_t ASYNC_IP2S_REQ                  : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t ASYNC_IN_CNT                    : 16;
	} bits;
};

union REG_PRE_RAW_RGBMAP_0_ASYNC_STATE {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_0_ASYNC_SRC_HDK_EN       : 1;
		uint32_t RGBMAP_0_ASYNC_DST_HDK_EN       : 1;
		uint32_t RGBMAP_0_ASYNC_S2IP_RDY         : 1;
		uint32_t RGBMAP_0_ASYNC_IP2S_REQ         : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t RGBMAP_0_ASYNC_IN_CNT           : 16;
	} bits;
};

union REG_PRE_RAW_RGBMAP_1_ASYNC_STATE {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_1_ASYNC_SRC_HDK_EN       : 1;
		uint32_t RGBMAP_1_ASYNC_DST_HDK_EN       : 1;
		uint32_t RGBMAP_1_ASYNC_S2IP_RDY         : 1;
		uint32_t RGBMAP_1_ASYNC_IP2S_REQ         : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t RGBMAP_1_ASYNC_IN_CNT           : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CSI_BDG_CTRL {
	uint32_t raw;
	struct {
		uint32_t CSI_MODE                        : 2;
		uint32_t CSI_IN_FORMAT                   : 1;
		uint32_t CSI_IN_YUV_FORMAT               : 1;
		uint32_t CH_NUM                          : 2;
		uint32_t DMA_WR_ENABLE                   : 1;
		uint32_t _rsv_7                          : 2;
		uint32_t Y_ONLY                          : 1;
		uint32_t PXL_DATA_SEL                    : 1;
		uint32_t VS_POL                          : 1;
		uint32_t HS_POL                          : 1;
		uint32_t RESET_MODE                      : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t RESET                           : 1;
		uint32_t _rsv_17                         : 7;
		uint32_t CSI_ENABLE                      : 1;
		uint32_t YUV2BAY_ENABLE                  : 1;
		uint32_t _rsv_26                         : 2;
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_CSI_BDG_SIZE {
	uint32_t raw;
	struct {
		uint32_t FRAME_WIDTHM1                   : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FRAME_HEIGHTM1                  : 13;
	} bits;
};

union REG_ISP_CSI_BDG_UP {
	uint32_t raw;
	struct {
		uint32_t CSI_UP_REG                      : 1;
	} bits;
};

union REG_ISP_CSI_BDG_PAT_GEN_CTRL {
	uint32_t raw;
	struct {
		uint32_t PAT_EN                          : 1;
		uint32_t GRA_INV                         : 1;
		uint32_t AUTO_EN                         : 1;
		uint32_t DITH_EN                         : 1;
		uint32_t SNOW_EN                         : 1;
		uint32_t FIX_MC                          : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t DITH_MD                         : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t BAYER_ID                        : 2;
	} bits;
};

union REG_ISP_CSI_BDG_PAT_IDX_CTRL {
	uint32_t raw;
	struct {
		uint32_t PAT_PRD                         : 8;
		uint32_t PAT_IDX                         : 5;
	} bits;
};

union REG_ISP_CSI_BDG_PAT_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t PAT_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PAT_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_PAT_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t PAT_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_BACKGROUND_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t FDE_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FDE_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_BACKGROUND_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t FDE_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_FIX_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t MDE_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MDE_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_FIX_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t MDE_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_MDE_V_SIZE {
	uint32_t raw;
	struct {
		uint32_t VMDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VMDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_MDE_H_SIZE {
	uint32_t raw;
	struct {
		uint32_t HMDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HMDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_FDE_V_SIZE {
	uint32_t raw;
	struct {
		uint32_t VFDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VFDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_FDE_H_SIZE {
	uint32_t raw;
	struct {
		uint32_t HFDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HFDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_HSYNC_CTRL {
	uint32_t raw;
	struct {
		uint32_t HS_STR                          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HS_STP                          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_VSYNC_CTRL {
	uint32_t raw;
	struct {
		uint32_t VS_STR                          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VS_STP                          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_TGEN_TT_SIZE {
	uint32_t raw;
	struct {
		uint32_t HTT                             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VTT                             : 13;
	} bits;
};

union REG_ISP_CSI_BDG_TGEN_ENABLE {
	uint32_t raw;
	struct {
		uint32_t INTERNAL_TIME_GEN_ENABLE        : 1;
	} bits;
};

union REG_ISP_CSI_BDG_RT_DEBUG_INFO_PXL_CNT {
	uint32_t raw;
	struct {
		uint32_t RT_DEBUG_INFO_PXL_CNT           : 32;
	} bits;
};

union REG_ISP_CSI_BDG_RT_DEBUG_INFO_LINE_CNT {
	uint32_t raw;
	struct {
		uint32_t RT_DEBUG_INFO_LINE_CNT          : 32;
	} bits;
};

union REG_ISP_CSI_BDG_FE_DEBUG_INFO_PXL_CNT {
	uint32_t raw;
	struct {
		uint32_t FE_DEBUG_INFO_PXL_CNT           : 32;
	} bits;
};

union REG_ISP_CSI_BDG_FE_DEBUG_INFO_LINE_CNT {
	uint32_t raw;
	struct {
		uint32_t FE_DEBUG_INFO_LINE_CNT          : 32;
	} bits;
};

union REG_ISP_CSI_BDG_DEBUG_INFO_FRAME_CNT {
	uint32_t raw;
	struct {
		uint32_t DEBUG_INFO_FRAME_CNT            : 32;
	} bits;
};

union REG_ISP_CSI_BDG_INTERRUPT_CTRL {
	uint32_t raw;
	struct {
		uint32_t FRAME_DROP_INT_EN               : 1;
		uint32_t FIFO_OVERFLOW_INT_EN            : 1;
		uint32_t FRAME_SIZE_ERROR_INT_EN         : 1;
		uint32_t VSYNC_RISING_INT_EN             : 1;
		uint32_t FRAME_START_INT_EN              : 1;
		uint32_t LINE_INTP_EN                    : 1;
	} bits;
};

union REG_ISP_CSI_BDG_INTERRUPT_STATUS {
	uint32_t raw;
	struct {
		uint32_t FRAME_DROP_INT                  : 1;
		uint32_t FIFO_OVERFLOW_INT               : 1;
		uint32_t FRAME_WIDTH_GT_INT              : 1;
		uint32_t FRAME_WIDTH_LS_INT              : 1;
		uint32_t FRAME_HEIGHT_GT_INT             : 1;
		uint32_t FRAME_HEIGHT_LS_INT             : 1;
		uint32_t VSYNC_RISING_INT                : 1;
		uint32_t FRAME_START_INT                 : 1;
		uint32_t FRAME_WIDTH_OVER_MAX_INT        : 1;
		uint32_t FRAME_HEIGHT_OVER_MAX_INT       : 1;
		uint32_t LINE_INTP_INT                   : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_IN                        : 16;
		uint32_t DUMMY_OUT                       : 16;
	} bits;
};

union REG_ISP_CSI_BDG_DEBUG_INFO_VSYNC_CNT {
	uint32_t raw;
	struct {
		uint32_t DEBUG_INFO_VSYNC_CNT            : 32;
	} bits;
};

union REG_ISP_CSI_BDG_DEBUG_INFO_SOF_CNT {
	uint32_t raw;
	struct {
		uint32_t DEBUG_INFO_SOF_CNT              : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_NO_RSPD_CYC_CNT {
	uint32_t raw;
	struct {
		uint32_t CH0_NO_RSPD_CYC                 : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_NO_RSPD_CYC_CNT {
	uint32_t raw;
	struct {
		uint32_t CH1_NO_RSPD_CYC                 : 32;
	} bits;
};

union REG_ISP_CSI_BDG_DEBUG_STATE {
	uint32_t raw;
	struct {
		uint32_t TOT_BLK_IDLE                    : 1;
		uint32_t TOT_DMA_IDLE                    : 1;
		uint32_t CSI_BDG_DMA_IDLE                : 1;
		uint32_t CSI_ASYNC_IDLE                  : 1;
		uint32_t RING_BUFF_IDLE                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DMA_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DMA_ST_DPCM_MODE                : 3;
		uint32_t _rsv_3                          : 13;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY {
	uint32_t raw;
	struct {
		uint32_t OUT_VSYNC_LINE_DELAY            : 12;
	} bits;
};

union REG_ISP_CSI_BDG_WR_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t WR_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t WR_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_RD_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t RD_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t RD_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CROP_EN                         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t HORZ_CROP_START                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HORZ_CROP_END                   : 13;
	} bits;
};

union REG_ISP_CSI_BDG_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t VERT_CROP_START                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VERT_CROP_END                   : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t LINE_INTP_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CHANNEL_LINE_COUNT {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_CNT                    : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_LINE_CNT                    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_HORZ_CROP_SE {
	uint32_t raw;
	struct {
		uint32_t HORZ_CROP_START_SE              : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HORZ_CROP_END_SE                : 13;
	} bits;
};

union REG_ISP_CSI_BDG_VERT_CROP_SE {
	uint32_t raw;
	struct {
		uint32_t VERT_CROP_START_SE              : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VERT_CROP_END_SE                : 13;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DMA_0 {
	uint32_t raw;
	struct {
		uint32_t DMA_ADDR_L                      : 32;
	} bits;
};

union REG_ISP_DMA_1 {
	uint32_t raw;
	struct {
		uint32_t DMA_ADDR_H                      : 8;
	} bits;
};

union REG_ISP_DMA_2 {
	uint32_t raw;
	struct {
		uint32_t DMA_CNT                         : 16;
	} bits;
};

union REG_ISP_DMA_3 {
	uint32_t raw;
	struct {
		uint32_t MIN_DMA_ADDR_L                  : 32;
	} bits;
};

union REG_ISP_DMA_4 {
	uint32_t raw;
	struct {
		uint32_t MIN_DMA_ADDR_H                  : 8;
	} bits;
};

union REG_ISP_DMA_5 {
	uint32_t raw;
	struct {
		uint32_t MAX_DMA_ADDR_L                  : 32;
	} bits;
};

union REG_ISP_DMA_6 {
	uint32_t raw;
	struct {
		uint32_t MAX_DMA_ADDR_H                  : 8;
	} bits;
};

union REG_ISP_DMA_7 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFFER_EN                  : 1;
	} bits;
};

union REG_ISP_DMA_8 {
	uint32_t raw;
	struct {
		uint32_t JOB_START                       : 1;
		uint32_t DMA_ABORT                       : 1;
	} bits;
};

union REG_ISP_DMA_9 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH                       : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t IMG_HEIGHT                      : 13;
	} bits;
};

union REG_ISP_DMA_10 {
	uint32_t raw;
	struct {
		uint32_t LINE_OFFSET                     : 16;
		uint32_t MOD_CTL                         : 3;
	} bits;
};

union REG_ISP_DMA_11 {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 8;
	} bits;
};

union REG_ISP_DMA_12 {
	uint32_t raw;
	struct {
		uint32_t INT_STS                         : 4;
	} bits;
};

union REG_ISP_DMA_13 {
	uint32_t raw;
	struct {
		uint32_t INT_STS_EN                      : 4;
	} bits;
};

union REG_ISP_DMA_14 {
	uint32_t raw;
	struct {
		uint32_t SHAW_SEL                        : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DBUS_SEL                        : 4;
	} bits;
};

union REG_ISP_DMA_15 {
	uint32_t raw;
	struct {
		uint32_t MAX_BURST_LEN                   : 8;
		uint32_t OT_ENABLE                       : 1;
		uint32_t SW_OVERWRITE                    : 1;
	} bits;
};

union REG_ISP_DMA_16 {
	uint32_t raw;
	struct {
		uint32_t DBUS                            : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CROP_0 {
	uint32_t raw;
	struct {
		uint32_t CROP_ENABLE                     : 1;
		uint32_t DMA_ENABLE                      : 1;
		uint32_t SHAW_READ_SEL                   : 1;
		uint32_t _rsv_3                          : 5;
		uint32_t DPCM_MODE                       : 3;
		uint32_t _rsv_11                         : 5;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_ISP_CROP_1 {
	uint32_t raw;
	struct {
		uint32_t CROP_START_Y                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CROP_END_Y                      : 12;
	} bits;
};

union REG_ISP_CROP_2 {
	uint32_t raw;
	struct {
		uint32_t CROP_START_X                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CROP_END_X                      : 12;
	} bits;
};

union REG_ISP_CROP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_BLC_0 {
	uint32_t raw;
	struct {
		uint32_t BLC_BYPASS                      : 1;
	} bits;
};

union REG_ISP_BLC_1 {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_BLC_2 {
	uint32_t raw;
	struct {
		uint32_t BLC_ENABLE                      : 1;
	} bits;
};

union REG_ISP_BLC_3 {
	uint32_t raw;
	struct {
		uint32_t BLC_OFFSET_R                    : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t BLC_OFFSET_GR                   : 10;
	} bits;
};

union REG_ISP_BLC_4 {
	uint32_t raw;
	struct {
		uint32_t BLC_OFFSET_GB                   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t BLC_OFFSET_B                    : 10;
	} bits;
};

union REG_ISP_BLC_5 {
	uint32_t raw;
	struct {
		uint32_t BLC_GAIN_R                      : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t BLC_GAIN_GR                     : 15;
	} bits;
};

union REG_ISP_BLC_6 {
	uint32_t raw;
	struct {
		uint32_t BLC_GAIN_GB                     : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t BLC_GAIN_B                      : 15;
	} bits;
};

union REG_ISP_BLC_7 {
	uint32_t raw;
	struct {
		uint32_t BLC_CHECKSUM                    : 32;
	} bits;
};

union REG_ISP_BLC_8 {
	uint32_t raw;
	struct {
		uint32_t BLC_INT                         : 1;
	} bits;
};

union REG_ISP_BLC_IMG_BAYERID {
	uint32_t raw;
	struct {
		uint32_t IMG_BAYERID                     : 2;
	} bits;
};

union REG_ISP_BLC_DUMMY {
	uint32_t raw;
	struct {
		uint32_t BLC_DUMMY                       : 16;
	} bits;
};

union REG_ISP_BLC_LOCATION {
	uint32_t raw;
	struct {
		uint32_t BLC_LOCATION                    : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LSCR_SHADOW_RD_SEL {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
	} bits;
};

union REG_ISP_LSCR_EN            {
	uint32_t raw;
	struct {
		uint32_t LSCR_EN                         : 1;
	} bits;
};

union REG_ISP_LSCR_INDEX_CLR     {
	uint32_t raw;
	struct {
		uint32_t LSCR_INDEX_CLR                  : 1;
	} bits;
};

union REG_ISP_LSCR_HSIZE         {
	uint32_t raw;
	struct {
		uint32_t LSCR_HSIZE                      : 13;
	} bits;
};

union REG_ISP_LSCR_VSIZE         {
	uint32_t raw;
	struct {
		uint32_t LSCR_VSIZE                      : 13;
	} bits;
};

union REG_ISP_LSCR_X_CENTER      {
	uint32_t raw;
	struct {
		uint32_t LSCR_X_CENTER                   : 13;
	} bits;
};

union REG_ISP_LSCR_Y_CENTER      {
	uint32_t raw;
	struct {
		uint32_t LSCR_Y_CENTER                   : 13;
	} bits;
};

union REG_ISP_LSCR_NORM_FACTOR   {
	uint32_t raw;
	struct {
		uint32_t LSCR_NORM_FACTOR                : 15;
	} bits;
};

union REG_ISP_LSCR_LSC_LUT       {
	uint32_t raw;
	struct {
		uint32_t LSCR_LSC_LUT                    : 12;
	} bits;
};

union REG_ISP_LSCR_LSC_STRENTH   {
	uint32_t raw;
	struct {
		uint32_t LSCR_LSC_STRENTH                : 12;
	} bits;
};

union REG_ISP_LSCR_DUMMY         {
	uint32_t raw;
	struct {
		uint32_t LSCR_DUMMY                      : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_AE_HIST_AE_HIST_STATUS {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_STATUS                  : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_GRACE_RESET             : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_MONITOR {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_MONITOR                 : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_BYPASS {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_BYPASS                  : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_KICKOFF {
	uint32_t raw;
	struct {
		uint32_t AE_ZERO_AE_SUM                  : 1;
		uint32_t _rsv_1                          : 1;
		uint32_t AE_WBGAIN_APPLY                 : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t LOADSHADOWREG                   : 1;
		uint32_t _rsv_5                          : 1;
		uint32_t HIST_ZEROHISTOGRAM              : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t HIST_WBGAIN_APPLY               : 1;
		uint32_t _rsv_9                          : 1;
		uint32_t AE_HIST_SHADOW_SELECT           : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_ENABLE {
	uint32_t raw;
	struct {
		uint32_t AE0_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AE0_GAIN_ENABLE                 : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t HIST0_ENABLE                    : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t HIST0_GAIN_ENABLE               : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t AE1_ENABLE                      : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t AE1_GAIN_ENABLE                 : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t HIST1_ENABLE                    : 1;
		uint32_t _rsv_25                         : 3;
		uint32_t HIST1_GAIN_ENABLE               : 1;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_OFFSETX {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_OFFSETX                 : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_OFFSETY {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_OFFSETY                 : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_NUMXM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_NUMXM1                  : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_NUMYM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_NUMYM1                  : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_WIDTH {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_WIDTH                   : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_HEIGHT                  : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_STS_DIV                 : 3;
	} bits;
};

union REG_ISP_AE_HIST_STS_HIST_MODE {
	uint32_t raw;
	struct {
		uint32_t STS_HIST0_MODE                  : 2;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_MONITOR_SELECT          : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_LOCATION                : 32;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_OFFSETX {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_OFFSETX                 : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_OFFSETY {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_OFFSETY                 : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_NUMXM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_NUMXM1                  : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_NUMYM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_NUMYM1                  : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_WIDTH {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_WIDTH                   : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_HEIGHT                  : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE1_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t STS_AE1_STS_DIV                 : 3;
	} bits;
};

union REG_ISP_AE_HIST_STS_HIST1_MODE {
	uint32_t raw;
	struct {
		uint32_t STS_HIST1_MODE                  : 2;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_BAYER_STARTING {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_BAYER_STARTING          : 2;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_DUMMY {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_DUMMY                   : 16;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_CHECKSUM {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_CHECKSUM                : 32;
	} bits;
};

union REG_ISP_AE_HIST_WBG_4 {
	uint32_t raw;
	struct {
		uint32_t AE0_WBG_RGAIN                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t AE0_WBG_GGAIN                   : 14;
	} bits;
};

union REG_ISP_AE_HIST_WBG_5 {
	uint32_t raw;
	struct {
		uint32_t AE0_WBG_BGAIN                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t AE1_WBG_BGAIN                   : 14;
	} bits;
};

union REG_ISP_AE_HIST_WBG_6 {
	uint32_t raw;
	struct {
		uint32_t AE1_WBG_RGAIN                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t AE1_WBG_GGAIN                   : 14;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_AWB_STATUS {
	uint32_t raw;
	struct {
		uint32_t AWB_STATUS                      : 32;
	} bits;
};

union REG_ISP_AWB_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t AWB_GRACE_RESET                 : 1;
	} bits;
};

union REG_ISP_AWB_MONITOR {
	uint32_t raw;
	struct {
		uint32_t AWB_MONITOR                     : 32;
	} bits;
};

union REG_ISP_AWB_SHADOW_SELECT {
	uint32_t raw;
	struct {
		uint32_t AWB_SHADOW_SELECT               : 1;
	} bits;
};

union REG_ISP_AWB_KICKOFF {
	uint32_t raw;
	struct {
		uint32_t AWB_ZERO_AWB_SUM                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AWB_WBGAIN_APPLY                : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t AWB_LOADSHADOWREG               : 1;
	} bits;
};

union REG_ISP_AWB_ENABLE {
	uint32_t raw;
	struct {
		uint32_t AWB_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AWB_GAIN_ENABLE                 : 1;
	} bits;
};

union REG_ISP_AWB_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t AWB_MONITOR_SELECT              : 32;
	} bits;
};

union REG_ISP_AWB_STS_OFFSETX {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_OFFSETX                 : 13;
	} bits;
};

union REG_ISP_AWB_STS_OFFSETY {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_OFFSETY                 : 13;
	} bits;
};

union REG_ISP_AWB_STS_NUMXM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_NUMXM1                  : 7;
	} bits;
};

union REG_ISP_AWB_STS_NUMYM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_NUMYM1                  : 7;
	} bits;
};

union REG_ISP_AWB_STS_WIDTH {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_WIDTH                   : 7;
	} bits;
};

union REG_ISP_AWB_STS_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_HEIGHT                  : 7;
	} bits;
};

union REG_ISP_AWB_STS_SKIPX {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_SKIPX                   : 3;
	} bits;
};

union REG_ISP_AWB_STS_SKIPY {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_SKIPY                   : 3;
	} bits;
};

union REG_ISP_AWB_STS_CORNER_AVG_EN {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_CORNER_AVG_EN           : 1;
	} bits;
};

union REG_ISP_AWB_STS_CORNER_SIZE {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_CORNER_SIZE             : 3;
	} bits;
};

union REG_ISP_AWB_STS_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_STS_DIV                 : 3;
	} bits;
};

union REG_ISP_AWB_STS_R_LOTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_R_LOTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_STS_R_UPTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_R_UPTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_STS_G_LOTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_G_LOTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_STS_G_UPTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_G_UPTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_STS_B_LOTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_B_LOTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_STS_B_UPTHD {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_B_UPTHD                 : 12;
	} bits;
};

union REG_ISP_AWB_WBG_4 {
	uint32_t raw;
	struct {
		uint32_t AWB_WBG_RGAIN                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t AWB_WBG_GGAIN                   : 14;
	} bits;
};

union REG_ISP_AWB_WBG_5 {
	uint32_t raw;
	struct {
		uint32_t AWB_WBG_BGAIN                   : 14;
	} bits;
};

union REG_ISP_AWB_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t AWB_MEM_SW_MODE                 : 1;
	} bits;
};

union REG_ISP_AWB_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t AWB_SW_RADDR                    : 7;
	} bits;
};

union REG_ISP_AWB_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AWB_LOCATION                    : 32;
	} bits;
};

union REG_ISP_AWB_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t AWB_RDATA                       : 31;
		uint32_t AWB_SW_READ                     : 1;
	} bits;
};

union REG_ISP_AWB_BAYER_STARTING {
	uint32_t raw;
	struct {
		uint32_t AWB_BAYER_STARTING              : 2;
	} bits;
};

union REG_ISP_AWB_DUMMY {
	uint32_t raw;
	struct {
		uint32_t AWB_DUMMY                       : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_AF_STATUS {
	uint32_t raw;
	struct {
		uint32_t AF_STATUS                       : 32;
	} bits;
};

union REG_ISP_AF_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t AF_GRACE_RESET                  : 1;
	} bits;
};

union REG_ISP_AF_MONITOR {
	uint32_t raw;
	struct {
		uint32_t AF_MONITOR                      : 32;
	} bits;
};

union REG_ISP_AF_BYPASS {
	uint32_t raw;
	struct {
		uint32_t AF_BYPASS                       : 1;
	} bits;
};

union REG_ISP_AF_KICKOFF {
	uint32_t raw;
	struct {
		uint32_t AF_ENABLE                       : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AF_WBGAIN_APPLY                 : 1;
	} bits;
};

union REG_ISP_AF_ENABLES {
	uint32_t raw;
	struct {
		uint32_t AF_HORIZON_0_ENABLE             : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AF_HORIZON_1_ENABLE             : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t AF_VERTICAL_0_ENABLE            : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t AF_GAMMA_ENABLE                 : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t AF_DPC_ENABLE                   : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t AF_HLC_ENABLE                   : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t AF_SHADOW_SELECT                : 1;
	} bits;
};

union REG_ISP_AF_OFFSET_X {
	uint32_t raw;
	struct {
		uint32_t AF_OFFSET_X                     : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t AF_OFFSET_Y                     : 13;
	} bits;
};

union REG_ISP_AF_MXN_IMAGE_WIDTH_M1 {
	uint32_t raw;
	struct {
		uint32_t AF_MXN_IMAGE_WIDTH              : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t AF_MXN_IMAGE_HEIGHT             : 13;
	} bits;
};

union REG_ISP_AF_BLOCK_WIDTH {
	uint32_t raw;
	struct {
		uint32_t AF_BLOCK_WIDTH                  : 8;
	} bits;
};

union REG_ISP_AF_BLOCK_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t AF_BLOCK_HEIGHT                 : 8;
	} bits;
};

union REG_ISP_AF_BLOCK_NUM_X {
	uint32_t raw;
	struct {
		uint32_t AF_BLOCK_NUM_X                  : 5;
	} bits;
};

union REG_ISP_AF_BLOCK_NUM_Y {
	uint32_t raw;
	struct {
		uint32_t AF_BLOCK_NUM_Y                  : 5;
	} bits;
};

union REG_ISP_AF_CROP_BAYERID {
	uint32_t raw;
	struct {
		uint32_t AF_CROP_BAYERID                 : 2;
	} bits;
};

union REG_ISP_AF_HOR_LOW_PASS_VALUE_SHIFT {
	uint32_t raw;
	struct {
		uint32_t AF_HOR_LOW_PASS_VALUE_SHIFT     : 4;
	} bits;
};

union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_0 {
	uint32_t raw;
	struct {
		uint32_t AF_CORNING_OFFSET_HORIZONTAL_0  : 18;
	} bits;
};

union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1 {
	uint32_t raw;
	struct {
		uint32_t AF_CORNING_OFFSET_HORIZONTAL_1  : 18;
	} bits;
};

union REG_ISP_AF_CORNING_OFFSET_VERTICAL {
	uint32_t raw;
	struct {
		uint32_t AF_CORNING_OFFSET_VERTICAL      : 13;
	} bits;
};

union REG_ISP_AF_HIGH_Y_THRE {
	uint32_t raw;
	struct {
		uint32_t AF_HIGH_Y_THRE                  : 12;
	} bits;
};

union REG_ISP_AF_LOW_PASS_HORIZON {
	uint32_t raw;
	struct {
		uint32_t AF_LOW_PASS_HORIZON_0           : 6;
		uint32_t AF_LOW_PASS_HORIZON_1           : 6;
		uint32_t AF_LOW_PASS_HORIZON_2           : 6;
		uint32_t AF_LOW_PASS_HORIZON_3           : 6;
		uint32_t AF_LOW_PASS_HORIZON_4           : 6;
	} bits;
};

union REG_ISP_AF_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AF_LOCATION                     : 32;
	} bits;
};

union REG_ISP_AF_HIGH_PASS_HORIZON_0 {
	uint32_t raw;
	struct {
		uint32_t AF_HIGH_PASS_HORIZON_0_0        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_0_1        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_0_2        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_0_3        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_0_4        : 6;
	} bits;
};

union REG_ISP_AF_HIGH_PASS_HORIZON_1 {
	uint32_t raw;
	struct {
		uint32_t AF_HIGH_PASS_HORIZON_1_0        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_1_1        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_1_2        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_1_3        : 6;
		uint32_t AF_HIGH_PASS_HORIZON_1_4        : 6;
	} bits;
};

union REG_ISP_AF_HIGH_PASS_VERTICAL_0 {
	uint32_t raw;
	struct {
		uint32_t AF_HIGH_PASS_VERTICAL_0_0       : 6;
		uint32_t AF_HIGH_PASS_VERTICAL_0_1       : 6;
		uint32_t AF_HIGH_PASS_VERTICAL_0_2       : 6;
	} bits;
};

union REG_ISP_AF_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t AF_MEM_SW_MODE                  : 1;
		uint32_t AF_R_MEM_SEL                    : 1;
		uint32_t AF_G_MEM_SEL                    : 1;
		uint32_t AF_B_MEM_SEL                    : 1;
		uint32_t AF_BLK_DIV_MEM_SEL              : 1;
		uint32_t AF_GAMMA_G_MEM_SEL              : 1;
		uint32_t AF_MAGFACTOR_MEM_SEL            : 1;
		uint32_t AF_BLK_DIV_DFF_SEL              : 1;
		uint32_t AF_GAMMA_G_DFF_SEL              : 1;
		uint32_t AF_MAGFACTOR_DFF_SEL            : 1;
	} bits;
};

union REG_ISP_AF_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t AF_MONITOR_SELECT               : 32;
	} bits;
};

union REG_ISP_AF_IMAGE_WIDTH {
	uint32_t raw;
	struct {
		uint32_t AF_IMAGE_WIDTH                  : 16;
	} bits;
};

union REG_ISP_AF_DUMMY {
	uint32_t raw;
	struct {
		uint32_t AF_DUMMY                        : 16;
	} bits;
};

union REG_ISP_AF_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t AF_SW_RADDR                     : 7;
	} bits;
};

union REG_ISP_AF_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t AF_RDATA                        : 31;
		uint32_t AF_SW_READ                      : 1;
	} bits;
};

union REG_ISP_AF_MXN_BORDER {
	uint32_t raw;
	struct {
		uint32_t AF_MXN_BORDER                   : 2;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_AF_GAMMA_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_DATA_E                    : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t GAMMA_DATA_O                    : 8;
		uint32_t _rsv_24                         : 7;
		uint32_t GAMMA_W                         : 1;
	} bits;
};

union REG_ISP_AF_GAMMA_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_WSEL                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_RSEL                      : 1;
	} bits;
};

union REG_ISP_AF_GAMMA_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MAX                       : 8;
	} bits;
};

union REG_ISP_AF_GAMMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ENABLE                    : 1;
		uint32_t GAMMA_SHDW_SEL                  : 1;
	} bits;
};

union REG_ISP_AF_GAMMA_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MEM_SW_MODE               : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_MEM_SEL                   : 1;
	} bits;
};

union REG_ISP_AF_GAMMA_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_SW_RADDR                  : 8;
	} bits;
};

union REG_ISP_AF_GAMMA_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_RDATA_R                   : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t GAMMA_SW_R                      : 1;
	} bits;
};

union REG_ISP_AF_GAMMA_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_AF_GAMMA_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_AF_GAMMA_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_AF_GAMMA_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

union REG_ISP_AF_GAMMA_PROG_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ST_ADDR                   : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t GAMMA_ST_W                      : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_GMS_STATUS {
	uint32_t raw;
	struct {
		uint32_t GMS_STATUS                      : 32;
	} bits;
};

union REG_ISP_GMS_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t GMS_GRACE_RESET                 : 1;
	} bits;
};

union REG_ISP_GMS_MONITOR {
	uint32_t raw;
	struct {
		uint32_t GMS_MONITOR                     : 32;
	} bits;
};

union REG_ISP_GMS_ENABLE {
	uint32_t raw;
	struct {
		uint32_t GMS_ENABLE                      : 1;
	} bits;
};

union REG_ISP_GMS_IMG_BAYERID {
	uint32_t raw;
	struct {
		uint32_t IMG_BAYERID                     : 2;
	} bits;
};

union REG_ISP_GMS_FLOW {
	uint32_t raw;
	struct {
		uint32_t GMS_ZEROGMSOGRAM                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GMS_SHADOW_SELECT               : 1;
	} bits;
};

union REG_ISP_GMS_START_X {
	uint32_t raw;
	struct {
		uint32_t GMS_START_X                     : 13;
	} bits;
};

union REG_ISP_GMS_START_Y {
	uint32_t raw;
	struct {
		uint32_t GMS_START_Y                     : 13;
	} bits;
};

union REG_ISP_GMS_LOCATION {
	uint32_t raw;
	struct {
		uint32_t GMS_LOCATION                    : 32;
	} bits;
};

union REG_ISP_GMS_X_SECTION_SIZE {
	uint32_t raw;
	struct {
		uint32_t GMS_X_SECTION_SIZE              : 10;
	} bits;
};

union REG_ISP_GMS_Y_SECTION_SIZE {
	uint32_t raw;
	struct {
		uint32_t GMS_Y_SECTION_SIZE              : 10;
	} bits;
};

union REG_ISP_GMS_X_GAP {
	uint32_t raw;
	struct {
		uint32_t GMS_X_GAP                       : 10;
	} bits;
};

union REG_ISP_GMS_Y_GAP {
	uint32_t raw;
	struct {
		uint32_t GMS_Y_GAP                       : 10;
	} bits;
};

union REG_ISP_GMS_DUMMY {
	uint32_t raw;
	struct {
		uint32_t GMS_DUMMY                       : 16;
	} bits;
};

union REG_ISP_GMS_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t GMS_MEM_SW_MODE                 : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GMS_MEM_SEL                     : 6;
	} bits;
};

union REG_ISP_GMS_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GMS_SW_RADDR                    : 11;
	} bits;
};

union REG_ISP_GMS_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t GMS_RDATA_R                     : 31;
		uint32_t GMS_SW_R                        : 1;
	} bits;
};

union REG_ISP_GMS_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t GMS_MONITOR_SELECT              : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_WBG_0 {
	uint32_t raw;
	struct {
		uint32_t WBG_BYPASS                      : 1;
	} bits;
};

union REG_ISP_WBG_1 {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_WBG_2 {
	uint32_t raw;
	struct {
		uint32_t WBG_ENABLE                      : 1;
	} bits;
};

union REG_ISP_WBG_4 {
	uint32_t raw;
	struct {
		uint32_t WBG_RGAIN                       : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t WBG_GGAIN                       : 14;
	} bits;
};

union REG_ISP_WBG_5 {
	uint32_t raw;
	struct {
		uint32_t WBG_BGAIN                       : 14;
	} bits;
};

union REG_ISP_WBG_6 {
	uint32_t raw;
	struct {
		uint32_t WBG_CHECKSUM                    : 32;
	} bits;
};

union REG_ISP_WBG_7 {
	uint32_t raw;
	struct {
		uint32_t WBG_INT                         : 1;
	} bits;
};

union REG_ISP_WBG_IMG_BAYERID {
	uint32_t raw;
	struct {
		uint32_t IMG_BAYERID                     : 2;
	} bits;
};

union REG_ISP_WBG_DUMMY {
	uint32_t raw;
	struct {
		uint32_t WBG_DUMMY                       : 32;
	} bits;
};

union REG_ISP_WBG_LOCATION {
	uint32_t raw;
	struct {
		uint32_t WBG_LOCATION                    : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LMAP_LMP_0 {
	uint32_t raw;
	struct {
		uint32_t LMAP_ENABLE                     : 1;
		uint32_t LMAP_W_BIT                      : 3;
		uint32_t LMAP_Y_MODE                     : 2;
		uint32_t LMAP_THD_L                      : 12;
		uint32_t LMAP_THD_H                      : 12;
		uint32_t LMAP_SOFTRST                    : 1;
	} bits;
};

union REG_ISP_LMAP_LMP_1 {
	uint32_t raw;
	struct {
		uint32_t LMAP_CROP_WIDTHM1               : 13;
		uint32_t LMAP_CROP_HEIGHTM1              : 13;
		uint32_t LMAP_H_BIT                      : 3;
		uint32_t LMAP_BAYER_ID                   : 2;
		uint32_t LMAP_SHDW_SEL                   : 1;
	} bits;
};

union REG_ISP_LMAP_LMP_2 {
	uint32_t raw;
	struct {
		uint32_t LMAP_W_GRID_NUM                 : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t LMAP_H_GRID_NUM                 : 9;
	} bits;
};

union REG_ISP_LMAP_LMP_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t LMAP_DEBUG_0                    : 32;
	} bits;
};

union REG_ISP_LMAP_LMP_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t LMAP_DEBUG_1                    : 32;
	} bits;
};

union REG_ISP_LMAP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t LMAP_DUMMY                      : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGBMAP_0 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_ENABLE                   : 1;
		uint32_t RGBMAP_W_BIT                    : 3;
		uint32_t RGBMAP_H_BIT                    : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t IMG_BAYERID                     : 2;
		uint32_t RGBMAP_W_GRID_NUM               : 10;
		uint32_t RGBMAP_H_GRID_NUM               : 10;
		uint32_t RGBMAP_SOFTRST                  : 1;
	} bits;
};

union REG_ISP_RGBMAP_1 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1                     : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t IMG_HEIGHTM1                    : 13;
		uint32_t RGBMAP_SHDW_SEL                 : 1;
	} bits;
};

union REG_ISP_RGBMAP_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_DEBUG_0                  : 32;
	} bits;
};

union REG_ISP_RGBMAP_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_DEBUG_1                  : 32;
	} bits;
};

union REG_ISP_RGBMAP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_DUMMY                    : 32;
	} bits;
};

union REG_ISP_RGBMAP_2 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_DMA_ENABLE               : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_RAW_TOP_RAW_0 {
	uint32_t raw;
	struct {
		uint32_t SVN_VERSION                     : 32;
	} bits;
};

union REG_RAW_TOP_READ_SEL {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_RAW_TOP_RAW_1 {
	uint32_t raw;
	struct {
		uint32_t TIMESTAMP                       : 32;
	} bits;
};

union REG_RAW_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t LS_CROP_DST_SEL                 : 1;
	} bits;
};

union REG_RAW_TOP_RAW_2 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1                     : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHTM1                    : 14;
	} bits;
};

union REG_RAW_TOP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_DUMMY                   : 32;
	} bits;
};

union REG_RAW_TOP_RAW_4 {
	uint32_t raw;
	struct {
		uint32_t YUV_IN_MODE                     : 1;
	} bits;
};

union REG_RAW_TOP_STATUS {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_STATUS                  : 32;
	} bits;
};

union REG_RAW_TOP_DEBUG {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_DEBUG                   : 32;
	} bits;
};

union REG_RAW_TOP_DEBUG_SELECT {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_DEBUG_SELECT            : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DPC_0 {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t BORDER                          : 2;
		uint32_t COLOR_MODE                      : 2;
	} bits;
};

union REG_ISP_DPC_1 {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_DPC_2 {
	uint32_t raw;
	struct {
		uint32_t DPC_ENABLE                      : 1;
		uint32_t GE_ENABLE                       : 1;
		uint32_t DPC_DYNAMICBPC_ENABLE           : 1;
		uint32_t DPC_STATICBPC_ENABLE            : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t DPC_CLUSTER_SIZE                : 2;
	} bits;
};

union REG_ISP_DPC_3 {
	uint32_t raw;
	struct {
		uint32_t DPC_R_BRIGHT_PIXEL_RATIO        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DPC_G_BRIGHT_PIXEL_RATIO        : 10;
	} bits;
};

union REG_ISP_DPC_4 {
	uint32_t raw;
	struct {
		uint32_t DPC_B_BRIGHT_PIXEL_RATIO        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DPC_R_DARK_PIXEL_RATIO          : 10;
	} bits;
};

union REG_ISP_DPC_5 {
	uint32_t raw;
	struct {
		uint32_t DPC_G_DARK_PIXEL_RATIO          : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DPC_B_DARK_PIXEL_RATIO          : 10;
	} bits;
};

union REG_ISP_DPC_6 {
	uint32_t raw;
	struct {
		uint32_t DPC_R_DARK_PIXEL_MINDIFF        : 8;
		uint32_t DPC_G_DARK_PIXEL_MINDIFF        : 8;
		uint32_t DPC_B_DARK_PIXEL_MINDIFF        : 8;
	} bits;
};

union REG_ISP_DPC_7 {
	uint32_t raw;
	struct {
		uint32_t DPC_R_BRIGHT_PIXEL_UPBOUD_RATIO : 8;
		uint32_t DPC_G_BRIGHT_PIXEL_UPBOUD_RATIO : 8;
		uint32_t DPC_B_BRIGHT_PIXEL_UPBOUD_RATIO : 8;
	} bits;
};

union REG_ISP_DPC_8 {
	uint32_t raw;
	struct {
		uint32_t DPC_FLAT_THRE_MIN_RB            : 8;
		uint32_t DPC_FLAT_THRE_MIN_G             : 8;
	} bits;
};

union REG_ISP_DPC_9 {
	uint32_t raw;
	struct {
		uint32_t DPC_FLAT_THRE_R                 : 8;
		uint32_t DPC_FLAT_THRE_G                 : 8;
		uint32_t DPC_FLAT_THRE_B                 : 8;
	} bits;
};

union REG_ISP_DPC_10 {
	uint32_t raw;
	struct {
		uint32_t GE_STRENGTH                     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t GE_COMBINEWEIGHT                : 4;
	} bits;
};

union REG_ISP_DPC_11 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE1                        : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE2                        : 12;
	} bits;
};

union REG_ISP_DPC_12 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE3                        : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE4                        : 12;
	} bits;
};

union REG_ISP_DPC_13 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE11                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE21                       : 12;
	} bits;
};

union REG_ISP_DPC_14 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE31                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE41                       : 12;
	} bits;
};

union REG_ISP_DPC_15 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE12                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE22                       : 12;
	} bits;
};

union REG_ISP_DPC_16 {
	uint32_t raw;
	struct {
		uint32_t GE_THRE32                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GE_THRE42                       : 12;
	} bits;
};

union REG_ISP_DPC_17 {
	uint32_t raw;
	struct {
		uint32_t DPC_MEM0_IMG0_ADDR              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t DPC_MEM0_IMG1_ADDR              : 12;
		uint32_t _rsv_28                         : 2;
		uint32_t DPC_MEM0_IMG_ADDR_SEL           : 1;
		uint32_t DPC_MEM_SW_MODE                 : 1;
	} bits;
};

union REG_ISP_DPC_18 {
	uint32_t raw;
	struct {
		uint32_t DPC_SW_RADDR                    : 12;
	} bits;
};

union REG_ISP_DPC_19 {
	uint32_t raw;
	struct {
		uint32_t DPC_RDATA_R                     : 24;
		uint32_t _rsv_24                         : 7;
		uint32_t DPC_SW_R                        : 1;
	} bits;
};

union REG_ISP_DPC_MEM_W0 {
	uint32_t raw;
	struct {
		uint32_t DPC_BP_MEM_D                    : 24;
		uint32_t _rsv_24                         : 7;
		uint32_t DPC_BP_MEM_W                    : 1;
	} bits;
};

union REG_ISP_DPC_WINDOW {
	uint32_t raw;
	struct {
		uint32_t IMG_WD                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t IMG_HT                          : 12;
	} bits;
};

union REG_ISP_DPC_MEM_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t DPC_BP_MEM_ST_ADDR              : 12;
		uint32_t _rsv_12                         : 19;
		uint32_t DPC_BP_MEM_ST_ADDR_W            : 1;
	} bits;
};

union REG_ISP_DPC_CHECKSUM {
	uint32_t raw;
	struct {
		uint32_t DPC_CHECKSUM                    : 32;
	} bits;
};

union REG_ISP_DPC_INT {
	uint32_t raw;
	struct {
		uint32_t DPC_INT                         : 1;
	} bits;
};

union REG_ISP_DPC_20 {
	uint32_t raw;
	struct {
		uint32_t PROB_OUT_SEL                    : 4;
		uint32_t PROB_PERFMT                     : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PROB_FMT                        : 6;
	} bits;
};

union REG_ISP_DPC_21 {
	uint32_t raw;
	struct {
		uint32_t PROB_LINE                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_PIX                        : 12;
	} bits;
};

union REG_ISP_DPC_22 {
	uint32_t raw;
	struct {
		uint32_t DPC_DBG0                        : 32;
	} bits;
};

union REG_ISP_DPC_23 {
	uint32_t raw;
	struct {
		uint32_t DPC_DBG1                        : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LSC_STATUS {
	uint32_t raw;
	struct {
		uint32_t LSC_STATUS                      : 32;
	} bits;
};

union REG_ISP_LSC_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t LSC_GRACE_RESET                 : 1;
	} bits;
};

union REG_ISP_LSC_MONITOR {
	uint32_t raw;
	struct {
		uint32_t LSC_MONITOR                     : 32;
	} bits;
};

union REG_ISP_LSC_ENABLE {
	uint32_t raw;
	struct {
		uint32_t LSC_ENABLE                      : 1;
	} bits;
};

union REG_ISP_LSC_KICKOFF {
	uint32_t raw;
	struct {
		uint32_t LSC_KICKOFF                     : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t LSC_GAINMOVER_ENABLE            : 1;
		uint32_t _rsv_5                          : 7;
		uint32_t LSC_SHADOW_SELECT               : 1;
	} bits;
};

union REG_ISP_LSC_STRENGTH {
	uint32_t raw;
	struct {
		uint32_t LSC_STRENGTH                    : 12;
	} bits;
};

union REG_ISP_LSC_IMG_BAYERID {
	uint32_t raw;
	struct {
		uint32_t IMG_BAYERID                     : 2;
	} bits;
};

union REG_ISP_LSC_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t LSC_MONITOR_SELECT              : 32;
	} bits;
};

union REG_ISP_LSC_BLK_NUM_SELECT {
	uint32_t raw;
	struct {
		uint32_t LSC_BLK_NUM_SELECT              : 1;
	} bits;
};

union REG_ISP_LSC_GAIN_BASE {
	uint32_t raw;
	struct {
		uint32_t LSC_GAIN_BASE                   : 2;
	} bits;
};

union REG_ISP_LSC_XSTEP {
	uint32_t raw;
	struct {
		uint32_t LSC_XSTEP                       : 15;
	} bits;
};

union REG_ISP_LSC_YSTEP {
	uint32_t raw;
	struct {
		uint32_t LSC_YSTEP                       : 15;
	} bits;
};

union REG_ISP_LSC_IMGX0 {
	uint32_t raw;
	struct {
		uint32_t LSC_IMGX0                       : 22;
	} bits;
};

union REG_ISP_LSC_IMGY0 {
	uint32_t raw;
	struct {
		uint32_t LSC_IMGY0                       : 22;
	} bits;
};

union REG_ISP_LSC_KERNEL_TABLE_WRITE {
	uint32_t raw;
	struct {
		uint32_t LSC_KERNEL_TABLE_WRITE          : 1;
	} bits;
};

union REG_ISP_LSC_KERNEL_TABLE_DATA {
	uint32_t raw;
	struct {
		uint32_t LSC_KERNEL_TABLE_DATA           : 32;
	} bits;
};

union REG_ISP_LSC_KERNEL_TABLE_CTRL {
	uint32_t raw;
	struct {
		uint32_t LSC_KERNEL_TABLE_START          : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t LSC_KERNEL_TABLE_W              : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t LSC_KERNEL_TABLE_DONE           : 1;
	} bits;
};

union REG_ISP_LSC_DUMMY {
	uint32_t raw;
	struct {
		uint32_t LSC_DUMMY                       : 16;
		uint32_t LSC_DEBUG                       : 16;
	} bits;
};

union REG_ISP_LSC_LOCATION {
	uint32_t raw;
	struct {
		uint32_t LSC_LOCATION                    : 32;
	} bits;
};

union REG_ISP_LSC_1ST_RUNHIT {
	uint32_t raw;
	struct {
		uint32_t LSC_1ST_RUNHIT                  : 32;
	} bits;
};

union REG_ISP_LSC_COMPARE_VALUE {
	uint32_t raw;
	struct {
		uint32_t LSC_COMPARE_VALUE               : 32;
	} bits;
};

union REG_ISP_LSC_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t LSC_MEM_SW_MODE                 : 1;
		uint32_t _rsv_1                          : 4;
		uint32_t LSC_CUBIC_KERNEL_MEM_SEL        : 1;
	} bits;
};

union REG_ISP_LSC_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t LSC_SW_RADDR                    : 7;
	} bits;
};

union REG_ISP_LSC_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t LSC_RDATA                       : 31;
		uint32_t LSC_SW_READ                     : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_FUSION_FS_CTRL {
	uint32_t raw;
	struct {
		uint32_t FS_ENABLE                       : 1;
		uint32_t FS_BPSS                         : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t FS_MC_ENABLE                    : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FS_OUT_SEL                      : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t FS_S_MAX                        : 20;
	} bits;
};

union REG_ISP_FUSION_FS_FRAME_SIZE {
	uint32_t raw;
	struct {
		uint32_t FS_WIDTHM1                      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FS_HEIGHTM1                     : 13;
	} bits;
};

union REG_ISP_FUSION_FS_SE_GAIN {
	uint32_t raw;
	struct {
		uint32_t FS_LS_GAIN                      : 14;
	} bits;
};

union REG_ISP_FUSION_FS_LUMA_THD {
	uint32_t raw;
	struct {
		uint32_t FS_LUMA_THD_L                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_LUMA_THD_H                   : 12;
	} bits;
};

union REG_ISP_FUSION_FS_WGT {
	uint32_t raw;
	struct {
		uint32_t FS_WGT_MAX                      : 8;
		uint32_t FS_WGT_MIN                      : 8;
	} bits;
};

union REG_ISP_FUSION_FS_WGT_SLOPE {
	uint32_t raw;
	struct {
		uint32_t FS_WGT_SLP                      : 18;
	} bits;
};

union REG_ISP_FUSION_FS_SHDW_READ_SEL {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_FUSION_FS_UP {
	uint32_t raw;
	struct {
		uint32_t FS_UP_REG                       : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LTM_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t LTM_ENABLE                      : 1;
		uint32_t DTONE_EHN_EN                    : 1;
		uint32_t BTONE_EHN_EN                    : 1;
		uint32_t DARK_LCE_EN                     : 1;
		uint32_t BRIT_LCE_EN                     : 1;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t BAYER_ID                        : 2;
		uint32_t _rsv_10                         : 2;
		uint32_t DBG_ENABLE                      : 1;
		uint32_t DBG_MODE                        : 3;
		uint32_t DE_MAX_THR                      : 12;
	} bits;
};

union REG_ISP_LTM_BLK_SIZE {
	uint32_t raw;
	struct {
		uint32_t HORZ_BLK_SIZE                   : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t BLK_WIDTHM1                     : 9;
		uint32_t _rsv_13                         : 3;
		uint32_t BLK_HEIGHTM1                    : 9;
		uint32_t _rsv_25                         : 3;
		uint32_t VERT_BLK_SIZE                   : 3;
	} bits;
};

union REG_ISP_LTM_FRAME_SIZE {
	uint32_t raw;
	struct {
		uint32_t FRAME_WIDTHM1                   : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FRAME_HEIGHTM1                  : 13;
	} bits;
};

union REG_ISP_LTM_BE_STRTH_CTRL {
	uint32_t raw;
	struct {
		uint32_t BE_LMAP_THR                     : 8;
		uint32_t BE_STRTH_DSHFT                  : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t BE_STRTH_GAIN                   : 11;
	} bits;
};

union REG_ISP_LTM_DE_STRTH_CTRL {
	uint32_t raw;
	struct {
		uint32_t DE_LMAP_THR                     : 8;
		uint32_t DE_STRTH_DSHFT                  : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t DE_STRTH_GAIN                   : 11;
	} bits;
};

union REG_ISP_LTM_FILTER_WIN_SIZE_CTRL {
	uint32_t raw;
	struct {
		uint32_t LMAP0_LP_RNG                    : 3;
		uint32_t _rsv_3                          : 5;
		uint32_t LMAP1_LP_RNG                    : 3;
		uint32_t _rsv_11                         : 5;
		uint32_t BE_RNG                          : 4;
		uint32_t _rsv_20                         : 4;
		uint32_t DE_RNG                          : 4;
	} bits;
};

union REG_ISP_LTM_BGAIN_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t BRI_IN_THD_L                    : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t BRI_IN_THD_H                    : 10;
		uint32_t _rsv_22                         : 2;
		uint32_t BRI_OUT_THD_L                   : 8;
	} bits;
};

union REG_ISP_LTM_BGAIN_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t BRI_OUT_THD_H                   : 8;
		uint32_t BRI_IN_GAIN_SLOP                : 21;
	} bits;
};

union REG_ISP_LTM_DGAIN_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t DAR_IN_THD_L                    : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t DAR_IN_THD_H                    : 10;
		uint32_t _rsv_22                         : 2;
		uint32_t DAR_OUT_THD_L                   : 8;
	} bits;
};

union REG_ISP_LTM_DGAIN_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t DAR_OUT_THD_H                   : 8;
		uint32_t DAR_IN_GAIN_SLOP                : 21;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_0               : 12;
		uint32_t BRI_LCE_UP_THD_0                : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_1               : 12;
		uint32_t BRI_LCE_UP_THD_1                : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_2 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_2               : 12;
		uint32_t BRI_LCE_UP_THD_2                : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_3 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_3               : 12;
		uint32_t BRI_LCE_UP_THD_3                : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_4 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_SLOPE_0         : 25;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_5 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_SLOPE_1         : 25;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_6 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_UP_GAIN_SLOPE_2         : 25;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_7 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_0             : 12;
		uint32_t BRI_LCE_DOWN_THD_0              : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_8 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_1             : 12;
		uint32_t BRI_LCE_DOWN_THD_1              : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_9 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_2             : 12;
		uint32_t BRI_LCE_DOWN_THD_2              : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_10 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_3             : 12;
		uint32_t BRI_LCE_DOWN_THD_3              : 20;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_11 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_SLOPE_0       : 25;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_12 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_SLOPE_1       : 25;
	} bits;
};

union REG_ISP_LTM_BRI_LCE_CTRL_13 {
	uint32_t raw;
	struct {
		uint32_t BRI_LCE_DOWN_GAIN_SLOPE_2       : 25;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_0                  : 12;
		uint32_t DAR_LCE_DIFF_THD_0              : 20;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_1                  : 12;
		uint32_t DAR_LCE_DIFF_THD_1              : 20;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_2 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_2                  : 12;
		uint32_t DAR_LCE_DIFF_THD_2              : 20;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_3 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_3                  : 12;
		uint32_t DAR_LCE_DIFF_THD_3              : 20;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_4 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_SLOPE_0            : 17;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_5 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_SLOPE_1            : 17;
	} bits;
};

union REG_ISP_LTM_DAR_LCE_CTRL_6 {
	uint32_t raw;
	struct {
		uint32_t DAR_LCE_GAIN_SLOPE_2            : 17;
	} bits;
};

union REG_ISP_LTM_CURVE_QUAN_BIT {
	uint32_t raw;
	struct {
		uint32_t DCRV_QUAN_BIT                   : 4;
		uint32_t BCRV_QUAN_BIT                   : 4;
		uint32_t GCRV_QUAN_BIT_0                 : 4;
		uint32_t GCRV_QUAN_BIT_1                 : 4;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIST_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIST_WGT_00            : 5;
		uint32_t DARK_LPF_DIST_WGT_01            : 5;
		uint32_t DARK_LPF_DIST_WGT_02            : 5;
		uint32_t DARK_LPF_DIST_WGT_03            : 5;
		uint32_t DARK_LPF_DIST_WGT_04            : 5;
		uint32_t DARK_LPF_DIST_WGT_05            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIST_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIST_WGT_06            : 5;
		uint32_t DARK_LPF_DIST_WGT_07            : 5;
		uint32_t DARK_LPF_DIST_WGT_08            : 5;
		uint32_t DARK_LPF_DIST_WGT_09            : 5;
		uint32_t DARK_LPF_DIST_WGT_10            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIFF_WGT_00            : 5;
		uint32_t DARK_LPF_DIFF_WGT_01            : 5;
		uint32_t DARK_LPF_DIFF_WGT_02            : 5;
		uint32_t DARK_LPF_DIFF_WGT_03            : 5;
		uint32_t DARK_LPF_DIFF_WGT_04            : 5;
		uint32_t DARK_LPF_DIFF_WGT_05            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIFF_WGT_06            : 5;
		uint32_t DARK_LPF_DIFF_WGT_07            : 5;
		uint32_t DARK_LPF_DIFF_WGT_08            : 5;
		uint32_t DARK_LPF_DIFF_WGT_09            : 5;
		uint32_t DARK_LPF_DIFF_WGT_10            : 5;
		uint32_t DARK_LPF_DIFF_WGT_11            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_2 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIFF_WGT_12            : 5;
		uint32_t DARK_LPF_DIFF_WGT_13            : 5;
		uint32_t DARK_LPF_DIFF_WGT_14            : 5;
		uint32_t DARK_LPF_DIFF_WGT_15            : 5;
		uint32_t DARK_LPF_DIFF_WGT_16            : 5;
		uint32_t DARK_LPF_DIFF_WGT_17            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_3 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIFF_WGT_18            : 5;
		uint32_t DARK_LPF_DIFF_WGT_19            : 5;
		uint32_t DARK_LPF_DIFF_WGT_20            : 5;
		uint32_t DARK_LPF_DIFF_WGT_21            : 5;
		uint32_t DARK_LPF_DIFF_WGT_22            : 5;
		uint32_t DARK_LPF_DIFF_WGT_23            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_4 {
	uint32_t raw;
	struct {
		uint32_t DARK_LPF_DIFF_WGT_24            : 5;
		uint32_t DARK_LPF_DIFF_WGT_25            : 5;
		uint32_t DARK_LPF_DIFF_WGT_26            : 5;
		uint32_t DARK_LPF_DIFF_WGT_27            : 5;
		uint32_t DARK_LPF_DIFF_WGT_28            : 5;
		uint32_t DARK_LPF_DIFF_WGT_29            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIST_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIST_WGT_00            : 5;
		uint32_t BRIT_LPF_DIST_WGT_01            : 5;
		uint32_t BRIT_LPF_DIST_WGT_02            : 5;
		uint32_t BRIT_LPF_DIST_WGT_03            : 5;
		uint32_t BRIT_LPF_DIST_WGT_04            : 5;
		uint32_t BRIT_LPF_DIST_WGT_05            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIST_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIST_WGT_06            : 5;
		uint32_t BRIT_LPF_DIST_WGT_07            : 5;
		uint32_t BRIT_LPF_DIST_WGT_08            : 5;
		uint32_t BRIT_LPF_DIST_WGT_09            : 5;
		uint32_t BRIT_LPF_DIST_WGT_10            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIFF_WGT_00            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_01            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_02            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_03            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_04            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_05            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIFF_WGT_06            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_07            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_08            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_09            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_10            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_11            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_2 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIFF_WGT_12            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_13            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_14            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_15            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_16            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_17            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_3 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIFF_WGT_18            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_19            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_20            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_21            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_22            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_23            : 5;
	} bits;
};

union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_4 {
	uint32_t raw;
	struct {
		uint32_t BRIT_LPF_DIFF_WGT_24            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_25            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_26            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_27            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_28            : 5;
		uint32_t BRIT_LPF_DIFF_WGT_29            : 5;
	} bits;
};

union REG_ISP_LTM_BE_DIST_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t BE_DIST_WGT_00                  : 5;
		uint32_t BE_DIST_WGT_01                  : 5;
		uint32_t BE_DIST_WGT_02                  : 5;
		uint32_t BE_DIST_WGT_03                  : 5;
		uint32_t BE_DIST_WGT_04                  : 5;
		uint32_t BE_DIST_WGT_05                  : 5;
	} bits;
};

union REG_ISP_LTM_BE_DIST_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t BE_DIST_WGT_06                  : 5;
		uint32_t BE_DIST_WGT_07                  : 5;
		uint32_t BE_DIST_WGT_08                  : 5;
		uint32_t BE_DIST_WGT_09                  : 5;
		uint32_t BE_DIST_WGT_10                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_DIST_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t DE_DIST_WGT_00                  : 5;
		uint32_t DE_DIST_WGT_01                  : 5;
		uint32_t DE_DIST_WGT_02                  : 5;
		uint32_t DE_DIST_WGT_03                  : 5;
		uint32_t DE_DIST_WGT_04                  : 5;
		uint32_t DE_DIST_WGT_05                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_DIST_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t DE_DIST_WGT_06                  : 5;
		uint32_t DE_DIST_WGT_07                  : 5;
		uint32_t DE_DIST_WGT_08                  : 5;
		uint32_t DE_DIST_WGT_09                  : 5;
		uint32_t DE_DIST_WGT_10                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_LUMA_WGT_0 {
	uint32_t raw;
	struct {
		uint32_t DE_LUMA_WGT_00                  : 5;
		uint32_t DE_LUMA_WGT_01                  : 5;
		uint32_t DE_LUMA_WGT_02                  : 5;
		uint32_t DE_LUMA_WGT_03                  : 5;
		uint32_t DE_LUMA_WGT_04                  : 5;
		uint32_t DE_LUMA_WGT_05                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_LUMA_WGT_1 {
	uint32_t raw;
	struct {
		uint32_t DE_LUMA_WGT_06                  : 5;
		uint32_t DE_LUMA_WGT_07                  : 5;
		uint32_t DE_LUMA_WGT_08                  : 5;
		uint32_t DE_LUMA_WGT_09                  : 5;
		uint32_t DE_LUMA_WGT_10                  : 5;
		uint32_t DE_LUMA_WGT_11                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_LUMA_WGT_2 {
	uint32_t raw;
	struct {
		uint32_t DE_LUMA_WGT_12                  : 5;
		uint32_t DE_LUMA_WGT_13                  : 5;
		uint32_t DE_LUMA_WGT_14                  : 5;
		uint32_t DE_LUMA_WGT_15                  : 5;
		uint32_t DE_LUMA_WGT_16                  : 5;
		uint32_t DE_LUMA_WGT_17                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_LUMA_WGT_3 {
	uint32_t raw;
	struct {
		uint32_t DE_LUMA_WGT_18                  : 5;
		uint32_t DE_LUMA_WGT_19                  : 5;
		uint32_t DE_LUMA_WGT_20                  : 5;
		uint32_t DE_LUMA_WGT_21                  : 5;
		uint32_t DE_LUMA_WGT_22                  : 5;
		uint32_t DE_LUMA_WGT_23                  : 5;
	} bits;
};

union REG_ISP_LTM_DE_LUMA_WGT_4 {
	uint32_t raw;
	struct {
		uint32_t DE_LUMA_WGT_24                  : 5;
		uint32_t DE_LUMA_WGT_25                  : 5;
		uint32_t DE_LUMA_WGT_26                  : 5;
		uint32_t DE_LUMA_WGT_27                  : 5;
		uint32_t DE_LUMA_WGT_28                  : 5;
		uint32_t DE_LUMA_WGT_29                  : 5;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_DATA_E              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t DTONE_CURVE_DATA_O              : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t DTONE_CURVE_W                   : 1;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_WSEL                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DTONE_CURVE_RSEL                : 1;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_MAX                 : 12;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_CTRL {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_ADDR_RST            : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DTONE_CURVE_PROG_HDK_DIS        : 1;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_MEM_SW_MODE         : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DTONE_CURVE_MEM_SEL             : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t DTONE_CURVE_INS_MEM_SEL         : 1;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_SW_RADDR            : 8;
	} bits;
};

union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t DTONE_CURVE_RDATA               : 12;
		uint32_t _rsv_12                         : 19;
		uint32_t DTONE_CURVE_SW_RD               : 1;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_DATA_E              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BTONE_CURVE_DATA_O              : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t BTONE_CURVE_W                   : 1;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_WSEL                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t BTONE_CURVE_RSEL                : 1;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_MAX                 : 12;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_CTRL {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_ADDR_RST            : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t BTONE_CURVE_PROG_HDK_DIS        : 1;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_MEM_SW_MODE         : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t BTONE_CURVE_MEM_SEL             : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t BTONE_CURVE_INS_MEM_SEL         : 1;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_SW_RADDR            : 9;
	} bits;
};

union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t BTONE_CURVE_RDATA               : 12;
		uint32_t _rsv_12                         : 19;
		uint32_t BTONE_CURVE_SW_RD               : 1;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_DATA_E             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GLOBAL_CURVE_DATA_O             : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t GLOBAL_CURVE_W                  : 1;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_WSEL               : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GLOBAL_CURVE_RSEL               : 1;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_MAX                : 12;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_CTRL {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_ADDR_RST           : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GLOBAL_CURVE_PROG_HDK_DIS       : 1;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_MEM_SW_MODE        : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GLOBAL_CURVE_MEM_SEL            : 1;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_SW_RADDR           : 10;
	} bits;
};

union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_CURVE_RDATA              : 12;
		uint32_t _rsv_12                         : 19;
		uint32_t GLOBAL_CURVE_SW_RD              : 1;
	} bits;
};

union REG_ISP_LTM_RESIZE_COEFF_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t RESIZE_COEFF_BUFF_ADDR          : 7;
		uint32_t _rsv_7                          : 24;
		uint32_t RESIZE_COEFF_BUFF_WEN           : 1;
	} bits;
};

union REG_ISP_LTM_RESIZE_COEFF_WDATA_0 {
	uint32_t raw;
	struct {
		uint32_t RESIZE_COEFF_BUFF_WDATA_0       : 26;
	} bits;
};

union REG_ISP_LTM_RESIZE_COEFF_WDATA_1 {
	uint32_t raw;
	struct {
		uint32_t RESIZE_COEFF_BUFF_WDATA_1       : 26;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t NORM_DEN                        : 16;
		uint32_t CFA_HFLP_STRTH                  : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t CFA_NP_YMIN                     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_NP_YSLOPE                   : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_2 {
	uint32_t raw;
	struct {
		uint32_t CFA_NP_LOW                      : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t CFA_NP_HIGH                     : 10;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_3 {
	uint32_t raw;
	struct {
		uint32_t CFA_DIFFTHD_MIN                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_DIFFTHD_SLOPE               : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_4 {
	uint32_t raw;
	struct {
		uint32_t CFA_DIFFW_LOW                   : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CFA_DIFFW_HIGH                  : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_5 {
	uint32_t raw;
	struct {
		uint32_t CFA_SADTHD_MIN                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_SADTHD_SLOPE                : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_6 {
	uint32_t raw;
	struct {
		uint32_t CFA_SADW_LOW                    : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CFA_SADW_HIGH                   : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_7 {
	uint32_t raw;
	struct {
		uint32_t CFA_LUMATHD_MIN                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_LUMATHD_SLOPE               : 8;
	} bits;
};

union REG_ISP_LTM_CFA_CTRL_8 {
	uint32_t raw;
	struct {
		uint32_t CFA_LUMAW_LOW                   : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CFA_LUMAW_HIGH                  : 8;
	} bits;
};

union REG_ISP_LTM_TILE_MODE_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t DMA_BLK_CROP_STR                : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t DMA_BLK_CROP_END                : 9;
		uint32_t _rsv_25                         : 6;
		uint32_t DMA_BLK_CROP_EN                 : 1;
	} bits;
};

union REG_ISP_LTM_TILE_MODE_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t BLK_WIN_CROP_STR                : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t BLK_WIN_CROP_END                : 9;
		uint32_t _rsv_25                         : 6;
		uint32_t BLK_WIN_CROP_EN                 : 1;
	} bits;
};

union REG_ISP_LTM_TILE_MODE_CTRL_2 {
	uint32_t raw;
	struct {
		uint32_t RS_OUT_CROP_STR                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t RS_OUT_CROP_END                 : 13;
		uint32_t _rsv_29                         : 2;
		uint32_t RS_OUT_CROP_EN                  : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_BNR_SHADOW_RD_SEL {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
	} bits;
};

union REG_ISP_BNR_OUT_SEL {
	uint32_t raw;
	struct {
		uint32_t BNR_OUT_SEL                     : 4;
	} bits;
};

union REG_ISP_BNR_INDEX_CLR {
	uint32_t raw;
	struct {
		uint32_t BNR_INDEX_CLR                   : 1;
	} bits;
};

union REG_ISP_BNR_BAYER_TYPE      {
	uint32_t raw;
	struct {
		uint32_t BNR_BAYER_TYPE                  : 2;
	} bits;
};

union REG_ISP_BNR_NS_LUMA_TH_R    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_LUMA_TH_R                : 10;
	} bits;
};

union REG_ISP_BNR_NS_SLOPE_R      {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_SLOPE_R                  : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET0_R    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET0_R                : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_R    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET1_R                : 10;
	} bits;
};

union REG_ISP_BNR_NS_LUMA_TH_GR   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_LUMA_TH_GR               : 10;
	} bits;
};

union REG_ISP_BNR_NS_SLOPE_GR     {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_SLOPE_GR                 : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET0_GR   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET0_GR               : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_GR   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET1_GR               : 10;
	} bits;
};

union REG_ISP_BNR_NS_LUMA_TH_GB   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_LUMA_TH_GB               : 10;
	} bits;
};

union REG_ISP_BNR_NS_SLOPE_GB     {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_SLOPE_GB                 : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET0_GB   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET0_GB               : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_GB   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET1_GB               : 10;
	} bits;
};

union REG_ISP_BNR_NS_LUMA_TH_B    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_LUMA_TH_B                : 10;
	} bits;
};

union REG_ISP_BNR_NS_SLOPE_B      {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_SLOPE_B                  : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET0_B    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET0_B                : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_B    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_OFFSET1_B                : 10;
	} bits;
};

union REG_ISP_BNR_NS_GAIN         {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_GAIN                     : 8;
	} bits;
};

union REG_ISP_BNR_STRENGTH_MODE   {
	uint32_t raw;
	struct {
		uint32_t BNR_STRENGTH_MODE               : 8;
	} bits;
};

union REG_ISP_BNR_INTENSITY_SEL   {
	uint32_t raw;
	struct {
		uint32_t BNR_INTENSITY_SEL               : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_INTRA_0  {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_INTRA_0              : 3;
	} bits;
};

union REG_ISP_BNR_WEIGHT_INTRA_1  {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_INTRA_1              : 3;
	} bits;
};

union REG_ISP_BNR_WEIGHT_INTRA_2  {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_INTRA_2              : 3;
	} bits;
};

union REG_ISP_BNR_WEIGHT_NORM_1   {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_NORM_1               : 8;
	} bits;
};

union REG_ISP_BNR_WEIGHT_NORM_2   {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_NORM_2               : 7;
	} bits;
};

union REG_ISP_BNR_LSC_RATIO       {
	uint32_t raw;
	struct {
		uint32_t BNR_LSC_RATIO                   : 5;
	} bits;
};

union REG_ISP_BNR_VAR_TH          {
	uint32_t raw;
	struct {
		uint32_t BNR_VAR_TH                      : 8;
	} bits;
};

union REG_ISP_BNR_WEIGHT_LUT      {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_LUT                  : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_SM       {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_SM                   : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_V        {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_V                    : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_H        {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_H                    : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_D45      {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_D45                  : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_D135     {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_D135                 : 5;
	} bits;
};

union REG_ISP_BNR_NEIGHBOR_MAX    {
	uint32_t raw;
	struct {
		uint32_t BNR_NEIGHBOR_MAX                : 1;
	} bits;
};

union REG_ISP_BNR_RES_K_SMOOTH    {
	uint32_t raw;
	struct {
		uint32_t BNR_RES_K_SMOOTH                : 9;
	} bits;
};

union REG_ISP_BNR_RES_K_TEXTURE   {
	uint32_t raw;
	struct {
		uint32_t BNR_RES_K_TEXTURE               : 9;
	} bits;
};

union REG_ISP_BNR_LSC_EN {
	uint32_t raw;
	struct {
		uint32_t BNR_LSC_EN                      : 1;
	} bits;
};

union REG_ISP_BNR_HSTR    {
	uint32_t raw;
	struct {
		uint32_t BNR_HSTR                        : 12;
	} bits;
};

union REG_ISP_BNR_HSIZE           {
	uint32_t raw;
	struct {
		uint32_t BNR_HSIZE                       : 12;
	} bits;
};

union REG_ISP_BNR_VSIZE           {
	uint32_t raw;
	struct {
		uint32_t BNR_VSIZE                       : 12;
	} bits;
};

union REG_ISP_BNR_X_CENTER        {
	uint32_t raw;
	struct {
		uint32_t BNR_X_CENTER                    : 12;
	} bits;
};

union REG_ISP_BNR_Y_CENTER        {
	uint32_t raw;
	struct {
		uint32_t BNR_Y_CENTER                    : 12;
	} bits;
};

union REG_ISP_BNR_NORM_FACTOR     {
	uint32_t raw;
	struct {
		uint32_t BNR_NORM_FACTOR                 : 15;
	} bits;
};

union REG_ISP_BNR_LSC_LUT         {
	uint32_t raw;
	struct {
		uint32_t BNR_LSC_LUT                     : 8;
	} bits;
};

union REG_ISP_BNR_LSC_STRENTH     {
	uint32_t raw;
	struct {
		uint32_t BNR_LSC_STRENTH                 : 12;
	} bits;
};

union REG_ISP_BNR_DUMMY           {
	uint32_t raw;
	struct {
		uint32_t BNR_DUMMY                       : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_MM_00 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_ENABLE                   : 1;
		uint32_t MMAP_1_ENABLE                   : 1;
		uint32_t MMAP_MRG_MODE                   : 1;
		uint32_t MM_DOUBLE_BUF_SEL               : 1;
		uint32_t ON_THE_FLY                      : 1;
		uint32_t FIRST_FRAME_RESET               : 1;
		uint32_t REG_2_TAP_EN                    : 1;
		uint32_t MIRROR_MODE_EN                  : 1;
		uint32_t MMAP_MRG_ALPH                   : 8;
		uint32_t GUARD_CNT                       : 8;
		uint32_t BYPASS                          : 1;
		uint32_t REG_3DNR_EN                     : 1;
		uint32_t YNR_EN                          : 1;
		uint32_t CNR_EN                          : 1;
		uint32_t FUSION_EN                       : 1;
		uint32_t IMG_BAYERID                     : 2;
		uint32_t DMA_BYPASS                      : 1;
	} bits;
};

union REG_ISP_MM_04 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LPF_00                   : 3;
		uint32_t MMAP_0_LPF_01                   : 3;
		uint32_t MMAP_0_LPF_02                   : 3;
		uint32_t MMAP_0_LPF_10                   : 3;
		uint32_t MMAP_0_LPF_11                   : 3;
		uint32_t MMAP_0_LPF_12                   : 3;
		uint32_t MMAP_0_LPF_20                   : 3;
		uint32_t MMAP_0_LPF_21                   : 3;
		uint32_t MMAP_0_LPF_22                   : 3;
	} bits;
};

union REG_ISP_MM_08 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_MAP_CORING               : 8;
		uint32_t MMAP_0_MAP_GAIN                 : 8;
		uint32_t MMAP_0_MAP_THD_L                : 8;
		uint32_t MMAP_0_MAP_THD_H                : 8;
	} bits;
};

union REG_ISP_MM_0C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_1      : 12;
	} bits;
};

union REG_ISP_MM_10 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_2      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_3      : 12;
	} bits;
};

union REG_ISP_MM_14 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_OUT_0     : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_OUT_1     : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_OUT_2     : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_OUT_3     : 6;
	} bits;
};

union REG_ISP_MM_18 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_0   : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_1   : 11;
	} bits;
};

union REG_ISP_MM_1C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_2   : 11;
		uint32_t _rsv_11                         : 1;
		uint32_t MMAP_0_MAP_DSHIFT_BIT           : 3;
	} bits;
};

union REG_ISP_MM_20 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_0       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_1       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_2       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_3       : 8;
	} bits;
};

union REG_ISP_MM_24 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_0      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_1      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_2      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_3      : 4;
	} bits;
};

union REG_ISP_MM_28 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_0    : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_1    : 9;
	} bits;
};

union REG_ISP_MM_2C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_2    : 9;
	} bits;
};

union REG_ISP_MM_30 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1                     : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHTM1                    : 14;
	} bits;
};

union REG_ISP_MM_34 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LPF_00                   : 3;
		uint32_t MMAP_1_LPF_01                   : 3;
		uint32_t MMAP_1_LPF_02                   : 3;
		uint32_t MMAP_1_LPF_10                   : 3;
		uint32_t MMAP_1_LPF_11                   : 3;
		uint32_t MMAP_1_LPF_12                   : 3;
		uint32_t MMAP_1_LPF_20                   : 3;
		uint32_t MMAP_1_LPF_21                   : 3;
		uint32_t MMAP_1_LPF_22                   : 3;
	} bits;
};

union REG_ISP_MM_38 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_MAP_CORING               : 8;
		uint32_t MMAP_1_MAP_GAIN                 : 8;
		uint32_t MMAP_1_MAP_THD_L                : 8;
		uint32_t MMAP_1_MAP_THD_H                : 8;
	} bits;
};

union REG_ISP_MM_3C {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_1      : 12;
	} bits;
};

union REG_ISP_MM_40 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_2      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_3      : 12;
	} bits;
};

union REG_ISP_MM_44 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_OUT_0     : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_OUT_1     : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_OUT_2     : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_OUT_3     : 6;
	} bits;
};

union REG_ISP_MM_48 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_0   : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_1   : 11;
	} bits;
};

union REG_ISP_MM_4C {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_2   : 11;
		uint32_t _rsv_11                         : 1;
		uint32_t MMAP_1_MAP_DSHIFT_BIT           : 3;
	} bits;
};

union REG_ISP_MM_50 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_IIR_PRTCT_LUT_IN_0       : 8;
		uint32_t MMAP_1_IIR_PRTCT_LUT_IN_1       : 8;
		uint32_t MMAP_1_IIR_PRTCT_LUT_IN_2       : 8;
		uint32_t MMAP_1_IIR_PRTCT_LUT_IN_3       : 8;
	} bits;
};

union REG_ISP_MM_54 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_IIR_PRTCT_LUT_OUT_0      : 4;
		uint32_t MMAP_1_IIR_PRTCT_LUT_OUT_1      : 4;
		uint32_t MMAP_1_IIR_PRTCT_LUT_OUT_2      : 4;
		uint32_t MMAP_1_IIR_PRTCT_LUT_OUT_3      : 4;
	} bits;
};

union REG_ISP_MM_58 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_IIR_PRTCT_LUT_SLOPE_0    : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MMAP_1_IIR_PRTCT_LUT_SLOPE_1    : 9;
	} bits;
};

union REG_ISP_MM_5C {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_IIR_PRTCT_LUT_SLOPE_2    : 9;
	} bits;
};

union REG_ISP_MM_60 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_W_BIT                    : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t RGBMAP_H_BIT                    : 3;
	} bits;
};

union REG_ISP_MM_64 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_0                    : 32;
	} bits;
};

union REG_ISP_MM_68 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_1                    : 32;
	} bits;
};

union REG_ISP_MM_6C {
	uint32_t raw;
	struct {
		uint32_t SRAM_WADD                       : 7;
		uint32_t SRAM_WEN                        : 1;
		uint32_t DMA_ENABLE                      : 8;
		uint32_t MANR_DEBUG                      : 16;
	} bits;
};

union REG_ISP_MM_70 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_GAIN_RATIO_R             : 16;
		uint32_t MMAP_0_GAIN_RATIO_G             : 16;
	} bits;
};

union REG_ISP_MM_74 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_GAIN_RATIO_B             : 16;
	} bits;
};

union REG_ISP_MM_78 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_SLOPE_R               : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_0_NS_SLOPE_G               : 10;
	} bits;
};

union REG_ISP_MM_7C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_SLOPE_B               : 10;
	} bits;
};

union REG_ISP_MM_80 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LUMA_TH0_R            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LUMA_TH0_G            : 12;
	} bits;
};

union REG_ISP_MM_84 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LUMA_TH0_B            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LOW_OFFSET_R          : 12;
	} bits;
};

union REG_ISP_MM_88 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LOW_OFFSET_G          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LOW_OFFSET_B          : 12;
	} bits;
};

union REG_ISP_MM_8C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_HIGH_OFFSET_R         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_HIGH_OFFSET_G         : 12;
	} bits;
};

union REG_ISP_MM_90 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_HIGH_OFFSET_B         : 12;
	} bits;
};

union REG_ISP_MM_A0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_GAIN_RATIO_R             : 16;
		uint32_t MMAP_1_GAIN_RATIO_G             : 16;
	} bits;
};

union REG_ISP_MM_A4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_GAIN_RATIO_B             : 16;
	} bits;
};

union REG_ISP_MM_A8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_SLOPE_R               : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_1_NS_SLOPE_G               : 10;
	} bits;
};

union REG_ISP_MM_AC {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_SLOPE_B               : 10;
	} bits;
};

union REG_ISP_MM_B0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LUMA_TH0_R            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LUMA_TH0_G            : 12;
	} bits;
};

union REG_ISP_MM_B4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LUMA_TH0_B            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LOW_OFFSET_R          : 12;
	} bits;
};

union REG_ISP_MM_B8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LOW_OFFSET_G          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LOW_OFFSET_B          : 12;
	} bits;
};

union REG_ISP_MM_BC {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_HIGH_OFFSET_R         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_HIGH_OFFSET_G         : 12;
	} bits;
};

union REG_ISP_MM_C0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_HIGH_OFFSET_B         : 12;
	} bits;
};

union REG_ISP_MM_C4 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_CROP                  : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHT_CROP                 : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE                     : 1;
	} bits;
};

union REG_ISP_MM_C8 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END                      : 14;
	} bits;
};

union REG_ISP_MM_CC {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END                      : 14;
	} bits;
};

union REG_ISP_MM_D0 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_CROP_SCALAR           : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHT_CROP_SCALAR          : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE_SCALAR              : 1;
	} bits;
};

union REG_ISP_MM_D4 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END_SCALAR               : 14;
	} bits;
};

union REG_ISP_MM_D8 {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END_SCALAR               : 14;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_FPN_STATUS {
	uint32_t raw;
	struct {
		uint32_t FPN_STATUS                      : 32;
	} bits;
};

union REG_ISP_FPN_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t FPN_GRACE_RESET                 : 1;
	} bits;
};

union REG_ISP_FPN_MONITOR {
	uint32_t raw;
	struct {
		uint32_t FPN_MONITOR                     : 32;
	} bits;
};

union REG_ISP_FPN_ENABLE {
	uint32_t raw;
	struct {
		uint32_t FPN_ENABLE                      : 1;
	} bits;
};

union REG_ISP_FPN_MAP_ENABLE {
	uint32_t raw;
	struct {
		uint32_t FPN_MAP_ENABLE                  : 1;
	} bits;
};

union REG_ISP_FPN_FLOW {
	uint32_t raw;
	struct {
		uint32_t FPN_ZEROFPNOGRAM                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t FPN_SHADOW_SELECT               : 1;
	} bits;
};

union REG_ISP_FPN_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t FPN_MONITOR_SELECT              : 32;
	} bits;
};

union REG_ISP_FPN_LOCATION {
	uint32_t raw;
	struct {
		uint32_t FPN_LOCATION                    : 32;
	} bits;
};

union REG_ISP_FPN_MEM_SELECT {
	uint32_t raw;
	struct {
		uint32_t FPN_MEM_SELECT                  : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t FPN_MEM_PINGPONG_SELECT         : 1;
		uint32_t _rsv_17                         : 7;
		uint32_t FPN_MEM_ORDER_SELECT            : 1;
	} bits;
};

union REG_ISP_FPN_SECTION_INFO_0 {
	uint32_t raw;
	struct {
		uint32_t FPN_STARTPIXEL0                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_WIDTH0                      : 10;
	} bits;
};

union REG_ISP_FPN_SECTION_INFO_1 {
	uint32_t raw;
	struct {
		uint32_t FPN_STARTPIXEL1                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_WIDTH1                      : 10;
	} bits;
};

union REG_ISP_FPN_SECTION_INFO_2 {
	uint32_t raw;
	struct {
		uint32_t FPN_STARTPIXEL2                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_WIDTH2                      : 10;
	} bits;
};

union REG_ISP_FPN_SECTION_INFO_3 {
	uint32_t raw;
	struct {
		uint32_t FPN_STARTPIXEL3                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_WIDTH3                      : 10;
	} bits;
};

union REG_ISP_FPN_SECTION_INFO_4 {
	uint32_t raw;
	struct {
		uint32_t FPN_STARTPIXEL4                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_WIDTH4                      : 10;
	} bits;
};

union REG_ISP_FPN_DEBUG {
	uint32_t raw;
	struct {
		uint32_t FPN_DEBUG                       : 32;
	} bits;
};

union REG_ISP_FPN_IMG_BAYERID {
	uint32_t raw;
	struct {
		uint32_t IMG_BAYERID                     : 2;
	} bits;
};

union REG_ISP_FPN_DUMMY {
	uint32_t raw;
	struct {
		uint32_t FPN_DUMMY                       : 16;
	} bits;
};

union REG_ISP_FPN_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t FPN_DATA_OFFSEST                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_DATA_GAIN                   : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t FPN_W                           : 1;
	} bits;
};

union REG_ISP_FPN_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t FPN_WSEL                        : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t FPN_RSEL                        : 1;
	} bits;
};

union REG_ISP_FPN_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t FPN_MAX                         : 12;
	} bits;
};

union REG_ISP_FPN_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t FPN_MEM_SW_MODE                 : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t FPN_MEM_SEL                     : 4;
	} bits;
};

union REG_ISP_FPN_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t FPN_SW_RADDR                    : 8;
	} bits;
};

union REG_ISP_FPN_MEM_SW_RDATA_OFFSET {
	uint32_t raw;
	struct {
		uint32_t FPN_RDATA_OFFSET_R              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_RDATA_OFFSET_GR             : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t FPN_SW_R                        : 1;
	} bits;
};

union REG_ISP_FPN_MEM_SW_RDATA_OFFSET_BG {
	uint32_t raw;
	struct {
		uint32_t FPN_RDATA_OFFSET_GB             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_RDATA_OFFSET_B              : 12;
	} bits;
};

union REG_ISP_FPN_MEM_SW_RDATA_GAIN {
	uint32_t raw;
	struct {
		uint32_t FPN_RDATA_GAIN_R                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_RDATA_GAIN_GR               : 12;
	} bits;
};

union REG_ISP_FPN_MEM_SW_RDATA_GAIN_BG {
	uint32_t raw;
	struct {
		uint32_t FPN_RDATA_GAIN_GB               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FPN_RDATA_GAIN_B                : 12;
	} bits;
};

union REG_ISP_FPN_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t RESERVED                        : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t FPN_PROG_1TO4_EN                : 1;
		uint32_t _rsv_5                          : 11;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGB_0 {
	uint32_t raw;
	struct {
		uint32_t COLOR_MODE                      : 2;
	} bits;
};

union REG_ISP_RGB_1 {
	uint32_t raw;
	struct {
		uint32_t RGB_SHDW_SEL                    : 1;
	} bits;
};

union REG_ISP_RGB_2 {
	uint32_t raw;
	struct {
		uint32_t SHDW_DMY                        : 32;
	} bits;
};

union REG_ISP_RGB_3 {
	uint32_t raw;
	struct {
		uint32_t DMY                             : 32;
	} bits;
};

union REG_ISP_RGB_4 {
	uint32_t raw;
	struct {
		uint32_t PROB_OUT_SEL                    : 4;
		uint32_t PROB_PERFMT                     : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PROB_FMT                        : 6;
	} bits;
};

union REG_ISP_RGB_5 {
	uint32_t raw;
	struct {
		uint32_t PROB_LINE                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_PIX                        : 12;
	} bits;
};

union REG_ISP_RGB_6 {
	uint32_t raw;
	struct {
		uint32_t PROB_R                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_G                          : 12;
	} bits;
};

union REG_ISP_RGB_7 {
	uint32_t raw;
	struct {
		uint32_t PROB_B                          : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CFA_0 {
	uint32_t raw;
	struct {
		uint32_t CFA_SHDW_SEL                    : 1;
		uint32_t CFA_ENABLE                      : 1;
		uint32_t CFA_FCR_ENABLE                  : 1;
		uint32_t CFA_MOIRE_ENABLE                : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t BORDER                          : 2;
	} bits;
};

union REG_ISP_CFA_1 {
	uint32_t raw;
	struct {
		uint32_t CFA_OUT_SEL                     : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t _rsv_4                          : 1;
		uint32_t DBG_EN                          : 1;
	} bits;
};

union REG_ISP_CFA_2 {
	uint32_t raw;
	struct {
		uint32_t IMG_WD                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t IMG_HT                          : 12;
	} bits;
};

union REG_ISP_CFA_3 {
	uint32_t raw;
	struct {
		uint32_t CFA_EDGEE_THD                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_SIGE_THD                    : 12;
	} bits;
};

union REG_ISP_CFA_4 {
	uint32_t raw;
	struct {
		uint32_t CFA_GSIG_TOL                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_RBSIG_TOL                   : 12;
	} bits;
};

union REG_ISP_CFA_4_1 {
	uint32_t raw;
	struct {
		uint32_t CFA_EDGE_TOL                    : 12;
	} bits;
};

union REG_ISP_CFA_5 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_THD                     : 16;
	} bits;
};

union REG_ISP_CFA_6 {
	uint32_t raw;
	struct {
		uint32_t CFA_FCR_HV_LOWERBND             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_FCR_HV_UPPERBND             : 12;
	} bits;
};

union REG_ISP_CFA_7 {
	uint32_t raw;
	struct {
		uint32_t CFA_FCR_SAT_LOWERBND            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_FCR_SAT_UPPERBND            : 12;
	} bits;
};

union REG_ISP_CFA_8 {
	uint32_t raw;
	struct {
		uint32_t CFA_FCR_W3_LOWERBND             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_FCR_W3_UPPERBND             : 12;
	} bits;
};

union REG_ISP_CFA_9 {
	uint32_t raw;
	struct {
		uint32_t CFA_FCR_W3MAX_WEIGHT            : 5;
		uint32_t _rsv_5                          : 11;
		uint32_t CFA_FCR_STRENGTH                : 10;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT00                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT01                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT02                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT03                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_1 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT04                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT05                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT06                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT07                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_2 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT08                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT09                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT10                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT11                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_3 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT12                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT13                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT14                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT15                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT16                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT17                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT18                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT19                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_5 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT20                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT21                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT22                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT23                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_6 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT24                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT25                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT26                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT27                   : 6;
	} bits;
};

union REG_ISP_CFA_GHP_LUT_7 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_LUT28                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CFA_GHP_LUT29                   : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CFA_GHP_LUT30                   : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CFA_GHP_LUT31                   : 6;
	} bits;
};

union REG_ISP_CFA_10 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_STRTH                 : 8;
		uint32_t CFA_MOIRE_WGHT_GAIN             : 8;
		uint32_t CFA_MOIRE_NP_YSLOPE             : 8;
	} bits;
};

union REG_ISP_CFA_11 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_NP_YMIN               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_MOIRE_NP_LOW                : 8;
		uint32_t CFA_MOIRE_NP_HIGH               : 8;
	} bits;
};

union REG_ISP_CFA_12 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_DIFFTHD_MIN           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_MOIRE_DIFFTHD_SLOPE         : 8;
	} bits;
};

union REG_ISP_CFA_13 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_DIFFW_LOW             : 8;
		uint32_t CFA_MOIRE_DIFFW_HIGH            : 8;
		uint32_t CFA_MOIRE_SADTHD_MIN            : 12;
	} bits;
};

union REG_ISP_CFA_14 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_SADTHD_SLOPE          : 8;
		uint32_t CFA_MOIRE_SADW_LOW              : 8;
		uint32_t CFA_MOIRE_SADW_HIGH             : 8;
	} bits;
};

union REG_ISP_CFA_15 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_LUMAW_LOW             : 8;
		uint32_t CFA_MOIRE_LUMAW_HIGH            : 8;
	} bits;
};

union REG_ISP_CFA_16 {
	uint32_t raw;
	struct {
		uint32_t CFA_MOIRE_LUMATHD_MIN           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_MOIRE_LUMATHD_SLOPE         : 8;
	} bits;
};

union REG_ISP_CFA_17 {
	uint32_t raw;
	struct {
		uint32_t DIR_READCNT_FROM_LINE0          : 5;
	} bits;
};

union REG_ISP_CFA_18 {
	uint32_t raw;
	struct {
		uint32_t PROB_OUT_SEL                    : 4;
		uint32_t PROB_PERFMT                     : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PROB_FMT                        : 6;
	} bits;
};

union REG_ISP_CFA_19 {
	uint32_t raw;
	struct {
		uint32_t PROB_LINE                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_PIX                        : 12;
	} bits;
};

union REG_ISP_CFA_20 {
	uint32_t raw;
	struct {
		uint32_t CFA_DBG0                        : 32;
	} bits;
};

union REG_ISP_CFA_21 {
	uint32_t raw;
	struct {
		uint32_t CFA_DBG1                        : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CCM_00 {
	uint32_t raw;
	struct {
		uint32_t CCM_00                          : 14;
	} bits;
};

union REG_ISP_CCM_01 {
	uint32_t raw;
	struct {
		uint32_t CCM_01                          : 14;
	} bits;
};

union REG_ISP_CCM_02 {
	uint32_t raw;
	struct {
		uint32_t CCM_02                          : 14;
	} bits;
};

union REG_ISP_CCM_10 {
	uint32_t raw;
	struct {
		uint32_t CCM_10                          : 14;
	} bits;
};

union REG_ISP_CCM_11 {
	uint32_t raw;
	struct {
		uint32_t CCM_11                          : 14;
	} bits;
};

union REG_ISP_CCM_12 {
	uint32_t raw;
	struct {
		uint32_t CCM_12                          : 14;
	} bits;
};

union REG_ISP_CCM_20 {
	uint32_t raw;
	struct {
		uint32_t CCM_20                          : 14;
	} bits;
};

union REG_ISP_CCM_21 {
	uint32_t raw;
	struct {
		uint32_t CCM_21                          : 14;
	} bits;
};

union REG_ISP_CCM_22 {
	uint32_t raw;
	struct {
		uint32_t CCM_22                          : 14;
	} bits;
};

union REG_ISP_CCM_CTRL {
	uint32_t raw;
	struct {
		uint32_t CCM_SHDW_SEL                    : 1;
		uint32_t _rsv_1                          : 1;
		uint32_t CCM_ENABLE                      : 1;
	} bits;
};

union REG_ISP_CCM_DBG {
	uint32_t raw;
	struct {
		uint32_t CONT_EN                         : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_3                          : 13;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_CCM_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_CCM_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_CCM_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_GAMMA_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_DATA_E                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_DATA_O                    : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t GAMMA_W                         : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_WSEL                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_RSEL                      : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MAX                       : 12;
	} bits;
};

union REG_ISP_GAMMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ENABLE                    : 1;
		uint32_t GAMMA_SHDW_SEL                  : 1;
	} bits;
};

union REG_ISP_GAMMA_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MEM_SW_MODE               : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_MEM_SEL                   : 1;
	} bits;
};

union REG_ISP_GAMMA_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_SW_RADDR                  : 8;
	} bits;
};

union REG_ISP_GAMMA_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_RDATA_R                   : 12;
		uint32_t _rsv_12                         : 19;
		uint32_t GAMMA_SW_R                      : 1;
	} bits;
};

union REG_ISP_GAMMA_MEM_SW_RDATA_BG {
	uint32_t raw;
	struct {
		uint32_t GAMMA_RDATA_G                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_RDATA_B                   : 12;
	} bits;
};

union REG_ISP_GAMMA_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t GMA_PROG_1TO3_EN                : 1;
		uint32_t _rsv_5                          : 11;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_GAMMA_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_GAMMA_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_GAMMA_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

union REG_ISP_GAMMA_PROG_DATA_G {
	uint32_t raw;
	struct {
		uint32_t GAMMA_G_DATA_E                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_G_DATA_O                  : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t GAMMA_G_W                       : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_CTRL_G {
	uint32_t raw;
	struct {
		uint32_t GAMMA_G_WSEL                    : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_G_RSEL                    : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_MAX_G {
	uint32_t raw;
	struct {
		uint32_t GAMMA_G_MAX                     : 12;
	} bits;
};

union REG_ISP_GAMMA_PROG_DATA_B {
	uint32_t raw;
	struct {
		uint32_t GAMMA_B_DATA_E                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_B_DATA_O                  : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t GAMMA_B_W                       : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_CTRL_B {
	uint32_t raw;
	struct {
		uint32_t GAMMA_B_WSEL                    : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_B_RSEL                    : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_MAX_B {
	uint32_t raw;
	struct {
		uint32_t GAMMA_B_MAX                     : 12;
	} bits;
};

union REG_ISP_GAMMA_PROG_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ST_ADDR                   : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t GAMMA_ST_W                      : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_HSV_0 {
	uint32_t raw;
	struct {
		uint32_t HSV_ENABLE                      : 1;
		uint32_t HSV_HSGAIN_ENABLE               : 1;
		uint32_t HSV_HTUNE_ENABLE                : 1;
		uint32_t HSV_STUNE_ENABLE                : 1;
		uint32_t HSV_SOFTRST                     : 1;
		uint32_t HSV_SGAIN_LUT_LAST_VAL          : 10;
		uint32_t HSV_H_LUT_LAST_VAL              : 15;
		uint32_t HSV_SHDW_SEL                    : 1;
		uint32_t HSV_HVGAIN_ENABLE               : 1;
	} bits;
};

union REG_ISP_HSV_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t HSV_DEBUG_0                     : 32;
	} bits;
};

union REG_ISP_HSV_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t HSV_DEBUG_1                     : 32;
	} bits;
};

union REG_ISP_HSV_1 {
	uint32_t raw;
	struct {
		uint32_t HSV_S_LUT_VAL_E                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HSV_S_LUT_VAL_O                 : 13;
		uint32_t _rsv_29                         : 2;
		uint32_t HSV_S_LUT_W                     : 1;
	} bits;
};

union REG_ISP_HSV_2 {
	uint32_t raw;
	struct {
		uint32_t HSV_H_LUT_VAL_E                 : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t HSV_H_LUT_VAL_O                 : 15;
		uint32_t HSV_H_LUT_W                     : 1;
	} bits;
};

union REG_ISP_HSV_3 {
	uint32_t raw;
	struct {
		uint32_t HSV_SGAIN_LUT_VAL_E             : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t HSV_SGAIN_LUT_VAL_O             : 10;
		uint32_t _rsv_26                         : 5;
		uint32_t HSV_SGAIN_LUT_W                 : 1;
	} bits;
};

union REG_ISP_HSV_4 {
	uint32_t raw;
	struct {
		uint32_t HSV_S_LUT_LAST_VAL              : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HSV_VGAIN_LUT_LAST_VAL          : 10;
	} bits;
};

union REG_ISP_HSV_5 {
	uint32_t raw;
	struct {
		uint32_t HSV_LUT_W_SEL                   : 1;
		uint32_t HSV_LUT_R_SEL                   : 1;
	} bits;
};

union REG_ISP_HSV_6 {
	uint32_t raw;
	struct {
		uint32_t HSV_VGAIN_LUT_VAL_E             : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t HSV_VGAIN_LUT_VAL_O             : 10;
		uint32_t _rsv_26                         : 5;
		uint32_t HSV_VGAIN_LUT_W                 : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DHZ_DEHAZE_PARA {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_W                        : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TH_SMOOTH                : 10;
	} bits;
};

union REG_ISP_DHZ_BYPASS {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_ENABLE                   : 1;
		uint32_t DEHAZE_SHDW_SEL                 : 1;
	} bits;
};

union REG_ISP_DHZ_0 {
	uint32_t raw;
	struct {
		uint32_t OP_START                        : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t AUTO_UPDATE_EN                  : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_DHZ_1 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_CUM_TH                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t DEHAZE_HIST_TH                  : 14;
	} bits;
};

union REG_ISP_DHZ_2 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SW_DC_TH                 : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DEHAZE_SW_AGLOBAL               : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t DEHAZE_SW_DC_AGLOBAL_TRIG       : 1;
	} bits;
};

union REG_ISP_DHZ_3 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_MIN                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t DEHAZE_TMAP_MAX                 : 13;
	} bits;
};

union REG_ISP_DHZ_4 {
	uint32_t raw;
	struct {
		uint32_t IMG_WD                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t IMG_HT                          : 12;
		uint32_t BORDER                          : 2;
	} bits;
};

union REG_ISP_DHZ_5 {
	uint32_t raw;
	struct {
		uint32_t FMT_ST                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FMT_END                         : 12;
		uint32_t TILE_NM                         : 4;
	} bits;
};

union REG_ISP_DHZ_6 {
	uint32_t raw;
	struct {
		uint32_t DBG_SEL                         : 3;
	} bits;
};

union REG_ISP_DHZ_7 {
	uint32_t raw;
	struct {
		uint32_t DHZ_DBG0                        : 32;
	} bits;
};

union REG_ISP_DHZ_8 {
	uint32_t raw;
	struct {
		uint32_t DHZ_DBG1                        : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CSC_0 {
	uint32_t raw;
	struct {
		uint32_t CSC_ENABLE                      : 1;
		uint32_t R2Y4_SHDW_SEL                   : 1;
	} bits;
};

union REG_ISP_CSC_1 {
	uint32_t raw;
	struct {
		uint32_t OP_START                        : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t R2Y4_BYPASS                     : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t AUTO_UPDATE_EN                  : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_CSC_2 {
	uint32_t raw;
	struct {
		uint32_t SHDW_UPDATE_REQ                 : 1;
	} bits;
};

union REG_ISP_CSC_3 {
	uint32_t raw;
	struct {
		uint32_t DMY0                            : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGBDITHER_RGB_DITHER {
	uint32_t raw;
	struct {
		uint32_t RGB_DITHER_ENABLE               : 1;
		uint32_t RGB_DITHER_MOD_EN               : 1;
		uint32_t RGB_DITHER_HISTIDX_EN           : 1;
		uint32_t RGB_DITHER_FMNUM_EN             : 1;
		uint32_t RGB_DITHER_SHDW_SEL             : 1;
		uint32_t RGB_DITHER_SOFTRST              : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t CROP_WIDTHM1                    : 12;
		uint32_t CROP_HEIGHTM1                   : 12;
	} bits;
};

union REG_ISP_RGBDITHER_RGB_DITHER_DEBUG0 {
	uint32_t raw;
	struct {
		uint32_t RGB_DITHER_DEBUG0               : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGBEE_0 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_SHDW_SEL                  : 1;
		uint32_t RGBEE_ENABLE                    : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t BORDER                          : 2;
	} bits;
};

union REG_ISP_RGBEE_1 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t _rsv_4                          : 1;
		uint32_t DBG_EN                          : 1;
	} bits;
};

union REG_ISP_RGBEE_2 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_OSGAIN                    : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t RGBEE_USGAIN                    : 10;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT00                  : 8;
		uint32_t RGBEE_AC_LUT01                  : 8;
		uint32_t RGBEE_AC_LUT02                  : 8;
		uint32_t RGBEE_AC_LUT03                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_1 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT04                  : 8;
		uint32_t RGBEE_AC_LUT05                  : 8;
		uint32_t RGBEE_AC_LUT06                  : 8;
		uint32_t RGBEE_AC_LUT07                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_2 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT08                  : 8;
		uint32_t RGBEE_AC_LUT09                  : 8;
		uint32_t RGBEE_AC_LUT10                  : 8;
		uint32_t RGBEE_AC_LUT11                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_3 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT12                  : 8;
		uint32_t RGBEE_AC_LUT13                  : 8;
		uint32_t RGBEE_AC_LUT14                  : 8;
		uint32_t RGBEE_AC_LUT15                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT16                  : 8;
		uint32_t RGBEE_AC_LUT17                  : 8;
		uint32_t RGBEE_AC_LUT18                  : 8;
		uint32_t RGBEE_AC_LUT19                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_5 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT20                  : 8;
		uint32_t RGBEE_AC_LUT21                  : 8;
		uint32_t RGBEE_AC_LUT22                  : 8;
		uint32_t RGBEE_AC_LUT23                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_6 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT24                  : 8;
		uint32_t RGBEE_AC_LUT25                  : 8;
		uint32_t RGBEE_AC_LUT26                  : 8;
		uint32_t RGBEE_AC_LUT27                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_7 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT28                  : 8;
		uint32_t RGBEE_AC_LUT29                  : 8;
		uint32_t RGBEE_AC_LUT30                  : 8;
		uint32_t RGBEE_AC_LUT31                  : 8;
	} bits;
};

union REG_ISP_RGBEE_AC_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_AC_LUT32                  : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT00                : 8;
		uint32_t RGBEE_EDGE_LUT01                : 8;
		uint32_t RGBEE_EDGE_LUT02                : 8;
		uint32_t RGBEE_EDGE_LUT03                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_1 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT04                : 8;
		uint32_t RGBEE_EDGE_LUT05                : 8;
		uint32_t RGBEE_EDGE_LUT06                : 8;
		uint32_t RGBEE_EDGE_LUT07                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_2 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT08                : 8;
		uint32_t RGBEE_EDGE_LUT09                : 8;
		uint32_t RGBEE_EDGE_LUT10                : 8;
		uint32_t RGBEE_EDGE_LUT11                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_3 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT12                : 8;
		uint32_t RGBEE_EDGE_LUT13                : 8;
		uint32_t RGBEE_EDGE_LUT14                : 8;
		uint32_t RGBEE_EDGE_LUT15                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT16                : 8;
		uint32_t RGBEE_EDGE_LUT17                : 8;
		uint32_t RGBEE_EDGE_LUT18                : 8;
		uint32_t RGBEE_EDGE_LUT19                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_5 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT20                : 8;
		uint32_t RGBEE_EDGE_LUT21                : 8;
		uint32_t RGBEE_EDGE_LUT22                : 8;
		uint32_t RGBEE_EDGE_LUT23                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_6 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT24                : 8;
		uint32_t RGBEE_EDGE_LUT25                : 8;
		uint32_t RGBEE_EDGE_LUT26                : 8;
		uint32_t RGBEE_EDGE_LUT27                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_7 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT28                : 8;
		uint32_t RGBEE_EDGE_LUT29                : 8;
		uint32_t RGBEE_EDGE_LUT30                : 8;
		uint32_t RGBEE_EDGE_LUT31                : 8;
	} bits;
};

union REG_ISP_RGBEE_EDGE_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_EDGE_LUT32                : 8;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT00                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT01                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_1 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT02                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT03                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_2 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT04                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT05                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_3 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT06                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT07                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT08                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT09                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_5 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT10                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT11                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_6 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT12                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT13                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_7 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT14                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT15                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT16                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT17                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_9 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT18                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT19                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_10 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT20                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT21                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_11 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT22                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT23                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT24                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT25                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_13 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT26                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT27                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_14 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT28                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT29                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_15 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT30                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBEE_NP_LUT31                  : 12;
	} bits;
};

union REG_ISP_RGBEE_NP_LUT_16 {
	uint32_t raw;
	struct {
		uint32_t RGBEE_NP_LUT32                  : 12;
	} bits;
};

union REG_ISP_RGBEE_4 {
	uint32_t raw;
	struct {
		uint32_t IMG_WD                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t IMG_HT                          : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_YUV_TOP_YUV_0 {
	uint32_t raw;
	struct {
		uint32_t OP_START                        : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t YUV_TOP_SEL                     : 1;
	} bits;
};

union REG_YUV_TOP_YUV_1 {
	uint32_t raw;
	struct {
		uint32_t CG_444_422_EN                   : 1;
		uint32_t CS_EN                           : 1;
	} bits;
};

union REG_YUV_TOP_YUV_2 {
	uint32_t raw;
	struct {
		uint32_t FD_INT                          : 1;
		uint32_t DMA_INT                         : 1;
		uint32_t FRAME_OVERFLOW                  : 1;
	} bits;
};

union REG_YUV_TOP_YUV_3 {
	uint32_t raw;
	struct {
		uint32_t DISABLE_DO                      : 1;
		uint32_t DISABLE_DMAO                    : 1;
		uint32_t Y42_SEL                         : 1;
	} bits;
};

union REG_YUV_TOP_YUV_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t DEBUG_BUS                       : 32;
	} bits;
};

union REG_YUV_TOP_YUV_4 {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};

union REG_YUV_TOP_YUV_DEBUG_STATE {
	uint32_t raw;
	struct {
		uint32_t _IP_DMA_IDLE                    : 1;
		uint32_t MA_IDLE_YCROP                   : 1;
		uint32_t MA_IDLE_UCROP                   : 1;
		uint32_t MA_IDLE_VCROP                   : 1;
		uint32_t IP_DMA_IDLE                     : 1;
		uint32_t OR_IP_DMA_IDLE                  : 1;
	} bits;
};

union REG_YUV_TOP_YUV_5 {
	uint32_t raw;
	struct {
		uint32_t DIS_UV2DRAM                     : 1;
		uint32_t LINE_THRES_EN                   : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t LINE_THRES                      : 12;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_PROG_SRAM0 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_LSB                  : 32;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_PROG_SRAM1 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_MSB                  : 2;
		uint32_t SRAM_H_IDX                      : 5;
		uint32_t SRAM_S_IDX                      : 4;
		uint32_t SRAM_V_IDX                      : 4;
		uint32_t SRAM_WR                         : 1;
		uint32_t SRAM_RD                         : 1;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_READ_SRAM0 {
	uint32_t raw;
	struct {
		uint32_t SRAM_RDATA_LSB                  : 32;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_READ_SRAM1 {
	uint32_t raw;
	struct {
		uint32_t SRAM_RDATA_MSB                  : 2;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_CONTROL {
	uint32_t raw;
	struct {
		uint32_t HSV3DLUT_ENABLE                 : 1;
		uint32_t HSV3DLUT_H_CLAMP_WRAP_OPT       : 1;
	} bits;
};

union REG_YUV_TOP_HSV_LUT_STATUS {
	uint32_t raw;
	struct {
		uint32_t CLR_STATUS_HSV_LUT_WR_ERR       : 1;
		uint32_t STATUS_HSV_LUT_WR_ERR           : 1;
	} bits;
};

union REG_YUV_TOP_HSV_ENABLE {
	uint32_t raw;
	struct {
		uint32_t HSV_ENABLE                      : 1;
		uint32_t SC_DMA_SWITCH                   : 1;
		uint32_t AVG_MODE                        : 1;
		uint32_t BYPASS_H                        : 1;
		uint32_t BYPASS_V                        : 1;
		uint32_t DROP_MODE                       : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t GUARD_CNT                       : 8;
		uint32_t DITH_EN                         : 1;
		uint32_t DITH_MD                         : 3;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_444_422_0 {
	uint32_t raw;
	struct {
		uint32_t OP_START                        : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t BYPASS_EN                       : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t AUTO_UPDATE_EN                  : 1;
		uint32_t DBG_EN                          : 1;
	} bits;
};

union REG_ISP_444_422_1 {
	uint32_t raw;
	struct {
		uint32_t SHDW_UPDATE_REQ                 : 1;
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_ISP_444_422_2 {
	uint32_t raw;
	struct {
		uint32_t FD_INT                          : 1;
		uint32_t _rsv_1                          : 1;
		uint32_t FRAME_OVERFLOW                  : 1;
	} bits;
};

union REG_ISP_444_422_3 {
	uint32_t raw;
	struct {
		uint32_t CHECKSUM                        : 32;
	} bits;
};

union REG_ISP_444_422_4 {
	uint32_t raw;
	struct {
		uint32_t REG_422_444                     : 1;
		uint32_t SWAP                            : 2;
	} bits;
};

union REG_ISP_444_422_5 {
	uint32_t raw;
	struct {
		uint32_t FIRST_FRAME_RESET               : 1;
		uint32_t TDNR_DISABLE                    : 1;
		uint32_t DMA_BYPASS                      : 1;
		uint32_t FORCE_MONO_ENABLE               : 1;
	} bits;
};

union REG_ISP_444_422_6 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_TRANS                 : 14;
	} bits;
};

union REG_ISP_444_422_7 {
	uint32_t raw;
	struct {
		uint32_t IMG_HEIGHT_TRANS                : 14;
	} bits;
};

union REG_ISP_444_422_8 {
	uint32_t raw;
	struct {
		uint32_t GUARD_CNT                       : 8;
		uint32_t DMA_ENABLE                      : 6;
		uint32_t UV_ROUNDING_TYPE_SEL            : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t TDNR_DEBUG_SEL                  : 16;
	} bits;
};

union REG_ISP_444_422_9 {
	uint32_t raw;
	struct {
		uint32_t DMA_WRITE_SEL_Y                 : 1;
		uint32_t DMA_WRITE_SEL_C                 : 1;
		uint32_t SENSOR_422_IN                   : 1;
	} bits;
};

union REG_ISP_444_422_10 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_CROP                  : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHT_CROP                 : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE                     : 1;
	} bits;
};

union REG_ISP_444_422_11 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END                      : 14;
	} bits;
};

union REG_ISP_444_422_12 {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END                      : 14;
	} bits;
};

union REG_ISP_444_422_13 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUT_IN_0               : 8;
		uint32_t REG_3DNR_LUT_IN_1               : 8;
		uint32_t REG_3DNR_LUT_IN_2               : 8;
		uint32_t REG_3DNR_LUT_IN_3               : 8;
	} bits;
};

union REG_ISP_444_422_14 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUT_OUT_0              : 8;
		uint32_t REG_3DNR_LUT_OUT_1              : 8;
		uint32_t REG_3DNR_LUT_OUT_2              : 8;
		uint32_t REG_3DNR_LUT_OUT_3              : 8;
	} bits;
};

union REG_ISP_444_422_15 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUT_SLOPE_0            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_LUT_SLOPE_1            : 12;
	} bits;
};

union REG_ISP_444_422_16 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUT_SLOPE_2            : 12;
		uint32_t _rsv_12                         : 3;
		uint32_t MOTION_SEL                      : 1;
		uint32_t REG_3DNR_BETA_MAX               : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_YUV_DITHER_Y_DITHER {
	uint32_t raw;
	struct {
		uint32_t Y_DITHER_EN                     : 1;
		uint32_t Y_DITHER_MOD_EN                 : 1;
		uint32_t Y_DITHER_HISTIDX_EN             : 1;
		uint32_t Y_DITHER_FMNUM_EN               : 1;
		uint32_t Y_DITHER_SHDW_SEL               : 1;
		uint32_t Y_DITHER_SOFTRST                : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t Y_DITHER_HEIGHTM1               : 12;
		uint32_t Y_DITHER_WIDTHM1                : 12;
	} bits;
};

union REG_ISP_YUV_DITHER_UV_DITHER {
	uint32_t raw;
	struct {
		uint32_t UV_DITHER_EN                    : 1;
		uint32_t UV_DITHER_MOD_EN                : 1;
		uint32_t UV_DITHER_HISTIDX_EN            : 1;
		uint32_t UV_DITHER_FMNUM_EN              : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t UV_DITHER_HEIGHTM1              : 12;
		uint32_t UV_DITHER_WIDTHM1               : 12;
	} bits;
};

union REG_ISP_YUV_DITHER_DEBUG_00 {
	uint32_t raw;
	struct {
		uint32_t UV_DITHER_DEBUG0                : 32;
	} bits;
};

union REG_ISP_YUV_DITHER_DEBUG_01 {
	uint32_t raw;
	struct {
		uint32_t Y_DITHER_DEBUG0                 : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_YNR_SHADOW_RD_SEL   {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
	} bits;
};

union REG_ISP_YNR_OUT_SEL         {
	uint32_t raw;
	struct {
		uint32_t YNR_OUT_SEL                     : 4;
	} bits;
};

union REG_ISP_YNR_INDEX_CLR {
	uint32_t raw;
	struct {
		uint32_t YNR_INDEX_CLR                   : 1;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH     {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH                 : 8;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE       {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE                   : 11;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET      {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET                  : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH     {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH                 : 8;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE       {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE                   : 11;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET      {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET                  : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_NS_TH    {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_NS_TH                : 4;
	} bits;
};

union REG_ISP_YNR_MOTION_POS_GAIN {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_POS_GAIN             : 7;
	} bits;
};

union REG_ISP_YNR_MOTION_NEG_GAIN {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_NEG_GAIN             : 7;
	} bits;
};

union REG_ISP_YNR_NS_GAIN         {
	uint32_t raw;
	struct {
		uint32_t YNR_NS_GAIN                     : 9;
	} bits;
};

union REG_ISP_YNR_STRENGTH_MODE   {
	uint32_t raw;
	struct {
		uint32_t YNR_STRENGTH_MODE               : 8;
	} bits;
};

union REG_ISP_YNR_INTENSITY_SEL   {
	uint32_t raw;
	struct {
		uint32_t YNR_INTENSITY_SEL               : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_INTRA_0  {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_INTRA_0              : 3;
	} bits;
};

union REG_ISP_YNR_WEIGHT_INTRA_1  {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_INTRA_1              : 3;
	} bits;
};

union REG_ISP_YNR_WEIGHT_INTRA_2  {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_INTRA_2              : 3;
	} bits;
};

union REG_ISP_YNR_WEIGHT_NORM_1   {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_NORM_1               : 7;
	} bits;
};

union REG_ISP_YNR_WEIGHT_NORM_2   {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_NORM_2               : 6;
	} bits;
};

union REG_ISP_YNR_ALPHA_GAIN      {
	uint32_t raw;
	struct {
		uint32_t YNR_ALPHA_GAIN                  : 10;
	} bits;
};

union REG_ISP_YNR_VAR_TH          {
	uint32_t raw;
	struct {
		uint32_t YNR_VAR_TH                      : 8;
	} bits;
};

union REG_ISP_YNR_WEIGHT_LUT      {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_LUT                  : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_SM       {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_SM                   : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_V        {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_V                    : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_H        {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_H                    : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_D45      {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_D45                  : 5;
	} bits;
};

union REG_ISP_YNR_WEIGHT_D135     {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_D135                 : 5;
	} bits;
};

union REG_ISP_YNR_NEIGHBOR_MAX    {
	uint32_t raw;
	struct {
		uint32_t YNR_NEIGHBOR_MAX                : 1;
	} bits;
};

union REG_ISP_YNR_RES_K_SMOOTH    {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_K_SMOOTH                : 9;
	} bits;
};

union REG_ISP_YNR_RES_K_TEXTURE   {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_K_TEXTURE               : 9;
	} bits;
};

union REG_ISP_YNR_DUMMY           {
	uint32_t raw;
	struct {
		uint32_t YNR_DUMMY                       : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CNR_00 {
	uint32_t raw;
	struct {
		uint32_t CNR_ENABLE                      : 1;
		uint32_t PFC_ENABLE                      : 1;
		uint32_t CNR_SWIN_ROWS                   : 6;
		uint32_t CNR_SWIN_COLS                   : 6;
		uint32_t CNR_DIFF_SHIFT_VAL              : 8;
		uint32_t _rsv_22                         : 2;
		uint32_t CNR_RATIO                       : 8;
	} bits;
};

union REG_ISP_CNR_01 {
	uint32_t raw;
	struct {
		uint32_t CNR_STRENGTH_MODE               : 8;
		uint32_t CNR_FUSION_INTENSITY_WEIGHT     : 4;
		uint32_t _rsv_12                         : 1;
		uint32_t CNR_WEIGHT_INTER_SEL            : 4;
		uint32_t CNR_VAR_TH                      : 9;
		uint32_t CNR_FLAG_NEIGHBOR_MAX_WEIGHT    : 1;
		uint32_t CNR_SHDW_SEL                    : 1;
		uint32_t CNR_OUT_SEL                     : 2;
	} bits;
};

union REG_ISP_CNR_02 {
	uint32_t raw;
	struct {
		uint32_t CNR_PURPLE_TH                   : 8;
		uint32_t CNR_CORRECT_STRENGTH            : 8;
		uint32_t CNR_DIFF_GAIN                   : 4;
		uint32_t UV_ROUNDING_ENABLE              : 1;
	} bits;
};

union REG_ISP_CNR_03 {
	uint32_t raw;
	struct {
		uint32_t CNR_PURPLE_CR                   : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CNR_GREEN_CB                    : 8;
	} bits;
};

union REG_ISP_CNR_04 {
	uint32_t raw;
	struct {
		uint32_t CNR_GREEN_CR                    : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CNR_PURPLE_CB                   : 8;
	} bits;
};

union REG_ISP_CNR_05 {
	uint32_t raw;
	struct {
		uint32_t WEIGHT_LUT_INTER_CNR_00         : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_01         : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_02         : 5;
		uint32_t _rsv_21                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_03         : 5;
	} bits;
};

union REG_ISP_CNR_06 {
	uint32_t raw;
	struct {
		uint32_t WEIGHT_LUT_INTER_CNR_04         : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_05         : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_06         : 5;
		uint32_t _rsv_21                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_07         : 5;
	} bits;
};

union REG_ISP_CNR_07 {
	uint32_t raw;
	struct {
		uint32_t WEIGHT_LUT_INTER_CNR_08         : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_09         : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_10         : 5;
		uint32_t _rsv_21                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_11         : 5;
	} bits;
};

union REG_ISP_CNR_08 {
	uint32_t raw;
	struct {
		uint32_t WEIGHT_LUT_INTER_CNR_12         : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_13         : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_14         : 5;
		uint32_t _rsv_21                         : 3;
		uint32_t WEIGHT_LUT_INTER_CNR_15         : 5;
	} bits;
};

union REG_ISP_CNR_09 {
	uint32_t raw;
	struct {
		uint32_t CNR_INTENSITY_SEL_0             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CNR_INTENSITY_SEL_1             : 9;
	} bits;
};

union REG_ISP_CNR_10 {
	uint32_t raw;
	struct {
		uint32_t CNR_INTENSITY_SEL_2             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CNR_INTENSITY_SEL_3             : 9;
	} bits;
};

union REG_ISP_CNR_11 {
	uint32_t raw;
	struct {
		uint32_t CNR_INTENSITY_SEL_4             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CNR_INTENSITY_SEL_5             : 9;
	} bits;
};

union REG_ISP_CNR_12 {
	uint32_t raw;
	struct {
		uint32_t CNR_INTENSITY_SEL_6             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CNR_INTENSITY_SEL_7             : 9;
	} bits;
};

union REG_ISP_CNR_13 {
	uint32_t raw;
	struct {
		uint32_t CNR_IMG_WIDTHM1                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CNR_IMG_HEIGHTM1                : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_EE_00 {
	uint32_t raw;
	struct {
		uint32_t EE_ENABLE                       : 1;
		uint32_t EE_BYPASS                       : 1;
		uint32_t MIRROR_MODE_EN                  : 1;
		uint32_t EE_DOUBLE_BUF_SEL               : 1;
		uint32_t EE_DEBUG_MODE                   : 4;
		uint32_t EE_TOTAL_CORING                 : 10;
		uint32_t _rsv_18                         : 6;
		uint32_t EE_TOTAL_GAIN                   : 8;
	} bits;
};

union REG_ISP_EE_04 {
	uint32_t raw;
	struct {
		uint32_t EE_TOTAL_OSHTTHRD               : 8;
		uint32_t EE_TOTAL_USHTTHRD               : 8;
		uint32_t EE_DEBUG_SHIFT_BIT              : 3;
	} bits;
};

union REG_ISP_EE_08 {
	uint32_t raw;
	struct {
		uint32_t EE_STD_SHTCTRL_EN               : 1;
		uint32_t EE_STD_SHTCTRL_MAX3X3_EN        : 1;
		uint32_t EE_STD_SHTCTRL_MIN3X3_EN        : 1;
		uint32_t _rsv_3                          : 5;
		uint32_t EE_STD_SHTCTRL_MIN_GAIN         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_STD_SHTCTRL_DIFF_GAIN        : 6;
		uint32_t _rsv_22                         : 1;
		uint32_t EE_STD_SHTCTRL_DIFF_THRD        : 9;
	} bits;
};

union REG_ISP_EE_0C {
	uint32_t raw;
	struct {
		uint32_t EE_LUMAREF_LPF_EN               : 1;
		uint32_t EE_LUMA_CORING_EN               : 1;
		uint32_t EE_LUMA_SHTCTRL_EN              : 1;
		uint32_t EE_DELTA_SHTCTRL_EN             : 1;
		uint32_t EE_DELTA_SHTCTRL_SHIFT          : 2;
		uint32_t EE_LUMA_ADPTCTRL_EN             : 1;
		uint32_t EE_DELTA_ADPTCTRL_EN            : 1;
		uint32_t EE_DELTA_ADPTCTRL_SHIFT         : 2;
	} bits;
};

union REG_ISP_EE_10 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRDET_LPF_BLDWGT            : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DIRDET_MIN_GAIN              : 6;
	} bits;
};

union REG_ISP_EE_14 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRDET_DIRGAIN               : 8;
		uint32_t EE_DIRDET_DIRTHRD               : 8;
		uint32_t EE_DIRDET_SUBGAIN               : 8;
		uint32_t EE_DIRDET_SUBTHRD               : 8;
	} bits;
};

union REG_ISP_EE_18 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRDET_SADNORM_TGT           : 8;
		uint32_t EE_DIRDET_DGRADPT_TRANS_SLOP    : 4;
		uint32_t EE_DIRDET_MIN_GAIN_ADPT_EN      : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t EE_DIRDET_MIDLEDIR_GAIN         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DIRDET_ALIGNDIR_GAIN         : 6;
	} bits;
};

union REG_ISP_EE_1C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRSUB_WGT_CORING_THRD       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_DIRDET_WGT_CORING_THRD       : 12;
	} bits;
};

union REG_ISP_EE_20 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_000_NORGAIN      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t EE_DIRCAL_DGR4_045_NORGAIN      : 13;
	} bits;
};

union REG_ISP_EE_24 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_090_NORGAIN      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t EE_DIRCAL_DGR4_135_NORGAIN      : 13;
	} bits;
};

union REG_ISP_EE_28 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_NOD_NORGAIN      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t EE_DIRCAL_DGR4_DIR_ADJGAIN      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DIRCAL_DGR4_NOD_CORINGTH     : 8;
	} bits;
};

union REG_ISP_EE_2C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_000_CORINGTH     : 8;
		uint32_t EE_DIRCAL_DGR4_045_CORINGTH     : 8;
		uint32_t EE_DIRCAL_DGR4_090_CORINGTH     : 8;
		uint32_t EE_DIRCAL_DGR4_135_CORINGTH     : 8;
	} bits;
};

union REG_ISP_EE_30 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER000_W1     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER000_W2     : 9;
	} bits;
};

union REG_ISP_EE_34 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER000_W3     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER000_W4     : 9;
	} bits;
};

union REG_ISP_EE_38 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER000_W5     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER000_W6     : 9;
	} bits;
};

union REG_ISP_EE_3C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER000_W7     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER000_W8     : 9;
	} bits;
};

union REG_ISP_EE_40 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER000_W9     : 9;
	} bits;
};

union REG_ISP_EE_58 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER045_W1     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER045_W2     : 9;
	} bits;
};

union REG_ISP_EE_5C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER045_W3     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER045_W4     : 9;
	} bits;
};

union REG_ISP_EE_60 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER045_W5     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER045_W6     : 9;
	} bits;
};

union REG_ISP_EE_64 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER045_W7     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER045_W8     : 9;
	} bits;
};

union REG_ISP_EE_68 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER045_W9     : 9;
	} bits;
};

union REG_ISP_EE_6C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER090_W1     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER090_W2     : 9;
	} bits;
};

union REG_ISP_EE_70 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER090_W3     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER090_W4     : 9;
	} bits;
};

union REG_ISP_EE_74 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER090_W5     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER090_W6     : 9;
	} bits;
};

union REG_ISP_EE_78 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER090_W7     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER090_W8     : 9;
	} bits;
};

union REG_ISP_EE_7C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER090_W9     : 9;
	} bits;
};

union REG_ISP_EE_80 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER135_W1     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER135_W2     : 9;
	} bits;
};

union REG_ISP_EE_84 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER135_W3     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER135_W4     : 9;
	} bits;
};

union REG_ISP_EE_88 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER135_W5     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER135_W6     : 9;
	} bits;
};

union REG_ISP_EE_8C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER135_W7     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTER135_W8     : 9;
	} bits;
};

union REG_ISP_EE_90 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTER135_W9     : 9;
	} bits;
};

union REG_ISP_EE_94 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W1     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W2     : 9;
	} bits;
};

union REG_ISP_EE_98 {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W3     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W4     : 9;
	} bits;
};

union REG_ISP_EE_9C {
	uint32_t raw;
	struct {
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W5     : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t EE_DIRCAL_DGR4_FILTERNOD_W6     : 9;
	} bits;
};

union REG_ISP_EE_A4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_00           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_01           : 10;
	} bits;
};

union REG_ISP_EE_A8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_02           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_03           : 10;
	} bits;
};

union REG_ISP_EE_AC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_04           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_05           : 10;
	} bits;
};

union REG_ISP_EE_B0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_06           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_07           : 10;
	} bits;
};

union REG_ISP_EE_B4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_08           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_09           : 10;
	} bits;
};

union REG_ISP_EE_B8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_10           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_11           : 10;
	} bits;
};

union REG_ISP_EE_BC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_12           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_13           : 10;
	} bits;
};

union REG_ISP_EE_C0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_14           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_15           : 10;
	} bits;
};

union REG_ISP_EE_C4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_16           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_17           : 10;
	} bits;
};

union REG_ISP_EE_C8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_18           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_19           : 10;
	} bits;
};

union REG_ISP_EE_CC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_20           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_21           : 10;
	} bits;
};

union REG_ISP_EE_D0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_22           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_23           : 10;
	} bits;
};

union REG_ISP_EE_D4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_24           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_25           : 10;
	} bits;
};

union REG_ISP_EE_D8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_26           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_27           : 10;
	} bits;
};

union REG_ISP_EE_DC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_28           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_29           : 10;
	} bits;
};

union REG_ISP_EE_E0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_30           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_LUMA_CORING_LUT_31           : 10;
	} bits;
};

union REG_ISP_EE_E4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_32           : 10;
	} bits;
};

union REG_ISP_EE_E8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_00          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_01          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_02          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_03          : 6;
	} bits;
};

union REG_ISP_EE_EC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_04          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_05          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_06          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_07          : 6;
	} bits;
};

union REG_ISP_EE_F0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_08          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_09          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_10          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_11          : 6;
	} bits;
};

union REG_ISP_EE_F4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_12          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_13          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_14          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_15          : 6;
	} bits;
};

union REG_ISP_EE_F8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_16          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_17          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_18          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_19          : 6;
	} bits;
};

union REG_ISP_EE_FC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_20          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_21          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_22          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_23          : 6;
	} bits;
};

union REG_ISP_EE_100 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_24          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_25          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_26          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_27          : 6;
	} bits;
};

union REG_ISP_EE_104 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_28          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_29          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_30          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_SHTCTRL_LUT_31          : 6;
	} bits;
};

union REG_ISP_EE_108 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_SHTCTRL_LUT_32          : 6;
	} bits;
};

union REG_ISP_EE_10C {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_00         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_01         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_02         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_03         : 6;
	} bits;
};

union REG_ISP_EE_110 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_04         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_05         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_06         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_07         : 6;
	} bits;
};

union REG_ISP_EE_114 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_08         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_09         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_10         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_11         : 6;
	} bits;
};

union REG_ISP_EE_118 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_12         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_13         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_14         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_15         : 6;
	} bits;
};

union REG_ISP_EE_11C {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_16         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_17         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_18         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_19         : 6;
	} bits;
};

union REG_ISP_EE_120 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_20         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_21         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_22         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_23         : 6;
	} bits;
};

union REG_ISP_EE_124 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_24         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_25         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_26         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_27         : 6;
	} bits;
};

union REG_ISP_EE_128 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_28         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_29         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_30         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_SHTCTRL_LUT_31         : 6;
	} bits;
};

union REG_ISP_EE_12C {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_SHTCTRL_LUT_32         : 6;
	} bits;
};

union REG_ISP_EE_130 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_00         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_01         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_02         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_03         : 6;
	} bits;
};

union REG_ISP_EE_134 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_04         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_05         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_06         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_07         : 6;
	} bits;
};

union REG_ISP_EE_138 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_08         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_09         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_10         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_11         : 6;
	} bits;
};

union REG_ISP_EE_13C {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_12         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_13         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_14         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_15         : 6;
	} bits;
};

union REG_ISP_EE_140 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_16         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_17         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_18         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_19         : 6;
	} bits;
};

union REG_ISP_EE_144 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_20         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_21         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_22         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_23         : 6;
	} bits;
};

union REG_ISP_EE_148 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_24         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_25         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_26         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_27         : 6;
	} bits;
};

union REG_ISP_EE_14C {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_28         : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_29         : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_30         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_LUMA_ADPTCTRL_LUT_31         : 6;
	} bits;
};

union REG_ISP_EE_150 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_32         : 6;
	} bits;
};

union REG_ISP_EE_154 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_00        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_01        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_02        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_03        : 6;
	} bits;
};

union REG_ISP_EE_158 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_04        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_05        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_06        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_07        : 6;
	} bits;
};

union REG_ISP_EE_15C {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_08        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_09        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_10        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_11        : 6;
	} bits;
};

union REG_ISP_EE_160 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_12        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_13        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_14        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_15        : 6;
	} bits;
};

union REG_ISP_EE_164 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_16        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_17        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_18        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_19        : 6;
	} bits;
};

union REG_ISP_EE_168 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_20        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_21        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_22        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_23        : 6;
	} bits;
};

union REG_ISP_EE_16C {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_24        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_25        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_26        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_27        : 6;
	} bits;
};

union REG_ISP_EE_170 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_28        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_29        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_30        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_DELTA_ADPTCTRL_LUT_31        : 6;
	} bits;
};

union REG_ISP_EE_174 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_32        : 6;
	} bits;
};

union REG_ISP_EE_178 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_00                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_01                 : 12;
	} bits;
};

union REG_ISP_EE_17C {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_02                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_03                 : 12;
	} bits;
};

union REG_ISP_EE_180 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_04                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_05                 : 12;
	} bits;
};

union REG_ISP_EE_184 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_06                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_07                 : 12;
	} bits;
};

union REG_ISP_EE_188 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_08                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_09                 : 12;
	} bits;
};

union REG_ISP_EE_18C {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_10                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_11                 : 12;
	} bits;
};

union REG_ISP_EE_190 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_12                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_13                 : 12;
	} bits;
};

union REG_ISP_EE_194 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_14                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_15                 : 12;
	} bits;
};

union REG_ISP_EE_198 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_16                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_17                 : 12;
	} bits;
};

union REG_ISP_EE_19C {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_18                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_19                 : 12;
	} bits;
};

union REG_ISP_EE_1A0 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_20                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_21                 : 12;
	} bits;
};

union REG_ISP_EE_1A4 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_22                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_23                 : 12;
	} bits;
};

union REG_ISP_EE_1A8 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_24                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_25                 : 12;
	} bits;
};

union REG_ISP_EE_1AC {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_26                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_27                 : 12;
	} bits;
};

union REG_ISP_EE_1B0 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_28                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_29                 : 12;
	} bits;
};

union REG_ISP_EE_1B4 {
	uint32_t raw;
	struct {
		uint32_t SAD_NORM_MUL_30                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SAD_NORM_MUL_31                 : 12;
	} bits;
};

union REG_ISP_EE_1B8 {
	uint32_t raw;
	struct {
		uint32_t EE_DEBUG_RESERVE                : 32;
	} bits;
};

union REG_ISP_EE_1BC {
	uint32_t raw;
	struct {
		uint32_t IMG_HEIGHT                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_WIDTH                       : 14;
	} bits;
};

union REG_ISP_EE_1C0 {
	uint32_t raw;
	struct {
		uint32_t GUARD_CNT                       : 8;
	} bits;
};

union REG_ISP_EE_1C4 {
	uint32_t raw;
	struct {
		uint32_t GUARD_CNT_UV                    : 8;
		uint32_t UV_BYPASS                       : 1;
		uint32_t UV_BYPASS_SEL                   : 1;
		uint32_t _rsv_10                         : 6;
		uint32_t EE_SHTCTRL_OSHTGAIN             : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_SHTCTRL_USHTGAIN             : 6;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_YCUR_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t YCUR_DATA_E                     : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t YCUR_DATA_O                     : 8;
		uint32_t _rsv_24                         : 7;
		uint32_t YCUR_W                          : 1;
	} bits;
};

union REG_ISP_YCUR_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t YCUR_WSEL                       : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t YCUR_RSEL                       : 1;
	} bits;
};

union REG_ISP_YCUR_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t YCUR_MAX                        : 9;
	} bits;
};

union REG_ISP_YCUR_CTRL {
	uint32_t raw;
	struct {
		uint32_t YCUR_ENABLE                     : 1;
		uint32_t YCUR_SHDW_SEL                   : 1;
	} bits;
};

union REG_ISP_YCUR_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t YCUR_MEM_SW_MODE                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t YCUR_MEM_SEL                    : 1;
	} bits;
};

union REG_ISP_YCUR_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t YCUR_SW_RADDR                   : 6;
	} bits;
};

union REG_ISP_YCUR_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t YCUR_RDATA_R                    : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t YCUR_SW_R                       : 1;
	} bits;
};

union REG_ISP_YCUR_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t GMA_PROG_1TO3_EN                : 1;
		uint32_t _rsv_5                          : 11;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_YCUR_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_YCUR_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_YCUR_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

union REG_ISP_YCUR_PROG_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t YCUR_ST_ADDR                    : 6;
		uint32_t _rsv_6                          : 25;
		uint32_t YCUR_ST_W                       : 1;
	} bits;
};

union REG_ISP_YCUR_RGB_DITHER {
	uint32_t raw;
	struct {
		uint32_t Y_DITHER_EN                     : 1;
		uint32_t Y_DITHER_MOD_EN                 : 1;
		uint32_t Y_DITHER_HISTIDX_EN             : 1;
		uint32_t Y_DITHER_FMNUM_EN               : 1;
		uint32_t _rsv_4                          : 1;
		uint32_t Y_DITHER_SOFTRST                : 1;
		uint32_t Y_DITHER_HEIGHTM1               : 12;
		uint32_t Y_DITHER_WIDTHM1                : 12;
	} bits;
};

union REG_ISP_YCUR_00 {
	uint32_t raw;
	struct {
		uint32_t Y_DITHER_DEBUG                  : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DCI_STATUS {
	uint32_t raw;
	struct {
		uint32_t DCI_STATUS                      : 32;
	} bits;
};

union REG_ISP_DCI_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t DCI_GRACE_RESET                 : 1;
	} bits;
};

union REG_ISP_DCI_MONITOR {
	uint32_t raw;
	struct {
		uint32_t DCI_MONITOR                     : 32;
	} bits;
};

union REG_ISP_DCI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DCI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 15;
		uint32_t DCI_HIST_ENABLE                 : 1;
	} bits;
};

union REG_ISP_DCI_MAP_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DCI_MAP_ENABLE                  : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t DCI_HISTO_BIG_ENDIAN            : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t DCI_ROI_ENABLE                  : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t DCI_DITHER_ENABLE               : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t DCI_ZEROING_ENABLE              : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t DCI_SHIFT_ENABLE                : 1;
		uint32_t _rsv_25                         : 3;
		uint32_t DCI_INDEX_ENABLE                : 1;
	} bits;
};

union REG_ISP_DCI_FLOW {
	uint32_t raw;
	struct {
		uint32_t DCI_ZERODCIOGRAM                : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DCI_SHADOW_SELECT               : 1;
	} bits;
};

union REG_ISP_DCI_DEMO_MODE {
	uint32_t raw;
	struct {
		uint32_t DCI_DEMO_MODE                   : 1;
	} bits;
};

union REG_ISP_DCI_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t DCI_MONITOR_SELECT              : 32;
	} bits;
};

union REG_ISP_DCI_LOCATION {
	uint32_t raw;
	struct {
		uint32_t DCI_LOCATION                    : 32;
	} bits;
};

union REG_ISP_DCI_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t DCI_PROG_DATA                   : 32;
	} bits;
};

union REG_ISP_DCI_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t DCI_PROG_CTRL                   : 32;
	} bits;
};

union REG_ISP_DCI_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t DCI_PROG_MAX                    : 32;
	} bits;
};

union REG_ISP_DCI_CTRL {
	uint32_t raw;
	struct {
		uint32_t DCI_CTRL                        : 32;
	} bits;
};

union REG_ISP_DCI_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t DCI_MEM_SW_MODE                 : 32;
	} bits;
};

union REG_ISP_DCI_MEM_RADDR {
	uint32_t raw;
	struct {
		uint32_t DCI_MEM_RADDR                   : 32;
	} bits;
};

union REG_ISP_DCI_MEM_RDATA {
	uint32_t raw;
	struct {
		uint32_t DCI_MEM_RDATA                   : 32;
	} bits;
};

union REG_ISP_DCI_DEBUG {
	uint32_t raw;
	struct {
		uint32_t DCI_DEBUG                       : 32;
	} bits;
};

union REG_ISP_DCI_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DCI_DUMMY                       : 32;
	} bits;
};

union REG_ISP_DCI_IMG_WIDTHM1 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1                     : 16;
	} bits;
};

union REG_ISP_DCI_LUT_ORDER_SELECT {
	uint32_t raw;
	struct {
		uint32_t DCI_LUT_ORDER_SELECT            : 1;
	} bits;
};

union REG_ISP_DCI_ROI_START {
	uint32_t raw;
	struct {
		uint32_t DCI_ROI_START_X                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t DCI_ROI_START_Y                 : 13;
	} bits;
};

union REG_ISP_DCI_ROI_GEO {
	uint32_t raw;
	struct {
		uint32_t DCI_ROI_WIDTHM1                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t DCI_ROI_HEIGHTM1                : 13;
	} bits;
};

union REG_ISP_DCI_MAPPED_LUT {
	uint32_t raw;
	struct {
		uint32_t DCI_MAPPED_LUT_LSB              : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DCI_MAPPED_LUT_MSB              : 10;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_TOP_0 {
	uint32_t raw;
	struct {
		uint32_t FRAME_DONE_PRE                  : 1;
		uint32_t FRAME_DONE_PRE1                 : 1;
		uint32_t FRAME_DONE_POST                 : 1;
		uint32_t SHAW_DONE_PRE                   : 1;
		uint32_t SHAW_DONE_PRE1                  : 1;
		uint32_t SHAW_DONE_POST                  : 1;
		uint32_t FRAME_ERR_PRE                   : 1;
		uint32_t FRAME_ERR_PRE1                  : 1;
		uint32_t FRAME_ERR_POST                  : 1;
		uint32_t CMDQ1_INT                       : 1;
		uint32_t CMDQ2_INT                       : 1;
		uint32_t CMDQ3_INT                       : 1;
		uint32_t FRAME_START_PRE                 : 1;
		uint32_t FRAME_START_PRE1                : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t PCHK0_ERR_PRE                   : 1;
		uint32_t PCHK0_ERR_PRE1                  : 1;
		uint32_t PCHK0_ERR_RAW                   : 1;
		uint32_t PCHK0_ERR_RGB                   : 1;
		uint32_t PCHK0_ERR_YUV                   : 1;
		uint32_t PCHK1_ERR_PRE                   : 1;
		uint32_t PCHK1_ERR_PRE1                  : 1;
		uint32_t PCHK1_ERR_RAW                   : 1;
		uint32_t PCHK1_ERR_RGB                   : 1;
		uint32_t PCHK1_ERR_YUV                   : 1;
		uint32_t LINE_REACH_INT_PRE              : 1;
		uint32_t LINE_REACH_INT_PRE1             : 1;
		uint32_t LINE_REACH_INT_YCROP            : 1;
	} bits;
};

union REG_ISP_TOP_1 {
	uint32_t raw;
	struct {
		uint32_t FRAME_DONE_PRE_EN               : 1;
		uint32_t FRAME_DONE_PRE1_EN              : 1;
		uint32_t FRAME_DONE_POST_EN              : 1;
		uint32_t SHAW_DONE_PRE_EN                : 1;
		uint32_t SHAW_DONE_PRE1_EN               : 1;
		uint32_t SHAW_DONE_POST_EN               : 1;
		uint32_t FRAME_ERR_PRE_EN                : 1;
		uint32_t FRAME_ERR_PRE1_EN               : 1;
		uint32_t FRAME_ERR_POST_EN               : 1;
		uint32_t CMDQ1_INT_EN                    : 1;
		uint32_t CMDQ2_INT_EN                    : 1;
		uint32_t CMDQ3_INT_EN                    : 1;
		uint32_t FRAME_START_PRE_EN              : 1;
		uint32_t FRAME_START_PRE1_EN             : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t PCHK0_ERR_PRE_EN                : 1;
		uint32_t PCHK0_ERR_PRE1_EN               : 1;
		uint32_t PCHK0_ERR_RAW_EN                : 1;
		uint32_t PCHK0_ERR_RGB_EN                : 1;
		uint32_t PCHK0_ERR_YUV_EN                : 1;
		uint32_t PCHK1_ERR_PRE_EN                : 1;
		uint32_t PCHK1_ERR_PRE1_EN               : 1;
		uint32_t PCHK1_ERR_RAW_EN                : 1;
		uint32_t PCHK1_ERR_RGB_EN                : 1;
		uint32_t PCHK1_ERR_YUV_EN                : 1;
		uint32_t LINE_REACH_INT_PRE_EN           : 1;
		uint32_t LINE_REACH_INT_PRE1_EN          : 1;
		uint32_t LINE_REACH_INT_YCROP_EN         : 1;
	} bits;
};

union REG_ISP_TOP_2 {
	uint32_t raw;
	struct {
		uint32_t TRIG_STR_PRE                    : 1;
		uint32_t TRIG_STR_PRE1                   : 1;
		uint32_t TRIG_STR_POST                   : 1;
		uint32_t SHAW_UP_PRE                     : 1;
		uint32_t SHAW_UP_PRE1                    : 1;
		uint32_t SHAW_UP_POST                    : 1;
	} bits;
};

union REG_ISP_TOP_3 {
	uint32_t raw;
	struct {
		uint32_t TRIG_STR_SEL_PRE                : 1;
		uint32_t TRIG_STR_SEL_PRE1               : 1;
		uint32_t TRIG_STR_SEL_POST               : 1;
		uint32_t SHAW_UP_SEL_PRE                 : 1;
		uint32_t SHAW_UP_SEL_PRE1                : 1;
		uint32_t SHAW_UP_SEL_POST                : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t BLK_IDLE_EN_CSI1                : 1;
		uint32_t BLK_IDLE_EN_CSI2                : 1;
		uint32_t BLK_IDLE_EN_ISP                 : 1;
	} bits;
};

union REG_ISP_TOP_4 {
	uint32_t raw;
	struct {
		uint32_t SRC_IN                          : 2;
		uint32_t DATA_FORMAT                     : 2;
		uint32_t DEST_OUT                        : 1;
		uint32_t ONLINE_MOD                      : 1;
		uint32_t SENSOR_MOD                      : 2;
		uint32_t HDR_EN                          : 1;
		uint32_t REG_3DNR_EN                     : 1;
	} bits;
};

union REG_ISP_TOP_5 {
	uint32_t raw;
	struct {
		uint32_t IMG_W                           : 16;
		uint32_t IMG_H                           : 16;
	} bits;
};

union REG_ISP_TOP_6 {
	uint32_t raw;
	struct {
		uint32_t IP_RST                          : 1;
		uint32_t CSI_RST                         : 1;
		uint32_t CSI2_RST                        : 1;
		uint32_t AXI_RST                         : 1;
		uint32_t CMDQ1_RST                       : 1;
		uint32_t CMDQ2_RST                       : 1;
		uint32_t CMDQ3_RST                       : 1;
	} bits;
};

union REG_ISP_TOP_7 {
	uint32_t raw;
	struct {
		uint32_t PRE_BLK_IDLE                    : 1;
		uint32_t PRE1_BLK_IDLE                   : 1;
		uint32_t RAW_BLK_IDLE                    : 1;
		uint32_t RGB_BLK_IDLE                    : 1;
		uint32_t YUV_BLK_IDLE                    : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PRE_DMA_IDLE                    : 1;
		uint32_t PRE1_DMA_IDLE                   : 1;
		uint32_t RAW_DMA_IDLE                    : 1;
		uint32_t RGB_DMA_IDLE                    : 1;
		uint32_t YUV_DMA_IDLE                    : 1;
	} bits;
};

union REG_ISP_TOP_C {
	uint32_t raw;
	struct {
		uint32_t SVN_REVISION                    : 32;
	} bits;
};

union REG_ISP_TOP_D {
	uint32_t raw;
	struct {
		uint32_t UNIX_TIMESTAMP                  : 32;
	} bits;
};

union REG_ISP_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t DBUS0                           : 32;
	} bits;
};

union REG_ISP_TOP_11 {
	uint32_t raw;
	struct {
		uint32_t DBUS1                           : 32;
	} bits;
};

union REG_ISP_TOP_12 {
	uint32_t raw;
	struct {
		uint32_t DBUS2                           : 32;
	} bits;
};

union REG_ISP_TOP_13 {
	uint32_t raw;
	struct {
		uint32_t DBUS3                           : 32;
	} bits;
};

union REG_ISP_TOP_14 {
	uint32_t raw;
	struct {
		uint32_t FORCE_ISP_INT                   : 1;
		uint32_t FORCE_ISP_INT_EN                : 1;
	} bits;
};

union REG_ISP_TOP_15 {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_PCHK_SHADOW_RD_SEL {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
	} bits;
};

union REG_ISP_PCHK_IN_SEL        {
	uint32_t raw;
	struct {
		uint32_t PCHK_IN_SEL                     : 6;
	} bits;
};

union REG_ISP_PCHK_RULE_EN       {
	uint32_t raw;
	struct {
		uint32_t PCHK_RULE_EN                    : 12;
	} bits;
};

union REG_ISP_PCHK_HSIZE         {
	uint32_t raw;
	struct {
		uint32_t PCHK_HSIZE                      : 16;
	} bits;
};

union REG_ISP_PCHK_VSIZE         {
	uint32_t raw;
	struct {
		uint32_t PCHK_VSIZE                      : 16;
	} bits;
};

union REG_ISP_PCHK_NRDY_LIMIT    {
	uint32_t raw;
	struct {
		uint32_t PCHK_NRDY_LIMIT                 : 24;
	} bits;
};

union REG_ISP_PCHK_NREQ_LIMIT    {
	uint32_t raw;
	struct {
		uint32_t PCHK_NREQ_LIMIT                 : 24;
	} bits;
};

union REG_ISP_PCHK_PFREQ_LIMIT   {
	uint32_t raw;
	struct {
		uint32_t PCHK_PFREQ_LIMIT                : 10;
	} bits;
};

union REG_ISP_PCHK_ERR_BUS       {
	uint32_t raw;
	struct {
		uint32_t PCHK_ERR_BUS                    : 12;
	} bits;
};

union REG_ISP_PCHK_ERR_XY        {
	uint32_t raw;
	struct {
		uint32_t X                               : 16;
		uint32_t Y                               : 16;
	} bits;
};

union REG_ISP_PCHK_ERR_CLR       {
	uint32_t raw;
	struct {
		uint32_t CLEAR                           : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGB_DITHER_RGB_DITHER {
	uint32_t raw;
	struct {
		uint32_t RGB_DITHER_ENABLE               : 1;
		uint32_t RGB_DITHER_MOD_EN               : 1;
		uint32_t RGB_DITHER_HISTIDX_EN           : 1;
		uint32_t RGB_DITHER_FMNUM_EN             : 1;
		uint32_t RGB_DITHER_SHDW_SEL             : 1;
		uint32_t RGB_DITHER_SOFTRST              : 1;
		uint32_t CROP_WIDTHM1                    : 12;
		uint32_t CROP_HEIGHTM1                   : 12;
	} bits;
};

union REG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0 {
	uint32_t raw;
	struct {
		uint32_t RGB_DITHER_DEBUG0               : 32;
	} bits;
};

#endif // _REG_FIELDS_H_
