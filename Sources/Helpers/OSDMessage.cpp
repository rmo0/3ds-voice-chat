#include "Helpers/OSDMessage.hpp"
#include <CTRPluginFramework.hpp>
#include <cstdio>
#include <cstring>

namespace CTRPluginFramework
{
    namespace OSDMessage
    {
        namespace
        {
            static constexpr int k_display_frames = 180;
            static constexpr int k_char_width = 6;
            static constexpr int k_line_height = 14;
            static constexpr int k_pad = 4;
            static constexpr int k_label_width = 8;  /* ラベル幅を統一 */

            /* Bottom-left: Status (継続表示) */
            static constexpr int k_status_x = 0;
            static constexpr int k_status_y = 228;

            /* Top-left: IP, Port, My */
            static constexpr int k_server_x = 0;
            static constexpr int k_server_y = 2;

            struct PendingMessage
            {
                std::string prefix;
                std::string status;
                Color       statusColor;
                int         framesLeft;
                bool        active;
            } s_pending;

            struct PersistentMessage
            {
                std::string prefix;
                std::string status;
                Color       statusColor;
            } s_persistent = { "", "", Color::White };

            struct PersistentServerInfo
            {
                std::string ip;
                u16         port;
                u16         my_port;
                bool        active;
            } s_server_info = { "", 0, 0, false };

            static bool s_osd_registered = false;

            static void DrawLine(const Screen& screen, int pos_x, int pos_y, const char* label, const std::string& value, const Color& valueColor)
            {
                int val_w = static_cast<int>(value.size() * k_char_width);
                int total = k_pad + k_label_width * k_char_width + val_w + k_pad;
                screen.DrawRect(pos_x, pos_y - 2, total, 14, Color::Black);
                screen.DrawRect(pos_x + 1, pos_y - 1, total - 2, 12, Color(40, 40, 40), true);
                screen.Draw(label, pos_x + k_pad, pos_y, Color::White);
                screen.Draw(value, pos_x + k_pad + k_label_width * k_char_width, pos_y, valueColor);
            }

            static void DrawMessage(const Screen& screen, int pos_x, int pos_y, const std::string& prefix, const std::string& status, const Color& statusColor)
            {
                std::string label = prefix;
                if (label.size() < (size_t)k_label_width)
                    label.resize(k_label_width, ' ');
                else if (label.size() > (size_t)k_label_width)
                    label.resize(k_label_width);
                DrawLine(screen, pos_x, pos_y, label.c_str(), status, statusColor);
            }

            static void DrawServerInfo(const Screen& screen)
            {
                if (!s_server_info.active || (s_server_info.ip.empty() && s_server_info.port == 0 && s_server_info.my_port == 0))
                    return;
                char label[16];
                int y = k_server_y;
                if (!s_server_info.ip.empty())
                {
                    snprintf(label, sizeof(label), "%-*s", k_label_width, "IP:");
                    DrawLine(screen, k_server_x, y, label, s_server_info.ip, Color::Cyan);
                    y += k_line_height;
                }
                if (s_server_info.port != 0)
                {
                    char buf[16];
                    snprintf(buf, sizeof(buf), "%d", s_server_info.port);
                    snprintf(label, sizeof(label), "%-*s", k_label_width, "Port:");
                    DrawLine(screen, k_server_x, y, label, buf, Color::Cyan);
                    y += k_line_height;
                }
                if (s_server_info.my_port != 0)
                {
                    char buf[16];
                    snprintf(buf, sizeof(buf), "%d", s_server_info.my_port);
                    snprintf(label, sizeof(label), "%-*s", k_label_width, "My:");
                    DrawLine(screen, k_server_x, y, label, buf, Color::Cyan);
                }
            }

            static bool OSDCallback(const Screen& screen)
            {
                if (!screen.IsTop)
                    return false;

                bool drew = false;

                /* Top-left: IP, Port (継続表示) */
                if (s_server_info.active)
                {
                    DrawServerInfo(screen);
                    drew = true;
                }

                /* Bottom-left: 一時通知 or Status */
                if (s_pending.active && s_pending.framesLeft > 0)
                {
                    DrawMessage(screen, k_status_x, k_status_y, s_pending.prefix, s_pending.status, s_pending.statusColor);
                    s_pending.framesLeft--;
                    if (s_pending.framesLeft <= 0)
                        s_pending.active = false;
                    drew = true;
                }
                else if (!s_persistent.prefix.empty() || !s_persistent.status.empty())
                {
                    DrawMessage(screen, k_status_x, k_status_y, s_persistent.prefix, s_persistent.status, s_persistent.statusColor);
                    drew = true;
                }

                if (!drew && s_osd_registered)
                {
                    OSD::Stop(OSDCallback);
                    s_osd_registered = false;
                }
                return drew;
            }

            static void EnsureOSDRunning()
            {
                if (!s_osd_registered)
                {
                    OSD::Run(OSDCallback);
                    s_osd_registered = true;
                }
            }
        }

        void NotifySuccess(const std::string& prefix, const std::string& status)
        {
            EnsureOSDRunning();
            s_pending.prefix      = prefix.empty() ? prefix : prefix + " ";
            s_pending.status     = status;
            s_pending.statusColor = Color::Green;
            s_pending.framesLeft  = k_display_frames;
            s_pending.active      = true;
        }

        void NotifyError(const std::string& prefix, const std::string& detail)
        {
            EnsureOSDRunning();
            s_pending.prefix      = prefix.empty() ? prefix : prefix + " ";
            s_pending.status     = detail;
            s_pending.statusColor = Color::Red;
            s_pending.framesLeft  = k_display_frames;
            s_pending.active      = true;
        }

        void NotifyInfo(const std::string& prefix, const std::string& detail, const Color& color)
        {
            EnsureOSDRunning();
            s_pending.prefix      = prefix.empty() ? prefix : prefix + " ";
            s_pending.status     = detail;
            s_pending.statusColor = color;
            s_pending.framesLeft  = k_display_frames;
            s_pending.active      = true;
        }

        void SetPersistentStatus(const std::string& prefix, const std::string& status, const Color& color)
        {
            EnsureOSDRunning();
            s_persistent.prefix      = prefix.empty() ? prefix : prefix + " ";
            s_persistent.status     = status;
            s_persistent.statusColor = color;
        }

        void SetPersistentServerInfo(const std::string& ip, u16 port, u16 my_port)
        {
            EnsureOSDRunning();
            s_server_info.ip      = ip;
            s_server_info.port    = port;
            s_server_info.my_port = my_port;
            s_server_info.active  = true;
        }

        void ClearPersistentStatus()
        {
            s_persistent.prefix.clear();
            s_persistent.status.clear();
        }

        void ClearPersistentServerInfo()
        {
            s_server_info.active = false;
            s_server_info.ip.clear();
            s_server_info.port = 0;
            s_server_info.my_port = 0;
        }
    }
}
