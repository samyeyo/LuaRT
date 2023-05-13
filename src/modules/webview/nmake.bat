@echo off
@setlocal EnableDelayedExpansion

if exist ..\..\..\bin\luart.exe (
    for /f %%i in ('..\..\..\bin\luart.exe -e "print(_ARCH)"') do nmake.exe /nologo PLATFORM=%%i
) else (
    nmake.exe /nologo %1
)