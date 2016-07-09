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

    bool isProcessRunning(std::string process_name) override;    
private:
    static std::string trim(const std::string& str);
};

}

#endif