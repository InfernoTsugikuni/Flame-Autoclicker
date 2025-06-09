; NSIS Script for FlameAutoclicker
!define PRODUCT_NAME "FlameAutoclicker"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "Your Company"
!define PRODUCT_WEB_SITE "http://www.yourwebsite.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\FlameAutoclicker.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Modern UI
!include "MUI2.nsh"

; General
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "FlameAutoclicker_Setup.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

; Compression settings for smaller installer
SetCompressor /SOLID lzma
SetCompressorDictSize 32

; Request application privileges
RequestExecutionLevel admin

; Interface Settings
!define MUI_ABORTWARNING

; Pages
!insertmacro MUI_PAGE_WELCOME
; !insertmacro MUI_PAGE_LICENSE "license.txt"  ; Commented out - create license.txt if needed
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_INSTFILES

; Languages
!insertmacro MUI_LANGUAGE "English"

; Installer Sections
Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Main executable
  File "build\FlameAutoclicker.exe"
  
  ; Qt Core Libraries
  File "build\Qt6Core.dll"
  File "build\Qt6Gui.dll"
  File "build\Qt6Network.dll"
  File "build\Qt6Pdf.dll"
  File "build\Qt6Svg.dll"
  File "build\Qt6Widgets.dll"
  
  ; Runtime Libraries
  File "build\libc++.dll"
  File "build\libgcc_s_seh-1.dll"
  File "build\libstdc++-6.dll"
  File "build\libunwind.dll"
  File "build\libwinpthread-1.dll"
  
  ; DirectX and OpenGL
  File "build\D3Dcompiler_47.dll"
  File "build\opengl32sw.dll"
  
  ; Qt Plugins and Resources
  SetOutPath "$INSTDIR\assets"
  File /r "build\assets\*.*"
  
  SetOutPath "$INSTDIR\generic"
  File /r "build\generic\*.*"
  
  SetOutPath "$INSTDIR\iconengines"
  File /r "build\iconengines\*.*"
  
  SetOutPath "$INSTDIR\imageformats"
  File /r "build\imageformats\*.*"
  
  SetOutPath "$INSTDIR\networkinformation"
  File /r "build\networkinformation\*.*"
  
  SetOutPath "$INSTDIR\platforms"
  File /r "build\platforms\*.*"
  
  SetOutPath "$INSTDIR\styles"
  File /r "build\styles\*.*"
  
  SetOutPath "$INSTDIR\tls"
  File /r "build\tls\*.*"
  
  SetOutPath "$INSTDIR\translations"
  File /r "build\translations\*.*"
  
  SetOutPath "$INSTDIR"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\FlameAutoclicker.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\FlameAutoclicker.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\FlameAutoclicker.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\FlameAutoclicker.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Finish page function - Ask user if they want to start the application
Function .onInstSuccess
  MessageBox MB_YESNO "Installation completed successfully. Would you like to start FlameAutoclicker now?" IDNO NoLaunch
    Exec "$INSTDIR\FlameAutoclicker.exe"
  NoLaunch:
FunctionEnd

; Uninstaller
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\FlameAutoclicker.exe"
  Delete "$INSTDIR\Qt6Core.dll"
  Delete "$INSTDIR\Qt6Gui.dll"
  Delete "$INSTDIR\Qt6Network.dll"
  Delete "$INSTDIR\Qt6Pdf.dll"
  Delete "$INSTDIR\Qt6Svg.dll"
  Delete "$INSTDIR\Qt6Widgets.dll"
  Delete "$INSTDIR\libc++.dll"
  Delete "$INSTDIR\libgcc_s_seh-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libunwind.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\D3Dcompiler_47.dll"
  Delete "$INSTDIR\opengl32sw.dll"
  
  RMDir /r "$INSTDIR\assets"
  RMDir /r "$INSTDIR\generic"
  RMDir /r "$INSTDIR\iconengines"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\networkinformation"
  RMDir /r "$INSTDIR\platforms"
  RMDir /r "$INSTDIR\styles"
  RMDir /r "$INSTDIR\tls"
  RMDir /r "$INSTDIR\translations"

  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

  RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd