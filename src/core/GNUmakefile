# | LuaRT - A Windows programming framework for Lua
# | Luart.org, Copyright (c) Tine Samir 2024
# | See Copyright Notice in LICENSE.TXT
# |--------------------------------------------------------------
# | LuaRT Core Makefile for the toolchain, the Core framework and the tools
# | You can prefix all the following tasks with PLATFORM=x64|x86  
# | Don't forget to get all submodules :
# | git clone --recurse-submodules git://github.com/samyeyo/LuaRT.git
# |--------------------------------------------------------------
# | Usage (default release build)			 : make
# | Usage (debug build) 		  			 : make debug
# | Usage (release with tools)		 		 : make all
# | Usage (build setup executable)	  		 : make setup
# | Usage (build tools)				  		 : make tools
# | Usage (clean all)	 				 	 : make clean
# |-------------------------------------------------------------
# | Or you can use default release build for any platform 
# | Usage (default x64 release build)		 : make x64
# | Usage (default x86 release build)		 : make x86
# |-------------------------------------------------------------

#---- Platform selection
#---- x86 for 32bits lua54.dll, liblua54.a and executables
#---- x64 for 64bits lua54.dll, liblua54.a and executables

PLATFORM ?= x64

#---- LuaRT version
LUART_MAJOR= 1
LUART_MINOR= 7
LUART_RELEASE= 1
VERSION= $(LUART_MAJOR).$(LUART_MINOR).$(LUART_RELEASE)
CLEAN=clean
CODEPAGE := $(shell chcp 65001)

ifeq ($(filter $(CLEAN),$(MAKECMDGOALS)),)
ifeq ($(PLATFORM), x86)
 ARCH = -m32 -march=pentium4 -msse -msse2 -mmmx -fno-unwind-tables -fno-asynchronous-unwind-tables 
 _ARCH = x86
 TARGET = pe-i386
 ifeq "$(wildcard $(windir)\system32\SensApi.dll)" ""
	$(error $(PLATFORM) build requires "$(windir)\system32\SensApi.dll")
 endif
endif
ifeq ($(PLATFORM), x64)
 ARCH = -m64   -Wno-int-to-pointer-cast
 _ARCH = x64
 TARGET = pe-x86-64
endif
endif

ifeq ($(STATIC), TRUE)
 LUA_API = 
else
 LUA_API = -DLUA_BUILD_AS_DLL
ifneq ($(NOMSG), TRUE)
$(info)
$(info ━━━━━━━━━━━━━━━━━━━━  Building LuaRT $(LUART_MAJOR).$(LUART_MINOR).$(LUART_RELEASE) for $(PLATFORM)  ━━━━━━━━━━━━━━━━━━━━)
endif
endif
DEFINES= -DLUART_MINOR=$(LUART_MINOR) -DLUART_MAJOR=$(LUART_MAJOR) -DLUART_RELEASE=$(LUART_RELEASE)
DEFINES_RES= --target=$(TARGET) -D LUART_MINOR=$(LUART_MINOR) -D LUART_MAJOR=$(LUART_MAJOR) -D LUART_RELEASE=$(LUART_RELEASE)

BUILD := release

DEST= ..\..\bin
cflags.release = $(DEFINES) $(ARCH) -s -static-libgcc -O2 -mfpmath=sse -mieee-fp -DLUA_ARCH=\"$(_ARCH)\" $(LUA_API) -D__MINGW64__ -D_WIN32_WINNT=0x0600 -DLUA_COMPAT_5_3 -I"..\..\include" -Wall -Wextra -I"." -fno-exceptions -fdata-sections -ffunction-sections -fipa-pta -ffreestanding -fno-stack-check -fno-ident -fomit-frame-pointer -Wl,--gc-sections -Wl,--build-id=none -Wl,-O1 -Wl,--as-needed -Wl,--no-insert-timestamp -Wl,--no-seh -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-parameter -Wno-implicit-fallthrough
ldflags.release= -static-libgcc -lm -Wl,--no-insert-timestamp -lgdi32 -L"." 
cflags.debug = $(DEFINES) $(ARCH) -static-libgcc -g -O0 -mfpmath=sse -mieee-fp -mmmx -msse -msse2 -DDEBUG -DLUA_ARCH=\"$(_ARCH)\" $(LUA_API) -D__MINGW64__ -D_WIN32_WINNT=0x0600 -DLUA_COMPAT_5_3 -I"..\..\include" -I"." 
ldflags.debug= -g -static-libgcc -lm -lgdi32 -L"."

