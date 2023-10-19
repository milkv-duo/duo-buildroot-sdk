cmake_minimum_required(VERSION 3.16.5)

project(driver C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(KERNEL_SOURCE ${TOP_DIR}/../Source)

set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)
set(CMAKE_INSTALL_INC_PREFIX ${TOP_DIR}/install/include)
set(CMAKE_DRIVER_DIR ${TOP_DIR}/driver)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

set(DRIVER_UART_DIR ${CMAKE_DRIVER_DIR}/uart)
set(DRIVER_GPIO_DIR ${CMAKE_DRIVER_DIR}/gpio)
set(DRIVER_I2C_DIR ${CMAKE_DRIVER_DIR}/i2c)

include(${TOP_DIR}/scripts/ParseConfiguration.cmake)
ParseConfiguration("${BUILD_ENV_PATH}/.config")

if(CONFIG_BOARD STREQUAL "cv181x_fpga" OR CONFIG_BOARD STREQUAL "cv181x_fpga_c906")
	add_compile_definitions(FPGA_PORTING)
endif()

add_compile_definitions(F18)

if (CONFIG_CHIP_ARCH_cv181x STREQUAL "y")
add_compile_definitions(__CV181X__)
endif()
if (CONFIG_CHIP_ARCH_cv180x STREQUAL "y")
add_compile_definitions(__CV180X__)
endif()

if (CONFIG_FAST_IMAGE_TYPE STRGREATER "0")
add_compile_definitions(FAST_IMAGE_ENABLE)
endif()

if (CONFIG_CHIP_ARCH_cv181x STREQUAL "y" OR CONFIG_CHIP_ARCH_cv180x STREQUAL "y")
add_compile_definitions(ARCH_RTOS_CV181X)
endif()

if (CONFIG_BOARD_wevb_0007a_spinor STREQUAL "y" OR
    CONFIG_BOARD_wevb_0007a_emmc STREQUAL "y" OR
    CONFIG_BOARD_wevb_0007a_spinand STREQUAL "y")
	add_compile_definitions(BOARD_WEVB_007A)
	message("use BOARD_WEVB_007A")
elseif (CONFIG_BOARD_wevb_0006a_spinor STREQUAL "y" OR
	CONFIG_BOARD_wevb_0006a_emmc STREQUAL "y" OR
	CONFIG_BOARD_wevb_0006a_spinand STREQUAL "y")
	add_compile_definitions(BOARD_WEVB_006A)
	message("use BOARD_WEVB_006A")
else()
	message("find no board")
endif()

# add driver folder here
if (RUN_ARCH STREQUAL "riscv64")
set(DRIVER_UART_DIR ${CMAKE_DRIVER_DIR}/uart)
set(DRIVER_SPINLOCK_DIR ${CMAKE_DRIVER_DIR}/spinlock)
set(DRIVER_RTOS_CMDQU_DIR ${CMAKE_DRIVER_DIR}/rtos_cmdqu)

set(driver_list
	common
	pinmux
	uart
	spinlock
	gpio
	rtos_cmdqu
)
else()
set(DRIVER_BASE_DIR ${CMAKE_DRIVER_DIR}/base)
set(DRIVER_SYS_DIR ${CMAKE_DRIVER_DIR}/sys)

set(driver_list
	common
	base
	i2c
	uart
	pinmux
	sys
)
endif()

add_library(driver STATIC dummy.c)
foreach(build_drv ${driver_list})
	add_subdirectory(${build_drv})
endforeach()

target_link_libraries(driver ${driver_list})
install(TARGETS driver DESTINATION lib)
