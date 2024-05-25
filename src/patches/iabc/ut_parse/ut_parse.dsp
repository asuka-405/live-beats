# Microsoft Developer Studio Project File - Name="ut_parse" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ut_parse - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ut_parse.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ut_parse.mak" CFG="ut_parse - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ut_parse - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ut_parse - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ut_parse - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ut_parse - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ut_parse - Win32 Release"
# Name "ut_parse - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\iabc\abcrules.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\abcrules.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\array.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\array.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\iabc_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\iabc_parse.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\iabc_preferences.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\list.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\list.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\map.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\map.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\media.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\media.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\mem_pool.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\mem_pool.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\music_info.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\music_info.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\mutex.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\note_info.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\note_info.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\parse.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\percentage_meter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\pitch.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\preprocess.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\scan.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\simple.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\src\iabc\string.cpp
# End Source File
# Begin Source File

SOURCE=.\ut_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\src\iabc\win32mutex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\event_log.h
# End Source File
# Begin Source File

SOURCE=..\mem_pool.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\null_media.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\parse.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\percentage_meter.h
# End Source File
# Begin Source File

SOURCE=..\pp.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\preprocess.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\scan.h
# End Source File
# Begin Source File

SOURCE=..\simple.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\simple.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\string.h
# End Source File
# Begin Source File

SOURCE=..\src\iabc\win32mutex.h
# End Source File
# Begin Source File

SOURCE=..\win32mutex.h
# End Source File
# Begin Source File

SOURCE=..\win32semaphore.h
# End Source File
# Begin Source File

SOURCE=..\win32threads.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
