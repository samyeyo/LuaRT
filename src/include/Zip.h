/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Zip.h | LuaRT Zip object header
*/


#pragma once

#include <luart.h>
#include <compression\lib\zip.h>


typedef struct {
	luart_type		type;
	struct zip_t	*zip;
	wchar_t			*fname;
	int				level;
	char			mode;
} Zip;

//---------------------------------------- Zip type

extern luart_type TZip;

LUA_CONSTRUCTOR(Zip);
extern const luaL_Reg Zip_methods[];
extern const luaL_Reg Zip_metafields[];
