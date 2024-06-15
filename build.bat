@echo off

setlocal

set compiler=cl
set compiler_include=C:\dev\shared\include
set compiler_warnings=/WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4996 /wd4456 /wd4267
set compiler_flags=/nologo /FC /Z7 /Od /GR-
set linker_flags=/debug /incremental:no /subsystem:console

set libs=SDL2.lib SDL2_image.lib SDL2_ttf.lib winmm.lib Shcore.lib

pushd ..\bin

%compiler% ..\src\sav.h /I%compiler_include% %compiler_warnings% %compiler_flags% /TP /DSAV_IMPLEMENTATION /c
%compiler% ..\src\surmonde.cpp sav.obj ..\vendor\glad.c /I%compiler_include% %compiler_warnings% %compiler_flags% /link %linker_flags% %libs%

popd
