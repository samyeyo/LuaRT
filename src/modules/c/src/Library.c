/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Library.c | LuaRT Library object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include "Value.h"
#include "Struct.h"
#include <stdint.h>
#include <Shlwapi.h>
#include "Library.h"
#include "include\dynload.h"
#include "include\dyncall.h"
#include "include\dyncall_callf.h"

luart_type TLibrary;

static const char *mode[] = {
	"default", "cdecl","stdcall", "cdecl", "fastcall"
};

//-------------------------------------[ Library Constructor ]
LUA_CONSTRUCTOR(Library) {
	const char *libname = lua_gettop(L) == 1 ? "ucrtbase.dll" : luaL_checkstring(L, 2);
	Library *l = (Library*)calloc(1, sizeof(Library));
	DLLib *lib;
	lib = dlLoadLibrary(libname);
	l->libname = PathFindFileNameA(libname);
	l->lib = lib;
	l->vm = dcNewCallVM(4096);
	lua_newinstance(L, l, Library);
	return 1;
}

//-------------------------------------[ Library.write() ]

LUA_METHOD(Library, __gc) {
	Library *l = lua_self(L, 1, Library);
	dcFree(l->vm);
	dlFreeLibrary(l->lib);
	free(l);
	return 0;
}

LUA_METHOD(Library, __metanewindex) {
	Library *l = lua_self(L, 1, Library);
	const char *funcname = luaL_checkstring(L, 2);
	
	if (dlFindSymbol(l->lib, funcname)) {
		lua_getmetatable(L, 1);
		lua_pushstring(L, funcname);
		lua_pushstring(L, luaL_checkstring(L, 3));
		lua_rawset(L, -3);
	} else luaL_error(L, "'%s' symbol not found in '%s'", funcname, l->libname);
	return 0;
}

typedef struct {
	void *pointers[256];
	size_t count;
} PtrSet;

static void *Arg(lua_State *L, DCCallVM *vm, int idx, const DCsigchar *c) {
	Value *v = NULL;

	if ((v = lua_iscinstance(L, idx, TValue))) {
		switch (*c) {
			case 'B': dcArgBool(vm, v->Bool); break;
			case 'C':
			case 'c': dcArgChar(vm, v->Char); break;
			case 's': dcArgShort(vm, v->Short); break;
			case 'j':
			case 'S': dcArgInt(vm, v->UShort); break;
			case 'J': dcArgLongLong(vm, v->ULong); break;
			case 'I': dcArgInt(vm, v->UInt32); break;
			case 'i':
			case 'l': dcArgLong(vm, v->Long); break;
			case 'L': dcArgLongLong(vm, v->LongLong); break;
			case 'f': dcArgFloat(vm, v->Float); break;
			case 'd': dcArgDouble(vm, v->Double); break;
			case 'W':
			case 'Z': 
			case 'p':
				dcArgPointer(vm, obj_topointer(L, v, TValue));
				break;
			default : luaL_error(L, "Unknown '%c' signature character", *c);
		}
	} else switch (*c) {
		case 'B': dcArgBool(vm, lua_toboolean(L, idx)); break;
		case 'C':
		case 'c': dcArgChar(vm, luaL_checkstring(L, idx)[0]); break;
		case 's': dcArgShort(vm, luaL_checkinteger(L, idx)); break;
		case 'j':
		case 'S': dcArgInt(vm, (unsigned short)luaL_checkinteger(L, idx)); break;
		case 'J': dcArgLongLong(vm, (unsigned long)luaL_checkinteger(L, idx)); break;
		case 'I': dcArgInt(vm, (uint32_t)luaL_checkinteger(L, idx)); break;
		case 'i':
		case 'l': dcArgLong(vm, luaL_checkinteger(L, idx)); break;
		case 'L': dcArgLongLong(vm, abs(lua_tonumber(L, idx))); break;
		case 'f':
		case 'd': dcArgDouble(vm, luaL_checknumber(L, idx)); break;
		case 'W': 
				if (lua_isstring(L, idx)) {
					wchar_t *str = lua_towstring(L, idx);
					dcArgPointer(vm, str);
					return str;
				}
		case 'Z': 
		case 'p':
			dcArgPointer(vm, topointer(L, idx));
			break;
		default : luaL_error(L, "Unknown '%c' signature character", *c);
	}
	return NULL;
}

