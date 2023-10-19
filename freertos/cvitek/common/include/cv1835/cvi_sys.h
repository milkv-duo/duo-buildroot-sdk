/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_sys.h
 * Description:
 *   MMF Programe Interface for system
 */


#ifndef __CVI_SYS_H__
#define __CVI_SYS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "cvi_comm_sys.h"
#include "cvi_type.h"
#include "cvi_common.h"

CVI_S32 CVI_SYS_VI_Open(void);
CVI_S32 CVI_SYS_VI_Close(void);

CVI_S32 CVI_SYS_Init(void);
CVI_S32 CVI_SYS_Exit(void);

CVI_S32 CVI_SYS_Bind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);
CVI_S32 CVI_SYS_UnBind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);
CVI_S32 CVI_SYS_GetBindbyDest(const MMF_CHN_S *pstDestChn, MMF_CHN_S *pstSrcChn);
CVI_S32 CVI_SYS_GetBindbySrc(const MMF_CHN_S *pstSrcChn, MMF_BIND_DEST_S *pstBindDest);

CVI_S32 CVI_SYS_GetVersion(MMF_VERSION_S *pstVersion);

CVI_S32 CVI_SYS_GetChipId(CVI_U32 *pu32ChipId);
CVI_S32 CVI_SYS_GetChipVersion(CVI_U32 *pu32ChipVersion);

CVI_S32 CVI_SYS_GetChipSNSize(CVI_U32 *pu32SNSize);
CVI_S32 CVI_SYS_GetChipSN(CVI_U8 *pu8SN, CVI_U32 u32SNSize);

CVI_S32 CVI_SYS_GetCustomCode(CVI_U32 *pu32CustomCode);


/*
 * u64Base is the global PTS of the system.
 * ADVICE:
 * 1. Better to call CVI_SYS_GetCurPTS on the host board to get u64Base.
 * 2. When os start up, call CVI_SYS_InitPTSBase to set the init PTS.
 * 3. When media bussines is running, synchronize the PTS one time per minute
 *     by calling CVI_SYS_SyncPTS.
 */
CVI_S32 CVI_SYS_GetCurPTS(CVI_U64 *pu64CurPTS);
CVI_S32 CVI_SYS_InitPTSBase(CVI_U64 u64PTSBase);
CVI_S32 CVI_SYS_SyncPTS(CVI_U64 u64PTSBase);

CVI_S32 CVI_SYS_IonAlloc(CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr, CVI_U32 u32Len);
CVI_S32 CVI_SYS_IonAlloc_Cached(CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr, CVI_U32 u32Len);
CVI_S32 CVI_SYS_IonFree(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr);
CVI_S32 CVI_SYS_IonFlushCache(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr, CVI_U32 u32Len);
CVI_S32 CVI_SYS_IonInvalidateCache(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr, CVI_U32 u32Len);

void *CVI_SYS_Mmap(CVI_U64 u64PhyAddr, CVI_U32 u32Size);
void *CVI_SYS_MmapCache(CVI_U64 u64PhyAddr, CVI_U32 u32Size);
CVI_S32 CVI_SYS_Munmap(void *pVirAddr, CVI_U32 u32Size);
CVI_S32 CVI_SYS_MflushCache(CVI_U64 u64PhyAddr, void *pVirAddr, CVI_U32 u32Size);

/* Close all the FD which is used by sys module */
CVI_S32 CVI_SYS_CloseFd(void);


/* Set/Get local timezone, range: [-86400, 86400] seconds (that is: [-24, 24] hours)  */
CVI_S32 CVI_SYS_SetTimeZone(CVI_S32 s32TimeZone);
CVI_S32 CVI_SYS_GetTimeZone(CVI_S32 *ps32TimeZone);

CVI_S32 CVI_SYS_SetTuningConnect(CVI_S32 s32Connect);
CVI_S32 CVI_SYS_GetTuningConnect(CVI_S32 *ps32Connect);


CVI_S32 CVI_SYS_SetVIVPSSMode(const VI_VPSS_MODE_S *pstVIVPSSMode);
CVI_S32 CVI_SYS_GetVIVPSSMode(VI_VPSS_MODE_S *pstVIVPSSMode);

CVI_S32 CVI_SYS_SetVPSSMode(VPSS_MODE_E enVPSSMode);
VPSS_MODE_E CVI_SYS_GetVPSSMode(void);
CVI_S32 CVI_SYS_SetVPSSModeEx(const VPSS_MODE_S *pstVPSSMode);
CVI_S32 CVI_SYS_GetVPSSModeEx(VPSS_MODE_S *pstVPSSMode);

const CVI_CHAR *CVI_SYS_GetModName(MOD_ID_E id);


CVI_S32 CVI_LOG_SetLevelConf(LOG_LEVEL_CONF_S *pstConf);
CVI_S32 CVI_LOG_GetLevelConf(LOG_LEVEL_CONF_S *pstConf);

CVI_S32 CVI_SYS_StartThermalThread(void);
CVI_S32 CVI_SYS_StopThermalThread(void);
void CVI_SYS_RegisterThermalCallback(void (*setFPS)(int));

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_SYS_H__ */

