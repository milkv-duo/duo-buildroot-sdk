#ifndef _U_CVI_VIP_DISP_H_
#define _U_CVI_VIP_DISP_H_

#ifndef LANE_MAX_NUM
#define LANE_MAX_NUM   5
#endif

#define MAX_BT_PINS 20

enum cvi_disp_intf {
	CVI_VIP_DISP_INTF_DSI = 0,
	CVI_VIP_DISP_INTF_BT,
	CVI_VIP_DISP_INTF_I80,
	CVI_VIP_DISP_INTF_LVDS,
	CVI_VIP_DISP_INTF_MAX,
};

struct cvi_dsi_intf_cfg {
	__u32 pixelclock;
	__u8 bitsperdata;
	__u8 lane_id[4];
	__u8 lane_pn_swap[4];
};

enum LVDS_OUT_BIT {
	LVDS_OUT_6BIT = 0,
	LVDS_OUT_8BIT,
	LVDS_OUT_10BIT,
	LVDS_OUT_MAX,
};

enum LVDS_MODE {
	LVDS_MODE_JEIDA = 0,
	LVDS_MODE_VESA,
	LVDS_MODE_MAX,
};

/*
 * @pixelclock: pixel clock in kHz
 */
struct cvi_lvds_intf_cfg {
	__u32 pixelclock;
	enum LVDS_OUT_BIT out_bits;
	enum LVDS_MODE mode;
	__u8 chn_num;
	__u8 vs_out_en;
	__u8 hs_out_en;
	__u8 hs_blk_en;
	__u8 msb_lsb_data_swap;
	__u8 serial_msb_first;
	__u8 even_odd_link_swap;
	__u8 enable;
	__s8 lane_id[LANE_MAX_NUM];
	__u8 lane_pn_swap[LANE_MAX_NUM];
	__u32 backlight_gpio_num;
	__s8 backlight_avtive;
};

enum sclr_top_vo_mux {
	SCLR_VO_MUX_BT_VS = 0,
	SCLR_VO_MUX_BT_HS,
	SCLR_VO_MUX_BT_HDE,
	SCLR_VO_MUX_BT_DATA0,
	SCLR_VO_MUX_BT_DATA1,
	SCLR_VO_MUX_BT_DATA2,
	SCLR_VO_MUX_BT_DATA3,
	SCLR_VO_MUX_BT_DATA4,
	SCLR_VO_MUX_BT_DATA5,
	SCLR_VO_MUX_BT_DATA6,
	SCLR_VO_MUX_BT_DATA7,
	SCLR_VO_MUX_BT_DATA8,
	SCLR_VO_MUX_BT_DATA9,
	SCLR_VO_MUX_BT_DATA10,
	SCLR_VO_MUX_BT_DATA11,
	SCLR_VO_MUX_BT_DATA12,
	SCLR_VO_MUX_BT_DATA13,
	SCLR_VO_MUX_BT_DATA14,
	SCLR_VO_MUX_BT_DATA15,
	SCLR_VO_MUX_TG_HS_TILE = 30,
	SCLR_VO_MUX_TG_VS_TILE,
	SCLR_VO_MUX_MAX,
};

enum sclr_top_vo_sel {
	SCLR_VO_CLK0 = 0,
	SCLR_VO_CLK1,
	SCLR_VO_D0,
	SCLR_VO_D1,
	SCLR_VO_D2,
	SCLR_VO_D3,
	SCLR_VO_D4,
	SCLR_VO_D5,
	SCLR_VO_D6,
	SCLR_VO_D7,
	SCLR_VO_D8,
	SCLR_VO_D9,
	SCLR_VO_D10,
	SCLR_VO_D11,
	SCLR_VO_D12,
	SCLR_VO_D13,
	SCLR_VO_D14,
	SCLR_VO_D15,
	SCLR_VO_D16,
	SCLR_VO_D17,
	SCLR_VO_D18,
	SCLR_VO_D19,
	SCLR_VO_D20,
	SCLR_VO_D21,
	SCLR_VO_D22,
	SCLR_VO_D23,
	SCLR_VO_D24,
	SCLR_VO_D25,
	SCLR_VO_D26,
	SCLR_VO_D27,
	SCLR_VO_D_MAX,
};

