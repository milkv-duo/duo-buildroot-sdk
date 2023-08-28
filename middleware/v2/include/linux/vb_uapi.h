#ifndef __U_VB_UAPI_H__
#define __U_VB_UAPI_H__

#ifdef __cplusplus
	extern "C" {
#endif

#define VB_POOL_NAME_LEN        (32)
#define VB_COMM_POOL_MAX_CNT    (16)
#define VB_POOL_MAX_BLK         (64)

enum VB_IOCTL {
	VB_IOCTL_SET_CONFIG,
	VB_IOCTL_GET_CONFIG,
	VB_IOCTL_INIT,
	VB_IOCTL_EXIT,
	VB_IOCTL_CREATE_POOL,
	VB_IOCTL_CREATE_EX_POOL,
	VB_IOCTL_DESTROY_POOL,
	VB_IOCTL_PHYS_TO_HANDLE,
	VB_IOCTL_GET_BLK_INFO,
	VB_IOCTL_GET_POOL_CFG,
	VB_IOCTL_GET_BLOCK,
	VB_IOCTL_RELEASE_BLOCK,
	VB_IOCTL_GET_POOL_MAX_CNT,
	VB_IOCTL_PRINT_POOL,
	VB_IOCTL_UNIT_TEST,
	VB_IOCTL_MAX,
};

/*
 * blk_size: size of blk in the pool.
 * blk_cnt: number of blk in the pool.
 * remap_mode: remap mode.
 * name: pool name
 * pool_id: pool id
 * mem_base: phy start addr of this pool
 */
struct cvi_vb_pool_cfg {
	__u32 blk_size;
	__u32 blk_cnt;
	__u8 remap_mode;
	char pool_name[VB_POOL_NAME_LEN];
	__u32 pool_id;
	__u64 mem_base;
};

struct cvi_vb_pool_ex_cfg {
	__u32 blk_cnt;
	__u64 au64PhyAddr[VB_POOL_MAX_BLK][3];
	__u32 pool_id;
};

/*
 * comm_pool_cnt: number of common pools used.
 * comm_pool: pool cfg for the pools.
 */
struct cvi_vb_cfg {
	__u32 comm_pool_cnt;
	struct cvi_vb_pool_cfg comm_pool[VB_COMM_POOL_MAX_CNT];
};

struct cvi_vb_blk_cfg {
	__u32 pool_id;
	__u32 blk_size;
	__u64 blk;
};

struct cvi_vb_blk_info {
	__u64 blk;
	__u32 pool_id;
	__u64 phy_addr;
	__u32 usr_cnt;
};

#ifdef __cplusplus
	}
#endif

#endif /* __U_VB_UAPI_H__ */
