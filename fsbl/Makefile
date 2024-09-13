.PHONY: FORCE
# Default goal is build all images
.DEFAULT_GOAL := all

# Avoid any implicit propagation of command line variable definitions to
# sub-Makefiles, like CFLAGS that we reserved for the firmware images'
# usage. Other command line options like "-s" are still propagated as usual.
MAKEOVERRIDES =

S := $(shell printf '\033[1;34;40m  MAKECMDGOALS %s \033[0m\n' '${MAKECMDGOALS}')
$(info ${S})

ARCH ?=

ifneq ($(origin CROSS_COMPILE),command line)
ifeq ($(ARCH),riscv)
CROSS_COMPILE := ${CROSS_COMPILE_GLIBC_RISCV64}
BOOT_CPU ?= riscv
else
CROSS_COMPILE := ${CROSS_COMPILE_64}
BOOT_CPU := aarch64
ARCH := aarch64
endif
endif

ifeq (${CHIP_ARCH},)
$(error CHIP_ARCH is undefined)
endif

ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif

################################################################################
# Default values for build configurations, and their dependencies
################################################################################
MAKE_HELPERS_DIRECTORY := make_helpers/

V ?= 1
DEBUG := 0
LOG_LEVEL := 2
ENABLE_ASSERTIONS := 1
PRINTF_TIMESTAMP := 0

NANDBOOT_V2 := 1


ifeq ($(STORAGE),spinor)
CFLAGS += -DBOOT_SPINOR
endif

ifeq ($(STORAGE),emmc)
CFLAGS += -DBOOT_EMMC
endif

ifeq ($(STORAGE),spinand)
CFLAGS += -DBOOT_SPINAND
endif

# Verbose flag
ifeq (${V},0)
        Q:=@
else
        Q:=
endif
export Q

PRINTABLE	:= abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789
PRINTABLE_TR	:= v1JfTUIeqdsE7P0oiczuVbW9aLnOFySG5YtDQ3lHN4rRpZABwmkCg2XhjM6x8K
CHIP_ARCH	:= $(shell echo '${CHIP_ARCH}' | tr A-Z a-z)
CHIP_ARCH_ALT	:= $(shell echo '${CHIP_ARCH}' | tr '${PRINTABLE}' '${PRINTABLE_TR}')
BUILD_BASE	:= ./build

O ?= ${BUILD_BASE}/${CHIP_ARCH}
BUILD_PLAT	= ${O}

BUILD_STRING := g$(shell git rev-parse --short HEAD 2> /dev/null)
BUILD_STRING := ${BUILD_STRING}$(shell if git diff-index --name-only HEAD | grep -q "."; then echo -dirty; fi)
VERSION_STRING := ${CHIP_ARCH_ALT}:${BUILD_STRING}

################################################################################
# Toolchain
################################################################################
HOSTCC			:=	gcc
export HOSTCC

CC			:=	${CROSS_COMPILE}gcc
CPP			:=	${CROSS_COMPILE}cpp
AS			:=	${CROSS_COMPILE}gcc
AR			:=	${CROSS_COMPILE}ar
LD			:=	${CROSS_COMPILE}ld
OC			:=	${CROSS_COMPILE}objcopy
OD			:=	${CROSS_COMPILE}objdump
NM			:=	${CROSS_COMPILE}nm
PP			:=	${CROSS_COMPILE}gcc -E
READELF			:=	${CROSS_COMPILE}readelf
GDB			:=	${CROSS_COMPILE}gdb

################################################################################
# Generic definitions
################################################################################
include ${MAKE_HELPERS_DIRECTORY}unix.mk
include ${MAKE_HELPERS_DIRECTORY}build_macros.mk
include ${MAKE_HELPERS_DIRECTORY}plat_helpers.mk

################################################################################
# Common sources and include directories
################################################################################
include lib/stdlib/stdlib.mk

################################################################################
# Convert building option
################################################################################
FSBL_SECURE_BOOT_SUPPORT := $(call yn10,${FSBL_SECURE_BOOT_SUPPORT})

