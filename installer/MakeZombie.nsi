;The Zombie Engine Setup Script
; based on NSIS Setup Script
;--------------------------------

!ifndef VERSION
  !define VERSION '0.8.2'
!endif

;--------------------------------
;Configuration

!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile ..\TheZombieEngine-${VERSION}-setup.exe
!endif

SetCompressor /SOLID lzma

InstType "Engine and editor"
InstType "Engine, editor and exporter"

InstallDir "$PROGRAMFILES\The Zombie Engine"
InstallDirRegKey HKLM Software\TheZombieEngine ""

RequestExecutionLevel admin

;--------------------------------
;Header Files

!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "Memento.nsh"
!include "WordFunc.nsh"
!include "WriteEnvStr.nsh"

;--------------------------------
;Functions

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

  !insertmacro VersionCompare

!endif

;--------------------------------
;Definitions

!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0

  ;!define MUI_HEADERIMAGE_BITMAP "${RESOURCEDIR}\iconTrag.bmp"
  ;!define MUI_HEADERIMAGE_RIGHT
  ;!define MUI_INSTFILESPAGE_COLORS /windows
  ;!define MUI_INSTFILESPAGE_PROGRESSBAR smooth
  ;!define MUI_WELCOMEFINISHPAGE_BITMAP "${RESOURCEDIR}\welcome2.bmp"
  ;!define MUI_FINISHPAGE_NOAUTOCLOSE
  ;!define MUI_COMPONENTSPAGE_NODESC
  ;!define MUI_UNFINISHPAGE_NOAUTOCLOSE

  ;!define MUI_CUSTOMFUNCTION_GUIINIT myInit

;--------------------------------
;Configuration

;Names
Name "The Zombie Engine"
Caption "The Zombie Engine ${VERSION} Setup"

;Memento Settings
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie"

;Interface Settings
!define MUI_ABORTWARNING

!define MUI_HEADERIMAGE
;!define MUI_WELCOMEFINISHPAGE_BITMAP "..\gamerns\libs\system\textures\rns_loadscreen.tga"

!define MUI_COMPONENTSPAGE_SMALLDESC

;Pages
!define MUI_WELCOMEPAGE_TITLE "Welcome to the The Zombie Engine ${VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of The Zombie Engine ${VERSION}, the game engine developed by Tragnarion Studios and released open source.$\r$\n$\r$\n$_CLICK"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\zombie\docs\Zombie LICENSE.txt"
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
Page custom PageReinstall PageLeaveReinstall
!endif
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_LINK "Visit The Zombie Engine site at SourceForge for the latest news, FAQs and support"
!define MUI_FINISHPAGE_LINK_LOCATION "http://thezombieengine.sf.net/"

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION LaunchConjurer
!define MUI_FINISHPAGE_NOREBOOTSUPPORT

!define MUI_FINISHPAGE_SHOWREADME
;!define MUI_FINISHPAGE_SHOWREADME_TEXT "Show release notes"
;!define MUI_FINISHPAGE_SHOWREADME_FUNCTION ShowReleaseNotes

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

${MementoSection} "The Zombie Engine Core and Conjurer editor" SecCore

  SetDetailsPrint textonly
  DetailPrint "Installing Zombie Engine Core Files..."
  SetDetailsPrint listonly

  SectionIn 1 2 RO
  SetOutPath $INSTDIR
  RMDir /r "$SMPROGRAMS\The Zombie Engine"

  SetOverwrite on
  
  SetOutPath $INSTDIR\zombie
  File "..\zombie\xmlschema.xml"
  File "..\zombie\docs\Zombie LICENSE.txt"

  SetOutPath $INSTDIR\zombie\bin\win32
  ;File "..\zombie\bin\win32\xcrashreport.exe"
  File "..\zombie\bin\win32\dbghelp.dll"
  File "..\zombie\bin\win32\devil.dll"
  File "..\zombie\bin\win32\ilu.dll"
  File "..\zombie\bin\win32\msvcp71.dll"
  File "..\zombie\bin\win32\msvcr71.dll"
  File "..\zombie\bin\win32d\python24.dll"
  File "..\zombie\bin\win32\renaissance.exe"
  File "..\zombie\bin\win32\conjurer.exe"
  
  SetOutPath $INSTDIR\zombie\bin\win32d
  ;File "..\zombie\bin\win32\xcrashreport.exe"
  File "..\zombie\bin\win32d\dbghelp.dll"
  File "..\zombie\bin\win32d\devil.dll"
  File "..\zombie\bin\win32d\ilu.dll"
  File "..\zombie\bin\win32d\msvcp71.dll"
  File "..\zombie\bin\win32d\msvcr71.dll"
  File "..\zombie\bin\win32d\python24.dll"
  File "..\zombie\bin\win32d\renaissance.exe"
  File "..\zombie\bin\win32d\conjurer.exe"

  SetOutPath $INSTDIR\zombie\data
  File /r /x ".svn" "..\zombie\data\*.*"
  File /r /x ".svn" "..\zombie\data\*"

  SetOutPath $INSTDIR\zombie\docs
  File /r /x ".svn" "..\zombie\docs\*.*"

