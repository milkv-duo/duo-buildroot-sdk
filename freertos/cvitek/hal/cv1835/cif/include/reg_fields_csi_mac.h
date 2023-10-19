#ifndef _REG_FIELDS_CSI_MAC_H_
#define _REG_FIELDS_CSI_MAC_H_

//#include <stdint.h>
#include "linux/types.h"
/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_MAC_1C4D_00 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_MODE                 : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t CSI_CTRL_ENABLE                 : 1;
		uint32_t CSI_VS_INV                      : 1;
		uint32_t CSI_HS_INV                      : 1;
		uint32_t CSI_HDR_INV                     : 1;
		uint32_t SUBLVDS_CTRL_ENABLE             : 1;
		uint32_t SUBLVDS_VS_INV                  : 1;
		uint32_t SUBLVDS_HS_INV                  : 1;
		uint32_t SUBLVDS_HDR_INV                 : 1;
		uint32_t MASK_UP                         : 1;
		uint32_t SHRD_SEL                        : 1;
		uint32_t SW_UP                           : 1;
	} bits;
};

union REG_SENSOR_MAC_1C4D_10 {
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

union REG_SENSOR_MAC_1C4D_14 {
	uint32_t raw;
	struct {
		uint32_t TTL_VS_BP                       : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_HS_BP                       : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_18 {
	uint32_t raw;
	struct {
		uint32_t TTL_IMG_WD                      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_IMG_HT                      : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_1C {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_0                      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_SYNC_1                      : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_20 {
	uint32_t raw;
	struct {
		uint32_t TTL_SYNC_2                      : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_24 {
	uint32_t raw;
	struct {
		uint32_t TTL_SAV_VLD                     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_SAV_BLK                     : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_28 {
	uint32_t raw;
	struct {
		uint32_t TTL_EAV_VLD                     : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t TTL_EAV_BLK                     : 12;
	} bits;
};

union REG_SENSOR_MAC_1C4D_30 {
	uint32_t raw;
	struct {
		uint32_t VI_SEL                          : 4;
		uint32_t VI_CLK_INV                      : 1;
		uint32_t VI_V_SEL_VS                     : 1;
		uint32_t VI_VS_DBG                       : 1;
	} bits;
};

union REG_SENSOR_MAC_1C4D_40 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_EN               : 1;
		uint32_t SENSOR_MAC_HDR_VSINV            : 1;
		uint32_t SENSOR_MAC_HDR_HSINV            : 1;
		uint32_t SENSOR_MAC_HDR_DEINV            : 1;
		uint32_t SENSOR_MAC_HDR_HDRINV           : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t SENSOR_MAC_HDR_MODE             : 1;
	} bits;
};

union REG_SENSOR_MAC_1C4D_44 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_SHIFT            : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_HDR_VSIZE            : 13;
	} bits;
};

union REG_SENSOR_MAC_1C4D_48 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_INFO_LINE_NUM        : 13;
		uint32_t _rsv_13                         : 3;
		uint32_t SENSOR_MAC_RM_INFO_LINE         : 1;
	} bits;
};

union REG_SENSOR_MAC_1C4D_4C {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MAC_HDR_LINE_CNT         : 14;
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
	} bits;
};

union REG_CSI_CTRL_TOP_08 {
	uint32_t raw;
	struct {
		uint32_t CSI_HDR_EN                      : 1;
		uint32_t CSI_HDR_MODE                    : 1;
		uint32_t _rsv_2                          : 2;
		uint32_t CSI_N0_OB_LEF                   : 12;
		uint32_t CSI_N0_OB_SEF                   : 12;
	} bits;
};

union REG_CSI_CTRL_TOP_0C {
	uint32_t raw;
	struct {
		uint32_t CSI_N0_LEF                      : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CSI_N1_OB_LEF                   : 12;
	} bits;
};

union REG_CSI_CTRL_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t CSI_N1_OB_SEF                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t CSI_N1_LEF                      : 12;
		uint32_t CSI_VC_LEF                      : 4;
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
		uint32_t CSI_DECODE_FORMAT               : 5;
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

#endif // _REG_FIELDS_CSI_MAC_H_
