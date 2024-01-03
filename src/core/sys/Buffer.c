/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Buffer.c | LuaRT Buffer object implementation
*/

#define LUA_LIB

#include <Buffer.h>
#include "lrtapi.h"
#include <luart.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <string.h>

luart_type TBuffer;
static const char* encodings[] = { "utf8", "unicode", "base64", "hex", NULL };

LUA_API Buffer *luart_tobuffer(lua_State *L, int idx) {
	if (lua_isstring(L, idx)) {
		size_t len;
		const char *str = lua_tolstring(L, idx, &len);
		lua_toBuffer(L,(void *)str, len);
		idx = -1;
	}
	return luaL_checkcinstance(L, idx, Buffer);
}

LUA_API void lua_toBuffer(lua_State *L, void *p, size_t len) {
	Buffer b;
	b.bytes = p;
	b.size = len;
	lua_pushlightuserdata(L, &b);
	lua_pushinstance(L, Buffer, 1);
}

static void table_toarray(lua_State *L, int idx, Buffer *b) {
	lua_Integer value;
	size_t i = 0;

	b->size = (size_t)luaL_len(L, idx);
	b->bytes = calloc(1, b->size);
	lua_pushvalue(L, idx);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (lua_type(L, -1) != LUA_TNUMBER)
			luaL_error(L, "invalid value in table in Buffer:from (number expected found %s)", luaL_typename(L, -1));
		value = lua_tointeger(L, -1);
		lua_pop(L, 1);
		if (value < 0 || value > 255) 
			luaL_error(L, "invalid value in table in Buffer:from (byte overflow)");
		b->bytes[i++] = (BYTE)value;
	}
	lua_pop(L, 1);
}

extern size_t posrelatI (lua_Integer pos, size_t len);
extern size_t getendpos (lua_State *L, int arg, lua_Integer def, size_t len);

void base64_decode(Buffer *b) {
	DWORD len = 0, skip = 0, flags = 0;
	if (CryptStringToBinaryA((LPCSTR)b->bytes, b->size, CRYPT_STRING_BASE64, NULL, &len, &skip, &flags))
    {
		BYTE *buff = malloc(sizeof(char)*len);
		if (CryptStringToBinaryA((LPCSTR)b->bytes, b->size, CRYPT_STRING_BASE64, buff, &len, &skip, &flags))
		{
			b->bytes = buff;
			b->size = len;
			return;
		}
		free(buff);
	}
}

unsigned char char2val(lua_State *L, unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else return luaL_error(L, "invalid character '%c' in hexadecimal sequence", c);
}

void hex_decode(lua_State *L, Buffer *b)
{
	unsigned char *result;
    size_t j = 0, i = 0;
	
	if (b->size & 1)
		luaL_error(L, "invalid hexadecimal sequence(odd number of characters)");
	result = malloc(b->size/2);
	while (i < b->size) {
		result[j++] = char2val(L, b->bytes[i])*16 + char2val(L, b->bytes[i+1]);
		i += 2;
	}
	b->bytes = result;
	b->size /= 2;
}

int base64_encode(lua_State *L, Buffer *b) {
	DWORD b64len = 0;
	int result = 0;
	if (CryptBinaryToStringA(b->bytes, b->size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &b64len))
	{
		char *buff = malloc(sizeof(char)*b64len);
		if (CryptBinaryToStringA(b->bytes, b->size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buff, &b64len)) {
			lua_pushlstring(L, buff, b64len);
			result = 1;
		}
		free(buff);
	}
	return result;
}

static void buff_init(lua_State *L, int idx, Buffer *b) {
	BYTE *src = NULL;
	BOOL free_src = FALSE;

	free(b->bytes);
	switch(lua_type(L, idx)) {
		case LUA_TNUMBER:	if ( (b->size = (size_t)luaL_checkinteger(L, idx)) == 0 ) luaL_error(L, "cannot create Buffer with zero length"); break;
		case LUA_TSTRING:	src = (BYTE*)luaL_checklstring(L, idx, &b->size); 
							if ( (idx > 0) && (lua_gettop(L) > 2) ){
								int encoding = luaL_checkoption(L, idx+1, "utf8", encodings);
								int len;
								switch (encoding) {
									case 0: break;
									case 1: free_src = TRUE;
											src = (BYTE*)utf8_towchar((const char *)src, &len);
											b->size = len; break;
									case 2: b->bytes = src;
											base64_decode(b);
											return;
									case 3: b->bytes = src;
											hex_decode(L, b);
											return;
									default:luaL_error(L, "unknown encoding '%s'", encodings[encoding]);  
								}
								
							}
							break;
		case LUA_TTABLE:	table_toarray(L, idx, b); return;
		default:			luaL_error(L, "cannot create Buffer from %s", luaL_typename(L, idx));
	}
	if ( b->size && ((b->bytes = calloc(1, b->size)) == NULL) )
		luaL_error(L, "memory allocation error: not enough memory");
	if (src) {
		memcpy(b->bytes, src, b->size);
		if (free_src)
			free(src);
	}
}

