/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2022. All rights reserved.
 *
 * File Name: fast_image.h
 * Description:
 */

#ifndef __FAST_IMAGE_HEADER__
#define __FAST_IMAGE_HEADER__

#include "rtos_cmdqu.h"
#ifndef __linux__
#include "types.h"
#else
#include <linux/kernel.h>

enum FAST_IMAGE_CMD_TYPE {
	FAST_SEND_STOP_REC = CMDQU_SYSTEM_LIMIT,
	FAST_SEND_QUERY_ISP_PADDR,
	FAST_SEND_QUERY_ISP_VADDR,
	FAST_SEND_QUERY_ISP_SIZE,
	FAST_SEND_QUERY_ISP_CTXT,
	FAST_SEND_QUERY_IMG_PADDR,
	FAST_SEND_QUERY_IMG_VADDR,
	FAST_SEND_QUERY_IMG_SIZE,
	FAST_SEND_QUERY_IMG_CTXT,
	FAST_SEND_QUERY_ENC_PADDR,
	FAST_SEND_QUERY_ENC_VADDR,
	FAST_SEND_QUERY_ENC_SIZE,
	FAST_SEND_QUERY_ENC_CTXT,
	FAST_SEND_QUERY_FREE_ISP_ION,
	FAST_SEND_QUERY_FREE_IMG_ION,
	FAST_SEND_QUERY_FREE_ENC_ION,
	FAST_SEND_QUERY_DUMP_MSG,
	FAST_SEND_QUERY_DUMP_MSG_INFO,
	FAST_SEND_QUERY_DUMP_EN,
	FAST_SEND_QUERY_DUMP_DIS,
	FAST_SEND_QUERY_DUMP_JPG,
	FAST_SEND_QUERY_DUMP_JPG_INFO,
	FAST_SEND_QUERY_TRACE_SNAPSHOT_START,
	FAST_SEND_QUERY_TRACE_SNAPSHOT_STOP,
	FAST_SEND_QUERY_TRACE_SNAPSHOT_DUMP,
	FAST_SEND_QUERY_TRACE_STREAM_START,
	FAST_SEND_QUERY_TRACE_STREAM_STOP,
	FAST_SEND_QUERY_TRACE_STREAM_DUMP,
	FAST_SEND_LIMIT,
};

#define FAST_IMAGE_DEV_NAME "cvi-fast-image"
#define FAST_IMAGE_SEND_STOP_REC                _IOW('r', FAST_SEND_STOP_REC, unsigned long)
#define FAST_IMAGE_QUERY_ISP_PADDR              _IOW('r', FAST_SEND_QUERY_ISP_PADDR, unsigned long)
#define FAST_IMAGE_QUERY_ISP_VADDR              _IOW('r', FAST_SEND_QUERY_ISP_VADDR, unsigned long)
#define FAST_IMAGE_QUERY_ISP_SIZE               _IOW('r', FAST_SEND_QUERY_ISP_SIZE, unsigned long)
#define FAST_IMAGE_QUERY_ISP_CTXT               _IOW('r', FAST_SEND_QUERY_ISP_CTXT, unsigned long)
#define FAST_IMAGE_QUERY_IMG_PADDR              _IOW('r', FAST_SEND_QUERY_IMG_PADDR, unsigned long)
#define FAST_IMAGE_QUERY_IMG_VADDR              _IOW('r', FAST_SEND_QUERY_IMG_VADDR, unsigned long)
#define FAST_IMAGE_QUERY_IMG_SIZE               _IOW('r', FAST_SEND_QUERY_IMG_SIZE, unsigned long)
#define FAST_IMAGE_QUERY_IMG_CTXT               _IOW('r', FAST_SEND_QUERY_IMG_CTXT, unsigned long)
#define FAST_IMAGE_QUERY_ENC_PADDR              _IOW('r', FAST_SEND_QUERY_ENC_PADDR, unsigned long)
#define FAST_IMAGE_QUERY_ENC_VADDR              _IOW('r', FAST_SEND_QUERY_ENC_VADDR, unsigned long)
#define FAST_IMAGE_QUERY_ENC_SIZE               _IOW('r', FAST_SEND_QUERY_ENC_SIZE, unsigned long)
#define FAST_IMAGE_QUERY_ENC_CTXT               _IOW('r', FAST_SEND_QUERY_ENC_CTXT, unsigned long)
#define FAST_IMAGE_QUERY_FREE_ISP_ION           _IOW('r', FAST_SEND_QUERY_FREE_ISP_ION, unsigned long)
#define FAST_IMAGE_QUERY_FREE_IMG_ION           _IOW('r', FAST_SEND_QUERY_FREE_IMG_ION, unsigned long)
#define FAST_IMAGE_QUERY_FREE_ENC_ION           _IOW('r', FAST_SEND_QUERY_FREE_ENC_ION, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_MSG               _IOW('r', FAST_SEND_QUERY_DUMP_MSG, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_MSG_INFO          _IOW('r', FAST_SEND_QUERY_DUMP_MSG_INFO, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_EN                _IOW('r', FAST_SEND_QUERY_DUMP_EN, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_DIS               _IOW('r', FAST_SEND_QUERY_DUMP_DIS, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_JPG               _IOW('r', FAST_SEND_QUERY_DUMP_JPG, unsigned long)
#define FAST_IMAGE_QUERY_DUMP_JPG_INFO          _IOW('r', FAST_SEND_QUERY_DUMP_JPG_INFO, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_SNAPSHOT_START   _IOW('r', FAST_SEND_QUERY_TRACE_SNAPSHOT_START, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_SNAPSHOT_STOP    _IOW('r', FAST_SEND_QUERY_TRACE_SNAPSHOT_STOP, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_SNAPSHOT_DUMP    _IOW('r', FAST_SEND_QUERY_TRACE_SNAPSHOT_DUMP, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_STREAM_START     _IOW('r', FAST_SEND_QUERY_TRACE_STREAM_START, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_STREAM_STOP      _IOW('r', FAST_SEND_QUERY_TRACE_STREAM_STOP, unsigned long)
#define FAST_IMAGE_QUERY_TRACE_STREAM_DUMP      _IOW('r', FAST_SEND_QUERY_TRACE_STREAM_DUMP, unsigned long)

