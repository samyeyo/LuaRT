/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Buffer.h | LuaRT Buffer object header
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <luart.h>
#include <stdlib.h>


typedef struct {
	luart_type		type;
	size_t			size;
	BYTE			*bytes;
	int				encoding;
} Buffer;

LUA_API luart_type TBuffer;

LUA_CONSTRUCTOR(Buffer);
extern const luaL_Reg Buffer_methods[];
extern const luaL_Reg Buffer_metafields[];

#define lua_pushbuffer(L, p, len) lua_toBuffer(L, (p), (len))
LUA_API void lua_toBuffer(lua_State *L, void *p, size_t len);
LUA_API Buffer *luart_tobuffer(lua_State *L, int idx);
int base64_encode(lua_State *L, Buffer *b);

#ifdef __cplusplus
}
#endif