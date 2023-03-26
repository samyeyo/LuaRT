/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Http.h | LuaRT Http object header
*/

#pragma once

#include <luart.h>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	luart_type		type;
	INTERNET_SCHEME	scheme;
	HINTERNET		h;
	HINTERNET		hcon;
	HINTERNET		request;
	wchar_t			*hostname;
	wchar_t			*username;
	wchar_t			*password;
	int 			port;
	BOOL			active;
} Http;

extern luart_type THttp;

//---------------------------------------- Http object
LUA_CONSTRUCTOR(Http);
extern const luaL_Reg Http_methods[];
extern const luaL_Reg Http_metafields[];

//---------------------------------------- Functions prototypes for Ftp object
LUA_METHOD(Http, open);
LUA_METHOD(Http, __gc);
LUA_METHOD(Http, close);
LUA_PROPERTY_GET(Http, hostname);
LUA_PROPERTY_GET(Http, port);

URL_COMPONENTSW *get_url(lua_State *L, int idx, wchar_t **url_str);
int get_response(lua_State *L, Http *http);
int constructor(lua_State *L, BOOL isFTP);

#ifdef __cplusplus
}
#endif