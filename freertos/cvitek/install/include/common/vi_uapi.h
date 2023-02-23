#ifndef __U_VI_UAPI_H__
#define __U_VI_UAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <types.h>

enum ISP_CMDQ_E {
	ISP_CMDQ_RECV_EV = 0x30,
	ISP_CMDQ_GET_STS_MEM,
	ISP_CMDQ_GET_TUNING_NODE_MEM,
	ISP_CMDQ_GET_LSC_PHY_BUF,
	ISP_CMDQ_MAX,
};

enum CODEC_CMDQ_E {
	CODEC_CMDQ_RECV_BUF = ISP_CMDQ_MAX,
	CODEC_CMDQ_MAX,
};

enum VI_CMDQ_E {
	VI_CMDQ_INFO_RECV_BUF_ACK = CODEC_CMDQ_MAX,
	VI_CMDQ_INFO_MAX,
};

/*
 * Events
 */
enum VI_EVENT {
	VI_EVENT_BASE,
	VI_EVENT_PRE0_SOF,
	VI_EVENT_PRE1_SOF,
	VI_EVENT_PRE0_EOF,
	VI_EVENT_PRE1_EOF,
	VI_EVENT_POST_EOF,
	VI_EVENT_POST1_EOF,
	VI_EVENT_ISP_PROC_READ,
	VI_EVENT_AWB0_DONE,
	VI_EVENT_AWB1_DONE,
	VI_EVENT_MAX,
};

enum ISP_BLC_ID {
	ISP_BLC_ID_FE0_LE = 0,
	ISP_BLC_ID_FE0_SE,
	ISP_BLC_ID_FE1_LE,
	ISP_BLC_ID_FE1_SE,
	ISP_BLC_ID_FE2_LE,
	ISP_BLC_ID_BE_LE,
	ISP_BLC_ID_BE_SE,
	ISP_BLC_ID_MAX,
};

enum ISP_WBG_ID {
	ISP_WBG_ID_FE0_RGBMAP_LE = 0,
	ISP_WBG_ID_FE0_RGBMAP_SE,
	ISP_WBG_ID_FE1_RGBMAP_LE,
	ISP_WBG_ID_FE1_RGBMAP_SE,
	ISP_WBG_ID_FE2_RGBMAP_LE,
	ISP_WBG_ID_RAW_TOP_LE,
	ISP_WBG_ID_RAW_TOP_SE,
	ISP_WBG_ID_MAX,
};

struct vi_event {
	__u32		dev_id;
	__u32		type;
	__u32		frame_sequence;
	__u32		stt_idx;
};

/* struct cvi_vip_memblock
 * @base: the address of the memory allocated.
 * @size: Size in bytes of the memblock.
 */
struct cvi_vip_memblock {
	__u8  raw_num;
	__u64 phy_addr;
	void  *vir_addr;
	__u32 size;
};

struct cvi_isp_sts_mem {
	__u8			raw_num;
	struct cvi_vip_memblock af;
	struct cvi_vip_memblock gms;
	struct cvi_vip_memblock ae_le;
	struct cvi_vip_memblock ae_se;
	struct cvi_vip_memblock awb;
	struct cvi_vip_memblock awb_post;
	struct cvi_vip_memblock dci;
	struct cvi_vip_memblock hist_edge_v;
	struct cvi_vip_memblock mmap;
};

#ifdef __cplusplus
}
#endif

#endif /* __U_VI_UAPI_H__ */
