/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name:vi_reg_fields.h
 * Description:HW register description
 */

#ifndef _VI_REG_FIELDS_H_
#define _VI_REG_FIELDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_PRE_RAW_BE_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t BAYER_TYPE_LE                   : 4;
		uint32_t BAYER_TYPE_SE                   : 4;
		uint32_t RGBIR_EN                        : 1;
		uint32_t CH_NUM                          : 1;
		uint32_t _rsv_10                         : 21;
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_PRE_RAW_BE_UP_PQ_EN {
	uint32_t raw;
	struct {
		uint32_t UP_PQ_EN                        : 1;
	} bits;
};

union REG_PRE_RAW_BE_IMG_SIZE_LE {
	uint32_t raw;
	struct {
		uint32_t FRAME_WIDTHM1                   : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FRAME_HEIGHTM1                  : 13;
	} bits;
};

union REG_PRE_RAW_BE_PRE_RAW_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RW                        : 16;
		uint32_t DUMMY_RO                        : 16;
	} bits;
};

union REG_PRE_RAW_BE_DEBUG_INFO {
	uint32_t raw;
	struct {
		uint32_t CH0_CROP_DONE                   : 1;
		uint32_t CH0_BLC_DONE                    : 1;
		uint32_t CH0_AF_DONE                     : 1;
		uint32_t CH0_DPC_DONE                    : 1;
		uint32_t CH0_ASYNC_DONE                  : 1;
		uint32_t CH0_PRE_WDMA_DONE               : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CH1_CROP_DONE                   : 1;
		uint32_t CH1_BLC_DONE                    : 1;
		uint32_t CH1_DPC_DONE                    : 1;
		uint32_t CH1_ASYNC_DONE                  : 1;
		uint32_t CH1_PRE_WDMA_DONE               : 1;
	} bits;
};

union REG_PRE_RAW_BE_DMA_IDLE_INFO {
	uint32_t raw;
	struct {
		uint32_t AF_DMA_IDLE                     : 1;
		uint32_t PRE_WDMA0_IDLE                  : 1;
		uint32_t PRE_WDMA1_IDLE                  : 1;
	} bits;
};

union REG_PRE_RAW_BE_IP_IDLE_INFO {
	uint32_t raw;
	struct {
		uint32_t CH0_CROP_IDLE                   : 1;
		uint32_t CH0_BLC_IDLE                    : 1;
		uint32_t CH0_DPC_IDLE                    : 1;
		uint32_t CH0_AF_IDLE                     : 1;
		uint32_t CH0_ASYNC_IDLE                  : 1;
		uint32_t CH0_PRE_WDMA_IDLE               : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CH1_CROP_IDLE                   : 1;
		uint32_t CH1_BLC_IDLE                    : 1;
		uint32_t CH1_DPC_IDLE                    : 1;
		uint32_t CH1_ASYNC_IDLE                  : 1;
		uint32_t CH1_PRE_WDMA_IDLE               : 1;
	} bits;
};

union REG_PRE_RAW_BE_LINE_BALANCE_CTRL {
	uint32_t raw;
	struct {
		uint32_t PASS_SEL                        : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t PASS_CNT_M1                     : 8;
	} bits;
};

union REG_PRE_RAW_BE_DEBUG_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DEBUG_EN                        : 1;
	} bits;
};

union REG_PRE_RAW_BE_TVALID_STATUS {
	uint32_t raw;
	struct {
		uint32_t IP_TVALID                       : 16;
	} bits;
};

union REG_PRE_RAW_BE_TREADY_STATUS {
	uint32_t raw;
	struct {
		uint32_t IP_TREADY                       : 16;
	} bits;
};

union REG_PRE_RAW_BE_PATGEN1 {
	uint32_t raw;
	struct {
		uint32_t X_CURSER                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t Y_CURSER                        : 14;
		uint32_t CURSER_EN                       : 1;
		uint32_t PG_ENABLE                       : 1;
	} bits;
};

union REG_PRE_RAW_BE_PATGEN2 {
	uint32_t raw;
	struct {
		uint32_t CURSER_VALUE                    : 16;
	} bits;
};

union REG_PRE_RAW_BE_PATGEN3 {
	uint32_t raw;
	struct {
		uint32_t VALUE_REPORT                    : 32;
	} bits;
};

union REG_PRE_RAW_BE_PATGEN4 {
	uint32_t raw;
	struct {
		uint32_t XCNT_RPT                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t YCNT_RPT                        : 14;
	} bits;
};

union REG_PRE_RAW_BE_CHKSUM_ENABLE {
	uint32_t raw;
	struct {
		uint32_t LEXP_CHKSUM_ENABLE              : 1;
		uint32_t SEXP_CHKSUM_ENABLE              : 1;
	} bits;
};

