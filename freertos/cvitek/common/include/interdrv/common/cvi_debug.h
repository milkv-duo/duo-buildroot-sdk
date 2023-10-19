/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_debug.h
 * Description:
 */

#ifndef __CVI_DEBUG_H__
#define __CVI_DEBUG_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cvi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define syslog(...)
#define LOG_LOCAL5 5

/*
 * Debug Config
 */
#define CONFIG_CVI_GDB_NO 1
#define CONFIG_CVI_GDB "n"
#define CONFIG_CVI_LOG_TRACE_SUPPORT 1
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static CVI_S32 *log_levels;
static CVI_CHAR const *log_name[8] = {
	(CVI_CHAR *)"EMG", (CVI_CHAR *)"ALT", (CVI_CHAR *)"CRI", (CVI_CHAR *)"ERR",
	(CVI_CHAR *)"WRN", (CVI_CHAR *)"NOT", (CVI_CHAR *)"INF", (CVI_CHAR *)"DBG"
};

#pragma GCC diagnostic pop

#define _GENERATE_STRING(STRING) (#STRING),
static const char *const MOD_STRING[] = FOREACH_MOD(_GENERATE_STRING);
#define CVI_GET_MOD_NAME(id) (id < CVI_ID_BUTT)? MOD_STRING[id] : "UNDEF"

/* #ifdef CVI_DEBUG */
#ifdef CONFIG_CVI_LOG_TRACE_SUPPORT

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

#ifndef FPGA_PORTING

	#define CVI_TRACE(level, enModId, fmt, ...)            \
	do {                                                   \
		CVI_S32 LogLevel = (log_levels == NULL) ? CONFIG_CVI_LOG_TRACE_LEVEL : log_levels[enModId];      \
		if (level <= LogLevel) {	       \
			syslog(LOG_LOCAL5|level, "[%s-%s] " fmt, CVI_GET_MOD_NAME(enModId), log_name[level],    \
				##__VA_ARGS__);           \
		}	\
	} while (0)
#else
	#define CVI_TRACE(level, enModId, fmt, ...) \
		printf(fmt, ##__VA_ARGS__)
#endif
#else
	#define CVI_ASSERT(expr)
	#define CVI_TRACE(level, enModId, fmt...)
#endif

#define CVI_TRACE_ID(level, id, fmt, ...)                                           \
		CVI_TRACE(level, id, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_LOG(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_LOG, "%s:%d:%s(): " fmt, __FILENAM__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_SYS(level, fmt, ...)                                           \
		CVI_TRACE(level, CVI_ID_SYS, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_VB(level, fmt, ...)                                           \
		CVI_TRACE(level, CVI_ID_VB, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_SNS(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_VI, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_VI(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_VI, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_VPSS(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_VPSS, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_VO(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_VO, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_GDC(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_GDC, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_RGN(level, fmt, ...)                                           \
		CVI_TRACE(level, CVI_ID_RGN, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_TRACE_MISC(level, fmt, ...)  \
		CVI_TRACE(level, CVI_ID_SYS, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMM_SYS_H__ */

