BUILD_DIR?=build/

C?=gcc
CFLAGS := -m16 -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-pie

CXX?=g++
CXXFLAGS?=-m16 -Wall -Wextra -g -fno-pie -fno-exceptions -nostdlib -fno-rtti -ffreestanding -nostdlib -fno-use-cxa-atexit -fno-builtin -Iinclude -fno-leading-underscore

LD?=ld
LDFLAGS?=-m elf_i386 -T linker.ld

ASM?=nasm
ASMFLAGS?=-f elf32

SOURCES_CPP=$(wildcard *.cpp)
SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/stage2/c/%.o, $(SOURCES_C))
OBJECTS_CPP=$(patsubst %.cpp, $(BUILD_DIR)/stage2/cpp/%.o, $(SOURCES_CPP))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/stage2/asm/%.o, $(SOURCES_ASM))

.PHONY: all stage2 clean always create_image

all: stage2

$(BUILD_DIR)/stage2/c/%.o: %.c always
	$(C) $(CFLAGS) -o $@ -c $<
	echo "Compiled $<"

$(BUILD_DIR)/stage2/cpp/%.o: %.cpp always
	$(CXX) $(CXXFLAGS) -o $@ -c $<
	echo "Compiled $<"

$(BUILD_DIR)/stage2/asm/%.o: %.asm always
	$(ASM) $(ASMFLAGS) -o $@ $<
	echo "Compiled $<"

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: $(OBJECTS_ASM) $(OBJECTS_CPP) $(OBJECTS_C)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/stage2.bin: $(BUILD_DIR)/stage2.bin
	objcopy -O binary $< $@

always:
	mkdir -p $(BUILD_DIR)/stage2/c
	mkdir -p $(BUILD_DIR)/stage2/cpp
	mkdir -p $(BUILD_DIR)/stage2/asm

clean:
	rm -rf $(BUILD_DIR)