/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Struct.c | LuaRT Struct object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include "Struct.h"
#include "Union.h"

luart_type TStruct;

//-------------------------------------[ Struct Constructor ]

size_t get_bytes(lua_State *L, const char *sig, int i) {
	switch(sig[i]) {
		case '.': {
			Struct *s;
			i += 3;
			luaL_checktype(L, i, LUA_TTABLE);
			lua_pushnil(L);
			if (lua_next(L, i))
				if ((s = lua_iscinstance(L, -1, TStruct)))
					return s->size;
			luaL_error(L, "Bad argument #%d (table field is not a Struct)", i);
		}
		case 'u': {
			Union *u;
			i += 3;
			luaL_checktype(L, i, LUA_TTABLE);
			lua_pushnil(L);
			if (lua_next(L, i))
				if ((u = lua_iscinstance(L, -1, TUnion)))
					return u->size;
			luaL_error(L, "Bad argument #%d (table field is not an Union)", i);
		}
		case 'C':
		case 'c': 	return sizeof(char);
		case 'S':
		case 's': 	return sizeof(int16_t);
		case 'i':	
		case 'I': 	
		case 'B': 	return sizeof(int32_t);
		case 'j':
		case 'l': 	
		case 'L': 	return sizeof(int64_t);
		case 'f':	return sizeof(float);
		case 'd': 	return sizeof(double);
		case 'W': 
		case 'Z': 
		case 'p': 	return sizeof(void*);
	}
	return luaL_error(L, "Unknown '%c' signature character", sig[i]);
}

static size_t get_offset(lua_State *L, const char *sig, int idx, size_t *padding, size_t *largest, size_t *last) {
	size_t i, size, bytes;
	bytes = get_bytes(L, sig, idx);
	if (((sig[idx] != '.') || (sig[idx] != 'u'))) {
		if (bytes < *last)
			bytes = *last;
		if (bytes > *largest)
			*largest = bytes;
		*last = bytes;
	}
	if (sig[idx+1] == '\0')
		*padding += *padding % *largest;
	*padding += bytes; 
	return *padding-bytes;
}

void init_field_struct(lua_State *L, Struct *s, Struct *from) {
	luart_type type;
	for (size_t i = 0; i < s->nfields; i++) {
		if (s->fields[i].type == '.') {
			lua_rawgeti(L, LUA_REGISTRYINDEX, from->fields[i].ref);
			lua_pushlightuserdata(L, s->data + s->fields[i].offset);
			lua_call(L, 1, 1);
			lua_tocinstance(L, -1, &type);
			s->fields[i].ref = luaL_ref(L, LUA_REGISTRYINDEX);
		} else if (s->fields[i].type == 'u') {
			lua_rawgeti(L, LUA_REGISTRYINDEX, from->fields[i].ref);
			lua_pushlightuserdata(L, s->data + s->fields[i].offset);
			lua_call(L, 1, 1);
			lua_tocinstance(L, -1, &type);
			s->fields[i].ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
	}
}

LUA_CONSTRUCTOR(Struct) {
	Struct *s = (Struct*)calloc(1, sizeof(Struct));
	Struct *from;

	lua_newinstance(L, s, Struct);
	if ((from = lua_iscinstance(L, 2, TStruct))) {	
		s->size = from->size;
		if (lua_isuserdata(L, 3)) {
			s->data = lua_touserdata(L, 3);
			s->owned = TRUE;
		}
		else 	
			s->data = calloc(1, from->size);
		s->nfields = from->nfields;
		s->name = from->name;
		s->fields = calloc(s->nfields, sizeof(Field));
		memcpy(s->fields, from->fields, s->nfields*sizeof(Field));
		init_field_struct(L, s, from);
	} else {
		const char *signature = luaL_checklstring(L, 2, &s->nfields);
		size_t padding = 0, largest = 0, last = 0;
		s->fields = calloc(s->nfields, sizeof(Field));
		for (int i = 0; i < s->nfields; i++) {
			Field *f = &s->fields[i];
			f->type = signature[i];
			f->offset = get_offset(L, signature, i, &padding, &largest, &last);
			f->size = padding-f->offset;
			if (f->type == '.' || f->type == 'u')
				f->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			else 
				lua_pushvalue(L, i+3);
			f->name = lua_tostring(L, -1);
			lua_pushinteger(L, i);
			lua_rawset(L, -3);
		}
		s->size = padding;
	}
	return 1;
}

static Field *get_field(Struct *s, const char *name) {
	for (int i = 0; i < s->nfields; i++)
		if (strcmp(s->fields[i].name, name) == 0)
			return &s->fields[i];
	return NULL;
}

static void setfield(lua_State *L, Struct *s, const char *field, int idx) {
	Field *f;
	if ((f = get_field(s, field))) {
		if (!s->data)
			luaL_error(L, "Cannot set field value on a <cdef> Struct");
		else switch (f->type) {
			case 'C':
			case 'c': 	*(s->data+f->offset) = luaL_checkstring(L, idx)[0]; break;
			case 'B': 	luaL_checktype(L, idx, LUA_TBOOLEAN);
			case 'S':
			case 's': 	*((int16_t *)(s->data+f->offset)) = lua_tointeger(L, idx); break;
			case 'i':
			case 'I': 	*((int32_t *)(s->data+f->offset)) = lua_tointeger(L, idx); break;
			case 'j':
			case 'J':	*((int64_t *)(s->data+f->offset)) = lua_tointeger(L, idx); break;
			case 'l': 	*((int64_t *)(s->data+f->offset)) = luaL_checknumber(L, idx); break;
			case 'L': 	*((uint64_t *)(s->data+f->offset)) = luaL_checknumber(L, idx); break;
			case 'f':	*((float *)(s->data+f->offset)) = luaL_checknumber(L, idx); break;
			case 'd': 	*((double *)(s->data+f->offset)) = luaL_checknumber(L, idx); break;
			case 'Z': 	*((char **)(s->data+f->offset)) = (char*)luaL_checkstring(L, idx); break;
			case 'p': 	{
				luaL_checktype(L, idx, LUA_TLIGHTUSERDATA);
				*((void **)(s->data+f->offset)) = lua_touserdata(L, idx);
				break;
			}
			case 'W': {
				if (*(s->data+f->offset))
					free(s->data+f->offset);
				*((wchar_t**)(s->data+f->offset)) = lua_towstring(L, idx);
				break;
			}
			default : luaL_error(L, "Unknown '%c' signature character", f->type);
		}
	} else luaL_error(L, "'%s.%s' field is not defined", s->name, field);
}

LUA_METHOD(Struct, __call) {
	int init = FALSE;
	Struct *s;

	init = lua_istable(L, 2);
	lua_pushvalue(L, 1);
	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);
	if (lua_isuserdata(L, 2))
		lua_pushvalue(L, 2);
	s = lua_pushinstance(L, Struct, 1 + lua_isuserdata(L, 2));
	s->name = ar.name;
	if (init) {
		Struct *from;
		if ((from = lua_iscinstance(L, 2, TStruct))) {
			memcpy(s->data, from->data, from->size);
		} else {
			lua_pushnil(L);
			while (lua_next(L, 2)) {
				setfield(L, s, luaL_checkstring(L, -2), -1);
				lua_pop(L, 1);
			}
		}
	}
	return 1;
}

