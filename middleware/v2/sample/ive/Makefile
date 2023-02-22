ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

SDIR = $(PWD)/src
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -Iinclude -I$(MW_INC) -I$(KERNEL_INC)
TARGET = $(SRCS:.c=)

PKG_CONFIG_PATH = $(MW_PATH)/pkgconfig
MW_LIBS = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --define-variable=mw_dir=$(MW_PATH) cvi_ive)

ifeq ($(SAMPLE_STATIC), 1)
LIBS += $(MW_PATH)/lib/libcvi_ive.a $(MW_PATH)/lib/libsys.a
endif

ifeq ($(SUBTYPE), asic)
LIBS += $(MW_LIBS)
endif

EXTRA_CFLAGS = $(INCS) $(DEFS)
LD_FLAGS = $(LIBS) -lm -lpthread

.PHONY : clean all
all: $(TARGET)

$(SDIR)/%: $(SDIR)/%.c
	@$(CXX) -o $(subst .o,,$(notdir $@)) $^ $(CXXFLAGS) $(EXTRA_CFLAGS) $(ELFFLAGS) $(LD_FLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(notdir $(TARGET))
