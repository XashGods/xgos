rm -rf build/ xgos.iso
cmake -B build -S . && cmake --build build
bash -E ./build_iso.sh
qemu-system-x86_64 -cdrom xgos.iso