union REG_PRE_RAW_BE_CHKSUM {
	uint32_t raw;
	struct {
		uint32_t UM_LEXP                         : 12;
		uint32_t UM_SEXP                         : 16;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DMA_CTL_SYS_CONTROL {
	uint32_t raw;
	struct {
		uint32_t QOS_SEL                         : 1;
		uint32_t SW_QOS                          : 1;
		uint32_t ENABLE_SEL                      : 1;
		uint32_t SW_DISABLE                      : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t BASEH                           : 8;
		uint32_t BASE_SEL                        : 1;
		uint32_t STRIDE_SEL                      : 1;
		uint32_t SEGLEN_SEL                      : 1;
		uint32_t SEGNUM_SEL                      : 1;
		uint32_t SLICE_ENABLE                    : 1;
		uint32_t UPDATE_BASE_ADDR                : 1;
		uint32_t _rsv_22                         : 6;
		uint32_t DBG_SEL                         : 3;
	} bits;
};

union REG_ISP_DMA_CTL_BASE_ADDR {
	uint32_t raw;
	struct {
		uint32_t BASEL                           : 32;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_SEGLEN {
	uint32_t raw;
	struct {
		uint32_t SEGLEN                          : 24;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_STRIDE {
	uint32_t raw;
	struct {
		uint32_t STRIDE                          : 24;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_SEGNUM {
	uint32_t raw;
	struct {
		uint32_t SEGNUM                          : 13;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_STATUS {
	uint32_t raw;
	struct {
		uint32_t STATUS                          : 32;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_SLICESIZE {
	uint32_t raw;
	struct {
		uint32_t SLICE_SIZE                      : 6;
		uint32_t _rsv_6                          : 10;
		uint32_t SLICE_NUM                       : 13;
	} bits;
};

union REG_ISP_DMA_CTL_DMA_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 16;
		uint32_t PERF_PATCH_ENABLE               : 1;
		uint32_t SEGLEN_LESS16_ENABLE            : 1;
		uint32_t SYNC_PATCH_ENABLE               : 1;
		uint32_t TRIG_PATCH_ENABLE               : 1;
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
		uint32_t _rsv_1                          : 19;
		uint32_t HIST_ZEROING_ENABLE             : 1;
		uint32_t _rsv_21                         : 7;
		uint32_t FORCE_CLK_ENABLE                : 1;
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
		uint32_t _rsv_11                         : 5;
		uint32_t AE_FACE_ENABLE                  : 4;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE0_HIST_ENABLE {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_HIST_ENABLE             : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t AE0_GAIN_ENABLE                 : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t HIST0_ENABLE                    : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t HIST0_GAIN_ENABLE               : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t IR_AE_ENABLE                    : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t IR_AE_GAIN_ENABLE               : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t IR_HIST_ENABLE                  : 1;
		uint32_t _rsv_25                         : 3;
		uint32_t IR_HIST_GAIN_ENABLE             : 1;
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
		uint32_t STS_AE0_NUMXM1                  : 6;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_NUMYM1 {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_NUMYM1                  : 6;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_WIDTH {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_WIDTH                   : 8;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_HEIGHT                  : 8;
	} bits;
};

union REG_ISP_AE_HIST_STS_AE_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t STS_AE0_STS_DIV                 : 13;
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

union REG_ISP_AE_HIST_STS_IR_AE_OFFSETX {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_OFFSETX               : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_OFFSETY {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_OFFSETY               : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_NUMXM1 {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_NUMXM1                : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_NUMYM1 {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_NUMYM1                : 5;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_WIDTH {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_WIDTH                 : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_HEIGHT                : 10;
	} bits;
};

union REG_ISP_AE_HIST_STS_IR_AE_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t STS_IR_AE_STS_DIV               : 3;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_BAYER_STARTING {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_BAYER_STARTING          : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
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

union REG_ISP_AE_HIST_WBG_7 {
	uint32_t raw;
	struct {
		uint32_t AE0_WBG_VGAIN                   : 14;
	} bits;
};

union REG_ISP_AE_HIST_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
		uint32_t _rsv_9                          : 7;
		uint32_t IR_DMI_ENABLE                   : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t IR_DMI_QOS                      : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t IR_FORCE_DMA_DISABLE            : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE0_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AE_FACE0_OFFSET_X               : 13;
		uint32_t AE_FACE0_OFFSET_Y               : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE1_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AE_FACE1_OFFSET_X               : 13;
		uint32_t AE_FACE1_OFFSET_Y               : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE2_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AE_FACE2_OFFSET_X               : 13;
		uint32_t AE_FACE2_OFFSET_Y               : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE3_LOCATION {
	uint32_t raw;
	struct {
		uint32_t AE_FACE3_OFFSET_X               : 13;
		uint32_t AE_FACE3_OFFSET_Y               : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE0_SIZE {
	uint32_t raw;
	struct {
		uint32_t AE_FACE0_SIZE_MINUS1_X          : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AE_FACE0_SIZE_MINUS1_Y          : 8;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE1_SIZE {
	uint32_t raw;
	struct {
		uint32_t AE_FACE1_SIZE_MINUS1_X          : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AE_FACE1_SIZE_MINUS1_Y          : 8;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE2_SIZE {
	uint32_t raw;
	struct {
		uint32_t AE_FACE2_SIZE_MINUS1_X          : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AE_FACE2_SIZE_MINUS1_Y          : 8;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE3_SIZE {
	uint32_t raw;
	struct {
		uint32_t AE_FACE3_SIZE_MINUS1_X          : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AE_FACE3_SIZE_MINUS1_Y          : 8;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE0_LOCATION {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE0_OFFSET_X            : 16;
		uint32_t IR_AE_FACE0_OFFSET_Y            : 16;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE1_LOCATION {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE1_OFFSET_X            : 16;
		uint32_t IR_AE_FACE1_OFFSET_Y            : 16;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE2_LOCATION {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE2_OFFSET_X            : 16;
		uint32_t IR_AE_FACE2_OFFSET_Y            : 16;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE3_LOCATION {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE3_OFFSET_X            : 16;
		uint32_t IR_AE_FACE3_OFFSET_Y            : 16;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE0_SIZE {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE0_SIZE_MINUS1_X       : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t IR_AE_FACE0_SIZE_MINUS1_Y       : 7;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE1_SIZE {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE1_SIZE_MINUS1_X       : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t IR_AE_FACE1_SIZE_MINUS1_Y       : 7;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE2_SIZE {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE2_SIZE_MINUS1_X       : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t IR_AE_FACE2_SIZE_MINUS1_Y       : 7;
	} bits;
};

union REG_ISP_AE_HIST_IR_AE_FACE3_SIZE {
	uint32_t raw;
	struct {
		uint32_t IR_AE_FACE3_SIZE_MINUS1_X       : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t IR_AE_FACE3_SIZE_MINUS1_Y       : 7;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE0_ENABLE {
	uint32_t raw;
	struct {
		uint32_t AE_FACE0_ENABLE                 : 1;
		uint32_t AE_FACE1_ENABLE                 : 1;
		uint32_t AE_FACE2_ENABLE                 : 1;
		uint32_t AE_FACE3_ENABLE                 : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE0_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t AE_FACE0_STS_DIV                : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE1_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t AE_FACE1_STS_DIV                : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE2_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t AE_FACE2_STS_DIV                : 13;
	} bits;
};

union REG_ISP_AE_HIST_AE_FACE3_STS_DIV {
	uint32_t raw;
	struct {
		uint32_t AE_FACE3_STS_DIV                : 13;
	} bits;
};

union REG_ISP_AE_HIST_STS_ENABLE {
	uint32_t raw;
	struct {
		uint32_t STS_AWB_ENABLE                  : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_ALGO_ENABLE {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 1;
		uint32_t AE_ALGO_ENABLE                  : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_LOW {
	uint32_t raw;
	struct {
		uint32_t AE_HIST_LOW                     : 8;
	} bits;
};

union REG_ISP_AE_HIST_AE_HIST_HIGH {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 8;
		uint32_t AE_HIST_HIGH                    : 8;
	} bits;
};

union REG_ISP_AE_HIST_AE_TOP {
	uint32_t raw;
	struct {
		uint32_t AE_AWB_TOP                      : 12;
	} bits;
};

union REG_ISP_AE_HIST_AE_BOT {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 12;
		uint32_t AE_AWB_BOT                      : 12;
	} bits;
};

union REG_ISP_AE_HIST_AE_OVEREXP_THR {
	uint32_t raw;
	struct {
		uint32_t AE_OVEREXP_THR                  : 10;
	} bits;
};

union REG_ISP_AE_HIST_AE_NUM_GAPLINE {
	uint32_t raw;
	struct {
		uint32_t AE_NUM_GAPLINE                  : 1;
	} bits;
};

union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_LUMA {
	uint32_t raw;
	struct {
		uint32_t SIMPLE2A_RESULT_LUMA            : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t E_COUNTER_0                     : 16;
	} bits;
};

union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_RGAIN {
	uint32_t raw;
	struct {
		uint32_t SIMPLE2A_RESULT_RGAIN           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t E_COUNTER_1                     : 16;
	} bits;
};

union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_BGAIN {
	uint32_t raw;
	struct {
		uint32_t SIMPLE2A_RESULT_BGAIN           : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t E_COUNTER_2                     : 16;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_00 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_00                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_01 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_01                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_02 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_02                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_03 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_03                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_04 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_04                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_05 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_05                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_06 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_06                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_07 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_07                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_08 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_08                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_09 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_09                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_10 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_10                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_11 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_11                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_12 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_12                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_13 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_13                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_14 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_14                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_15 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_15                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_16 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_16                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_17 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_17                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_18 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_18                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_19 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_19                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_20 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_20                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_21 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_21                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_22 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_22                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_23 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_23                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_24 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_24                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_25 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_25                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_26 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_26                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_27 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_27                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_28 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_28                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_29 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_29                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_30 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_30                       : 32;
	} bits;
};

union REG_ISP_AE_HIST_AE_WGT_31 {
	uint32_t raw;
	struct {
		uint32_t AE_WGT_31                       : 32;
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
		uint32_t OUT_SHIFTBIT                    : 3;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
		uint32_t _rsv_17                         : 11;
		uint32_t FORCE_CLK_ENABLE                : 1;
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

union REG_ISP_GMS_X_SIZEM1 {
	uint32_t raw;
	struct {
		uint32_t GMS_X_SIZEM1                    : 10;
	} bits;
};

union REG_ISP_GMS_Y_SIZEM1 {
	uint32_t raw;
	struct {
		uint32_t GMS_Y_SIZEM1                    : 10;
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

union REG_ISP_GMS_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
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
		uint32_t _rsv_5                          : 3;
		uint32_t AF_REVERT_EXPOSURE              : 1;
		uint32_t _rsv_9                          : 7;
		uint32_t AF_GAIN_ENABLE                  : 1;
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
		uint32_t _rsv_25                         : 3;
		uint32_t FORCE_CLK_ENABLE                : 1;
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
		uint32_t AF_BLOCK_NUM_Y                  : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
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
		uint32_t AF_CORNING_OFFSET_HORIZONTAL_0  : 8;
	} bits;
};

union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1 {
	uint32_t raw;
	struct {
		uint32_t AF_CORNING_OFFSET_HORIZONTAL_1  : 8;
	} bits;
};

union REG_ISP_AF_CORNING_OFFSET_VERTICAL {
	uint32_t raw;
	struct {
		uint32_t AF_CORNING_OFFSET_VERTICAL      : 8;
	} bits;
};

union REG_ISP_AF_HIGH_Y_THRE {
	uint32_t raw;
	struct {
		uint32_t AF_HIGH_Y_THRE                  : 8;
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

union REG_ISP_AF_TH_LOW    {
	uint32_t raw;
	struct {
		uint32_t AF_TH_LOW                       : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AF_TH_HIGH                      : 8;
	} bits;
};

union REG_ISP_AF_GAIN_LOW  {
	uint32_t raw;
	struct {
		uint32_t AF_GAIN_LOW                     : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t AF_GAIN_HIGH                    : 8;
	} bits;
};

union REG_ISP_AF_SLOP_LOW {
	uint32_t raw;
	struct {
		uint32_t AF_SLOP_LOW                     : 4;
		uint32_t AF_SLOP_HIGH                    : 4;
	} bits;
};

union REG_ISP_AF_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
	} bits;
};

union REG_ISP_AF_SQUARE_ENABLE {
	uint32_t raw;
	struct {
		uint32_t AF_SQUARE_ENABLE                : 1;
	} bits;
};

union REG_ISP_AF_OUTSHIFT {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 4;
		uint32_t AF_OUTSHIFT                     : 3;
	} bits;
};

union REG_ISP_AF_NUM_GAPLINE {
	uint32_t raw;
	struct {
		uint32_t AF_NUM_GAPLINE                  : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_PRE_RAW_FE_PRE_RAW_CTRL {
	uint32_t raw;
	struct {
		uint32_t BAYER_TYPE_LE                   : 4;
		uint32_t BAYER_TYPE_SE                   : 4;
		uint32_t _rsv_8                          : 8;
		uint32_t UP_PQ_EN                        : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t RGBIR_EN                        : 1;
		uint32_t _rsv_21                         : 10;
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_FRAME_SIZE {
	uint32_t raw;
	struct {
		uint32_t FRAME_WIDTHM1                   : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FRAME_HEIGHTM1                  : 13;
	} bits;
};

union REG_PRE_RAW_FE_LE_RGBMAP_GRID_NUMBER {
	uint32_t raw;
	struct {
		uint32_t LE_RGBMP_H_GRID_NUMM1           : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t LE_RGBMP_H_GRID_SIZE            : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t LE_RGBMP_V_GRID_NUMM1           : 10;
		uint32_t _rsv_26                         : 2;
		uint32_t LE_RGBMP_V_GRID_SIZE            : 3;
	} bits;
};

union REG_PRE_RAW_FE_SE_RGBMAP_GRID_NUMBER {
	uint32_t raw;
	struct {
		uint32_t SE_RGBMP_H_GRID_NUMM1           : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t SE_RGBMP_H_GRID_SIZE            : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t SE_RGBMP_V_GRID_NUMM1           : 10;
		uint32_t _rsv_26                         : 2;
		uint32_t SE_RGBMP_V_GRID_SIZE            : 3;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_POST_NO_RSPD_CYC {
	uint32_t raw;
	struct {
		uint32_t POST_NO_RSPD_CYC                : 32;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC {
	uint32_t raw;
	struct {
		uint32_t POST_RGBMAP_NO_RSPD_CYC         : 32;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_FRAME_VLD {
	uint32_t raw;
	struct {
		uint32_t FE_FRAME_VLD_CH0                : 1;
		uint32_t FE_FRAME_VLD_CH1                : 1;
		uint32_t FE_FRAME_VLD_CH2                : 1;
		uint32_t FE_FRAME_VLD_CH3                : 1;
		uint32_t FE_PQ_VLD_CH0                   : 1;
		uint32_t FE_PQ_VLD_CH1                   : 1;
		uint32_t FE_PQ_VLD_CH2                   : 1;
		uint32_t FE_PQ_VLD_CH3                   : 1;
		uint32_t _rsv_8                          : 8;
		uint32_t POST_RAW_IDLE                   : 1;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_DEBUG_STATE {
	uint32_t raw;
	struct {
		uint32_t PRE_RAW_FE_IDLE                 : 32;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RW                        : 16;
		uint32_t DUMMY_RO                        : 16;
	} bits;
};

union REG_PRE_RAW_FE_PRE_RAW_DEBUG_INFO {
	uint32_t raw;
	struct {
		uint32_t IP_FRAME_DONE_STS               : 32;
	} bits;
};

union REG_PRE_RAW_FE_IDLE_INFO {
	uint32_t raw;
	struct {
		uint32_t IP_DMA_IDLE                     : 32;
	} bits;
};

union REG_PRE_RAW_FE_CHECK_SUM {
	uint32_t raw;
	struct {
		uint32_t LEXP_CHKSUM_ENABLE              : 1;
		uint32_t SEXP_CHKSUM_ENABLE              : 1;
	} bits;
};

union REG_PRE_RAW_FE_CHECK_SUM_VALUE {
	uint32_t raw;
	struct {
		uint32_t LEXP_CHKSUM_VALUE               : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t SEXP_CHKSUM_VALUE               : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CSI_BDG_DVP_BDG_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t CSI_MODE                        : 2;
		uint32_t CSI_IN_FORMAT                   : 1;
		uint32_t CSI_IN_YUV_FORMAT               : 1;
		uint32_t CH_NUM                          : 2;
		uint32_t CH0_DMA_WR_ENABLE               : 1;
		uint32_t _rsv_7                          : 2;
		uint32_t Y_ONLY                          : 1;
		uint32_t PXL_DATA_SEL                    : 1;
		uint32_t VS_POL                          : 1;
		uint32_t HS_POL                          : 1;
		uint32_t RESET_MODE                      : 1;
		uint32_t VS_MODE                         : 1;
		uint32_t ABORT_MODE                      : 1;
		uint32_t RESET                           : 1;
		uint32_t ABORT                           : 1;
		uint32_t _rsv_18                         : 2;
		uint32_t YUV_PACK_MODE                   : 1;
		uint32_t MULTI_CH_FRAME_SYNC_EN          : 1;
		uint32_t CH0_DMA_420_WR_ENABLE           : 1;
		uint32_t _rsv_23                         : 1;
		uint32_t CSI_ENABLE                      : 1;
		uint32_t TGEN_ENABLE                     : 1;
		uint32_t YUV2BAY_ENABLE                  : 1;
		uint32_t _rsv_27                         : 1;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_29                         : 2;
		uint32_t CSI_UP_REG                      : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BDG_INTERRUPT_CTRL {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_INT_EN                   : 1;
		uint32_t CH0_TRIG_INT_EN                 : 1;
		uint32_t CH0_DROP_INT_EN                 : 1;
		uint32_t CH0_SIZE_ERROR_INT_EN           : 1;
		uint32_t _rsv_4                          : 24;
		uint32_t SLICE_LINE_INTP_EN              : 1;
		uint32_t DMA_ERROR_INTP_EN               : 1;
		uint32_t LINE_INTP_EN                    : 1;
		uint32_t FIFO_OVERFLOW_INT_EN            : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BDG_DMA_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DMA_ST_DPCM_MODE                : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t DPCM_MIPI_OPT                   : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t DROP_MODE                       : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t AVG_MODE                        : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BDG_DMA_LD_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DMA_LD_DPCM_MODE                : 3;
		uint32_t _rsv_3                          : 13;
		uint32_t DPCM_RX_XSTR                    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH0_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_BLC_SUM {
	uint32_t raw;
	struct {
		uint32_t CH0_BLC_SUM                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_PAT_GEN_CTRL {
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

union REG_ISP_CSI_BDG_DVP_PAT_IDX_CTRL {
	uint32_t raw;
	struct {
		uint32_t PAT_PRD                         : 8;
		uint32_t PAT_IDX                         : 5;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_PAT_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t PAT_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PAT_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_PAT_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t PAT_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t FDE_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FDE_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t FDE_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_FIX_COLOR_0 {
	uint32_t raw;
	struct {
		uint32_t MDE_R                           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MDE_G                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_FIX_COLOR_1 {
	uint32_t raw;
	struct {
		uint32_t MDE_B                           : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_MDE_V_SIZE {
	uint32_t raw;
	struct {
		uint32_t VMDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VMDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_MDE_H_SIZE {
	uint32_t raw;
	struct {
		uint32_t HMDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HMDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_FDE_V_SIZE {
	uint32_t raw;
	struct {
		uint32_t VFDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VFDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_FDE_H_SIZE {
	uint32_t raw;
	struct {
		uint32_t HFDE_STR                        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HFDE_STP                        : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_HSYNC_CTRL {
	uint32_t raw;
	struct {
		uint32_t HS_STR                          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t HS_STP                          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_VSYNC_CTRL {
	uint32_t raw;
	struct {
		uint32_t VS_STR                          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VS_STP                          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_TGEN_TT_SIZE {
	uint32_t raw;
	struct {
		uint32_t HTT                             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t VTT                             : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_LINE_INTP_HEIGHT_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_INTP_HEIGHTM1          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_CNT                      : 16;
		uint32_t CH0_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH0_TOT_BLK_IDLE                : 1;
		uint32_t CH0_TOT_DMA_IDLE                : 1;
		uint32_t CH0_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_DROP_INT              : 1;
		uint32_t CH0_VS_INT                      : 1;
		uint32_t CH0_TRIG_INT                    : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t CH0_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH0_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH0_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH0_FRAME_HEIGHT_LS_INT         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_1 {
	uint32_t raw;
	struct {
		uint32_t FIFO_OVERFLOW_INT               : 1;
		uint32_t FRAME_RESOLUTION_OVER_MAX_INT   : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t DMA_ERROR_INT                   : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CH0_LINE_INTP_INT               : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t CH0_SLICE_LINE_INTP_INT         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_BDG_DEBUG {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_IDLE                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_OUT_VSYNC_LINE_DELAY {
	uint32_t raw;
	struct {
		uint32_t OUT_VSYNC_LINE_DELAY            : 12;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_WR_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t WR_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t WR_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_RD_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t RD_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t RD_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_IN                        : 16;
		uint32_t DUMMY_OUT                       : 16;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_SLICE_LINE_INTP_HEIGHT_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_SLICE_LINE_INTP_HEIGHTM1    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_WDMA_CH0_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_CROP_EN                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_WDMA_CH0_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_HORZ_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH0_HORZ_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_WDMA_CH0_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_VERT_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH0_VERT_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_0 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_1 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_VALUE                  : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_CROP_0 {
	uint32_t raw;
	struct {
		uint32_t CROP_ENABLE                     : 1;
		uint32_t DMA_ENABLE                      : 1;
		uint32_t SHAW_READ_SEL                   : 1;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t DPCM_MODE                       : 3;
		uint32_t _rsv_11                         : 5;
		uint32_t DPCM_XSTR                       : 13;
		uint32_t _rsv_29                         : 2;
		uint32_t DMI16B_EN                       : 1;
	} bits;
};

union REG_CROP_1 {
	uint32_t raw;
	struct {
		uint32_t CROP_START_Y                    : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CROP_END_Y                      : 13;
	} bits;
};

union REG_CROP_2 {
	uint32_t raw;
	struct {
		uint32_t CROP_START_X                    : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CROP_END_X                      : 13;
	} bits;
};

union REG_CROP_3 {
	uint32_t raw;
	struct {
		uint32_t IN_WIDTHM1                      : 16;
		uint32_t IN_HEIGHTM1                     : 16;
	} bits;
};

union REG_CROP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};

union REG_CROP_DEBUG {
	uint32_t raw;
	struct {
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t FORCE_DMA_DISABLE               : 1;
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
		uint32_t _rsv_1                          : 27;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_BLC_3 {
	uint32_t raw;
	struct {
		uint32_t BLC_OFFSET_R                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BLC_OFFSET_GR                   : 12;
	} bits;
};

union REG_ISP_BLC_4 {
	uint32_t raw;
	struct {
		uint32_t BLC_OFFSET_GB                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BLC_OFFSET_B                    : 12;
	} bits;
};

union REG_ISP_BLC_5 {
	uint32_t raw;
	struct {
		uint32_t BLC_GAIN_R                      : 16;
		uint32_t BLC_GAIN_GR                     : 16;
	} bits;
};

union REG_ISP_BLC_6 {
	uint32_t raw;
	struct {
		uint32_t BLC_GAIN_GB                     : 16;
		uint32_t BLC_GAIN_B                      : 16;
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
		uint32_t _rsv_1                          : 15;
		uint32_t FORCE_BAYER_ENABLE              : 1;
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

union REG_ISP_BLC_9 {
	uint32_t raw;
	struct {
		uint32_t BLC_2NDOFFSET_R                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BLC_2NDOFFSET_GR                : 12;
	} bits;
};

union REG_ISP_BLC_A {
	uint32_t raw;
	struct {
		uint32_t BLC_2NDOFFSET_GB                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BLC_2NDOFFSET_B                 : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LMAP_LMP_0 {
	uint32_t raw;
	struct {
		uint32_t LMAP_ENABLE                     : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t LMAP_Y_MODE                     : 2;
		uint32_t LMAP_THD_L                      : 8;
		uint32_t LMAP_THD_H                      : 8;
		uint32_t _rsv_22                         : 8;
		uint32_t LMAP_SOFTRST                    : 1;
		uint32_t FORCE_DMA_DISABLE               : 1;
	} bits;
};

union REG_ISP_LMAP_LMP_1 {
	uint32_t raw;
	struct {
		uint32_t LMAP_CROP_WIDTHM1               : 13;
		uint32_t LMAP_CROP_HEIGHTM1              : 13;
		uint32_t _rsv_26                         : 1;
		uint32_t LMAP_BAYER_ID                   : 4;
		uint32_t LMAP_SHDW_SEL                   : 1;
	} bits;
};

union REG_ISP_LMAP_LMP_2 {
	uint32_t raw;
	struct {
		uint32_t LMAP_W_GRID_NUM                 : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t LMAP_W_BIT                      : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t LMAP_H_GRID_NUM                 : 10;
		uint32_t _rsv_26                         : 2;
		uint32_t LMAP_H_BIT                      : 3;
		uint32_t LMAP_OUT_SEL                    : 1;
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

union REG_ISP_LMAP_LMP_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t LMAP_DEBUG_2                    : 32;
	} bits;
};

union REG_ISP_LMAP_LMP_3 {
	uint32_t raw;
	struct {
		uint32_t DEBUG_DATA_SEL                  : 2;
	} bits;
};

union REG_ISP_LMAP_LMP_4 {
	uint32_t raw;
	struct {
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_LMAP_LMP_SET_SEL {
	uint32_t raw;
	struct {
		uint32_t SET_SEL                         : 1;
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
		uint32_t IMG_BAYERID                     : 4;
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
		uint32_t FORCE_DMA_DISABLE               : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DEBUG_DATA_SEL                  : 2;
	} bits;
};

union REG_ISP_RGBMAP_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_DEBUG_2                  : 32;
	} bits;
};

union REG_ISP_RGBMAP_3 {
	uint32_t raw;
	struct {
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_RGBMAP_SET_SEL {
	uint32_t raw;
	struct {
		uint32_t SET_SEL                         : 1;
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
		uint32_t _rsv_1                          : 27;
		uint32_t FORCE_CLK_ENABLE                : 1;
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
		uint32_t _rsv_14                         : 2;
		uint32_t WBG_IRGAIN                      : 14;
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
		uint32_t IMG_BAYERID                     : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
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

union REG_ISP_WBG_34 {
	uint32_t raw;
	struct {
		uint32_t RGAIN_FRACTION                  : 24;
	} bits;
};

union REG_ISP_WBG_38 {
	uint32_t raw;
	struct {
		uint32_t GGAIN_FRACTION                  : 24;
	} bits;
};

union REG_ISP_WBG_3C {
	uint32_t raw;
	struct {
		uint32_t BGAIN_FRACTION                  : 24;
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
		uint32_t CH_NM                           : 1;
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
		uint32_t DELAY                           : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t _rsv_9                          : 7;
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
		uint32_t DPC_MEM0_IMG0_ADDR              : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t DPC_MEM0_IMG1_ADDR              : 11;
		uint32_t _rsv_27                         : 3;
		uint32_t DPC_MEM0_IMG_ADDR_SEL           : 1;
		uint32_t DPC_MEM_PROG_MODE               : 1;
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
		uint32_t DPC_BP_MEM_ST_ADDR              : 11;
		uint32_t _rsv_11                         : 20;
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

union REG_ISP_DPC_24 {
	uint32_t raw;
	struct {
		uint32_t DPC_IR_BRIGHT_PIXEL_RATIO       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DPC_IR_DARK_PIXEL_RATIO         : 10;
	} bits;
};

union REG_ISP_DPC_25 {
	uint32_t raw;
	struct {
		uint32_t DPC_IR_DARK_PIXEL_MINDIFF       : 8;
		uint32_t DPC_IR_BRIGHT_PIXEL_UPBOUD_RATIO: 8;
		uint32_t DPC_FLAT_THRE_MIN_IR            : 8;
		uint32_t DPC_FLAT_THRE_IR                : 8;
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
		uint32_t _rsv_1                          : 15;
		uint32_t FRAME_DONE_SEL_TAIL0_ALL1       : 1;
	} bits;
};

union REG_RAW_TOP_UP_PQ_EN {
	uint32_t raw;
	struct {
		uint32_t UP_PQ_EN                        : 1;
		uint32_t _rsv_1                          : 15;
		uint32_t CHK_SUM_EN                      : 1;
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

union REG_RAW_TOP_RAW_BAYER_TYPE_TOPLEFT {
	uint32_t raw;
	struct {
		uint32_t BAYER_TYPE_TOPLEFT              : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t RGBIR_ENABLE                    : 1;
	} bits;
};

union REG_RAW_TOP_RDMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t RDMI_EN                         : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t PASS_SEL                        : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PASS_CNT_M1                     : 8;
		uint32_t CH_NUM                          : 1;
	} bits;
};

union REG_RAW_TOP_RDMA_SIZE {
	uint32_t raw;
	struct {
		uint32_t RDMI_WIDTHM1                    : 16;
		uint32_t RDMI_HEIGHTM1                   : 16;
	} bits;
};

union REG_RAW_TOP_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DPCM_MODE                       : 3;
		uint32_t _rsv_3                          : 5;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_RAW_TOP_STVALID_STATUS {
	uint32_t raw;
	struct {
		uint32_t STVALID_STATUS                  : 32;
	} bits;
};

union REG_RAW_TOP_STREADY_STATUS {
	uint32_t raw;
	struct {
		uint32_t STREADY_STATUS                  : 32;
	} bits;
};

union REG_RAW_TOP_PATGEN1 {
	uint32_t raw;
	struct {
		uint32_t X_CURSER                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t Y_CURSER                        : 14;
		uint32_t CURSER_EN                       : 1;
		uint32_t PG_ENABLE                       : 1;
	} bits;
};

union REG_RAW_TOP_PATGEN2 {
	uint32_t raw;
	struct {
		uint32_t CURSER_VALUE                    : 16;
	} bits;
};

union REG_RAW_TOP_PATGEN3 {
	uint32_t raw;
	struct {
		uint32_t VALUE_REPORT                    : 32;
	} bits;
};

union REG_RAW_TOP_PATGEN4 {
	uint32_t raw;
	struct {
		uint32_t XCNT_RPT                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t YCNT_RPT                        : 14;
		uint32_t PG_LCAC_ENABLE                  : 1;
		uint32_t CURSER_LCAC_EN                  : 1;
	} bits;
};

union REG_RAW_TOP_RO_IDLE {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_RO_IDLE                 : 32;
	} bits;
};

union REG_RAW_TOP_RO_DONE {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_RO_DONE                 : 32;
	} bits;
};

union REG_RAW_TOP_DMA_IDLE {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_DMA_IDLE                : 32;
	} bits;
};

union REG_RAW_TOP_LE_LMAP_GRID_NUMBER {
	uint32_t raw;
	struct {
		uint32_t LE_LMP_H_GRID_NUMM1             : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t LE_LMP_H_GRID_SIZE              : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t LE_LMP_V_GRID_NUMM1             : 10;
		uint32_t _rsv_26                         : 2;
		uint32_t LE_LMP_V_GRID_SIZE              : 3;
	} bits;
};

union REG_RAW_TOP_SE_LMAP_GRID_NUMBER {
	uint32_t raw;
	struct {
		uint32_t SE_LMP_H_GRID_NUMM1             : 10;
		uint32_t _rsv_10                         : 2;
		uint32_t SE_LMP_H_GRID_SIZE              : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t SE_LMP_V_GRID_NUMM1             : 10;
		uint32_t _rsv_26                         : 2;
		uint32_t SE_LMP_V_GRID_SIZE              : 3;
	} bits;
};

union REG_RAW_TOP_CHECKSUM_0 {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_CHECKSUM_0              : 32;
	} bits;
};

union REG_RAW_TOP_CHECKSUM_1 {
	uint32_t raw;
	struct {
		uint32_t RAW_TOP_CHECKSUM_1              : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGBCAC_CTRL {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_ENABLE                   : 1;
		uint32_t RGBCAC_OUT_SEL                  : 2;
		uint32_t RGBCAC_SHDW_SEL                 : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t SOFTRST                         : 1;
	} bits;
};

union REG_ISP_RGBCAC_PURPLE_TH {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_PURPLE_TH_LE             : 8;
		uint32_t RGBCAC_PURPLE_TH_SE             : 8;
		uint32_t RGBCAC_CORRECT_STRENGTH_LE      : 8;
		uint32_t RGBCAC_CORRECT_STRENGTH_SE      : 8;
	} bits;
};

union REG_ISP_RGBCAC_PURPLE_CBCR {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_PURPLE_CB                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_PURPLE_CR                : 12;
	} bits;
};

union REG_ISP_RGBCAC_PURPLE_CBCR2 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_PURPLE_CB2               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_PURPLE_CR2               : 12;
	} bits;
};

union REG_ISP_RGBCAC_PURPLE_CBCR3 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_PURPLE_CB3               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_PURPLE_CR3               : 12;
	} bits;
};

union REG_ISP_RGBCAC_GREEN_CBCR {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_GREEN_CB                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_GREEN_CR                 : 12;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_CORING {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_CORING              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_EDGE_SCALE               : 12;
	} bits;
};

union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_DEPURPLE_STR_RATIO_MIN_LE: 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_DEPURPLE_STR_RATIO_MIN_SE: 12;
	} bits;
};

union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_DEPURPLE_STR_RATIO_MAX_LE: 12;
		uint32_t _rsv_12                         : 4;
		uint32_t RGBCAC_DEPURPLE_STR_RATIO_MAX_SE: 12;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_WGT_LUT0 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_WGT_LUT_00          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_01          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_02          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_03          : 6;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_WGT_LUT1 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_WGT_LUT_04          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_05          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_06          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_07          : 6;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_WGT_LUT2 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_WGT_LUT_08          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_09          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_10          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_11          : 6;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_WGT_LUT3 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_WGT_LUT_12          : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_13          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_14          : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t RGBCAC_EDGE_WGT_LUT_15          : 6;
	} bits;
};

union REG_ISP_RGBCAC_EDGE_WGT_LUT4 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_EDGE_WGT_LUT_16          : 6;
	} bits;
};

union REG_ISP_RGBCAC_LUMA {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_LUMA_SCALE               : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t RGBCAC_LUMA2                    : 12;
	} bits;
};

union REG_ISP_RGBCAC_LUMA_BLEND {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_LUMA_BLEND_WGT           : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t RGBCAC_LUMA_BLEND_WGT2          : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t RGBCAC_LUMA_BLEND_WGT3          : 6;
	} bits;
};

union REG_ISP_RGBCAC_LUMA_FILTER0 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_LUMA_FILTER_00           : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_LUMA_FILTER_01           : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_LUMA_FILTER_02           : 7;
	} bits;
};

union REG_ISP_RGBCAC_LUMA_FILTER1 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_LUMA_FILTER_03           : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_LUMA_FILTER_04           : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_LUMA_FILTER_05           : 7;
	} bits;
};

union REG_ISP_RGBCAC_VAR_FILTER0 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_VAR_FILTER_00            : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_VAR_FILTER_01            : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_VAR_FILTER_02            : 7;
	} bits;
};

union REG_ISP_RGBCAC_VAR_FILTER1 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_VAR_FILTER_03            : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_VAR_FILTER_04            : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_VAR_FILTER_05            : 7;
	} bits;
};

union REG_ISP_RGBCAC_CHROMA_FILTER0 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_CHROMA_FILTER_00         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_CHROMA_FILTER_01         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_CHROMA_FILTER_02         : 7;
	} bits;
};

union REG_ISP_RGBCAC_CHROMA_FILTER1 {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_CHROMA_FILTER_03         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t RGBCAC_CHROMA_FILTER_04         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t RGBCAC_CHROMA_FILTER_05         : 7;
	} bits;
};

union REG_ISP_RGBCAC_CBCR_STR {
	uint32_t raw;
	struct {
		uint32_t RGBCAC_CB_STR_LE                : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t RGBCAC_CR_STR_LE                : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t RGBCAC_CB_STR_SE                : 5;
		uint32_t _rsv_21                         : 3;
		uint32_t RGBCAC_CR_STR_SE                : 5;
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
		uint32_t BNR_NS_LOW_OFFSET_R             : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_R    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_HIGH_OFFSET_R            : 10;
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
		uint32_t BNR_NS_LOW_OFFSET_GR            : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_GR   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_HIGH_OFFSET_GR           : 10;
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
		uint32_t BNR_NS_LOW_OFFSET_GB            : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_GB   {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_HIGH_OFFSET_GB           : 10;
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
		uint32_t BNR_NS_LOW_OFFSET_B             : 10;
	} bits;
};

union REG_ISP_BNR_NS_OFFSET1_B    {
	uint32_t raw;
	struct {
		uint32_t BNR_NS_HIGH_OFFSET_B            : 10;
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

union REG_ISP_BNR_INTENSITY_SEL {
	uint32_t raw;
	struct {
		uint32_t BNR_INTENSITY_SEL_00            : 5;
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
		uint32_t BNR_WEIGHT_NORM_1               : 7;
	} bits;
};

union REG_ISP_BNR_WEIGHT_NORM_2   {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_NORM_2               : 8;
	} bits;
};

union REG_ISP_BNR_VAR_TH          {
	uint32_t raw;
	struct {
		uint32_t BNR_VAR_TH                      : 8;
	} bits;
};

union REG_ISP_BNR_WEIGHT_LUT {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_LUT                  : 5;
	} bits;
};

union REG_ISP_BNR_WEIGHT_SM       {
	uint32_t raw;
	struct {
		uint32_t BNR_WEIGHT_SMOOTH               : 5;
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
		uint32_t BNR_FLAG_NEIGHBOR_MAX           : 1;
	} bits;
};

union REG_ISP_BNR_RES_K_SMOOTH    {
	uint32_t raw;
	struct {
		uint32_t BNR_RES_RATIO_K_SMOOTH          : 9;
	} bits;
};

union REG_ISP_BNR_RES_K_TEXTURE   {
	uint32_t raw;
	struct {
		uint32_t BNR_RES_RATIO_K_TEXTURE         : 9;
	} bits;
};

union REG_ISP_BNR_RES_MAX {
	uint32_t raw;
	struct {
		uint32_t BNR_RES_MAX                     : 12;
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
union REG_CA_00 {
	uint32_t raw;
	struct {
		uint32_t CACP_ENABLE                     : 1;
		uint32_t CACP_MODE                       : 1;
		uint32_t CACP_DBG_MODE                   : 1;
		uint32_t CACP_MEM_SW_MODE                : 1;
		uint32_t CACP_SHDW_READ_SEL              : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t _rsv_9                          : 7;
		uint32_t CACP_ISO_RATIO                  : 11;
	} bits;
};

union REG_CA_04 {
	uint32_t raw;
	struct {
		uint32_t CACP_MEM_D                      : 24;
		uint32_t _rsv_24                         : 7;
		uint32_t CACP_MEM_W                      : 1;
	} bits;
};

union REG_CA_08 {
	uint32_t raw;
	struct {
		uint32_t CACP_MEM_ST_ADDR                : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t CACP_MEM_ST_ADDR_W              : 1;
	} bits;
};

union REG_CA_0C {
	uint32_t raw;
	struct {
		uint32_t CACP_MEM_SW_RADDR               : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t CACP_MEM_SW_R                   : 1;
	} bits;
};

union REG_CA_10 {
	uint32_t raw;
	struct {
		uint32_t CACP_MEM_SW_RDATA_R             : 24;
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
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t SOFTRST                         : 1;
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
union REG_ISP_MMAP_00 {
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
		uint32_t INTER_1_EN                      : 1;
		uint32_t INTER_2_EN                      : 1;
		uint32_t INTER_3_EN                      : 1;
		uint32_t INTER_4_EN                      : 1;
		uint32_t DMA_SEL                         : 1;
		uint32_t RGBMAP_SW_CROP                  : 1;
	} bits;
};

union REG_ISP_MMAP_04 {
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
		uint32_t _rsv_27                         : 2;
		uint32_t FORCE_CLK_EN                    : 1;
		uint32_t REG_8BIT_RGBMAP_MODE            : 1;
		uint32_t WH_SW_MODE                      : 1;
	} bits;
};

union REG_ISP_MMAP_08 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_MAP_CORING               : 8;
		uint32_t MMAP_0_MAP_GAIN                 : 8;
		uint32_t MMAP_0_MAP_THD_L                : 8;
		uint32_t MMAP_0_MAP_THD_H                : 8;
	} bits;
};

union REG_ISP_MMAP_0C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_1      : 12;
	} bits;
};

union REG_ISP_MMAP_10 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_2      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_IN_3      : 12;
	} bits;
};

union REG_ISP_MMAP_14 {
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

union REG_ISP_MMAP_18 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_0   : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_1   : 11;
	} bits;
};

union REG_ISP_MMAP_1C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_LUMA_ADAPT_LUT_SLOPE_2   : 11;
		uint32_t _rsv_11                         : 1;
		uint32_t MMAP_0_MAP_DSHIFT_BIT           : 3;
	} bits;
};

union REG_ISP_MMAP_20 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_0       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_1       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_2       : 8;
		uint32_t MMAP_0_IIR_PRTCT_LUT_IN_3       : 8;
	} bits;
};

union REG_ISP_MMAP_24 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_0      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_1      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_2      : 4;
		uint32_t MMAP_0_IIR_PRTCT_LUT_OUT_3      : 4;
	} bits;
};

union REG_ISP_MMAP_28 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_0    : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_1    : 9;
	} bits;
};

union REG_ISP_MMAP_2C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_IIR_PRTCT_LUT_SLOPE_2    : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MMAP_0_MH_WGT                   : 4;
	} bits;
};

union REG_ISP_MMAP_30 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1_SW                  : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHTM1_SW                 : 14;
	} bits;
};

union REG_ISP_MMAP_34 {
	uint32_t raw;
	struct {
		uint32_t V_THD_L                         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t V_THD_H                         : 12;
	} bits;
};

union REG_ISP_MMAP_38 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_MAP_CORING               : 8;
	} bits;
};

union REG_ISP_MMAP_3C {
	uint32_t raw;
	struct {
		uint32_t V_WGT_SLP                       : 19;
		uint32_t _rsv_19                         : 5;
		uint32_t MOTION_LS_MODE                  : 1;
		uint32_t MOTION_LS_SEL                   : 1;
		uint32_t MOTION_YV_LS_MODE               : 1;
		uint32_t MOTION_YV_LS_SEL                : 1;
	} bits;
};

union REG_ISP_MMAP_40 {
	uint32_t raw;
	struct {
		uint32_t V_WGT_MAX                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t V_WGT_MIN                       : 9;
	} bits;
};

union REG_ISP_MMAP_44 {
	uint32_t raw;
	struct {
		uint32_t MMAP_MED_WGT                    : 9;
		uint32_t _rsv_9                          : 6;
		uint32_t MMAP_MED_ENABLE                 : 1;
	} bits;
};

union REG_ISP_MMAP_4C {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_1      : 12;
	} bits;
};

union REG_ISP_MMAP_50 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_2      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_IN_3      : 12;
	} bits;
};

union REG_ISP_MMAP_54 {
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

union REG_ISP_MMAP_58 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_0   : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_1   : 11;
	} bits;
};

union REG_ISP_MMAP_5C {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_LUMA_ADAPT_LUT_SLOPE_2   : 11;
	} bits;
};

union REG_ISP_MMAP_60 {
	uint32_t raw;
	struct {
		uint32_t RGBMAP_W_BIT                    : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t RGBMAP_H_BIT                    : 3;
	} bits;
};

union REG_ISP_MMAP_64 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_0                    : 32;
	} bits;
};

union REG_ISP_MMAP_68 {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA_1                    : 32;
	} bits;
};

union REG_ISP_MMAP_6C {
	uint32_t raw;
	struct {
		uint32_t SRAM_WADD                       : 7;
		uint32_t SRAM_WEN                        : 1;
		uint32_t FORCE_DMA_DISABLE               : 8;
		uint32_t MANR_DEBUG                      : 16;
	} bits;
};

union REG_ISP_MMAP_70 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_GAIN_RATIO_R             : 16;
		uint32_t MMAP_0_GAIN_RATIO_G             : 16;
	} bits;
};

union REG_ISP_MMAP_74 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_GAIN_RATIO_B             : 16;
	} bits;
};

union REG_ISP_MMAP_78 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_SLOPE_R               : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_0_NS_SLOPE_G               : 10;
	} bits;
};

union REG_ISP_MMAP_7C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_SLOPE_B               : 10;
	} bits;
};

union REG_ISP_MMAP_80 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LUMA_TH0_R            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LUMA_TH0_G            : 12;
	} bits;
};

union REG_ISP_MMAP_84 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LUMA_TH0_B            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LOW_OFFSET_R          : 12;
	} bits;
};

union REG_ISP_MMAP_88 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LOW_OFFSET_G          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LOW_OFFSET_B          : 12;
	} bits;
};

union REG_ISP_MMAP_8C {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_HIGH_OFFSET_R         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_HIGH_OFFSET_G         : 12;
	} bits;
};

union REG_ISP_MMAP_90 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_HIGH_OFFSET_B         : 12;
	} bits;
};

union REG_ISP_MMAP_A0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_GAIN_RATIO_R             : 16;
		uint32_t MMAP_1_GAIN_RATIO_G             : 16;
	} bits;
};

union REG_ISP_MMAP_A4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_GAIN_RATIO_B             : 16;
	} bits;
};

union REG_ISP_MMAP_A8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_SLOPE_R               : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_1_NS_SLOPE_G               : 10;
	} bits;
};

union REG_ISP_MMAP_AC {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_SLOPE_B               : 10;
	} bits;
};

union REG_ISP_MMAP_B0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LUMA_TH0_R            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LUMA_TH0_G            : 12;
	} bits;
};

union REG_ISP_MMAP_B4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LUMA_TH0_B            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LOW_OFFSET_R          : 12;
	} bits;
};

union REG_ISP_MMAP_B8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LOW_OFFSET_G          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LOW_OFFSET_B          : 12;
	} bits;
};

union REG_ISP_MMAP_BC {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_HIGH_OFFSET_R         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_HIGH_OFFSET_G         : 12;
	} bits;
};

union REG_ISP_MMAP_C0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_HIGH_OFFSET_B         : 12;
	} bits;
};

union REG_ISP_MMAP_C4 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_CROP                  : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHT_CROP                 : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE                     : 1;
	} bits;
};

union REG_ISP_MMAP_C8 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END                      : 14;
	} bits;
};

union REG_ISP_MMAP_CC {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END                      : 14;
	} bits;
};

union REG_ISP_MMAP_D0 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTH_CROP_SCALAR           : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHT_CROP_SCALAR          : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE_SCALAR              : 1;
	} bits;
};

union REG_ISP_MMAP_D4 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END_SCALAR               : 14;
	} bits;
};

union REG_ISP_MMAP_D8 {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END_SCALAR               : 14;
	} bits;
};

union REG_ISP_MMAP_DC {
	uint32_t raw;
	struct {
		uint32_t COEF_R                          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t COEF_G                          : 11;
	} bits;
};

union REG_ISP_MMAP_E0 {
	uint32_t raw;
	struct {
		uint32_t COEF_B                          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t COEF_I                          : 11;
	} bits;
};

union REG_ISP_MMAP_E4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_GAIN_RATIO_I             : 16;
		uint32_t MMAP_0_NS_SLOPE_I               : 10;
	} bits;
};

union REG_ISP_MMAP_E8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_LUMA_TH0_I            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_0_NS_LOW_OFFSET_I          : 12;
	} bits;
};

union REG_ISP_MMAP_EC {
	uint32_t raw;
	struct {
		uint32_t MMAP_0_NS_HIGH_OFFSET_I         : 12;
	} bits;
};

union REG_ISP_MMAP_F0 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_GAIN_RATIO_I             : 16;
		uint32_t MMAP_1_NS_SLOPE_I               : 10;
	} bits;
};

union REG_ISP_MMAP_F4 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_LUMA_TH0_I            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MMAP_1_NS_LOW_OFFSET_I          : 12;
	} bits;
};

union REG_ISP_MMAP_F8 {
	uint32_t raw;
	struct {
		uint32_t MMAP_1_NS_HIGH_OFFSET_I         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISTORY_SEL_0                   : 1;
		uint32_t HISTORY_SEL_1                   : 1;
		uint32_t HISTORY_SEL_2                   : 1;
		uint32_t HISTORY_SEL_3                   : 1;
	} bits;
};

union REG_ISP_MMAP_FC {
	uint32_t raw;
	struct {
		uint32_t MANR_STATUS                     : 28;
		uint32_t MANR_STATUS_MUX                 : 4;
	} bits;
};

union REG_ISP_MMAP_100 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_CENTERX                : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t MMAP_LSC_CENTERY                : 13;
	} bits;
};

union REG_ISP_MMAP_104 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_NORM                   : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t MMAP_LSC_DY_GAIN                : 8;
	} bits;
};

union REG_ISP_MMAP_108 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_01       : 10;
	} bits;
};

union REG_ISP_MMAP_10C {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_03       : 10;
	} bits;
};

union REG_ISP_MMAP_110 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_05       : 10;
	} bits;
};

