﻿#pragma once

#include <config/SaturationChangerConfig.h>
#include "SaturationController.h"

namespace SaturationChanger {

#ifdef PLATFORM_LINUX
#include "../adl/adl_sdk.h"
#include <dlfcn.h>	//dyopen, dlsym, dlclose
#include <stdlib.h>
#include <string.h>	//memeset
#include <unistd.h>	//sleep
#endif

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#define NVAPI_MAX_PHYSICAL_GPUS		64
#define NVAPI_MAX_USAGES_PER_GPU	34
#define NVAPI_MAX_LEVEL				63
#define NVAPI_DEFAULT_LEVEL			0

typedef struct {
    unsigned int version;
    int currentLevel;
    int minLevel;
    int maxLevel;
} NV_DISPLAY_DVC_INFO;

typedef struct {
    int version;
    int drvVersion;
    int bldChangeListNum;
    char szBuildBranchString[64];
    char szAdapterString[64];
} NV_DISPLAY_DRIVER_VERSION;

typedef enum {
    NV_SYSTEM_TYPE_UNKNOWN = 0,
    NV_SYSTEM_TYPE_LAPTOP = 1,
    NV_SYSTEM_TYPE_DESKTOP = 2,

} NV_SYSTEM_TYPE;

enum _NvAPI_Status {
    NVAPI_OK = 0,
    NVAPI_ERROR = -1,
    NVAPI_LIBRARY_NOT_FOUND = -2,
    NVAPI_NO_IMPLEMENTATION = -3,
    NVAPI_API_NOT_INITIALIZED = -4,
    NVAPI_INVALID_ARGUMENT = -5,
    NVAPI_NVIDIA_DEVICE_NOT_FOUND = -6,
    NVAPI_END_ENUMERATION = -7,
    NVAPI_INVALID_HANDLE = -8,
    NVAPI_INCOMPATIBLE_STRUCT_VERSION = -9,
    NVAPI_HANDLE_INVALIDATED = -10,
    NVAPI_OPENGL_CONTEXT_NOT_CURRENT = -11,
    NVAPI_INVALID_POINTER = -14,
    NVAPI_NO_GL_EXPERT = -12,
    NVAPI_INSTRUMENTATION_DISABLED = -13,
    NVAPI_NO_GL_NSIGHT = -15,
    NVAPI_EXPECTED_LOGICAL_GPU_HANDLE = -100,
    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE = -101,
    NVAPI_EXPECTED_DISPLAY_HANDLE = -102,
    NVAPI_INVALID_COMBINATION = -103,
    NVAPI_NOT_SUPPORTED = -104,
    NVAPI_PORTID_NOT_FOUND = -105,
    NVAPI_EXPECTED_UNATTACHED_DISPLAY_HANDLE = -106,
    NVAPI_INVALID_PERF_LEVEL = -107,
    NVAPI_DEVICE_BUSY = -108,
    NVAPI_NV_PERSIST_FILE_NOT_FOUND = -109,
    NVAPI_PERSIST_DATA_NOT_FOUND = -110,
    NVAPI_EXPECTED_TV_DISPLAY = -111,
    NVAPI_EXPECTED_TV_DISPLAY_ON_DCONNECTOR = -112,
    NVAPI_NO_ACTIVE_SLI_TOPOLOGY = -113,
    NVAPI_SLI_RENDERING_MODE_NOTALLOWED = -114,
    NVAPI_EXPECTED_DIGITAL_FLAT_PANEL = -115,
    NVAPI_ARGUMENT_EXCEED_MAX_SIZE = -116,
    NVAPI_DEVICE_SWITCHING_NOT_ALLOWED = -117,
    NVAPI_TESTING_CLOCKS_NOT_SUPPORTED = -118,
    NVAPI_UNKNOWN_UNDERSCAN_CONFIG = -119,
    NVAPI_TIMEOUT_RECONFIGURING_GPU_TOPO = -120,
    NVAPI_DATA_NOT_FOUND = -121,
    NVAPI_EXPECTED_ANALOG_DISPLAY = -122,
    NVAPI_NO_VIDLINK = -123,
    NVAPI_REQUIRES_REBOOT = -124,
    NVAPI_INVALID_HYBRID_MODE = -125,
    NVAPI_MIXED_TARGET_TYPES = -126,
    NVAPI_SYSWOW64_NOT_SUPPORTED = -127,
    NVAPI_IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED = -128,
    NVAPI_REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS = -129,
    NVAPI_OUT_OF_MEMORY = -130,
    NVAPI_WAS_STILL_DRAWING = -131,
    NVAPI_FILE_NOT_FOUND = -132,
    NVAPI_TOO_MANY_UNIQUE_STATE_OBJECTS = -133,
    NVAPI_INVALID_CALL = -134,
    NVAPI_D3D10_1_LIBRARY_NOT_FOUND = -135,
    NVAPI_FUNCTION_NOT_FOUND = -136,
    NVAPI_INVALID_USER_PRIVILEGE = -137,
    NVAPI_EXPECTED_NON_PRIMARY_DISPLAY_HANDLE = -138,
    NVAPI_EXPECTED_COMPUTE_GPU_HANDLE = -139,
    NVAPI_STEREO_NOT_INITIALIZED = -140,
    NVAPI_STEREO_REGISTRY_ACCESS_FAILED = -141,
    NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED = -142,
    NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED = -143,
    NVAPI_STEREO_NOT_ENABLED = -144,
    NVAPI_STEREO_NOT_TURNED_ON = -145,
    NVAPI_STEREO_INVALID_DEVICE_INTERFACE = -146,
    NVAPI_STEREO_PARAMETER_OUT_OF_RANGE = -147,
    NVAPI_STEREO_FRUSTUM_ADJUST_MODE_NOT_SUPPORTED = -148,
    NVAPI_TOPO_NOT_POSSIBLE = -149,
    NVAPI_MODE_CHANGE_FAILED = -150,
    NVAPI_D3D11_LIBRARY_NOT_FOUND = -151,
    NVAPI_INVALID_ADDRESS = -152,
    NVAPI_STRING_TOO_SMALL = -153,
    NVAPI_MATCHING_DEVICE_NOT_FOUND = -154,
    NVAPI_DRIVER_RUNNING = -155,
    NVAPI_DRIVER_NOTRUNNING = -156,
    NVAPI_ERROR_DRIVER_RELOAD_REQUIRED = -157,
    NVAPI_SET_NOT_ALLOWED = -158,
    NVAPI_ADVANCED_DISPLAY_TOPOLOGY_REQUIRED = -159,
    NVAPI_SETTING_NOT_FOUND = -160,
    NVAPI_SETTING_SIZE_TOO_LARGE = -161,
    NVAPI_TOO_MANY_SETTINGS_IN_PROFILE = -162,
    NVAPI_PROFILE_NOT_FOUND = -163,
    NVAPI_PROFILE_NAME_IN_USE = -164,
    NVAPI_PROFILE_NAME_EMPTY = -165,
    NVAPI_EXECUTABLE_NOT_FOUND = -166,
    NVAPI_EXECUTABLE_ALREADY_IN_USE = -167,
    NVAPI_DATATYPE_MISMATCH = -168,
    NVAPI_PROFILE_REMOVED = -169,
    NVAPI_UNREGISTERED_RESOURCE = -170,
    NVAPI_ID_OUT_OF_RANGE = -171,
    NVAPI_DISPLAYCONFIG_VALIDATION_FAILED = -172,
    NVAPI_DPMST_CHANGED = -173,
    NVAPI_INSUFFICIENT_BUFFER = -174,
    NVAPI_ACCESS_DENIED = -175,
    NVAPI_MOSAIC_NOT_ACTIVE = -176,
    NVAPI_SHARE_RESOURCE_RELOCATED = -177,
    NVAPI_REQUEST_USER_TO_DISABLE_DWM = -178,
    NVAPI_D3D_DEVICE_LOST = -179,
    NVAPI_INVALID_CONFIGURATION = -180,
    NVAPI_STEREO_HANDSHAKE_NOT_DONE = -181,
    NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS = -182,
    NVAPI_DEFAULT_STEREO_PROFILE_IS_NOT_DEFINED = -183,
    NVAPI_DEFAULT_STEREO_PROFILE_DOES_NOT_EXIST = -184,
    NVAPI_CLUSTER_ALREADY_EXISTS = -185,
    NVAPI_DPMST_DISPLAY_ID_EXPECTED = -186,
    NVAPI_INVALID_DISPLAY_ID = -187,
    NVAPI_STREAM_IS_OUT_OF_SYNC = -188,
    NVAPI_INCOMPATIBLE_AUDIO_DRIVER = -189,
    NVAPI_VALUE_ALREADY_SET = -190,
    NVAPI_TIMEOUT = -191,
    NVAPI_GPU_WORKSTATION_FEATURE_INCOMPLETE = -192,
    NVAPI_STEREO_INIT_ACTIVATION_NOT_DONE = -193,
    NVAPI_SYNC_NOT_ACTIVE = -194,
    NVAPI_SYNC_MASTER_NOT_FOUND = -195,
    NVAPI_INVALID_SYNC_TOPOLOGY = -196
};

typedef int*(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef int (*NvAPI_Initialize_t)();
typedef int (*NvAPI_Unload_t)();
typedef int (*NvAPI_EnumPhysicalGPUs_t)(int** handles, int* count);
typedef int (*NvAPI_GPU_GetUsages_t)(int* handle, unsigned int* usages);
typedef int (*NvAPI_GetDVCInfo_t)(int hNvDisplay, int outputId, NV_DISPLAY_DVC_INFO* pDVCInfo);
typedef int (*NvAPI_GPU_GetFullName_t)(int* handle, char szName[64]);
typedef int (*NvAPI_GPU_GetActiveOutputs_t)(int* handle, int* pOutputsMask);
typedef int (*NvAPI_SetDVCLevel_t)(int handle, int outputId, int level);
typedef int (*NvAPI_EnumNvidiaDisplayHandle_t)(int thisEnum, int* pNvDispHandle);
typedef int (*NvAPI_GetInterfaceVersionString_t)(char szVersion[64]);
typedef int (*NvAPI_GetErrorMessage_t)(_NvAPI_Status nr, char szDesc[64]);
typedef int (*NvAPI_GetDVCInfoEx_t)(int hNvDisplay, int outputId, NV_DISPLAY_DVC_INFO* pDVCInfo);
typedef int (*NvAPI_GetAssociatedNvidiaDisplayHandle_t)(const char* szDisplayName, int* pNvDispHandle);
typedef int (*NvAPI_GPU_GetSystemType_t)(int* hPhysicalGpu, NV_SYSTEM_TYPE* pSystemType);

#define NV_DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
NV_DECLARE_HANDLE(NvDisplayHandle);

class NvidiaSaturationController : public SaturationController {
public:
    NvidiaSaturationController(int display_id);

    ~NvidiaSaturationController() override {
    }

    void setGameSaturation(const Configuration& conf) override;
    void setDesktopSaturation(const Configuration& conf) override;

private:
    int currentDVC;
    int displayHandle;
    int displayId;

    NvAPI_QueryInterface_t NvAPI_QueryInterface;
    NvAPI_Initialize_t NvAPI_Initialize;
    NvAPI_Unload_t NvAPI_Unload;
    NvAPI_EnumPhysicalGPUs_t NvAPI_EnumPhysicalGPUs;
    NvAPI_GPU_GetUsages_t NvAPI_GPU_GetUsages;
    NvAPI_GPU_GetFullName_t NvAPI_GPU_GetFullName;
    NvAPI_GPU_GetActiveOutputs_t NvAPI_GPU_GetActiveOutputs;
    NvAPI_GetDVCInfo_t NvAPI_GetDVCInfo;
    NvAPI_GetDVCInfoEx_t NvAPI_GetDVCInfoEx;
    NvAPI_SetDVCLevel_t NvAPI_SetDVCLevel;
    NvAPI_EnumNvidiaDisplayHandle_t NvAPI_EnumNvidiaDisplayHandle;
    NvAPI_GetInterfaceVersionString_t NvAPI_GetInterfaceVersionString;
    NvAPI_GetErrorMessage_t NvAPI_GetErrorMessage;
    NvAPI_GetAssociatedNvidiaDisplayHandle_t NvAPI_GetAssociatedNvidiaDisplayHandle;
    NvAPI_GPU_GetSystemType_t NvAPI_GPU_GetSystemType;

    bool initializeLibrary();
    bool unloadLibrary() const;

    bool setDVCLevel(int level) const;
    int getDVCLevel() const;
    int enumerateNvidiaDisplayHandle(int index);
};

}
