!include "x64.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Xapl"
!define PRODUCT_VERSION "0.9.5"
!define PRODUCT_PUBLISHER "Xive Software"
!define PRODUCT_WEB_SITE "http://www.xivesoftware.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Xapl.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

!define CSIDL_LOCALAPPDATA '0x1C'

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

Function .onInit
 ; Allow only one installer to run at a time; this brings the currently running installer to the top
 System::Call "kernel32::CreateMutexA(i 0, i 0, t '$(^Name)') i .r0 ?e"
 Pop $0
 StrCmp $0 0 launch
  StrLen $0 "$(^Name)"
  IntOp $0 $0 + 1
 loop:
   FindWindow $1 '#32770' '' 0 $1
   IntCmp $1 0 +4
   System::Call "user32::GetWindowText(i r1, t .r2, i r0) i."
   StrCmp $2 "$(^Name)" 0 loop
   System::Call "user32::SetForegroundWindow(i r1) i."
   Abort
 launch:
FunctionEnd

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"

; Welcome page
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING.rtf"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Xapl"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Xapl.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\..\bin\msw\Release\x64\Xapl-${PRODUCT_VERSION}-Setup64.exe"
InstallDir "$PROGRAMFILES64\Xapl"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\..\bin\msw\Release\x64\Xapl.exe"
  File "..\..\bin\msw\Release\x64\xAutoUpdater.exe"
  File "..\..\bin\msw\Release\x64\tag.dll"
  File "..\..\bin\msw\Release\x64\bassmix.dll"
  File "..\..\bin\msw\Release\x64\bass.dll"
  File "..\..\bin\msw\Release\x64\libcurl.dll"
  File "..\..\bin\msw\Release\x64\zlib1.dll"
  SetOutPath "$INSTDIR\plugins\sound"
  File "..\..\bin\msw\Release\x64\plugins\sound\basswma.dll"
  File "..\..\bin\msw\Release\x64\plugins\sound\bassflac.dll"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Xapl.lnk" "$INSTDIR\Xapl.exe"
  CreateShortCut "$DESKTOP\Xapl.lnk" "$INSTDIR\Xapl.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  SetRegView 64
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Xapl.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Xapl.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\bass.dll"
  Delete "$INSTDIR\bassmix.dll"
  Delete "$INSTDIR\plugins\sound\bassflac.dll"
  Delete "$INSTDIR\plugins\sound\basswma.dll"
  RMDir  "$INSTDIR\plugins\sound"
  RMDir  "$INSTDIR\plugins"
  Delete "$INSTDIR\libcurl.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\tag.dll"
  Delete "$INSTDIR\Xapl.exe"
  Delete "$INSTDIR\xAutoUpdater.exe"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$DESKTOP\Xapl.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Xapl.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR"
  
  System::Call 'shell32::SHGetSpecialFolderPathA(i $HWNDPARENT, t .r1, i ${CSIDL_LOCALAPPDATA}, b 'false') i r0'
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Would you like to remove all $(^Name) settings and playlists?" IDNO +2
  RMDir /r "$1\Xapl"
  
  SetRegView 64
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd