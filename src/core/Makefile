# | LuaRT - A Windows programming framework for Lua
# | Luart.objrg, Copyright (c) Tine Samir 2024
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

#---- Platform selection, leave blank to autodetect
#---- x86 for 32bits lua54.dll, liblua54.a and executables
#---- x64 for 64bits lua54.dll, liblua54.a and executables

PLATFORM=

!if ([cl /? 2>&1 | findstr /C:"x86" > nul] == 0)
PLATFORM=x86
!else
PLATFORM=x64
!endif


#---- Configuration
RM= del /Q
CC= cl
LD= link

DEST= ..\..\bin


!if "$(PLATFORM)" == "x64"
_ARCH = x64
!else
_ARCH = x86
!endif

#---- LuaRT version
LUART_MAJOR= 1
LUART_MINOR= 6
LUART_RELEASE= 0
VERSION= $(LUART_MAJOR).$(LUART_MINOR).$(LUART_RELEASE)
DEFINES_RES= /d LUART_MINOR="$(LUART_MINOR)" /d LUART_MAJOR="$(LUART_MAJOR)" /d LUART_RELEASE="$(LUART_RELEASE)"

!if "$(BUILD)" == "debug"
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /DLUA_ARCH=\"$(_ARCH)\" /DLUART_MINOR=$(LUART_MINOR) /DLUART_MAJOR=$(LUART_MAJOR) /DLUART_RELEASE=$(LUART_RELEASE) /I"..\..\include" /I"." /Z7
LDFLAGS= /nologo /DEBUG /NODEFAULTLIB:msvcrt
!else
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /DLUA_ARCH=\"$(_ARCH)\" /DLUART_MINOR=$(LUART_MINOR) /DLUART_MAJOR=$(LUART_MAJOR) /DLUART_RELEASE=$(LUART_RELEASE) /I"..\..\include" /I"." /MD /Gy /Gw /GF /Oi /O2 /GA
LDFLAGS= /nologo /OPT:REF /OPT:ICF
!endif

DLL_LIBS =	shlwapi.lib advapi32.lib gdi32.lib comctl32.lib kernel32.lib user32.lib oleaut32.lib ole32.lib credui.lib crypt32.lib wininet.lib

LIBS	= 	lua54.lib ole32.lib comctl32.lib
UI_LIBS = 	gdi32.lib uxtheme.lib shell32.lib user32.lib kernel32.lib dwmapi.lib windowscodecs.lib comdlg32.lib ole32.lib shlwapi.lib oleaut32.lib
STATIC_LIBS=ole32.lib comctl32.lib crypt32.lib credui.lib wininet.lib ws2_32.lib Secur32.lib dnsapi.lib iphlpapi.lib urlmon.lib sensapi.lib $(DLL_LIBS)


#---- Source files
LUA_A=		lua54.dll
LUA_O=		lua\lapi.obj lua\lcode.obj lua\lctype.obj lua\ldebug.obj lua\ldo.obj lua\ldump.obj lua\lfunc.obj lua\lgc.obj lua\llex.obj lua\lmem.obj lua\lobject.obj lua\lopcodes.obj lua\lparser.obj lua\lstate.obj lua\lstring.obj lua\ltable.obj lua\ltm.obj lua\lundump.obj lua\lvm.obj lua\lzio.obj	
LIB_O=		lua\lauxlib.obj lua\lbaselib.obj lua\lcorolib.obj lua\ldblib.obj lua\lmathlib.obj lua\loadlib.obj lua\ltablib.obj string\string.obj string\lstrlib.obj sys\sys.obj console\console.obj lua\liolib.obj lua\loslib.obj lua\lutf8lib.obj compression\compression.obj compression\Zip.obj compression\lib\miniz.obj compression\lib\zip.obj lrtapi.obj lrtobject.obj sys\Date.obj sys\File.obj sys\Pipe.obj sys\Directory.obj sys\Buffer.obj sys\Com.obj lembed.obj sys\async.obj sys\Task.obj
UI_O=  		ui\ui.obj ui\Widget.obj ui\Entry.obj ui\Items.obj ui\Menu.obj ui\Window.obj