static int wrapcall(lua_State *L) {	
	const char *funcname = lua_tostring(L, lua_upvalueindex(1));
	void *func = lua_touserdata(L, lua_upvalueindex(2));
	const char *signature = lua_tostring(L, lua_upvalueindex(3));
	void *vm = lua_touserdata(L, lua_upvalueindex(4));
	int nargs = 0;
	PtrSet ptrset = {0};

	dcReset(vm);
	const DCsigchar *cr = signature;
	const DCsigchar *c = ++signature;
	while (*c != ')') {
		if (*c == '+') {
			const DCsigchar *cc = lua_tostring(L, ++nargs);
			while (*cc != 0) {
				Arg(L, vm, ++nargs, cc);		
				cc++;
			}
			c++;
			break;
		} else if (*c == '\0')
			luaL_error(L, "Not enough arguments to call '%s'", funcname);
		if ((ptrset.pointers[ptrset.count] = Arg(L, vm, ++nargs, c))) {
			ptrset.count++;
			if (ptrset.count > 255)
				luaL_error(L, "maximal arguments of pointers reached");
		}
		c++;
	}
	c++;
	switch (*c) { 
		case '\0': dcCallVoid(vm, func); return 0;
		case 'B': lua_pushboolean(L, dcCallBool(vm, func)); break;
		case 'C':
		case 'c': {
			DCchar ch = dcCallChar(vm, func);
			lua_pushlstring(L, &ch, 1);
			break;
		}
		case 'S':
		case 's': 
		case 'j':
		case 'J':
		case 'i':
		case 'I': 
		case 'l': lua_pushinteger(L, dcCallInt(vm, func)); break;
		case 'L': 
		case 'f': 
		case 'd': lua_pushnumber(L, dcCallDouble(vm, func)); break;
		case 'p':	{
						lua_pushstring(L, "pointer");
						lua_pushlightuserdata(L, dcCallPointer(vm, func)); 
						lua_pushinstance(L, Value, 2);
						break;
					
					}
		case 'Z': lua_pushstring(L, dcCallPointer(vm, func)); break;
		case 'W': lua_pushwstring(L, dcCallPointer(vm, func)); break;
		default : return luaL_error(L, "Unknown '%c' signature character", *c);
	}
	for (int i = 0; i < ptrset.count; i++)
		free(ptrset.pointers[i]);
	return 1;
}

LUA_METHOD(Library, __metaindex) {
	Library *l = lua_self(L, 1, Library);
	const char *funcname = luaL_checkstring(L, 2);
	void *func;
	
	if ((func = dlFindSymbol(l->lib, funcname))) {
		lua_getmetatable(L, 1);
		lua_pushstring(L, funcname);
		lua_pushlightuserdata(L, func);
		if (lua_getfield(L, -3, funcname) == LUA_TNIL)
			luaL_error(L, "No signature defined for function '%s'", funcname);
		lua_pushlightuserdata(L, l->vm);
		lua_pushcclosure(L, wrapcall, 4);		
	} else luaL_error(L, "'%s' symbol not found in '%s'", funcname, l->libname);
	return 1;
}

LUA_METHOD(Library, __tostring) {
	Library *l = lua_self(L, 1, Library);
	lua_pushfstring(L, "Library : %s <%p>", l->libname, l->lib);
	return 1;
}

OBJECT_METAFIELDS(Library)
	METHOD(Library, __gc)
	METHOD(Library, __metanewindex)
	METHOD(Library, __metaindex)
	METHOD(Library, __tostring)
END

OBJECT_MEMBERS(Library)
END
