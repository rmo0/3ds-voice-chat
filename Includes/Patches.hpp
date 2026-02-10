#ifndef PATCHES_HPP
#define PATCHES_HPP

#include "types.h"

namespace CTRPluginFramework
{
    constexpr u32 k_patches_sharedmem_addr = 0x7500000;
    constexpr u32 k_patches_sharedmem_size = 0x20000;

    bool InitSocAndHooks(void);
    void OnProcessExitSoc(void);
    void *GetSocContext(void);
    u32 GetSocContextSize(void);
}

#endif
