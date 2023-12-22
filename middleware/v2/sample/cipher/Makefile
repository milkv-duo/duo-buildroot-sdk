SHELL = /bin/bash

STATIC ?= 0

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -I$(MW_INC) -I../common/ -I. -I$(KERNEL_INC)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

TARGET = sample_cipher
ifeq ($(CONFIG_ENABLE_SDK_ASAN), y)
TARGET = sample_cipher_asan
endif

EXTRA_CFLAGS = $(INCS)

ldflags-y += -lcipher -lmisc -lsys
ldflags-y += -lcvi_bin -lcvi_bin_isp $(ISP_LIB)
ifeq ($(MULTI_PROCESS_SUPPORT), 1)
ldflags-y += -lnanomsg -lvenc -lvdec
endif
ldflags-y += -lvpu -lpthread -latomic -ldl

ifeq ($(SAMPLE_STATIC),1)
LIB_DEP := $(MW_LIB)/libcipher.a $(MW_LIB)/libsys.a
else
LIB_DEP := $(MW_LIB)/libcipher.so $(MW_LIB)/libsys.so
endif

.PHONY : clean all
all : $(TARGET)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET): $(OBJS) $(LIB_DEP)
	@$(CXX) -o $@ $(OBJS) $(ELFFLAGS) $(ldflags-y)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CC))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
