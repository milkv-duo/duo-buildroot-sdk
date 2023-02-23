file(GLOB _SOURCES "src/${RUN_ARCH}/*.c")

include_directories(include)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/jenc)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/rtos_cmdqu)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/fast_image)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)
include_directories(../comm/include)

add_library(vcodec STATIC ${_SOURCES})
install(TARGETS vcodec DESTINATION lib)

