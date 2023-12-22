SHELL=/bin/bash
-include $(BUILD_PATH)/.config
#
export CVIARCH_L := $(shell echo $(CVIARCH) | tr A-Z a-z)
#
export CHIP_ARCH_L := $(shell echo $(CHIP_ARCH) | tr A-Z a-z)

ifeq ($(KERNEL_DIR), )
$(info Please set KERNEL_DIR global variable!!)
endif

ifeq ($(INSTALL_DIR), )
INSTALL_DIR = ko
endif
CUR_DIR = $(PWD)

$(info ** [ KERNEL_DIR ] ** = $(KERNEL_DIR))
$(info ** [ INSTALL_DIR ] ** = $(INSTALL_DIR))

MAKE_EXT_KO_CP :=
ifneq (${FLASH_SIZE_SHRINK},y)
define MAKE_EXT_KO_CP
	find $(1) -name '*.ko' -print -exec cp {} $(INSTALL_DIR)/3rd/ \;;
endef
endif

define MAKE_EXT_KO
	( cd $(1) && $(MAKE) KERNEL_DIR=$(KERNEL_DIR) all -j$(shell nproc))
	$(call MAKE_EXT_KO_CP, $(1))
endef

SUBDIRS += $(shell find ./extdrv -maxdepth 1 -mindepth 1 -type d | grep -v "git")
exclude_dirs = 
SUBDIRS := $(filter-out $(exclude_dirs), $(SUBDIRS))

# prepare ko list
KO_LIST = wiegand-gpio


OTHERS :=

ifeq (y, ${CONFIG_CP_EXT_WIRELESS})
KO_LIST += wireless
OTHERS += cp_ext_wireless
endif

ifeq (, ${CONFIG_NO_TP})
	KO_LIST += tp
	OTHERS += cp_ext_tp
endif

$(info ** [ KO_LIST ] ** = $(KO_LIST))

export CROSS_COMPILE=$(patsubst "%",%,$(CONFIG_CROSS_COMPILE_KERNEL))
export ARCH=$(patsubst "%",%,$(CONFIG_ARCH))

.PHONY : prepare clean all
all: prepare $(KO_LIST) $(OTHERS)

prepare:
	@mkdir -p $(INSTALL_DIR)/3rd

tp:
	@$(call MAKE_EXT_KO, extdrv/${@})

wireless:
	@$(call MAKE_EXT_KO, extdrv/${@})

wiegand-gpio:
	@$(call MAKE_EXT_KO, extdrv/${@})

cp_ext_wireless:
	@find extdrv/wireless -name '*.ko' -print -exec cp {} $(INSTALL_DIR)/3rd/ \;

cp_ext_tp:
	@find extdrv/tp -name '*.ko' -print -exec cp {} $(INSTALL_DIR)/3rd/ \;

clean:
	@for subdir in $(SUBDIRS); do cd $$subdir && $(MAKE) clean && cd $(CUR_DIR); done
	@rm -f  $(INSTALL_DIR)/*.ko
	@rm -f  $(INSTALL_DIR)/3rd/*.ko
