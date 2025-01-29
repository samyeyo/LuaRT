/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Value.c | LuaRT Value object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include <stdint.h>
#include <string.h>
#include "Library.h"
#include "Struct.h"
#include "Union.h"
#include "Pointer.h"
#include "Array.h"

luart_type TPointer;

//-------------------------------------[ Pointer Constructor ]

LUA_CONSTRUCTOR(Pointer) {
    Pointer *p = calloc(1, sizeof(Pointer));
    int arg = lua_type(L, 2);
    
    p->kind = _void;

    if (lua_gettop(L) > 1)
        switch (arg) {
            case LUA_TNIL:
                break;

            case LUA_TLIGHTUSERDATA:
                p->ptr = lua_touserdata(L, 2);
                p->kind = lua_gettop(L) > 2  ? lua_tointeger(L, 3) : _void;
                break;

            default: {
                luart_type type;
                void *obj = lua_tocinstance(L, 2, &type);
                if (!obj) {
                    free(p);
                    luaL_error(L, "cannot get Pointer from a %s", luaL_typename(L, 2));
                }
                p->ptr = obj_topointer(L, obj, type, NULL);
                if (type == TValue)
                    p->kind = ((Value*)obj)->kind;
                else if (type == TStruct) {
                    p->kind = _struct; 
                    p->Struct = obj;
    next:          if (!p->ptr) {
                        if (lua_isuserdata(L, 3))
                            p->ptr = lua_touserdata(L, 3);
                        else
    err:                    luaL_error(L, "cannot get a Pointer from %s definition", luaL_typename(L, 2));
                    }
                }  else if (type == TUnion) {
                    p->kind = _union; 
                    p->Union = obj;
                    goto next;
                }  else if (type == TArray) {
                    p->kind = ((Array*)obj)->kind;
                    p->Array = (Array*)obj;
                    goto next;
                }
            }
        }
    lua_newinstance(L, p, Pointer);
	return 1;
}

LUA_PROPERTY_GET(Pointer, isnull) {
    lua_pushboolean(L, lua_self(L, 1, Pointer)->ptr == NULL);
    return 1;
}

LUA_PROPERTY_GET(Pointer, size) {
    lua_pushinteger(L, sizeof(NULL));
    return 1;
}

LUA_PROPERTY_GET(Pointer, type) {
    lua_pushstring(L, pnames[lua_self(L, 1, Pointer)->kind]);
    return 1;
}

LUA_PROPERTY_SET(Pointer, autorelease) {
    lua_self(L, 1, Pointer)->autorelease = lua_toboolean(L, 2);
    return 0;
}

LUA_PROPERTY_GET(Pointer, autorelease) {
    lua_pushboolean(L, lua_self(L, 1, Pointer)->autorelease);
    return 1;
}

LUA_PROPERTY_SET(Pointer, content) {
    Pointer *p = lua_self(L, 1, Pointer);
    
    switch(lua_type(L, 2)) {
        case LUA_TNIL:
            if (p->autorelease)
                free(p->ptr);
            p->ptr = NULL;
            break;

        case LUA_TTABLE:
            luart_type type;
            size_t size;
            void *ptr, *obj = lua_tocinstance(L, 2, &type);
            if (!obj) {
                free(p);
                luaL_error(L, "cannot set Pointer content from a %s", luaL_typename(L, 2));
            }
            ptr = obj_topointer(L, obj, type, &size);
            memcpy(p->ptr, ptr, size);
            break;

         default:
            Value v = {0};
            get_value(L, 2, &v);
            memcpy(p->ptr, &v.Char, csizes[v.kind]);       
    }
    return 0;
}

LUA_PROPERTY_GET(Pointer, content) {
    Pointer *p = lua_self(L, 1, Pointer);
    Value v = {0};
    v.type = TValue;
    v.kind = p->kind;

    switch (p->kind) {
        case _void:         luaL_error(L, "cannot get content of a void* Pointer");
        case _string:		v.String = strdup(p->ptr);
                            v.size = strlen(v.String)+1;
                            break;
        case _wstring:		v.WString = wcsdup(p->ptr);
                            v.size = wcslen(v.WString)+1;
                            break;
        case _struct:       lua_pushlightuserdata(L, p->Struct);
                            lua_pushlightuserdata(L, p->ptr);
                            lua_pushinstance(L, Struct, 2);
                            return 1;
        case _union:        lua_pushlightuserdata(L, p->Union);
                            lua_pushlightuserdata(L, p->ptr);
                            lua_pushinstance(L, Union, 2);
                            return 1;
        case _array:        lua_pushlightuserdata(L, p->Array);
                            lua_pushlightuserdata(L, p->ptr);
                            lua_pushinstance(L, Array, 2);
                            return 1;
        default:            v.size = csizes[p->kind];
                            memcpy(&v.Char, p->ptr, csizes[p->kind]);
    }
    lua_pushlightuserdata(L, &v);
    lua_pushinstance(L, Value, 1);    
    return 1;
}

//-------------------------------------[ Pointer.as() ]
LUA_METHOD(Pointer, as) {
    Pointer *p = lua_self(L, 1, Pointer);
    void *obj = NULL;

    lua_pushlightuserdata(L, p->ptr);

    switch (lua_type(L, 2)) {   
        case LUA_TSTRING:   lua_pushinteger(L, luaL_checkoption(L, 2, NULL, pnames));
                            break;

        case LUA_TTABLE:    luart_type type;
                            if (!(obj = lua_tocinstance(L, 2, &type)))
                                luaL_error(L, "cannot cast Pointer to %s", luaL_typename(L, 2));
                            void *ptr = obj_topointer(L, obj, type, NULL);
                            if (!ptr) {                                
                                if (type == TArray)
                                    lua_pushinteger(L, _array);
                                else if (type == TStruct)
                                    lua_pushinteger(L, _struct);
                                else if (type == TUnion)
                                    lua_pushinteger(L, _union);
                                else goto err;
                                break;
                            }
        default:         
err:                        luaL_error(L, "cannot cast Pointer to %s value", luaL_typename(L, 2));
    }
    Pointer *new = lua_pushinstance(L, Pointer, 2);
    if (obj)
        new->Array = obj;
    return 1;
}

//-------------------------------------[ Pointer.inc() ]
LUA_METHOD(Pointer, inc) {
    Pointer *p = lua_self(L, 1, Pointer);
    p->ptr = ((char*)p->ptr) + luaL_optinteger(L, 2, 1)*csizes[p->kind];
    lua_pushvalue(L, 1);
    return 1;
}

//-------------------------------------[ Pointer.dec() ]
LUA_METHOD(Pointer, dec) {
    Pointer *p = lua_self(L, 1, Pointer);
    p->ptr = ((char*)p->ptr) - luaL_optinteger(L, 2, 1)*csizes[p->kind];
    lua_pushvalue(L, 1);
    return 1;
}

//-------------------------------------[ Pointer indexing ]
LUA_METHOD(Pointer, __metaindex) {
    Pointer *p = lua_self(L, 1, Pointer);
    int idx = luaL_checkinteger(L, 2);
    Value v = {0};
    v.type = TValue;
    v.kind = p->kind;

    switch (p->kind) {
        case _void:         luaL_error(L, "cannot index a void* Pointer");
        case _string:		{
                                v.kind = _char;
                                v.Char = *(((char *)p->ptr)+idx);
                                v.size = csizes[_uchar];
                                break;
                            }
        case _wstring:		{
                                v.kind = _wchar_t;
                                v.WChar = *(((wchar_t *)p->ptr)+idx);
                                v.size = csizes[_wchar_t];
                                break;
                            }
        case _struct:       lua_pushlightuserdata(L, p->Struct);
                            lua_pushlightuserdata(L, ((char*)p->ptr)+p->Struct->size*idx);
                            lua_pushinstance(L, Struct, 2);
                            return 1;
        case _union:        lua_pushlightuserdata(L, p->Union);
                            lua_pushlightuserdata(L, ((char*)p->ptr)+p->Union->size*idx);
                            lua_pushinstance(L, Union, 2);
                            return 1;
        case _array:        lua_pushlightuserdata(L, p->Array);
                            lua_pushlightuserdata(L, ((char*)p->ptr)+p->Array->size*idx);
                            lua_pushinstance(L, Array, 2);
                            return 1;
        default:            memcpy(&v.Char, ((char*)p->ptr)+csizes[p->kind]*idx, csizes[p->kind]);
    }
    lua_pushlightuserdata(L, &v);
    lua_pushinstance(L, Value, 1);    
    return 1;
}

LUA_METHOD(Pointer, __metanewindex) {
    Pointer *p = lua_self(L, 1, Pointer);
    int idx = luaL_checkinteger(L, 2);
    
    switch(lua_type(L, 3)) {
        case LUA_TNIL:
           goto error;

        case LUA_TTABLE:
            luart_type type;
            size_t size = 0;
            void *ptr, *obj = lua_tocinstance(L, 3, &type);
            if (!obj) {
                free(p);
error:          luaL_error(L, "cannot set memory content from %s", luaL_typename(L, 3));
            }
            ptr = obj_topointer(L, obj, type, &size);
            memcpy(((char*)p->ptr)+size*idx, ptr, size);
            break;

         default:
            Value v = {0};
            get_value(L, 3, &v);
            memcpy(((char*)p->ptr)+csizes[v.kind]*idx, &v.Char, csizes[v.kind]);       
    }
    return 0;
}

