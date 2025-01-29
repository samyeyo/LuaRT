# | tracker module for LuaRT
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
VERBOSE= >nul 2>&1

MODULE=		tracker
VERSION=	0.5
SRC= 		src\tracker.obj

LIBS= "$(LUART_PATH)"\lib\lua54.lib gdi32.lib kernel32.lib user32.lib comctl32.lib
RM= del /Q
CP= copy /Y
LD= link.exe

#---- Uncomment to manually set LuaRT installation path 
#LUART_PATH=

!if "$(LUART_PATH)" == ""
!message ** LuaRT is not installed on this computer.
!message ** Please install it or set the LuaRT installation path manually in the Makefile
!message
!else

!if "$(BUILD)" == "debug"
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /I"$(LUART_PATH)\include" /I"." /Z7
LDFLAGS= /nologo /DEBUG
VERBOSE=
!else
CFLAGS = /nologo /D_WIN32_WINNT=0x0600 /DLUA_COMPAT_5_3 /DLUART_RELEASE=$(LUART_RELEASE) /I"$(LUART_PATH)\include" /I"." /MD /MD /Gy /Gw /GF /Oi /O2 /GA
LDFLAGS= /nologo /OPT:REF /OPT:ICF
!endif

all: $(MODULE).dll

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

clean:
	@echo|set /p dummy="▸  Cleaning $(MODULE) module... "
	@$(RM) $(MODULE).* >nul 2>&1
	@$(RM) $(SRC) >nul 2>&1
	@echo ✓

!endif
