#include <CTRPluginFramework.hpp>
#include "PluginHooks.hpp"

namespace CTRPluginFramework
{
    int main(void)
    {
        PluginMenu *menu = new PluginMenu("3DS Voice Chat", 0, 0, 1,
            "UDP voice chat plugin.\nConnect = on/off, R = PTT, Set server IP = destination.");

        menu->SynchronizeWithFrame(true);
        InitMenu(*menu);
        menu->Run();

        delete menu;
        return 0;
    }
}
