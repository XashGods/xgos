# Toolchain & Prerequisites

This document outlines the tools and environment required to build and run XG OS.

## Cross-Compiler

We target i686-elf. You need:

- **binutils** and **GCC** for `i686-elf`
- Install a prebuilt toolchain or build your own. On Ubuntu:
  ```bash
  sudo apt-get install gcc-multilib build-essential bison flex libgmp-dev libmpfr-dev libmpc-dev texinfo
  # Download and build binutils
  wget https://ftp.gnu.org/gnu/binutils/binutils-with-gold-2.44.tar.xz
  tar xf binutils-with-gold-2.44.tar.xz && mkdir binutils-build && cd binutils-build
  ../binutils-with-gold-2.44/configure --target=i686-elf --disable-nls --disable-werror --prefix="$HOME/opt/cross"
  make -j$(nproc) && make install
  cd ..
  # Download and build GCC
  wget https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.xz
  tar xf gcc-15.1.0.tar.xz && mkdir gcc-build && cd gcc-build
  ../gcc-15.1.0/configure --target=i686-elf --disable-nls --disable-libstdcxx-pch --with-newlib --disable-shared --disable-threads --disable-multilib --disable-libssp --disable-libada --enable-languages=c,c++ --prefix="$HOME/opt/cross"
  make -j$(nproc) all-gcc && make install-gcc
  make -j$(nproc) all-target-libgcc && make install-target-libgcc
  ```

Ensure `PATH` includes your cross toolchain, e.g.:
```bash
export PATH="$HOME/opt/cross/bin:$PATH"
```

## Assembler & Linker

- **NASM** (or YASM) for assembly.
- **GNU ld** or **LLD** for linking with our custom `linker.ld`.

Install on Ubuntu:
```bash
sudo apt-get install nasm
```

## Emulation & Debugging

- **QEMU** and/or **Bochs** for running the OS:
```bash
sudo apt-get install qemu-system-x86 bochs bochs-x
```
- **GDB** for debugging:
```bash
sudo apt-get install gdb-multiarch
```

## Documentation & References

- OSDev Wiki (https://wiki.osdev.org)
- "Jump into OSDev" tutorial
- Bran’s Kernel Development Tutorial