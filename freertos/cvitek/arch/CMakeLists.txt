cmake_minimum_required(VERSION 3.16.5)

project(arch C ASM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing")
#set(SAFETY_FLAGS "-Werror -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${RUN_TYPE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)

if (RUN_ARCH STREQUAL "riscv64")
    add_subdirectory(riscv64)
else()
    add_subdirectory(arm64)
endif()
