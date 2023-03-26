/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Zip.h | LuaRT Zip object header
*/


#pragma once

#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Zip type

extern luart_type TZip;

struct zip_t;

typedef struct {
	luart_type		type;
	struct zip_t	*zip;
	wchar_t			*fname;
	int				level;
	char			mode;
} Zip;

LUA_CONSTRUCTOR(Zip);
extern const luaL_Reg Zip_methods[];
extern const luaL_Reg Zip_metafields[];

#ifdef __cplusplus
}
#endif