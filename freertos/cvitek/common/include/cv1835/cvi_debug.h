/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_debug.h
 * Description:
 */

#ifndef __CVI_DEBUG_H__
#define __CVI_DEBUG_H__

#include "cvi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*
 * Debug Config
 */
#define CONFIG_CVI_GDB_NO 1
#define CONFIG_CVI_GDB "n"
//#define CONFIG_CVI_LOG_TRACE_SUPPORT 1
#define CONFIG_CVI_LOG_TRACE_ALL 1
#define CONFIG_CVI_LOG_TRACE_LEVEL 4


#define CVI_DBG_EMERG      0   /* system is unusable                   */
#define CVI_DBG_ALERT      1   /* action must be taken immediately     */
#define CVI_DBG_CRIT       2   /* critical conditions                  */
#define CVI_DBG_ERR        3   /* error conditions                     */
#define CVI_DBG_WARN       4   /* warning conditions                   */
#define CVI_DBG_NOTICE     5   /* normal but significant condition     */
#define CVI_DBG_INFO       6   /* informational                        */
#define CVI_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct _LOG_LEVEL_CONF_S {
	MOD_ID_E  enModId;
	CVI_S32   s32Level;
	char   cModName[16];
} LOG_LEVEL_CONF_S;

#define CVI_PRINT printf

/* #ifdef CVI_DEBUG */
#ifdef CONFIG_CVI_LOG_TRACE_SUPPORT
	extern const CVI_CHAR *CVI_SYS_GetModName(MOD_ID_E id);
	extern CVI_S32 *log_levels;
	extern char *log_name[8];

	#define CVI_ASSERT(expr)                               \
	do {                                                   \
		if (!(expr)) {                                 \
			printf("\nASSERT at:\n"                \
			       "  >Function : %s\n"            \
			       "  >Line No. : %d\n"            \
			       "  >Condition: %s\n",           \
			       __func__, __LINE__, #expr);     \
			_exit(-1);                             \
		} \
	} while (0)

	#define CVI_TRACE(level, enModId, fmt, ...)            \
	do {                                                   \
		CVI_S32 LogLevel = (log_levels == NULL) ? CONFIG_CVI_LOG_TRACE_LEVEL : log_levels[enModId];      \
		if (level <= LogLevel)	       \
			syslog(LOG_LOCAL5|level, "[%s-%s] " fmt, CVI_SYS_GetModName(enModId), log_name[level],    \
				##__VA_ARGS__);           \
	} while (0)
#else
	#define CVI_ASSERT(expr)
	#define CVI_TRACE(level, enModId, fmt...)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMM_SYS_H__ */

