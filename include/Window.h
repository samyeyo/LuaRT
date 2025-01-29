/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Window.h | LuaRT Window object header file
*/

#pragma once
#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

LUA_CONSTRUCTOR(Window);

LUA_API luart_type TWindow;

extern const luaL_Reg Window_methods[];
extern const luaL_Reg Window_metafields[];

#ifdef __cplusplus
}
#endif