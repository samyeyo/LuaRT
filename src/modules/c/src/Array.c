/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Array.c | LuaRT Array object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include <stdint.h>
#include <string.h>
#include "Value.h"
#include "Array.h"
#include "Struct.h"
#include "Union.h"
#include <stdio.h>
#include <math.h>

luart_type TArray;

//-------------------------------------[ Array Constructor ]
LUA_CONSTRUCTOR(Array) {
    Array *a = calloc(1, sizeof(Array));
    Value *val;
    luart_type type;
    int idx = 3;
    size_t nelem = 1;

    if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
        Array *from = lua_touserdata(L, 2);
        a->ndim = from->ndim;
        a->kind = from->kind;
        a->data = lua_touserdata(L, 3);
        a->name = from->name;
        a->size = from->size;
        a->owned = a->data != NULL;  
        memcpy(&a->dimensions, from->dimensions, sizeof(size_t)*a->ndim);
        if (!a->owned) {
            for (int i = 0; i < a->ndim; i++)
                nelem *= a->dimensions[i];
            a->data = calloc(nelem, a->size);
        }
    } else {
        switch(lua_type(L, 2)) {
            case LUA_TSTRING:   a->kind = luaL_checkoption(L, 2, NULL, csig);
                                a->size = csizes[a->kind]; 
                                break;

            case LUA_TTABLE:    luart_type type;
                                size_t len;
                                void *obj = lua_tocinstance(L, 2, &type);
                                if (!obj) {
                                    free(a);
                                    luaL_error(L, "cannot construct an Array of %s", luaL_typename(L, 2));
                                }
                                void *ptr = obj_topointer(L, obj, type, &len);
                                a->size = len;
                                if (type == TValue) {
                                    a->kind = ((Value*)obj)->kind;
                                    break;
                                } else if (type == TStruct) {
                                    a->kind = _struct; 
                                    a->Struct = (Struct*)obj;
                                    break;
                                } else if (type == TUnion) {
                                    a->kind = _union; 
                                    a->Union = (Union*)obj;
                                    break;
                                } else if (type == TArray) {
                                    a->kind = _array;
                                    a->Array = (Array*)obj;
                                    break;
                                } else if (type == TPointer) {
                                    a->kind = _void;
                                    a->Pointer = (Pointer*)obj;
                                    break;
                                }                                 
            default:            luaL_error(L, "cannot use %s as Array element", luaL_typename(L, 2));
        } 
        a->ndim = lua_gettop(L)-idx+1;
        for (int i = 0; i < a->ndim; i++) {
            int size = luaL_checkinteger(L, idx+i);
            nelem *= size;
            a->dimensions[i] = size; 
        }
    }
    lua_newinstance(L, a, Array);
	return 1;
} 

