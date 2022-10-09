/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lrtapi.h | LuaRT internal header
*/

#pragma once

#define FDSET_SIZE 1024
#include <lua.h>
#include <luart.h>

//--- Utility macro to calculate UTF8 char size in bytes
#define utf8_charsize(c) (((0xE5000000 >> ((((unsigned char)*c) >> 3) & 0x1E)) & 3) + 1)

//--- Utility functions for UTF8 <=> Wide string conversions
wchar_t *utf8_towchar(const char *str, int *len);
char *wchar_toutf8(const wchar_t *str, int *len);

//--- LuaRT standard modules init functions
LUAMOD_API int luaopen_sys(lua_State *L);
LUAMOD_API int luaopen_crypto(lua_State *L);
LUAMOD_API int luaopen_compression(lua_State *L);
LUAMOD_API int luaopen_net(lua_State *L);
LUAMOD_API int luaopen_ui(lua_State *L);
LUAMOD_API int luaopen_console(lua_State *L);
LUAMOD_API int luaopen_embed(lua_State *L);
LUAMOD_API int luaopen_io(lua_State *L);
//--- Lua standard modules included in LuaRT compatiblity version
LUAMOD_API int luaopen_os(lua_State *L);
LUAMOD_API int luaopen_com(lua_State *L);

//--- Pushes Windows system error string on stack
int lasterror(lua_State *L, DWORD err);

int obj_each_iter(lua_State *L);