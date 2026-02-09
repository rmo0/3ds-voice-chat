#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "types.h"

struct sockaddr_in;

namespace CTRPluginFramework
{
    class Socket
    {
    public:
        Socket();
        ~Socket();

        bool Init(void* context_addr, u32 context_size);
        void Exit();

        bool IsOpen() const { return _initialized; }

        int SendTo(const void* data, u32 len, const struct sockaddr_in* dest);
        int RecvFrom(void* buf, u32 buf_len, struct sockaddr_in* src);

        int GetFd() const { return _fd; }

    private:
        bool _initialized;
        int _fd;
    };
}
#endif
