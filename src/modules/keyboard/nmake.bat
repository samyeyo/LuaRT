@echo off
@chcp 65001 >nul 2>&1

if "%PLATFORM%"=="" (
    echo ** Unable to find Visual C/C++ and Windows SDK
    echo ** Please ensure you are running this script from a Visual Studio Developer Command Prompt.
    exit /b 1
)

nmake.exe /nologo %1 LUART_PATH=..\..\.. PLATFORM=%PLATFORM%