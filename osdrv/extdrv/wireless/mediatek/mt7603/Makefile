#MODE - STA or AP
ifeq ($(WIFI_MODE),)
RT28xx_MODE = STA
else
RT28xx_MODE = $(WIFI_MODE)
endif

#TARTET - LINUX, UCOS, ECOS, or THREADX.
ifeq ($(TARGET),)
TARGET = LINUX
endif

# CHIPSET
# rt2860, rt2870, rt2880, rt2070, rt3070, rt3090, rt3572, rt3062, rt3562, rt3593, rt3573
# rt3562(for rt3592), rt3050, rt3350, rt3352, rt5350, rt5370, rt5390, rt5572, rt5592, 
# rt8592(for rt85592), 
# mt7601e, mt7601u,
# mt7650e, mt7630e, mt7610e, mt7650u, mt7630u, mt7610u
# mt7662e, mt7632e, mt7612e, mt7662u, mt7632u, mt7612u
# mt7603e, mt7603u, mt7636u, mt7636s

ifeq ($(CHIPSET),)
#CHIPSET = mt7601u
#CHIPSET = mt7650e mt7630e mt7610e
#CHIPSET = mt7650u mt7630u mt7610u
#CHIPSET = mt7662e mt7632e mt7612e
#CHIPSET = mt7662u mt7632u mt7612u
#CHIPSET = mt7603e
CHIPSET = mt7603u
#CHIPSET = mt7636u
#CHIPSET = mt7636s
endif

MODULE = $(word 1, $(CHIPSET))

#OS ABL - YES or NO
OSABL = NO

#Build Prealloc ko
#PREALLOC = NO
PREALLOC = YES

ifneq ($(TARGET),THREADX)
#RT28xx_DIR = home directory of RT28xx source code
#RT28xx_DIR = $(PWD)
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
RT28xx_DIR := $(dir $(MAKEFILE_PATH))
endif

include $(RT28xx_DIR)/os/linux/config.mk

RTMP_SRC_DIR = $(RT28xx_DIR)/RT$(MODULE)

#PLATFORM: Target platform
#PLATFORM = PC
#PLATFORM = MSTAR
#PLATFORM = HISILICON
#PLATFORM = HE_TV
#PLATFORM = MT53XX

LINUX_SRC = /media/cvitek/jinyu.zhao/linux/build/cv1838_wevb_0001a
CROSS_COMPILE =aarch64-linux-gnu-
ARCH ?= arm64

#APSOC
ifeq ($(MODULE),3050)
PLATFORM = RALINK_3050
endif
ifeq ($(MODULE),3052)
PLATFORM = RALINK_3052
endif
ifeq ($(MODULE),3350)
PLATFORM = RALINK_3050
endif
ifeq ($(MODULE),3352)
PLATFORM = RALINK_3352
endif
ifeq ($(CHIPSET),mt7628)
PLATFORM = MT7628
endif

#RELEASE Package
RELEASE = DPOA


ifeq ($(TARGET),LINUX)
MAKE = make
endif

ifeq ($(TARGET), UCOS)
MAKE = make
endif
ifeq ($(TARGET),THREADX)
MAKE = gmake
endif

ifeq ($(TARGET), ECOS)
MAKE = make
MODULE = $(shell pwd | sed "s/.*\///" ).o
export MODULE
endif


ifeq ($(PLATFORM),PC)
PREALLOC = NO
# Linux 2.6
LINUX_SRC = /lib/modules/$(shell uname -r)/build
# Linux 2.4 Change to your local setting
#LINUX_SRC = /usr/src/linux-2.4
LINUX_SRC_MODULE = /lib/modules/$(shell uname -r)/kernel/drivers/net/wireless/
endif

ifeq ($(PLATFORM),MSTAR)
PREALLOC = YES
endif

ifeq ($(PLATFORM),HISILICON)
PREALLOC = YES
endif

ifeq ($(PLATFORM),HE_TV)
PREALLOC = NO
TARGET = LINUX
#****** For system auto build ******
#LINUX_SRC=$(KERNEL_OBJ_ROOT)/$(KERNEL_VER)/$(KERNEL_CONFIG)_modules
#****** For local build ******
# uncomment the following lines
VM_LINUX_ROOT ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
LINUX_ROOT ?= $(VM_LINUX_ROOT)
ifeq ($(LINUX_SRC),)
LINUX_SRC=$(VM_LINUX_ROOT)/output/mtk_android/mt5399_cn_android/rel/obj/kernel/chiling/kernel/linux-3.10/mt5399_android_smp_mod_defconfig_modules
endif
OBJ_ROOT ?= $(VM_LINUX_ROOT)/output/mtk_android/mt5399_cn_android/rel/obj
export KERNEL_OBJ_ROOT=$(LINUX_SRC)/../..
ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE=/mtkoss/gnuarm/vfp_4.5.1_2.6.27_cortex-a9-rhel4/i686/bin/armv7a-mediatek451_001_vfp-linux-gnueabi-
endif
ifeq "$(CC)" "gcc"
CC ?= $(CROSS_COMPILE)gcc
endif
$(warning =============================================)
$(warning CC=$(CC) for wifi driver LINUX_SRC=$(LINUX_SRC))
$(warning TARGET=$(TARGET))
$(warning =============================================)
endif

