project(libc C ASM)
FILE(GLOB_RECURSE srcs *.c *.S)

include_directories(include ../kernel/include)
add_library(c STATIC ${srcs} include/sys/stat.h)
target_compile_definitions(c PUBLIC __is_libk)

SET(CMAKE_C_FLAGS "-m32 -ffreestanding -O0 -Wall -Wextra -nostdlib -lgcc -g3")