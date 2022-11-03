# | LuaRT - A Windows programming framework for Lua
# | Luart.org, Copyright (c) Tine Samir 2022.
# | See Copyright Notice in LICENSE.TXT
# |--------------------------------------------------------------
# | LuaRT Makefile
# | You can prefix all the following tasks with PLATFORM=x64|x86  
# |--------------------------------------------------------------
# | Usage (default release build)			 : make
# | Usage (debug build) 		  			 : make debug
# | Usage (release build with QuickRT) 		 : make all
# | Usage (build QuickRT only)		 		 : make quickrt
# | Usage (build setup executable)	  		 : make setup
# | Usage (build only rtc) 				 	 : make rtc
# | Usage (clean all)	 				 	 : make clean
# |-------------------------------------------------------------
# | Or you can use default release build for any platform 
# | Usage (default x64 release build)		 : make x64
# | Usage (default x86 release build)		 : make x86
# |-------------------------------------------------------------
# |	LuaRT-Studio uses a specific Lua54.dll 
# | Usage 				 				 	 : make luart_studio

#---- Platform selection
#---- x86 for 32bits lua54.dll, liblua54.a and executables
#---- x64 for 64bits lua54.dll, liblua54.a and executables

PLATFORM ?= x64

#---- LuaRT version
LUART_MAJOR= 1
LUART_MINOR= 1
LUART_RELEASE= 0
VERSION= $(LUART_MAJOR).$(LUART_MINOR).$(LUART_RELEASE)

ifeq ($(PLATFORM), x86)
$(info --------------------------------- Building for Windows x86 Platform)
 ARCH = -m32 -march=pentium4 -msse -msse2 -mmmx -fno-unwind-tables -fno-asynchronous-unwind-tables 
 _ARCH = x86
 TARGET = pe-i386
 ifeq "$(wildcard $(windir)\system32\SensApi.dll)" ""
	$(error $(PLATFORM) build requires "$(windir)\system32\SensApi.dll")
 endif
endif
ifeq ($(PLATFORM), x64)
ifneq ($(MAKECMDGOALS),x86)
$(info --------------------------------- Building for Windows x64 Platform)
endif
 ARCH = -m64 -mavx2 -mfma -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-int-conversion
 _ARCH = x64
 TARGET = pe-x86-64
endif

DEFINES= -DLUART_MINOR=$(LUART_MINOR) -DLUART_MAJOR=$(LUART_MAJOR) -DLUART_RELEASE=$(LUART_RELEASE)
DEFINES_RES= --target=$(TARGET) -D LUART_MINOR=$(LUART_MINOR) -D LUART_MAJOR=$(LUART_MAJOR) -D LUART_RELEASE=$(LUART_RELEASE)

BUILD := release

DEST= ..\bin
cflags.release = $(DEFINES) $(ARCH) -s -static-libgcc -std=c99 -Os -mfpmath=sse -mieee-fp -DLUA_ARCH=\"$(_ARCH)\" -D__MINGW64__ -D_WIN32_WINNT=0x0600 -DLUA_COMPAT_5_3 -I".\include" -Wall -Wextra -I"." -I".\\lua\\"  -fno-exceptions -fdata-sections -ffunction-sections -fipa-pta -ffreestanding -fno-stack-check -fno-ident -fomit-frame-pointer -Wl,--gc-sections -Wl,--build-id=none -Wl,-O1 -Wl,--as-needed -Wl,--no-insert-timestamp -Wl,--no-seh -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-parameter -Wno-implicit-fallthrough
ldflags.release= -s -static-libgcc -lm -Wl,--no-insert-timestamp -Wl,--no-seh -lgdi32 -L"." 
cflags.debug = $(DEFINES) $(ARCH) -static-libgcc -std=c99 -g -O0 -mfpmath=sse -mieee-fp -mmmx -msse -msse2 -DDEBUG -DLUA_ARCH=\"$(_ARCH)\" -D__MINGW64__ -D_WIN32_WINNT=0x0600 -DLUA_COMPAT_5_3 -I".\include" -I"." -I".\\lua\\"
ldflags.debug= -g -static-libgcc -lm -lgdi32 -L"."

