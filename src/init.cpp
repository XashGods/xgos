// C++ initialization call global constructors
#include <stddef.h>
extern "C" {
    using ctor_t = void (*)();
    extern ctor_t __init_array_start[];
    extern ctor_t __init_array_end[];

    void init_global_ctors() {
        for (ctor_t* ctor = __init_array_start; ctor < __init_array_end; ++ctor) {
            (*ctor)();
        }
    }
}