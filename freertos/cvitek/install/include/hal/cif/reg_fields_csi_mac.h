#ifndef _REG_FIELDS_CSI_MAC_H_
#define _REG_FIELDS_CSI_MAC_H_

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_MAC_00 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_MODE                 : 3;
		uint32_t BT_DEMUX_ENABLE                 : 1;
		uint32_t CSI_CTRL_ENABLE                 : 1;
		uint32_t CSI_VS_INV                      : 1;
		uint32_t CSI_HS_INV                      : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t SUBLVDS_CTRL_ENABLE             : 1;
		uint32_t SUBLVDS_VS_INV                  : 1;
		uint32_t SUBLVDS_HS_INV                  : 1;
		uint32_t SUBLVDS_HDR_INV                 : 1;
		uint32_t SLVSEC_CTRL_ENABLE              : 1;
		uint32_t SLVSEC_VS_INV                   : 1;
		uint32_t SLVSEC_HS_INV                   : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t MASK_UP                         : 1;
		uint32_t SHRD_SEL                        : 1;
		uint32_t SW_UP                           : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t DBG_SEL                         : 8;
	} bits;
};

union REG_SENSOR_MAC_10 {
	uint32_t raw;
	struct {
		uint32_t TTL_IP_EN                       : 1;
		uint32_t TTL_SENSOR_BIT                  : 2;
		uint32_t _rsv_3                          : 1;
		uint32_t TTL_BT_FMT_OUT                  : 2;
		uint32_t _rsv_6                          : 2;
		uint32_t TTL_FMT_IN                      : 4;
		uint32_t TTL_BT_DATA_SEQ                 : 2;
		uint32_t TTL_VS_INV                      : 1;
		uint32_t TTL_HS_INV                      : 1;
	} bits;
};

union REG_SENSOR_MAC_14 {
	uint32_t raw;
	struct {
		uint32_t TTL_VS_BP                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_HS_BP                       : 12;
	} bits;
};

union REG_SENSOR_MAC_18 {
	uint32_t raw;
	struct {
		uint32_t TTL_IMG_WD                      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_IMG_HT                      : 12;
	} bits;
};

union REG_SENSOR_MAC_1C {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_0                      : 16;
		uint32_t TTL_SYNC_1                      : 16;
	} bits;
};

union REG_SENSOR_MAC_20 {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_2                      : 16;
	} bits;
};

union REG_SENSOR_MAC_24 {
	uint32_t raw;
	struct {
		uint32_t TTL_SAV_VLD                     : 16;
		uint32_t TTL_SAV_BLK                     : 16;
	} bits;
};

union REG_SENSOR_MAC_28 {
	uint32_t raw;
	struct {
		uint32_t TTL_EAV_VLD                     : 16;
		uint32_t TTL_EAV_BLK                     : 16;
	} bits;
};

union REG_SENSOR_MAC_30 {
	uint32_t raw;
	struct {
		uint32_t VI_SEL                          : 3;
		uint32_t VI_FROM                         : 1;
		uint32_t VI_CLK_INV                      : 1;
		uint32_t VI_V_SEL_VS                     : 1;
		uint32_t VI_VS_DBG                       : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t PAD_VI0_CLK_INV                 : 1;
		uint32_t PAD_VI1_CLK_INV                 : 1;
		uint32_t PAD_VI2_CLK_INV                 : 1;
	} bits;
};

union REG_SENSOR_MAC_34 {
	uint32_t raw;
	struct {
		uint32_t VI_VS_DLY                       : 5;
		uint32_t _rsv_5                          : 1;
		uint32_t VI_VS_DLY_EN                    : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t VI_HS_DLY                       : 5;
		uint32_t _rsv_13                         : 1;
		uint32_t VI_HS_DLY_EN                    : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t VI_VDE_DLY                      : 5;
		uint32_t _rsv_21                         : 1;
		uint32_t VI_VDE_DLY_EN                   : 1;
		uint32_t _rsv_23                         : 1;
		uint32_t VI_HDE_DLY                      : 5;
		uint32_t _rsv_29                         : 1;
		uint32_t VI_HDE_DLY_EN                   : 1;
	} bits;
};

