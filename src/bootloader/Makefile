BUILD_DIR?=build/
SRC_DIR?=src/

C?=i386-elf-gcc
CFLAGS :=-nostdlib -static -ffreestanding -O2 -Wall -Wextra  -I../kernel -I../include
CXX=i386-elf-g++
CXXFLAGS?= -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I../kernel -I../include

LD?=i386-elf-ld -nostdlib -T linker.ld

ASM?=i386-elf-as

SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/boot/c/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/boot/asm/%.o, $(SOURCES_ASM))
OBJECTS_KERNEL=$(wildcard $(BUILD_DIR)/boot/cpp/*.o)

.PHONY: all boot clean always create_image

all: boot

$(BUILD_DIR)/boot/c/%.o: %.c always
	$(C) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/boot/asm/%.o: %.asm always
	$(ASM) $< -o $@ 

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: $(OBJECTS_ASM) $(OBJECTS_KERNEL) $(OBJECTS_C) 
	i386-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

always:
	mkdir -p $(BUILD_DIR)/boot/c
	mkdir -p $(BUILD_DIR)/boot/cpp
	mkdir -p $(BUILD_DIR)/boot/asm

clean:
	rm -rf $(BUILD_DIR)
