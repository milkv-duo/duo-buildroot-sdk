#ifndef __U_CVI_VI_CTX_H__
#define __U_CVI_VI_CTX_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <linux/cvi_defines.h>
#include <linux/cvi_comm_vi.h>
//#include <linux/cvi_base_ctx.h>

#define VI_SHARE_MEM_SIZE           (0x2000)

struct cvi_vi_ctx {
	VI_STATE_E vi_stt;
	CVI_U8   total_chn_num;
	CVI_U8   total_dev_num;
	CVI_BOOL is_enable[VI_MAX_CHN_NUM + VI_MAX_EXT_CHN_NUM];
	CVI_BOOL isDevEnable[VI_MAX_DEV_NUM];
	CVI_BOOL isTile;

	// mod param
	VI_MOD_PARAM_S modParam;

	// dev
	VI_DEV_ATTR_S devAttr[VI_MAX_DEV_NUM];
	VI_DEV_ATTR_EX_S devAttrEx[VI_MAX_DEV_NUM];
	VI_DEV_TIMING_ATTR_S stTimingAttr[VI_MAX_DEV_NUM];
	VI_DEV_BIND_PIPE_S devBindPipeAttr[VI_MAX_DEV_NUM];

	// pipe
	CVI_BOOL isPipeCreated[VI_MAX_PIPE_NUM];
	CVI_BOOL isDisEnable[VI_MAX_PIPE_NUM];
	VI_PIPE_FRAME_SOURCE_E enSource[VI_MAX_PIPE_NUM];
	VI_PIPE_ATTR_S pipeAttr[VI_MAX_PIPE_NUM];
	CROP_INFO_S    pipeCrop[VI_MAX_PIPE_NUM];
	VI_DUMP_ATTR_S dumpAttr[VI_MAX_PIPE_NUM];

	// chn
	VI_CHN_ATTR_S chnAttr[VI_MAX_CHN_NUM];
	VI_CHN_STATUS_S chnStatus[VI_MAX_CHN_NUM];
	VI_CROP_INFO_S chnCrop[VI_MAX_CHN_NUM];
	ROTATION_E enRotation[VI_MAX_CHN_NUM];
	VI_LDC_ATTR_S stLDCAttr[VI_MAX_CHN_NUM];
	VI_EARLY_INTERRUPT_S enEalyInt[VI_MAX_CHN_NUM];

	CVI_U32 blk_size[VI_MAX_CHN_NUM];
	CVI_U32 timeout_cnt;
	CVI_U8 bypass_frm[VI_MAX_CHN_NUM];
#ifdef __arm__
	CVI_U32 vi_raw_blk[2];
#else
	CVI_U64 vi_raw_blk[2];
#endif
	//ToDo
	//struct cvi_gdc_mesh mesh[VI_MAX_CHN_NUM];

	VI_CHN chn_bind[VI_MAX_CHN_NUM][VI_MAX_EXTCHN_BIND_PER_CHN];
	VI_EXT_CHN_ATTR_S stExtChnAttr[VI_MAX_EXT_CHN_NUM];
	//ToDo
	//FISHEYE_ATTR_S stFishEyeAttr[VI_MAX_EXT_CHN_NUM];
};

#ifdef __cplusplus
}
#endif

#endif /* __U_CVI_VI_CTX_H__ */
