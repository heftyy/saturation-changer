#pragma once

#include "ProcessFinder.h"

#ifdef PLATFORM_WINDOWS

#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

class WinProcessFinder : public ProcessFinder {

public:
    virtual bool isProcessRunning(std::string process_name) override;

private:
    PROCESSENTRY32 entry;
    HANDLE snapshot;
};

#endif