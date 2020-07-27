cmake_minimum_required (VERSION 2.8)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
PROJECT (kernel C ASM)

FILE(GLOB GENERIC_SRCS "*.c")
FILE(GLOB_RECURSE DRIVER_SRCS "drivers/*.c")
FILE(GLOB_RECURSE THREAD_SRCS "threading/*.c")
FILE(GLOB_RECURSE SYSCALL_SRCS "syscalls/*.c")
FILE(GLOB_RECURSE FS_SRCS "fs/*.c")
FILE(GLOB_RECURSE MM_SRCS "mm/*.c")
list(APPEND GENERIC_SRCS ${DRIVER_SRCS} ${THREAD_SRCS} ${SYSCALL_SRCS} ${MM_SRCS} ${FS_SRCS})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(load_arch)
include_directories(../libk/include)
include_directories(include)

LOAD_ARCH("x86")

# Now set up our environment
ADD_EXECUTABLE(${PROJECT_NAME}.bin ${ARCH_LAYOUT} ${ARCH_SRCS} ${GENERIC_SRCS})
target_link_libraries(${PROJECT_NAME}.bin c gcc)
SET (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
SET (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET (CMAKE_ASM_FLAGS "${ARCH_ASM_FLAGS} ${PLATFORM_ASM_FLAGS}")

SET(CMAKE_C_FLAGS "${ARCH_C_FLAGS} ${PLATFORM_C_FLAGS} -ffreestanding -Wall -Wextra -nostdlib -g3")
SET_TARGET_PROPERTIES(${PROJECT_NAME}.bin PROPERTIES LINK_FLAGS "-T ${ARCH_LAYOUT} -N ${ARCH_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")

file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/isodir ${CMAKE_BINARY_DIR}/isodir/boot ${CMAKE_BINARY_DIR}/isodir/boot/grub)

file(WRITE ${CMAKE_BINARY_DIR}/isodir/boot/grub/grub.cfg "timeout=0\n menuentry \"Kernel \" {\n	multiboot2 /boot/${PROJECT_NAME}.bin\n}")

find_program(MAKE_RESCUE grub-mkrescue REQUIRED)

add_custom_target(
        kernel.iso
        COMMAND cp ${CMAKE_BINARY_DIR}/src/kernel/${PROJECT_NAME}.bin ${CMAKE_BINARY_DIR}/isodir/boot/${PROJECT_NAME}.bin
        DEPENDS ${PROJECT_NAME}.bin
)

add_custom_command(
        COMMAND ${MAKE_RESCUE}
        ARGS -o ${PROJECT_NAME}.iso ${CMAKE_BINARY_DIR}/isodir
        TARGET kernel.iso
)