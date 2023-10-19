file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")
file(GLOB _UAPI_HEADERS "include/uapi/*.h")

include_directories(include)
include_directories(../config)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/include/private)
include_directories(${KERNEL_SOURCE}/FreeRTOS-Plus-POSIX/FreeRTOS-Plus-POSIX/include)

add_library(halvip OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/hal/vip)
install(FILES ${_UAPI_HEADERS} DESTINATION include/hal/vip/uapi)