CC= gcc
CFLAGS := ${cflags.${BUILD}}
LDFLAGS := ${ldflags.${BUILD}}
LIBS= -lshlwapi -loleaut32 -lole32 -lcrypt32 -lwininet -luuid
LUART_LIBS= -lshlwapi -lcomctl32 -lws2_32 -lSecur32 -ldnsapi -lcrypt32 -lcredui -luxtheme -loleaut32 -lole32 -lwininet -liphlpapi -lwindowscodecs -ldwmapi -lsensapi -lcomdlg32 -luuid
RM= del /Q

LUA_A=		lua54.dll
CORE_O=		lua\lapi.o lrtapi.o lrtobject.o lua\lcode.o lua\lctype.o lua\ldebug.o lua\ldo.o lua\ldump.o lua\lfunc.o lua\lgc.o lua\llex.o lua\lmem.o lua\lobject.o lua\lopcodes.o lua\lparser.o lua\lstate.o lua\lstring.o lua\ltable.o lua\ltm.o lua\lundump.o lua\lvm.o lua\lzio.o
OBJECTS_O=	sys\Date.o sys\File.o sys\Pipe.o sys\Directory.o sys\Buffer.o sys\Com.o
LIB_O=		$(LIBOBJ_O) lua\lauxlib.o lua\lbaselib.o lua\lcorolib.o lua\ldblib.o lua\lmathlib.o lua\loadlib.o lua\ltablib.o string\string.o sys\sys.o console\console.o lua\liolib.o lua\loslib.o lua\lutf8lib.o
LUART_LIB_O= crypto\crypto.o net\net.o lembed.o compression\compression.o
LUART_OBJ_O= crypto\Cipher.o net\Socket.o net\Http.o net\Ftp.o compression\Zip.o compression\lib\miniz.o compression\lib\zip.o
LUART_UI_O=  ui\ui.o ui\Widget.o ui\Entry.o ui\Items.o ui\Menu.o ui\Window.o
BASE_O= 	$(CORE_O) $(LIB_O) $(OBJECTS_O)

LUART_T=	luart.exe
LUAWRT_T=	wluart.exe

ALL_O= $(BASE_O)
ifeq ($(PLATFORM), x86)
ALL_T= libsensapi.a $(LUA_A) $(LUART_T) $(LUAWRT_T) luart-static.exe wluart-static.exe rtc
else
ALL_T= $(LUA_A) $(LUART_T) $(LUAWRT_T) luart-static.exe wluart-static.exe rtc
endif
ALL_A= $(LUA_A)

default: $(ALL_T)

all: clean $(ALL_T) quickrt
	@echo --------------------------------- LuaRT has been successfully compiled

o:	$(ALL_O)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	$(info $<)

# libsensapi.a must be generated for i686 mingw-w64 (32bits)
libsensapi.a:
	@echo Generating $@
	@gendef $(firstword "$(windir)\syswow64\SensApi.dll" "$(windir)\system32\SensApi.dll") >nul 2>&1
	@dlltool -mi386 -f--32 -d SensApi.def -k -l $@
	@$(RM) SensApi.def

debug:
	$(MAKE) "BUILD=debug"

x64: clean
	@$(MAKE) "PLATFORM=x64"

x86: clean
	@$(MAKE) "PLATFORM=x86"

setup: $(ALL_T) quickrt
	@windres $(DEFINES_RES) -I"./lua/" -D RTWIN resources\resource_install.rc -o resource.o
	@$(CC) -mwindows $(CFLAGS) -DRTWIN $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) $(BASE_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o wluart-static.exe luart.c $(LUART_LIBS) -lgdi32
	@..\bin\luart.exe makedist.lua $(VERSION) $(PLATFORM)
	@-copy /Y LuaRT-$(VERSION)-$(PLATFORM).exe ..\LuaRT-$(VERSION)-$(PLATFORM).exe >nul
	@echo --------------------------------- LuaRT setup has been successfully compiled

wluart-static.exe: $(BASE_O) $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) luart.c ui\ui.o
	@windres $(DEFINES_RES) -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	$(info Linking wluart-static.exe...)
	@$(CC) -mwindows $(CFLAGS) -DRTWIN $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O)  $(BASE_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o wluart-static.exe luart.c $(LUART_LIBS) -lgdi32
	@-copy /Y wluart-static.exe $(DEST)\wluart-static.exe >nul

luart-static.exe: $(BASE_O) $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) luart.c ui\ui.o
	@windres $(DEFINES_RES)  -I"./lua/" resources\resource.rc -o resource.o
	$(info Linking luart-static.exe...)
	@$(CC) resource.o $(CFLAGS) $(LDFLAGS) -o luart-static.exe $(LUART_LIB_O) $(LUART_OBJ_O) $(BASE_O) -mconsole luart.c $(LDFLAGS) $(LUART_LIBS) 
	@-copy /Y luart-static.exe $(DEST)\luart-static.exe >nul
	
