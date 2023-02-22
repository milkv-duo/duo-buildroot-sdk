SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/src/*.c) $(wildcard $(SDIR)/../vdec/src/*.c)
VENC_SRCS = $(SDIR)/sample_venc.c
VCODEC_SRCS = $(SDIR)/sample_vcodec.c
VENC_INC=../../modules/venc
SAMPLE_VENC_INC=$(SDIR)/include
SAMPLE_VDEC_INC=$(SDIR)/../vdec/include
INCS = -I$(MW_INC) -I$(ISP_INC) -I$(COMM_INC) -I$(VENC_INC) -I$(KERNEL_INC) -I$(SAMPLE_VENC_INC) -I$(SAMPLE_VDEC_INC) -I../common -I$(SYS_INC)

OBJS = $(SRCS:.c=.o)
VENC_OBJS = $(VENC_SRCS:.c=.o)
VCODEC_OBJS = $(VCODEC_SRCS:.c=.o)

DEPS = $(SRCS:.c=.d)
VENC_DEPS = $(VENC_SRCS:.c=.d)
VCODEC_DEPS = $(VCODEC_SRCS:.c=.d)

TARGET_VENC = sample_venc
TARGET_VCODEC = sample_vcodec
ifeq ($(SDK_VER),32bit)
TARGET_VENC_WITH_ASAN = sample_venc_asan
TARGET_VCODEC_WITH_ASAN = sample_vcodec_asan
else ifeq ($(SDK_VER), 64bit)
TARGET_VENC_WITH_ASAN = sample_venc_asan
TARGET_VCODEC_WITH_ASAN = sample_vcodec_asan
else
TARGET_VENC_WITH_ASAN =
TARGET_VCODEC_WITH_ASAN =
endif

PKG_CONFIG_PATH = $(MW_PATH)/pkgconfig
REQUIRES = cvi_common cvi_sample cvi_vdec
MW_LIBS = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --define-variable=mw_dir=$(MW_PATH) $(REQUIRES))

LIBS = $(MW_LIBS)
ifeq ($(MULTI_PROCESS_SUPPORT), 1)
DEFS += -DRPC_MULTI_PROCESS
LIBS += -lnanomsg
endif

ifeq ($(CLI_DEBUG_SUPPORT), 1)
DEFS += -DCLI_DEBUG_SUPPORT
LIBS += -lcli
endif

EXTRA_CFLAGS = $(INCS) $(DEFS)
EXTRA_LDFLAGS = $(LIBS) -lini -lpthread

EXTRA_LDFLAGS_WITH_ASAN = -Wl,-Bdynamic $(LIBS) -lini -lpthread

ifeq ($(DEBUG), 1)
EXTRA_LDFLAGS += -g -O0
EXTRA_LDFLAGS_WITH_ASAN += -g -O0
endif

EXTRA_LDFLAGS_WITH_ASAN += -fno-omit-frame-pointer -fsanitize-recover=address -lasan

.PHONY : clean all

all : $(TARGET_VENC) $(TARGET_VCODEC) $(TARGET_VENC_WITH_ASAN) $(TARGET_VCODEC_WITH_ASAN)

$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET_VENC) : $(VENC_OBJS) $(OBJS) $(MW_LIB)/libvenc.a $(MW_LIB)/libvpu.a $(MW_LIB)/libsys.a
	@$(CXX) -o $@ $(OBJS) $(VENC_OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CC))] $(notdir $@)

$(TARGET_VCODEC) : $(VCODEC_OBJS) $(OBJS) $(MW_LIB)/libvenc.a $(MW_LIB)/libvdec.a $(MW_LIB)/libvpu.a $(MW_LIB)/libsys.a
	@$(CXX) -o $@ $(OBJS) $(VCODEC_OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

$(TARGET_VENC_WITH_ASAN) : $(VENC_OBJS) $(OBJS)
	@$(CXX) -o $@ $(OBJS) $(VENC_OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS_WITH_ASAN)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CC))] $(notdir $@)

$(TARGET_VCODEC_WITH_ASAN) : $(VCODEC_OBJS) $(OBJS)
	@$(CXX) -o $@ $(OBJS) $(VCODEC_OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS_WITH_ASAN)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(VENC_OBJS) $(VCODEC_OBJS) $(DEPS) $(VENC_DEPS) $(VCODEC_DEPS) $(COMM_DEPS) $(COMM_OBJ) $(TARGET_VENC) $(TARGET_VCODEC) $(TARGET_VENC_WITH_ASAN) $(TARGET_VCODEC_WITH_ASAN)

-include $(VENC_DEPS) $(VCODEC_DEPS) $(DEPS) $(COMM_DEPS)
