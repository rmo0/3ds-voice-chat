#ifndef VOICECHAT_HPP
#define VOICECHAT_HPP

#include <3ds.h>
#include "Helpers.hpp"
#include "Memory/SvcBuffer.hpp"
#include "Network/Socket.hpp"
#include "types.h"
#include "Unicode.h"

namespace CTRPluginFramework
{
    class VoiceChat
    {
    public:
        VoiceChat() : _running(false), _ptt_pressed(false) {}

        bool Init();
        void Exit();
        bool Start();
        void Stop();
        bool IsRunning() const { return _running; }
        void Update();

    private:
        bool _running;
        bool _ptt_pressed;

        SvcBuffer _soc_context;
        SvcBuffer _mic_buffer;
        SvcBuffer _tx_buffer;
        SvcBuffer _rx_buffer;

        Socket _socket;
        Handle _mic_event;
        u32 _mic_last_offset;

        void UpdateMicSend();
        void UpdateRecvPlay();
    };
}
#endif
