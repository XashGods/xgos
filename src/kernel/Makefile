SRC_DIR?=src/
BUILD_DIR?=build/
CXX=i386-elf-g++
CXXFLAGS?= -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

SOURCES_CPP=$(wildcard *.cpp)
OBJECTS_CPP=$(patsubst %.cpp, $(BUILD_DIR)/boot/cpp/%.o, $(SOURCES_CPP))

.PHONY: all always clean

all: always $(OBJECTS_CPP)

$(BUILD_DIR)/boot/cpp/%.o: %.cpp always
	$(CXX) -c $< -o $@ $(CXXFLAGS)

always:
	mkdir -p $(BUILD_DIR)/boot/cpp

clean:
	rm -rf $(BUILD_DIR)/boot/cpp