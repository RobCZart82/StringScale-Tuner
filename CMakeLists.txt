cmake_minimum_required(VERSION 3.21)
project(StringScaleTuner VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
add_library(strings_core STATIC src/pitch_detector.cpp src/tuning.cpp)
target_include_directories(strings_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
if(MSVC)
    target_compile_options(strings_core PRIVATE /W4)
else()
    target_compile_options(strings_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(strings_core_tests tests/pitch_detector_test.cpp)
    target_link_libraries(strings_core_tests PRIVATE strings_core)
    add_test(NAME strings_core_tests COMMAND strings_core_tests)
endif()

option(STRINGS_BUILD_GUI "Build the Qt 6 desktop application" ON)
if(STRINGS_BUILD_GUI)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
    qt_standard_project_setup()
    qt_add_executable(StringScaleTuner
        MACOSX_BUNDLE
        src/main.cpp
        src/main_window.hpp src/main_window.cpp
        src/audio_capture.hpp src/audio_capture.cpp
        src/tuner_widgets.hpp src/tuner_widgets.cpp
    )
    target_link_libraries(StringScaleTuner PRIVATE strings_core
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)
    set_target_properties(StringScaleTuner PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.robczart.stringscaletuner"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_NAME "StringScale Tuner"
    )
    install(TARGETS StringScaleTuner BUNDLE DESTINATION . RUNTIME DESTINATION bin)
    if(WIN32)
        include(InstallRequiredSystemLibraries)
    endif()
    if(WIN32 OR APPLE)
        qt_generate_deploy_app_script(
            TARGET StringScaleTuner
            OUTPUT_SCRIPT deploy_script
        )
        install(SCRIPT ${deploy_script})
    endif()
    install(FILES LICENSE DESTINATION .)
endif()