export WIFI_TARGET := mt7603_cfg80211
ifeq ($(PLATFORM),MT53XX)
MOD_PREALLOC = mtprealloc
TARGET = LINUX
ifeq "$(UTOPIA)" "true"
    LINUX_SRC = $(UTOPIA_KERNEL_ROOT)/$(subst linux-,,$(KERNEL_VER))
    OUT_STA_KO = $(LINUX_ROOT)/linux_mts/ko_modules/wlan/mtk/$(WIFI_TARGET)/os/linux/
else
    LINUX_SRC=$(KERNEL_OBJ_ROOT)/$(KERNEL_VER)/$(KERNEL_CONFIG)_modules
    #LINUX_SRC=../../../../../output/Sony_linux/Stellar_US/rel/obj/kernel/linux_core/kernel/linux-3.0/mt5881_mod_defconfig_modules
    OUT_STA_KO = $(OBJ_ROOT)/linux_mts/ko_modules/wlan/mtk/$(WIFI_TARGET)/os/linux/
endif
CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)

include $(LINUX_ROOT)/linux_mts/build/mak/toolchain.mak

ifeq "$(CC)" "gcc"
CC ?= $(CROSS_COMPILE)gcc
endif
#ifeq ($(CROSS_COMPILE),)
#CROSS_COMPILE=armv7a-mediatek451_001_vfp-linux-gnueabi-
#endif
$(warning =============================================)
$(warning CC=$(CC) ODB=$(ODB) for wifi driver LINUX_SRC=$(LINUX_SRC))
$(warning TARGET=$(TARGET))
$(warning KERNEL_VER_FOR_3RD=$(KERNEL_VER_FOR_3RD))
$(warning OUTPUT_ROOT=$(OUTPUT_ROOT))
$(warning RT28xx_DIR=$(RT28xx_DIR))
endif

export OSABL RT28xx_DIR RT28xx_MODE LINUX_SRC CROSS_COMPILE CROSS_COMPILE_INCLUDE PLATFORM RELEASE CHIPSET MODULE RTMP_SRC_DIR LINUX_SRC_MODULE TARGET HAS_WOW_SUPPORT PREALLOC

$(shell cp -f $(RT28xx_DIR)/os/linux/Makefile.6 $(RT28xx_DIR)/os/linux/Makefile)
obj-y += /os/linux/

# The targets that may be used.
PHONY += all build_tools test UCOS THREADX LINUX release prerelease clean uninstall install libwapi osabl sdk_build_tools

ifeq ($(TARGET),LINUX)
all: build_tools $(TARGET) plug_in
else
all: $(TARGET)
endif 

build_tools:
	$(MAKE) -C tools
	$(RT28xx_DIR)/tools/bin2h

sdk_build_tools:
	if [ -f $(RT28xx_DIR)/eeprom_log ]; then \
		rm -f $(RT28xx_DIR)/eeprom_log
	fi
	echo $(EE_TYPE) >> eeprom_log
	echo $(CHIPSET) >> eeprom_log
	if [ -f $(RT28xx_DIR)/eeprom/SA/MT7603E_EEPROM.bin ]; then \
		echo 'find SA/MT7603E_EEPROM.bin' >> eeprom_log ; \
		cp -f $(RT28xx_DIR)/eeprom/SA/MT7603E_EEPROM.bin $(RT28xx_DIR)/eeprom/MT7603E_EEPROM.bin ; \
	else \
		cp -f $(RT28xx_DIR)/eeprom/$(EE_TYPE)/MT7603E_EEPROM.bin $(RT28xx_DIR)/eeprom/MT7603E_EEPROM.bin ; \
	fi
	$(MAKE) -C tools
	$(RT28xx_DIR)/tools/bin2h
#	rm -f $(RT28xx_DIR)/eeprom/SA/MT7603E_EEPROM.bin
	
test:
	$(MAKE) -C tools test

UCOS:
	$(MAKE) -C os/ucos/ MODE=$(RT28xx_MODE)
	echo $(RT28xx_MODE)

ECOS:
	$(MAKE) -C os/ecos/ MODE=$(RT28xx_MODE)
	cp -f os/ecos/$(MODULE) $(MODULE)

THREADX:
	$(MAKE) -C $(RT28xx_DIR)/os/Threadx -f $(RT28xx_DIR)/os/ThreadX/Makefile

