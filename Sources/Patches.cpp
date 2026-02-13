#include "Patches.hpp"
#include <CTRPluginFramework.hpp>
#include <3ds.h>
#include <3ds/ipc.h>
#include "csvc.h"

#include <vector>

extern "C" Handle SOCU_handle;

namespace CTRPluginFramework
{
    static bool s_soc_inited_by_patches = false;

    static Result SOCU_Shutdown(void)
    {
        u32 *cmdbuf = getThreadCommandBuffer();
        cmdbuf[0] = IPC_MakeHeader(0x19, 0, 0);
        Result ret = svcSendSyncRequest(SOCU_handle);
        if (ret != 0)
            return ret;
        return cmdbuf[1];
    }

    static u32 SocketInitHook(u32 mem, u32 size)
    {
        SOCU_Shutdown();
        return HookContext::GetCurrent().OriginalFunction<u32>(mem, size);
    }

    static u32 SocketExitHook(u32 returnVal)
    {
        socExit();
        socInit((u32 *)k_patches_sharedmem_addr, k_patches_sharedmem_size);
        return HookContext::GetCurrent().OriginalFunction<u32>(returnVal);
    }

    static bool InstallSocketHooks(void)
    {
        static const std::vector<u32> socInitPattern = {
            0xE92D4070, 0xE1A05000, 0xE1A04001,
            0xE59F10A4, 0xE59F00A4, 0xE24DD008
        };
        static const std::vector<u32> socExitPattern = {
            0xE59F003C, 0xE92D4038, 0xE3A05000,
            0xE5C05000, 0xE5900004, 0xE58D0000, 0xE1A0000D
        };

        Hook initHook, exitHook;
        u32 addr = Utils::Search(0x00100000, Process::GetTextSize(), socInitPattern);
        if (!addr)
            return false;

        initHook.InitializeForMitm(addr, (u32)SocketInitHook);
        if (initHook.Enable() != HookResult::Success)
            return false;

        addr = Utils::Search(0x00100000, Process::GetTextSize(), socExitPattern);
        if (!addr)
            return false;

        exitHook.InitializeForMitm(addr + 0x40, (u32)SocketExitHook);
        if (exitHook.Enable() != HookResult::Success)
            return false;

        return true;
    }

    bool InitSocAndHooks(void)
    {
        if (s_soc_inited_by_patches)
            return true;

        if (System::IsCitra())
        {
            if (R_FAILED(socInit((u32 *)0, 0x20000)))
                return false;
            s_soc_inited_by_patches = true;
            return true;
        }

        u32 out_addr = 0;
        Result r = svcControlMemoryUnsafe(&out_addr, k_patches_sharedmem_addr, k_patches_sharedmem_size,
            (MemOp)(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), (MemPerm)(MEMPERM_READ | MEMPERM_WRITE));
        if (R_FAILED(r))
            return false;

        if (R_FAILED(socInit((u32 *)k_patches_sharedmem_addr, k_patches_sharedmem_size)))
        {
            svcControlMemoryUnsafe(nullptr, k_patches_sharedmem_addr, k_patches_sharedmem_size, MEMOP_FREE, (MemPerm)0);
            return false;
        }

        if (!InstallSocketHooks())
        {
            socExit();
            svcControlMemoryUnsafe(nullptr, k_patches_sharedmem_addr, k_patches_sharedmem_size, MEMOP_FREE, (MemPerm)0);
            return false;
        }

        s_soc_inited_by_patches = true;
        return true;
    }

    void OnProcessExitSoc(void)
    {
        if (!s_soc_inited_by_patches)
            return;
        socExit();
        if (!System::IsCitra())
            svcControlMemoryUnsafe(nullptr, k_patches_sharedmem_addr, k_patches_sharedmem_size, MEMOP_FREE, (MemPerm)0);
        s_soc_inited_by_patches = false;
    }

    void *GetSocContext(void)
    {
        if (!s_soc_inited_by_patches)
            return nullptr;
        return System::IsCitra() ? (void *)1 : (void *)k_patches_sharedmem_addr;
    }

    u32 GetSocContextSize(void)
    {
        return s_soc_inited_by_patches ? k_patches_sharedmem_size : 0;
    }
}
