/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Buffer.h | LuaRT Buffer object header
*/

#pragma once

#include <luart.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	luart_type		type;
	size_t			size;
	BYTE			*bytes;
} Buffer;

extern luart_type TBuffer;

LUA_CONSTRUCTOR(Buffer);
extern const luaL_Reg Buffer_methods[];
extern const luaL_Reg Buffer_metafields[];

#define lua_pushbuffer(L, p, len) lua_toBuffer(L, (p), (len))
void lua_toBuffer(lua_State *L, void *p, size_t len);
Buffer *luart_tobuffer(lua_State *L, int idx);
int base64_encode(lua_State *L, Buffer *b);

#ifdef __cplusplus
}
#endif