/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __ASM_MACROS_S__
#define __ASM_MACROS_S__

#include <cpu.h>
#include <platform.h>

	.macro atf_state_set _regv, _rega, _val
		li \_regv, \_val
		li \_rega, ATF_STATE_REG
		sw \_regv, (\_rega)
	.endm

	.macro atf_err_set _regv, _rega, _val
		li \_regv, \_val
		li \_rega, ATF_ERR_REG
		sw \_regv, (\_rega)
	.endm

	.macro reg_write _regv, _rega, _addr, _val
		li \_regv, \_val
		li \_rega, \_addr
		sw \_regv, (\_rega)
	.endm

#endif /* __ASM_MACROS_S__ */
