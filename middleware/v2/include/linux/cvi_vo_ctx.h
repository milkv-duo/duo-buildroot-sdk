#ifndef __U_CVI_VO_CTX_H__
#define __U_CVI_VO_CTX_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <linux/cvi_vip.h>
#include <linux/cvi_defines.h>
#include <linux/cvi_comm_vo.h>
#include <linux/cvi_comm_region.h>

#define VO_SHARE_MEM_SIZE           (0x1000)

struct cvi_vo_ctx {
	CVI_BOOL is_dev_enable[VO_MAX_DEV_NUM];
	CVI_BOOL is_layer_enable[VO_MAX_LAYER_NUM];
	CVI_BOOL is_chn_enable[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];

	// dev
	VO_PUB_ATTR_S stPubAttr;

	// layer
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	CVI_U32 u32DisBufLen;
	CVI_S32 proc_amp[PROC_AMP_MAX];

	// chn
	VO_CHN_ATTR_S stChnAttr;
	ROTATION_E enRotation;
	RGN_HANDLE rgn_handle[RGN_MAX_NUM_VO];
	RGN_HANDLE rgn_coverEx_handle[RGN_COVEREX_MAX_NUM];
	CVI_U64 u64DisplayPts[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];
	CVI_U64 u64PreDonePts[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];

	// for calculating chn frame rate
	VO_CHN_STATUS_S chnStatus[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];

	struct cvi_rgn_cfg rgn_cfg;
	struct cvi_rgn_coverex_cfg rgn_coverex_cfg;

	struct {
		CVI_U64 paddr;
		CVI_VOID *vaddr;
	} mesh;
	//pthread_t thread[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];
	//struct task_struct *thread[VO_MAX_LAYER_NUM][VO_MAX_CHN_NUM];
	CVI_BOOL show;
	CVI_BOOL pause;
	CVI_BOOL clearchnbuf;
	CVI_BOOL fb_on_vpss;
	struct{
		__u32 left;
		__u32 top;
		__u32 width;
		__u32 height;
	} rect_crop;
	__u8 u8VoDev;
	CVI_BOOL bVideoFrameValid;
};

#ifdef __cplusplus
}
#endif

#endif /* __U_CVI_VO_CTX_H__ */
