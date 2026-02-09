#include "VoiceChat/VoiceChat.hpp"
#include "ncsnd.h"
#include <3ds.h>
#include "csvc.h"

namespace CTRPluginFramework
{
    void VoiceChat::UpdateRecvPlay()
    {
        if (_rx_buffer.Empty())
            return;

        struct sockaddr_in from;
        int recv_len = _socket.RecvFrom(_rx_buffer.Ptr(), _rx_buffer.size, &from);
        if (recv_len <= 0)
            return;

        svcFlushDataCacheRange(_rx_buffer.Ptr(), (u32)recv_len);

        ncsndDirectSound sound;
        ncsndInitializeDirectSound(&sound);
        sound.channelData.channelAmount = 1;
        sound.channelData.channelEncoding = NCSND_ENCODING_PCM8;
        sound.channelData.sampleRate = 16360;
        sound.channelData.leftSampleData = _rx_buffer.Ptr();
        sound.channelData.rightSampleData = nullptr;
        sound.channelData.sampleDataLength = (u32)recv_len;
        ncsndPlayDirectSound(0, 0, &sound);
    }
}
