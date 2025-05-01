cmake_minimum_required(VERSION 3.15)

set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm ASM)
set(CMAKE_ASM_NASM_FLAGS "-f elf32")
project(XGOS LANGUAGES C CXX ASM_NASM)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/Toolchain-i686-elf.cmake" CACHE STRING "Toolchain file")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra")

enable_language(ASM_NASM)

add_executable(kernel
        src/boot.asm
        src/kernel.cpp
        linker.ld
)

target_link_options(kernel PRIVATE "-T${CMAKE_SOURCE_DIR}/linker.ld" "-nostdlib")