cmake_minimum_required(VERSION 3.16.5)

project(arch C ASM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)
set(CMAKE_INSTALL_INC_PREFIX ${TOP_DIR}/install/include)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing")
#set(SAFETY_FLAGS "-Werror -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

include(${TOP_DIR}/scripts/ParseConfiguration.cmake)
ParseConfiguration("${BUILD_ENV_PATH}/.config")

if (ARCH STREQUAL "")
    message(*** Please set ARCH in scripts/*cmake. ***)
    exit()
endif()

if (CONFIG_CHIP_ARCH_cv181x STREQUAL "y")
add_compile_definitions(__CV181X__)
endif()
if (CONFIG_CHIP_ARCH_cv180x STREQUAL "y")
add_compile_definitions(__CV180X__)
endif()

if (RUN_ARCH STREQUAL "riscv64")
file(GLOB _SOURCES	"src/${RUN_ARCH}/*.c"
					"src/${RUN_ARCH}/riscv_optimized/*.S"
)
file(GLOB _HEADERS	"include/${RUN_ARCH}/*.h"
					"include/interdrv/common/*.h"
					"include/interdrv/chip/${RUN_ARCH}/*.h"
					"include/interdrv/chip/${RUN_ARCH}/isp/*.h"
)
file(GLOB _HEADERS_LINUX "include/${RUN_ARCH}/linux/*.h")

include_directories(include/${RUN_ARCH})
include_directories(include/interdrv/common)
include_directories(include/interdrv/chip/${RUN_ARCH})

install(FILES ${_HEADERS_LINUX} DESTINATION include/common/linux)

else()
file(GLOB _SOURCES	"src/${CHIP}/*.c"
					"src/${CHIP}/media/*.c"
					"src/${CHIP}/arm_optimized-routines/*.c"
					"src/${CHIP}/arm_optimized-routines/${ARCH}/*.c"
					"src/${CHIP}/arm_optimized-routines/${ARCH}/*.S")
file(GLOB _HEADERS	"include/${CHIP}/*.h"
					"include/${CHIP}/arm_optimized-routines/*.h")
file(GLOB _HEADERS_LINUX "include/${CHIP}/linux/*.h")
file(GLOB _HEADERS_MEDIA "include/${CHIP}/media/*.h")

include_directories(include/${CHIP})
include_directories(include/${CHIP}/arm_optimized-routines)
install(FILES ${_HEADERS_LINUX} DESTINATION include/common/linux)
install(FILES ${_HEADERS_MEDIA} DESTINATION include/common/media)
endif()

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

add_library(common STATIC ${_SOURCES})

install(TARGETS common DESTINATION lib)
install(FILES ${_HEADERS} DESTINATION include/common)
