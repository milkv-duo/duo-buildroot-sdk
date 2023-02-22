ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
include ../sample.mk

SDIR = $(PWD)/src
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -Iinclude -I$(MW_INC) -I$(KERNEL_INC)
TARGET = $(SRCS:.c=)

MW_LIBS = -lcvi_gyro

ifeq ($(SAMPLE_STATIC), 1)
LIBS += $(MW_PATH)/lib/libcvi_gyro.a
endif

ifeq ($(SUBTYPE), asic)
LIBS += $(MW_LIBS)
endif

EXTRA_CFLAGS = $(INCS) $(DEFS)
LD_FLAGS = $(LIBS) -lm -lpthread

ifeq ("$(wildcard $(MW_INC)/cvi_gyro.h)","")
TARGET :=
endif

.PHONY : clean all
all: $(TARGET)

$(SDIR)/%: $(SDIR)/%.c
	@$(CXX) -o $(subst .o,,$(notdir $@)) $^ $(CXXFLAGS) $(EXTRA_CFLAGS) $(ELFFLAGS) $(LD_FLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(notdir $(TARGET))
