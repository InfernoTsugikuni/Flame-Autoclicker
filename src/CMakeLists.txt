cmake_minimum_required(VERSION 3.16)
project(FlameAutoclicker VERSION 0.1 LANGUAGES CXX)

# Enable Qt features
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt modules
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

# Source files - Include all files consistently
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    Content.cpp
    Content.h
    Functions.cpp
    Functions.h
    TitleBar.cpp
    TitleBar.h
    AutoClicker.h
    AutoClicker.cpp
    hotkeysettingstab.h
    hotkeysettingstab.cpp
)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Create main executable
qt_add_executable(FlameAutoclicker MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    hotkeysettingswindow.h
    hotkeysettingswindow.cpp
    assets/icon.rc
)

# Link Qt libraries using keyword signature for Qt6 compatibility
target_link_libraries(FlameAutoclicker PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

# Windows-specific configuration (Windows-only app)
if (WIN32)
    # Try to find Qt installation path
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    if(QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_WINDEPLOYQT_EXECUTABLE ${QT_QMAKE_EXECUTABLE} PATH)
        set(QT_WINDEPLOYQT_EXECUTABLE "${QT_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

        # Deploy Qt
        add_custom_command(TARGET FlameAutoclicker POST_BUILD
            COMMAND ${QT_WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:FlameAutoclicker>
            COMMENT "Deploying Qt libraries")
    endif()

    # Handle LLVM-MinGW runtime DLLs if using LLVM-MinGW
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        foreach(dll IN ITEMS libc++.dll libc++abi.dll libunwind.dll libwinpthread-1.dll)
            set(dll_path "${COMPILER_DIR}/${dll}")
            if (EXISTS "${dll_path}")
                add_custom_command(TARGET FlameAutoclicker POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${dll_path}"
                        "$<TARGET_FILE_DIR:FlameAutoclicker>"
                    COMMENT "Copying ${dll} to output directory"
                )
            else()
                message(STATUS "LLVM-MinGW runtime DLL not found: ${dll_path}")
            endif()
        endforeach()
    endif()
endif()

# Set target properties for Windows
set_target_properties(FlameAutoclicker PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Install rules for Windows
install(TARGETS FlameAutoclicker
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize for Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FlameAutoclicker)
endif()
