#include "WinProcessFinder.h"
#include <iostream>
#include <algorithm>

#ifdef PLATFORM_WINDOWS

namespace SaturationChanger {

bool WinProcessFinder::isProcessRunning(std::string process_name) {
    /*
    HWND test = FindWindow(nullptr, process_name.c_str());
    if (!test) { // Process is not running
        return false;
    }
    return true;
    */

    DWORD pid = 0;
    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process)) {
        do {
            process_name = trim(process_name);
            std::string current_exe = trim(process.szExeFile);
            std::transform(process_name.begin(), process_name.end(), process_name.begin(), ::tolower);
            std::transform(current_exe.begin(), current_exe.end(), current_exe.begin(), ::tolower);

            if (process_name.compare(current_exe) == 0) {
                pid = process.th32ProcessID;
                return true;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    return false;
}

std::string WinProcessFinder::trim(const std::string& str) {
    if (str.length() > 0) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }    
    return str;
}

}

#endif