union REG_ISP_MMAP_114 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_07       : 10;
	} bits;
};

union REG_ISP_MMAP_118 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_09       : 10;
	} bits;
};

union REG_ISP_MMAP_11C {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_11       : 10;
	} bits;
};

union REG_ISP_MMAP_120 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_13       : 10;
	} bits;
};

union REG_ISP_MMAP_124 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t MMAP_LSC_COMP_GAIN_LUT_15       : 10;
	} bits;
};

union REG_ISP_MMAP_128 {
	uint32_t raw;
	struct {
		uint32_t MMAP_LSC_COMP_GAIN_LUT_16       : 10;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_GAMMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ENABLE                    : 1;
		uint32_t GAMMA_SHDW_SEL                  : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_GAMMA_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_WSEL                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_RSEL                      : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t GAMMA_PROG_EN                   : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t GAMMA_PROG_1TO3_EN              : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t GAMMA_PROG_MODE                 : 2;
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

union REG_ISP_GAMMA_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MAX                       : 13;
	} bits;
};

union REG_ISP_GAMMA_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_SW_RADDR                  : 8;
		uint32_t _rsv_8                          : 4;
		uint32_t GAMMA_SW_R_MEM_SEL              : 1;
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
		uint32_t SOFTRST                         : 1;
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

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CLUT_CTRL {
	uint32_t raw;
	struct {
		uint32_t CLUT_ENABLE                     : 1;
		uint32_t CLUT_SHDW_SEL                   : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t PROG_EN                         : 1;
	} bits;
};

union REG_ISP_CLUT_PROG_ADDR {
	uint32_t raw;
	struct {
		uint32_t SRAM_R_IDX                      : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t SRAM_G_IDX                      : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t SRAM_B_IDX                      : 5;
	} bits;
};

union REG_ISP_CLUT_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t SRAM_WDATA                      : 30;
		uint32_t _rsv_30                         : 1;
		uint32_t SRAM_WR                         : 1;
	} bits;
};

union REG_ISP_CLUT_PROG_RDATA {
	uint32_t raw;
	struct {
		uint32_t SRAM_RDATA                      : 30;
		uint32_t _rsv_30                         : 1;
		uint32_t SRAM_RD                         : 1;
	} bits;
};

union REG_ISP_CLUT_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
	} bits;
};

union REG_ISP_CLUT_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_CLUT_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_CLUT_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_DEHAZE_DHZ_SMOOTH {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_W                        : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TH_SMOOTH                : 10;
	} bits;
};

union REG_ISP_DEHAZE_DHZ_SKIN {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SKIN_CB                  : 8;
		uint32_t DEHAZE_SKIN_CR                  : 8;
	} bits;
};

union REG_ISP_DEHAZE_DHZ_WGT {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_A_LUMA_WGT               : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t DEHAZE_BLEND_WGT                : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t DEHAZE_TMAP_SCALE               : 8;
		uint32_t DEHAZE_D_WGT                    : 5;
	} bits;
};

union REG_ISP_DEHAZE_DHZ_BYPASS {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_ENABLE                   : 1;
		uint32_t DEHAZE_LUMA_LUT_ENABLE          : 1;
		uint32_t DEHAZE_SKIN_LUT_ENABLE          : 1;
		uint32_t DEHAZE_SHDW_SEL                 : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_DEHAZE_0 {
	uint32_t raw;
	struct {
		uint32_t SOFTRST                         : 1;
		uint32_t _rsv_1                          : 4;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CHECK_SUM                       : 16;
	} bits;
};

union REG_ISP_DEHAZE_1 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_CUM_TH                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t DEHAZE_HIST_TH                  : 14;
	} bits;
};

union REG_ISP_DEHAZE_2 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SW_DC_TH                 : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t DEHAZE_SW_AGLOBAL_R             : 12;
		uint32_t DEHAZE_SW_DC_TRIG               : 1;
		uint32_t _rsv_29                         : 2;
		uint32_t DEHAZE_SW_DC_AGLOBAL_TRIG       : 1;
	} bits;
};

union REG_ISP_DEHAZE_28 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SW_AGLOBAL_G             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t DEHAZE_SW_AGLOBAL_B             : 12;
	} bits;
};

union REG_ISP_DEHAZE_2C {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_AGLOBAL_MAX              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t DEHAZE_AGLOBAL_MIN              : 12;
	} bits;
};

union REG_ISP_DEHAZE_3 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_MIN                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t DEHAZE_TMAP_MAX                 : 13;
	} bits;
};

union REG_ISP_DEHAZE_5 {
	uint32_t raw;
	struct {
		uint32_t FMT_ST                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FMT_END                         : 12;
		uint32_t TILE_NM                         : 4;
	} bits;
};

union REG_ISP_DEHAZE_6 {
	uint32_t raw;
	struct {
		uint32_t DBG_SEL                         : 3;
	} bits;
};

union REG_ISP_DEHAZE_7 {
	uint32_t raw;
	struct {
		uint32_t DHZ_DBG0                        : 32;
	} bits;
};

union REG_ISP_DEHAZE_8 {
	uint32_t raw;
	struct {
		uint32_t DHZ_DBG1                        : 32;
	} bits;
};

union REG_ISP_DEHAZE_9 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_LUMA_LUT00               : 8;
		uint32_t DEHAZE_LUMA_LUT01               : 8;
		uint32_t DEHAZE_LUMA_LUT02               : 8;
		uint32_t DEHAZE_LUMA_LUT03               : 8;
	} bits;
};

union REG_ISP_DEHAZE_10 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_LUMA_LUT04               : 8;
		uint32_t DEHAZE_LUMA_LUT05               : 8;
		uint32_t DEHAZE_LUMA_LUT06               : 8;
		uint32_t DEHAZE_LUMA_LUT07               : 8;
	} bits;
};

union REG_ISP_DEHAZE_11 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_LUMA_LUT08               : 8;
		uint32_t DEHAZE_LUMA_LUT09               : 8;
		uint32_t DEHAZE_LUMA_LUT10               : 8;
		uint32_t DEHAZE_LUMA_LUT11               : 8;
	} bits;
};

union REG_ISP_DEHAZE_12 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_LUMA_LUT12               : 8;
		uint32_t DEHAZE_LUMA_LUT13               : 8;
		uint32_t DEHAZE_LUMA_LUT14               : 8;
		uint32_t DEHAZE_LUMA_LUT15               : 8;
	} bits;
};

union REG_ISP_DEHAZE_17 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SKIN_LUT00               : 8;
		uint32_t DEHAZE_SKIN_LUT01               : 8;
		uint32_t DEHAZE_SKIN_LUT02               : 8;
		uint32_t DEHAZE_SKIN_LUT03               : 8;
	} bits;
};

union REG_ISP_DEHAZE_18 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SKIN_LUT04               : 8;
		uint32_t DEHAZE_SKIN_LUT05               : 8;
		uint32_t DEHAZE_SKIN_LUT06               : 8;
		uint32_t DEHAZE_SKIN_LUT07               : 8;
	} bits;
};

union REG_ISP_DEHAZE_19 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SKIN_LUT08               : 8;
		uint32_t DEHAZE_SKIN_LUT09               : 8;
		uint32_t DEHAZE_SKIN_LUT10               : 8;
		uint32_t DEHAZE_SKIN_LUT11               : 8;
	} bits;
};

union REG_ISP_DEHAZE_20 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_SKIN_LUT12               : 8;
		uint32_t DEHAZE_SKIN_LUT13               : 8;
		uint32_t DEHAZE_SKIN_LUT14               : 8;
		uint32_t DEHAZE_SKIN_LUT15               : 8;
	} bits;
};

union REG_ISP_DEHAZE_25 {
	uint32_t raw;
	struct {
		uint32_t AGLOBAL_R                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t AGLOBAL_G                       : 12;
	} bits;
};

union REG_ISP_DEHAZE_26 {
	uint32_t raw;
	struct {
		uint32_t AGLOBAL_B                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t DC_TH                           : 10;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_00 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT000         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT001         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT002         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT003         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_01 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT004         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT005         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT006         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT007         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_02 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT008         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT009         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT010         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT011         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_03 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT012         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT013         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT014         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT015         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_04 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT016         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT017         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT018         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT019         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_05 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT020         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT021         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT022         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT023         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_06 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT024         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT025         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT026         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT027         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_07 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT028         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT029         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT030         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT031         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_08 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT032         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT033         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT034         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT035         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_09 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT036         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT037         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT038         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT039         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_10 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT040         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT041         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT042         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT043         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_11 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT044         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT045         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT046         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT047         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_12 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT048         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT049         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT050         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT051         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_13 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT052         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT053         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT054         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT055         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_14 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT056         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT057         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT058         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT059         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_15 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT060         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT061         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT062         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT063         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_16 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT064         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT065         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT066         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT067         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_17 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT068         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT069         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT070         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT071         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_18 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT072         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT073         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT074         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT075         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_19 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT076         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT077         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT078         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT079         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_20 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT080         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT081         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT082         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT083         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_21 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT084         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT085         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT086         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT087         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_22 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT088         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT089         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT090         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT091         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_23 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT092         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT093         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT094         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT095         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_24 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT096         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT097         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT098         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT099         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_25 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT100         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT101         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT102         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT103         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_26 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT104         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT105         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT106         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT107         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_27 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT108         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT109         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT110         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT111         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_28 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT112         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT113         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT114         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT115         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_29 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT116         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT117         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT118         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT119         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_30 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT120         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT121         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT122         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT123         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_31 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT124         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT125         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT126         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t DEHAZE_TMAP_GAIN_LUT127         : 7;
	} bits;
};

union REG_ISP_DEHAZE_TMAP_32 {
	uint32_t raw;
	struct {
		uint32_t DEHAZE_TMAP_GAIN_LUT128         : 7;
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

union REG_ISP_CSC_4 {
	uint32_t raw;
	struct {
		uint32_t COEFF_00                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t COEFF_01                        : 14;
	} bits;
};

union REG_ISP_CSC_5 {
	uint32_t raw;
	struct {
		uint32_t COEFF_02                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t COEFF_10                        : 14;
	} bits;
};

union REG_ISP_CSC_6 {
	uint32_t raw;
	struct {
		uint32_t COEFF_11                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t COEFF_12                        : 14;
	} bits;
};

union REG_ISP_CSC_7 {
	uint32_t raw;
	struct {
		uint32_t COEFF_20                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t COEFF_21                        : 14;
	} bits;
};

union REG_ISP_CSC_8 {
	uint32_t raw;
	struct {
		uint32_t COEFF_22                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t OFFSET_0                        : 11;
	} bits;
};

union REG_ISP_CSC_9 {
	uint32_t raw;
	struct {
		uint32_t OFFSET_1                        : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t OFFSET_2                        : 11;
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
		uint32_t _rsv_6                          : 2;
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
		uint32_t _rsv_1                          : 3;
		uint32_t DCI_HIST_ENABLE                 : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t DCI_UV_GAIN_ENABLE              : 1;
		uint32_t _rsv_9                          : 19;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_DCI_MAP_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DCI_MAP_ENABLE                  : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DCI_PER1SAMPLE_ENABLE           : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t DCI_HISTO_BIG_ENDIAN            : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t DCI_ROI_ENABLE                  : 1;
		uint32_t _rsv_13                         : 7;
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

union REG_ISP_DCI_UV_GAIN_MAX {
	uint32_t raw;
	struct {
		uint32_t DCI_UV_GAIN_MAX                 : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t DCI_UV_GAIN_MIN                 : 7;
	} bits;
};

union REG_ISP_DCI_MAP_DBG {
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

union REG_ISP_DCI_BAYER_STARTING {
	uint32_t raw;
	struct {
		uint32_t DCI_BAYER_STARTING              : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
	} bits;
};

union REG_ISP_DCI_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
	} bits;
};

union REG_ISP_DCI_GAMMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t _rsv_1                          : 1;
		uint32_t GAMMA_SHDW_SEL                  : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_DCI_GAMMA_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_WSEL                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_RSEL                      : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t GAMMA_PROG_EN                   : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t GAMMA_PROG_1TO3_EN              : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t GAMMA_PROG_MODE                 : 2;
	} bits;
};

union REG_ISP_DCI_GAMMA_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_DATA_E                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_DATA_O                    : 12;
		uint32_t _rsv_28                         : 3;
		uint32_t GAMMA_W                         : 1;
	} bits;
};

union REG_ISP_DCI_GAMMA_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t GAMMA_MAX                       : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_HIST_EDGE_V_STATUS {
	uint32_t raw;
	struct {
		uint32_t STATUS                          : 32;
	} bits;
};

union REG_ISP_HIST_EDGE_V_SW_CTL {
	uint32_t raw;
	struct {
		uint32_t SW_RESET                        : 1;
		uint32_t CLR_SRAM                        : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SHAW_SEL                        : 1;
		uint32_t TILE_NM                         : 4;
	} bits;
};

union REG_ISP_HIST_EDGE_V_BYPASS {
	uint32_t raw;
	struct {
		uint32_t BYPASS                          : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_HIST_EDGE_V_IP_CONFIG {
	uint32_t raw;
	struct {
		uint32_t HIST_EDGE_V_ENABLE              : 1;
		uint32_t HIST_EDGE_V_LUMA_MODE           : 1;
	} bits;
};

union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETX {
	uint32_t raw;
	struct {
		uint32_t HIST_EDGE_V_OFFSETX             : 13;
	} bits;
};

union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETY {
	uint32_t raw;
	struct {
		uint32_t HIST_EDGE_V_OFFSETY             : 13;
	} bits;
};

union REG_ISP_HIST_EDGE_V_MONITOR {
	uint32_t raw;
	struct {
		uint32_t MONITOR                         : 32;
	} bits;
};

union REG_ISP_HIST_EDGE_V_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t MONITOR_SEL                     : 32;
	} bits;
};

union REG_ISP_HIST_EDGE_V_LOCATION {
	uint32_t raw;
	struct {
		uint32_t LOCATION                        : 32;
	} bits;
};

union REG_ISP_HIST_EDGE_V_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 16;
	} bits;
};

union REG_ISP_HIST_EDGE_V_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_FUSION_FS_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t FS_ENABLE                       : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t SE_IN_SEL                       : 1;
		uint32_t FORCE_PCLK_ENABLE               : 1;
		uint32_t FS_MC_ENABLE                    : 1;
		uint32_t FS_DC_MODE                      : 1;
		uint32_t FS_LUMA_MODE                    : 1;
		uint32_t FS_LMAP_GUIDE_DC_MODE           : 1;
		uint32_t FS_LMAP_GUIDE_LUMA_MODE         : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t FS_S_MAX                        : 20;
	} bits;
};

union REG_FUSION_FS_SE_GAIN {
	uint32_t raw;
	struct {
		uint32_t FS_LS_GAIN                      : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t FS_OUT_SEL                      : 4;
	} bits;
};

union REG_FUSION_FS_LUMA_THD {
	uint32_t raw;
	struct {
		uint32_t FS_LUMA_THD_L                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_LUMA_THD_H                   : 12;
	} bits;
};

union REG_FUSION_FS_WGT {
	uint32_t raw;
	struct {
		uint32_t FS_WGT_MAX                      : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t FS_WGT_MIN                      : 9;
	} bits;
};

union REG_FUSION_FS_WGT_SLOPE {
	uint32_t raw;
	struct {
		uint32_t FS_WGT_SLP                      : 19;
	} bits;
};

union REG_FUSION_FS_SHDW_READ_SEL {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_FUSION_FS_MOTION_LUT_IN {
	uint32_t raw;
	struct {
		uint32_t FS_MOTION_LUT_IN_0              : 8;
		uint32_t FS_MOTION_LUT_IN_1              : 8;
		uint32_t FS_MOTION_LUT_IN_2              : 8;
		uint32_t FS_MOTION_LUT_IN_3              : 8;
	} bits;
};

union REG_FUSION_FS_MOTION_LUT_OUT_0 {
	uint32_t raw;
	struct {
		uint32_t FS_MOTION_LUT_OUT_0             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t FS_MOTION_LUT_OUT_1             : 9;
	} bits;
};

union REG_FUSION_FS_MOTION_LUT_OUT_1 {
	uint32_t raw;
	struct {
		uint32_t FS_MOTION_LUT_OUT_2             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t FS_MOTION_LUT_OUT_3             : 9;
	} bits;
};

union REG_FUSION_FS_MOTION_LUT_SLOPE_0 {
	uint32_t raw;
	struct {
		uint32_t FS_MOTION_LUT_SLOPE_0           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_MOTION_LUT_SLOPE_1           : 12;
	} bits;
};

union REG_FUSION_FS_MOTION_LUT_SLOPE_1 {
	uint32_t raw;
	struct {
		uint32_t FS_MOTION_LUT_SLOPE_2           : 12;
	} bits;
};

union REG_FUSION_FS_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t LE_IN_SEL                       : 1;
		uint32_t _rsv_1                          : 5;
		uint32_t FS_FUSION_TYPE                  : 2;
		uint32_t _rsv_8                          : 8;
		uint32_t FS_FUSION_LWGT                  : 9;
	} bits;
};

union REG_FUSION_FS_CALIB_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t FS_CALIB_LUMA_LOW_TH            : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_CALIB_LUMA_HIGH_TH           : 12;
	} bits;
};

union REG_FUSION_FS_CALIB_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t FS_CALIB_DIF_TH                 : 12;
	} bits;
};

union REG_FUSION_FS_SE_FIX_OFFSET_0 {
	uint32_t raw;
	struct {
		uint32_t FS_SE_FIX_OFFSET_R              : 21;
	} bits;
};

union REG_FUSION_FS_SE_FIX_OFFSET_1 {
	uint32_t raw;
	struct {
		uint32_t FS_SE_FIX_OFFSET_G              : 21;
	} bits;
};

union REG_FUSION_FS_SE_FIX_OFFSET_2 {
	uint32_t raw;
	struct {
		uint32_t FS_SE_FIX_OFFSET_B              : 21;
	} bits;
};

union REG_FUSION_FS_CALIB_OUT_0 {
	uint32_t raw;
	struct {
		uint32_t FS_CAL_PXL_NUM                  : 20;
	} bits;
};

union REG_FUSION_FS_CALIB_OUT_1 {
	uint32_t raw;
	struct {
		uint32_t FS_PXL_DIFF_SUM_R               : 32;
	} bits;
};

union REG_FUSION_FS_CALIB_OUT_2 {
	uint32_t raw;
	struct {
		uint32_t FS_PXL_DIFF_SUM_G               : 32;
	} bits;
};

union REG_FUSION_FS_CALIB_OUT_3 {
	uint32_t raw;
	struct {
		uint32_t FS_PXL_DIFF_SUM_B               : 32;
	} bits;
};

union REG_FUSION_FS_LMAP_DARK_THD {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_DARK_THD_L        : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_LMAP_GUIDE_DARK_THD_H        : 12;
	} bits;
};

union REG_FUSION_FS_LMAP_DARK_WGT {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_DARK_WGT_L        : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t FS_LMAP_GUIDE_DARK_WGT_H        : 9;
	} bits;
};

union REG_FUSION_FS_LMAP_DARK_WGT_SLOPE {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_DARK_WGT_SLP      : 19;
	} bits;
};

union REG_FUSION_FS_LMAP_BRIT_THD {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_BRIT_THD_L        : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t FS_LMAP_GUIDE_BRIT_THD_H        : 12;
	} bits;
};

union REG_FUSION_FS_LMAP_BRIT_WGT {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_BRIT_WGT_L        : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t FS_LMAP_GUIDE_BRIT_WGT_H        : 9;
	} bits;
};

union REG_FUSION_FS_LMAP_BRIT_WGT_SLOPE {
	uint32_t raw;
	struct {
		uint32_t FS_LMAP_GUIDE_BRIT_WGT_SLP      : 19;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_LTM_H00 {
	uint32_t raw;
	struct {
		uint32_t LTM_ENABLE                      : 1;
		uint32_t LTM_DARK_ENH_ENABLE             : 1;
		uint32_t LTM_BRIT_ENH_ENABLE             : 1;
		uint32_t _rsv_3                          : 2;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_6                          : 4;
		uint32_t FORCE_PCLK_ENABLE               : 1;
		uint32_t _rsv_11                         : 2;
		uint32_t LTM_DBG_MODE                    : 3;
		uint32_t FORCE_DMA_DISABLE               : 2;
		uint32_t DARK_TONE_WGT_REFINE_ENABLE     : 1;
		uint32_t BRIT_TONE_WGT_REFINE_ENABLE     : 1;
		uint32_t _rsv_20                         : 11;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_LTM_H04 {
	uint32_t raw;
	struct {
		uint32_t IMG_WIDTHM1_SW                  : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_HEIGHTM1_SW                 : 14;
		uint32_t FIRST_FRAME_RESET               : 1;
		uint32_t WH_SW_MODE                      : 1;
	} bits;
};

union REG_LTM_H08 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 8;
		uint32_t LTM_BE_STRTH_DSHFT              : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t LTM_BE_STRTH_GAIN               : 11;
	} bits;
};

union REG_LTM_H0C {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 8;
		uint32_t LTM_DE_STRTH_DSHFT              : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t LTM_DE_STRTH_GAIN               : 11;
	} bits;
};

union REG_LTM_H14 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 16;
		uint32_t LTM_BE_RNG                      : 4;
		uint32_t _rsv_20                         : 4;
		uint32_t LTM_DE_RNG                      : 4;
	} bits;
};

union REG_LTM_H18 {
	uint32_t raw;
	struct {
		uint32_t LTM_BRI_IN_THD_L                : 8;
		uint32_t LTM_BRI_IN_THD_H                : 8;
		uint32_t _rsv_16                         : 7;
		uint32_t LTM_BRI_OUT_THD_L               : 9;
	} bits;
};

union REG_LTM_H1C {
	uint32_t raw;
	struct {
		uint32_t LTM_BRI_OUT_THD_H               : 9;
		uint32_t LTM_BRI_IN_GAIN_SLOP            : 17;
	} bits;
};

union REG_LTM_H20 {
	uint32_t raw;
	struct {
		uint32_t LTM_DAR_IN_THD_L                : 8;
		uint32_t LTM_DAR_IN_THD_H                : 8;
		uint32_t _rsv_16                         : 7;
		uint32_t LTM_DAR_OUT_THD_L               : 9;
	} bits;
};

union REG_LTM_H24 {
	uint32_t raw;
	struct {
		uint32_t LTM_DAR_OUT_THD_H               : 9;
		uint32_t LTM_DAR_IN_GAIN_SLOP            : 17;
	} bits;
};

union REG_LTM_H28 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 4;
		uint32_t LTM_B_CURVE_QUAN_BIT            : 4;
		uint32_t _rsv_8                          : 4;
		uint32_t LTM_G_CURVE_1_QUAN_BIT          : 4;
	} bits;
};

union REG_LTM_H2C {
	uint32_t raw;
	struct {
		uint32_t LTM_BE_DIST_WGT_00              : 5;
		uint32_t LTM_BE_DIST_WGT_01              : 5;
		uint32_t LTM_BE_DIST_WGT_02              : 5;
		uint32_t LTM_BE_DIST_WGT_03              : 5;
		uint32_t LTM_BE_DIST_WGT_04              : 5;
		uint32_t LTM_BE_DIST_WGT_05              : 5;
	} bits;
};

union REG_LTM_H30 {
	uint32_t raw;
	struct {
		uint32_t LTM_DE_DIST_WGT_00              : 5;
		uint32_t LTM_DE_DIST_WGT_01              : 5;
		uint32_t LTM_DE_DIST_WGT_02              : 5;
		uint32_t LTM_DE_DIST_WGT_03              : 5;
		uint32_t LTM_DE_DIST_WGT_04              : 5;
		uint32_t LTM_DE_DIST_WGT_05              : 5;
	} bits;
};

union REG_LTM_H34 {
	uint32_t raw;
	struct {
		uint32_t LUT_DBG_RADDR                   : 10;
		uint32_t _rsv_10                         : 4;
		uint32_t LUT_DBG_RSEL                    : 1;
		uint32_t LUT_DBG_READ_EN_1T              : 1;
		uint32_t LUT_PROG_EN_BRIGHT              : 1;
		uint32_t LUT_PROG_EN_DARK                : 1;
		uint32_t LUT_PROG_EN_GLOBAL              : 1;
	} bits;
};

union REG_LTM_H38 {
	uint32_t raw;
	struct {
		uint32_t LUT_WDATA                       : 32;
	} bits;
};

union REG_LTM_H3C {
	uint32_t raw;
	struct {
		uint32_t LUT_WSTADDR                     : 10;
		uint32_t _rsv_10                         : 4;
		uint32_t LUT_WSEL                        : 1;
		uint32_t LUT_WDATA_TRIG_1T               : 1;
		uint32_t LUT_WSTADDR_TRIG_1T             : 1;
	} bits;
};

union REG_LTM_H40 {
	uint32_t raw;
	struct {
		uint32_t BRIGHT_LUT_MAX                  : 16;
	} bits;
};

union REG_LTM_H44 {
	uint32_t raw;
	struct {
		uint32_t DARK_LUT_MAX                    : 16;
	} bits;
};

union REG_LTM_H48 {
	uint32_t raw;
	struct {
		uint32_t GLOBAL_LUT_MAX                  : 16;
	} bits;
};

union REG_LTM_H4C {
	uint32_t raw;
	struct {
		uint32_t LUT_DBG_RDATA                   : 32;
	} bits;
};

union REG_LTM_H50 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RW                        : 16;
		uint32_t DUMMY_RO                        : 16;
	} bits;
};

union REG_LTM_H54 {
	uint32_t raw;
	struct {
		uint32_t CROP_W_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_W_END_SCALAR               : 14;
		uint32_t _rsv_30                         : 1;
		uint32_t CROP_ENABLE_SCALAR              : 1;
	} bits;
};

union REG_LTM_H58 {
	uint32_t raw;
	struct {
		uint32_t CROP_H_STR_SCALAR               : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CROP_H_END_SCALAR               : 14;
	} bits;
};

union REG_LTM_H5C {
	uint32_t raw;
	struct {
		uint32_t IMG_HEIGHT_CROP_SCALAR          : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IMG_WIDTH_CROP_SCALAR           : 14;
	} bits;
};

union REG_LTM_H60 {
	uint32_t raw;
	struct {
		uint32_t HW_MEM_SEL                      : 1;
		uint32_t INTER_1_BYPASS                  : 1;
		uint32_t INTER_2_BYPASS                  : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t LMAP_ASSIGN                     : 1;
		uint32_t LMAP_DEBUG                      : 1;
		uint32_t PHASE_COMP                      : 1;
		uint32_t SCALER_PUSH_ON                  : 1;
		uint32_t _rsv_8                          : 16;
		uint32_t LMAP_DEBUG_VALUE                : 8;
	} bits;
};

union REG_LTM_H64 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_ENABLE                   : 1;
		uint32_t LTM_EE_TOTAL_GAIN               : 8;
		uint32_t LTM_EE_LUMA_GAIN_ENABLE         : 1;
		uint32_t LTM_EE_LUMA_MODE                : 1;
		uint32_t LTM_EE_SOFT_CLAMP_ENABLE        : 1;
	} bits;
};

