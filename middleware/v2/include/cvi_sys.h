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

#include <stdbool.h>
#include <stdint.h>

#include "cvi_debug.h"
#include <linux/cvi_comm_sys.h>
#include <linux/cvi_type.h>
#include <linux/cvi_common.h>



/**
 * @brief system initialization.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_Init(void);

/**
 * @brief system exit.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_Exit(void);

/**
 * @brief Open vi device.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_VI_Open(void);

/**
 * @brief Close vi device.
 *
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_VI_Close(void);

/**
 * @brief Bind the two channels.
 *
 * @param pstSrcChn(In), source channel.
 * @param pstDestChn(In), destination channel.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_Bind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);

/**
 * @brief UnBind the two channels.
 *
 * @param pstSrcChn(In), source channel.
 * @param pstDestChn(In), destination channel.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_UnBind(const MMF_CHN_S *pstSrcChn, const MMF_CHN_S *pstDestChn);

/**
 * @brief Get source channel by destination channel.
 *
 * @param pstDestChn(In), destination channel.
 * @param pstSrcChn(Out), source channel.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetBindbyDest(const MMF_CHN_S *pstDestChn, MMF_CHN_S *pstSrcChn);

/**
 * @brief Get destination channel by source channel.
 *
 * @param pstSrcChn(In), source channel.
 * @param pstDestChn(Out), all destination channels.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetBindbySrc(const MMF_CHN_S *pstSrcChn, MMF_BIND_DEST_S *pstBindDest);

/**
 * @brief Get MMF version.
 *
 * @param pstVersion(Out), version info.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetVersion(MMF_VERSION_S *pstVersion);

/**
 * @brief Get chip ID.
 *
 * @param pu32ChipId(Out), chip ID.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetChipId(CVI_U32 *pu32ChipId);

/**
 * @brief Get chip version.
 *
 * @param pu32ChipVersion(Out), chip version.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetChipVersion(CVI_U32 *pu32ChipVersion);

/**
 * @brief Get the startup reason.
 *
 * @param pu32PowerOnReason(Out), reason.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetPowerOnReason(CVI_U32 *pu32PowerOnReason);

/**
 * @brief Gets the current timestamp.
 *
 * @param pu64CurPTS(Out), timestamp, microsecond(us).
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetCurPTS(CVI_U64 *pu64CurPTS);

/**
 * @brief Alloc ion memory.
 *
 * @param pu64PhyAddr(Out), physical address.
 * @param ppVirAddr(Out), virtual address.
 * @param strName(In), Named the Ion.
 * @param u32Len(In), Length.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_IonAlloc(CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr, const CVI_CHAR *strName, CVI_U32 u32Len);

/**
 * @brief Alloc cached ion memory.
 *
 * @param pu64PhyAddr(Out), physical address.
 * @param ppVirAddr(Out), virtual address.
 * @param strName(In), Named the Ion.
 * @param u32Len(In), Length.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_IonAlloc_Cached(CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr,
				 const CVI_CHAR *strName, CVI_U32 u32Len);

/**
 * @brief Free ion memory.
 *
 * @param u64PhyAddr(In), physical address.
 * @param pVirAddr(In), virtual address.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_IonFree(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr);

/**
 * @brief Flush cache data to DRAM.
 *
 * @param u64PhyAddr(In), physical address.
 * @param pVirAddr(In), virtual address.
 * @param u32Len(In), Length.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_IonFlushCache(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr, CVI_U32 u32Len);

/**
 * @brief Invalid cached data.
 *
 * @param u64PhyAddr(In), physical address.
 * @param pVirAddr(In), virtual address.
 * @param u32Len(In), Length.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_IonInvalidateCache(CVI_U64 u64PhyAddr, CVI_VOID *pVirAddr, CVI_U32 u32Len);

/**
 * @brief Get ion file descriptor.
 *
 * @return file descriptor.
 */
CVI_S32 CVI_SYS_IonGetFd(CVI_VOID);

