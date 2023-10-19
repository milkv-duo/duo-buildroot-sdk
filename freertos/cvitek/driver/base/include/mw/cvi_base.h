#ifndef _U_MW_CVI_BASE_H_
#define _U_MW_CVI_BASE_H_

#include "cvi_comm_vb.h"
#include "cvi_comm_video.h"
#include "cvi_comm_sys.h"
#include <stdatomic.h>

#define NUM_OF_PLANES               (3)

#define BASE_SHARE_MEM_SIZE         (0xA0000)

#define BASE_VB_COMM_POOL_OFFSET    (0x10)
#define BASE_VB_BLK_MOD_ID_OFFSET   (BASE_VB_COMM_POOL_OFFSET + VB_COMM_POOL_RSV_SIZE)
#define BASE_LOG_LEVEL_OFFSET       (BASE_VB_BLK_MOD_ID_OFFSET + VB_BLK_MOD_ID_RSV_SIZE * VB_MAX_POOLS)
#define BASE_BIND_INFO_OFFSET       (BASE_LOG_LEVEL_OFFSET + LOG_LEVEL_RSV_SIZE)
#define BASE_VERSION_INFO_OFFSET    (BASE_BIND_INFO_OFFSET + BIND_INFO_RSV_SIZE)

#define VB_COMM_POOL_RSV_SIZE       (sizeof(struct VB_POOL_S) * VB_MAX_POOLS)
#define VB_BLK_MOD_ID_RSV_SIZE      (sizeof(uint64_t) * VB_POOL_MAX_BLK) //per pool
#define LOG_LEVEL_RSV_SIZE          (sizeof(int) * CVI_ID_BUTT)
#define BIND_INFO_RSV_SIZE          (sizeof(struct BIND_NODE_S) * BIND_NODE_MAXNUM)
#define VERSION_INFO_RSV_SIZE       (sizeof(struct MMF_VERSION_S))

#ifdef __arm__
#define FIFO_HEAD(name, type)						\
	struct name {							\
		struct type *fifo;					\
		__u32 padding;						\
		int front, tail, capacity;				\
	} __aligned(8)
#else
#define FIFO_HEAD(name, type)						\
	struct name {							\
		struct type *fifo;					\
		int front, tail, capacity;				\
	}
#endif

#define CHN_MATCH(x, y) (((x)->enModId == (y)->enModId) && ((x)->s32DevId == (y)->s32DevId)             \
	&& ((x)->s32ChnId == (y)->s32ChnId))

// start point is included.
// end point is excluded.
struct crop_size {
	uint16_t  start_x;
	uint16_t  start_y;
	uint16_t  end_x;
	uint16_t  end_y;
};

struct buffer {
	uint64_t phy_addr[NUM_OF_PLANES];
	size_t length[NUM_OF_PLANES];
	uint32_t stride[NUM_OF_PLANES];
	struct SIZE_S size;
	uint64_t u64PTS;
	uint8_t dev_num;
	enum PIXEL_FORMAT_E enPixelFormat;
	uint32_t frm_num;
	struct crop_size frame_crop;
};

struct VB_S {
	uint32_t vb_pool;
	uint64_t phy_addr;
	void *vir_addr;
	atomic_uint usr_cnt;
	struct buffer buf;
	uint32_t magic;
	uint64_t *mod_ids;
	bool external;
};

FIFO_HEAD(vbq, vb_s*);

struct VB_POOL_S {
	uint32_t poolID;
	int16_t ownerID;
	uint64_t memBase;
	void *vmemBase;
#ifdef __arm__
	__u32 padding; /* padding for keeping same size of this structure */
#endif
	struct vbq freeList;
	struct VB_POOL_CONFIG_S config;
	bool bIsCommPool;
	uint32_t u32FreeBlkCnt;
	uint32_t u32MinFreeBlkCnt;
	char acPoolName[MAX_VB_POOL_NAME_LEN];
} __aligned(8);

#endif // _U_MW_CVI_BASE_H_
