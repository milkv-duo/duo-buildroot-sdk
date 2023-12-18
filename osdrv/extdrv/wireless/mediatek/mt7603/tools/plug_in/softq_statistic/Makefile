EXTRA_CFLAGS+= $(MODULE_FLAGS)

MODULE_NAME=softq_stat

OBJS:= softq_statistic.o

OBJS+= softq_stat_proc.o
OBJS+= softq_stat_file.o

#######################################
# For integrating to kernel source
######################################
obj-m += $(MODULE_NAME).o


$(MODULE_NAME)-objs += $(OBJS)

install:

clean:
	rm -rf *.o *.ko *.modules
