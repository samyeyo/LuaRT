/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lembed.c | LuaRT embed module implementation
*/

#include <luart.h>
#include <File.h>

extern int Zip_extract(lua_State *L); 
extern struct zip_t *fs;

LUA_METHOD(embed, File) {
    wchar_t tmp[MAX_PATH];

	GetTempPathW(MAX_PATH, tmp);
    lua_getglobal(L, "embed");
    lua_pushcfunction(L, Zip_extract);
    lua_getfield(L, -2, "zip");
    lua_pushvalue(L, 1);
    lua_pushwstring(L, tmp);
    lua_pcall(L, 3, LUA_MULTRET, 0);
    return 1;
}

static const luaL_Reg embedlib[] = {
	{"File",	embed_File},
	{NULL, NULL}
};

static const luaL_Reg embed_properties[] = {
	{NULL, NULL}
};

//-------------------------------------------------[luaopen_embed() "embed" module]
int luaopen_embed(lua_State *L) {
	lua_registermodule(L, "embed", embedlib, embed_properties, luaL_embedclose);
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaded");
    lua_getfield(L, -1, "zip");
    lua_pushstring(L, "zip");
    lua_getfield(L, -2, "Zip");
    lua_pushlightuserdata(L, fs);
    lua_pcall(L, 1, 1, 0);
    lua_rawset(L, -6);
    lua_pop(L, 3);
	return 1;
}
