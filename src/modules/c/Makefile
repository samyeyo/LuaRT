# | c module for LuaRT
# | Luart.org, Copyright (c) Tine Samir 2024.
# | See Copyright Notice in LICENSE.TXT
# |--------------------------------------------------------------
# | Makefile
# | Please set LUART_PATH to your LuaRT folder if autodetection fails
# |--------------------------------------------------------------
# | Usage (default release build)			 : make
# | Usage (debug build) 		  			 : make debug
# | Usage (clean all)	 				 	 : make clean
# |-------------------------------------------------------------
VERBOSE= >nul 2>&1

MODULE=		C 
VERSION=	0.7
SRC= 		src\C.obj src\Library.obj src\Struct.obj src\Value.obj src\Union.obj src\Array.obj src\Pointer.obj

!if "$(BUILD)" == "debug"
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /DLUA_ARCH=\"$(_ARCH)\" /DLUART_MINOR=$(LUART_MINOR) /DLUART_MAJOR=$(LUART_MAJOR) /DLUART_RELEASE=$(LUART_RELEASE) /I"$(LUART_PATH)\include" /I"src/include" /I"." /Z7
LDFLAGS= /nologo /DEBUG
VERBOSE=
PROGRESS= 
!else
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /DLUA_ARCH=\"$(_ARCH)\" /DLUART_MINOR=$(LUART_MINOR) /DLUART_MAJOR=$(LUART_MAJOR) /DLUART_RELEASE=$(LUART_RELEASE) /I"$(LUART_PATH)\include" /I"src/include" /I"." /MD /Gy /GR- /GL /GS- /Oi /O2
LDFLAGS= /nologo /NODEFAULTLIB:libcmt /OPT:REF /OPT:ICF /LTCG
PROGRESS= "■"
!endif

LIBS= "$(LUART_PATH)\lib\lua54.lib" "src/lib/$(PLATFORM)/libdyncall_s.lib" "src/lib/$(PLATFORM)/libdynload_s.lib" user32.lib shlwapi.lib
RM= del /Q
CP= copy /Y
LD= link.exe

all: install

debug:
	@$(MAKE) /nologo "BUILD=debug"

infomodule: 
	@chcp 65001 >nul 2>&1
	@cmd /c echo.
	@echo|set /p dummy="▸  Building module $(MODULE) $(VERSION)	  "

.c.obj: 
	@$(CC) $(CFLAGS) /c /Fo$@ $< $(VERBOSE)
	@echo|set /p dummy=$(PROGRESS)

$(MODULE).dll: infomodule $(SRC)
	@$(LD) /DLL $(LDFLAGS) $(SRC) $(LIBS) /OUT:$@ $(VERBOSE) 
	
install: $(MODULE).dll
!if !exists("$(LUART_PATH)\modules\$(MODULE)")
	@mkdir $(LUART_PATH)\modules\$(MODULE) >nul 2>&1
!endif
	@$(CP) $(MODULE).dll $(LUART_PATH)\\modules\\$(MODULE) >nul 2>&1

clean:
	@echo|set /p dummy="▸  Cleaning $(MODULE) module... "
	@$(RM) $(MODULE).* >nul 2>&1
	@$(RM) $(SRC) >nul 2>&1
	@echo ✓
!if exists("$(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll")
	@$(RM) $(LUART_PATH)\modules\$(MODULE)\$(MODULE).dll >nul 2>&1
!endif
!if exists("$(LUART_PATH)\modules\$(MODULE)")
	@rmdir /S /Q $(LUART_PATH)\modules\$(MODULE) >nul 2>&1 
!endif
