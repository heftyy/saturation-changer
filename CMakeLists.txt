cmake_minimum_required(VERSION 3.2)
project(saturation_changer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# The project version number.
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

set(PLATFORM_WINDOWS 0)
set(PLATFORM_LINUX 0)

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    set(PLATFORM_WINDOWS 1)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long")
    set(PLATFORM_LINUX 1)
endif()

if(UNIX)
    add_definitions(-std=c++14)
endif(UNIX)

find_package(Qt5Widgets REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if(WIN32)
    add_custom_target(Qt5CopyBinaries ALL
            # todo: check if debug and release folder exist
            # debug version
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms

            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/libEGLd.dll                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug

            # release version
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Core.dll                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/libEGL.dll                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release

            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../plugins/platforms/qwindows.dll     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms
            COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../plugins/platforms/qwindowsd.dll    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms

            # Output Message
            COMMENT "Copying Qt binaries from '${Qt5Core_DIR}/../../../bin/' to '${CMAKE_BINARY_DIR}'" VERBATIM
            )
endif(WIN32)