################################################################################
# CPU and platform
################################################################################
$(call print_var,CHIP_ARCH)
$(call print_var,BOOT_CPU)

include ${PLAT_MAKEFILE_FULL}

# Check CPU define
$(eval $(call add_define,$(shell echo '${BOOT_CPU}' | tr a-z A-Z)))
ifeq (${BOOT_CPU},aarch64)

else ifeq (${BOOT_CPU},riscv)
else
$(error "BOOT_CPU=${BOOT_CPU} is not supported")
endif

CPPFLAGS += \
	${DEFINES} ${INCLUDES} \
	-nostdinc \
	-Wmissing-include-dirs -Werror

TF_CFLAGS += -ggdb3 -gdwarf-2
ASFLAGS += -g -Wa,--gdwarf-2

# Include the CPU specific operations makefile
include lib/cpu/${BOOT_CPU}/cpu.mk

INCLUDES += -Ibuild
INCLUDES += -Iinclude/cpu

################################################################################
# Build options checks
################################################################################
$(eval $(call assert_boolean,DEBUG))
$(eval $(call assert_boolean,ENABLE_ASSERTIONS))
$(eval $(call assert_boolean,NANDBOOT_V2))
$(eval $(call assert_boolean,NANDBOOT_V2))
$(eval $(call assert_boolean,PAGE_SIZE_64KB))
$(eval $(call assert_boolean,TEST_FROM_SPINOR1))
$(eval $(call assert_boolean,PRINTF_TIMESTAMP))

################################################################################
# Add definitions to the cpp preprocessor based on the current build options.
# This is done after including the platform specific makefile to allow the
# platform to overwrite the default options
################################################################################
$(call print_var,TEST_FROM_SPINOR1)
$(call print_var,PAGE_SIZE_64KB)

$(eval $(call add_define,TEST_FROM_SPINOR1))
$(eval $(call add_define,PAGE_SIZE_64KB))
$(eval $(call add_define,PRINTF_TIMESTAMP))

$(eval $(call add_define,ENABLE_ASSERTIONS))
$(eval $(call add_define,LOG_LEVEL))
$(eval $(call add_define,__CVITEK__))
$(eval $(call add_define,NANDBOOT_V2))

ifeq (${BOOT_CPU},riscv)
$(eval $(call add_define_val,TOC_HEADER_NAME,0xC906B001))
else
$(eval $(call add_define_val,TOC_HEADER_NAME,0xAA640001))
endif

ifeq (${RTOS_ENABLE_FREERTOS},y)
$(eval $(call add_define,RTOS_ENABLE_FREERTOS))
$(eval $(call add_define_val,RTOS_DUMP_PRINT_SZ_IDX,${RTOS_DUMP_PRINT_SZ_IDX}))
$(eval $(call add_define_val,RTOS_FAST_IMAGE_TYPE,${RTOS_FAST_IMAGE_TYPE}))
$(eval $(call add_define_val,RTOS_DUMP_PRINT_ENABLE,$(call yn10,${RTOS_DUMP_PRINT_ENABLE})))
endif

ifeq ($(OD_CLK_SEL),y)
$(eval $(call add_define,OD_CLK_SEL))
endif

$(eval $(call add_define,FSBL_SECURE_BOOT_SUPPORT))
$(eval $(call add_define, USB_DL_BY_FSBL))

ifeq ($(KERNEL_SUSPEND),y)
$(eval $(call add_define,CONFIG_PM_SLEEP))
endif

################################################################################
# Build targets
################################################################################
.PHONY: all fip clean bl-check bl-build fake-blcp
.SUFFIXES:

export BUILD_PLAT NM


all: fip bl2 blmacros


include ${MAKE_HELPERS_DIRECTORY}fip.mk

$(eval $(call MAKE_BL,2))

# Convert '#define ...' to ELF symbols
BLMACROS_LINKERFILE := make_helpers/get_macros.ld.S
BLMACROS_ELF := ${BUILD_PLAT}/blmacros/blmacros.elf
$(eval $(call MAKE_BL,macros))

clean:
	$(print_target)
	$(call SHELL_REMOVE_DIR,${BUILD_PLAT})
