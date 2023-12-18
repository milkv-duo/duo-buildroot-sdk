PWD := $(shell pwd)

ccflags-y += -I$(src) -I$(src)/$(CHIP) -I$(srctree)/drivers/tee

obj-m := gt9xx.o

all:modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(shell pwd)  modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

clean:
	rm -f *.o* *.ko
	rm -f Module.symvers modules.order
	rm -f cvi_wiegand.mod.c
	rm -rf .tmp_versions
	rm -f .*cmd
	rm -f $(CHIP)/*.o*
	rm -f $(CHIP)/.*cmd