BASE_O= 	$(LUA_O) $(LIB_O)
STATIC_O=	..\modules\net\src\Socket.obj ..\modules\net\src\Http.obj ..\modules\net\src\Ftp.obj ..\modules\net\src\net.obj ..\modules\crypto\src\Cipher.obj ..\modules\crypto\src\crypto.obj
ALL_O= 		$(BASE_O)

LUART_T=	luart.exe
LUAWRT_T=	wluart.exe
ALL_T= 		$(LUA_A) $(LUART_T) $(LUAWRT_T) luart-static.exe wluart-static.exe $(DEST)\rtc.exe $(DEST)\wrtc.exe

!if "$(STATIC)" == "" && "$(BUILD)" == ""
!message ━━━━━━━━━━━━━━━━━━━━  Building LuaRT $(LUART_MAJOR).$(LUART_MINOR).$(LUART_RELEASE) for $(PLATFORM)  ━━━━━━━━━━━━━━━━━━━━
!endif

all: rtc toolchain

debug:
	@$(MAKE) /nologo "BUILD=debug"

utf8:
	@chcp 65001 >nul 2>&1

default: toolchain

infolua: utf8
	@cmd /c echo.
	@echo|set /p dummy="▸  Building Lua 5.4.6 VM          "

infocore:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT core framework  "

infostatic:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building static executables    "

infotoolchain:
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT toolchain       "

infotools: utf8
	@cmd /c echo.
	@echo|set /p dummy="▸  Building QuickRT/LuaRT Studio  "

infosetup: utf8
	@cmd /c echo.
	@echo|set /p dummy="▸  Building LuaRT installer       "

.cpp.obj:
	@$(CC) /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Fo$@ $< >nul 2>&1
	@echo|set /p dummy="■"

.c.obj: 
	@$(CC) /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Fo$@ $< >nul 2>&1
	@echo|set /p dummy="■"

lua: infolua $(LUA_O) 

core: infocore $(REQ) $(LIB_O) $(UI_O)

toolchain: lua core $(LUA_A) infotoolchain $(LUART_T) $(LUAWRT_T)

lua\lvm.obj: 
!if "$(BUILD)" == "debug"
	@$(CC) /nologo /c /Z7 /Fo$@ lua\lvm.c >nul 2>&1
!else
	@$(CC) /nologo /MD /Gy /GF /Oi /GA /c /Ox /Fo$@ lua\lvm.c >nul 2>&1
!endif

static: 
	@$(RM) $(BASE_O)  >nul 2>&1
	@$(RM) $(STATIC_O) >nul 2>&1
	@$(RM) $(UI_O)  >nul 2>&1
	@cmd /c echo.
	@echo|set /p dummy="▸  Building static executables    "
	@$(MAKE) /nologo luart-static "BUILD=$(BUILD)" "STATIC=TRUE"

tools: infotoolchain toolchain rtc
	@echo|set /p dummy=" "

$(LUA_A): $(BASE_O)
	@$(LD) /DLL $(LDFLAGS) $(DLL_LIBS) $(BASE_O) /OUT:$@ >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul
	@-copy /Y lua54.lib "..\..\lib\lua54.lib" >nul

$(LUART_T): $(LUA_A)
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /i "lua/" resources/resource.rc >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) luart.c /link $(LDFLAGS) resource.res $(LIBS) /OUT:$@ >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul

$(LUAWRT_T): $(BASE_O) $(UI_O)
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /d RTWIN /i "lua/" resources/resource.rc >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) /c /DRTWIN $(CFLAGS) luart.c /Fowluart.obj >nul 2>&1
	@$(LD) $(LDFLAGS) resource.res wluart.obj $(UI_O) $(LIBS) $(UI_LIBS) /OUT:$@ /SUBSYSTEM:windows >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y $@ "$(DEST)\$@" >nul

!if "$(BUILD)" == "debug"
luart-static: 

rtc:

