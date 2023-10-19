#ifndef _REG_FIELDS_CSI_WRAP_H_
#define _REG_FIELDS_CSI_WRAP_H_

//#include <stdint.h>
#include "linux/types.h"

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_CSI_DPHY_4LANE_WRAP_00 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_EN_BIST                  : 5;
		uint32_t _rsv_5                          : 19;
		uint32_t MIPIRX_EN_LOW_BAND_RXAFE        : 2;
		uint32_t MIPIRX_PD_IBIAS                 : 1;
		uint32_t MIPIRX_PD_RXLP                  : 5;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_04 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RT_CTRL                  : 4;
		uint32_t MIPIRX_SAMPLE_MODE              : 2;
		uint32_t MIPIRX_SEL_CLK_2LTO3L_TOP0      : 1;
		uint32_t MIPIRX_SEL_CLK_3LTO2L_TOP0      : 1;
		uint32_t MIPIRX_SEL_CLK_CHANNEL          : 5;
		uint32_t MIPIRX_EN_CLKIN_MPLL_TOP0       : 1;
		uint32_t MIPIRX_SEL_MPLL_DIV_TOP0        : 2;
		uint32_t MIPIMPLL_CLK_CSI_EN             : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_08 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST0               : 16;
		uint32_t MIPIRX_TEST_BIST1               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_0C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST2               : 16;
		uint32_t MIPIRX_TEST_BIST3               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_10 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST4               : 16;
		uint32_t MIPIRX_TEST_DEMUX0              : 8;
		uint32_t MIPIRX_TEST_DEMUX1              : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_14 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_DEMUX2              : 8;
		uint32_t MIPIRX_TEST_DEMUX3              : 8;
		uint32_t MIPIRX_TEST_DEMUX4              : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_18 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_GPIO                : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_1C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_GPIO_FRC_MODE_M          : 5;
		uint32_t MIPIRX_GPIO_FRC_MODE_P          : 5;
		uint32_t MIPIRX_GPIO_IEM                 : 5;
		uint32_t MIPIRX_GPIO_IEP                 : 5;
		uint32_t MIPIRX_GPIO_OENM                : 5;
		uint32_t MIPIRX_GPIO_OENP                : 5;
		uint32_t MIPIRX_GPIO_LPBK                : 2;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_20 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_GPO_IM                   : 5;
		uint32_t MIPIRX_GPO_IP                   : 5;
		uint32_t MIPIRX_GPI_CM                   : 5;
		uint32_t MIPIRX_GPI_CP                   : 5;
		uint32_t MIPIRX_GPI_FRC_CM               : 5;
		uint32_t MIPIRX_GPI_FRC_CP               : 5;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_24 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_GPO_FRC_IM               : 5;
		uint32_t MIPIRX_GPO_FRC_IP               : 5;
		uint32_t MIPIRX_GPIO_FRC_OENM            : 5;
		uint32_t MIPIRX_GPIO_FRC_OENP            : 5;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_28 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_CK_SEL                 : 3;
		uint32_t CSI_LANE_CK_PNSWAP              : 1;
		uint32_t CSI_LANE_D0_SEL                 : 3;
		uint32_t CSI_LANE_D0_PNSWAP              : 1;
		uint32_t CSI_LANE_D1_SEL                 : 3;
		uint32_t CSI_LANE_D1_PNSWAP              : 1;
		uint32_t CSI_LANE_D2_SEL                 : 3;
		uint32_t CSI_LANE_D2_PNSWAP              : 1;
		uint32_t CSI_LANE_D3_SEL                 : 3;
		uint32_t CSI_LANE_D3_PNSWAP              : 1;
		uint32_t CSI_LANE_D0_CKINV               : 1;
		uint32_t CSI_LANE_D1_CKINV               : 1;
		uint32_t CSI_LANE_D2_CKINV               : 1;
		uint32_t CSI_LANE_D3_CKINV               : 1;
		uint32_t DESKEW_LANE_EN                  : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_2C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL0                  : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_30 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL1                  : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_34 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL2                  : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_38 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL3                  : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_3C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL4                  : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_40 {
	uint32_t raw;
	struct {
		uint32_t AD_D0_DATA                      : 8;
		uint32_t AD_D1_DATA                      : 8;
		uint32_t AD_D2_DATA                      : 8;
		uint32_t AD_D3_DATA                      : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_44 {
	uint32_t raw;
	struct {
		uint32_t AD_D4_DATA                      : 8;
		uint32_t AD_LPOUTN                       : 5;
		uint32_t _rsv_13                         : 3;
		uint32_t AD_LPOUTP                       : 5;
		uint32_t AD_GPI_CM                       : 5;
		uint32_t AD_GPI_CP                       : 5;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_48 {
	uint32_t raw;
	struct {
		uint32_t PRBS9_TEST_PERIOD               : 16;
		uint32_t T_HS_SETTLE                     : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_4C {
	uint32_t raw;
	struct {
		uint32_t CSI_CK_PHASE                    : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_FRC_60 {
	uint32_t raw;
	struct {
		uint32_t FORCE_DESKEW_CODE0              : 1;
		uint32_t FORCE_DESKEW_CODE1              : 1;
		uint32_t FORCE_DESKEW_CODE2              : 1;
		uint32_t FORCE_DESKEW_CODE3              : 1;
		uint32_t FORCE_DESKEW_CODE4              : 1;
		uint32_t FORCE_BIST_MISC0                : 1;
		uint32_t FORCE_BIST_MISC1                : 1;
		uint32_t FORCE_BIST_MISC2                : 1;
		uint32_t FORCE_BIST_MISC3                : 1;
		uint32_t FORCE_BIST_MISC4                : 1;
		uint32_t FORCE_PD_RT                     : 1;
		uint32_t FORCE_PD_RXAFE_IB               : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t DESKEW_CODE0                    : 8;
		uint32_t DESKEW_CODE1                    : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_FRC_64 {
	uint32_t raw;
	struct {
		uint32_t DESKEW_CODE2                    : 8;
		uint32_t DESKEW_CODE3                    : 8;
		uint32_t DESKEW_CODE4                    : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_FRC_68 {
	uint32_t raw;
	struct {
		uint32_t BIST_MISC0                      : 2;
		uint32_t BIST_MISC1                      : 2;
		uint32_t BIST_MISC2                      : 2;
		uint32_t BIST_MISC3                      : 2;
		uint32_t BIST_MISC4                      : 2;
		uint32_t _rsv_10                         : 2;
		uint32_t PD_RT                           : 5;
		uint32_t _rsv_17                         : 3;
		uint32_t PD_RXAFE_IB                     : 5;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DFT_6C {
	uint32_t raw;
	struct {
		uint32_t DUMMY_0                         : 8;
		uint32_t DUMMY_1                         : 8;
		uint32_t DUMMY_2                         : 8;
		uint32_t DUMMY_3                         : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DFT_70 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_4                         : 8;
		uint32_t DUMMY_5                         : 8;
		uint32_t DUMMY_6                         : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DFT_74 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RD_0                      : 8;
		uint32_t DUMMY_RD_1                      : 8;
		uint32_t DUMMY_RD_2                      : 8;
		uint32_t DUMMY_RD_3                      : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DFT_78 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RD_4                      : 8;
		uint32_t DUMMY_RD_5                      : 8;
		uint32_t DUMMY_RD_6                      : 8;
		uint32_t DUMMY_RD_7                      : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DFT_7C {
	uint32_t raw;
	struct {
		uint32_t DUMMY_RD_8                      : 8;
		uint32_t DUMMY_RD_9                      : 8;
		uint32_t DUMMY_RD_A                      : 8;
		uint32_t DUMMY_RD_B                      : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_CK_80 {
	uint32_t raw;
	struct {
		uint32_t CK_LP_P_DBG_MODE                : 1;
		uint32_t CK_LP_N_DBG_MODE                : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_CK_84 {
	uint32_t raw;
	struct {
		uint32_t CK_LP_P_DBG_COUNT               : 16;
		uint32_t CK_LP_N_DBG_COUNT               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DBG_90 {
	uint32_t raw;
	struct {
		uint32_t DBG_SEL                         : 16;
		uint32_t DBG_CK_SEL                      : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_LVDS_F0 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MODE                     : 1;
		uint32_t SLVDS_INV_EN                    : 1;
		uint32_t SLVDS_BIT_MODE                  : 2;
		uint32_t SLVDS_LANE_EN                   : 4;
		uint32_t _rsv_8                          : 8;
		uint32_t SLVDS_SAV_1ST                   : 12;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_LVDS_F4 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_SAV_2ND                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_SAV_3RD                   : 12;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_0 {
	uint32_t raw;
	struct {
		uint32_t D0_PRBS9_EN                     : 1;
		uint32_t D0_PRBS9_CLR_ERR                : 1;
		uint32_t D0_PRBS9_SOURCE                 : 1;
		uint32_t D0_PRBS9_STOP_WHEN_DONE         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t D0_CALIB_MAX                    : 8;
		uint32_t D0_CALIB_STEP                   : 8;
		uint32_t D0_CALIB_PATTERN                : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_1 {
	uint32_t raw;
	struct {
		uint32_t D0_CALIB_EN                     : 1;
		uint32_t D0_CALIB_SOURCE                 : 1;
		uint32_t D0_CALIB_MODE                   : 1;
		uint32_t D0_CALIB_IGNORE                 : 1;
		uint32_t D0_CALIB_SETTLE                 : 3;
		uint32_t D0_CALIB_PHASE_NO_SHIFT         : 1;
		uint32_t D0_CALIB_SET_PHASE              : 8;
		uint32_t D0_CALIB_CYCLE                  : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_2 {
	uint32_t raw;
	struct {
		uint32_t D0_PRBS9_RX_ERR                 : 1;
		uint32_t D0_PRBS9_TEST_DONE              : 1;
		uint32_t D0_PRBS9_TEST_PASS              : 1;
		uint32_t D0_SKEW_CALIB_DONE              : 1;
		uint32_t D0_SKEW_CALIB_FAIL              : 1;
		uint32_t D0_DATALP_STATE                 : 4;
		uint32_t D0_DATALP_LPREQ2ERR             : 1;
		uint32_t D0_DATALP_DATAESC2ERR           : 1;
		uint32_t D0_DATALP_RSTTRI2ERR            : 1;
		uint32_t D0_DATALP_HSTEST2ERR            : 1;
		uint32_t D0_DATALP_ESCULP2ERR            : 1;
		uint32_t D0_DATALP_HS2ERR                : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t D0_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_3 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_4 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_5 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_6 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_7 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_8 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_9 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_A {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_B {
	uint32_t raw;
	struct {
		uint32_t D0_LP_P_DBG_MODE                : 1;
		uint32_t D0_LP_N_DBG_MODE                : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D0_C {
	uint32_t raw;
	struct {
		uint32_t D0_LP_P_DBG_COUNT               : 16;
		uint32_t D0_LP_N_DBG_COUNT               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_0 {
	uint32_t raw;
	struct {
		uint32_t D1_PRBS9_EN                     : 1;
		uint32_t D1_PRBS9_CLR_ERR                : 1;
		uint32_t D1_PRBS9_SOURCE                 : 1;
		uint32_t D1_PRBS9_STOP_WHEN_DONE         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t D1_CALIB_MAX                    : 8;
		uint32_t D1_CALIB_STEP                   : 8;
		uint32_t D1_CALIB_PATTERN                : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_1 {
	uint32_t raw;
	struct {
		uint32_t D1_CALIB_EN                     : 1;
		uint32_t D1_CALIB_SOURCE                 : 1;
		uint32_t D1_CALIB_MODE                   : 1;
		uint32_t D1_CALIB_IGNORE                 : 1;
		uint32_t D1_CALIB_SETTLE                 : 3;
		uint32_t D1_CALIB_PHASE_NO_SHIFT         : 1;
		uint32_t D1_CALIB_SET_PHASE              : 8;
		uint32_t D1_CALIB_CYCLE                  : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_2 {
	uint32_t raw;
	struct {
		uint32_t D1_PRBS9_RX_ERR                 : 1;
		uint32_t D1_PRBS9_TEST_DONE              : 1;
		uint32_t D1_PRBS9_TEST_PASS              : 1;
		uint32_t D1_SKEW_CALIB_DONE              : 1;
		uint32_t D1_SKEW_CALIB_FAIL              : 1;
		uint32_t D1_DATALP_STATE                 : 4;
		uint32_t D1_DATALP_LPREQ2ERR             : 1;
		uint32_t D1_DATALP_DATAESC2ERR           : 1;
		uint32_t D1_DATALP_RSTTRI2ERR            : 1;
		uint32_t D1_DATALP_HSTEST2ERR            : 1;
		uint32_t D1_DATALP_ESCULP2ERR            : 1;
		uint32_t D1_DATALP_HS2ERR                : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t D1_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_3 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_4 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_5 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_6 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_7 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_8 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_9 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_A {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_B {
	uint32_t raw;
	struct {
		uint32_t D1_LP_P_DBG_MODE                : 1;
		uint32_t D1_LP_N_DBG_MODE                : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D1_C {
	uint32_t raw;
	struct {
		uint32_t D1_LP_P_DBG_COUNT               : 16;
		uint32_t D1_LP_N_DBG_COUNT               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_0 {
	uint32_t raw;
	struct {
		uint32_t D2_PRBS9_EN                     : 1;
		uint32_t D2_PRBS9_CLR_ERR                : 1;
		uint32_t D2_PRBS9_SOURCE                 : 1;
		uint32_t D2_PRBS9_STOP_WHEN_DONE         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t D2_CALIB_MAX                    : 8;
		uint32_t D2_CALIB_STEP                   : 8;
		uint32_t D2_CALIB_PATTERN                : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_1 {
	uint32_t raw;
	struct {
		uint32_t D2_CALIB_EN                     : 1;
		uint32_t D2_CALIB_SOURCE                 : 1;
		uint32_t D2_CALIB_MODE                   : 1;
		uint32_t D2_CALIB_IGNORE                 : 1;
		uint32_t D2_CALIB_SETTLE                 : 3;
		uint32_t D2_CALIB_PHASE_NO_SHIFT         : 1;
		uint32_t D2_CALIB_SET_PHASE              : 8;
		uint32_t D2_CALIB_CYCLE                  : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_2 {
	uint32_t raw;
	struct {
		uint32_t D2_PRBS9_RX_ERR                 : 1;
		uint32_t D2_PRBS9_TEST_DONE              : 1;
		uint32_t D2_PRBS9_TEST_PASS              : 1;
		uint32_t D2_SKEW_CALIB_DONE              : 1;
		uint32_t D2_SKEW_CALIB_FAIL              : 1;
		uint32_t D2_DATALP_STATE                 : 4;
		uint32_t D2_DATALP_LPREQ2ERR             : 1;
		uint32_t D2_DATALP_DATAESC2ERR           : 1;
		uint32_t D2_DATALP_RSTTRI2ERR            : 1;
		uint32_t D2_DATALP_HSTEST2ERR            : 1;
		uint32_t D2_DATALP_ESCULP2ERR            : 1;
		uint32_t D2_DATALP_HS2ERR                : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t D2_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_3 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_4 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_5 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_6 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_7 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_8 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_9 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_A {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_B {
	uint32_t raw;
	struct {
		uint32_t D2_LP_P_DBG_MODE                : 1;
		uint32_t D2_LP_N_DBG_MODE                : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D2_C {
	uint32_t raw;
	struct {
		uint32_t D2_LP_P_DBG_COUNT               : 16;
		uint32_t D2_LP_N_DBG_COUNT               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_0 {
	uint32_t raw;
	struct {
		uint32_t D3_PRBS9_EN                     : 1;
		uint32_t D3_PRBS9_CLR_ERR                : 1;
		uint32_t D3_PRBS9_SOURCE                 : 1;
		uint32_t D3_PRBS9_STOP_WHEN_DONE         : 1;
		uint32_t _rsv_4                          : 4;
		uint32_t D3_CALIB_MAX                    : 8;
		uint32_t D3_CALIB_STEP                   : 8;
		uint32_t D3_CALIB_PATTERN                : 8;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_1 {
	uint32_t raw;
	struct {
		uint32_t D3_CALIB_EN                     : 1;
		uint32_t D3_CALIB_SOURCE                 : 1;
		uint32_t D3_CALIB_MODE                   : 1;
		uint32_t D3_CALIB_IGNORE                 : 1;
		uint32_t D3_CALIB_SETTLE                 : 3;
		uint32_t D3_CALIB_PHASE_NO_SHIFT         : 1;
		uint32_t D3_CALIB_SET_PHASE              : 8;
		uint32_t D3_CALIB_CYCLE                  : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_2 {
	uint32_t raw;
	struct {
		uint32_t D3_PRBS9_RX_ERR                 : 1;
		uint32_t D3_PRBS9_TEST_DONE              : 1;
		uint32_t D3_PRBS9_TEST_PASS              : 1;
		uint32_t D3_SKEW_CALIB_DONE              : 1;
		uint32_t D3_SKEW_CALIB_FAIL              : 1;
		uint32_t D3_DATALP_STATE                 : 4;
		uint32_t D3_DATALP_LPREQ2ERR             : 1;
		uint32_t D3_DATALP_DATAESC2ERR           : 1;
		uint32_t D3_DATALP_RSTTRI2ERR            : 1;
		uint32_t D3_DATALP_HSTEST2ERR            : 1;
		uint32_t D3_DATALP_ESCULP2ERR            : 1;
		uint32_t D3_DATALP_HS2ERR                : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t D3_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_3 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_4 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_5 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_6 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_7 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_8 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_9 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_A {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_B {
	uint32_t raw;
	struct {
		uint32_t D3_LP_P_DBG_MODE                : 1;
		uint32_t D3_LP_N_DBG_MODE                : 1;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_D3_C {
	uint32_t raw;
	struct {
		uint32_t D3_LP_P_DBG_COUNT               : 16;
		uint32_t D3_LP_N_DBG_COUNT               : 16;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_0 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE0_L            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_1 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE0_H            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_2 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE1_L            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_3 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE1_H            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_4 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE2_L            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_5 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE2_H            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_6 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE3_L            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_7 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE3_H            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_8 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE4_L            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_TEST_9 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE4_H            : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DUMMY_D0 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D0                        : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DUMMY_D1 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D1                        : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DUMMY_D2 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D2                        : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DUMMY_D3 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D3                        : 32;
	} bits;
};

union REG_CSI_DPHY_4LANE_WRAP_DBG_300 {
	uint32_t raw;
	struct {
		uint32_t CK_HS_STATE                     : 1;
		uint32_t CK_ULPS_STATE                   : 1;
		uint32_t CK_STOPSTATE                    : 1;
		uint32_t CK_ERR_STATE                    : 1;
		uint32_t D0_DATAHS_STATE                 : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t D1_DATAHS_STATE                 : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t D2_DATAHS_STATE                 : 3;
		uint32_t _rsv_15                         : 1;
		uint32_t D3_DATAHS_STATE                 : 3;
		uint32_t _rsv_19                         : 1;
		uint32_t DESKEW_STATE                    : 2;
	} bits;
};

#endif // _REG_FIELDS_CSI_WRAP_H_
