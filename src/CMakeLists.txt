set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(config)
add_subdirectory(proto)

set(RESOURCE icons/icons.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

#QT5_WRAP_UI(UI_HEADERS
#        dialog/chatdialog.ui
#        )

set(SATURATION_CHANGER_SOURCE_FILES
        #${UI_HEADERS}
        main.cpp
        Window.cpp
        Window.h
        ProcessFinder.h
        WinProcessFinder.h
        LinuxProcessFinder.h
        SaturationController.h
        LinuxProcessFinder.cpp
        WinProcessFinder.cpp
        AmdSaturationController.cpp
        AmdSaturationController.h
        NvidiaSaturationController.cpp
        NvidiaSaturationController.h
        ProcessMonitor.cpp
        ProcessMonitor.h
        )

add_executable(saturation_changer WIN32 ${SATURATION_CHANGER_SOURCE_FILES} ${RESOURCE_ADDED})
target_link_libraries(saturation_changer ${QT_QTMAIN_LIBRARY} Qt5::Widgets proto)

IF(WIN32) # Check if we are on Windows
    if(MSVC) # Check if we are using the Visual Studio compiler
        set_target_properties(saturation_changer PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    else()
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC)")
    endif()
elseif(UNIX)
    # Nothing special required
else()
    message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()
