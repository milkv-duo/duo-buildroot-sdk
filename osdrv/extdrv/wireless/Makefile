#
# Makefile for the Linux Wireless network device drivers.
#

obj-$(CONFIG_WLAN_VENDOR_REALTEK) += realtek/
obj-$(CONFIG_WLAN_VENDOR_BROADCOM) += broadcom/
obj-$(CONFIG_WLAN_VENDOR_MEDIATEK) += mediatek/
obj-$(CONFIG_WLAN_VENDOR_ICOMMSEMI) += icommsemi/

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(shell pwd) modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

clean:
#	$(MAKE) -C $(KSRC) M=$(shell pwd) clean
#	cd hal ; rm -fr */*/*/*.mod.c */*/*/*.mod */*/*/*.o */*/*/.*.cmd */*/*/*.ko
#	cd hal ; rm -fr */*/*.mod.c */*/*.mod */*/*.o */*/.*.cmd */*/*.ko
#	cd hal ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
#	cd hal ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
#	cd core ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
#	cd core ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
#	cd os_dep/linux ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
#	cd os_dep ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
#	cd platform ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	find . -name *.d -delete
	find . -name *.ko | xargs -i rm -rf {}
	find . -name Module.symvers | xargs -i rm -rf {}
	find . -name Module.markers | xargs -i rm -rf {}
	find . -name modules.order | xargs -i rm -rf {}
	find . -name *.mod.c | xargs -i rm -rf {}
	find . -name *.mod | xargs -i rm -rf {}
	find . -name *.o | xargs -i rm -rf {}
	find . -name *.cmd | xargs -i rm -rf {}
	find . -name *~ | xargs -i rm -rf {}
	rm -fr .tmp_versions
#	rm -fr Module.symvers ; rm -fr Module.markers ; rm -fr modules.order
#	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
#	rm -fr .tmp_versions