//-------------------------------------[ Array.dimension property ]
LUA_PROPERTY_GET(Array, dimensions) {
    Array *a = lua_self(L, 1, Array);

    lua_createtable(L, a->ndim, 0);
    for (int i = 0; i < a->ndim; i++) {
        lua_pushinteger(L, a->dimensions[i]);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

//-------------------------------------[ Array.length property ]
size_t array_length(Array *a) {
    size_t length = 1;

    for (int i = 0; i < a->ndim; i++)
        length *= a->dimensions[i];

    return length;
}

inline size_t array_size(Array *a) {
    return array_length(a) * a->size;
}

LUA_PROPERTY_GET(Array, size) {
    size_t size = array_size(lua_self(L, 1, Array));

    if (size > INT64_MAX)
        lua_pushnumber(L, size);
    else
        lua_pushinteger(L, size);
    return 1;
}

LUA_PROPERTY_GET(Array, length) {
    size_t length = array_length(lua_self(L, 1, Array));

    if (length > INT64_MAX)
        lua_pushnumber(L, length);
    else
        lua_pushinteger(L, length);
    return 1;
}

LUA_PROPERTY_GET(Array, type) {
    Array *a = lua_self(L, 1, Array);
    switch (a->kind) {
        case _array:    lua_pushfstring(L, "Array<%s>", a->Array->name ? a->Array->name : "Array"); break;
        case _struct:   lua_pushfstring(L, "Array<%s>", a->Struct->name ? a->Struct->name : "Struct"); break;
        case _union:  	lua_pushfstring(L, "Array<%s>", a->Union->name ? a->Union->name : "Union"); break;
        default:        lua_pushfstring(L, "Array<%s>", cnames[a->kind]); break;
    }	
    return 1;
}

size_t array_pos(lua_State *L, Array *a, int idx) {
    size_t byte_position = 0;
    size_t stride = a->size;
    size_t *indices = calloc(1, a->ndim * sizeof(size_t));

    if (!idx)
        indices[0] = luaL_checkinteger(L, 2);
    else for (size_t i = 0; i < a->ndim; i++)
        indices[i] = luaL_checkinteger(L, i+idx);

    for (size_t i = a->ndim; i > 0; i--) {
        if (indices[i-1] <0 || indices[i-1] >= a->dimensions[i-1])
            luaL_error(L, "index out of bounds (index: %d, dimension: %d)\n", indices[i-1], a->dimensions[i-1]); 
        byte_position += indices[i-1] * stride;
        stride *= a->dimensions[i-1];
    }
    free(indices);
    return byte_position;
}

static int get_array_value(lua_State *L, Array *a, void *data) {
    switch(a->kind) {
        case _struct:   lua_pushlightuserdata(L, a->Struct);
                        lua_pushlightuserdata(L, data);
                        lua_pushinstance(L, Struct, 2); break;
        case _union:     lua_pushlightuserdata(L, a->Union);
                        lua_pushlightuserdata(L, data);
                        lua_pushinstance(L, Union, 2); break;
        case _uchar:     lua_pushinteger(L, *(unsigned char *)data); break;
        case _char: 	lua_pushlstring(L, (char *)data, 1); break;
        case _wchar_t:	lua_pushlwstring(L,  (wchar_t *)data, 1); break;
        case _bool: 	lua_pushboolean(L, *((int *)data)); break;
        case _ushort:	lua_pushinteger(L, *((uint16_t*)data)); break;
        case _short: 	lua_pushinteger(L, *((int16_t*)data)); break;
        case _int:	    lua_pushinteger(L, *((int32_t *)data)); break;
#ifndef WIN64
        case _size:
        case _uint: 	lua_pushnumber(L, *((uint32_t*)data)); break;
#endif
        case _long:	    lua_pushinteger(L, *((long*)data)); break;
#ifdef WIN64
        case _size:
        case _ulong:	lua_pushinteger(L, *((unsigned long*)data)); break;
#endif
        case _longlong: lua_pushinteger(L, *((int64_t *)data)); break;
        case _ulonglong:lua_pushnumber(L, *((uint64_t *)data)); break;
        case _float:	lua_pushnumber(L, *((float *)data)); break;
        case _double: 	lua_pushnumber(L, *((double *)data)); break;
        case _string: 	lua_pushstring(L, *(char **)data); break;
        case _void: 	lua_pushlightuserdata(L, *(void **)data);
                        lua_pushinteger(L, _void);
                        lua_pushinstance(L, Pointer, 2);
                        break;
        case _array:    {
                            Array new = {0};
                            new.ndim = a->ndim-1;
                            memcpy(new.dimensions, &a->dimensions[1], new.ndim*sizeof(size_t));
                            new.size = a->size;
                            new.kind = a->kind;
                            lua_pushlightuserdata(L, &new);
                            lua_pushlightuserdata(L, data);
                            lua_pushinstance(L, Array, 2);
                            break;
                        }
        case _wstring: 	lua_pushwstring(L, *(wchar_t **)data); break;    
    }
    return 1;    
}

//-------------------------------------[ Array.get() ]
LUA_METHOD(Array, get) {
    Array *a = lua_self(L, 1, Array);
    
    if (!a->data) 
        luaL_error(L, "Cannot get value from Array definition");
    get_array_value(L, a, a->data + array_pos(L, a, 2));
    return 1;
}

//-------------------------------------[ Array.set() ]
static int set_value(lua_State *L, Array *a, char *data, int idx) {
    switch(a->kind) {
        case _uchar:
        case _char: 	*data = lua_isinteger(L, idx) ? lua_tointeger(L, idx) : luaL_checkstring(L, idx)[0]; break;
        case _wchar_t:  if (lua_isinteger(L, idx))
                            *((wchar_t*)data) = lua_tointeger(L, idx);
                        else {
                            wchar_t *str = lua_towstring(L, idx);
                                *((wchar_t*)data) = str[0];
                            free(str);
                        } break;
        case _bool: 	luaL_checktype(L, idx, LUA_TBOOLEAN);
        case _ushort:   *((uint16_t *)data) = lua_tointeger(L, idx); break;
        case _short: 	*((int16_t *)data) = lua_tointeger(L, idx); break;
        case _int:      *((uint32_t *)data) = lua_tointeger(L, idx); break;
#ifndef WIN64
        case _size:
        case _uint: 	*((int32_t *)data) = lua_tointeger(L, idx); break;
#endif
        case _long:     *((int64_t *)data) = lua_tointeger(L, idx); break;
#ifdef WIN64
        case _size:
        case _ulong:	*((uint64_t *)data) = lua_tointeger(L, idx); break;
#endif
        case _longlong: *((int64_t *)data) = luaL_checknumber(L, idx); break;
        case _ulonglong:*((uint64_t *)data) = luaL_checknumber(L, idx); break;
        case _float:	*((float *)data) = luaL_checknumber(L, idx); break;
        case _double: 	*((double *)data) = luaL_checknumber(L, idx); break;
        case _string: 	*((char **)data) = (char*)luaL_checkstring(L, idx); break;
        case _wstring:  {
                            if (*data)
                                free(data);
                            *((wchar_t**)data) = lua_towstring(L, idx);
                            break;
                        }
        case _void:
        case _array:
        case _union:
        case _struct:   {
                            luart_type type;
                            void *ptr, *obj = lua_tocinstance(L, idx, &type);
                            if (!obj || !(ptr = obj_topointer(L, obj, type, NULL)))
                                luaL_error(L, "cannot set content from %s", luaL_typename(L, 2));
                            memcpy(data, ptr, a->size);
                        }
    }
    return 1;    
}

LUA_METHOD(Array, set) {
    Array *a = lua_self(L, 1, Array);  
    int narg = lua_gettop(L)-1;

    if (!a->data) 
        luaL_error(L, "Cannot set value from Array definition");
    set_value(L, a, a->data + array_pos(L, a, 2), a->ndim+2);
    return 0;
}

//-------------------------------------[ Array.__gc() ]
LUA_METHOD(Array, __gc) {
    Array *a = lua_self(L, 1, Array);
    if (!a->owned)
        free(a->data);
    free(a);
	return 0;
}

//-------------------------------------[ Array.__call() ]

static void set_row(lua_State *L, Array *a, size_t *from) {
    if (!lua_istable(L, -1))
        luaL_error(L, "Bad value during Array assignment (expecting table, found %s)", luaL_typename(L, -1));
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (lua_istable(L, -1))
            set_row(L, a, from);
        else {
            set_value(L, a, a->data + (*from)*a->size, -1);
            (*from)++;
        }
        lua_pop(L, 1);
    }
}

LUA_METHOD(Array, __call) {
	Array *new, *a = lua_self(L, 1, Array);
	BOOL isinit = lua_type(L, 2) == LUA_TTABLE;

	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);
	a->name = ar.name;
    lua_pushlightuserdata(L, a);
    lua_pushlightuserdata(L, NULL);
	new = lua_pushinstance(L, Array, 2);
	if (isinit) {
        size_t index = 0;
        lua_pushvalue(L, 2);
        set_row(L, new, &index);
        lua_pop(L, 1);
	} 
	return 1;
}

