BUILD_DIR?=build/
SRC_DIR?=src/

C?=x86_64-elf-gcc
CFLAGS :=-nostdlib -static -ffreestanding -O2 -Wall -Wextra
CXX?=x86_64-elf-g++
CXXFLAGS?= -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

LD?=x86_64-elf-ld -nostdlib -T linker.ld

ASM?=x86_64-elf-as

SOURCES_CPP=$(wildcard *.cpp)
SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/boot/c/%.o, $(SOURCES_C))
OBJECTS_CPP=$(patsubst %.cpp, $(BUILD_DIR)/boot/cpp/%.o, $(SOURCES_CPP))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/boot/asm/%.o, $(SOURCES_ASM))

.PHONY: all boot clean always create_image

all: boot

$(BUILD_DIR)/boot/c/%.o: %.c always
	$(C) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/boot/cpp/%.o: %.cpp always
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/boot/asm/%.o: %.asm always
	$(ASM) $< -o $@ 

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: $(OBJECTS_ASM) $(OBJECTS_C) $(OBJECTS_CPP)
	x86_64-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

always:
	mkdir -p $(BUILD_DIR)/boot/c
	mkdir -p $(BUILD_DIR)/boot/cpp
	mkdir -p $(BUILD_DIR)/boot/asm

clean:
	rm -rf $(BUILD_DIR)