union REG_LTM_H68 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_00         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_01         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_02         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_03         : 8;
	} bits;
};

union REG_LTM_H6C {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_04         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_05         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_06         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_07         : 8;
	} bits;
};

union REG_LTM_H70 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_08         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_09         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_10         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_11         : 8;
	} bits;
};

union REG_LTM_H74 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_12         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_13         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_14         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_15         : 8;
	} bits;
};

union REG_LTM_H78 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_16         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_17         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_18         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_19         : 8;
	} bits;
};

union REG_LTM_H7C {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_20         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_21         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_22         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_23         : 8;
	} bits;
};

union REG_LTM_H80 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_24         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_25         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_26         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_27         : 8;
	} bits;
};

union REG_LTM_H84 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_28         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_29         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_30         : 8;
		uint32_t LTM_EE_LUMA_GAIN_LUT_31         : 8;
	} bits;
};

union REG_LTM_H88 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_LUMA_GAIN_LUT_32         : 8;
	} bits;
};

union REG_LTM_H8C {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 12;
		uint32_t LMAP_W_BIT                      : 3;
		uint32_t _rsv_15                         : 13;
		uint32_t LMAP_H_BIT                      : 3;
	} bits;
};

union REG_LTM_H90 {
	uint32_t raw;
	struct {
		uint32_t LTM_V_BLEND_THD_L               : 10;
		uint32_t LTM_V_BLEND_THD_H               : 10;
	} bits;
};

union REG_LTM_H94 {
	uint32_t raw;
	struct {
		uint32_t LTM_V_BLEND_WGT_MIN             : 9;
		uint32_t LTM_V_BLEND_WGT_MAX             : 9;
	} bits;
};

union REG_LTM_H98 {
	uint32_t raw;
	struct {
		uint32_t LTM_V_BLEND_WGT_SLOPE           : 19;
	} bits;
};

union REG_LTM_H9C {
	uint32_t raw;
	struct {
		uint32_t LTM_DE_LMAP_LUT_IN_0            : 8;
		uint32_t LTM_DE_LMAP_LUT_IN_1            : 8;
		uint32_t LTM_DE_LMAP_LUT_IN_2            : 8;
		uint32_t LTM_DE_LMAP_LUT_IN_3            : 8;
	} bits;
};

union REG_LTM_HA0 {
	uint32_t raw;
	struct {
		uint32_t LTM_DE_LMAP_LUT_OUT_0           : 8;
		uint32_t LTM_DE_LMAP_LUT_OUT_1           : 8;
		uint32_t LTM_DE_LMAP_LUT_OUT_2           : 8;
		uint32_t LTM_DE_LMAP_LUT_OUT_3           : 8;
	} bits;
};

union REG_LTM_HA4 {
	uint32_t raw;
	struct {
		uint32_t LTM_DE_LMAP_LUT_SLOPE_0         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t LTM_DE_LMAP_LUT_SLOPE_1         : 12;
	} bits;
};

union REG_LTM_HA8 {
	uint32_t raw;
	struct {
		uint32_t LTM_DE_LMAP_LUT_SLOPE_2         : 12;
	} bits;
};

union REG_LTM_HAC {
	uint32_t raw;
	struct {
		uint32_t LTM_BE_LMAP_LUT_IN_0            : 8;
		uint32_t LTM_BE_LMAP_LUT_IN_1            : 8;
		uint32_t LTM_BE_LMAP_LUT_IN_2            : 8;
		uint32_t LTM_BE_LMAP_LUT_IN_3            : 8;
	} bits;
};

union REG_LTM_HB0 {
	uint32_t raw;
	struct {
		uint32_t LTM_BE_LMAP_LUT_OUT_0           : 8;
		uint32_t LTM_BE_LMAP_LUT_OUT_1           : 8;
		uint32_t LTM_BE_LMAP_LUT_OUT_2           : 8;
		uint32_t LTM_BE_LMAP_LUT_OUT_3           : 8;
	} bits;
};

union REG_LTM_HB4 {
	uint32_t raw;
	struct {
		uint32_t LTM_BE_LMAP_LUT_SLOPE_0         : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t LTM_BE_LMAP_LUT_SLOPE_1         : 12;
	} bits;
};

union REG_LTM_HB8 {
	uint32_t raw;
	struct {
		uint32_t LTM_BE_LMAP_LUT_SLOPE_2         : 12;
	} bits;
};

union REG_LTM_HBC {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_MOTION_LUT_IN_0          : 8;
		uint32_t LTM_EE_MOTION_LUT_IN_1          : 8;
		uint32_t LTM_EE_MOTION_LUT_IN_2          : 8;
		uint32_t LTM_EE_MOTION_LUT_IN_3          : 8;
	} bits;
};

union REG_LTM_HC0 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_MOTION_LUT_OUT_0         : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t LTM_EE_MOTION_LUT_OUT_1         : 9;
	} bits;
};

union REG_LTM_HC4 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_MOTION_LUT_OUT_2         : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t LTM_EE_MOTION_LUT_OUT_3         : 9;
	} bits;
};

union REG_LTM_HC8 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_MOTION_LUT_SLOPE_0       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t LTM_EE_MOTION_LUT_SLOPE_1       : 12;
	} bits;
};

union REG_LTM_HCC {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_MOTION_LUT_SLOPE_2       : 12;
	} bits;
};

union REG_LTM_HD0 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_TOTAL_OSHTTHRD           : 8;
		uint32_t LTM_EE_TOTAL_USHTTHRD           : 8;
		uint32_t LTM_EE_SHTCTRL_OSHTGAIN         : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t LTM_EE_SHTCTRL_USHTGAIN         : 6;
	} bits;
};

union REG_LTM_HD4 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_TOTAL_OSHTTHRD_CLP       : 8;
		uint32_t LTM_EE_TOTAL_USHTTHRD_CLP       : 8;
	} bits;
};

union REG_LTM_HD8 {
	uint32_t raw;
	struct {
		uint32_t LTM_EE_UPPER_BOUND_LEFT_DIFF    : 8;
		uint32_t LTM_EE_LOWER_BOUND_RIGHT_DIFF   : 8;
	} bits;
};

union REG_LTM_HDC {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 16;
		uint32_t LTM_EE_MIN_Y                    : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_CA_LITE_00 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_ENABLE                  : 1;
		uint32_t CA_LITE_SHDW_READ_SEL           : 1;
	} bits;
};

union REG_CA_LITE_04 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_IN_0                : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CA_LITE_LUT_IN_1                : 9;
	} bits;
};

union REG_CA_LITE_08 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_IN_2                : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CA_LITE_LUT_IN_3                : 9;
	} bits;
};

union REG_CA_LITE_0C {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_IN_4                : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CA_LITE_LUT_IN_5                : 9;
	} bits;
};

union REG_CA_LITE_10 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_OUT_0               : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t CA_LITE_LUT_OUT_1               : 11;
	} bits;
};

union REG_CA_LITE_14 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_OUT_2               : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t CA_LITE_LUT_OUT_3               : 11;
	} bits;
};

union REG_CA_LITE_18 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_OUT_4               : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t CA_LITE_LUT_OUT_5               : 11;
	} bits;
};

union REG_CA_LITE_1C {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_SLP_0               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CA_LITE_LUT_SLP_1               : 12;
	} bits;
};

union REG_CA_LITE_20 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_SLP_2               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CA_LITE_LUT_SLP_3               : 12;
	} bits;
};

union REG_CA_LITE_24 {
	uint32_t raw;
	struct {
		uint32_t CA_LITE_LUT_SLP_4               : 12;
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
		uint32_t _rsv_6                          : 1;
		uint32_t FORCE_CLK_EN                    : 1;
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
		uint32_t TDNR_ENABLE                     : 1;
		uint32_t DMA_CROP_ENABLE                 : 1;
		uint32_t FORCE_MONO_ENABLE               : 1;
		uint32_t DEBUG_STATUS_EN                 : 1;
		uint32_t REG_3DNR_COMP_GAIN_ENABLE       : 1;
		uint32_t REG_3DNR_LUMAREF_LPF_ENABLE     : 1;
	} bits;
};

union REG_ISP_444_422_6 {
	uint32_t raw;
	struct {
		uint32_t TDNR_DEBUG_STATUS               : 32;
	} bits;
};

union REG_ISP_444_422_8 {
	uint32_t raw;
	struct {
		uint32_t GUARD_CNT                       : 8;
		uint32_t FORCE_DMA_DISABLE               : 6;
		uint32_t UV_ROUNDING_TYPE_SEL            : 1;
		uint32_t TDNR_PIXEL_LP                   : 1;
		uint32_t TDNR_DEBUG_SEL                  : 16;
	} bits;
};

union REG_ISP_444_422_9 {
	uint32_t raw;
	struct {
		uint32_t DMA_WRITE_SEL_Y                 : 1;
		uint32_t DMA_WRITE_SEL_C                 : 1;
		uint32_t DMA_SEL                         : 1;
		uint32_t _rsv_3                          : 2;
		uint32_t AVG_MODE_READ                   : 1;
		uint32_t AVG_MODE_WRITE                  : 1;
		uint32_t DROP_MODE_WRITE                 : 1;
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
		uint32_t REG_3DNR_Y_LUT_IN_0             : 8;
		uint32_t REG_3DNR_Y_LUT_IN_1             : 8;
		uint32_t REG_3DNR_Y_LUT_IN_2             : 8;
		uint32_t REG_3DNR_Y_LUT_IN_3             : 8;
	} bits;
};

union REG_ISP_444_422_14 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_LUT_OUT_0            : 8;
		uint32_t REG_3DNR_Y_LUT_OUT_1            : 8;
		uint32_t REG_3DNR_Y_LUT_OUT_2            : 8;
		uint32_t REG_3DNR_Y_LUT_OUT_3            : 8;
	} bits;
};

union REG_ISP_444_422_15 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_LUT_SLOPE_0          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_Y_LUT_SLOPE_1          : 12;
	} bits;
};

union REG_ISP_444_422_16 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_LUT_SLOPE_2          : 12;
		uint32_t _rsv_12                         : 3;
		uint32_t MOTION_SEL                      : 1;
		uint32_t REG_3DNR_Y_BETA_MAX             : 8;
	} bits;
};

union REG_ISP_444_422_17 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_U_LUT_IN_0             : 8;
		uint32_t REG_3DNR_U_LUT_IN_1             : 8;
		uint32_t REG_3DNR_U_LUT_IN_2             : 8;
		uint32_t REG_3DNR_U_LUT_IN_3             : 8;
	} bits;
};

union REG_ISP_444_422_18 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_U_LUT_OUT_0            : 8;
		uint32_t REG_3DNR_U_LUT_OUT_1            : 8;
		uint32_t REG_3DNR_U_LUT_OUT_2            : 8;
		uint32_t REG_3DNR_U_LUT_OUT_3            : 8;
	} bits;
};

union REG_ISP_444_422_19 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_U_LUT_SLOPE_0          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_U_LUT_SLOPE_1          : 12;
	} bits;
};

union REG_ISP_444_422_20 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_U_LUT_SLOPE_2          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_U_BETA_MAX             : 8;
	} bits;
};

union REG_ISP_444_422_21 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_V_LUT_IN_0             : 8;
		uint32_t REG_3DNR_V_LUT_IN_1             : 8;
		uint32_t REG_3DNR_V_LUT_IN_2             : 8;
		uint32_t REG_3DNR_V_LUT_IN_3             : 8;
	} bits;
};

union REG_ISP_444_422_22 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_V_LUT_OUT_0            : 8;
		uint32_t REG_3DNR_V_LUT_OUT_1            : 8;
		uint32_t REG_3DNR_V_LUT_OUT_2            : 8;
		uint32_t REG_3DNR_V_LUT_OUT_3            : 8;
	} bits;
};

union REG_ISP_444_422_23 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_V_LUT_SLOPE_0          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_V_LUT_SLOPE_1          : 12;
	} bits;
};

union REG_ISP_444_422_24 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_V_LUT_SLOPE_2          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_V_BETA_MAX             : 8;
	} bits;
};

union REG_ISP_444_422_25 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_Y_LUT_IN_0      : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_IN_1      : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_IN_2      : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_IN_3      : 8;
	} bits;
};

union REG_ISP_444_422_26 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_Y_LUT_OUT_0     : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_OUT_1     : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_OUT_2     : 8;
		uint32_t REG_3DNR_MOTION_Y_LUT_OUT_3     : 8;
	} bits;
};

union REG_ISP_444_422_27 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_Y_LUT_SLOPE_0   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_MOTION_Y_LUT_SLOPE_1   : 12;
	} bits;
};

union REG_ISP_444_422_28 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_Y_LUT_SLOPE_2   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_MOTION_C_LUT_SLOPE_0   : 12;
	} bits;
};

union REG_ISP_444_422_29 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_C_LUT_SLOPE_1   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_MOTION_C_LUT_SLOPE_2   : 12;
	} bits;
};

union REG_ISP_444_422_30 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_C_LUT_IN_0      : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_IN_1      : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_IN_2      : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_IN_3      : 8;
	} bits;
};

union REG_ISP_444_422_31 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_MOTION_C_LUT_OUT_0     : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_OUT_1     : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_OUT_2     : 8;
		uint32_t REG_3DNR_MOTION_C_LUT_OUT_3     : 8;
	} bits;
};

union REG_ISP_444_422_80 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_EE_COMP_GAIN           : 9;
	} bits;
};

union REG_ISP_444_422_84 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_00  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_01  : 10;
	} bits;
};

union REG_ISP_444_422_88 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_02  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_03  : 10;
	} bits;
};

union REG_ISP_444_422_8C {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_04  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_05  : 10;
	} bits;
};

union REG_ISP_444_422_90 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_06  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_07  : 10;
	} bits;
};

union REG_ISP_444_422_94 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_08  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_09  : 10;
	} bits;
};

union REG_ISP_444_422_98 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_10  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_11  : 10;
	} bits;
};

union REG_ISP_444_422_9C {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_12  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_13  : 10;
	} bits;
};

union REG_ISP_444_422_A0 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_14  : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_15  : 10;
	} bits;
};

union REG_ISP_444_422_A4 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LUMA_COMP_GAIN_LUT_16  : 10;
	} bits;
};

union REG_ISP_444_422_A8 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_CENTERX            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t REG_3DNR_LSC_CENTERY            : 13;
	} bits;
};

union REG_ISP_444_422_AC {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_NORM               : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t REG_3DNR_LSC_DY_GAIN            : 8;
	} bits;
};

union REG_ISP_444_422_B0 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_00   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_01   : 10;
	} bits;
};

union REG_ISP_444_422_B4 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_02   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_03   : 10;
	} bits;
};

union REG_ISP_444_422_B8 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_04   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_05   : 10;
	} bits;
};

union REG_ISP_444_422_BC {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_06   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_07   : 10;
	} bits;
};

union REG_ISP_444_422_C0 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_08   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_09   : 10;
	} bits;
};

union REG_ISP_444_422_C4 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_10   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_11   : 10;
	} bits;
};

union REG_ISP_444_422_C8 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_12   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_13   : 10;
	} bits;
};

union REG_ISP_444_422_CC {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_14   : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_15   : 10;
	} bits;
};

union REG_ISP_444_422_D0 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_LSC_COMP_GAIN_LUT_16   : 10;
	} bits;
};

union REG_ISP_444_422_D4 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_GAIN_LUT_IN_0        : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_IN_1        : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_IN_2        : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_IN_3        : 8;
	} bits;
};

union REG_ISP_444_422_D8 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_GAIN_LUT_OUT_0       : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_OUT_1       : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_OUT_2       : 8;
		uint32_t REG_3DNR_Y_GAIN_LUT_OUT_3       : 8;
	} bits;
};

union REG_ISP_444_422_DC {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_GAIN_LUT_SLOPE_0     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_Y_GAIN_LUT_SLOPE_1     : 12;
	} bits;
};

union REG_ISP_444_422_E0 {
	uint32_t raw;
	struct {
		uint32_t REG_3DNR_Y_GAIN_LUT_SLOPE_2     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t REG_3DNR_Y_MOTION_MAX           : 8;
		uint32_t REG_3DNR_C_MOTION_MAX           : 8;
	} bits;
};

union REG_ISP_444_422_E4 {
	uint32_t raw;
	struct {
		uint32_t MOT_DEBUG_LUT_IN_0              : 8;
		uint32_t MOT_DEBUG_LUT_IN_1              : 8;
		uint32_t MOT_DEBUG_LUT_IN_2              : 8;
		uint32_t MOT_DEBUG_LUT_IN_3              : 8;
	} bits;
};

union REG_ISP_444_422_E8 {
	uint32_t raw;
	struct {
		uint32_t MOT_DEBUG_LUT_OUT_0             : 8;
		uint32_t MOT_DEBUG_LUT_OUT_1             : 8;
		uint32_t MOT_DEBUG_LUT_OUT_2             : 8;
		uint32_t MOT_DEBUG_LUT_OUT_3             : 8;
	} bits;
};

union REG_ISP_444_422_EC {
	uint32_t raw;
	struct {
		uint32_t MOT_DEBUG_LUT_SLOPE_0           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t MOT_DEBUG_LUT_SLOPE_1           : 12;
	} bits;
};

union REG_ISP_444_422_F0 {
	uint32_t raw;
	struct {
		uint32_t MOT_DEBUG_LUT_SLOPE_2           : 12;
		uint32_t MOT_DEBUG_SWITCH                : 1;
		uint32_t REG_3DNR_Y_PIX_GAIN_ENABLE      : 1;
		uint32_t REG_3DNR_C_PIX_GAIN_ENABLE      : 1;
		uint32_t REG_3DNR_PIX_GAIN_MODE          : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_FBCE_00 {
	uint32_t raw;
	struct {
		uint32_t FBCE_EN                         : 1;
		uint32_t SHD_RD                          : 1;
		uint32_t FORCE_CKE                       : 1;
		uint32_t _rsv_3                          : 5;
		uint32_t DEBUG                           : 16;
		uint32_t DUMMY                           : 8;
	} bits;
};

union REG_FBCE_10 {
	uint32_t raw;
	struct {
		uint32_t Y_LOSSLESS                      : 1;
		uint32_t Y_BASE_QDPCM_Q                  : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t Y_BASE_PCM_BD_MINUS2            : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t Y_DEFAULT_GR_K                  : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t Y_CPLX_SHIFT                    : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t Y_PEN_POS_SHIFT                 : 3;
	} bits;
};

union REG_FBCE_14 {
	uint32_t raw;
	struct {
		uint32_t Y_MIN_CU_BIT                    : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t Y_MAX_CU_BIT                    : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t Y_TOTAL_LINE_BIT_BUDGET         : 15;
	} bits;
};

union REG_FBCE_18 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 6;
		uint32_t Y_BIT_STREAM_SIZE               : 18;
	} bits;
};

union REG_FBCE_20 {
	uint32_t raw;
	struct {
		uint32_t C_LOSSLESS                      : 1;
		uint32_t C_BASE_QDPCM_Q                  : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t C_BASE_PCM_BD_MINUS2            : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t C_DEFAULT_GR_K                  : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t C_CPLX_SHIFT                    : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t C_PEN_POS_SHIFT                 : 3;
	} bits;
};

union REG_FBCE_24 {
	uint32_t raw;
	struct {
		uint32_t C_MIN_CU_BIT                    : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t C_MAX_CU_BIT                    : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t C_TOTAL_LINE_BIT_BUDGET         : 15;
	} bits;
};

union REG_FBCE_28 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 6;
		uint32_t C_BIT_STREAM_SIZE               : 18;
	} bits;
};

union REG_FBCE_2C {
	uint32_t raw;
	struct {
		uint32_t Y_TOTAL_FIRST_LINE_BIT_BUDGET   : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t C_TOTAL_FIRST_LINE_BIT_BUDGET   : 15;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_FBCD_00 {
	uint32_t raw;
	struct {
		uint32_t FBCD_EN                         : 1;
		uint32_t FF_DROP                         : 1;
		uint32_t FBCD_AUTO                       : 1;
		uint32_t FORCE_CKE                       : 1;
	} bits;
};

union REG_FBCD_0C {
	uint32_t raw;
	struct {
		uint32_t Y_BIT_STREAM_SIZE               : 22;
		uint32_t _rsv_22                         : 3;
		uint32_t Y_STREAM_TAG                    : 3;
	} bits;
};

union REG_FBCD_10 {
	uint32_t raw;
	struct {
		uint32_t C_BIT_STREAM_SIZE               : 22;
		uint32_t _rsv_22                         : 3;
		uint32_t C_STREAM_TAG                    : 3;
	} bits;
};

union REG_FBCD_14 {
	uint32_t raw;
	struct {
		uint32_t DEBUG_SEL                       : 4;
	} bits;
};

union REG_FBCD_20 {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 8;
		uint32_t SHD_RD                          : 1;
	} bits;
};

union REG_FBCD_24 {
	uint32_t raw;
	struct {
		uint32_t Y_LOSSLESS                      : 1;
		uint32_t Y_BASE_QDPCM_Q                  : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t Y_BASE_PCM_BD_MINUS2            : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t Y_DEFAULT_GR_K                  : 3;
	} bits;
};

union REG_FBCD_28 {
	uint32_t raw;
	struct {
		uint32_t C_LOSSLESS                      : 1;
		uint32_t C_BASE_QDPCM_Q                  : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t C_BASE_PCM_BD_MINUS2            : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t C_DEFAULT_GR_K                  : 3;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_YUV_DITHER_Y_DITHER {
	uint32_t raw;
	struct {
		uint32_t Y_DITHER_ENABLE                 : 1;
		uint32_t Y_DITHER_MOD_ENABLE             : 1;
		uint32_t Y_DITHER_HISTIDX_ENABLE         : 1;
		uint32_t Y_DITHER_FMNUM_ENABLE           : 1;
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
		uint32_t UV_DITHER_ENABLE                : 1;
		uint32_t UV_DITHER_MOD_ENABLE            : 1;
		uint32_t UV_DITHER_HISTIDX_ENABLE        : 1;
		uint32_t UV_DITHER_FMNUM_ENABLE          : 1;
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

union REG_ISP_YNR_NS0_LUMA_TH_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_00              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_01              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_02              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_03              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_04              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_LUMA_TH_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_LUMA_TH_05              : 8;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE_00       {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE_00                : 11;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE_01                : 11;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE_02                : 11;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE_03                : 11;
	} bits;
};

union REG_ISP_YNR_NS0_SLOPE_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_SLOPE_04                : 11;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_00               : 8;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_01               : 8;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_02               : 8;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_03               : 8;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_04               : 8;
	} bits;
};

union REG_ISP_YNR_NS0_OFFSET_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS0_OFFSET_05               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_00              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_01              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_02              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_03              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_04              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_LUMA_TH_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_LUMA_TH_05              : 8;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE_00                : 11;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE_01                : 11;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE_02                : 11;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE_03                : 11;
	} bits;
};

union REG_ISP_YNR_NS1_SLOPE_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_SLOPE_04                : 11;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_00               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_01               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_02               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_03               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_04               : 8;
	} bits;
};

union REG_ISP_YNR_NS1_OFFSET_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_NS1_OFFSET_05               : 8;
	} bits;
};

union REG_ISP_YNR_NS_GAIN         {
	uint32_t raw;
	struct {
		uint32_t YNR_NS_GAIN                     : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_00               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_01               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_02               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_03               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_04               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_05               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_06 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_06               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_07 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_07               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_08 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_08               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_09 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_09               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_10 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_10               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_11 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_11               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_12               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_13 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_13               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_14 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_14               : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_LUT_15 {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_LUT_15               : 8;
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
		uint32_t YNR_WEIGHT_NORM_2               : 8;
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

union REG_ISP_YNR_WEIGHT_SM       {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_SMOOTH               : 5;
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
		uint32_t YNR_FLAG_NEIGHBOR_MAX_WEIGHT    : 1;
	} bits;
};

union REG_ISP_YNR_RES_K_SMOOTH    {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_RATIO_K_SMOOTH          : 9;
	} bits;
};

union REG_ISP_YNR_RES_K_TEXTURE   {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_RATIO_K_TEXTURE         : 9;
	} bits;
};

union REG_ISP_YNR_FILTER_MODE_EN {
	uint32_t raw;
	struct {
		uint32_t YNR_FILTER_MODE_ENABLE          : 1;
	} bits;
};

union REG_ISP_YNR_FILTER_MODE_ALPHA {
	uint32_t raw;
	struct {
		uint32_t YNR_FILTER_MODE_ALPHA           : 9;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_00 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_00              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_01 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_01              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_02 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_02              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_03 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_03              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_04 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_04              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_05 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_05              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_06 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_06              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_07 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_07              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_08 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_08              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_09 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_09              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_10 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_10              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_11 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_11              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_12              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_13 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_13              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_14 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_14              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOT_LUT_15 {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOT_LUT_15              : 8;
	} bits;
};

union REG_ISP_YNR_RES_MAX {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MAX                     : 8;
	} bits;
};

union REG_ISP_YNR_RES_MOTION_MAX {
	uint32_t raw;
	struct {
		uint32_t YNR_RES_MOTION_MAX              : 8;
	} bits;
};

union REG_ISP_YNR_MOTION_NS_CLIP_MAX {
	uint32_t raw;
	struct {
		uint32_t YNR_MOTION_NS_CLIP_MAX          : 8;
	} bits;
};

union REG_ISP_YNR_WEIGHT_LUT      {
	uint32_t raw;
	struct {
		uint32_t YNR_WEIGHT_LUT                  : 5;
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
union REG_ISP_CNR_ENABLE {
	uint32_t raw;
	struct {
		uint32_t CNR_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t PFC_ENABLE                      : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CNR_DIFF_SHIFT_VAL              : 8;
		uint32_t CNR_RATIO                       : 8;
		uint32_t CNR_OUT_SEL                     : 2;
	} bits;
};

union REG_ISP_CNR_STRENGTH_MODE {
	uint32_t raw;
	struct {
		uint32_t CNR_STRENGTH_MODE               : 8;
		uint32_t CNR_FUSION_INTENSITY_WEIGHT     : 4;
		uint32_t _rsv_12                         : 4;
		uint32_t CNR_WEIGHT_INTER_SEL            : 4;
		uint32_t CNR_VAR_TH                      : 9;
		uint32_t _rsv_29                         : 1;
		uint32_t CNR_FLAG_NEIGHBOR_MAX_WEIGHT    : 1;
		uint32_t CNR_SHDW_SEL                    : 1;
	} bits;
};

union REG_ISP_CNR_PURPLE_TH {
	uint32_t raw;
	struct {
		uint32_t CNR_PURPLE_TH                   : 8;
		uint32_t CNR_CORRECT_STRENGTH            : 8;
		uint32_t CNR_DIFF_GAIN                   : 4;
		uint32_t _rsv_20                         : 4;
		uint32_t CNR_MOTION_ENABLE               : 1;
	} bits;
};

union REG_ISP_CNR_PURPLE_CB {
	uint32_t raw;
	struct {
		uint32_t CNR_PURPLE_CB                   : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CNR_PURPLE_CR                   : 8;
	} bits;
};

union REG_ISP_CNR_GREEN_CB {
	uint32_t raw;
	struct {
		uint32_t CNR_GREEN_CB                    : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t CNR_GREEN_CR                    : 8;
	} bits;
};

union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_00 {
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

union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_04 {
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

union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_08 {
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

union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_12 {
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

union REG_ISP_CNR_MOTION_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t CNR_MOTION_LUT_0                : 8;
		uint32_t CNR_MOTION_LUT_1                : 8;
		uint32_t CNR_MOTION_LUT_2                : 8;
		uint32_t CNR_MOTION_LUT_3                : 8;
	} bits;
};

union REG_ISP_CNR_MOTION_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t CNR_MOTION_LUT_4                : 8;
		uint32_t CNR_MOTION_LUT_5                : 8;
		uint32_t CNR_MOTION_LUT_6                : 8;
		uint32_t CNR_MOTION_LUT_7                : 8;
	} bits;
};

union REG_ISP_CNR_MOTION_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t CNR_MOTION_LUT_8                : 8;
		uint32_t CNR_MOTION_LUT_9                : 8;
		uint32_t CNR_MOTION_LUT_10               : 8;
		uint32_t CNR_MOTION_LUT_11               : 8;
	} bits;
};

