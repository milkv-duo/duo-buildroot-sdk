/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name:vi_vreg_blocks.h
 * Description:HW register description
 */

#ifndef _VI_VREG_BLOCKS_H_
#define _VI_VREG_BLOCKS_H_

#ifdef __cplusplus
extern "C" {
#endif


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
struct VREG_PRE_RAW_BE_TOP_CTRL {
	union REG_PRE_RAW_BE_TOP_CTRL           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_UP_PQ_EN {
	union REG_PRE_RAW_BE_UP_PQ_EN           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_IMG_SIZE_LE {
	union REG_PRE_RAW_BE_IMG_SIZE_LE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_PRE_RAW_DUMMY {
	union REG_PRE_RAW_BE_PRE_RAW_DUMMY      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_DEBUG_INFO {
	union REG_PRE_RAW_BE_DEBUG_INFO         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_DMA_IDLE_INFO {
	union REG_PRE_RAW_BE_DMA_IDLE_INFO      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_IP_IDLE_INFO {
	union REG_PRE_RAW_BE_IP_IDLE_INFO       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_LINE_BALANCE_CTRL {
	union REG_PRE_RAW_BE_LINE_BALANCE_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_DEBUG_ENABLE {
	union REG_PRE_RAW_BE_DEBUG_ENABLE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_TVALID_STATUS {
	union REG_PRE_RAW_BE_TVALID_STATUS      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_TREADY_STATUS {
	union REG_PRE_RAW_BE_TREADY_STATUS      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_PATGEN1 {
	union REG_PRE_RAW_BE_PATGEN1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_PATGEN2 {
	union REG_PRE_RAW_BE_PATGEN2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_PATGEN3 {
	union REG_PRE_RAW_BE_PATGEN3            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_PATGEN4 {
	union REG_PRE_RAW_BE_PATGEN4            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_CHKSUM_ENABLE {
	union REG_PRE_RAW_BE_CHKSUM_ENABLE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_CHKSUM {
	union REG_PRE_RAW_BE_CHKSUM             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_BE_T {
	struct VREG_PRE_RAW_BE_TOP_CTRL          TOP_CTRL;
	struct VREG_PRE_RAW_BE_UP_PQ_EN          UP_PQ_EN;
	struct VREG_PRE_RAW_BE_IMG_SIZE_LE       IMG_SIZE_LE;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_PRE_RAW_BE_PRE_RAW_DUMMY     PRE_RAW_DUMMY;
	struct VREG_PRE_RAW_BE_DEBUG_INFO        BE_INFO;
	struct VREG_PRE_RAW_BE_DMA_IDLE_INFO     BE_DMA_IDLE_INFO;
	struct VREG_PRE_RAW_BE_IP_IDLE_INFO      BE_IP_IDLE_INFO;
	struct VREG_PRE_RAW_BE_LINE_BALANCE_CTRL  LINE_BALANCE_CTRL;
	struct VREG_PRE_RAW_BE_DEBUG_ENABLE      DEBUG_ENABLE;
	struct VREG_PRE_RAW_BE_TVALID_STATUS     TVALID_STATUS;
	struct VREG_PRE_RAW_BE_TREADY_STATUS     TREADY_STATUS;
	struct VREG_PRE_RAW_BE_PATGEN1           PATGEN1;
	struct VREG_PRE_RAW_BE_PATGEN2           PATGEN2;
	struct VREG_PRE_RAW_BE_PATGEN3           PATGEN3;
	struct VREG_PRE_RAW_BE_PATGEN4           PATGEN4;
	struct VREG_PRE_RAW_BE_CHKSUM_ENABLE     CHKSUM_ENABLE;
	struct VREG_PRE_RAW_BE_CHKSUM            CHKSUM;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DMA_CTL_SYS_CONTROL {
	union REG_ISP_DMA_CTL_SYS_CONTROL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_BASE_ADDR {
	union REG_ISP_DMA_CTL_BASE_ADDR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_SEGLEN {
	union REG_ISP_DMA_CTL_DMA_SEGLEN        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_STRIDE {
	union REG_ISP_DMA_CTL_DMA_STRIDE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_SEGNUM {
	union REG_ISP_DMA_CTL_DMA_SEGNUM        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_STATUS {
	union REG_ISP_DMA_CTL_DMA_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_SLICESIZE {
	union REG_ISP_DMA_CTL_DMA_SLICESIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_DMA_DUMMY {
	union REG_ISP_DMA_CTL_DMA_DUMMY         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DMA_CTL_T {
	struct VREG_ISP_DMA_CTL_SYS_CONTROL      SYS_CONTROL;
	struct VREG_ISP_DMA_CTL_BASE_ADDR        BASE_ADDR;
	struct VREG_ISP_DMA_CTL_DMA_SEGLEN       DMA_SEGLEN;
	struct VREG_ISP_DMA_CTL_DMA_STRIDE       DMA_STRIDE;
	struct VREG_ISP_DMA_CTL_DMA_SEGNUM       DMA_SEGNUM;
	struct VREG_ISP_DMA_CTL_DMA_STATUS       DMA_STATUS;
	struct VREG_ISP_DMA_CTL_DMA_SLICESIZE    DMA_SLICESIZE;
	struct VREG_ISP_DMA_CTL_DMA_DUMMY        DMA_DUMMY;
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

struct VREG_ISP_AE_HIST_STS_AE0_HIST_ENABLE {
	union REG_ISP_AE_HIST_STS_AE0_HIST_ENABLE  write;
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

struct VREG_ISP_AE_HIST_STS_IR_AE_OFFSETX {
	union REG_ISP_AE_HIST_STS_IR_AE_OFFSETX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_OFFSETY {
	union REG_ISP_AE_HIST_STS_IR_AE_OFFSETY  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_NUMXM1 {
	union REG_ISP_AE_HIST_STS_IR_AE_NUMXM1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_NUMYM1 {
	union REG_ISP_AE_HIST_STS_IR_AE_NUMYM1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_WIDTH {
	union REG_ISP_AE_HIST_STS_IR_AE_WIDTH   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_HEIGHT {
	union REG_ISP_AE_HIST_STS_IR_AE_HEIGHT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_IR_AE_STS_DIV {
	union REG_ISP_AE_HIST_STS_IR_AE_STS_DIV  write;
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

struct VREG_ISP_AE_HIST_WBG_7 {
	union REG_ISP_AE_HIST_WBG_7             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_DMI_ENABLE {
	union REG_ISP_AE_HIST_DMI_ENABLE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE0_LOCATION {
	union REG_ISP_AE_HIST_AE_FACE0_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE1_LOCATION {
	union REG_ISP_AE_HIST_AE_FACE1_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE2_LOCATION {
	union REG_ISP_AE_HIST_AE_FACE2_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE3_LOCATION {
	union REG_ISP_AE_HIST_AE_FACE3_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE0_SIZE {
	union REG_ISP_AE_HIST_AE_FACE0_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE1_SIZE {
	union REG_ISP_AE_HIST_AE_FACE1_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE2_SIZE {
	union REG_ISP_AE_HIST_AE_FACE2_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE3_SIZE {
	union REG_ISP_AE_HIST_AE_FACE3_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE0_LOCATION {
	union REG_ISP_AE_HIST_IR_AE_FACE0_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE1_LOCATION {
	union REG_ISP_AE_HIST_IR_AE_FACE1_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE2_LOCATION {
	union REG_ISP_AE_HIST_IR_AE_FACE2_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE3_LOCATION {
	union REG_ISP_AE_HIST_IR_AE_FACE3_LOCATION  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE0_SIZE {
	union REG_ISP_AE_HIST_IR_AE_FACE0_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE1_SIZE {
	union REG_ISP_AE_HIST_IR_AE_FACE1_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE2_SIZE {
	union REG_ISP_AE_HIST_IR_AE_FACE2_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_IR_AE_FACE3_SIZE {
	union REG_ISP_AE_HIST_IR_AE_FACE3_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE0_ENABLE {
	union REG_ISP_AE_HIST_AE_FACE0_ENABLE   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE0_STS_DIV {
	union REG_ISP_AE_HIST_AE_FACE0_STS_DIV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE1_STS_DIV {
	union REG_ISP_AE_HIST_AE_FACE1_STS_DIV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE2_STS_DIV {
	union REG_ISP_AE_HIST_AE_FACE2_STS_DIV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_FACE3_STS_DIV {
	union REG_ISP_AE_HIST_AE_FACE3_STS_DIV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_STS_ENABLE {
	union REG_ISP_AE_HIST_STS_ENABLE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_ALGO_ENABLE {
	union REG_ISP_AE_HIST_AE_ALGO_ENABLE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_LOW {
	union REG_ISP_AE_HIST_AE_HIST_LOW       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_HIST_HIGH {
	union REG_ISP_AE_HIST_AE_HIST_HIGH      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_TOP {
	union REG_ISP_AE_HIST_AE_TOP            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_BOT {
	union REG_ISP_AE_HIST_AE_BOT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_OVEREXP_THR {
	union REG_ISP_AE_HIST_AE_OVEREXP_THR    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_NUM_GAPLINE {
	union REG_ISP_AE_HIST_AE_NUM_GAPLINE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_LUMA {
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_LUMA  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_RGAIN {
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_RGAIN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_BGAIN {
	union REG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_BGAIN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_00 {
	union REG_ISP_AE_HIST_AE_WGT_00         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_01 {
	union REG_ISP_AE_HIST_AE_WGT_01         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_02 {
	union REG_ISP_AE_HIST_AE_WGT_02         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_03 {
	union REG_ISP_AE_HIST_AE_WGT_03         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_04 {
	union REG_ISP_AE_HIST_AE_WGT_04         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_05 {
	union REG_ISP_AE_HIST_AE_WGT_05         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_06 {
	union REG_ISP_AE_HIST_AE_WGT_06         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_07 {
	union REG_ISP_AE_HIST_AE_WGT_07         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_08 {
	union REG_ISP_AE_HIST_AE_WGT_08         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_09 {
	union REG_ISP_AE_HIST_AE_WGT_09         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_10 {
	union REG_ISP_AE_HIST_AE_WGT_10         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_11 {
	union REG_ISP_AE_HIST_AE_WGT_11         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_12 {
	union REG_ISP_AE_HIST_AE_WGT_12         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_13 {
	union REG_ISP_AE_HIST_AE_WGT_13         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_14 {
	union REG_ISP_AE_HIST_AE_WGT_14         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_15 {
	union REG_ISP_AE_HIST_AE_WGT_15         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_16 {
	union REG_ISP_AE_HIST_AE_WGT_16         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_17 {
	union REG_ISP_AE_HIST_AE_WGT_17         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_18 {
	union REG_ISP_AE_HIST_AE_WGT_18         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_19 {
	union REG_ISP_AE_HIST_AE_WGT_19         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_20 {
	union REG_ISP_AE_HIST_AE_WGT_20         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_21 {
	union REG_ISP_AE_HIST_AE_WGT_21         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_22 {
	union REG_ISP_AE_HIST_AE_WGT_22         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_23 {
	union REG_ISP_AE_HIST_AE_WGT_23         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_24 {
	union REG_ISP_AE_HIST_AE_WGT_24         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_25 {
	union REG_ISP_AE_HIST_AE_WGT_25         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_26 {
	union REG_ISP_AE_HIST_AE_WGT_26         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_27 {
	union REG_ISP_AE_HIST_AE_WGT_27         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_28 {
	union REG_ISP_AE_HIST_AE_WGT_28         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_29 {
	union REG_ISP_AE_HIST_AE_WGT_29         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_30 {
	union REG_ISP_AE_HIST_AE_WGT_30         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_AE_WGT_31 {
	union REG_ISP_AE_HIST_AE_WGT_31         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AE_HIST_T {
	struct VREG_ISP_AE_HIST_AE_HIST_STATUS   AE_HIST_STATUS;
	struct VREG_ISP_AE_HIST_AE_HIST_GRACE_RESET  AE_HIST_GRACE_RESET;
	struct VREG_ISP_AE_HIST_AE_HIST_MONITOR  AE_HIST_MONITOR;
	struct VREG_ISP_AE_HIST_AE_HIST_BYPASS   AE_HIST_BYPASS;
	struct VREG_ISP_AE_HIST_AE_KICKOFF       AE_KICKOFF;
	struct VREG_ISP_AE_HIST_STS_AE0_HIST_ENABLE  STS_AE0_HIST_ENABLE;
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
	struct VREG_ISP_AE_HIST_STS_IR_AE_OFFSETX  STS_IR_AE_OFFSETX;
	struct VREG_ISP_AE_HIST_STS_IR_AE_OFFSETY  STS_IR_AE_OFFSETY;
	struct VREG_ISP_AE_HIST_STS_IR_AE_NUMXM1  STS_IR_AE_NUMXM1;
	struct VREG_ISP_AE_HIST_STS_IR_AE_NUMYM1  STS_IR_AE_NUMYM1;
	struct VREG_ISP_AE_HIST_STS_IR_AE_WIDTH  STS_IR_AE_WIDTH;
	struct VREG_ISP_AE_HIST_STS_IR_AE_HEIGHT  STS_IR_AE_HEIGHT;
	struct VREG_ISP_AE_HIST_STS_IR_AE_STS_DIV  STS_IR_AE_STS_DIV;
	struct VREG_RESV                        _resv_0x64[1];
	struct VREG_ISP_AE_HIST_AE_HIST_BAYER_STARTING  AE_HIST_BAYER_STARTING;
	struct VREG_ISP_AE_HIST_AE_HIST_DUMMY    AE_HIST_DUMMY;
	struct VREG_ISP_AE_HIST_AE_HIST_CHECKSUM  AE_HIST_CHECKSUM;
	struct VREG_ISP_AE_HIST_WBG_4            WBG_4;
	struct VREG_ISP_AE_HIST_WBG_5            WBG_5;
	struct VREG_ISP_AE_HIST_WBG_6            WBG_6;
	struct VREG_ISP_AE_HIST_WBG_7            WBG_7;
	struct VREG_RESV                        _resv_0x84[7];
	struct VREG_ISP_AE_HIST_DMI_ENABLE       DMI_ENABLE;
	struct VREG_RESV                        _resv_0xa4[3];
	struct VREG_ISP_AE_HIST_AE_FACE0_LOCATION  AE_FACE0_LOCATION;
	struct VREG_ISP_AE_HIST_AE_FACE1_LOCATION  AE_FACE1_LOCATION;
	struct VREG_ISP_AE_HIST_AE_FACE2_LOCATION  AE_FACE2_LOCATION;
	struct VREG_ISP_AE_HIST_AE_FACE3_LOCATION  AE_FACE3_LOCATION;
	struct VREG_ISP_AE_HIST_AE_FACE0_SIZE    AE_FACE0_SIZE;
	struct VREG_ISP_AE_HIST_AE_FACE1_SIZE    AE_FACE1_SIZE;
	struct VREG_ISP_AE_HIST_AE_FACE2_SIZE    AE_FACE2_SIZE;
	struct VREG_ISP_AE_HIST_AE_FACE3_SIZE    AE_FACE3_SIZE;
	struct VREG_ISP_AE_HIST_IR_AE_FACE0_LOCATION  IR_AE_FACE0_LOCATION;
	struct VREG_ISP_AE_HIST_IR_AE_FACE1_LOCATION  IR_AE_FACE1_LOCATION;
	struct VREG_ISP_AE_HIST_IR_AE_FACE2_LOCATION  IR_AE_FACE2_LOCATION;
	struct VREG_ISP_AE_HIST_IR_AE_FACE3_LOCATION  IR_AE_FACE3_LOCATION;
	struct VREG_ISP_AE_HIST_IR_AE_FACE0_SIZE  IR_AE_FACE0_SIZE;
	struct VREG_ISP_AE_HIST_IR_AE_FACE1_SIZE  IR_AE_FACE1_SIZE;
	struct VREG_ISP_AE_HIST_IR_AE_FACE2_SIZE  IR_AE_FACE2_SIZE;
	struct VREG_ISP_AE_HIST_IR_AE_FACE3_SIZE  IR_AE_FACE3_SIZE;
	struct VREG_ISP_AE_HIST_AE_FACE0_ENABLE  AE_FACE0_ENABLE;
	struct VREG_ISP_AE_HIST_AE_FACE0_STS_DIV  AE_FACE0_STS_DIV;
	struct VREG_ISP_AE_HIST_AE_FACE1_STS_DIV  AE_FACE1_STS_DIV;
	struct VREG_ISP_AE_HIST_AE_FACE2_STS_DIV  AE_FACE2_STS_DIV;
	struct VREG_ISP_AE_HIST_AE_FACE3_STS_DIV  AE_FACE3_STS_DIV;
	struct VREG_ISP_AE_HIST_STS_ENABLE       STS_ENABLE;
	struct VREG_ISP_AE_HIST_AE_ALGO_ENABLE   AE_ALGO_ENABLE;
	struct VREG_ISP_AE_HIST_AE_HIST_LOW      AE_HIST_LOW;
	struct VREG_ISP_AE_HIST_AE_HIST_HIGH     AE_HIST_HIGH;
	struct VREG_ISP_AE_HIST_AE_TOP           AE_TOP;
	struct VREG_ISP_AE_HIST_AE_BOT           AE_BOT;
	struct VREG_ISP_AE_HIST_AE_OVEREXP_THR   AE_OVEREXP_THR;
	struct VREG_ISP_AE_HIST_AE_NUM_GAPLINE   AE_NUM_GAPLINE;
	struct VREG_RESV                        _resv_0x124[23];
	struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_LUMA  AE_SIMPLE2A_RESULT_LUMA;
	struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_RGAIN  AE_SIMPLE2A_RESULT_RGAIN;
	struct VREG_ISP_AE_HIST_AE_SIMPLE2A_RESULT_BGAIN  AE_SIMPLE2A_RESULT_BGAIN;
	struct VREG_RESV                        _resv_0x18c[29];
	struct VREG_ISP_AE_HIST_AE_WGT_00        AE_WGT_00;
	struct VREG_ISP_AE_HIST_AE_WGT_01        AE_WGT_01;
	struct VREG_ISP_AE_HIST_AE_WGT_02        AE_WGT_02;
	struct VREG_ISP_AE_HIST_AE_WGT_03        AE_WGT_03;
	struct VREG_ISP_AE_HIST_AE_WGT_04        AE_WGT_04;
	struct VREG_ISP_AE_HIST_AE_WGT_05        AE_WGT_05;
	struct VREG_ISP_AE_HIST_AE_WGT_06        AE_WGT_06;
	struct VREG_ISP_AE_HIST_AE_WGT_07        AE_WGT_07;
	struct VREG_ISP_AE_HIST_AE_WGT_08        AE_WGT_08;
	struct VREG_ISP_AE_HIST_AE_WGT_09        AE_WGT_09;
	struct VREG_ISP_AE_HIST_AE_WGT_10        AE_WGT_10;
	struct VREG_ISP_AE_HIST_AE_WGT_11        AE_WGT_11;
	struct VREG_ISP_AE_HIST_AE_WGT_12        AE_WGT_12;
	struct VREG_ISP_AE_HIST_AE_WGT_13        AE_WGT_13;
	struct VREG_ISP_AE_HIST_AE_WGT_14        AE_WGT_14;
	struct VREG_ISP_AE_HIST_AE_WGT_15        AE_WGT_15;
	struct VREG_ISP_AE_HIST_AE_WGT_16        AE_WGT_16;
	struct VREG_ISP_AE_HIST_AE_WGT_17        AE_WGT_17;
	struct VREG_ISP_AE_HIST_AE_WGT_18        AE_WGT_18;
	struct VREG_ISP_AE_HIST_AE_WGT_19        AE_WGT_19;
	struct VREG_ISP_AE_HIST_AE_WGT_20        AE_WGT_20;
	struct VREG_ISP_AE_HIST_AE_WGT_21        AE_WGT_21;
	struct VREG_ISP_AE_HIST_AE_WGT_22        AE_WGT_22;
	struct VREG_ISP_AE_HIST_AE_WGT_23        AE_WGT_23;
	struct VREG_ISP_AE_HIST_AE_WGT_24        AE_WGT_24;
	struct VREG_ISP_AE_HIST_AE_WGT_25        AE_WGT_25;
	struct VREG_ISP_AE_HIST_AE_WGT_26        AE_WGT_26;
	struct VREG_ISP_AE_HIST_AE_WGT_27        AE_WGT_27;
	struct VREG_ISP_AE_HIST_AE_WGT_28        AE_WGT_28;
	struct VREG_ISP_AE_HIST_AE_WGT_29        AE_WGT_29;
	struct VREG_ISP_AE_HIST_AE_WGT_30        AE_WGT_30;
	struct VREG_ISP_AE_HIST_AE_WGT_31        AE_WGT_31;
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

struct VREG_ISP_GMS_X_SIZEM1 {
	union REG_ISP_GMS_X_SIZEM1              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_Y_SIZEM1 {
	union REG_ISP_GMS_Y_SIZEM1              write;
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

struct VREG_ISP_GMS_DMI_ENABLE {
	union REG_ISP_GMS_DMI_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GMS_T {
	struct VREG_ISP_GMS_STATUS               GMS_STATUS;
	struct VREG_ISP_GMS_GRACE_RESET          GMS_GRACE_RESET;
	struct VREG_ISP_GMS_MONITOR              GMS_MONITOR;
	struct VREG_ISP_GMS_ENABLE               GMS_ENABLE;
	struct VREG_RESV                        _resv_0x10[1];
	struct VREG_ISP_GMS_FLOW                 GMS_FLOW;
	struct VREG_ISP_GMS_START_X              GMS_START_X;
	struct VREG_ISP_GMS_START_Y              GMS_START_Y;
	struct VREG_ISP_GMS_LOCATION             GMS_LOCATION;
	struct VREG_RESV                        _resv_0x24[1];
	struct VREG_ISP_GMS_X_SIZEM1             GMS_X_SIZEM1;
	struct VREG_ISP_GMS_Y_SIZEM1             GMS_Y_SIZEM1;
	struct VREG_ISP_GMS_X_GAP                GMS_X_GAP;
	struct VREG_ISP_GMS_Y_GAP                GMS_Y_GAP;
	struct VREG_ISP_GMS_DUMMY                GMS_DUMMY;
	struct VREG_RESV                        _resv_0x3c[1];
	struct VREG_ISP_GMS_MEM_SW_MODE          GMS_SW_MODE;
	struct VREG_ISP_GMS_MEM_SW_RADDR         GMS_SW_RADDR;
	struct VREG_ISP_GMS_MEM_SW_RDATA         GMS_SW_RDATA;
	struct VREG_ISP_GMS_MONITOR_SELECT       GMS_MONITOR_SELECT;
	struct VREG_RESV                        _resv_0x50[20];
	struct VREG_ISP_GMS_DMI_ENABLE           DMI_ENABLE;
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

struct VREG_ISP_AF_TH_LOW    {
	union REG_ISP_AF_TH_LOW                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_GAIN_LOW  {
	union REG_ISP_AF_GAIN_LOW               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_SLOP_LOW {
	union REG_ISP_AF_SLOP_LOW               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_DMI_ENABLE {
	union REG_ISP_AF_DMI_ENABLE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_SQUARE_ENABLE {
	union REG_ISP_AF_SQUARE_ENABLE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_OUTSHIFT {
	union REG_ISP_AF_OUTSHIFT               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_AF_NUM_GAPLINE {
	union REG_ISP_AF_NUM_GAPLINE            write;
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
	struct VREG_RESV                        _resv_0x30[1];
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
	struct VREG_ISP_AF_TH_LOW                TH_LOW;
	struct VREG_ISP_AF_GAIN_LOW              GAIN_LOW;
	struct VREG_ISP_AF_SLOP_LOW              SLOP_LOW;
	struct VREG_RESV                        _resv_0x8c[5];
	struct VREG_ISP_AF_DMI_ENABLE            DMI_ENABLE;
	struct VREG_RESV                        _resv_0xa4[45];
	struct VREG_ISP_AF_SQUARE_ENABLE         SQUARE_ENABLE;
	struct VREG_RESV                        _resv_0x15c[2];
	struct VREG_ISP_AF_OUTSHIFT              OUTSHIFT;
	struct VREG_RESV                        _resv_0x168[1];
	struct VREG_ISP_AF_NUM_GAPLINE           NUM_GAPLINE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_PRE_RAW_FE_PRE_RAW_CTRL {
	union REG_PRE_RAW_FE_PRE_RAW_CTRL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_FRAME_SIZE {
	union REG_PRE_RAW_FE_PRE_RAW_FRAME_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_LE_RGBMAP_GRID_NUMBER {
	union REG_PRE_RAW_FE_LE_RGBMAP_GRID_NUMBER  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_SE_RGBMAP_GRID_NUMBER {
	union REG_PRE_RAW_FE_SE_RGBMAP_GRID_NUMBER  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_POST_NO_RSPD_CYC {
	union REG_PRE_RAW_FE_PRE_RAW_POST_NO_RSPD_CYC  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC {
	union REG_PRE_RAW_FE_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_FRAME_VLD {
	union REG_PRE_RAW_FE_PRE_RAW_FRAME_VLD  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_DEBUG_STATE {
	union REG_PRE_RAW_FE_PRE_RAW_DEBUG_STATE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_DUMMY {
	union REG_PRE_RAW_FE_PRE_RAW_DUMMY      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_PRE_RAW_DEBUG_INFO {
	union REG_PRE_RAW_FE_PRE_RAW_DEBUG_INFO  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_IDLE_INFO {
	union REG_PRE_RAW_FE_IDLE_INFO          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_CHECK_SUM {
	union REG_PRE_RAW_FE_CHECK_SUM          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_CHECK_SUM_VALUE {
	union REG_PRE_RAW_FE_CHECK_SUM_VALUE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_FE_T {
	struct VREG_PRE_RAW_FE_PRE_RAW_CTRL      PRE_RAW_CTRL;
	struct VREG_PRE_RAW_FE_PRE_RAW_FRAME_SIZE  PRE_RAW_FRAME_SIZE;
	struct VREG_RESV                        _resv_0x8[2];
	struct VREG_PRE_RAW_FE_LE_RGBMAP_GRID_NUMBER  LE_RGBMAP_GRID_NUMBER;
	struct VREG_PRE_RAW_FE_SE_RGBMAP_GRID_NUMBER  SE_RGBMAP_GRID_NUMBER;
	struct VREG_RESV                        _resv_0x18[2];
	struct VREG_PRE_RAW_FE_PRE_RAW_POST_NO_RSPD_CYC  PRE_RAW_POST_NO_RSPD_CYC;
	struct VREG_PRE_RAW_FE_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  PRE_RAW_POST_RGBMAP_NO_RSPD_CYC;
	struct VREG_PRE_RAW_FE_PRE_RAW_FRAME_VLD  PRE_RAW_FRAME_VLD;
	struct VREG_PRE_RAW_FE_PRE_RAW_DEBUG_STATE  PRE_RAW_DEBUG_STATE;
	struct VREG_PRE_RAW_FE_PRE_RAW_DUMMY     PRE_RAW_DUMMY;
	struct VREG_PRE_RAW_FE_PRE_RAW_DEBUG_INFO  PRE_RAW_INFO;
	struct VREG_RESV                        _resv_0x38[6];
	struct VREG_PRE_RAW_FE_IDLE_INFO         FE_IDLE_INFO;
	struct VREG_RESV                        _resv_0x54[3];
	struct VREG_PRE_RAW_FE_CHECK_SUM         FE_CHECK_SUM;
	struct VREG_PRE_RAW_FE_CHECK_SUM_VALUE   FE_CHECK_SUM_VALUE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CSI_BDG_DVP_BDG_TOP_CTRL {
	union REG_ISP_CSI_BDG_DVP_BDG_TOP_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BDG_INTERRUPT_CTRL {
	union REG_ISP_CSI_BDG_DVP_BDG_INTERRUPT_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BDG_DMA_DPCM_MODE {
	union REG_ISP_CSI_BDG_DVP_BDG_DMA_DPCM_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BDG_DMA_LD_DPCM_MODE {
	union REG_ISP_CSI_BDG_DVP_BDG_DMA_LD_DPCM_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_SIZE {
	union REG_ISP_CSI_BDG_DVP_CH0_SIZE      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_CROP_EN {
	union REG_ISP_CSI_BDG_DVP_CH0_CROP_EN   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_HORZ_CROP {
	union REG_ISP_CSI_BDG_DVP_CH0_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_VERT_CROP {
	union REG_ISP_CSI_BDG_DVP_CH0_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_BLC_SUM {
	union REG_ISP_CSI_BDG_DVP_CH0_BLC_SUM   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_PAT_GEN_CTRL {
	union REG_ISP_CSI_BDG_DVP_PAT_GEN_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_PAT_IDX_CTRL {
	union REG_ISP_CSI_BDG_DVP_PAT_IDX_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_PAT_COLOR_0 {
	union REG_ISP_CSI_BDG_DVP_PAT_COLOR_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_PAT_COLOR_1 {
	union REG_ISP_CSI_BDG_DVP_PAT_COLOR_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_0 {
	union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_1 {
	union REG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_FIX_COLOR_0 {
	union REG_ISP_CSI_BDG_DVP_FIX_COLOR_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_FIX_COLOR_1 {
	union REG_ISP_CSI_BDG_DVP_FIX_COLOR_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_MDE_V_SIZE {
	union REG_ISP_CSI_BDG_DVP_MDE_V_SIZE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_MDE_H_SIZE {
	union REG_ISP_CSI_BDG_DVP_MDE_H_SIZE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_FDE_V_SIZE {
	union REG_ISP_CSI_BDG_DVP_FDE_V_SIZE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_FDE_H_SIZE {
	union REG_ISP_CSI_BDG_DVP_FDE_H_SIZE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_HSYNC_CTRL {
	union REG_ISP_CSI_BDG_DVP_HSYNC_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_VSYNC_CTRL {
	union REG_ISP_CSI_BDG_DVP_VSYNC_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_TGEN_TT_SIZE {
	union REG_ISP_CSI_BDG_DVP_TGEN_TT_SIZE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_LINE_INTP_HEIGHT_0 {
	union REG_ISP_CSI_BDG_DVP_LINE_INTP_HEIGHT_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_0 {
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_1 {
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_2 {
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_2   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_3 {
	union REG_ISP_CSI_BDG_DVP_CH0_DEBUG_3   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_0 {
	union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_1 {
	union REG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_BDG_DEBUG {
	union REG_ISP_CSI_BDG_DVP_BDG_DEBUG     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_OUT_VSYNC_LINE_DELAY {
	union REG_ISP_CSI_BDG_DVP_OUT_VSYNC_LINE_DELAY  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_WR_URGENT_CTRL {
	union REG_ISP_CSI_BDG_DVP_WR_URGENT_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_RD_URGENT_CTRL {
	union REG_ISP_CSI_BDG_DVP_RD_URGENT_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_DUMMY {
	union REG_ISP_CSI_BDG_DVP_DUMMY         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_SLICE_LINE_INTP_HEIGHT_0 {
	union REG_ISP_CSI_BDG_DVP_SLICE_LINE_INTP_HEIGHT_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_CROP_EN {
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_HORZ_CROP {
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_VERT_CROP {
	union REG_ISP_CSI_BDG_DVP_WDMA_CH0_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_0 {
	union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_1 {
	union REG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DVP_T {
	struct VREG_ISP_CSI_BDG_DVP_BDG_TOP_CTRL  CSI_BDG_TOP_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_BDG_INTERRUPT_CTRL  CSI_BDG_INTERRUPT_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_BDG_DMA_DPCM_MODE  CSI_BDG_DMA_DPCM_MODE;
	struct VREG_ISP_CSI_BDG_DVP_BDG_DMA_LD_DPCM_MODE  CSI_BDG_DMA_LD_DPCM_MODE;
	struct VREG_ISP_CSI_BDG_DVP_CH0_SIZE     CH0_SIZE;
	struct VREG_RESV                        _resv_0x14[3];
	struct VREG_ISP_CSI_BDG_DVP_CH0_CROP_EN  CH0_CROP_EN;
	struct VREG_ISP_CSI_BDG_DVP_CH0_HORZ_CROP  CH0_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_DVP_CH0_VERT_CROP  CH0_VERT_CROP;
	struct VREG_ISP_CSI_BDG_DVP_CH0_BLC_SUM  CH0_BLC_SUM;
	struct VREG_RESV                        _resv_0x30[12];
	struct VREG_ISP_CSI_BDG_DVP_PAT_GEN_CTRL  CSI_PAT_GEN_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_PAT_IDX_CTRL  CSI_PAT_IDX_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_PAT_COLOR_0  CSI_PAT_COLOR_0;
	struct VREG_ISP_CSI_BDG_DVP_PAT_COLOR_1  CSI_PAT_COLOR_1;
	struct VREG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_0  CSI_BACKGROUND_COLOR_0;
	struct VREG_ISP_CSI_BDG_DVP_BACKGROUND_COLOR_1  CSI_BACKGROUND_COLOR_1;
	struct VREG_ISP_CSI_BDG_DVP_FIX_COLOR_0  CSI_FIX_COLOR_0;
	struct VREG_ISP_CSI_BDG_DVP_FIX_COLOR_1  CSI_FIX_COLOR_1;
	struct VREG_ISP_CSI_BDG_DVP_MDE_V_SIZE   CSI_MDE_V_SIZE;
	struct VREG_ISP_CSI_BDG_DVP_MDE_H_SIZE   CSI_MDE_H_SIZE;
	struct VREG_ISP_CSI_BDG_DVP_FDE_V_SIZE   CSI_FDE_V_SIZE;
	struct VREG_ISP_CSI_BDG_DVP_FDE_H_SIZE   CSI_FDE_H_SIZE;
	struct VREG_ISP_CSI_BDG_DVP_HSYNC_CTRL   CSI_HSYNC_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_VSYNC_CTRL   CSI_VSYNC_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_TGEN_TT_SIZE  CSI_TGEN_TT_SIZE;
	struct VREG_ISP_CSI_BDG_DVP_LINE_INTP_HEIGHT_0  LINE_INTP_HEIGHT_0;
	struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_0  CH0_DEBUG_0;
	struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_1  CH0_DEBUG_1;
	struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_2  CH0_DEBUG_2;
	struct VREG_ISP_CSI_BDG_DVP_CH0_DEBUG_3  CH0_DEBUG_3;
	struct VREG_RESV                        _resv_0xb0[12];
	struct VREG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	struct VREG_ISP_CSI_BDG_DVP_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	struct VREG_ISP_CSI_BDG_DVP_BDG_DEBUG    BDG_DEBUG;
	struct VREG_ISP_CSI_BDG_DVP_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	struct VREG_ISP_CSI_BDG_DVP_WR_URGENT_CTRL  CSI_WR_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_RD_URGENT_CTRL  CSI_RD_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_DVP_DUMMY        CSI_DUMMY;
	struct VREG_RESV                        _resv_0xfc[1];
	struct VREG_ISP_CSI_BDG_DVP_SLICE_LINE_INTP_HEIGHT_0  SLICE_LINE_INTP_HEIGHT_0;
	struct VREG_RESV                        _resv_0x104[3];
	struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_CROP_EN  WDMA_CH0_CROP_EN;
	struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_HORZ_CROP  WDMA_CH0_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_DVP_WDMA_CH0_VERT_CROP  WDMA_CH0_VERT_CROP;
	struct VREG_RESV                        _resv_0x11c[13];
	struct VREG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	struct VREG_ISP_CSI_BDG_DVP_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_CROP_0 {
	union REG_CROP_0                        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_1 {
	union REG_CROP_1                        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_2 {
	union REG_CROP_2                        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_3 {
	union REG_CROP_3                        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_DUMMY {
	union REG_CROP_DUMMY                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_DEBUG {
	union REG_CROP_DEBUG                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CROP_T {
	struct VREG_CROP_0                       REG_0;
	struct VREG_CROP_1                       REG_1;
	struct VREG_CROP_2                       REG_2;
	struct VREG_CROP_3                       REG_3;
	struct VREG_CROP_DUMMY                   DUMMY;
	struct VREG_CROP_DEBUG                   DEBUG;
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

struct VREG_ISP_BLC_DUMMY {
	union REG_ISP_BLC_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_LOCATION {
	union REG_ISP_BLC_LOCATION              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_9 {
	union REG_ISP_BLC_9                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BLC_A {
	union REG_ISP_BLC_A                     write;
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
	struct VREG_RESV                        _resv_0x24[1];
	struct VREG_ISP_BLC_DUMMY                BLC_DUMMY;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_BLC_LOCATION             BLC_LOCATION;
	struct VREG_ISP_BLC_9                    BLC_9;
	struct VREG_ISP_BLC_A                    BLC_A;
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

struct VREG_ISP_LMAP_LMP_DEBUG_2 {
	union REG_ISP_LMAP_LMP_DEBUG_2          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_3 {
	union REG_ISP_LMAP_LMP_3                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_4 {
	union REG_ISP_LMAP_LMP_4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_LMP_SET_SEL {
	union REG_ISP_LMAP_LMP_SET_SEL          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LMAP_T {
	struct VREG_ISP_LMAP_LMP_0               LMP_0;
	struct VREG_ISP_LMAP_LMP_1               LMP_1;
	struct VREG_ISP_LMAP_LMP_2               LMP_2;
	struct VREG_ISP_LMAP_LMP_DEBUG_0         LMP_DEBUG_0;
	struct VREG_ISP_LMAP_LMP_DEBUG_1         LMP_DEBUG_1;
	struct VREG_ISP_LMAP_DUMMY               DUMMY;
	struct VREG_ISP_LMAP_LMP_DEBUG_2         LMP_DEBUG_2;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_LMAP_LMP_3               LMP_3;
	struct VREG_ISP_LMAP_LMP_4               LMP_4;
	struct VREG_ISP_LMAP_LMP_SET_SEL         LMP_SET_SEL;
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

struct VREG_ISP_RGBMAP_DEBUG_2 {
	union REG_ISP_RGBMAP_DEBUG_2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_3 {
	union REG_ISP_RGBMAP_3                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_SET_SEL {
	union REG_ISP_RGBMAP_SET_SEL            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBMAP_T {
	struct VREG_ISP_RGBMAP_0                 RGBMAP_0;
	struct VREG_ISP_RGBMAP_1                 RGBMAP_1;
	struct VREG_ISP_RGBMAP_DEBUG_0           RGBMAP_DEBUG_0;
	struct VREG_ISP_RGBMAP_DEBUG_1           RGBMAP_DEBUG_1;
	struct VREG_ISP_RGBMAP_DUMMY             DUMMY;
	struct VREG_ISP_RGBMAP_2                 RGBMAP_2;
	struct VREG_ISP_RGBMAP_DEBUG_2           RGBMAP_DEBUG_2;
	struct VREG_ISP_RGBMAP_3                 RGBMAP_3;
	struct VREG_ISP_RGBMAP_SET_SEL           RGBMAP_SET_SEL;
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

struct VREG_ISP_WBG_34 {
	union REG_ISP_WBG_34                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_38 {
	union REG_ISP_WBG_38                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_WBG_3C {
	union REG_ISP_WBG_3C                    write;
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
	struct VREG_ISP_WBG_34                   WBG_34;
	struct VREG_ISP_WBG_38                   WBG_38;
	struct VREG_ISP_WBG_3C                   WBG_3C;
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

struct VREG_ISP_DPC_24 {
	union REG_ISP_DPC_24                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DPC_25 {
	union REG_ISP_DPC_25                    write;
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
	struct VREG_ISP_DPC_24                   DPC_24;
	struct VREG_ISP_DPC_25                   DPC_25;
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

struct VREG_RAW_TOP_UP_PQ_EN {
	union REG_RAW_TOP_UP_PQ_EN              write;
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

struct VREG_RAW_TOP_RAW_BAYER_TYPE_TOPLEFT {
	union REG_RAW_TOP_RAW_BAYER_TYPE_TOPLEFT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RDMI_ENABLE {
	union REG_RAW_TOP_RDMI_ENABLE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RDMA_SIZE {
	union REG_RAW_TOP_RDMA_SIZE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_DPCM_MODE {
	union REG_RAW_TOP_DPCM_MODE             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_STVALID_STATUS {
	union REG_RAW_TOP_STVALID_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_STREADY_STATUS {
	union REG_RAW_TOP_STREADY_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_PATGEN1 {
	union REG_RAW_TOP_PATGEN1               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_PATGEN2 {
	union REG_RAW_TOP_PATGEN2               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_PATGEN3 {
	union REG_RAW_TOP_PATGEN3               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_PATGEN4 {
	union REG_RAW_TOP_PATGEN4               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RO_IDLE {
	union REG_RAW_TOP_RO_IDLE               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_RO_DONE {
	union REG_RAW_TOP_RO_DONE               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_DMA_IDLE {
	union REG_RAW_TOP_DMA_IDLE              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_LE_LMAP_GRID_NUMBER {
	union REG_RAW_TOP_LE_LMAP_GRID_NUMBER   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_SE_LMAP_GRID_NUMBER {
	union REG_RAW_TOP_SE_LMAP_GRID_NUMBER   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_CHECKSUM_0 {
	union REG_RAW_TOP_CHECKSUM_0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_CHECKSUM_1 {
	union REG_RAW_TOP_CHECKSUM_1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_TOP_T {
	struct VREG_RAW_TOP_RAW_0                RAW_0;
	struct VREG_RAW_TOP_READ_SEL             READ_SEL;
	struct VREG_RAW_TOP_RAW_1                RAW_1;
	struct VREG_RESV                        _resv_0xc[1];
	struct VREG_RAW_TOP_CTRL                 CTRL;
	struct VREG_RAW_TOP_UP_PQ_EN             UP_PQ_EN;
	struct VREG_RAW_TOP_RAW_2                RAW_2;
	struct VREG_RAW_TOP_DUMMY                DUMMY;
	struct VREG_RAW_TOP_RAW_4                RAW_4;
	struct VREG_RAW_TOP_STATUS               STATUS;
	struct VREG_RAW_TOP_DEBUG                DEBUG;
	struct VREG_RAW_TOP_DEBUG_SELECT         DEBUG_SELECT;
	struct VREG_RAW_TOP_RAW_BAYER_TYPE_TOPLEFT  RAW_BAYER_TYPE_TOPLEFT;
	struct VREG_RAW_TOP_RDMI_ENABLE          RDMI_ENABLE;
	struct VREG_RAW_TOP_RDMA_SIZE            RDMA_SIZE;
	struct VREG_RAW_TOP_DPCM_MODE            DPCM_MODE;
	struct VREG_RAW_TOP_STVALID_STATUS       STVALID_STATUS;
	struct VREG_RAW_TOP_STREADY_STATUS       STREADY_STATUS;
	struct VREG_RAW_TOP_PATGEN1              PATGEN1;
	struct VREG_RAW_TOP_PATGEN2              PATGEN2;
	struct VREG_RAW_TOP_PATGEN3              PATGEN3;
	struct VREG_RAW_TOP_PATGEN4              PATGEN4;
	struct VREG_RAW_TOP_RO_IDLE              RO_IDLE;
	struct VREG_RAW_TOP_RO_DONE              RO_DONE;
	struct VREG_RAW_TOP_DMA_IDLE             DMA_IDLE;
	struct VREG_RESV                        _resv_0x64[1];
	struct VREG_RAW_TOP_LE_LMAP_GRID_NUMBER  LE_LMAP_GRID_NUMBER;
	struct VREG_RAW_TOP_SE_LMAP_GRID_NUMBER  SE_LMAP_GRID_NUMBER;
	struct VREG_RAW_TOP_CHECKSUM_0           CHECKSUM_0;
	struct VREG_RAW_TOP_CHECKSUM_1           CHECKSUM_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGBCAC_CTRL {
	union REG_ISP_RGBCAC_CTRL               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_PURPLE_TH {
	union REG_ISP_RGBCAC_PURPLE_TH          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_PURPLE_CBCR {
	union REG_ISP_RGBCAC_PURPLE_CBCR        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_PURPLE_CBCR2 {
	union REG_ISP_RGBCAC_PURPLE_CBCR2       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_PURPLE_CBCR3 {
	union REG_ISP_RGBCAC_PURPLE_CBCR3       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_GREEN_CBCR {
	union REG_ISP_RGBCAC_GREEN_CBCR         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_CORING {
	union REG_ISP_RGBCAC_EDGE_CORING        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN {
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX {
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_WGT_LUT0 {
	union REG_ISP_RGBCAC_EDGE_WGT_LUT0      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_WGT_LUT1 {
	union REG_ISP_RGBCAC_EDGE_WGT_LUT1      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_WGT_LUT2 {
	union REG_ISP_RGBCAC_EDGE_WGT_LUT2      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_WGT_LUT3 {
	union REG_ISP_RGBCAC_EDGE_WGT_LUT3      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_EDGE_WGT_LUT4 {
	union REG_ISP_RGBCAC_EDGE_WGT_LUT4      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_LUMA {
	union REG_ISP_RGBCAC_LUMA               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_LUMA_BLEND {
	union REG_ISP_RGBCAC_LUMA_BLEND         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_LUMA_FILTER0 {
	union REG_ISP_RGBCAC_LUMA_FILTER0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_LUMA_FILTER1 {
	union REG_ISP_RGBCAC_LUMA_FILTER1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_VAR_FILTER0 {
	union REG_ISP_RGBCAC_VAR_FILTER0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_VAR_FILTER1 {
	union REG_ISP_RGBCAC_VAR_FILTER1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_CHROMA_FILTER0 {
	union REG_ISP_RGBCAC_CHROMA_FILTER0     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_CHROMA_FILTER1 {
	union REG_ISP_RGBCAC_CHROMA_FILTER1     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_CBCR_STR {
	union REG_ISP_RGBCAC_CBCR_STR           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGBCAC_T {
	struct VREG_ISP_RGBCAC_CTRL              RGBCAC_CTRL;
	struct VREG_ISP_RGBCAC_PURPLE_TH         RGBCAC_PURPLE_TH;
	struct VREG_ISP_RGBCAC_PURPLE_CBCR       RGBCAC_PURPLE_CBCR;
	struct VREG_ISP_RGBCAC_PURPLE_CBCR2      RGBCAC_PURPLE_CBCR2;
	struct VREG_ISP_RGBCAC_PURPLE_CBCR3      RGBCAC_PURPLE_CBCR3;
	struct VREG_ISP_RGBCAC_GREEN_CBCR        RGBCAC_GREEN_CBCR;
	struct VREG_ISP_RGBCAC_EDGE_CORING       RGBCAC_EDGE_CORING;
	struct VREG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN  RGBCAC_DEPURPLE_STR_RATIO_MIN;
	struct VREG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX  RGBCAC_DEPURPLE_STR_RATIO_MAX;
	struct VREG_ISP_RGBCAC_EDGE_WGT_LUT0     RGBCAC_EDGE_WGT_LUT0;
	struct VREG_ISP_RGBCAC_EDGE_WGT_LUT1     RGBCAC_EDGE_WGT_LUT1;
	struct VREG_ISP_RGBCAC_EDGE_WGT_LUT2     RGBCAC_EDGE_WGT_LUT2;
	struct VREG_ISP_RGBCAC_EDGE_WGT_LUT3     RGBCAC_EDGE_WGT_LUT3;
	struct VREG_ISP_RGBCAC_EDGE_WGT_LUT4     RGBCAC_EDGE_WGT_LUT4;
	struct VREG_ISP_RGBCAC_LUMA              RGBCAC_LUMA;
	struct VREG_ISP_RGBCAC_LUMA_BLEND        RGBCAC_LUMA_BLEND;
	struct VREG_ISP_RGBCAC_LUMA_FILTER0      RGBCAC_LUMA_FILTER0;
	struct VREG_ISP_RGBCAC_LUMA_FILTER1      RGBCAC_LUMA_FILTER1;
	struct VREG_ISP_RGBCAC_VAR_FILTER0       RGBCAC_VAR_FILTER0;
	struct VREG_ISP_RGBCAC_VAR_FILTER1       RGBCAC_VAR_FILTER1;
	struct VREG_ISP_RGBCAC_CHROMA_FILTER0    RGBCAC_CHROMA_FILTER0;
	struct VREG_ISP_RGBCAC_CHROMA_FILTER1    RGBCAC_CHROMA_FILTER1;
	struct VREG_ISP_RGBCAC_CBCR_STR          RGBCAC_CBCR_STR;
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

struct VREG_ISP_BNR_INTENSITY_SEL {
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

struct VREG_ISP_BNR_VAR_TH          {
	union REG_ISP_BNR_VAR_TH                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_BNR_WEIGHT_LUT {
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

struct VREG_ISP_BNR_RES_MAX {
	union REG_ISP_BNR_RES_MAX               write;
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
	struct VREG_RESV                        _resv_0xc[61];
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
	struct VREG_RESV                        _resv_0x218[3];
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
	struct VREG_ISP_BNR_RES_MAX              RES_MAX;
	struct VREG_RESV                        _resv_0x25c[872];
	struct VREG_ISP_BNR_DUMMY                DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_CA_00 {
	union REG_CA_00                         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_04 {
	union REG_CA_04                         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_08 {
	union REG_CA_08                         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_0C {
	union REG_CA_0C                         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_10 {
	union REG_CA_10                         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_T {
	struct VREG_CA_00                        REG_00;
	struct VREG_CA_04                        REG_04;
	struct VREG_CA_08                        REG_08;
	struct VREG_CA_0C                        REG_0C;
	struct VREG_CA_10                        REG_10;
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
struct VREG_ISP_MMAP_00 {
	union REG_ISP_MMAP_00                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_04 {
	union REG_ISP_MMAP_04                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_08 {
	union REG_ISP_MMAP_08                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_0C {
	union REG_ISP_MMAP_0C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_10 {
	union REG_ISP_MMAP_10                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_14 {
	union REG_ISP_MMAP_14                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_18 {
	union REG_ISP_MMAP_18                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_1C {
	union REG_ISP_MMAP_1C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_20 {
	union REG_ISP_MMAP_20                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_24 {
	union REG_ISP_MMAP_24                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_28 {
	union REG_ISP_MMAP_28                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_2C {
	union REG_ISP_MMAP_2C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_30 {
	union REG_ISP_MMAP_30                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_34 {
	union REG_ISP_MMAP_34                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_38 {
	union REG_ISP_MMAP_38                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_3C {
	union REG_ISP_MMAP_3C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_40 {
	union REG_ISP_MMAP_40                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_44 {
	union REG_ISP_MMAP_44                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_4C {
	union REG_ISP_MMAP_4C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_50 {
	union REG_ISP_MMAP_50                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_54 {
	union REG_ISP_MMAP_54                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_58 {
	union REG_ISP_MMAP_58                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_5C {
	union REG_ISP_MMAP_5C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_60 {
	union REG_ISP_MMAP_60                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_64 {
	union REG_ISP_MMAP_64                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_68 {
	union REG_ISP_MMAP_68                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_6C {
	union REG_ISP_MMAP_6C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_70 {
	union REG_ISP_MMAP_70                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_74 {
	union REG_ISP_MMAP_74                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_78 {
	union REG_ISP_MMAP_78                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_7C {
	union REG_ISP_MMAP_7C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_80 {
	union REG_ISP_MMAP_80                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_84 {
	union REG_ISP_MMAP_84                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_88 {
	union REG_ISP_MMAP_88                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_8C {
	union REG_ISP_MMAP_8C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_90 {
	union REG_ISP_MMAP_90                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_A0 {
	union REG_ISP_MMAP_A0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_A4 {
	union REG_ISP_MMAP_A4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_A8 {
	union REG_ISP_MMAP_A8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_AC {
	union REG_ISP_MMAP_AC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_B0 {
	union REG_ISP_MMAP_B0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_B4 {
	union REG_ISP_MMAP_B4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_B8 {
	union REG_ISP_MMAP_B8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_BC {
	union REG_ISP_MMAP_BC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_C0 {
	union REG_ISP_MMAP_C0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_C4 {
	union REG_ISP_MMAP_C4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_C8 {
	union REG_ISP_MMAP_C8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_CC {
	union REG_ISP_MMAP_CC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_D0 {
	union REG_ISP_MMAP_D0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_D4 {
	union REG_ISP_MMAP_D4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_D8 {
	union REG_ISP_MMAP_D8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_DC {
	union REG_ISP_MMAP_DC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_E0 {
	union REG_ISP_MMAP_E0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_E4 {
	union REG_ISP_MMAP_E4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_E8 {
	union REG_ISP_MMAP_E8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_EC {
	union REG_ISP_MMAP_EC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_F0 {
	union REG_ISP_MMAP_F0                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_F4 {
	union REG_ISP_MMAP_F4                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_F8 {
	union REG_ISP_MMAP_F8                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_FC {
	union REG_ISP_MMAP_FC                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_100 {
	union REG_ISP_MMAP_100                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_104 {
	union REG_ISP_MMAP_104                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_108 {
	union REG_ISP_MMAP_108                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_10C {
	union REG_ISP_MMAP_10C                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_110 {
	union REG_ISP_MMAP_110                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_114 {
	union REG_ISP_MMAP_114                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_118 {
	union REG_ISP_MMAP_118                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_11C {
	union REG_ISP_MMAP_11C                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_120 {
	union REG_ISP_MMAP_120                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_124 {
	union REG_ISP_MMAP_124                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_128 {
	union REG_ISP_MMAP_128                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_MMAP_T {
	struct VREG_ISP_MMAP_00                  REG_00;
	struct VREG_ISP_MMAP_04                  REG_04;
	struct VREG_ISP_MMAP_08                  REG_08;
	struct VREG_ISP_MMAP_0C                  REG_0C;
	struct VREG_ISP_MMAP_10                  REG_10;
	struct VREG_ISP_MMAP_14                  REG_14;
	struct VREG_ISP_MMAP_18                  REG_18;
	struct VREG_ISP_MMAP_1C                  REG_1C;
	struct VREG_ISP_MMAP_20                  REG_20;
	struct VREG_ISP_MMAP_24                  REG_24;
	struct VREG_ISP_MMAP_28                  REG_28;
	struct VREG_ISP_MMAP_2C                  REG_2C;
	struct VREG_ISP_MMAP_30                  REG_30;
	struct VREG_ISP_MMAP_34                  REG_34;
	struct VREG_ISP_MMAP_38                  REG_38;
	struct VREG_ISP_MMAP_3C                  REG_3C;
	struct VREG_ISP_MMAP_40                  REG_40;
	struct VREG_ISP_MMAP_44                  REG_44;
	struct VREG_RESV                        _resv_0x48[1];
	struct VREG_ISP_MMAP_4C                  REG_4C;
	struct VREG_ISP_MMAP_50                  REG_50;
	struct VREG_ISP_MMAP_54                  REG_54;
	struct VREG_ISP_MMAP_58                  REG_58;
	struct VREG_ISP_MMAP_5C                  REG_5C;
	struct VREG_ISP_MMAP_60                  REG_60;
	struct VREG_ISP_MMAP_64                  REG_64;
	struct VREG_ISP_MMAP_68                  REG_68;
	struct VREG_ISP_MMAP_6C                  REG_6C;
	struct VREG_ISP_MMAP_70                  REG_70;
	struct VREG_ISP_MMAP_74                  REG_74;
	struct VREG_ISP_MMAP_78                  REG_78;
	struct VREG_ISP_MMAP_7C                  REG_7C;
	struct VREG_ISP_MMAP_80                  REG_80;
	struct VREG_ISP_MMAP_84                  REG_84;
	struct VREG_ISP_MMAP_88                  REG_88;
	struct VREG_ISP_MMAP_8C                  REG_8C;
	struct VREG_ISP_MMAP_90                  REG_90;
	struct VREG_RESV                        _resv_0x94[3];
	struct VREG_ISP_MMAP_A0                  REG_A0;
	struct VREG_ISP_MMAP_A4                  REG_A4;
	struct VREG_ISP_MMAP_A8                  REG_A8;
	struct VREG_ISP_MMAP_AC                  REG_AC;
	struct VREG_ISP_MMAP_B0                  REG_B0;
	struct VREG_ISP_MMAP_B4                  REG_B4;
	struct VREG_ISP_MMAP_B8                  REG_B8;
	struct VREG_ISP_MMAP_BC                  REG_BC;
	struct VREG_ISP_MMAP_C0                  REG_C0;
	struct VREG_ISP_MMAP_C4                  REG_C4;
	struct VREG_ISP_MMAP_C8                  REG_C8;
	struct VREG_ISP_MMAP_CC                  REG_CC;
	struct VREG_ISP_MMAP_D0                  REG_D0;
	struct VREG_ISP_MMAP_D4                  REG_D4;
	struct VREG_ISP_MMAP_D8                  REG_D8;
	struct VREG_ISP_MMAP_DC                  REG_DC;
	struct VREG_ISP_MMAP_E0                  REG_E0;
	struct VREG_ISP_MMAP_E4                  REG_E4;
	struct VREG_ISP_MMAP_E8                  REG_E8;
	struct VREG_ISP_MMAP_EC                  REG_EC;
	struct VREG_ISP_MMAP_F0                  REG_F0;
	struct VREG_ISP_MMAP_F4                  REG_F4;
	struct VREG_ISP_MMAP_F8                  REG_F8;
	struct VREG_ISP_MMAP_FC                  REG_FC;
	struct VREG_ISP_MMAP_100                 REG_100;
	struct VREG_ISP_MMAP_104                 REG_104;
	struct VREG_ISP_MMAP_108                 REG_108;
	struct VREG_ISP_MMAP_10C                 REG_10C;
	struct VREG_ISP_MMAP_110                 REG_110;
	struct VREG_ISP_MMAP_114                 REG_114;
	struct VREG_ISP_MMAP_118                 REG_118;
	struct VREG_ISP_MMAP_11C                 REG_11C;
	struct VREG_ISP_MMAP_120                 REG_120;
	struct VREG_ISP_MMAP_124                 REG_124;
	struct VREG_ISP_MMAP_128                 REG_128;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_GAMMA_CTRL {
	union REG_ISP_GAMMA_CTRL                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_CTRL {
	union REG_ISP_GAMMA_PROG_CTRL           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_ST_ADDR {
	union REG_ISP_GAMMA_PROG_ST_ADDR        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_DATA {
	union REG_ISP_GAMMA_PROG_DATA           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_GAMMA_PROG_MAX {
	union REG_ISP_GAMMA_PROG_MAX            write;
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

struct VREG_ISP_GAMMA_T {
	struct VREG_ISP_GAMMA_CTRL               GAMMA_CTRL;
	struct VREG_ISP_GAMMA_PROG_CTRL          GAMMA_PROG_CTRL;
	struct VREG_ISP_GAMMA_PROG_ST_ADDR       GAMMA_PROG_ST_ADDR;
	struct VREG_ISP_GAMMA_PROG_DATA          GAMMA_PROG_DATA;
	struct VREG_ISP_GAMMA_PROG_MAX           GAMMA_PROG_MAX;
	struct VREG_ISP_GAMMA_MEM_SW_RADDR       GAMMA_SW_RADDR;
	struct VREG_ISP_GAMMA_MEM_SW_RDATA       GAMMA_SW_RDATA;
	struct VREG_ISP_GAMMA_MEM_SW_RDATA_BG    GAMMA_SW_RDATA_BG;
	struct VREG_ISP_GAMMA_DBG                GAMMA_DBG;
	struct VREG_ISP_GAMMA_DMY0               GAMMA_DMY0;
	struct VREG_ISP_GAMMA_DMY1               GAMMA_DMY1;
	struct VREG_ISP_GAMMA_DMY_R              GAMMA_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CLUT_CTRL {
	union REG_ISP_CLUT_CTRL                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_PROG_ADDR {
	union REG_ISP_CLUT_PROG_ADDR            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_PROG_DATA {
	union REG_ISP_CLUT_PROG_DATA            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_PROG_RDATA {
	union REG_ISP_CLUT_PROG_RDATA           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_DBG {
	union REG_ISP_CLUT_DBG                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_DMY0 {
	union REG_ISP_CLUT_DMY0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_DMY1 {
	union REG_ISP_CLUT_DMY1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_DMY_R {
	union REG_ISP_CLUT_DMY_R                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CLUT_T {
	struct VREG_ISP_CLUT_CTRL                CLUT_CTRL;
	struct VREG_ISP_CLUT_PROG_ADDR           CLUT_PROG_ADDR;
	struct VREG_ISP_CLUT_PROG_DATA           CLUT_PROG_DATA;
	struct VREG_ISP_CLUT_PROG_RDATA          CLUT_PROG_RDATA;
	struct VREG_RESV                        _resv_0x10[4];
	struct VREG_ISP_CLUT_DBG                 CLUT_DBG;
	struct VREG_ISP_CLUT_DMY0                CLUT_DMY0;
	struct VREG_ISP_CLUT_DMY1                CLUT_DMY1;
	struct VREG_ISP_CLUT_DMY_R               CLUT_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_DEHAZE_DHZ_SMOOTH {
	union REG_ISP_DEHAZE_DHZ_SMOOTH         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_DHZ_SKIN {
	union REG_ISP_DEHAZE_DHZ_SKIN           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_DHZ_WGT {
	union REG_ISP_DEHAZE_DHZ_WGT            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_DHZ_BYPASS {
	union REG_ISP_DEHAZE_DHZ_BYPASS         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_0 {
	union REG_ISP_DEHAZE_0                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_1 {
	union REG_ISP_DEHAZE_1                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_2 {
	union REG_ISP_DEHAZE_2                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_28 {
	union REG_ISP_DEHAZE_28                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_2C {
	union REG_ISP_DEHAZE_2C                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_3 {
	union REG_ISP_DEHAZE_3                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_5 {
	union REG_ISP_DEHAZE_5                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_6 {
	union REG_ISP_DEHAZE_6                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_7 {
	union REG_ISP_DEHAZE_7                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_8 {
	union REG_ISP_DEHAZE_8                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_9 {
	union REG_ISP_DEHAZE_9                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_10 {
	union REG_ISP_DEHAZE_10                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_11 {
	union REG_ISP_DEHAZE_11                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_12 {
	union REG_ISP_DEHAZE_12                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_17 {
	union REG_ISP_DEHAZE_17                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_18 {
	union REG_ISP_DEHAZE_18                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_19 {
	union REG_ISP_DEHAZE_19                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_20 {
	union REG_ISP_DEHAZE_20                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_25 {
	union REG_ISP_DEHAZE_25                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_26 {
	union REG_ISP_DEHAZE_26                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_00 {
	union REG_ISP_DEHAZE_TMAP_00            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_01 {
	union REG_ISP_DEHAZE_TMAP_01            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_02 {
	union REG_ISP_DEHAZE_TMAP_02            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_03 {
	union REG_ISP_DEHAZE_TMAP_03            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_04 {
	union REG_ISP_DEHAZE_TMAP_04            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_05 {
	union REG_ISP_DEHAZE_TMAP_05            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_06 {
	union REG_ISP_DEHAZE_TMAP_06            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_07 {
	union REG_ISP_DEHAZE_TMAP_07            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_08 {
	union REG_ISP_DEHAZE_TMAP_08            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_09 {
	union REG_ISP_DEHAZE_TMAP_09            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_10 {
	union REG_ISP_DEHAZE_TMAP_10            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_11 {
	union REG_ISP_DEHAZE_TMAP_11            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_12 {
	union REG_ISP_DEHAZE_TMAP_12            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_13 {
	union REG_ISP_DEHAZE_TMAP_13            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_14 {
	union REG_ISP_DEHAZE_TMAP_14            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_15 {
	union REG_ISP_DEHAZE_TMAP_15            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_16 {
	union REG_ISP_DEHAZE_TMAP_16            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_17 {
	union REG_ISP_DEHAZE_TMAP_17            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_18 {
	union REG_ISP_DEHAZE_TMAP_18            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_19 {
	union REG_ISP_DEHAZE_TMAP_19            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_20 {
	union REG_ISP_DEHAZE_TMAP_20            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_21 {
	union REG_ISP_DEHAZE_TMAP_21            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_22 {
	union REG_ISP_DEHAZE_TMAP_22            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_23 {
	union REG_ISP_DEHAZE_TMAP_23            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_24 {
	union REG_ISP_DEHAZE_TMAP_24            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_25 {
	union REG_ISP_DEHAZE_TMAP_25            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_26 {
	union REG_ISP_DEHAZE_TMAP_26            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_27 {
	union REG_ISP_DEHAZE_TMAP_27            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_28 {
	union REG_ISP_DEHAZE_TMAP_28            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_29 {
	union REG_ISP_DEHAZE_TMAP_29            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_30 {
	union REG_ISP_DEHAZE_TMAP_30            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_31 {
	union REG_ISP_DEHAZE_TMAP_31            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_TMAP_32 {
	union REG_ISP_DEHAZE_TMAP_32            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DEHAZE_T {
	struct VREG_ISP_DEHAZE_DHZ_SMOOTH        DHZ_SMOOTH;
	struct VREG_ISP_DEHAZE_DHZ_SKIN          DHZ_SKIN;
	struct VREG_ISP_DEHAZE_DHZ_WGT           DHZ_WGT;
	struct VREG_RESV                        _resv_0xc[2];
	struct VREG_ISP_DEHAZE_DHZ_BYPASS        DHZ_BYPASS;
	struct VREG_ISP_DEHAZE_0                 REG_0;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_DEHAZE_1                 REG_1;
	struct VREG_ISP_DEHAZE_2                 REG_2;
	struct VREG_ISP_DEHAZE_28                REG_28;
	struct VREG_ISP_DEHAZE_2C                REG_2C;
	struct VREG_ISP_DEHAZE_3                 REG_3;
	struct VREG_ISP_DEHAZE_5                 REG_5;
	struct VREG_ISP_DEHAZE_6                 REG_6;
	struct VREG_ISP_DEHAZE_7                 REG_7;
	struct VREG_ISP_DEHAZE_8                 REG_8;
	struct VREG_RESV                        _resv_0x44[3];
	struct VREG_ISP_DEHAZE_9                 REG_9;
	struct VREG_ISP_DEHAZE_10                REG_10;
	struct VREG_ISP_DEHAZE_11                REG_11;
	struct VREG_ISP_DEHAZE_12                REG_12;
	struct VREG_ISP_DEHAZE_17                REG_17;
	struct VREG_ISP_DEHAZE_18                REG_18;
	struct VREG_ISP_DEHAZE_19                REG_19;
	struct VREG_ISP_DEHAZE_20                REG_20;
	struct VREG_ISP_DEHAZE_25                REG_25;
	struct VREG_ISP_DEHAZE_26                REG_26;
	struct VREG_ISP_DEHAZE_TMAP_00           TMAP_00;
	struct VREG_ISP_DEHAZE_TMAP_01           TMAP_01;
	struct VREG_ISP_DEHAZE_TMAP_02           TMAP_02;
	struct VREG_ISP_DEHAZE_TMAP_03           TMAP_03;
	struct VREG_ISP_DEHAZE_TMAP_04           TMAP_04;
	struct VREG_ISP_DEHAZE_TMAP_05           TMAP_05;
	struct VREG_ISP_DEHAZE_TMAP_06           TMAP_06;
	struct VREG_ISP_DEHAZE_TMAP_07           TMAP_07;
	struct VREG_ISP_DEHAZE_TMAP_08           TMAP_08;
	struct VREG_ISP_DEHAZE_TMAP_09           TMAP_09;
	struct VREG_ISP_DEHAZE_TMAP_10           TMAP_10;
	struct VREG_ISP_DEHAZE_TMAP_11           TMAP_11;
	struct VREG_ISP_DEHAZE_TMAP_12           TMAP_12;
	struct VREG_ISP_DEHAZE_TMAP_13           TMAP_13;
	struct VREG_ISP_DEHAZE_TMAP_14           TMAP_14;
	struct VREG_ISP_DEHAZE_TMAP_15           TMAP_15;
	struct VREG_ISP_DEHAZE_TMAP_16           TMAP_16;
	struct VREG_ISP_DEHAZE_TMAP_17           TMAP_17;
	struct VREG_ISP_DEHAZE_TMAP_18           TMAP_18;
	struct VREG_ISP_DEHAZE_TMAP_19           TMAP_19;
	struct VREG_ISP_DEHAZE_TMAP_20           TMAP_20;
	struct VREG_ISP_DEHAZE_TMAP_21           TMAP_21;
	struct VREG_ISP_DEHAZE_TMAP_22           TMAP_22;
	struct VREG_ISP_DEHAZE_TMAP_23           TMAP_23;
	struct VREG_ISP_DEHAZE_TMAP_24           TMAP_24;
	struct VREG_ISP_DEHAZE_TMAP_25           TMAP_25;
	struct VREG_ISP_DEHAZE_TMAP_26           TMAP_26;
	struct VREG_ISP_DEHAZE_TMAP_27           TMAP_27;
	struct VREG_ISP_DEHAZE_TMAP_28           TMAP_28;
	struct VREG_ISP_DEHAZE_TMAP_29           TMAP_29;
	struct VREG_ISP_DEHAZE_TMAP_30           TMAP_30;
	struct VREG_ISP_DEHAZE_TMAP_31           TMAP_31;
	struct VREG_ISP_DEHAZE_TMAP_32           TMAP_32;
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

struct VREG_ISP_CSC_4 {
	union REG_ISP_CSC_4                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_5 {
	union REG_ISP_CSC_5                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_6 {
	union REG_ISP_CSC_6                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_7 {
	union REG_ISP_CSC_7                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_8 {
	union REG_ISP_CSC_8                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_9 {
	union REG_ISP_CSC_9                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSC_T {
	struct VREG_ISP_CSC_0                    REG_0;
	struct VREG_ISP_CSC_1                    REG_1;
	struct VREG_ISP_CSC_2                    REG_2;
	struct VREG_ISP_CSC_3                    REG_3;
	struct VREG_ISP_CSC_4                    REG_4;
	struct VREG_ISP_CSC_5                    REG_5;
	struct VREG_ISP_CSC_6                    REG_6;
	struct VREG_ISP_CSC_7                    REG_7;
	struct VREG_ISP_CSC_8                    REG_8;
	struct VREG_ISP_CSC_9                    REG_9;
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

struct VREG_ISP_DCI_UV_GAIN_MAX {
	union REG_ISP_DCI_UV_GAIN_MAX           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_MAP_DBG {
	union REG_ISP_DCI_MAP_DBG               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_BAYER_STARTING {
	union REG_ISP_DCI_BAYER_STARTING        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_DMI_ENABLE {
	union REG_ISP_DCI_DMI_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_GAMMA_PROG_CTRL {
	union REG_ISP_DCI_GAMMA_PROG_CTRL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_GAMMA_PROG_DATA {
	union REG_ISP_DCI_GAMMA_PROG_DATA       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_DCI_GAMMA_PROG_MAX {
	union REG_ISP_DCI_GAMMA_PROG_MAX        write;
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
	struct VREG_RESV                        _resv_0x5c[1];
	struct VREG_ISP_DCI_UV_GAIN_MAX          DCI_UV_GAIN_MAX;
	struct VREG_RESV                        _resv_0x64[7];
	struct VREG_ISP_DCI_MAP_DBG              DCI_MAP_DBG;
	struct VREG_RESV                        _resv_0x84[1];
	struct VREG_ISP_DCI_BAYER_STARTING       DCI_BAYER_STARTING;
	struct VREG_RESV                        _resv_0x8c[5];
	struct VREG_ISP_DCI_DMI_ENABLE           DMI_ENABLE;
	struct VREG_RESV                        _resv_0xa4[88];
	struct VREG_ISP_DCI_GAMMA_PROG_CTRL      GAMMA_PROG_CTRL;
	struct VREG_RESV                        _resv_0x208[1];
	struct VREG_ISP_DCI_GAMMA_PROG_DATA      GAMMA_PROG_DATA;
	struct VREG_ISP_DCI_GAMMA_PROG_MAX       GAMMA_PROG_MAX;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_HIST_EDGE_V_STATUS {
	union REG_ISP_HIST_EDGE_V_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_SW_CTL {
	union REG_ISP_HIST_EDGE_V_SW_CTL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_BYPASS {
	union REG_ISP_HIST_EDGE_V_BYPASS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_IP_CONFIG {
	union REG_ISP_HIST_EDGE_V_IP_CONFIG     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETX {
	union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETX  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETY {
	union REG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETY  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_MONITOR {
	union REG_ISP_HIST_EDGE_V_MONITOR       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_MONITOR_SELECT {
	union REG_ISP_HIST_EDGE_V_MONITOR_SELECT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_LOCATION {
	union REG_ISP_HIST_EDGE_V_LOCATION      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_DUMMY {
	union REG_ISP_HIST_EDGE_V_DUMMY         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_DMI_ENABLE {
	union REG_ISP_HIST_EDGE_V_DMI_ENABLE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_HIST_EDGE_V_T {
	struct VREG_ISP_HIST_EDGE_V_STATUS       STATUS;
	struct VREG_ISP_HIST_EDGE_V_SW_CTL       SW_CTL;
	struct VREG_ISP_HIST_EDGE_V_BYPASS       BYPASS;
	struct VREG_ISP_HIST_EDGE_V_IP_CONFIG    IP_CONFIG;
	struct VREG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETX  HIST_EDGE_V_OFFSETX;
	struct VREG_ISP_HIST_EDGE_V_HIST_EDGE_V_OFFSETY  HIST_EDGE_V_OFFSETY;
	struct VREG_ISP_HIST_EDGE_V_MONITOR      MONITOR;
	struct VREG_ISP_HIST_EDGE_V_MONITOR_SELECT  MONITOR_SELECT;
	struct VREG_ISP_HIST_EDGE_V_LOCATION     LOCATION;
	struct VREG_ISP_HIST_EDGE_V_DUMMY        DUMMY;
	struct VREG_ISP_HIST_EDGE_V_DMI_ENABLE   DMI_ENABLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_FUSION_FS_CTRL_0 {
	union REG_FUSION_FS_CTRL_0              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_SE_GAIN {
	union REG_FUSION_FS_SE_GAIN             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LUMA_THD {
	union REG_FUSION_FS_LUMA_THD            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_WGT {
	union REG_FUSION_FS_WGT                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_WGT_SLOPE {
	union REG_FUSION_FS_WGT_SLOPE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_SHDW_READ_SEL {
	union REG_FUSION_FS_SHDW_READ_SEL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_MOTION_LUT_IN {
	union REG_FUSION_FS_MOTION_LUT_IN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_MOTION_LUT_OUT_0 {
	union REG_FUSION_FS_MOTION_LUT_OUT_0    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_MOTION_LUT_OUT_1 {
	union REG_FUSION_FS_MOTION_LUT_OUT_1    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_MOTION_LUT_SLOPE_0 {
	union REG_FUSION_FS_MOTION_LUT_SLOPE_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_MOTION_LUT_SLOPE_1 {
	union REG_FUSION_FS_MOTION_LUT_SLOPE_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CTRL_1 {
	union REG_FUSION_FS_CTRL_1              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_CTRL_0 {
	union REG_FUSION_FS_CALIB_CTRL_0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_CTRL_1 {
	union REG_FUSION_FS_CALIB_CTRL_1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_SE_FIX_OFFSET_0 {
	union REG_FUSION_FS_SE_FIX_OFFSET_0     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_SE_FIX_OFFSET_1 {
	union REG_FUSION_FS_SE_FIX_OFFSET_1     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_SE_FIX_OFFSET_2 {
	union REG_FUSION_FS_SE_FIX_OFFSET_2     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_OUT_0 {
	union REG_FUSION_FS_CALIB_OUT_0         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_OUT_1 {
	union REG_FUSION_FS_CALIB_OUT_1         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_OUT_2 {
	union REG_FUSION_FS_CALIB_OUT_2         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_CALIB_OUT_3 {
	union REG_FUSION_FS_CALIB_OUT_3         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_DARK_THD {
	union REG_FUSION_FS_LMAP_DARK_THD       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_DARK_WGT {
	union REG_FUSION_FS_LMAP_DARK_WGT       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_DARK_WGT_SLOPE {
	union REG_FUSION_FS_LMAP_DARK_WGT_SLOPE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_BRIT_THD {
	union REG_FUSION_FS_LMAP_BRIT_THD       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_BRIT_WGT {
	union REG_FUSION_FS_LMAP_BRIT_WGT       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_FS_LMAP_BRIT_WGT_SLOPE {
	union REG_FUSION_FS_LMAP_BRIT_WGT_SLOPE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FUSION_T {
	struct VREG_FUSION_FS_CTRL_0             FS_CTRL_0;
	struct VREG_RESV                        _resv_0x4[1];
	struct VREG_FUSION_FS_SE_GAIN            FS_SE_GAIN;
	struct VREG_FUSION_FS_LUMA_THD           FS_LUMA_THD;
	struct VREG_FUSION_FS_WGT                FS_WGT;
	struct VREG_FUSION_FS_WGT_SLOPE          FS_WGT_SLOPE;
	struct VREG_FUSION_FS_SHDW_READ_SEL      FS_SHDW_READ_SEL;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_FUSION_FS_MOTION_LUT_IN      FS_MOTION_LUT_IN;
	struct VREG_FUSION_FS_MOTION_LUT_OUT_0   FS_MOTION_LUT_OUT_0;
	struct VREG_FUSION_FS_MOTION_LUT_OUT_1   FS_MOTION_LUT_OUT_1;
	struct VREG_FUSION_FS_MOTION_LUT_SLOPE_0  FS_MOTION_LUT_SLOPE_0;
	struct VREG_FUSION_FS_MOTION_LUT_SLOPE_1  FS_MOTION_LUT_SLOPE_1;
	struct VREG_FUSION_FS_CTRL_1             FS_CTRL_1;
	struct VREG_RESV                        _resv_0x38[6];
	struct VREG_FUSION_FS_CALIB_CTRL_0       FS_CALIB_CTRL_0;
	struct VREG_FUSION_FS_CALIB_CTRL_1       FS_CALIB_CTRL_1;
	struct VREG_FUSION_FS_SE_FIX_OFFSET_0    FS_SE_FIX_OFFSET_0;
	struct VREG_FUSION_FS_SE_FIX_OFFSET_1    FS_SE_FIX_OFFSET_1;
	struct VREG_FUSION_FS_SE_FIX_OFFSET_2    FS_SE_FIX_OFFSET_2;
	struct VREG_FUSION_FS_CALIB_OUT_0        FS_CALIB_OUT_0;
	struct VREG_FUSION_FS_CALIB_OUT_1        FS_CALIB_OUT_1;
	struct VREG_FUSION_FS_CALIB_OUT_2        FS_CALIB_OUT_2;
	struct VREG_FUSION_FS_CALIB_OUT_3        FS_CALIB_OUT_3;
	struct VREG_FUSION_FS_LMAP_DARK_THD      FS_LMAP_DARK_THD;
	struct VREG_FUSION_FS_LMAP_DARK_WGT      FS_LMAP_DARK_WGT;
	struct VREG_FUSION_FS_LMAP_DARK_WGT_SLOPE  FS_LMAP_DARK_WGT_SLOPE;
	struct VREG_FUSION_FS_LMAP_BRIT_THD      FS_LMAP_BRIT_THD;
	struct VREG_FUSION_FS_LMAP_BRIT_WGT      FS_LMAP_BRIT_WGT;
	struct VREG_FUSION_FS_LMAP_BRIT_WGT_SLOPE  FS_LMAP_BRIT_WGT_SLOPE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_LTM_H00 {
	union REG_LTM_H00                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H04 {
	union REG_LTM_H04                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H08 {
	union REG_LTM_H08                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H0C {
	union REG_LTM_H0C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H14 {
	union REG_LTM_H14                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H18 {
	union REG_LTM_H18                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H1C {
	union REG_LTM_H1C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H20 {
	union REG_LTM_H20                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H24 {
	union REG_LTM_H24                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H28 {
	union REG_LTM_H28                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H2C {
	union REG_LTM_H2C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H30 {
	union REG_LTM_H30                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H34 {
	union REG_LTM_H34                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H38 {
	union REG_LTM_H38                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H3C {
	union REG_LTM_H3C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H40 {
	union REG_LTM_H40                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H44 {
	union REG_LTM_H44                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H48 {
	union REG_LTM_H48                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H4C {
	union REG_LTM_H4C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H50 {
	union REG_LTM_H50                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H54 {
	union REG_LTM_H54                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H58 {
	union REG_LTM_H58                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H5C {
	union REG_LTM_H5C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H60 {
	union REG_LTM_H60                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H64 {
	union REG_LTM_H64                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H68 {
	union REG_LTM_H68                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H6C {
	union REG_LTM_H6C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H70 {
	union REG_LTM_H70                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H74 {
	union REG_LTM_H74                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H78 {
	union REG_LTM_H78                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H7C {
	union REG_LTM_H7C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H80 {
	union REG_LTM_H80                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H84 {
	union REG_LTM_H84                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H88 {
	union REG_LTM_H88                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H8C {
	union REG_LTM_H8C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H90 {
	union REG_LTM_H90                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H94 {
	union REG_LTM_H94                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H98 {
	union REG_LTM_H98                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_H9C {
	union REG_LTM_H9C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HA0 {
	union REG_LTM_HA0                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HA4 {
	union REG_LTM_HA4                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HA8 {
	union REG_LTM_HA8                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HAC {
	union REG_LTM_HAC                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HB0 {
	union REG_LTM_HB0                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HB4 {
	union REG_LTM_HB4                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HB8 {
	union REG_LTM_HB8                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HBC {
	union REG_LTM_HBC                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HC0 {
	union REG_LTM_HC0                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HC4 {
	union REG_LTM_HC4                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HC8 {
	union REG_LTM_HC8                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HCC {
	union REG_LTM_HCC                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HD0 {
	union REG_LTM_HD0                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HD4 {
	union REG_LTM_HD4                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HD8 {
	union REG_LTM_HD8                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_HDC {
	union REG_LTM_HDC                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_LTM_T {
	struct VREG_LTM_H00                      REG_H00;
	struct VREG_LTM_H04                      REG_H04;
	struct VREG_LTM_H08                      REG_H08;
	struct VREG_LTM_H0C                      REG_H0C;
	struct VREG_RESV                        _resv_0x10[1];
	struct VREG_LTM_H14                      REG_H14;
	struct VREG_LTM_H18                      REG_H18;
	struct VREG_LTM_H1C                      REG_H1C;
	struct VREG_LTM_H20                      REG_H20;
	struct VREG_LTM_H24                      REG_H24;
	struct VREG_LTM_H28                      REG_H28;
	struct VREG_LTM_H2C                      REG_H2C;
	struct VREG_LTM_H30                      REG_H30;
	struct VREG_LTM_H34                      REG_H34;
	struct VREG_LTM_H38                      REG_H38;
	struct VREG_LTM_H3C                      REG_H3C;
	struct VREG_LTM_H40                      REG_H40;
	struct VREG_LTM_H44                      REG_H44;
	struct VREG_LTM_H48                      REG_H48;
	struct VREG_LTM_H4C                      REG_H4C;
	struct VREG_LTM_H50                      REG_H50;
	struct VREG_LTM_H54                      REG_H54;
	struct VREG_LTM_H58                      REG_H58;
	struct VREG_LTM_H5C                      REG_H5C;
	struct VREG_LTM_H60                      REG_H60;
	struct VREG_LTM_H64                      REG_H64;
	struct VREG_LTM_H68                      REG_H68;
	struct VREG_LTM_H6C                      REG_H6C;
	struct VREG_LTM_H70                      REG_H70;
	struct VREG_LTM_H74                      REG_H74;
	struct VREG_LTM_H78                      REG_H78;
	struct VREG_LTM_H7C                      REG_H7C;
	struct VREG_LTM_H80                      REG_H80;
	struct VREG_LTM_H84                      REG_H84;
	struct VREG_LTM_H88                      REG_H88;
	struct VREG_LTM_H8C                      REG_H8C;
	struct VREG_LTM_H90                      REG_H90;
	struct VREG_LTM_H94                      REG_H94;
	struct VREG_LTM_H98                      REG_H98;
	struct VREG_LTM_H9C                      REG_H9C;
	struct VREG_LTM_HA0                      REG_HA0;
	struct VREG_LTM_HA4                      REG_HA4;
	struct VREG_LTM_HA8                      REG_HA8;
	struct VREG_LTM_HAC                      REG_HAC;
	struct VREG_LTM_HB0                      REG_HB0;
	struct VREG_LTM_HB4                      REG_HB4;
	struct VREG_LTM_HB8                      REG_HB8;
	struct VREG_LTM_HBC                      REG_HBC;
	struct VREG_LTM_HC0                      REG_HC0;
	struct VREG_LTM_HC4                      REG_HC4;
	struct VREG_LTM_HC8                      REG_HC8;
	struct VREG_LTM_HCC                      REG_HCC;
	struct VREG_LTM_HD0                      REG_HD0;
	struct VREG_LTM_HD4                      REG_HD4;
	struct VREG_LTM_HD8                      REG_HD8;
	struct VREG_LTM_HDC                      REG_HDC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_CA_LITE_00 {
	union REG_CA_LITE_00                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_04 {
	union REG_CA_LITE_04                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_08 {
	union REG_CA_LITE_08                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_0C {
	union REG_CA_LITE_0C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_10 {
	union REG_CA_LITE_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_14 {
	union REG_CA_LITE_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_18 {
	union REG_CA_LITE_18                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_1C {
	union REG_CA_LITE_1C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_20 {
	union REG_CA_LITE_20                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_24 {
	union REG_CA_LITE_24                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_CA_LITE_T {
	struct VREG_CA_LITE_00                   REG_00;
	struct VREG_CA_LITE_04                   REG_04;
	struct VREG_CA_LITE_08                   REG_08;
	struct VREG_CA_LITE_0C                   REG_0C;
	struct VREG_CA_LITE_10                   REG_10;
	struct VREG_CA_LITE_14                   REG_14;
	struct VREG_CA_LITE_18                   REG_18;
	struct VREG_CA_LITE_1C                   REG_1C;
	struct VREG_CA_LITE_20                   REG_20;
	struct VREG_CA_LITE_24                   REG_24;
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

struct VREG_ISP_444_422_17 {
	union REG_ISP_444_422_17                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_18 {
	union REG_ISP_444_422_18                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_19 {
	union REG_ISP_444_422_19                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_20 {
	union REG_ISP_444_422_20                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_21 {
	union REG_ISP_444_422_21                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_22 {
	union REG_ISP_444_422_22                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_23 {
	union REG_ISP_444_422_23                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_24 {
	union REG_ISP_444_422_24                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_25 {
	union REG_ISP_444_422_25                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_26 {
	union REG_ISP_444_422_26                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_27 {
	union REG_ISP_444_422_27                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_28 {
	union REG_ISP_444_422_28                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_29 {
	union REG_ISP_444_422_29                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_30 {
	union REG_ISP_444_422_30                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_31 {
	union REG_ISP_444_422_31                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_80 {
	union REG_ISP_444_422_80                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_84 {
	union REG_ISP_444_422_84                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_88 {
	union REG_ISP_444_422_88                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_8C {
	union REG_ISP_444_422_8C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_90 {
	union REG_ISP_444_422_90                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_94 {
	union REG_ISP_444_422_94                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_98 {
	union REG_ISP_444_422_98                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_9C {
	union REG_ISP_444_422_9C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_A0 {
	union REG_ISP_444_422_A0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_A4 {
	union REG_ISP_444_422_A4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_A8 {
	union REG_ISP_444_422_A8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_AC {
	union REG_ISP_444_422_AC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_B0 {
	union REG_ISP_444_422_B0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_B4 {
	union REG_ISP_444_422_B4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_B8 {
	union REG_ISP_444_422_B8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_BC {
	union REG_ISP_444_422_BC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_C0 {
	union REG_ISP_444_422_C0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_C4 {
	union REG_ISP_444_422_C4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_C8 {
	union REG_ISP_444_422_C8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_CC {
	union REG_ISP_444_422_CC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_D0 {
	union REG_ISP_444_422_D0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_D4 {
	union REG_ISP_444_422_D4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_D8 {
	union REG_ISP_444_422_D8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_DC {
	union REG_ISP_444_422_DC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_E0 {
	union REG_ISP_444_422_E0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_E4 {
	union REG_ISP_444_422_E4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_E8 {
	union REG_ISP_444_422_E8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_EC {
	union REG_ISP_444_422_EC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_444_422_F0 {
	union REG_ISP_444_422_F0                write;
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
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_444_422_8                REG_8;
	struct VREG_ISP_444_422_9                REG_9;
	struct VREG_ISP_444_422_10               REG_10;
	struct VREG_ISP_444_422_11               REG_11;
	struct VREG_ISP_444_422_12               REG_12;
	struct VREG_ISP_444_422_13               REG_13;
	struct VREG_ISP_444_422_14               REG_14;
	struct VREG_ISP_444_422_15               REG_15;
	struct VREG_ISP_444_422_16               REG_16;
	struct VREG_ISP_444_422_17               REG_17;
	struct VREG_ISP_444_422_18               REG_18;
	struct VREG_ISP_444_422_19               REG_19;
	struct VREG_ISP_444_422_20               REG_20;
	struct VREG_ISP_444_422_21               REG_21;
	struct VREG_ISP_444_422_22               REG_22;
	struct VREG_ISP_444_422_23               REG_23;
	struct VREG_ISP_444_422_24               REG_24;
	struct VREG_ISP_444_422_25               REG_25;
	struct VREG_ISP_444_422_26               REG_26;
	struct VREG_ISP_444_422_27               REG_27;
	struct VREG_ISP_444_422_28               REG_28;
	struct VREG_ISP_444_422_29               REG_29;
	struct VREG_ISP_444_422_30               REG_30;
	struct VREG_ISP_444_422_31               REG_31;
	struct VREG_ISP_444_422_80               REG_80;
	struct VREG_ISP_444_422_84               REG_84;
	struct VREG_ISP_444_422_88               REG_88;
	struct VREG_ISP_444_422_8C               REG_8C;
	struct VREG_ISP_444_422_90               REG_90;
	struct VREG_ISP_444_422_94               REG_94;
	struct VREG_ISP_444_422_98               REG_98;
	struct VREG_ISP_444_422_9C               REG_9C;
	struct VREG_ISP_444_422_A0               REG_A0;
	struct VREG_ISP_444_422_A4               REG_A4;
	struct VREG_ISP_444_422_A8               REG_A8;
	struct VREG_ISP_444_422_AC               REG_AC;
	struct VREG_ISP_444_422_B0               REG_B0;
	struct VREG_ISP_444_422_B4               REG_B4;
	struct VREG_ISP_444_422_B8               REG_B8;
	struct VREG_ISP_444_422_BC               REG_BC;
	struct VREG_ISP_444_422_C0               REG_C0;
	struct VREG_ISP_444_422_C4               REG_C4;
	struct VREG_ISP_444_422_C8               REG_C8;
	struct VREG_ISP_444_422_CC               REG_CC;
	struct VREG_ISP_444_422_D0               REG_D0;
	struct VREG_ISP_444_422_D4               REG_D4;
	struct VREG_ISP_444_422_D8               REG_D8;
	struct VREG_ISP_444_422_DC               REG_DC;
	struct VREG_ISP_444_422_E0               REG_E0;
	struct VREG_ISP_444_422_E4               REG_E4;
	struct VREG_ISP_444_422_E8               REG_E8;
	struct VREG_ISP_444_422_EC               REG_EC;
	struct VREG_ISP_444_422_F0               REG_F0;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_FBCE_00 {
	union REG_FBCE_00                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_10 {
	union REG_FBCE_10                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_14 {
	union REG_FBCE_14                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_18 {
	union REG_FBCE_18                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_20 {
	union REG_FBCE_20                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_24 {
	union REG_FBCE_24                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_28 {
	union REG_FBCE_28                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_2C {
	union REG_FBCE_2C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCE_T {
	struct VREG_FBCE_00                      REG_00;
	struct VREG_RESV                        _resv_0x4[3];
	struct VREG_FBCE_10                      REG_10;
	struct VREG_FBCE_14                      REG_14;
	struct VREG_FBCE_18                      REG_18;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_FBCE_20                      REG_20;
	struct VREG_FBCE_24                      REG_24;
	struct VREG_FBCE_28                      REG_28;
	struct VREG_FBCE_2C                      REG_2C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_FBCD_00 {
	union REG_FBCD_00                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_0C {
	union REG_FBCD_0C                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_10 {
	union REG_FBCD_10                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_14 {
	union REG_FBCD_14                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_20 {
	union REG_FBCD_20                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_24 {
	union REG_FBCD_24                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_28 {
	union REG_FBCD_28                       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_FBCD_T {
	struct VREG_FBCD_00                      REG_00;
	struct VREG_RESV                        _resv_0x4[2];
	struct VREG_FBCD_0C                      REG_0C;
	struct VREG_FBCD_10                      REG_10;
	struct VREG_FBCD_14                      REG_14;
	struct VREG_RESV                        _resv_0x18[2];
	struct VREG_FBCD_20                      REG_20;
	struct VREG_FBCD_24                      REG_24;
	struct VREG_FBCD_28                      REG_28;
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

struct VREG_ISP_YNR_NS0_LUMA_TH_00 {
	union REG_ISP_YNR_NS0_LUMA_TH_00        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH_01 {
	union REG_ISP_YNR_NS0_LUMA_TH_01        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH_02 {
	union REG_ISP_YNR_NS0_LUMA_TH_02        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH_03 {
	union REG_ISP_YNR_NS0_LUMA_TH_03        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH_04 {
	union REG_ISP_YNR_NS0_LUMA_TH_04        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_LUMA_TH_05 {
	union REG_ISP_YNR_NS0_LUMA_TH_05        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE_00       {
	union REG_ISP_YNR_NS0_SLOPE_00          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE_01 {
	union REG_ISP_YNR_NS0_SLOPE_01          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE_02 {
	union REG_ISP_YNR_NS0_SLOPE_02          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE_03 {
	union REG_ISP_YNR_NS0_SLOPE_03          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_SLOPE_04 {
	union REG_ISP_YNR_NS0_SLOPE_04          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_00 {
	union REG_ISP_YNR_NS0_OFFSET_00         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_01 {
	union REG_ISP_YNR_NS0_OFFSET_01         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_02 {
	union REG_ISP_YNR_NS0_OFFSET_02         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_03 {
	union REG_ISP_YNR_NS0_OFFSET_03         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_04 {
	union REG_ISP_YNR_NS0_OFFSET_04         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS0_OFFSET_05 {
	union REG_ISP_YNR_NS0_OFFSET_05         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_00 {
	union REG_ISP_YNR_NS1_LUMA_TH_00        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_01 {
	union REG_ISP_YNR_NS1_LUMA_TH_01        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_02 {
	union REG_ISP_YNR_NS1_LUMA_TH_02        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_03 {
	union REG_ISP_YNR_NS1_LUMA_TH_03        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_04 {
	union REG_ISP_YNR_NS1_LUMA_TH_04        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_LUMA_TH_05 {
	union REG_ISP_YNR_NS1_LUMA_TH_05        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE_00 {
	union REG_ISP_YNR_NS1_SLOPE_00          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE_01 {
	union REG_ISP_YNR_NS1_SLOPE_01          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE_02 {
	union REG_ISP_YNR_NS1_SLOPE_02          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE_03 {
	union REG_ISP_YNR_NS1_SLOPE_03          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_SLOPE_04 {
	union REG_ISP_YNR_NS1_SLOPE_04          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_00 {
	union REG_ISP_YNR_NS1_OFFSET_00         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_01 {
	union REG_ISP_YNR_NS1_OFFSET_01         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_02 {
	union REG_ISP_YNR_NS1_OFFSET_02         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_03 {
	union REG_ISP_YNR_NS1_OFFSET_03         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_04 {
	union REG_ISP_YNR_NS1_OFFSET_04         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS1_OFFSET_05 {
	union REG_ISP_YNR_NS1_OFFSET_05         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_NS_GAIN         {
	union REG_ISP_YNR_NS_GAIN               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_00 {
	union REG_ISP_YNR_MOTION_LUT_00         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_01 {
	union REG_ISP_YNR_MOTION_LUT_01         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_02 {
	union REG_ISP_YNR_MOTION_LUT_02         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_03 {
	union REG_ISP_YNR_MOTION_LUT_03         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_04 {
	union REG_ISP_YNR_MOTION_LUT_04         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_05 {
	union REG_ISP_YNR_MOTION_LUT_05         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_06 {
	union REG_ISP_YNR_MOTION_LUT_06         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_07 {
	union REG_ISP_YNR_MOTION_LUT_07         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_08 {
	union REG_ISP_YNR_MOTION_LUT_08         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_09 {
	union REG_ISP_YNR_MOTION_LUT_09         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_10 {
	union REG_ISP_YNR_MOTION_LUT_10         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_11 {
	union REG_ISP_YNR_MOTION_LUT_11         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_12 {
	union REG_ISP_YNR_MOTION_LUT_12         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_13 {
	union REG_ISP_YNR_MOTION_LUT_13         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_14 {
	union REG_ISP_YNR_MOTION_LUT_14         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_LUT_15 {
	union REG_ISP_YNR_MOTION_LUT_15         write;
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

struct VREG_ISP_YNR_FILTER_MODE_EN {
	union REG_ISP_YNR_FILTER_MODE_EN        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_FILTER_MODE_ALPHA {
	union REG_ISP_YNR_FILTER_MODE_ALPHA     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_00 {
	union REG_ISP_YNR_RES_MOT_LUT_00        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_01 {
	union REG_ISP_YNR_RES_MOT_LUT_01        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_02 {
	union REG_ISP_YNR_RES_MOT_LUT_02        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_03 {
	union REG_ISP_YNR_RES_MOT_LUT_03        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_04 {
	union REG_ISP_YNR_RES_MOT_LUT_04        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_05 {
	union REG_ISP_YNR_RES_MOT_LUT_05        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_06 {
	union REG_ISP_YNR_RES_MOT_LUT_06        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_07 {
	union REG_ISP_YNR_RES_MOT_LUT_07        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_08 {
	union REG_ISP_YNR_RES_MOT_LUT_08        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_09 {
	union REG_ISP_YNR_RES_MOT_LUT_09        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_10 {
	union REG_ISP_YNR_RES_MOT_LUT_10        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_11 {
	union REG_ISP_YNR_RES_MOT_LUT_11        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_12 {
	union REG_ISP_YNR_RES_MOT_LUT_12        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_13 {
	union REG_ISP_YNR_RES_MOT_LUT_13        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_14 {
	union REG_ISP_YNR_RES_MOT_LUT_14        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOT_LUT_15 {
	union REG_ISP_YNR_RES_MOT_LUT_15        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MAX {
	union REG_ISP_YNR_RES_MAX               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_RES_MOTION_MAX {
	union REG_ISP_YNR_RES_MOTION_MAX        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_MOTION_NS_CLIP_MAX {
	union REG_ISP_YNR_MOTION_NS_CLIP_MAX    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YNR_WEIGHT_LUT      {
	union REG_ISP_YNR_WEIGHT_LUT            write;
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
	struct VREG_ISP_YNR_NS0_LUMA_TH_00       NS0_LUMA_TH_00;
	struct VREG_ISP_YNR_NS0_LUMA_TH_01       NS0_LUMA_TH_01;
	struct VREG_ISP_YNR_NS0_LUMA_TH_02       NS0_LUMA_TH_02;
	struct VREG_ISP_YNR_NS0_LUMA_TH_03       NS0_LUMA_TH_03;
	struct VREG_ISP_YNR_NS0_LUMA_TH_04       NS0_LUMA_TH_04;
	struct VREG_ISP_YNR_NS0_LUMA_TH_05       NS0_LUMA_TH_05;
	struct VREG_ISP_YNR_NS0_SLOPE_00         NS0_SLOPE_00;
	struct VREG_ISP_YNR_NS0_SLOPE_01         NS0_SLOPE_01;
	struct VREG_ISP_YNR_NS0_SLOPE_02         NS0_SLOPE_02;
	struct VREG_ISP_YNR_NS0_SLOPE_03         NS0_SLOPE_03;
	struct VREG_ISP_YNR_NS0_SLOPE_04         NS0_SLOPE_04;
	struct VREG_ISP_YNR_NS0_OFFSET_00        NS0_OFFSET_00;
	struct VREG_ISP_YNR_NS0_OFFSET_01        NS0_OFFSET_01;
	struct VREG_ISP_YNR_NS0_OFFSET_02        NS0_OFFSET_02;
	struct VREG_ISP_YNR_NS0_OFFSET_03        NS0_OFFSET_03;
	struct VREG_ISP_YNR_NS0_OFFSET_04        NS0_OFFSET_04;
	struct VREG_ISP_YNR_NS0_OFFSET_05        NS0_OFFSET_05;
	struct VREG_ISP_YNR_NS1_LUMA_TH_00       NS1_LUMA_TH_00;
	struct VREG_ISP_YNR_NS1_LUMA_TH_01       NS1_LUMA_TH_01;
	struct VREG_ISP_YNR_NS1_LUMA_TH_02       NS1_LUMA_TH_02;
	struct VREG_ISP_YNR_NS1_LUMA_TH_03       NS1_LUMA_TH_03;
	struct VREG_ISP_YNR_NS1_LUMA_TH_04       NS1_LUMA_TH_04;
	struct VREG_ISP_YNR_NS1_LUMA_TH_05       NS1_LUMA_TH_05;
	struct VREG_ISP_YNR_NS1_SLOPE_00         NS1_SLOPE_00;
	struct VREG_ISP_YNR_NS1_SLOPE_01         NS1_SLOPE_01;
	struct VREG_ISP_YNR_NS1_SLOPE_02         NS1_SLOPE_02;
	struct VREG_ISP_YNR_NS1_SLOPE_03         NS1_SLOPE_03;
	struct VREG_ISP_YNR_NS1_SLOPE_04         NS1_SLOPE_04;
	struct VREG_ISP_YNR_NS1_OFFSET_00        NS1_OFFSET_00;
	struct VREG_ISP_YNR_NS1_OFFSET_01        NS1_OFFSET_01;
	struct VREG_ISP_YNR_NS1_OFFSET_02        NS1_OFFSET_02;
	struct VREG_ISP_YNR_NS1_OFFSET_03        NS1_OFFSET_03;
	struct VREG_ISP_YNR_NS1_OFFSET_04        NS1_OFFSET_04;
	struct VREG_ISP_YNR_NS1_OFFSET_05        NS1_OFFSET_05;
	struct VREG_ISP_YNR_NS_GAIN              NS_GAIN;
	struct VREG_ISP_YNR_MOTION_LUT_00        MOTION_LUT_00;
	struct VREG_ISP_YNR_MOTION_LUT_01        MOTION_LUT_01;
	struct VREG_ISP_YNR_MOTION_LUT_02        MOTION_LUT_02;
	struct VREG_ISP_YNR_MOTION_LUT_03        MOTION_LUT_03;
	struct VREG_ISP_YNR_MOTION_LUT_04        MOTION_LUT_04;
	struct VREG_ISP_YNR_MOTION_LUT_05        MOTION_LUT_05;
	struct VREG_ISP_YNR_MOTION_LUT_06        MOTION_LUT_06;
	struct VREG_ISP_YNR_MOTION_LUT_07        MOTION_LUT_07;
	struct VREG_ISP_YNR_MOTION_LUT_08        MOTION_LUT_08;
	struct VREG_ISP_YNR_MOTION_LUT_09        MOTION_LUT_09;
	struct VREG_ISP_YNR_MOTION_LUT_10        MOTION_LUT_10;
	struct VREG_ISP_YNR_MOTION_LUT_11        MOTION_LUT_11;
	struct VREG_ISP_YNR_MOTION_LUT_12        MOTION_LUT_12;
	struct VREG_ISP_YNR_MOTION_LUT_13        MOTION_LUT_13;
	struct VREG_ISP_YNR_MOTION_LUT_14        MOTION_LUT_14;
	struct VREG_ISP_YNR_MOTION_LUT_15        MOTION_LUT_15;
	struct VREG_ISP_YNR_WEIGHT_INTRA_0       WEIGHT_INTRA_0;
	struct VREG_ISP_YNR_WEIGHT_INTRA_1       WEIGHT_INTRA_1;
	struct VREG_ISP_YNR_WEIGHT_INTRA_2       WEIGHT_INTRA_2;
	struct VREG_ISP_YNR_WEIGHT_NORM_1        WEIGHT_NORM_1;
	struct VREG_ISP_YNR_WEIGHT_NORM_2        WEIGHT_NORM_2;
	struct VREG_ISP_YNR_ALPHA_GAIN           ALPHA_GAIN;
	struct VREG_ISP_YNR_VAR_TH               VAR_TH;
	struct VREG_ISP_YNR_WEIGHT_SM            WEIGHT_SM;
	struct VREG_ISP_YNR_WEIGHT_V             WEIGHT_V;
	struct VREG_ISP_YNR_WEIGHT_H             WEIGHT_H;
	struct VREG_ISP_YNR_WEIGHT_D45           WEIGHT_D45;
	struct VREG_ISP_YNR_WEIGHT_D135          WEIGHT_D135;
	struct VREG_ISP_YNR_NEIGHBOR_MAX         NEIGHBOR_MAX;
	struct VREG_ISP_YNR_RES_K_SMOOTH         RES_K_SMOOTH;
	struct VREG_ISP_YNR_RES_K_TEXTURE        RES_K_TEXTURE;
	struct VREG_ISP_YNR_FILTER_MODE_EN       FILTER_MODE_EN;
	struct VREG_ISP_YNR_FILTER_MODE_ALPHA    FILTER_MODE_ALPHA;
	struct VREG_ISP_YNR_RES_MOT_LUT_00       RES_MOT_LUT_00;
	struct VREG_ISP_YNR_RES_MOT_LUT_01       RES_MOT_LUT_01;
	struct VREG_ISP_YNR_RES_MOT_LUT_02       RES_MOT_LUT_02;
	struct VREG_ISP_YNR_RES_MOT_LUT_03       RES_MOT_LUT_03;
	struct VREG_ISP_YNR_RES_MOT_LUT_04       RES_MOT_LUT_04;
	struct VREG_ISP_YNR_RES_MOT_LUT_05       RES_MOT_LUT_05;
	struct VREG_ISP_YNR_RES_MOT_LUT_06       RES_MOT_LUT_06;
	struct VREG_ISP_YNR_RES_MOT_LUT_07       RES_MOT_LUT_07;
	struct VREG_ISP_YNR_RES_MOT_LUT_08       RES_MOT_LUT_08;
	struct VREG_ISP_YNR_RES_MOT_LUT_09       RES_MOT_LUT_09;
	struct VREG_ISP_YNR_RES_MOT_LUT_10       RES_MOT_LUT_10;
	struct VREG_ISP_YNR_RES_MOT_LUT_11       RES_MOT_LUT_11;
	struct VREG_ISP_YNR_RES_MOT_LUT_12       RES_MOT_LUT_12;
	struct VREG_ISP_YNR_RES_MOT_LUT_13       RES_MOT_LUT_13;
	struct VREG_ISP_YNR_RES_MOT_LUT_14       RES_MOT_LUT_14;
	struct VREG_ISP_YNR_RES_MOT_LUT_15       RES_MOT_LUT_15;
	struct VREG_ISP_YNR_RES_MAX              RES_MAX;
	struct VREG_ISP_YNR_RES_MOTION_MAX       RES_MOTION_MAX;
	struct VREG_ISP_YNR_MOTION_NS_CLIP_MAX   MOTION_NS_CLIP_MAX;
	struct VREG_RESV                        _resv_0x168[38];
	struct VREG_ISP_YNR_WEIGHT_LUT           WEIGHT_LUT;
	struct VREG_RESV                        _resv_0x204[894];
	struct VREG_ISP_YNR_DUMMY                DUMMY;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CNR_ENABLE {
	union REG_ISP_CNR_ENABLE                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_STRENGTH_MODE {
	union REG_ISP_CNR_STRENGTH_MODE         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_PURPLE_TH {
	union REG_ISP_CNR_PURPLE_TH             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_PURPLE_CB {
	union REG_ISP_CNR_PURPLE_CB             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_GREEN_CB {
	union REG_ISP_CNR_GREEN_CB              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_00 {
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_04 {
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_08 {
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_12 {
	union REG_ISP_CNR_WEIGHT_LUT_INTER_CNR_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_MOTION_LUT_0 {
	union REG_ISP_CNR_MOTION_LUT_0          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_MOTION_LUT_4 {
	union REG_ISP_CNR_MOTION_LUT_4          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_MOTION_LUT_8 {
	union REG_ISP_CNR_MOTION_LUT_8          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_MOTION_LUT_12 {
	union REG_ISP_CNR_MOTION_LUT_12         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_PURPLE_CB2 {
	union REG_ISP_CNR_PURPLE_CB2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_MASK {
	union REG_ISP_CNR_MASK                  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_DUMMY {
	union REG_ISP_CNR_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE {
	union REG_ISP_CNR_EDGE_SCALE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_RATIO_SPEED {
	union REG_ISP_CNR_EDGE_RATIO_SPEED      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_DEPURPLE_WEIGHT_TH {
	union REG_ISP_CNR_DEPURPLE_WEIGHT_TH    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_CORING_MOTION_LUT_0 {
	union REG_ISP_CNR_CORING_MOTION_LUT_0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_CORING_MOTION_LUT_4 {
	union REG_ISP_CNR_CORING_MOTION_LUT_4   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_CORING_MOTION_LUT_8 {
	union REG_ISP_CNR_CORING_MOTION_LUT_8   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_CORING_MOTION_LUT_12 {
	union REG_ISP_CNR_CORING_MOTION_LUT_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE_LUT_0 {
	union REG_ISP_CNR_EDGE_SCALE_LUT_0      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE_LUT_4 {
	union REG_ISP_CNR_EDGE_SCALE_LUT_4      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE_LUT_8 {
	union REG_ISP_CNR_EDGE_SCALE_LUT_8      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE_LUT_12 {
	union REG_ISP_CNR_EDGE_SCALE_LUT_12     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_EDGE_SCALE_LUT_16 {
	union REG_ISP_CNR_EDGE_SCALE_LUT_16     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CNR_T {
	struct VREG_ISP_CNR_ENABLE               CNR_ENABLE;
	struct VREG_ISP_CNR_STRENGTH_MODE        CNR_STRENGTH_MODE;
	struct VREG_ISP_CNR_PURPLE_TH            CNR_PURPLE_TH;
	struct VREG_ISP_CNR_PURPLE_CB            CNR_PURPLE_CB;
	struct VREG_ISP_CNR_GREEN_CB             CNR_GREEN_CB;
	struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_00  WEIGHT_LUT_INTER_CNR_00;
	struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_04  WEIGHT_LUT_INTER_CNR_04;
	struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_08  WEIGHT_LUT_INTER_CNR_08;
	struct VREG_ISP_CNR_WEIGHT_LUT_INTER_CNR_12  WEIGHT_LUT_INTER_CNR_12;
	struct VREG_RESV                        _resv_0x24[2];
	struct VREG_ISP_CNR_MOTION_LUT_0         CNR_MOTION_LUT_0;
	struct VREG_ISP_CNR_MOTION_LUT_4         CNR_MOTION_LUT_4;
	struct VREG_ISP_CNR_MOTION_LUT_8         CNR_MOTION_LUT_8;
	struct VREG_ISP_CNR_MOTION_LUT_12        CNR_MOTION_LUT_12;
	struct VREG_ISP_CNR_PURPLE_CB2           CNR_PURPLE_CB2;
	struct VREG_ISP_CNR_MASK                 CNR_MASK;
	struct VREG_ISP_CNR_DUMMY                CNR_DUMMY;
	struct VREG_ISP_CNR_EDGE_SCALE           CNR_EDGE_SCALE;
	struct VREG_ISP_CNR_EDGE_RATIO_SPEED     CNR_EDGE_RATIO_SPEED;
	struct VREG_ISP_CNR_DEPURPLE_WEIGHT_TH   CNR_DEPURPLE_WEIGHT_TH;
	struct VREG_ISP_CNR_CORING_MOTION_LUT_0  CNR_CORING_MOTION_LUT_0;
	struct VREG_ISP_CNR_CORING_MOTION_LUT_4  CNR_CORING_MOTION_LUT_4;
	struct VREG_ISP_CNR_CORING_MOTION_LUT_8  CNR_CORING_MOTION_LUT_8;
	struct VREG_ISP_CNR_CORING_MOTION_LUT_12  CNR_CORING_MOTION_LUT_12;
	struct VREG_ISP_CNR_EDGE_SCALE_LUT_0     CNR_EDGE_SCALE_LUT_0;
	struct VREG_ISP_CNR_EDGE_SCALE_LUT_4     CNR_EDGE_SCALE_LUT_4;
	struct VREG_ISP_CNR_EDGE_SCALE_LUT_8     CNR_EDGE_SCALE_LUT_8;
	struct VREG_ISP_CNR_EDGE_SCALE_LUT_12    CNR_EDGE_SCALE_LUT_12;
	struct VREG_ISP_CNR_EDGE_SCALE_LUT_16    CNR_EDGE_SCALE_LUT_16;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_YCURV_YCUR_CTRL {
	union REG_ISP_YCURV_YCUR_CTRL           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_PROG_CTRL {
	union REG_ISP_YCURV_YCUR_PROG_CTRL      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_PROG_ST_ADDR {
	union REG_ISP_YCURV_YCUR_PROG_ST_ADDR   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_PROG_DATA {
	union REG_ISP_YCURV_YCUR_PROG_DATA      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_PROG_MAX {
	union REG_ISP_YCURV_YCUR_PROG_MAX       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_MEM_SW_MODE {
	union REG_ISP_YCURV_YCUR_MEM_SW_MODE    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_MEM_SW_RDATA {
	union REG_ISP_YCURV_YCUR_MEM_SW_RDATA   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_DBG {
	union REG_ISP_YCURV_YCUR_DBG            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_DMY0 {
	union REG_ISP_YCURV_YCUR_DMY0           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_DMY1 {
	union REG_ISP_YCURV_YCUR_DMY1           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_YCUR_DMY_R {
	union REG_ISP_YCURV_YCUR_DMY_R          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_YCURV_T {
	struct VREG_ISP_YCURV_YCUR_CTRL          YCUR_CTRL;
	struct VREG_ISP_YCURV_YCUR_PROG_CTRL     YCUR_PROG_CTRL;
	struct VREG_ISP_YCURV_YCUR_PROG_ST_ADDR  YCUR_PROG_ST_ADDR;
	struct VREG_ISP_YCURV_YCUR_PROG_DATA     YCUR_PROG_DATA;
	struct VREG_ISP_YCURV_YCUR_PROG_MAX      YCUR_PROG_MAX;
	struct VREG_ISP_YCURV_YCUR_MEM_SW_MODE   YCUR_SW_MODE;
	struct VREG_ISP_YCURV_YCUR_MEM_SW_RDATA  YCUR_SW_RDATA;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_YCURV_YCUR_DBG           YCUR_DBG;
	struct VREG_ISP_YCURV_YCUR_DMY0          YCUR_DMY0;
	struct VREG_ISP_YCURV_YCUR_DMY1          YCUR_DMY1;
	struct VREG_ISP_YCURV_YCUR_DMY_R         YCUR_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_TOP_INT_EVENT0 {
	union REG_ISP_TOP_INT_EVENT0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_INT_EVENT1 {
	union REG_ISP_TOP_INT_EVENT1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_INT_EVENT2 {
	union REG_ISP_TOP_INT_EVENT2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_ERROR_STS {
	union REG_ISP_TOP_ERROR_STS             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_INT_EVENT0_EN {
	union REG_ISP_TOP_INT_EVENT0_EN         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_INT_EVENT1_EN {
	union REG_ISP_TOP_INT_EVENT1_EN         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_INT_EVENT2_EN {
	union REG_ISP_TOP_INT_EVENT2_EN         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SW_CTRL_0 {
	union REG_ISP_TOP_SW_CTRL_0             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SW_CTRL_1 {
	union REG_ISP_TOP_SW_CTRL_1             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_CTRL_MODE_SEL0 {
	union REG_ISP_TOP_CTRL_MODE_SEL0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_CTRL_MODE_SEL1 {
	union REG_ISP_TOP_CTRL_MODE_SEL1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SCENARIOS_CTRL {
	union REG_ISP_TOP_SCENARIOS_CTRL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SW_RST {
	union REG_ISP_TOP_SW_RST                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_BLK_IDLE {
	union REG_ISP_TOP_BLK_IDLE              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_BLK_IDLE_ENABLE {
	union REG_ISP_TOP_BLK_IDLE_ENABLE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_DBUS0 {
	union REG_ISP_TOP_DBUS0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_DBUS1 {
	union REG_ISP_TOP_DBUS1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_DBUS2 {
	union REG_ISP_TOP_DBUS2                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_DBUS3 {
	union REG_ISP_TOP_DBUS3                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_FORCE_INT {
	union REG_ISP_TOP_FORCE_INT             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_DUMMY {
	union REG_ISP_TOP_DUMMY                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_IP_ENABLE0 {
	union REG_ISP_TOP_IP_ENABLE0            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_IP_ENABLE1 {
	union REG_ISP_TOP_IP_ENABLE1            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_IP_ENABLE2 {
	union REG_ISP_TOP_IP_ENABLE2            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_CMDQ_CTRL {
	union REG_ISP_TOP_CMDQ_CTRL             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_CMDQ_TRIG {
	union REG_ISP_TOP_CMDQ_TRIG             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_TRIG_CNT {
	union REG_ISP_TOP_TRIG_CNT              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SVN_VERSION {
	union REG_ISP_TOP_SVN_VERSION           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_TIMESTAMP {
	union REG_ISP_TOP_TIMESTAMP             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_SCLIE_ENABLE {
	union REG_ISP_TOP_SCLIE_ENABLE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_W_SLICE_THRESH_MAIN {
	union REG_ISP_TOP_W_SLICE_THRESH_MAIN   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_W_SLICE_THRESH_SUB_CURR {
	union REG_ISP_TOP_W_SLICE_THRESH_SUB_CURR  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_W_SLICE_THRESH_SUB_PRV {
	union REG_ISP_TOP_W_SLICE_THRESH_SUB_PRV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_R_SLICE_THRESH_MAIN {
	union REG_ISP_TOP_R_SLICE_THRESH_MAIN   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_R_SLICE_THRESH_SUB_CURR {
	union REG_ISP_TOP_R_SLICE_THRESH_SUB_CURR  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_R_SLICE_THRESH_SUB_PRV {
	union REG_ISP_TOP_R_SLICE_THRESH_SUB_PRV  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_RAW_FRAME_VALID {
	union REG_ISP_TOP_RAW_FRAME_VALID       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_FIRST_FRAME {
	union REG_ISP_TOP_FIRST_FRAME           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_TOP_T {
	struct VREG_ISP_TOP_INT_EVENT0           INT_EVENT0;
	struct VREG_ISP_TOP_INT_EVENT1           INT_EVENT1;
	struct VREG_ISP_TOP_INT_EVENT2           INT_EVENT2;
	struct VREG_ISP_TOP_ERROR_STS            ERROR_STS;
	struct VREG_ISP_TOP_INT_EVENT0_EN        INT_EVENT0_EN;
	struct VREG_ISP_TOP_INT_EVENT1_EN        INT_EVENT1_EN;
	struct VREG_ISP_TOP_INT_EVENT2_EN        INT_EVENT2_EN;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_ISP_TOP_SW_CTRL_0            SW_CTRL_0;
	struct VREG_ISP_TOP_SW_CTRL_1            SW_CTRL_1;
	struct VREG_ISP_TOP_CTRL_MODE_SEL0       CTRL_MODE_SEL0;
	struct VREG_ISP_TOP_CTRL_MODE_SEL1       CTRL_MODE_SEL1;
	struct VREG_ISP_TOP_SCENARIOS_CTRL       SCENARIOS_CTRL;
	struct VREG_ISP_TOP_SW_RST               SW_RST;
	struct VREG_ISP_TOP_BLK_IDLE             BLK_IDLE;
	struct VREG_ISP_TOP_BLK_IDLE_ENABLE      BLK_IDLE_ENABLE;
	struct VREG_ISP_TOP_DBUS0                DBUS0;
	struct VREG_ISP_TOP_DBUS1                DBUS1;
	struct VREG_ISP_TOP_DBUS2                DBUS2;
	struct VREG_ISP_TOP_DBUS3                DBUS3;
	struct VREG_ISP_TOP_FORCE_INT            FORCE_INT;
	struct VREG_ISP_TOP_DUMMY                DUMMY;
	struct VREG_ISP_TOP_IP_ENABLE0           IP_ENABLE0;
	struct VREG_ISP_TOP_IP_ENABLE1           IP_ENABLE1;
	struct VREG_ISP_TOP_IP_ENABLE2           IP_ENABLE2;
	struct VREG_RESV                        _resv_0x64[1];
	struct VREG_ISP_TOP_CMDQ_CTRL            CMDQ_CTRL;
	struct VREG_ISP_TOP_CMDQ_TRIG            CMDQ_TRIG;
	struct VREG_ISP_TOP_TRIG_CNT             TRIG_CNT;
	struct VREG_ISP_TOP_SVN_VERSION          SVN_VERSION;
	struct VREG_ISP_TOP_TIMESTAMP            TIMESTAMP;
	struct VREG_RESV                        _resv_0x7c[1];
	struct VREG_ISP_TOP_SCLIE_ENABLE         SCLIE_ENABLE;
	struct VREG_ISP_TOP_W_SLICE_THRESH_MAIN  W_SLICE_THRESH_MAIN;
	struct VREG_ISP_TOP_W_SLICE_THRESH_SUB_CURR  W_SLICE_THRESH_SUB_CURR;
	struct VREG_ISP_TOP_W_SLICE_THRESH_SUB_PRV  W_SLICE_THRESH_SUB_PRV;
	struct VREG_ISP_TOP_R_SLICE_THRESH_MAIN  R_SLICE_THRESH_MAIN;
	struct VREG_ISP_TOP_R_SLICE_THRESH_SUB_CURR  R_SLICE_THRESH_SUB_CURR;
	struct VREG_ISP_TOP_R_SLICE_THRESH_SUB_PRV  R_SLICE_THRESH_SUB_PRV;
	struct VREG_ISP_TOP_RAW_FRAME_VALID      RAW_FRAME_VALID;
	struct VREG_ISP_TOP_FIRST_FRAME          FIRST_FRAME;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_WDMA_CORE_SHADOW_RD_SEL   {
	union REG_WDMA_CORE_SHADOW_RD_SEL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_IP_DISABLE {
	union REG_WDMA_CORE_IP_DISABLE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_DISABLE_SEGLEN {
	union REG_WDMA_CORE_DISABLE_SEGLEN      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_UP_RING_BASE {
	union REG_WDMA_CORE_UP_RING_BASE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NORM_STATUS0 {
	union REG_WDMA_CORE_NORM_STATUS0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NORM_STATUS1 {
	union REG_WDMA_CORE_NORM_STATUS1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_EN {
	union REG_WDMA_CORE_RING_BUFFER_EN      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NORM_PERF  {
	union REG_WDMA_CORE_NORM_PERF           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_PATCH_ENABLE {
	union REG_WDMA_CORE_RING_PATCH_ENABLE   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_SET_RING_BASE {
	union REG_WDMA_CORE_SET_RING_BASE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BASE_ADDR_L {
	union REG_WDMA_CORE_RING_BASE_ADDR_L    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BASE_ADDR_H {
	union REG_WDMA_CORE_RING_BASE_ADDR_H    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE0 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE1 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE2 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE2   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE3 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE3   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE4 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE4   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE5 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE5   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE6 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE6   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE7 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE7   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE8 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE8   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE9 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE9   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE10 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE11 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE11  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE12 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE13 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE13  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE14 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE15 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE15  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE16 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE16  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE17 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE17  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE18 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE18  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE19 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE19  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE20 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE20  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE21 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE21  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE22 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE22  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE23 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE23  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE24 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE24  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE25 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE25  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE26 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE26  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE27 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE27  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE28 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE28  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE29 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE29  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE30 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE30  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_RING_BUFFER_SIZE31 {
	union REG_WDMA_CORE_RING_BUFFER_SIZE31  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS0 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS1 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS2 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS3 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS4 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS4  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS5 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS5  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS6 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS6  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS7 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS7  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS8 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS8  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS9 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS9  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS10 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS11 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS11  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS12 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS13 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS13  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS14 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS15 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS15  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS16 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS16  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS17 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS17  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS18 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS18  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS19 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS19  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS20 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS20  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS21 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS21  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS22 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS22  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS23 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS23  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS24 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS24  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS25 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS25  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS26 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS26  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS27 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS27  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS28 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS28  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS29 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS29  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS30 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS30  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS31 {
	union REG_WDMA_CORE_NEXT_DMA_ADDR_STS31  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_WDMA_CORE_T {
	struct VREG_WDMA_CORE_SHADOW_RD_SEL      SHADOW_RD_SEL;
	struct VREG_WDMA_CORE_IP_DISABLE         IP_DISABLE;
	struct VREG_WDMA_CORE_DISABLE_SEGLEN     DISABLE_SEGLEN;
	struct VREG_WDMA_CORE_UP_RING_BASE       UP_RING_BASE;
	struct VREG_WDMA_CORE_NORM_STATUS0       NORM_STATUS0;
	struct VREG_WDMA_CORE_NORM_STATUS1       NORM_STATUS1;
	struct VREG_WDMA_CORE_RING_BUFFER_EN     RING_BUFFER_EN;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_WDMA_CORE_NORM_PERF          NORM_PERF;
	struct VREG_RESV                        _resv_0x24[1];
	struct VREG_WDMA_CORE_RING_PATCH_ENABLE  RING_PATCH_ENABLE;
	struct VREG_WDMA_CORE_SET_RING_BASE      SET_RING_BASE;
	struct VREG_WDMA_CORE_RING_BASE_ADDR_L   RING_BASE_ADDR_L;
	struct VREG_WDMA_CORE_RING_BASE_ADDR_H   RING_BASE_ADDR_H;
	struct VREG_RESV                        _resv_0x38[18];
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE0  RING_BUFFER_SIZE0;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE1  RING_BUFFER_SIZE1;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE2  RING_BUFFER_SIZE2;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE3  RING_BUFFER_SIZE3;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE4  RING_BUFFER_SIZE4;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE5  RING_BUFFER_SIZE5;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE6  RING_BUFFER_SIZE6;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE7  RING_BUFFER_SIZE7;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE8  RING_BUFFER_SIZE8;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE9  RING_BUFFER_SIZE9;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE10  RING_BUFFER_SIZE10;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE11  RING_BUFFER_SIZE11;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE12  RING_BUFFER_SIZE12;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE13  RING_BUFFER_SIZE13;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE14  RING_BUFFER_SIZE14;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE15  RING_BUFFER_SIZE15;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE16  RING_BUFFER_SIZE16;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE17  RING_BUFFER_SIZE17;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE18  RING_BUFFER_SIZE18;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE19  RING_BUFFER_SIZE19;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE20  RING_BUFFER_SIZE20;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE21  RING_BUFFER_SIZE21;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE22  RING_BUFFER_SIZE22;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE23  RING_BUFFER_SIZE23;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE24  RING_BUFFER_SIZE24;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE25  RING_BUFFER_SIZE25;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE26  RING_BUFFER_SIZE26;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE27  RING_BUFFER_SIZE27;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE28  RING_BUFFER_SIZE28;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE29  RING_BUFFER_SIZE29;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE30  RING_BUFFER_SIZE30;
	struct VREG_WDMA_CORE_RING_BUFFER_SIZE31  RING_BUFFER_SIZE31;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS0  NEXT_DMA_ADDR_STS0;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS1  NEXT_DMA_ADDR_STS1;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS2  NEXT_DMA_ADDR_STS2;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS3  NEXT_DMA_ADDR_STS3;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS4  NEXT_DMA_ADDR_STS4;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS5  NEXT_DMA_ADDR_STS5;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS6  NEXT_DMA_ADDR_STS6;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS7  NEXT_DMA_ADDR_STS7;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS8  NEXT_DMA_ADDR_STS8;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS9  NEXT_DMA_ADDR_STS9;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS10  NEXT_DMA_ADDR_STS10;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS11  NEXT_DMA_ADDR_STS11;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS12  NEXT_DMA_ADDR_STS12;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS13  NEXT_DMA_ADDR_STS13;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS14  NEXT_DMA_ADDR_STS14;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS15  NEXT_DMA_ADDR_STS15;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS16  NEXT_DMA_ADDR_STS16;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS17  NEXT_DMA_ADDR_STS17;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS18  NEXT_DMA_ADDR_STS18;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS19  NEXT_DMA_ADDR_STS19;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS20  NEXT_DMA_ADDR_STS20;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS21  NEXT_DMA_ADDR_STS21;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS22  NEXT_DMA_ADDR_STS22;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS23  NEXT_DMA_ADDR_STS23;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS24  NEXT_DMA_ADDR_STS24;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS25  NEXT_DMA_ADDR_STS25;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS26  NEXT_DMA_ADDR_STS26;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS27  NEXT_DMA_ADDR_STS27;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS28  NEXT_DMA_ADDR_STS28;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS29  NEXT_DMA_ADDR_STS29;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS30  NEXT_DMA_ADDR_STS30;
	struct VREG_WDMA_CORE_NEXT_DMA_ADDR_STS31  NEXT_DMA_ADDR_STS31;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_RDMA_CORE_SHADOW_RD_SEL   {
	union REG_RDMA_CORE_SHADOW_RD_SEL       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_IP_DISABLE {
	union REG_RDMA_CORE_IP_DISABLE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_UP_RING_BASE {
	union REG_RDMA_CORE_UP_RING_BASE        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NORM_STATUS0 {
	union REG_RDMA_CORE_NORM_STATUS0        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NORM_STATUS1 {
	union REG_RDMA_CORE_NORM_STATUS1        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_EN {
	union REG_RDMA_CORE_RING_BUFFER_EN      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NORM_PERF  {
	union REG_RDMA_CORE_NORM_PERF           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_AR_PRIORITY_SEL {
	union REG_RDMA_CORE_AR_PRIORITY_SEL     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_PATCH_ENABLE {
	union REG_RDMA_CORE_RING_PATCH_ENABLE   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_SET_RING_BASE {
	union REG_RDMA_CORE_SET_RING_BASE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BASE_ADDR_L {
	union REG_RDMA_CORE_RING_BASE_ADDR_L    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BASE_ADDR_H {
	union REG_RDMA_CORE_RING_BASE_ADDR_H    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE0 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE0   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE1 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE1   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE2 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE2   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE3 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE3   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE4 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE4   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE5 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE5   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE6 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE6   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE7 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE7   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE8 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE8   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE9 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE9   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE10 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE11 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE11  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE12 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE13 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE13  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE14 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE15 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE15  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE16 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE16  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE17 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE17  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE18 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE18  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE19 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE19  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE20 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE20  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE21 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE21  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE22 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE22  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE23 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE23  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE24 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE24  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE25 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE25  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE26 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE26  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE27 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE27  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE28 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE28  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE29 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE29  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE30 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE30  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_RING_BUFFER_SIZE31 {
	union REG_RDMA_CORE_RING_BUFFER_SIZE31  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS0 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS1 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS2 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS3 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS4 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS4  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS5 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS5  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS6 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS6  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS7 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS7  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS8 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS8  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS9 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS9  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS10 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS11 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS11  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS12 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS13 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS13  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS14 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS15 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS15  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS16 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS16  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS17 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS17  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS18 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS18  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS19 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS19  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS20 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS20  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS21 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS21  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS22 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS22  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS23 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS23  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS24 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS24  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS25 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS25  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS26 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS26  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS27 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS27  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS28 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS28  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS29 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS29  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS30 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS30  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS31 {
	union REG_RDMA_CORE_NEXT_DMA_ADDR_STS31  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RDMA_CORE_T {
	struct VREG_RDMA_CORE_SHADOW_RD_SEL      SHADOW_RD_SEL;
	struct VREG_RDMA_CORE_IP_DISABLE         IP_DISABLE;
	struct VREG_RESV                        _resv_0x8[1];
	struct VREG_RDMA_CORE_UP_RING_BASE       UP_RING_BASE;
	struct VREG_RDMA_CORE_NORM_STATUS0       NORM_STATUS0;
	struct VREG_RDMA_CORE_NORM_STATUS1       NORM_STATUS1;
	struct VREG_RDMA_CORE_RING_BUFFER_EN     RING_BUFFER_EN;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_RDMA_CORE_NORM_PERF          NORM_PERF;
	struct VREG_RDMA_CORE_AR_PRIORITY_SEL    AR_PRIORITY_SEL;
	struct VREG_RDMA_CORE_RING_PATCH_ENABLE  RING_PATCH_ENABLE;
	struct VREG_RDMA_CORE_SET_RING_BASE      SET_RING_BASE;
	struct VREG_RDMA_CORE_RING_BASE_ADDR_L   RING_BASE_ADDR_L;
	struct VREG_RDMA_CORE_RING_BASE_ADDR_H   RING_BASE_ADDR_H;
	struct VREG_RESV                        _resv_0x38[18];
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE0  RING_BUFFER_SIZE0;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE1  RING_BUFFER_SIZE1;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE2  RING_BUFFER_SIZE2;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE3  RING_BUFFER_SIZE3;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE4  RING_BUFFER_SIZE4;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE5  RING_BUFFER_SIZE5;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE6  RING_BUFFER_SIZE6;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE7  RING_BUFFER_SIZE7;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE8  RING_BUFFER_SIZE8;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE9  RING_BUFFER_SIZE9;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE10  RING_BUFFER_SIZE10;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE11  RING_BUFFER_SIZE11;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE12  RING_BUFFER_SIZE12;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE13  RING_BUFFER_SIZE13;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE14  RING_BUFFER_SIZE14;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE15  RING_BUFFER_SIZE15;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE16  RING_BUFFER_SIZE16;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE17  RING_BUFFER_SIZE17;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE18  RING_BUFFER_SIZE18;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE19  RING_BUFFER_SIZE19;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE20  RING_BUFFER_SIZE20;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE21  RING_BUFFER_SIZE21;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE22  RING_BUFFER_SIZE22;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE23  RING_BUFFER_SIZE23;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE24  RING_BUFFER_SIZE24;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE25  RING_BUFFER_SIZE25;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE26  RING_BUFFER_SIZE26;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE27  RING_BUFFER_SIZE27;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE28  RING_BUFFER_SIZE28;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE29  RING_BUFFER_SIZE29;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE30  RING_BUFFER_SIZE30;
	struct VREG_RDMA_CORE_RING_BUFFER_SIZE31  RING_BUFFER_SIZE31;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS0  NEXT_DMA_ADDR_STS0;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS1  NEXT_DMA_ADDR_STS1;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS2  NEXT_DMA_ADDR_STS2;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS3  NEXT_DMA_ADDR_STS3;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS4  NEXT_DMA_ADDR_STS4;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS5  NEXT_DMA_ADDR_STS5;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS6  NEXT_DMA_ADDR_STS6;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS7  NEXT_DMA_ADDR_STS7;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS8  NEXT_DMA_ADDR_STS8;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS9  NEXT_DMA_ADDR_STS9;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS10  NEXT_DMA_ADDR_STS10;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS11  NEXT_DMA_ADDR_STS11;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS12  NEXT_DMA_ADDR_STS12;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS13  NEXT_DMA_ADDR_STS13;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS14  NEXT_DMA_ADDR_STS14;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS15  NEXT_DMA_ADDR_STS15;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS16  NEXT_DMA_ADDR_STS16;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS17  NEXT_DMA_ADDR_STS17;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS18  NEXT_DMA_ADDR_STS18;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS19  NEXT_DMA_ADDR_STS19;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS20  NEXT_DMA_ADDR_STS20;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS21  NEXT_DMA_ADDR_STS21;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS22  NEXT_DMA_ADDR_STS22;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS23  NEXT_DMA_ADDR_STS23;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS24  NEXT_DMA_ADDR_STS24;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS25  NEXT_DMA_ADDR_STS25;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS26  NEXT_DMA_ADDR_STS26;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS27  NEXT_DMA_ADDR_STS27;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS28  NEXT_DMA_ADDR_STS28;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS29  NEXT_DMA_ADDR_STS29;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS30  NEXT_DMA_ADDR_STS30;
	struct VREG_RDMA_CORE_NEXT_DMA_ADDR_STS31  NEXT_DMA_ADDR_STS31;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CSI_BDG_LITE_BDG_TOP_CTRL {
	union REG_ISP_CSI_BDG_LITE_BDG_TOP_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_0 {
	union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_1 {
	union REG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_FRAME_VLD {
	union REG_ISP_CSI_BDG_LITE_FRAME_VLD    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_SIZE {
	union REG_ISP_CSI_BDG_LITE_CH0_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_SIZE {
	union REG_ISP_CSI_BDG_LITE_CH1_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_SIZE {
	union REG_ISP_CSI_BDG_LITE_CH2_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_SIZE {
	union REG_ISP_CSI_BDG_LITE_CH3_SIZE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_CROP_EN {
	union REG_ISP_CSI_BDG_LITE_CH0_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_HORZ_CROP {
	union REG_ISP_CSI_BDG_LITE_CH0_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_VERT_CROP {
	union REG_ISP_CSI_BDG_LITE_CH0_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_CROP_EN {
	union REG_ISP_CSI_BDG_LITE_CH1_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_HORZ_CROP {
	union REG_ISP_CSI_BDG_LITE_CH1_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_VERT_CROP {
	union REG_ISP_CSI_BDG_LITE_CH1_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_CROP_EN {
	union REG_ISP_CSI_BDG_LITE_CH2_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_HORZ_CROP {
	union REG_ISP_CSI_BDG_LITE_CH2_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_VERT_CROP {
	union REG_ISP_CSI_BDG_LITE_CH2_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_CROP_EN {
	union REG_ISP_CSI_BDG_LITE_CH3_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_HORZ_CROP {
	union REG_ISP_CSI_BDG_LITE_CH3_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_VERT_CROP {
	union REG_ISP_CSI_BDG_LITE_CH3_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_LINE_INTP_HEIGHT {
	union REG_ISP_CSI_BDG_LITE_LINE_INTP_HEIGHT  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_0 {
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_1 {
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_2 {
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_3 {
	union REG_ISP_CSI_BDG_LITE_CH0_DEBUG_3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_0 {
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_1 {
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_2 {
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_3 {
	union REG_ISP_CSI_BDG_LITE_CH1_DEBUG_3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_0 {
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_1 {
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_2 {
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_3 {
	union REG_ISP_CSI_BDG_LITE_CH2_DEBUG_3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_0 {
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_1 {
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_2 {
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_2  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_3 {
	union REG_ISP_CSI_BDG_LITE_CH3_DEBUG_3  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_0 {
	union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_1 {
	union REG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_BDG_DEBUG {
	union REG_ISP_CSI_BDG_LITE_BDG_DEBUG    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_WR_URGENT_CTRL {
	union REG_ISP_CSI_BDG_LITE_WR_URGENT_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_RD_URGENT_CTRL {
	union REG_ISP_CSI_BDG_LITE_RD_URGENT_CTRL  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_DUMMY {
	union REG_ISP_CSI_BDG_LITE_DUMMY        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_0 {
	union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_1 {
	union REG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LITE_T {
	struct VREG_ISP_CSI_BDG_LITE_BDG_TOP_CTRL  CSI_BDG_TOP_CTRL;
	struct VREG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_0  CSI_BDG_INTERRUPT_CTRL_0;
	struct VREG_ISP_CSI_BDG_LITE_BDG_INTERRUPT_CTRL_1  CSI_BDG_INTERRUPT_CTRL_1;
	struct VREG_ISP_CSI_BDG_LITE_FRAME_VLD   FRAME_VLD;
	struct VREG_ISP_CSI_BDG_LITE_CH0_SIZE    CH0_SIZE;
	struct VREG_ISP_CSI_BDG_LITE_CH1_SIZE    CH1_SIZE;
	struct VREG_ISP_CSI_BDG_LITE_CH2_SIZE    CH2_SIZE;
	struct VREG_ISP_CSI_BDG_LITE_CH3_SIZE    CH3_SIZE;
	struct VREG_ISP_CSI_BDG_LITE_CH0_CROP_EN  CH0_CROP_EN;
	struct VREG_ISP_CSI_BDG_LITE_CH0_HORZ_CROP  CH0_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_LITE_CH0_VERT_CROP  CH0_VERT_CROP;
	struct VREG_RESV                        _resv_0x2c[1];
	struct VREG_ISP_CSI_BDG_LITE_CH1_CROP_EN  CH1_CROP_EN;
	struct VREG_ISP_CSI_BDG_LITE_CH1_HORZ_CROP  CH1_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_LITE_CH1_VERT_CROP  CH1_VERT_CROP;
	struct VREG_RESV                        _resv_0x3c[1];
	struct VREG_ISP_CSI_BDG_LITE_CH2_CROP_EN  CH2_CROP_EN;
	struct VREG_ISP_CSI_BDG_LITE_CH2_HORZ_CROP  CH2_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_LITE_CH2_VERT_CROP  CH2_VERT_CROP;
	struct VREG_RESV                        _resv_0x4c[1];
	struct VREG_ISP_CSI_BDG_LITE_CH3_CROP_EN  CH3_CROP_EN;
	struct VREG_ISP_CSI_BDG_LITE_CH3_HORZ_CROP  CH3_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_LITE_CH3_VERT_CROP  CH3_VERT_CROP;
	struct VREG_RESV                        _resv_0x5c[16];
	struct VREG_ISP_CSI_BDG_LITE_LINE_INTP_HEIGHT  LINE_INTP_HEIGHT;
	struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_0  CH0_DEBUG_0;
	struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_1  CH0_DEBUG_1;
	struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_2  CH0_DEBUG_2;
	struct VREG_ISP_CSI_BDG_LITE_CH0_DEBUG_3  CH0_DEBUG_3;
	struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_0  CH1_DEBUG_0;
	struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_1  CH1_DEBUG_1;
	struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_2  CH1_DEBUG_2;
	struct VREG_ISP_CSI_BDG_LITE_CH1_DEBUG_3  CH1_DEBUG_3;
	struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_0  CH2_DEBUG_0;
	struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_1  CH2_DEBUG_1;
	struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_2  CH2_DEBUG_2;
	struct VREG_ISP_CSI_BDG_LITE_CH2_DEBUG_3  CH2_DEBUG_3;
	struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_0  CH3_DEBUG_0;
	struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_1  CH3_DEBUG_1;
	struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_2  CH3_DEBUG_2;
	struct VREG_ISP_CSI_BDG_LITE_CH3_DEBUG_3  CH3_DEBUG_3;
	struct VREG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	struct VREG_ISP_CSI_BDG_LITE_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	struct VREG_ISP_CSI_BDG_LITE_BDG_DEBUG   BDG_DEBUG;
	struct VREG_RESV                        _resv_0xec[1];
	struct VREG_ISP_CSI_BDG_LITE_WR_URGENT_CTRL  CSI_WR_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_LITE_RD_URGENT_CTRL  CSI_RD_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_LITE_DUMMY       CSI_DUMMY;
	struct VREG_RESV                        _resv_0xfc[21];
	struct VREG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	struct VREG_ISP_CSI_BDG_LITE_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_RAW_RDMA_CTRL_READ_SEL {
	union REG_RAW_RDMA_CTRL_READ_SEL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_RDMA_CTRL_CONFIG {
	union REG_RAW_RDMA_CTRL_CONFIG          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_RDMA_CTRL_RDMA_SIZE {
	union REG_RAW_RDMA_CTRL_RDMA_SIZE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_RDMA_CTRL_DPCM_MODE {
	union REG_RAW_RDMA_CTRL_DPCM_MODE       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_RAW_RDMA_CTRL_T {
	struct VREG_RAW_RDMA_CTRL_READ_SEL       READ_SEL;
	struct VREG_RAW_RDMA_CTRL_CONFIG         CONFIG;
	struct VREG_RAW_RDMA_CTRL_RDMA_SIZE      RDMA_SIZE;
	struct VREG_RAW_RDMA_CTRL_DPCM_MODE      DPCM_MODE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LDCI_ENABLE {
	union REG_ISP_LDCI_ENABLE               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_STRENGTH {
	union REG_ISP_LDCI_STRENGTH             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_MAX {
	union REG_ISP_LDCI_LUMA_WGT_MAX         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_IIR_ALPHA {
	union REG_ISP_LDCI_IDX_IIR_ALPHA        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_EDGE_SCALE {
	union REG_ISP_LDCI_EDGE_SCALE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_EDGE_CLAMP {
	union REG_ISP_LDCI_EDGE_CLAMP           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_NORM {
	union REG_ISP_LDCI_IDX_FILTER_NORM      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_IDX_00 {
	union REG_ISP_LDCI_TONE_CURVE_IDX_00    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_BLK_SIZE_X {
	union REG_ISP_LDCI_BLK_SIZE_X           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_BLK_SIZE_X1 {
	union REG_ISP_LDCI_BLK_SIZE_X1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_SUBBLK_SIZE_X {
	union REG_ISP_LDCI_SUBBLK_SIZE_X        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_SUBBLK_SIZE_X1 {
	union REG_ISP_LDCI_SUBBLK_SIZE_X1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_INTERP_NORM_LR {
	union REG_ISP_LDCI_INTERP_NORM_LR       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_SUB_INTERP_NORM_LR {
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_MEAN_NORM_X {
	union REG_ISP_LDCI_MEAN_NORM_X          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_VAR_NORM_Y {
	union REG_ISP_LDCI_VAR_NORM_Y           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_UV_GAIN_MAX {
	union REG_ISP_LDCI_UV_GAIN_MAX          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IMG_WIDTHM1 {
	union REG_ISP_LDCI_IMG_WIDTHM1          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_STATUS {
	union REG_ISP_LDCI_STATUS               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_GRACE_RESET {
	union REG_ISP_LDCI_GRACE_RESET          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_MONITOR {
	union REG_ISP_LDCI_MONITOR              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_FLOW {
	union REG_ISP_LDCI_FLOW                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_MONITOR_SELECT {
	union REG_ISP_LDCI_MONITOR_SELECT       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LOCATION {
	union REG_ISP_LDCI_LOCATION             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_DEBUG {
	union REG_ISP_LDCI_DEBUG                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_DUMMY {
	union REG_ISP_LDCI_DUMMY                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_DMI_ENABLE {
	union REG_ISP_LDCI_DMI_ENABLE           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_DCI_BAYER_STARTING {
	union REG_ISP_LDCI_DCI_BAYER_STARTING   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_00 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_00    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_02 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_02    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_04 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_04    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_06 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_06    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_08 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_08    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_10 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_10    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_12 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_12    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_IDX_FILTER_LUT_14 {
	union REG_ISP_LDCI_IDX_FILTER_LUT_14    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_INTERP_NORM_LR1 {
	union REG_ISP_LDCI_INTERP_NORM_LR1      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_SUB_INTERP_NORM_LR1 {
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_00 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_00  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_02 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_02  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_04 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_04  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_06 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_06  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_08 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_08  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_10 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_10  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_12 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_12  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_14 {
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_14  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_00 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_00      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_04 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_04      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_08 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_08      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_12 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_12      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_16 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_16      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_20 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_20      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_24 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_24      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_28 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_28      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_LUMA_WGT_LUT_32 {
	union REG_ISP_LDCI_LUMA_WGT_LUT_32      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_VAR_FILTER_LUT_00 {
	union REG_ISP_LDCI_VAR_FILTER_LUT_00    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_VAR_FILTER_LUT_02 {
	union REG_ISP_LDCI_VAR_FILTER_LUT_02    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_VAR_FILTER_LUT_04 {
	union REG_ISP_LDCI_VAR_FILTER_LUT_04    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LDCI_T {
	struct VREG_ISP_LDCI_ENABLE              LDCI_ENABLE;
	struct VREG_ISP_LDCI_STRENGTH            LDCI_STRENGTH;
	struct VREG_ISP_LDCI_LUMA_WGT_MAX        LDCI_LUMA_WGT_MAX;
	struct VREG_ISP_LDCI_IDX_IIR_ALPHA       LDCI_IDX_IIR_ALPHA;
	struct VREG_ISP_LDCI_EDGE_SCALE          LDCI_EDGE_SCALE;
	struct VREG_ISP_LDCI_EDGE_CLAMP          LDCI_EDGE_CLAMP;
	struct VREG_ISP_LDCI_IDX_FILTER_NORM     LDCI_IDX_FILTER_NORM;
	struct VREG_ISP_LDCI_TONE_CURVE_IDX_00   LDCI_TONE_CURVE_IDX_00;
	struct VREG_RESV                        _resv_0x20[3];
	struct VREG_ISP_LDCI_BLK_SIZE_X          LDCI_BLK_SIZE_X;
	struct VREG_ISP_LDCI_BLK_SIZE_X1         LDCI_BLK_SIZE_X1;
	struct VREG_ISP_LDCI_SUBBLK_SIZE_X       LDCI_SUBBLK_SIZE_X;
	struct VREG_ISP_LDCI_SUBBLK_SIZE_X1      LDCI_SUBBLK_SIZE_X1;
	struct VREG_ISP_LDCI_INTERP_NORM_LR      LDCI_INTERP_NORM_LR;
	struct VREG_ISP_LDCI_SUB_INTERP_NORM_LR  LDCI_SUB_INTERP_NORM_LR;
	struct VREG_ISP_LDCI_MEAN_NORM_X         LDCI_MEAN_NORM_X;
	struct VREG_ISP_LDCI_VAR_NORM_Y          LDCI_VAR_NORM_Y;
	struct VREG_ISP_LDCI_UV_GAIN_MAX         LDCI_UV_GAIN_MAX;
	struct VREG_ISP_LDCI_IMG_WIDTHM1         LDCI_IMG_WIDTHM1;
	struct VREG_RESV                        _resv_0x54[11];
	struct VREG_ISP_LDCI_STATUS              LDCI_STATUS;
	struct VREG_ISP_LDCI_GRACE_RESET         LDCI_GRACE_RESET;
	struct VREG_ISP_LDCI_MONITOR             LDCI_MONITOR;
	struct VREG_ISP_LDCI_FLOW                LDCI_FLOW;
	struct VREG_ISP_LDCI_MONITOR_SELECT      LDCI_MONITOR_SELECT;
	struct VREG_ISP_LDCI_LOCATION            LDCI_LOCATION;
	struct VREG_ISP_LDCI_DEBUG               LDCI_DEBUG;
	struct VREG_ISP_LDCI_DUMMY               LDCI_DUMMY;
	struct VREG_ISP_LDCI_DMI_ENABLE          DMI_ENABLE;
	struct VREG_RESV                        _resv_0xa4[1];
	struct VREG_ISP_LDCI_DCI_BAYER_STARTING  DCI_BAYER_STARTING;
	struct VREG_RESV                        _resv_0xac[1];
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_00   LDCI_IDX_FILTER_LUT_00;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_02   LDCI_IDX_FILTER_LUT_02;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_04   LDCI_IDX_FILTER_LUT_04;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_06   LDCI_IDX_FILTER_LUT_06;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_08   LDCI_IDX_FILTER_LUT_08;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_10   LDCI_IDX_FILTER_LUT_10;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_12   LDCI_IDX_FILTER_LUT_12;
	struct VREG_ISP_LDCI_IDX_FILTER_LUT_14   LDCI_IDX_FILTER_LUT_14;
	struct VREG_ISP_LDCI_INTERP_NORM_LR1     LDCI_INTERP_NORM_LR1;
	struct VREG_ISP_LDCI_SUB_INTERP_NORM_LR1  LDCI_SUB_INTERP_NORM_LR1;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_00  LDCI_TONE_CURVE_LUT_00_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_02  LDCI_TONE_CURVE_LUT_00_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_04  LDCI_TONE_CURVE_LUT_00_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_06  LDCI_TONE_CURVE_LUT_00_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_08  LDCI_TONE_CURVE_LUT_00_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_10  LDCI_TONE_CURVE_LUT_00_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_12  LDCI_TONE_CURVE_LUT_00_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_00_14  LDCI_TONE_CURVE_LUT_00_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_00  LDCI_TONE_CURVE_LUT_01_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_02  LDCI_TONE_CURVE_LUT_01_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_04  LDCI_TONE_CURVE_LUT_01_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_06  LDCI_TONE_CURVE_LUT_01_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_08  LDCI_TONE_CURVE_LUT_01_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_10  LDCI_TONE_CURVE_LUT_01_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_12  LDCI_TONE_CURVE_LUT_01_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_01_14  LDCI_TONE_CURVE_LUT_01_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_00  LDCI_TONE_CURVE_LUT_02_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_02  LDCI_TONE_CURVE_LUT_02_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_04  LDCI_TONE_CURVE_LUT_02_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_06  LDCI_TONE_CURVE_LUT_02_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_08  LDCI_TONE_CURVE_LUT_02_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_10  LDCI_TONE_CURVE_LUT_02_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_12  LDCI_TONE_CURVE_LUT_02_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_02_14  LDCI_TONE_CURVE_LUT_02_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_00  LDCI_TONE_CURVE_LUT_03_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_02  LDCI_TONE_CURVE_LUT_03_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_04  LDCI_TONE_CURVE_LUT_03_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_06  LDCI_TONE_CURVE_LUT_03_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_08  LDCI_TONE_CURVE_LUT_03_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_10  LDCI_TONE_CURVE_LUT_03_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_12  LDCI_TONE_CURVE_LUT_03_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_03_14  LDCI_TONE_CURVE_LUT_03_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_00  LDCI_TONE_CURVE_LUT_04_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_02  LDCI_TONE_CURVE_LUT_04_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_04  LDCI_TONE_CURVE_LUT_04_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_06  LDCI_TONE_CURVE_LUT_04_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_08  LDCI_TONE_CURVE_LUT_04_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_10  LDCI_TONE_CURVE_LUT_04_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_12  LDCI_TONE_CURVE_LUT_04_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_04_14  LDCI_TONE_CURVE_LUT_04_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_00  LDCI_TONE_CURVE_LUT_05_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_02  LDCI_TONE_CURVE_LUT_05_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_04  LDCI_TONE_CURVE_LUT_05_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_06  LDCI_TONE_CURVE_LUT_05_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_08  LDCI_TONE_CURVE_LUT_05_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_10  LDCI_TONE_CURVE_LUT_05_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_12  LDCI_TONE_CURVE_LUT_05_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_05_14  LDCI_TONE_CURVE_LUT_05_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_00  LDCI_TONE_CURVE_LUT_06_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_02  LDCI_TONE_CURVE_LUT_06_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_04  LDCI_TONE_CURVE_LUT_06_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_06  LDCI_TONE_CURVE_LUT_06_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_08  LDCI_TONE_CURVE_LUT_06_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_10  LDCI_TONE_CURVE_LUT_06_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_12  LDCI_TONE_CURVE_LUT_06_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_06_14  LDCI_TONE_CURVE_LUT_06_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_00  LDCI_TONE_CURVE_LUT_07_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_02  LDCI_TONE_CURVE_LUT_07_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_04  LDCI_TONE_CURVE_LUT_07_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_06  LDCI_TONE_CURVE_LUT_07_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_08  LDCI_TONE_CURVE_LUT_07_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_10  LDCI_TONE_CURVE_LUT_07_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_12  LDCI_TONE_CURVE_LUT_07_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_07_14  LDCI_TONE_CURVE_LUT_07_14;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_00  LDCI_TONE_CURVE_LUT_P_00;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_02  LDCI_TONE_CURVE_LUT_P_02;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_04  LDCI_TONE_CURVE_LUT_P_04;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_06  LDCI_TONE_CURVE_LUT_P_06;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_08  LDCI_TONE_CURVE_LUT_P_08;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_10  LDCI_TONE_CURVE_LUT_P_10;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_12  LDCI_TONE_CURVE_LUT_P_12;
	struct VREG_ISP_LDCI_TONE_CURVE_LUT_P_14  LDCI_TONE_CURVE_LUT_P_14;
	struct VREG_RESV                        _resv_0x1f8[2];
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_00     LDCI_LUMA_WGT_LUT_00;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_04     LDCI_LUMA_WGT_LUT_04;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_08     LDCI_LUMA_WGT_LUT_08;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_12     LDCI_LUMA_WGT_LUT_12;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_16     LDCI_LUMA_WGT_LUT_16;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_20     LDCI_LUMA_WGT_LUT_20;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_24     LDCI_LUMA_WGT_LUT_24;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_28     LDCI_LUMA_WGT_LUT_28;
	struct VREG_ISP_LDCI_LUMA_WGT_LUT_32     LDCI_LUMA_WGT_LUT_32;
	struct VREG_RESV                        _resv_0x224[3];
	struct VREG_ISP_LDCI_VAR_FILTER_LUT_00   LDCI_VAR_FILTER_LUT_00;
	struct VREG_ISP_LDCI_VAR_FILTER_LUT_02   LDCI_VAR_FILTER_LUT_02;
	struct VREG_ISP_LDCI_VAR_FILTER_LUT_04   LDCI_VAR_FILTER_LUT_04;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_RGB_TOP_0 {
	union REG_ISP_RGB_TOP_0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_1 {
	union REG_ISP_RGB_TOP_1                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_2 {
	union REG_ISP_RGB_TOP_2                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_3 {
	union REG_ISP_RGB_TOP_3                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_4 {
	union REG_ISP_RGB_TOP_4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_5 {
	union REG_ISP_RGB_TOP_5                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_6 {
	union REG_ISP_RGB_TOP_6                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_7 {
	union REG_ISP_RGB_TOP_7                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_8 {
	union REG_ISP_RGB_TOP_8                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_9 {
	union REG_ISP_RGB_TOP_9                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_10 {
	union REG_ISP_RGB_TOP_10                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_11 {
	union REG_ISP_RGB_TOP_11                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_12 {
	union REG_ISP_RGB_TOP_12                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_13 {
	union REG_ISP_RGB_TOP_13                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_14 {
	union REG_ISP_RGB_TOP_14                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_DBG_IP_S_VLD {
	union REG_ISP_RGB_TOP_DBG_IP_S_VLD      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_DBG_IP_S_RDY {
	union REG_ISP_RGB_TOP_DBG_IP_S_RDY      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_DBG_DMI_VLD {
	union REG_ISP_RGB_TOP_DBG_DMI_VLD       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_DBG_DMI_RDY {
	union REG_ISP_RGB_TOP_DBG_DMI_RDY       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_PATGEN1 {
	union REG_ISP_RGB_TOP_PATGEN1           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_PATGEN2 {
	union REG_ISP_RGB_TOP_PATGEN2           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_PATGEN3 {
	union REG_ISP_RGB_TOP_PATGEN3           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_PATGEN4 {
	union REG_ISP_RGB_TOP_PATGEN4           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_CHK_SUM {
	union REG_ISP_RGB_TOP_CHK_SUM           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_DMA_IDLE {
	union REG_ISP_RGB_TOP_DMA_IDLE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_RGB_TOP_T {
	struct VREG_ISP_RGB_TOP_0                REG_0;
	struct VREG_ISP_RGB_TOP_1                REG_1;
	struct VREG_ISP_RGB_TOP_2                REG_2;
	struct VREG_ISP_RGB_TOP_3                REG_3;
	struct VREG_ISP_RGB_TOP_4                REG_4;
	struct VREG_ISP_RGB_TOP_5                REG_5;
	struct VREG_ISP_RGB_TOP_6                REG_6;
	struct VREG_ISP_RGB_TOP_7                REG_7;
	struct VREG_ISP_RGB_TOP_8                REG_8;
	struct VREG_ISP_RGB_TOP_9                REG_9;
	struct VREG_RESV                        _resv_0x28[2];
	struct VREG_ISP_RGB_TOP_10               REG_10;
	struct VREG_ISP_RGB_TOP_11               REG_11;
	struct VREG_ISP_RGB_TOP_12               REG_12;
	struct VREG_ISP_RGB_TOP_13               REG_13;
	struct VREG_ISP_RGB_TOP_14               REG_14;
	struct VREG_RESV                        _resv_0x44[3];
	struct VREG_ISP_RGB_TOP_DBG_IP_S_VLD     DBG_IP_S_VLD;
	struct VREG_ISP_RGB_TOP_DBG_IP_S_RDY     DBG_IP_S_RDY;
	struct VREG_ISP_RGB_TOP_DBG_DMI_VLD      DBG_DMI_VLD;
	struct VREG_ISP_RGB_TOP_DBG_DMI_RDY      DBG_DMI_RDY;
	struct VREG_ISP_RGB_TOP_PATGEN1          PATGEN1;
	struct VREG_ISP_RGB_TOP_PATGEN2          PATGEN2;
	struct VREG_ISP_RGB_TOP_PATGEN3          PATGEN3;
	struct VREG_ISP_RGB_TOP_PATGEN4          PATGEN4;
	struct VREG_ISP_RGB_TOP_CHK_SUM          CHK_SUM;
	struct VREG_ISP_RGB_TOP_DMA_IDLE         DMA_IDLE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_PRE_WDMA_CTRL {
	union REG_PRE_WDMA_CTRL                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_WDMA_CTRL_PRE_RAW_BE_RDMI_DPCM {
	union REG_PRE_WDMA_CTRL_PRE_RAW_BE_RDMI_DPCM  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_WDMA_CTRL_DUMMY {
	union REG_PRE_WDMA_CTRL_DUMMY           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_WDMA_CTRL_DEBUG_INFO {
	union REG_PRE_WDMA_CTRL_DEBUG_INFO      write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_WDMA_CTRL_T {
	struct VREG_PRE_WDMA_CTRL                PRE_WDMA_CTRL;
	struct VREG_PRE_WDMA_CTRL_PRE_RAW_BE_RDMI_DPCM  PRE_RAW_BE_RDMI_DPCM;
	struct VREG_PRE_WDMA_CTRL_DUMMY          DUMMY;
	struct VREG_RESV                        _resv_0xc[12];
	struct VREG_PRE_WDMA_CTRL_DEBUG_INFO     INFO;
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

struct VREG_ISP_EE_0C {
	union REG_ISP_EE_0C                     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_10 {
	union REG_ISP_EE_10                     write;
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

struct VREG_ISP_EE_HCC {
	union REG_ISP_EE_HCC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_HD0 {
	union REG_ISP_EE_HD0                    write;
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

struct VREG_ISP_EE_1C4 {
	union REG_ISP_EE_1C4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1C8 {
	union REG_ISP_EE_1C8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1CC {
	union REG_ISP_EE_1CC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1D0 {
	union REG_ISP_EE_1D0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1D4 {
	union REG_ISP_EE_1D4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1D8 {
	union REG_ISP_EE_1D8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1DC {
	union REG_ISP_EE_1DC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1E0 {
	union REG_ISP_EE_1E0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1E4 {
	union REG_ISP_EE_1E4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1E8 {
	union REG_ISP_EE_1E8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1EC {
	union REG_ISP_EE_1EC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1F0 {
	union REG_ISP_EE_1F0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1F4 {
	union REG_ISP_EE_1F4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1F8 {
	union REG_ISP_EE_1F8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_1FC {
	union REG_ISP_EE_1FC                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_EE_T {
	struct VREG_ISP_EE_00                    REG_00;
	struct VREG_ISP_EE_04                    REG_04;
	struct VREG_RESV                        _resv_0x8[1];
	struct VREG_ISP_EE_0C                    REG_0C;
	struct VREG_ISP_EE_10                    REG_10;
	struct VREG_RESV                        _resv_0x14[36];
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
	struct VREG_ISP_EE_HCC                   REG_HCC;
	struct VREG_ISP_EE_HD0                   REG_HD0;
	struct VREG_RESV                        _resv_0xd4[23];
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
	struct VREG_RESV                        _resv_0x1ac[6];
	struct VREG_ISP_EE_1C4                   REG_1C4;
	struct VREG_ISP_EE_1C8                   REG_1C8;
	struct VREG_ISP_EE_1CC                   REG_1CC;
	struct VREG_ISP_EE_1D0                   REG_1D0;
	struct VREG_ISP_EE_1D4                   REG_1D4;
	struct VREG_ISP_EE_1D8                   REG_1D8;
	struct VREG_ISP_EE_1DC                   REG_1DC;
	struct VREG_ISP_EE_1E0                   REG_1E0;
	struct VREG_ISP_EE_1E4                   REG_1E4;
	struct VREG_ISP_EE_1E8                   REG_1E8;
	struct VREG_ISP_EE_1EC                   REG_1EC;
	struct VREG_ISP_EE_1F0                   REG_1F0;
	struct VREG_ISP_EE_1F4                   REG_1F4;
	struct VREG_ISP_EE_1F8                   REG_1F8;
	struct VREG_ISP_EE_1FC                   REG_1FC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_YGAMMA_GAMMA_CTRL {
	union REG_YGAMMA_GAMMA_CTRL             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_PROG_CTRL {
	union REG_YGAMMA_GAMMA_PROG_CTRL        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_PROG_ST_ADDR {
	union REG_YGAMMA_GAMMA_PROG_ST_ADDR     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_PROG_DATA {
	union REG_YGAMMA_GAMMA_PROG_DATA        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_PROG_MAX {
	union REG_YGAMMA_GAMMA_PROG_MAX         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_MEM_SW_RADDR {
	union REG_YGAMMA_GAMMA_MEM_SW_RADDR     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_MEM_SW_RDATA {
	union REG_YGAMMA_GAMMA_MEM_SW_RDATA     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_MEM_SW_RDATA_BG {
	union REG_YGAMMA_GAMMA_MEM_SW_RDATA_BG  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_DBG {
	union REG_YGAMMA_GAMMA_DBG              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_DMY0 {
	union REG_YGAMMA_GAMMA_DMY0             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_DMY1 {
	union REG_YGAMMA_GAMMA_DMY1             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_GAMMA_DMY_R {
	union REG_YGAMMA_GAMMA_DMY_R            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YGAMMA_T {
	struct VREG_YGAMMA_GAMMA_CTRL            GAMMA_CTRL;
	struct VREG_YGAMMA_GAMMA_PROG_CTRL       GAMMA_PROG_CTRL;
	struct VREG_YGAMMA_GAMMA_PROG_ST_ADDR    GAMMA_PROG_ST_ADDR;
	struct VREG_YGAMMA_GAMMA_PROG_DATA       GAMMA_PROG_DATA;
	struct VREG_YGAMMA_GAMMA_PROG_MAX        GAMMA_PROG_MAX;
	struct VREG_YGAMMA_GAMMA_MEM_SW_RADDR    GAMMA_SW_RADDR;
	struct VREG_YGAMMA_GAMMA_MEM_SW_RDATA    GAMMA_SW_RDATA;
	struct VREG_YGAMMA_GAMMA_MEM_SW_RDATA_BG  GAMMA_SW_RDATA_BG;
	struct VREG_YGAMMA_GAMMA_DBG             GAMMA_DBG;
	struct VREG_YGAMMA_GAMMA_DMY0            GAMMA_DMY0;
	struct VREG_YGAMMA_GAMMA_DMY1            GAMMA_DMY1;
	struct VREG_YGAMMA_GAMMA_DMY_R           GAMMA_DMY_R;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_PREYEE_00 {
	union REG_ISP_PREYEE_00                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_04 {
	union REG_ISP_PREYEE_04                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_0C {
	union REG_ISP_PREYEE_0C                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_10 {
	union REG_ISP_PREYEE_10                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_A4 {
	union REG_ISP_PREYEE_A4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_A8 {
	union REG_ISP_PREYEE_A8                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_AC {
	union REG_ISP_PREYEE_AC                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_B0 {
	union REG_ISP_PREYEE_B0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_B4 {
	union REG_ISP_PREYEE_B4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_B8 {
	union REG_ISP_PREYEE_B8                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_BC {
	union REG_ISP_PREYEE_BC                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_C0 {
	union REG_ISP_PREYEE_C0                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_C4 {
	union REG_ISP_PREYEE_C4                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_C8 {
	union REG_ISP_PREYEE_C8                 write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_HCC {
	union REG_ISP_PREYEE_HCC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_HD0 {
	union REG_ISP_PREYEE_HD0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_130 {
	union REG_ISP_PREYEE_130                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_134 {
	union REG_ISP_PREYEE_134                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_138 {
	union REG_ISP_PREYEE_138                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_13C {
	union REG_ISP_PREYEE_13C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_140 {
	union REG_ISP_PREYEE_140                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_144 {
	union REG_ISP_PREYEE_144                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_148 {
	union REG_ISP_PREYEE_148                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_14C {
	union REG_ISP_PREYEE_14C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_150 {
	union REG_ISP_PREYEE_150                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_154 {
	union REG_ISP_PREYEE_154                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_158 {
	union REG_ISP_PREYEE_158                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_15C {
	union REG_ISP_PREYEE_15C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_160 {
	union REG_ISP_PREYEE_160                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_164 {
	union REG_ISP_PREYEE_164                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_168 {
	union REG_ISP_PREYEE_168                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_16C {
	union REG_ISP_PREYEE_16C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_170 {
	union REG_ISP_PREYEE_170                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_174 {
	union REG_ISP_PREYEE_174                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_178 {
	union REG_ISP_PREYEE_178                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_17C {
	union REG_ISP_PREYEE_17C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_180 {
	union REG_ISP_PREYEE_180                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_184 {
	union REG_ISP_PREYEE_184                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_188 {
	union REG_ISP_PREYEE_188                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_18C {
	union REG_ISP_PREYEE_18C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_190 {
	union REG_ISP_PREYEE_190                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_194 {
	union REG_ISP_PREYEE_194                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_198 {
	union REG_ISP_PREYEE_198                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_19C {
	union REG_ISP_PREYEE_19C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1A0 {
	union REG_ISP_PREYEE_1A0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1A4 {
	union REG_ISP_PREYEE_1A4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1A8 {
	union REG_ISP_PREYEE_1A8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1C4 {
	union REG_ISP_PREYEE_1C4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1C8 {
	union REG_ISP_PREYEE_1C8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1CC {
	union REG_ISP_PREYEE_1CC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1D0 {
	union REG_ISP_PREYEE_1D0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1D4 {
	union REG_ISP_PREYEE_1D4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1D8 {
	union REG_ISP_PREYEE_1D8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1DC {
	union REG_ISP_PREYEE_1DC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1E0 {
	union REG_ISP_PREYEE_1E0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1E4 {
	union REG_ISP_PREYEE_1E4                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1E8 {
	union REG_ISP_PREYEE_1E8                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1EC {
	union REG_ISP_PREYEE_1EC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1F0 {
	union REG_ISP_PREYEE_1F0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_1FC {
	union REG_ISP_PREYEE_1FC                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_PREYEE_T {
	struct VREG_ISP_PREYEE_00                REG_00;
	struct VREG_ISP_PREYEE_04                REG_04;
	struct VREG_RESV                        _resv_0x8[1];
	struct VREG_ISP_PREYEE_0C                REG_0C;
	struct VREG_ISP_PREYEE_10                REG_10;
	struct VREG_RESV                        _resv_0x14[36];
	struct VREG_ISP_PREYEE_A4                REG_A4;
	struct VREG_ISP_PREYEE_A8                REG_A8;
	struct VREG_ISP_PREYEE_AC                REG_AC;
	struct VREG_ISP_PREYEE_B0                REG_B0;
	struct VREG_ISP_PREYEE_B4                REG_B4;
	struct VREG_ISP_PREYEE_B8                REG_B8;
	struct VREG_ISP_PREYEE_BC                REG_BC;
	struct VREG_ISP_PREYEE_C0                REG_C0;
	struct VREG_ISP_PREYEE_C4                REG_C4;
	struct VREG_ISP_PREYEE_C8                REG_C8;
	struct VREG_ISP_PREYEE_HCC               REG_HCC;
	struct VREG_ISP_PREYEE_HD0               REG_HD0;
	struct VREG_RESV                        _resv_0xd4[23];
	struct VREG_ISP_PREYEE_130               REG_130;
	struct VREG_ISP_PREYEE_134               REG_134;
	struct VREG_ISP_PREYEE_138               REG_138;
	struct VREG_ISP_PREYEE_13C               REG_13C;
	struct VREG_ISP_PREYEE_140               REG_140;
	struct VREG_ISP_PREYEE_144               REG_144;
	struct VREG_ISP_PREYEE_148               REG_148;
	struct VREG_ISP_PREYEE_14C               REG_14C;
	struct VREG_ISP_PREYEE_150               REG_150;
	struct VREG_ISP_PREYEE_154               REG_154;
	struct VREG_ISP_PREYEE_158               REG_158;
	struct VREG_ISP_PREYEE_15C               REG_15C;
	struct VREG_ISP_PREYEE_160               REG_160;
	struct VREG_ISP_PREYEE_164               REG_164;
	struct VREG_ISP_PREYEE_168               REG_168;
	struct VREG_ISP_PREYEE_16C               REG_16C;
	struct VREG_ISP_PREYEE_170               REG_170;
	struct VREG_ISP_PREYEE_174               REG_174;
	struct VREG_ISP_PREYEE_178               REG_178;
	struct VREG_ISP_PREYEE_17C               REG_17C;
	struct VREG_ISP_PREYEE_180               REG_180;
	struct VREG_ISP_PREYEE_184               REG_184;
	struct VREG_ISP_PREYEE_188               REG_188;
	struct VREG_ISP_PREYEE_18C               REG_18C;
	struct VREG_ISP_PREYEE_190               REG_190;
	struct VREG_ISP_PREYEE_194               REG_194;
	struct VREG_ISP_PREYEE_198               REG_198;
	struct VREG_ISP_PREYEE_19C               REG_19C;
	struct VREG_ISP_PREYEE_1A0               REG_1A0;
	struct VREG_ISP_PREYEE_1A4               REG_1A4;
	struct VREG_ISP_PREYEE_1A8               REG_1A8;
	struct VREG_RESV                        _resv_0x1ac[6];
	struct VREG_ISP_PREYEE_1C4               REG_1C4;
	struct VREG_ISP_PREYEE_1C8               REG_1C8;
	struct VREG_ISP_PREYEE_1CC               REG_1CC;
	struct VREG_ISP_PREYEE_1D0               REG_1D0;
	struct VREG_ISP_PREYEE_1D4               REG_1D4;
	struct VREG_ISP_PREYEE_1D8               REG_1D8;
	struct VREG_ISP_PREYEE_1DC               REG_1DC;
	struct VREG_ISP_PREYEE_1E0               REG_1E0;
	struct VREG_ISP_PREYEE_1E4               REG_1E4;
	struct VREG_ISP_PREYEE_1E8               REG_1E8;
	struct VREG_ISP_PREYEE_1EC               REG_1EC;
	struct VREG_ISP_PREYEE_1F0               REG_1F0;
	struct VREG_RESV                        _resv_0x1f4[2];
	struct VREG_ISP_PREYEE_1FC               REG_1FC;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CSI_BDG_TOP_CTRL {
	union REG_ISP_CSI_BDG_TOP_CTRL          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_INTERRUPT_CTRL {
	union REG_ISP_CSI_BDG_INTERRUPT_CTRL    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DMA_DPCM_MODE {
	union REG_ISP_CSI_BDG_DMA_DPCM_MODE     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DMA_LD_DPCM_MODE {
	union REG_ISP_CSI_BDG_DMA_LD_DPCM_MODE  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_SIZE {
	union REG_ISP_CSI_BDG_CH0_SIZE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_SIZE {
	union REG_ISP_CSI_BDG_CH1_SIZE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_SIZE {
	union REG_ISP_CSI_BDG_CH2_SIZE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_SIZE {
	union REG_ISP_CSI_BDG_CH3_SIZE          write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_CROP_EN {
	union REG_ISP_CSI_BDG_CH0_CROP_EN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_HORZ_CROP {
	union REG_ISP_CSI_BDG_CH0_HORZ_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_VERT_CROP {
	union REG_ISP_CSI_BDG_CH0_VERT_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_BLC_SUM {
	union REG_ISP_CSI_BDG_CH0_BLC_SUM       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_CROP_EN {
	union REG_ISP_CSI_BDG_CH1_CROP_EN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_HORZ_CROP {
	union REG_ISP_CSI_BDG_CH1_HORZ_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_VERT_CROP {
	union REG_ISP_CSI_BDG_CH1_VERT_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_BLC_SUM {
	union REG_ISP_CSI_BDG_CH1_BLC_SUM       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_CROP_EN {
	union REG_ISP_CSI_BDG_CH2_CROP_EN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_HORZ_CROP {
	union REG_ISP_CSI_BDG_CH2_HORZ_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_VERT_CROP {
	union REG_ISP_CSI_BDG_CH2_VERT_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_BLC_SUM {
	union REG_ISP_CSI_BDG_CH2_BLC_SUM       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_CROP_EN {
	union REG_ISP_CSI_BDG_CH3_CROP_EN       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_HORZ_CROP {
	union REG_ISP_CSI_BDG_CH3_HORZ_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_VERT_CROP {
	union REG_ISP_CSI_BDG_CH3_VERT_CROP     write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_BLC_SUM {
	union REG_ISP_CSI_BDG_CH3_BLC_SUM       write;
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

struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT_0 {
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_DEBUG_0 {
	union REG_ISP_CSI_BDG_CH0_DEBUG_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_DEBUG_1 {
	union REG_ISP_CSI_BDG_CH0_DEBUG_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_DEBUG_2 {
	union REG_ISP_CSI_BDG_CH0_DEBUG_2       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH0_DEBUG_3 {
	union REG_ISP_CSI_BDG_CH0_DEBUG_3       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_DEBUG_0 {
	union REG_ISP_CSI_BDG_CH1_DEBUG_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_DEBUG_1 {
	union REG_ISP_CSI_BDG_CH1_DEBUG_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_DEBUG_2 {
	union REG_ISP_CSI_BDG_CH1_DEBUG_2       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH1_DEBUG_3 {
	union REG_ISP_CSI_BDG_CH1_DEBUG_3       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_DEBUG_0 {
	union REG_ISP_CSI_BDG_CH2_DEBUG_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_DEBUG_1 {
	union REG_ISP_CSI_BDG_CH2_DEBUG_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_DEBUG_2 {
	union REG_ISP_CSI_BDG_CH2_DEBUG_2       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH2_DEBUG_3 {
	union REG_ISP_CSI_BDG_CH2_DEBUG_3       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_DEBUG_0 {
	union REG_ISP_CSI_BDG_CH3_DEBUG_0       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_DEBUG_1 {
	union REG_ISP_CSI_BDG_CH3_DEBUG_1       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_DEBUG_2 {
	union REG_ISP_CSI_BDG_CH3_DEBUG_2       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_CH3_DEBUG_3 {
	union REG_ISP_CSI_BDG_CH3_DEBUG_3       write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS_0 {
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS_1 {
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_DEBUG {
	union REG_ISP_CSI_BDG_DEBUG             write;
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

struct VREG_ISP_CSI_BDG_DUMMY {
	union REG_ISP_CSI_BDG_DUMMY             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT_1 {
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_0 {
	union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_1 {
	union REG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH0_CROP_EN {
	union REG_ISP_CSI_BDG_WDMA_CH0_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH0_HORZ_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH0_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH0_VERT_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH0_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH1_CROP_EN {
	union REG_ISP_CSI_BDG_WDMA_CH1_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH1_HORZ_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH1_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH1_VERT_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH1_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH2_CROP_EN {
	union REG_ISP_CSI_BDG_WDMA_CH2_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH2_HORZ_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH2_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH2_VERT_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH2_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH3_CROP_EN {
	union REG_ISP_CSI_BDG_WDMA_CH3_CROP_EN  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH3_HORZ_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH3_HORZ_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_WDMA_CH3_VERT_CROP {
	union REG_ISP_CSI_BDG_WDMA_CH3_VERT_CROP  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_TRIG_DLY_CONTROL_0 {
	union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_0  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_TRIG_DLY_CONTROL_1 {
	union REG_ISP_CSI_BDG_TRIG_DLY_CONTROL_1  write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CSI_BDG_T {
	struct VREG_ISP_CSI_BDG_TOP_CTRL         CSI_BDG_TOP_CTRL;
	struct VREG_ISP_CSI_BDG_INTERRUPT_CTRL   CSI_BDG_INTERRUPT_CTRL;
	struct VREG_ISP_CSI_BDG_DMA_DPCM_MODE    CSI_BDG_DMA_DPCM_MODE;
	struct VREG_ISP_CSI_BDG_DMA_LD_DPCM_MODE  CSI_BDG_DMA_LD_DPCM_MODE;
	struct VREG_ISP_CSI_BDG_CH0_SIZE         CH0_SIZE;
	struct VREG_ISP_CSI_BDG_CH1_SIZE         CH1_SIZE;
	struct VREG_ISP_CSI_BDG_CH2_SIZE         CH2_SIZE;
	struct VREG_ISP_CSI_BDG_CH3_SIZE         CH3_SIZE;
	struct VREG_ISP_CSI_BDG_CH0_CROP_EN      CH0_CROP_EN;
	struct VREG_ISP_CSI_BDG_CH0_HORZ_CROP    CH0_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_CH0_VERT_CROP    CH0_VERT_CROP;
	struct VREG_ISP_CSI_BDG_CH0_BLC_SUM      CH0_BLC_SUM;
	struct VREG_ISP_CSI_BDG_CH1_CROP_EN      CH1_CROP_EN;
	struct VREG_ISP_CSI_BDG_CH1_HORZ_CROP    CH1_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_CH1_VERT_CROP    CH1_VERT_CROP;
	struct VREG_ISP_CSI_BDG_CH1_BLC_SUM      CH1_BLC_SUM;
	struct VREG_ISP_CSI_BDG_CH2_CROP_EN      CH2_CROP_EN;
	struct VREG_ISP_CSI_BDG_CH2_HORZ_CROP    CH2_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_CH2_VERT_CROP    CH2_VERT_CROP;
	struct VREG_ISP_CSI_BDG_CH2_BLC_SUM      CH2_BLC_SUM;
	struct VREG_ISP_CSI_BDG_CH3_CROP_EN      CH3_CROP_EN;
	struct VREG_ISP_CSI_BDG_CH3_HORZ_CROP    CH3_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_CH3_VERT_CROP    CH3_VERT_CROP;
	struct VREG_ISP_CSI_BDG_CH3_BLC_SUM      CH3_BLC_SUM;
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
	struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT_0  LINE_INTP_HEIGHT_0;
	struct VREG_ISP_CSI_BDG_CH0_DEBUG_0      CH0_DEBUG_0;
	struct VREG_ISP_CSI_BDG_CH0_DEBUG_1      CH0_DEBUG_1;
	struct VREG_ISP_CSI_BDG_CH0_DEBUG_2      CH0_DEBUG_2;
	struct VREG_ISP_CSI_BDG_CH0_DEBUG_3      CH0_DEBUG_3;
	struct VREG_ISP_CSI_BDG_CH1_DEBUG_0      CH1_DEBUG_0;
	struct VREG_ISP_CSI_BDG_CH1_DEBUG_1      CH1_DEBUG_1;
	struct VREG_ISP_CSI_BDG_CH1_DEBUG_2      CH1_DEBUG_2;
	struct VREG_ISP_CSI_BDG_CH1_DEBUG_3      CH1_DEBUG_3;
	struct VREG_ISP_CSI_BDG_CH2_DEBUG_0      CH2_DEBUG_0;
	struct VREG_ISP_CSI_BDG_CH2_DEBUG_1      CH2_DEBUG_1;
	struct VREG_ISP_CSI_BDG_CH2_DEBUG_2      CH2_DEBUG_2;
	struct VREG_ISP_CSI_BDG_CH2_DEBUG_3      CH2_DEBUG_3;
	struct VREG_ISP_CSI_BDG_CH3_DEBUG_0      CH3_DEBUG_0;
	struct VREG_ISP_CSI_BDG_CH3_DEBUG_1      CH3_DEBUG_1;
	struct VREG_ISP_CSI_BDG_CH3_DEBUG_2      CH3_DEBUG_2;
	struct VREG_ISP_CSI_BDG_CH3_DEBUG_3      CH3_DEBUG_3;
	struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS_0  INTERRUPT_STATUS_0;
	struct VREG_ISP_CSI_BDG_INTERRUPT_STATUS_1  INTERRUPT_STATUS_1;
	struct VREG_ISP_CSI_BDG_DEBUG            BDG_DEBUG;
	struct VREG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	struct VREG_ISP_CSI_BDG_WR_URGENT_CTRL   CSI_WR_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_RD_URGENT_CTRL   CSI_RD_URGENT_CTRL;
	struct VREG_ISP_CSI_BDG_DUMMY            CSI_DUMMY;
	struct VREG_ISP_CSI_BDG_LINE_INTP_HEIGHT_1  LINE_INTP_HEIGHT_1;
	struct VREG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_0  SLICE_LINE_INTP_HEIGHT_0;
	struct VREG_ISP_CSI_BDG_SLICE_LINE_INTP_HEIGHT_1  SLICE_LINE_INTP_HEIGHT_1;
	struct VREG_RESV                        _resv_0x108[2];
	struct VREG_ISP_CSI_BDG_WDMA_CH0_CROP_EN  WDMA_CH0_CROP_EN;
	struct VREG_ISP_CSI_BDG_WDMA_CH0_HORZ_CROP  WDMA_CH0_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_WDMA_CH0_VERT_CROP  WDMA_CH0_VERT_CROP;
	struct VREG_RESV                        _resv_0x11c[1];
	struct VREG_ISP_CSI_BDG_WDMA_CH1_CROP_EN  WDMA_CH1_CROP_EN;
	struct VREG_ISP_CSI_BDG_WDMA_CH1_HORZ_CROP  WDMA_CH1_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_WDMA_CH1_VERT_CROP  WDMA_CH1_VERT_CROP;
	struct VREG_RESV                        _resv_0x12c[1];
	struct VREG_ISP_CSI_BDG_WDMA_CH2_CROP_EN  WDMA_CH2_CROP_EN;
	struct VREG_ISP_CSI_BDG_WDMA_CH2_HORZ_CROP  WDMA_CH2_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_WDMA_CH2_VERT_CROP  WDMA_CH2_VERT_CROP;
	struct VREG_RESV                        _resv_0x13c[1];
	struct VREG_ISP_CSI_BDG_WDMA_CH3_CROP_EN  WDMA_CH3_CROP_EN;
	struct VREG_ISP_CSI_BDG_WDMA_CH3_HORZ_CROP  WDMA_CH3_HORZ_CROP;
	struct VREG_ISP_CSI_BDG_WDMA_CH3_VERT_CROP  WDMA_CH3_VERT_CROP;
	struct VREG_RESV                        _resv_0x14c[1];
	struct VREG_ISP_CSI_BDG_TRIG_DLY_CONTROL_0  TRIG_DLY_CONTROL_0;
	struct VREG_ISP_CSI_BDG_TRIG_DLY_CONTROL_1  TRIG_DLY_CONTROL_1;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_LCAC_REG00 {
	union REG_ISP_LCAC_REG00                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG04 {
	union REG_ISP_LCAC_REG04                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG08 {
	union REG_ISP_LCAC_REG08                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG0C {
	union REG_ISP_LCAC_REG0C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG10 {
	union REG_ISP_LCAC_REG10                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG14 {
	union REG_ISP_LCAC_REG14                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG18 {
	union REG_ISP_LCAC_REG18                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG1C {
	union REG_ISP_LCAC_REG1C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG20 {
	union REG_ISP_LCAC_REG20                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG24 {
	union REG_ISP_LCAC_REG24                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG28 {
	union REG_ISP_LCAC_REG28                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG2C {
	union REG_ISP_LCAC_REG2C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG30 {
	union REG_ISP_LCAC_REG30                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG34 {
	union REG_ISP_LCAC_REG34                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG38 {
	union REG_ISP_LCAC_REG38                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG3C {
	union REG_ISP_LCAC_REG3C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG40 {
	union REG_ISP_LCAC_REG40                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG44 {
	union REG_ISP_LCAC_REG44                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG48 {
	union REG_ISP_LCAC_REG48                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG4C {
	union REG_ISP_LCAC_REG4C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG50 {
	union REG_ISP_LCAC_REG50                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG54 {
	union REG_ISP_LCAC_REG54                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG58 {
	union REG_ISP_LCAC_REG58                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG5C {
	union REG_ISP_LCAC_REG5C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG60 {
	union REG_ISP_LCAC_REG60                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG64 {
	union REG_ISP_LCAC_REG64                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG68 {
	union REG_ISP_LCAC_REG68                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG6C {
	union REG_ISP_LCAC_REG6C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG70 {
	union REG_ISP_LCAC_REG70                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG74 {
	union REG_ISP_LCAC_REG74                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG78 {
	union REG_ISP_LCAC_REG78                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG7C {
	union REG_ISP_LCAC_REG7C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG80 {
	union REG_ISP_LCAC_REG80                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG84 {
	union REG_ISP_LCAC_REG84                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG88 {
	union REG_ISP_LCAC_REG88                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG8C {
	union REG_ISP_LCAC_REG8C                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_REG90 {
	union REG_ISP_LCAC_REG90                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LCAC_T {
	struct VREG_ISP_LCAC_REG00               REG00;
	struct VREG_ISP_LCAC_REG04               REG04;
	struct VREG_ISP_LCAC_REG08               REG08;
	struct VREG_ISP_LCAC_REG0C               REG0C;
	struct VREG_ISP_LCAC_REG10               REG10;
	struct VREG_ISP_LCAC_REG14               REG14;
	struct VREG_ISP_LCAC_REG18               REG18;
	struct VREG_ISP_LCAC_REG1C               REG1C;
	struct VREG_ISP_LCAC_REG20               REG20;
	struct VREG_ISP_LCAC_REG24               REG24;
	struct VREG_ISP_LCAC_REG28               REG28;
	struct VREG_ISP_LCAC_REG2C               REG2C;
	struct VREG_ISP_LCAC_REG30               REG30;
	struct VREG_ISP_LCAC_REG34               REG34;
	struct VREG_ISP_LCAC_REG38               REG38;
	struct VREG_ISP_LCAC_REG3C               REG3C;
	struct VREG_ISP_LCAC_REG40               REG40;
	struct VREG_ISP_LCAC_REG44               REG44;
	struct VREG_ISP_LCAC_REG48               REG48;
	struct VREG_ISP_LCAC_REG4C               REG4C;
	struct VREG_ISP_LCAC_REG50               REG50;
	struct VREG_ISP_LCAC_REG54               REG54;
	struct VREG_ISP_LCAC_REG58               REG58;
	struct VREG_ISP_LCAC_REG5C               REG5C;
	struct VREG_ISP_LCAC_REG60               REG60;
	struct VREG_ISP_LCAC_REG64               REG64;
	struct VREG_ISP_LCAC_REG68               REG68;
	struct VREG_ISP_LCAC_REG6C               REG6C;
	struct VREG_ISP_LCAC_REG70               REG70;
	struct VREG_ISP_LCAC_REG74               REG74;
	struct VREG_ISP_LCAC_REG78               REG78;
	struct VREG_ISP_LCAC_REG7C               REG7C;
	struct VREG_ISP_LCAC_REG80               REG80;
	struct VREG_ISP_LCAC_REG84               REG84;
	struct VREG_ISP_LCAC_REG88               REG88;
	struct VREG_ISP_LCAC_REG8C               REG8C;
	struct VREG_ISP_LCAC_REG90               REG90;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_ISP_CFA_00 {
	union REG_ISP_CFA_00                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_04 {
	union REG_ISP_CFA_04                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_0C {
	union REG_ISP_CFA_0C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_10 {
	union REG_ISP_CFA_10                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_14 {
	union REG_ISP_CFA_14                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_18 {
	union REG_ISP_CFA_18                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_1C {
	union REG_ISP_CFA_1C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_20 {
	union REG_ISP_CFA_20                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_30 {
	union REG_ISP_CFA_30                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_34 {
	union REG_ISP_CFA_34                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_38 {
	union REG_ISP_CFA_38                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_3C {
	union REG_ISP_CFA_3C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_40 {
	union REG_ISP_CFA_40                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_44 {
	union REG_ISP_CFA_44                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_48 {
	union REG_ISP_CFA_48                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_4C {
	union REG_ISP_CFA_4C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_70 {
	union REG_ISP_CFA_70                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_74 {
	union REG_ISP_CFA_74                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_78 {
	union REG_ISP_CFA_78                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_7C {
	union REG_ISP_CFA_7C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_80 {
	union REG_ISP_CFA_80                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_90 {
	union REG_ISP_CFA_90                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_94 {
	union REG_ISP_CFA_94                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_98 {
	union REG_ISP_CFA_98                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_9C {
	union REG_ISP_CFA_9C                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_A0 {
	union REG_ISP_CFA_A0                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_A4 {
	union REG_ISP_CFA_A4                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_A8 {
	union REG_ISP_CFA_A8                    write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_110 {
	union REG_ISP_CFA_110                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_120 {
	union REG_ISP_CFA_120                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_124 {
	union REG_ISP_CFA_124                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_128 {
	union REG_ISP_CFA_128                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_12C {
	union REG_ISP_CFA_12C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_130 {
	union REG_ISP_CFA_130                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_134 {
	union REG_ISP_CFA_134                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_138 {
	union REG_ISP_CFA_138                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_13C {
	union REG_ISP_CFA_13C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_140 {
	union REG_ISP_CFA_140                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_144 {
	union REG_ISP_CFA_144                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_148 {
	union REG_ISP_CFA_148                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_14C {
	union REG_ISP_CFA_14C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_150 {
	union REG_ISP_CFA_150                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_154 {
	union REG_ISP_CFA_154                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_158 {
	union REG_ISP_CFA_158                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_15C {
	union REG_ISP_CFA_15C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_160 {
	union REG_ISP_CFA_160                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_164 {
	union REG_ISP_CFA_164                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_168 {
	union REG_ISP_CFA_168                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_16C {
	union REG_ISP_CFA_16C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_170 {
	union REG_ISP_CFA_170                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_174 {
	union REG_ISP_CFA_174                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_178 {
	union REG_ISP_CFA_178                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_17C {
	union REG_ISP_CFA_17C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_180 {
	union REG_ISP_CFA_180                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_184 {
	union REG_ISP_CFA_184                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_188 {
	union REG_ISP_CFA_188                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_18C {
	union REG_ISP_CFA_18C                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_CFA_T {
	struct VREG_ISP_CFA_00                   REG_00;
	struct VREG_ISP_CFA_04                   REG_04;
	struct VREG_RESV                        _resv_0x8[1];
	struct VREG_ISP_CFA_0C                   REG_0C;
	struct VREG_ISP_CFA_10                   REG_10;
	struct VREG_ISP_CFA_14                   REG_14;
	struct VREG_ISP_CFA_18                   REG_18;
	struct VREG_ISP_CFA_1C                   REG_1C;
	struct VREG_ISP_CFA_20                   REG_20;
	struct VREG_RESV                        _resv_0x24[3];
	struct VREG_ISP_CFA_30                   REG_30;
	struct VREG_ISP_CFA_34                   REG_34;
	struct VREG_ISP_CFA_38                   REG_38;
	struct VREG_ISP_CFA_3C                   REG_3C;
	struct VREG_ISP_CFA_40                   REG_40;
	struct VREG_ISP_CFA_44                   REG_44;
	struct VREG_ISP_CFA_48                   REG_48;
	struct VREG_ISP_CFA_4C                   REG_4C;
	struct VREG_RESV                        _resv_0x50[8];
	struct VREG_ISP_CFA_70                   REG_70;
	struct VREG_ISP_CFA_74                   REG_74;
	struct VREG_ISP_CFA_78                   REG_78;
	struct VREG_ISP_CFA_7C                   REG_7C;
	struct VREG_ISP_CFA_80                   REG_80;
	struct VREG_RESV                        _resv_0x84[3];
	struct VREG_ISP_CFA_90                   REG_90;
	struct VREG_ISP_CFA_94                   REG_94;
	struct VREG_ISP_CFA_98                   REG_98;
	struct VREG_ISP_CFA_9C                   REG_9C;
	struct VREG_ISP_CFA_A0                   REG_A0;
	struct VREG_ISP_CFA_A4                   REG_A4;
	struct VREG_ISP_CFA_A8                   REG_A8;
	struct VREG_RESV                        _resv_0xac[25];
	struct VREG_ISP_CFA_110                  REG_110;
	struct VREG_RESV                        _resv_0x114[3];
	struct VREG_ISP_CFA_120                  REG_120;
	struct VREG_ISP_CFA_124                  REG_124;
	struct VREG_ISP_CFA_128                  REG_128;
	struct VREG_ISP_CFA_12C                  REG_12C;
	struct VREG_ISP_CFA_130                  REG_130;
	struct VREG_ISP_CFA_134                  REG_134;
	struct VREG_ISP_CFA_138                  REG_138;
	struct VREG_ISP_CFA_13C                  REG_13C;
	struct VREG_ISP_CFA_140                  REG_140;
	struct VREG_ISP_CFA_144                  REG_144;
	struct VREG_ISP_CFA_148                  REG_148;
	struct VREG_ISP_CFA_14C                  REG_14C;
	struct VREG_ISP_CFA_150                  REG_150;
	struct VREG_ISP_CFA_154                  REG_154;
	struct VREG_ISP_CFA_158                  REG_158;
	struct VREG_ISP_CFA_15C                  REG_15C;
	struct VREG_ISP_CFA_160                  REG_160;
	struct VREG_ISP_CFA_164                  REG_164;
	struct VREG_ISP_CFA_168                  REG_168;
	struct VREG_ISP_CFA_16C                  REG_16C;
	struct VREG_ISP_CFA_170                  REG_170;
	struct VREG_ISP_CFA_174                  REG_174;
	struct VREG_ISP_CFA_178                  REG_178;
	struct VREG_ISP_CFA_17C                  REG_17C;
	struct VREG_ISP_CFA_180                  REG_180;
	struct VREG_ISP_CFA_184                  REG_184;
	struct VREG_ISP_CFA_188                  REG_188;
	struct VREG_ISP_CFA_18C                  REG_18C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_PRE_RAW_VI_SEL_0 {
	union REG_PRE_RAW_VI_SEL_0              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_1 {
	union REG_PRE_RAW_VI_SEL_1              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_2 {
	union REG_PRE_RAW_VI_SEL_2              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_3 {
	union REG_PRE_RAW_VI_SEL_3              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_4 {
	union REG_PRE_RAW_VI_SEL_4              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_5 {
	union REG_PRE_RAW_VI_SEL_5              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_6 {
	union REG_PRE_RAW_VI_SEL_6              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_7 {
	union REG_PRE_RAW_VI_SEL_7              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_8 {
	union REG_PRE_RAW_VI_SEL_8              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_PRE_RAW_VI_SEL_T {
	struct VREG_PRE_RAW_VI_SEL_0             REG_0;
	struct VREG_PRE_RAW_VI_SEL_1             REG_1;
	struct VREG_RESV                        _resv_0x8[2];
	struct VREG_PRE_RAW_VI_SEL_2             REG_2;
	struct VREG_PRE_RAW_VI_SEL_3             REG_3;
	struct VREG_RESV                        _resv_0x18[2];
	struct VREG_PRE_RAW_VI_SEL_4             REG_4;
	struct VREG_PRE_RAW_VI_SEL_5             REG_5;
	struct VREG_PRE_RAW_VI_SEL_6             REG_6;
	struct VREG_PRE_RAW_VI_SEL_7             REG_7;
	struct VREG_PRE_RAW_VI_SEL_8             REG_8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct VREG_YUV_TOP_YUV_0 {
	union REG_YUV_TOP_YUV_0                 write;
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

struct VREG_YUV_TOP_YUV_CTRL {
	union REG_YUV_TOP_YUV_CTRL              write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_IMGW_M1 {
	union REG_YUV_TOP_IMGW_M1               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_STVALID_STATUS {
	union REG_YUV_TOP_STVALID_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_STREADY_STATUS {
	union REG_YUV_TOP_STREADY_STATUS        write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_PATGEN1 {
	union REG_YUV_TOP_PATGEN1               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_PATGEN2 {
	union REG_YUV_TOP_PATGEN2               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_PATGEN3 {
	union REG_YUV_TOP_PATGEN3               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_PATGEN4 {
	union REG_YUV_TOP_PATGEN4               write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_CHECK_SUM {
	union REG_YUV_TOP_CHECK_SUM             write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_YUV_TOP_T {
	struct VREG_YUV_TOP_YUV_0                YUV_0;
	struct VREG_RESV                        _resv_0x4[1];
	struct VREG_YUV_TOP_YUV_2                YUV_2;
	struct VREG_YUV_TOP_YUV_3                YUV_3;
	struct VREG_YUV_TOP_YUV_DEBUG_0          YUV_DEBUG_0;
	struct VREG_YUV_TOP_YUV_4                YUV_4;
	struct VREG_YUV_TOP_YUV_DEBUG_STATE      YUV_DEBUG_STATE;
	struct VREG_RESV                        _resv_0x1c[1];
	struct VREG_YUV_TOP_YUV_5                YUV_5;
	struct VREG_RESV                        _resv_0x24[15];
	struct VREG_YUV_TOP_YUV_CTRL             YUV_CTRL;
	struct VREG_YUV_TOP_IMGW_M1              IMGW_M1;
	struct VREG_RESV                        _resv_0x68[1];
	struct VREG_YUV_TOP_STVALID_STATUS       STVALID_STATUS;
	struct VREG_YUV_TOP_STREADY_STATUS       STREADY_STATUS;
	struct VREG_YUV_TOP_PATGEN1              PATGEN1;
	struct VREG_YUV_TOP_PATGEN2              PATGEN2;
	struct VREG_YUV_TOP_PATGEN3              PATGEN3;
	struct VREG_YUV_TOP_PATGEN4              PATGEN4;
	struct VREG_YUV_TOP_CHECK_SUM            CHECK_SUM;
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

struct VREG_ISP_LSC_DMI_WIDTHM1 {
	union REG_ISP_LSC_DMI_WIDTHM1           write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_DMI_HEIGHTM1 {
	union REG_ISP_LSC_DMI_HEIGHTM1          write;
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

struct VREG_ISP_LSC_INITX0 {
	union REG_ISP_LSC_INITX0                write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_INITY0 {
	union REG_ISP_LSC_INITY0                write;
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

struct VREG_ISP_LSC_INTERPOLATION {
	union REG_ISP_LSC_INTERPOLATION         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_DMI_ENABLE {
	union REG_ISP_LSC_DMI_ENABLE            write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_BLD {
	union REG_ISP_LSC_BLD                   write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_INTP_GAIN_MAX {
	union REG_ISP_LSC_INTP_GAIN_MAX         write;
	union CMDSET_FIELD                      ctrl;
};

struct VREG_ISP_LSC_INTP_GAIN_MIN {
	union REG_ISP_LSC_INTP_GAIN_MIN         write;
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
	struct VREG_RESV                        _resv_0x20[2];
	struct VREG_ISP_LSC_DMI_WIDTHM1          LSC_DMI_WIDTHM1;
	struct VREG_ISP_LSC_DMI_HEIGHTM1         LSC_DMI_HEIGHTM1;
	struct VREG_RESV                        _resv_0x30[3];
	struct VREG_ISP_LSC_GAIN_BASE            LSC_GAIN_BASE;
	struct VREG_ISP_LSC_XSTEP                LSC_XSTEP;
	struct VREG_ISP_LSC_YSTEP                LSC_YSTEP;
	struct VREG_ISP_LSC_IMGX0                LSC_IMGX0;
	struct VREG_ISP_LSC_IMGY0                LSC_IMGY0;
	struct VREG_RESV                        _resv_0x50[2];
	struct VREG_ISP_LSC_INITX0               LSC_INITX0;
	struct VREG_ISP_LSC_INITY0               LSC_INITY0;
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
	struct VREG_ISP_LSC_INTERPOLATION        INTERPOLATION;
	struct VREG_RESV                        _resv_0x94[3];
	struct VREG_ISP_LSC_DMI_ENABLE           DMI_ENABLE;
	struct VREG_ISP_LSC_BLD                  LSC_BLD;
	struct VREG_ISP_LSC_INTP_GAIN_MAX        LSC_INTP_GAIN_MAX;
	struct VREG_ISP_LSC_INTP_GAIN_MIN        LSC_INTP_GAIN_MIN;
};

#ifdef __cplusplus
}
#endif

#endif /* _VI_VREG_BLOCKS_H_ */
