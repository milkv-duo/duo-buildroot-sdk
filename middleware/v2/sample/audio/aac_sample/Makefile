.PHONY: CREATE_DIR

#SDK_VER ?= uclibc
#SDK_VER ?= 64bit
#SDK_VER ?= musl_riscv64
## GCC COMPILER ##
##TOOLPATH=~/master_repo3/host-tools/gcc/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
#TOOLPATH=../../../../host-tools/gcc/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
#CROSS_COMPILE=$(TOOLPATH)/aarch64-linux-gnu-
#TOOLPATH=../../../../host-tools/gcc/arm-cvitek-linux-uclibcgnueabihf/bin
#CROSS_COMPILE=$(TOOLPATH)/arm-cvitek-linux-uclibcgnueabihf-

# KERNEL_INC := $(KERNEL_PATH)/build/$(PROJECT_FULLNAME)/riscv/usr/include
# TOOLPATH=../../../../host-tools/gcc/riscv64-linux-musl-x86_64/bin
# CROSS_COMPILE=$(TOOLPATH)/riscv64-unknown-linux-musl-
# CC = $(CROSS_COMPILE)gcc
# CXX = $(CROSS_COMPILE)g++
# AR = $(CROSS_COMPILE)ar
# LD = $(CROSS_COMPILE)ld
# STRIP = $(CROSS_COMPILE)strip
SHELL = /bin/bash
ifeq ($(PARAM_FILE), )
ifeq ($(MW_VER), )
PARAM_FILE:=../../../Makefile.param
include $(PARAM_FILE)
else
PARAM_FILE:=../../../../$(shell echo $(MW_VER))/Makefile.param
include $(PARAM_FILE)
endif
endif


FDK_AAC_DIR=./fdkaac
CUR = $(PWD)

