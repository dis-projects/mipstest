cmake_minimum_required(VERSION 3.5.2)

project(mipstest)

enable_language(C ASM)

SET(TRIPLE mipsel-elvees-elf)
SET(CMAKE_TOOLCHAIN_SYSTEM_NAME Generic)
SET(CMAKE_TOOLCHAIN_SYSTEM_PROCESSOR mips32)
SET(CMAKE_TOOLCHAIN_SYSTEM_TARGET ${TRIPLE})
SET(CMAKE_C_COMPILER   "${TRIPLE}-gcc${EXECUTABLE_SUFFIX}")
SET(CMAKE_CXX_COMPILER "${TRIPLE}-g++${EXECUTABLE_SUFFIX}")
SET(CMAKE_ASM_COMPILER "${TRIPLE}-gcc${EXECUTABLE_SUFFIX}")
SET(TDIR "/opt/Elvees-MCom03-MIPS32.GCC-tools-11.2.0")
#SET(LINK_OBJS "${TDIR}/lib/gcc/${TRIPLE}/11.2.0/crti.o")
 
file(GLOB SYSLIB_SRC lib/*.c)
add_library(syslib STATIC ${SYSLIB_SRC} lib/syscall-asm.s)
target_include_directories(syslib PRIVATE include)
target_compile_options(syslib PRIVATE
	-mips32 -Wall -mlong-calls -EL -fno-builtin-malloc -msoft-float
	-fdata-sections -ffunction-sections)

add_executable(mipstest1 test/reset.S test/test1-syscall.c test/calls.c)
target_include_directories(mipstest1 PRIVATE include)
target_link_libraries(mipstest1 PRIVATE c syslib)
set_target_properties(mipstest1 PROPERTIES
	LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/risc_core.xl -Xlinker -Map -Xlinker mipstest1.map -msoft-float")

add_executable(mipstest2 test/reset.S test/test2-syscall.c test/calls.c)
target_include_directories(mipstest2 PRIVATE include)
target_link_libraries(mipstest2 PRIVATE c syslib)
set_target_properties(mipstest2 PROPERTIES
	LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/risc_core.xl -Xlinker -Map -Xlinker mipstest2.map -msoft-float")

add_executable(simple test/reset.S test/simple.c test/calls.c)
target_include_directories(simple PRIVATE include)
target_link_libraries(simple PRIVATE c syslib)
set_target_properties(simple PROPERTIES
        LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/risc_core.xl -Xlinker -Map -Xlinker simple.map -msoft-float")

add_executable(writer test/reset.S test/writer.c test/calls.c)
target_include_directories(writer PRIVATE include)
target_link_libraries(writer PRIVATE c syslib)
set_target_properties(writer PROPERTIES
        LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/risc_core.xl -Xlinker -u -Xlinker sbrk -Xlinker -Map -Xlinker writer.map -msoft-float")
