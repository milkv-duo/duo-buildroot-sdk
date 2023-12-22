/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_vb.h
 * Description:
 *   MMF Programe Interface for video buffer management moudle
 */

#ifndef __CVI_VB_H__
#define __CVI_VB_H__

#include "cvi_comm_vb.h"
#include <linux/cvi_comm_video.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#ifdef NOT_HAVE_GET_BLOCK_WITH_ID
#define CVI_VB_GET_BLOCK(id, size, MOD)	CVI_VB_GetBlock((id), (size))
#else
#define CVI_VB_GET_BLOCK(id, size, MOD)	CVI_VB_GetBlockwithID((id), (size), (MOD))
#endif


/**
 * @brief VB initialization.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_Init(void);

/**
 * @brief VB exit.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_Exit(void);

/**
 * @brief Creating VB pool dynamically.
 *
 * @param pstVbPoolCfg(In), vb pool config.
 * @return pool ID.
 */
VB_POOL CVI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);

/**
 * @brief Creating external VB pool dynamically.
 *
 * @param pstVbPoolCfg(In), vb pool config.
 * @return pool ID.
 */
VB_POOL CVI_VB_CreateExPool(VB_POOL_CONFIG_EX_S *pstVbPoolExCfg);

/**
 * @brief Destroy VB pool.
 *
 * @param Pool(In), vb pool ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_DestroyPool(VB_POOL Pool);

/**
 * @brief Set common vb config.
 *
 * @param pstVbConfig(In), common vb config.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_SetConfig(const VB_CONFIG_S *pstVbConfig);

/**
 * @brief Get common vb config.
 *
 * @param pstVbConfig(Out), common vb config.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_GetConfig(VB_CONFIG_S *pstVbConfig);

/**
 * @brief Get vb block.
 *
 * @param Pool(In), pool ID, VB_INVALID_POOLID as search from all pools.
 * @param u32BlkSize(In), block size.
 * @return vb block.
 */
VB_BLK CVI_VB_GetBlock(VB_POOL Pool, CVI_U32 u32BlkSize);

/**
 * @brief Release vb block.
 *
 * @param Block(In), vb block.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_ReleaseBlock(VB_BLK Block);

/**
 * @brief Find vb block by physical address.
 *
 * @param u64PhyAddr(In), vb physical address.
 * @return vb block.
 */
VB_BLK CVI_VB_PhysAddr2Handle(CVI_U64 u64PhyAddr);

/**
 * @brief Find physical address by vb block.
 *
 * @param Block(In), vb block.
 * @return physical address.
 */
CVI_U64 CVI_VB_Handle2PhysAddr(VB_BLK Block);

/**
 * @brief Find pool ID by vb block.
 *
 * @param Block(In), vb block.
 * @return pool ID.
 */
VB_POOL CVI_VB_Handle2PoolId(VB_BLK Block);

/**
 * @brief inquire VB use count.
 *
 * @param Block(In), vb block.
 * @param pCnt(In), use count.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_InquireUserCnt(VB_BLK Block, CVI_U32 *pCnt);

/**
 * @brief mmap the whole pool to get virtual-address.
 *
 * @param Pool(In), pool ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_MmapPool(VB_POOL Pool);

/**
 * @brief unmap the whole pool.
 *
 * @param Pool(In), pool ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_MunmapPool(VB_POOL Pool);

/**
 * @brief Get block virtual address.
 *
 * @param Pool(In), pool ID.
 * @param Block(In), vb block.
 * @param ppVirAddr(In), virtual address.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_VB_GetBlockVirAddr(VB_POOL Pool, VB_BLK Block, void **ppVirAddr);

/**
 * @brief Print pool usage information.
 *
 * @param Pool(In), pool ID.
 * @return void.
 */
CVI_VOID CVI_VB_PrintPool(VB_POOL Pool);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_VB_H__ */

