#ifndef __VI_DEFINES_H__
#define __VI_DEFINES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vi_tun_cfg.h>
#include <vi_isp.h>
#include <vi_drv.h>

#define FPGA_TEST

#define VI_REG_ADDR_BASE	0x0A000000
#define VI_IRQ_NUM		20

/**
 * struct cvi_vi - VI IP abstraction
 */
struct cvi_vi_dev {
	void				*reg_base;
	int				irq_num;
	struct isp_ctx			ctx;
	struct cvi_isp_mbus_framefmt	usr_fmt;
	struct cvi_isp_rect		usr_crop;
	//struct cvi_isp_snr_info		snr_info[ISP_PRERAW_MAX];
	u32				pre_fe_sof_cnt[ISP_PRERAW_MAX][ISP_FE_CHN_MAX];
	u32				pre_fe_frm_num[ISP_PRERAW_MAX][ISP_FE_CHN_MAX];
	u32				pre_be_frm_num[ISP_PRERAW_MAX][ISP_BE_CHN_MAX];
	bool				preraw_first_frm[ISP_PRERAW_MAX];
	u32				postraw_frame_number[ISP_PRERAW_MAX];
	u32				drop_frame_number[ISP_PRERAW_MAX];
	u8				pre_fe_state[ISP_PRERAW_MAX][ISP_FE_CHN_MAX];
	u8				pre_be_state[ISP_BE_CHN_MAX];
	volatile u8			postraw_state;
	u8				isp_streamoff;
	u8				isp_streamon;
};
#ifdef __cplusplus
}
#endif

#endif /* __VI_DEFINES_H__ */
