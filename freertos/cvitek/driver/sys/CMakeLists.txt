file(GLOB_RECURSE _SOURCES "*.c")
file(GLOB_RECURSE _HEADERS "*.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMJPEG_INTERFACE_API" )

include_directories(include/)

#include_directories(${DRIVER_SYS_DIR}/include)
include_directories(${DRIVER_VIP_DIR}/include)
include_directories(${DRIVER_BASE_DIR}/include)

#include_directories(${DRIVER_VENC_DIR}/)
#include_directories(${DRIVER_VENC_DIR}/cvi)
#include_directories(${DRIVER_VENC_DIR}/sample)
#include_directories(${DRIVER_VENC_DIR}/sample/helper)
#include_directories(${DRIVER_VENC_DIR}/sample/helper/misc)
#include_directories(${DRIVER_VENC_DIR}/sample/helper/mscv)
#include_directories(${DRIVER_VENC_DIR}/vdi)
#include_directories(${DRIVER_VENC_DIR}/vdi/linux/driver/)
#include_directories(${DRIVER_VENC_DIR}/vpuapi)
#include_directories(${DRIVER_VENC_DIR}/vpuapi/coda9/)
#include_directories(${DRIVER_VENC_DIR}/vpuapi/coda9/980_roi_rc_lib/)
#
#include_directories(${DRIVER_JPEG_DIR}/bm_jpg_interface/inc)
#include_directories(${DRIVER_JPEG_DIR}/bmjpuapi)
#include_directories(${DRIVER_JPEG_DIR}/bmjpuapi/src)
#include_directories(${DRIVER_JPEG_DIR}/bmjpulite)
#include_directories(${DRIVER_JPEG_DIR}/include)
#include_directories(${DRIVER_JPEG_DIR}/jdi)
#include_directories(${DRIVER_JPEG_DIR}/jdi/linux/driver/)
#include_directories(${DRIVER_JPEG_DIR}/jpuapi)
#include_directories(${DRIVER_JPEG_DIR}/test)

include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/cif)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)

add_library(sys OBJECT ${_SOURCES})

install(FILES ${_HEADERS} DESTINATION include/driver/sys)
