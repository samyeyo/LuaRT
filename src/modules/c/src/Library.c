/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Library.c | LuaRT Library object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <windows.h>
#include <Buffer.h>
#include "Value.h"
#include "Struct.h"
#include "Pointer.h"
#include <stdint.h>
#include <Shlwapi.h>
#include "Library.h"
#include "include\dynload.h"
#include "include\dyncall.h"
#include "include\dyncall_callf.h"

luart_type TLibrary;

static const char mode[] = {
	'd', 'c', 's', 'f', 'F', 0
};

//-------------------------------------[ Library Constructor ]
LUA_CONSTRUCTOR(Library) {
	const char *libname = lua_gettop(L) == 1 ? "ucrtbase.dll" : luaL_checkstring(L, 2);
	Library *l;
	DLLib *lib;
	if (!(lib = dlLoadLibrary(libname)))
		lua_pushnil(L);
	else {
		l = (Library*)calloc(1, sizeof(Library));
		l->libname = PathFindFileNameA(libname);
		l->lib = lib;
		l->vm = dcNewCallVM(4096);
		lua_newinstance(L, l, Library);
	}
	return 1;
}

//-------------------------------------[ Library.__gc() ]
LUA_METHOD(Library, __gc) {
	Library *l = lua_self(L, 1, Library);
	dcFree(l->vm);
	dlFreeLibrary(l->lib);
	free(l);
	return 0;
}

//-------------------------------------[ Library set new index ]
LUA_METHOD(Library, __metanewindex) {
	Library *l = lua_self(L, 1, Library);
	const char *symbol = luaL_checkstring(L, 2);

	if (dlFindSymbol(l->lib, symbol)) {
		lua_getmetatable(L, 1);
		lua_pushstring(L, symbol);
		lua_pushstring(L, luaL_checkstring(L, 3));
		lua_rawset(L, -3);
	} else luaL_error(L, "'%s' symbol not found in '%s'", symbol, l->libname);
	return 0;
}

//-------------------------------------[ Library function call ]
typedef struct {
	void *pointers[256];
	size_t count;
} PtrSet;

void ArgStruct(lua_State *L, DCCallVM *vm, Struct *s);

static void ArgUnion(lua_State *L, DCCallVM *vm, Union *u) {
	switch (u->current->type) {
		case 'u':	lua_rawgeti(L, LUA_REGISTRYINDEX, u->current->ref);
					Union *uu = luaL_checkcinstance(L, -1, Union);
					uu->data = u->data;
					ArgUnion(L, vm, uu);
					uu->data = NULL;
					lua_pop(L, 1);
					break;
		case '.': 	lua_rawgeti(L, LUA_REGISTRYINDEX, u->current->ref); 
					Struct *s = luaL_checkcinstance(L, -1, Struct);
					s->data = u->data;
					ArgStruct(L, vm, s);
					s->data = NULL;
					lua_pop(L, 1);
					break;
		case 'C':
		case 'c': 	dcArgChar(vm, *(char *)(u->data)); break;
		case 'B': 	dcArgBool(vm, *((int *)(u->data))); break;
		case 'w':
		case 'S':	dcArgShort(vm, *((uint16_t*)(u->data))); break;
		case 's':	dcArgShort(vm, *((short*)(u->data))); break;
		case 'j':
		case 'i':	dcArgInt(vm, *((int32_t *)(u->data))); break;
		case 'J':
#ifndef WIN64
			case '#':
#endif		
		case 'I': 	dcArgInt(vm, *((uint32_t*)(u->data))); break;
		case 'l': 	dcArgLongLong(vm, *((int64_t *)(u->data))); break;
#ifdef WIN64
			case '#':
#endif
		case 'L': 	dcArgLongLong(vm, *((uint64_t *)(u->data))); break;
		case 'f':	dcArgFloat(vm, *((float *)(u->data))); break;
		case 'd': 	dcArgDouble(vm, *((double *)(u->data))); break;
		case 'Z': 	
		case 'p': 	
		case 'W': 	dcArgPointer(vm, *(void **)(u->data)); break;
		default : 	luaL_error(L, "Unknown '%c' signature character in %s Union ", u->current->type, u->name);
	}		
}