union REG_ISP_CNR_MOTION_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t CNR_MOTION_LUT_12               : 8;
		uint32_t CNR_MOTION_LUT_13               : 8;
		uint32_t CNR_MOTION_LUT_14               : 8;
		uint32_t CNR_MOTION_LUT_15               : 8;
	} bits;
};

union REG_ISP_CNR_PURPLE_CB2 {
	uint32_t raw;
	struct {
		uint32_t CNR_PURPLE_CB2                  : 8;
		uint32_t CNR_PURPLE_CR2                  : 8;
		uint32_t CNR_PURPLE_CB3                  : 8;
		uint32_t CNR_PURPLE_CR3                  : 8;
	} bits;
};

union REG_ISP_CNR_MASK {
	uint32_t raw;
	struct {
		uint32_t CNR_MASK                        : 8;
	} bits;
};

union REG_ISP_CNR_DUMMY {
	uint32_t raw;
	struct {
		uint32_t CNR_DUMMY                       : 32;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE                  : 8;
		uint32_t CNR_EDGE_CORING                 : 8;
		uint32_t CNR_EDGE_MIN                    : 8;
		uint32_t CNR_EDGE_MAX                    : 8;
	} bits;
};

union REG_ISP_CNR_EDGE_RATIO_SPEED {
	uint32_t raw;
	struct {
		uint32_t CNR_RATIO_SPEED                 : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t CNR_CB_STR                      : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t CNR_CR_STR                      : 5;
	} bits;
};

union REG_ISP_CNR_DEPURPLE_WEIGHT_TH {
	uint32_t raw;
	struct {
		uint32_t CNR_DEPURPLE_WEIGHT_TH          : 8;
		uint32_t CNR_DEPURPLE_STR_MIN_RATIO      : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t CNR_DEPURPLE_STR_MAX_RATIO      : 7;
	} bits;
};

union REG_ISP_CNR_CORING_MOTION_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t CNR_CORING_MOTION_LUT_00        : 8;
		uint32_t CNR_CORING_MOTION_LUT_01        : 8;
		uint32_t CNR_CORING_MOTION_LUT_02        : 8;
		uint32_t CNR_CORING_MOTION_LUT_03        : 8;
	} bits;
};

union REG_ISP_CNR_CORING_MOTION_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t CNR_CORING_MOTION_LUT_04        : 8;
		uint32_t CNR_CORING_MOTION_LUT_05        : 8;
		uint32_t CNR_CORING_MOTION_LUT_06        : 8;
		uint32_t CNR_CORING_MOTION_LUT_07        : 8;
	} bits;
};

union REG_ISP_CNR_CORING_MOTION_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t CNR_CORING_MOTION_LUT_08        : 8;
		uint32_t CNR_CORING_MOTION_LUT_09        : 8;
		uint32_t CNR_CORING_MOTION_LUT_10        : 8;
		uint32_t CNR_CORING_MOTION_LUT_11        : 8;
	} bits;
};

union REG_ISP_CNR_CORING_MOTION_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t CNR_CORING_MOTION_LUT_12        : 8;
		uint32_t CNR_CORING_MOTION_LUT_13        : 8;
		uint32_t CNR_CORING_MOTION_LUT_14        : 8;
		uint32_t CNR_CORING_MOTION_LUT_15        : 8;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE_LUT_0 {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE_LUT_00           : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CNR_EDGE_SCALE_LUT_01           : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_02           : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_03           : 6;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE_LUT_4 {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE_LUT_04           : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CNR_EDGE_SCALE_LUT_05           : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_06           : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_07           : 6;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE_LUT_8 {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE_LUT_08           : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CNR_EDGE_SCALE_LUT_09           : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_10           : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_11           : 6;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE_LUT_12           : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CNR_EDGE_SCALE_LUT_13           : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_14           : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t CNR_EDGE_SCALE_LUT_15           : 6;
	} bits;
};

union REG_ISP_CNR_EDGE_SCALE_LUT_16 {
	uint32_t raw;
	struct {
		uint32_t CNR_EDGE_SCALE_LUT_16           : 6;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_YCURV_YCUR_CTRL {
	uint32_t raw;
	struct {
		uint32_t YCUR_ENABLE                     : 1;
		uint32_t YCUR_SHDW_SEL                   : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t YCUR_WSEL                       : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t YCUR_RSEL                       : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t YCUR_PROG_EN                    : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_PROG_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t YCUR_ST_ADDR                    : 6;
		uint32_t _rsv_6                          : 25;
		uint32_t YCUR_ST_W                       : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t YCUR_DATA_E                     : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t YCUR_DATA_O                     : 8;
		uint32_t _rsv_24                         : 7;
		uint32_t YCUR_W                          : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t YCUR_MAX                        : 9;
	} bits;
};

union REG_ISP_YCURV_YCUR_MEM_SW_MODE {
	uint32_t raw;
	struct {
		uint32_t YCUR_SW_RADDR                   : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t YCUR_SW_R_MEM_SEL               : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t YCUR_RDATA_R                    : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t YCUR_SW_R                       : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t SOFTRST                         : 1;
	} bits;
};

union REG_ISP_YCURV_YCUR_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_ISP_YCURV_YCUR_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_ISP_YCURV_YCUR_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_TOP_INT_EVENT0 {
	uint32_t raw;
	struct {
		uint32_t FRAME_DONE_FE0                  : 4;
		uint32_t FRAME_DONE_FE1                  : 2;
		uint32_t FRAME_DONE_FE2                  : 2;
		uint32_t FRAME_DONE_BE                   : 2;
		uint32_t FRAME_DONE_RAW                  : 1;
		uint32_t FRAME_DONE_RGB                  : 1;
		uint32_t FRAME_DONE_YUV                  : 1;
		uint32_t FRAME_DONE_POST                 : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t SHAW_DONE_FE0                   : 4;
		uint32_t SHAW_DONE_FE1                   : 2;
		uint32_t SHAW_DONE_FE2                   : 2;
		uint32_t SHAW_DONE_BE                    : 2;
		uint32_t SHAW_DONE_RAW                   : 1;
		uint32_t SHAW_DONE_RGB                   : 1;
		uint32_t SHAW_DONE_YUV                   : 1;
		uint32_t SHAW_DONE_POST                  : 1;
	} bits;
};

union REG_ISP_TOP_INT_EVENT1 {
	uint32_t raw;
	struct {
		uint32_t PQ_DONE_FE0                     : 4;
		uint32_t PQ_DONE_FE1                     : 2;
		uint32_t PQ_DONE_FE2                     : 2;
		uint32_t PQ_DONE_BE                      : 2;
		uint32_t PQ_DONE_RAW                     : 1;
		uint32_t PQ_DONE_RGB                     : 1;
		uint32_t PQ_DONE_YUV                     : 1;
		uint32_t PQ_DONE_POST                    : 1;
	} bits;
};

union REG_ISP_TOP_INT_EVENT2 {
	uint32_t raw;
	struct {
		uint32_t FRAME_START_FE0                 : 4;
		uint32_t FRAME_START_FE1                 : 2;
		uint32_t FRAME_START_FE2                 : 2;
		uint32_t FRAME_ERR                       : 1;
		uint32_t PCHK_ERR                        : 1;
		uint32_t CMDQ_INT                        : 1;
		uint32_t LINE_INTP_FE0                   : 1;
		uint32_t LINE_INTP_FE1                   : 1;
		uint32_t LINE_INTP_FE2                   : 1;
		uint32_t LINE_INTP_POST                  : 1;
		uint32_t INT_BDG_LITE                    : 1;
		uint32_t INT_DMA_ERR                     : 1;
	} bits;
};

union REG_ISP_TOP_ERROR_STS {
	uint32_t raw;
	struct {
		uint32_t PCHK0_ERR_FE0                   : 1;
		uint32_t PCHK0_ERR_FE1                   : 1;
		uint32_t PCHK0_ERR_FE2                   : 1;
		uint32_t PCHK0_ERR_BE                    : 1;
		uint32_t PCHK0_ERR_RAW                   : 1;
		uint32_t PCHK0_ERR_RGB                   : 1;
		uint32_t PCHK0_ERR_YUV                   : 1;
		uint32_t PCHK1_ERR_FE0                   : 1;
		uint32_t PCHK1_ERR_FE1                   : 1;
		uint32_t PCHK1_ERR_FE2                   : 1;
		uint32_t PCHK1_ERR_BE                    : 1;
		uint32_t PCHK1_ERR_RAW                   : 1;
		uint32_t PCHK1_ERR_RGB                   : 1;
		uint32_t PCHK1_ERR_YUV                   : 1;
	} bits;
};

union REG_ISP_TOP_INT_EVENT0_EN {
	uint32_t raw;
	struct {
		uint32_t FRAME_DONE_ENABLE_FE0           : 4;
		uint32_t FRAME_DONE_ENABLE_FE1           : 2;
		uint32_t FRAME_DONE_ENABLE_FE2           : 2;
		uint32_t FRAME_DONE_ENABLE_BE            : 2;
		uint32_t FRAME_DONE_ENABLE_RAW           : 1;
		uint32_t FRAME_DONE_ENABLE_RGB           : 1;
		uint32_t FRAME_DONE_ENABLE_YUV           : 1;
		uint32_t FRAME_DONE_ENABLE_POST          : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t SHAW_DONE_ENABLE_FE0            : 4;
		uint32_t SHAW_DONE_ENABLE_FE1            : 2;
		uint32_t SHAW_DONE_ENABLE_FE2            : 2;
		uint32_t SHAW_DONE_ENABLE_BE             : 2;
		uint32_t SHAW_DONE_ENABLE_RAW            : 1;
		uint32_t SHAW_DONE_ENABLE_RGB            : 1;
		uint32_t SHAW_DONE_ENABLE_YUV            : 1;
		uint32_t SHAW_DONE_ENABLE_POST           : 1;
	} bits;
};

union REG_ISP_TOP_INT_EVENT1_EN {
	uint32_t raw;
	struct {
		uint32_t PQ_DONE_ENABLE_FE0              : 4;
		uint32_t PQ_DONE_ENABLE_FE1              : 2;
		uint32_t PQ_DONE_ENABLE_FE2              : 2;
		uint32_t PQ_DONE_ENABLE_BE               : 2;
		uint32_t PQ_DONE_ENABLE_RAW              : 1;
		uint32_t PQ_DONE_ENABLE_RGB              : 1;
		uint32_t PQ_DONE_ENABLE_YUV              : 1;
		uint32_t PQ_DONE_ENABLE_POST             : 1;
	} bits;
};

union REG_ISP_TOP_INT_EVENT2_EN {
	uint32_t raw;
	struct {
		uint32_t FRAME_START_ENABLE_FE0          : 4;
		uint32_t FRAME_START_ENABLE_FE1          : 2;
		uint32_t FRAME_START_ENABLE_FE2          : 2;
		uint32_t FRAME_ERR_ENABLE                : 1;
		uint32_t PCHK_ERR_ENABLE                 : 1;
		uint32_t CMDQ_INT_ENABLE                 : 1;
		uint32_t LINE_INTP_ENABLE_FE0            : 1;
		uint32_t LINE_INTP_ENABLE_FE1            : 1;
		uint32_t LINE_INTP_ENABLE_FE2            : 1;
		uint32_t LINE_INTP_ENABLE_POST           : 1;
		uint32_t INT_BDG_LITE_ENABLE             : 1;
		uint32_t INT_DMA_ERR_ENABLE              : 1;
	} bits;
};

union REG_ISP_TOP_SW_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t TRIG_STR_FE0                    : 4;
		uint32_t TRIG_STR_FE1                    : 2;
		uint32_t TRIG_STR_FE2                    : 2;
		uint32_t TRIG_STR_BE                     : 2;
		uint32_t TRIG_STR_RAW                    : 1;
		uint32_t TRIG_STR_POST                   : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t SHAW_UP_FE0                     : 4;
		uint32_t SHAW_UP_FE1                     : 2;
		uint32_t SHAW_UP_FE2                     : 2;
		uint32_t SHAW_UP_BE                      : 2;
		uint32_t SHAW_UP_RAW                     : 1;
		uint32_t SHAW_UP_POST                    : 1;
	} bits;
};

union REG_ISP_TOP_SW_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t PQ_UP_FE0                       : 4;
		uint32_t PQ_UP_FE1                       : 2;
		uint32_t PQ_UP_FE2                       : 2;
		uint32_t PQ_UP_BE                        : 2;
		uint32_t PQ_UP_RAW                       : 1;
		uint32_t PQ_UP_POST                      : 1;
	} bits;
};

union REG_ISP_TOP_CTRL_MODE_SEL0 {
	uint32_t raw;
	struct {
		uint32_t TRIG_STR_SEL_FE0                : 4;
		uint32_t TRIG_STR_SEL_FE1                : 2;
		uint32_t TRIG_STR_SEL_FE2                : 2;
		uint32_t TRIG_STR_SEL_BE                 : 2;
		uint32_t TRIG_STR_SEL_RAW                : 1;
		uint32_t TRIG_STR_SEL_POST               : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t SHAW_UP_SEL_FE0                 : 4;
		uint32_t SHAW_UP_SEL_FE1                 : 2;
		uint32_t SHAW_UP_SEL_FE2                 : 2;
		uint32_t SHAW_UP_SEL_BE                  : 2;
		uint32_t SHAW_UP_SEL_RAW                 : 1;
		uint32_t SHAW_UP_SEL_POST                : 1;
	} bits;
};

union REG_ISP_TOP_CTRL_MODE_SEL1 {
	uint32_t raw;
	struct {
		uint32_t PQ_UP_SEL_FE0                   : 4;
		uint32_t PQ_UP_SEL_FE1                   : 2;
		uint32_t PQ_UP_SEL_FE2                   : 2;
		uint32_t PQ_UP_SEL_BE                    : 2;
		uint32_t PQ_UP_SEL_RAW                   : 1;
		uint32_t PQ_UP_SEL_POST                  : 1;
	} bits;
};

union REG_ISP_TOP_SCENARIOS_CTRL {
	uint32_t raw;
	struct {
		uint32_t DST2SC                          : 1;
		uint32_t DST2DMA                         : 1;
		uint32_t PRE2BE_L_ENABLE                 : 1;
		uint32_t PRE2BE_S_ENABLE                 : 1;
		uint32_t PRE2YUV_422_ENABLE              : 1;
		uint32_t BE2RAW_L_ENABLE                 : 1;
		uint32_t BE2RAW_S_ENABLE                 : 1;
		uint32_t BE_RDMA_L_ENABLE                : 1;
		uint32_t BE_RDMA_S_ENABLE                : 1;
		uint32_t BE_WDMA_L_ENABLE                : 1;
		uint32_t BE_WDMA_S_ENABLE                : 1;
		uint32_t BE_SRC_SEL                      : 2;
		uint32_t AF_RAW0YUV1                     : 1;
		uint32_t RGBMP_ONLINE_L_ENABLE           : 1;
		uint32_t RGBMP_ONLINE_S_ENABLE           : 1;
		uint32_t RAW2YUV_422_ENABLE              : 1;
		uint32_t HDR_ENABLE                      : 1;
		uint32_t HW_AUTO_ENABLE                  : 1;
		uint32_t HW_AUTO_ISO                     : 2;
		uint32_t DCI_RGB0YUV1                    : 1;
		uint32_t FE_DMA_SHARE_ENABLE             : 1;
		uint32_t FE2_DMA_SHARE_ENABLE            : 1;
	} bits;
};

union REG_ISP_TOP_SW_RST {
	uint32_t raw;
	struct {
		uint32_t ISP_RST                         : 1;
		uint32_t CSI0_RST                        : 1;
		uint32_t CSI1_RST                        : 1;
		uint32_t CSI_BE_RST                      : 1;
		uint32_t CSI2_RST                        : 1;
		uint32_t BDG_LITE_RST                    : 1;
		uint32_t AXI_RST                         : 1;
		uint32_t CMDQ_RST                        : 1;
		uint32_t APB_RST                         : 1;
		uint32_t RAW_RST                         : 1;
	} bits;
};

union REG_ISP_TOP_BLK_IDLE {
	uint32_t raw;
	struct {
		uint32_t FE0_BLK_IDLE                    : 1;
		uint32_t FE1_BLK_IDLE                    : 1;
		uint32_t FE2_BLK_IDLE                    : 1;
		uint32_t BE_BLK_IDLE                     : 1;
		uint32_t RAW_BLK_IDLE                    : 1;
		uint32_t RGB_BLK_IDLE                    : 1;
		uint32_t YUV_BLK_IDLE                    : 1;
		uint32_t RDMA_IDLE                       : 1;
		uint32_t WDMA0_IDLE                      : 1;
		uint32_t WDMA1_IDLE                      : 1;
	} bits;
};

union REG_ISP_TOP_BLK_IDLE_ENABLE {
	uint32_t raw;
	struct {
		uint32_t BLK_IDLE_CSI0_EN                : 1;
		uint32_t BLK_IDLE_CSI1_EN                : 1;
		uint32_t BLK_IDLE_CSI2_EN                : 1;
		uint32_t BLK_IDLE_BDG_LITE_EN            : 1;
		uint32_t BLK_IDLE_BE_EN                  : 1;
		uint32_t BLK_IDLE_POST_EN                : 1;
		uint32_t BLK_IDLE_APB_EN                 : 1;
		uint32_t BLK_IDLE_AXI_EN                 : 1;
		uint32_t BLK_IDLE_CMDQ_EN                : 1;
		uint32_t BLK_IDLE_RAW_EN                 : 1;
		uint32_t BLK_IDLE_RGB_EN                 : 1;
		uint32_t BLK_IDLE_YUV_EN                 : 1;
	} bits;
};

union REG_ISP_TOP_DBUS0 {
	uint32_t raw;
	struct {
		uint32_t DBUS0                           : 32;
	} bits;
};

union REG_ISP_TOP_DBUS1 {
	uint32_t raw;
	struct {
		uint32_t DBUS1                           : 32;
	} bits;
};

union REG_ISP_TOP_DBUS2 {
	uint32_t raw;
	struct {
		uint32_t DBUS2                           : 32;
	} bits;
};

union REG_ISP_TOP_DBUS3 {
	uint32_t raw;
	struct {
		uint32_t DBUS3                           : 32;
	} bits;
};

union REG_ISP_TOP_FORCE_INT {
	uint32_t raw;
	struct {
		uint32_t FORCE_ISP_INT                   : 1;
		uint32_t FORCE_ISP_INT_EN                : 1;
	} bits;
};

union REG_ISP_TOP_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 28;
		uint32_t DBUS_SEL                        : 4;
	} bits;
};

union REG_ISP_TOP_IP_ENABLE0 {
	uint32_t raw;
	struct {
		uint32_t FE0_RGBMAP_L_ENABLE             : 1;
		uint32_t FE0_RGBMAP_S_ENABLE             : 1;
		uint32_t FE0_BLC_L_ENABLE                : 1;
		uint32_t FE0_BLC_S_ENABLE                : 1;
		uint32_t FE1_RGBMAP_L_ENABLE             : 1;
		uint32_t FE1_RGBMAP_S_ENABLE             : 1;
		uint32_t FE1_BLC_L_ENABLE                : 1;
		uint32_t FE1_BLC_S_ENABLE                : 1;
		uint32_t FE2_RGBMAP_L_ENABLE             : 1;
		uint32_t FE2_RGBMAP_S_ENABLE             : 1;
		uint32_t FE2_BLC_L_ENABLE                : 1;
		uint32_t FE2_BLC_S_ENABLE                : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t BE_BLC_L_ENABLE                 : 1;
		uint32_t BE_BLC_S_ENABLE                 : 1;
		uint32_t BE_CROP_L_ENABLE                : 1;
		uint32_t BE_CROP_S_ENABLE                : 1;
		uint32_t BE_DPC_L_ENABLE                 : 1;
		uint32_t BE_DPC_S_ENABLE                 : 1;
		uint32_t BE_AF_ENABLE                    : 1;
	} bits;
};

union REG_ISP_TOP_IP_ENABLE1 {
	uint32_t raw;
	struct {
		uint32_t RAW_CROP_L_ENABLE               : 1;
		uint32_t RAW_CROP_S_ENABLE               : 1;
		uint32_t RAW_BNR_ENABLE                  : 1;
		uint32_t RAW_CFA_ENABLE                  : 1;
		uint32_t RAW_LSCM_ENABLE                 : 1;
		uint32_t RAW_WBG_L_ENABLE                : 1;
		uint32_t RAW_WBG_S_ENABLE                : 1;
		uint32_t RAW_LMP_L_ENABLE                : 1;
		uint32_t RAW_LMP_S_ENABLE                : 1;
		uint32_t RAW_AE_L_ENABLE                 : 1;
		uint32_t RAW_AE_S_ENABLE                 : 1;
		uint32_t RAW_GMS_ENABLE                  : 1;
		uint32_t RAW_RGBCAC_ENABLE               : 1;
		uint32_t RAW_LCAC_ENABLE                 : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t RGB_FUSION_ENABLE               : 1;
		uint32_t RGB_LTM_ENABLE                  : 1;
		uint32_t RGB_MANR_ENABLE                 : 1;
		uint32_t RGB_HISTV_ENABLE                : 1;
		uint32_t RGB_GAMMA_ENABLE                : 1;
		uint32_t RGB_DHZ_ENABLE                  : 1;
		uint32_t RGB_RGBDITHER_ENABLE            : 1;
		uint32_t RGB_CLUT_ENABLE                 : 1;
		uint32_t RGB_R2Y4_ENABLE                 : 1;
		uint32_t RGB_USER_GAMMA_ENABLE           : 1;
		uint32_t RGB_CCM_0_ENABLE                : 1;
		uint32_t RGB_CCM_1_ENABLE                : 1;
	} bits;
};

union REG_ISP_TOP_IP_ENABLE2 {
	uint32_t raw;
	struct {
		uint32_t YUV_PREYEE_ENABLE               : 1;
		uint32_t YUV_DITHER_ENABLE               : 1;
		uint32_t YUV_3DNR_ENABLE                 : 1;
		uint32_t YUV_YNR_ENABLE                  : 1;
		uint32_t YUV_CNR_ENABLE                  : 1;
		uint32_t YUV_EE_ENABLE                   : 1;
		uint32_t YUV_CROP_Y_ENABLE               : 1;
		uint32_t YUV_CROP_C_ENABLE               : 1;
		uint32_t YUV_YCURVE_ENABLE               : 1;
		uint32_t YUV_CA2_ENABLE                  : 1;
		uint32_t YUV_CA_ENABLE                   : 1;
		uint32_t YUV_DCI_ENABLE                  : 1;
		uint32_t YUV_LDCI_ENABLE                 : 1;
	} bits;
};

union REG_ISP_TOP_CMDQ_CTRL {
	uint32_t raw;
	struct {
		uint32_t CMDQ_TSK_EN                     : 8;
		uint32_t CMDQ_FLAG_SEL                   : 2;
		uint32_t CMDQ_TASK_SEL                   : 2;
	} bits;
};

union REG_ISP_TOP_CMDQ_TRIG {
	uint32_t raw;
	struct {
		uint32_t CMDQ_TSK_TRIG                   : 8;
	} bits;
};

union REG_ISP_TOP_TRIG_CNT {
	uint32_t raw;
	struct {
		uint32_t TRIG_STR_CNT                    : 4;
		uint32_t VSYNC_DELAY                     : 4;
	} bits;
};

union REG_ISP_TOP_SVN_VERSION {
	uint32_t raw;
	struct {
		uint32_t SVN_REVISION                    : 32;
	} bits;
};

union REG_ISP_TOP_TIMESTAMP {
	uint32_t raw;
	struct {
		uint32_t UNIX_TIMESTAMP                  : 32;
	} bits;
};

union REG_ISP_TOP_SCLIE_ENABLE {
	uint32_t raw;
	struct {
		uint32_t SLICE_ENABLE_MAIN_LEXP          : 1;
		uint32_t SLICE_ENABLE_MAIN_SEXP          : 1;
		uint32_t SLICE_ENABLE_SUB_LEXP           : 1;
		uint32_t SLICE_ENABLE_SUB_SEXP           : 1;
	} bits;
};

union REG_ISP_TOP_W_SLICE_THRESH_MAIN {
	uint32_t raw;
	struct {
		uint32_t W_SLICE_THR_MAIN_LEXP           : 16;
		uint32_t W_SLICE_THR_MAIN_SEXP           : 16;
	} bits;
};

union REG_ISP_TOP_W_SLICE_THRESH_SUB_CURR {
	uint32_t raw;
	struct {
		uint32_t W_SLICE_THR_SUB_CUR_LEXP        : 16;
		uint32_t W_SLICE_THR_SUB_CUR_SEXP        : 16;
	} bits;
};

union REG_ISP_TOP_W_SLICE_THRESH_SUB_PRV {
	uint32_t raw;
	struct {
		uint32_t W_SLICE_THR_SUB_PRV_LEXP        : 16;
		uint32_t W_SLICE_THR_SUB_PRV_SEXP        : 16;
	} bits;
};

union REG_ISP_TOP_R_SLICE_THRESH_MAIN {
	uint32_t raw;
	struct {
		uint32_t R_SLICE_THR_MAIN_LEXP           : 16;
		uint32_t R_SLICE_THR_MAIN_SEXP           : 16;
	} bits;
};

union REG_ISP_TOP_R_SLICE_THRESH_SUB_CURR {
	uint32_t raw;
	struct {
		uint32_t R_SLICE_THR_SUB_CUR_LEXP        : 16;
		uint32_t R_SLICE_THR_SUB_CUR_SEXP        : 16;
	} bits;
};

union REG_ISP_TOP_R_SLICE_THRESH_SUB_PRV {
	uint32_t raw;
	struct {
		uint32_t R_SLICE_THR_SUB_PRV_LEXP        : 16;
		uint32_t R_SLICE_THR_SUB_PRV_SEXP        : 16;
	} bits;
};

