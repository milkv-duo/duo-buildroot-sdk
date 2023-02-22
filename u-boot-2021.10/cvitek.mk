ifeq (${UBOOT_VBOOT}, 1)
KBUILD_CPPFLAGS += -DUBOOT_VBOOT
endif

ifeq (${CONFIG_SKIP_RAMDISK},y)
KBUILD_CPPFLAGS += -DCONFIG_SKIP_RAMDISK=${CONFIG_SKIP_RAMDISK}
endif

ifeq (${CONFIG_USE_DEFAULT_ENV},y)
KBUILD_CPPFLAGS += -DCONFIG_USE_DEFAULT_ENV=${CONFIG_USE_DEFAULT_ENV}
endif

ifeq (${STORAGE_TYPE}, spinand)
KBUILD_CFLAGS += -DCONFIG_NAND_SUPPORT
KBUILD_CFLAGS += -DSPINAND_SQSH
endif

ifeq (${STORAGE_TYPE}, emmc)
KBUILD_CFLAGS += -DCONFIG_EMMC_SUPPORT
endif

ifeq (${STORAGE_TYPE}, sd)
KBUILD_CFLAGS += -DCONFIG_SD_BOOT
endif

ifeq (${RELEASE}, 1)
KBUILD_CPPFLAGS += -DRELEASE
endif

cvichip = $(shell echo $(CHIP) | tr a-z A-Z)
cviboard = $(shell echo $(CVIBOARD) | tr a-z A-Z)
KBUILD_CPPFLAGS += -DCVICHIP=${CHIP}
KBUILD_CPPFLAGS += -DCVIBOARD=${CVIBOARD} -D${cvichip}_${cviboard}

ifneq (${PANEL_TUNING_PARAM},)
cvi_panel = $(shell echo $(PANEL_TUNING_PARAM) | tr a-z A-Z)
KBUILD_CPPFLAGS += -D${cvi_panel}
KBUILD_CPPFLAGS += $(if $(findstring I80,$(cvi_panel)),-D${cvichip}_${cviboard}_I80,)
endif

ifneq (${PANEL_LANE_NUM_TUNING_PARAM},)
KBUILD_CPPFLAGS += -D$(shell echo $(PANEL_LANE_NUM_TUNING_PARAM) | tr a-z A-Z)
endif

ifneq (${PANEL_LANE_SWAP_TUNING_PARAM},)
KBUILD_CPPFLAGS += -D$(shell echo $(PANEL_LANE_SWAP_TUNING_PARAM) | tr a-z A-Z)
endif

KBUILD_CPPFLAGS += $(if $(findstring CV183X,$(CHIP_ARCH)),-DBOOTLOGO_ISP_RESET,)

KBUILD_CFLAGS += -I$(srctree)/include/cvitek
