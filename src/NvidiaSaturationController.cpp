#include "NvidiaSaturationController.h"

namespace SaturationChanger {

NvidiaSaturationController::NvidiaSaturationController(int display_id) : 
    NvAPI_QueryInterface(nullptr), NvAPI_Initialize(nullptr), NvAPI_Unload(nullptr), 
    NvAPI_EnumPhysicalGPUs(nullptr), NvAPI_GPU_GetUsages(nullptr), NvAPI_GPU_GetFullName(nullptr), 
    NvAPI_GPU_GetActiveOutputs(nullptr), NvAPI_GetDVCInfo(nullptr), NvAPI_GetDVCInfoEx(nullptr), 
    NvAPI_SetDVCLevel(nullptr), NvAPI_EnumNvidiaDisplayHandle(nullptr), NvAPI_GetInterfaceVersionString(nullptr), 
    NvAPI_GetErrorMessage(nullptr), NvAPI_GetAssociatedNvidiaDisplayHandle(nullptr), NvAPI_GPU_GetSystemType(nullptr) {

    initializeLibrary();    
    enumerateNvidiaDisplayHandle(display_id);
    currentDVC = getDVCLevel();
}

NvidiaSaturationController::~NvidiaSaturationController() {
    unloadLibrary();
}

void NvidiaSaturationController::setGameSaturation(const Configuration& conf) {    
    if(conf.display_id() != displayId) {        
        setDVCLevel(conf.desktop_saturation());

        enumerateNvidiaDisplayHandle(conf.display_id());
        displayId = conf.display_id();
        currentDVC = getDVCLevel();
    }

    if (currentDVC != conf.game_saturation()) {
        currentDVC = setDVCLevel(conf.game_saturation()) ? conf.game_saturation() : -255;
    }        
}

void NvidiaSaturationController::setDesktopSaturation(const Configuration& conf) {
    if (conf.display_id() != displayId) {
        setDVCLevel(conf.desktop_saturation());        

        enumerateNvidiaDisplayHandle(conf.display_id());
        displayId = conf.display_id();
        currentDVC = getDVCLevel();
    }

    if (currentDVC != conf.desktop_saturation()) {
        currentDVC = setDVCLevel(conf.desktop_saturation()) ? conf.desktop_saturation() : -255;
    }
}

bool NvidiaSaturationController::initializeLibrary() {
#ifdef _WIN64
    hDLL = LoadLibraryA("nvapi64.dll");
#else
    hDLL = LoadLibraryA("nvapi.dll");
#endif

    if (hDLL == NULL) {
        return false;
    }

    NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress(hDLL, "nvapi_QueryInterface");

    NvAPI_Initialize = (NvAPI_Initialize_t) (*NvAPI_QueryInterface)(0x0150E828);
    NvAPI_Unload = (NvAPI_Unload_t) (*NvAPI_QueryInterface)(0x0D22BDD7E);
    NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t) (*NvAPI_QueryInterface)(0xE5AC921F);
    NvAPI_GPU_GetFullName = (NvAPI_GPU_GetFullName_t) (*NvAPI_QueryInterface)(0xCEEE8E9F);
    NvAPI_GPU_GetActiveOutputs = (NvAPI_GPU_GetActiveOutputs_t) (*NvAPI_QueryInterface)(0x0E3E89B6F);
    NvAPI_GetDVCInfo = (NvAPI_GetDVCInfo_t) (*NvAPI_QueryInterface)(0x4085DE45);
    NvAPI_SetDVCLevel = (NvAPI_SetDVCLevel_t) (*NvAPI_QueryInterface)(0x172409B4);
    NvAPI_EnumNvidiaDisplayHandle = (NvAPI_EnumNvidiaDisplayHandle_t) (*NvAPI_QueryInterface)(0x9ABDD40D);
    NvAPI_GetInterfaceVersionString = (NvAPI_GetInterfaceVersionString_t) (*NvAPI_QueryInterface)(0x1053FA5);
    NvAPI_GetErrorMessage = (NvAPI_GetErrorMessage_t) (*NvAPI_QueryInterface)(0x6C2D048C);
    NvAPI_GetDVCInfoEx = (NvAPI_GetDVCInfoEx_t) (*NvAPI_QueryInterface)(0x0E45002D);
    NvAPI_GetAssociatedNvidiaDisplayHandle = (NvAPI_GetAssociatedNvidiaDisplayHandle_t) (*NvAPI_QueryInterface)(0x35C29134);
    NvAPI_GPU_GetSystemType = (NvAPI_GPU_GetSystemType_t) (*NvAPI_QueryInterface)(0xBAAABFCC);

    if (NvAPI_Initialize == NULL || NvAPI_Unload == NULL ||
        NvAPI_EnumPhysicalGPUs == NULL || NvAPI_GPU_GetFullName == NULL ||
        NvAPI_GPU_GetActiveOutputs == NULL || NvAPI_GetDVCInfo == NULL ||
        NvAPI_SetDVCLevel == NULL || NvAPI_EnumNvidiaDisplayHandle == NULL ||
        NvAPI_GetInterfaceVersionString == NULL || NvAPI_GetErrorMessage == NULL ||
        NvAPI_GetDVCInfoEx == NULL || NvAPI_GPU_GetSystemType == NULL) {
        return false;
    }

    int ret = (*NvAPI_Initialize)();
    if (ret == 0) {
        return true;
    }
    return false;
}

bool NvidiaSaturationController::unloadLibrary() const {
    int ret = (*NvAPI_Unload)();

#ifdef PLATFORM_LINUX
    dlclose(hDLL);
#else
    FreeLibrary(hDLL);
#endif

    if (ret == 0)
        return true;
    return false;
}

bool NvidiaSaturationController::setDVCLevel(int level) const {
    _NvAPI_Status status = (_NvAPI_Status) (*NvAPI_SetDVCLevel)(displayHandle, 0, level);
    if (status != NVAPI_OK) {
        return false;
    }
    return true;
}

int NvidiaSaturationController::getDVCLevel() const {
    NV_DISPLAY_DVC_INFO info;

    info.version = sizeof(NV_DISPLAY_DVC_INFO) | 0x10000;
    _NvAPI_Status status = (_NvAPI_Status) (*NvAPI_GetDVCInfo)(displayHandle, 0, &info);

    if (status != NVAPI_OK) {
        return -1;
    }
    return info.currentLevel;
}

int NvidiaSaturationController::enumerateNvidiaDisplayHandle(int index) {
    _NvAPI_Status status = (_NvAPI_Status) (*NvAPI_EnumNvidiaDisplayHandle)(index, &displayHandle);
    if (status != 0 && status == NVAPI_END_ENUMERATION) {
        return -1;
    }
    return displayHandle;
}

}
