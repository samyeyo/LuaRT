# | canvas module for LuaRT
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

MODULE=		canvas
VERSION=	1.0
SRC= 		src\Canvas.obj src\Direct2D.obj src\Image.obj src\Gradient.obj

!if "$(BUILD)" == "debug"
CFLAGS = /nologo /D_WIN32_WINNT=0x0603 /DLUA_COMPAT_5_3 /I"$(LUART_PATH)\include" /Z7 /EHsc
LDFLAGS= /nologo /DEBUG
VERBOSE=
PROGRESS= 
!else
CFLAGS = /nologo /D_WIN32_WINNT=0x0603 /DLUA_COMPAT_5_3 /I"$(LUART_PATH)\include" /MD /Gy /GF /GA /GR- /GS- /Oi /O2 /EHsc
LDFLAGS= /nologo /OPT:REF /OPT:ICF
VERBOSE= 2>&1 >nul
PROGRESS= "■"
!endif

LIBS= "$(LUART_PATH)\lib\lua54.lib" d2d1.lib dwrite.lib ole32.lib windowscodecs.lib comctl32.lib user32.lib gdi32.lib
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

.cpp.obj: 
	@$(CC) $(CFLAGS) /c /Fo$@ $< $(VERBOSE)
	@echo|set /p dummy=$(PROGRESS)

$(MODULE).dll: infomodule $(SRC)
	@$(LD) /DLL $(LDFLAGS) $(LIBS) $(SRC) /OUT:$@ $(VERBOSE)
	
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
