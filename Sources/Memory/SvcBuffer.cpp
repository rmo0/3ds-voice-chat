#include "Memory/SvcBuffer.hpp"
#include <3ds.h>
#include "csvc.h"

namespace CTRPluginFramework
{
    namespace
    {
        inline u32 align_up(u32 size, u32 align)
        {
            return (size + align - 1) & ~(align - 1);
        }
    }

    bool SvcBuffer::Alloc(u32 size_bytes)
    {
        if (addr != 0)
            return false;
        size_bytes = align_up(size_bytes, 0x1000);
        u32 out = 0;
        Result r = svcControlMemoryUnsafe(&out, 0, size_bytes, (MemOp)(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), (MemPerm)(MEMPERM_READ | MEMPERM_WRITE));
        if (R_FAILED(r))
            return false;
        addr = out;
        size = size_bytes;
        return true;
    }

    void SvcBuffer::Free()
    {
        if (addr == 0)
            return;
        svcControlMemoryUnsafe(&addr, addr, size, MEMOP_FREE, (MemPerm)0);
        addr = 0;
        size = 0;
    }
}
