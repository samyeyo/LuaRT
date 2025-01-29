#include <luart.h>
#include "Library.h"
#include "Value.h"
#include "Struct.h"
#include "Union.h"
#include "Array.h"
#include "Pointer.h"
#include <Buffer.h>

void *obj_topointer(lua_State *L, void *val, luart_type type, size_t *size) {
	void *result = NULL;
	size_t len = 0;

	if (type == TValue) {
		len = ((Value *)val)->size;
		if (((Value*)val)->kind >= _string)
			result = ((Value *)val)->String;
		else
			result = &((Value *)val)->Char;
	} else if (type == TStruct) {
		result = ((Struct *)val)->data;
		len = ((Struct *)val)->size;
	}
	else if (type == TUnion){
		result = ((Union *)val)->data;
		len =  ((Union *)val)->size;
	}
	else if (type == TBuffer){
		result = ((Buffer *)val)->bytes;
		len =  ((Buffer *)val)->size;
	} else if (type == TPointer) {
		result = ((Pointer *)val)->ptr;
		len = sizeof(void*);
	} else if (type == TArray) {
		result = ((Array *)val)->data;
		len = array_size((Array *)val);
	} else result = val;
	if (size)
		*size = len;
	return result;
}

void *topointer(lua_State *L, int idx) {
	luart_type type;
	void *val;

	int t = lua_type(L, idx);

	switch (t) {
		case LUA_TNIL:				return NULL;
		case LUA_TLIGHTUSERDATA:	return lua_touserdata(L, idx);
		case LUA_TSTRING:			return (void*)lua_tostring(L, idx);
		case LUA_TTABLE:			if((val = lua_tocinstance(L, idx, &type)))
										return obj_topointer(L, val, type, NULL);
	}
	luaL_error(L, "cannot get memory address of a '%s'", luaL_typename(L, idx));
	return NULL;
}

//------------------------------------ c.string() function
LUA_METHOD(c, string) {
	size_t len;
	const char *str = NULL;
	int type = lua_type(L, 1);
	Value *v;
	void *val;
	
	lua_pushstring(L, "string");
	lua_pushnil(L);
	v = lua_pushinstance(L, Value, 2);

	switch (type) {
		case LUA_TTABLE:	if(!((val = lua_tocinstance(L, 1, &type)) && (str == obj_topointer(L, val, type, &len))))
								goto err;	
							break;
		case LUA_TSTRING:	v->String = strdup(lua_tolstring(L, 1, &len));
							goto done;
		case LUA_TNUMBER:	len = luaL_checkinteger(L, 1)+1;
							break;
		default:			
err:						luaL_error(L, "cannot convert '%s' to C string", luaL_typename(L, 1));
	}
	v->String = calloc(1, len);
	if (str)
		memcpy(v->String, str, len);
done: 
	v->size = len;
	return 1;
}

//------------------------------------ c.wstring() function
LUA_METHOD(c, wstring) {
	size_t len;
	const wchar_t *str = NULL;
	int type = lua_type(L, 1);
	Value *v;
	void *val;
	
	lua_pushstring(L, "wstring");
	lua_pushnil(L);
	v = lua_pushinstance(L, Value, 2);

	switch (type) {
		case LUA_TTABLE:	if((val = lua_tocinstance(L, 1, &type)) && (str == obj_topointer(L, val, type, &len)))
								len *= sizeof(wchar_t);
							else goto err;	
							break;
		case LUA_TSTRING:	v->WString = lua_tolwstring(L, 1, (int*)&len);
							len *= sizeof(wchar_t);
							goto done;
		case LUA_TNUMBER:	len = (luaL_checkinteger(L, 1)+1)*sizeof(wchar_t);
							break;
		default:			
err:						luaL_error(L, "cannot convert '%s' to C string", luaL_typename(L, 1));
	}
	v->WString = calloc(1, len);
	if (str)
		memcpy(v->WString, str, len);
done: 
	v->size = len;
	return 1;
}

//------------------------------------ c types functions
static int pushValue(lua_State *L, const char *type) {
	int n = lua_gettop(L);

	lua_pushstring(L, type);
	Value *v = lua_pushinstance(L, Value, 1);
	if (n)
		set_value(L, 1, v);
	return 1;
}

#define CTYPE(x) LUA_METHOD(c, x) { \
	return pushValue(L, #x); \
}

#define ASTYPE(x) LUA_METHOD(c, as_x##x) { \
	return pushValue(L, #x); \
}