${MementoSectionEnd}

${MementoSection} "Clean working copy data" SecWorkingCopy

  SetDetailsPrint textonly
  DetailPrint "Copying Clean working copy data..."
  SetDetailsPrint listonly

  SectionIn 1 2
  
  SetOutPath $INSTDIR\gamerns
  File /r /x ".svn" "..\gamerns\*.*"
  File /r /x ".svn" "..\gamerns\*"

  CreateDirectory "$INSTDIR\gamernstemp"
  
${MementoSectionEnd}

${MementoSection} "3D Studio MAX 9 plugins" SecMAX9Plugins

  SetDetailsPrint textonly
  DetailPrint "Installing 3D Studio MAX 9 plugins..."
  SetDetailsPrint listonly

  SectionIn 2
  
  ; Gets the 3dsmax directory
  ReadRegDWORD $0 HKLM "SOFTWARE\Autodesk\3dsmax\9.0\MAX-1:409" "Installdir"
  StrCmp $0 "" nomax +1
  
  SetOutPath $0
  File "..\zombie\bin\dll\msvcp71.dll"
  File "..\zombie\bin\dll\msvcr71.dll"
  ;File "..\zombie\bin\win32\xcrashreport.exe"
  File "..\zombie\bin\dll\devil.dll"
  File "..\zombie\bin\dll\ilu.dll"

  SetOutPath $0\plugins
  File "..\zombie\bin\win32d\nmaterialstextures.dlt"
  File "..\zombie\bin\win32d\n3dsmaxexport.dle"
  File "..\zombie\bin\win32d\n3dsutilfunctions.dlu" 
  File "..\zombie\code\exporter\3dsplugin\3dsMax\plugins\nebula.ms"

  SetOutPath $0
  File /r /x ".svn" "..\zombie\code\exporter\3dsplugin\3dsMax\*.*"
  File /r /x ".svn" "..\zombie\code\exporter\3dsplugin\3dsMax\*"
  
nomax:
	
${MementoSectionEnd}

${MementoSection} "Python" SecPython

  SetDetailsPrint textonly
  DetailPrint "Installing Python stuff ..."
  SetDetailsPrint listonly

  SectionIn 1 2

  SetOutPath $INSTDIR\deps
  File "..\deps\python-2.4.4.msi"
  File "..\deps\libxml2.exe"
  File "..\deps\wxPython2.6-win32-unicode-2.6.3.3-py24.exe"
  
  ReadRegStr $0 HKLM "SOFTWARE\Python\PythonCore\2.4\InstallPath" ""
  StrCmp $0 "" +1 pythonExists
  ReadRegStr $0 HKU "Software\Python\PythonCore\2.4\InstallPath" ""
  StrCmp $0 "" +1 pythonExists
  ReadRegStr $0 HKCU "Software\Python\PythonCore\2.4\InstallPath" ""
  StrCmp $0 "" +1 pythonExists

  DetailPrint "Installing Python ..."
  ExecWait 'msiexec /i "$INSTDIR\deps\Python-2.4.4.msi"'
  ReadRegStr $0 HKLM "SOFTWARE\Python\PythonCore\2.4\InstallPath" ""
  
pythonExists:

  DetailPrint "Installing libXML ..."
  IfFileExists "$0\Lib\site-packages" libXMLExists
  ExecWait "$INSTDIR\deps\libxml2.exe"
  
libXMLExists:

  DetailPrint "Installing WxPython ..."
  IfFileExists "$0\Lib\site-packages\wx-2.6-msw-unicode" wxPythonExists
  ExecWait "$INSTDIR\deps\wxPython2.6-win32-unicode-2.6.3.3-py24.exe"

