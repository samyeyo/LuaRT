# | Webview module for LuaRT
# | Luart.org, Copyright (c) Tine Samir 2022.
# | See Copyright Notice in LICENSE.TXT
# |--------------------------------------------------------------
# | Makefile
# | Please set LUART_PATH to your LuaRT folder if autodetection fails
# |--------------------------------------------------------------
# | Usage (default release build)			 : make
# | Usage (debug build) 		  			 : make debug
# | Usage (clean all)	 				 	 : make clean
# |-------------------------------------------------------------


#---- LuaRT installation path (set it manually if autodetection fails)
LUART_PATH=..\..\..

ifneq ("$(wildcard $(PATH_TO_FILE))","")
    $(error LuaRT toolchain not compiled. Please compile it before to compile modules)
endif

MODULE=webview
VERSION=0.5

SRC= src\handler.o src\webview.o src\WebView2Loader.o

ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifeq ($(LUART_PATH),)
LUART_PATH=$(shell luart.exe -e "print(sys.registry.read('HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT', 'InstallLocation', false) or '')"  2>nul)
endif

ifeq ($(PLATFORM),)
PLATFORM=$(shell $(LUART_PATH)\bin\luart.exe -e "print(_ARCH) " 2>nul)
endif

ifeq ($(PLATFORM),x86)
ARCH = -m32 -march=pentium4 -msse -msse2 -mmmx -fno-unwind-tables -fno-asynchronous-unwind-tables
SCRIPT=-Wl,-T$(LUART_PATH)\src\core\x86-dll.ldscript
endif

ifeq ($(PLATFORM),x64)
ARCH = -m64 -mavx2 -mfma -fno-unwind-tables -fno-asynchronous-unwind-tables
SCRIPT=-Wl,-T$(LUART_PATH)\src\core\x64-dll.ldscript
endif

ifeq ($(LUART_PATH),)
$(info )
$(info > LuaRT is not installed on this computer)
$(info > Please set the LuaRT installation path manually in the Makefile, or download and install the LuaRT toolchain)
$(error )
endif
endif


LUART_INCLUDE = -I$(LUART_PATH)\include -I.\WebView\include
LUART_LIB = -L$(LUART_PATH)\lib 

BUILD := release

cflags.release = $(ARCH) -shared -s -Os -mfpmath=sse -mieee-fp -DLUART_TYPE=__decl(dllimport) -D__MINGW64__ -D_WIN32_WINNT=0x0700 $(LUART_INCLUDE) -static-libgcc -static-libstdc++ -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections -fipa-pta -ffreestanding -fno-stack-check -fno-ident -fomit-frame-pointer -Wl,--gc-sections -Wl,--build-id=none -Wl,-O1 -Wl,--as-needed -Wl,--no-insert-timestamp -Wl,--no-seh
ldflags.release= -Wl,--no-insert-timestamp $(LUART_LIB)
cflags.debug = $(ARCH) -shared -g -O0 -mfpmath=sse -mieee-fp -DDEBUG -D__MINGW64__ -D_WIN32_WINNT=0x0700 $(LUART_INCLUDE)
ldflags.debug= $(LUART_LIB)

CXX= g++
CFLAGS := ${cflags.${BUILD}}
LDFLAGS := ${ldflags.${BUILD}}
LIBS= -llua54 -lcomctl32 -lole32 -lversion
RM= del /Q
CP= copy /Y

default: all

all: install

infomodule: 
	@chcp 65001 >nul 2>&1
	@cmd /c echo.
	@echo|set /p dummy="▸  Building module $(MODULE) $(VERSION)	  "

%.o : %.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@
	@echo|set /p dummy="■"

$(MODULE).dll: infomodule $(SRC)
	@$(CXX) $(CFLAGS) $(LDFLAGS) $(SRC) -o $@ $(LIBS)
	

debug:
	@$(MAKE) "BUILD=debug"

.PHONY: clean all

install: $(MODULE).dll
ifeq ($(wildcard $(LUART_PATH)\modules\$(MODULE)),)
	@mkdir $(LUART_PATH)\modules\$(MODULE) >nul 2>&1
endif
	@$(CP) $(MODULE).dll $(LUART_PATH)\modules\$(MODULE) >nul 2>&1

clean:
ifneq ($(wildcard $(MODULE).dll),)
	@$(RM) $(MODULE).dll >nul 2>&1
endif
	@$(RM) $(SRC) >nul 2>&1
ifneq ($(wildcard $(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll),)
	@$(RM) $(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll >nul 2>&1
endif
ifneq ($(wildcard $(LUART_PATH)\modules\$(MODULE)),)
	@rmdir /S /Q $(LUART_PATH)\modules\$(MODULE) >nul 2>&1
endif