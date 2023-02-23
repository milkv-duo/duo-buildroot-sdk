#ifndef __CVI_VI_CTX_H__
#define __CVI_VI_CTX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cvi_defines.h>
#include <cvi_comm_vi.h>

struct cvi_vi_ctx {
	CVI_U8     total_chn_num;
	CVI_U8     total_dev_num;
	CVI_BOOL   is_enable[VI_MAX_CHN_NUM];
	CVI_BOOL   isDevEnable[VI_MAX_DEV_NUM];
	CVI_BOOL   isTile;

	// dev
	VI_DEV_ATTR_S devAttr[VI_MAX_DEV_NUM];

	// chn
	VI_CHN_STATUS_S chnStatus[VI_MAX_CHN_NUM];

	CVI_U32 blk_size[VI_MAX_CHN_NUM];
	CVI_U32 timeout_cnt;
	CVI_U8  bypass_frm[VI_MAX_CHN_NUM];

	CVI_U32 vi_mem_base;
	CVI_U32 vi_mem_size;
};

#ifdef __cplusplus
}
#endif

#endif /* __CVI_VI_CTX_H__ */