wxPythonExists:

  DetailPrint "Installing wxPython property grid ..."
  SetOutPath "$0\Lib\site-packages\wx-2.6-msw-unicode"
  File /r /x ".svn" "..\wxWidgets\*.*"
  File /r /x ".svn" "..\wxWidgets\*"

  Delete "$INSTDIR\deps\python-2.4.4.msi"
  Delete "$INSTDIR\deps\libxml2.exe"
  Delete "$INSTDIR\deps\wxPython2.6-win32-unicode-2.6.3.3-py24.exe"
  RMDir /r "$INSTDIR\deps"

${MementoSectionEnd}

${MementoSection} "Start Menu Shortcuts" SecStartMenuShortcuts

  SetDetailsPrint textonly
  DetailPrint "Installing Start Menu Shortcuts..."
  SetDetailsPrint listonly

  SectionIn 1 2
  
  ; Remove shortcuts, if any
  RMDir /r "$SMPROGRAMS\The Zombie Engine"

  CreateDirectory "$SMPROGRAMS\The Zombie Engine"

  SetOutPath $INSTDIR\zombie\bin\win32d
  ;CreateDirectory "$SMPROGRAMS\The Zombie Engine\Debug"
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Conjurer Debug.lnk" "$INSTDIR\zombie\bin\win32d\conjurer.exe" "" "$INSTDIR\zombie\bin\win32\conjurer.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Renaissance Debug.lnk" "$INSTDIR\zombie\bin\win32d\renaissance.exe" "" "$INSTDIR\zombie\bin\win32\renaissance.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Presentation Conjurer Debug.lnk" "$INSTDIR\zombie\bin\win32d\conjurer.exe" "-w 1024 -h 768 -sync -fullscreen"  "$INSTDIR\zombie\bin\win32\conjurer.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Debug\Presentation Renaissance Debug.lnk" "$INSTDIR\zombie\bin\win32d\renaissance.exe" "-w 1024 -h 768 -sync -fullscreen"  "$INSTDIR\zombie\bin\win32\renaissance.exe" 0

  SetOutPath $INSTDIR\zombie\bin\win32
  ;CreateDirectory "$SMPROGRAMS\The Zombie Engine\Release"
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Conjurer.lnk" "$INSTDIR\zombie\bin\win32\conjurer.exe" "" "$INSTDIR\zombie\bin\win32\conjurer.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Renaissance.lnk" "$INSTDIR\zombie\bin\win32\renaissance.exe" "" "$INSTDIR\zombie\bin\win32\renaissance.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Presentation Conjurer.lnk" "$INSTDIR\zombie\bin\win32\conjurer.exe" "-w 1024 -h 768 -sync -fullscreen"  "$INSTDIR\zombie\bin\win32\conjurer.exe" 0
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Release\Presentation Renaissance.lnk" "$INSTDIR\zombie\bin\win32\renaissance.exe" "-w 1024 -h 768 -sync -fullscreen"  "$INSTDIR\zombie\bin\win32\renaissance.exe" 0

  SetOutPath $INSTDIR
  CreateShortCut "$SMPROGRAMS\The Zombie Engine\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  ;CreateShortCut "$SMPROGRAMS\The Zombie Engine\Documentation.lnk" "\\trag-server\tragnarion\docs\nebula2\nebula2.chm" "" "\\trag-server\tragnarion\docs\nebula2\nebula2.chm" 0
  ;CreateDirectory "$SMPROGRAMS\The Zombie Engine\Manuals"
  ;CreateShortCut "$SMPROGRAMS\The Zombie Engine\Manuals\User Manual.lnk" "http://wiki.tragnarion.com/tiki-index.php?page=SWD+Nebula+Conjurer" "" "" 0
  ;CreateShortCut "$SMPROGRAMS\The Zombie Engine\Manuals\Exporter Manual.lnk" "http://wiki.tragnarion.com/tiki-index.php?page=SWD+Nebula+MAX+Exporter" "" "" 0
${MementoSectionEnd}

${MementoSection} "Desktop Shortcut" SecDesktopShortcuts

  SetDetailsPrint textonly
  DetailPrint "Installing Desktop Shortcut..."
  SetDetailsPrint listonly

  SectionIn 1 2

  CreateShortCut "$DESKTOP\Conjurer.lnk" "$INSTDIR\zombie\bin\win32\conjurer.exe" "" "$INSTDIR\zombie\bin\win32\conjurer.exe" 0

${MementoSectionEnd}

${MementoSectionDone}

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "The core files required to use The Zombie Engine"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecWorkingCopy} "Clean working copy data, data needed to run the tools and engine"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMAX9Plugins} "3D Studio MAX 9 plugins to export scenes, materials, indoors, etc."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPython} "Python setup and dependencies needed for the Conjurer editor including python, libXML2, wxPython and the wxPython property grid"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenuShortcuts} "Start Menu Shortcuts"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopShortcuts} "Desktop Shortcut"
  
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Installer Functions

