#include "VoiceChat/VoiceChat.hpp"
#include "Patches.hpp"
#include "ncsnd.h"
#include <3ds.h>
#include "csvc.h"

namespace CTRPluginFramework
{
    namespace
    {
        const u32 k_soc_context_size = 0x10000;
        const u32 k_mic_buffer_size  = 0x4000;
        const u32 k_tx_buffer_size   = 0x800;
        const u32 k_rx_buffer_size   = 0x800;
    }

    bool VoiceChat::Init()
    {
        void *soc_ctx = GetSocContext();
        u32 soc_size = GetSocContextSize();
        if (!soc_ctx || soc_size == 0)
        {
            if (!_soc_context.Alloc(k_soc_context_size))
                return false;
            soc_ctx = _soc_context.Ptr();
            soc_size = _soc_context.size;
            _soc_context_owned = true;
        }
        else
        {
            _soc_context_owned = false;
        }

        if (!_mic_buffer.Alloc(k_mic_buffer_size))
        {
            if (_soc_context_owned)
                _soc_context.Free();
            return false;
        }
        if (!_tx_buffer.Alloc(k_tx_buffer_size))
        {
            _mic_buffer.Free();
            if (_soc_context_owned)
                _soc_context.Free();
            return false;
        }
        if (!_rx_buffer.Alloc(k_rx_buffer_size))
        {
            _tx_buffer.Free();
            _mic_buffer.Free();
            if (_soc_context_owned)
                _soc_context.Free();
            return false;
        }

        if (!_socket.Init(soc_ctx, soc_size))
        {
            Exit();
            return false;
        }

        if (R_FAILED(ncsndInit(false)))
        {
            _socket.Exit();
            Exit();
            return false;
        }

        if (R_FAILED(micInit((u8*)_mic_buffer.Ptr(), _mic_buffer.size)))
        {
            ncsndExit();
            _socket.Exit();
            Exit();
            return false;
        }

        _mic_last_offset = 0;
        return true;
    }

    void VoiceChat::Exit()
    {
        Stop();
        _socket.Exit();
        micExit();
        ncsndExit();
        _rx_buffer.Free();
        _tx_buffer.Free();
        _mic_buffer.Free();
        if (_soc_context_owned)
            _soc_context.Free();
    }

    bool VoiceChat::Start()
    {
        if (_running)
            return true;
        _running = true;
        _mic_last_offset = 0;
        return true;
    }

    void VoiceChat::Stop()
    {
        _running = false;
        MICU_StopSampling();
    }

    void VoiceChat::Update()
    {
        if (!_running)
            return;

        hidScanInput();
        u32 keys_held = hidKeysHeld();
        _ptt_pressed = (keys_held & KEY_R) != 0;

        UpdateMicSend();
        UpdateRecvPlay();
    }
}
