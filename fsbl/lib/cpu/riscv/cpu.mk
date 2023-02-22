#include lib/cpu/${BOOT_CPU}/cpu-ops.mk

ASFLAGS +=\
	$(CPPFLAGS) \
	-DRISCV \
	-D__ASSEMBLY__ \
	-march=rv64imafdcvxthead -mstrict-align \
	-mcmodel=medany \
	-mabi=lp64d \
	-ffreestanding  \
	-Wa,--fatal-warnings

TF_CFLAGS += \
	$(CPPFLAGS) \
	-DRISCV \
	-march=rv64imafdcvxthead \
	-mcmodel=medany \
	-mabi=lp64d \
	-ffreestanding -fno-builtin -Wall -std=gnu99 \
	-Os -ffunction-sections -fdata-sections \
	-fno-delete-null-pointer-checks

TF_LDFLAGS += \
	--fatal-warnings -Os \
	--gc-sections \
	${TF_LDFLAGS_aarch64}

CPU_INCLUDES := \
	-Iinclude/cpu/${BOOT_CPU} \

CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/cpu_helper.c \
	lib/cpu/${BOOT_CPU}/misc_helpers.c \
	lib/cpu/${BOOT_CPU}/cache.c \
	lib/cpu/${BOOT_CPU}/delay_timer.c \
	lib/cpu/${BOOT_CPU}/bl2_helper.c \
	plat/${CHIP_ARCH}/uart/uart_dw.c


BL1_CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/bl1_entrypoint.S \
#	lib/cpu/${BOOT_CPU}/bl1_exceptions.S

BL2_CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/bl2_entrypoint.S \
#	lib/cpu/${BOOT_CPU}/bl2_exceptions.S
