file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")

include_directories(include)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
#include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/spinlock)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)

add_library(spinlock OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/driver/spinlock)
