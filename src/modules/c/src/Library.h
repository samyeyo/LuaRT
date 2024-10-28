/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Library.h | LuaRT Library object header
*/

#include <luart.h>
#include "include\dyncall.h"
#include "include\dynload.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Library object
typedef struct {
	luart_type		type;
	DCCallVM 		*vm;
	DCint			mode;
	DLLib			*lib;
	const char		*libname;
} Library;

extern luart_type TLibrary;

LUA_CONSTRUCTOR(Library);
extern const luaL_Reg Library_methods[];
extern const luaL_Reg Library_metafields[];

void *topointer(lua_State *L, int idx);
void *obj_topointer(lua_State *L, void *val, luart_type type);

#ifdef __cplusplus
}
#endif