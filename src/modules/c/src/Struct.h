/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Struct.h | LuaRT Struct object header
*/

#pragma once

#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Struct object

typedef struct {
	char 			type;
	unsigned short 	offset;
	int 			ref;
	const char		*name;
	int				size;
	size_t			nelements;
} Field;

typedef struct {
	luart_type 		type;
	char 			*data;
	size_t 			size;
	const char 		*name;
	size_t			nfields;
	Field 			*fields;
	BOOL			owned;
} Struct;

extern luart_type TStruct;

LUA_CONSTRUCTOR(Struct);
extern const luaL_Reg Struct_methods[];
extern const luaL_Reg Struct_metafields[];

size_t get_bytes(lua_State *L, const char *sig, int i);

#ifdef __cplusplus
}
#endif