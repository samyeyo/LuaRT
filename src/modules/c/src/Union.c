/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
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
#include "Value.h"
#include "Array.h"

luart_type TUnion;

//-------------------------------------[ Union Constructor ]

LUA_CONSTRUCTOR(Union) {
	Union *u = (Union*)calloc(1, sizeof(Union));
	Union *from;

	lua_newinstance(L, u, Union);
	if ( (lua_isuserdata(L, 2) && (from = lua_touserdata(L, 2))) || (from = lua_iscinstance(L, 2, TUnion))) {	
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
		const char *signature = luaL_checkstring(L, 2);
		char *cleaned = strdup(signature);
		size_t siglen = strlen(signature);
		removeBrackets(cleaned);
		u->nfields = strlen(cleaned);
		free(cleaned);
		size_t largest = 0, fidx = -1;
		u->fields = calloc(u->nfields, sizeof(UField));
		int idx = 0;
		for (int i = 0; i < siglen; i++) {
			UField *f = &u->fields[++fidx];
			f->type = signature[i];
			f->size = get_bytes(L, signature, i);
			if (signature[i+1] == '[') {
				char* end;
				size_t nelements = strtol(signature + i + 2, &end, 10); 
				if (*end == ']') {
					i = end-(signature+i); 
					f->size *= nelements;
					f->nelements = nelements;
				} else {
					if (nelements)
						luaL_error(L, "Malformed array signature '%c[]'", signature[idx]);
					else luaL_error(L, "Expecting valid number for %c[] array length",  signature[idx]);
				} 
			}
			if (f->size > largest)
				largest = f->size;
			if (f->type == '.' || f->type == 'u')
				f->ref = luaL_ref(L, LUA_REGISTRYINDEX);
			else 
				lua_pushvalue(L, idx+3);
			if (!(f->name = lua_tostring(L, -1)))
				luaL_error(L, "bad argument #%d (expecting string, found nil)", idx+2);
			lua_pushinteger(L, idx);
			lua_rawset(L, -3);
			idx++;
		}
		u->size = largest;
	}
	return 1;
}


LUA_PROPERTY_GET(Union, size) {
	lua_pushinteger(L, lua_self(L, 1, Union)->size);
	return 1;
}