#endif // end of __linux__

#define C906_MAGIC_HEADER 0xA55AC906 // master cpu is c906
#define CA53_MAGIC_HEADER 0xA55ACA53 // master cpu is ca53

#ifdef __riscv
#define RTOS_MAGIC_HEADER C906_MAGIC_HEADER
#else
#define RTOS_MAGIC_HEADER CA53_MAGIC_HEADER
#endif

enum E_IMAGE_TYPE {
	E_FAST_NONE = 0,
	E_FAST_JEPG = 1,
	E_FAST_H264,
	E_FAST_H265,
};

enum _MUC_STATUS_E {
	MCU_STATUS_NONOS_INIT = 1,
	MCU_STATUS_NONOS_RUNNING,
	MCU_STATUS_NONOS_DONE,
	MCU_STATUS_RTOS_T1_INIT,  // before linux running
	MCU_STATUS_RTOS_T1_RUNNING,
	MCU_STATUS_RTOS_T2_INIT,  // after linux running
	MCU_STATUS_RTOS_T2_RUNNING,
	MCU_STATUS_LINUX_INIT,
	MCU_STATUS_LINUX_RUNNING,
};

enum DUMP_PRINT_SIZE_E {
	DUMP_PRINT_SZ_IDX_0K = 0,
	DUMP_PRINT_SZ_IDX_4K = 12, // 4096 = 1<<12
	DUMP_PRINT_SZ_IDX_8K,
	DUMP_PRINT_SZ_IDX_16K,
	DUMP_PRINT_SZ_IDX_32K,
	DUMP_PRINT_SZ_IDX_64K,
	DUMP_PRINT_SZ_IDX_128K,
	DUMP_PRINT_SZ_IDX_LIMIT,
};

#define ATTR __attribute__

#ifndef __packed
#define __packed ATTR((packed))
#endif

#ifndef __aligned
#define __aligned(x) ATTR((aligned(x)))
#endif

/* this structure should be modified both fsbl & MCU & osdrv side */
struct transfer_config_t {
	uint32_t conf_magic;
	uint32_t conf_size;  //conf_size exclude mcu_status & linux_status
	uint32_t isp_buffer_addr;
	uint32_t isp_buffer_size;
	uint32_t encode_img_addr;
	uint32_t encode_img_size;
	uint32_t encode_buf_addr;
	uint32_t encode_buf_size;
	uint8_t  dump_print_enable;
	uint8_t  dump_print_size_idx;
	uint16_t image_type;
	uint16_t checksum; // checksum exclude mcu_status & linux_status
	uint8_t  mcu_status;
	uint8_t  linux_status;
} __packed __aligned(0x40);

struct trace_snapshot_t {
	uint32_t ptr;
	uint16_t size;
	uint16_t type;
} __packed;

#endif // end of __FAST_IMAGE_HEADER__
