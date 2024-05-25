# Microsoft Developer Studio Project File - Name="iabc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=iabc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iabc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iabc.mak" CFG="iabc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iabc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "iabc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iabc - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /Od /I "src" /I "wx/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\obj\iabc.lib"

!ELSEIF  "$(CFG)" == "iabc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "iabc___Win32_Debug"
# PROP BASE Intermediate_Dir "iabc___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "src" /I "..\..\include" /I "..\..\lib\mswd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\iabc.lib"

!ENDIF 

# Begin Target

# Name "iabc - Win32 Release"
# Name "iabc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\iabc\abcrules.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\array.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\beamer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\dispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\draw_figure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\drawtemp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\event.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\event_log.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\factory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\figure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\figure_factory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\gm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_media.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_media_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_parse.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_preferences.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\key_sig.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\list_box_control.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\map.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\media.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\mem_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\mem_pool.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\message_box.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\midi_media.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\music_info.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\note_figure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\note_info.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\parse.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\percentage_meter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\pitch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\polygon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\preprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\registry_defaults.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\scan.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\semaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\simple.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\slurer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\staff.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\string.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\text_figure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\threads.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\time_sig.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wd_data.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wd_temp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32semaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32threads.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win_page.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\window_media.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\winres.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_dispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_list_box.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_percentage_meter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_percentage_meter.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_pp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_pp_temp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_text_buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_winres.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\iabc\abcrules.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\array.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\beamer.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\debug.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\dispatch.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\draw_figure.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\drawtemp.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\event.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\event_log.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\factory.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\figure.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\figure_factory.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\gm.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_media.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_media_manager.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_parse.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\iabc_pp.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\key_sig.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\list.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\list_box_control.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\map.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\media.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\mem_pool.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\message_box.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\midi_media.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\music_info.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\mutex.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\note_figure.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\note_info.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\null_media.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\parse.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\percentage_meter.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\pitch.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\polygon.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\preprocess.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\registry.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\registry_defaults.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\scan.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\semaphore.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\simple.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\slurer.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\staff.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\staff_info.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\string.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\text_figure.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\threads.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\time_sig.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wd_data.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32mutex.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32res.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32semaphore.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win32threads.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\win_page.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\window_feature.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\window_figure.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\window_media.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\winres.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_dispatch.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_list_box.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_mutex.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_pp.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_pp_temp.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_semaphore.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_text_buffer.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_threads.h
# End Source File
# Begin Source File

SOURCE=.\src\iabc\wx_winres.h
# End Source File
# End Group
# End Target
# End Project
