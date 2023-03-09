#ifndef __U_VI_SNSR_H__
#define __U_VI_SNSR_H__

#include <linux/cif_uapi.h>

#define MAX_WDR_FRAME_NUM	2
#define ISP_MAX_SNS_REGS	32

/**
 * struct active_size_s - linear/wdr image information
 *
 * @width: image total width
 * @height: image total height
 * @start_x: horizontal shift of the 1st pixel
 * @start_y: horizontal shift of the 1st pixel
 * @active_w: effective image width
 * @active_h: effective image height
 * @max_width: max width for buffer allocation
 * @max_height: max height for buffer allocation
 */

struct active_size_s {
	unsigned short		width;
	unsigned short		height;
	unsigned short		start_x;
	unsigned short		start_y;
	unsigned short		active_w;
	unsigned short		active_h;
	unsigned short		max_width;
	unsigned short		max_height;
};

/**
 * struct wdr_size_s - structure for CVI_SNSR_G_WDR_SIZE
 *
 * @frm_num: [output] Effective image instance. 1 for linear mode, >1 for wdr mode.
 * @img_size: [output] Image information.
 */

struct wdr_size_s {
	unsigned int		frm_num;
	struct active_size_s	img_size[MAX_WDR_FRAME_NUM];
};

enum isp_sns_type_e {
	ISP_SNS_I2C_TYPE = 0,
	ISP_SNS_SSP_TYPE,
	ISP_SNS_TYPE_BUTT,
};

enum sns_wdr_e {
	SNS_WDR_MODE_NONE = 0,
	SNS_WDR_MODE_2TO1_LINE,
	SNS_WDR_MODE_BUTT
};

/**
 * struct isp_i2c_data - sensor setting with i2c interface.
 *
 * @update: update this register or not
 * @drop_frame: drop next frame or not
 * @i2c_dev: i2c device number.
 * @dev_addr: sensor slave address
 * @dly_frm_num: this setting would be set with delay frame number
 * @drop_frame_cnt: this setting would be set with drop frame
 * @reg_addr: sensor register address
 * @addr_bytes: sensor register address bytes number
 * @data: sensor register value
 * @data_bytes: sensor register value bytes number
 */

struct isp_i2c_data {
	unsigned char	update;
	unsigned char	drop_frame;
	unsigned char	vblank_update;
	unsigned char	i2c_dev;
	unsigned char	dev_addr;
	unsigned char	dly_frm_num;
	unsigned short	drop_frame_cnt;
	unsigned short	reg_addr;
	unsigned short	addr_bytes;
	unsigned short	data;
	unsigned short	data_bytes;
};

/**
 * struct snsr_regs_s - structure of sensor update wrapper
 *
 * @sns_type: i2c or other interface
 * @magic_num: the frame number to updated
 * @magic_num_dly: the frame number to updated on synthetic hdr mode
 * @magic_num_vblank: the frame number to updated on vblank time
 * @regs_num: the maximum sensor registers to be updated
 * @i2c_data: sensor registers to be updated
 * @use_snsr_sram: does this sensor support group update
 * @need_update: global flag for sensor update. Ignore this wrapper
 *	when it is zero.
 */

struct snsr_regs_s {
	enum isp_sns_type_e	sns_type;
	unsigned int		magic_num;
	unsigned int		magic_num_dly;
	unsigned int		magic_num_vblank;
	unsigned int		regs_num;
	struct isp_i2c_data	i2c_data[ISP_MAX_SNS_REGS];
	unsigned char		cfg_valid_max_dly_frm;
	unsigned char		use_snsr_sram;
	unsigned char		need_update;
};

/**
 * struct snsr_isp_s - structure of isp update wrapper
 *
 * @wdr: the image information for isp driver.
 * @need_update: global flag for isp update. Ignore this wrapper
 *	when it is zero.
 */

struct snsr_isp_s {
	struct wdr_size_s	wdr;
	unsigned char		dly_frm_num;
	unsigned char		need_update;
};

/**
 * struct snsr_cif_s - structure of cif(mipi_rx) update wrapper
 *
 * @wdr: the image information for isp driver.
 * @need_update: global flag for cif update. Ignore this wrapper
 *	when it is zero.
 */

struct snsr_cif_s {
	struct manual_wdr_s	wdr_manu;
	unsigned char		dly_frm_num;
	unsigned char		need_update;
};

/**
 * struct snsr_cfg_node_s - structure of cfg node for runtime update
 *
 * @snsr: [output] snsr wrapper for runtime update
 * @isp: [output] isp wrapper for runtime update
 * @cif: [output] cif wrapper for runtime update
 * @configed: [intput] after CVI_SNSR_G_CFG_NODE is called, this flag
 *	is set as false by sensor driver. The caller shall set it as
 *	true after this cfg_node is passed to isp driver.
 */

struct snsr_cfg_node_s {
	struct snsr_regs_s	snsr;
	struct snsr_isp_s	isp;
	struct snsr_cif_s	cif;
	unsigned char		configed;
};

#endif // __U_VI_SNSR_H__
