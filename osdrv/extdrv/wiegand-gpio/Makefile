PWD := $(shell pwd)

ccflags-y += -I$(src) -I$(src)/$(CHIP) -I$(srctree)/drivers/tee

obj-m += cvi_wiegand_gpio.o

all:
	$(MAKE) ARCH=$(ARCH) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	rm -f *.o* *.ko
	rm -f Module.symvers modules.order
	rm -f cvi_wiegand.mod.c
	rm -rf .tmp_versions
	rm -f .*cmd
	rm -f *.mod.c
	rm -f $(CHIP)/*.o*
	rm -f $(CHIP)/.*cmd
