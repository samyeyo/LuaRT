@echo off
@chcp 65001 >nul 2>&1

nmake.exe /nologo %1 LUART_PATH=..\..\.. PLATFORM=%SDK_ARCH%