/*
 * Copyright CviTek Inc.
 *
 * Created Time: Feb, 2020
 */
#ifndef __CVI_JPEG_CFG_H__
#define __CVI_JPEG_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BM_DBG_MSG_ENABLE

#define BM_MASK_ERR     0x1
#define BM_MASK_FLOW    0x2
#define BM_MASK_MEM     0x4
#define BM_MASK_TRACE   0x10
#define BM_MASK_PERF    0x20
#define BM_MASK_ALL     0xFFFF

#ifdef BM_DBG_MSG_ENABLE
#define BM_DBG_ERR(msg, ...)    if (jpu_level & BM_MASK_ERR)\
	{ printf("[ERR] %s = %d, " msg, __func__, __LINE__, ## __VA_ARGS__); }
#define BM_DBG_FLOW(msg, ...)   if (jpu_level & BM_MASK_FLOW)\
	{ printf("[FLOW] %s = %d, " msg, __func__, __LINE__, ## __VA_ARGS__); }
#define BM_DBG_MEM(msg, ...)    if (jpu_level & BM_MASK_MEM)\
	{ printf("[MEM] %s = %d, " msg, __func__, __LINE__, ## __VA_ARGS__); }
#define BM_DBG_TRACE(msg, ...)  if (jpu_level & BM_MASK_TRACE)\
	{ printf("[TRACE] %s = %d, " msg, __func__, __LINE__, ## __VA_ARGS__); }
#define BM_DBG_PERF(msg, ...)   if (jpu_level & BM_MASK_PERF)\
	{ printf("[PERF] %s = %d, " msg, __func__, __LINE__, ## __VA_ARGS__); }
extern int jpu_level;
#else
#define BM_DBG_ERR(msg, ...)
#define BM_DBG_FLOW(msg, ...)
#define BM_DBG_MEM(msg, ...)
#define BM_DBG_TRACE(msg, ...)
#define BM_DBG_PERF(msg, ...)
#endif

//#define PROFILE_PERFORMANCE
#define JPEG_CODEC_INTR_NUM  75
//#define SUPPORT_INTERRUPT

#ifdef SUPPORT_INTERRUPT
int irq_handler_jpeg_codec(int irqn, void *priv);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