union REG_ISP_TOP_RAW_FRAME_VALID {
	uint32_t raw;
	struct {
		uint32_t RAW_FRAME_VLD                   : 1;
		uint32_t RAW_PQ_VLD                      : 1;
	} bits;
};

union REG_ISP_TOP_FIRST_FRAME {
	uint32_t raw;
	struct {
		uint32_t FIRST_FRAME_SW                  : 5;
		uint32_t FIRST_FRAME_TOP                 : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_WDMA_CORE_SHADOW_RD_SEL   {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
		uint32_t ABORT_MODE                      : 1;
	} bits;
};

union REG_WDMA_CORE_IP_DISABLE {
	uint32_t raw;
	struct {
		uint32_t IP_DISABLE                      : 32;
	} bits;
};

union REG_WDMA_CORE_DISABLE_SEGLEN {
	uint32_t raw;
	struct {
		uint32_t SEGLEN_DISABLE                  : 32;
	} bits;
};

union REG_WDMA_CORE_UP_RING_BASE {
	uint32_t raw;
	struct {
		uint32_t UP_RING_BASE                    : 32;
	} bits;
};

union REG_WDMA_CORE_NORM_STATUS0 {
	uint32_t raw;
	struct {
		uint32_t ABORT_DONE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t ERROR_AXI                       : 1;
		uint32_t ERROR_DMI                       : 1;
		uint32_t SLOT_FULL                       : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t ERROR_ID                        : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t DMA_VERSION                     : 16;
	} bits;
};

union REG_WDMA_CORE_NORM_STATUS1 {
	uint32_t raw;
	struct {
		uint32_t ID_IDLE                         : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_EN {
	uint32_t raw;
	struct {
		uint32_t RING_ENABLE                     : 32;
	} bits;
};

union REG_WDMA_CORE_NORM_PERF  {
	uint32_t raw;
	struct {
		uint32_t BWLWIN                          : 10;
		uint32_t BWLTXN                          : 6;
		uint32_t QOSO_TH                         : 4;
		uint32_t QOSO_EN                         : 1;
	} bits;
};

union REG_WDMA_CORE_RING_PATCH_ENABLE {
	uint32_t raw;
	struct {
		uint32_t RING_PATCH_ENABLE               : 32;
	} bits;
};

union REG_WDMA_CORE_SET_RING_BASE {
	uint32_t raw;
	struct {
		uint32_t SET_RING_BASE                   : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BASE_ADDR_L {
	uint32_t raw;
	struct {
		uint32_t RING_BASE_L                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BASE_ADDR_H {
	uint32_t raw;
	struct {
		uint32_t RING_BASE_H                     : 8;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE0 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE0                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE1 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE1                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE2 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE2                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE3 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE3                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE4 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE4                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE5 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE5                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE6 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE6                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE7 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE7                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE8 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE8                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE9 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE9                      : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE10 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE10                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE11 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE11                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE12 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE12                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE13 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE13                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE14 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE14                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE15 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE15                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE16 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE16                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE17 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE17                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE18 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE18                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE19 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE19                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE20 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE20                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE21 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE21                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE22 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE22                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE23 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE23                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE24 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE24                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE25 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE25                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE26 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE26                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE27 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE27                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE28 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE28                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE29 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE29                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE30 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE30                     : 32;
	} bits;
};

union REG_WDMA_CORE_RING_BUFFER_SIZE31 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE31                     : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS0 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR0                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS1 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR1                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS2 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR2                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS3 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR3                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS4 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR4                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS5 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR5                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS6 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR6                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS7 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR7                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS8 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR8                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS9 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR9                  : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS10 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR10                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS11 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR11                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS12 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR12                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS13 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR13                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS14 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR14                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS15 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR15                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS16 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR16                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS17 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR17                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS18 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR18                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS19 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR19                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS20 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR20                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS21 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR21                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS22 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR22                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS23 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR23                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS24 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR24                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS25 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR25                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS26 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR26                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS27 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR27                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS28 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR28                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS29 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR29                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS30 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR30                 : 32;
	} bits;
};

union REG_WDMA_CORE_NEXT_DMA_ADDR_STS31 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR31                 : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_RDMA_CORE_SHADOW_RD_SEL   {
	uint32_t raw;
	struct {
		uint32_t SHADOW_RD_SEL                   : 1;
		uint32_t ABORT_MODE                      : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t MAX_OSTD                        : 8;
		uint32_t OSTD_SW_EN                      : 1;
	} bits;
};

union REG_RDMA_CORE_IP_DISABLE {
	uint32_t raw;
	struct {
		uint32_t IP_DISABLE                      : 32;
	} bits;
};

union REG_RDMA_CORE_UP_RING_BASE {
	uint32_t raw;
	struct {
		uint32_t UP_RING_BASE                    : 32;
	} bits;
};

union REG_RDMA_CORE_NORM_STATUS0 {
	uint32_t raw;
	struct {
		uint32_t ABORT_DONE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t ERROR_AXI                       : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t ERROR_ID                        : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t DMA_VERSION                     : 16;
	} bits;
};

union REG_RDMA_CORE_NORM_STATUS1 {
	uint32_t raw;
	struct {
		uint32_t ID_IDLE                         : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_EN {
	uint32_t raw;
	struct {
		uint32_t RING_ENABLE                     : 32;
	} bits;
};

union REG_RDMA_CORE_NORM_PERF  {
	uint32_t raw;
	struct {
		uint32_t BWLWIN                          : 10;
		uint32_t BWLTXN                          : 6;
	} bits;
};

union REG_RDMA_CORE_AR_PRIORITY_SEL {
	uint32_t raw;
	struct {
		uint32_t AR_PRIORITY_SEL                 : 1;
		uint32_t QOS_PRIORITY_SEL                : 1;
		uint32_t ARB_HIST_DISABLE                : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t ARB_USAGE_TH                    : 4;
	} bits;
};

union REG_RDMA_CORE_RING_PATCH_ENABLE {
	uint32_t raw;
	struct {
		uint32_t RING_PATCH_ENABLE               : 32;
	} bits;
};

union REG_RDMA_CORE_SET_RING_BASE {
	uint32_t raw;
	struct {
		uint32_t SET_RING_BASE                   : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BASE_ADDR_L {
	uint32_t raw;
	struct {
		uint32_t RING_BASE_L                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BASE_ADDR_H {
	uint32_t raw;
	struct {
		uint32_t RING_BASE_H                     : 8;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE0 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE0                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE1 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE1                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE2 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE2                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE3 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE3                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE4 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE4                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE5 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE5                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE6 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE6                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE7 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE7                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE8 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE8                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE9 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE9                      : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE10 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE10                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE11 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE11                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE12 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE12                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE13 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE13                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE14 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE14                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE15 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE15                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE16 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE16                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE17 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE17                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE18 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE18                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE19 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE19                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE20 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE20                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE21 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE21                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE22 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE22                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE23 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE23                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE24 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE24                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE25 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE25                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE26 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE26                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE27 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE27                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE28 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE28                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE29 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE29                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE30 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE30                     : 32;
	} bits;
};

union REG_RDMA_CORE_RING_BUFFER_SIZE31 {
	uint32_t raw;
	struct {
		uint32_t RBUF_SIZE31                     : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS0 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR0                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS1 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR1                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS2 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR2                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS3 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR3                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS4 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR4                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS5 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR5                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS6 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR6                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS7 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR7                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS8 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR8                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS9 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR9                  : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS10 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR10                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS11 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR11                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS12 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR12                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS13 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR13                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS14 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR14                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS15 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR15                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS16 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR16                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS17 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR17                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS18 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR18                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS19 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR19                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS20 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR20                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS21 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR21                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS22 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR22                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS23 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR23                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS24 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR24                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS25 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR25                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS26 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR26                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS27 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR27                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS28 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR28                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS29 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR29                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS30 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR30                 : 32;
	} bits;
};

union REG_RDMA_CORE_NEXT_DMA_ADDR_STS31 {
	uint32_t raw;
	struct {
		uint32_t NEXT_DMA_ADDR31                 : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CSI_BDG_LITE_BDG_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t CSI_MODE                        : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t CH_NUM                          : 2;
		uint32_t CH0_DMA_WR_ENABLE               : 1;
		uint32_t CH1_DMA_WR_ENABLE               : 1;
		uint32_t CH2_DMA_WR_ENABLE               : 1;
		uint32_t Y_ONLY                          : 1;
		uint32_t _rsv_10                         : 1;
		uint32_t VS_POL                          : 1;
		uint32_t HS_POL                          : 1;
		uint32_t RESET_MODE                      : 1;
		uint32_t VS_MODE                         : 1;
		uint32_t ABORT_MODE                      : 1;
		uint32_t RESET                           : 1;
		uint32_t ABORT                           : 1;
		uint32_t CH3_DMA_WR_ENABLE               : 1;
		uint32_t CH3_DMA_420_WR_ENABLE           : 1;
		uint32_t _rsv_20                         : 2;
		uint32_t CH0_DMA_420_WR_ENABLE           : 1;
		uint32_t CH1_DMA_420_WR_ENABLE           : 1;
		uint32_t CSI_ENABLE                      : 1;
		uint32_t _rsv_25                         : 2;
		uint32_t CH2_DMA_420_WR_ENABLE           : 1;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_29                         : 2;
		uint32_t CSI_UP_REG                      : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_INT_EN                   : 1;
		uint32_t CH0_TRIG_INT_EN                 : 1;
		uint32_t CH0_DROP_INT_EN                 : 1;
		uint32_t CH0_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH0_FRAME_DONE_EN               : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CH1_VS_INT_EN                   : 1;
		uint32_t CH1_TRIG_INT_EN                 : 1;
		uint32_t CH1_DROP_INT_EN                 : 1;
		uint32_t CH1_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH1_FRAME_DONE_EN               : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_VS_INT_EN                   : 1;
		uint32_t CH2_TRIG_INT_EN                 : 1;
		uint32_t CH2_DROP_INT_EN                 : 1;
		uint32_t CH2_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH2_FRAME_DONE_EN               : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t CH3_VS_INT_EN                   : 1;
		uint32_t CH3_TRIG_INT_EN                 : 1;
		uint32_t CH3_DROP_INT_EN                 : 1;
		uint32_t CH3_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH3_FRAME_DONE_EN               : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_1 {
	uint32_t raw;
	struct {
		uint32_t LINE_INTP_EN                    : 1;
		uint32_t FIFO_OVERFLOW_INT_EN            : 1;
		uint32_t DMA_ERROR_INTP_EN               : 1;
		uint32_t _rsv_3                          : 5;
		uint32_t DROP_MODE                       : 4;
		uint32_t AVG_MODE                        : 4;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_FRAME_VLD {
	uint32_t raw;
	struct {
		uint32_t FRAME_VLD_CH0                   : 1;
		uint32_t FRAME_VLD_CH1                   : 1;
		uint32_t FRAME_VLD_CH2                   : 1;
		uint32_t FRAME_VLD_CH3                   : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH1_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH2_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH3_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH0_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH1_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH1_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH1_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH2_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH2_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH2_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH3_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH3_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH3_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_LINE_INTP_HEIGHT {
	uint32_t raw;
	struct {
		uint32_t LINE_INTP_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_CNT                      : 16;
		uint32_t CH0_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH0_TOT_BLK_IDLE                : 1;
		uint32_t CH0_TOT_DMA_IDLE                : 1;
		uint32_t CH0_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH1_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH1_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH1_VS_CNT                      : 16;
		uint32_t CH1_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH1_TOT_BLK_IDLE                : 1;
		uint32_t CH1_TOT_DMA_IDLE                : 1;
		uint32_t CH1_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH2_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH2_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH2_VS_CNT                      : 16;
		uint32_t CH2_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH2_TOT_BLK_IDLE                : 1;
		uint32_t CH2_TOT_DMA_IDLE                : 1;
		uint32_t CH2_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH3_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH3_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH3_VS_CNT                      : 16;
		uint32_t CH3_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH3_TOT_BLK_IDLE                : 1;
		uint32_t CH3_TOT_DMA_IDLE                : 1;
		uint32_t CH3_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_DROP_INT              : 1;
		uint32_t CH0_VS_INT                      : 1;
		uint32_t CH0_TRIG_INT                    : 1;
		uint32_t CH0_FRAME_DONE                  : 1;
		uint32_t CH0_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH0_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH0_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH0_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH1_FRAME_DROP_INT              : 1;
		uint32_t CH1_VS_INT                      : 1;
		uint32_t CH1_TRIG_INT                    : 1;
		uint32_t CH1_FRAME_DONE                  : 1;
		uint32_t CH1_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH1_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH1_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH1_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH2_FRAME_DROP_INT              : 1;
		uint32_t CH2_VS_INT                      : 1;
		uint32_t CH2_TRIG_INT                    : 1;
		uint32_t CH2_FRAME_DONE                  : 1;
		uint32_t CH2_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH2_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH2_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH2_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH3_FRAME_DROP_INT              : 1;
		uint32_t CH3_VS_INT                      : 1;
		uint32_t CH3_TRIG_INT                    : 1;
		uint32_t CH3_FRAME_DONE                  : 1;
		uint32_t CH3_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH3_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH3_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH3_FRAME_HEIGHT_LS_INT         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_1 {
	uint32_t raw;
	struct {
		uint32_t FIFO_OVERFLOW_INT               : 1;
		uint32_t FRAME_RESOLUTION_OVER_MAX_INT   : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t LINE_INTP_INT                   : 1;
		uint32_t DMA_ERROR_INT                   : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_BDG_DEBUG {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_IDLE                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_WR_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t WR_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t WR_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_RD_URGENT_CTRL {
	uint32_t raw;
	struct {
		uint32_t RD_NEAR_OVERFLOW_THRESHOLD      : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t RD_SAFE_THRESHOLD               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_IN                        : 16;
		uint32_t DUMMY_OUT                       : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_0 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_1 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_VALUE                  : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_RAW_RDMA_CTRL_READ_SEL {
	uint32_t raw;
	struct {
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_RAW_RDMA_CTRL_CONFIG {
	uint32_t raw;
	struct {
		uint32_t LE_RDMA_EN                      : 1;
		uint32_t SE_RDMA_EN                      : 1;
	} bits;
};

union REG_RAW_RDMA_CTRL_RDMA_SIZE {
	uint32_t raw;
	struct {
		uint32_t RDMI_WIDTHM1                    : 16;
		uint32_t RDMI_HEIGHTM1                   : 16;
	} bits;
};

union REG_RAW_RDMA_CTRL_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DPCM_MODE                       : 3;
		uint32_t _rsv_3                          : 5;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LDCI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t LDCI_ENABLE                     : 1;
		uint32_t LDCI_STATS_ENABLE               : 1;
		uint32_t LDCI_MAP_ENABLE                 : 1;
		uint32_t LDCI_UV_GAIN_ENABLE             : 1;
		uint32_t _rsv_4                          : 2;
		uint32_t LDCI_FIRST_FRAME_ENABLE         : 1;
		uint32_t LDCI_ZEROING_ENABLE             : 1;
		uint32_t LDCI_IMAGE_SIZE_DIV_BY_16X12    : 1;
		uint32_t _rsv_9                          : 19;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_ISP_LDCI_STRENGTH {
	uint32_t raw;
	struct {
		uint32_t LDCI_STRENGTH                   : 9;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_MAX {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_MAX               : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t LDCI_LUMA_WGT_MIN               : 8;
	} bits;
};

union REG_ISP_LDCI_IDX_IIR_ALPHA {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_IIR_ALPHA              : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_VAR_IIR_ALPHA              : 10;
	} bits;
};

union REG_ISP_LDCI_EDGE_SCALE {
	uint32_t raw;
	struct {
		uint32_t LDCI_EDGE_SCALE                 : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_EDGE_CORING                : 8;
	} bits;
};

union REG_ISP_LDCI_EDGE_CLAMP {
	uint32_t raw;
	struct {
		uint32_t LDCI_VAR_MAP_MAX                : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t LDCI_VAR_MAP_MIN                : 8;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_NORM {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_NORM            : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_VAR_FILTER_NORM            : 9;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_IDX_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_IDX_00          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_01          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_02          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_03          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_04          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_05          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_06          : 4;
		uint32_t LDCI_TONE_CURVE_IDX_07          : 4;
	} bits;
};

union REG_ISP_LDCI_BLK_SIZE_X {
	uint32_t raw;
	struct {
		uint32_t LDCI_BLK_SIZE_X                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t LDCI_BLK_SIZE_Y                 : 13;
	} bits;
};

union REG_ISP_LDCI_BLK_SIZE_X1 {
	uint32_t raw;
	struct {
		uint32_t LDCI_BLK_SIZE_X1                : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t LDCI_BLK_SIZE_Y1                : 13;
	} bits;
};

union REG_ISP_LDCI_SUBBLK_SIZE_X {
	uint32_t raw;
	struct {
		uint32_t LDCI_SUBBLK_SIZE_X              : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t LDCI_SUBBLK_SIZE_Y              : 16;
	} bits;
};

union REG_ISP_LDCI_SUBBLK_SIZE_X1 {
	uint32_t raw;
	struct {
		uint32_t LDCI_SUBBLK_SIZE_X1             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t LDCI_SUBBLK_SIZE_Y1             : 16;
	} bits;
};

union REG_ISP_LDCI_INTERP_NORM_LR {
	uint32_t raw;
	struct {
		uint32_t LDCI_INTERP_NORM_LR             : 16;
		uint32_t LDCI_INTERP_NORM_UD             : 16;
	} bits;
};

union REG_ISP_LDCI_SUB_INTERP_NORM_LR {
	uint32_t raw;
	struct {
		uint32_t LDCI_SUB_INTERP_NORM_LR         : 16;
		uint32_t LDCI_SUB_INTERP_NORM_UD         : 13;
	} bits;
};

union REG_ISP_LDCI_MEAN_NORM_X {
	uint32_t raw;
	struct {
		uint32_t LDCI_MEAN_NORM_X                : 15;
		uint32_t _rsv_15                         : 1;
		uint32_t LDCI_MEAN_NORM_Y                : 14;
	} bits;
};

union REG_ISP_LDCI_VAR_NORM_Y {
	uint32_t raw;
	struct {
		uint32_t LDCI_VAR_NORM_Y                 : 14;
	} bits;
};

union REG_ISP_LDCI_UV_GAIN_MAX {
	uint32_t raw;
	struct {
		uint32_t LDCI_UV_GAIN_MAX                : 7;
		uint32_t _rsv_7                          : 9;
		uint32_t LDCI_UV_GAIN_MIN                : 7;
	} bits;
};

union REG_ISP_LDCI_IMG_WIDTHM1 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IMG_WIDTHM1                : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t LDCI_IMG_HEIGHTM1               : 14;
	} bits;
};

union REG_ISP_LDCI_STATUS {
	uint32_t raw;
	struct {
		uint32_t LDCI_STATUS                     : 32;
	} bits;
};

union REG_ISP_LDCI_GRACE_RESET {
	uint32_t raw;
	struct {
		uint32_t LDCI_GRACE_RESET                : 1;
	} bits;
};

union REG_ISP_LDCI_MONITOR {
	uint32_t raw;
	struct {
		uint32_t LDCI_MONITOR                    : 32;
	} bits;
};

union REG_ISP_LDCI_FLOW {
	uint32_t raw;
	struct {
		uint32_t LDCI_ZERODCIOGRAM               : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t LDCI_SHADOW_SELECT              : 1;
	} bits;
};

union REG_ISP_LDCI_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t LDCI_MONITOR_SELECT             : 32;
	} bits;
};

union REG_ISP_LDCI_LOCATION {
	uint32_t raw;
	struct {
		uint32_t LDCI_LOCATION                   : 32;
	} bits;
};

union REG_ISP_LDCI_DEBUG {
	uint32_t raw;
	struct {
		uint32_t LDCI_DEBUG                      : 32;
	} bits;
};

union REG_ISP_LDCI_DUMMY {
	uint32_t raw;
	struct {
		uint32_t LDCI_DUMMY                      : 32;
	} bits;
};

union REG_ISP_LDCI_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t DMI_QOS                         : 2;
		uint32_t _rsv_6                          : 2;
		uint32_t FORCE_DMA_DISABLE               : 2;
	} bits;
};

union REG_ISP_LDCI_DCI_BAYER_STARTING {
	uint32_t raw;
	struct {
		uint32_t DCI_BAYER_STARTING              : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_00          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_01          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_02          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_03          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_04          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_05          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_06          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_07          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_08          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_09          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_10          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_11          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_12          : 11;
		uint32_t _rsv_11                         : 5;
		uint32_t LDCI_IDX_FILTER_LUT_13          : 11;
	} bits;
};

union REG_ISP_LDCI_IDX_FILTER_LUT_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_IDX_FILTER_LUT_14          : 11;
	} bits;
};

union REG_ISP_LDCI_INTERP_NORM_LR1 {
	uint32_t raw;
	struct {
		uint32_t LDCI_INTERP_NORM_LR1            : 16;
		uint32_t LDCI_INTERP_NORM_UD1            : 16;
	} bits;
};

union REG_ISP_LDCI_SUB_INTERP_NORM_LR1 {
	uint32_t raw;
	struct {
		uint32_t LDCI_SUB_INTERP_NORM_LR1        : 16;
		uint32_t LDCI_SUB_INTERP_NORM_UD1        : 13;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_00_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_00_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_00_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_01_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_01_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_01_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_02_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_02_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_02_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_03_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_03_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_03_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_04_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_04_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_04_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_05_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_05_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_05_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_06_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_06_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_06_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_00       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_01       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_02       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_03       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_04       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_05       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_06       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_07       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_08       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_09       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_10       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_11       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_12       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_13       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_07_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_07_14       : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_07_15       : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_00        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_01        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_02        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_03        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_04        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_05        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_06 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_06        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_07        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_08        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_09        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_10 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_10        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_11        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_12        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_13        : 10;
	} bits;
};

union REG_ISP_LDCI_TONE_CURVE_LUT_P_14 {
	uint32_t raw;
	struct {
		uint32_t LDCI_TONE_CURVE_LUT_P_14        : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_TONE_CURVE_LUT_P_15        : 10;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_00            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_01            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_02            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_03            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_04            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_05            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_06            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_07            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_08 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_08            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_09            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_10            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_11            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_12 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_12            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_13            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_14            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_15            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_16 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_16            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_17            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_18            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_19            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_20 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_20            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_21            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_22            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_23            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_24 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_24            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_25            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_26            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_27            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_28 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_28            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_29            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_30            : 8;
		uint32_t LDCI_LUMA_WGT_LUT_31            : 8;
	} bits;
};

union REG_ISP_LDCI_LUMA_WGT_LUT_32 {
	uint32_t raw;
	struct {
		uint32_t LDCI_LUMA_WGT_LUT_32            : 8;
	} bits;
};

union REG_ISP_LDCI_VAR_FILTER_LUT_00 {
	uint32_t raw;
	struct {
		uint32_t LDCI_VAR_FILTER_LUT_00          : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_VAR_FILTER_LUT_01          : 10;
	} bits;
};

union REG_ISP_LDCI_VAR_FILTER_LUT_02 {
	uint32_t raw;
	struct {
		uint32_t LDCI_VAR_FILTER_LUT_02          : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_VAR_FILTER_LUT_03          : 10;
	} bits;
};

union REG_ISP_LDCI_VAR_FILTER_LUT_04 {
	uint32_t raw;
	struct {
		uint32_t LDCI_VAR_FILTER_LUT_04          : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LDCI_VAR_FILTER_LUT_05          : 10;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_RGB_TOP_0 {
	uint32_t raw;
	struct {
		uint32_t RGBTOP_BAYER_TYPE               : 4;
		uint32_t RGBTOP_RGBIR_ENABLE             : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t UP_PQ_EN                        : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t PROG_HDK_DIS                    : 1;
	} bits;
};

union REG_ISP_RGB_TOP_1 {
	uint32_t raw;
	struct {
		uint32_t RGB_SHDW_SEL                    : 1;
	} bits;
};

union REG_ISP_RGB_TOP_2 {
	uint32_t raw;
	struct {
		uint32_t SHDW_DMY                        : 32;
	} bits;
};

union REG_ISP_RGB_TOP_3 {
	uint32_t raw;
	struct {
		uint32_t DMY                             : 32;
	} bits;
};

union REG_ISP_RGB_TOP_4 {
	uint32_t raw;
	struct {
		uint32_t PROB_OUT_SEL                    : 5;
		uint32_t PROB_PERFMT                     : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t PROB_FMT                        : 6;
	} bits;
};

union REG_ISP_RGB_TOP_5 {
	uint32_t raw;
	struct {
		uint32_t PROB_LINE                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_PIX                        : 12;
	} bits;
};

union REG_ISP_RGB_TOP_6 {
	uint32_t raw;
	struct {
		uint32_t PROB_R                          : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_G                          : 12;
	} bits;
};

union REG_ISP_RGB_TOP_7 {
	uint32_t raw;
	struct {
		uint32_t PROB_B                          : 12;
	} bits;
};

union REG_ISP_RGB_TOP_8 {
	uint32_t raw;
	struct {
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t DBG_EN                          : 1;
	} bits;
};

union REG_ISP_RGB_TOP_9 {
	uint32_t raw;
	struct {
		uint32_t RGBTOP_IMGW_M1                  : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t RGBTOP_IMGH_M1                  : 13;
	} bits;
};

union REG_ISP_RGB_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t IR_BIT_MODE                     : 1;
		uint32_t IR_SW_MODE                      : 1;
		uint32_t IR_DMI_ENABLE                   : 1;
		uint32_t IR_CROP_ENABLE                  : 1;
		uint32_t IR_DMI_NUM_SW                   : 14;
	} bits;
};

union REG_ISP_RGB_TOP_11 {
	uint32_t raw;
	struct {
		uint32_t IR_IMG_WIDTH                    : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IR_IMG_HEIGHT                   : 14;
	} bits;
};

union REG_ISP_RGB_TOP_12 {
	uint32_t raw;
	struct {
		uint32_t IR_CROP_W_STR                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IR_CROP_W_END                   : 14;
	} bits;
};

union REG_ISP_RGB_TOP_13 {
	uint32_t raw;
	struct {
		uint32_t IR_CROP_H_STR                   : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t IR_CROP_H_END                   : 14;
	} bits;
};

union REG_ISP_RGB_TOP_14 {
	uint32_t raw;
	struct {
		uint32_t IRM_ENABLE                      : 1;
		uint32_t IRM_HW_RQOS                     : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t IR_BLENDING_WGT                 : 9;
		uint32_t _rsv_13                         : 3;
		uint32_t IR_DMI_NUM                      : 14;
	} bits;
};

union REG_ISP_RGB_TOP_DBG_IP_S_VLD {
	uint32_t raw;
	struct {
		uint32_t IP_S_TVALID                     : 31;
		uint32_t IP_DBG_EN                       : 1;
	} bits;
};

union REG_ISP_RGB_TOP_DBG_IP_S_RDY {
	uint32_t raw;
	struct {
		uint32_t IP_S_TREADY                     : 31;
	} bits;
};

union REG_ISP_RGB_TOP_DBG_DMI_VLD {
	uint32_t raw;
	struct {
		uint32_t IP_DMI_VALID                    : 16;
	} bits;
};

union REG_ISP_RGB_TOP_DBG_DMI_RDY {
	uint32_t raw;
	struct {
		uint32_t IP_DMI_READY                    : 16;
	} bits;
};

union REG_ISP_RGB_TOP_PATGEN1 {
	uint32_t raw;
	struct {
		uint32_t X_CURSER                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t Y_CURSER                        : 14;
		uint32_t CURSER_EN                       : 1;
		uint32_t PG_ENABLE                       : 1;
	} bits;
};

union REG_ISP_RGB_TOP_PATGEN2 {
	uint32_t raw;
	struct {
		uint32_t CURSER_VALUE                    : 16;
	} bits;
};

union REG_ISP_RGB_TOP_PATGEN3 {
	uint32_t raw;
	struct {
		uint32_t VALUE_REPORT                    : 32;
	} bits;
};

union REG_ISP_RGB_TOP_PATGEN4 {
	uint32_t raw;
	struct {
		uint32_t XCNT_RPT                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t YCNT_RPT                        : 14;
	} bits;
};

union REG_ISP_RGB_TOP_CHK_SUM {
	uint32_t raw;
	struct {
		uint32_t CHK_SUM_Y                       : 8;
		uint32_t CHK_SUM_U                       : 8;
		uint32_t CHK_SUM_V                       : 8;
		uint32_t CHK_SUM_EN                      : 1;
	} bits;
};

union REG_ISP_RGB_TOP_DMA_IDLE {
	uint32_t raw;
	struct {
		uint32_t IP_DMA_IDLE                     : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_PRE_WDMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t WDMI_EN_LE                      : 1;
		uint32_t WDMI_EN_SE                      : 1;
		uint32_t DMA_WR_MODE                     : 1;
		uint32_t DMA_WR_MSB                      : 1;
		uint32_t _rsv_4                          : 27;
		uint32_t SHDW_READ_SEL                   : 1;
	} bits;
};

union REG_PRE_WDMA_CTRL_PRE_RAW_BE_RDMI_DPCM {
	uint32_t raw;
	struct {
		uint32_t DPCM_MODE                       : 3;
		uint32_t MIPI_OPT                        : 1;
		uint32_t _rsv_4                          : 12;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_PRE_WDMA_CTRL_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RW                        : 16;
		uint32_t DUMMY_RO                        : 16;
	} bits;
};

union REG_PRE_WDMA_CTRL_DEBUG_INFO {
	uint32_t raw;
	struct {
		uint32_t WDMA_DONE_LE                    : 1;
		uint32_t WDMA_DONE_SE                    : 1;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_EE_00 {
	uint32_t raw;
	struct {
		uint32_t EE_ENABLE                       : 1;
		uint32_t EE_SHADOW_SEL                   : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t EE_DEBUG_MODE                   : 4;
		uint32_t EE_TOTAL_CORING                 : 8;
		uint32_t EE_TOTAL_MOTION_CORING          : 8;
		uint32_t EE_TOTAL_GAIN                   : 8;
	} bits;
};

union REG_ISP_EE_04 {
	uint32_t raw;
	struct {
		uint32_t EE_TOTAL_OSHTTHRD               : 8;
		uint32_t EE_TOTAL_USHTTHRD               : 8;
		uint32_t EE_DEBUG_SHIFT_BIT              : 3;
		uint32_t _rsv_19                         : 12;
		uint32_t EE_PRE_PROC_ENABLE              : 1;
	} bits;
};

union REG_ISP_EE_0C {
	uint32_t raw;
	struct {
		uint32_t EE_LUMAREF_LPF_EN               : 1;
		uint32_t EE_LUMA_CORING_EN               : 1;
		uint32_t _rsv_2                          : 4;
		uint32_t EE_LUMA_ADPTCTRL_EN             : 1;
		uint32_t EE_DELTA_ADPTCTRL_EN            : 1;
		uint32_t EE_DELTA_ADPTCTRL_SHIFT         : 2;
		uint32_t EE_CHROMAREF_LPF_EN             : 1;
		uint32_t EE_CHROMA_ADPTCTRL_EN           : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MF_CORE_GAIN                 : 8;
	} bits;
};

union REG_ISP_EE_10 {
	uint32_t raw;
	struct {
		uint32_t HF_BLEND_WGT                    : 8;
		uint32_t MF_BLEND_WGT                    : 8;
	} bits;
};

union REG_ISP_EE_A4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_IN_0         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_1         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_2         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_3         : 8;
	} bits;
};

union REG_ISP_EE_A8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_OUT_0        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_1        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_2        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_3        : 8;
	} bits;
};

union REG_ISP_EE_AC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_SLOPE_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_LUMA_CORING_LUT_SLOPE_1      : 12;
	} bits;
};

union REG_ISP_EE_B0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_SLOPE_2      : 12;
	} bits;
};

union REG_ISP_EE_B4 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_IN_0       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_1       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_2       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_3       : 8;
	} bits;
};

union REG_ISP_EE_B8 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_OUT_0      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_1      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_2      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_3      : 8;
	} bits;
};

union REG_ISP_EE_BC {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_SLOPE_0    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MOTION_CORING_LUT_SLOPE_1    : 12;
	} bits;
};

union REG_ISP_EE_C0 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_SLOPE_2    : 12;
	} bits;
};

union REG_ISP_EE_C4 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_IN_0          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_1          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_2          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_3          : 8;
	} bits;
};

union REG_ISP_EE_C8 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_OUT_0         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_1         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_2         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_3         : 8;
	} bits;
};

union REG_ISP_EE_HCC {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_0       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_1       : 12;
	} bits;
};

union REG_ISP_EE_HD0 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_2       : 12;
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
		uint32_t EE_CHROMA_ADPTCTRL_LUT_00       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_01       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_02       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_03       : 6;
	} bits;
};