$(LUA_A): $(BASE_O)
	$(info Linking lua54.dll...)
	@$(CC) $(CFLAGS) -DLUA_BUILD_AS_DLL -shared -o $@ $(BASE_O) $(LIBS) $(LDFLAGS) -Wl,--out-implib,liblua54.a
	@-copy /Y $@ "$(DEST)\$@" >nul

$(LUART_T): $(LUA_A) $(LUART_LIB_O) $(LUART_OBJ_O) luart.c
	@windres $(DEFINES_RES)  -I"./lua/" resources\resource.rc -o resource.o
	$(info Linking luart.exe...)
	@$(CC) resource.o $(CFLAGS) $(LDFLAGS) -o $@ $(LUART_LIB_O) $(LUART_OBJ_O) -mconsole luart.c $(LDFLAGS) -llua54 $(LUART_LIBS) -flto=auto
	@-copy /Y $@ "$(DEST)\$@" >nul

$(LUAWRT_T): $(LUA_A) $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) luart.c ui\ui.o
	@windres $(DEFINES_RES)  -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	$(info Linking wluart.exe...)
	@$(CC) -mwindows $(CFLAGS) -DRTWIN $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o $@ luart.c -llua54 $(LUART_LIBS) -lgdi32 -flto=auto
	@-copy /Y $@ "$(DEST)\$@" >nul

quickrt: rtc
	@rtc -w -i ..\tools\QuickRT\contrib\quickrt.ico ..\tools\QuickRT\src\quickrt.lua -o ..\tools\QuickRT\QuickRT.exe ..\tools\QuickRT\src
	@-copy /Y ..\bin\lua54.dll ..\tools\QuickRT\lua54.dll >nul

rtc: $(LUA_A) $(LUART_T) $(LUAWRT_T) luart-static.exe wluart-static.exe
	@$(RM) $(DEST)\rtc.exe >nul 2>&1
	@$(RM) $(DEST)\wrtc.exe >nul 2>&1
	@-copy $(DEST)\*.exe ..\tools\rtc\src >nul
	@echo Compiling rtc.exe...
	@windres $(DEFINES_RES)  -I"./lua/" resources\resource.rc -o resource.o
	@$(CC) resource.o $(CFLAGS) $(LDFLAGS) -o luart.exe $(LUART_LIB_O) $(LUART_OBJ_O) -mconsole -DRTC luart.c $(LDFLAGS) -llua54 $(LUART_LIBS) -flto=auto
	@luart.exe ..\tools\rtc\src\rtc.lua -o rtc.exe ..\tools\rtc\src >nul
	@echo Compiling wrtc.exe...
	@windres $(DEFINES_RES)  -I"./lua/" -D RTWIN resources\resource.rc -o resource.o
	@$(CC) -mwindows $(CFLAGS) $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) -Wl,--no-insert-timestamp -Wl,--no-seh resource.o -L"." -o wluart.exe -DRTC -DRTWIN luart.c -llua54 $(LUART_LIBS) -lgdi32 
	@luart.exe ..\tools\rtc\src\rtc.lua -o wrtc.exe -w -i ..\tools\rtc\src\img\rtc.ico ..\tools\rtc\src\rtc.lua ..\tools\rtc\src
	@-copy /Y rtc.exe "$(DEST)\rtc.exe" >nul	
	@-copy /Y wrtc.exe "$(DEST)\wrtc.exe" >nul	
	@$(RM) luart.exe >nul 2>&1
	@$(RM) wluart.exe >nul 2>&1
	@$(RM) ..\tools\rtc\src\*.exe >nul 2>&1

