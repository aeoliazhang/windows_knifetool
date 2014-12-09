# Microsoft Developer Studio Project File - Name="knife" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=knife - Win32 Driver Check Edtion
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "knife.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "knife.mak" CFG="knife - Win32 Driver Check Edtion"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "knife - Win32 Driver Check Edtion" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "knife___Win32_Driver_Check_Edtion"
# PROP BASE Intermediate_Dir "knife___Win32_Driver_Check_Edtion"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "syschecked"
# PROP Intermediate_Dir "syschecked"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Gz /MLd /W3 /WX /Z7 /Od /D WIN32=100 /D _X86_=1 /D WINVER=0x500 /D DBG=1 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ntoskrnl.lib hal.lib bufferoverflowk.lib ntstrsafe.lib /nologo /base:"0x10000" /stack:0x1000000,0x1000 /entry:"DriverEntry" /subsystem:console /map /debug /machine:I386 /nodefaultlib /out:"..\..\bin\knife.sys" /pdbtype:sept /subsystem:native /driver /SECTION:INIT,D /RELEASE /IGNORE:4078
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "knife - Win32 Driver Check Edtion"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CommonFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\knife.cpp
# End Source File
# Begin Source File

SOURCE=.\Modules.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects.cpp
# End Source File
# Begin Source File

SOURCE=.\Pe.cpp
# End Source File
# Begin Source File

SOURCE=.\Processes.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessKill.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadowSSDT.cpp
# End Source File
# Begin Source File

SOURCE=.\SSDT.cpp
# End Source File
# Begin Source File

SOURCE=.\Threads.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CommonDef.h
# End Source File
# Begin Source File

SOURCE=.\CommonFuncs.h
# End Source File
# Begin Source File

SOURCE=.\IOCtrlCode.h
# End Source File
# Begin Source File

SOURCE=.\knife.h
# End Source File
# Begin Source File

SOURCE=.\Modules.h
# End Source File
# Begin Source File

SOURCE=.\Objects.h
# End Source File
# Begin Source File

SOURCE=.\Pe.h
# End Source File
# Begin Source File

SOURCE=.\Predef.h
# End Source File
# Begin Source File

SOURCE=.\Processes.h
# End Source File
# Begin Source File

SOURCE=.\ProcessKill.h
# End Source File
# Begin Source File

SOURCE=.\ShadowSSDT.h
# End Source File
# Begin Source File

SOURCE=.\SSDT.h
# End Source File
# Begin Source File

SOURCE=.\Threads.h
# End Source File
# Begin Source File

SOURCE=.\WinNtDef.h
# End Source File
# Begin Source File

SOURCE=.\WinNtFunc.h
# End Source File
# End Group
# End Target
# End Project
