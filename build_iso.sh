#!/usr/bin/env bash
set -euo pipefail

# Automated build & ISO creation for XG OS
# Requirements: cmake, make, grub-mkrescue, xorriso (provided by grub-pc-bin or grub2-tools)

# Configuration
BUILD_DIR=build/
ISO_DIR=build/iso_root
ISO_NAME=xgos.iso
KERNEL_BIN=kernel

# Clean previous outputs
rm -rf "$BUILD_DIR" "$ISO_DIR" "$ISO_NAME"

## Build the kernel (using cross-toolchain)
# Detect and add freestanding cross-compiler
CROSS_BIN=${CROSS_BIN:-"$HOME/opt/cross/bin"}
if [ -x "$CROSS_BIN/x86_64-elf-gcc" ]; then
  echo "[*] Found cross-compiler at $CROSS_BIN"
  export PATH="$CROSS_BIN:$PATH"
elif command -v x86_64-elf-gcc >/dev/null 2>&1; then
  echo "[*] Using cross-compiler from PATH"
else
  echo "Error: x86_64-elf-gcc not found."
  echo "Please install the freestanding cross toolchain and ensure x86_64-elf-gcc is in PATH or set CROSS_BIN."
  exit 1
fi

echo "[1/4] Configuring project with x86_64-elf toolchain..."
cmake -B "$BUILD_DIR" -S . \
      -DCMAKE_TOOLCHAIN_FILE="$(pwd)/cmake/Toolchain-x86_64-elf.cmake"
echo "[2/4] Building kernel..."
cmake --build "$BUILD_DIR"

# Prepare ISO directory structure
echo "[3/4] Preparing ISO tree..."
mkdir -p "$ISO_DIR/boot/grub"
cp "$BUILD_DIR/$KERNEL_BIN" "$ISO_DIR/boot/$KERNEL_BIN"
cat > "$ISO_DIR/boot/grub/grub.cfg" << EOF
set timeout=0
menuentry "XG OS" {
  multiboot /boot/$KERNEL_BIN
  boot
}
EOF

# Locate GRUB modules directory
echo "Locating GRUB modules..."
GRUB_MODULE_DIR=""
for dir in "/usr/lib/grub/i386-pc" "/usr/lib/grub2/i386-pc"; do
  if [ -d "$dir" ]; then
    GRUB_MODULE_DIR="$dir"
    break
  fi
done
if [ -z "$GRUB_MODULE_DIR" ]; then
  echo "Error: GRUB modules directory not found."
  echo "Install grub-pc-bin (or grub2-tools) and try again."
  exit 1
fi

# Create bootable ISO
echo "[4/4] Generating ISO ($ISO_NAME)..."
grub-mkrescue -d "$GRUB_MODULE_DIR" -o "$ISO_NAME" "$ISO_DIR"

echo "Done. Created ISO: $ISO_NAME"