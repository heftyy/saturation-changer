#include "AmdSaturationController.h"

namespace SaturationChanger {

AmdSaturationController::AmdSaturationController(int display_id) :
        i(0), j(0), iNumberAdapters(0), iAdapterIndex(0), iDisplayIndex(0),
        iNumDisplays(0), iColorCaps(0), iValidBits(0),
        iCurrent(0), iDefault(0), iMin(0), iMax(0), iStep(0) {

    initializeLibrary();

    currentSaturation = getSetting(display_id, ADL_DISPLAY_COLOR_SATURATION);
    currentBrightness = getSetting(display_id, ADL_DISPLAY_COLOR_BRIGHTNESS);
    currentContrast = getSetting(display_id, ADL_DISPLAY_COLOR_CONTRAST);    
}

AmdSaturationController::~AmdSaturationController() {
    ADL_Main_Memory_Free((void**) &lpAdapterInfo);
    ADL_Main_Memory_Free((void**) &lpAdlDisplayInfo);
    ADL_Main_Control_Destroy();

    if (hDLL != NULL) {
#ifdef PLATFORM_LINUX
        dlclose(hDLL);
#else
        FreeLibrary(hDLL);
#endif
    }
}

void AmdSaturationController::setGameSaturation(const Configuration& conf) {
    if (conf.game_saturation() != currentSaturation &&
        conf.game_brightness() != currentBrightness &&
        conf.game_contrast() != currentContrast) {

        currentSaturation = setSetting(conf.game_saturation(), conf.display_id(), ADL_DISPLAY_COLOR_SATURATION);
        currentBrightness = setSetting(conf.game_brightness(), conf.display_id(), ADL_DISPLAY_COLOR_BRIGHTNESS);
        currentContrast = setSetting(conf.game_contrast(), conf.display_id(), ADL_DISPLAY_COLOR_CONTRAST);
    }
}

void AmdSaturationController::setDesktopSaturation(const Configuration& conf) {
    if (conf.desktop_saturation() != currentSaturation &&
        conf.desktop_brightness() != currentBrightness &&
        conf.desktop_contrast() != currentContrast) {

        currentSaturation = setSetting(conf.desktop_saturation(), conf.display_id(), ADL_DISPLAY_COLOR_SATURATION);
        currentBrightness = setSetting(conf.desktop_brightness(), conf.display_id(), ADL_DISPLAY_COLOR_BRIGHTNESS);
        currentContrast = setSetting(conf.desktop_contrast(), conf.display_id(), ADL_DISPLAY_COLOR_CONTRAST);
    }
}


void AmdSaturationController::initializeLibrary() {
#ifdef PLATFORM_LINUX
    hDLL = dlopen("libatiadlxx.so", RTLD_LAZY | RTLD_GLOBAL);
#else
    hDLL = LoadLibraryA("atiadlxx.dll");
    if (hDLL == NULL)
        // A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
        // Try to load the 32 bit library (atiadlxy.dll) instead
        hDLL = LoadLibraryA("atiadlxy.dll");
#endif

    if (NULL == hDLL) {
        printf("ADL library not found!\n");
        return;
    }

    ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) GetProcAddress(hDLL, "ADL_Main_Control_Create");
    ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) GetProcAddress(hDLL, "ADL_Main_Control_Destroy");
    ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(hDLL,
                                                                                         "ADL_Adapter_NumberOfAdapters_Get");
    ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(hDLL, "ADL_Adapter_AdapterInfo_Get");
    ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET) GetProcAddress(hDLL, "ADL_Display_DisplayInfo_Get");
    ADL_Display_ColorCaps_Get = (ADL_DISPLAY_COLORCAPS_GET) GetProcAddress(hDLL, "ADL_Display_ColorCaps_Get");
    ADL_Display_Color_Get = (ADL_DISPLAY_COLOR_GET) GetProcAddress(hDLL, "ADL_Display_Color_Get");
    ADL_Display_Color_Set = (ADL_DISPLAY_COLOR_SET) GetProcAddress(hDLL, "ADL_Display_Color_Set");
    if (NULL == ADL_Main_Control_Create ||
        NULL == ADL_Main_Control_Destroy ||
        NULL == ADL_Adapter_NumberOfAdapters_Get ||
        NULL == ADL_Adapter_AdapterInfo_Get ||
        NULL == ADL_Display_DisplayInfo_Get ||
        NULL == ADL_Display_ColorCaps_Get ||
        NULL == ADL_Display_Color_Get ||
        NULL == ADL_Display_Color_Set) {
        printf("ADL's API is missing!\n");
        return;
    }

    // Initialize ADL. The second parameter is 1, which means:
    // retrieve adapter information only for adapters that are physically present and enabled in the system
    if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1)) {
        printf("ADL Initialization Error!\n");
        return;
    }

    // Obtain the number of adapters for the system
    if (ADL_OK != ADL_Adapter_NumberOfAdapters_Get(&iNumberAdapters)) {
        printf("Cannot get the number of adapters!\n");
        return;
    }

    if (0 < iNumberAdapters) {
        lpAdapterInfo = (LPAdapterInfo) malloc(sizeof(AdapterInfo) * iNumberAdapters);
        memset(lpAdapterInfo, '\0', sizeof(AdapterInfo) * iNumberAdapters);

        // Get the AdapterInfo structure for all adapters in the system
        ADL_Adapter_AdapterInfo_Get(lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);
    }
}

