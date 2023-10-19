cmake_minimum_required(VERSION 3.16.5)

project(hal C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(KERNEL_SOURCE ${TOP_DIR}/../Source)

set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)
set(CMAKE_INSTALL_INC_PREFIX ${TOP_DIR}/install/include)

set(SAFETY_FLAGS "-Wall -Wextra -fno-strict-aliasing")
#set(SAFETY_FLAGS "-Werror -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SAFETY_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFETY_FLAGS}")

# add hal folder here
set(build_list
	uart
	pinmux
	# i2c
)

foreach(build_hal ${build_list})
    add_subdirectory(${build_hal})
    set(hal_list ${hal_list} hal${build_hal})
endforeach()

add_library(hal STATIC dummy.c)
target_link_libraries(hal ${hal_list})

file(GLOB _HEADERS "config/*.h")
install(FILES ${_HEADERS} DESTINATION include/hal/config)
install(TARGETS hal DESTINATION lib)
