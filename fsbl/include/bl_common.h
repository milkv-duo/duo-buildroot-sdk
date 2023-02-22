/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __BL_COMMON_H__
#define __BL_COMMON_H__

/*******************************************************************************
 * Constants to indicate type of exception to the common exception handler.
 ******************************************************************************/
#ifndef __ASSEMBLY__
#include <stddef.h>
#include <stdint.h>
#include <types.h>
#include <utils_def.h> /* To retain compatibility */

extern const char build_message[];
extern const char version_string[];

extern unsigned char __XLAT_VARS_START__[];
extern unsigned char __BSS_START__[];
extern unsigned char __BSS_END__[];
extern unsigned char __DATA_RAM_START__[];
extern unsigned char __DATA_RAM_END__[];
extern unsigned char __DATA_ROM_START__[];

void bl1_early_platform_setup(void);

#endif /*__ASSEMBLY__*/

#endif /* __BL_COMMON_H__ */
