/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/* The log output macros print output to the console. These macros produce
 * compiled log output only if the LOG_LEVEL defined in the makefile (or the
 * make command line) is greater or equal than the level required for that
 * type of log output.
 * The format expected is the same as for printf(). For example:
 * INFO("Info %s.\n", "message")    -> INFO:    Info message.
 * WARN("Warning %s.\n", "message") -> WARNING: Warning message.
 */

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_NOTICE 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_VERBOSE 4

#ifndef __ASSEMBLY__
#include <stddef.h>

int putchar_l(int c, int level);

#define MSG_SOH "\001"
#define MSG_SOH_ASCII ('\001')

#define MSG_ERROR	MSG_SOH "0"
#define MSG_WARN	MSG_SOH "1"
#define MSG_NOTICE	MSG_SOH "2"
#define MSG_INFO	MSG_SOH "3"
#define MSG_VERBOSE	MSG_SOH "4"

#define ERROR(...) tf_printf(MSG_ERROR " E:" __VA_ARGS__)
#define WARN(...) tf_printf(MSG_WARN " W:" __VA_ARGS__)
#define NOTICE(...) tf_printf(MSG_NOTICE __VA_ARGS__)
#define INFO(...) tf_printf(MSG_INFO " I:" __VA_ARGS__)

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define VERBOSE(...) tf_printf(MSG_VERBOSE __VA_ARGS__)
#else
#define VERBOSE(...)
#endif

void __dead2 do_panic(void);
#define panic() do_panic()

/* Function called when stack protection check code detects a corrupted stack */
void __dead2 __stack_chk_fail(void);

void tf_printf(const char *fmt, ...) __printflike(1, 2);
int tf_snprintf(char *s, size_t n, const char *fmt, ...) __printflike(3, 4);
void tf_print_buffer(const void *buf, size_t size, const char *name);

#endif /* __ASSEMBLY__ */

#define ATF_ERR_NONE 0xBEFFFFFF
#define ATF_ERR_BL1_RETURN 0xBE000001

#define ATF_ERR_PLAT_PANIC 0xBE00E002
#define ATF_ERR_PLAT_SYSTEM_RESET 0xBE00E003
#define ATF_ERR_PLAT_SYSTEM_PWR_CYC 0xBE00E004

#define ATF_STATE_RESET_WAIT 0xBE003001
#define ATF_STATE_RESET_RTC_WAIT 0xBE003002

#define ATF_STATE_BL1_ENTRY_POINT 0xB1000000
#define ATF_STATE_BL1_EARLY_PLATFORM_SETUP 0xB1001000

#define ATF_STATE_BL1_APPLY_EFUSE_PATCH 0xB1001020
#define ATF_STATE_BL1_APPLY_SW_CONF 0xB1001022

#define ATF_STATE_BL1_BOOT_SPEED_1 0xB1002001
#define ATF_STATE_BL1_BOOT_SPEED_2 0xB1002002

#define ATF_STATE_BL1_MAIN 0xB100F000

#define ATF_STATE_BL1_DEVICE_SETUP 0xB100F001
#define ATF_STATE_BL1_SD_POW_EN 0xB100F002
#define ATF_STATE_BL1_FLASH_INIT 0xB100F005
#define ATF_STATE_BL1_FLASH_INIT_END 0xB100F006
#define ATF_STATE_BL1_DEVICE_SETUP_END 0xB100F00F

#define ATF_STATE_BL1_SPINOR_INFO 0xB100F100

#define ATF_STATE_BL1_LOAD_PARAM 0xB100F801
#define ATF_STATE_BL1_LOAD_PARAM_END 0xB100F811
#define ATF_STATE_BL1_LOAD_BLCP 0xB100F802
#define ATF_STATE_BL1_LOAD_BLCP_PARAM 0xB100F803
#define ATF_STATE_BL1_LOAD_BL2 0xB100F804

#define ATF_STATE_BL1_JUMP_BL2 0xB100FE00

#define ATF_STATE_BL2_ENTRY_POINT 0xB2000000
#define ATF_STATE_BL2_MAIN 0xB200F000

#define ATF_STATE_WAIT_DEBUG 0xC0001002
#define ATF_STATE_ENTER_DEBUG 0xC0001003

#define ATF_STATE_BOOTSRC_SPI_NAND 0xB3000001
#define ATF_STATE_BOOTSRC_SPI_NOR 0xB3000002
#define ATF_STATE_BOOTSRC_EMMC 0xB3000003
#define ATF_STATE_BOOTSRC_SD 0xB3000004
#define ATF_STATE_BOOTSRC_USB 0xB3000005
#define ATF_STATE_BOOTSRC_UART 0xB3000006

#define ATF_STATE_USB_ENUM_DONE 0xC0001004
#define ATF_STATE_USB_SEND_VID_DONE 0xC0001005
#define ATF_STATE_FIP_HEADER_IS_VALID 0xC0002001

#define ATF_STATE_USB_WAIT_ENUM 0xC0003101
#define ATF_STATE_USB_PHY_SETTING_DONE 0xC0003001
#define ATF_STATE_USB_HW_INIT_DONE 0xC0003002
#define ATF_STATE_USB_DL_BY_CONF_DONE 0xC0003003
#define ATF_STATE_USB_ENUM_FAIL 0xC0003004
#define ATF_STATE_USB_DATA_TIMEOUT 0xC0003005
#define ATF_STATE_USB_TRANSFER_DONE 0xC0003006
#define ATF_STATE_USB_ERR 0xC0003007
#define ATF_STATE_USB_SECOND_STAGE 0xC0003008
#define ATF_STATE_USB_SET_HSPERI_CLK_DONE 0xC0003009
#define ATF_STATE_USB_UNSET_HSPERI_CLK_DONE 0xC000300A

#define ATF_STATE_UART_WAIT_MAGIC 0xC0004004

#define ATF_STATE_SPINAND_INIT 0xD0001000
#define ATF_STATE_SPINAND_SCAN_VECTOR 0xD0001002
#define ATF_STATE_SPINAND_SCAN_FIP_BLKS 0xD0001003
#define ATF_STATE_SPINAND_READ_FIP_BIN 0xD0001004

#define CP_STATE_ENTRY_POINT 0xB8000000
#define CP_STATE_INIT_DONE 0xB800000F

#endif /* __DEBUG_H__ */