CTYPE(uchar)
CTYPE(char)
CTYPE(wchar_t)
CTYPE(short)
CTYPE(ushort)
CTYPE(int)
CTYPE(uint)
CTYPE(bool)
CTYPE(long)
CTYPE(longlong)
CTYPE(ulong)
CTYPE(ulonglong)
CTYPE(float)
CTYPE(double)
CTYPE(size_t)
CTYPE(int16_t)
CTYPE(int32_t)
CTYPE(int64_t)
CTYPE(uint16_t)
CTYPE(uint32_t)
CTYPE(uint64_t)

//------------------------------------ c.sizeof() function
LUA_METHOD(c, sizeof) {
	size_t size = 0;
	switch(lua_type(L, 1)) {
            case LUA_TSTRING:   int kind = luaL_checkoption(L, 1, NULL, csig);
								size = kind < 25 ? csizes[kind] : 0; 
                                break;

            case LUA_TTABLE:    luart_type type;
                                size_t len;
                                void *obj = lua_tocinstance(L, 1, &type);
                                if (obj) {
                                    obj_topointer(L, obj, type, &size);
                                	break;        
								}
            default:            luaL_error(L, "cannot get the C size of %s", luaL_typename(L, 1));
        } 
    if (size > INT64_MAX)
        lua_pushnumber(L, size);
    else
        lua_pushinteger(L, size);
	return 1;
}

//------------------------------------ c.type() function
LUA_METHOD(c, type) {
	int type;
	void *val = lua_tocinstance(L, 1, &type);

	if (val) {
		if (type == TValue)
			lua_pushstring(L, cnames[((Value *)val)->kind]);
		else if (type == TStruct)
			lua_pushstring(L, ((Struct *)val)->data ? ((Struct *)val)->name : "Struct<cdef>");
		else if (type == TUnion)
			lua_pushstring(L, ((Struct *)val)->data ? ((Struct *)val)->name : "Union<cdef>");
		else if (type == TArray) {
			Array *a = (Array *)val;
			switch (a->kind) {
				case _array:    lua_pushfstring(L, "Array<%s>", a->Array->name ? a->Array->name : "Array"); break;
				case _struct:   lua_pushfstring(L, "Array<%s>", a->Struct->name ? a->Struct->name : "Struct"); break;
				case _union:  	lua_pushfstring(L, "Array<%s>", a->Union->name ? a->Union->name : "Union"); break;
				default:        lua_pushfstring(L, "Array<%s>", cnames[a->kind]); break;
			}			
		}
		else if (type == TPointer) {
			Pointer *p = val;
			switch (p->kind) {
				case _struct:   lua_pushfstring(L, "%s*", p->Struct->name ? p->Struct->name : "Struct"); break;
				case _union:  	lua_pushfstring(L, "%s*", p->Union->name ? p->Union->name : "Union"); break;
				default:        lua_pushstring(L, pnames[p->kind]);
			}			
		}
		return 1;
	}
	return 0;
}

//------------------------------------ c.NULL property
LUA_PROPERTY_GET(c, NULL) {
	lua_pushlightuserdata(L, NULL);
	lua_pushinteger(L, _void);
	lua_pushinstance(L, Pointer, 2);
	return 1;
}

//--- Registration of module properties and functions        
MODULE_PROPERTIES(c)
	READONLY_PROPERTY(c, NULL)
END

MODULE_FUNCTIONS(c)
	METHOD(c, uchar)
	METHOD(c, char)
	METHOD(c, wchar_t)
	METHOD(c, short)
	METHOD(c, ushort)
	METHOD(c, int)
	METHOD(c, uint)
	METHOD(c, bool)
	METHOD(c, long)
	METHOD(c, longlong)
	METHOD(c, ulong)
	METHOD(c, ulonglong)
	METHOD(c, float)
	METHOD(c, double)
	METHOD(c, size_t)
	METHOD(c, int16_t)
	METHOD(c, int32_t)
	METHOD(c, int64_t)
	METHOD(c, uint16_t)
	METHOD(c, uint32_t)
	METHOD(c, uint64_t)
	METHOD(c, string)
	METHOD(c, wstring)
	METHOD(c, type)
	METHOD(c, sizeof)
END

//----- "luaopen_ctypes" module registration function
int __declspec(dllexport) luaopen_c(lua_State *L)
{    
    lua_regmodule(L, c);
	lua_regobjectmt(L, Library);
	lua_regobjectmt(L, Value);
	lua_regobjectmt(L, Union);
	lua_regobjectmt(L, Struct);
	lua_regobjectmt(L, Pointer);
	lua_regobjectmt(L, Array);
    return 1;
}