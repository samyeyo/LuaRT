@echo off
@chcp 65001 >nul 2>&1

@SET p=
FOR /F "delims=" %%i IN ('where luart.exe 2^>nul') DO SET p=%%~di%%~pi..
IF not DEFINED p SET p=..\..\..

nmake.exe /nologo %1 LUART_PATH=%p% PLATFORM=%SDK_ARCH%