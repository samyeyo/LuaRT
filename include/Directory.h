/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Directory.h | LuaRT Directory object header
*/

#include <luart.h>
#include <File.h>
#include <Date.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Functions prototypes
void init_fullpath(lua_State *L, File *f);

//---------------------------------------- Directory object
typedef File Directory;

extern luart_type TDirectory;

LUA_CONSTRUCTOR(Directory);
extern const luaL_Reg Directory_methods[];
extern const luaL_Reg Directory_metafields[];

#ifdef __cplusplus
}
#endif