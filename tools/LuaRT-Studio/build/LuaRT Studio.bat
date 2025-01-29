@echo off
REM Batch script to build LuaRT Studio

windres ../studio/res/studio.rc -o studio.o
gcc -Os -s -mwindows -static-libgcc -mfpmath=sse -mieee-fp -fno-exceptions -fdata-sections -ffunction-sections -Wl,--gc-sections -fipa-pta -ffreestanding -fno-stack-check -fno-ident -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--build-id=none -Wl,-O1 -Wl,--as-needed -Wl,--no-insert-timestamp -Wl,--no-seh -flto -o "../LuaRT Studio.exe" win32_starter.c studio.o -lshlwapi
del /Q studio.o