static void ArgStruct(lua_State *L, DCCallVM *vm, Struct *s) {
	for (int i = 0; i < s->nfields; i++) {
		Field f = s->fields[i];
		switch (f.type) {
			case 'u':	lua_rawgeti(L, LUA_REGISTRYINDEX, f.ref);
						Union *u = luaL_checkcinstance(L, -1, Union);
						u->data = s->data+f.offset;
						ArgUnion(L, vm, u);
						u->data = NULL;
						lua_pop(L, 1);
						break;
			case '.': 	lua_rawgeti(L, LUA_REGISTRYINDEX, f.ref); 
						Struct *ss = luaL_checkcinstance(L, -1, Struct);
						ss->data = s->data+f.offset;
						ArgStruct(L, vm, ss);
						ss->data = NULL;
						lua_pop(L, 1);
						break;
			case 'C':
			case 'c': 	dcArgChar(vm, *(char *)(s->data+f.offset)); break;
			case 'B': 	dcArgBool(vm, *((int *)(s->data+f.offset))); break;
			case 'w':
			case 'S':	dcArgShort(vm, *((uint16_t*)(s->data+f.offset))); break;
			case 's':	dcArgShort(vm, *((short*)(s->data+f.offset))); break;
			case 'j':
			case 'i':	dcArgInt(vm, *((int32_t *)(s->data+f.offset))); break;
			case 'J':
#ifndef WIN64
			case '#':
#endif			
			case 'I': 	dcArgInt(vm, *((uint32_t*)(s->data+f.offset))); break;
			case 'l': 	dcArgLongLong(vm, *((int64_t *)(s->data+f.offset))); break;
#ifdef WIN64
			case '#':
#endif
			case 'L': 	dcArgLongLong(vm, *((uint64_t *)(s->data+f.offset))); break;
			case 'f':	dcArgFloat(vm, *((float *)(s->data+f.offset))); break;
			case 'd': 	dcArgDouble(vm, *((double *)(s->data+f.offset))); break;
			case 'Z': 	
			case 'p': 	
			case 'W': 	dcArgPointer(vm, *(void **)(s->data+f.offset)); break;
			default : 	luaL_error(L, "Unknown '%c' signature character", f.type);
		}
	}
}