/**
 * @brief memory copy by TPU.
 *
 * @param u64PhyDst(In), destination physical address.
 * @param u64PhySrc(In), source physical address.
 * @param u32Len(In), Length.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_TDMACopy(CVI_U64 u64PhyDst, CVI_U64 u64PhySrc, CVI_U32 u32Len);

/**
 * @brief 2D memory copy by TPU.
 *
 * @param pointer(In), 2D memory.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_TDMACopy2D(CVI_TDMA_2D_S *param);

/**
 * @brief The physical address maps to a virtual address.
 *
 * @param u64PhyAddr(In), physical address.
 * @param u32Size(In), buf size.
 * @return virtual address.
 */
void *CVI_SYS_Mmap(CVI_U64 u64PhyAddr, CVI_U32 u32Size);

/**
 * @brief The physical address maps to a virtual address in cache memory.
 *
 * @param u64PhyAddr(In), physical address.
 * @param u32Size(In), buf size.
 * @return virtual address.
 */
void *CVI_SYS_MmapCache(CVI_U64 u64PhyAddr, CVI_U32 u32Size);

/**
 * @brief Removing an Address Mapping.
 *
 * @param pVirAddr(In), virtual address.
 * @param u32Size(In), buf size.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_Munmap(void *pVirAddr, CVI_U32 u32Size);

/**
 * @brief Set vi-vpss online mode or offline mode.
 *
 * @param pstVIVPSSMode(In), vi-vpss mode.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_SetVIVPSSMode(const VI_VPSS_MODE_S *pstVIVPSSMode);

/**
 * @brief Get vi-vpss mode.
 *
 * @param pstVIVPSSMode(Out), vi-vpss mode.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetVIVPSSMode(VI_VPSS_MODE_S *pstVIVPSSMode);

/**
 * @brief Set vpss single mode or dual mode.
 *
 * @param enVPSSMode(In), vpss mode.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_SetVPSSMode(VPSS_MODE_E enVPSSMode);

/**
 * @brief Get vpss mode.
 *
 * @param enVPSSMode(In), vpss mode.
 * @return vpss mode.
 */
VPSS_MODE_E CVI_SYS_GetVPSSMode(void);

/**
 * @brief Set vpss extension mode.
 *
 * @param pstVPSSMode(In), vpss extension mode.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_SetVPSSModeEx(const VPSS_MODE_S *pstVPSSMode);

/**
 * @brief Get vpss extension mode.
 *
 * @param pstVPSSMode(Out), vpss extension mode.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_GetVPSSModeEx(VPSS_MODE_S *pstVPSSMode);

/**
 * @brief Get module name.
 *
 * @param id(In), module ID.
 * @return module name.
 */
const CVI_CHAR *CVI_SYS_GetModName(MOD_ID_E id);

/**
 * @brief Set modules log level.
 *
 * @param pstConf(In), log level config.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_LOG_SetLevelConf(LOG_LEVEL_CONF_S *pstConf);

/**
 * @brief Get modules log level.
 *
 * @param pstConf(Out), log level config.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_LOG_GetLevelConf(LOG_LEVEL_CONF_S *pstConf);

/**
 * @brief Register thermal callback.
 *
 * @param setFPS(In), Thermal Callback.
 * @return void.
 */
void CVI_SYS_RegisterThermalCallback(void (*setFPS)(int));

/**
 * @brief Begin Trace debug.
 *
 * @param name(In), tag name.
 * @return void.
 */
void CVI_SYS_TraceBegin(const char *name);

/**
 * @brief Trace Counter.
 *
 * @param name(In), tag name.
 * @param value(In), Counter value.
 * @return void.
 */
void CVI_SYS_TraceCounter(const char *name, signed int value);

/**
 * @brief End Trace debug.
 *
 * @return void.
 */
void CVI_SYS_TraceEnd(void);

CVI_S32 CVI_SYS_StartThermalThread(void);
CVI_S32 CVI_SYS_StopThermalThread(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_SYS_H__ */

