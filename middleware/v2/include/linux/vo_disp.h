#ifndef _U_CVI_VIP_DISP_H_
#define _U_CVI_VIP_DISP_H_

#ifndef LANE_MAX_NUM
#define LANE_MAX_NUM   5
#endif

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

enum BT_MODE {
	BT_MODE_656 = 0,
	BT_MODE_1120,
	BT_MODE_601,
	BT_MODE_MAX,
};

/*
 * @pixelclock: pixel clock in kHz
 */
struct cvi_bt_intf_cfg {
	__u32 pixelclock;
	enum BT_MODE mode;
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