void *Arg(lua_State *L, DCCallVM *vm, int idx, const DCsigchar *c) {
	Value *v = NULL;
	DCstruct* cs;

	if (*c == '.') {
		Struct *s = luaL_checkcinstance(L, idx, Struct);
#ifdef WIN64
		if (s->size <= 8)
			dcArgLongLong(vm, *(long long *)s->data);
		else
			dcArgPointer(vm, s->data);
#else
		if (s->size <= 4)
			dcArgInt(vm, *(int *)s->data);
		else
			ArgStruct(L, vm, s);
#endif
	} else if (*c == 'u') 
		ArgUnion(L, vm, luaL_checkcinstance(L, idx, Union));
	else if ((v = lua_iscinstance(L, idx, TValue))) {
		switch (*c) {
			case 'B': dcArgBool(vm, v->Bool); break;
			case 'w': dcArgShort(vm, (unsigned int)v->WChar);
			case 'C':
			case 'c': dcArgChar(vm, v->Char); break;
			case 's': dcArgShort(vm, v->Short); break;
			case 'j':
			case 'S': dcArgInt(vm, v->UShort); break;
#ifndef WIN64
			case '#':			
			case 'J': dcArgLong(vm, v->ULong); break;
#endif
			case 'I': dcArgInt(vm, v->UInt32); break;
			case 'i':
			case 'l': dcArgLongLong(vm, v->Long); break;
#ifdef WIN64
			case '#':
			case 'L': dcArgLongLong(vm, v->LongLong); break;
#endif
			case 'f': dcArgFloat(vm, v->Float); break;
			case 'd': dcArgDouble(vm, v->Double); break;
			case 'W':
			case 'Z': 
			case 'p':
				dcArgPointer(vm, obj_topointer(L, v, TValue, NULL));
				break;
			default : luaL_error(L, "Unknown '%c' signature character", *c);
		}
	} else switch (*c) {
		case 'B': dcArgBool(vm, lua_toboolean(L, idx)); break;
		case 'C':
		case 'c': dcArgChar(vm, luaL_checkstring(L, idx)[0]); break;
		case 's': dcArgShort(vm, luaL_checkinteger(L, idx)); break;
		case 'j':
		case 'w': if (lua_isstring(L, idx)) {
					wchar_t *w = lua_towstring(L, idx);
					dcArgShort(vm, (unsigned short)w[0]);
					free(w);
				  } else dcArgShort(vm, (unsigned short)luaL_checkinteger(L, idx)); break;	
				  break;
		case 'S': dcArgShort(vm, (unsigned short)luaL_checkinteger(L, idx)); break;
		case 'J': dcArgLong(vm, (unsigned long)luaL_checkinteger(L, idx)); break;
#ifndef WIN64
			case '#':
#endif
		case 'I': dcArgInt(vm, (uint32_t)luaL_checkinteger(L, idx)); break;
		case 'i':
		case 'l': dcArgLongLong(vm, luaL_checkinteger(L, idx)); break;
#ifdef WIN64
			case '#':
#endif		
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

static void addPtr(lua_State *L, PtrSet *ptrset, void *ptr) {
	if ((ptrset->pointers[ptrset->count] = ptr)) {
		ptrset->count++;
		if (ptrset->count > 255)
			luaL_error(L, "maximal pointers arguments reached");
	}
}

int wrapcall(lua_State *L) {	
	const char *funcname = lua_tostring(L, lua_upvalueindex(1));
	void *func = lua_touserdata(L, lua_upvalueindex(2));
	const char *signature = lua_tostring(L, lua_upvalueindex(3));
	void *vm = lua_touserdata(L, lua_upvalueindex(4));
	int nargs = 0;
	PtrSet ptrset = {0};
	void *ptr;

	dcReset(vm);
	const DCsigchar *cr = signature;
	const DCsigchar *c = signature;
	if (*c != '(') {
		int i = 0;
		do 
			if (*c == mode[i]) {
				dcMode(vm, i);
				break;
			}
		while (mode[++i]);
		++c;
	}
	if (*c != '(')
		luaL_error(L, "wrong signature (expected '(' found '%c')", *c);
	++c;
	while (*c != ')') {
		if (*c == '+') {
			const DCsigchar *cc = lua_tostring(L, ++nargs);
			while (*cc != 0) {
				addPtr(L, &ptrset, Arg(L, vm, ++nargs, cc));		
				cc++;
			}
			c++;
			break;
		} else if (*c == '\0')
			luaL_error(L, "Not enough arguments to call '%s'", funcname);
		addPtr(L, &ptrset, Arg(L, vm, ++nargs, c));
		c++;
	}
	c++;
	
	switch (*c) { 
		case '\0': dcCallVoid(vm, func); lua_pushnil(L); break;
		case 'B': lua_pushboolean(L, dcCallBool(vm, func)); break;
		case 'w': {
					wchar_t w = dcCallShort(vm, func);
					lua_pushlwstring(L, &w, 1);
					break;
				  }
		case 'c': {
					char c = dcCallChar(vm, func);
					lua_pushlstring(L, &c, 1);
					break;
				  }
		case 'C': 
		case 's': 
		case 'j': 
		case 'i': lua_pushinteger(L, dcCallInt(vm, func)); break;
		case 'S':
		case 'J': 
		case 'l': 
		case 'I': lua_pushinteger(L, (unsigned int)dcCallInt(vm, func)); break;
		case 'L': lua_pushnumber(L, (unsigned long long)dcCallLongLong(vm, func)); break;
		case 'f': lua_pushnumber(L, dcCallFloat(vm, func)); break;
		case 'd': lua_pushnumber(L, dcCallDouble(vm, func)); break;
		case 'p':	{
						lua_pushlightuserdata(L, dcCallPointer(vm, func)); 
						lua_pushinteger(L, _void); 
						lua_pushinstance(L, Pointer, 2);
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


//-------------------------------------[ Library get function ]
static BOOL isFunction(HMODULE  hModule, const char *funcname) {
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD *pFunctions = (DWORD *)((BYTE*)hModule + pExportDir->AddressOfFunctions);
    DWORD *pNames = (DWORD *)((BYTE*)hModule + pExportDir->AddressOfNames);
    WORD *pOrdinals = (WORD *)((BYTE*)hModule + pExportDir->AddressOfNameOrdinals);
    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
        char *functionName = (char*)((BYTE*)hModule + pNames[i]);
		if (strcmp(functionName, funcname) == 0) {
			DWORD functionRVA = pFunctions[pOrdinals[i]];

			PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeaders);
			BOOL isFunction = FALSE;
			for (WORD j = 0; j < pNtHeaders->FileHeader.NumberOfSections; j++, pSection++) {
				if ((functionRVA >= pSection->VirtualAddress) && (functionRVA < (pSection->VirtualAddress + pSection->Misc.VirtualSize))) {
					if (pSection->Characteristics & IMAGE_SCN_CNT_CODE)
						return TRUE;
				}
			}
			break;
		}
	}
    return FALSE;
}

LUA_METHOD(Library, __metaindex) {
	Library *l = lua_self(L, 1, Library);
	const char *symbol = luaL_checkstring(L, 2);
	void *func;
	
	if ((func = dlFindSymbol(l->lib, symbol))) {
			lua_getmetatable(L, 1);
		if (isFunction((HMODULE)l->lib, symbol)) {
			lua_pushstring(L, symbol);
			lua_pushlightuserdata(L, func);
			if (lua_getfield(L, -3, symbol) == LUA_TNIL)
				luaL_error(L, "No signature defined for function '%s'", symbol);
			lua_pushlightuserdata(L, l->vm);
			lua_pushcclosure(L, wrapcall, 4);	
		} else {
			if (lua_getfield(L, -1, symbol) == LUA_TNIL)
				luaL_error(L, "No signature defined for symbol '%s'", symbol);
			const char *c = lua_tostring(L, -1);
			switch (*c) {
				case 'B': lua_pushboolean(L, *(int *)func); break;
				case 'C': lua_pushinteger(L, *(unsigned char *)func); break;
				case 'c': lua_pushlstring(L, func, 1); break;
				case 'S': lua_pushinteger(L, *(unsigned short *)func); break;
				case 's': lua_pushinteger(L, *(short *)func); break;
				case 'j': 
				case 'J':
				case 'i':
				case 'I': 
				case 'l': lua_pushinteger(L, *(lua_Integer *)func); break;
				case 'L': lua_pushnumber(L, *(unsigned long long *)func); break;
				case 'f': lua_pushnumber(L, *(float *)func); break;
				case 'd': lua_pushnumber(L, *(double *)func); break;
				case 'p':	{
								lua_pushlightuserdata(L, (void *)func); 
								lua_pushinteger(L, _void); 
								lua_pushinstance(L, Pointer, 2);
								break;
							
							}
				case 'Z': lua_pushstring(L, (char *)func); break;
				case 'W': lua_pushwstring(L, (wchar_t *)func); break;				
			}
		}	
	} else luaL_error(L, "'%s' symbol not found in '%s'", symbol, l->libname);
	return 1;
}

//-------------------------------------[ Library tostring() ]
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
