#pragma once

#include <iostream>
#include <config/SaturationChangerConfig.h>
#include "SaturationController.h"

namespace SaturationChanger {

#ifdef PLATFORM_LINUX

#include "../adl/adl_sdk.h"
#include <dlfcn.h>	//dyopen, dlsym, dlclose
#include <stdlib.h>
#include <string.h>	//memeset
#include <unistd.h>	//sleep

#else
#include <windows.h>
#include <tchar.h>
#include "../adl/adl_sdk.h"
#endif

#include <stdio.h>

// Definitions of the used function pointers. Add more if you use other ADL APIs
typedef int(* ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
typedef int(* ADL_MAIN_CONTROL_DESTROY)();
typedef int(* ADL_ADAPTER_NUMBEROFADAPTERS_GET)(int*);
typedef int(* ADL_ADAPTER_ADAPTERINFO_GET)(LPAdapterInfo, int);
typedef int(* ADL_DISPLAY_COLORCAPS_GET)(int, int, int*, int*);
typedef int(* ADL_DISPLAY_COLOR_GET)(int, int, int, int*, int*, int*, int*, int*);
typedef int(* ADL_DISPLAY_COLOR_SET)(int, int, int, int);
typedef int(* ADL_DISPLAY_DISPLAYINFO_GET)(int, int*, ADLDisplayInfo**, int);

class AmdSaturationController : public SaturationController {
public:
    explicit AmdSaturationController(int display_id);
    ~AmdSaturationController();

    void setGameSaturation(const Configuration& conf) override;
    void setDesktopSaturation(const Configuration& conf) override;

private:
    int currentSaturation;
    int currentBrightness;
    int currentContrast;

#ifdef PLATFORM_LINUX
    void* hDLL;        // Handle to .so library
#else
    HINSTANCE hDLL;		// Handle to DLL
#endif

    ADL_MAIN_CONTROL_CREATE ADL_Main_Control_Create;
    ADL_MAIN_CONTROL_DESTROY ADL_Main_Control_Destroy;
    ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get;
    ADL_ADAPTER_ADAPTERINFO_GET ADL_Adapter_AdapterInfo_Get;
    ADL_DISPLAY_COLORCAPS_GET ADL_Display_ColorCaps_Get;
    ADL_DISPLAY_COLOR_GET ADL_Display_Color_Get;
    ADL_DISPLAY_COLOR_SET ADL_Display_Color_Set;
    ADL_DISPLAY_DISPLAYINFO_GET ADL_Display_DisplayInfo_Get;

    LPAdapterInfo lpAdapterInfo = NULL;
    LPADLDisplayInfo lpAdlDisplayInfo = NULL;
    int i, j;
    int iNumberAdapters;
    int iAdapterIndex;
    int iDisplayIndex;
    int iNumDisplays;
    int iColorCaps, iValidBits;
    int iCurrent, iDefault, iMin, iMax, iStep;

    // Memory allocation function
    static void* __stdcall ADL_Main_Memory_Alloc(int iSize) {
        void* lpBuffer = malloc(iSize);
        return lpBuffer;
    }

    // Optional Memory de-allocation function
    static void __stdcall ADL_Main_Memory_Free(void** lpBuffer) {
        if (NULL != *lpBuffer) {
            free(*lpBuffer);
            *lpBuffer = NULL;
        }
    }

#ifdef PLATFORM_LINUX
    void* GetProcAddress(void* pLibrary, const char* name);
    void Sleep(int time);
#endif

    void initializeLibrary();
    int getSetting(int logical_display_id, int setting);
    int setSetting(int value, int logical_display_id, int setting);
};

}
