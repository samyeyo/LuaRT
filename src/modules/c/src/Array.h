/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Array.h | LuaRT Array object header
*/

#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Array object

typedef Array {
	luart_type 	type;
	char		signature;
	size_t		dimensions;
	size_t 		size;
} Array;

extern luart_type TArray;

LUA_CONSTRUCTOR(Array);
extern const luaL_Reg Array_methods[];
extern const luaL_Reg Array_metafields[];

#ifdef __cplusplus
}
#endif