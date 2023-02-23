#ifndef _REG_FIELDS_CSI_WRAP_H_
#define _REG_FIELDS_CSI_WRAP_H_

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_PHY_2L_00 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MODE                     : 2;
	} bits;
};

union REG_SENSOR_PHY_2L_04 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_D0_SEL                 : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t CSI_LANE_D1_SEL                 : 2;
	} bits;
};

union REG_SENSOR_PHY_2L_08 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_CK_SEL                 : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t CSI_LANE_CK_PNSWAP              : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CSI_LANE_D0_PNSWAP              : 1;
		uint32_t CSI_LANE_D1_PNSWAP              : 1;
		uint32_t _rsv_10                         : 6;
		uint32_t CSI_CK_PHASE                    : 8;
	} bits;
};

union REG_SENSOR_PHY_2L_0C {
	uint32_t raw;
	struct {
		uint32_t DESKEW_LANE_EN                  : 2;
		uint32_t _rsv_2                          : 14;
		uint32_t PRBS9_TEST_PERIOD               : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_10 {
	uint32_t raw;
	struct {
		uint32_t T_HS_SETTLE                     : 8;
		uint32_t T_ALL_ZERO                      : 8;
		uint32_t AUTO_IGNORE                     : 1;
		uint32_t AUTO_SYNC                       : 1;
	} bits;
};

union REG_SENSOR_PHY_2L_20 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_INV_EN                    : 1;
		uint32_t _rsv_1                          : 1;
		uint32_t SLVDS_BIT_MODE                  : 2;
		uint32_t SLVDS_LANE_EN                   : 2;
		uint32_t _rsv_6                          : 6;
		uint32_t SLVDS_FORCE_RESYNC              : 1;
		uint32_t SLVDS_RESYNC                    : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t SLVDS_SAV_1ST                   : 12;
	} bits;
};

union REG_SENSOR_PHY_2L_24 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_SAV_2ND                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_SAV_3RD                   : 12;
	} bits;
};

union REG_SENSOR_PHY_2L_28 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_D0_SYNC_STATE             : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t SLVDS_D1_SYNC_STATE             : 2;
	} bits;
};

union REG_SENSOR_PHY_2L_30 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_LANE_EN                  : 2;
		uint32_t _rsv_2                          : 6;
		uint32_t SLVSEC_SKEW_CNT_EN              : 1;
		uint32_t SLVSEC_TRAIN_SEQ_CHK_EN         : 1;
		uint32_t _rsv_10                         : 6;
		uint32_t SLVSEC_SKEW_CONS                : 5;
		uint32_t SLVSEC_FORCE_RESYNC             : 1;
		uint32_t SLVSEC_RESYNC                   : 1;
		uint32_t _rsv_23                         : 1;
		uint32_t SLVSEC_UNSTABLE_SKEW_CNT        : 8;
	} bits;
};

union REG_SENSOR_PHY_2L_34 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_SYNC_SYMBOL              : 9;
		uint32_t _rsv_9                          : 1;
		uint32_t SLVSEC_STANDBY_SYMBOL           : 9;
		uint32_t _rsv_19                         : 1;
		uint32_t SLVSEC_DESKEW_SYMBOL            : 9;
	} bits;
};

