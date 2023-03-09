#ifndef __CVI_SCENE_DECODE_H__
#define __CVI_SCENE_DECODE_H__

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include "cvi_type.h"
#elif defined(__CV181X__) || defined(__CV180X__)
#include <linux/cvi_type.h>
#endif // SOCs

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

CVI_S32 SCENE_DecodeS32Array(const char * const pszSrc, CVI_S32 *pas32Dest, CVI_U32 u32DestLength);

CVI_S32 SCENE_DecodeU8Array(const char * const pszSrc, CVI_U8 *pau8Dest, CVI_U32 u32DestLength);
CVI_S32 SCENE_DecodeU16Array(const char * const pszSrc, CVI_U16 *pau16Dest, CVI_U32 u32DestLength);
CVI_S32 SCENE_DecodeU32Array(const char * const pszSrc, CVI_U32 *pau32Dest, CVI_U32 u32DestLength);
CVI_S32 SCENE_DecodeU64Array(const char * const pszSrc, CVI_U64 *pau64Dest, CVI_U32 u32DestLength);

CVI_VOID SCENE_PrintS8Array(const char *pszSrc, const CVI_S8 * const pas8Data, CVI_U32 u32DataLength);
CVI_VOID SCENE_PrintS32Array(const char *pszSrc, const CVI_S32 * const pas32Data, CVI_U32 u32DataLength);

CVI_VOID SCENE_PrintU8Array(const char *pszSrc, const CVI_U8 * const pau8Data, CVI_U32 u32DataLength);
CVI_VOID SCENE_PrintU16Array(const char *pszSrc, const CVI_U16 * const pau16Data, CVI_U32 u32DataLength);
CVI_VOID SCENE_PrintU32Array(const char *pszSrc, const CVI_U32 * const pau32Data, CVI_U32 u32DataLength);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENE_DECODE_H__
