SHELL = /bin/bash
STATIC = 0
ifeq ($(PARAM_FILE), )
ifeq ($(MW_VER), )
PARAM_FILE:=../../Makefile.param
include $(PARAM_FILE)
else
PARAM_FILE:=../../../$(shell echo $(MW_VER))/Makefile.param
include $(PARAM_FILE)
endif
endif

include ../sample.mk

SDIR = $(PWD)
3PARTY_INCLUDE = include/include
SRCS = $(wildcard $(SDIR)/*.c)
INCS = -I$(MW_INC) -I$(ISP_INC) -I$(COMM_INC) -I$(3PARTY_INCLUDE) -Iaac_sample
INCS += -I$(MW_INC)/../3rdparty/libmad/include
INCS += -I$(MW_INC)/../3rdparty/lame/include
ifeq ($(CVIARCH), $(filter $(CVIARCH), CV181X CV180X))
INCS += -I$(PWD)/include -I$(KERNEL_INC)
ifneq ($(CONFIG_ENABLE_SDK_ASAN), y)
USE_CVIAUDIO_STATIC = no
endif
else
INCS += -I$(PWD)/include
USE_CVIAUDIO_STATIC = no
endif

ifeq ($(USE_CVIAUDIO_STATIC), yes)
SRCS = $(filter-out $(SDIR)/cvi_audio_dl_adp.c, $(wildcard $(SDIR)/*.c))
endif
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

USE_ION_MEM = no
USE_ALSA = no
USE_TINYALSA = yes
USE_NE10 = no
USE_SYSTRACE = no
USE_ALGO = yes
USE_CVI_AEC = yes
SUPPORT_EXTERNAL_AAC = yes
USE_SYS_GLOBAL_LOG = no
MULTI_PROCESS_SUPPORT_AUDIO = no
USE_NEXT_SSP_DUAL = yes
USE_SONIC_LIB = no

DEFINES = -DCHIP_CV1835
DEFINES += -DCVI_MODIFIED
DEFINES += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
DEFINES += -DUSE_ALSA
DEFINES += -DUSE_ALGO_SOURCE
ifeq ($(USE_NE10), yes)
DEFINES += -DUSE_NE10_FFT
endif
ifeq ($(USE_ION_MEM), yes)
DEFINES += -DION_MEM
endif
ifeq ($(USE_SYS_GLOBAL_LOG), yes)
DEFINES += -DAUDIO_PRINT_WITH_GLOBAL_COMM_LOG
endif
ifeq ($(USE_CVI_AEC), no)
DEFINES += -DEXTERNAL_AEC
else
#DEFINES += -DUSE_CVI_AEC
endif
ifeq ($(SUPPORT_EXTERNAL_AAC), yes)
DEFINES += -DSUPPORT_EXTERNAL_AAC
endif

ifeq ($(USE_NEXT_SSP_DUAL), yes)
DEFINES += -DNEXT_SSP_ALGO
endif

ifeq ($(USE_CVIAUDIO_STATIC), yes)
DEFINES += -DCVIAUDIO_STATIC
STATIC_FLAG = -static
else
STATIC_FALG =
endif

ifeq ($(USE_SONIC_LIB), yes)
DEFINES += -DUSE_SONIC_LIB
endif

ifeq ($(CVIARCH), $(filter $(CVIARCH), CV181X CV180X))
DEFINES += -DAUD_SUPPORT_KERNEL_MODE
endif


TEST_INCLUDES =
TEST_INCLUDES += $(INCS)
ifeq ($(USE_ALSA), yes)
INCS += -Iinclude/alsa
endif
ifeq ($(USE_TINYALSA), yes)
INCS += -Iinclude/tinyalsa
endif
ifeq ($(SUPPORT_EXTERNAL_AAC), yes)
TEST_INCLUDES += -I ./aac_sample
endif

INCS += -I ./include/

AEC_TEST_LDLIBS += -lcvi_vqe
ifeq ($(CVIARCH), $(filter $(CVIARCH), CV181X CV180X))
AEC_TEST_LDLIBS += -lsys
endif

MP3_PLAYER_OBJ = ./cvi_mp3_player/cvi_mp3_player.o
TEST_OBJ = cvi_sample_audio.o cvi_audio_parse_param.o
ifeq ($(USE_CVIAUDIO_STATIC), no)
TEST_OBJ += cvi_audio_dl_adp.o
endif
TEST_OBJ_KER_MODE = cvi_sample_audio_rtos.o

LDLIBS =
LDLIBS += -lcvi_audio
LDLIBS += -lcvi_vqe
LDLIBS += -lcvi_VoiceEngine
LDLIBS += -lcvi_RES1
LDLIBS += -lcvi_ssp

ifeq ($(USE_CVIAUDIO_STATIC), yes)
LDLIBS += -lcvi_ssp
endif

ifeq ($(USE_ALSA), yes)
LDLIBS += -lasound
endif
ifeq ($(USE_TINYALSA), yes)
LDLIBS += -ltinyalsa
endif
ifeq ($(USE_SYS_GLOBAL_LOG), yes)
LDLIBS += -lvpu -lsys
endif

ifeq ($(SUPPORT_EXTERNAL_AAC), yes)
TEST_OBJ += ./aac_sample/cvi_audio_aac_adp.o
LDLIBS += -laacdec2
LDLIBS += -laacsbrdec2
LDLIBS += -laacenc2
LDLIBS += -laaccomm2
LDLIBS += -laacsbrenc2
endif
LDLIBS += -pthread
ifeq ($(CVIARCH), $(filter $(CVIARCH), CV181X CV180X))
LDLIBS += -lsys
endif

LDLIBS_WO_AAC = -lsys -latomic  -lcvi_audio -lcvi_vqe -lcvi_VoiceEngine -lcvi_RES1 -ltinyalsa -lm -lpthread -ldl

EXTRA_CFLAGS = -fPIC $(OPT_LEVEL) -Wno-implicit-function-declaration -I. $(INCS) $(DEFINES)
EXTRA_LDFLAGS = $(LDLIBS) -lm -lpthread -ldl

MP3_EXTRA_LDFLAGS = -lcvi_mp3 -lmad -lcvi_audio -lm -lpthread  -ldl -lcvi_VoiceEngine -lcvi_vqe -ltinyalsa -lcvi_RES1

EXTRA_CFLAGS += -DCLI_DEBUG_SUPPORT
EXTRA_LDFLAGS += -lcli

ifeq ($(MULTI_PROCESS_SUPPORT_AUDIO), yes)
DEFINES += -DRPC_MULTI_PROCESS
DEFINES += -DRPC_MULTI_PROCESS_AUDIO
DEFINES += -DCVIAUDIO_STATIC
EXTRA_LDFLAGS += -lnanomsg
endif

ifeq ($(SAMPLE_STATIC),1)
#softlink so lib will not be enough
#need to link all the audio related .a file
AEC_TEST_LDLIBS = $(LDLIBS)
else ifeq ($(USE_CVIAUDIO_STATIC), yes)
AEC_TEST_LDLIBS = $(LDLIBS)
endif

AUDIO_ELFFLAGS := $(ELFFLAGS)
ELFFLAGS = $(filter-out -static, $(AUDIO_ELFFLAGS))

.PHONY : clean all


ifeq ($(MULTI_PROCESS_SUPPORT_AUDIO), yes)
ifeq ($(CONFIG_ENABLE_SDK_ASAN), y)
all : sample_audio check_and_leave sample_audio_asan
else
all : sample_audio check_and_leave sample_multi_process
endif
else
all : sample_audio check_and_leave
endif


%.o : %.c
	@$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

sample_audio: $(TEST_OBJ)
	@$(CXX) -o $@ $(TEST_OBJ) $(ELFFLAGS) $(STATIC_FLAG) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

sample_audio_asan: $(TEST_OBJ)
	@$(CXX) -o $@ $(TEST_OBJ) $(ELFFLAGS) $(EXTRA_LDFLAGS_WITH_ASAN) $(EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

cvi_mp3player: $(MP3_PLAYER_OBJ)
	@$(CXX) -o $@ $(MP3_PLAYER_OBJ) $(ELFFLAGS) $(EXTRA_LDFLAGS) $(MP3_EXTRA_LDFLAGS)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)
	@cp cvi_mp3player  sample_mp3player

sample_audio_rtos: $(TEST_OBJ_KER_MODE)
	$(CXX) -o $@ $(TEST_OBJ_KER_MODE) $(ELFFLAGS) -static $(LDLIBS_WO_AAC)
	@echo -e $(BLUE)[LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

check_and_leave:
	@echo "audio check and leave..remove unnecessary libs"
	@rm -rf $(MW_PATH)/lib/3rd/libmad*
	@rm -rf $(MW_PATH)/lib/3rd/libmp3lame*

sample_multi_process:
	cd ./multiprocess_case/down_example; make clean;make
	cd ./multiprocess_case/up_example; make clean;make

clean:
	@echo "sample/audio/clean in"  $(MW_PATH)
	@rm -f $(OBJS) $(DEPS) $(TEST_OBJ) $(MP3_PLAYER_OBJ)
	@rm -rf sample_audio*
	@rm -rf *.o
	@rm -rf cvi_mp3player
	@rm -rf sample_mp3*
	@rm -rf ./cvi_mp3_player/*.d
	@rm -rf cvi_mp3recorder
	@rm -rf ssp_NotchTest*
	@rm -rf $(MW_PATH)/lib/3rd/libmad*
	@rm -rf $(MW_PATH)/lib/3rd/libmp3lame*
	@rm -rf $(MW_PATH)/lib/3rd/libcvi_mp3*

-include $(DEPS)