union REG_ISP_EE_17C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_04       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_05       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_06       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_07       : 6;
	} bits;
};

union REG_ISP_EE_180 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_08       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_09       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_10       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_11       : 6;
	} bits;
};

union REG_ISP_EE_184 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_12       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_13       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_14       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_15       : 6;
	} bits;
};

union REG_ISP_EE_188 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_16       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_17       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_18       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_19       : 6;
	} bits;
};

union REG_ISP_EE_18C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_20       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_21       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_22       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_23       : 6;
	} bits;
};

union REG_ISP_EE_190 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_24       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_25       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_26       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_27       : 6;
	} bits;
};

union REG_ISP_EE_194 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_28       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_29       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_30       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_31       : 6;
	} bits;
};

union REG_ISP_EE_198 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_32       : 6;
	} bits;
};

union REG_ISP_EE_19C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_IN_0          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_1          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_2          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_3          : 8;
	} bits;
};

union REG_ISP_EE_1A0 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_OUT_0         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_1         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_2         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_3         : 8;
	} bits;
};

union REG_ISP_EE_1A4 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_0       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_1       : 12;
	} bits;
};

union REG_ISP_EE_1A8 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_2       : 12;
	} bits;
};

union REG_ISP_EE_1C4 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 16;
		uint32_t EE_SHTCTRL_OSHTGAIN             : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_SHTCTRL_USHTGAIN             : 6;
	} bits;
};

union REG_ISP_EE_1C8 {
	uint32_t raw;
	struct {
		uint32_t EE_TOTAL_OSHTTHRD_CLP           : 8;
		uint32_t EE_TOTAL_USHTTHRD_CLP           : 8;
	} bits;
};

union REG_ISP_EE_1CC {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_IN_0              : 8;
		uint32_t EE_MOTION_LUT_IN_1              : 8;
		uint32_t EE_MOTION_LUT_IN_2              : 8;
		uint32_t EE_MOTION_LUT_IN_3              : 8;
	} bits;
};

union REG_ISP_EE_1D0 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_OUT_0             : 8;
		uint32_t EE_MOTION_LUT_OUT_1             : 8;
		uint32_t EE_MOTION_LUT_OUT_2             : 8;
		uint32_t EE_MOTION_LUT_OUT_3             : 8;
	} bits;
};

union REG_ISP_EE_1D4 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_SLOPE_0           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MOTION_LUT_SLOPE_1           : 12;
	} bits;
};

union REG_ISP_EE_1D8 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_SLOPE_2           : 12;
	} bits;
};

union REG_ISP_EE_1DC {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_0                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_1                       : 9;
	} bits;
};

union REG_ISP_EE_1E0 {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_2                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_3                       : 9;
	} bits;
};

union REG_ISP_EE_1E4 {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_4                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_5                       : 9;
	} bits;
};

union REG_ISP_EE_1E8 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_0                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_1                       : 9;
	} bits;
};

union REG_ISP_EE_1EC {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_2                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_3                       : 9;
	} bits;
};

union REG_ISP_EE_1F0 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_4                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_5                       : 9;
	} bits;
};

union REG_ISP_EE_1F4 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_6                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_7                       : 9;
	} bits;
};

union REG_ISP_EE_1F8 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_8                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_9                       : 9;
	} bits;
};

union REG_ISP_EE_1FC {
	uint32_t raw;
	struct {
		uint32_t EE_SOFT_CLAMP_ENABLE            : 1;
		uint32_t EE_CBCR_SWITCH                  : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t EE_UPPER_BOUND_LEFT_DIFF        : 8;
		uint32_t EE_LOWER_BOUND_RIGHT_DIFF       : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_YGAMMA_GAMMA_CTRL {
	uint32_t raw;
	struct {
		uint32_t YGAMMA_ENABLE                   : 1;
		uint32_t GAMMA_SHDW_SEL                  : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_PROG_CTRL {
	uint32_t raw;
	struct {
		uint32_t GAMMA_WSEL                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t GAMMA_RSEL                      : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t GAMMA_PROG_EN                   : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t GAMMA_PROG_1TO3_EN              : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t GAMMA_PROG_MODE                 : 2;
		uint32_t _rsv_18                         : 6;
		uint32_t GAMMA_W                         : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_PROG_ST_ADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_ST_ADDR                   : 8;
		uint32_t _rsv_8                          : 23;
		uint32_t GAMMA_ST_W                      : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_PROG_DATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_DATA_E                    : 16;
		uint32_t GAMMA_DATA_O                    : 16;
	} bits;
};

union REG_YGAMMA_GAMMA_PROG_MAX {
	uint32_t raw;
	struct {
		uint32_t YGAMMA_MAX                      : 17;
	} bits;
};

union REG_YGAMMA_GAMMA_MEM_SW_RADDR {
	uint32_t raw;
	struct {
		uint32_t GAMMA_SW_RADDR                  : 8;
		uint32_t _rsv_8                          : 4;
		uint32_t GAMMA_SW_R_MEM_SEL              : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_MEM_SW_RDATA {
	uint32_t raw;
	struct {
		uint32_t GAMMA_RDATA_R                   : 16;
		uint32_t _rsv_16                         : 15;
		uint32_t GAMMA_SW_R                      : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_MEM_SW_RDATA_BG {
	uint32_t raw;
	struct {
		uint32_t GAMMA_RDATA_G                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t GAMMA_RDATA_B                   : 12;
	} bits;
};

union REG_YGAMMA_GAMMA_DBG {
	uint32_t raw;
	struct {
		uint32_t PROG_HDK_DIS                    : 1;
		uint32_t SOFTRST                         : 1;
	} bits;
};

union REG_YGAMMA_GAMMA_DMY0 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF0                        : 32;
	} bits;
};

union REG_YGAMMA_GAMMA_DMY1 {
	uint32_t raw;
	struct {
		uint32_t DMY_DEF1                        : 32;
	} bits;
};

union REG_YGAMMA_GAMMA_DMY_R {
	uint32_t raw;
	struct {
		uint32_t DMY_RO                          : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_PREYEE_00 {
	uint32_t raw;
	struct {
		uint32_t EE_ENABLE                       : 1;
		uint32_t EE_SHADOW_SEL                   : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t EE_DEBUG_MODE                   : 4;
		uint32_t EE_TOTAL_CORING                 : 8;
		uint32_t EE_TOTAL_MOTION_CORING          : 8;
		uint32_t EE_TOTAL_GAIN                   : 8;
	} bits;
};

union REG_ISP_PREYEE_04 {
	uint32_t raw;
	struct {
		uint32_t EE_TOTAL_OSHTTHRD               : 8;
		uint32_t EE_TOTAL_USHTTHRD               : 8;
		uint32_t EE_DEBUG_SHIFT_BIT              : 3;
		uint32_t _rsv_19                         : 12;
		uint32_t EE_PRE_PROC_ENABLE              : 1;
	} bits;
};

union REG_ISP_PREYEE_0C {
	uint32_t raw;
	struct {
		uint32_t EE_LUMAREF_LPF_EN               : 1;
		uint32_t EE_LUMA_CORING_EN               : 1;
		uint32_t _rsv_2                          : 4;
		uint32_t EE_LUMA_ADPTCTRL_EN             : 1;
		uint32_t EE_DELTA_ADPTCTRL_EN            : 1;
		uint32_t EE_DELTA_ADPTCTRL_SHIFT         : 2;
		uint32_t EE_CHROMAREF_LPF_EN             : 1;
		uint32_t EE_CHROMA_ADPTCTRL_EN           : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MF_CORE_GAIN                 : 8;
	} bits;
};

union REG_ISP_PREYEE_10 {
	uint32_t raw;
	struct {
		uint32_t HF_BLEND_WGT                    : 8;
		uint32_t MF_BLEND_WGT                    : 8;
	} bits;
};

union REG_ISP_PREYEE_A4 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_IN_0         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_1         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_2         : 8;
		uint32_t EE_LUMA_CORING_LUT_IN_3         : 8;
	} bits;
};

union REG_ISP_PREYEE_A8 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_OUT_0        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_1        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_2        : 8;
		uint32_t EE_LUMA_CORING_LUT_OUT_3        : 8;
	} bits;
};

union REG_ISP_PREYEE_AC {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_SLOPE_0      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_LUMA_CORING_LUT_SLOPE_1      : 12;
	} bits;
};

union REG_ISP_PREYEE_B0 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_CORING_LUT_SLOPE_2      : 12;
	} bits;
};

union REG_ISP_PREYEE_B4 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_IN_0       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_1       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_2       : 8;
		uint32_t EE_MOTION_CORING_LUT_IN_3       : 8;
	} bits;
};

union REG_ISP_PREYEE_B8 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_OUT_0      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_1      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_2      : 8;
		uint32_t EE_MOTION_CORING_LUT_OUT_3      : 8;
	} bits;
};

union REG_ISP_PREYEE_BC {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_SLOPE_0    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MOTION_CORING_LUT_SLOPE_1    : 12;
	} bits;
};

union REG_ISP_PREYEE_C0 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_CORING_LUT_SLOPE_2    : 12;
	} bits;
};

union REG_ISP_PREYEE_C4 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_IN_0          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_1          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_2          : 8;
		uint32_t EE_MCORE_GAIN_LUT_IN_3          : 8;
	} bits;
};

union REG_ISP_PREYEE_C8 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_OUT_0         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_1         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_2         : 8;
		uint32_t EE_MCORE_GAIN_LUT_OUT_3         : 8;
	} bits;
};

union REG_ISP_PREYEE_HCC {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_0       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_1       : 12;
	} bits;
};

union REG_ISP_PREYEE_HD0 {
	uint32_t raw;
	struct {
		uint32_t EE_MCORE_GAIN_LUT_SLOPE_2       : 12;
	} bits;
};

union REG_ISP_PREYEE_130 {
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

union REG_ISP_PREYEE_134 {
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

union REG_ISP_PREYEE_138 {
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

union REG_ISP_PREYEE_13C {
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

union REG_ISP_PREYEE_140 {
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

union REG_ISP_PREYEE_144 {
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

union REG_ISP_PREYEE_148 {
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

union REG_ISP_PREYEE_14C {
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

union REG_ISP_PREYEE_150 {
	uint32_t raw;
	struct {
		uint32_t EE_LUMA_ADPTCTRL_LUT_32         : 6;
	} bits;
};

union REG_ISP_PREYEE_154 {
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

union REG_ISP_PREYEE_158 {
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

union REG_ISP_PREYEE_15C {
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

union REG_ISP_PREYEE_160 {
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

union REG_ISP_PREYEE_164 {
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

union REG_ISP_PREYEE_168 {
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

union REG_ISP_PREYEE_16C {
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

union REG_ISP_PREYEE_170 {
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

union REG_ISP_PREYEE_174 {
	uint32_t raw;
	struct {
		uint32_t EE_DELTA_ADPTCTRL_LUT_32        : 6;
	} bits;
};

union REG_ISP_PREYEE_178 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_00       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_01       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_02       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_03       : 6;
	} bits;
};

union REG_ISP_PREYEE_17C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_04       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_05       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_06       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_07       : 6;
	} bits;
};

union REG_ISP_PREYEE_180 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_08       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_09       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_10       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_11       : 6;
	} bits;
};

union REG_ISP_PREYEE_184 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_12       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_13       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_14       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_15       : 6;
	} bits;
};

union REG_ISP_PREYEE_188 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_16       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_17       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_18       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_19       : 6;
	} bits;
};

union REG_ISP_PREYEE_18C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_20       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_21       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_22       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_23       : 6;
	} bits;
};

union REG_ISP_PREYEE_190 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_24       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_25       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_26       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_27       : 6;
	} bits;
};

union REG_ISP_PREYEE_194 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_28       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_29       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_30       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_CHROMA_ADPTCTRL_LUT_31       : 6;
	} bits;
};

union REG_ISP_PREYEE_198 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_ADPTCTRL_LUT_32       : 6;
	} bits;
};

union REG_ISP_PREYEE_19C {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_IN_0          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_1          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_2          : 8;
		uint32_t EE_CHROMA_AMP_LUT_IN_3          : 8;
	} bits;
};

union REG_ISP_PREYEE_1A0 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_OUT_0         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_1         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_2         : 8;
		uint32_t EE_CHROMA_AMP_LUT_OUT_3         : 8;
	} bits;
};

union REG_ISP_PREYEE_1A4 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_0       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_1       : 12;
	} bits;
};

union REG_ISP_PREYEE_1A8 {
	uint32_t raw;
	struct {
		uint32_t EE_CHROMA_AMP_LUT_SLOPE_2       : 12;
	} bits;
};

union REG_ISP_PREYEE_1C4 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 16;
		uint32_t EE_SHTCTRL_OSHTGAIN             : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t EE_SHTCTRL_USHTGAIN             : 6;
	} bits;
};

union REG_ISP_PREYEE_1C8 {
	uint32_t raw;
	struct {
		uint32_t EE_TOTAL_OSHTTHRD_CLP           : 8;
		uint32_t EE_TOTAL_USHTTHRD_CLP           : 8;
	} bits;
};

union REG_ISP_PREYEE_1CC {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_IN_0              : 8;
		uint32_t EE_MOTION_LUT_IN_1              : 8;
		uint32_t EE_MOTION_LUT_IN_2              : 8;
		uint32_t EE_MOTION_LUT_IN_3              : 8;
	} bits;
};

union REG_ISP_PREYEE_1D0 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_OUT_0             : 8;
		uint32_t EE_MOTION_LUT_OUT_1             : 8;
		uint32_t EE_MOTION_LUT_OUT_2             : 8;
		uint32_t EE_MOTION_LUT_OUT_3             : 8;
	} bits;
};

union REG_ISP_PREYEE_1D4 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_SLOPE_0           : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t EE_MOTION_LUT_SLOPE_1           : 12;
	} bits;
};

union REG_ISP_PREYEE_1D8 {
	uint32_t raw;
	struct {
		uint32_t EE_MOTION_LUT_SLOPE_2           : 12;
	} bits;
};

