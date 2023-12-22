EXTRA_CFLAGS+= $(MODULE_FLAGS)

MODULE_NAME=calib_robust_test

OBJS:= calib_robust_main.o

OBJS+= calib_robust_proc.o
OBJS+= calib_robust_file.o

#######################################
# For integrating to kernel source
######################################
obj-m += $(MODULE_NAME).o


$(MODULE_NAME)-objs += $(OBJS)

install:

clean:
	rm -rf *.o *.ko *.modules