!else
luart-static: $(BASE_O) $(UI_O) $(STATIC_O) infostatic
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /d RTWIN /i "lua/" resources/resource.rc >nul
	@echo|set /p dummy="■"
	@$(CC) /c /DRTWIN /DLUART_STATIC $(CFLAGS) luart.c /Fowluart-static.obj >nul 2>&1
	@$(LD) $(LDFLAGS) resource.res wluart-static.obj $(BASE_O) $(STATIC_O) $(UI_O) $(UI_LIBS) $(STATIC_LIBS) /OUT:wluart-static.exe /SUBSYSTEM:windows >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y wluart-static.exe $(DEST)\wluart-static.exe >nul
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /i "lua/" resources/resource.rc >nul
	@echo|set /p dummy="■"
	@$(CC) /c $(CFLAGS) /DLUART_STATIC luart.c /Foluart-static.obj  >nul 2>&1
	@$(LD) $(LDFLAGS) resource.res luart-static.obj $(BASE_O) $(STATIC_O) $(STATIC_LIBS) /OUT:luart-static.exe >nul 2>&1
	@echo|set /p dummy="■"
	@-copy /Y luart-static.exe $(DEST)\luart-static.exe >nul

rtc: toolchain luart-static ..\..\tools\rtc\src\rtc.lua
	@$(RM) $(DEST)\rtc.exe >nul 2>&1
	@$(RM) $(DEST)\wrtc.exe >nul 2>&1
	@$(RM) $(DEST)\rtcheck.exe >nul 2>&1
	@-copy $(DEST)\*.exe ..\..\tools\rtc\src >nul
	@cmd /c echo.
	@echo|set /p dummy="▸  Building rtc compiler          "
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /d RTC /i "lua/" resources/resource.rc >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) /D RTC luart.c /link $(LDFLAGS) resource.res $(BASE_O) $(STATIC_O) $(STATIC_LIBS) /OUT:luart.exe  >nul 2>&1
	@luart.exe ..\..\tools\rtc\src\rtc.lua -o rtc.exe ..\..\tools\rtc\src  >nul 2>&1
	@echo|set /p dummy="■"
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /d RTC /d RTWIN /i "lua/" resources/resource.rc >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) /c /DRTWIN /DRTC $(CFLAGS) luart.c /Fowluart.obj >nul 2>&1
	@$(LD) $(LDFLAGS) resource.res wluart.obj $(BASE_O) $(STATIC_O) $(UI_O) $(UI_LIBS) $(STATIC_LIBS) /OUT:wluart.exe /SUBSYSTEM:windows >nul 2>&1
	@echo|set /p dummy="■"
	@luart.exe ..\..\tools\rtc\src\rtc.lua -o wrtc.exe -w -i ..\..\tools\rtc\src\img\rtc.ico ..\..\tools\rtc\src\rtc.lua ..\..\tools\rtc\src >nul 2>&1
	@-copy /Y rtc.exe "$(DEST)\rtc.exe" >nul	
	@-copy /Y wrtc.exe "$(DEST)\wrtc.exe" >nul	
	@$(RM) luart.exe >nul 2>&1
	@$(RM) wluart.exe >nul 2>&1
	@$(RM) ..\..\tools\rtc\src\*.exe >nul 2>&1 
!endif

