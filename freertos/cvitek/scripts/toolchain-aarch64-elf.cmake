include(CMakeForceCompiler)

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
SET( CMAKE_SYSTEM_NAME Generic)
set( CMAKE_SYSTEM_PROCESSOR     aarch64 )
set( ARCH aarch64 )
#set( CROSS_COMPILE aarch64-linux-gnu-)
set( CROSS_COMPILE aarch64-elf-)

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_S_COMPILER ${CROSS_COMPILE}gcc)

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

# skip tests
set(CMAKE_C_COMPILER_WORKS 1)

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy
	    CACHE FILEPATH "The toolchain objcopy command " FORCE )

# Set the CMAKE C flags (which should also be used by the assembler!
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DFREERTOS_BSP" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--gc-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcmodel=large" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFREERTOS_BSP -DGUEST" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_SMP" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_64BIT" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_PHYS_ADDR_T_64BIT" )
# need to compatible with linux-kernel structure size, please check with linux-kernl .config
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_DEBUG_SPINLOCK" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEVICE_FROM_DTS" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__linux" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses" )
