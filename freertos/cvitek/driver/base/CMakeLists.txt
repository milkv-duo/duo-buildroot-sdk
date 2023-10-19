file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")

include_directories(include)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/private)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include)

add_library(base OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/driver/base)