union REG_SENSOR_PHY_2L_38 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PRBS9_TEST_PERIOD        : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_3C {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_CLR             : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_40 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_MASK            : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_44 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_STATUS          : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_48 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D0_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D0_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D0_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_2L_4C {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D0_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D0_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D0_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D0_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D0_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D0_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D0_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D0_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D0_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_2L_50 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D1_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D1_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D1_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_2L_54 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D1_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D1_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D1_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D1_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D1_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D1_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D1_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D1_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D1_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_2L_DBG_90 {
	uint32_t raw;
	struct {
		uint32_t CK_HS_STATE                     : 1;
		uint32_t CK_ULPS_STATE                   : 1;
		uint32_t CK_STOPSTATE                    : 1;
		uint32_t CK_ERR_STATE                    : 1;
		uint32_t DESKEW_STATE                    : 2;
	} bits;
};

union REG_SENSOR_PHY_2L_DBG_94 {
	uint32_t raw;
	struct {
		uint32_t D0_DATAHS_STATE                 : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t D1_DATAHS_STATE                 : 3;
	} bits;
};

union REG_SENSOR_PHY_2L_STATUS_98 {
	uint32_t raw;
	struct {
		uint32_t CK_LP_STATUS_CLR                : 8;
		uint32_t D0_LP_STATUS_CLR                : 8;
		uint32_t D1_LP_STATUS_CLR                : 8;
	} bits;
};

union REG_SENSOR_PHY_2L_STATUS_9C {
	uint32_t raw;
	struct {
		uint32_t CK_LP_STATUS_OUT                : 8;
		uint32_t D0_LP_STATUS_OUT                : 8;
		uint32_t D1_LP_STATUS_OUT                : 8;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_0 {
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

union REG_SENSOR_PHY_2L_D0_1 {
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

union REG_SENSOR_PHY_2L_D0_2 {
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
		uint32_t D0_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D0_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_3 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_4 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_5 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_6 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_7 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_8 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_9 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_A {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D0_B {
	uint32_t raw;
	struct {
		uint32_t D0_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D0_CALIB_THRESHOLD              : 8;
		uint32_t D0_CALIB_GP_COUNT               : 9;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_0 {
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

union REG_SENSOR_PHY_2L_D1_1 {
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

union REG_SENSOR_PHY_2L_D1_2 {
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
		uint32_t D1_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D1_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_3 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_4 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_5 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_6 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_7 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_8 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_9 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_A {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_2L_D1_B {
	uint32_t raw;
	struct {
		uint32_t D1_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D1_CALIB_THRESHOLD              : 8;
		uint32_t D1_CALIB_GP_COUNT               : 9;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_PHY_4L_00 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_MODE                     : 2;
	} bits;
};

union REG_SENSOR_PHY_4L_04 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_D0_SEL                 : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t CSI_LANE_D1_SEL                 : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t CSI_LANE_D2_SEL                 : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t CSI_LANE_D3_SEL                 : 3;
	} bits;
};

union REG_SENSOR_PHY_4L_08 {
	uint32_t raw;
	struct {
		uint32_t CSI_LANE_CK_SEL                 : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t CSI_LANE_CK_PNSWAP              : 1;
		uint32_t _rsv_5                          : 3;
		uint32_t CSI_LANE_D0_PNSWAP              : 1;
		uint32_t CSI_LANE_D1_PNSWAP              : 1;
		uint32_t CSI_LANE_D2_PNSWAP              : 1;
		uint32_t CSI_LANE_D3_PNSWAP              : 1;
		uint32_t _rsv_12                         : 4;
		uint32_t CSI_CK_PHASE                    : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_0C {
	uint32_t raw;
	struct {
		uint32_t DESKEW_LANE_EN                  : 4;
		uint32_t _rsv_4                          : 12;
		uint32_t PRBS9_TEST_PERIOD               : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_10 {
	uint32_t raw;
	struct {
		uint32_t T_HS_SETTLE                     : 8;
		uint32_t T_ALL_ZERO                      : 8;
		uint32_t AUTO_IGNORE                     : 1;
		uint32_t AUTO_SYNC                       : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_20 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_INV_EN                    : 1;
		uint32_t _rsv_1                          : 1;
		uint32_t SLVDS_BIT_MODE                  : 2;
		uint32_t SLVDS_LANE_EN                   : 4;
		uint32_t _rsv_8                          : 4;
		uint32_t SLVDS_FORCE_RESYNC              : 1;
		uint32_t SLVDS_RESYNC                    : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t SLVDS_SAV_1ST                   : 12;
	} bits;
};

union REG_SENSOR_PHY_4L_24 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_SAV_2ND                   : 12;
		uint32_t _rsv_12                         : 4;
		uint32_t SLVDS_SAV_3RD                   : 12;
	} bits;
};

union REG_SENSOR_PHY_4L_28 {
	uint32_t raw;
	struct {
		uint32_t SLVDS_D0_SYNC_STATE             : 2;
		uint32_t _rsv_2                          : 2;
		uint32_t SLVDS_D1_SYNC_STATE             : 2;
		uint32_t _rsv_6                          : 2;
		uint32_t SLVDS_D2_SYNC_STATE             : 2;
		uint32_t _rsv_10                         : 2;
		uint32_t SLVDS_D3_SYNC_STATE             : 2;
	} bits;
};

union REG_SENSOR_PHY_4L_30 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_LANE_EN                  : 4;
		uint32_t _rsv_4                          : 4;
		uint32_t SLVSEC_SKEW_CNT_EN              : 1;
		uint32_t SLVSEC_TRAIN_SEQ_CHK_EN         : 1;
		uint32_t _rsv_10                         : 6;
		uint32_t SLVSEC_SKEW_CONS                : 5;
		uint32_t SLVSEC_FORCE_RESYNC             : 1;
		uint32_t SLVSEC_RESYNC                   : 1;
		uint32_t _rsv_23                         : 1;
		uint32_t SLVSEC_UNSTABLE_SKEW_CNT        : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_34 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_SYNC_SYMBOL              : 9;
		uint32_t _rsv_9                          : 1;
		uint32_t SLVSEC_STANDBY_SYMBOL           : 9;
		uint32_t _rsv_19                         : 1;
		uint32_t SLVSEC_DESKEW_SYMBOL            : 9;
	} bits;
};

union REG_SENSOR_PHY_4L_38 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PRBS9_TEST_PERIOD        : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_3C {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_CLR             : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_40 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_MASK            : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_44 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_PHY_INTR_STATUS          : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_48 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D0_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D0_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D0_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_4C {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D0_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D0_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D0_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D0_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D0_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D0_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D0_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D0_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D0_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_50 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D1_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D1_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D1_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_54 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D1_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D1_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D1_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D1_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D1_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D1_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D1_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D1_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D1_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_58 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D2_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D2_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D2_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_5C {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D2_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D2_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D2_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D2_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D2_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D2_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D2_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D2_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D2_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_60 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D3_TEST_PAT_EN           : 1;
		uint32_t SLVSEC_D3_CLR_TEST_PAT_ERR      : 1;
		uint32_t SLVSEC_D3_TEST_STOP_WHEN_DONE   : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_64 {
	uint32_t raw;
	struct {
		uint32_t SLVSEC_D3_TEST_PAT_ERR_CNT      : 16;
		uint32_t SLVSEC_D3_TEST_PAT_ERR          : 1;
		uint32_t SLVSEC_D3_TEST_PAT_PASS         : 1;
		uint32_t SLVSEC_D3_TEST_PAT_DONE         : 1;
		uint32_t _rsv_19                         : 5;
		uint32_t SLVSEC_D3_START_CODE_ERR        : 1;
		uint32_t SLVSEC_D3_END_CODE_ERR          : 1;
		uint32_t SLVSEC_D3_DESKEW_CODE_ERR       : 1;
		uint32_t SLVSEC_D3_STANDBY_CODE_ERR      : 1;
		uint32_t SLVSEC_D3_SYNC_CODE_ERR         : 1;
	} bits;
};

union REG_SENSOR_PHY_4L_DBG_90 {
	uint32_t raw;
	struct {
		uint32_t CK_HS_STATE                     : 1;
		uint32_t CK_ULPS_STATE                   : 1;
		uint32_t CK_STOPSTATE                    : 1;
		uint32_t CK_ERR_STATE                    : 1;
		uint32_t DESKEW_STATE                    : 2;
	} bits;
};

union REG_SENSOR_PHY_4L_DBG_94 {
	uint32_t raw;
	struct {
		uint32_t D0_DATAHS_STATE                 : 3;
		uint32_t _rsv_3                          : 1;
		uint32_t D1_DATAHS_STATE                 : 3;
		uint32_t _rsv_7                          : 1;
		uint32_t D2_DATAHS_STATE                 : 3;
		uint32_t _rsv_11                         : 1;
		uint32_t D3_DATAHS_STATE                 : 3;
	} bits;
};

union REG_SENSOR_PHY_4L_STATUS_98 {
	uint32_t raw;
	struct {
		uint32_t CK_LP_STATUS_CLR                : 8;
		uint32_t D0_LP_STATUS_CLR                : 8;
		uint32_t D1_LP_STATUS_CLR                : 8;
		uint32_t D2_LP_STATUS_CLR                : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_STATUS_9C {
	uint32_t raw;
	struct {
		uint32_t D3_LP_STATUS_CLR                : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_STATUS_A4 {
	uint32_t raw;
	struct {
		uint32_t CK_LP_STATUS_OUT                : 8;
		uint32_t D0_LP_STATUS_OUT                : 8;
		uint32_t D1_LP_STATUS_OUT                : 8;
		uint32_t D2_LP_STATUS_OUT                : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_STATUS_A8 {
	uint32_t raw;
	struct {
		uint32_t D3_LP_STATUS_OUT                : 8;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_0 {
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

union REG_SENSOR_PHY_4L_D0_1 {
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

union REG_SENSOR_PHY_4L_D0_2 {
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
		uint32_t D0_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D0_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_3 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_4 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_5 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_6 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_7 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_8 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_9 {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_A {
	uint32_t raw;
	struct {
		uint32_t D0_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D0_B {
	uint32_t raw;
	struct {
		uint32_t D0_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D0_CALIB_THRESHOLD              : 8;
		uint32_t D0_CALIB_GP_COUNT               : 9;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_0 {
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

union REG_SENSOR_PHY_4L_D1_1 {
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

union REG_SENSOR_PHY_4L_D1_2 {
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
		uint32_t D1_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D1_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_3 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_4 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_5 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_6 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_7 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_8 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_9 {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_A {
	uint32_t raw;
	struct {
		uint32_t D1_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D1_B {
	uint32_t raw;
	struct {
		uint32_t D1_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D1_CALIB_THRESHOLD              : 8;
		uint32_t D1_CALIB_GP_COUNT               : 9;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_0 {
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

union REG_SENSOR_PHY_4L_D2_1 {
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

union REG_SENSOR_PHY_4L_D2_2 {
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
		uint32_t D2_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D2_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_3 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_4 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_5 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_6 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_7 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_8 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_9 {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_A {
	uint32_t raw;
	struct {
		uint32_t D2_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D2_B {
	uint32_t raw;
	struct {
		uint32_t D2_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D2_CALIB_THRESHOLD              : 8;
		uint32_t D2_CALIB_GP_COUNT               : 9;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_0 {
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

union REG_SENSOR_PHY_4L_D3_1 {
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

union REG_SENSOR_PHY_4L_D3_2 {
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
		uint32_t D3_DATA_EXIST_1ST_BYTE          : 1;
		uint32_t D3_PRBS9_ERR_CNT                : 16;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_3 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_0          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_4 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_1          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_5 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_2          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_6 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_3          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_7 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_4          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_8 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_5          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_9 {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_6          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_A {
	uint32_t raw;
	struct {
		uint32_t D3_SKEW_CALIB_RESULT_7          : 32;
	} bits;
};

union REG_SENSOR_PHY_4L_D3_B {
	uint32_t raw;
	struct {
		uint32_t D3_CALIB_OPTION                 : 1;
		uint32_t _rsv_1                          : 7;
		uint32_t D3_CALIB_THRESHOLD              : 8;
		uint32_t D3_CALIB_GP_COUNT               : 9;
	} bits;
};

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_SENSOR_PHY_TOP_00 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_EN_BIST                  : 6;
		uint32_t _rsv_6                          : 6;
		uint32_t MIPIRX_EN_LOW_BAND_RXAFE        : 2;
		uint32_t MIPIRX_PD_IBIAS                 : 1;
		uint32_t _rsv_15                         : 1;
		uint32_t MIPIRX_PD_RXLP                  : 6;
	} bits;
};

union REG_SENSOR_PHY_TOP_04 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RT_CTRL                  : 4;
		uint32_t MIPIRX_SAMPLE_MODE              : 2;
		uint32_t MIPIRX_SEL_CLK_P0TOP1           : 1;
		uint32_t MIPIRX_SEL_CLK_P1TOP0           : 1;
		uint32_t _rsv_8                          : 8;
		uint32_t MIPIRX_SEL_CLK_CHANNEL          : 6;
		uint32_t _rsv_22                         : 6;
		uint32_t MIPIRX_EN_CLKIN_MPLL_TOP0       : 1;
		uint32_t MIPIRX_SEL_MPLL_DIV_TOP0        : 2;
		uint32_t MIPIMPLL_CLK_CSI_EN             : 1;
	} bits;
};

union REG_SENSOR_PHY_TOP_08 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST0               : 16;
		uint32_t MIPIRX_TEST_BIST1               : 16;
	} bits;
};

union REG_SENSOR_PHY_TOP_0C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST2               : 16;
		uint32_t MIPIRX_TEST_BIST3               : 16;
	} bits;
};

union REG_SENSOR_PHY_TOP_10 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_BIST4               : 16;
		uint32_t MIPIRX_TEST_BIST5               : 16;
	} bits;
};

union REG_SENSOR_PHY_TOP_20 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_DEMUX0              : 8;
		uint32_t MIPIRX_TEST_DEMUX1              : 8;
		uint32_t MIPIRX_TEST_DEMUX2              : 8;
		uint32_t MIPIRX_TEST_DEMUX3              : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_24 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_DEMUX4              : 8;
		uint32_t MIPIRX_TEST_DEMUX5              : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_2C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_SEL_IBIAS_MODE           : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_30 {
	uint32_t raw;
	struct {
		uint32_t SENSOR_PHY_MODE                 : 3;
	} bits;
};

union REG_SENSOR_PHY_TOP_34 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL0                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_38 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL1                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_3C {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL2                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_40 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL3                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_44 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL4                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_48 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_RO_CAL5                  : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_70 {
	uint32_t raw;
	struct {
		uint32_t AD_D0_DATA                      : 8;
		uint32_t AD_D1_DATA                      : 8;
		uint32_t AD_D2_DATA                      : 8;
		uint32_t AD_D3_DATA                      : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_74 {
	uint32_t raw;
	struct {
		uint32_t AD_D4_DATA                      : 8;
		uint32_t AD_D5_DATA                      : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_7C {
	uint32_t raw;
	struct {
		uint32_t AD_LPOUTN                       : 6;
		uint32_t _rsv_6                          : 10;
		uint32_t AD_LPOUTP                       : 6;
	} bits;
};

union REG_SENSOR_PHY_TOP_80 {
	uint32_t raw;
	struct {
		uint32_t AD_D0_CLK_INV                   : 1;
		uint32_t AD_D1_CLK_INV                   : 1;
		uint32_t AD_D2_CLK_INV                   : 1;
		uint32_t AD_D3_CLK_INV                   : 1;
		uint32_t AD_D4_CLK_INV                   : 1;
		uint32_t AD_D5_CLK_INV                   : 1;
		uint32_t _rsv_6                          : 10;
		uint32_t FORCE_DESKEW_CODE0              : 1;
		uint32_t FORCE_DESKEW_CODE1              : 1;
		uint32_t FORCE_DESKEW_CODE2              : 1;
		uint32_t FORCE_DESKEW_CODE3              : 1;
		uint32_t FORCE_DESKEW_CODE4              : 1;
		uint32_t FORCE_DESKEW_CODE5              : 1;
	} bits;
};

union REG_SENSOR_PHY_TOP_84 {
	uint32_t raw;
	struct {
		uint32_t DESKEW_CODE0                    : 8;
		uint32_t DESKEW_CODE1                    : 8;
		uint32_t DESKEW_CODE2                    : 8;
		uint32_t DESKEW_CODE3                    : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_88 {
	uint32_t raw;
	struct {
		uint32_t DESKEW_CODE4                    : 8;
		uint32_t DESKEW_CODE5                    : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_90 {
	uint32_t raw;
	struct {
		uint32_t PD_RT                           : 6;
		uint32_t _rsv_6                          : 10;
		uint32_t FORCE_PD_RT                     : 6;
	} bits;
};

union REG_SENSOR_PHY_TOP_94 {
	uint32_t raw;
	struct {
		uint32_t PD_RXAFE_IB                     : 6;
		uint32_t _rsv_6                          : 10;
		uint32_t FORCE_PD_RXAFE_IB               : 6;
	} bits;
};

union REG_SENSOR_PHY_TOP_A0 {
	uint32_t raw;
	struct {
		uint32_t CAM0_VTT                        : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CAM0_VS_STR                     : 14;
	} bits;
};

union REG_SENSOR_PHY_TOP_A4 {
	uint32_t raw;
	struct {
		uint32_t CAM0_VS_STP                     : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CAM0_HTT                        : 14;
	} bits;
};

union REG_SENSOR_PHY_TOP_A8 {
	uint32_t raw;
	struct {
		uint32_t CAM0_HS_STR                     : 14;
		uint32_t _rsv_14                         : 2;
		uint32_t CAM0_HS_STP                     : 14;
	} bits;
};

union REG_SENSOR_PHY_TOP_AC {
	uint32_t raw;
	struct {
		uint32_t CAM0_VS_POL                     : 1;
		uint32_t CAM0_HS_POL                     : 1;
		uint32_t CAM0_TGEN_EN                    : 1;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_100 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_0                         : 8;
		uint32_t DUMMY_1                         : 8;
		uint32_t DUMMY_2                         : 8;
		uint32_t DUMMY_3                         : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_104 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_4                         : 8;
		uint32_t DUMMY_5                         : 8;
		uint32_t DUMMY_6                         : 8;
		uint32_t DUMMY_7                         : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_108 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_8                         : 8;
		uint32_t DUMMY_9                         : 8;
		uint32_t DUMMY_10                        : 8;
		uint32_t DUMMY_11                        : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_10C {
	uint32_t raw;
	struct {
		uint32_t DUMMY_12                        : 8;
		uint32_t DUMMY_13                        : 8;
		uint32_t DUMMY_14                        : 8;
		uint32_t DUMMY_15                        : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_110 {
	uint32_t raw;
	struct {
		uint32_t RO_DESKEW_CODE0                 : 8;
		uint32_t RO_DESKEW_CODE1                 : 8;
		uint32_t RO_DESKEW_CODE2                 : 8;
		uint32_t RO_DESKEW_CODE3                 : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_114 {
	uint32_t raw;
	struct {
		uint32_t RO_DESKEW_CODE4                 : 8;
		uint32_t RO_DESKEW_CODE5                 : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_DFT_11C {
	uint32_t raw;
	struct {
		uint32_t RO_PD_RT                        : 6;
		uint32_t _rsv_6                          : 10;
		uint32_t RO_PD_RXAFE_IB                  : 6;
	} bits;
};

union REG_SENSOR_PHY_TOP_DBG_12 {
	uint32_t raw;
	struct {
		uint32_t DBG_SEL                         : 16;
		uint32_t DBG_CK_SEL                      : 8;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_0 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE0_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_1 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE0_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_2 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE1_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_3 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE1_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_4 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE2_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_5 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE2_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_6 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE3_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_7 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE3_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_8 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE4_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_9 {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE4_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_A {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE5_L            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_TEST_B {
	uint32_t raw;
	struct {
		uint32_t MIPIRX_TEST_RXAFE5_H            : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D0 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D0                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D1 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D1                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D2 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D2                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D3 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D3                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D4 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D4                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D5 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D5                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D6 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D6                        : 32;
	} bits;
};

union REG_SENSOR_PHY_TOP_DUMMY_D7 {
	uint32_t raw;
	struct {
		uint32_t DUMMY_D7                        : 32;
	} bits;
};

#endif // _REG_FIELDS_CSI_WRAP_H_
