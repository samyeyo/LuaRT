/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lrtapi.h | LuaRT internal header
*/

#pragma once


#ifdef __cplusplus
extern "C" {
#endif


#define FDSET_SIZE 1024
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
LUAMOD_API int luaopen_os(lua_State *L);
LUAMOD_API int luaopen_utf8(lua_State *L);
LUAMOD_API int luaopen_com(lua_State *L);

int obj_each_iter(lua_State *L);
extern wchar_t * GetCurrentDir();
extern wchar_t temp_path[MAX_PATH];

#ifdef __cplusplus
}
#endif