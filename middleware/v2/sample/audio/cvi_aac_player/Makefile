#!bash/bin
EXENAME = cvi_aac_player
SDK_VER?=64bit
MW_PATCH = $(PWD)/../../..
C_SRCS = $(wildcard *.c)
OBJS = $(C_SRCS:.c = .o)
PWD = $(shell pwd)

ifeq ($(SDK_VER), 64bit)
CC = aarch64-linux-gnu-gcc
LIBS = -L$(PWD)/aac_lib/64bit -lfdkaac_dec
LIBS += -L$(MW_PATCH)/3rdparty/libaac/64bit -lcvi_aac_dec
endif

ifeq ($(SDK_VER), 32bit)
CC = arm-linux-gnueabihf-gcc
LIBS = -L$(PWD)/aac_lib/32bit -lfdkaac_dec
LIBS += -L$(MW_PATCH)/3rdparty/libaac/32bit -lcvi_aac_dec
endif

DEFINES =
INC = -I$(MW_PATCH)/include
INC += -I$(PWD)/ -I $(PWD)/../

LIBS += -L$(MW_PATCH)/lib -lcvi_audio -lm -lpthread  -ldl -lcvi_VoiceEngine -lcvi_vqe -ltinyalsa -lcvi_RES1

CFLAGS = -g -Wall -O0 -fPIC -fgnu89-inline  $(DEFINES) $(INC)
LDFLAGS = -lpthread -pthread  -lm


C_OBJS = $(C_SRCS:.c=.o)

all: $(EXENAME)

$(EXENAME): $(C_OBJS)
	$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS) $(LIBS) $(INC)

clean:
	$(RM) $(C_OBJ) server $(SHARED) *.o $(EXENAME)


.PHONY:clean all

-include $(DEPS)


