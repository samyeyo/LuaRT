/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Union.c | LuaRT Union object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include "Union.h"
#include "Struct.h"

luart_type TUnion;

//-------------------------------------[ Union Constructor ]

// size_t get_bytes(lua_State *L, const char *sig, int i) {
// 	switch(sig[i]) {
// 		case '.': {
// 			Struct *s;
// 			i += 3;
// 			luaL_checktype(L, i, LUA_TTABLE);
// 			lua_pushnil(L);
// 			if (lua_next(L, i))
// 				if ((s = lua_iscinstance(L, -1, TStruct)))
// 					return s->size;
// 			luaL_error(L, "Bad argument #%d (table field is not a Struct)", i);
// 		}
// 		case 'u': {
// 			Union *u;
// 			i += 3;
// 			luaL_checktype(L, i, LUA_TTABLE);
// 			lua_pushnil(L);
// 			if (lua_next(L, i))
// 				if ((u = lua_iscinstance(L, -1, TUnion)))
// 					return u->size;
// 			luaL_error(L, "Bad argument #%d (table field is not an Union)", i);
// 		}
// 		case 'C':
// 		case 'c': 	return sizeof(char);
// 		case 'S':
// 		case 's': 	return sizeof(int16_t);
// 		case 'i':	
// 		case 'I': 	
// 		case 'B': 	return sizeof(int32_t);
// 		case 'j':
// 		case 'l': 	
// 		case 'L': 	return sizeof(int64_t);
// 		case 'f':	return sizeof(float);
// 		case 'd': 	return sizeof(double);
// 		case 'W': 
// 		case 'Z': 
// 		case 'p': 	return sizeof(void*);
// 	}
// 	return luaL_error(L, "Unknown '%c' signature character", sig[i]);
// }

LUA_CONSTRUCTOR(Union) {
	Union *u = (Union*)calloc(1, sizeof(Union));
	Union *from;

	lua_newinstance(L, u, Union);
	if ((from = lua_iscinstance(L, 2, TUnion))) {	
		u->size = from->size;
		if (lua_isuserdata(L, 3)) {
			u->data = lua_touserdata(L, 3);
			u->owned = TRUE;
		}
		else 	
			u->data = calloc(1, from->size);
		u->nfields = from->nfields;
		u->name = from->name;
		u->fields = calloc(u->nfields, sizeof(UField));
		memcpy(u->fields, from->fields, u->nfields*sizeof(UField));
	} else {
		const char *signature = luaL_checklstring(L, 2, &u->nfields);
		size_t largest = 0;
		u->fields = calloc(u->nfields, sizeof(UField));
		for (int i = 0; i < u->nfields; i++) {
			UField *f = &u->fields[i];
			f->type = signature[i];
			f->size = get_bytes(L, signature, i);
			if (f->size > largest)
				largest = f->size;
			if (f->type == '.')
				f->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			else 
				lua_pushvalue(L, i+3);
			f->name = lua_tostring(L, -1);
			lua_pushinteger(L, i);
			lua_rawset(L, -3);
		}
		u->size = largest;
	}
	return 1;
}

static UField *get_field(Union *u, const char *name) {
	for (int i = 0; i < u->nfields; i++)
		if (strcmp(u->fields[i].name, name) == 0)
			return &u->fields[i];
	return NULL;
}