CC= gcc
CFLAGS := ${cflags.${BUILD}}
LDFLAGS := ${ldflags.${BUILD}}
LIBS= -lshlwapi -lcomctl32 -lcrypt32 -lcredui -luxtheme -loleaut32 -lole32 -lwindowscodecs -ldwmapi -lsensapi -lcomdlg32 -luuid -lwininet -lws2_32 -lSecur32 -ldnsapi -liphlpapi -lurlmon
RM= del /Q

LUA_A=		lua54.dll
LUA_O=		lua\lapi.o lua\lcode.o lua\lctype.o lua\ldebug.o lua\ldo.o lua\ldump.o lua\lfunc.o lua\lgc.o lua\llex.o lua\lmem.o lua\lobject.o lua\lopcodes.o lua\lparser.o lua\lstate.o lua\lstring.o lua\ltable.o lua\ltm.o lua\lundump.o lua\lvm.o lua\lzio.o	
LIB_O=		lua\lauxlib.o lua\lbaselib.o lua\lcorolib.o lua\ldblib.o lua\lmathlib.o lua\loadlib.o lua\ltablib.o string\string.o string\lstrlib.o sys\sys.o console\console.o lua\liolib.o lua\loslib.o lua\lutf8lib.o compression\compression.o compression\Zip.o compression\lib\miniz.o compression\lib\zip.o lrtapi.o lrtobject.o sys\Date.o sys\File.o sys\Pipe.o sys\Directory.o sys\Buffer.o sys\Com.o lembed.o sys\async.o sys\Task.o
UI_O=  		ui\ui.o ui\Widget.o ui\Entry.o ui\Items.o ui\Menu.o ui\Window.o

BASE_O= 	$(LUA_O) $(LIB_O)
STATIC_O=	..\modules\net\src\Socket.o ..\modules\net\src\Http.o ..\modules\net\src\Ftp.o ..\modules\net\src\net.o ..\modules\crypto\src\Cipher.o ..\modules\crypto\src\crypto.o

LUART_T=	luart.exe
LUAWRT_T=	wluart.exe

ALL_O= $(BASE_O)
ifeq ($(PLATFORM), x86)
REQ= libsensapi.a
ALL_T= libsensapi.a $(LUA_A) $(LUART_T) $(LUAWRT_T) static $(DEST)\rtc.exe $(DEST)\wrtc.exe
else
ALL_T= $(LUA_A) $(LUART_T) $(LUAWRT_T) static $(DEST)\rtc.exe $(DEST)\wrtc.exe
endif
ALL_A= $(LUA_A)

default: toolchain

infolua:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building Lua 5.4.6 VM          "

infocore:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT core framework  "

infotoolchain:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT toolchain       "

infosetup:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT installer       "

lua: infolua $(LUA_O) 

core: infocore $(REQ) $(LIB_O) $(UI_O)

static: 
	@$(RM) $(BASE_O)  >nul 2>&1
	@$(RM) $(STATIC_O) >nul 2>&1
	@$(RM) $(UI_O)  >nul 2>&1
	@cmd /c echo.
	@echo|set /p dummy="▸  Building static executables    "
	@$(MAKE) --no-print-directory luart-static BUILD=$(BUILD) STATIC=TRUE NOMSG=TRUE

