#pragma once

#include "ProcessFinder.h"

#ifdef PLATFORM_WINDOWS

#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

namespace SaturationChanger {

class WinProcessFinder : public ProcessFinder {
public:
    WinProcessFinder() {
    }

    virtual bool isProcessRunning(std::string process_name) override;    
};

}

#endif