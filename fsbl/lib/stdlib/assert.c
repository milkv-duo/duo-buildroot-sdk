/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <console.h>
#include <debug.h>
#include <platform.h>

void __assert(const char *file, unsigned int line, const char *assertion)
{
	tf_printf("ASSERT: %s <%d> : %s\n", file, line, assertion);
	console_flush();
	panic_handler();
}