static void setfield(lua_State *L, Union *u, const char *field, int idx) {
	UField *f;
	if ((f = get_field(u, field))) {
		if (!u->data)
			luaL_error(L, "Cannot set field value on a Struct declaration");
		else switch (f->type) {
			case 'C':
			case 'c': 	*(u->data) = luaL_checkstring(L, idx)[0]; break;
			case 'B': 	luaL_checktype(L, idx, LUA_TBOOLEAN);
			case 'S':
			case 's': 	*((int16_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'i':
			case 'I': 	*((int32_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'j':
			case 'J':	*((int64_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'l': 	*((int64_t *)(u->data)) = luaL_checknumber(L, idx); break;
			case 'L': 	*((uint64_t *)(u->data)) = luaL_checknumber(L, idx); break;
			case 'f':	*((float *)(u->data)) = luaL_checknumber(L, idx); break;
			case 'd': 	*((double *)(u->data)) = luaL_checknumber(L, idx); break;
			case 'Z': 	*((char **)(u->data)) = (char*)luaL_checkstring(L, idx); break;
			case 'p': 	{
				luaL_checktype(L, idx, LUA_TLIGHTUSERDATA);
				*((void **)(u->data)) = lua_touserdata(L, idx);
				break;
			}
			case 'W': {
				if (*(u->data))
					free(u->data);
				*((wchar_t**)(u->data)) = lua_towstring(L, idx);
				break;
			}
			default : luaL_error(L, "Unknown '%c' signature character", f->type);
		}
	} else luaL_error(L, "'%s.%s' field is not defined", u->name, field);
}

LUA_METHOD(Union, __call) {
	int init = FALSE;
	Union *u;

	init = lua_istable(L, 2);
	lua_pushvalue(L, 1);
	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);
	if (lua_isuserdata(L, 2))
		lua_pushvalue(L, 2);
	u = lua_pushinstance(L, Union, 1 + lua_isuserdata(L, 2));
	u->name = ar.name;
	if (init) {
		Union *from;
		if ((from = lua_iscinstance(L, 2, TUnion))) {
			memcpy(u->data, from->data, from->size);
		} else {
			lua_pushnil(L);
			while (lua_next(L, 2)) {
				setfield(L, u, luaL_checkstring(L, -2), -1);
				lua_pop(L, 1);
			}
		}
	}
	return 1;
}

LUA_METHOD(Union, __metaindex) {
	Union *u = lua_self(L, 1, Union);
	const char *field = lua_tostring(L, 2);
	UField *f;
	if ((f = get_field(u, field))) {
		if (!u->data)
			lua_pushlstring(L, &u->fields[1].type, 1);
		else switch (f->type) {
			case '.':	lua_rawgeti(L, LUA_REGISTRYINDEX, f->ref); break;
			case 'C':
			case 'c': 	lua_pushlstring(L, (char *)(u->data), 1); break;
			case 'B': 	lua_pushboolean(L, *((int *)(u->data))); break;
			case 'S':	lua_pushinteger(L, *((uint16_t*)(u->data))); break;
			case 's': 	lua_pushinteger(L, *((int16_t*)(u->data))); break;
			case 'i':	lua_pushinteger(L, *((int32_t *)(u->data))); break;
			case 'I': 	lua_pushnumber(L, *((uint32_t*)(u->data))); break;
			case 'j':	lua_pushinteger(L, *((int64_t*)(u->data))); break;
			case 'J':	lua_pushnumber(L, *((uint64_t*)(u->data))); break;
			case 'l': 	lua_pushinteger(L, *((int64_t *)(u->data))); break;
			case 'L': 	lua_pushnumber(L, *((uint64_t *)(u->data))); break;
			case 'f':	lua_pushnumber(L, *((float *)(u->data))); break;
			case 'd': 	lua_pushnumber(L, *((double *)(u->data))); break;
			case 'Z': 	lua_pushstring(L, *(char **)(u->data)); break;
			case 'p': 	lua_pushlightuserdata(L, *(void **)(u->data)); break;
			case 'W': 	lua_pushwstring(L, *(wchar_t **)(u->data)); break;
			default : luaL_error(L, "Unknown '%c' signature character", f->type);
		}
	} else luaL_error(L, "Struct field '%s' is not defined", lua_tostring(L, 2));
	return 1;
}

LUA_METHOD(Union, __metanewindex) {
	setfield(L, lua_self(L, 1, Union), lua_tostring(L, 2), 3);
	return 0;
}

//-------------------------------------[ Union.__gc() ]
LUA_METHOD(Union, __gc) {
	Union *u = lua_self(L, 1, Union);

	for (int i = 0; i < u->nfields; i++)
		if (u->fields[i].ref)
			luaL_unref(L, LUA_REGISTRYINDEX, u->fields[i].ref);
	if (!u->owned)
		free(u->data);
	free(u->fields);
	free(u);
	return 0;
}

LUA_METHOD(Union, __len) {
	lua_pushinteger(L, lua_self(L, 1, Union)->size);
	return 1;
}

LUA_METHOD(Union, __tostring) {
	Union *u = lua_self(L, 1, Union);
	if (u->name)
		lua_pushfstring(L, "%s: %p", u->name, u);
	else
		lua_pushfstring(L, "Union: %p", u);
	return 1;
}

OBJECT_METAFIELDS(Union)
	METHOD(Union, __gc)
	METHOD(Union, __metanewindex)
	METHOD(Union, __metaindex)
	METHOD(Union, __call)
	METHOD(Union, __len)
	METHOD(Union, __tostring)
END

OBJECT_MEMBERS(Union)
	// METHOD(Struct, new)
END
