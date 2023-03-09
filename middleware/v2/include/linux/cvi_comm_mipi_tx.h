/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common_mipi_tx.h
 * Description:
 */

#ifndef __CVI_COMM_MIPI_TX_H__
#define __CVI_COMM_MIPI_TX_H__

#define MIPI_TX_NAME "/dev/cvi-mipi-tx"

struct dsc_instr {
	CVI_U8	delay;
	CVI_U8  data_type;
	CVI_U8	size;
	CVI_U8	*data;
};

struct mipi_hs_timing_cfg {
	CVI_U8 prepare;
	CVI_U8 zero;
	CVI_U8 trail;
};

#endif // __CVI_COMM_MIPI_TX_H__

