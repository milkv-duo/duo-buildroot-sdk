CONFIG_AIC8800_BTLPM_SUPPORT := m
CONFIG_AIC8800_WLAN_SUPPORT := m
CONFIG_AIC8800 := m

obj-$(CONFIG_AIC8800_BTLPM_SUPPORT) += aic8800_btlpm/
obj-$(CONFIG_AIC8800_WLAN_SUPPORT) += aic8800_fdrv/
obj-$(CONFIG_AIC8800) += aic8800_bsp/


# Platform support list
CONFIG_PLATFORM_ROCKCHIP = n
CONFIG_PLATFORM_ROCKCHIP2 = n
CONFIG_PLATFORM_ALLWINNER = n
CONFIG_PLATFORM_AMLOGIC = n
CONFIG_PLATFORM_UBUNTU = n
CONFIG_PLATFORM_CVITEK = y

MAKEFLAGS +=-j$(shell nproc)

ifeq ($(CONFIG_PLATFORM_ROCKCHIP), y)
#KDIR = /home/yaya/E/Rockchip/3229/Android7/RK3229_ANDROID7.1_v1.01_20170914/rk3229_Android7.1_v1.01_xml0914/kernel
#ARCH = arm
#CROSS_COMPILE = /home/yaya/E/Rockchip/3229/Android7/RK3229_ANDROID7.1_v1.01_20170914/rk3229_Android7.1_v1.01_xml0914/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KDIR  = /home/yaya/E/Rockchip/3229/Android9/rk3229_android9.0_box/kernel
ARCH = arm
CROSS_COMPILE = /home/yaya/E/Rockchip/3229/Android9/rk3229_android9.0_box/prebuilts/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
#KDIR  = /home/yaya/E/Rockchip/3399/rk3399-android-10/kernel
#ARCH = arm64
#CROSS_COMPILE = /home/yaya/E/Rockchip/3399/rk3399-android-10/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
ccflags-y += -DANDROID_PLATFORM
ccflags-y += -DCONFIG_PLATFORM_ROCKCHIP
endif

ifeq ($(CONFIG_PLATFORM_ROCKCHIP2), y)
ARCH = arm64
KDIR = /home/yaya/E/Rockchip/3566/firefly/Android11.0/Firefly-RK356X_Android11.0_git_20210824/RK356X_Android11.0/kernel
CROSS_COMPILE = /home/yaya/E/Rockchip/3566/firefly/Android11.0/Firefly-RK356X_Android11.0_git_20210824/RK356X_Android11.0/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
ccflags-y += -DANDROID_PLATFORM
ccflags-y += -DCONFIG_PLATFORM_ROCKCHIP2
endif


ifeq ($(CONFIG_PLATFORM_ALLWINNER), y)
KDIR  = /home/yaya/E/Allwinner/R818/R818/AndroidQ/lichee/kernel/linux-4.9
ARCH = arm64
CROSS_COMPILE = /home/yaya/E/Allwinner/R818/R818/AndroidQ/lichee/out/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
ccflags-y += -DANDROID_PLATFORM
endif

ifeq ($(CONFIG_PLATFORM_AMLOGIC), y)
ccflags-y += -DANDROID_PLATFORM
ARCH = arm
CROSS_COMPILE = /home/yaya/D/Workspace/CyberQuantum/JinHaoYue/amls905x3/SDK/20191101-0tt-asop/android9.0/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androidkernel-
KDIR = /home/yaya/D/Workspace/CyberQuantum/JinHaoYue/amls905x3/SDK/20191101-0tt-asop/android9.0/out/target/product/u202/obj/KERNEL_OBJ/

endif

ifeq ($(CONFIG_PLATFORM_UBUNTU), y)
KDIR  = /lib/modules/$(shell uname -r)/build
PWD   = $(shell pwd)
KVER = $(shell uname -r)
MODDESTDIR = /lib/modules/$(KVER)/kernel/drivers/net/wireless/aic8800
ARCH = x86_64
CROSS_COMPILE =
endif

ifeq ($(CONFIG_PLATFORM_CVITEK), y)
ARCH ?= riscv
#CROSS_COMPILE ?=
endif

all: modules
modules:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

install:
	mkdir -p $(MODDESTDIR)
	install -p -m 644 aic8800_bsp/aic8800_bsp.ko  $(MODDESTDIR)/
	install -p -m 644 aic8800_fdrv/aic8800_fdrv.ko  $(MODDESTDIR)/
	install -p -m 644 aic8800_btlpm/aic8800_btlpm.ko  $(MODDESTDIR)/
	/sbin/depmod -a ${KVER}

uninstall:
	rm -rfv $(MODDESTDIR)/aic8800_bsp.ko
	rm -rfv $(MODDESTDIR)/aic8800_fdrv.ko
	rm -rfv $(MODDESTDIR)/aic8800_btlpm.ko
	/sbin/depmod -a ${KVER}

clean:
	cd aic8800_bsp/;make clean;cd ..
	cd aic8800_fdrv/;make clean;cd ..
	cd aic8800_btlpm/;make clean;cd ..
	rm -rf modules.order Module.symvers .tmp_versions/

