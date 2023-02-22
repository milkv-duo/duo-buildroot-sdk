#
# Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

STDLIB_SRCS := \
	$(addprefix lib/stdlib/, \
	misc.c \
	assert.c \
	mem.c \
	putchar.c \
	strchr.c \
	strcmp.c \
	strlen.c \
	strncmp.c \
	strnlen.c)

STDLIB_INCLUDES := \
	-Iinclude/stdlib \
	-Iinclude/stdlib/sys
