/*
 * Copyright CviTek Inc.
 *
 * Created Time: May, 2021
 */
#ifndef __CVI_CIF_H__
#define __CVI_CIF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef RUN_IN_SRAM
#include "drv/cif_drv.h"

#elif (RUN_TYPE == CVIRTOS)
#include "cif_drv.h"
#endif

#include "gpio.h"
#include "cif_uapi.h"
#include "sensor.h"

#define CIF_MAX_CSI_NUM		2
#define CIF_MAX_MAC_NUM		3

#define CSIMAC0_INTR_NUM	(22)
#define CSIMAC1_INTR_NUM	(23)

/* Register Base Address */
#define DPHY_TOP_BASE		(0x0A0D0000)
#define DPHY_4L_BASE		(0x0A0D0300)
#define DPHY_2L_BASE		(0x0A0D0600)
#define SENSOR_MAC0_BASE	(0x0A0C2000)
#define SENSOR_MAC1_BASE	(0x0A0C4000)
#define SENSOR_MAC_VI_BASE	(0x0A0C6000)

#define SENSOR_CSI0_BASE	(0x0A0C2400)
#define SENSOR_CSI1_BASE	(0x0A0C4400)

// CAM_PLL
#define CLK_CAM0_SRC_DIV	(0x030028C0)
#define CLK_CAM1_SRC_DIV	(0x030028C4)

/* Register fileds */
#define DPHY_TOP_REG_04		(DPHY_TOP_BASE + 0x04)
#define DPHY_TOP_REG_30		(DPHY_TOP_BASE + 0x30)

#define DPHY_4L_REG_04		(DPHY_4L_BASE + 0x04)
#define DPHY_4L_REG_08		(DPHY_4L_BASE + 0x08)
#define DPHY_4L_REG_0C		(DPHY_4L_BASE + 0x0C)
#define DPHY_4L_REG_10		(DPHY_4L_BASE + 0x10)

#define DPHY_2L_REG_04		(DPHY_2L_BASE + 0x04)
#define DPHY_2L_REG_08		(DPHY_2L_BASE + 0x08)
#define DPHY_2L_REG_0C		(DPHY_2L_BASE + 0x0C)
#define DPHY_2L_REG_10		(DPHY_2L_BASE + 0x10)

// CAM_PLL
#define REG_CAM_DIV_DIS		(4)
#define REG_CAM_SRC		(8)
#define REG_CAM_SRC_MASK	(3 << REG_CAM_SRC)
#define REG_CAM_DIV		(16)
#define REG_CAM_DIV_MASK	(0x3F << REG_CAM_DIV)

/* Macro */
#define CSI_HDR_ID_VALUE	(0xfff00)
#define CSI_HDR_VC_VALUE	(0xdff00)
#define CSI_LINEAR_VALUE	(0xcff00)

#define CIF_PHY_LANE_NUM	6

#define MAX_PAD_NUM		19

#define ENOIOCTLCMD		515 /* No ioctl command */

/* Struct */
struct cvi_csi_status {
	unsigned int			errcnt_ecc;
	unsigned int			errcnt_crc;
	unsigned int			errcnt_hdr;
	unsigned int			errcnt_wc;
	unsigned int			fifo_full;
};

struct cvi_lvds_status {
	unsigned int			fifo_full;
};

struct cvi_link {
	struct cif_ctx			cif_ctx;
	unsigned int			irq_num;
	// struct reset_control		*phy_reset;
	// struct reset_control		*phy_apb_reset;
	unsigned int			is_on;
	struct cif_param		param;
	struct combo_dev_attr_s		attr;
	enum clk_edge_e			clk_edge;
	enum output_msb_e		msb;
	unsigned int			crop_top;
	unsigned int			distance_fp;
	int				snsr_rst_pin;
	enum of_gpio_flags		snsr_rst_pol;
	union {
		struct cvi_csi_status	sts_csi;
		struct cvi_lvds_status	sts_lvds;
	};
//	struct device			*dev;
	enum rx_mac_clk_e		mac_clk;
	enum ttl_bt_fmt_out		bt_fmt_out;
};

struct cvi_cam_clk {
	int				is_on;
	// struct clk			*clk_o;
};

struct cvi_cif_dev {
	uint32_t		devno;
	// struct miscdevice	miscdev;
	// spinlock_t		lock;
	// struct mutex		mutex;
	struct cvi_link		link[MAX_LINK_NUM];
	// struct cvi_cam_clk	clk_cam0;
	// struct cvi_cam_clk	clk_cam1;
	// struct cvi_cam_clk	vip_sys2;
	// struct cvi_cam_clk	clk_div_0_src_vip_sys_2; /* mipipll */
	// struct cvi_cam_clk	clk_div_1_src_vip_sys_2; /* fpll */
	unsigned int		max_mac_clk;
	void			*pad_ctrl;
};

#endif //__CVI_CIF_H__
