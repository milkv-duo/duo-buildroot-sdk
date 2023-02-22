SHELL = /bin/bash

STATIC ?= 0
ifeq ($(PARAM_FILE), )
ifeq ($(MW_VER), )
PARAM_FILE:=../../../Makefile.param
include $(PARAM_FILE)
else
PARAM_FILE:=../../../../$(shell echo $(MW_VER))/Makefile.param
include $(PARAM_FILE)
endif
endif
include ../../sample.mk

SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -I$(MW_INC) -I../common/ -I$(MW_INC)/../3rdparty/libmad/include -I ./
ifeq ($(CHIP_ARCH), $(filter $(CHIP_ARCH), CV181X CV180X))
INCS += -I$(KERNEL_INC)
endif
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

TARGET = cvi_mp3player

EXTRA_CFLAGS = $(INCS)
EXTRA_LDFLAGS +=  -lcvi_audio
EXTRA_LDFLAGS +=  -lm -lpthread  -ldl -lcvi_mp3 -lcvi_VoiceEngine -lmad
EXTRA_LDFLAGS +=  -lcvi_vqe -ltinyalsa -lcvi_RES1 -lcli

ifeq ($(SAMPLE_STATIC),1)
LIB_DEP := $(MW_LIB)/libmad.a
else
LIB_DEP := $(MW_LIB)/libmad.so
endif

.PHONY : clean all
all : $(TARGET)

$(SDIR)/%.o : $(SDIR)/%.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET) : $(OBJS) $(LIB_DEP)
	@$(CXX) -o $@ $(OBJS) $(ELFFLAGS) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CC))] $(notdir $@)

clean:
	@rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
