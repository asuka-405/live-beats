# Microsoft Developer Studio Project File - Name="main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=main - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "main.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "main.mak" CFG="main - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "main - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "main - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "main - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../bin"
# PROP Intermediate_Dir "../../obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I ".." /I "../../wx/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../../wx/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx.lib xpm.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /out:"../../bin/iabc.exe" /libpath:"../../lib" /libpath:"../../wx/lib"

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\src" /I "..\..\include" /I "..\..\lib\mswd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"docview.res" /i "../../wx/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /out:"../../bin/iabc.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "main - Win32 Release"
# Name "main - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\resources.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\system_dirs.cpp
# End Source File
# Begin Source File

SOURCE=.\wx24_resources.cpp
# End Source File
# Begin Source File

SOURCE=.\wx_resources.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resources.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\system_dirs.h
# End Source File
# Begin Source File

SOURCE=.\wx24_resources.h
# End Source File
# Begin Source File

SOURCE=.\wx_resources.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\main\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=..\main\chart.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\colours.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\cross.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\disable.bmp
# End Source File
# Begin Source File

SOURCE=..\main\doc.ico
# End Source File
# Begin Source File

SOURCE=..\main\docview.rc
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\error.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\info.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=..\main\notepad.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_dwn.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_enl.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_shr.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_up.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_zin.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\plot_zot.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\question.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\tick.bmp
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\tip.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\warning.ico
# End Source File
# Begin Source File

SOURCE=..\main\wx\msw\watch1.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\resources.wxr
# End Source File
# End Target
# End Project
