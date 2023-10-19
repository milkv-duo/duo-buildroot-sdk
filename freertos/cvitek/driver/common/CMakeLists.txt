file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")

include_directories(include)

include_directories(${DRIVER_UART_DIR}/include)
include_directories(${DRIVER_I2C_DIR}/include)
include_directories(${DRIVER_RTOS_CMDQU_DIR}/include)
include_directories(${DRIVER_FAST_IMAGE_DIR}/include)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/pinmux)

add_library(common OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/driver/common)
