#ifndef __U_CVI_VPSS_CTX_H__
#define __U_CVI_VPSS_CTX_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <linux/cvi_defines.h>
#include <linux/cvi_comm_vpss.h>
#include <linux/cvi_comm_region.h>
#include <linux/cvi_base_ctx.h>
#include <linux/cvi_vip_sc.h>

struct VPSS_GRP_WORK_STATUS_S {
	CVI_U32 u32RecvCnt;
	CVI_U32 u32LostCnt;
	CVI_U32 u32StartFailCnt; //start job fail cnt
	CVI_U32 u32CostTime; // current job cost time in us
	CVI_U32 u32MaxCostTime;
	CVI_U32 u32HwCostTime; // current job Hw cost time in us
	CVI_U32 u32HwMaxCostTime;
};

struct VPSS_CHN_WORK_STATUS_S {
	CVI_U32 u32SendOk; // send OK cnt after latest chn enable
	CVI_U64 u64PrevTime; // latest time (us)
	CVI_U32 u32FrameNum;  //The number of Frame in one second
	CVI_U32 u32RealFrameRate; // chn real time frame rate
};

struct VPSS_CHN_CFG {
	CVI_BOOL isEnabled;
	CVI_BOOL isMuted;
	VPSS_CHN_ATTR_S stChnAttr;
	VPSS_CROP_INFO_S stCropInfo;
	ROTATION_E enRotation;
	CVI_U32 blk_size;
	CVI_U32 align;
	RGN_HANDLE rgn_handle[RGN_MAX_LAYER_VPSS][RGN_MAX_NUM_VPSS]; //only overlay
	RGN_HANDLE coverEx_handle[RGN_COVEREX_MAX_NUM];
	RGN_HANDLE mosaic_handle[RGN_MOSAIC_MAX_NUM];
	VPSS_SCALE_COEF_E enCoef;
	CVI_U32 YRatio;
	VPSS_LDC_ATTR_S stLDCAttr;
	CVI_U32 VbPool;
	VPSS_CHN_BUF_WRAP_S stBufWrap;
	CVI_U64 bufWrapPhyAddr;
	CVI_U32 u32BufWrapDepth;
	struct VPSS_CHN_WORK_STATUS_S stChnWorkStatus;

	// hw cfgs;
	CVI_BOOL is_cfg_changed;
};

struct cvi_vpss_ctx {
	CVI_BOOL isCreated;
	CVI_BOOL isStarted;
	VPSS_GRP_ATTR_S stGrpAttr;
	VPSS_CROP_INFO_S stGrpCropInfo;
	CVI_U8 chnNum;
	CVI_U8 u8DevId;
	struct VPSS_CHN_CFG stChnCfgs[VPSS_MAX_CHN_NUM];
	struct crop_size frame_crop;
	struct mutex lock;
	CVI_S16 s16OffsetTop;
	CVI_S16 s16OffsetBottom;
	CVI_S16 s16OffsetLeft;
	CVI_S16 s16OffsetRight;
	struct VPSS_GRP_WORK_STATUS_S stGrpWorkStatus;
	CVI_VOID *hw_cfgs;

	// hw cfgs;
	CVI_BOOL is_cfg_changed;
};

#ifdef __cplusplus
}
#endif

#endif /* __U_CVI_VPSS_CTX_H__ */
