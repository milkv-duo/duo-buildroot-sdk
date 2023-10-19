file(GLOB _SOURCES "src/*.c")
file(GLOB _HEADERS "include/*.h")

include_directories(include)
include_directories(../config)
include_directories(${TOP_DIR}/driver/i2c/include)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)

add_library(hali2c OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/hal/i2c)
