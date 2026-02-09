#include "VoiceChat/VoiceChat.hpp"
#include <3ds.h>
#include "csvc.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace CTRPluginFramework
{
    namespace
    {
        const u32 k_tx_buffer_size = 0x800;
    }

    void VoiceChat::UpdateMicSend()
    {
        if (!_ptt_pressed)
        {
            bool sampling = false;
            MICU_IsSampling(&sampling);
            if (sampling)
                MICU_StopSampling();
            return;
        }

        u32 data_size = micGetSampleDataSize();
        if (data_size == 0)
            return;

        bool sampling = false;
        if (R_FAILED(MICU_IsSampling(&sampling)) || !sampling)
        {
            MICU_StartSampling(MICU_ENCODING_PCM8_SIGNED, MICU_SAMPLE_RATE_16360, 0, data_size - 4, true);
            return;
        }

        u32 last_offset = micGetLastSampleOffset();
        u8* mic_ptr = (u8*)_mic_buffer.Ptr();
        if (!mic_ptr || _tx_buffer.Empty())
            return;

        u32 to_send = last_offset >= _mic_last_offset
            ? (last_offset - _mic_last_offset)
            : (data_size - _mic_last_offset + last_offset);
        if (to_send > k_tx_buffer_size)
            to_send = k_tx_buffer_size;
        if (to_send == 0)
            return;

        u32 copy_start = _mic_last_offset;
        _mic_last_offset = last_offset;
        std::memcpy(_tx_buffer.Ptr(), mic_ptr + copy_start, to_send);
        svcFlushDataCacheRange(_tx_buffer.Ptr(), to_send);

        struct sockaddr_in dest;
        std::memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(0x1234);
        dest.sin_addr.s_addr = INADDR_BROADCAST;
        _socket.SendTo(_tx_buffer.Ptr(), to_send, &dest);
    }
}
