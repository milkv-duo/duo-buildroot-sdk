cmake_minimum_required(VERSION 3.16.5)

project(arch C ASM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(KERNEL_SOURCE ${TOP_DIR}/../Source)
set(TRACE_SOURCE ${TOP_DIR}/../Tracealyzer)
set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)
set(CMAKE_INSTALL_INC_PREFIX ${TOP_DIR}/install/include)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing -static --specs=nosys.specs -D__BUILD_RTOS_KERNEL__")
#set(SAFETY_FLAGS "-Werror -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

if (RUN_ARCH STREQUAL "riscv64")
file(GLOB _SOURCES
    "${KERNEL_SOURCE}/*.c"
    "${KERNEL_SOURCE}/portable/GCC/RISC-V/*.c"
    "${KERNEL_SOURCE}/portable/GCC/RISC-V/*.S"
    "${KERNEL_SOURCE}/portable/MemMang/heap_4.c"
    "${TRACE_SOURCE}/src/*.c"
)

include_directories(include/${RUN_ARCH})
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)

include_directories(${KERNEL_SOURCE}/portable/GCC/RISC-V)
include_directories(${KERNEL_SOURCE}/portable/GCC/RISC-V/chip_specific_extensions/thead_c906_rv64imafdc/)
include_directories(${KERNEL_SOURCE}/include)
include_directories(${TRACE_SOURCE}/include)
include_directories(${TRACE_SOURCE}/config)

file(GLOB _HEADERS "include/${RUN_ARCH}/*.h"
                   "${KERNEL_SOURCE}/include/*.h"
                   "${KERNEL_SOURCE}/portable/GCC/RISC-V/*.h"
                   "${KERNEL_SOURCE}/portable/GCC/RISC-V/chip_specific_extensions/thead_c906_rv64imafdc/*.h"
                   "${TRACE_SOURCE}/include/*.h"
                   "${TRACE_SOURCE}/config/*.h"
)

add_library(kernel STATIC ${_SOURCES})

install(TARGETS kernel DESTINATION lib)
install(FILES ${_HEADERS} DESTINATION include/kernel)

else()
file(GLOB _SOURCES
    "src/*.c"
    "${KERNEL_SOURCE}/*.c"
    "${KERNEL_SOURCE}/portable/GCC/ARM_CA53_64_BIT/*.c"
    "${KERNEL_SOURCE}/portable/GCC/ARM_CA53_64_BIT/*.S"
    "${KERNEL_SOURCE}/portable/MemMang/heap_4.c"
    "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/source/*.c"
)

include_directories(include/${CHIP})
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)

include_directories(${KERNEL_SOURCE}/portable/GCC/ARM_CA53_64_BIT)
include_directories(${KERNEL_SOURCE}/include)

include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/private)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include/portable)

file(GLOB _HEADERS "include/${CHIP}/*.h"
                    "${KERNEL_SOURCE}/include/*.h"
                    "${KERNEL_SOURCE}/portable/GCC/ARM_CA53_64_BIT/*.h"
                    "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/private/*.h"
                    "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include/*.h"
                    "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include/portable/*.h"
)

file(GLOB _POSIX_HEADERS "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/FreeRTOS_POSIX/*.h")
file(GLOB _POSIX_SYS_HEADERS "${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/FreeRTOS_POSIX/sys/*.h")

add_library(kernel STATIC ${_SOURCES})

install(TARGETS kernel DESTINATION lib)
install(FILES ${_HEADERS} DESTINATION include/kernel)
install(FILES ${_POSIX_HEADERS} DESTINATION include/kernel/FreeRTOS_POSIX)
install(FILES ${_POSIX_SYS_HEADERS} DESTINATION include/kernel/FreeRTOS_POSIX/sys)
endif()
