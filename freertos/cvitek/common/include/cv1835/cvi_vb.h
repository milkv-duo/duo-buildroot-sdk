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
#include "cvi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

VB_POOL CVI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);
CVI_S32 CVI_VB_DestroyPool(VB_POOL Pool);

VB_BLK CVI_VB_GetBlock(VB_POOL Pool, CVI_U32 u32BlkSize);
CVI_S32 CVI_VB_ReleaseBlock(VB_BLK Block);

VB_BLK CVI_VB_PhysAddr2Handle(CVI_U64 u64PhyAddr);
CVI_U64 CVI_VB_Handle2PhysAddr(VB_BLK Block);
VB_POOL CVI_VB_Handle2PoolId(VB_BLK Block);

CVI_S32 CVI_VB_InquireUserCnt(VB_BLK Block, CVI_U32 *pCnt);

//CVI_S32 CVI_VB_GetSupplementAddr(VB_BLK Block, VIDEO_SUPPLEMENT_S *pstSupplement);
//CVI_S32 CVI_VB_SetSupplementConfig(const VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);
//CVI_S32 CVI_VB_GetSupplementConfig(VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);


CVI_S32 CVI_VB_Init(void);
CVI_S32 CVI_VB_Exit(void);
VB_POOL CVI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);
CVI_S32 CVI_VB_DestroyPool(VB_POOL Pool);
CVI_S32 CVI_VB_SetConfig(const VB_CONFIG_S *pstVbConfig);
CVI_S32 CVI_VB_GetConfig(VB_CONFIG_S *pstVbConfig);

CVI_S32 CVI_VB_MmapPool(VB_POOL Pool);
CVI_S32 CVI_VB_MunmapPool(VB_POOL Pool);

CVI_S32 CVI_VB_GetBlockVirAddr(VB_POOL Pool, VB_BLK Block, void **ppVirAddr);

CVI_S32 CVI_VB_InitModCommPool(VB_UID_E enVbUid);
CVI_S32 CVI_VB_ExitModCommPool(VB_UID_E enVbUid);

CVI_S32 CVI_VB_SetModPoolConfig(VB_UID_E enVbUid, const VB_CONFIG_S *pstVbConfig);
CVI_S32 CVI_VB_GetModPoolConfig(VB_UID_E enVbUid, VB_CONFIG_S *pstVbConfig);

CVI_VOID CVI_VB_PrintPool(VB_POOL Pool);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_VB_H__ */

