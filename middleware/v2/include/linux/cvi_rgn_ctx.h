#ifndef __U_CVI_RGN_CTX_H__
#define __U_CVI_RGN_CTX_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <linux/cvi_comm_region.h>

/*
 * @ion_len: canvas's ion length.
 * @canvas_idx: the canvas buf-idx used by hw now.
 * @canvas_get: true if CVI_RGN_GetCanvasInfo(), false after CVI_RGN_UpdateCanvas().
 */
struct cvi_rgn_ctx {
	RGN_HANDLE Handle;
	CVI_BOOL bCreated;
	CVI_BOOL bUsed;
	RGN_ATTR_S stRegion;
	MMF_CHN_S stChn;
	RGN_CHN_ATTR_S stChnAttr;
	RGN_CANVAS_INFO_S stCanvasInfo[RGN_MAX_BUF_NUM];
	CVI_U32 u32MaxNeedIon;
	CVI_U32 ion_len;
	CVI_U8 canvas_idx;
	CVI_BOOL canvas_get;
	CVI_BOOL odec_data_valid;
	struct hlist_node node;
};

#ifdef __cplusplus
}
#endif

#endif /* __U_CVI_RGN_CTX_H__ */
