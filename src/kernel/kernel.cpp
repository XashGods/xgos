#include "kernel.h"
#include "vga.hpp"

void init_kernel() {
    CVGA vga;
    vga.Initialize();
    vga.WriteString("Kernel working written in C++\n");
    vga.WriteString("XGOS - XashGods\n");
}

extern "C" void cpp_main(void)
{
    init_kernel();
}