int AmdSaturationController::getSetting(int logical_display_id, int setting) {
    // Repeat for all available adapters in the system
    for (i = 0; i < iNumberAdapters; i++) {
        iAdapterIndex = lpAdapterInfo[i].iAdapterIndex;
        ADL_Main_Memory_Free((void**) &lpAdlDisplayInfo);
        if (ADL_OK !=
            ADL_Display_DisplayInfo_Get(lpAdapterInfo[i].iAdapterIndex, &iNumDisplays, &lpAdlDisplayInfo, 0))
            continue;

        for (j = 0; j < iNumDisplays; j++) {
            // For each display, check its status. Use the display only if it's connected AND mapped (iDisplayInfoValue: bit 0 and 1 )
            if ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) !=
                ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) &
                 lpAdlDisplayInfo[j].iDisplayInfoValue))
                continue;   // Skip the not connected or not mapped displays

            // Is the display mapped to this adapter? This test is too restrictive and may not be needed.
            if (iAdapterIndex != lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex)
                continue;

            if (lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex != logical_display_id)
                continue;

            iDisplayIndex = lpAdlDisplayInfo[j].displayID.iDisplayLogicalIndex;

            ADL_Display_ColorCaps_Get(iAdapterIndex, iDisplayIndex, &iColorCaps, &iValidBits);

            // Use only the valid bits from iColorCaps
            iColorCaps &= iValidBits;

            // Check if the display supports this particular capability
            if (setting & iColorCaps) {
                // Get the Current display Brightness, Default value, Min, Max and Step
                if (ADL_OK == ADL_Display_Color_Get(iAdapterIndex, iDisplayIndex, setting,
                                                    &iCurrent, &iDefault, &iMin, &iMax, &iStep)) {
                    return iCurrent;
                }
            }
        }
    }
    return -1;
}

int AmdSaturationController::setSetting(int value, int logical_display_id, int setting) {
    // Repeat for all available adapters in the system
    for (i = 0; i < iNumberAdapters; i++) {
        iAdapterIndex = lpAdapterInfo[i].iAdapterIndex;
        ADL_Main_Memory_Free((void**) &lpAdlDisplayInfo);
        if (ADL_OK !=
            ADL_Display_DisplayInfo_Get(lpAdapterInfo[i].iAdapterIndex, &iNumDisplays, &lpAdlDisplayInfo, 0))
            continue;

        for (j = 0; j < iNumDisplays; j++) {
            // For each display, check its status. Use the display only if it's connected AND mapped (iDisplayInfoValue: bit 0 and 1 )
            if ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) !=
                ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) &
                 lpAdlDisplayInfo[j].iDisplayInfoValue))
                continue;   // Skip the not connected or not mapped displays

            // Is the display mapped to this adapter? This test is too restrictive and may not be needed.
            if (iAdapterIndex != lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex)
                continue;

            if (lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex != logical_display_id)
                continue;

            iDisplayIndex = lpAdlDisplayInfo[j].displayID.iDisplayLogicalIndex;

            ADL_Display_ColorCaps_Get(iAdapterIndex, iDisplayIndex, &iColorCaps, &iValidBits);

            // Use only the valid bits from iColorCaps
            iColorCaps &= iValidBits;

            // Check if the display supports this particular capability
            if (setting & iColorCaps) {
                // Get the Current display Brightness, Default value, Min, Max and Step
                if (ADL_OK == ADL_Display_Color_Get(iAdapterIndex, iDisplayIndex, setting,
                                                    &iCurrent, &iDefault, &iMin, &iMax, &iStep)) {
                    if (iCurrent != value) {
                        ADL_Display_Color_Set(iAdapterIndex, iDisplayIndex, setting, value);
                        std::cout << "Changed saturation successfully to " << value << std::endl;
                        return value;
                    }
                    // Set half of the Min brightness for .5 sec
                }
            }
        }
    }
    return -1;
}

#ifdef PLATFORM_LINUX
void AmdSaturationController::Sleep(int time) {
    usleep(time * 1000);
}

void* AmdSaturationController::GetProcAddress(void* pLibrary, const char* name) {
    return dlsym(pLibrary, name);
}
#endif

}
