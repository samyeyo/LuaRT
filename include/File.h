/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | File.h | LuaRT File object header
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <luart.h>
#include <Date.h>
#include <stdlib.h>

typedef struct {
	luart_type	type;
	FILE		*stream;
	FILE		*stdstream;
	HANDLE		h;
	BOOL		exists;
	BOOL		std;
	int			encoding;
	SYSTEMTIME	modified;
	SYSTEMTIME	accessed;
	SYSTEMTIME	created;
	wchar_t		*fname;
	wchar_t		*fullpath;
	int			mode;
} File;

extern luart_type TFile;

typedef enum { ASCII, UTF8, UNICODE } Encoding;

//---------------------------------------- Console.stdout get property
extern int console_getstdout(lua_State *L);

//---------------------------------------- File object
LUA_CONSTRUCTOR(File);
extern const luaL_Reg File_methods[];
extern const luaL_Reg File_metafields[];

LUA_METHOD(File, gc);
LUA_METHOD(File, move);
LUA_METHOD(File, remove);
LUA_PROPERTY_GET(File, filename);
LUA_PROPERTY_GET(File, parent);
LUA_PROPERTY_GET(File, path);
LUA_PROPERTY_GET(File, fullpath);
LUA_PROPERTY_GET(File, hidden);
LUA_PROPERTY_GET(File, created);
LUA_PROPERTY_GET(File, modified);
LUA_PROPERTY_GET(File, accessed);
LUA_PROPERTY_SET(File, hidden);
LUA_PROPERTY_SET(File, created);
LUA_PROPERTY_SET(File, modified);
LUA_PROPERTY_SET(File, accessed);

LUALIB_API wchar_t *luaL_checkFilename(lua_State *L, int idx);

//---------------------------------------- TemporaryFile type
LUA_CONSTRUCTOR(TemporaryFile);

//---------------------------------------- Pipe type
LUA_CONSTRUCTOR(Pipe);
extern const luaL_Reg Pipe_methods[];
extern const luaL_Reg Pipe_metafields[];

#ifdef __cplusplus
}
#endif