//-------------------------------------[ Pointer.__tostring() ]
LUA_METHOD(Pointer, __tostring) {
    Pointer *p = lua_self(L, 1, Pointer);

    switch (p->kind) {
        case _char:
        case _string:   lua_pushstring(L, p->ptr); break;
        case _wchar_t:
        case _wstring:  lua_pushwstring(L, p->ptr); break;
        case _struct:   lua_pushfstring(L, "Pointer<%s*>: %p", p->Struct->name ? p->Struct->name : "Struct", p->ptr); break;
        case _union:    lua_pushfstring(L, "Pointer<%s*>: %p", p->Union->name ? p->Union->name : "Union", p->ptr); break;
        default:        lua_pushfstring(L, "Pointer<%s>: %p", pnames[p->kind], p->ptr);
    }
    return 1;
}

//-------------------------------------[ Pointer.__eq() ]
LUA_METHOD(Pointer, __eq) {
    lua_pushboolean(L, lua_self(L, 1, Pointer)->ptr == lua_self(L, 2, Pointer)->ptr);
    return 1;
}

//-------------------------------------[ Pointer.__lt() ]
LUA_METHOD(Pointer, __lt) {
    lua_pushboolean(L, lua_self(L, 1, Pointer)->ptr < lua_self(L, 2, Pointer)->ptr);
    return 1;
}

//-------------------------------------[ Pointer.__le() ]
LUA_METHOD(Pointer, __le) {
    lua_pushboolean(L, lua_self(L, 1, Pointer)->ptr <= lua_self(L, 2, Pointer)->ptr);
    return 1;
}

//-------------------------------------[ Pointer.__add() ]
LUA_METHOD(Pointer, __add) {
    Pointer *p = lua_self(L, 1, Pointer);
    int size;

    switch (p->kind) {
        case _struct:    size = p->Struct->size; break;
        case _union:     size = p->Union->size; break;
        default:         size = csizes[p->kind];
    }
    lua_pushlightuserdata(L, ((char*)p->ptr) + luaL_checkinteger(L, 2)*size);
    lua_pushinteger(L, p->kind);
    lua_pushinstance(L, Pointer, 2);
    return 1;
}

//-------------------------------------[ Pointer.__sub() ]
LUA_METHOD(Pointer, __sub) {
    Pointer *p = lua_self(L, 1, Pointer);
    Pointer *pp;

    if ((pp = lua_iscinstance(L, 2, TPointer)))
        lua_pushinteger(L, ((char*)p->ptr - (char*)pp->ptr));
    else {
        lua_pushlightuserdata(L, ((char*)p->ptr) - luaL_checkinteger(L, 2)*csizes[p->kind]);
        lua_pushinteger(L, p->kind);
        lua_pushinstance(L, Pointer, 2);
    }
    return 1;
}

//-------------------------------------[ Pointer.__gc() ]
LUA_METHOD(Pointer, __gc) {
    Pointer *p = lua_self(L, 1, Pointer);

    if (p->autorelease)
        free(p->ptr);
    free(p);
    return 0;
}

//-------------------------------------[ Pointer.__call() ]
LUA_METHOD(Pointer, __call) {
	Pointer *p = lua_self(L, 1, Pointer);
    int nargs = lua_gettop(L);

	luaL_checkstring(L, 2);
	DCCallVM* vm = dcNewCallVM(4096);	
	lua_pushstring(L, "function pointer");
	lua_pushlightuserdata(L, p->ptr);
	lua_pushvalue(L, 2);
	lua_pushlightuserdata(L, vm);
	lua_pushcclosure(L, wrapcall, 4);
    for (int i = 3; i <= nargs; i++)
        lua_pushvalue(L, i);
	if (lua_pcall(L, nargs-2, 1, 0))
		lua_error(L);
	dcFree(vm);
	return 1;
}

OBJECT_METAFIELDS(Pointer)
	METHOD(Pointer, __gc)
	METHOD(Pointer, __tostring)
	METHOD(Pointer, __eq)
	METHOD(Pointer, __add)
	METHOD(Pointer, __sub)
	METHOD(Pointer, __le)
	METHOD(Pointer, __lt)
	METHOD(Pointer, __call)
	METHOD(Pointer, __metaindex)
	METHOD(Pointer, __metanewindex)
	{"__len", Pointer_getsize},
END

OBJECT_MEMBERS(Pointer)
	READONLY_PROPERTY(Pointer, type)
	READONLY_PROPERTY(Pointer, size)
	READONLY_PROPERTY(Pointer, isnull)
	READWRITE_PROPERTY(Pointer, autorelease)
	READWRITE_PROPERTY(Pointer, content)
	METHOD(Pointer, as)
	METHOD(Pointer, inc)
	METHOD(Pointer, dec)
END
