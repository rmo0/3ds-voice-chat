#include "Network/Socket.hpp"
#include <3ds.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace CTRPluginFramework
{
    Socket::Socket() : _initialized(false), _fd(-1) {}

    Socket::~Socket()
    {
        Exit();
    }

    bool Socket::Init(void* context_addr, u32 context_size)
    {
        if (_initialized)
            return true;
        if (context_addr == nullptr || context_size == 0)
            return false;

        if (R_FAILED(socInit((u32*)context_addr, context_size)))
            return false;

        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_fd < 0)
        {
            socExit();
            return false;
        }

        struct sockaddr_in bind_addr;
        std::memset(&bind_addr, 0, sizeof(bind_addr));
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = INADDR_ANY;
        bind_addr.sin_port = 0;
        if (bind(_fd, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0)
        {
            close(_fd);
            _fd = -1;
            socExit();
            return false;
        }

        _initialized = true;
        return true;
    }

    void Socket::Exit()
    {
        if (!_initialized)
            return;
        if (_fd >= 0)
        {
            close(_fd);
            _fd = -1;
        }
        socExit();
        _initialized = false;
    }

    int Socket::SendTo(const void* data, u32 len, const struct sockaddr_in* dest)
    {
        if (!_initialized || _fd < 0 || data == nullptr || dest == nullptr)
            return -1;
        return sendto(_fd, data, len, 0, (const struct sockaddr*)dest, sizeof(struct sockaddr_in));
    }

    int Socket::RecvFrom(void* buf, u32 buf_len, struct sockaddr_in* src)
    {
        if (!_initialized || _fd < 0 || buf == nullptr)
            return -1;
        socklen_t src_len = src ? sizeof(struct sockaddr_in) : 0;
        return recvfrom(_fd, buf, buf_len, 0, src ? (struct sockaddr*)src : nullptr, src ? &src_len : nullptr);
    }
}