//-------------------------------------[ Array.__tostring() ]
LUA_METHOD(Array, __tostring) {
    Array *a = lua_self(L, 1, Array);

    switch (a->kind) {
        case _char:
        case _string:   lua_pushstring(L, a->data); break;
        case _wchar_t:
        case _wstring:  lua_pushwstring(L, (wchar_t*)a->data); break;
        case _struct:   lua_pushfstring(L, "Array<%s>: %p", a->Struct->name ? a->Struct->name : "Struct", a->Struct); break;
        case _union:    lua_pushfstring(L, "Array<%s>: %p", a->Union->name ? a->Union->name : "Union", a->Union); break;
        default:        lua_pushfstring(L, "Array<%s>: %p", a->data ? cnames[a->kind] : "cdef", a->data);
    }
	return 1;
}

//-------------------------------------[ Array.__metaindex() ]
LUA_METHOD(Array, __metaindex) {
    Array *a = lua_self(L, 1, Array);

    if (!a->data) 
        luaL_error(L, "Cannot get value from Array definition");
    if (a->ndim == 1)
        get_array_value(L, a, a->data + array_pos(L, a, 0));
    else {
        Array new = {0};
        new.ndim = a->ndim-1;
        memcpy(new.dimensions, &a->dimensions[1], new.ndim*sizeof(size_t));
        new.size = a->size;
        new.kind = a->kind;
        lua_pushlightuserdata(L, &new);
        size_t pos = array_pos(L, a, 0);
        lua_pushlightuserdata(L, a->data + array_pos(L, a, 0));
        lua_pushinstance(L, Array, 2);
    }
	return 1;
}

LUA_METHOD(Array, __metanewindex) {
    Array *a = lua_self(L, 1, Array);

    if (!a->data) 
        luaL_error(L, "Cannot set value from Array definition");
    if (a->ndim == 1)
        set_value(L, a, a->data + array_pos(L, a, 0), 3);
    else {
        size_t index = array_pos(L, a, 0);
        lua_pushvalue(L, 3);
        set_row(L, a, &index);
    }
	return 0;
}

LUA_METHOD(Array, __len) {
    return Array_getsize(L);
}

OBJECT_METAFIELDS(Array)
	METHOD(Array, __gc)
    METHOD(Array, __metaindex)
    METHOD(Array, __metanewindex)
	METHOD(Array, __call)
	METHOD(Array, __len)
	METHOD(Array, __tostring)
END

OBJECT_MEMBERS(Array)
    METHOD(Array, get)
    METHOD(Array, set)
    READONLY_PROPERTY(Array, dimensions)
    READONLY_PROPERTY(Array, length)
    READONLY_PROPERTY(Array, size)
    READONLY_PROPERTY(Array, type)
END