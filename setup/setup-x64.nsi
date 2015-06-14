
; NSIS install script
; use makensis /DVERSION=x.y.z setup.nsi
;

XPStyle on
!define PROGRAM_NAME "DiffImg"
!define TARGET_NAME "${PROGRAM_NAME}"
!define ORGANIZATION_NAME "TheHive"
!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0
!define ARCH 'x64'

!include "x64.nsh"


BrandingText "${PROGRAM_NAME} installation"; remove the nsis release on the gui

###########################################################################################
# init functions
###########################################################################################

Function .onInit

    # set the 64 registry
    SetRegView 64 

	# fading splashscreen !!
	# the plugins dir is automatically deleted when the installer exits
	InitPluginsDir
	File /oname=$PLUGINSDIR\splash.bmp "splashscreen-x64.bmp"
	#optional
	#File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"

	advsplash::show 2000 600 400 -1 $PLUGINSDIR\splash

	Pop $0          ; $0 has '1' if the user closed the splash screen early,
					; '0' if everything closed normally, and '-1' if some error occurred.

	Delete $PLUGINSDIR\splash.bmp

	# uninstall previous release before installing a new one
	ReadRegStr $R0 HKLM \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)" \
	"UninstallString"
	StrCmp $R0 "" done

	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
	"${PROGRAM_NAME} (x64) is already installed. $\n$\nClick `OK` to remove the \
	previous version or `Cancel` to cancel this upgrade." \
	IDOK uninst
	Abort

	;Run the uninstaller
	uninst:
	ClearErrors
	ExecWait "$INSTDIR\uninstall.exe /S"

	done:

FunctionEnd

;--------------------------------

Function LaunchLink
  ExecShell "" "$INSTDIR\${PROGRAM_NAME}.exe"
FunctionEnd

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  !ifndef VERSION
    !define VERSION 'bxx'
  !endif

;--------------------------------
;Configuration

  ;Name and file
  Name "${PROGRAM_NAME} (x64)"
  
  ; output directory if not set by batch
  !ifndef SETUP_DIR
	!define SETUP_DIR '.\'
 !endif

  ; output setup file
  !ifdef OUTFILE
    OutFile "${SETUP_DIR}\${OUTFILE}"
  !else
    OutFile "${SETUP_DIR}\${PROGRAM_NAME}-${ARCH}-${VERSION}-setup.exe"
  !endif

  ;Default installation folder
  InstallDir "$PROGRAMFILES64\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)" "Install_Dir"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP "banner.bmp" ; optional installer pages
  !define MUI_HEADERIMAGE_UNBITMAP "banner.bmp" ; uninstaller pages
  !define MUI_ABORTWARNING
  !define MUI_ICON "icon_install.ico"
  !define MUI_UNICON "icon_uninstall.ico"
  !define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp" ; welcome/finish banner
  !define MUI_WELCOMEPAGE_TITLE "Welcome to the ${PROGRAM_NAME} ${VERSION} Setup Wizard"
  !define MUI_FINISHPAGE_NOAUTOCLOSE
 
  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_FINISHPAGE_RUN_TEXT "Start ${PROGRAM_NAME} (x64) after install"
  !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
  !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED

;--------------------------------
;Pages

  ;!insertmacro MUI_PAGE_LICENSE "License.txt"
  ;!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_WELCOME ; affiche un welcome
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH ; affiche une page de fin
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SectionIn RO
  
  ; Program files there
  SetOutPath $INSTDIR
  File "..\build\Release\${PROGRAM_NAME}.exe"
  File "$%QTDIR%\bin\QtCore4.dll"
  File "$%QTDIR%\bin\QtGui4.dll"

  ; Minggw case ...
  File /nonfatal "$%QTDIR%\bin\libstdc++-6.dll"
  File /nonfatal "$%QTDIR%\bin\libgcc_s_seh-1.dll"
  File /nonfatal "$%QTDIR%\bin\libwinpthread-1.dll"
  File /nonfatal "$%QTDIR%\bin\zlib1.dll"
  
  
  ; qt image plugin
  SetOutPath "$INSTDIR\plugins\imageformats"
  File  "$%QTDIR%\plugins\imageformats\*.dll"
  
  ; Lang files
  SetOutPath "$INSTDIR\lang"
  File /nonfatal "..\lang\*.qm"
  File /nonfatal "*.qm"
  
    ; test files
  SetOutPath "$INSTDIR\test"
  File /r "..\test\*.*"
  
  ; Other files
  SetOutPath $INSTDIR
  File "..\CREDITS.txt"
  File "..\AUTHORS.txt"
  File "..\LICENSE.txt"
  File "..\Changelog.txt"
  File "..\diffimg-portable.bat"
  
  ; resources files
  SetOutPath $INSTDIR
  File "icon.ico"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)" "Version" "${VERSION}"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)" "DisplayName" "${PROGRAM_NAME} (x64)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" "DisplayIcon" '"$INSTDIR\icon.ico"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" "Publisher" "${ORGANIZATION_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" "DisplayVersion" "${VERSION}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

;--------------------------------
; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  SetShellVarContext all		; scope is "All Users"
  CreateDirectory "$SMPROGRAMS\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)"
  CreateShortCut "$SMPROGRAMS\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut  "$SMPROGRAMS\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)\${PROGRAM_NAME} (x64).lnk" "$INSTDIR\${PROGRAM_NAME}.exe" "" "$INSTDIR\${PROGRAM_NAME}.exe" 0
  
  ;create desktop shortcut
  CreateShortCut "$DESKTOP\${PROGRAM_NAME} (x64).lnk" "$INSTDIR\${PROGRAM_NAME}.exe" "" "$INSTDIR\icon.ico"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME} (x64)"
  DeleteRegKey HKLM "SOFTWARE\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)"
	
  ; Remove files and uninstaller
  Delete "$INSTDIR\*.*"
  
  ; Remove shortcuts, if any
  SetShellVarContext all		; scope is "All Users"
  Delete "$SMPROGRAMS\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)\*.*"
  
  ; remove desktop link
  Delete "$DESKTOP\${PROGRAM_NAME} (x64).lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${ORGANIZATION_NAME}\${PROGRAM_NAME} (x64)"
  RMDir "$SMPROGRAMS\${ORGANIZATION_NAME}"
  RMDir /r "$INSTDIR\plugins"
  RMDir /r "$INSTDIR"

SectionEnd