LUA_CONSTRUCTOR(Buffer) {
	Buffer *b = calloc(1, sizeof(Buffer));
	if (lua_islightuserdata(L, 2)) {
		Buffer *from = lua_touserdata(L, 2);
		b->size = from->size;
		b->bytes = malloc(b->size);
		memcpy(b->bytes, from->bytes, b->size);
	}
	else if (!lua_isnil(L, 2))
		buff_init(L, 2, b);
	lua_newinstance(L, b, Buffer);
	return 1;
}

LUA_METHOD(Buffer, sub) {
	Buffer *b, *buff = lua_self(L, 1, Buffer);
	size_t start = posrelatI(luaL_optinteger(L, 2, 0), buff->size); 
	size_t end =  getendpos(L, 3, -1, buff->size);
	
	if (start > end)
		end = buff->size;
	lua_pushnil(L);
	b = lua_pushinstance(L, Buffer, 1);
	b->size = end-start+1;
	b->bytes = malloc(b->size);
	memcpy(b->bytes, buff->bytes+start-1, b->size);
	return 1;
}

LUA_METHOD(Buffer, append) {
	Buffer *b = lua_self(L, 1, Buffer);
	Buffer temp = {0};

	buff_init(L, 2, &temp);
	b->bytes = realloc(b->bytes, b->size + temp.size);
	memcpy(b->bytes+b->size, temp.bytes, temp.size);
	free(temp.bytes);
	b->size += temp.size;
	return 0;
}

LUA_METHOD(Buffer, from) {
	Buffer *b = lua_self(L, 1, Buffer);
	buff_init(L, 2, b);
	return 0;
}

extern int str_pack (lua_State *L);
extern int str_unpack (lua_State *L);

LUA_METHOD(Buffer, pack) {
	Buffer *b = lua_self(L, 1, Buffer);
	int n = lua_gettop(L);

	lua_pushcfunction(L, str_pack);
	lua_insert(L, -n);
	lua_call(L, n-1, 1);
	buff_init(L, -1, b);
	return 0;
}

LUA_METHOD(Buffer, unpack) {
	Buffer *b = lua_self(L, 1, Buffer);
	int nargs = lua_gettop(L);

	lua_pushcfunction(L, str_unpack);
	lua_pushvalue(L, 2);
	lua_pushlstring(L, (const char *)b->bytes, b->size);
	if (nargs == 3)
		lua_pushvalue(L, 3);
	lua_call(L, nargs, LUA_MULTRET);
	return lua_gettop(L)-nargs;
}

LUA_METHOD(Buffer, contains) {
	Buffer *b = lua_self(L, 1, Buffer);
	size_t len;
	const char *str = luaL_tolstring(L, 2, &len);
	const char *pos;
  	const char *last = (const char *) b->bytes + b->size - len;
  	BOOL result = FALSE;

  	if (len && (b->size > len))
	  	for (pos = (const char *)b->bytes; pos <= last; ++pos)
			if (pos[0] == ((const char *) str)[0] && !memcmp ((const void *) &pos[1], (const void *) ((const char *) str + 1), len - 1)) {
				result = TRUE;
				break;
			}
  	if (result)
  		lua_pushinteger(L, pos-(char*)b->bytes+1);
	else
  		lua_pushboolean(L, result);
	return 2;
}

LUA_PROPERTY_GET(Buffer, len) {
	lua_pushinteger(L, lua_self(L, 1, Buffer)->size);
	return 1;
}

LUA_PROPERTY_SET(Buffer, len) {
	Buffer *b = lua_self(L, 1, Buffer);
	b->size = (size_t)luaL_checkinteger(L, 2);
	if ( (b->bytes = realloc(b->bytes, b->size)) == NULL)
		luaL_error(L, "Buffer allocation error: not enough memory");
	return 0;
}

LUA_METHOD(Buffer, __index) {
	Buffer *b = lua_self(L, 1, Buffer);
	lua_Integer idx, i;
	int isnum = 0;

	if ((idx = lua_tointegerx(L, 2, &isnum)) || isnum) {
		idx = lua_tointeger(L, 2);
		i = idx < 0 ? (lua_Integer)(b->size+idx) : idx-1;
		if (i<0 || i > (lua_Integer)(b->size-1))
			luaL_error(L, "out of bounds index for Buffer");
		lua_pushinteger(L, b->bytes[i]);
		return 1;
	}
	return 0;
}

