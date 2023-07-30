@echo off

set dir=%cd%
for /f %%i in ('dir /ad/b "%dir%"') do @(if exist %%i/nmake.bat @(cd %%i & @nmake.bat %1 & cd ..) else @(if exist %%i/Makefile @(cd %%i & @nmake.exe %1 & cd ..)))