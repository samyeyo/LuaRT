# | audio module for LuaRT
# | Luart.org, Copyright (c) Tine Samir 2023.
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

MODULE=audio
VERSION=0.2

ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifeq ($(LUART_PATH),)
LUART_PATH=$(shell luart.exe -e "print(sys.registry.read('HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT', 'InstallLocation', false) or '')"  2>nul)
endif

ifeq ($(PLATFORM),)
PLATFORM=$(shell $(LUART_PATH)\bin\luart.exe -e "print(_ARCH) " 2>nul)
endif

ifeq ($(PLATFORM),x86)
ARCH = -m32 -march=pentium4 -msse -msse2 -mmmx -fno-unwind-tables -fno-asynchronous-unwind-tables -DMA_NO_AVX2
SCRIPT=-Wl,-T$(LUART_PATH)\src\core\x86-dll.ldscript
endif

ifeq ($(PLATFORM),x64)
ARCH = -m64 -mavx2 -mfma
SCRIPT=-Wl,-T$(LUART_PATH)\src\core\x64-dll.ldscript
endif

ifeq ($(LUART_PATH),)
$(info )
$(info > LuaRT is not installed on this computer)
$(info > Please set the LuaRT installation path manually in the Makefile, or download and install the LuaRT toolchain)
$(error )
endif
endif

SRC= src\audio.o src\sound.o src\ma_reverb_node.o

LUART_INCLUDE = -I$(LUART_PATH)\include
LUART_LIB = -L$(LUART_PATH)\lib 

BUILD := release

cflags.release = $(ARCH) -shared -s -Os -mfpmath=sse -mieee-fp -DLUART_TYPE=__decl(dllimport) -D__MINGW64__ -D_WIN32_WINNT=0x600 $(LUART_INCLUDE) -static-libgcc -fno-exceptions -fdata-sections -ffunction-sections -fipa-pta -ffreestanding -fno-stack-check -fno-ident -fomit-frame-pointer -Wl,--gc-sections -Wl,--build-id=none -Wl,-O1 -Wl,--as-needed -Wl,--no-insert-timestamp -Wl,--no-seh -DMA_ENABLE_ONLY_SPECIFIC_BACKENDS -DMA_ENABLE_WASAPI
ldflags.release= -Wl,--no-insert-timestamp $(LUART_LIB) $(SCRIPT)
cflags.debug = $(ARCH) -shared -g -O0 -mfpmath=sse -mieee-fp -DDEBUG -D__MINGW64__ -D_WIN32_WINNT=0x0600 $(LUART_INCLUDE) 
ldflags.debug= $(LUART_LIB)

CC= gcc
CFLAGS := ${cflags.${BUILD}}
LDFLAGS := ${ldflags.${BUILD}}
LIBS= -llua54
RM= del /Q
CP= copy /Y

default: all

all: install

infomodule: 
	@chcp 65001 >nul 2>&1
	@cmd /c echo.
	@echo|set /p dummy="▸  Building module $(MODULE) $(VERSION)	  "

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo|set /p dummy="■"

$(MODULE).dll: infomodule $(SRC)
	@$(CC) $(CFLAGS)  $(LDFLAGS) $(SRC) -o $@ $(LIBS)
	

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
	@echo|set /p dummy="▸  Cleaning $(MODULE) module... "
	@$(RM) $(SRC) >nul 2>&1
	@echo ✓
ifneq ($(wildcard $(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll),)
	@$(RM) $(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll >nul 2>&1
endif
ifneq ($(wildcard $(LUART_PATH)\modules\$(MODULE)),)
	@rmdir /S /Q $(LUART_PATH)\modules\$(MODULE) >nul 2>&1
endif

