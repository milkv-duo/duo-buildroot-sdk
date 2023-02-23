include(CMakeForceCompiler)

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
SET( CMAKE_SYSTEM_NAME Generic)
set( CMAKE_SYSTEM_PROCESSOR     riscv64 )
set( ARCH riscv64 )
#set( CROSS_COMPILE aarch64-linux-gnu-)
set( CROSS_COMPILE riscv64-unknown-elf-)

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
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64imafdc" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mabi=lp64d" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--gc-sections" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcmodel=medany" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFREERTOS_BSP" )
if ($ENV{DDR_64MB_SIZE} STREQUAL "y")
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX_BSP_64MB" )
else()
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX_BSP_128MB" )
endif()
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__riscv_xlen=64" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTHEAD_C906" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_64BIT" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRISCV_FPU" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPXP_PLAT" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFPGA_PLAT" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRISCV_QEMU" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses" )
