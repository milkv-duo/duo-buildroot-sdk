SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

PANEL_INC =../../component/panel/$(shell echo $(CVIARCH) | tr A-Z a-z)
SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -I$(MW_INC) -I$(ISP_INC) -I$(KERNEL_INC) -I$(PANEL_INC) -Iinclude
OBJS = $(patsubst %.c, %.o, $(SRCS))
DEPS = $(SRCS:.c=.d)
TARGET_A = $(MW_LIB)/libsample.a
TARGET_SO = $(MW_LIB)/libsample.so

EXTRA_CFLAGS += $(INCS)

ifeq ($(DEBUG), 1)
	EXTRA_CFLAGS += -g -O0
endif

# add sensor -D definition
include $(BUILD_PATH)/.config
include Kbuild
EXTRA_CFLAGS += $(KBUILD_DEFINES)

.PHONY : clean all
all : $(TARGET_A) $(TARGET_SO)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET_A): $(OBJS)
	@$(AR) $(ARFLAGS) $(TARGET_A) $(OBJS)
	@echo -e $(YELLOW)[LINK]$(END)[$(notdir $(AR))] $(notdir $(TARGET_A))

$(TARGET_SO): $(OBJS)
	@$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $(TARGET_SO) --start-group $(OBJS) --end-group
	@echo -e $(GREEN)[LINK]$(END)[$(notdir $(LD))] $(notdir $(TARGET_SO))

clean:
	@rm -f $(OBJS) $(TARGET_A) $(TARGET_SO)

