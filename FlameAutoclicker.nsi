; FlameAutoclicker NSIS Installer Script

!define APP_NAME "FlameAutoclicker"
!define VERSION "1.0"
!define COMPANY "InferSoftware"
!define INSTALL_DIR "$PROGRAMFILES64\${APP_NAME}"

Outfile "FlameAutoclickerInstaller.exe"

InstallDir "${INSTALL_DIR}"

RequestExecutionLevel admin

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
    SetOutPath "$INSTDIR"

    ; Main app
    File "build\FlameAutoclicker.exe"

    ; Assets (optional)
    SetOutPath "$INSTDIR\assets"
    File /r "build\assets\*.*"

    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    ; Start menu shortcut (optional)
    CreateShortCut "$SMPROGRAMS\${APP_NAME}.lnk" "$INSTDIR\FlameAutoclicker.exe"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\FlameAutoclicker.exe"
    Delete "$INSTDIR\Uninstall.exe"
    Delete "$SMPROGRAMS\${APP_NAME}.lnk"

    RMDir /r "$INSTDIR\assets"
    RMDir "$INSTDIR"
SectionEnd