union REG_SENSOR_MAC_40 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_EN               : 1;
		uint32_t SENSOR_MAC_HDR_VSINV            : 1;
		uint32_t SENSOR_MAC_HDR_HSINV            : 1;
		uint32_t SENSOR_MAC_HDR_DEINV            : 1;
		uint32_t SENSOR_MAC_HDR_HDR0INV          : 1;
		uint32_t SENSOR_MAC_HDR_HDR1INV          : 1;
		uint32_t SENSOR_MAC_HDR_BLCINV           : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t SENSOR_MAC_HDR_MODE             : 1;
	} bits;
};

union REG_SENSOR_MAC_44 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_SHIFT            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_HDR_VSIZE            : 13;
	} bits;
};

union REG_SENSOR_MAC_48 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_INFO_LINE_NUM        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_RM_INFO_LINE         : 1;
	} bits;
};

union REG_SENSOR_MAC_4C {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_LINE_CNT         : 14;
	} bits;
};

union REG_SENSOR_MAC_50 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_BLC0_EN              : 1;
		uint32_t SENSOR_MAC_BLC1_EN              : 1;
	} bits;
};

union REG_SENSOR_MAC_54 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_BLC0_START           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_BLC0_SIZE            : 13;
	} bits;
};

union REG_SENSOR_MAC_58 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_BLC1_START           : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_BLC1_SIZE            : 13;
	} bits;
};

union REG_SENSOR_MAC_60 {
	uint32_t raw;
	struct {
		uint32_t VI_VS_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_HS_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_VDE_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_HDE_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_64 {
	uint32_t raw;
	struct {
		uint32_t VI_D0_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D1_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D2_SEL                       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D3_SEL                       : 6;
	} bits;
};

union REG_SENSOR_MAC_68 {
	uint32_t raw;
	struct {
		uint32_t VI_D4_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D5_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D6_SEL                       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D7_SEL                       : 6;
	} bits;
};

union REG_SENSOR_MAC_6C {
	uint32_t raw;
	struct {
		uint32_t VI_D8_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D9_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D10_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D11_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_70 {
	uint32_t raw;
	struct {
		uint32_t VI_D12_SEL                      : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D13_SEL                      : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D14_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D15_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_74 {
	uint32_t raw;
	struct {
		uint32_t VI_BT_D0_SEL                    : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t VI_BT_D1_SEL                    : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t VI_BT_D2_SEL                    : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t VI_BT_D3_SEL                    : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t VI_BT_D4_SEL                    : 3;
		uint32_t _rsv_19                         : 1;
		uint32_t VI_BT_D5_SEL                    : 3;
		uint32_t _rsv_23                         : 1;
		uint32_t VI_BT_D6_SEL                    : 3;
		uint32_t _rsv_27                         : 1;
		uint32_t VI_BT_D7_SEL                    : 3;
	} bits;
};

union REG_SENSOR_MAC_80 {
	uint32_t raw;
	struct {
		uint32_t BT_CLR_SYNC_LOST_1T             : 1;
		uint32_t BT_IP_EN                        : 1;
		uint32_t BT_DDR_MODE                     : 1;
		uint32_t BT_HS_GATE_BY_VDE               : 1;
		uint32_t BT_VS_INV                       : 1;
		uint32_t BT_HS_INV                       : 1;
		uint32_t BT_VS_AS_VDE                    : 1;
		uint32_t BT_HS_AS_HDE                    : 1;
		uint32_t BT_SW_EN_CLK                    : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t BT_DEMUX_CH                     : 2;
		uint32_t _rsv_18                         : 2;
		uint32_t BT_FMT_SEL                      : 3;
		uint32_t _rsv_23                         : 1;
		uint32_t BT_SYNC_LOST                    : 1;
	} bits;
};

union REG_SENSOR_MAC_84 {
	uint32_t raw;
	struct {
		uint32_t BT_V_CTRL_DLY                   : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t BT_H_CTRL_DLY                   : 5;
	} bits;
};

union REG_SENSOR_MAC_88 {
	uint32_t raw;
	struct {
		uint32_t BT_IMG_WD_M1                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BT_IMG_HT_M1                    : 12;
	} bits;
};

union REG_SENSOR_MAC_8C {
	uint32_t raw;
	struct {
		uint32_t BT_VS_BP_M1                     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BT_HS_BP_M1                     : 12;
	} bits;
};

union REG_SENSOR_MAC_90 {
	uint32_t raw;
	struct {
		uint32_t BT_VS_FP_M1                     : 8;
		uint32_t BT_HS_FP_M1                     : 8;
	} bits;
};

union REG_SENSOR_MAC_94 {
	uint32_t raw;
	struct {
		uint32_t BT_SYNC_0                       : 8;
		uint32_t BT_SYNC_1                       : 8;
		uint32_t BT_SYNC_2                       : 8;
	} bits;
};

union REG_SENSOR_MAC_98 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_0                    : 8;
		uint32_t BT_SAV_BLK_0                    : 8;
		uint32_t BT_EAV_VLD_0                    : 8;
		uint32_t BT_EAV_BLK_0                    : 8;
	} bits;
};

union REG_SENSOR_MAC_9C {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_1                    : 8;
		uint32_t BT_SAV_BLK_1                    : 8;
		uint32_t BT_EAV_VLD_1                    : 8;
		uint32_t BT_EAV_BLK_1                    : 8;
	} bits;
};

union REG_SENSOR_MAC_A0 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_2                    : 8;
		uint32_t BT_SAV_BLK_2                    : 8;
		uint32_t BT_EAV_VLD_2                    : 8;
		uint32_t BT_EAV_BLK_2                    : 8;
	} bits;
};

union REG_SENSOR_MAC_A4 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_3                    : 8;
		uint32_t BT_SAV_BLK_3                    : 8;
		uint32_t BT_EAV_VLD_3                    : 8;
		uint32_t BT_EAV_BLK_3                    : 8;
	} bits;
};

