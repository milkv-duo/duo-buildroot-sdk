#ifndef _REG_BLOCKS_H_
#define _REG_BLOCKS_H_

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_PRE_RAW_T {
	union REG_PRE_RAW_TOP_CTRL              TOP_CTRL;
	union REG_PRE_RAW_UP                    REG_UP;
	union REG_PRE_RAW_DUMMY                 DUMMY;
	union REG_PRE_RAW_POST_NO_RSPD_CYC      POST_NO_RSPD_CYC;
	union REG_PRE_RAW_POST_RGBMAP_NO_RSPD_CYC  POST_RGBMAP_NO_RSPD_CYC;
	union REG_PRE_RAW_FRAME_VLD             FRAME_VLD;
	union REG_PRE_RAW_DEBUG_STATE           DEBUG_STATE;
	uint32_t                                _resv_0x1c[1];
	union REG_PRE_RAW_ASYNC_STATE           ASYNC_STATE;
	union REG_PRE_RAW_RGBMAP_0_ASYNC_STATE  RGBMAP_0_ASYNC_STATE;
	union REG_PRE_RAW_RGBMAP_1_ASYNC_STATE  RGBMAP_1_ASYNC_STATE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSI_BDG_T {
	union REG_ISP_CSI_BDG_CTRL              CSI_CTRL;
	union REG_ISP_CSI_BDG_SIZE              CSI_SIZE;
	union REG_ISP_CSI_BDG_UP                CSI_BDG_UP;
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
	union REG_ISP_CSI_BDG_TGEN_ENABLE       CSI_TGEN_ENABLE;
	union REG_ISP_CSI_BDG_RT_DEBUG_INFO_PXL_CNT  CSI_RT_INFO_PXL_CNT;
	union REG_ISP_CSI_BDG_RT_DEBUG_INFO_LINE_CNT  CSI_RT_INFO_LINE_CNT;
	union REG_ISP_CSI_BDG_FE_DEBUG_INFO_PXL_CNT  CSI_FE_INFO_PXL_CNT;
	union REG_ISP_CSI_BDG_FE_DEBUG_INFO_LINE_CNT  CSI_FE_INFO_LINE_CNT;
	union REG_ISP_CSI_BDG_DEBUG_INFO_FRAME_CNT  CSI_INFO_FRAME_CNT;
	union REG_ISP_CSI_BDG_INTERRUPT_CTRL    CSI_INTERRUPT_CTRL;
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS  CSI_INTERRUPT_STATUS;
	union REG_ISP_CSI_BDG_DUMMY             CSI_DUMMY;
	union REG_ISP_CSI_BDG_DEBUG_INFO_VSYNC_CNT  CSI_INFO_VSYNC_CNT;
	union REG_ISP_CSI_BDG_DEBUG_INFO_SOF_CNT  CSI_INFO_SOF_CNT;
	union REG_ISP_CSI_BDG_CH0_NO_RSPD_CYC_CNT  CH0_NO_RSPD_CYC_CNT;
	union REG_ISP_CSI_BDG_CH1_NO_RSPD_CYC_CNT  CH1_NO_RSPD_CYC_CNT;
	union REG_ISP_CSI_BDG_DEBUG_STATE       CSI_DEBUG_STATE;
	union REG_ISP_CSI_BDG_DMA_DPCM_MODE     CSI_DMA_DPCM_MODE;
	union REG_ISP_CSI_BDG_OUT_VSYNC_LINE_DELAY  CSI_OUT_VSYNC_LINE_DELAY;
	union REG_ISP_CSI_BDG_WR_URGENT_CTRL    CSI_WR_URGENT_CTRL;
	union REG_ISP_CSI_BDG_RD_URGENT_CTRL    CSI_RD_URGENT_CTRL;
	union REG_ISP_CSI_BDG_CROP_EN           CSI_CROP_EN;
	union REG_ISP_CSI_BDG_HORZ_CROP         CSI_HORZ_CROP;
	union REG_ISP_CSI_BDG_VERT_CROP         CSI_VERT_CROP;
	union REG_ISP_CSI_BDG_LINE_INTP_HEIGHT  CSI_LINE_INTP_HEIGHT;
	union REG_ISP_CSI_BDG_CHANNEL_LINE_COUNT  CSI_CHANNEL_LINE_COUNT;
	union REG_ISP_CSI_BDG_HORZ_CROP_SE      CSI_HORZ_CROP_SE;
	union REG_ISP_CSI_BDG_VERT_CROP_SE      CSI_VERT_CROP_SE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DMA_T {
	union REG_ISP_DMA_0                     DMA_0;
	union REG_ISP_DMA_1                     DMA_1;
	union REG_ISP_DMA_2                     DMA_2;
	union REG_ISP_DMA_3                     DMA_3;
	union REG_ISP_DMA_4                     DMA_4;
	union REG_ISP_DMA_5                     DMA_5;
	union REG_ISP_DMA_6                     DMA_6;
	union REG_ISP_DMA_7                     DMA_7;
	union REG_ISP_DMA_8                     DMA_8;
	union REG_ISP_DMA_9                     DMA_9;
	union REG_ISP_DMA_10                    DMA_10;
	union REG_ISP_DMA_11                    DMA_11;
	union REG_ISP_DMA_12                    DMA_12;
	union REG_ISP_DMA_13                    DMA_13;
	union REG_ISP_DMA_14                    DMA_14;
	union REG_ISP_DMA_15                    DMA_15;
	union REG_ISP_DMA_16                    DMA_16;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CROP_T {
	union REG_ISP_CROP_0                    CROP_0;
	union REG_ISP_CROP_1                    CROP_1;
	union REG_ISP_CROP_2                    CROP_2;
	uint32_t                                _resv_0xc[1];
	union REG_ISP_CROP_DUMMY                DUMMY;
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
	union REG_ISP_BLC_IMG_BAYERID           IMG_BAYERID;
	union REG_ISP_BLC_DUMMY                 BLC_DUMMY;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_BLC_LOCATION              BLC_LOCATION;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LSCR_T {
	union REG_ISP_LSCR_SHADOW_RD_SEL        SHADOW_RD_SEL;
	union REG_ISP_LSCR_EN                   EN;
	union REG_ISP_LSCR_INDEX_CLR            INDEX_CLR;
	uint32_t                                _resv_0xc[61];
	union REG_ISP_LSCR_HSIZE                HSIZE;
	union REG_ISP_LSCR_VSIZE                VSIZE;
	union REG_ISP_LSCR_X_CENTER             X_CENTER;
	union REG_ISP_LSCR_Y_CENTER             Y_CENTER;
	union REG_ISP_LSCR_NORM_FACTOR          NORM_FACTOR;
	union REG_ISP_LSCR_LSC_LUT              LSC_LUT;
	union REG_ISP_LSCR_LSC_STRENTH          LSC_STRENTH;
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
	union REG_ISP_AE_HIST_AE_HIST_ENABLE    AE_HIST_ENABLE;
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
	union REG_ISP_AE_HIST_STS_AE1_OFFSETX   STS_AE1_OFFSETX;
	union REG_ISP_AE_HIST_STS_AE1_OFFSETY   STS_AE1_OFFSETY;
	union REG_ISP_AE_HIST_STS_AE1_NUMXM1    STS_AE1_NUMXM1;
	union REG_ISP_AE_HIST_STS_AE1_NUMYM1    STS_AE1_NUMYM1;
	union REG_ISP_AE_HIST_STS_AE1_WIDTH     STS_AE1_WIDTH;
	union REG_ISP_AE_HIST_STS_AE1_HEIGHT    STS_AE1_HEIGHT;
	union REG_ISP_AE_HIST_STS_AE1_STS_DIV   STS_AE1_STS_DIV;
	union REG_ISP_AE_HIST_STS_HIST1_MODE    STS_HIST1_MODE;
	union REG_ISP_AE_HIST_AE_HIST_BAYER_STARTING  AE_HIST_BAYER_STARTING;
	union REG_ISP_AE_HIST_AE_HIST_DUMMY     AE_HIST_DUMMY;
	union REG_ISP_AE_HIST_AE_HIST_CHECKSUM  AE_HIST_CHECKSUM;
	union REG_ISP_AE_HIST_WBG_4             WBG_4;
	union REG_ISP_AE_HIST_WBG_5             WBG_5;
	union REG_ISP_AE_HIST_WBG_6             WBG_6;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_AWB_T {
	union REG_ISP_AWB_STATUS                STATUS;
	union REG_ISP_AWB_GRACE_RESET           GRACE_RESET;
	union REG_ISP_AWB_MONITOR               MONITOR;
	union REG_ISP_AWB_SHADOW_SELECT         SHADOW_SELECT;
	union REG_ISP_AWB_KICKOFF               KICKOFF;
	union REG_ISP_AWB_ENABLE                ENABLE;
	uint32_t                                _resv_0x18[1];
	union REG_ISP_AWB_MONITOR_SELECT        MONITOR_SELECT;
	union REG_ISP_AWB_STS_OFFSETX           STS_OFFSETX;
	union REG_ISP_AWB_STS_OFFSETY           STS_OFFSETY;
	union REG_ISP_AWB_STS_NUMXM1            STS_NUMXM1;
	union REG_ISP_AWB_STS_NUMYM1            STS_NUMYM1;
	union REG_ISP_AWB_STS_WIDTH             STS_WIDTH;
	union REG_ISP_AWB_STS_HEIGHT            STS_HEIGHT;
	union REG_ISP_AWB_STS_SKIPX             STS_SKIPX;
	union REG_ISP_AWB_STS_SKIPY             STS_SKIPY;
	union REG_ISP_AWB_STS_CORNER_AVG_EN     STS_CORNER_AVG_EN;
	union REG_ISP_AWB_STS_CORNER_SIZE       STS_CORNER_SIZE;
	union REG_ISP_AWB_STS_STS_DIV           STS_STS_DIV;
	uint32_t                                _resv_0x4c[1];
	union REG_ISP_AWB_STS_R_LOTHD           STS_R_LOTHD;
	union REG_ISP_AWB_STS_R_UPTHD           STS_R_UPTHD;
	union REG_ISP_AWB_STS_G_LOTHD           STS_G_LOTHD;
	union REG_ISP_AWB_STS_G_UPTHD           STS_G_UPTHD;
	union REG_ISP_AWB_STS_B_LOTHD           STS_B_LOTHD;
	union REG_ISP_AWB_STS_B_UPTHD           STS_B_UPTHD;
	uint32_t                                _resv_0x68[3];
	union REG_ISP_AWB_WBG_4                 WBG_4;
	union REG_ISP_AWB_WBG_5                 WBG_5;
	uint32_t                                _resv_0x7c[1];
	union REG_ISP_AWB_MEM_SW_MODE           SW_MODE;
	union REG_ISP_AWB_MEM_SW_RADDR          SW_RADDR;
	union REG_ISP_AWB_LOCATION              LOCATION;
	union REG_ISP_AWB_MEM_SW_RDATA          SW_RDATA;
	union REG_ISP_AWB_BAYER_STARTING        BAYER_STARTING;
	union REG_ISP_AWB_DUMMY                 DUMMY;
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
	union REG_ISP_AF_CROP_BAYERID           CROP_BAYERID;
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
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_AF_GAMMA_T {
	union REG_ISP_AF_GAMMA_PROG_DATA        GAMMA_PROG_DATA;
	union REG_ISP_AF_GAMMA_PROG_CTRL        GAMMA_PROG_CTRL;
	union REG_ISP_AF_GAMMA_PROG_MAX         GAMMA_PROG_MAX;
	union REG_ISP_AF_GAMMA_CTRL             GAMMA_CTRL;
	union REG_ISP_AF_GAMMA_MEM_SW_MODE      GAMMA_SW_MODE;
	union REG_ISP_AF_GAMMA_MEM_SW_RADDR     GAMMA_SW_RADDR;
	union REG_ISP_AF_GAMMA_MEM_SW_RDATA     GAMMA_SW_RDATA;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_AF_GAMMA_DBG              GAMMA_DBG;
	union REG_ISP_AF_GAMMA_DMY0             GAMMA_DMY0;
	union REG_ISP_AF_GAMMA_DMY1             GAMMA_DMY1;
	union REG_ISP_AF_GAMMA_DMY_R            GAMMA_DMY_R;
	union REG_ISP_AF_GAMMA_PROG_ST_ADDR     GAMMA_PROG_ST_ADDR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_GMS_T {
	union REG_ISP_GMS_STATUS                GMS_STATUS;
	union REG_ISP_GMS_GRACE_RESET           GMS_GRACE_RESET;
	union REG_ISP_GMS_MONITOR               GMS_MONITOR;
	union REG_ISP_GMS_ENABLE                GMS_ENABLE;
	union REG_ISP_GMS_IMG_BAYERID           IMG_BAYERID;
	union REG_ISP_GMS_FLOW                  GMS_FLOW;
	union REG_ISP_GMS_START_X               GMS_START_X;
	union REG_ISP_GMS_START_Y               GMS_START_Y;
	union REG_ISP_GMS_LOCATION              GMS_LOCATION;
	uint32_t                                _resv_0x24[1];
	union REG_ISP_GMS_X_SECTION_SIZE        GMS_X_SECTION_SIZE;
	union REG_ISP_GMS_Y_SECTION_SIZE        GMS_Y_SECTION_SIZE;
	union REG_ISP_GMS_X_GAP                 GMS_X_GAP;
	union REG_ISP_GMS_Y_GAP                 GMS_Y_GAP;
	union REG_ISP_GMS_DUMMY                 GMS_DUMMY;
	uint32_t                                _resv_0x3c[1];
	union REG_ISP_GMS_MEM_SW_MODE           GMS_SW_MODE;
	union REG_ISP_GMS_MEM_SW_RADDR          GMS_SW_RADDR;
	union REG_ISP_GMS_MEM_SW_RDATA          GMS_SW_RDATA;
	union REG_ISP_GMS_MONITOR_SELECT        GMS_MONITOR_SELECT;
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
	uint32_t                                _resv_0x14[1];
	union REG_RAW_TOP_RAW_2                 RAW_2;
	union REG_RAW_TOP_DUMMY                 DUMMY;
	union REG_RAW_TOP_RAW_4                 RAW_4;
	union REG_RAW_TOP_STATUS                STATUS;
	union REG_RAW_TOP_DEBUG                 DEBUG;
	union REG_RAW_TOP_DEBUG_SELECT          DEBUG_SELECT;
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
	union REG_ISP_LSC_BLK_NUM_SELECT        LSC_BLK_NUM_SELECT;
	uint32_t                                _resv_0x24[6];
	union REG_ISP_LSC_GAIN_BASE             LSC_GAIN_BASE;
	union REG_ISP_LSC_XSTEP                 LSC_XSTEP;
	union REG_ISP_LSC_YSTEP                 LSC_YSTEP;
	union REG_ISP_LSC_IMGX0                 LSC_IMGX0;
	union REG_ISP_LSC_IMGY0                 LSC_IMGY0;
	uint32_t                                _resv_0x50[4];
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
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_FUSION_T {
	union REG_ISP_FUSION_FS_CTRL            FS_CTRL;
	union REG_ISP_FUSION_FS_FRAME_SIZE      FS_FRAME_SIZE;
	union REG_ISP_FUSION_FS_SE_GAIN         FS_SE_GAIN;
	union REG_ISP_FUSION_FS_LUMA_THD        FS_LUMA_THD;
	union REG_ISP_FUSION_FS_WGT             FS_WGT;
	union REG_ISP_FUSION_FS_WGT_SLOPE       FS_WGT_SLOPE;
	union REG_ISP_FUSION_FS_SHDW_READ_SEL   FS_SHDW_READ_SEL;
	union REG_ISP_FUSION_FS_UP              FS_UP;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_LTM_T {
	union REG_ISP_LTM_TOP_CTRL              LTM_TOP_CTRL;
	union REG_ISP_LTM_BLK_SIZE              LTM_BLK_SIZE;
	union REG_ISP_LTM_FRAME_SIZE            LTM_FRAME_SIZE;
	union REG_ISP_LTM_BE_STRTH_CTRL         LTM_BE_STRTH_CTRL;
	union REG_ISP_LTM_DE_STRTH_CTRL         LTM_DE_STRTH_CTRL;
	union REG_ISP_LTM_FILTER_WIN_SIZE_CTRL  LTM_FILTER_WIN_SIZE_CTRL;
	union REG_ISP_LTM_BGAIN_CTRL_0          LTM_BGAIN_CTRL_0;
	union REG_ISP_LTM_BGAIN_CTRL_1          LTM_BGAIN_CTRL_1;
	union REG_ISP_LTM_DGAIN_CTRL_0          LTM_DGAIN_CTRL_0;
	union REG_ISP_LTM_DGAIN_CTRL_1          LTM_DGAIN_CTRL_1;
	union REG_ISP_LTM_BRI_LCE_CTRL_0        LTM_BRI_LCE_CTRL_0;
	union REG_ISP_LTM_BRI_LCE_CTRL_1        LTM_BRI_LCE_CTRL_1;
	union REG_ISP_LTM_BRI_LCE_CTRL_2        LTM_BRI_LCE_CTRL_2;
	union REG_ISP_LTM_BRI_LCE_CTRL_3        LTM_BRI_LCE_CTRL_3;
	union REG_ISP_LTM_BRI_LCE_CTRL_4        LTM_BRI_LCE_CTRL_4;
	union REG_ISP_LTM_BRI_LCE_CTRL_5        LTM_BRI_LCE_CTRL_5;
	union REG_ISP_LTM_BRI_LCE_CTRL_6        LTM_BRI_LCE_CTRL_6;
	union REG_ISP_LTM_BRI_LCE_CTRL_7        LTM_BRI_LCE_CTRL_7;
	union REG_ISP_LTM_BRI_LCE_CTRL_8        LTM_BRI_LCE_CTRL_8;
	union REG_ISP_LTM_BRI_LCE_CTRL_9        LTM_BRI_LCE_CTRL_9;
	union REG_ISP_LTM_BRI_LCE_CTRL_10       LTM_BRI_LCE_CTRL_10;
	union REG_ISP_LTM_BRI_LCE_CTRL_11       LTM_BRI_LCE_CTRL_11;
	union REG_ISP_LTM_BRI_LCE_CTRL_12       LTM_BRI_LCE_CTRL_12;
	union REG_ISP_LTM_BRI_LCE_CTRL_13       LTM_BRI_LCE_CTRL_13;
	union REG_ISP_LTM_DAR_LCE_CTRL_0        LTM_DAR_LCE_CTRL_0;
	union REG_ISP_LTM_DAR_LCE_CTRL_1        LTM_DAR_LCE_CTRL_1;
	union REG_ISP_LTM_DAR_LCE_CTRL_2        LTM_DAR_LCE_CTRL_2;
	union REG_ISP_LTM_DAR_LCE_CTRL_3        LTM_DAR_LCE_CTRL_3;
	union REG_ISP_LTM_DAR_LCE_CTRL_4        LTM_DAR_LCE_CTRL_4;
	union REG_ISP_LTM_DAR_LCE_CTRL_5        LTM_DAR_LCE_CTRL_5;
	union REG_ISP_LTM_DAR_LCE_CTRL_6        LTM_DAR_LCE_CTRL_6;
	union REG_ISP_LTM_CURVE_QUAN_BIT        LTM_CURVE_QUAN_BIT;
	union REG_ISP_LTM_LMAP0_LP_DIST_WGT_0   LTM_LMAP0_LP_DIST_WGT_0;
	union REG_ISP_LTM_LMAP0_LP_DIST_WGT_1   LTM_LMAP0_LP_DIST_WGT_1;
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_0   LTM_LMAP0_LP_DIFF_WGT_0;
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_1   LTM_LMAP0_LP_DIFF_WGT_1;
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_2   LTM_LMAP0_LP_DIFF_WGT_2;
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_3   LTM_LMAP0_LP_DIFF_WGT_3;
	union REG_ISP_LTM_LMAP0_LP_DIFF_WGT_4   LTM_LMAP0_LP_DIFF_WGT_4;
	union REG_ISP_LTM_LMAP1_LP_DIST_WGT_0   LTM_LMAP1_LP_DIST_WGT_0;
	union REG_ISP_LTM_LMAP1_LP_DIST_WGT_1   LTM_LMAP1_LP_DIST_WGT_1;
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_0   LTM_LMAP1_LP_DIFF_WGT_0;
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_1   LTM_LMAP1_LP_DIFF_WGT_1;
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_2   LTM_LMAP1_LP_DIFF_WGT_2;
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_3   LTM_LMAP1_LP_DIFF_WGT_3;
	union REG_ISP_LTM_LMAP1_LP_DIFF_WGT_4   LTM_LMAP1_LP_DIFF_WGT_4;
	union REG_ISP_LTM_BE_DIST_WGT_0         LTM_BE_DIST_WGT_0;
	union REG_ISP_LTM_BE_DIST_WGT_1         LTM_BE_DIST_WGT_1;
	union REG_ISP_LTM_DE_DIST_WGT_0         LTM_DE_DIST_WGT_0;
	union REG_ISP_LTM_DE_DIST_WGT_1         LTM_DE_DIST_WGT_1;
	union REG_ISP_LTM_DE_LUMA_WGT_0         LTM_DE_LUMA_WGT_0;
	union REG_ISP_LTM_DE_LUMA_WGT_1         LTM_DE_LUMA_WGT_1;
	union REG_ISP_LTM_DE_LUMA_WGT_2         LTM_DE_LUMA_WGT_2;
	union REG_ISP_LTM_DE_LUMA_WGT_3         LTM_DE_LUMA_WGT_3;
	union REG_ISP_LTM_DE_LUMA_WGT_4         LTM_DE_LUMA_WGT_4;
	uint32_t                                _resv_0xdc[1];
	union REG_ISP_LTM_DTONE_CURVE_PROG_DATA  DTONE_CURVE_PROG_DATA;
	union REG_ISP_LTM_DTONE_CURVE_PROG_CTRL  DTONE_CURVE_PROG_CTRL;
	union REG_ISP_LTM_DTONE_CURVE_PROG_MAX  DTONE_CURVE_PROG_MAX;
	union REG_ISP_LTM_DTONE_CURVE_CTRL      DTONE_CURVE_CTRL;
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_MODE  DTONE_CURVE_SW_MODE;
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RADDR  DTONE_CURVE_SW_RADDR;
	union REG_ISP_LTM_DTONE_CURVE_MEM_SW_RDATA  DTONE_CURVE_SW_RDATA;
	uint32_t                                _resv_0xfc[1];
	union REG_ISP_LTM_BTONE_CURVE_PROG_DATA  BTONE_CURVE_PROG_DATA;
	union REG_ISP_LTM_BTONE_CURVE_PROG_CTRL  BTONE_CURVE_PROG_CTRL;
	union REG_ISP_LTM_BTONE_CURVE_PROG_MAX  BTONE_CURVE_PROG_MAX;
	union REG_ISP_LTM_BTONE_CURVE_CTRL      BTONE_CURVE_CTRL;
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_MODE  BTONE_CURVE_SW_MODE;
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RADDR  BTONE_CURVE_SW_RADDR;
	union REG_ISP_LTM_BTONE_CURVE_MEM_SW_RDATA  BTONE_CURVE_SW_RDATA;
	uint32_t                                _resv_0x11c[1];
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_DATA  GLOBAL_CURVE_PROG_DATA;
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_CTRL  GLOBAL_CURVE_PROG_CTRL;
	union REG_ISP_LTM_GLOBAL_CURVE_PROG_MAX  GLOBAL_CURVE_PROG_MAX;
	union REG_ISP_LTM_GLOBAL_CURVE_CTRL     GLOBAL_CURVE_CTRL;
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_MODE  GLOBAL_CURVE_SW_MODE;
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RADDR  GLOBAL_CURVE_SW_RADDR;
	union REG_ISP_LTM_GLOBAL_CURVE_MEM_SW_RDATA  GLOBAL_CURVE_SW_RDATA;
	uint32_t                                _resv_0x13c[1];
	union REG_ISP_LTM_RESIZE_COEFF_PROG_CTRL  RESIZE_COEFF_PROG_CTRL;
	union REG_ISP_LTM_RESIZE_COEFF_WDATA_0  RESIZE_COEFF_WDATA_0;
	union REG_ISP_LTM_RESIZE_COEFF_WDATA_1  RESIZE_COEFF_WDATA_1;
	union REG_ISP_LTM_CFA_CTRL_0            CFA_CTRL_0;
	union REG_ISP_LTM_CFA_CTRL_1            CFA_CTRL_1;
	union REG_ISP_LTM_CFA_CTRL_2            CFA_CTRL_2;
	union REG_ISP_LTM_CFA_CTRL_3            CFA_CTRL_3;
	union REG_ISP_LTM_CFA_CTRL_4            CFA_CTRL_4;
	union REG_ISP_LTM_CFA_CTRL_5            CFA_CTRL_5;
	union REG_ISP_LTM_CFA_CTRL_6            CFA_CTRL_6;
	union REG_ISP_LTM_CFA_CTRL_7            CFA_CTRL_7;
	union REG_ISP_LTM_CFA_CTRL_8            CFA_CTRL_8;
	union REG_ISP_LTM_TILE_MODE_CTRL_0      TILE_MODE_CTRL_0;
	union REG_ISP_LTM_TILE_MODE_CTRL_1      TILE_MODE_CTRL_1;
	union REG_ISP_LTM_TILE_MODE_CTRL_2      TILE_MODE_CTRL_2;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_BNR_T {
	union REG_ISP_BNR_SHADOW_RD_SEL         SHADOW_RD_SEL;
	union REG_ISP_BNR_OUT_SEL               OUT_SEL;
	union REG_ISP_BNR_INDEX_CLR             INDEX_CLR;
	uint32_t                                _resv_0xc[1];
	union REG_ISP_BNR_BAYER_TYPE            BAYER_TYPE;
	uint32_t                                _resv_0x14[59];
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
	uint32_t                                _resv_0x218[2];
	union REG_ISP_BNR_LSC_RATIO             LSC_RATIO;
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
	uint32_t                                _resv_0x258[106];
	union REG_ISP_BNR_LSC_EN                LSC_EN;
	uint32_t                                _resv_0x404[2];
	union REG_ISP_BNR_HSTR                  HSTR;
	union REG_ISP_BNR_HSIZE                 HSIZE;
	union REG_ISP_BNR_VSIZE                 VSIZE;
	union REG_ISP_BNR_X_CENTER              X_CENTER;
	union REG_ISP_BNR_Y_CENTER              Y_CENTER;
	union REG_ISP_BNR_NORM_FACTOR           NORM_FACTOR;
	union REG_ISP_BNR_LSC_LUT               LSC_LUT;
	union REG_ISP_BNR_LSC_STRENTH           LSC_STRENTH;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_MM_T {
	union REG_ISP_MM_00                     REG_00;
	union REG_ISP_MM_04                     REG_04;
	union REG_ISP_MM_08                     REG_08;
	union REG_ISP_MM_0C                     REG_0C;
	union REG_ISP_MM_10                     REG_10;
	union REG_ISP_MM_14                     REG_14;
	union REG_ISP_MM_18                     REG_18;
	union REG_ISP_MM_1C                     REG_1C;
	union REG_ISP_MM_20                     REG_20;
	union REG_ISP_MM_24                     REG_24;
	union REG_ISP_MM_28                     REG_28;
	union REG_ISP_MM_2C                     REG_2C;
	union REG_ISP_MM_30                     REG_30;
	union REG_ISP_MM_34                     REG_34;
	union REG_ISP_MM_38                     REG_38;
	union REG_ISP_MM_3C                     REG_3C;
	union REG_ISP_MM_40                     REG_40;
	union REG_ISP_MM_44                     REG_44;
	union REG_ISP_MM_48                     REG_48;
	union REG_ISP_MM_4C                     REG_4C;
	union REG_ISP_MM_50                     REG_50;
	union REG_ISP_MM_54                     REG_54;
	union REG_ISP_MM_58                     REG_58;
	union REG_ISP_MM_5C                     REG_5C;
	union REG_ISP_MM_60                     REG_60;
	union REG_ISP_MM_64                     REG_64;
	union REG_ISP_MM_68                     REG_68;
	union REG_ISP_MM_6C                     REG_6C;
	union REG_ISP_MM_70                     REG_70;
	union REG_ISP_MM_74                     REG_74;
	union REG_ISP_MM_78                     REG_78;
	union REG_ISP_MM_7C                     REG_7C;
	union REG_ISP_MM_80                     REG_80;
	union REG_ISP_MM_84                     REG_84;
	union REG_ISP_MM_88                     REG_88;
	union REG_ISP_MM_8C                     REG_8C;
	union REG_ISP_MM_90                     REG_90;
	uint32_t                                _resv_0x94[3];
	union REG_ISP_MM_A0                     REG_A0;
	union REG_ISP_MM_A4                     REG_A4;
	union REG_ISP_MM_A8                     REG_A8;
	union REG_ISP_MM_AC                     REG_AC;
	union REG_ISP_MM_B0                     REG_B0;
	union REG_ISP_MM_B4                     REG_B4;
	union REG_ISP_MM_B8                     REG_B8;
	union REG_ISP_MM_BC                     REG_BC;
	union REG_ISP_MM_C0                     REG_C0;
	union REG_ISP_MM_C4                     REG_C4;
	union REG_ISP_MM_C8                     REG_C8;
	union REG_ISP_MM_CC                     REG_CC;
	union REG_ISP_MM_D0                     REG_D0;
	union REG_ISP_MM_D4                     REG_D4;
	union REG_ISP_MM_D8                     REG_D8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_FPN_T {
	union REG_ISP_FPN_STATUS                STATUS;
	union REG_ISP_FPN_GRACE_RESET           GRACE_RESET;
	union REG_ISP_FPN_MONITOR               MONITOR;
	union REG_ISP_FPN_ENABLE                ENABLE;
	union REG_ISP_FPN_MAP_ENABLE            MAP_ENABLE;
	union REG_ISP_FPN_FLOW                  FLOW;
	uint32_t                                _resv_0x18[1];
	union REG_ISP_FPN_MONITOR_SELECT        MONITOR_SELECT;
	union REG_ISP_FPN_LOCATION              LOCATION;
	uint32_t                                _resv_0x24[8];
	union REG_ISP_FPN_MEM_SELECT            MEM_SELECT;
	union REG_ISP_FPN_SECTION_INFO_0        SECTION_INFO_0;
	union REG_ISP_FPN_SECTION_INFO_1        SECTION_INFO_1;
	union REG_ISP_FPN_SECTION_INFO_2        SECTION_INFO_2;
	union REG_ISP_FPN_SECTION_INFO_3        SECTION_INFO_3;
	uint32_t                                _resv_0x58[1];
	union REG_ISP_FPN_SECTION_INFO_4        SECTION_INFO_4;
	union REG_ISP_FPN_DEBUG                 DEBUG;
	union REG_ISP_FPN_IMG_BAYERID           IMG_BAYERID;
	union REG_ISP_FPN_DUMMY                 DUMMY;
	uint32_t                                _resv_0x6c[1];
	union REG_ISP_FPN_PROG_DATA             PROG_DATA;
	union REG_ISP_FPN_PROG_CTRL             PROG_CTRL;
	union REG_ISP_FPN_PROG_MAX              PROG_MAX;
	uint32_t                                _resv_0x7c[1];
	union REG_ISP_FPN_MEM_SW_MODE           SW_MODE;
	union REG_ISP_FPN_MEM_SW_RADDR          SW_RADDR;
	union REG_ISP_FPN_MEM_SW_RDATA_OFFSET   SW_RDATA_OFFSET;
	union REG_ISP_FPN_MEM_SW_RDATA_OFFSET_BG  SW_RDATA_OFFSET_BG;
	union REG_ISP_FPN_MEM_SW_RDATA_GAIN     SW_RDATA_GAIN;
	union REG_ISP_FPN_MEM_SW_RDATA_GAIN_BG  SW_RDATA_GAIN_BG;
	uint32_t                                _resv_0x98[2];
	union REG_ISP_FPN_DBG                   DBG;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGB_T {
	union REG_ISP_RGB_0                     REG_0;
	union REG_ISP_RGB_1                     REG_1;
	union REG_ISP_RGB_2                     REG_2;
	union REG_ISP_RGB_3                     REG_3;
	union REG_ISP_RGB_4                     REG_4;
	union REG_ISP_RGB_5                     REG_5;
	union REG_ISP_RGB_6                     REG_6;
	union REG_ISP_RGB_7                     REG_7;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CFA_T {
	union REG_ISP_CFA_0                     REG_0;
	union REG_ISP_CFA_1                     REG_1;
	union REG_ISP_CFA_2                     REG_2;
	union REG_ISP_CFA_3                     REG_3;
	union REG_ISP_CFA_4                     REG_4;
	union REG_ISP_CFA_4_1                   REG_4_1;
	union REG_ISP_CFA_5                     REG_5;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_CFA_6                     REG_6;
	union REG_ISP_CFA_7                     REG_7;
	union REG_ISP_CFA_8                     REG_8;
	union REG_ISP_CFA_9                     REG_9;
	union REG_ISP_CFA_GHP_LUT_0             GHP_LUT_0;
	union REG_ISP_CFA_GHP_LUT_1             GHP_LUT_1;
	union REG_ISP_CFA_GHP_LUT_2             GHP_LUT_2;
	union REG_ISP_CFA_GHP_LUT_3             GHP_LUT_3;
	union REG_ISP_CFA_GHP_LUT_4             GHP_LUT_4;
	union REG_ISP_CFA_GHP_LUT_5             GHP_LUT_5;
	union REG_ISP_CFA_GHP_LUT_6             GHP_LUT_6;
	union REG_ISP_CFA_GHP_LUT_7             GHP_LUT_7;
	uint32_t                                _resv_0x50[1];
	union REG_ISP_CFA_10                    REG_10;
	union REG_ISP_CFA_11                    REG_11;
	union REG_ISP_CFA_12                    REG_12;
	union REG_ISP_CFA_13                    REG_13;
	union REG_ISP_CFA_14                    REG_14;
	union REG_ISP_CFA_15                    REG_15;
	union REG_ISP_CFA_16                    REG_16;
	union REG_ISP_CFA_17                    REG_17;
	union REG_ISP_CFA_18                    REG_18;
	union REG_ISP_CFA_19                    REG_19;
	union REG_ISP_CFA_20                    REG_20;
	union REG_ISP_CFA_21                    REG_21;
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
struct REG_ISP_GAMMA_T {
	union REG_ISP_GAMMA_PROG_DATA           GAMMA_PROG_DATA;
	union REG_ISP_GAMMA_PROG_CTRL           GAMMA_PROG_CTRL;
	union REG_ISP_GAMMA_PROG_MAX            GAMMA_PROG_MAX;
	union REG_ISP_GAMMA_CTRL                GAMMA_CTRL;
	union REG_ISP_GAMMA_MEM_SW_MODE         GAMMA_SW_MODE;
	union REG_ISP_GAMMA_MEM_SW_RADDR        GAMMA_SW_RADDR;
	union REG_ISP_GAMMA_MEM_SW_RDATA        GAMMA_SW_RDATA;
	union REG_ISP_GAMMA_MEM_SW_RDATA_BG     GAMMA_SW_RDATA_BG;
	union REG_ISP_GAMMA_DBG                 GAMMA_DBG;
	union REG_ISP_GAMMA_DMY0                GAMMA_DMY0;
	union REG_ISP_GAMMA_DMY1                GAMMA_DMY1;
	union REG_ISP_GAMMA_DMY_R               GAMMA_DMY_R;
	union REG_ISP_GAMMA_PROG_DATA_G         GAMMA_PROG_DATA_G;
	union REG_ISP_GAMMA_PROG_CTRL_G         GAMMA_PROG_CTRL_G;
	union REG_ISP_GAMMA_PROG_MAX_G          GAMMA_PROG_MAX_G;
	uint32_t                                _resv_0x3c[1];
	union REG_ISP_GAMMA_PROG_DATA_B         GAMMA_PROG_DATA_B;
	union REG_ISP_GAMMA_PROG_CTRL_B         GAMMA_PROG_CTRL_B;
	union REG_ISP_GAMMA_PROG_MAX_B          GAMMA_PROG_MAX_B;
	uint32_t                                _resv_0x4c[1];
	union REG_ISP_GAMMA_PROG_ST_ADDR        GAMMA_PROG_ST_ADDR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_HSV_T {
	union REG_ISP_HSV_0                     HSV_0;
	union REG_ISP_HSV_DEBUG_0               HSV_DEBUG_0;
	union REG_ISP_HSV_DEBUG_1               HSV_DEBUG_1;
	union REG_ISP_HSV_1                     HSV_1;
	union REG_ISP_HSV_2                     HSV_2;
	union REG_ISP_HSV_3                     HSV_3;
	union REG_ISP_HSV_4                     HSV_4;
	union REG_ISP_HSV_5                     HSV_5;
	union REG_ISP_HSV_6                     HSV_6;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_DHZ_T {
	union REG_ISP_DHZ_DEHAZE_PARA           DEHAZE_PARA;
	uint32_t                                _resv_0x4[4];
	union REG_ISP_DHZ_BYPASS                DHZ_BYPASS;
	union REG_ISP_DHZ_0                     REG_0;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_DHZ_1                     REG_1;
	union REG_ISP_DHZ_2                     REG_2;
	union REG_ISP_DHZ_3                     REG_3;
	uint32_t                                _resv_0x2c[1];
	union REG_ISP_DHZ_4                     REG_4;
	union REG_ISP_DHZ_5                     REG_5;
	union REG_ISP_DHZ_6                     REG_6;
	union REG_ISP_DHZ_7                     REG_7;
	union REG_ISP_DHZ_8                     REG_8;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CSC_T {
	union REG_ISP_CSC_0                     REG_0;
	union REG_ISP_CSC_1                     REG_1;
	union REG_ISP_CSC_2                     REG_2;
	union REG_ISP_CSC_3                     REG_3;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGBDITHER_T {
	union REG_ISP_RGBDITHER_RGB_DITHER      RGB_DITHER;
	union REG_ISP_RGBDITHER_RGB_DITHER_DEBUG0  RGB_DITHER_DEBUG0;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGBEE_T {
	union REG_ISP_RGBEE_0                   REG_0;
	union REG_ISP_RGBEE_1                   REG_1;
	union REG_ISP_RGBEE_2                   REG_2;
	union REG_ISP_RGBEE_AC_LUT_0            AC_LUT_0;
	union REG_ISP_RGBEE_AC_LUT_1            AC_LUT_1;
	union REG_ISP_RGBEE_AC_LUT_2            AC_LUT_2;
	union REG_ISP_RGBEE_AC_LUT_3            AC_LUT_3;
	union REG_ISP_RGBEE_AC_LUT_4            AC_LUT_4;
	union REG_ISP_RGBEE_AC_LUT_5            AC_LUT_5;
	union REG_ISP_RGBEE_AC_LUT_6            AC_LUT_6;
	union REG_ISP_RGBEE_AC_LUT_7            AC_LUT_7;
	union REG_ISP_RGBEE_AC_LUT_8            AC_LUT_8;
	union REG_ISP_RGBEE_EDGE_LUT_0          EDGE_LUT_0;
	union REG_ISP_RGBEE_EDGE_LUT_1          EDGE_LUT_1;
	union REG_ISP_RGBEE_EDGE_LUT_2          EDGE_LUT_2;
	union REG_ISP_RGBEE_EDGE_LUT_3          EDGE_LUT_3;
	union REG_ISP_RGBEE_EDGE_LUT_4          EDGE_LUT_4;
	union REG_ISP_RGBEE_EDGE_LUT_5          EDGE_LUT_5;
	union REG_ISP_RGBEE_EDGE_LUT_6          EDGE_LUT_6;
	union REG_ISP_RGBEE_EDGE_LUT_7          EDGE_LUT_7;
	union REG_ISP_RGBEE_EDGE_LUT_8          EDGE_LUT_8;
	union REG_ISP_RGBEE_NP_LUT_0            NP_LUT_0;
	union REG_ISP_RGBEE_NP_LUT_1            NP_LUT_1;
	union REG_ISP_RGBEE_NP_LUT_2            NP_LUT_2;
	union REG_ISP_RGBEE_NP_LUT_3            NP_LUT_3;
	union REG_ISP_RGBEE_NP_LUT_4            NP_LUT_4;
	union REG_ISP_RGBEE_NP_LUT_5            NP_LUT_5;
	union REG_ISP_RGBEE_NP_LUT_6            NP_LUT_6;
	union REG_ISP_RGBEE_NP_LUT_7            NP_LUT_7;
	union REG_ISP_RGBEE_NP_LUT_8            NP_LUT_8;
	union REG_ISP_RGBEE_NP_LUT_9            NP_LUT_9;
	union REG_ISP_RGBEE_NP_LUT_10           NP_LUT_10;
	union REG_ISP_RGBEE_NP_LUT_11           NP_LUT_11;
	union REG_ISP_RGBEE_NP_LUT_12           NP_LUT_12;
	union REG_ISP_RGBEE_NP_LUT_13           NP_LUT_13;
	union REG_ISP_RGBEE_NP_LUT_14           NP_LUT_14;
	union REG_ISP_RGBEE_NP_LUT_15           NP_LUT_15;
	union REG_ISP_RGBEE_NP_LUT_16           NP_LUT_16;
	union REG_ISP_RGBEE_4                   REG_4;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_YUV_TOP_T {
	union REG_YUV_TOP_YUV_0                 YUV_0;
	union REG_YUV_TOP_YUV_1                 YUV_1;
	union REG_YUV_TOP_YUV_2                 YUV_2;
	union REG_YUV_TOP_YUV_3                 YUV_3;
	union REG_YUV_TOP_YUV_DEBUG_0           YUV_DEBUG_0;
	union REG_YUV_TOP_YUV_4                 YUV_4;
	union REG_YUV_TOP_YUV_DEBUG_STATE       YUV_DEBUG_STATE;
	uint32_t                                _resv_0x1c[1];
	union REG_YUV_TOP_YUV_5                 YUV_5;
	uint32_t                                _resv_0x24[7];
	union REG_YUV_TOP_HSV_LUT_PROG_SRAM0    HSV_LUT_PROG_SRAM0;
	union REG_YUV_TOP_HSV_LUT_PROG_SRAM1    HSV_LUT_PROG_SRAM1;
	union REG_YUV_TOP_HSV_LUT_READ_SRAM0    HSV_LUT_READ_SRAM0;
	union REG_YUV_TOP_HSV_LUT_READ_SRAM1    HSV_LUT_READ_SRAM1;
	union REG_YUV_TOP_HSV_LUT_CONTROL       HSV_LUT_CONTROL;
	uint32_t                                _resv_0x54[2];
	union REG_YUV_TOP_HSV_LUT_STATUS        HSV_LUT_STATUS;
	union REG_YUV_TOP_HSV_ENABLE            HSV_ENABLE;
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
	union REG_ISP_444_422_7                 REG_7;
	union REG_ISP_444_422_8                 REG_8;
	union REG_ISP_444_422_9                 REG_9;
	union REG_ISP_444_422_10                REG_10;
	union REG_ISP_444_422_11                REG_11;
	union REG_ISP_444_422_12                REG_12;
	union REG_ISP_444_422_13                REG_13;
	union REG_ISP_444_422_14                REG_14;
	union REG_ISP_444_422_15                REG_15;
	union REG_ISP_444_422_16                REG_16;
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
	uint32_t                                _resv_0xc[61];
	union REG_ISP_YNR_NS0_LUMA_TH           NS0_LUMA_TH;
	union REG_ISP_YNR_NS0_SLOPE             NS0_SLOPE;
	union REG_ISP_YNR_NS0_OFFSET            NS0_OFFSET;
	uint32_t                                _resv_0x10c[1];
	union REG_ISP_YNR_NS1_LUMA_TH           NS1_LUMA_TH;
	union REG_ISP_YNR_NS1_SLOPE             NS1_SLOPE;
	union REG_ISP_YNR_NS1_OFFSET            NS1_OFFSET;
	uint32_t                                _resv_0x11c[1];
	union REG_ISP_YNR_MOTION_NS_TH          MOTION_NS_TH;
	union REG_ISP_YNR_MOTION_POS_GAIN       MOTION_POS_GAIN;
	union REG_ISP_YNR_MOTION_NEG_GAIN       MOTION_NEG_GAIN;
	union REG_ISP_YNR_NS_GAIN               NS_GAIN;
	union REG_ISP_YNR_STRENGTH_MODE         STRENGTH_MODE;
	union REG_ISP_YNR_INTENSITY_SEL         INTENSITY_SEL;
	uint32_t                                _resv_0x138[50];
	union REG_ISP_YNR_WEIGHT_INTRA_0        WEIGHT_INTRA_0;
	union REG_ISP_YNR_WEIGHT_INTRA_1        WEIGHT_INTRA_1;
	union REG_ISP_YNR_WEIGHT_INTRA_2        WEIGHT_INTRA_2;
	uint32_t                                _resv_0x20c[1];
	union REG_ISP_YNR_WEIGHT_NORM_1         WEIGHT_NORM_1;
	union REG_ISP_YNR_WEIGHT_NORM_2         WEIGHT_NORM_2;
	uint32_t                                _resv_0x218[2];
	union REG_ISP_YNR_ALPHA_GAIN            ALPHA_GAIN;
	union REG_ISP_YNR_VAR_TH                VAR_TH;
	union REG_ISP_YNR_WEIGHT_LUT            WEIGHT_LUT;
	union REG_ISP_YNR_WEIGHT_SM             WEIGHT_SM;
	union REG_ISP_YNR_WEIGHT_V              WEIGHT_V;
	union REG_ISP_YNR_WEIGHT_H              WEIGHT_H;
	union REG_ISP_YNR_WEIGHT_D45            WEIGHT_D45;
	union REG_ISP_YNR_WEIGHT_D135           WEIGHT_D135;
	union REG_ISP_YNR_NEIGHBOR_MAX          NEIGHBOR_MAX;
	uint32_t                                _resv_0x244[3];
	union REG_ISP_YNR_RES_K_SMOOTH          RES_K_SMOOTH;
	union REG_ISP_YNR_RES_K_TEXTURE         RES_K_TEXTURE;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_CNR_T {
	union REG_ISP_CNR_00                    CNR_00;
	union REG_ISP_CNR_01                    CNR_01;
	union REG_ISP_CNR_02                    CNR_02;
	union REG_ISP_CNR_03                    CNR_03;
	union REG_ISP_CNR_04                    CNR_04;
	union REG_ISP_CNR_05                    CNR_05;
	union REG_ISP_CNR_06                    CNR_06;
	union REG_ISP_CNR_07                    CNR_07;
	union REG_ISP_CNR_08                    CNR_08;
	union REG_ISP_CNR_09                    CNR_09;
	union REG_ISP_CNR_10                    CNR_10;
	union REG_ISP_CNR_11                    CNR_11;
	union REG_ISP_CNR_12                    CNR_12;
	union REG_ISP_CNR_13                    CNR_13;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_EE_T {
	union REG_ISP_EE_00                     REG_00;
	union REG_ISP_EE_04                     REG_04;
	union REG_ISP_EE_08                     REG_08;
	union REG_ISP_EE_0C                     REG_0C;
	union REG_ISP_EE_10                     REG_10;
	union REG_ISP_EE_14                     REG_14;
	union REG_ISP_EE_18                     REG_18;
	union REG_ISP_EE_1C                     REG_1C;
	union REG_ISP_EE_20                     REG_20;
	union REG_ISP_EE_24                     REG_24;
	union REG_ISP_EE_28                     REG_28;
	union REG_ISP_EE_2C                     REG_2C;
	union REG_ISP_EE_30                     REG_30;
	union REG_ISP_EE_34                     REG_34;
	union REG_ISP_EE_38                     REG_38;
	union REG_ISP_EE_3C                     REG_3C;
	union REG_ISP_EE_40                     REG_40;
	uint32_t                                _resv_0x44[5];
	union REG_ISP_EE_58                     REG_58;
	union REG_ISP_EE_5C                     REG_5C;
	union REG_ISP_EE_60                     REG_60;
	union REG_ISP_EE_64                     REG_64;
	union REG_ISP_EE_68                     REG_68;
	union REG_ISP_EE_6C                     REG_6C;
	union REG_ISP_EE_70                     REG_70;
	union REG_ISP_EE_74                     REG_74;
	union REG_ISP_EE_78                     REG_78;
	union REG_ISP_EE_7C                     REG_7C;
	union REG_ISP_EE_80                     REG_80;
	union REG_ISP_EE_84                     REG_84;
	union REG_ISP_EE_88                     REG_88;
	union REG_ISP_EE_8C                     REG_8C;
	union REG_ISP_EE_90                     REG_90;
	union REG_ISP_EE_94                     REG_94;
	union REG_ISP_EE_98                     REG_98;
	union REG_ISP_EE_9C                     REG_9C;
	uint32_t                                _resv_0xa0[1];
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
	union REG_ISP_EE_CC                     REG_CC;
	union REG_ISP_EE_D0                     REG_D0;
	union REG_ISP_EE_D4                     REG_D4;
	union REG_ISP_EE_D8                     REG_D8;
	union REG_ISP_EE_DC                     REG_DC;
	union REG_ISP_EE_E0                     REG_E0;
	union REG_ISP_EE_E4                     REG_E4;
	union REG_ISP_EE_E8                     REG_E8;
	union REG_ISP_EE_EC                     REG_EC;
	union REG_ISP_EE_F0                     REG_F0;
	union REG_ISP_EE_F4                     REG_F4;
	union REG_ISP_EE_F8                     REG_F8;
	union REG_ISP_EE_FC                     REG_FC;
	union REG_ISP_EE_100                    REG_100;
	union REG_ISP_EE_104                    REG_104;
	union REG_ISP_EE_108                    REG_108;
	union REG_ISP_EE_10C                    REG_10C;
	union REG_ISP_EE_110                    REG_110;
	union REG_ISP_EE_114                    REG_114;
	union REG_ISP_EE_118                    REG_118;
	union REG_ISP_EE_11C                    REG_11C;
	union REG_ISP_EE_120                    REG_120;
	union REG_ISP_EE_124                    REG_124;
	union REG_ISP_EE_128                    REG_128;
	union REG_ISP_EE_12C                    REG_12C;
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
	union REG_ISP_EE_1AC                    REG_1AC;
	union REG_ISP_EE_1B0                    REG_1B0;
	union REG_ISP_EE_1B4                    REG_1B4;
	union REG_ISP_EE_1B8                    REG_1B8;
	union REG_ISP_EE_1BC                    REG_1BC;
	union REG_ISP_EE_1C0                    REG_1C0;
	union REG_ISP_EE_1C4                    REG_1C4;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_YCUR_T {
	union REG_ISP_YCUR_PROG_DATA            YCUR_PROG_DATA;
	union REG_ISP_YCUR_PROG_CTRL            YCUR_PROG_CTRL;
	union REG_ISP_YCUR_PROG_MAX             YCUR_PROG_MAX;
	union REG_ISP_YCUR_CTRL                 YCUR_CTRL;
	union REG_ISP_YCUR_MEM_SW_MODE          YCUR_SW_MODE;
	union REG_ISP_YCUR_MEM_SW_RADDR         YCUR_SW_RADDR;
	union REG_ISP_YCUR_MEM_SW_RDATA         YCUR_SW_RDATA;
	uint32_t                                _resv_0x1c[1];
	union REG_ISP_YCUR_DBG                  YCUR_DBG;
	union REG_ISP_YCUR_DMY0                 YCUR_DMY0;
	union REG_ISP_YCUR_DMY1                 YCUR_DMY1;
	union REG_ISP_YCUR_DMY_R                YCUR_DMY_R;
	union REG_ISP_YCUR_PROG_ST_ADDR         YCUR_PROG_ST_ADDR;
	union REG_ISP_YCUR_RGB_DITHER           RGB_DITHER;
	union REG_ISP_YCUR_00                   REG_00;
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
	uint32_t                                _resv_0x5c[41];
	union REG_ISP_DCI_MAPPED_LUT            DCI_MAPPED_LUT;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_TOP_T {
	union REG_ISP_TOP_0                     REG_0;
	union REG_ISP_TOP_1                     REG_1;
	union REG_ISP_TOP_2                     REG_2;
	union REG_ISP_TOP_3                     REG_3;
	union REG_ISP_TOP_4                     REG_4;
	union REG_ISP_TOP_5                     REG_5;
	union REG_ISP_TOP_6                     REG_6;
	union REG_ISP_TOP_7                     REG_7;
	uint32_t                                _resv_0x20[4];
	union REG_ISP_TOP_C                     REG_C;
	union REG_ISP_TOP_D                     REG_D;
	uint32_t                                _resv_0x38[2];
	union REG_ISP_TOP_10                    REG_10;
	union REG_ISP_TOP_11                    REG_11;
	union REG_ISP_TOP_12                    REG_12;
	union REG_ISP_TOP_13                    REG_13;
	union REG_ISP_TOP_14                    REG_14;
	union REG_ISP_TOP_15                    REG_15;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_PCHK_T {
	union REG_ISP_PCHK_SHADOW_RD_SEL        SHADOW_RD_SEL;
	union REG_ISP_PCHK_IN_SEL               IN_SEL;
	union REG_ISP_PCHK_RULE_EN              RULE_EN;
	uint32_t                                _resv_0xc[1];
	union REG_ISP_PCHK_HSIZE                HSIZE;
	union REG_ISP_PCHK_VSIZE                VSIZE;
	union REG_ISP_PCHK_NRDY_LIMIT           NRDY_LIMIT;
	union REG_ISP_PCHK_NREQ_LIMIT           NREQ_LIMIT;
	union REG_ISP_PCHK_PFREQ_LIMIT          PFREQ_LIMIT;
	uint32_t                                _resv_0x24[55];
	union REG_ISP_PCHK_ERR_BUS              ERR_BUS;
	union REG_ISP_PCHK_ERR_XY               ERR_XY;
	uint32_t                                _resv_0x108[62];
	union REG_ISP_PCHK_ERR_CLR              ERR_CLR;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_ISP_RGB_DITHER_T {
	union REG_ISP_RGB_DITHER_RGB_DITHER     RGB_DITHER;
	union REG_ISP_RGB_DITHER_RGB_DITHER_DEBUG0  RGB_DITHER_DEBUG0;
};

#endif // _REG_BLOCKS_H_
