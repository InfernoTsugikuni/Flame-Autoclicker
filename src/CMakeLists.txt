cmake_minimum_required(VERSION 3.16)
project(FlameAutoclicker VERSION 0.1 LANGUAGES CXX)

# -------------------------
# Qt Build Settings
# -------------------------
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Output Paths
# -------------------------
# All build output goes to build/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -------------------------
# Qt Modules
# -------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

# -------------------------
# Sources
# -------------------------
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    Content.cpp
    Content.h
    Functions.cpp
    Functions.h
    AutoClicker.h
    AutoClicker.cpp
    hotkeysettingstab.h
    hotkeysettingstab.cpp
    hotkeysettingswindow.h
    hotkeysettingswindow.cpp
    QtBlaze.h
    resources/icon.rc
    assets/assets.qrc
)

# -------------------------
# Copy Assets to build/bin
# -------------------------
file(COPY ${CMAKE_SOURCE_DIR}/assets
     DESTINATION ${CMAKE_BINARY_DIR}/bin)

# -------------------------
# Executable
# -------------------------
if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(FlameAutoclicker
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(FlameAutoclicker ${PROJECT_SOURCES})
endif()

target_link_libraries(FlameAutoclicker PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

set_target_properties(FlameAutoclicker PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# -------------------------
# Windows Deployment
# -------------------------
if (WIN32)
    # Locate windeployqt
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    if (QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_WINDEPLOYQT_PATH ${QT_QMAKE_EXECUTABLE} PATH)
        set(QT_WINDEPLOYQT "${QT_WINDEPLOYQT_PATH}/windeployqt.exe")

        # Deploy Qt DLLs into build/bin
        add_custom_command(TARGET FlameAutoclicker POST_BUILD
            COMMAND "${QT_WINDEPLOYQT}"
                --dir "$<TARGET_FILE_DIR:FlameAutoclicker>"
                "$<TARGET_FILE:FlameAutoclicker>"
            COMMENT "Running windeployqt..."
        )
    endif()

    # Copy LLVM-MinGW runtime libs if applicable
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        foreach(dll IN ITEMS libc++.dll libc++abi.dll libunwind.dll libwinpthread-1.dll)
            set(dll_path "${COMPILER_DIR}/${dll}")
            if (EXISTS "${dll_path}")
                add_custom_command(TARGET FlameAutoclicker POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${dll_path}"
                        "$<TARGET_FILE_DIR:FlameAutoclicker>"
                    COMMENT "Copying ${dll}..."
                )
            endif()
        endforeach()
    endif()
endif()

# -------------------------
# Install (Packaging Output)
# -------------------------
# Output goes to build/install/
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

install(TARGETS FlameAutoclicker
    RUNTIME DESTINATION .
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION .
)

# -------------------------
# Qt Finalization
# -------------------------
if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FlameAutoclicker)
endif()
