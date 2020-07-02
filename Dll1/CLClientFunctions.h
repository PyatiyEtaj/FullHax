#pragma once
#include "JustOneHeader.h"
#include "OffsEnum.h"
#include "HelpfulFuncs.h"

namespace CLClientFuncs {
    typedef bool (*isLobbyLaunched)(const char*);
    DWORD cfModule;
    isLobbyLaunched p_isLobbyLauncheed;

    isLobbyLaunched Init(const std::vector<int>& offs) {
        cfModule = (DWORD)GetModuleHandleA("crossfire.exe");
        p_isLobbyLauncheed = isLobbyLaunched((PBYTE)(cfModule + 0x290580 + 4096));
        return p_isLobbyLauncheed;
    }
}