LINUX:
ifneq (,$(findstring 2.4,$(LINUX_SRC)))

ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.4.util $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
endif

	cp -f os/linux/Makefile.4 $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/

ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.4.netif $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
endif

ifeq ($(RT28xx_MODE),AP)
ifneq ($(PLATFORM),BB_SOC)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_ap.o /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/rtutil$(MODULE)_ap.o /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/rtnet$(MODULE)_ap.o /tftpboot
endif
ifeq ($(PLATFORM),INF_AMAZON_SE)
	cp -f /tftpboot/rt2870ap.o /backup/ifx/build/root_filesystem/lib/modules/2.4.31-Amazon_SE-3.6.2.2-R0416_Ralink/kernel/drivers/net
endif
endif #BB_SOC
else #AP	
ifeq ($(RT28xx_MODE),APSTA)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_apsta.o /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/rtutil$(MODULE)_apsta.o /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/rtnet$(MODULE)_apsta.o /tftpboot
endif
else #APSTA
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_sta.o /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/rtutil$(MODULE)_sta.o /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/rtnet$(MODULE)_sta.o /tftpboot
endif
endif #APSTA
endif #AP
else #2.4

ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.util $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
endif
ifeq ($(PREALLOC), YES)
#build prealloc.ko
	cp -f os/linux/Makefile.6.prealloc os/linux/Makefile
	$(MAKE) ARCH=$(ARCH) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
	$(CROSS_COMPILE)strip --strip-unneeded $(RT28xx_DIR)/os/linux/mtprealloc.ko
endif
	cp -f os/linux/Makefile.6 $(RT28xx_DIR)/os/linux/Makefile
ifeq ($(PLATFORM),DM6446)
	$(MAKE)  ARCH=arm CROSS_COMPILE=arm_v5t_le- -C  $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
else
ifeq ($(PLATFORM),FREESCALE8377)
	$(MAKE) ARCH=powerpc CROSS_COMPILE=$(CROSS_COMPILE) -C  $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
else
#build mt7603u_sta.ko
	$(MAKE) ARCH=$(ARCH) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
	$(CROSS_COMPILE)strip --strip-unneeded $(RT28xx_DIR)/os/linux/mt7603u_sta.ko
endif
endif #DM6446

ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.netif $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
endif

ifeq ($(RT28xx_MODE),AP)
ifneq ($(PLATFORM),BB_SOC)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_ap.ko /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_ap_util.ko /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_ap_net.ko /tftpboot
endif
else #BB_SOC
	rm -f os/linux/$(MODULE)_ap.ko.lzma
	/root/bin/lzma e os/linux/$(MODULE)_ap.ko os/linux/$(MODULE)_ap.ko.lzma
endif #BB_SOC
else #AP	
ifeq ($(RT28xx_MODE),APSTA)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_apsta.ko /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_apsta_util.ko /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_apsta_net.ko /tftpboot
endif
else #APSTA
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_sta_util.ko /tftpboot
	cp -f $(RT28xx_DIR)/os/linux/$(MODULE)_sta_net.ko /tftpboot
endif
ifeq ($(PLATFORM),MT53XX)
ifneq ($(ANDROID),true)
	if [ ! -d $(OUTPUT_ROOT)/basic/wifi_ko ]; then \
		mkdir -p $(OUTPUT_ROOT)/basic/wifi_ko/; \
	fi
	if [ -d $(OUTPUT_ROOT)/basic/wifi_ko ]; then \
	cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(OUTPUT_ROOT)/basic/wifi_ko/mt7603u_sta.ko; \
	fi
ifeq "$(PREALLOC)" "YES"
	if [ -d $(OUTPUT_ROOT)/basic/wifi_ko ]; then \
		cp -f $(RT28xx_DIR)/os/linux/$(MOD_PREALLOC).ko $(OUTPUT_ROOT)/basic/wifi_ko/$(MOD_PREALLOC).ko; \
	fi