LUA_PROPERTY_GET(Union, type) {
	Union *u = lua_self(L, 1, Union);
	lua_pushstring(L, u->data ? u->name : "Union<cdef>");
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
		if  (f->nelements) {
			switch(lua_type(L, idx)) {
				case LUA_TSTRING:	if (f->type == 'w') {
										int len;
										wchar_t *str = lua_tolwstring(L, idx, &len);
										memcpy(u->data, str, len < f->size ? len : f->size);
										free(str);
									} else {
										size_t len;
										const char *str = lua_tolstring(L, idx, &len);
										memcpy(u->data, str, len < f->size ? len : f->size);
									} break;

				case LUA_TTABLE:	void *obj, *ptr;
									luart_type type;
									size_t len;
									if((obj = lua_tocinstance(L, idx, &type)) && (ptr = obj_topointer(L, obj, type, &len))) {
										memcpy(u->data, ptr, len < f->size ? len : f->size);
										break;
									}
				default:			luaL_error(L, "cannot set array field '%s' with %s", f->name, luaL_typename(L, idx));										
			}
		} else switch (f->type) {
			case 'u':	Union *uu = luaL_checkcinstance(L, idx, Union);
						memcpy(u->data, uu->data, uu->size);
						break;
			case '.':   Struct *s = luaL_checkcinstance(L, idx, Struct);
						memcpy(u->data, s->data, s->size);
						break;
			case 'C':
			case 'c': 	*(u->data) = lua_isinteger(L, idx) ? lua_tointeger(L, idx) : luaL_checkstring(L, idx)[0]; break;
			case 'w':   if (lua_isinteger(L, idx))
							 *((wchar_t*)u->data) = lua_tointeger(L, idx);
						else {
							wchar_t *str = lua_towstring(L, idx);
							 *((wchar_t*)u->data) = str[0];
							free(str);
						} break;
			case 'B': 	luaL_checktype(L, idx, LUA_TBOOLEAN);
			case 'S':
			case 's': 	*((int16_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'i':
#ifndef WIN64
			case '#':
#endif
			case 'I': 	*((int32_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'j':
			case 'J':	*((int64_t *)(u->data)) = lua_tointeger(L, idx); break;
			case 'l': 	*((int64_t *)(u->data)) = luaL_checknumber(L, idx); break;
#ifdef WIN64
			case '#':
#endif			
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
		u->current = f;
	} else luaL_error(L, "'%s.%s' field is not defined", u->name, field);
}

LUA_METHOD(Union, __call) {
	int init = FALSE;
	Union *u;
	Union *uu = lua_tocinstance(L, 1, NULL);

	init = lua_istable(L, 2);
	lua_pushvalue(L, 1);
	if (uu->data)
		luaL_error(L, "bad self to create new Union value (expecting Union cdef, found Union value)");
	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);
	uu->name = ar.name;
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
		if (f->nelements) {
			Array a = {0};
			a.ndim = 1;
			a.dimensions[0] = f->nelements;
			a.size = f->size/f->nelements;
			a.kind = sigchar_toctype(L, f->type);
			lua_pushlightuserdata(L, &a);
			lua_pushlightuserdata(L, u->data);
			Array *new = lua_pushinstance(L, Array, 2);
			if (a.kind > _wstring) {
				lua_rawgeti(L, LUA_REGISTRYINDEX, f->ref);
				new->Struct = lua_tocinstance(L, -1, NULL);
				lua_pop(L, 1);
			}
		} else switch (f->type) {
			case 'u':
			case '.':	lua_rawgeti(L, LUA_REGISTRYINDEX, f->ref);
						lua_pushlightuserdata(L, u->data);
						lua_call(L, 1, 1);
						break;
			case 'C':	lua_pushinteger(L, *(unsigned char *)u->data); break;
			case 'c': 	lua_pushlstring(L, (char *)u->data, 1); break;
			case 'w':	lua_pushlwstring(L,  (wchar_t *)u->data, 1); break;
			case 'B': 	lua_pushboolean(L, *((int *)(u->data))); break;
			case 'S':	lua_pushinteger(L, *((uint16_t*)(u->data))); break;
			case 's': 	lua_pushinteger(L, *((int16_t*)(u->data))); break;
			case 'i':	lua_pushinteger(L, *((int32_t *)(u->data))); break;
#ifndef WIN64
			case '#':
#endif			
			case 'I': 	lua_pushnumber(L, *((uint32_t*)(u->data))); break;
			case 'j':	lua_pushinteger(L, *((int64_t*)(u->data))); break;
			case 'J':	lua_pushnumber(L, *((uint64_t*)(u->data))); break;
			case 'l': 	lua_pushinteger(L, *((int64_t *)(u->data))); break;
#ifdef WIN64
			case '#':
#endif			
			case 'L': 	lua_pushnumber(L, *((uint64_t *)(u->data))); break;
			case 'f':	lua_pushnumber(L, *((float *)(u->data))); break;
			case 'd': 	lua_pushnumber(L, *((double *)(u->data))); break;
			case 'Z': 	lua_pushstring(L, *(char **)(u->data)); break;
			case 'p': 	lua_pushlightuserdata(L, *(void **)(u->data)); break;
			case 'W': 	lua_pushwstring(L, *(wchar_t **)(u->data)); break;
			default : luaL_error(L, "Unknown '%c' signature character", f->type);
		}
	} else luaL_error(L, "Union field '%s' is not defined", lua_tostring(L, 2));
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
		lua_pushfstring(L, "Union<cdef>: %p", u);
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
	READONLY_PROPERTY(Union, size)
	READONLY_PROPERTY(Union, type)
END
