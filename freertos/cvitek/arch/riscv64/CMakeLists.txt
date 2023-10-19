file(GLOB _SOURCES "src/*.c" "src/*.S")
file(GLOB _HEADERS "include/*.h")
file(GLOB _HEADERS_ASM "include/asm/*.h")

include_directories(include)
include_directories(${TOP_DIR}/common/include)

add_library(arch STATIC ${_SOURCES})

install(TARGETS arch DESTINATION lib)
install(FILES ${_HEADERS} DESTINATION include/arch)
install(FILES ${_HEADERS_ASM} DESTINATION include/arch/asm)