Function .onInit

  ${MementoSectionRestore}

FunctionEnd

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

Var ReinstallPageCheck

Function PageReinstall

  ReadRegStr $R0 HKLM "Software\Zombie" ""

  ${If} $R0 == ""
    Abort
  ${EndIf}

  ReadRegDWORD $R0 HKLM "Software\Zombie" "VersionMajor"
  ReadRegDWORD $R1 HKLM "Software\Zombie" "VersionMinor"
  ReadRegDWORD $R2 HKLM "Software\Zombie" "VersionRevision"
  ReadRegDWORD $R3 HKLM "Software\Zombie" "VersionBuild"
  StrCpy $R0 $R0.$R1.$R2.$R3

  ${VersionCompare} ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD} $R0 $R0
  ${If} $R0 == 0
    StrCpy $R1 "The Zombie Engine ${VERSION} is already installed. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Add/Reinstall components"
    StrCpy $R3 "Uninstall The Zombie Engine"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose the maintenance option to perform."
    StrCpy $R0 "2"
  ${ElseIf} $R0 == 1
    StrCpy $R1 "An older version of The Zombie Engine is installed on your system. It's recommended that you uninstall the current version before installing. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install The Zombie Engine."
    StrCpy $R0 "1"
  ${ElseIf} $R0 == 2
    StrCpy $R1 "A newer version of The Zombie Engine is already installed! It is not recommended that you install an older version. If you really want to install this older version, it's better to uninstall the current version first. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install The Zombie Engine."
    StrCpy $R0 "1"
  ${Else}
    Abort
  ${EndIf}

  nsDialogs::Create /NOUNLOAD 1018

  ${NSD_CreateLabel} 0 0 100% 24u $R1
  Pop $R1

  ${NSD_CreateRadioButton} 30u 50u -30u 8u $R2
  Pop $R2
  ${NSD_OnClick} $R2 PageReinstallUpdateSelection

  ${NSD_CreateRadioButton} 30u 70u -30u 8u $R3
  Pop $R3
  ${NSD_OnClick} $R3 PageReinstallUpdateSelection

  ${If} $ReinstallPageCheck != 2
    SendMessage $R2 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${Else}
    SendMessage $R3 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${EndIf}

  nsDialogs::Show

FunctionEnd

Function PageReinstallUpdateSelection

  Pop $R1

  ${NSD_GetState} $R2 $R1

  ${If} $R1 == ${BST_CHECKED}
    StrCpy $ReinstallPageCheck 1
  ${Else}
    StrCpy $ReinstallPageCheck 2
  ${EndIf}

FunctionEnd

Function PageLeaveReinstall

  ${NSD_GetState} $R2 $R1

  StrCmp $R0 "1" 0 +2
    StrCmp $R1 "1" reinst_uninstall reinst_done

  StrCmp $R0 "2" 0 +3
    StrCmp $R1 "1" reinst_done reinst_uninstall

  reinst_uninstall:
  ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "UninstallString"

  ;Run uninstaller
  HideWindow

    ClearErrors
    ExecWait '$R1 _?=$INSTDIR'

    IfErrors no_remove_uninstaller
    IfFileExists "$INSTDIR\zombie\bin\win32\conjurer.exe" no_remove_uninstaller

      Delete $R1
      RMDir $INSTDIR

    no_remove_uninstaller:

  StrCmp $R0 "2" 0 +2
    Quit

  BringToFront

  reinst_done:

FunctionEnd

!endif # VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

Function ShowReleaseNotes
FunctionEnd

;--------------------------------
;Uninstaller Section

Section Uninstall

  SetDetailsPrint textonly
  DetailPrint "Uninstalling The Zombie Engine..."
  SetDetailsPrint listonly

  IfFileExists $INSTDIR\zombie\bin\win32\conjurer.exe zombie_installed
    MessageBox MB_YESNO "It does not appear that Zombie is installed in the directory '$INSTDIR'.$\r$\nContinue anyway (not recommended)?" IDYES zombie_installed
    Abort "Uninstall aborted by user"
  zombie_installed:

  SetDetailsPrint textonly
  DetailPrint "Deleting Registry Keys..."
  SetDetailsPrint listonly

  ; Remove registry keys
  DeleteRegKey HKCR ".n2"
  DeleteRegKey HKCR "n2_ext"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie"
  DeleteRegKey HKLM "Software\Zombie"

  SetDetailsPrint textonly
  DetailPrint "Deleting Environment variables..."
  SetDetailsPrint listonly
  
  ; Write the environment variable/Exec
  Push "NEBULA2_HOME"      # name
  Call un.DeleteEnvStr
					   
  Push "CONJURER_WC"      
  Call un.DeleteEnvStr

  Push "CONJURER_WCTMP"
  Call un.DeleteEnvStr
  
  SetDetailsPrint textonly
  DetailPrint "Deleting Files and shortcuts..."
  SetDetailsPrint listonly

  RMDir /r "$SMPROGRAMS\The Zombie Engine"
  RMDir /r $INSTDIR
  Delete "$DESKTOP\Conjurer.lnk"

  SetDetailsPrint both