#---- Warning, be sure to use the x86 / x64 LuaRT-Studio git branch accordingly to the PLATFORM target
luart_studio: rtc
	@$(RM) lrtapi.o
	@$(CC) $(CFLAGS) -DRTCOMPAT -c lrtapi.c -o lrtapi.o
	$(info lrtapi.c)
	$(info Linking lua54.dll for LuaRT-Studio...)
	@$(CC) $(CFLAGS) -DLUA_BUILD_AS_DLL -shared -o lua54.dll $(BASE_O) $(LUART_LIB_O) $(LUART_OBJ_O) $(LUART_UI_O) $(LDFLAGS) $(LUART_LIBS)
	@-copy /Y ..\bin\wluart.exe "..\tools\LuaRT-Studio\bin\luart.exe" >nul	
	@-copy /Y ..\bin\wrtc.exe "..\tools\LuaRT-Studio\bin\wrtc.exe" >nul	
	@-copy /Y lua54.dll "..\tools\LuaRT-Studio\bin\lua54.dll" >nul	
	@$(RM) lrtapi.o
	@$(CC) $(CFLAGS) -c lrtapi.c -o lrtapi.o
	$(info done)
	
clean:
	@$(RM) $(ALL_T) >nul 2>&1
	@$(RM) $(LUART_T) >nul 2>&1
	@$(RM) $(LUAWRT_T)  >nul 2>&1
	@$(RM) $(LUART_LIB_O) >nul 2>&1
	@$(RM) $(LUART_UI_O) >nul 2>&1
	@$(RM) $(LUART_OBJ_O) >nul 2>&1
	@$(RM) $(BASE_O) >nul 2>&1
	@$(RM) $(DEST)\luart*.exe >nul 2>&1
	@$(RM) $(DEST)\wluart*.exe >nul 2>&1
	@$(RM) $(DEST)\lua54*.dll >nul 2>&1
	@$(RM) $(DEST)\rtc.exe >nul 2>&1
	@$(RM) $(DEST)\wrtc.exe >nul 2>&1
	@$(RM) lua54*.dll >nul 2>&1
	@$(RM) liblua54.a >nul 2>&1
	@$(RM) resource.o >nul 2>&1
	@$(RM) rtc.exe >nul 2>&1
	@$(RM) wrtc.exe >nul 2>&1
	@$(RM) luart*.exe >nul 2>&1
	@$(RM) wluart*.exe >nul 2>&1
	@$(RM) quickrt.exe >nul 2>&1
	@$(RM) libsensapi.a >nul 2>&1
	@$(RM) ..\tools\rtc\src\*.exe >nul 2>&1
	@$(RM) ..\setup\luaRT.zip >nul 2>&1
	@$(RM) ..\tools\QuickRT\QuickRT.exe >nul 2>&1
	@$(RM) ..\tools\QuickRT\lua54.dll >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x64.exe >nul 2>&1
	@$(RM) LuaRT-$(VERSION)-x86.exe >nul 2>&1
	@$(RM) ..\LuaRT-$(VERSION)-x64.exe >nul 2>&1
	@$(RM) ..\LuaRT-$(VERSION)-x86.exe >nul 2>&1
	@echo LuaRT cleaning done

ALL= all

.PHONY: all clean

lvm.o: 
	$(CC) $(CFLAGS) -c -O2 lvm.c -o $@ 

lua\lapi.o: lua\lapi.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lapi.h lua\llimits.h lua\lstate.h \
 lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldebug.h lua\ldo.h lua\lfunc.h lua\lgc.h lua\lstring.h \
 lua\ltable.h lua\lundump.h lua\lvm.h
lua\lauxlib.o: lua\lauxlib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h
lua\lbaselib.o: lua\lbaselib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\lcode.o: lua\lcode.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lcode.h lua\llex.h lua\lobject.h \
 lua\llimits.h lua\lzio.h lua\lmem.h lua\lopcodes.h lua\lparser.h lua\ldebug.h lua\lstate.h lua\ltm.h \
 lua\ldo.h lua\lgc.h lua\lstring.h lua\ltable.h lua\lvm.h
lua\lcorolib.o: lua\lcorolib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\lctype.o: lua\lctype.c lua\lprefix.h lua\lctype.h lua\lua.h lua\luaconf.h lua\llimits.h
lua\ldblib.o: lua\ldblib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\ldebug.o: lua\ldebug.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lapi.h lua\llimits.h lua\lstate.h \
 lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\lcode.h lua\llex.h lua\lopcodes.h lua\lparser.h \
 lua\ldebug.h lua\ldo.h lua\lfunc.h lua\lstring.h lua\lgc.h lua\ltable.h lua\lvm.h
lua\ldo.o: lua\ldo.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lapi.h lua\llimits.h lua\lstate.h \
 lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldebug.h lua\ldo.h lua\lfunc.h lua\lgc.h lua\lopcodes.h \
 lua\lparser.h lua\lstring.h lua\ltable.h lua\lundump.h lua\lvm.h
