/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_vi.c
 * Description:
 *   Common sample code for video input.
 */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include "cvi_mipi.h"
#include "cvi_sns_ctrl.h"
#include "cvi_common.h"
//#include "cvi_awb_comm.h"
//#include "cvi_comm_isp.h"
#include "sample_comm.h"
#include "cvi_isp.h"
#include "mmio.h"

#if 0
VI_DEV_ATTR_S DEV_ATTR_NEXTCHIP_N5_2M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2020_2M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_OS08A20_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_OS08A20_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2592,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1944,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2592, 1944},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX327_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX307_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		2160
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX335_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2592,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1944,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2592, 1944},
	{
		WDR_MODE_NONE,
		1944
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_IMX335_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_F35_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_H65_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};


VI_DEV_ATTR_S DEV_ATTR_IMX297_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_2To1_LINE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX297_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MCS369Q_4M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MCS369_2M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MM308M2_2M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PICO_640_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            632,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            479,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{632, 479},
	{
		WDR_MODE_2To1_LINE,
		479
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PICO_384_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            384,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            288,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{384, 288},
	{
		WDR_MODE_2To1_LINE,
		288
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX327_SUBLVDS_2M_BASE = {
	VI_MODE_LVDS,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX307_SUBLVDS_2M_BASE = {
	VI_MODE_LVDS,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_SC3335_3M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2304,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1296,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2304, 1296},
	{
		WDR_MODE_NONE,
		1296
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC8238_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		2160
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC4210_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_PIPE_ATTR_S PIPE_ATTR_1280x720_RAW12_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	1280, 720,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_12,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	1920, 1080,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_12,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	3840, 2160,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_10,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	2592, 1944,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_10,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_384x288_422_SDR8_LINEAR = {
	{384, 288},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_632x479_422_SDR8_LINEAR = {
	{632, 479},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1280x720_420_SDR8_LINEAR = {
	{1280, 720},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR = {
	{1920, 1080},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR = {
	{1920, 1080},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2560x1440_422_SDR8_LINEAR = {
	{2560, 1440},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2560x1440_420_SDR8_LINEAR = {
	{2560, 1440},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2592x1944_420_SDR8_LINEAR = {
	{2592, 1944},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR = {
	{3840, 2160},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,      0,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2304x1296_420_SDR8_LINEAR = {
	{2304, 1296},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	0,	0,
	1,
	{ -1, -1}
};

/*
 * Brief: get picture size(w*h), according enPicSize
 */
CVI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
	switch (enPicSize) {
	case PIC_CIF:   /* 352 * 288 */
		pstSize->u32Width  = 352;
		pstSize->u32Height = 288;
		break;

	case PIC_D1_PAL:   /* 720 * 576 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 576;
		break;

	case PIC_D1_NTSC:   /* 720 * 480 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 480;
		break;

	case PIC_720P:   /* 1280 * 720 */
		pstSize->u32Width  = 1280;
		pstSize->u32Height = 720;
		break;

	case PIC_1080P:  /* 1920 * 1080 */
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1080;
		break;

	case PIC_1088:  /* 1920 * 1088*/
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1088;
		break;

	case PIC_1440P:  /* 2560 * 1440 */
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1440;
		break;

	case PIC_2304x1296:
		pstSize->u32Width  = 2304;
		pstSize->u32Height = 1296;
		break;

	case PIC_2592x1520:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1520;
		break;

	case PIC_2592x1944:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1944;
		break;

	case PIC_2592x1536:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1536;
		break;

	case PIC_2716x1524:
		pstSize->u32Width  = 2716;
		pstSize->u32Height = 1524;
		break;

	case PIC_3840x2160:
		pstSize->u32Width  = 3840;
		pstSize->u32Height = 2160;
		break;

	case PIC_3000x3000:
		pstSize->u32Width  = 3000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4000x3000:
		pstSize->u32Width  = 4000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4096x2160:
		pstSize->u32Width  = 4096;
		pstSize->u32Height = 2160;
		break;

	case PIC_3840x8640:
		pstSize->u32Width = 3840;
		pstSize->u32Height = 8640;
		break;

	case PIC_640x480:
		pstSize->u32Width = 640;
		pstSize->u32Height = 480;
		break;
	case PIC_479P:  /* 632 * 479 */
		pstSize->u32Width  = 632;
		pstSize->u32Height = 479;
		break;
	case PIC_288P:  /* 384 * 288 */
		pstSize->u32Width  = 384;
		pstSize->u32Height = 288;
		break;
	default:
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}
#endif

void SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;

	for (i = 0; i < VI_MAX_DEV_NUM; i++) {
		pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
		pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
		pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
		//memset(&pstViConfig->astViInfo[i].stSnapInfo, 0, sizeof(SAMPLE_SNAP_INFO_S));
		pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = CVI_FALSE;
		pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = CVI_FALSE;
	}

	pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
	pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
}

#if 0
CVI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr)
{
	switch (enSnsType) {
	default:
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX297_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_5M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_F35_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_F35_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_H65_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstViDevAttr, &DEV_ATTR_PICO_640_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstViDevAttr, &DEV_ATTR_PICO_384_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_SUBLVDS_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_SUBLVDS_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MCS369Q_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MCS369_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MM308M2_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_NEXTCHIP_N5_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC3335_3M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2020_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC8238_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC8238_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC4210_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC4210_4M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S *pstChnAttr)
{
	switch (enSnsType) {
	default:
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstChnAttr, &CHN_ATTR_632x479_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstChnAttr, &CHN_ATTR_384x288_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2304x1296_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	}
	return CVI_SUCCESS;
}
#endif

CVI_S32 SAMPLE_COMM_VI_ResetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_ResetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "mipi %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("mipi %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	SAMPLE_SNS_TYPE_E enSnsType;

	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		enSnsType = g_enSnsType[u32SnsId];
		/* need to invert the clk for timnig issue. */
		if (enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT ||
				enSnsType == VIVO_MCS369_2M_30FPS_12BIT)
			CVI_MIPI_SetClkEdge(ViPipe, 0);
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		CVI_MIPI_SetMipiAttr(ViPipe, (CVI_VOID *)&stDevAttr);
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_EnableSensorClock(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorClock(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d clock enable failed!\n", i);
			return s32Ret;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StartSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_U32 s32Ret;
	CVI_S32 i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		s32Ret = SAMPLE_COMM_ISP_SetSnsObj(u32SnsId, pstViInfo->stSnsInfo.enSnsType);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor obj(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_SetSnsInit(u32SnsId, pstViInfo->stSnsInfo.u8HwSync);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor(%d) hwsync failed !\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_PatchSnsObj(u32SnsId, &pstViInfo->stSnsInfo);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "patch rx attr(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId, pstViInfo->stSnsInfo.s32BusId);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
			return s32Ret;
		}
	}
	s32Ret = SAMPLE_COMM_ISP_SetSensorMode(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	/*TODO@CF. Need add sample function.*/
	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		SAMPLE_PRT("sensor %d stDevAttr.devno %d\n", i, stDevAttr.devno);
		pstViInfo->stSnsInfo.MipiDev = stDevAttr.devno;
	}
	//asm volatile ("b .");
	s32Ret = SAMPLE_COMM_VI_ResetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_ResetMipi(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetMipi failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_SetMipiAttr failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_EnableSensorClock(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "EnableSensorClock failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	usleep(20);
	s32Ret = SAMPLE_COMM_VI_UnresetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "UnresetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}
#if 0
CVI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret;
	VI_DEV              ViDev;
	SAMPLE_SNS_TYPE_E   enSnsType;
	VI_DEV_ATTR_S       stViDevAttr;

	ViDev       = pstViInfo->stDevInfo.ViDev;
	enSnsType   = pstViInfo->stSnsInfo.enSnsType;

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;

	s32Ret = CVI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetDevAttr failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_EnableDev(ViDev);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret;
	VI_DEV ViDev;

	ViDev   = pstViInfo->stDevInfo.ViDev;
	s32Ret  = CVI_VI_DisableDev(ViDev);

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             i;
	CVI_S32             s32PipeCnt = 0;
	CVI_S32             s32Ret;
	VI_DEV_BIND_PIPE_S  stDevBindPipe = {0};

	for (i = 0; i < 4; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
			s32PipeCnt++;
			stDevBindPipe.u32Num = s32PipeCnt;
		}
	}

	s32Ret = CVI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

/******************************************************************************
 * funciton : Get enSize by diffrent sensor
 ******************************************************************************/
CVI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (!penSize)
		return CVI_FAILURE;

	switch (enMode) {
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		*penSize = PIC_720P;
		break;
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
		*penSize = PIC_1080P;
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		*penSize = PIC_3840x2160;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_2592x1944;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_1440P;
		break;
	case PICO640_THERMAL_479P:
		*penSize = PIC_479P;
		break;
	case PICO384_THERMAL_384X288:
		*penSize = PIC_288P;
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		*penSize = PIC_1440P;
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		*penSize = PIC_2304x1296;
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		*penSize = PIC_3840x2160;
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32             i;
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn = 0;
	VI_CHN_ATTR_S       stChnAttr;
	VI_VPSS_MODE_E      enMastPipeMode;
	VI_DEV_ATTR_S       stViDevAttr;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		if (i < VI_MAX_CHN_NUM) {
			ViPipe	    = pstViConfig->astViInfo[i].stPipeInfo.aPipe[0];
			ViChn	    = pstViConfig->astViInfo[i].stChnInfo.ViChn;

			SAMPLE_COMM_VI_GetDevAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stViDevAttr);
			SAMPLE_COMM_VI_GetChnAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stChnAttr);
			stChnAttr.enDynamicRange = pstViConfig->astViInfo[i].stChnInfo.enDynamicRange;
			stChnAttr.enVideoFormat  = pstViConfig->astViInfo[i].stChnInfo.enVideoFormat;
			stChnAttr.enCompressMode = pstViConfig->astViInfo[i].stChnInfo.enCompressMode;
			stChnAttr.bLVDSflow = (stViDevAttr.enIntfMode == VI_MODE_LVDS) ? 1 : 0;
			stChnAttr.u8TotalChnNum = pstViConfig->s32WorkingViNum;

			s32Ret = CVI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetChnAttr failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			enMastPipeMode = pstViConfig->astViInfo[i].stPipeInfo.enMastPipeMode;

			if (enMastPipeMode == VI_OFFLINE_VPSS_OFFLINE
			    || enMastPipeMode == VI_ONLINE_VPSS_OFFLINE) {
				s32Ret = CVI_VI_EnableChn(ViPipe, ViChn);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableChn failed with %#x!\n",
									s32Ret);
					return CVI_FAILURE;
				}
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn;
	VI_VPSS_MODE_E      enMastPipeMode;

	ViChn  = pstViInfo->stChnInfo.ViChn;

	if (ViChn < VI_MAX_CHN_NUM) {
		enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

		if (enMastPipeMode == VI_OFFLINE_VPSS_OFFLINE
		    || enMastPipeMode == VI_ONLINE_VPSS_OFFLINE) {
			s32Ret = CVI_VI_DisableChn(ViPipe, ViChn);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableChn failed with %#x!\n",
								s32Ret);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;
	CVI_S32 s32ViNum;
	CVI_S32 s32Ret = CVI_SUCCESS;

	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);

		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
			return CVI_FAILURE;
		}
	}

	s32Ret = SAMPLE_COMM_ISP_Run(0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ISP_Run failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe = 0;
	ISP_PUB_ATTR_S stPubAttr;
	ISP_STATISTICS_CFG_S stsCfg;
	ISP_BIND_ATTR_S stBindAttr;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  &&
			pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[0];
#if USE_USER_SEN_DRIVER
			SAMPLE_COMM_ISP_Aelib_Callback(ViPipe);
#else
			ae0_register_callback(ViPipe);
#endif
			SAMPLE_COMM_ISP_Awblib_Callback(ViPipe);

			snprintf(stBindAttr.stAeLib.acLibName, sizeof(CVI_AE_LIB_NAME), "%s", CVI_AE_LIB_NAME);
			stBindAttr.stAeLib.s32Id = ViPipe;
			stBindAttr.sensorId = 0;
			snprintf(stBindAttr.stAwbLib.acLibName, sizeof(CVI_AWB_LIB_NAME), "%s", CVI_AWB_LIB_NAME);
			stBindAttr.stAwbLib.s32Id = ViPipe;
			s32Ret = CVI_ISP_SetBindAttr(ViPipe, &stBindAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "Bind Algo failed with %#x!\n", s32Ret);
			}
			s32Ret = CVI_ISP_MemInit(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "Init Ext memory failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
			s32Ret = CVI_ISP_SetPubAttr(ViPipe, &stPubAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SetPubAttr failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			stsCfg.stAECfg.stCrop[0].bEnable = stsCfg.stAECfg.stCrop[1].bEnable = 1;
			stsCfg.stAECfg.stCrop[0].u16X = stsCfg.stAECfg.stCrop[0].u16Y = 0;
			stsCfg.stAECfg.stCrop[0].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[0].u16H = stPubAttr.stWndRect.u32Height;
			stsCfg.stAECfg.stCrop[1].u16X = stsCfg.stAECfg.stCrop[1].u16Y = 0;
			stsCfg.stAECfg.stCrop[1].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[1].u16H = stPubAttr.stWndRect.u32Height;
			stsCfg.stWBCfg.u16ZoneRow = AWB_ZONE_ORIG_ROW;
			stsCfg.stWBCfg.u16ZoneCol = AWB_ZONE_ORIG_COLUMN;
			stsCfg.stWBCfg.stCrop.u16X = stsCfg.stWBCfg.stCrop.u16Y = 0;
			stsCfg.stWBCfg.stCrop.u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stWBCfg.stCrop.u16H = stPubAttr.stWndRect.u32Height;
			stsCfg.stWBCfg.u16BlackLevel = 0;
			stsCfg.stWBCfg.u16WhiteLevel = 4095;
			stsCfg.stFocusCfg.stConfig.bEnable = 1;
			stsCfg.stFocusCfg.stConfig.u8HFltShift = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[0] = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[1] = 2;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[2] = 3;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[3] = 5;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[4] = 10;
			stsCfg.stFocusCfg.stConfig.stRawCfg.PreGammaEn = 0;
			stsCfg.stFocusCfg.stConfig.stPreFltCfg.PreFltEn = 1;
			stsCfg.stFocusCfg.stConfig.u16Hwnd = 17;
			stsCfg.stFocusCfg.stConfig.u16Vwnd = 15;
			stsCfg.stFocusCfg.stConfig.stCrop.bEnable = 1;
			// AF offset and size has some limitation.
			stsCfg.stFocusCfg.stConfig.stCrop.u16X = AF_XOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16Y = AF_YOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16W = stPubAttr.stWndRect.u32Width - AF_XOFFSET_MIN * 2;
			stsCfg.stFocusCfg.stConfig.stCrop.u16H = stPubAttr.stWndRect.u32Height - AF_YOFFSET_MIN * 2;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[0] = 1;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[1] = 4;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[2] = 8;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[3] = 16;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[4] = 0;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[0] = 1;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[1] = 2;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[2] = 4;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[3] = 8;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[4] = 0;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[0] = 1;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[1] = 16;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[2] = 0;
			stsCfg.unKey.bit1FEAeGloStat = stsCfg.unKey.bit1FEAeLocStat =
				stsCfg.unKey.bit1AwbStat1 = stsCfg.unKey.bit1AwbStat2 = stsCfg.unKey.bit1FEAfStat = 1;
			s32Ret = CVI_ISP_SetStatisticsConfig(ViPipe, &stsCfg);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Set Statistic failed with %#x!\n", s32Ret);
				return s32Ret;
			}

			s32Ret = CVI_ISP_Init(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Init failed with %#x!\n", s32Ret);
				return s32Ret;
			}
		}
	}
	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe)
{
	CVI_S32  s32Ret;

	s32Ret = CVI_VI_StopPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_StopPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_DestroyPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DestroyPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32  i, ret = CVI_SUCCESS;
	VI_PIPE ViPipe;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[i];
			ret = SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
			if (ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_StopViPipe is fail\n");
				return ret;
			}
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S *pstViInfo)
{
	SAMPLE_COMM_VI_StopViChn(pstViInfo);

	SAMPLE_COMM_VI_StopViPipe(pstViInfo);

	SAMPLE_COMM_VI_StopDev(pstViInfo);

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32           i;
	CVI_S32           s32ViNum;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "null ptr\n");
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_OPEN(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Open();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI reopen err\n");
	}

	return s32ret;
}

CVI_S32 SAMPLE_COMM_VI_CLOSE(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Close();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI close err\n");
	}

	return s32ret;
}
#endif

int sample_snsr_test(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SONY_IMX327_MIPI_2M_30FPS_12BIT;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32 s32WorkSnsId = 0;
	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	CVI_S32 s32Ret = CVI_SUCCESS;
	const ISP_SNS_OBJ_S *pstSnsObj;
	ISP_SENSOR_EXP_FUNC_S stSensorExpFunc;
	uint64_t bayer_w_addr[3] = {0x121000000, 0x122000000, 0x123000000};
	CVI_U16 snsr_w = 1948, snsr_h = 1097;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	//stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	//stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	//stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	//stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	//stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	//stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/* Skip other flow.....*/

	/* clock enable */
	vip_clk_en();
	/* VIP reset */
	vip_isp_clk_reset();

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		printf("start snsr fail\n");
		return s32Ret;
	}
	//s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	//if (s32Ret != CVI_SUCCESS) {
	//	CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
	//	return s32Ret;
	//}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
		printf("start mipi fail\n");
		return s32Ret;
	}
	/************************************************
	 * start sensor
	 ************************************************/
	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(0);
	if (!pstSnsObj) {
		printf("no sensor obj\n");
		return CVI_FAILURE;
	}
	pstSnsObj->pfnExpSensorCb(&stSensorExpFunc);
	stSensorExpFunc.pfn_cmos_sensor_init(0);
	/************************************************
	 * [Hack] configure isp, check result by CVD....
	 ************************************************/

	/* configure preraw out mode */
	mmio_clrsetbits_32(0x0a000000, 0x03, 1);
	/* configure isptop */
	mmio_write_32(0x0a070000, 0xffffffff);
	mmio_setbits_32(0x0a070004, 0x1049);
	mmio_setbits_32(0x0a07000C, 0x9);
	mmio_clrbits_32(0x0a070010, 0x0c);
	mmio_write_32(0x0a070014, (snsr_w - 1) | ((snsr_h - 1) << 16));
	/* configure csibdg */
	mmio_setbits_32(0x0a001000, (1 << 13));
	mmio_clrbits_32(0x0a001000, 0x4);
	mmio_clrbits_32(0x0a001000, 0x30);
	mmio_setbits_32(0x0a001000, 0x40);
	mmio_setbits_32(0x0a001000, 0x40);
	mmio_write_32(0x0a001004, (snsr_w - 1) | ((snsr_h - 1) << 16));
	mmio_clrsetbits_32(0x0a001000, 0x3, 1);
	/* configure wdma */
	mmio_write_32(0x0a001124, (snsr_w - 1) | ((snsr_h - 1) << 16));
	mmio_write_32(0x0a001108, snsr_w * 3 / 2);
	mmio_clrsetbits_32(0x0a001128, 0xFFFF, (snsr_w * 3 / 2 + 0x1F) & ~0x1F);
	mmio_clrsetbits_32(0x0a001128, 0x70000, 3 << 16);
	mmio_write_32(0x0a001100, bayer_w_addr[0] & 0xFFFFFFFF);
	mmio_write_32(0x0a001104, bayer_w_addr[0] >> 32);
	/* configure isp enable */
	mmio_setbits_32(0x0a001008, (1 << 0));
	mmio_setbits_32(0x0a070008, 0x28);
	mmio_setbits_32(0x0a001000, (1 << 24));

	return CVI_SUCCESS;
}

