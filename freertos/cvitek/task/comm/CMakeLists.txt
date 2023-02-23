file(GLOB _SOURCES "src/${RUN_ARCH}/*.c")

if (CONFIG_FAST_IMAGE_TYPE STRGREATER "0")
add_compile_definitions(FAST_IMAGE_ENABLE)
endif()

include_directories(include)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/spinlock)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/jenc)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/rtos_cmdqu)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/fast_image)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)

add_library(comm STATIC ${_SOURCES})
install(TARGETS comm DESTINATION lib)

