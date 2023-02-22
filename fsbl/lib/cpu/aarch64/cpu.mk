include lib/cpu/${BOOT_CPU}/cpu-ops.mk

ASFLAGS +=\
	$(CPPFLAGS) \
	-D__ASSEMBLY__ \
	-march=armv8-a \
	-ffreestanding  \
	-Wa,--fatal-warnings

TF_CFLAGS += \
	$(CPPFLAGS) \
	-march=armv8-a -mstrict-align \
	-ffreestanding -fno-builtin -Wall -std=gnu99 \
	-Os -ffunction-sections -fdata-sections \
	-fno-delete-null-pointer-checks

TF_LDFLAGS += \
	--fatal-warnings -Os \
	--gc-sections \
	${TF_LDFLAGS_aarch64}

CPU_INCLUDES :=

CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/cpu_helper.c \
	lib/cpu/${BOOT_CPU}/cache_helpers.S \
	lib/cpu/${BOOT_CPU}/misc_helpers.S \
	lib/cpu/${BOOT_CPU}/delay_timer.c \
	lib/cpu/${BOOT_CPU}/console.S \
	lib/cpu/${BOOT_CPU}/bl2_helper.c \
	plat/${CHIP_ARCH}/uart/16550_console_aarch64.S

BL1_CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/bl1_entrypoint.S \
	lib/cpu/${BOOT_CPU}/bl1_exceptions.S

BL2_CPU_SOURCES := \
	lib/cpu/${BOOT_CPU}/bl2_entrypoint.S \
	lib/cpu/${BOOT_CPU}/bl2_exceptions.S
