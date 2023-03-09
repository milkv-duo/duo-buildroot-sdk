#ifndef __U_CVI_VIP_DSI__
#define __U_CVI_VIP_DSI__

#define CMD_MAX_NUM    128
#define RX_MAX_NUM     4
#define LANE_MAX_NUM   5

enum output_mode_e {
	OUTPUT_MODE_CSI            = 0x0,              /* csi mode */
	OUTPUT_MODE_DSI_VIDEO      = 0x1,              /* dsi video mode */
	OUTPUT_MODE_DSI_CMD        = 0x2,              /* dsi command mode */

	OUTPUT_MODE_BUTT
};

enum video_mode_e {
	BURST_MODE                      = 0x0,
	NON_BURST_MODE_SYNC_PULSES      = 0x1,
	NON_BURST_MODE_SYNC_EVENTS      = 0x2,
};

enum output_format_e {
	OUT_FORMAT_RGB_16_BIT          = 0x0,
	OUT_FORMAT_RGB_18_BIT          = 0x1,
	OUT_FORMAT_RGB_24_BIT          = 0x2,
	OUT_FORMAT_RGB_30_BIT          = 0x3,
	OUT_FORMAT_YUV420_8_BIT_NORMAL = 0x4,
	OUT_FORMAT_YUV420_8_BIT_LEGACY = 0x5,
	OUT_FORMAT_YUV422_8_BIT        = 0x6,

	OUT_FORMAT_BUTT
};

enum mipi_tx_lane_id {
	MIPI_TX_LANE_CLK = 0,
	MIPI_TX_LANE_0,
	MIPI_TX_LANE_1,
	MIPI_TX_LANE_2,
	MIPI_TX_LANE_3,
	MIPI_TX_LANE_MAX,
};

struct sync_info_s {
	unsigned short  vid_hsa_pixels;
	unsigned short  vid_hbp_pixels;
	unsigned short  vid_hfp_pixels;
	unsigned short  vid_hline_pixels;
	unsigned short  vid_vsa_lines;
	unsigned short  vid_vbp_lines;
	unsigned short  vid_vfp_lines;
	unsigned short  vid_active_lines;
	unsigned short  edpi_cmd_size;
	bool            vid_vsa_pos_polarity;
	bool            vid_hsa_pos_polarity;
};

/*
 * devno: device number
 * lane_id: -1 - disable
 * output mode: CSI/DSI_VIDEO/DSI_CMD
 * phy_data_rate: mbps
 * pixel_clk: KHz
 */
struct combo_dev_cfg_s {
	unsigned int            devno;
	enum mipi_tx_lane_id    lane_id[LANE_MAX_NUM];
	enum output_mode_e      output_mode;
	enum video_mode_e       video_mode;
	enum output_format_e    output_format;
	struct sync_info_s      sync_info;
	unsigned int            phy_data_rate;
	unsigned int            pixel_clk;
	bool                    lane_pn_swap[LANE_MAX_NUM];
};

struct cmd_info_s {
	unsigned int        devno;                   /* device number */
	unsigned short      data_type;
	unsigned short      cmd_size;
#ifdef __arm__
	unsigned char	    *cmd;
	unsigned int	    padding;
#else
	unsigned char       *cmd;
#endif
};

/*
 * devno: device number
 * data_type: DSI data type
 * data_param: data param,low 8 bit:first param.high 8 bit:second param, set 0 if not use
 * get_data_size: read data size
 * get_data: read data memery address, should  malloc by user
 */
struct get_cmd_info_s {
	unsigned int        devno;
	unsigned short      data_type;
	unsigned short      data_param;
	unsigned short      get_data_size;
#ifdef __arm__
	unsigned int        padding1;
	unsigned char       *get_data;
	unsigned int        padding2;
#else
	unsigned int        padding1;
	unsigned char       *get_data;
#endif
};

struct hs_settle_s {
	unsigned char	    prepare;
	unsigned char	    zero;
	unsigned char	    trail;
};

#define CVI_VIP_MIPI_TX_IOC_MAGIC   't'

#define CVI_VIP_MIPI_TX_SET_DEV_CFG              _IOW(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x01, struct combo_dev_cfg_s)
#define CVI_VIP_MIPI_TX_GET_DEV_CFG              _IOWR(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x01, struct combo_dev_cfg_s)
#define CVI_VIP_MIPI_TX_SET_CMD                  _IOW(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x02, struct cmd_info_s)
#define CVI_VIP_MIPI_TX_ENABLE                   _IO(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x03)
#define CVI_VIP_MIPI_TX_GET_CMD                  _IOWR(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x04, struct get_cmd_info_s)
#define CVI_VIP_MIPI_TX_DISABLE                  _IO(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x05)
#define CVI_VIP_MIPI_TX_SET_HS_SETTLE            _IOW(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x06, struct hs_settle_s)
#define CVI_VIP_MIPI_TX_GET_HS_SETTLE            _IOWR(CVI_VIP_MIPI_TX_IOC_MAGIC, 0x06, struct hs_settle_s)

#endif // __U_CVI_VIP_DSI__
