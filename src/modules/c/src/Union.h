/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Union.h | LuaRT Union object header
*/

#pragma once

#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Union object

typedef struct {
	char 			type;
	int 			ref;
	size_t			size;
	size_t			nelements;
	const char		*name;
} UField;

typedef struct {
	luart_type 		type;
	char 			*data;
	size_t 			size;
	const char 		*name;
	size_t			nfields;
	UField 			*fields;
	UField			*current;		
	BOOL			owned;
} Union;

extern luart_type TUnion;

LUA_CONSTRUCTOR(Union);
extern const luaL_Reg Union_methods[];
extern const luaL_Reg Union_metafields[];

#ifdef __cplusplus
}
#endif