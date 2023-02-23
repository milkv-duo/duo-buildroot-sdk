#ifndef __VI_H__
#define __VI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vi_tun_cfg.h>
#include <vi_isp.h>
#include <vi_uapi.h>
#include <vi_common.h>
#include <vi_defines.h>


#define OFFLINE_RAW_BUF_NUM	2
#define OFFLINE_PRE_BE_BUF_NUM	2
#define OFFLINE_YUV_BUF_NUM	2
#define MAX_RGBMAP_BUF_NUM	3

enum cvi_isp_postraw_state {
	ISP_POSTRAW_IDLE,
	ISP_POSTRAW_RUNNING,
};

enum cvi_isp_pre_be_state {
	ISP_PRE_BE_IDLE,
	ISP_PRE_BE_RUNNING,
};

enum cvi_isp_preraw_state {
	ISP_PRERAW_IDLE,
	ISP_PRERAW_RUNNING,
};

struct vi_plane {
	__u64 addr;
};

/*
 * @index:
 * @length: length of planes
 * @planes: to describe buf
 * @reserved
 */
struct vi_buffer {
	__u32 index;
	__u32 length;
	struct vi_plane planes[3];
	__u32 reserved;
};

/* struct mempool
 * @base: the address of the mempool
 * @size: the size of the mempool
 * @byteused: the number of bytes used
 */
struct _mempool {
	uint64_t base;
	uint32_t size;
	uint32_t byteused;
} isp_mempool;

struct _membuf {
	uint64_t bayer_le[OFFLINE_RAW_BUF_NUM];
	uint64_t bayer_se[OFFLINE_RAW_BUF_NUM];
	uint64_t prebe_le[OFFLINE_PRE_BE_BUF_NUM];
	uint64_t prebe_se[OFFLINE_PRE_BE_BUF_NUM];
	uint64_t yuv_yuyv[ISP_CHN_MAX][2];//yuv sensor is yuyv format
	uint64_t manr;
	uint64_t rgbmap_le[MAX_RGBMAP_BUF_NUM];
	uint64_t rgbmap_se[MAX_RGBMAP_BUF_NUM];
	uint64_t lmap_le;
	uint64_t lmap_se;
	uint64_t lsc;
	uint64_t tdnr[4];//0 for UV, 1 for Y, 2 for uv fbc double buffer, 3 for y fbc double buffer
	uint64_t ldci;
	//struct cvi_vip_isp_fswdr_report *fswdr_rpt;

	struct cvi_isp_sts_mem sts_mem[2];
	uint8_t pre_fe_sts_busy_idx;
	uint8_t pre_be_sts_busy_idx;
	uint8_t post_sts_busy_idx;

	//spinlock_t pre_fe_sts_lock;
	//uint8_t pre_fe_sts_in_use;
	//spinlock_t pre_be_sts_lock;
	//uint8_t pre_be_sts_in_use;
	//spinlock_t post_sts_lock;
	//uint8_t post_sts_in_use;
} isp_bufpool[ISP_PRERAW_MAX] = {0};

static u8 RGBMAP_BUF_IDX	= 2;

/* viproc control for sensor numbers */
static int viproc_en[2] = {1, 0};

/* control internal patgen
 *
 * 1: enable
 * 0: disable
 */
static int csi_patgen_en[ISP_PRERAW_MAX] = {0, 0};

/* runtime tuning control
 * ctrl:
 *	0: all ch stop update.
 *	1: stop after apply ch1 setting
 *	2: stop after apply ch2 setting
 */
int tuning_dis[4] = {0, 0, 0, 0}; //ctrl, fe, be, post

/* Runtime to enable/disable isp_top_clk
 * Ctrl:
 *	0: Disable isp_top_clk dynamic contorl
 *	1: Enable isp_top_clk dynamic control
 */
int clk_dynamic_en;

//void _pre_hw_enque(
//	struct cvi_vi_dev *vdev,
//	const enum cvi_isp_raw raw_num,
//	const u8 chn_num);
//static void _vi_sw_init(struct cvi_vi_dev *vdev);
//static int _vi_clk_ctrl(struct cvi_vi_dev *vdev, u8 enable);
//void _postraw_outbuf_enq(struct cvi_vi_dev *vdev, const enum cvi_isp_raw raw_num);

static void _vi_deferred_thread(void *arg);
void _vi_send_isp_cmdq(enum cvi_isp_raw raw_num, enum ISP_CMDQ_E cmdq_id, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __VI_H__ */
