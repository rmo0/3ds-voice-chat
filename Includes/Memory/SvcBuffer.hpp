#ifndef SVCBUFFER_HPP
#define SVCBUFFER_HPP

#include "types.h"

namespace CTRPluginFramework
{
    struct SvcBuffer
    {
        u32 addr;
        u32 size;
        bool alloc_fixed;

        SvcBuffer() : addr(0), size(0), alloc_fixed(false) {}

        bool Alloc(u32 size_bytes);
        bool AllocFixed(u32 fixed_addr, u32 size_bytes);
        void Free();

        void* Ptr() const { return addr ? reinterpret_cast<void*>(addr) : nullptr; }
        bool Empty() const { return addr == 0 || size == 0; }
    };
}
#endif