toolchain: static
	@$(RM) $(BASE_O)  >nul 2>&1
	@$(RM) $(STATIC_O) >nul 2>&1
	@$(RM) $(UI_O)  >nul 2>&1
	@$(MAKE) --no-print-directory nexttoolchain BUILD=$(BUILD) STATIC=FALSE NOMSG=TRUE

nexttoolchain: lua core $(LUA_A) infotoolchain $(LUART_T) $(LUAWRT_T) $(DEST)\rtc.exe $(DEST)\wrtc.exe
	@cmd /c echo.

all: clean toolchain tools

o:	$(ALL_O)

lua\lvm.o: 
	@$(CC) $(CFLAGS) -O2 -c lua\lvm.c -o lua\lvm.o

%.o : %.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@ 
	@echo|set /p dummy="■"

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@ 
	@echo|set /p dummy="■"

# libsensapi.a must be generated for i686 mingw-w64 (32bits)
libsensapi.a:
	@gendef $(firstword "$(windir)\syswow64\SensApi.dll" "$(windir)\system32\SensApi.dll") >nul 2>&1
	@dlltool -mi386 -f--32 -d SensApi.def -k -l $@
	@$(RM) SensApi.def

debug:
	@$(MAKE) --no-print-directory "BUILD=debug" NOMSG=TRUE

x64: clean
	@$(MAKE) --no-print-directory "PLATFORM=x64" NOMSG=TRUE

x86: clean
	@$(MAKE) --no-print-directory "PLATFORM=x86" NOMSG=TRUE

setup: toolchain tools infosetup
	@echo|set /p dummy="■"
	@..\..\bin\luart.exe makedist.lua $(VERSION) $(PLATFORM)
	@echo|set /p dummy="■"
	@-move /Y LuaRT-$(VERSION)-$(PLATFORM).zip ..\..\LuaRT-$(VERSION)-$(PLATFORM).zip >nul

