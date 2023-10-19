file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")

include_directories(include)
include_directories(../config)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

add_library(haluart OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/hal/uart)