LUA_METHOD(Struct, __metaindex) {
	Struct *s = lua_self(L, 1, Struct);
	const char *field = lua_tostring(L, 2);
	Field *f;
	if ((f = get_field(s, field))) {
		if (!s->data)
			lua_pushlstring(L, &s->fields[1].type, 1);
		else switch (f->type) {
			case 'u':
			case '.':	lua_rawgeti(L, LUA_REGISTRYINDEX, f->ref); break;
			case 'C':
			case 'c': 	lua_pushlstring(L, (char *)(s->data+f->offset), 1); break;
			case 'B': 	lua_pushboolean(L, *((int *)(s->data+f->offset))); break;
			case 'S':	lua_pushinteger(L, *((uint16_t*)(s->data+f->offset))); break;
			case 's': 	lua_pushinteger(L, *((int16_t*)(s->data+f->offset))); break;
			case 'i':	lua_pushinteger(L, *((int32_t *)(s->data+f->offset))); break;
			case 'I': 	lua_pushnumber(L, *((uint32_t*)(s->data+f->offset))); break;
			case 'j':	lua_pushinteger(L, *((int64_t*)(s->data+f->offset))); break;
			case 'J':	lua_pushnumber(L, *((uint64_t*)(s->data+f->offset))); break;
			case 'l': 	lua_pushinteger(L, *((int64_t *)(s->data+f->offset))); break;
			case 'L': 	lua_pushnumber(L, *((uint64_t *)(s->data+f->offset))); break;
			case 'f':	lua_pushnumber(L, *((float *)(s->data+f->offset))); break;
			case 'd': 	lua_pushnumber(L, *((double *)(s->data+f->offset))); break;
			case 'Z': 	lua_pushstring(L, *(char **)(s->data+f->offset)); break;
			case 'p': 	lua_pushlightuserdata(L, *(void **)(s->data+f->offset)); break;
			case 'W': 	lua_pushwstring(L, *(wchar_t **)(s->data+f->offset)); break;
			default : luaL_error(L, "Unknown '%c' signature character", f->type);
		}
	} else luaL_error(L, "Struct field '%s' is not defined", lua_tostring(L, 2));
	return 1;
}

LUA_METHOD(Struct, __metanewindex) {
	setfield(L, lua_self(L, 1, Struct), lua_tostring(L, 2), 3);
	return 0;
}

//-------------------------------------[ Struct.__gc() ]
LUA_METHOD(Struct, __gc) {
	Struct *s = lua_self(L, 1, Struct);

	if (s->data) {
		for (int i = 0; i < s->nfields; i++) {
			if (s->fields[i].type == 'W')
				free(*((void**)(s->data + s->fields[i].offset)));
			if (s->fields[i].ref)
				luaL_unref(L, LUA_REGISTRYINDEX, s->fields[i].ref);
		}
	}
	if (!s->owned)
		free(s->data);
	free(s->fields);
	free(s);
	return 0;
}

LUA_METHOD(Struct, __len) {
	lua_pushinteger(L, lua_self(L, 1, Struct)->size);
	return 1;
}

LUA_METHOD(Struct, __tostring) {
	Struct *s = lua_self(L, 1, Struct);
	if (s->name)
		lua_pushfstring(L, "%s: %p", s->name, s->data);
	else
		lua_pushfstring(L, "Struct: %p", s);
	return 1;
}

OBJECT_METAFIELDS(Struct)
	METHOD(Struct, __gc)
	METHOD(Struct, __metanewindex)
	METHOD(Struct, __metaindex)
	METHOD(Struct, __call)
	METHOD(Struct, __len)
	METHOD(Struct, __tostring)
END

OBJECT_MEMBERS(Struct)
	// METHOD(Struct, new)
END
