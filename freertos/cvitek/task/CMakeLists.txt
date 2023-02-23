cmake_minimum_required(VERSION 3.16.5)

project(arch C ASM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)
set(CMAKE_INSTALL_INC_PREFIX ${TOP_DIR}/install/include)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing -static --specs=nosys.specs")
#set(SAFETY_FLAGS "-Werror -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS} -D${RUN_TYPE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

include(${TOP_DIR}/scripts/ParseConfiguration.cmake)
ParseConfiguration("${BUILD_ENV_PATH}/.config")

if (RUN_TYPE STREQUAL "BLINKY_DEMO")
    add_subdirectory(demo/Blinky_Demo)
elseif (RUN_TYPE STREQUAL "FULL_DEMO")
    add_subdirectory(demo/Full_Demo)
elseif (RUN_TYPE STREQUAL "POSIX_DEMO")
    add_subdirectory(demo/POSIX_Demo)
elseif (RUN_TYPE STREQUAL "CVIRTOS")
if (CHIP STREQUAL "cv1835")
    add_subdirectory(comm)
    
    add_subdirectory(vip)
    add_subdirectory(display)
    add_subdirectory(vcodec)
elseif (CHIP STREQUAL "cv181x" OR CHIP STREQUAL "cv180x")
    add_subdirectory(comm)
    add_subdirectory(rgn)
    add_subdirectory(audio)
if (CONFIG_FAST_IMAGE_TYPE STRGREATER "0")
    add_subdirectory(vi)
    
    add_subdirectory(camera)
    add_subdirectory(vcodec)
    # add_subdirectory(vip)
endif()
endif()

if(CONFIG_BOARD STREQUAL "cv181x_fpga" OR CONFIG_BOARD STREQUAL "cv181x_fpga_c906")
	add_compile_definitions(FPGA_PORTING)
endif()

endif()
add_subdirectory(main)

add_custom_target(cvirtos.bin
    DEPENDS ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.elf
)
add_custom_command(TARGET cvirtos.bin
    COMMAND echo "Build cvirtos.bin and it depends on ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.elf"
    POST_BUILD
    DEPENDS ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.elf
    COMMAND ${CMAKE_OBJCOPY} ARGS ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.elf -O binary ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.bin
    COMMAND ${CMAKE_OBJDUMP} ARGS -dS ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.elf > ${CMAKE_INSTALL_PREFIX}/bin/cvirtos.dis
    VERBATIM
)
