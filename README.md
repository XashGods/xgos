# XG OS

XG OS is a hobby operating system targeting the i686 architecture.  
This repository follows the [Roadmap](ROADMAP.md) for guided development.

## Prerequisites

Before building, please complete the steps in [docs/PREREQUISITES.md](docs/PREREQUISITES.md).  
Git version control will be set up separately.

## Building

```bash
mkdir -p build
cd build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-i686-elf.cmake ..
make
```

This will produce the `kernel` ELF binary.  
Subsequent steps will include creating a bootable image and writing the bootloader.