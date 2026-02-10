#include "PluginHooks.hpp"
#include "Patches.hpp"
#include "VoiceChat/VoiceChat.hpp"
#include "csvc.h"
#include <3ds.h>

#include <string>
#include <vector>

namespace CTRPluginFramework
{
    static VoiceChat    s_voice_chat;
    static MenuEntry *  s_connect_entry = nullptr;
    static bool         s_was_connected = false;
    static bool         s_voice_inited = false;

    static void VoiceChatFrameCallback(void)
    {
        if (s_connect_entry && s_was_connected && !s_connect_entry->IsActivated())
        {
            s_voice_chat.Stop();
            s_was_connected = false;
        }
        if (s_connect_entry && s_connect_entry->IsActivated())
            s_was_connected = true;
    }

    static void ConnectGameFunc(MenuEntry *entry)
    {
        if (entry->WasJustActivated())
        {
            if (!s_voice_inited)
            {
                if (!s_voice_chat.Init())
                {
                    OSD::Notify("Voice Chat: Init failed", Color::Red);
                    entry->Disable();
                    return;
                }
                s_voice_inited = true;
            }
            s_voice_chat.Start();
        }
        s_voice_chat.Update();
    }

    static void SetServerIPMenuFunc(MenuEntry *entry)
    {
        (void)entry;
        std::string ip = s_voice_chat.GetServerIP();
        Keyboard kbd("Server IP address");
        kbd.SetMaxLength(16);
        if (kbd.Open(ip, ip) != -1)
        {
            s_voice_chat.SetServerIP(ip);
            OSD::Notify("Server IP: " + ip, Color::Cyan);
        }
    }

    static void ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if (R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    void PatchProcess(FwkSettings &settings)
    {
        (void)settings;
        ToggleTouchscreenForceOn();
        InitSocAndHooks();
    }

    void OnProcessExit(void)
    {
        if (s_voice_inited)
        {
            s_voice_chat.Exit();
            s_voice_inited = false;
        }
        OnProcessExitSoc();
        ToggleTouchscreenForceOn();
    }

    void InitMenu(PluginMenu &menu)
    {
        MenuFolder *vc_folder = new MenuFolder("Voice Chat", "UDP voice chat (PTT: R)");

        s_connect_entry = new MenuEntry("Connect", ConnectGameFunc, nullptr,
            "Enable voice chat. When on, R = Push-to-Talk.");
        *vc_folder += s_connect_entry;

        *vc_folder += new MenuEntry("Set server IP", nullptr, SetServerIPMenuFunc,
            "Set destination IP (empty = broadcast).");

        menu += vc_folder;
        menu.Callback(VoiceChatFrameCallback);
    }
}