union REG_ISP_PREYEE_1DC {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_0                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_1                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1E0 {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_2                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_3                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1E4 {
	uint32_t raw;
	struct {
		uint32_t HF_COEF_4                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t HF_COEF_5                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1E8 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_0                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_1                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1EC {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_2                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_3                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1F0 {
	uint32_t raw;
	struct {
		uint32_t MF_COEF_4                       : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t MF_COEF_5                       : 9;
	} bits;
};

union REG_ISP_PREYEE_1FC {
	uint32_t raw;
	struct {
		uint32_t EE_SOFT_CLAMP_ENABLE            : 1;
		uint32_t EE_CBCR_SWITCH                  : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t EE_UPPER_BOUND_LEFT_DIFF        : 8;
		uint32_t EE_LOWER_BOUND_RIGHT_DIFF       : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CSI_BDG_TOP_CTRL {
	uint32_t raw;
	struct {
		uint32_t CSI_MODE                        : 2;
		uint32_t CSI_IN_FORMAT                   : 1;
		uint32_t CSI_IN_YUV_FORMAT               : 1;
		uint32_t CH_NUM                          : 2;
		uint32_t CH0_DMA_WR_ENABLE               : 1;
		uint32_t CH1_DMA_WR_ENABLE               : 1;
		uint32_t CH2_DMA_WR_ENABLE               : 1;
		uint32_t Y_ONLY                          : 1;
		uint32_t PXL_DATA_SEL                    : 1;
		uint32_t VS_POL                          : 1;
		uint32_t HS_POL                          : 1;
		uint32_t RESET_MODE                      : 1;
		uint32_t VS_MODE                         : 1;
		uint32_t ABORT_MODE                      : 1;
		uint32_t RESET                           : 1;
		uint32_t ABORT                           : 1;
		uint32_t CH3_DMA_WR_ENABLE               : 1;
		uint32_t CH3_DMA_420_WR_ENABLE           : 1;
		uint32_t YUV_PACK_MODE                   : 1;
		uint32_t MULTI_CH_FRAME_SYNC_EN          : 1;
		uint32_t CH0_DMA_420_WR_ENABLE           : 1;
		uint32_t CH1_DMA_420_WR_ENABLE           : 1;
		uint32_t CSI_ENABLE                      : 1;
		uint32_t TGEN_ENABLE                     : 1;
		uint32_t YUV2BAY_ENABLE                  : 1;
		uint32_t CH2_DMA_420_WR_ENABLE           : 1;
		uint32_t SHDW_READ_SEL                   : 1;
		uint32_t _rsv_29                         : 2;
		uint32_t CSI_UP_REG                      : 1;
	} bits;
};

union REG_ISP_CSI_BDG_INTERRUPT_CTRL {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_INT_EN                   : 1;
		uint32_t CH0_TRIG_INT_EN                 : 1;
		uint32_t CH0_DROP_INT_EN                 : 1;
		uint32_t CH0_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH1_VS_INT_EN                   : 1;
		uint32_t CH1_TRIG_INT_EN                 : 1;
		uint32_t CH1_DROP_INT_EN                 : 1;
		uint32_t CH1_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH2_VS_INT_EN                   : 1;
		uint32_t CH2_TRIG_INT_EN                 : 1;
		uint32_t CH2_DROP_INT_EN                 : 1;
		uint32_t CH2_SIZE_ERROR_INT_EN           : 1;
		uint32_t CH3_VS_INT_EN                   : 1;
		uint32_t CH3_TRIG_INT_EN                 : 1;
		uint32_t CH3_DROP_INT_EN                 : 1;
		uint32_t CH3_SIZE_ERROR_INT_EN           : 1;
		uint32_t _rsv_16                         : 12;
		uint32_t SLICE_LINE_INTP_EN              : 1;
		uint32_t DMA_ERROR_INTP_EN               : 1;
		uint32_t LINE_INTP_EN                    : 1;
		uint32_t FIFO_OVERFLOW_INT_EN            : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DMA_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DMA_ST_DPCM_MODE                : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t DPCM_MIPI_OPT                   : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t DROP_MODE                       : 4;
		uint32_t AVG_MODE                        : 4;
		uint32_t DPCM_XSTR                       : 13;
	} bits;
};

union REG_ISP_CSI_BDG_DMA_LD_DPCM_MODE {
	uint32_t raw;
	struct {
		uint32_t DMA_LD_DPCM_MODE                : 3;
		uint32_t _rsv_3                          : 13;
		uint32_t DPCM_RX_XSTR                    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH1_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH2_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_SIZE {
	uint32_t raw;
	struct {
		uint32_t CH3_FRAME_WIDTHM1               : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_FRAME_HEIGHTM1              : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH0_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH0_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH0_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_BLC_SUM {
	uint32_t raw;
	struct {
		uint32_t CH0_BLC_SUM                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH1_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH1_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH1_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_BLC_SUM {
	uint32_t raw;
	struct {
		uint32_t CH1_BLC_SUM                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH2_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH2_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH2_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH2_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_BLC_SUM {
	uint32_t raw;
	struct {
		uint32_t CH2_BLC_SUM                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t CH3_CROP_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t CH3_HORZ_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_HORZ_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t CH3_VERT_CROP_START             : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_VERT_CROP_END               : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_BLC_SUM {
	uint32_t raw;
	struct {
		uint32_t CH3_BLC_SUM                     : 32;
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

union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_INTP_HEIGHTM1          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_LINE_INTP_HEIGHTM1          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH0_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH0_VS_CNT                      : 16;
		uint32_t CH0_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH0_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH0_TOT_BLK_IDLE                : 1;
		uint32_t CH0_TOT_DMA_IDLE                : 1;
		uint32_t CH0_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH1_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH1_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH1_VS_CNT                      : 16;
		uint32_t CH1_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH1_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH1_TOT_BLK_IDLE                : 1;
		uint32_t CH1_TOT_DMA_IDLE                : 1;
		uint32_t CH1_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH2_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH2_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH2_VS_CNT                      : 16;
		uint32_t CH2_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH2_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH2_TOT_BLK_IDLE                : 1;
		uint32_t CH2_TOT_DMA_IDLE                : 1;
		uint32_t CH2_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_DEBUG_0 {
	uint32_t raw;
	struct {
		uint32_t CH3_PXL_CNT                     : 32;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_DEBUG_1 {
	uint32_t raw;
	struct {
		uint32_t CH3_LINE_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_DEBUG_2 {
	uint32_t raw;
	struct {
		uint32_t CH3_VS_CNT                      : 16;
		uint32_t CH3_TRIG_CNT                    : 16;
	} bits;
};

union REG_ISP_CSI_BDG_CH3_DEBUG_3 {
	uint32_t raw;
	struct {
		uint32_t CH3_TOT_BLK_IDLE                : 1;
		uint32_t CH3_TOT_DMA_IDLE                : 1;
		uint32_t CH3_BDG_DMA_IDLE                : 1;
	} bits;
};

union REG_ISP_CSI_BDG_INTERRUPT_STATUS_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_FRAME_DROP_INT              : 1;
		uint32_t CH0_VS_INT                      : 1;
		uint32_t CH0_TRIG_INT                    : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t CH0_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH0_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH0_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH0_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH1_FRAME_DROP_INT              : 1;
		uint32_t CH1_VS_INT                      : 1;
		uint32_t CH1_TRIG_INT                    : 1;
		uint32_t _rsv_11                         : 1;
		uint32_t CH1_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH1_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH1_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH1_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH2_FRAME_DROP_INT              : 1;
		uint32_t CH2_VS_INT                      : 1;
		uint32_t CH2_TRIG_INT                    : 1;
		uint32_t _rsv_19                         : 1;
		uint32_t CH2_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH2_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH2_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH2_FRAME_HEIGHT_LS_INT         : 1;
		uint32_t CH3_FRAME_DROP_INT              : 1;
		uint32_t CH3_VS_INT                      : 1;
		uint32_t CH3_TRIG_INT                    : 1;
		uint32_t _rsv_27                         : 1;
		uint32_t CH3_FRAME_WIDTH_GT_INT          : 1;
		uint32_t CH3_FRAME_WIDTH_LS_INT          : 1;
		uint32_t CH3_FRAME_HEIGHT_GT_INT         : 1;
		uint32_t CH3_FRAME_HEIGHT_LS_INT         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_INTERRUPT_STATUS_1 {
	uint32_t raw;
	struct {
		uint32_t FIFO_OVERFLOW_INT               : 1;
		uint32_t FRAME_RESOLUTION_OVER_MAX_INT   : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t DMA_ERROR_INT                   : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CH0_LINE_INTP_INT               : 1;
		uint32_t CH1_LINE_INTP_INT               : 1;
		uint32_t CH2_LINE_INTP_INT               : 1;
		uint32_t CH3_LINE_INTP_INT               : 1;
		uint32_t CH0_SLICE_LINE_INTP_INT         : 1;
		uint32_t CH1_SLICE_LINE_INTP_INT         : 1;
		uint32_t CH2_SLICE_LINE_INTP_INT         : 1;
		uint32_t CH3_SLICE_LINE_INTP_INT         : 1;
	} bits;
};

union REG_ISP_CSI_BDG_DEBUG {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_IDLE                  : 1;
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

union REG_ISP_CSI_BDG_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY_IN                        : 16;
		uint32_t DUMMY_OUT                       : 16;
	} bits;
};

union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_1 {
	uint32_t raw;
	struct {
		uint32_t CH2_LINE_INTP_HEIGHTM1          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_LINE_INTP_HEIGHTM1          : 13;
	} bits;
};

union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_0 {
	uint32_t raw;
	struct {
		uint32_t CH0_SLICE_LINE_INTP_HEIGHTM1    : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH1_SLICE_LINE_INTP_HEIGHTM1    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_1 {
	uint32_t raw;
	struct {
		uint32_t CH2_SLICE_LINE_INTP_HEIGHTM1    : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CH3_SLICE_LINE_INTP_HEIGHTM1    : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH0_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_CROP_EN                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH0_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_HORZ_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH0_HORZ_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH0_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH0_VERT_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH0_VERT_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH1_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t ST_CH1_CROP_EN                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH1_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH1_HORZ_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH1_HORZ_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH1_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH1_VERT_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH1_VERT_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH2_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t ST_CH2_CROP_EN                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH2_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH2_HORZ_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH2_HORZ_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH2_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH2_VERT_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH2_VERT_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH3_CROP_EN {
	uint32_t raw;
	struct {
		uint32_t ST_CH3_CROP_EN                  : 1;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH3_HORZ_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH3_HORZ_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH3_HORZ_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_WDMA_CH3_VERT_CROP {
	uint32_t raw;
	struct {
		uint32_t ST_CH3_VERT_CROP_START          : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t ST_CH3_VERT_CROP_END            : 13;
	} bits;
};

union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_0 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_EN                     : 1;
	} bits;
};

union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_1 {
	uint32_t raw;
	struct {
		uint32_t TRIG_DLY_VALUE                  : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_LCAC_REG00 {
	uint32_t raw;
	struct {
		uint32_t LCAC_ENABLE                     : 1;
		uint32_t LCAC_SHDW_SEL                   : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t LCAC_OUT_SEL                    : 3;
	} bits;
};

union REG_ISP_LCAC_REG04 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_STR_R1                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t LCAC_LTI_STR_B1                 : 12;
	} bits;
};

union REG_ISP_LCAC_REG08 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_STR_R2_LE              : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t LCAC_LTI_STR_B2_LE              : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t LCAC_LTI_WGT_R_LE               : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t LCAC_LTI_WGT_B_LE               : 7;
	} bits;
};

union REG_ISP_LCAC_REG0C {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_STR_R2_SE              : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t LCAC_LTI_STR_B2_SE              : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t LCAC_LTI_WGT_R_SE               : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t LCAC_LTI_WGT_B_SE               : 7;
	} bits;
};

union REG_ISP_LCAC_REG10 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_KERNEL_R0              : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LCAC_LTI_KERNEL_R1              : 10;
	} bits;
};

union REG_ISP_LCAC_REG14 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_KERNEL_R2              : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LCAC_LTI_KERNEL_B0              : 10;
	} bits;
};

union REG_ISP_LCAC_REG18 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_KERNEL_B1              : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t LCAC_LTI_KERNEL_B2              : 10;
	} bits;
};

union REG_ISP_LCAC_REG1C {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_EDGE_SCALE_R_LE        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t LCAC_LTI_EDGE_SCALE_G_LE        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t LCAC_LTI_EDGE_SCALE_B_LE        : 6;
	} bits;
};

union REG_ISP_LCAC_REG20 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_EDGE_SCALE_R_SE        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t LCAC_LTI_EDGE_SCALE_G_SE        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t LCAC_LTI_EDGE_SCALE_B_SE        : 6;
	} bits;
};

union REG_ISP_LCAC_REG24 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_EDGE_CORING_R          : 8;
		uint32_t LCAC_LTI_EDGE_CORING_G          : 8;
		uint32_t LCAC_LTI_EDGE_CORING_B          : 8;
	} bits;
};

union REG_ISP_LCAC_REG28 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_WGT_MAX_R              : 8;
		uint32_t LCAC_LTI_WGT_MIN_R              : 8;
		uint32_t LCAC_LTI_WGT_MAX_B              : 8;
		uint32_t LCAC_LTI_WGT_MIN_B              : 8;
	} bits;
};

union REG_ISP_LCAC_REG2C {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_VAR_WGT_R              : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t LCAC_LTI_VAR_WGT_B              : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t LCAC_FILTER_SCALE               : 4;
		uint32_t _rsv_20                         : 4;
		uint32_t LCAC_FCF_LUMA_BLEND_WGT         : 7;
	} bits;
};

union REG_ISP_LCAC_REG30 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_EDGE_SCALE_R_LE        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_G_LE        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_B_LE        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_Y_LE        : 6;
	} bits;
};

union REG_ISP_LCAC_REG34 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_EDGE_SCALE_R_SE        : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_G_SE        : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_B_SE        : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t LCAC_FCF_EDGE_SCALE_Y_SE        : 6;
	} bits;
};

union REG_ISP_LCAC_REG38 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_EDGE_CORING_R          : 8;
		uint32_t LCAC_FCF_EDGE_CORING_G            : 8;
		uint32_t LCAC_FCF_EDGE_CORING_B            : 8;
		uint32_t LCAC_FCF_EDGE_CORING_Y            : 8;
	} bits;
};

union REG_ISP_LCAC_REG3C {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_WGT_MAX_R                : 8;
		uint32_t LCAC_FCF_WGT_MIN_R                : 8;
		uint32_t LCAC_FCF_WGT_MAX_B                : 8;
		uint32_t LCAC_FCF_WGT_MIN_B                : 8;
	} bits;
};

union REG_ISP_LCAC_REG40 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_VAR_WGT_R                : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t LCAC_FCF_VAR_WGT_G                : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t LCAC_FCF_VAR_WGT_B                : 5;
	} bits;
};

union REG_ISP_LCAC_REG44 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_FILTER_KERNEL_00         : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t LCAC_FCF_FILTER_KERNEL_01         : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t LCAC_FCF_FILTER_KERNEL_02         : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t LCAC_FCF_FILTER_KERNEL_03         : 7;
	} bits;
};

union REG_ISP_LCAC_REG48 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LUMA_KERNEL_00               : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t LCAC_LUMA_KERNEL_01               : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t LCAC_LUMA_KERNEL_02               : 7;
		uint32_t _rsv_23                         : 1;
		uint32_t LCAC_LUMA_KERNEL_03               : 7;
	} bits;
};

union REG_ISP_LCAC_REG4C {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_FILTER_KERNEL_04       : 7;
		uint32_t _rsv_7                          : 1;
		uint32_t LCAC_LUMA_KERNEL_04             : 7;
	} bits;
};

union REG_ISP_LCAC_REG50 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_00            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_01            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_02            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_03            : 8;
	} bits;
};

union REG_ISP_LCAC_REG54 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_04            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_05            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_06            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_07            : 8;
	} bits;
};

union REG_ISP_LCAC_REG58 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_08            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_09            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_10            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_11            : 8;
	} bits;
};

union REG_ISP_LCAC_REG5C {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_12            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_13            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_14            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_15            : 8;
	} bits;
};

union REG_ISP_LCAC_REG60 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_16            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_17            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_18            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_19            : 8;
	} bits;
};

union REG_ISP_LCAC_REG64 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_20            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_21            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_22            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_23            : 8;
	} bits;
};

union REG_ISP_LCAC_REG68 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_24            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_25            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_26            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_27            : 8;
	} bits;
};

union REG_ISP_LCAC_REG6C {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_28            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_29            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_30            : 8;
		uint32_t LCAC_LTI_LUMA_LUT_31            : 8;
	} bits;
};

union REG_ISP_LCAC_REG70 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_00            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_01            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_02            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_03            : 8;
	} bits;
};

union REG_ISP_LCAC_REG74 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_04            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_05            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_06            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_07            : 8;
	} bits;
};

union REG_ISP_LCAC_REG78 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_08            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_09            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_10            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_11            : 8;
	} bits;
};

union REG_ISP_LCAC_REG7C {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_12            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_13            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_14            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_15            : 8;
	} bits;
};

union REG_ISP_LCAC_REG80 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_16            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_17            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_18            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_19            : 8;
	} bits;
};

union REG_ISP_LCAC_REG84 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_20            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_21            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_22            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_23            : 8;
	} bits;
};

union REG_ISP_LCAC_REG88 {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_24            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_25            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_26            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_27            : 8;
	} bits;
};

union REG_ISP_LCAC_REG8C {
	uint32_t raw;
	struct {
		uint32_t LCAC_FCF_LUMA_LUT_28            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_29            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_30            : 8;
		uint32_t LCAC_FCF_LUMA_LUT_31            : 8;
	} bits;
};

union REG_ISP_LCAC_REG90 {
	uint32_t raw;
	struct {
		uint32_t LCAC_LTI_LUMA_LUT_32            : 8;
		uint32_t _rsv_8                          : 8;
		uint32_t LCAC_FCF_LUMA_LUT_32            : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_ISP_CFA_00 {
	uint32_t raw;
	struct {
		uint32_t CFA_SHDW_SEL                    : 1;
		uint32_t CFA_ENABLE                      : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t CFA_YMOIRE_ENABLE               : 1;
		uint32_t DELAY                           : 1;
		uint32_t FORCE_CLK_ENABLE                : 1;
		uint32_t CFA_FORCE_DIR_ENABLE            : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t CFA_FORCE_DIR_SEL               : 2;
	} bits;
};

union REG_ISP_CFA_04 {
	uint32_t raw;
	struct {
		uint32_t CFA_OUT_SEL                     : 1;
		uint32_t CONT_EN                         : 1;
		uint32_t _rsv_2                          : 1;
		uint32_t SOFTRST                         : 1;
		uint32_t _rsv_4                          : 1;
		uint32_t DBG_EN                          : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t CFA_EDGEE_THD2                  : 12;
	} bits;
};

union REG_ISP_CFA_0C {
	uint32_t raw;
	struct {
		uint32_t CFA_EDGEE_THD                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_SIGE_THD                    : 12;
	} bits;
};

union REG_ISP_CFA_10 {
	uint32_t raw;
	struct {
		uint32_t CFA_GSIG_TOL                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_RBSIG_TOL                   : 12;
	} bits;
};

union REG_ISP_CFA_14 {
	uint32_t raw;
	struct {
		uint32_t CFA_EDGE_TOL                    : 12;
	} bits;
};

union REG_ISP_CFA_18 {
	uint32_t raw;
	struct {
		uint32_t CFA_GHP_THD                     : 16;
	} bits;
};

union REG_ISP_CFA_1C {
	uint32_t raw;
	struct {
		uint32_t CFA_RB_VT_ENABLE                : 1;
	} bits;
};

union REG_ISP_CFA_20 {
	uint32_t raw;
	struct {
		uint32_t CFA_RBSIG_LUMA_THD              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_VW_THD                      : 12;
	} bits;
};

union REG_ISP_CFA_30 {
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

union REG_ISP_CFA_34 {
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

union REG_ISP_CFA_38 {
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

union REG_ISP_CFA_3C {
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

union REG_ISP_CFA_40 {
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

union REG_ISP_CFA_44 {
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

union REG_ISP_CFA_48 {
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

union REG_ISP_CFA_4C {
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

union REG_ISP_CFA_70 {
	uint32_t raw;
	struct {
		uint32_t DIR_READCNT_FROM_LINE0          : 5;
	} bits;
};

union REG_ISP_CFA_74 {
	uint32_t raw;
	struct {
		uint32_t PROB_OUT_SEL                    : 4;
		uint32_t PROB_PERFMT                     : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t PROB_FMT                        : 6;
	} bits;
};

union REG_ISP_CFA_78 {
	uint32_t raw;
	struct {
		uint32_t PROB_LINE                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t PROB_PIX                        : 12;
	} bits;
};

union REG_ISP_CFA_7C {
	uint32_t raw;
	struct {
		uint32_t CFA_DBG0                        : 32;
	} bits;
};

union REG_ISP_CFA_80 {
	uint32_t raw;
	struct {
		uint32_t CFA_DBG1                        : 32;
	} bits;
};

union REG_ISP_CFA_90 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_REF_MAXG_ONLY        : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t CFA_YMOIRE_NP                   : 8;
	} bits;
};

union REG_ISP_CFA_94 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_DETAIL_TH            : 8;
		uint32_t CFA_YMOIRE_DETAIL_LOW           : 9;
	} bits;
};

union REG_ISP_CFA_98 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_DETAIL_HIGH          : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CFA_YMOIRE_DETAIL_SLOPE         : 15;
	} bits;
};

union REG_ISP_CFA_9C {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_EDGE_TH              : 8;
		uint32_t CFA_YMOIRE_EDGE_LOW             : 9;
	} bits;
};

union REG_ISP_CFA_A0 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_EDGE_HIGH            : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CFA_YMOIRE_EDGE_SLOPE           : 15;
	} bits;
};

union REG_ISP_CFA_A4 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_LUT_TH               : 10;
		uint32_t _rsv_10                         : 6;
		uint32_t CFA_YMOIRE_LUT_LOW              : 9;
	} bits;
};

union REG_ISP_CFA_A8 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_LUT_HIGH             : 9;
		uint32_t _rsv_9                          : 7;
		uint32_t CFA_YMOIRE_LUT_SLOPE            : 15;
	} bits;
};

union REG_ISP_CFA_110 {
	uint32_t raw;
	struct {
		uint32_t CFA_YMOIRE_LPF_W                : 8;
		uint32_t CFA_YMOIRE_DC_W                 : 8;
	} bits;
};

union REG_ISP_CFA_120 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_ENABLE               : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t CFA_CMOIRE_STRTH                : 8;
	} bits;
};

union REG_ISP_CFA_124 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_SAT_X0               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_SAT_Y0               : 12;
	} bits;
};

union REG_ISP_CFA_128 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_SAT_SLP0             : 18;
	} bits;
};

union REG_ISP_CFA_12C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_SAT_X1               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_SAT_Y1               : 12;
	} bits;
};

union REG_ISP_CFA_130 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PTCLR_X0             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_PTCLR_Y0             : 12;
	} bits;
};

union REG_ISP_CFA_134 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PTCLR_SLP0           : 18;
	} bits;
};

union REG_ISP_CFA_138 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PTCLR_X1             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_PTCLR_Y1             : 12;
	} bits;
};

union REG_ISP_CFA_13C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PROTCLR1_ENABLE      : 1;
		uint32_t CFA_CMOIRE_PROTCLR2_ENABLE      : 1;
		uint32_t CFA_CMOIRE_PROTCLR3_ENABLE      : 1;
	} bits;
};

union REG_ISP_CFA_140 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PROTCLR1             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_PROTCLR2             : 12;
	} bits;
};

union REG_ISP_CFA_144 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PROTCLR3             : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_PD_X0                : 12;
	} bits;
};

union REG_ISP_CFA_148 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PD_Y0                : 12;
	} bits;
};

union REG_ISP_CFA_14C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PD_SLP0              : 18;
	} bits;
};

union REG_ISP_CFA_150 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_PD_X1                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_PD_Y1                : 12;
	} bits;
};

union REG_ISP_CFA_154 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_X0              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_EDGE_Y0              : 12;
	} bits;
};

union REG_ISP_CFA_158 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_SLP0            : 18;
	} bits;
};

union REG_ISP_CFA_15C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_X1              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CFA_CMOIRE_EDGE_Y1              : 12;
	} bits;
};

union REG_ISP_CFA_160 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_LUMAGAIN_ENABLE      : 1;
	} bits;
};

union REG_ISP_CFA_164 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_LUMATG               : 12;
	} bits;
};

union REG_ISP_CFA_168 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D0C0            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D0C1            : 13;
	} bits;
};

union REG_ISP_CFA_16C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D0C2            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D45C0           : 13;
	} bits;
};

union REG_ISP_CFA_170 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D45C1           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D45C2           : 13;
	} bits;
};

union REG_ISP_CFA_174 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D45C3           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D45C4           : 13;
	} bits;
};

union REG_ISP_CFA_178 {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D45C5           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D45C6           : 13;
	} bits;
};

union REG_ISP_CFA_17C {
	uint32_t raw;
	struct {
		uint32_t CFA_CMOIRE_EDGE_D45C7           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t CFA_CMOIRE_EDGE_D45C8           : 13;
	} bits;
};

union REG_ISP_CFA_180 {
	uint32_t raw;
	struct {
		uint32_t _CFA_SHPN_ENABLE                : 1;
		uint32_t _CFA_SHPN_PRE_PROC_ENABLE       : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t _CFA_SHPN_MIN_Y                 : 12;
	} bits;
};

union REG_ISP_CFA_184 {
	uint32_t raw;
	struct {
		uint32_t _CFA_SHPN_MIN_GAIN              : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t _CFA_SHPN_MAX_GAIN              : 12;
	} bits;
};

union REG_ISP_CFA_188 {
	uint32_t raw;
	struct {
		uint32_t CFA_SHPN_MF_CORE_GAIN           : 8;
		uint32_t CFA_SHPN_HF_BLEND_WGT           : 8;
		uint32_t CFA_SHPN_MF_BLEND_WGT           : 8;
	} bits;
};

union REG_ISP_CFA_18C {
	uint32_t raw;
	struct {
		uint32_t CFA_SHPN_CORE_VALUE             : 12;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_PRE_RAW_VI_SEL_0 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_RESET                 : 1;
		uint32_t RING_BUFF_MONITOR_EN            : 1;
		uint32_t DMA_LD_DPCM_MODE                : 3;
		uint32_t _rsv_5                          : 11;
		uint32_t DPCM_RX_XSTR                    : 13;
	} bits;
};

union REG_PRE_RAW_VI_SEL_1 {
	uint32_t raw;
	struct {
		uint32_t FRAME_WIDTHM1                   : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t FRAME_HEIGHTM1                  : 13;
	} bits;
};

union REG_PRE_RAW_VI_SEL_2 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_THRESHOLD_0           : 16;
		uint32_t RING_BUFF_THRESHOLD_1           : 16;
	} bits;
};

union REG_PRE_RAW_VI_SEL_3 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_THRESHOLD_2           : 16;
		uint32_t RING_BUFF_THRESHOLD_3           : 16;
	} bits;
};

union REG_PRE_RAW_VI_SEL_4 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_ABOVE_TH_0            : 32;
	} bits;
};

union REG_PRE_RAW_VI_SEL_5 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_ABOVE_TH_1            : 32;
	} bits;
};

union REG_PRE_RAW_VI_SEL_6 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_ABOVE_TH_2            : 32;
	} bits;
};

union REG_PRE_RAW_VI_SEL_7 {
	uint32_t raw;
	struct {
		uint32_t RING_BUFF_ABOVE_TH_3            : 32;
	} bits;
};

union REG_PRE_RAW_VI_SEL_8 {
	uint32_t raw;
	struct {
		uint32_t IP_DMA_IDLE                     : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_YUV_TOP_YUV_0 {
	uint32_t raw;
	struct {
		uint32_t _rsv_0                          : 8;
		uint32_t YUV_TOP_SEL                     : 1;
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
		uint32_t _rsv_0                          : 2;
		uint32_t YONLY_EN                        : 1;
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
		uint32_t MA_IDLE                         : 16;
		uint32_t _rsv_16                         : 15;
		uint32_t IDLE                            : 1;
	} bits;
};

union REG_YUV_TOP_YUV_5 {
	uint32_t raw;
	struct {
		uint32_t DIS_UV2DRAM                     : 1;
		uint32_t LINE_THRES_EN                   : 1;
		uint32_t _rsv_2                          : 6;
		uint32_t LINE_THRES                      : 12;
		uint32_t _rsv_20                         : 4;
		uint32_t PG2_ENABLE                      : 1;
	} bits;
};

union REG_YUV_TOP_YUV_CTRL {
	uint32_t raw;
	struct {
		uint32_t CHECKSUM_ENABLE                 : 1;
		uint32_t SC_DMA_SWITCH                   : 1;
		uint32_t AVG_MODE                        : 1;
		uint32_t BYPASS_H                        : 1;
		uint32_t BYPASS_V                        : 1;
		uint32_t DROP_MODE                       : 1;
		uint32_t YC_SWAP                         : 1;
		uint32_t CURSER2_EN                      : 1;
		uint32_t GUARD_CNT                       : 8;
	} bits;
};

union REG_YUV_TOP_IMGW_M1 {
	uint32_t raw;
	struct {
		uint32_t YUV_TOP_IMGW_M1                 : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t YUV_TOP_IMGH_M1                 : 13;
	} bits;
};

union REG_YUV_TOP_STVALID_STATUS {
	uint32_t raw;
	struct {
		uint32_t STVALID_STATUS                  : 32;
	} bits;
};

union REG_YUV_TOP_STREADY_STATUS {
	uint32_t raw;
	struct {
		uint32_t STREADY_STATUS                  : 32;
	} bits;
};

union REG_YUV_TOP_PATGEN1 {
	uint32_t raw;
	struct {
		uint32_t X_CURSER                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t Y_CURSER                        : 14;
		uint32_t CURSER_EN                       : 1;
		uint32_t PG_ENABLE                       : 1;
	} bits;
};

union REG_YUV_TOP_PATGEN2 {
	uint32_t raw;
	struct {
		uint32_t CURSER_VALUE                    : 16;
	} bits;
};

union REG_YUV_TOP_PATGEN3 {
	uint32_t raw;
	struct {
		uint32_t VALUE_REPORT                    : 32;
	} bits;
};

union REG_YUV_TOP_PATGEN4 {
	uint32_t raw;
	struct {
		uint32_t XCNT_RPT                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t YCNT_RPT                        : 14;
	} bits;
};

union REG_YUV_TOP_CHECK_SUM {
	uint32_t raw;
	struct {
		uint32_t K_SUM                           : 32;
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
		uint32_t _rsv_1                          : 3;
		uint32_t LSC_GAIN_3P9_0_4P8_1            : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t LSC_GAIN_BICUBIC_0_BILINEAR_1   : 1;
		uint32_t LSC_BOUNDARY_INTERPOLATION_MODE : 1;
		uint32_t _rsv_10                         : 2;
		uint32_t LSC_RENORMALIZE_ENABLE          : 1;
		uint32_t _rsv_13                         : 3;
		uint32_t LSC_HDR_ENABLE                  : 1;
		uint32_t _rsv_17                         : 3;
		uint32_t LSC_BLOCKING_GAIN_UPDATE_ENABLE : 1;
		uint32_t _rsv_21                         : 3;
		uint32_t LSC_35TILE_ENABLE               : 1;
		uint32_t _rsv_25                         : 3;
		uint32_t FORCE_CLK_ENABLE                : 1;
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
		uint32_t IMG_BAYERID                     : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t FORCE_BAYER_ENABLE              : 1;
	} bits;
};

union REG_ISP_LSC_MONITOR_SELECT {
	uint32_t raw;
	struct {
		uint32_t LSC_MONITOR_SELECT              : 32;
	} bits;
};

union REG_ISP_LSC_DMI_WIDTHM1 {
	uint32_t raw;
	struct {
		uint32_t LSC_DMI_WIDTHM1                 : 13;
	} bits;
};

union REG_ISP_LSC_DMI_HEIGHTM1 {
	uint32_t raw;
	struct {
		uint32_t LSC_DMI_HEIGHTM1                : 13;
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

union REG_ISP_LSC_INITX0 {
	uint32_t raw;
	struct {
		uint32_t LSC_INITX0                      : 22;
	} bits;
};

union REG_ISP_LSC_INITY0 {
	uint32_t raw;
	struct {
		uint32_t LSC_INITY0                      : 22;
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

union REG_ISP_LSC_INTERPOLATION {
	uint32_t raw;
	struct {
		uint32_t LSC_BOUNDARY_INTERPOLATION_LF_RANGE: 6;
		uint32_t _rsv_6                          : 2;
		uint32_t LSC_BOUNDARY_INTERPOLATION_UP_RANGE: 6;
		uint32_t _rsv_14                         : 2;
		uint32_t LSC_BOUNDARY_INTERPOLATION_RT_RANGE: 6;
		uint32_t _rsv_22                         : 2;
		uint32_t LSC_BOUNDARY_INTERPOLATION_DN_RANGE: 6;
	} bits;
};

union REG_ISP_LSC_DMI_ENABLE {
	uint32_t raw;
	struct {
		uint32_t DMI_ENABLE                      : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t DMI_QOS                         : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t FORCE_DMA_DISABLE               : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t DMI_PULL_AFTER_DONE             : 1;
	} bits;
};

union REG_ISP_LSC_BLD {
	uint32_t raw;
	struct {
		uint32_t LSC_BLDRATIO_ENABLE             : 1;
		uint32_t _rsv_1                          : 15;
		uint32_t LSC_BLDRATIO                    : 9;
	} bits;
};

union REG_ISP_LSC_INTP_GAIN_MAX {
	uint32_t raw;
	struct {
		uint32_t LSC_INTP_GAIN_MAX               : 26;
	} bits;
};

union REG_ISP_LSC_INTP_GAIN_MIN {
	uint32_t raw;
	struct {
		uint32_t LSC_INTP_GAIN_MIN               : 26;
	} bits;
};

#ifdef __cplusplus
}
#endif

#endif /* _VI_REG_FIELDS_H_ */