endif
	$(CROSS_COMPILE)strip --strip-unneeded $(OUTPUT_ROOT)/basic/wifi_ko/*.ko
else
	if [ ! -d $(OUTPUT_ROOT)/basic/modules ]; then \
		mkdir -p $(OUTPUT_ROOT)/basic/modules/; \
	fi
ifeq "$(UTOPIA)" "true"
	if [ -d $(OUTPUT_ROOT)/basic/modules ]; then \
		cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(OUTPUT_ROOT)/basic/modules/wlan.ko; \
		cp -f $(RT28xx_DIR)/os/linux/$(MOD_PREALLOC).ko $(OUTPUT_ROOT)/basic/modules/mtprealloc.ko; \
	fi
else
	if [ -d $(OUTPUT_ROOT)/basic/modules ]; then \
		cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(OUTPUT_ROOT)/basic/modules/wlan.ko; \
	fi
ifeq "$(PREALLOC)" "YES"
	if [ -d $(OUTPUT_ROOT)/basic/modules ]; then \
		cp -f $(RT28xx_DIR)/os/linux/$(MOD_PREALLOC).ko $(OUTPUT_ROOT)/basic/modules/$(MOD_PREALLOC).ko; \
	fi
endif
endif
#$(CROSS_COMPILE)strip --strip-unneeded $(THIRDPARTY_LIB_ROOT)/wlan/$(KERNEL_VER_FOR_3RD)/mtk/$(WIFI_TARGET)/*.ko
endif
endif #MT53XX
endif #APSTA
endif #AP
endif #2.4
ifeq ($(PLATFORM),MT85XX)
	mkdir -p $(RT28xx_DIR)/../../../../../BDP_Generic/build_linux_ko/src/driver/wlan/
	cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(RT28xx_DIR)/../../../../../BDP_Generic/build_linux_ko/src/driver/wlan/
endif

plug_in:
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/tools/plug_in MODULE_FLAGS="$(WFLAGS)"

release: build_tools
	$(MAKE) -C $(RT28xx_DIR)/striptool -f Makefile.release clean
	$(MAKE) -C $(RT28xx_DIR)/striptool -f Makefile.release
	striptool/striptool.out
ifeq ($(PLATFORM),MT85XX)
	cp -f tools/osabl_check.sh $(RELEASE)/
endif
ifeq ($(RELEASE), DPO)
	gcc -o striptool/banner striptool/banner.c
	./striptool/banner -b striptool/copyright.gpl -s DPO/ -d DPO_GPL -R
	./striptool/banner -b striptool/copyright.frm -s DPO_GPL/include/firmware.h
endif

prerelease:
ifeq ($(MODULE), 2880)
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.release.2880 prerelease
else
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.release prerelease
endif
	cp $(RT28xx_DIR)/os/linux/Makefile.DPB $(RTMP_SRC_DIR)/os/linux/.
	cp $(RT28xx_DIR)/os/linux/Makefile.DPA $(RTMP_SRC_DIR)/os/linux/.
	cp $(RT28xx_DIR)/os/linux/Makefile.DPC $(RTMP_SRC_DIR)/os/linux/.
ifeq ($(RT28xx_MODE),STA)
	cp $(RT28xx_DIR)/os/linux/Makefile.DPD $(RTMP_SRC_DIR)/os/linux/.
	cp $(RT28xx_DIR)/os/linux/Makefile.DPO $(RTMP_SRC_DIR)/os/linux/.
endif	

clean:
ifeq ($(TARGET), LINUX)
	cp -f os/linux/Makefile.clean os/linux/Makefile
	$(MAKE) -C os/linux clean
	rm -rf os/linux/Makefile
endif	
ifeq ($(TARGET), UCOS)
	$(MAKE) -C os/ucos clean MODE=$(RT28xx_MODE)
endif
ifeq ($(TARGET), ECOS)
	$(MAKE) -C os/ecos clean MODE=$(RT28xx_MODE)
endif

uninstall:
ifeq ($(TARGET), LINUX)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.4 uninstall
else
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.6 uninstall
endif
endif

install:
ifeq ($(TARGET), LINUX)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.4 install
else
	$(MAKE) -C $(RT28xx_DIR)/os/linux -f Makefile.6 install
endif
endif

libwapi:
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.libwapi.4 $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
else
	cp -f os/linux/Makefile.libwapi.6 $(RT28xx_DIR)/os/linux/Makefile	
	$(MAKE) -C  $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules	
endif	

osutil:
ifeq ($(OSABL),YES)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.4.util $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
else
	cp -f os/linux/Makefile.6.util $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
ifeq ($(PLATFORM),MT85XX)
	cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(RT28xx_DIR)/../../../../../../BDP_Generic/build_linux_ko/src/driver/wlan/
endif
endif
endif

osnet:
ifeq ($(OSABL),YES)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.4.netif $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
else
	cp -f os/linux/Makefile.6.netif $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
ifeq ($(PLATFORM),MT85XX)
	cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(RT28xx_DIR)/../../../../../../BDP_Generic/build_linux_ko/src/driver/wlan/
endif
endif
endif

osdrv:
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.4 $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(RT28xx_DIR)/os/linux/
else
	cp -f os/linux/Makefile.6 $(RT28xx_DIR)/os/linux/Makefile
	$(MAKE) -C $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
ifeq ($(PLATFORM),MT85XX)
ifeq ($(OSABL),YES)
	cp -f $(RT28xx_DIR)/os/linux/mt7603u_sta.ko $(RT28xx_DIR)/../../../../../../BDP_Generic/build_linux_ko/src/driver/wlan/
endif
endif
endif

# Declare the contents of the .PHONY variable as phony.  We keep that information in a variable
.PHONY: $(PHONY)



