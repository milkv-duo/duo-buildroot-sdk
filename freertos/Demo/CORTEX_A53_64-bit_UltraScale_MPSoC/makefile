#/*
# * FreeRTOS Kernel V10.3.0
# * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * http://www.FreeRTOS.org
# * http://aws.amazon.com/freertos
# *
# * 1 tab == 4 spaces!
# */

#CC = aarch64-none-elf-gcc
CC = aarch64-elf-gcc
#OBJCOPY = aarch64-none-elf-objcopy 
OBJCOPY = aarch64-elf-objcopy 
#OBJDUMP = aarch64-none-elf-objdump
OBJDUMP = aarch64-elf-objdump
#ARCH = aarch-none-elf-ar 
ARCH = aarch-elf-ar 
CRT0 = boot.s 
DEBUG = -g 
OPTIM = -O2
LDSCRIPT =./RTOSDemo_A53/src/lscript.ld

#
# CFLAGS common to both the THUMB and ARM mode builds
#

CFLAGS= \
-I. \
-I../Common/include \
-I../../Source/include \
-I../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include \
-I../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include/portable \
-I../../Source/FreeRTOS-Plus-POSIX/include \
-I../../Source/FreeRTOS-Plus-POSIX/include/FreeRTOS_POSIX \
-I../../Source/FreeRTOS-Plus-POSIX/include/private \
-I../../Source/portable/GCC/ARM_CA53_64_BIT \
-I./RTOSDemo_A53/src \
-I./RTOSDemo_A53/src/Full_Demo \
-I./RTOSDemo_A53_bsp/psu_cortexa53_0/include \
-Wall \
-Wextra \
-Wstrict-prototypes \
-Wmissing-prototypes \
-Wmissing-declarations \
-Wno-strict-aliasing \
-DFREERTOS_BSP \
-DGUEST \
-T$(LDSCRIPT) \
$(DEBUG) \
$(OPTIM) \
-ffunction-sections \
-fdata-sections \
-march=armv8-a

#LINKER_FLAGS=-Xlinker -ortosdemo.elf -Xlinker -M -Xlinker -Map=rtosdemo.map
LINKER_FLAGS=-Wl,--start-group,-lxil,-lgcc,-lc,--end-group -L ./RTOSDemo_A53_bsp/psu_cortexa53_0/lib -o rtosdemo.elf -Wl,-Map=rtosdemo.map -Wl,--gc-sections

#
# Source files that can be built to THUMB mode.
#
FREERTOS_SRC= \
../../Source/tasks.c \
../../Source/queue.c \
../../Source/list.c \
../../Source/timers.c \
../../Source/event_groups.c \
../../Source/croutine.c \
../../Source/stream_buffer.c \
../../Source/portable/GCC/ARM_CA53_64_BIT/port.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c \
../../Source/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c


DEMO_APP_SRC= \
../../Source/portable/MemMang/heap_4.c \
./RTOSDemo_A53/src/main.c \
./RTOSDemo_A53/src/Full_Demo/IntQueueTimer.c \
./RTOSDemo_A53/src/Full_Demo/main_full.c \
./RTOSDemo_A53/src/Blinky_Demo/main_blinky.c \
./RTOSDemo_A53/src/POSIX_Demo/posix_demo.c \
./RTOSDemo_A53/src/POSIX_Demo/main_posix.c \
./RTOSDemo_A53/src/FreeRTOS_tick_config.c \
./RTOSDemo_A53/src/platform.c \
./RTOSDemo_A53/src/system.c \
./RTOSDemo_A53/src/uart_dw.c \
../Common/Minimal/AbortDelay.c \
../Common/Minimal/EventGroupsDemo.c \
../Common/Minimal/GenQTest.c \
../Common/Minimal/IntQueue.c \
../Common/Minimal/IntSemTest.c \
../Common/Minimal/QueueOverwrite.c \
../Common/Minimal/StaticAllocation.c \
../Common/Minimal/TaskNotify.c \
../Common/Minimal/TimerDemo.c \
../Common/Minimal/blocktim.c \
../Common/Minimal/countsem.c \
../Common/Minimal/dynamic.c \
../Common/Minimal/flop.c \
../Common/Minimal/recmutex.c \
../Common/Minimal/semtest.c 


ARM_SRC= \
./RTOSDemo_A53/src/Full_Demo/reg_test.S \
../../Source/portable/GCC/ARM_CA53_64_BIT/portASM.S

#
# Define all object files.
#
ARM_OBJ = $(ARM_SRC:.S=.o)
FREERTOS_OBJ = $(FREERTOS_SRC:.c=.o)
DEMO_APP_OBJ = $(DEMO_APP_SRC:.c=.o)

rtosdemo.bin : rtosdemo.elf
	$(OBJDUMP) rtosdemo.elf -dS > rtosdemo.dis
	$(OBJCOPY) rtosdemo.elf -O binary rtosdemo.bin

rtosdemo.hex : rtosdemo.elf
	$(OBJCOPY) rtosdemo.elf -O ihex rtosdemo.hex

rtosdemo.elf : $(ARM_OBJ) $(DEMO_APP_OBJ) $(FREERTOS_OBJ) Makefile FreeRTOSConfig.h
	$(CC) $(CFLAGS) $(ARM_OBJ) $(DEMO_APP_OBJ) $(FREERTOS_OBJ)  --specs=nosys.specs $(LINKER_FLAGS)

$(DEMO_APP_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile FreeRTOSConfig.h
	$(CC) -c $(FLAGS) $(CFLAGS) $< -o $@

$(ZYNQ_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile FreeRTOSConfig.h
	$(CC) -c $(FLAGS) $(CFLAGS) $< -o $@

$(FREERTOS_OBJ)  : %.o : %.c $(LDSCRIPT) Makefile FreeRTOSConfig.h
	$(CC) -c $(FLAGS) $(CFLAGS) $< -o $@

$(ARM_OBJ) : %.o : %.S $(LDSCRIPT) Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	touch Makefile
	find ../Common/ -name *.o | xargs rm
	find ../../Source/ -name *.o | xargs rm
	find ./RTOSDemo_A53/ -name *.o | xargs rm
	rm ./rtosdemo.elf ./rtosdemo.map ./rtosdemo.bin ./rtosdemo.dis