#aac common  -------------------------------------------start
AACCOMM_SRC =
AACCOMM_SRC += $(wildcard $(PWD)/fdkaac/libSYS/src/*.cpp)
AACCOMM_SRC += $(wildcard $(PWD)/fdkaac/libPCMutils/src/*.cpp)
AACCOMM_SRC += $(wildcard $(PWD)/fdkaac/libFDK/src/*.cpp)

AACCOMM_INCS =
AACCOMM_INCS += -I ./fdkaac/libFDK/include/
AACCOMM_INCS += -I ./fdkaac/libSYS/include/
AACCOMM_INCS += -I ./fdkaac/libPCMutils/include/
AACCOMM_INCS += -I $(KERNEL_INC)
OBJS_COMM = $(AACCOMM_SRC:.cpp=.o)
#aac encoder -------------------------------------------end

#aac sbr encoder -------------------------------------------start
AACSBRENC_SRC =
AACSBRENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSBRenc/src/*.cpp)
AACSBRENC_SRC += $(AACSBRENC_SRC_EXTRA)

OBJS_SBRENC = $(AACSBRENC_SRC:.cpp=.o)
#aac sbr encoder -------------------------------------------end

#sbr decoder -------------------------------------------start
AACSBRDEC_SRC =
AACSBRDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSBRdec/src/*.cpp)
AACSBRDEC_SRC += $(AACSBRDEC_SRC_EXTRA)
OBJS_SBRDEC = $(AACSBRDEC_SRC:.cpp=.o)
#sbr decoder -------------------------------------------end

#aac encoder -------------------------------------------start
SYS_SRC = \
    libSYS/src/genericStds.cpp \
    libSYS/src/syslib_channelMapDescr.cpp

AACENC_SRC =  $(wildcard $(PWD)/fdkaac/libAACenc/src/*.cpp)
AACENC_SRC_EXTRA = $(wildcard $(PWD)/fdkaac/libSACenc/src/*.cpp)
#AACENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSBRenc/src/*.cpp)
AACENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libMpegTPEnc/src/*.cpp)
##AACENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSYS/src/*.cpp)
##AACENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libPCMutils/src/*.cpp)
##AACENC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libFDK/src/*.cpp)

AACENC_SRC += $(AACENC_SRC_EXTRA)
AACENC_SRC += ./aacenc_interface.cpp

AACENC_INCS =
AACENC_INCS += -I ./fdkaac/libFDK/include/
AACENC_INCS += -I ./fdkaac/libSYS/include/
AACENC_INCS += -I ./fdkaac/libMpegTPEnc/include/
AACENC_INCS += -I ./fdkaac/libSBRenc/include/
AACENC_INCS += -I ./fdkaac/libAACenc/include/
AACENC_INCS += -I ./fdkaac/libSACenc/include/
AACENC_INCS += -I ./fdkaac/libPCMutils/include/
AACENC_INCS += -I $(KERNEL_INC)
OBJS_ENC = $(AACENC_SRC:.cpp=.o)
#aac encoder -------------------------------------------end

#aac decoder -------------------------------------------start
AACDEC_SRC = $(wildcard $(PWD)/fdkaac/libAACdec/src/*.cpp)
##AACDEC_SRC_EXTRA = $(wildcard $(PWD)/fdkaac/libFDK/src/*.cpp)
##AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSYS/src/*.cpp)
AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libArithCoding/src/*.cpp)
AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libMpegTPDec/src/*.cpp)
#AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSBRdec/src/*.cpp)
##AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libPCMutils/src/*.cpp)
AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libDRCdec/src/*.cpp)
AACDEC_SRC_EXTRA += $(wildcard $(PWD)/fdkaac/libSACdec/src/*.cpp)

AACDEC_SRC += $(AACDEC_SRC_EXTRA)
AACDEC_SRC += ./aacdec_interface.cpp

AACDEC_INCS =
AACDEC_INCS += -I ./fdkaac/libFDK/include/
AACDEC_INCS += -I ./fdkaac/libSYS/include/
AACDEC_INCS += -I ./fdkaac/libAACdec/include/
AACDEC_INCS += -I ./fdkaac/libArithCoding/include/
AACDEC_INCS += -I ./fdkaac/libAACdec/src/
AACDEC_INCS += -I ./fdkaac/libMpegTPDec/include/
AACDEC_INCS += -I ./fdkaac/libSBRdec/include/
AACDEC_INCS += -I ./fdkaac/libPCMutils/include/
AACDEC_INCS += -I ./fdkaac/libDRCdec/include/
AACDEC_INCS += -I ./fdkaac/libSACdec/include/
AACDEC_INCS += -I $(KERNEL_INC)

OBJS_DEC = $(AACDEC_SRC:.cpp=.o)
#aac decoder -------------------------------------------end

#AAC  test sample code --------------------------start
AAC_TEST =  $(PWD)/sample_audio_aac
AAC_TEST_SRC += $(PWD)/cvi_audio_aac_adp.c
AAC_TEST_SRC += $(PWD)/../cvi_audio_dl_adp.c

MW_LIB ?= $(PWD)/../../../lib
AAC_LDFLAGS =
AAC_LDFLAGS += -L$(MW_LIB) -L$(MW_LIB)/3rd
ifeq ($(SDK_VER), 64bit)
AAC_LDFLAGS += -L $(PWD)/64bit
endif
ifeq ($(SDK_VER), 32bit)
AAC_LDFLAGS += -L $(PWD)/32bit
endif

ifeq ($(SDK_VER), uclibc)
AAC_LDFLAGS += -L $(PWD)/uclibc
endif

ifeq ($(SDK_VER), glibc_riscv64)
AAC_LDFLAGS += -L $(PWD)/riscv64
endif

ifeq ($(SDK_VER), musl_riscv64)
AAC_LDFLAGS += -L $(PWD)/musl_riscv64
endif

AAC_LDLIBS =
AAC_LDLIBS += -lpthread
AAC_LDLIBS += -lm -ldl
AAC_LDLIBS += -lstdc++
AAC_LDLIBS += -laacdec2
AAC_LDLIBS += -laacenc2
AAC_LDLIBS += -laaccomm2
AAC_LDLIBS += -laacsbrdec2
AAC_LDLIBS += -laacsbrenc2


#AAC_INCLUDES = -I$(MW_INC)
AAC_INCLUDES =
AAC_INCLUDES += -I ../../../include
AAC_INCLUDES += -I$(PWD)
AAC_INCLUDES += -I$(KERNEL_INC)
DEFINES += -DCVI_MODIFIED

AAC_CFLAGS += -I . -fPIC -Wno-unused-result  -Wno-format -Wl,--fatal-warning $(AAC_INCLUDES) $(DEFINES)

ifeq ($(SDK_VER), 64bit)
AACENC_TARGET = $(CUR)/64bit/libaacenc2.a
AACENC_TARGET_S = $(CUR)/64bit/libaacenc2.so

AACDEC_TARGET = $(CUR)/64bit/libaacdec2.a
AACDEC_TARGET_S = $(CUR)/64bit/libaacdec2.so

AACCOMM_TARGET = $(CUR)/64bit/libaaccomm2.a
AACCOMM_TARGET_S = $(CUR)/64bit/libaaccomm2.so

AACSBRENC_TARGET = $(CUR)/64bit/libaacsbrenc2.a
AACSBRENC_TARGET_S = $(CUR)/64bit/libaacsbrenc2.so

AACSBRDEC_TARGET = $(CUR)/64bit/libaacsbrdec2.a
AACSBRDEC_TARGET_S = $(CUR)/64bit/libaacsbrdec2.so

TARGET_FLD = $(MW_LIB)/../modules/audio/lib
PREBUILD_FLD = ./64bit
endif

ifeq ($(SDK_VER), 32bit)
AACENC_TARGET = $(CUR)/32bit/libaacenc2.a
AACENC_TARGET_S = $(CUR)/32bit/libaacenc2.so

AACDEC_TARGET = $(CUR)/32bit/libaacdec2.a
AACDEC_TARGET_S = $(CUR)/32bit/libaacdec2.so

AACCOMM_TARGET = $(CUR)/32bit/libaaccomm2.a
AACCOMM_TARGET_S = $(CUR)/32bit/libaaccomm2.so

AACSBRENC_TARGET = $(CUR)/32bit/libaacsbrenc2.a
AACSBRENC_TARGET_S = $(CUR)/32bit/libaacsbrenc2.so

AACSBRDEC_TARGET = $(CUR)/32bit/libaacsbrdec2.a
AACSBRDEC_TARGET_S = $(CUR)/32bit/libaacsbrdec2.so

TARGET_FLD = $(MW_LIB)/../modules/audio/lib_32
PREBUILD_FLD = ./32bit
endif

ifeq ($(SDK_VER), uclibc)
AACENC_TARGET = $(CUR)/uclibc/libaacenc2.a
AACENC_TARGET_S = $(CUR)/uclibc/libaacenc2.so

AACDEC_TARGET = $(CUR)/uclibc/libaacdec2.a
AACDEC_TARGET_S = $(CUR)/uclibc/libaacdec2.so

AACCOMM_TARGET = $(CUR)/uclibc/libaaccomm2.a
AACCOMM_TARGET_S = $(CUR)/uclibc/libaaccomm2.so

AACSBRENC_TARGET = $(CUR)/uclibc/libaacsbrenc2.a
AACSBRENC_TARGET_S = $(CUR)/uclibc/libaacsbrenc2.so

AACSBRDEC_TARGET = $(CUR)/uclibc/libaacsbrdec2.a
AACSBRDEC_TARGET_S = $(CUR)/uclibc/libaacsbrdec2.so

TARGET_FLD = $(MW_LIB)/../modules/audio/uclibc
PREBUILD_FLD = ./uclibc
endif

ifeq ($(SDK_VER), glibc_riscv64)
AACENC_TARGET = $(CUR)/riscv64/libaacenc2.a
AACENC_TARGET_S = $(CUR)/riscv64/libaacenc2.so

AACDEC_TARGET = $(CUR)/riscv64/libaacdec2.a
AACDEC_TARGET_S = $(CUR)/riscv64/libaacdec2.so

AACCOMM_TARGET = $(CUR)/riscv64/libaaccomm2.a
AACCOMM_TARGET_S = $(CUR)/riscv64/libaaccomm2.so

AACSBRENC_TARGET = $(CUR)/riscv64/libaacsbrenc2.a
AACSBRENC_TARGET_S = $(CUR)/riscv64/libaacsbrenc2.so

AACSBRDEC_TARGET = $(CUR)/riscv64/libaacsbrdec2.a
AACSBRDEC_TARGET_S = $(CUR)/riscv64/libaacsbrdec2.so

TARGET_FLD = $(MW_LIB)/../modules/audio/riscv64
PREBUILD_FLD = ./riscv64
endif

ifeq ($(SDK_VER), musl_riscv64)
AACENC_TARGET = $(CUR)/$(SDK_VER)/libaacenc2.a
AACENC_TARGET_S = $(CUR)/$(SDK_VER)/libaacenc2.so

AACDEC_TARGET = $(CUR)/$(SDK_VER)/libaacdec2.a
AACDEC_TARGET_S = $(CUR)/$(SDK_VER)/libaacdec2.so

AACCOMM_TARGET = $(CUR)/$(SDK_VER)/libaaccomm2.a
AACCOMM_TARGET_S = $(CUR)/$(SDK_VER)/libaaccomm2.so

AACSBRENC_TARGET = $(CUR)/$(SDK_VER)/libaacsbrenc2.a
AACSBRENC_TARGET_S = $(CUR)/$(SDK_VER)/libaacsbrenc2.so

AACSBRDEC_TARGET = $(CUR)/$(SDK_VER)/libaacsbrdec2.a
AACSBRDEC_TARGET_S = $(CUR)/$(SDK_VER)/libaacsbrdec2.so

TARGET_FLD = $(MW_LIB)/../modules/audio/$(SDK_VER)
PREBUILD_FLD = ./$(SDK_VER)
endif


ARFLAGS = rcs
ARFLAGS_S = -shared -fPIC -o
CFLAGS = -fPIC -c -Os  -ffunction-sections -fdata-sections
CFLAGS += -DCVI_MODIFIED

AAC_TEST_LIB = $(AAC_LDLIBS)



BUILDLIST=
BUILDLIST += $(AACENC_TARGET)
BUILDLIST += $(AACDEC_TARGET)
BUILDLIST += $(AACCOMM_TARGET)
BUILDLIST += $(AACSBRENC_TARGET)
BUILDLIST += $(AACSBRDEC_TARGET)
BUILDLIST += $(AAC_TEST)

OBJECTPATHS_ENCSRC=$(addprefix $(PWD)/,$(notdir $(OBJS_ENC)))
OBJECTPATHS_DECSRC=$(addprefix $(PWD)/,$(notdir $(OBJS_DEC)))
OBJECTPATHS_COMMSRC=$(addprefix $(PWD)/,$(notdir $(OBJS_COMM)))
OBJECTPATHS_SBRENCSRC=$(addprefix $(PWD)/,$(notdir $(OBJS_SBRENC)))
OBJECTPATHS_SBRDECSRC=$(addprefix $(PWD)/,$(notdir $(OBJS_SBRDEC)))
all:$(BUILDLIST)


$(OBJS_ENC):
	$(CC) $(AACENC_SRC) $(CFLAGS) $(AACENC_INCS)


$(OBJS_DEC):
	$(CC) $(AACDEC_SRC) $(CFLAGS) $(AACDEC_INCS)


$(OBJS_COMM):
	$(CC) $(AACCOMM_SRC) $(CFLAGS) $(AACCOMM_INCS)

$(OBJS_SBRENC):
	$(CC) $(AACSBRENC_SRC) $(CFLAGS) $(AACENC_INCS)

$(OBJS_SBRDEC):
	$(CC) $(AACSBRDEC_SRC) $(CFLAGS) $(AACDEC_INCS)


$(AACENC_TARGET) : $(OBJS_ENC)
	$(AR) $(ARFLAGS) $(AACENC_TARGET) $(OBJECTPATHS_ENCSRC)
	$(CC) $(ARFLAGS_S)  $(AACENC_TARGET_S) $(OBJECTPATHS_ENCSRC)
	$(STRIP) $(AACENC_TARGET_S)
	rm -rf  $(OBJECTPATHS_ENCSRC)

$(AACDEC_TARGET) : $(OBJS_DEC)
	$(AR) $(ARFLAGS) $(AACDEC_TARGET) $(OBJECTPATHS_DECSRC)
	$(CC) $(ARFLAGS_S)  $(AACDEC_TARGET_S) $(OBJECTPATHS_DECSRC)
	$(STRIP) $(AACDEC_TARGET_S)
	rm -rf  $(OBJECTPATHS_DECSRC)

$(AACCOMM_TARGET) : $(OBJS_COMM)
	$(AR) $(ARFLAGS) $(AACCOMM_TARGET) $(OBJECTPATHS_COMMSRC)
	$(CC) $(ARFLAGS_S)  $(AACCOMM_TARGET_S) $(OBJECTPATHS_COMMSRC)
	$(STRIP) $(AACCOMM_TARGET_S)
	rm -rf  $(OBJECTPATHS_COMMSRC)

$(AACSBRENC_TARGET) : $(OBJS_SBRENC)
	$(AR) $(ARFLAGS) $(AACSBRENC_TARGET) $(OBJECTPATHS_SBRENCSRC)
	$(CC) $(ARFLAGS_S)  $(AACSBRENC_TARGET_S) $(OBJECTPATHS_SBRENCSRC)
	$(STRIP) $(AACSBRENC_TARGET_S)
	rm -rf  $(OBJECTPATHS_SBRENCSRC)

$(AACSBRDEC_TARGET) : $(OBJS_SBRDEC)
	$(AR) $(ARFLAGS) $(AACSBRDEC_TARGET) $(OBJECTPATHS_SBRDECSRC)
	$(CC) $(ARFLAGS_S)  $(AACSBRDEC_TARGET_S) $(OBJECTPATHS_SBRDECSRC)
	$(STRIP) $(AACSBRDEC_TARGET_S)
	rm -rf  $(OBJECTPATHS_SBRDECSRC)

$(AAC_TEST): $(AACENC_TARGET) $(AACDEC_TARGET) $(AACCOMM_TARGET) $(AACSBRENC_TARGET) $(AACSBRDEC_TARGET)
	$(CC) -o   $@ $(AAC_LDFLAGS) $(AAC_TEST_SRC) $(AAC_TEST_LIB) $(AAC_CFLAGS)

.PHONY : clean
clean:
	rm -f    $(OBJECTPATHS_ENCSRC) $(OBJECTPATHS_DECSRC) $(OBJECTPATHS_COMMSRC) $(OBJECTPATHS_ENCSRC) $(OBJECTPATHS_SBRDECSRC) $(AAC_TEST)

