#ifndef OSD_MESSAGE_HPP
#define OSD_MESSAGE_HPP

#include <CTRPluginFramework.hpp>
#include <string>
#include "types.h"

namespace CTRPluginFramework
{
    namespace OSDMessage
    {
        void NotifySuccess(const std::string& prefix, const std::string& status);
        void NotifyError(const std::string& prefix, const std::string& detail);
        void NotifyInfo(const std::string& prefix, const std::string& detail, const Color& color);

        void SetPersistentStatus(const std::string& prefix, const std::string& status, const Color& color);
        void SetPersistentServerInfo(const std::string& ip, u16 port, u16 my_port);
        void ClearPersistentStatus();
        void ClearPersistentServerInfo();
    }
}

#endif