luart-static: $(BASE_O) $(UI_O) $(STATIC_O)
	@windres $(DEFINES_RES) -I"." -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CXX) -mwindows $(LDFLAGS) $(CFLAGS) -static-libstdc++ -fno-rtti -DRTWIN -DLUART_STATIC $(BASE_O) $(STATIC_O) $(UI_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o wluart-static.exe luart.c $(LIBS) -lgdi32 $(LDSCRIPT)
	@echo|set /p dummy="■"
	@-copy /Y wluart-static.exe $(DEST)\wluart-static.exe >nul
	@windres $(DEFINES_RES) -I"." -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CXX) resource.o $(CFLAGS) -static-libstdc++ -fno-rtti $(LDFLAGS) -DLUART_STATIC -o luart-static.exe $(BASE_O) $(STATIC_O) -mconsole luart.c $(LDFLAGS) $(LIBS) $(LDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y luart-static.exe $(DEST)\luart-static.exe >nul

$(LUA_A): $(BASE_O)
	@$(CXX) -shared $(CFLAGS) -static-libstdc++ -fno-rtti -o $@ $(BASE_O) $(DLLLDSCRIPT) $(LDFLAGS) $(LIBS) -Wl,--out-implib,liblua54.a >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul
	@-copy /Y liblua54.a "..\..\lib\liblua54.a" >nul

$(LUART_T): $(LUA_A) luart.c
	@windres $(DEFINES_RES) -I"." -I"./lua/" resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CC) resource.o $(CFLAGS) $(LDFLAGS) -o $@ -mconsole luart.c $(LDFLAGS) -llua54 $(LIBS) $(LDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul

$(LUAWRT_T): $(LUA_A) $(UI_O) luart.c ui\ui.o
	@windres $(DEFINES_RES) -I"." -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CC) -mwindows $(LDFLAGS) $(CFLAGS) -DRTWIN $(UI_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o $@ luart.c -llua54 $(LIBS) -lgdi32 $(LDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul

$(DEST)\rtc.exe $(DEST)\wrtc.exe: $(LUA_A) $(LUART_T) $(LUAWRT_T) ..\..\tools\rtc\src\rtc.lua
	@-$(RM) $(DEST)\rtc.exe >nul 2>&1
	@-$(RM) $(DEST)\wrtc.exe >nul 2>&1
	@-copy $(DEST)\*.exe ..\..\tools\rtc\src >nul
	@windres $(DEFINES_RES) -D RTC -I"." -I"../../include/lua/" resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CC) resource.o $(CFLAGS) $(LDFLAGS) -o luart.exe -mconsole -DRTC luart.c $(LDFLAGS) -llua54 $(LIBS) $(LDSCRIPT) >nul 2>&1
	@luart.exe ..\..\tools\rtc\src\rtc.lua -o rtc.exe ..\..\tools\rtc\src >nul
	@echo|set /p dummy="■"
	@windres $(DEFINES_RES) -D RTC -I"." -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CC) -mwindows $(CFLAGS) $(UI_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o wluart.exe -DRTC -DRTWIN luart.c -llua54 $(LIBS) -lgdi32 $(LDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@luart.exe ..\..\tools\rtc\src\rtc.lua -o wrtc.exe -w -i ..\..\tools\rtc\src\img\rtc.ico ..\..\tools\rtc\src\rtc.lua ..\..\tools\rtc\src >nul 
	@-copy /Y rtc.exe "$(DEST)\rtc.exe" >nul	
	@-copy /Y wrtc.exe "$(DEST)\wrtc.exe" >nul	
	@$(RM) luart.exe >nul 2>&1
	@$(RM) wluart.exe >nul 2>&1
	@$(RM) ..\..\tools\rtc\src\*.exe >nul 2>&1 

staticui: $(UI_O)

#---- Warning, be sure to use the x86 / x64 LuaRT-Studio git branch accordingly to the PLATFORM target
tools: $(DEST)\rtc.exe
	@rtc.exe -s -w -o rtcheck.exe -i ..\..\setup\img\update.ico ..\..\setup\update.lua ..\..\setup\img\ 2>&1 >nul
	@-copy /Y rtcheck.exe "..\..\tools\LuaRT-Studio\bin\rtcheck.exe" 2>&1 >nul
	@-move /Y rtcheck.exe ..\..\bin 2>&1 >nul	
	@$(MAKE) LUART_PATH=$(shell echo %CD%)\..\.. -C ..\..\tools\RTBuilder --no-print-directory
	@echo|set /p dummy="▸  Building QuickRT/LuaRT Studio  "
	@$(RM) $(UI_O)  >nul 2>&1
	@$(MAKE) --no-print-directory staticui BUILD=$(BUILD) STATIC=TRUE NOMSG=TRUE
	@$(CC) $(CFLAGS) -DAIO -c lrtapi.c -o lrtapi.o
	@echo|set /p dummy="■"
	@$(CXX) -shared $(CFLAGS) -static-libstdc++ -fno-rtti -DAIO -o lua54.dll $(BASE_O) $(UI_O) $(LDFLAGS) $(LIBS) $(DLLLDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@rtc -i ..\..\tools\QuickRT\contrib\quickrt.ico ..\..\tools\QuickRT\src\quickrt.lua -o ..\..\tools\QuickRT\QuickRT.exe ..\..\tools\QuickRT\src >nul
	@-copy /Y lua54.dll ..\..\tools\QuickRT\lua54.dll >nul	
	@echo|set /p dummy="■"	
	@$(CC) $(CFLAGS) -DAIO -DRTCOMPAT -DLUA_BUILD_AS_DLL -c lrtapi.c -o lrtapi.o
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) -DRTWIN -DLUA_BUILD_AS_DLL -c sys/File.c -o sys/File.o
	@echo|set /p dummy="■"
	@$(CXX) $(CFLAGS) -static-libstdc++ -fno-rtti -DAIO -DRTCOMPAT -DLUA_BUILD_AS_DLL -shared -o lua54.dll $(BASE_O) $(UI_O) $(LDFLAGS) $(LIBS) $(DLLLDSCRIPT) >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) -c lrtapi.c -o lrtapi.o
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) -c sys/File.c -o sys/File.o
	@echo|set /p dummy="■"
	@windres $(DEFINES_RES) -D RTWIN -I"." -I"../../include/lua/" resources\resource.rc -o resource.o
	@echo|set /p dummy="■"
	@$(CXX) -static-libstdc++ -fno-rtti resource.o $(CFLAGS) $(LDFLAGS) -o luart.exe -mconsole luart.c $(LDFLAGS) -llua54 $(LIBS) $(LDSCRIPT) >nul 2>&1	
	@echo|set /p dummy="■"
	@-copy /Y luart.exe "..\..\tools\LuaRT-Studio\bin\luart.exe" >nul	
	@-copy /Y ..\..\bin\wrtc.exe "..\..\tools\LuaRT-Studio\bin\wrtc.exe" >nul	
	@-copy /Y lua54.dll "..\..\tools\LuaRT-Studio\bin\lua54.dll" >nul	
	@echo|set /p dummy="■"
	@$(RM) lua54.dll
	@rmdir  "..\..\tools\LuaRT-Studio\examples\" /s /q
	@xcopy "..\..\examples"  "..\..\tools\LuaRT-Studio\examples\" /s /e /y >nul
	@echo|set /p dummy="■"
	@xcopy "..\..\modules"  "..\..\tools\LuaRT-Studio\modules\" /s /e /y >nul
	@echo|set /p dummy="■"
	@cmd /c echo.

