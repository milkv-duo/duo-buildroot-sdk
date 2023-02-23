file(GLOB _SOURCES "src/${CHIP}/*.c")

include_directories(${DRIVER_GPIO_DIR}/include)
include_directories(${DRIVER_CIF_DIR}/include)
include_directories(${DRIVER_VI_DIR}/include)
include_directories(${DRIVER_SENSOR_DIR}/include)

include_directories(include)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/snsr_i2c)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/cif)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/rtos_cmdqu)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/fast_image)

include_directories(../isp/isp/include/${CHIP}/)
include_directories(../isp/isp/${CHIP}/isp/inc/)
include_directories(../isp/isp/${CHIP}/isp/src/)
include_directories(../isp/isp/${CHIP}/isp_algo/inc/)

include_directories(../isp/tmp_inc/)

add_library(vip STATIC ${_SOURCES})
install(TARGETS vip DESTINATION lib)
