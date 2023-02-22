SHELL = /bin/bash
STATIC ?= 0

ifeq ($(PARAM_FILE), )
ifeq ($(MW_VER), )
PARAM_FILE:=../../../../Makefile.param
include $(PARAM_FILE)
else
PARAM_FILE:=../../../../../$(shell echo $(MW_VER))/Makefile.param
include $(PARAM_FILE)
endif
endif
include ../../../sample.mk



EXENAME = audup_master
EXENAME2 = audup_slave
AAC_OBJ = ../../aac_sample/cvi_audio_aac_adp.o
C_OBJS = cvi_rpc_upmaster.o
C_OBJS2 = cvi_rpc_upslave.o

DEFINES =
DEFINES += -DCVIAUDIO_STATIC
DEFINES += -DSUPPORT_EXTERNAL_AAC

INC = -I ./ -I$(MW_INC) -I$(KERNEL_INC) -I ../../aac_sample
LIBS += -L$(MW_LIB) -lcvi_audio -lm -lpthread  -ldl -lcvi_VoiceEngine -lcvi_vqe -ltinyalsa -lcvi_RES1 -lcli -lcvi_ssp -lsys
LIBS += -L$(MW_LIB)/3rd -lnanomsg
LIBS += -laacdec2
LIBS += -laacsbrdec2
LIBS += -laacenc2
LIBS += -laaccomm2
LIBS += -laacsbrenc2

CFLAGS = -g -Wall -O0 -fPIC -fgnu89-inline -march=rv64imafdcvxthead -mabi=lp64d $(DEFINES) $(INC)
LDFLAGS = -lpthread -pthread  -lm

all: $(EXENAME) $(EXENAME2)

$(EXENAME): $(C_OBJS) $(AAC_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXENAME2): $(C_OBJS2) $(AAC_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
clean:
	$(RM) $(EXENAME) $(EXENAME2) *.o *.d


.PHONY:clean all

-include $(DEPS)