LUA_METHOD(Buffer, __metanewindex) {
	Buffer *b = lua_self(L, 1, Buffer);
	lua_Integer idx = luaL_checkinteger(L, 2);
	lua_Integer i = idx < 0 ? (lua_Integer)(b->size+idx) :idx-1;
	lua_Integer value = luaL_checkinteger(L, 3);
	if (i<0 || i > (lua_Integer)(b->size-1))
		luaL_error(L, "out of bounds index for Buffer");
	if (value<0 || value>255)
		luaL_error(L, "invalid value (byte overflow)");
	b->bytes[i] = (BYTE)value;
	return 0;
}

LUA_METHOD(Buffer, __tostring) {
	Buffer *b = lua_self(L, 1, Buffer);
	lua_pushlstring(L, (const char*)b->bytes, b->size);
	return 1;
}

int hex_encode(lua_State *L, const char *bb, size_t len) {
    static const char xx[]= "0123456789ABCDEF";
	int n = len*2;
	char *xp = malloc(n);
    while (--n >= 0)
		xp[n] = xx[(bb[n>>1] >> ((1 - (n&1)) << 2)) & 0xF];
	lua_pushlstring(L, xp, len*2);
	free(xp);
	return 1;
}

LUA_METHOD(Buffer, encode) {
	int encoding = luaL_checkoption(L, 2, "utf8", encodings);
	Buffer *b = lua_self(L, 1, Buffer);

	switch(encoding) {
		case 0:		lua_pushlstring(L, (const char *)b->bytes, b->size); break;
		case 1:		lua_pushlwstring(L, (wchar_t*)b->bytes, b->size/2); break;
		case 2:		base64_encode(L, b); break;
		case 3:		hex_encode(L, (const char *)b->bytes, b->size); break;
		default:	luaL_error(L, "unknown encoding '%s'", encodings[encoding]); 
	}		
	return 1;
}

LUA_METHOD(Buffer, __len) {
	lua_pushinteger(L, lua_self(L, 1, Buffer)->size);
	return 1;
}

LUA_METHOD(Buffer, __gc) {
	Buffer *b = lua_self(L, 1, Buffer);
	if (b->size)
		free(b->bytes);
	free(b);
	return 0;
}

LUA_METHOD(Buffer, __concat) {
	luaL_tolstring(L, 1, NULL);
	luaL_tolstring(L, 2, NULL);
	lua_concat(L, 2);
	lua_pushinstance(L, Buffer, 1);
	return 1;
}

LUA_METHOD(Buffer, __eq) {
	size_t len1, len2;
	const char *b1 = luaL_tolstring(L, 1, &len1);
	const char *b2 = luaL_tolstring(L, 2, &len2);
	lua_pushboolean(L, (len1 == len2) && (memcmp(b1, b2, len1) == 0));
	return 1;
}

static int Buffer_iter(lua_State *L) {
	Buffer *b = lua_self(L, lua_upvalueindex(1), Buffer);
	size_t pos = (size_t)lua_tointeger(L, lua_upvalueindex(2));
	
	if (pos < b->size) { 
		lua_pushinteger(L, (unsigned char)b->bytes[pos]);
		lua_pushinteger(L, ++pos);
		lua_replace(L, lua_upvalueindex(2));
		return 1;
	}
	return 0;	
}

static int Buffer___iterate(lua_State *L) {
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, Buffer_iter, 2);
	return 1;
}

const luaL_Reg Buffer_metafields[] = {
	{"__gc",			Buffer___gc},
	{"__metaindex",		Buffer___index},
	{"__metanewindex",	Buffer___metanewindex},
	{"__tostring",		Buffer___tostring},
	{"__len",			Buffer___len},
	{"__concat",		Buffer___concat},
	{"__iterate",		Buffer___iterate},
	{"__eq",			Buffer___eq},
	{NULL, NULL}
};

const luaL_Reg Buffer_methods[] = {
	{"sub",			Buffer_sub},
	{"from",		Buffer_from},
	{"pack",		Buffer_pack},
	{"unpack",		Buffer_unpack},
	{"append",		Buffer_append},
	{"contains",	Buffer_contains},
	{"encode",		Buffer_encode},
	{"set_size",	Buffer_setlen},
	{"get_size",	Buffer_getlen},
	{NULL, NULL}
};