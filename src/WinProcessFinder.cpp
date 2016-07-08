#include "WinProcessFinder.h"

#ifdef PLATFORM_WINDOWS

namespace SaturationChanger {

bool WinProcessFinder::isProcessRunning(std::string process_name) {
    bool process_found = false;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32 entry;

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            char DefChar = ' ';
            char iter_process_name[260];
//            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, iter_process_name, 260, &DefChar, NULL);

            if (stricmp(entry.szExeFile, process_name.c_str()) == 0)
            {
                process_found = true;
                break;
            }
        }
    }

    CloseHandle(snapshot);

    return process_found;
}

}

#endif