enum sclr_top_vo_d_sel {
	SCLR_VO_VIVO_D0 = SCLR_VO_D13,
	SCLR_VO_VIVO_D1 = SCLR_VO_D14,
	SCLR_VO_VIVO_D2 = SCLR_VO_D15,
	SCLR_VO_VIVO_D3 = SCLR_VO_D16,
	SCLR_VO_VIVO_D4 = SCLR_VO_D17,
	SCLR_VO_VIVO_D5 = SCLR_VO_D18,
	SCLR_VO_VIVO_D6 = SCLR_VO_D19,
	SCLR_VO_VIVO_D7 = SCLR_VO_D20,
	SCLR_VO_VIVO_D8 = SCLR_VO_D21,
	SCLR_VO_VIVO_D9 = SCLR_VO_D22,
	SCLR_VO_VIVO_D10 = SCLR_VO_D23,
	SCLR_VO_VIVO_CLK = SCLR_VO_CLK1,
	SCLR_VO_MIPI_TXM4 = SCLR_VO_D24,
	SCLR_VO_MIPI_TXP4 = SCLR_VO_D25,
	SCLR_VO_MIPI_TXM3 = SCLR_VO_D26,
	SCLR_VO_MIPI_TXP3 = SCLR_VO_D27,
	SCLR_VO_MIPI_TXM2 = SCLR_VO_D0,
	SCLR_VO_MIPI_TXP2 = SCLR_VO_CLK0,
	SCLR_VO_MIPI_TXM1 = SCLR_VO_D2,
	SCLR_VO_MIPI_TXP1 = SCLR_VO_D1,
	SCLR_VO_MIPI_TXM0 = SCLR_VO_D4,
	SCLR_VO_MIPI_TXP0 = SCLR_VO_D3,
	SCLR_VO_MIPI_RXN5 = SCLR_VO_D12,
	SCLR_VO_MIPI_RXP5 = SCLR_VO_D11,
	SCLR_VO_MIPI_RXN2 = SCLR_VO_D10,
	SCLR_VO_MIPI_RXP2 = SCLR_VO_D9,
	SCLR_VO_MIPI_RXN1 = SCLR_VO_D8,
	SCLR_VO_MIPI_RXP1 = SCLR_VO_D7,
	SCLR_VO_MIPI_RXN0 = SCLR_VO_D6,
	SCLR_VO_MIPI_RXP0 = SCLR_VO_D5,
	SCLR_VO_PAD_MAX = SCLR_VO_D_MAX
};

struct vo_d_remap {
	enum sclr_top_vo_d_sel sel;
	enum sclr_top_vo_mux mux;
};

struct bt_pins {
	unsigned char pin_num;
	struct vo_d_remap d_pins[MAX_BT_PINS];
};

enum BT_MODE {
	BT_MODE_656 = 0,
	BT_MODE_1120,
	BT_MODE_601,
	BT_MODE_MAX,
};

enum BT_CLK_MODE {
	BT_CLK_MODE_27M = 0,
	BT_CLK_MODE_36M,
	BT_CLK_MODE_37P125M,
	BT_CLK_MODE_72M,
	BT_CLK_MODE_74P25M,
	BT_CLK_MODE_148P5M,
};

/*
 * @pixelclock: pixel clock in kHz
 */
struct cvi_bt_intf_cfg {
	__u8 bt_clk;
	enum BT_MODE mode;
	struct bt_pins pins;
};

struct cvi_disp_intf_cfg {
	enum cvi_disp_intf intf_type;
	union {
		struct cvi_dsi_intf_cfg dsi_cfg;
		struct cvi_lvds_intf_cfg lvds_cfg;
		struct cvi_bt_intf_cfg bt_cfg;
	};
};

#endif	// _U_CVI_VIP_DISP_H_
