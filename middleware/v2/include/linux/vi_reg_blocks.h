/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name:vi_reg_blocks.h
 * Description:HW register description
 */

#ifndef _VI_REG_BLOCKS_H_
#define _VI_REG_BLOCKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_PRE_RAW_BE_T {
	union REG_PRE_RAW_BE_TOP_CTRL           TOP_CTRL;
	union REG_PRE_RAW_BE_UP_PQ_EN           UP_PQ_EN;
	union REG_PRE_RAW_BE_IMG_SIZE_LE        IMG_SIZE_LE;
	uint32_t                                _resv_0xc[1];
	union REG_PRE_RAW_BE_PRE_RAW_DUMMY      PRE_RAW_DUMMY;
	union REG_PRE_RAW_BE_DEBUG_INFO         BE_INFO;
	union REG_PRE_RAW_BE_DMA_IDLE_INFO      BE_DMA_IDLE_INFO;
	union REG_PRE_RAW_BE_IP_IDLE_INFO       BE_IP_IDLE_INFO;
	union REG_PRE_RAW_BE_LINE_BALANCE_CTRL  LINE_BALANCE_CTRL;
	union REG_PRE_RAW_BE_DEBUG_ENABLE       DEBUG_ENABLE;
	union REG_PRE_RAW_BE_TVALID_STATUS      TVALID_STATUS;
	union REG_PRE_RAW_BE_TREADY_STATUS      TREADY_STATUS;
	union REG_PRE_RAW_BE_PATGEN1            PATGEN1;
	union REG_PRE_RAW_BE_PATGEN2            PATGEN2;
	union REG_PRE_RAW_BE_PATGEN3            PATGEN3;
	union REG_PRE_RAW_BE_PATGEN4            PATGEN4;
	union REG_PRE_RAW_BE_CHKSUM_ENABLE      CHKSUM_ENABLE;
	union REG_PRE_RAW_BE_CHKSUM             CHKSUM;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DMA_CTL_T {
	union REG_ISP_DMA_CTL_SYS_CONTROL       SYS_CONTROL;
	union REG_ISP_DMA_CTL_BASE_ADDR         BASE_ADDR;
	union REG_ISP_DMA_CTL_DMA_SEGLEN        DMA_SEGLEN;
	union REG_ISP_DMA_CTL_DMA_STRIDE        DMA_STRIDE;
	union REG_ISP_DMA_CTL_DMA_SEGNUM        DMA_SEGNUM;
	union REG_ISP_DMA_CTL_DMA_STATUS        DMA_STATUS;
	union REG_ISP_DMA_CTL_DMA_SLICESIZE     DMA_SLICESIZE;
	union REG_ISP_DMA_CTL_DMA_DUMMY         DMA_DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_AE_HIST_T {
	union REG_ISP_AE_HIST_AE_HIST_STATUS    AE_HIST_STATUS;
	union REG_ISP_AE_HIST_AE_HIST_GRACE_RESET  AE_HIST_GRACE_RESET;
	union REG_ISP_AE_HIST_AE_HIST_MONITOR   AE_HIST_MONITOR;
	union REG_ISP_AE_HIST_AE_HIST_BYPASS    AE_HIST_BYPASS;
	union REG_ISP_AE_HIST_AE_KICKOFF        AE_KICKOFF;
	union REG_ISP_AE_HIST_STS_AE0_HIST_ENABLE  STS_AE0_HIST_ENABLE;
	union REG_ISP_AE_HIST_STS_AE_OFFSETX    STS_AE_OFFSETX;
	union REG_ISP_AE_HIST_STS_AE_OFFSETY    STS_AE_OFFSETY;
	union REG_ISP_AE_HIST_STS_AE_NUMXM1     STS_AE_NUMXM1;
	union REG_ISP_AE_HIST_STS_AE_NUMYM1     STS_AE_NUMYM1;
	union REG_ISP_AE_HIST_STS_AE_WIDTH      STS_AE_WIDTH;
	union REG_ISP_AE_HIST_STS_AE_HEIGHT     STS_AE_HEIGHT;
	union REG_ISP_AE_HIST_STS_AE_STS_DIV    STS_AE_STS_DIV;
	union REG_ISP_AE_HIST_STS_HIST_MODE     STS_HIST_MODE;
	uint32_t                                _resv_0x38[1];
	union REG_ISP_AE_HIST_AE_HIST_MONITOR_SELECT  AE_HIST_MONITOR_SELECT;
	union REG_ISP_AE_HIST_AE_HIST_LOCATION  AE_HIST_LOCATION;
	uint32_t                                _resv_0x44[1];
	union REG_ISP_AE_HIST_STS_IR_AE_OFFSETX  STS_IR_AE_OFFSETX;
	union REG_ISP_AE_HIST_STS_IR_AE_OFFSETY  STS_IR_AE_OFFSETY;
	union REG_ISP_AE_HIST_STS_IR_AE_NUMXM1  STS_IR_AE_NUMXM1;
	union REG_ISP_AE_HIST_STS_IR_AE_NUMYM1  STS_IR_AE_NUMYM1;
	union REG_ISP_AE_HIST_STS_IR_AE_WIDTH   STS_IR_AE_WIDTH;
	union REG_ISP_AE_HIST_STS_IR_AE_HEIGHT  STS_IR_AE_HEIGHT;
	union REG_ISP_AE_HIST_STS_IR_AE_STS_DIV  STS_IR_AE_STS_DIV;
	uint32_t                                _resv_0x64[1];
	union REG_ISP_AE_HIST_AE_HIST_BAYER_STARTING  AE_HIST_BAYER_STARTING;
	union REG_ISP_AE_HIST_AE_HIST_DUMMY     AE_HIST_DUMMY;
	union REG_ISP_AE_HIST_AE_HIST_CHECKSUM  AE_HIST_CHECKSUM;
	union REG_ISP_AE_HIST_WBG_4             WBG_4;
	union REG_ISP_AE_HIST_WBG_5             WBG_5;
	union REG_ISP_AE_HIST_WBG_6             WBG_6;
	union REG_ISP_AE_HIST_WBG_7             WBG_7;
	uint32_t                                _resv_0x84[7];
	union REG_ISP_AE_HIST_DMI_ENABLE        DMI_ENABLE;
	uint32_t                                _resv_0xa4[3];
	union REG_ISP_AE_HIST_AE_FACE0_LOCATION  AE_FACE0_LOCATION;
	union REG_ISP_AE_HIST_AE_FACE1_LOCATION  AE_FACE1_LOCATION;
	union REG_ISP_AE_HIST_AE_FACE2_LOCATION  AE_FACE2_LOCATION;
	union REG_ISP_AE_HIST_AE_FACE3_LOCATION  AE_FACE3_LOCATION;
	union REG_ISP_AE_HIST_AE_FACE0_SIZE     AE_FACE0_SIZE;
	union REG_ISP_AE_HIST_AE_FACE1_SIZE     AE_FACE1_SIZE;
	union REG_ISP_AE_HIST_AE_FACE2_SIZE     AE_FACE2_SIZE;
	union REG_ISP_AE_HIST_AE_FACE3_SIZE     AE_FACE3_SIZE;
	union REG_ISP_AE_HIST_IR_AE_FACE0_LOCATION  IR_AE_FACE0_LOCATION;
	union REG_ISP_AE_HIST_IR_AE_FACE1_LOCATION  IR_AE_FACE1_LOCATION;
	union REG_ISP_AE_HIST_IR_AE_FACE2_LOCATION  IR_AE_FACE2_LOCATION;
	union REG_ISP_AE_HIST_IR_AE_FACE3_LOCATION  IR_AE_FACE3_LOCATION;
	union REG_ISP_AE_HIST_IR_AE_FACE0_SIZE  IR_AE_FACE0_SIZE;
	union REG_ISP_AE_HIST_IR_AE_FACE1_SIZE  IR_AE_FACE1_SIZE;
	union REG_ISP_AE_HIST_IR_AE_FACE2_SIZE  IR_AE_FACE2_SIZE;
	union REG_ISP_AE_HIST_IR_AE_FACE3_SIZE  IR_AE_FACE3_SIZE;
	union REG_ISP_AE_HIST_AE_FACE0_ENABLE   AE_FACE0_ENABLE;
	union REG_ISP_AE_HIST_AE_FACE0_STS_DIV  AE_FACE0_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE1_STS_DIV  AE_FACE1_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE2_STS_DIV  AE_FACE2_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE3_STS_DIV  AE_FACE3_STS_DIV;
	union REG_ISP_AE_HIST_STS_ENABLE        STS_ENABLE;
	union REG_ISP_AE_HIST_AE_ALGO_ENABLE    AE_ALGO_ENABLE;
	union REG_ISP_AE_HIST_AE_HIST_LOW       AE_HIST_LOW;
	union REG_ISP_AE_HIST_AE_HIST_HIGH      AE_HIST_HIGH;
	union REG_ISP_AE_HIST_AE_TOP            AE_TOP;
	union REG_ISP_AE_HIST_AE_BOT            AE_BOT;
	union REG_ISP_AE_HIST_AE_OVEREXP_THR    AE_OVEREXP_THR;
	union REG_ISP_AE_HIST_AE_NUM_GAPLINE    AE_NUM_GAPLINE;
	uint32_t                                _resv_0x124[23];
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_LUMA  AE_SIMPLE2A_RESULT_LUMA;
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_RGAIN  AE_SIMPLE2A_RESULT_RGAIN;
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_BGAIN  AE_SIMPLE2A_RESULT_BGAIN;
	uint32_t                                _resv_0x18c[29];
	union REG_ISP_AE_HIST_AE_WGT_00         AE_WGT_00;
	union REG_ISP_AE_HIST_AE_WGT_01         AE_WGT_01;
	union REG_ISP_AE_HIST_AE_WGT_02         AE_WGT_02;
	union REG_ISP_AE_HIST_AE_WGT_03         AE_WGT_03;
	union REG_ISP_AE_HIST_AE_WGT_04         AE_WGT_04;
	union REG_ISP_AE_HIST_AE_WGT_05         AE_WGT_05;
	union REG_ISP_AE_HIST_AE_WGT_06         AE_WGT_06;
	union REG_ISP_AE_HIST_AE_WGT_07         AE_WGT_07;
	union REG_ISP_AE_HIST_AE_WGT_08         AE_WGT_08;
	union REG_ISP_AE_HIST_AE_WGT_09         AE_WGT_09;
	union REG_ISP_AE_HIST_AE_WGT_10         AE_WGT_10;
	union REG_ISP_AE_HIST_AE_WGT_11         AE_WGT_11;
	union REG_ISP_AE_HIST_AE_WGT_12         AE_WGT_12;
	union REG_ISP_AE_HIST_AE_WGT_13         AE_WGT_13;
	union REG_ISP_AE_HIST_AE_WGT_14         AE_WGT_14;
	union REG_ISP_AE_HIST_AE_WGT_15         AE_WGT_15;
	union REG_ISP_AE_HIST_AE_WGT_16         AE_WGT_16;
	union REG_ISP_AE_HIST_AE_WGT_17         AE_WGT_17;
	union REG_ISP_AE_HIST_AE_WGT_18         AE_WGT_18;
	union REG_ISP_AE_HIST_AE_WGT_19         AE_WGT_19;
	union REG_ISP_AE_HIST_AE_WGT_20         AE_WGT_20;
	union REG_ISP_AE_HIST_AE_WGT_21         AE_WGT_21;
	union REG_ISP_AE_HIST_AE_WGT_22         AE_WGT_22;
	union REG_ISP_AE_HIST_AE_WGT_23         AE_WGT_23;
	union REG_ISP_AE_HIST_AE_WGT_24         AE_WGT_24;
	union REG_ISP_AE_HIST_AE_WGT_25         AE_WGT_25;
	union REG_ISP_AE_HIST_AE_WGT_26         AE_WGT_26;
	union REG_ISP_AE_HIST_AE_WGT_27         AE_WGT_27;
	union REG_ISP_AE_HIST_AE_WGT_28         AE_WGT_28;
	union REG_ISP_AE_HIST_AE_WGT_29         AE_WGT_29;
	union REG_ISP_AE_HIST_AE_WGT_30         AE_WGT_30;
	union REG_ISP_AE_HIST_AE_WGT_31         AE_WGT_31;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_GMS_T {
	union REG_ISP_GMS_STATUS                GMS_STATUS;
	union REG_ISP_GMS_GRACE_RESET           GMS_GRACE_RESET;
	union REG_ISP_GMS_MONITOR               GMS_MONITOR;
	union REG_ISP_GMS_ENABLE                GMS_ENABLE;
	uint32_t                                _resv_0x10[1];
	union REG_ISP_GMS_FLOW                  GMS_FLOW;
	union REG_ISP_GMS_START_X               GMS_START_X;
	union REG_ISP_GMS_START_Y               GMS_START_Y;
	union REG_ISP_GMS_LOCATION              GMS_LOCATION;
	uint32_t                                _resv_0x24[1];
	union REG_ISP_GMS_X_SIZEM1              GMS_X_SIZEM1;
	union REG_ISP_GMS_Y_SIZEM1              GMS_Y_SIZEM1;
	union REG_ISP_GMS_X_GAP                 GMS_X_GAP;
	union REG_ISP_GMS_Y_GAP                 GMS_Y_GAP;
	union REG_ISP_GMS_DUMMY                 GMS_DUMMY;
	uint32_t                                _resv_0x3c[1];
	union REG_ISP_GMS_MEM_SW_MODE           GMS_SW_MODE;
	union REG_ISP_GMS_MEM_SW_RADDR          GMS_SW_RADDR;
	union REG_ISP_GMS_MEM_SW_RDATA          GMS_SW_RDATA;
	union REG_ISP_GMS_MONITOR_SELECT        GMS_MONITOR_SELECT;
	uint32_t                                _resv_0x50[20];
	union REG_ISP_GMS_DMI_ENABLE            DMI_ENABLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_AF_T {
	union REG_ISP_AF_STATUS                 STATUS;
	union REG_ISP_AF_GRACE_RESET            GRACE_RESET;
	union REG_ISP_AF_MONITOR                MONITOR;
	union REG_ISP_AF_BYPASS                 BYPASS;
	union REG_ISP_AF_KICKOFF                KICKOFF;
	union REG_ISP_AF_ENABLES                ENABLES;
	union REG_ISP_AF_OFFSET_X               OFFSET_X;
	union REG_ISP_AF_MXN_IMAGE_WIDTH_M1     MXN_IMAGE_WIDTH_M1;
	union REG_ISP_AF_BLOCK_WIDTH            BLOCK_WIDTH;
	union REG_ISP_AF_BLOCK_HEIGHT           BLOCK_HEIGHT;
	union REG_ISP_AF_BLOCK_NUM_X            BLOCK_NUM_X;
	union REG_ISP_AF_BLOCK_NUM_Y            BLOCK_NUM_Y;
	uint32_t                                _resv_0x30[1];
	union REG_ISP_AF_HOR_LOW_PASS_VALUE_SHIFT  HOR_LOW_PASS_VALUE_SHIFT;
	union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_0  OFFSET_HORIZONTAL_0;
	union REG_ISP_AF_CORNING_OFFSET_HORIZONTAL_1  OFFSET_HORIZONTAL_1;
	union REG_ISP_AF_CORNING_OFFSET_VERTICAL  OFFSET_VERTICAL;
	union REG_ISP_AF_HIGH_Y_THRE            HIGH_Y_THRE;
	union REG_ISP_AF_LOW_PASS_HORIZON       LOW_PASS_HORIZON;
	union REG_ISP_AF_LOCATION               LOCATION;
	union REG_ISP_AF_HIGH_PASS_HORIZON_0    HIGH_PASS_HORIZON_0;
	union REG_ISP_AF_HIGH_PASS_HORIZON_1    HIGH_PASS_HORIZON_1;
	union REG_ISP_AF_HIGH_PASS_VERTICAL_0   HIGH_PASS_VERTICAL_0;
	union REG_ISP_AF_MEM_SW_MODE            SW_MODE;
	union REG_ISP_AF_MONITOR_SELECT         MONITOR_SELECT;
	uint32_t                                _resv_0x64[2];
	union REG_ISP_AF_IMAGE_WIDTH            IMAGE_WIDTH;
	union REG_ISP_AF_DUMMY                  DUMMY;
	union REG_ISP_AF_MEM_SW_RADDR           SW_RADDR;
	union REG_ISP_AF_MEM_SW_RDATA           SW_RDATA;
	union REG_ISP_AF_MXN_BORDER             MXN_BORDER;
	union REG_ISP_AF_TH_LOW                 TH_LOW;
	union REG_ISP_AF_GAIN_LOW               GAIN_LOW;
	union REG_ISP_AF_SLOP_LOW               SLOP_LOW;
	uint32_t                                _resv_0x8c[5];
	union REG_ISP_AF_DMI_ENABLE             DMI_ENABLE;
	uint32_t                                _resv_0xa4[45];
	union REG_ISP_AF_SQUARE_ENABLE          SQUARE_ENABLE;
	uint32_t                                _resv_0x15c[2];
	union REG_ISP_AF_OUTSHIFT               OUTSHIFT;
	uint32_t                                _resv_0x168[1];
	union REG_ISP_AF_NUM_GAPLINE            NUM_GAPLINE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_PRE_RAW_FE_T {
	union REG_PRE_RAW_FE_PRE_RAW_CTRL       PRE_RAW_CTRL;
	union REG_PRE_RAW_FE_PRE_RAW_FRAME_SIZE  PRE_RAW_FRAME_SIZE;
	uint32_t                                _resv_0x8[2];
	union REG_PRE_RAW_FE_LE_RGBMAP_GRID_NUMBER  LE_RGBMAP_GRID_NUMBER;
	union REG_PRE_RAW_FE_SE_RGBMAP_GRID_NUMBER  SE_RGBMAP_GRID_NUMBER;
	uint32_t                                _resv_0x18[2];
	union REG_PRE_RAW_FE_PRE_RAW_POST_NO_RSPD_CYC  PRE_RAW_POST_NO_RSPD_CYC;
	union REG_PRE_RAW_FE_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  PRE_RAW_POST_RGBMAP_NO_RSPD_CYC;
	union REG_PRE_RAW_FE_PRE_RAW_FRAME_VLD  PRE_RAW_FRAME_VLD;
	union REG_PRE_RAW_FE_PRE_RAW_DEBUG_STATE  PRE_RAW_DEBUG_STATE;
	union REG_PRE_RAW_FE_PRE_RAW_DUMMY      PRE_RAW_DUMMY;
	union REG_PRE_RAW_FE_PRE_RAW_DEBUG_INFO  PRE_RAW_INFO;
	uint32_t                                _resv_0x38[6];
	union REG_PRE_RAW_FE_IDLE_INFO          FE_IDLE_INFO;
	uint32_t                                _resv_0x54[3];
	union REG_PRE_RAW_FE_CHECK_SUM          FE_CHECK_SUM;
	union REG_PRE_RAW_FE_CHECK_SUM_VALUE    FE_CHECK_SUM_VALUE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSI_BDG_DVP_T {
	union REG_ISP_CSI_BDG_DVP_BDG_TOP_CTRL  CSI_BDG_TOP_CTRL;
	union REG_ISP_CSI_BDG_DVP_BDG_INTERRUPT_CTRL  CSI_BDG_INTERRUPT_CTRL;
	union REG_ISP_CSI_BDG_DVP_BDG_DMA_DPCM_MODE  CSI_BDG_DMA_DPCM_MODE;
	union REG_ISP_CSI_BDG_DVP_BDG_DMA_LD_DPCM_MODE  CSI_BDG_DMA_LD_DPCM_MODE;
	union REG_ISP_CSI_BDG_DVP_CH0_SIZE      CH0_SIZE;
	uint32_t                                _resv_0x14[3];
	union REG_ISP_CSI_BDG_DVP_CH0_CROP_EN   CH0_CROP_EN;
	union REG_ISP_CSI_BDG_DVP_CH0_HORZ_CROP  CH0_HORZ_CROP;
	union REG_ISP_CSI_BDG_DVP_CH0_VERT_CROP  CH0_VERT_CROP;
	union REG_ISP_CSI_BDG_DVP_CH0_BLC_SUM   CH0_BLC_SUM;
	uint32_t                                _resv_0x30[12];
	union REG_ISP_CSI_BDG_DVP_PAT_GEN_CTRL  CSI_PAT_GEN_CTRL;
	union REG_ISP_CSI_BDG_DVP_PAT_IDX_CTRL  CSI_PAT_IDX_CTRL;
	union REG_ISP_CSI_BDG_DVP_PAT_COLOR_0   CSI_PAT_COLOR_0;
	union REG_ISP_CSI_BDG_DVP_PAT_COLOR_1   CSI_PAT_COLOR_1;
	union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_0  CSI_BACKGROUND_COLOR_0;
	union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_1  CSI_BACKGROUND_COLOR_1;
	union REG_ISP_CSI_BDG_DVP_FIX_COLOR_0   CSI_FIX_COLOR_0;
	union REG_ISP_CSI_BDG_DVP_FIX_COLOR_1   CSI_FIX_COLOR_1;
	union REG_ISP_CSI_BDG_DVP_MDE_V_SIZE    CSI_MDE_V_SIZE;
	union REG_ISP_CSI_BDG_DVP_MDE_H_SIZE    CSI_MDE_H_SIZE;
	union REG_ISP_CSI_BDG_DVP_FDE_V_SIZE    CSI_FDE_V_SIZE;
	union REG_ISP_CSI_BDG_DVP_FDE_H_SIZE    CSI_FDE_H_SIZE;
	union REG_ISP_CSI_BDG_DVP_HSYNC_CTRL    CSI_HSYNC_CTRL;
	union REG_ISP_CSI_BDG_DVP_VSYNC_CTRL    CSI_VSYNC_CTRL;
	union REG_ISP_CSI_BDG_DVP_TGEN_TT_SIZE  CSI_TGEN_TT_SIZE;
	union REG_ISP_CSI_BDG_DVP_LINE_INTP_HEIGHT_0  LINE_INTP_HEIGHT_0;
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_0   CH0_DEBUG_0;
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_1   CH0_DEBUG_1;
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_2   CH0_DEBUG_2;
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_3   CH0_DEBUG_3;
	uint32_t                                _resv_0xb0[12];
	union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	union REG_ISP_CSI_BDG_DVP_BDG_DEBUG     BDG_DEBUG;
	union REG_ISP_CSI_BDG_DVP_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	union REG_ISP_CSI_BDG_DVP_WR_URGENT_CTRL  CSI_WR_URGENT_CTRL;
	union REG_ISP_CSI_BDG_DVP_RD_URGENT_CTRL  CSI_RD_URGENT_CTRL;
	union REG_ISP_CSI_BDG_DVP_DUMMY         CSI_DUMMY;
	uint32_t                                _resv_0xfc[1];
	union REG_ISP_CSI_BDG_DVP_SLICE_LINE_INTP_HEIGHT_0  SLICE_LINE_INTP_HEIGHT_0;
	uint32_t                                _resv_0x104[3];
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_CROP_EN  WDMA_CH0_CROP_EN;
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_HORZ_CROP  WDMA_CH0_HORZ_CROP;
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_VERT_CROP  WDMA_CH0_VERT_CROP;
	uint32_t                                _resv_0x11c[13];
	union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_CROP_T {
	union REG_CROP_0                        REG_0;
	union REG_CROP_1                        REG_1;
	union REG_CROP_2                        REG_2;
	union REG_CROP_3                        REG_3;
	union REG_CROP_DUMMY                    DUMMY;
	union REG_CROP_DEBUG                    DEBUG;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_BLC_T {
	union REG_ISP_BLC_0                     BLC_0;
	union REG_ISP_BLC_1                     BLC_1;
	union REG_ISP_BLC_2                     BLC_2;
	union REG_ISP_BLC_3                     BLC_3;
	union REG_ISP_BLC_4                     BLC_4;
	union REG_ISP_BLC_5                     BLC_5;
	union REG_ISP_BLC_6                     BLC_6;
	union REG_ISP_BLC_7                     BLC_7;
	union REG_ISP_BLC_8                     BLC_8;
	uint32_t                                _resv_0x24[1];
	union REG_ISP_BLC_DUMMY                 BLC_DUMMY;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_BLC_LOCATION              BLC_LOCATION;
	union REG_ISP_BLC_9                     BLC_9;
	union REG_ISP_BLC_A                     BLC_A;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LMAP_T {
	union REG_ISP_LMAP_LMP_0                LMP_0;
	union REG_ISP_LMAP_LMP_1                LMP_1;
	union REG_ISP_LMAP_LMP_2                LMP_2;
	union REG_ISP_LMAP_LMP_DEBUG_0          LMP_DEBUG_0;
	union REG_ISP_LMAP_LMP_DEBUG_1          LMP_DEBUG_1;
	union REG_ISP_LMAP_DUMMY                DUMMY;
	union REG_ISP_LMAP_LMP_DEBUG_2          LMP_DEBUG_2;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_LMAP_LMP_3                LMP_3;
	union REG_ISP_LMAP_LMP_4                LMP_4;
	union REG_ISP_LMAP_LMP_SET_SEL          LMP_SET_SEL;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGBMAP_T {
	union REG_ISP_RGBMAP_0                  RGBMAP_0;
	union REG_ISP_RGBMAP_1                  RGBMAP_1;
	union REG_ISP_RGBMAP_DEBUG_0            RGBMAP_DEBUG_0;
	union REG_ISP_RGBMAP_DEBUG_1            RGBMAP_DEBUG_1;
	union REG_ISP_RGBMAP_DUMMY              DUMMY;
	union REG_ISP_RGBMAP_2                  RGBMAP_2;
	union REG_ISP_RGBMAP_DEBUG_2            RGBMAP_DEBUG_2;
	union REG_ISP_RGBMAP_3                  RGBMAP_3;
	union REG_ISP_RGBMAP_SET_SEL            RGBMAP_SET_SEL;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_WBG_T {
	union REG_ISP_WBG_0                     WBG_0;
	union REG_ISP_WBG_1                     WBG_1;
	union REG_ISP_WBG_2                     WBG_2;
	uint32_t                                _resv_0xc[1];
	union REG_ISP_WBG_4                     WBG_4;
	union REG_ISP_WBG_5                     WBG_5;
	union REG_ISP_WBG_6                     WBG_6;
	union REG_ISP_WBG_7                     WBG_7;
	uint32_t                                _resv_0x20[1];
	union REG_ISP_WBG_IMG_BAYERID           IMG_BAYERID;
	union REG_ISP_WBG_DUMMY                 WBG_DUMMY;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_WBG_LOCATION              WBG_LOCATION;
	union REG_ISP_WBG_34                    WBG_34;
	union REG_ISP_WBG_38                    WBG_38;
	union REG_ISP_WBG_3C                    WBG_3C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DPC_T {
	union REG_ISP_DPC_0                     DPC_0;
	union REG_ISP_DPC_1                     DPC_1;
	union REG_ISP_DPC_2                     DPC_2;
	union REG_ISP_DPC_3                     DPC_3;
	union REG_ISP_DPC_4                     DPC_4;
	union REG_ISP_DPC_5                     DPC_5;
	union REG_ISP_DPC_6                     DPC_6;
	union REG_ISP_DPC_7                     DPC_7;
	union REG_ISP_DPC_8                     DPC_8;
	union REG_ISP_DPC_9                     DPC_9;
	union REG_ISP_DPC_10                    DPC_10;
	union REG_ISP_DPC_11                    DPC_11;
	union REG_ISP_DPC_12                    DPC_12;
	union REG_ISP_DPC_13                    DPC_13;
	union REG_ISP_DPC_14                    DPC_14;
	union REG_ISP_DPC_15                    DPC_15;
	union REG_ISP_DPC_16                    DPC_16;
	union REG_ISP_DPC_17                    DPC_17;
	union REG_ISP_DPC_18                    DPC_18;
	union REG_ISP_DPC_19                    DPC_19;
	union REG_ISP_DPC_MEM_W0                DPC_MEM_W0;
	union REG_ISP_DPC_WINDOW                DPC_WINDOW;
	union REG_ISP_DPC_MEM_ST_ADDR           DPC_MEM_ST_ADDR;
	uint32_t                                _resv_0x5c[1];
	union REG_ISP_DPC_CHECKSUM              DPC_CHECKSUM;
	union REG_ISP_DPC_INT                   DPC_INT;
	uint32_t                                _resv_0x68[2];
	union REG_ISP_DPC_20                    DPC_20;
	union REG_ISP_DPC_21                    DPC_21;
	union REG_ISP_DPC_22                    DPC_22;
	union REG_ISP_DPC_23                    DPC_23;
	union REG_ISP_DPC_24                    DPC_24;
	union REG_ISP_DPC_25                    DPC_25;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_RAW_TOP_T {
	union REG_RAW_TOP_RAW_0                 RAW_0;
	union REG_RAW_TOP_READ_SEL              READ_SEL;
	union REG_RAW_TOP_RAW_1                 RAW_1;
	uint32_t                                _resv_0xc[1];
	union REG_RAW_TOP_CTRL                  CTRL;
	union REG_RAW_TOP_UP_PQ_EN              UP_PQ_EN;
	union REG_RAW_TOP_RAW_2                 RAW_2;
	union REG_RAW_TOP_DUMMY                 DUMMY;
	union REG_RAW_TOP_RAW_4                 RAW_4;
	union REG_RAW_TOP_STATUS                STATUS;
	union REG_RAW_TOP_DEBUG                 DEBUG;
	union REG_RAW_TOP_DEBUG_SELECT          DEBUG_SELECT;
	union REG_RAW_TOP_RAW_BAYER_TYPE_TOPLEFT  RAW_BAYER_TYPE_TOPLEFT;
	union REG_RAW_TOP_RDMI_ENABLE           RDMI_ENABLE;
	union REG_RAW_TOP_RDMA_SIZE             RDMA_SIZE;
	union REG_RAW_TOP_DPCM_MODE             DPCM_MODE;
	union REG_RAW_TOP_STVALID_STATUS        STVALID_STATUS;
	union REG_RAW_TOP_STREADY_STATUS        STREADY_STATUS;
	union REG_RAW_TOP_PATGEN1               PATGEN1;
	union REG_RAW_TOP_PATGEN2               PATGEN2;
	union REG_RAW_TOP_PATGEN3               PATGEN3;
	union REG_RAW_TOP_PATGEN4               PATGEN4;
	union REG_RAW_TOP_RO_IDLE               RO_IDLE;
	union REG_RAW_TOP_RO_DONE               RO_DONE;
	union REG_RAW_TOP_DMA_IDLE              DMA_IDLE;
	uint32_t                                _resv_0x64[1];
	union REG_RAW_TOP_LE_LMAP_GRID_NUMBER   LE_LMAP_GRID_NUMBER;
	union REG_RAW_TOP_SE_LMAP_GRID_NUMBER   SE_LMAP_GRID_NUMBER;
	union REG_RAW_TOP_CHECKSUM_0            CHECKSUM_0;
	union REG_RAW_TOP_CHECKSUM_1            CHECKSUM_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGBCAC_T {
	union REG_ISP_RGBCAC_CTRL               RGBCAC_CTRL;
	union REG_ISP_RGBCAC_PURPLE_TH          RGBCAC_PURPLE_TH;
	union REG_ISP_RGBCAC_PURPLE_CBCR        RGBCAC_PURPLE_CBCR;
	union REG_ISP_RGBCAC_PURPLE_CBCR2       RGBCAC_PURPLE_CBCR2;
	union REG_ISP_RGBCAC_PURPLE_CBCR3       RGBCAC_PURPLE_CBCR3;
	union REG_ISP_RGBCAC_GREEN_CBCR         RGBCAC_GREEN_CBCR;
	union REG_ISP_RGBCAC_EDGE_CORING        RGBCAC_EDGE_CORING;
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN  RGBCAC_DEPURPLE_STR_RATIO_MIN;
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX  RGBCAC_DEPURPLE_STR_RATIO_MAX;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT0      RGBCAC_EDGE_WGT_LUT0;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT1      RGBCAC_EDGE_WGT_LUT1;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT2      RGBCAC_EDGE_WGT_LUT2;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT3      RGBCAC_EDGE_WGT_LUT3;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT4      RGBCAC_EDGE_WGT_LUT4;
	union REG_ISP_RGBCAC_LUMA               RGBCAC_LUMA;
	union REG_ISP_RGBCAC_LUMA_BLEND         RGBCAC_LUMA_BLEND;
	union REG_ISP_RGBCAC_LUMA_FILTER0       RGBCAC_LUMA_FILTER0;
	union REG_ISP_RGBCAC_LUMA_FILTER1       RGBCAC_LUMA_FILTER1;
	union REG_ISP_RGBCAC_VAR_FILTER0        RGBCAC_VAR_FILTER0;
	union REG_ISP_RGBCAC_VAR_FILTER1        RGBCAC_VAR_FILTER1;
	union REG_ISP_RGBCAC_CHROMA_FILTER0     RGBCAC_CHROMA_FILTER0;
	union REG_ISP_RGBCAC_CHROMA_FILTER1     RGBCAC_CHROMA_FILTER1;
	union REG_ISP_RGBCAC_CBCR_STR           RGBCAC_CBCR_STR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_BNR_T {
	union REG_ISP_BNR_SHADOW_RD_SEL         SHADOW_RD_SEL;
	union REG_ISP_BNR_OUT_SEL               OUT_SEL;
	union REG_ISP_BNR_INDEX_CLR             INDEX_CLR;
	uint32_t                                _resv_0xc[61];
	union REG_ISP_BNR_NS_LUMA_TH_R          NS_LUMA_TH_R;
	union REG_ISP_BNR_NS_SLOPE_R            NS_SLOPE_R;
	union REG_ISP_BNR_NS_OFFSET0_R          NS_OFFSET0_R;
	union REG_ISP_BNR_NS_OFFSET1_R          NS_OFFSET1_R;
	union REG_ISP_BNR_NS_LUMA_TH_GR         NS_LUMA_TH_GR;
	union REG_ISP_BNR_NS_SLOPE_GR           NS_SLOPE_GR;
	union REG_ISP_BNR_NS_OFFSET0_GR         NS_OFFSET0_GR;
	union REG_ISP_BNR_NS_OFFSET1_GR         NS_OFFSET1_GR;
	union REG_ISP_BNR_NS_LUMA_TH_GB         NS_LUMA_TH_GB;
	union REG_ISP_BNR_NS_SLOPE_GB           NS_SLOPE_GB;
	union REG_ISP_BNR_NS_OFFSET0_GB         NS_OFFSET0_GB;
	union REG_ISP_BNR_NS_OFFSET1_GB         NS_OFFSET1_GB;
	union REG_ISP_BNR_NS_LUMA_TH_B          NS_LUMA_TH_B;
	union REG_ISP_BNR_NS_SLOPE_B            NS_SLOPE_B;
	union REG_ISP_BNR_NS_OFFSET0_B          NS_OFFSET0_B;
	union REG_ISP_BNR_NS_OFFSET1_B          NS_OFFSET1_B;
	union REG_ISP_BNR_NS_GAIN               NS_GAIN;
	union REG_ISP_BNR_STRENGTH_MODE         STRENGTH_MODE;
	union REG_ISP_BNR_INTENSITY_SEL         INTENSITY_SEL;
	uint32_t                                _resv_0x14c[45];
	union REG_ISP_BNR_WEIGHT_INTRA_0        WEIGHT_INTRA_0;
	union REG_ISP_BNR_WEIGHT_INTRA_1        WEIGHT_INTRA_1;
	union REG_ISP_BNR_WEIGHT_INTRA_2        WEIGHT_INTRA_2;
	uint32_t                                _resv_0x20c[1];
	union REG_ISP_BNR_WEIGHT_NORM_1         WEIGHT_NORM_1;
	union REG_ISP_BNR_WEIGHT_NORM_2         WEIGHT_NORM_2;
	uint32_t                                _resv_0x218[3];
	union REG_ISP_BNR_VAR_TH                VAR_TH;
	union REG_ISP_BNR_WEIGHT_LUT            WEIGHT_LUT;
	union REG_ISP_BNR_WEIGHT_SM             WEIGHT_SM;
	union REG_ISP_BNR_WEIGHT_V              WEIGHT_V;
	union REG_ISP_BNR_WEIGHT_H              WEIGHT_H;
	union REG_ISP_BNR_WEIGHT_D45            WEIGHT_D45;
	union REG_ISP_BNR_WEIGHT_D135           WEIGHT_D135;
	union REG_ISP_BNR_NEIGHBOR_MAX          NEIGHBOR_MAX;
	uint32_t                                _resv_0x244[3];
	union REG_ISP_BNR_RES_K_SMOOTH          RES_K_SMOOTH;
	union REG_ISP_BNR_RES_K_TEXTURE         RES_K_TEXTURE;
	union REG_ISP_BNR_RES_MAX               RES_MAX;
	uint32_t                                _resv_0x25c[872];
	union REG_ISP_BNR_DUMMY                 DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_CA_T {
	union REG_CA_00                         REG_00;
	union REG_CA_04                         REG_04;
	union REG_CA_08                         REG_08;
	union REG_CA_0C                         REG_0C;
	union REG_CA_10                         REG_10;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CCM_T {
	union REG_ISP_CCM_00                    CCM_00;
	union REG_ISP_CCM_01                    CCM_01;
	union REG_ISP_CCM_02                    CCM_02;
	union REG_ISP_CCM_10                    CCM_10;
	union REG_ISP_CCM_11                    CCM_11;
	union REG_ISP_CCM_12                    CCM_12;
	union REG_ISP_CCM_20                    CCM_20;
	union REG_ISP_CCM_21                    CCM_21;
	union REG_ISP_CCM_22                    CCM_22;
	union REG_ISP_CCM_CTRL                  CCM_CTRL;
	union REG_ISP_CCM_DBG                   CCM_DBG;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_CCM_DMY0                  DMY0;
	union REG_ISP_CCM_DMY1                  DMY1;
	union REG_ISP_CCM_DMY_R                 DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_MMAP_T {
	union REG_ISP_MMAP_00                   REG_00;
	union REG_ISP_MMAP_04                   REG_04;
	union REG_ISP_MMAP_08                   REG_08;
	union REG_ISP_MMAP_0C                   REG_0C;
	union REG_ISP_MMAP_10                   REG_10;
	union REG_ISP_MMAP_14                   REG_14;
	union REG_ISP_MMAP_18                   REG_18;
	union REG_ISP_MMAP_1C                   REG_1C;
	union REG_ISP_MMAP_20                   REG_20;
	union REG_ISP_MMAP_24                   REG_24;
	union REG_ISP_MMAP_28                   REG_28;
	union REG_ISP_MMAP_2C                   REG_2C;
	union REG_ISP_MMAP_30                   REG_30;
	union REG_ISP_MMAP_34                   REG_34;
	union REG_ISP_MMAP_38                   REG_38;
	union REG_ISP_MMAP_3C                   REG_3C;
	union REG_ISP_MMAP_40                   REG_40;
	union REG_ISP_MMAP_44                   REG_44;
	uint32_t                                _resv_0x48[1];
	union REG_ISP_MMAP_4C                   REG_4C;
	union REG_ISP_MMAP_50                   REG_50;
	union REG_ISP_MMAP_54                   REG_54;
	union REG_ISP_MMAP_58                   REG_58;
	union REG_ISP_MMAP_5C                   REG_5C;
	union REG_ISP_MMAP_60                   REG_60;
	union REG_ISP_MMAP_64                   REG_64;
	union REG_ISP_MMAP_68                   REG_68;
	union REG_ISP_MMAP_6C                   REG_6C;
	union REG_ISP_MMAP_70                   REG_70;
	union REG_ISP_MMAP_74                   REG_74;
	union REG_ISP_MMAP_78                   REG_78;
	union REG_ISP_MMAP_7C                   REG_7C;
	union REG_ISP_MMAP_80                   REG_80;
	union REG_ISP_MMAP_84                   REG_84;
	union REG_ISP_MMAP_88                   REG_88;
	union REG_ISP_MMAP_8C                   REG_8C;
	union REG_ISP_MMAP_90                   REG_90;
	uint32_t                                _resv_0x94[3];
	union REG_ISP_MMAP_A0                   REG_A0;
	union REG_ISP_MMAP_A4                   REG_A4;
	union REG_ISP_MMAP_A8                   REG_A8;
	union REG_ISP_MMAP_AC                   REG_AC;
	union REG_ISP_MMAP_B0                   REG_B0;
	union REG_ISP_MMAP_B4                   REG_B4;
	union REG_ISP_MMAP_B8                   REG_B8;
	union REG_ISP_MMAP_BC                   REG_BC;
	union REG_ISP_MMAP_C0                   REG_C0;
	union REG_ISP_MMAP_C4                   REG_C4;
	union REG_ISP_MMAP_C8                   REG_C8;
	union REG_ISP_MMAP_CC                   REG_CC;
	union REG_ISP_MMAP_D0                   REG_D0;
	union REG_ISP_MMAP_D4                   REG_D4;
	union REG_ISP_MMAP_D8                   REG_D8;
	union REG_ISP_MMAP_DC                   REG_DC;
	union REG_ISP_MMAP_E0                   REG_E0;
	union REG_ISP_MMAP_E4                   REG_E4;
	union REG_ISP_MMAP_E8                   REG_E8;
	union REG_ISP_MMAP_EC                   REG_EC;
	union REG_ISP_MMAP_F0                   REG_F0;
	union REG_ISP_MMAP_F4                   REG_F4;
	union REG_ISP_MMAP_F8                   REG_F8;
	union REG_ISP_MMAP_FC                   REG_FC;
	union REG_ISP_MMAP_100                  REG_100;
	union REG_ISP_MMAP_104                  REG_104;
	union REG_ISP_MMAP_108                  REG_108;
	union REG_ISP_MMAP_10C                  REG_10C;
	union REG_ISP_MMAP_110                  REG_110;
	union REG_ISP_MMAP_114                  REG_114;
	union REG_ISP_MMAP_118                  REG_118;
	union REG_ISP_MMAP_11C                  REG_11C;
	union REG_ISP_MMAP_120                  REG_120;
	union REG_ISP_MMAP_124                  REG_124;
	union REG_ISP_MMAP_128                  REG_128;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_GAMMA_T {
	union REG_ISP_GAMMA_CTRL                GAMMA_CTRL;
	union REG_ISP_GAMMA_PROG_CTRL           GAMMA_PROG_CTRL;
	union REG_ISP_GAMMA_PROG_ST_ADDR        GAMMA_PROG_ST_ADDR;
	union REG_ISP_GAMMA_PROG_DATA           GAMMA_PROG_DATA;
	union REG_ISP_GAMMA_PROG_MAX            GAMMA_PROG_MAX;
	union REG_ISP_GAMMA_MEM_SW_RADDR        GAMMA_SW_RADDR;
	union REG_ISP_GAMMA_MEM_SW_RDATA        GAMMA_SW_RDATA;
	union REG_ISP_GAMMA_MEM_SW_RDATA_BG     GAMMA_SW_RDATA_BG;
	union REG_ISP_GAMMA_DBG                 GAMMA_DBG;
	union REG_ISP_GAMMA_DMY0                GAMMA_DMY0;
	union REG_ISP_GAMMA_DMY1                GAMMA_DMY1;
	union REG_ISP_GAMMA_DMY_R               GAMMA_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CLUT_T {
	union REG_ISP_CLUT_CTRL                 CLUT_CTRL;
	union REG_ISP_CLUT_PROG_ADDR            CLUT_PROG_ADDR;
	union REG_ISP_CLUT_PROG_DATA            CLUT_PROG_DATA;
	union REG_ISP_CLUT_PROG_RDATA           CLUT_PROG_RDATA;
	uint32_t                                _resv_0x10[4];
	union REG_ISP_CLUT_DBG                  CLUT_DBG;
	union REG_ISP_CLUT_DMY0                 CLUT_DMY0;
	union REG_ISP_CLUT_DMY1                 CLUT_DMY1;
	union REG_ISP_CLUT_DMY_R                CLUT_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DEHAZE_T {
	union REG_ISP_DEHAZE_DHZ_SMOOTH         DHZ_SMOOTH;
	union REG_ISP_DEHAZE_DHZ_SKIN           DHZ_SKIN;
	union REG_ISP_DEHAZE_DHZ_WGT            DHZ_WGT;
	uint32_t                                _resv_0xc[2];
	union REG_ISP_DEHAZE_DHZ_BYPASS         DHZ_BYPASS;
	union REG_ISP_DEHAZE_0                  REG_0;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_DEHAZE_1                  REG_1;
	union REG_ISP_DEHAZE_2                  REG_2;
	union REG_ISP_DEHAZE_28                 REG_28;
	union REG_ISP_DEHAZE_2C                 REG_2C;
	union REG_ISP_DEHAZE_3                  REG_3;
	union REG_ISP_DEHAZE_5                  REG_5;
	union REG_ISP_DEHAZE_6                  REG_6;
	union REG_ISP_DEHAZE_7                  REG_7;
	union REG_ISP_DEHAZE_8                  REG_8;
	uint32_t                                _resv_0x44[3];
	union REG_ISP_DEHAZE_9                  REG_9;
	union REG_ISP_DEHAZE_10                 REG_10;
	union REG_ISP_DEHAZE_11                 REG_11;
	union REG_ISP_DEHAZE_12                 REG_12;
	union REG_ISP_DEHAZE_17                 REG_17;
	union REG_ISP_DEHAZE_18                 REG_18;
	union REG_ISP_DEHAZE_19                 REG_19;
	union REG_ISP_DEHAZE_20                 REG_20;
	union REG_ISP_DEHAZE_25                 REG_25;
	union REG_ISP_DEHAZE_26                 REG_26;
	union REG_ISP_DEHAZE_TMAP_00            TMAP_00;
	union REG_ISP_DEHAZE_TMAP_01            TMAP_01;
	union REG_ISP_DEHAZE_TMAP_02            TMAP_02;
	union REG_ISP_DEHAZE_TMAP_03            TMAP_03;
	union REG_ISP_DEHAZE_TMAP_04            TMAP_04;
	union REG_ISP_DEHAZE_TMAP_05            TMAP_05;
	union REG_ISP_DEHAZE_TMAP_06            TMAP_06;
	union REG_ISP_DEHAZE_TMAP_07            TMAP_07;
	union REG_ISP_DEHAZE_TMAP_08            TMAP_08;
	union REG_ISP_DEHAZE_TMAP_09            TMAP_09;
	union REG_ISP_DEHAZE_TMAP_10            TMAP_10;
	union REG_ISP_DEHAZE_TMAP_11            TMAP_11;
	union REG_ISP_DEHAZE_TMAP_12            TMAP_12;
	union REG_ISP_DEHAZE_TMAP_13            TMAP_13;
	union REG_ISP_DEHAZE_TMAP_14            TMAP_14;
	union REG_ISP_DEHAZE_TMAP_15            TMAP_15;
	union REG_ISP_DEHAZE_TMAP_16            TMAP_16;
	union REG_ISP_DEHAZE_TMAP_17            TMAP_17;
	union REG_ISP_DEHAZE_TMAP_18            TMAP_18;
	union REG_ISP_DEHAZE_TMAP_19            TMAP_19;
	union REG_ISP_DEHAZE_TMAP_20            TMAP_20;
	union REG_ISP_DEHAZE_TMAP_21            TMAP_21;
	union REG_ISP_DEHAZE_TMAP_22            TMAP_22;
	union REG_ISP_DEHAZE_TMAP_23            TMAP_23;
	union REG_ISP_DEHAZE_TMAP_24            TMAP_24;
	union REG_ISP_DEHAZE_TMAP_25            TMAP_25;
	union REG_ISP_DEHAZE_TMAP_26            TMAP_26;
	union REG_ISP_DEHAZE_TMAP_27            TMAP_27;
	union REG_ISP_DEHAZE_TMAP_28            TMAP_28;
	union REG_ISP_DEHAZE_TMAP_29            TMAP_29;
	union REG_ISP_DEHAZE_TMAP_30            TMAP_30;
	union REG_ISP_DEHAZE_TMAP_31            TMAP_31;
	union REG_ISP_DEHAZE_TMAP_32            TMAP_32;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSC_T {
	union REG_ISP_CSC_0                     REG_0;
	union REG_ISP_CSC_1                     REG_1;
	union REG_ISP_CSC_2                     REG_2;
	union REG_ISP_CSC_3                     REG_3;
	union REG_ISP_CSC_4                     REG_4;
	union REG_ISP_CSC_5                     REG_5;
	union REG_ISP_CSC_6                     REG_6;
	union REG_ISP_CSC_7                     REG_7;
	union REG_ISP_CSC_8                     REG_8;
	union REG_ISP_CSC_9                     REG_9;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGB_DITHER_T {
	union REG_ISP_RGB_DITHER_RGB_DITHER     RGB_DITHER;
	union REG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0  RGB_DITHER_DEBUG0;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DCI_T {
	union REG_ISP_DCI_STATUS                DCI_STATUS;
	union REG_ISP_DCI_GRACE_RESET           DCI_GRACE_RESET;
	union REG_ISP_DCI_MONITOR               DCI_MONITOR;
	union REG_ISP_DCI_ENABLE                DCI_ENABLE;
	union REG_ISP_DCI_MAP_ENABLE            DCI_MAP_ENABLE;
	union REG_ISP_DCI_FLOW                  DCI_FLOW;
	union REG_ISP_DCI_DEMO_MODE             DCI_DEMO_MODE;
	union REG_ISP_DCI_MONITOR_SELECT        DCI_MONITOR_SELECT;
	union REG_ISP_DCI_LOCATION              DCI_LOCATION;
	uint32_t                                _resv_0x24[1];
	union REG_ISP_DCI_PROG_DATA             DCI_PROG_DATA;
	union REG_ISP_DCI_PROG_CTRL             DCI_PROG_CTRL;
	union REG_ISP_DCI_PROG_MAX              DCI_PROG_MAX;
	union REG_ISP_DCI_CTRL                  DCI_CTRL;
	union REG_ISP_DCI_MEM_SW_MODE           DCI_SW_MODE;
	union REG_ISP_DCI_MEM_RADDR             DCI_MEM_RADDR;
	union REG_ISP_DCI_MEM_RDATA             DCI_MEM_RDATA;
	union REG_ISP_DCI_DEBUG                 DCI_DEBUG;
	union REG_ISP_DCI_DUMMY                 DCI_DUMMY;
	union REG_ISP_DCI_IMG_WIDTHM1           IMG_WIDTHM1;
	union REG_ISP_DCI_LUT_ORDER_SELECT      DCI_LUT_ORDER_SELECT;
	union REG_ISP_DCI_ROI_START             DCI_ROI_START;
	union REG_ISP_DCI_ROI_GEO               DCI_ROI_GEO;
	uint32_t                                _resv_0x5c[1];
	union REG_ISP_DCI_UV_GAIN_MAX           DCI_UV_GAIN_MAX;
	uint32_t                                _resv_0x64[7];
	union REG_ISP_DCI_MAP_DBG               DCI_MAP_DBG;
	uint32_t                                _resv_0x84[1];
	union REG_ISP_DCI_BAYER_STARTING        DCI_BAYER_STARTING;
	uint32_t                                _resv_0x8c[5];
	union REG_ISP_DCI_DMI_ENABLE            DMI_ENABLE;
	uint32_t                                _resv_0xa4[87];
	union REG_ISP_DCI_GAMMA_CTRL            GAMMA_CTRL;
	union REG_ISP_DCI_GAMMA_PROG_CTRL       GAMMA_PROG_CTRL;
	uint32_t                                _resv_0x208[1];
	union REG_ISP_DCI_GAMMA_PROG_DATA       GAMMA_PROG_DATA;
	union REG_ISP_DCI_GAMMA_PROG_MAX        GAMMA_PROG_MAX;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_HIST_EDGE_V_T {
	union REG_ISP_HIST_EDGE_V_STATUS        STATUS;
	union REG_ISP_HIST_EDGE_V_SW_CTL        SW_CTL;
	union REG_ISP_HIST_EDGE_V_BYPASS        BYPASS;
	union REG_ISP_HIST_EDGE_V_IP_CONFIG     IP_CONFIG;
	union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETX  HIST_EDGE_V_OFFSETX;
	union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETY  HIST_EDGE_V_OFFSETY;
	union REG_ISP_HIST_EDGE_V_MONITOR       MONITOR;
	union REG_ISP_HIST_EDGE_V_MONITOR_SELECT  MONITOR_SELECT;
	union REG_ISP_HIST_EDGE_V_LOCATION      LOCATION;
	union REG_ISP_HIST_EDGE_V_DUMMY         DUMMY;
	union REG_ISP_HIST_EDGE_V_DMI_ENABLE    DMI_ENABLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_FUSION_T {
	union REG_FUSION_FS_CTRL_0              FS_CTRL_0;
	uint32_t                                _resv_0x4[1];
	union REG_FUSION_FS_SE_GAIN             FS_SE_GAIN;
	union REG_FUSION_FS_LUMA_THD            FS_LUMA_THD;
	union REG_FUSION_FS_WGT                 FS_WGT;
	union REG_FUSION_FS_WGT_SLOPE           FS_WGT_SLOPE;
	union REG_FUSION_FS_SHDW_READ_SEL       FS_SHDW_READ_SEL;
	uint32_t                                _resv_0x1c[1];
	union REG_FUSION_FS_MOTION_LUT_IN       FS_MOTION_LUT_IN;
	union REG_FUSION_FS_MOTION_LUT_OUT_0    FS_MOTION_LUT_OUT_0;
	union REG_FUSION_FS_MOTION_LUT_OUT_1    FS_MOTION_LUT_OUT_1;
	union REG_FUSION_FS_MOTION_LUT_SLOPE_0  FS_MOTION_LUT_SLOPE_0;
	union REG_FUSION_FS_MOTION_LUT_SLOPE_1  FS_MOTION_LUT_SLOPE_1;
	union REG_FUSION_FS_CTRL_1              FS_CTRL_1;
	uint32_t                                _resv_0x38[6];
	union REG_FUSION_FS_CALIB_CTRL_0        FS_CALIB_CTRL_0;
	union REG_FUSION_FS_CALIB_CTRL_1        FS_CALIB_CTRL_1;
	union REG_FUSION_FS_SE_FIX_OFFSET_0     FS_SE_FIX_OFFSET_0;
	union REG_FUSION_FS_SE_FIX_OFFSET_1     FS_SE_FIX_OFFSET_1;
	union REG_FUSION_FS_SE_FIX_OFFSET_2     FS_SE_FIX_OFFSET_2;
	union REG_FUSION_FS_CALIB_OUT_0         FS_CALIB_OUT_0;
	union REG_FUSION_FS_CALIB_OUT_1         FS_CALIB_OUT_1;
	union REG_FUSION_FS_CALIB_OUT_2         FS_CALIB_OUT_2;
	union REG_FUSION_FS_CALIB_OUT_3         FS_CALIB_OUT_3;
	union REG_FUSION_FS_LMAP_DARK_THD       FS_LMAP_DARK_THD;
	union REG_FUSION_FS_LMAP_DARK_WGT       FS_LMAP_DARK_WGT;
	union REG_FUSION_FS_LMAP_DARK_WGT_SLOPE  FS_LMAP_DARK_WGT_SLOPE;
	union REG_FUSION_FS_LMAP_BRIT_THD       FS_LMAP_BRIT_THD;
	union REG_FUSION_FS_LMAP_BRIT_WGT       FS_LMAP_BRIT_WGT;
	union REG_FUSION_FS_LMAP_BRIT_WGT_SLOPE  FS_LMAP_BRIT_WGT_SLOPE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_LTM_T {
	union REG_LTM_H00                       REG_H00;
	union REG_LTM_H04                       REG_H04;
	union REG_LTM_H08                       REG_H08;
	union REG_LTM_H0C                       REG_H0C;
	uint32_t                                _resv_0x10[1];
	union REG_LTM_H14                       REG_H14;
	union REG_LTM_H18                       REG_H18;
	union REG_LTM_H1C                       REG_H1C;
	union REG_LTM_H20                       REG_H20;
	union REG_LTM_H24                       REG_H24;
	union REG_LTM_H28                       REG_H28;
	union REG_LTM_H2C                       REG_H2C;
	union REG_LTM_H30                       REG_H30;
	union REG_LTM_H34                       REG_H34;
	union REG_LTM_H38                       REG_H38;
	union REG_LTM_H3C                       REG_H3C;
	union REG_LTM_H40                       REG_H40;
	union REG_LTM_H44                       REG_H44;
	union REG_LTM_H48                       REG_H48;
	union REG_LTM_H4C                       REG_H4C;
	union REG_LTM_H50                       REG_H50;
	union REG_LTM_H54                       REG_H54;
	union REG_LTM_H58                       REG_H58;
	union REG_LTM_H5C                       REG_H5C;
	union REG_LTM_H60                       REG_H60;
	union REG_LTM_H64                       REG_H64;
	union REG_LTM_H68                       REG_H68;
	union REG_LTM_H6C                       REG_H6C;
	union REG_LTM_H70                       REG_H70;
	union REG_LTM_H74                       REG_H74;
	union REG_LTM_H78                       REG_H78;
	union REG_LTM_H7C                       REG_H7C;
	union REG_LTM_H80                       REG_H80;
	union REG_LTM_H84                       REG_H84;
	union REG_LTM_H88                       REG_H88;
	union REG_LTM_H8C                       REG_H8C;
	union REG_LTM_H90                       REG_H90;
	union REG_LTM_H94                       REG_H94;
	union REG_LTM_H98                       REG_H98;
	union REG_LTM_H9C                       REG_H9C;
	union REG_LTM_HA0                       REG_HA0;
	union REG_LTM_HA4                       REG_HA4;
	union REG_LTM_HA8                       REG_HA8;
	union REG_LTM_HAC                       REG_HAC;
	union REG_LTM_HB0                       REG_HB0;
	union REG_LTM_HB4                       REG_HB4;
	union REG_LTM_HB8                       REG_HB8;
	union REG_LTM_HBC                       REG_HBC;
	union REG_LTM_HC0                       REG_HC0;
	union REG_LTM_HC4                       REG_HC4;
	union REG_LTM_HC8                       REG_HC8;
	union REG_LTM_HCC                       REG_HCC;
	union REG_LTM_HD0                       REG_HD0;
	union REG_LTM_HD4                       REG_HD4;
	union REG_LTM_HD8                       REG_HD8;
	union REG_LTM_HDC                       REG_HDC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_CA_LITE_T {
	union REG_CA_LITE_00                    REG_00;
	union REG_CA_LITE_04                    REG_04;
	union REG_CA_LITE_08                    REG_08;
	union REG_CA_LITE_0C                    REG_0C;
	union REG_CA_LITE_10                    REG_10;
	union REG_CA_LITE_14                    REG_14;
	union REG_CA_LITE_18                    REG_18;
	union REG_CA_LITE_1C                    REG_1C;
	union REG_CA_LITE_20                    REG_20;
	union REG_CA_LITE_24                    REG_24;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_444_422_T {
	union REG_ISP_444_422_0                 REG_0;
	union REG_ISP_444_422_1                 REG_1;
	union REG_ISP_444_422_2                 REG_2;
	union REG_ISP_444_422_3                 REG_3;
	union REG_ISP_444_422_4                 REG_4;
	union REG_ISP_444_422_5                 REG_5;
	union REG_ISP_444_422_6                 REG_6;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_444_422_8                 REG_8;
	union REG_ISP_444_422_9                 REG_9;
	union REG_ISP_444_422_10                REG_10;
	union REG_ISP_444_422_11                REG_11;
	union REG_ISP_444_422_12                REG_12;
	union REG_ISP_444_422_13                REG_13;
	union REG_ISP_444_422_14                REG_14;
	union REG_ISP_444_422_15                REG_15;
	union REG_ISP_444_422_16                REG_16;
	union REG_ISP_444_422_17                REG_17;
	union REG_ISP_444_422_18                REG_18;
	union REG_ISP_444_422_19                REG_19;
	union REG_ISP_444_422_20                REG_20;
	union REG_ISP_444_422_21                REG_21;
	union REG_ISP_444_422_22                REG_22;
	union REG_ISP_444_422_23                REG_23;
	union REG_ISP_444_422_24                REG_24;
	union REG_ISP_444_422_25                REG_25;
	union REG_ISP_444_422_26                REG_26;
	union REG_ISP_444_422_27                REG_27;
	union REG_ISP_444_422_28                REG_28;
	union REG_ISP_444_422_29                REG_29;
	union REG_ISP_444_422_30                REG_30;
	union REG_ISP_444_422_31                REG_31;
	union REG_ISP_444_422_80                REG_80;
	union REG_ISP_444_422_84                REG_84;
	union REG_ISP_444_422_88                REG_88;
	union REG_ISP_444_422_8C                REG_8C;
	union REG_ISP_444_422_90                REG_90;
	union REG_ISP_444_422_94                REG_94;
	union REG_ISP_444_422_98                REG_98;
	union REG_ISP_444_422_9C                REG_9C;
	union REG_ISP_444_422_A0                REG_A0;
	union REG_ISP_444_422_A4                REG_A4;
	union REG_ISP_444_422_A8                REG_A8;
	union REG_ISP_444_422_AC                REG_AC;
	union REG_ISP_444_422_B0                REG_B0;
	union REG_ISP_444_422_B4                REG_B4;
	union REG_ISP_444_422_B8                REG_B8;
	union REG_ISP_444_422_BC                REG_BC;
	union REG_ISP_444_422_C0                REG_C0;
	union REG_ISP_444_422_C4                REG_C4;
	union REG_ISP_444_422_C8                REG_C8;
	union REG_ISP_444_422_CC                REG_CC;
	union REG_ISP_444_422_D0                REG_D0;
	union REG_ISP_444_422_D4                REG_D4;
	union REG_ISP_444_422_D8                REG_D8;
	union REG_ISP_444_422_DC                REG_DC;
	union REG_ISP_444_422_E0                REG_E0;
	union REG_ISP_444_422_E4                REG_E4;
	union REG_ISP_444_422_E8                REG_E8;
	union REG_ISP_444_422_EC                REG_EC;
	union REG_ISP_444_422_F0                REG_F0;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_FBCE_T {
	union REG_FBCE_00                       REG_00;
	uint32_t                                _resv_0x4[3];
	union REG_FBCE_10                       REG_10;
	union REG_FBCE_14                       REG_14;
	union REG_FBCE_18                       REG_18;
	uint32_t                                _resv_0x1c[1];
	union REG_FBCE_20                       REG_20;
	union REG_FBCE_24                       REG_24;
	union REG_FBCE_28                       REG_28;
	union REG_FBCE_2C                       REG_2C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_FBCD_T {
	union REG_FBCD_00                       REG_00;
	uint32_t                                _resv_0x4[2];
	union REG_FBCD_0C                       REG_0C;
	union REG_FBCD_10                       REG_10;
	union REG_FBCD_14                       REG_14;
	uint32_t                                _resv_0x18[2];
	union REG_FBCD_20                       REG_20;
	union REG_FBCD_24                       REG_24;
	union REG_FBCD_28                       REG_28;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_YUV_DITHER_T {
	union REG_ISP_YUV_DITHER_Y_DITHER       Y_DITHER;
	union REG_ISP_YUV_DITHER_UV_DITHER      UV_DITHER;
	union REG_ISP_YUV_DITHER_DEBUG_00       DEBUG_00;
	union REG_ISP_YUV_DITHER_DEBUG_01       DEBUG_01;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_YNR_T {
	union REG_ISP_YNR_SHADOW_RD_SEL         SHADOW_RD_SEL;
	union REG_ISP_YNR_OUT_SEL               OUT_SEL;
	union REG_ISP_YNR_INDEX_CLR             INDEX_CLR;
	union REG_ISP_YNR_NS0_LUMA_TH_00        NS0_LUMA_TH_00;
	union REG_ISP_YNR_NS0_LUMA_TH_01        NS0_LUMA_TH_01;
	union REG_ISP_YNR_NS0_LUMA_TH_02        NS0_LUMA_TH_02;
	union REG_ISP_YNR_NS0_LUMA_TH_03        NS0_LUMA_TH_03;
	union REG_ISP_YNR_NS0_LUMA_TH_04        NS0_LUMA_TH_04;
	union REG_ISP_YNR_NS0_LUMA_TH_05        NS0_LUMA_TH_05;
	union REG_ISP_YNR_NS0_SLOPE_00          NS0_SLOPE_00;
	union REG_ISP_YNR_NS0_SLOPE_01          NS0_SLOPE_01;
	union REG_ISP_YNR_NS0_SLOPE_02          NS0_SLOPE_02;
	union REG_ISP_YNR_NS0_SLOPE_03          NS0_SLOPE_03;
	union REG_ISP_YNR_NS0_SLOPE_04          NS0_SLOPE_04;
	union REG_ISP_YNR_NS0_OFFSET_00         NS0_OFFSET_00;
	union REG_ISP_YNR_NS0_OFFSET_01         NS0_OFFSET_01;
	union REG_ISP_YNR_NS0_OFFSET_02         NS0_OFFSET_02;
	union REG_ISP_YNR_NS0_OFFSET_03         NS0_OFFSET_03;
	union REG_ISP_YNR_NS0_OFFSET_04         NS0_OFFSET_04;
	union REG_ISP_YNR_NS0_OFFSET_05         NS0_OFFSET_05;
	union REG_ISP_YNR_NS1_LUMA_TH_00        NS1_LUMA_TH_00;
	union REG_ISP_YNR_NS1_LUMA_TH_01        NS1_LUMA_TH_01;
	union REG_ISP_YNR_NS1_LUMA_TH_02        NS1_LUMA_TH_02;
	union REG_ISP_YNR_NS1_LUMA_TH_03        NS1_LUMA_TH_03;
	union REG_ISP_YNR_NS1_LUMA_TH_04        NS1_LUMA_TH_04;
	union REG_ISP_YNR_NS1_LUMA_TH_05        NS1_LUMA_TH_05;
	union REG_ISP_YNR_NS1_SLOPE_00          NS1_SLOPE_00;
	union REG_ISP_YNR_NS1_SLOPE_01          NS1_SLOPE_01;
	union REG_ISP_YNR_NS1_SLOPE_02          NS1_SLOPE_02;
	union REG_ISP_YNR_NS1_SLOPE_03          NS1_SLOPE_03;
	union REG_ISP_YNR_NS1_SLOPE_04          NS1_SLOPE_04;
	union REG_ISP_YNR_NS1_OFFSET_00         NS1_OFFSET_00;
	union REG_ISP_YNR_NS1_OFFSET_01         NS1_OFFSET_01;
	union REG_ISP_YNR_NS1_OFFSET_02         NS1_OFFSET_02;
	union REG_ISP_YNR_NS1_OFFSET_03         NS1_OFFSET_03;
	union REG_ISP_YNR_NS1_OFFSET_04         NS1_OFFSET_04;
	union REG_ISP_YNR_NS1_OFFSET_05         NS1_OFFSET_05;
	union REG_ISP_YNR_NS_GAIN               NS_GAIN;
	union REG_ISP_YNR_MOTION_LUT_00         MOTION_LUT_00;
	union REG_ISP_YNR_MOTION_LUT_01         MOTION_LUT_01;
	union REG_ISP_YNR_MOTION_LUT_02         MOTION_LUT_02;
	union REG_ISP_YNR_MOTION_LUT_03         MOTION_LUT_03;
	union REG_ISP_YNR_MOTION_LUT_04         MOTION_LUT_04;
	union REG_ISP_YNR_MOTION_LUT_05         MOTION_LUT_05;
	union REG_ISP_YNR_MOTION_LUT_06         MOTION_LUT_06;
	union REG_ISP_YNR_MOTION_LUT_07         MOTION_LUT_07;
	union REG_ISP_YNR_MOTION_LUT_08         MOTION_LUT_08;
	union REG_ISP_YNR_MOTION_LUT_09         MOTION_LUT_09;
	union REG_ISP_YNR_MOTION_LUT_10         MOTION_LUT_10;
	union REG_ISP_YNR_MOTION_LUT_11         MOTION_LUT_11;
	union REG_ISP_YNR_MOTION_LUT_12         MOTION_LUT_12;
	union REG_ISP_YNR_MOTION_LUT_13         MOTION_LUT_13;
	union REG_ISP_YNR_MOTION_LUT_14         MOTION_LUT_14;
	union REG_ISP_YNR_MOTION_LUT_15         MOTION_LUT_15;
	union REG_ISP_YNR_WEIGHT_INTRA_0        WEIGHT_INTRA_0;
	union REG_ISP_YNR_WEIGHT_INTRA_1        WEIGHT_INTRA_1;
	union REG_ISP_YNR_WEIGHT_INTRA_2        WEIGHT_INTRA_2;
	union REG_ISP_YNR_WEIGHT_NORM_1         WEIGHT_NORM_1;
	union REG_ISP_YNR_WEIGHT_NORM_2         WEIGHT_NORM_2;
	union REG_ISP_YNR_ALPHA_GAIN            ALPHA_GAIN;
	union REG_ISP_YNR_VAR_TH                VAR_TH;
	union REG_ISP_YNR_WEIGHT_SM             WEIGHT_SM;
	union REG_ISP_YNR_WEIGHT_V              WEIGHT_V;
	union REG_ISP_YNR_WEIGHT_H              WEIGHT_H;
	union REG_ISP_YNR_WEIGHT_D45            WEIGHT_D45;
	union REG_ISP_YNR_WEIGHT_D135           WEIGHT_D135;
	union REG_ISP_YNR_NEIGHBOR_MAX          NEIGHBOR_MAX;
	union REG_ISP_YNR_RES_K_SMOOTH          RES_K_SMOOTH;
	union REG_ISP_YNR_RES_K_TEXTURE         RES_K_TEXTURE;
	union REG_ISP_YNR_FILTER_MODE_EN        FILTER_MODE_EN;
	union REG_ISP_YNR_FILTER_MODE_ALPHA     FILTER_MODE_ALPHA;
	union REG_ISP_YNR_RES_MOT_LUT_00        RES_MOT_LUT_00;
	union REG_ISP_YNR_RES_MOT_LUT_01        RES_MOT_LUT_01;
	union REG_ISP_YNR_RES_MOT_LUT_02        RES_MOT_LUT_02;
	union REG_ISP_YNR_RES_MOT_LUT_03        RES_MOT_LUT_03;
	union REG_ISP_YNR_RES_MOT_LUT_04        RES_MOT_LUT_04;
	union REG_ISP_YNR_RES_MOT_LUT_05        RES_MOT_LUT_05;
	union REG_ISP_YNR_RES_MOT_LUT_06        RES_MOT_LUT_06;
	union REG_ISP_YNR_RES_MOT_LUT_07        RES_MOT_LUT_07;
	union REG_ISP_YNR_RES_MOT_LUT_08        RES_MOT_LUT_08;
	union REG_ISP_YNR_RES_MOT_LUT_09        RES_MOT_LUT_09;
	union REG_ISP_YNR_RES_MOT_LUT_10        RES_MOT_LUT_10;
	union REG_ISP_YNR_RES_MOT_LUT_11        RES_MOT_LUT_11;
	union REG_ISP_YNR_RES_MOT_LUT_12        RES_MOT_LUT_12;
	union REG_ISP_YNR_RES_MOT_LUT_13        RES_MOT_LUT_13;
	union REG_ISP_YNR_RES_MOT_LUT_14        RES_MOT_LUT_14;
	union REG_ISP_YNR_RES_MOT_LUT_15        RES_MOT_LUT_15;
	union REG_ISP_YNR_RES_MAX               RES_MAX;
	union REG_ISP_YNR_RES_MOTION_MAX        RES_MOTION_MAX;
	union REG_ISP_YNR_MOTION_NS_CLIP_MAX    MOTION_NS_CLIP_MAX;
	uint32_t                                _resv_0x168[38];
	union REG_ISP_YNR_WEIGHT_LUT            WEIGHT_LUT;
	uint32_t                                _resv_0x204[894];
	union REG_ISP_YNR_DUMMY                 DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CNR_T {
	union REG_ISP_CNR_ENABLE                CNR_ENABLE;
	union REG_ISP_CNR_STRENGTH_MODE         CNR_STRENGTH_MODE;
	union REG_ISP_CNR_PURPLE_TH             CNR_PURPLE_TH;
	union REG_ISP_CNR_PURPLE_CB             CNR_PURPLE_CB;
	union REG_ISP_CNR_GREEN_CB              CNR_GREEN_CB;
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_00  WEIGHT_LUT_INTER_CNR_00;
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_04  WEIGHT_LUT_INTER_CNR_04;
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_08  WEIGHT_LUT_INTER_CNR_08;
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_12  WEIGHT_LUT_INTER_CNR_12;
	uint32_t                                _resv_0x24[2];
	union REG_ISP_CNR_MOTION_LUT_0          CNR_MOTION_LUT_0;
	union REG_ISP_CNR_MOTION_LUT_4          CNR_MOTION_LUT_4;
	union REG_ISP_CNR_MOTION_LUT_8          CNR_MOTION_LUT_8;
	union REG_ISP_CNR_MOTION_LUT_12         CNR_MOTION_LUT_12;
	union REG_ISP_CNR_PURPLE_CB2            CNR_PURPLE_CB2;
	union REG_ISP_CNR_MASK                  CNR_MASK;
	union REG_ISP_CNR_DUMMY                 CNR_DUMMY;
	union REG_ISP_CNR_EDGE_SCALE            CNR_EDGE_SCALE;
	union REG_ISP_CNR_EDGE_RATIO_SPEED      CNR_EDGE_RATIO_SPEED;
	union REG_ISP_CNR_DEPURPLE_WEIGHT_TH    CNR_DEPURPLE_WEIGHT_TH;
	union REG_ISP_CNR_CORING_MOTION_LUT_0   CNR_CORING_MOTION_LUT_0;
	union REG_ISP_CNR_CORING_MOTION_LUT_4   CNR_CORING_MOTION_LUT_4;
	union REG_ISP_CNR_CORING_MOTION_LUT_8   CNR_CORING_MOTION_LUT_8;
	union REG_ISP_CNR_CORING_MOTION_LUT_12  CNR_CORING_MOTION_LUT_12;
	union REG_ISP_CNR_EDGE_SCALE_LUT_0      CNR_EDGE_SCALE_LUT_0;
	union REG_ISP_CNR_EDGE_SCALE_LUT_4      CNR_EDGE_SCALE_LUT_4;
	union REG_ISP_CNR_EDGE_SCALE_LUT_8      CNR_EDGE_SCALE_LUT_8;
	union REG_ISP_CNR_EDGE_SCALE_LUT_12     CNR_EDGE_SCALE_LUT_12;
	union REG_ISP_CNR_EDGE_SCALE_LUT_16     CNR_EDGE_SCALE_LUT_16;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_YCURV_T {
	union REG_ISP_YCURV_YCUR_CTRL           YCUR_CTRL;
	union REG_ISP_YCURV_YCUR_PROG_CTRL      YCUR_PROG_CTRL;
	union REG_ISP_YCURV_YCUR_PROG_ST_ADDR   YCUR_PROG_ST_ADDR;
	union REG_ISP_YCURV_YCUR_PROG_DATA      YCUR_PROG_DATA;
	union REG_ISP_YCURV_YCUR_PROG_MAX       YCUR_PROG_MAX;
	union REG_ISP_YCURV_YCUR_MEM_SW_MODE    YCUR_SW_MODE;
	union REG_ISP_YCURV_YCUR_MEM_SW_RDATA   YCUR_SW_RDATA;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_YCURV_YCUR_DBG            YCUR_DBG;
	union REG_ISP_YCURV_YCUR_DMY0           YCUR_DMY0;
	union REG_ISP_YCURV_YCUR_DMY1           YCUR_DMY1;
	union REG_ISP_YCURV_YCUR_DMY_R          YCUR_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_TOP_T {
	union REG_ISP_TOP_INT_EVENT0            INT_EVENT0;
	union REG_ISP_TOP_INT_EVENT1            INT_EVENT1;
	union REG_ISP_TOP_INT_EVENT2            INT_EVENT2;
	union REG_ISP_TOP_ERROR_STS             ERROR_STS;
	union REG_ISP_TOP_INT_EVENT0_EN         INT_EVENT0_EN;
	union REG_ISP_TOP_INT_EVENT1_EN         INT_EVENT1_EN;
	union REG_ISP_TOP_INT_EVENT2_EN         INT_EVENT2_EN;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_TOP_SW_CTRL_0             SW_CTRL_0;
	union REG_ISP_TOP_SW_CTRL_1             SW_CTRL_1;
	union REG_ISP_TOP_CTRL_MODE_SEL0        CTRL_MODE_SEL0;
	union REG_ISP_TOP_CTRL_MODE_SEL1        CTRL_MODE_SEL1;
	union REG_ISP_TOP_SCENARIOS_CTRL        SCENARIOS_CTRL;
	union REG_ISP_TOP_SW_RST                SW_RST;
	union REG_ISP_TOP_BLK_IDLE              BLK_IDLE;
	union REG_ISP_TOP_BLK_IDLE_ENABLE       BLK_IDLE_ENABLE;
	union REG_ISP_TOP_DBUS0                 DBUS0;
	union REG_ISP_TOP_DBUS1                 DBUS1;
	union REG_ISP_TOP_DBUS2                 DBUS2;
	union REG_ISP_TOP_DBUS3                 DBUS3;
	union REG_ISP_TOP_FORCE_INT             FORCE_INT;
	union REG_ISP_TOP_DUMMY                 DUMMY;
	union REG_ISP_TOP_IP_ENABLE0            IP_ENABLE0;
	union REG_ISP_TOP_IP_ENABLE1            IP_ENABLE1;
	union REG_ISP_TOP_IP_ENABLE2            IP_ENABLE2;
	uint32_t                                _resv_0x64[1];
	union REG_ISP_TOP_CMDQ_CTRL             CMDQ_CTRL;
	union REG_ISP_TOP_CMDQ_TRIG             CMDQ_TRIG;
	union REG_ISP_TOP_TRIG_CNT              TRIG_CNT;
	union REG_ISP_TOP_SVN_VERSION           SVN_VERSION;
	union REG_ISP_TOP_TIMESTAMP             TIMESTAMP;
	uint32_t                                _resv_0x7c[1];
	union REG_ISP_TOP_SCLIE_ENABLE          SCLIE_ENABLE;
	union REG_ISP_TOP_W_SLICE_THRESH_MAIN   W_SLICE_THRESH_MAIN;
	union REG_ISP_TOP_W_SLICE_THRESH_SUB_CURR  W_SLICE_THRESH_SUB_CURR;
	union REG_ISP_TOP_W_SLICE_THRESH_SUB_PRV  W_SLICE_THRESH_SUB_PRV;
	union REG_ISP_TOP_R_SLICE_THRESH_MAIN   R_SLICE_THRESH_MAIN;
	union REG_ISP_TOP_R_SLICE_THRESH_SUB_CURR  R_SLICE_THRESH_SUB_CURR;
	union REG_ISP_TOP_R_SLICE_THRESH_SUB_PRV  R_SLICE_THRESH_SUB_PRV;
	union REG_ISP_TOP_RAW_FRAME_VALID       RAW_FRAME_VALID;
	union REG_ISP_TOP_FIRST_FRAME           FIRST_FRAME;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_WDMA_CORE_T {
	union REG_WDMA_CORE_SHADOW_RD_SEL       SHADOW_RD_SEL;
	union REG_WDMA_CORE_IP_DISABLE          IP_DISABLE;
	union REG_WDMA_CORE_DISABLE_SEGLEN      DISABLE_SEGLEN;
	union REG_WDMA_CORE_UP_RING_BASE        UP_RING_BASE;
	union REG_WDMA_CORE_NORM_STATUS0        NORM_STATUS0;
	union REG_WDMA_CORE_NORM_STATUS1        NORM_STATUS1;
	union REG_WDMA_CORE_RING_BUFFER_EN      RING_BUFFER_EN;
	uint32_t                                _resv_0x1c[1];
	union REG_WDMA_CORE_NORM_PERF           NORM_PERF;
	uint32_t                                _resv_0x24[1];
	union REG_WDMA_CORE_RING_PATCH_ENABLE   RING_PATCH_ENABLE;
	union REG_WDMA_CORE_SET_RING_BASE       SET_RING_BASE;
	union REG_WDMA_CORE_RING_BASE_ADDR_L    RING_BASE_ADDR_L;
	union REG_WDMA_CORE_RING_BASE_ADDR_H    RING_BASE_ADDR_H;
	uint32_t                                _resv_0x38[18];
	union REG_WDMA_CORE_RING_BUFFER_SIZE0   RING_BUFFER_SIZE0;
	union REG_WDMA_CORE_RING_BUFFER_SIZE1   RING_BUFFER_SIZE1;
	union REG_WDMA_CORE_RING_BUFFER_SIZE2   RING_BUFFER_SIZE2;
	union REG_WDMA_CORE_RING_BUFFER_SIZE3   RING_BUFFER_SIZE3;
	union REG_WDMA_CORE_RING_BUFFER_SIZE4   RING_BUFFER_SIZE4;
	union REG_WDMA_CORE_RING_BUFFER_SIZE5   RING_BUFFER_SIZE5;
	union REG_WDMA_CORE_RING_BUFFER_SIZE6   RING_BUFFER_SIZE6;
	union REG_WDMA_CORE_RING_BUFFER_SIZE7   RING_BUFFER_SIZE7;
	union REG_WDMA_CORE_RING_BUFFER_SIZE8   RING_BUFFER_SIZE8;
	union REG_WDMA_CORE_RING_BUFFER_SIZE9   RING_BUFFER_SIZE9;
	union REG_WDMA_CORE_RING_BUFFER_SIZE10  RING_BUFFER_SIZE10;
	union REG_WDMA_CORE_RING_BUFFER_SIZE11  RING_BUFFER_SIZE11;
	union REG_WDMA_CORE_RING_BUFFER_SIZE12  RING_BUFFER_SIZE12;
	union REG_WDMA_CORE_RING_BUFFER_SIZE13  RING_BUFFER_SIZE13;
	union REG_WDMA_CORE_RING_BUFFER_SIZE14  RING_BUFFER_SIZE14;
	union REG_WDMA_CORE_RING_BUFFER_SIZE15  RING_BUFFER_SIZE15;
	union REG_WDMA_CORE_RING_BUFFER_SIZE16  RING_BUFFER_SIZE16;
	union REG_WDMA_CORE_RING_BUFFER_SIZE17  RING_BUFFER_SIZE17;
	union REG_WDMA_CORE_RING_BUFFER_SIZE18  RING_BUFFER_SIZE18;
	union REG_WDMA_CORE_RING_BUFFER_SIZE19  RING_BUFFER_SIZE19;
	union REG_WDMA_CORE_RING_BUFFER_SIZE20  RING_BUFFER_SIZE20;
	union REG_WDMA_CORE_RING_BUFFER_SIZE21  RING_BUFFER_SIZE21;
	union REG_WDMA_CORE_RING_BUFFER_SIZE22  RING_BUFFER_SIZE22;
	union REG_WDMA_CORE_RING_BUFFER_SIZE23  RING_BUFFER_SIZE23;
	union REG_WDMA_CORE_RING_BUFFER_SIZE24  RING_BUFFER_SIZE24;
	union REG_WDMA_CORE_RING_BUFFER_SIZE25  RING_BUFFER_SIZE25;
	union REG_WDMA_CORE_RING_BUFFER_SIZE26  RING_BUFFER_SIZE26;
	union REG_WDMA_CORE_RING_BUFFER_SIZE27  RING_BUFFER_SIZE27;
	union REG_WDMA_CORE_RING_BUFFER_SIZE28  RING_BUFFER_SIZE28;
	union REG_WDMA_CORE_RING_BUFFER_SIZE29  RING_BUFFER_SIZE29;
	union REG_WDMA_CORE_RING_BUFFER_SIZE30  RING_BUFFER_SIZE30;
	union REG_WDMA_CORE_RING_BUFFER_SIZE31  RING_BUFFER_SIZE31;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS0  NEXT_DMA_ADDR_STS0;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS1  NEXT_DMA_ADDR_STS1;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS2  NEXT_DMA_ADDR_STS2;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS3  NEXT_DMA_ADDR_STS3;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS4  NEXT_DMA_ADDR_STS4;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS5  NEXT_DMA_ADDR_STS5;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS6  NEXT_DMA_ADDR_STS6;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS7  NEXT_DMA_ADDR_STS7;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS8  NEXT_DMA_ADDR_STS8;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS9  NEXT_DMA_ADDR_STS9;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS10  NEXT_DMA_ADDR_STS10;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS11  NEXT_DMA_ADDR_STS11;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS12  NEXT_DMA_ADDR_STS12;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS13  NEXT_DMA_ADDR_STS13;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS14  NEXT_DMA_ADDR_STS14;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS15  NEXT_DMA_ADDR_STS15;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS16  NEXT_DMA_ADDR_STS16;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS17  NEXT_DMA_ADDR_STS17;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS18  NEXT_DMA_ADDR_STS18;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS19  NEXT_DMA_ADDR_STS19;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS20  NEXT_DMA_ADDR_STS20;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS21  NEXT_DMA_ADDR_STS21;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS22  NEXT_DMA_ADDR_STS22;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS23  NEXT_DMA_ADDR_STS23;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS24  NEXT_DMA_ADDR_STS24;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS25  NEXT_DMA_ADDR_STS25;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS26  NEXT_DMA_ADDR_STS26;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS27  NEXT_DMA_ADDR_STS27;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS28  NEXT_DMA_ADDR_STS28;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS29  NEXT_DMA_ADDR_STS29;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS30  NEXT_DMA_ADDR_STS30;
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS31  NEXT_DMA_ADDR_STS31;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_RDMA_CORE_T {
	union REG_RDMA_CORE_SHADOW_RD_SEL       SHADOW_RD_SEL;
	union REG_RDMA_CORE_IP_DISABLE          IP_DISABLE;
	uint32_t                                _resv_0x8[1];
	union REG_RDMA_CORE_UP_RING_BASE        UP_RING_BASE;
	union REG_RDMA_CORE_NORM_STATUS0        NORM_STATUS0;
	union REG_RDMA_CORE_NORM_STATUS1        NORM_STATUS1;
	union REG_RDMA_CORE_RING_BUFFER_EN      RING_BUFFER_EN;
	uint32_t                                _resv_0x1c[1];
	union REG_RDMA_CORE_NORM_PERF           NORM_PERF;
	union REG_RDMA_CORE_AR_PRIORITY_SEL     AR_PRIORITY_SEL;
	union REG_RDMA_CORE_RING_PATCH_ENABLE   RING_PATCH_ENABLE;
	union REG_RDMA_CORE_SET_RING_BASE       SET_RING_BASE;
	union REG_RDMA_CORE_RING_BASE_ADDR_L    RING_BASE_ADDR_L;
	union REG_RDMA_CORE_RING_BASE_ADDR_H    RING_BASE_ADDR_H;
	uint32_t                                _resv_0x38[18];
	union REG_RDMA_CORE_RING_BUFFER_SIZE0   RING_BUFFER_SIZE0;
	union REG_RDMA_CORE_RING_BUFFER_SIZE1   RING_BUFFER_SIZE1;
	union REG_RDMA_CORE_RING_BUFFER_SIZE2   RING_BUFFER_SIZE2;
	union REG_RDMA_CORE_RING_BUFFER_SIZE3   RING_BUFFER_SIZE3;
	union REG_RDMA_CORE_RING_BUFFER_SIZE4   RING_BUFFER_SIZE4;
	union REG_RDMA_CORE_RING_BUFFER_SIZE5   RING_BUFFER_SIZE5;
	union REG_RDMA_CORE_RING_BUFFER_SIZE6   RING_BUFFER_SIZE6;
	union REG_RDMA_CORE_RING_BUFFER_SIZE7   RING_BUFFER_SIZE7;
	union REG_RDMA_CORE_RING_BUFFER_SIZE8   RING_BUFFER_SIZE8;
	union REG_RDMA_CORE_RING_BUFFER_SIZE9   RING_BUFFER_SIZE9;
	union REG_RDMA_CORE_RING_BUFFER_SIZE10  RING_BUFFER_SIZE10;
	union REG_RDMA_CORE_RING_BUFFER_SIZE11  RING_BUFFER_SIZE11;
	union REG_RDMA_CORE_RING_BUFFER_SIZE12  RING_BUFFER_SIZE12;
	union REG_RDMA_CORE_RING_BUFFER_SIZE13  RING_BUFFER_SIZE13;
	union REG_RDMA_CORE_RING_BUFFER_SIZE14  RING_BUFFER_SIZE14;
	union REG_RDMA_CORE_RING_BUFFER_SIZE15  RING_BUFFER_SIZE15;
	union REG_RDMA_CORE_RING_BUFFER_SIZE16  RING_BUFFER_SIZE16;
	union REG_RDMA_CORE_RING_BUFFER_SIZE17  RING_BUFFER_SIZE17;
	union REG_RDMA_CORE_RING_BUFFER_SIZE18  RING_BUFFER_SIZE18;
	union REG_RDMA_CORE_RING_BUFFER_SIZE19  RING_BUFFER_SIZE19;
	union REG_RDMA_CORE_RING_BUFFER_SIZE20  RING_BUFFER_SIZE20;
	union REG_RDMA_CORE_RING_BUFFER_SIZE21  RING_BUFFER_SIZE21;
	union REG_RDMA_CORE_RING_BUFFER_SIZE22  RING_BUFFER_SIZE22;
	union REG_RDMA_CORE_RING_BUFFER_SIZE23  RING_BUFFER_SIZE23;
	union REG_RDMA_CORE_RING_BUFFER_SIZE24  RING_BUFFER_SIZE24;
	union REG_RDMA_CORE_RING_BUFFER_SIZE25  RING_BUFFER_SIZE25;
	union REG_RDMA_CORE_RING_BUFFER_SIZE26  RING_BUFFER_SIZE26;
	union REG_RDMA_CORE_RING_BUFFER_SIZE27  RING_BUFFER_SIZE27;
	union REG_RDMA_CORE_RING_BUFFER_SIZE28  RING_BUFFER_SIZE28;
	union REG_RDMA_CORE_RING_BUFFER_SIZE29  RING_BUFFER_SIZE29;
	union REG_RDMA_CORE_RING_BUFFER_SIZE30  RING_BUFFER_SIZE30;
	union REG_RDMA_CORE_RING_BUFFER_SIZE31  RING_BUFFER_SIZE31;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS0  NEXT_DMA_ADDR_STS0;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS1  NEXT_DMA_ADDR_STS1;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS2  NEXT_DMA_ADDR_STS2;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS3  NEXT_DMA_ADDR_STS3;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS4  NEXT_DMA_ADDR_STS4;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS5  NEXT_DMA_ADDR_STS5;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS6  NEXT_DMA_ADDR_STS6;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS7  NEXT_DMA_ADDR_STS7;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS8  NEXT_DMA_ADDR_STS8;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS9  NEXT_DMA_ADDR_STS9;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS10  NEXT_DMA_ADDR_STS10;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS11  NEXT_DMA_ADDR_STS11;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS12  NEXT_DMA_ADDR_STS12;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS13  NEXT_DMA_ADDR_STS13;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS14  NEXT_DMA_ADDR_STS14;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS15  NEXT_DMA_ADDR_STS15;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS16  NEXT_DMA_ADDR_STS16;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS17  NEXT_DMA_ADDR_STS17;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS18  NEXT_DMA_ADDR_STS18;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS19  NEXT_DMA_ADDR_STS19;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS20  NEXT_DMA_ADDR_STS20;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS21  NEXT_DMA_ADDR_STS21;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS22  NEXT_DMA_ADDR_STS22;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS23  NEXT_DMA_ADDR_STS23;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS24  NEXT_DMA_ADDR_STS24;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS25  NEXT_DMA_ADDR_STS25;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS26  NEXT_DMA_ADDR_STS26;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS27  NEXT_DMA_ADDR_STS27;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS28  NEXT_DMA_ADDR_STS28;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS29  NEXT_DMA_ADDR_STS29;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS30  NEXT_DMA_ADDR_STS30;
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS31  NEXT_DMA_ADDR_STS31;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSI_BDG_LITE_T {
	union REG_ISP_CSI_BDG_LITE_BDG_TOP_CTRL  CSI_BDG_TOP_CTRL;
	union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_0  CSI_BDG_INTERRUPT_CTRL_0;
	union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_1  CSI_BDG_INTERRUPT_CTRL_1;
	union REG_ISP_CSI_BDG_LITE_FRAME_VLD    FRAME_VLD;
	union REG_ISP_CSI_BDG_LITE_CH0_SIZE     CH0_SIZE;
	union REG_ISP_CSI_BDG_LITE_CH1_SIZE     CH1_SIZE;
	union REG_ISP_CSI_BDG_LITE_CH2_SIZE     CH2_SIZE;
	union REG_ISP_CSI_BDG_LITE_CH3_SIZE     CH3_SIZE;
	union REG_ISP_CSI_BDG_LITE_CH0_CROP_EN  CH0_CROP_EN;
	union REG_ISP_CSI_BDG_LITE_CH0_HORZ_CROP  CH0_HORZ_CROP;
	union REG_ISP_CSI_BDG_LITE_CH0_VERT_CROP  CH0_VERT_CROP;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_CSI_BDG_LITE_CH1_CROP_EN  CH1_CROP_EN;
	union REG_ISP_CSI_BDG_LITE_CH1_HORZ_CROP  CH1_HORZ_CROP;
	union REG_ISP_CSI_BDG_LITE_CH1_VERT_CROP  CH1_VERT_CROP;
	uint32_t                                _resv_0x3c[1];
	union REG_ISP_CSI_BDG_LITE_CH2_CROP_EN  CH2_CROP_EN;
	union REG_ISP_CSI_BDG_LITE_CH2_HORZ_CROP  CH2_HORZ_CROP;
	union REG_ISP_CSI_BDG_LITE_CH2_VERT_CROP  CH2_VERT_CROP;
	uint32_t                                _resv_0x4c[1];
	union REG_ISP_CSI_BDG_LITE_CH3_CROP_EN  CH3_CROP_EN;
	union REG_ISP_CSI_BDG_LITE_CH3_HORZ_CROP  CH3_HORZ_CROP;
	union REG_ISP_CSI_BDG_LITE_CH3_VERT_CROP  CH3_VERT_CROP;
	uint32_t                                _resv_0x5c[16];
	union REG_ISP_CSI_BDG_LITE_LINE_INTP_HEIGHT  LINE_INTP_HEIGHT;
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_0  CH0_DEBUG_0;
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_1  CH0_DEBUG_1;
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_2  CH0_DEBUG_2;
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_3  CH0_DEBUG_3;
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_0  CH1_DEBUG_0;
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_1  CH1_DEBUG_1;
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_2  CH1_DEBUG_2;
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_3  CH1_DEBUG_3;
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_0  CH2_DEBUG_0;
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_1  CH2_DEBUG_1;
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_2  CH2_DEBUG_2;
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_3  CH2_DEBUG_3;
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_0  CH3_DEBUG_0;
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_1  CH3_DEBUG_1;
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_2  CH3_DEBUG_2;
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_3  CH3_DEBUG_3;
	union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	union REG_ISP_CSI_BDG_LITE_BDG_DEBUG    BDG_DEBUG;
	uint32_t                                _resv_0xec[1];
	union REG_ISP_CSI_BDG_LITE_WR_URGENT_CTRL  CSI_WR_URGENT_CTRL;
	union REG_ISP_CSI_BDG_LITE_RD_URGENT_CTRL  CSI_RD_URGENT_CTRL;
	union REG_ISP_CSI_BDG_LITE_DUMMY        CSI_DUMMY;
	uint32_t                                _resv_0xfc[21];
	union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_RAW_RDMA_CTRL_T {
	union REG_RAW_RDMA_CTRL_READ_SEL        READ_SEL;
	union REG_RAW_RDMA_CTRL_CONFIG          CONFIG;
	union REG_RAW_RDMA_CTRL_RDMA_SIZE       RDMA_SIZE;
	union REG_RAW_RDMA_CTRL_DPCM_MODE       DPCM_MODE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LDCI_T {
	union REG_ISP_LDCI_ENABLE               LDCI_ENABLE;
	union REG_ISP_LDCI_STRENGTH             LDCI_STRENGTH;
	union REG_ISP_LDCI_LUMA_WGT_MAX         LDCI_LUMA_WGT_MAX;
	union REG_ISP_LDCI_IDX_IIR_ALPHA        LDCI_IDX_IIR_ALPHA;
	union REG_ISP_LDCI_EDGE_SCALE           LDCI_EDGE_SCALE;
	union REG_ISP_LDCI_EDGE_CLAMP           LDCI_EDGE_CLAMP;
	union REG_ISP_LDCI_IDX_FILTER_NORM      LDCI_IDX_FILTER_NORM;
	union REG_ISP_LDCI_TONE_CURVE_IDX_00    LDCI_TONE_CURVE_IDX_00;
	uint32_t                                _resv_0x20[3];
	union REG_ISP_LDCI_BLK_SIZE_X           LDCI_BLK_SIZE_X;
	union REG_ISP_LDCI_BLK_SIZE_X1          LDCI_BLK_SIZE_X1;
	union REG_ISP_LDCI_SUBBLK_SIZE_X        LDCI_SUBBLK_SIZE_X;
	union REG_ISP_LDCI_SUBBLK_SIZE_X1       LDCI_SUBBLK_SIZE_X1;
	union REG_ISP_LDCI_INTERP_NORM_LR       LDCI_INTERP_NORM_LR;
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR   LDCI_SUB_INTERP_NORM_LR;
	union REG_ISP_LDCI_MEAN_NORM_X          LDCI_MEAN_NORM_X;
	union REG_ISP_LDCI_VAR_NORM_Y           LDCI_VAR_NORM_Y;
	union REG_ISP_LDCI_UV_GAIN_MAX          LDCI_UV_GAIN_MAX;
	union REG_ISP_LDCI_IMG_WIDTHM1          LDCI_IMG_WIDTHM1;
	uint32_t                                _resv_0x54[11];
	union REG_ISP_LDCI_STATUS               LDCI_STATUS;
	union REG_ISP_LDCI_GRACE_RESET          LDCI_GRACE_RESET;
	union REG_ISP_LDCI_MONITOR              LDCI_MONITOR;
	union REG_ISP_LDCI_FLOW                 LDCI_FLOW;
	union REG_ISP_LDCI_MONITOR_SELECT       LDCI_MONITOR_SELECT;
	union REG_ISP_LDCI_LOCATION             LDCI_LOCATION;
	union REG_ISP_LDCI_DEBUG                LDCI_DEBUG;
	union REG_ISP_LDCI_DUMMY                LDCI_DUMMY;
	union REG_ISP_LDCI_DMI_ENABLE           DMI_ENABLE;
	uint32_t                                _resv_0xa4[1];
	union REG_ISP_LDCI_DCI_BAYER_STARTING   DCI_BAYER_STARTING;
	uint32_t                                _resv_0xac[1];
	union REG_ISP_LDCI_IDX_FILTER_LUT_00    LDCI_IDX_FILTER_LUT_00;
	union REG_ISP_LDCI_IDX_FILTER_LUT_02    LDCI_IDX_FILTER_LUT_02;
	union REG_ISP_LDCI_IDX_FILTER_LUT_04    LDCI_IDX_FILTER_LUT_04;
	union REG_ISP_LDCI_IDX_FILTER_LUT_06    LDCI_IDX_FILTER_LUT_06;
	union REG_ISP_LDCI_IDX_FILTER_LUT_08    LDCI_IDX_FILTER_LUT_08;
	union REG_ISP_LDCI_IDX_FILTER_LUT_10    LDCI_IDX_FILTER_LUT_10;
	union REG_ISP_LDCI_IDX_FILTER_LUT_12    LDCI_IDX_FILTER_LUT_12;
	union REG_ISP_LDCI_IDX_FILTER_LUT_14    LDCI_IDX_FILTER_LUT_14;
	union REG_ISP_LDCI_INTERP_NORM_LR1      LDCI_INTERP_NORM_LR1;
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR1  LDCI_SUB_INTERP_NORM_LR1;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_00  LDCI_TONE_CURVE_LUT_00_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_02  LDCI_TONE_CURVE_LUT_00_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_04  LDCI_TONE_CURVE_LUT_00_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_06  LDCI_TONE_CURVE_LUT_00_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_08  LDCI_TONE_CURVE_LUT_00_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_10  LDCI_TONE_CURVE_LUT_00_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_12  LDCI_TONE_CURVE_LUT_00_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_14  LDCI_TONE_CURVE_LUT_00_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_00  LDCI_TONE_CURVE_LUT_01_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_02  LDCI_TONE_CURVE_LUT_01_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_04  LDCI_TONE_CURVE_LUT_01_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_06  LDCI_TONE_CURVE_LUT_01_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_08  LDCI_TONE_CURVE_LUT_01_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_10  LDCI_TONE_CURVE_LUT_01_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_12  LDCI_TONE_CURVE_LUT_01_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_14  LDCI_TONE_CURVE_LUT_01_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_00  LDCI_TONE_CURVE_LUT_02_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_02  LDCI_TONE_CURVE_LUT_02_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_04  LDCI_TONE_CURVE_LUT_02_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_06  LDCI_TONE_CURVE_LUT_02_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_08  LDCI_TONE_CURVE_LUT_02_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_10  LDCI_TONE_CURVE_LUT_02_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_12  LDCI_TONE_CURVE_LUT_02_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_14  LDCI_TONE_CURVE_LUT_02_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_00  LDCI_TONE_CURVE_LUT_03_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_02  LDCI_TONE_CURVE_LUT_03_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_04  LDCI_TONE_CURVE_LUT_03_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_06  LDCI_TONE_CURVE_LUT_03_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_08  LDCI_TONE_CURVE_LUT_03_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_10  LDCI_TONE_CURVE_LUT_03_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_12  LDCI_TONE_CURVE_LUT_03_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_14  LDCI_TONE_CURVE_LUT_03_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_00  LDCI_TONE_CURVE_LUT_04_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_02  LDCI_TONE_CURVE_LUT_04_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_04  LDCI_TONE_CURVE_LUT_04_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_06  LDCI_TONE_CURVE_LUT_04_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_08  LDCI_TONE_CURVE_LUT_04_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_10  LDCI_TONE_CURVE_LUT_04_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_12  LDCI_TONE_CURVE_LUT_04_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_14  LDCI_TONE_CURVE_LUT_04_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_00  LDCI_TONE_CURVE_LUT_05_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_02  LDCI_TONE_CURVE_LUT_05_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_04  LDCI_TONE_CURVE_LUT_05_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_06  LDCI_TONE_CURVE_LUT_05_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_08  LDCI_TONE_CURVE_LUT_05_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_10  LDCI_TONE_CURVE_LUT_05_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_12  LDCI_TONE_CURVE_LUT_05_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_14  LDCI_TONE_CURVE_LUT_05_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_00  LDCI_TONE_CURVE_LUT_06_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_02  LDCI_TONE_CURVE_LUT_06_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_04  LDCI_TONE_CURVE_LUT_06_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_06  LDCI_TONE_CURVE_LUT_06_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_08  LDCI_TONE_CURVE_LUT_06_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_10  LDCI_TONE_CURVE_LUT_06_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_12  LDCI_TONE_CURVE_LUT_06_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_14  LDCI_TONE_CURVE_LUT_06_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_00  LDCI_TONE_CURVE_LUT_07_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_02  LDCI_TONE_CURVE_LUT_07_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_04  LDCI_TONE_CURVE_LUT_07_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_06  LDCI_TONE_CURVE_LUT_07_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_08  LDCI_TONE_CURVE_LUT_07_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_10  LDCI_TONE_CURVE_LUT_07_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_12  LDCI_TONE_CURVE_LUT_07_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_14  LDCI_TONE_CURVE_LUT_07_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_00  LDCI_TONE_CURVE_LUT_P_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_02  LDCI_TONE_CURVE_LUT_P_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_04  LDCI_TONE_CURVE_LUT_P_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_06  LDCI_TONE_CURVE_LUT_P_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_08  LDCI_TONE_CURVE_LUT_P_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_10  LDCI_TONE_CURVE_LUT_P_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_12  LDCI_TONE_CURVE_LUT_P_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_14  LDCI_TONE_CURVE_LUT_P_14;
	uint32_t                                _resv_0x1f8[2];
	union REG_ISP_LDCI_LUMA_WGT_LUT_00      LDCI_LUMA_WGT_LUT_00;
	union REG_ISP_LDCI_LUMA_WGT_LUT_04      LDCI_LUMA_WGT_LUT_04;
	union REG_ISP_LDCI_LUMA_WGT_LUT_08      LDCI_LUMA_WGT_LUT_08;
	union REG_ISP_LDCI_LUMA_WGT_LUT_12      LDCI_LUMA_WGT_LUT_12;
	union REG_ISP_LDCI_LUMA_WGT_LUT_16      LDCI_LUMA_WGT_LUT_16;
	union REG_ISP_LDCI_LUMA_WGT_LUT_20      LDCI_LUMA_WGT_LUT_20;
	union REG_ISP_LDCI_LUMA_WGT_LUT_24      LDCI_LUMA_WGT_LUT_24;
	union REG_ISP_LDCI_LUMA_WGT_LUT_28      LDCI_LUMA_WGT_LUT_28;
	union REG_ISP_LDCI_LUMA_WGT_LUT_32      LDCI_LUMA_WGT_LUT_32;
	uint32_t                                _resv_0x224[3];
	union REG_ISP_LDCI_VAR_FILTER_LUT_00    LDCI_VAR_FILTER_LUT_00;
	union REG_ISP_LDCI_VAR_FILTER_LUT_02    LDCI_VAR_FILTER_LUT_02;
	union REG_ISP_LDCI_VAR_FILTER_LUT_04    LDCI_VAR_FILTER_LUT_04;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGB_TOP_T {
	union REG_ISP_RGB_TOP_0                 REG_0;
	union REG_ISP_RGB_TOP_1                 REG_1;
	union REG_ISP_RGB_TOP_2                 REG_2;
	union REG_ISP_RGB_TOP_3                 REG_3;
	union REG_ISP_RGB_TOP_4                 REG_4;
	union REG_ISP_RGB_TOP_5                 REG_5;
	union REG_ISP_RGB_TOP_6                 REG_6;
	union REG_ISP_RGB_TOP_7                 REG_7;
	union REG_ISP_RGB_TOP_8                 REG_8;
	union REG_ISP_RGB_TOP_9                 REG_9;
	uint32_t                                _resv_0x28[2];
	union REG_ISP_RGB_TOP_10                REG_10;
	union REG_ISP_RGB_TOP_11                REG_11;
	union REG_ISP_RGB_TOP_12                REG_12;
	union REG_ISP_RGB_TOP_13                REG_13;
	union REG_ISP_RGB_TOP_14                REG_14;
	uint32_t                                _resv_0x44[3];
	union REG_ISP_RGB_TOP_DBG_IP_S_VLD      DBG_IP_S_VLD;
	union REG_ISP_RGB_TOP_DBG_IP_S_RDY      DBG_IP_S_RDY;
	union REG_ISP_RGB_TOP_DBG_DMI_VLD       DBG_DMI_VLD;
	union REG_ISP_RGB_TOP_DBG_DMI_RDY       DBG_DMI_RDY;
	union REG_ISP_RGB_TOP_PATGEN1           PATGEN1;
	union REG_ISP_RGB_TOP_PATGEN2           PATGEN2;
	union REG_ISP_RGB_TOP_PATGEN3           PATGEN3;
	union REG_ISP_RGB_TOP_PATGEN4           PATGEN4;
	union REG_ISP_RGB_TOP_CHK_SUM           CHK_SUM;
	union REG_ISP_RGB_TOP_DMA_IDLE          DMA_IDLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_PRE_WDMA_CTRL_T {
	union REG_PRE_WDMA_CTRL                 PRE_WDMA_CTRL;
	union REG_PRE_WDMA_CTRL_PRE_RAW_BE_RDMI_DPCM  PRE_RAW_BE_RDMI_DPCM;
	union REG_PRE_WDMA_CTRL_DUMMY           DUMMY;
	uint32_t                                _resv_0xc[12];
	union REG_PRE_WDMA_CTRL_DEBUG_INFO      INFO;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_EE_T {
	union REG_ISP_EE_00                     REG_00;
	union REG_ISP_EE_04                     REG_04;
	uint32_t                                _resv_0x8[1];
	union REG_ISP_EE_0C                     REG_0C;
	union REG_ISP_EE_10                     REG_10;
	uint32_t                                _resv_0x14[36];
	union REG_ISP_EE_A4                     REG_A4;
	union REG_ISP_EE_A8                     REG_A8;
	union REG_ISP_EE_AC                     REG_AC;
	union REG_ISP_EE_B0                     REG_B0;
	union REG_ISP_EE_B4                     REG_B4;
	union REG_ISP_EE_B8                     REG_B8;
	union REG_ISP_EE_BC                     REG_BC;
	union REG_ISP_EE_C0                     REG_C0;
	union REG_ISP_EE_C4                     REG_C4;
	union REG_ISP_EE_C8                     REG_C8;
	union REG_ISP_EE_HCC                    REG_HCC;
	union REG_ISP_EE_HD0                    REG_HD0;
	uint32_t                                _resv_0xd4[23];
	union REG_ISP_EE_130                    REG_130;
	union REG_ISP_EE_134                    REG_134;
	union REG_ISP_EE_138                    REG_138;
	union REG_ISP_EE_13C                    REG_13C;
	union REG_ISP_EE_140                    REG_140;
	union REG_ISP_EE_144                    REG_144;
	union REG_ISP_EE_148                    REG_148;
	union REG_ISP_EE_14C                    REG_14C;
	union REG_ISP_EE_150                    REG_150;
	union REG_ISP_EE_154                    REG_154;
	union REG_ISP_EE_158                    REG_158;
	union REG_ISP_EE_15C                    REG_15C;
	union REG_ISP_EE_160                    REG_160;
	union REG_ISP_EE_164                    REG_164;
	union REG_ISP_EE_168                    REG_168;
	union REG_ISP_EE_16C                    REG_16C;
	union REG_ISP_EE_170                    REG_170;
	union REG_ISP_EE_174                    REG_174;
	union REG_ISP_EE_178                    REG_178;
	union REG_ISP_EE_17C                    REG_17C;
	union REG_ISP_EE_180                    REG_180;
	union REG_ISP_EE_184                    REG_184;
	union REG_ISP_EE_188                    REG_188;
	union REG_ISP_EE_18C                    REG_18C;
	union REG_ISP_EE_190                    REG_190;
	union REG_ISP_EE_194                    REG_194;
	union REG_ISP_EE_198                    REG_198;
	union REG_ISP_EE_19C                    REG_19C;
	union REG_ISP_EE_1A0                    REG_1A0;
	union REG_ISP_EE_1A4                    REG_1A4;
	union REG_ISP_EE_1A8                    REG_1A8;
	uint32_t                                _resv_0x1ac[6];
	union REG_ISP_EE_1C4                    REG_1C4;
	union REG_ISP_EE_1C8                    REG_1C8;
	union REG_ISP_EE_1CC                    REG_1CC;
	union REG_ISP_EE_1D0                    REG_1D0;
	union REG_ISP_EE_1D4                    REG_1D4;
	union REG_ISP_EE_1D8                    REG_1D8;
	union REG_ISP_EE_1DC                    REG_1DC;
	union REG_ISP_EE_1E0                    REG_1E0;
	union REG_ISP_EE_1E4                    REG_1E4;
	union REG_ISP_EE_1E8                    REG_1E8;
	union REG_ISP_EE_1EC                    REG_1EC;
	union REG_ISP_EE_1F0                    REG_1F0;
	union REG_ISP_EE_1F4                    REG_1F4;
	union REG_ISP_EE_1F8                    REG_1F8;
	union REG_ISP_EE_1FC                    REG_1FC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_YGAMMA_T {
	union REG_YGAMMA_GAMMA_CTRL             GAMMA_CTRL;
	union REG_YGAMMA_GAMMA_PROG_CTRL        GAMMA_PROG_CTRL;
	union REG_YGAMMA_GAMMA_PROG_ST_ADDR     GAMMA_PROG_ST_ADDR;
	union REG_YGAMMA_GAMMA_PROG_DATA        GAMMA_PROG_DATA;
	union REG_YGAMMA_GAMMA_PROG_MAX         GAMMA_PROG_MAX;
	union REG_YGAMMA_GAMMA_MEM_SW_RADDR     GAMMA_SW_RADDR;
	union REG_YGAMMA_GAMMA_MEM_SW_RDATA     GAMMA_SW_RDATA;
	union REG_YGAMMA_GAMMA_MEM_SW_RDATA_BG  GAMMA_SW_RDATA_BG;
	union REG_YGAMMA_GAMMA_DBG              GAMMA_DBG;
	union REG_YGAMMA_GAMMA_DMY0             GAMMA_DMY0;
	union REG_YGAMMA_GAMMA_DMY1             GAMMA_DMY1;
	union REG_YGAMMA_GAMMA_DMY_R            GAMMA_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_PREYEE_T {
	union REG_ISP_PREYEE_00                 REG_00;
	union REG_ISP_PREYEE_04                 REG_04;
	uint32_t                                _resv_0x8[1];
	union REG_ISP_PREYEE_0C                 REG_0C;
	union REG_ISP_PREYEE_10                 REG_10;
	uint32_t                                _resv_0x14[36];
	union REG_ISP_PREYEE_A4                 REG_A4;
	union REG_ISP_PREYEE_A8                 REG_A8;
	union REG_ISP_PREYEE_AC                 REG_AC;
	union REG_ISP_PREYEE_B0                 REG_B0;
	union REG_ISP_PREYEE_B4                 REG_B4;
	union REG_ISP_PREYEE_B8                 REG_B8;
	union REG_ISP_PREYEE_BC                 REG_BC;
	union REG_ISP_PREYEE_C0                 REG_C0;
	union REG_ISP_PREYEE_C4                 REG_C4;
	union REG_ISP_PREYEE_C8                 REG_C8;
	union REG_ISP_PREYEE_HCC                REG_HCC;
	union REG_ISP_PREYEE_HD0                REG_HD0;
	uint32_t                                _resv_0xd4[23];
	union REG_ISP_PREYEE_130                REG_130;
	union REG_ISP_PREYEE_134                REG_134;
	union REG_ISP_PREYEE_138                REG_138;
	union REG_ISP_PREYEE_13C                REG_13C;
	union REG_ISP_PREYEE_140                REG_140;
	union REG_ISP_PREYEE_144                REG_144;
	union REG_ISP_PREYEE_148                REG_148;
	union REG_ISP_PREYEE_14C                REG_14C;
	union REG_ISP_PREYEE_150                REG_150;
	union REG_ISP_PREYEE_154                REG_154;
	union REG_ISP_PREYEE_158                REG_158;
	union REG_ISP_PREYEE_15C                REG_15C;
	union REG_ISP_PREYEE_160                REG_160;
	union REG_ISP_PREYEE_164                REG_164;
	union REG_ISP_PREYEE_168                REG_168;
	union REG_ISP_PREYEE_16C                REG_16C;
	union REG_ISP_PREYEE_170                REG_170;
	union REG_ISP_PREYEE_174                REG_174;
	union REG_ISP_PREYEE_178                REG_178;
	union REG_ISP_PREYEE_17C                REG_17C;
	union REG_ISP_PREYEE_180                REG_180;
	union REG_ISP_PREYEE_184                REG_184;
	union REG_ISP_PREYEE_188                REG_188;
	union REG_ISP_PREYEE_18C                REG_18C;
	union REG_ISP_PREYEE_190                REG_190;
	union REG_ISP_PREYEE_194                REG_194;
	union REG_ISP_PREYEE_198                REG_198;
	union REG_ISP_PREYEE_19C                REG_19C;
	union REG_ISP_PREYEE_1A0                REG_1A0;
	union REG_ISP_PREYEE_1A4                REG_1A4;
	union REG_ISP_PREYEE_1A8                REG_1A8;
	uint32_t                                _resv_0x1ac[6];
	union REG_ISP_PREYEE_1C4                REG_1C4;
	union REG_ISP_PREYEE_1C8                REG_1C8;
	union REG_ISP_PREYEE_1CC                REG_1CC;
	union REG_ISP_PREYEE_1D0                REG_1D0;
	union REG_ISP_PREYEE_1D4                REG_1D4;
	union REG_ISP_PREYEE_1D8                REG_1D8;
	union REG_ISP_PREYEE_1DC                REG_1DC;
	union REG_ISP_PREYEE_1E0                REG_1E0;
	union REG_ISP_PREYEE_1E4                REG_1E4;
	union REG_ISP_PREYEE_1E8                REG_1E8;
	union REG_ISP_PREYEE_1EC                REG_1EC;
	union REG_ISP_PREYEE_1F0                REG_1F0;
	uint32_t                                _resv_0x1f4[2];
	union REG_ISP_PREYEE_1FC                REG_1FC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSI_BDG_T {
	union REG_ISP_CSI_BDG_TOP_CTRL          CSI_BDG_TOP_CTRL;
	union REG_ISP_CSI_BDG_INTERRUPT_CTRL    CSI_BDG_INTERRUPT_CTRL;
	union REG_ISP_CSI_BDG_DMA_DPCM_MODE     CSI_BDG_DMA_DPCM_MODE;
	union REG_ISP_CSI_BDG_DMA_LD_DPCM_MODE  CSI_BDG_DMA_LD_DPCM_MODE;
	union REG_ISP_CSI_BDG_CH0_SIZE          CH0_SIZE;
	union REG_ISP_CSI_BDG_CH1_SIZE          CH1_SIZE;
	union REG_ISP_CSI_BDG_CH2_SIZE          CH2_SIZE;
	union REG_ISP_CSI_BDG_CH3_SIZE          CH3_SIZE;
	union REG_ISP_CSI_BDG_CH0_CROP_EN       CH0_CROP_EN;
	union REG_ISP_CSI_BDG_CH0_HORZ_CROP     CH0_HORZ_CROP;
	union REG_ISP_CSI_BDG_CH0_VERT_CROP     CH0_VERT_CROP;
	union REG_ISP_CSI_BDG_CH0_BLC_SUM       CH0_BLC_SUM;
	union REG_ISP_CSI_BDG_CH1_CROP_EN       CH1_CROP_EN;
	union REG_ISP_CSI_BDG_CH1_HORZ_CROP     CH1_HORZ_CROP;
	union REG_ISP_CSI_BDG_CH1_VERT_CROP     CH1_VERT_CROP;
	union REG_ISP_CSI_BDG_CH1_BLC_SUM       CH1_BLC_SUM;
	union REG_ISP_CSI_BDG_CH2_CROP_EN       CH2_CROP_EN;
	union REG_ISP_CSI_BDG_CH2_HORZ_CROP     CH2_HORZ_CROP;
	union REG_ISP_CSI_BDG_CH2_VERT_CROP     CH2_VERT_CROP;
	union REG_ISP_CSI_BDG_CH2_BLC_SUM       CH2_BLC_SUM;
	union REG_ISP_CSI_BDG_CH3_CROP_EN       CH3_CROP_EN;
	union REG_ISP_CSI_BDG_CH3_HORZ_CROP     CH3_HORZ_CROP;
	union REG_ISP_CSI_BDG_CH3_VERT_CROP     CH3_VERT_CROP;
	union REG_ISP_CSI_BDG_CH3_BLC_SUM       CH3_BLC_SUM;
	union REG_ISP_CSI_BDG_PAT_GEN_CTRL      CSI_PAT_GEN_CTRL;
	union REG_ISP_CSI_BDG_PAT_IDX_CTRL      CSI_PAT_IDX_CTRL;
	union REG_ISP_CSI_BDG_PAT_COLOR_0       CSI_PAT_COLOR_0;
	union REG_ISP_CSI_BDG_PAT_COLOR_1       CSI_PAT_COLOR_1;
	union REG_ISP_CSI_BDG_BACKGROUND_COLOR_0  CSI_BACKGROUND_COLOR_0;
	union REG_ISP_CSI_BDG_BACKGROUND_COLOR_1  CSI_BACKGROUND_COLOR_1;
	union REG_ISP_CSI_BDG_FIX_COLOR_0       CSI_FIX_COLOR_0;
	union REG_ISP_CSI_BDG_FIX_COLOR_1       CSI_FIX_COLOR_1;
	union REG_ISP_CSI_BDG_MDE_V_SIZE        CSI_MDE_V_SIZE;
	union REG_ISP_CSI_BDG_MDE_H_SIZE        CSI_MDE_H_SIZE;
	union REG_ISP_CSI_BDG_FDE_V_SIZE        CSI_FDE_V_SIZE;
	union REG_ISP_CSI_BDG_FDE_H_SIZE        CSI_FDE_H_SIZE;
	union REG_ISP_CSI_BDG_HSYNC_CTRL        CSI_HSYNC_CTRL;
	union REG_ISP_CSI_BDG_VSYNC_CTRL        CSI_VSYNC_CTRL;
	union REG_ISP_CSI_BDG_TGEN_TT_SIZE      CSI_TGEN_TT_SIZE;
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_0  LINE_INTP_HEIGHT_0;
	union REG_ISP_CSI_BDG_CH0_DEBUG_0       CH0_DEBUG_0;
	union REG_ISP_CSI_BDG_CH0_DEBUG_1       CH0_DEBUG_1;
	union REG_ISP_CSI_BDG_CH0_DEBUG_2       CH0_DEBUG_2;
	union REG_ISP_CSI_BDG_CH0_DEBUG_3       CH0_DEBUG_3;
	union REG_ISP_CSI_BDG_CH1_DEBUG_0       CH1_DEBUG_0;
	union REG_ISP_CSI_BDG_CH1_DEBUG_1       CH1_DEBUG_1;
	union REG_ISP_CSI_BDG_CH1_DEBUG_2       CH1_DEBUG_2;
	union REG_ISP_CSI_BDG_CH1_DEBUG_3       CH1_DEBUG_3;
	union REG_ISP_CSI_BDG_CH2_DEBUG_0       CH2_DEBUG_0;
	union REG_ISP_CSI_BDG_CH2_DEBUG_1       CH2_DEBUG_1;
	union REG_ISP_CSI_BDG_CH2_DEBUG_2       CH2_DEBUG_2;
	union REG_ISP_CSI_BDG_CH2_DEBUG_3       CH2_DEBUG_3;
	union REG_ISP_CSI_BDG_CH3_DEBUG_0       CH3_DEBUG_0;
	union REG_ISP_CSI_BDG_CH3_DEBUG_1       CH3_DEBUG_1;
	union REG_ISP_CSI_BDG_CH3_DEBUG_2       CH3_DEBUG_2;
	union REG_ISP_CSI_BDG_CH3_DEBUG_3       CH3_DEBUG_3;
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	union REG_ISP_CSI_BDG_DEBUG             BDG_DEBUG;
	union REG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	union REG_ISP_CSI_BDG_WR_URGENT_CTRL    CSI_WR_URGENT_CTRL;
	union REG_ISP_CSI_BDG_RD_URGENT_CTRL    CSI_RD_URGENT_CTRL;
	union REG_ISP_CSI_BDG_DUMMY             CSI_DUMMY;
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_1  LINE_INTP_HEIGHT_1;
	union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_0  SLICE_LINE_INTP_HEIGHT_0;
	union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_1  SLICE_LINE_INTP_HEIGHT_1;
	uint32_t                                _resv_0x108[2];
	union REG_ISP_CSI_BDG_WDMA_CH0_CROP_EN  WDMA_CH0_CROP_EN;
	union REG_ISP_CSI_BDG_WDMA_CH0_HORZ_CROP  WDMA_CH0_HORZ_CROP;
	union REG_ISP_CSI_BDG_WDMA_CH0_VERT_CROP  WDMA_CH0_VERT_CROP;
	uint32_t                                _resv_0x11c[1];
	union REG_ISP_CSI_BDG_WDMA_CH1_CROP_EN  WDMA_CH1_CROP_EN;
	union REG_ISP_CSI_BDG_WDMA_CH1_HORZ_CROP  WDMA_CH1_HORZ_CROP;
	union REG_ISP_CSI_BDG_WDMA_CH1_VERT_CROP  WDMA_CH1_VERT_CROP;
	uint32_t                                _resv_0x12c[1];
	union REG_ISP_CSI_BDG_WDMA_CH2_CROP_EN  WDMA_CH2_CROP_EN;
	union REG_ISP_CSI_BDG_WDMA_CH2_HORZ_CROP  WDMA_CH2_HORZ_CROP;
	union REG_ISP_CSI_BDG_WDMA_CH2_VERT_CROP  WDMA_CH2_VERT_CROP;
	uint32_t                                _resv_0x13c[1];
	union REG_ISP_CSI_BDG_WDMA_CH3_CROP_EN  WDMA_CH3_CROP_EN;
	union REG_ISP_CSI_BDG_WDMA_CH3_HORZ_CROP  WDMA_CH3_HORZ_CROP;
	union REG_ISP_CSI_BDG_WDMA_CH3_VERT_CROP  WDMA_CH3_VERT_CROP;
	uint32_t                                _resv_0x14c[1];
	union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LCAC_T {
	union REG_ISP_LCAC_REG00                REG00;
	union REG_ISP_LCAC_REG04                REG04;
	union REG_ISP_LCAC_REG08                REG08;
	union REG_ISP_LCAC_REG0C                REG0C;
	union REG_ISP_LCAC_REG10                REG10;
	union REG_ISP_LCAC_REG14                REG14;
	union REG_ISP_LCAC_REG18                REG18;
	union REG_ISP_LCAC_REG1C                REG1C;
	union REG_ISP_LCAC_REG20                REG20;
	union REG_ISP_LCAC_REG24                REG24;
	union REG_ISP_LCAC_REG28                REG28;
	union REG_ISP_LCAC_REG2C                REG2C;
	union REG_ISP_LCAC_REG30                REG30;
	union REG_ISP_LCAC_REG34                REG34;
	union REG_ISP_LCAC_REG38                REG38;
	union REG_ISP_LCAC_REG3C                REG3C;
	union REG_ISP_LCAC_REG40                REG40;
	union REG_ISP_LCAC_REG44                REG44;
	union REG_ISP_LCAC_REG48                REG48;
	union REG_ISP_LCAC_REG4C                REG4C;
	union REG_ISP_LCAC_REG50                REG50;
	union REG_ISP_LCAC_REG54                REG54;
	union REG_ISP_LCAC_REG58                REG58;
	union REG_ISP_LCAC_REG5C                REG5C;
	union REG_ISP_LCAC_REG60                REG60;
	union REG_ISP_LCAC_REG64                REG64;
	union REG_ISP_LCAC_REG68                REG68;
	union REG_ISP_LCAC_REG6C                REG6C;
	union REG_ISP_LCAC_REG70                REG70;
	union REG_ISP_LCAC_REG74                REG74;
	union REG_ISP_LCAC_REG78                REG78;
	union REG_ISP_LCAC_REG7C                REG7C;
	union REG_ISP_LCAC_REG80                REG80;
	union REG_ISP_LCAC_REG84                REG84;
	union REG_ISP_LCAC_REG88                REG88;
	union REG_ISP_LCAC_REG8C                REG8C;
	union REG_ISP_LCAC_REG90                REG90;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CFA_T {
	union REG_ISP_CFA_00                    REG_00;
	union REG_ISP_CFA_04                    REG_04;
	uint32_t                                _resv_0x8[1];
	union REG_ISP_CFA_0C                    REG_0C;
	union REG_ISP_CFA_10                    REG_10;
	union REG_ISP_CFA_14                    REG_14;
	union REG_ISP_CFA_18                    REG_18;
	union REG_ISP_CFA_1C                    REG_1C;
	union REG_ISP_CFA_20                    REG_20;
	uint32_t                                _resv_0x24[3];
	union REG_ISP_CFA_30                    REG_30;
	union REG_ISP_CFA_34                    REG_34;
	union REG_ISP_CFA_38                    REG_38;
	union REG_ISP_CFA_3C                    REG_3C;
	union REG_ISP_CFA_40                    REG_40;
	union REG_ISP_CFA_44                    REG_44;
	union REG_ISP_CFA_48                    REG_48;
	union REG_ISP_CFA_4C                    REG_4C;
	uint32_t                                _resv_0x50[8];
	union REG_ISP_CFA_70                    REG_70;
	union REG_ISP_CFA_74                    REG_74;
	union REG_ISP_CFA_78                    REG_78;
	union REG_ISP_CFA_7C                    REG_7C;
	union REG_ISP_CFA_80                    REG_80;
	uint32_t                                _resv_0x84[3];
	union REG_ISP_CFA_90                    REG_90;
	union REG_ISP_CFA_94                    REG_94;
	union REG_ISP_CFA_98                    REG_98;
	union REG_ISP_CFA_9C                    REG_9C;
	union REG_ISP_CFA_A0                    REG_A0;
	union REG_ISP_CFA_A4                    REG_A4;
	union REG_ISP_CFA_A8                    REG_A8;
	uint32_t                                _resv_0xac[25];
	union REG_ISP_CFA_110                   REG_110;
	uint32_t                                _resv_0x114[3];
	union REG_ISP_CFA_120                   REG_120;
	union REG_ISP_CFA_124                   REG_124;
	union REG_ISP_CFA_128                   REG_128;
	union REG_ISP_CFA_12C                   REG_12C;
	union REG_ISP_CFA_130                   REG_130;
	union REG_ISP_CFA_134                   REG_134;
	union REG_ISP_CFA_138                   REG_138;
	union REG_ISP_CFA_13C                   REG_13C;
	union REG_ISP_CFA_140                   REG_140;
	union REG_ISP_CFA_144                   REG_144;
	union REG_ISP_CFA_148                   REG_148;
	union REG_ISP_CFA_14C                   REG_14C;
	union REG_ISP_CFA_150                   REG_150;
	union REG_ISP_CFA_154                   REG_154;
	union REG_ISP_CFA_158                   REG_158;
	union REG_ISP_CFA_15C                   REG_15C;
	union REG_ISP_CFA_160                   REG_160;
	union REG_ISP_CFA_164                   REG_164;
	union REG_ISP_CFA_168                   REG_168;
	union REG_ISP_CFA_16C                   REG_16C;
	union REG_ISP_CFA_170                   REG_170;
	union REG_ISP_CFA_174                   REG_174;
	union REG_ISP_CFA_178                   REG_178;
	union REG_ISP_CFA_17C                   REG_17C;
	union REG_ISP_CFA_180                   REG_180;
	union REG_ISP_CFA_184                   REG_184;
	union REG_ISP_CFA_188                   REG_188;
	union REG_ISP_CFA_18C                   REG_18C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_PRE_RAW_VI_SEL_T {
	union REG_PRE_RAW_VI_SEL_0              REG_0;
	union REG_PRE_RAW_VI_SEL_1              REG_1;
	uint32_t                                _resv_0x8[2];
	union REG_PRE_RAW_VI_SEL_2              REG_2;
	union REG_PRE_RAW_VI_SEL_3              REG_3;
	uint32_t                                _resv_0x18[2];
	union REG_PRE_RAW_VI_SEL_4              REG_4;
	union REG_PRE_RAW_VI_SEL_5              REG_5;
	union REG_PRE_RAW_VI_SEL_6              REG_6;
	union REG_PRE_RAW_VI_SEL_7              REG_7;
	union REG_PRE_RAW_VI_SEL_8              REG_8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_YUV_TOP_T {
	union REG_YUV_TOP_YUV_0                 YUV_0;
	uint32_t                                _resv_0x4[1];
	union REG_YUV_TOP_YUV_2                 YUV_2;
	union REG_YUV_TOP_YUV_3                 YUV_3;
	union REG_YUV_TOP_YUV_DEBUG_0           YUV_DEBUG_0;
	union REG_YUV_TOP_YUV_4                 YUV_4;
	union REG_YUV_TOP_YUV_DEBUG_STATE       YUV_DEBUG_STATE;
	uint32_t                                _resv_0x1c[1];
	union REG_YUV_TOP_YUV_5                 YUV_5;
	uint32_t                                _resv_0x24[15];
	union REG_YUV_TOP_YUV_CTRL              YUV_CTRL;
	union REG_YUV_TOP_IMGW_M1               IMGW_M1;
	uint32_t                                _resv_0x68[1];
	union REG_YUV_TOP_STVALID_STATUS        STVALID_STATUS;
	union REG_YUV_TOP_STREADY_STATUS        STREADY_STATUS;
	union REG_YUV_TOP_PATGEN1               PATGEN1;
	union REG_YUV_TOP_PATGEN2               PATGEN2;
	union REG_YUV_TOP_PATGEN3               PATGEN3;
	union REG_YUV_TOP_PATGEN4               PATGEN4;
	union REG_YUV_TOP_CHECK_SUM             CHECK_SUM;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LSC_T {
	union REG_ISP_LSC_STATUS                LSC_STATUS;
	union REG_ISP_LSC_GRACE_RESET           LSC_GRACE_RESET;
	union REG_ISP_LSC_MONITOR               LSC_MONITOR;
	union REG_ISP_LSC_ENABLE                LSC_ENABLE;
	union REG_ISP_LSC_KICKOFF               LSC_KICKOFF;
	union REG_ISP_LSC_STRENGTH              LSC_STRENGTH;
	union REG_ISP_LSC_IMG_BAYERID           IMG_BAYERID;
	union REG_ISP_LSC_MONITOR_SELECT        LSC_MONITOR_SELECT;
	uint32_t                                _resv_0x20[2];
	union REG_ISP_LSC_DMI_WIDTHM1           LSC_DMI_WIDTHM1;
	union REG_ISP_LSC_DMI_HEIGHTM1          LSC_DMI_HEIGHTM1;
	uint32_t                                _resv_0x30[3];
	union REG_ISP_LSC_GAIN_BASE             LSC_GAIN_BASE;
	union REG_ISP_LSC_XSTEP                 LSC_XSTEP;
	union REG_ISP_LSC_YSTEP                 LSC_YSTEP;
	union REG_ISP_LSC_IMGX0                 LSC_IMGX0;
	union REG_ISP_LSC_IMGY0                 LSC_IMGY0;
	uint32_t                                _resv_0x50[2];
	union REG_ISP_LSC_INITX0                LSC_INITX0;
	union REG_ISP_LSC_INITY0                LSC_INITY0;
	union REG_ISP_LSC_KERNEL_TABLE_WRITE    LSC_KERNEL_TABLE_WRITE;
	union REG_ISP_LSC_KERNEL_TABLE_DATA     LSC_KERNEL_TABLE_DATA;
	union REG_ISP_LSC_KERNEL_TABLE_CTRL     LSC_KERNEL_TABLE_CTRL;
	union REG_ISP_LSC_DUMMY                 LSC_DUMMY;
	union REG_ISP_LSC_LOCATION              LSC_LOCATION;
	union REG_ISP_LSC_1ST_RUNHIT            LSC_1ST_RUNHIT;
	union REG_ISP_LSC_COMPARE_VALUE         LSC_COMPARE_VALUE;
	uint32_t                                _resv_0x7c[1];
	union REG_ISP_LSC_MEM_SW_MODE           LSC_SW_MODE;
	union REG_ISP_LSC_MEM_SW_RADDR          LSC_SW_RADDR;
	uint32_t                                _resv_0x88[1];
	union REG_ISP_LSC_MEM_SW_RDATA          LSC_SW_RDATA;
	union REG_ISP_LSC_INTERPOLATION         INTERPOLATION;
	uint32_t                                _resv_0x94[3];
	union REG_ISP_LSC_DMI_ENABLE            DMI_ENABLE;
	union REG_ISP_LSC_BLD                   LSC_BLD;
	union REG_ISP_LSC_INTP_GAIN_MAX         LSC_INTP_GAIN_MAX;
	union REG_ISP_LSC_INTP_GAIN_MIN         LSC_INTP_GAIN_MIN;
};

#ifdef __cplusplus
}
#endif

#endif /* _VI_REG_BLOCKS_H_ */
