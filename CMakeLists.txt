cmake_minimum_required(VERSION 3.15)

set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm ASM)
set(CMAKE_ASM_NASM_FLAGS "-f elf64")
project(XGOS LANGUAGES C CXX ASM_NASM)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/Toolchain-x86_64-elf.cmake" CACHE STRING "Cross-toolchain file for x86_64-elf builds")

# kernel-specific flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2")

# disable PIE (Position Independent Executable) for kernel
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -no-pie")

enable_language(ASM_NASM)

add_executable(kernel
        src/boot.asm
        src/init.cpp
        src/console.cpp
        src/memory.cpp
        src/paging.cpp
        src/math.cpp
        src/graphics.cpp
        src/graphics_demo.cpp
        src/kernel.cpp
        linker.ld
)

target_link_options(kernel PRIVATE 
    "-T${CMAKE_SOURCE_DIR}/linker.ld" 
    "-nostdlib" 
    "-fno-pie" 
    "-no-pie"
    "-z" "max-page-size=0x1000"
)