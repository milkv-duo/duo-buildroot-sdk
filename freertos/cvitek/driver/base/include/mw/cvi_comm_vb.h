#ifndef __CVI_COMM_VB_H__
#define __CVI_COMM_VB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define VB_MAX_POOLS            (512)
#define VB_POOL_MAX_BLK         (128)

/*
 * VB_REMAP_MODE_NONE: no remap.
 * VB_REMAP_MODE_NOCACHE: no cache remap.
 * VB_REMAP_MODE_CACHED: cache remap. flush cache is needed.
 */
enum VB_REMAP_MODE_E {
	VB_REMAP_MODE_NONE = 0,
	VB_REMAP_MODE_NOCACHE = 1,
	VB_REMAP_MODE_CACHED = 2,
	VB_REMAP_MODE_BUTT
};

/*
 * u32BlkSize: size of blk in the pool.
 * u32BlkCnt: number of blk in the pool.
 * enRemapMode: remap mode.
 */
#define MAX_VB_POOL_NAME_LEN (32)
struct VB_POOL_CONFIG_S {
	uint32_t u32BlkSize;
	uint32_t u32BlkCnt;
	enum VB_REMAP_MODE_E enRemapMode;
	char acName[MAX_VB_POOL_NAME_LEN];
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_VB_H_ */
