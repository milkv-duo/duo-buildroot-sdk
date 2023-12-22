SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

PANEL_INC =../../component/panel/$(shell echo $(CHIP_ARCH) | tr A-Z a-z)
SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -I$(MW_INC) -I$(ISP_INC) -I$(KERNEL_INC) -I$(PANEL_INC) -I../common
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

TARGET = sample_panel
ifeq ($(CONFIG_ENABLE_SDK_ASAN), y)
TARGET = sample_panel_asan
endif

LIBS = -lvpu -lsys -lmipi_tx -lmisc

ifeq ($(MULTI_PROCESS_SUPPORT), 1)
DEFS += -DRPC_MULTI_PROCESS
LIBS += -lnanomsg
LIBS += -lvenc -lvdec -lmisc
endif

EXTRA_CFLAGS = $(INCS)
EXTRA_LDFLAGS = $(LIBS) -lm -lpthread -latomic
EXTRA_LDFLAGS += -lcvi_bin -lcvi_bin_isp $(ISP_LIB)

.PHONY : clean all
all: $(TARGET)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET): $(OBJS) $(MW_LIB)/libvpu.a $(MW_LIB)/libsys.a $(MW_LIB)/libmipi_tx.a
	@$(CXX) -o $@ $(OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CC))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