union REG_SENSOR_MAC_A8 {
	uint32_t raw;
	struct {
		uint32_t BT_YC_INV                       : 4;
	} bits;
};

union REG_SENSOR_MAC_B0 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_CROP_START_X         : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_CROP_END_X           : 13;
		uint32_t _rsv_29                         : 2;
		uint32_t SENSOR_MAC_CROP_EN              : 1;
	} bits;
};

union REG_SENSOR_MAC_B4 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_CROP_START_Y         : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_CROP_END_Y           : 13;
	} bits;
};

union REG_SENSOR_MAC_B8 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_SWAPUV_EN            : 1;
		uint32_t SENSOR_MAC_SWAPYC_EN            : 1;
	} bits;
};

union REG_SENSOR_MAC_BC {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_HTOTAL_MAX       : 16;
		uint32_t SENSOR_MAC_DBG_EN               : 1;
	} bits;
};

union REG_SENSOR_MAC_C0 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_VTOTAL_MAX       : 32;
	} bits;
};

union REG_SENSOR_MAC_C4 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_HTOTAL           : 16;
	} bits;
};

union REG_SENSOR_MAC_C8 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_VTOTAL           : 32;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SUBLVDS_CTRL_TOP_00 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_ENABLE                    : 8;
		uint32_t SLVDS_BIT_MODE                  : 2;
		uint32_t SLVDS_DATA_REVERSE              : 1;
		uint32_t _rsv_11                         : 1;
		uint32_t SLVDS_HDR_MODE                  : 1;
		uint32_t SLVDS_HDR_PATTERN               : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t SLVDS_VFPORCH                   : 10;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_04 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_SYNC_1ST                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_SYNC_2ND                  : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_08 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_SYNC_3RD                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_NORM_BK_SAV               : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_0C {
	uint32_t raw;
	struct {
		uint32_t SLVDS_NORM_BK_EAV               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_NORM_SAV                  : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_NORM_EAV                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N0_BK_SAV                 : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_14 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N0_BK_EAV                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N1_BK_SAV                 : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_18 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N1_BK_EAV                 : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N0_LEF_SAV                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_1C {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N0_LEF_EAV                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N0_SEF_SAV                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_20 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N0_SEF_EAV                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N1_LEF_SAV                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_24 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N1_LEF_EAV                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N1_SEF_SAV                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_28 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N1_SEF_EAV                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_2C {
	uint32_t raw;
	struct {
		uint32_t VS_GEN_SYNC_CODE                : 12;
		uint32_t VS_GEN_BY_SYNC_CODE             : 1;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_30 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_LANE_MODE                 : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t SLVDS_SYNC_SOURCE               : 8;
		uint32_t SLVDS_FIFO_CLR                  : 1;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_40 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_FIFO_FULL                 : 1;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_50 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N0_LSEF_SAV               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N0_LSEF_EAV               : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_54 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_N1_LSEF_SAV               : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_N1_LSEF_EAV               : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_58 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_HDR_P2_HSIZE              : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t SLVDS_HDR_P2_HBLANK             : 14;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_60 {
	uint32_t raw;
	struct {
		uint32_t HISPI_MODE                      : 1;
		uint32_t HISPI_USE_HSIZE                 : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t HISPI_HDR_PSP_MODE              : 1;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_64 {
	uint32_t raw;
	struct {
		uint32_t HISPI_NORM_SOF                  : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISPI_NORM_EOF                  : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_68 {
	uint32_t raw;
	struct {
		uint32_t HISPI_HDR_T1_SOF                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISPI_HDR_T1_EOF                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_6C {
	uint32_t raw;
	struct {
		uint32_t HISPI_HDR_T1_SOL                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISPI_HDR_T1_EOL                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_70 {
	uint32_t raw;
	struct {
		uint32_t HISPI_HDR_T2_SOF                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISPI_HDR_T2_EOF                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_74 {
	uint32_t raw;
	struct {
		uint32_t HISPI_HDR_T2_SOL                : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t HISPI_HDR_T2_EOL                : 12;
	} bits;
};

union REG_SUBLVDS_CTRL_TOP_80 {
	uint32_t raw;
	struct {
		uint32_t DBG_SEL                         : 8;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_CSI_CTRL_TOP_00 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_MODE                   : 3;
		uint32_t CSI_IGNORE_ECC                  : 1;
		uint32_t CSI_VC_CHECK                    : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CSI_VC_SET                      : 4;
		uint32_t CSI_LINE_START_SENT             : 1;
		uint32_t CSI_FORMAT_FRC                  : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t CSI_FORMAT_SET                  : 8;
	} bits;
};

union REG_CSI_CTRL_TOP_04 {
	uint32_t raw;
	struct {
		uint32_t CSI_INTR_MASK                   : 8;
		uint32_t CSI_INTR_CLR                    : 8;
		uint32_t CSI_HDR_EN                      : 1;
		uint32_t CSI_HDR_MODE                    : 1;
		uint32_t CSI_ID_RM_ELSE                  : 1;
		uint32_t CSI_ID_RM_OB                    : 1;
	} bits;
};

union REG_CSI_CTRL_TOP_08 {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_OB_LEF                   : 16;
		uint32_t CSI_N0_OB_SEF                   : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_0C {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_LEF                      : 16;
		uint32_t CSI_N1_OB_LEF                   : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t CSI_N1_OB_SEF                   : 16;
		uint32_t CSI_N1_LEF                      : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_14 {
	uint32_t raw;
	struct {
		uint32_t CSI_BLC_DT                      : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t CSI_BLC_EN                      : 1;
		uint32_t _rsv_9                          : 3;
		uint32_t CSI_BLC_FORMAT_SET              : 3;
	} bits;
};

union REG_CSI_CTRL_TOP_18 {
	uint32_t raw;
	struct {
		uint32_t CSI_VC_MAP_CH00                 : 4;
		uint32_t CSI_VC_MAP_CH01                 : 4;
		uint32_t CSI_VC_MAP_CH10                 : 4;
		uint32_t CSI_VC_MAP_CH11                 : 4;
	} bits;
};

union REG_CSI_CTRL_TOP_1C {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_SEF                      : 16;
		uint32_t CSI_N1_SEF                      : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_20 {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_SEF2                     : 16;
		uint32_t CSI_N1_SEF2                     : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_24 {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_OB_SEF2                  : 16;
		uint32_t CSI_N1_OB_SEF2                  : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_30 {
	uint32_t raw;
	struct {
		uint32_t CSI_ECC_PH_DBG                  : 32;
	} bits;
};

union REG_CSI_CTRL_TOP_34 {
	uint32_t raw;
	struct {
		uint32_t CSI_ECC_CLR_PH_DBG              : 1;
	} bits;
};

union REG_CSI_CTRL_TOP_40 {
	uint32_t raw;
	struct {
		uint32_t CSI_ECC_NO_ERROR                : 1;
		uint32_t CSI_ECC_CORRECTED_ERROR         : 1;
		uint32_t CSI_ECC_ERROR                   : 1;
		uint32_t _rsv_3                          : 1;
		uint32_t CSI_CRC_ERROR                   : 1;
		uint32_t CSI_WC_ERROR                    : 1;
		uint32_t _rsv_6                          : 2;
		uint32_t CSI_FIFO_FULL                   : 1;
		uint32_t _rsv_9                          : 7;
		uint32_t CSI_DECODE_FORMAT               : 6;
	} bits;
};

union REG_CSI_CTRL_TOP_48 {
	uint32_t raw;
	struct {
		uint32_t SHORT_PKT_RSV1                  : 16;
		uint32_t SHORT_PKT_RSV2                  : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_4C {
	uint32_t raw;
	struct {
		uint32_t SHORT_PKT_RSV3                  : 16;
		uint32_t SHORT_PKT_RSV4                  : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_50 {
	uint32_t raw;
	struct {
		uint32_t GENERIC_SHORT_PKT1              : 16;
		uint32_t GENERIC_SHORT_PKT2              : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_54 {
	uint32_t raw;
	struct {
		uint32_t GENERIC_SHORT_PKT3              : 16;
		uint32_t GENERIC_SHORT_PKT4              : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_58 {
	uint32_t raw;
	struct {
		uint32_t GENERIC_SHORT_PKT5              : 16;
		uint32_t GENERIC_SHORT_PKT6              : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_5C {
	uint32_t raw;
	struct {
		uint32_t GENERIC_SHORT_PKT7              : 16;
		uint32_t GENERIC_SHORT_PKT8              : 16;
	} bits;
};

union REG_CSI_CTRL_TOP_60 {
	uint32_t raw;
	struct {
		uint32_t CSI_INTR_STATUS                 : 8;
	} bits;
};

union REG_CSI_CTRL_TOP_64 {
	uint32_t raw;
	struct {
		uint32_t CSI_DBG_SEL                     : 8;
	} bits;
};

union REG_CSI_CTRL_TOP_70 {
	uint32_t raw;
	struct {
		uint32_t CSI_VS_GEN_MODE                 : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t CSI_VS_GEN_BY_VCSET             : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CSI_VS_DELAY_SEL                : 2;
		uint32_t CSI_HS_DELAY_SEL                : 2;
	} bits;
};

union REG_CSI_CTRL_TOP_74 {
	uint32_t raw;
	struct {
		uint32_t CSI_HDR_DT_MODE                 : 1;
		uint32_t _rsv_1                          : 3;
		uint32_t CSI_HDR_DT_FORMAT               : 6;
		uint32_t _rsv_10                         : 2;
		uint32_t CSI_HDR_DT_LEF                  : 6;
		uint32_t _rsv_18                         : 2;
		uint32_t CSI_HDR_DT_SEF                  : 6;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_MAC_VI_00 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_MODE                 : 3;
		uint32_t BT_DEMUX_ENABLE                 : 1;
		uint32_t CSI_CTRL_ENABLE                 : 1;
		uint32_t CSI_VS_INV                      : 1;
		uint32_t CSI_HS_INV                      : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t SUBLVDS_CTRL_ENABLE             : 1;
		uint32_t SUBLVDS_VS_INV                  : 1;
		uint32_t SUBLVDS_HS_INV                  : 1;
		uint32_t SUBLVDS_HDR_INV                 : 1;
		uint32_t SLVSEC_CTRL_ENABLE              : 1;
		uint32_t SLVSEC_VS_INV                   : 1;
		uint32_t SLVSEC_HS_INV                   : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t MASK_UP                         : 1;
		uint32_t SHRD_SEL                        : 1;
		uint32_t SW_UP                           : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t DBG_SEL                         : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_10 {
	uint32_t raw;
	struct {
		uint32_t TTL_IP_EN                       : 1;
		uint32_t TTL_SENSOR_BIT                  : 2;
		uint32_t _rsv_3                          : 1;
		uint32_t TTL_BT_FMT_OUT                  : 2;
		uint32_t _rsv_6                          : 2;
		uint32_t TTL_FMT_IN                      : 4;
		uint32_t TTL_BT_DATA_SEQ                 : 2;
		uint32_t TTL_VS_INV                      : 1;
		uint32_t TTL_HS_INV                      : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_14 {
	uint32_t raw;
	struct {
		uint32_t TTL_VS_BP                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_HS_BP                       : 12;
	} bits;
};

union REG_SENSOR_MAC_VI_18 {
	uint32_t raw;
	struct {
		uint32_t TTL_IMG_WD                      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_IMG_HT                      : 12;
	} bits;
};

union REG_SENSOR_MAC_VI_1C {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_0                      : 16;
		uint32_t TTL_SYNC_1                      : 16;
	} bits;
};

union REG_SENSOR_MAC_VI_20 {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_2                      : 16;
	} bits;
};

union REG_SENSOR_MAC_VI_24 {
	uint32_t raw;
	struct {
		uint32_t TTL_SAV_VLD                     : 16;
		uint32_t TTL_SAV_BLK                     : 16;
	} bits;
};

union REG_SENSOR_MAC_VI_28 {
	uint32_t raw;
	struct {
		uint32_t TTL_EAV_VLD                     : 16;
		uint32_t TTL_EAV_BLK                     : 16;
	} bits;
};

union REG_SENSOR_MAC_VI_30 {
	uint32_t raw;
	struct {
		uint32_t VI_SEL                          : 3;
		uint32_t VI_FROM                         : 1;
		uint32_t VI_CLK_INV                      : 1;
		uint32_t VI_V_SEL_VS                     : 1;
		uint32_t VI_VS_DBG                       : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t PAD_VI0_CLK_INV                 : 1;
		uint32_t PAD_VI1_CLK_INV                 : 1;
		uint32_t PAD_VI2_CLK_INV                 : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_34 {
	uint32_t raw;
	struct {
		uint32_t VI_VS_DLY                       : 5;
		uint32_t _rsv_5                          : 1;
		uint32_t VI_VS_DLY_EN                    : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t VI_HS_DLY                       : 5;
		uint32_t _rsv_13                         : 1;
		uint32_t VI_HS_DLY_EN                    : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t VI_VDE_DLY                      : 5;
		uint32_t _rsv_21                         : 1;
		uint32_t VI_VDE_DLY_EN                   : 1;
		uint32_t _rsv_23                         : 1;
		uint32_t VI_HDE_DLY                      : 5;
		uint32_t _rsv_29                         : 1;
		uint32_t VI_HDE_DLY_EN                   : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_40 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_EN               : 1;
		uint32_t SENSOR_MAC_HDR_VSINV            : 1;
		uint32_t SENSOR_MAC_HDR_HSINV            : 1;
		uint32_t SENSOR_MAC_HDR_DEINV            : 1;
		uint32_t SENSOR_MAC_HDR_HDR0INV          : 1;
		uint32_t SENSOR_MAC_HDR_HDR1INV          : 1;
		uint32_t SENSOR_MAC_HDR_BLCINV           : 1;
		uint32_t _rsv_7                          : 1;
		uint32_t SENSOR_MAC_HDR_MODE             : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_44 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_SHIFT            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_HDR_VSIZE            : 13;
	} bits;
};

union REG_SENSOR_MAC_VI_48 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_INFO_LINE_NUM        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_RM_INFO_LINE         : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_4C {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_LINE_CNT         : 14;
	} bits;
};

union REG_SENSOR_MAC_VI_60 {
	uint32_t raw;
	struct {
		uint32_t VI_VS_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_HS_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_VDE_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_HDE_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_VI_64 {
	uint32_t raw;
	struct {
		uint32_t VI_D0_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D1_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D2_SEL                       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D3_SEL                       : 6;
	} bits;
};

union REG_SENSOR_MAC_VI_68 {
	uint32_t raw;
	struct {
		uint32_t VI_D4_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D5_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D6_SEL                       : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D7_SEL                       : 6;
	} bits;
};

union REG_SENSOR_MAC_VI_6C {
	uint32_t raw;
	struct {
		uint32_t VI_D8_SEL                       : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D9_SEL                       : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D10_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D11_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_VI_70 {
	uint32_t raw;
	struct {
		uint32_t VI_D12_SEL                      : 6;
		uint32_t _rsv_6                          : 2;
		uint32_t VI_D13_SEL                      : 6;
		uint32_t _rsv_14                         : 2;
		uint32_t VI_D14_SEL                      : 6;
		uint32_t _rsv_22                         : 2;
		uint32_t VI_D15_SEL                      : 6;
	} bits;
};

union REG_SENSOR_MAC_VI_74 {
	uint32_t raw;
	struct {
		uint32_t VI_BT_D0_SEL                    : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t VI_BT_D1_SEL                    : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t VI_BT_D2_SEL                    : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t VI_BT_D3_SEL                    : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t VI_BT_D4_SEL                    : 3;
		uint32_t _rsv_19                         : 1;
		uint32_t VI_BT_D5_SEL                    : 3;
		uint32_t _rsv_23                         : 1;
		uint32_t VI_BT_D6_SEL                    : 3;
		uint32_t _rsv_27                         : 1;
		uint32_t VI_BT_D7_SEL                    : 3;
	} bits;
};

union REG_SENSOR_MAC_VI_80 {
	uint32_t raw;
	struct {
		uint32_t BT_CLR_SYNC_LOST_1T             : 1;
		uint32_t BT_IP_EN                        : 1;
		uint32_t BT_DDR_MODE                     : 1;
		uint32_t BT_HS_GATE_BY_VDE               : 1;
		uint32_t BT_VS_INV                       : 1;
		uint32_t BT_HS_INV                       : 1;
		uint32_t BT_VS_AS_VDE                    : 1;
		uint32_t BT_HS_AS_HDE                    : 1;
		uint32_t BT_SW_EN_CLK                    : 7;
		uint32_t _rsv_15                         : 1;
		uint32_t BT_DEMUX_CH                     : 2;
		uint32_t _rsv_18                         : 2;
		uint32_t BT_FMT_SEL                      : 3;
		uint32_t _rsv_23                         : 1;
		uint32_t BT_SYNC_LOST                    : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_84 {
	uint32_t raw;
	struct {
		uint32_t BT_V_CTRL_DLY                   : 5;
		uint32_t _rsv_5                          : 3;
		uint32_t BT_H_CTRL_DLY                   : 5;
	} bits;
};

union REG_SENSOR_MAC_VI_88 {
	uint32_t raw;
	struct {
		uint32_t BT_IMG_WD_M1                    : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BT_IMG_HT_M1                    : 12;
	} bits;
};

union REG_SENSOR_MAC_VI_8C {
	uint32_t raw;
	struct {
		uint32_t BT_VS_BP_M1                     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t BT_HS_BP_M1                     : 12;
	} bits;
};

union REG_SENSOR_MAC_VI_90 {
	uint32_t raw;
	struct {
		uint32_t BT_VS_FP_M1                     : 8;
		uint32_t BT_HS_FP_M1                     : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_94 {
	uint32_t raw;
	struct {
		uint32_t BT_SYNC_0                       : 8;
		uint32_t BT_SYNC_1                       : 8;
		uint32_t BT_SYNC_2                       : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_98 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_0                    : 8;
		uint32_t BT_SAV_BLK_0                    : 8;
		uint32_t BT_EAV_VLD_0                    : 8;
		uint32_t BT_EAV_BLK_0                    : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_9C {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_1                    : 8;
		uint32_t BT_SAV_BLK_1                    : 8;
		uint32_t BT_EAV_VLD_1                    : 8;
		uint32_t BT_EAV_BLK_1                    : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_A0 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_2                    : 8;
		uint32_t BT_SAV_BLK_2                    : 8;
		uint32_t BT_EAV_VLD_2                    : 8;
		uint32_t BT_EAV_BLK_2                    : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_A4 {
	uint32_t raw;
	struct {
		uint32_t BT_SAV_VLD_3                    : 8;
		uint32_t BT_SAV_BLK_3                    : 8;
		uint32_t BT_EAV_VLD_3                    : 8;
		uint32_t BT_EAV_BLK_3                    : 8;
	} bits;
};

union REG_SENSOR_MAC_VI_A8 {
	uint32_t raw;
	struct {
		uint32_t BT_YC_INV                       : 4;
	} bits;
};

union REG_SENSOR_MAC_VI_B0 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_CROP_START_X         : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_CROP_END_X           : 13;
		uint32_t _rsv_29                         : 2;
		uint32_t SENSOR_MAC_CROP_EN              : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_B4 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_CROP_START_Y         : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_CROP_END_Y           : 13;
	} bits;
};

union REG_SENSOR_MAC_VI_B8 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_SWAPUV_EN            : 1;
		uint32_t SENSOR_MAC_SWAPYC_EN            : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_BC {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_HTOTAL_MAX       : 16;
		uint32_t SENSOR_MAC_DBG_EN               : 1;
	} bits;
};

union REG_SENSOR_MAC_VI_C0 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_VTOTAL_MAX       : 32;
	} bits;
};

union REG_SENSOR_MAC_VI_C4 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_HTOTAL           : 16;
	} bits;
};

union REG_SENSOR_MAC_VI_C8 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_DBG_VTOTAL           : 32;
	} bits;
};

#endif // _REG_FIELDS_CSI_MAC_H_