#---- Warning, be sure to use the x86 / x64 LuaRT-Studio git branch accordingly to the PLATFORM target
buildtools: infotools rtc
	@$(CC) /DAIO /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Folrtapi.obj lrtapi.c >nul 2>&1
	@echo|set /p dummy="■"
	@$(LD) /DLL $(LDFLAGS) $(DLL_LIBS) $(UI_LIBS) $(BASE_O) $(UI_O) /OUT:lua54.dll >nul 2>&1
	@echo|set /p dummy="■"
	@rtc -i ..\..\tools\QuickRT\contrib\quickrt.ico ..\..\tools\QuickRT\src\quickrt.lua -o ..\..\tools\QuickRT\QuickRT.exe ..\..\tools\QuickRT\src >nul 2>&1
	@-copy /Y lua54.dll ..\..\tools\QuickRT\lua54.dll >nul	
	@echo|set /p dummy="■"	
	@$(CC) /DRTCOMPAT /DAIO /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Folrtapi.obj lrtapi.c >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) /DRTWIN /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Fosys\File.obj sys\File.c >nul 2>&1
	@echo|set /p dummy="■"	
	@$(LD) /DLL $(LDFLAGS) $(DLL_LIBS) $(UI_LIBS) $(BASE_O) $(UI_O) /OUT:lua54.dll >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Folrtapi.obj lrtapi.c >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) /DLUA_BUILD_AS_DLL $(CFLAGS) /c /Fosys\File.obj sys\File.c >nul 2>&1
	@echo|set /p dummy="■"	
	@rc /fo .\\resource.res $(DEFINES_RES) /r /v /d RTWIN /i "lua/" resources/resource.rc >nul 2>&1
	@echo|set /p dummy="■"
	@$(CC) $(CFLAGS) luart.c /link $(LDFLAGS) resource.res $(LIBS) /OUT:luart.exe >nul 2>&1
	@echo|set /p dummy="■"
	@copy /Y luart.exe "..\..\tools\LuaRT-Studio\bin\luart.exe" >nul	
	@-copy /Y ..\..\bin\wrtc.exe "..\..\tools\LuaRT-Studio\bin\wrtc.exe" >nul	
	@-copy /Y lua54.dll "..\..\tools\LuaRT-Studio\bin\lua54.dll" >nul	
	@echo|set /p dummy="■"
	@$(RM) lua54.dll
	@rmdir  "..\..\tools\LuaRT-Studio\examples\" /s /q
	@xcopy "..\..\examples"  "..\..\tools\LuaRT-Studio\examples\" /s /e /y >nul
	@echo|set /p dummy="■"
	@xcopy "..\..\modules"  "..\..\tools\LuaRT-Studio\modules\" /s /e /y >nul
	@echo|set /p dummy="■"
	@..\..\bin\rtc.exe -s -w -o rtcheck.exe -i ..\..\setup\img\update.ico ..\..\setup\update.lua ..\..\setup\img\ 2>&1 >nul
	@-copy /Y rtcheck.exe "..\..\tools\LuaRT-Studio\bin\rtcheck.exe" 2>&1 >nul
	@-move /Y rtcheck.exe ..\..\bin 2>&1 >nul
	@echo|set /p dummy="■"

setup: toolchain tools buildtools infosetup
	@..\..\bin\luart.exe makedist.lua $(VERSION) $(PLATFORM)
	@echo|set /p dummy="■"
	@-move /Y LuaRT-$(VERSION)-$(PLATFORM).zip ..\..\LuaRT-$(VERSION)-$(PLATFORM).zip >nul
	@cmd /c echo.

clean: utf8
	@echo|set /p dummy="▸  Cleaning Lua 5.4 VM... "
	@$(RM) $(BASE_O) >nul 2>&1
	@echo ✓
	@echo|set /p dummy="▸  Cleaning LuaRT Core framework... "
	@$(RM) *.obj >nul 2>&1
	@$(RM) *.pdb >nul 2>&1
	@$(RM) *.exp >nul 2>&1
	@$(RM) *.lib >nul 2>&1
	@$(RM) *.ilk >nul 2>&1
	@$(RM) lua54.* >nul 2>&1
	@$(RM) resource.res >nul 2>&1
	@$(RM) ..\..\lib\lua54.lib >nul 2>&1
	@$(RM) $(ALL_T) >nul 2>&1
	@$(RM) $(LUART_T) >nul 2>&1
	@$(RM) $(LUAWRT_T)  >nul 2>&1
	@$(RM) $(UI_O) >nul 2>&1
	@echo ✓
	@echo|set /p dummy="▸  Cleaning LuaRT toolchain... "
	@$(RM) $(DEST)\*.exe >nul 2>&1
	@$(RM) rtc.exe >nul 2>&1
	@$(RM) wrtc.exe >nul 2>&1
	@$(RM) luart*.exe >nul 2>&1
	@$(RM) wluart*.exe >nul 2>&1
	@$(RM) quickrt.exe >nul 2>&1
	@$(RM) ..\..\tools\rtc\src\*.exe >nul 2>&1
	@$(RM) ..\..\setup\luaRT.zip >nul 2>&1
	@$(RM) ..\..\tools\QuickRT\QuickRT.exe >nul 2>&1
	@$(RM) ..\..\tools\QuickRT\lua54.dll >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x64.* >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x86.* >nul 2>&1
	@$(RM) ..\..\LuaRT-$(VERSION)-x64.* >nul 2>&1
	@$(RM) ..\..\LuaRT-$(VERSION)-x86.* >nul 2>&1
	@echo ✓