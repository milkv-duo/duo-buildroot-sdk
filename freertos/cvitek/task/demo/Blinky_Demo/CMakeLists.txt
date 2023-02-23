file(GLOB _SOURCES "*.c")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__BUILD_RTOS_KERNEL__")

include_directories(src)
include_directories(include)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

add_library(demo STATIC ${_SOURCES})
install(TARGETS demo DESTINATION lib)