clean:	
	@echo|set /p dummy="▸  Cleaning Lua 5.4 VM... "
	@$(RM) $(BASE_O) >nul 2>&1
	@echo ✓
	@echo|set /p dummy="▸  Cleaning LuaRT Core framework... "
	@$(RM) lua54*.dll >nul 2>&1
	@$(RM) liblua54.a >nul 2>&1
	@$(RM) resource.o >nul 2>&1
	@$(RM) libsensapi.a >nul 2>&1
	@$(RM) ..\..\lib\liblua54.a >nul 2>&1
	@$(RM) $(ALL_T) >nul 2>&1
	@$(RM) $(LUART_T) >nul 2>&1
	@$(RM) $(LUAWRT_T)  >nul 2>&1
	@$(RM) $(UI_O) >nul 2>&1
	@echo ✓
	@echo|set /p dummy="▸  Cleaning LuaRT toolchain... "
	@$(RM) $(DEST)\luart*.exe >nul 2>&1
	@$(RM) $(DEST)\wluart*.exe >nul 2>&1
	@$(RM) $(DEST)\lua54*.dll >nul 2>&1
	@$(RM) $(DEST)\rtc.exe >nul 2>&1
	@$(RM) $(DEST)\wrtc.exe >nul 2>&1
	@$(RM) rtc.exe >nul 2>&1
	@$(RM) wrtc.exe >nul 2>&1
	@$(RM) luart*.exe >nul 2>&1
	@$(RM) wluart*.exe >nul 2>&1
	@$(RM) quickrt.exe >nul 2>&1
	@$(RM) ..\..\tools\rtc\src\*.exe >nul 2>&1
	@$(RM) ..\..\setup\luaRT.zip >nul 2>&1
	@$(RM) ..\..\tools\QuickRT\QuickRT.exe >nul 2>&1
	@$(RM) ..\..\tools\QuickRT\lua54.dll >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x64.exe >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x86.exe >nul 2>&1
	@$(RM) ..\..\LuaRT-$(VERSION)-x64.exe >nul 2>&1
	@$(RM) ..\..\LuaRT-$(VERSION)-x86.exe >nul 2>&1
	@$(MAKE) -C ..\..\tools\RTBuilder clean --no-print-directory >nul 2>&1
	@echo ✓

ALL= all


.PHONY: clean
