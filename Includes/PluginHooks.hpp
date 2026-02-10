#ifndef PLUGINHOOKS_HPP
#define PLUGINHOOKS_HPP

#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework
{
    void PatchProcess(FwkSettings &settings);
    void OnProcessExit(void);
    void InitMenu(PluginMenu &menu);
}

#endif