SectionEnd

Function .onInstSuccess

    ; Write the environment variable/Exec
    Push "NEBULA2_HOME"      # name
    Push "$INSTDIR\zombie"          # value
    Call WriteEnvStr
                           
    Push "CONJURER_WC"      
    Push "$INSTDIR\gamerns\"
    Call WriteEnvStr

    Push "CONJURER_WCTMP"
    Push "$INSTDIR\gamernstemp\"
    Call WriteEnvStr
    
    ; Remove registry keys
    DeleteRegKey HKCR ".n2"
    DeleteRegKey HKCR "n2_ext"

    ; Register the .n2 file type
    WriteRegStr HKCR ".n2" "" "n2_ext"
    WriteRegStr HKCR "n2_ext" "" "Nebula2 viewer file type"
    WriteRegStr HKCR "n2_ext\DefaultIcon" "" "$INSTDIR\zombie\bin\win32\conjurer.exe,0"

    WriteRegStr HKCR "n2_ext\Shell" "" "open"
    WriteRegStr HKCR "n2_ext\Shell\Open" "" "Open"
    WriteRegStr HKCR "n2_ext\Shell\Open\Command" "" "$\"$INSTDIR\zombie\bin\win32\conjurer.exe$\" -w 1024 -h 768 -view2 $\"%1$\""

    WriteRegStr HKCR "n2_ext\Shell" "" "open"
    WriteRegStr HKCR "n2_ext\Shell\Open Debug" "" "Open Debug"
    WriteRegStr HKCR "n2_ext\Shell\Open Debug\Command" "" "$\"$INSTDIR\zombie\bin\win32d\conjurer.exe$\" -w 1024 -h 768 -view2 $\"%1$\""

    WriteRegStr HKCR "n2_ext\Shell" "" "open"
    WriteRegStr HKCR "n2_ext\Shell\Presentation" "" "Open Presentation"
    WriteRegStr HKCR "n2_ext\Shell\Presentation\Command" "" "$\"$INSTDIR\zombie\bin\win32\conjurer.exe$\" -w 1024 -h 768 -sync -fullscreen -view2 $\"%1$\""

    WriteRegStr HKCR "n2_ext\Shell" "" "open"
    WriteRegStr HKCR "n2_ext\Shell\Embedded" "" "Editor Embedded"
    WriteRegStr HKCR "n2_ext\Shell\Embedded\Command" "" "$\"$INSTDIR\zombie\bin\win32\conjurer.exe$\" -gui $\"outgui:outgui.py$\" -w 1024 -h 768 -embedded -view2 $\"%1$\""

    WriteRegStr HKCR "n2_ext\Shell" "" "open"
    WriteRegStr HKCR "n2_ext\Shell\Embedded Debug" "" "Editor Embedded Debug"
    WriteRegStr HKCR "n2_ext\Shell\Embedded Debug\Command" "" "$\"$INSTDIR\zombie\bin\win32d\conjurer.exe$\" -gui $\"outgui:outgui.py$\" -w 1024 -h 768 -embedded -view2 $\"%1$\""

  ; Write installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Zombie" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\Zombie" "Version" "${VERSION}"

  ; Write the unistall keys for windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "DisplayName" "The Zombie Engine"
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "InstallLocation" "$INSTDIR"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "NoRepair" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "DisplayIcon" "$INSTDIR\zombie\bin\win32\conjurer.exe,0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Zombie" "URLInfoAbout" "http://thezombieengine.sourceforge.net/"

  WriteUninstaller "$INSTDIR\uninstall.exe"

  ${MementoSectionSave}

  SetDetailsPrint both
	
FunctionEnd

Function LaunchConjurer

  Exec '"$INSTDIR\zombie\bin\win32\conjurer.exe" -wc "$INSTDIR\gamerns" -w 1024 -h 768'
  
FunctionEnd
