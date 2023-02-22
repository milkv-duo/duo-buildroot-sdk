SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/../venc/src/*.c) $(wildcard $(SDIR)/src/*.c)
VDEC_SRCS = $(SDIR)/sample_vdec.c
VDEC_INC=../../modules/vdec
SAMPLE_VENC_INC=$(SDIR)/../venc/include
SAMPLE_VDEC_INC=$(SDIR)/include

INCS = -I$(MW_INC) -I$(ISP_INC) -I$(COMM_INC) -I$(SAMPLE_VDEC_INC) -I$(SAMPLE_VENC_INC) -I$(VDEC_INC) -I$(KERNEL_INC) -I../common -I$(SYS_INC)

OBJS = $(SRCS:.c=.o)
VDEC_OBJS = $(VDEC_SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
VDEC_DEPS = $(VDEC_SRCS:.c=.d)

TARGET = sample_vdec

ifeq ($(SDK_VER),32bit)
TARGET_WITH_ASAN = sample_vdec_asan
else ifeq ($(SDK_VER),64bit)
TARGET_WITH_ASAN = sample_vdec_asan
else
TARGET_WITH_ASAN =
endif

PKG_CONFIG_PATH = $(MW_PATH)/pkgconfig
REQUIRES = cvi_common cvi_vdec cvi_sample
LIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --define-variable=mw_dir=$(MW_PATH) $(REQUIRES))

ifeq ($(MULTI_PROCESS_SUPPORT), 1)
DEFS += -DRPC_MULTI_PROCESS
LIBS += -lnanomsg
endif

ifeq ($(CLI_DEBUG_SUPPORT), 1)
DEFS += -DCLI_DEBUG_SUPPORT
LIBS += -lcli
endif

EXTRA_CFLAGS = $(DEFS) $(INCS)
EXTRA_LDFLAGS = $(LIBS) -lini -lm -lpthread


EXTRA_LDFLAGS_WITH_ASAN = -Wl,-Bdynamic $(LIBS) -lini -lm -lpthread
EXTRA_LDFLAGS_WITH_ASAN +=  -fno-omit-frame-pointer -fsanitize-recover=address -lasan

.PHONY : clean all
all: $(TARGET) $(TARGET_WITH_ASAN)

$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET): $(VDEC_OBJS) $(OBJS) $(MW_LIB)/libvdec.a $(MW_LIB)/libvpu.a $(MW_LIB)/libsys.a
	@$(CXX) -o $@ -Wl,--start-group $(OBJS) $(VDEC_OBJS) -Wl,--end-group $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

$(TARGET_WITH_ASAN): $(VDEC_OBJS) $(OBJS)
	@$(CXX) -o $@ -Wl,--start-group $(OBJS) $(VDEC_OBJS) -Wl,--end-group $(ELFFLAGS) $(EXTRA_LDFLAGS_WITH_ASAN)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

clean:
	@rm -f $(VDEC_OBJS) $(OBJS) $(VDEC_DEPS) $(DEPS) $(TARGET) $(TARGET_WITH_ASAN)

-include $(VDEC_DEPS) $(DEPS)
