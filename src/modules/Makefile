# | LuaRT - A Windows programming framework for Lua
# | Luart.org, Copyright (c) Tine Samir 2023
# | See Copyright Notice in LICENSE.TXT
# |--------------------------------------------------------------
# | LuaRT Makefile for binary modules
# |--------------------------------------------------------------
# | Usage (default release build)			 : make
# | Usage (debug build) 		  			 : make debug
# | Usage (release with tools)		 		 : make all
# | Usage (clean all)	 				 	 : make clean
# |-------------------------------------------------------------

TOPTARGETS := all clean debug

SUBDIRS := $(wildcard */.)

all clean debug: $(SUBDIRS)

$(SUBDIRS):
	@chcp 65001 >nul 2>&1
	@$(MAKE) -C $@ $(MAKECMDGOALS) --no-print-directory

.PHONY: all clean debug $(SUBDIRS)