lua\ldump.o: lua\ldump.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lobject.h lua\llimits.h lua\lstate.h \
 lua\ltm.h lua\lzio.h lua\lmem.h lua\lundump.h
lua\lfunc.o: lua\lfunc.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h lua\lobject.h \
 lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lfunc.h lua\lgc.h
lua\lgc.o: lua\lgc.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h lua\lobject.h \
 lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lfunc.h lua\lgc.h lua\lstring.h lua\ltable.h
lua\liolib.o: lua\liolib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\llex.o: lua\llex.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lctype.h lua\llimits.h lua\ldebug.h \
 lua\lstate.h lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lgc.h lua\llex.h lua\lparser.h \
 lua\lstring.h lua\ltable.h
lua\lmathlib.o: lua\lmathlib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\lmem.o: lua\lmem.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h lua\lobject.h \
 lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lgc.h
lua\loadlib.o: lua\loadlib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\lobject.o: lua\lobject.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lctype.h lua\llimits.h \
 lua\ldebug.h lua\lstate.h lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lstring.h lua\lgc.h \
 lua\lvm.h
lua\lopcodes.o: lua\lopcodes.c lua\lprefix.h lua\lopcodes.h lua\llimits.h lua\lua.h lua\luaconf.h
lua\loslib.o: lua\loslib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\lparser.o: lua\lparser.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lcode.h lua\llex.h lua\lobject.h \
 lua\llimits.h lua\lzio.h lua\lmem.h lua\lopcodes.h lua\lparser.h lua\ldebug.h lua\lstate.h lua\ltm.h \
 lua\ldo.h lua\lfunc.h lua\lstring.h lua\lgc.h lua\ltable.h
lua\lrtapi.o: lua\lrtapi.c lua\luart.h lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h
lua\lstate.o: lua\lstate.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lapi.h lua\llimits.h lua\lstate.h \
 lua\lobject.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldebug.h lua\ldo.h lua\lfunc.h lua\lgc.h lua\llex.h \
 lua\lstring.h lua\ltable.h
lua\lstring.o: lua\lstring.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h \
 lua\lobject.h lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lstring.h lua\lgc.h
string\string.o: string\string.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\ltable.o: lua\ltable.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h lua\lobject.h \
 lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lgc.h lua\lstring.h lua\ltable.h lua\lvm.h
lua\ltablib.o: lua\ltablib.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\lauxlib.h lua\lualib.h
lua\ltm.o: lua\ltm.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h lua\lobject.h \
 lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lgc.h lua\lstring.h lua\ltable.h lua\lvm.h
lua\lundump.o: lua\lundump.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\ldebug.h lua\lstate.h \
 lua\lobject.h lua\llimits.h lua\ltm.h lua\lzio.h lua\lmem.h lua\ldo.h lua\lfunc.h lua\lstring.h lua\lgc.h \
 lua\lundump.h
lua\lzio.o: lua\lzio.c lua\lprefix.h lua\lua.h lua\luaconf.h lua\llimits.h lua\lmem.h lua\lstate.h \
 lua\lobject.h lua\ltm.h lua\lzio.h

# LuaRT standard objects
sys\File.o: sys\File.c include\File.h include\Buffer.h include\luart.h
sys\Directory.o: sys\Directory.c include\Directory.h include\File.h include\luart.h
sys\Pipe.o: sys\Pipe.c include\Pipe.h include\File.h include\Buffer.h include\luart.h
sys\Buffer.o: sys\Buffer.c include\Buffer.h include\luart.h
sys\Date.o: sys\Date.c include\Date.h include\luart.h
sys\Com.o: sys\Com.c include\Com.h include\luart.h
compression\Zip.o: compression\Zip.c include\Zip.h include\luart.h

 # LuaRT library modules
sys\sys.o: sys\sys.c include\Date.h include\File.h include\Buffer.h include\luart.h lrtapi.h
console\console.o: console\console.c include\Date.h include\File.h include\Buffer.h include\luart.h lrtapi.h
compression\zip.o: compression\zip.c include\File.h compression\lib\zip.h compression\lib\miniz.h \
 include\File.h include\Buffer.h include\luart.h lrtapi.h
ui\Widget.o: ui\Widget.c ui\Widget.h include\luart.h lrtapi.h
ui\ui.o: ui\ui.c ui\Widget.h include\luart.h lrtapi.h