#include <luart.h>
#include "Library.h"
#include "Value.h"
#include "Struct.h"
#include "Union.h"
#include <Buffer.h>

void *obj_topointer(lua_State *L, void *val, luart_type type) {
	if (type == TValue) {
		if (((Value*)val)->kind >= _string)
			return ((Value *)val)->Pointer;
		else return &((Value *)val)->Char;
	} else if (type == TStruct)
		return ((Struct *)val)->data;
	else if (type == TBuffer)
		return ((Buffer *)val)->bytes;
	return NULL;
}

void *topointer(lua_State *L, int idx) {
	Buffer *b;
	Value *v;
	luart_type type;
	void *val;

	int t = lua_type(L, idx);

	switch (t) {
		// case LUA_TLIGHTUSERDATA:	return lua_touserdata(L, idx);
		case LUA_TSTRING:			return (void*)lua_tostring(L, idx);
		case LUA_TTABLE:			if((val = lua_tocinstance(L, idx, &type)))
										return obj_topointer(L, val, type);
	}
	luaL_error(L, "cannot get memory address of a '%s'", luaL_typename(L, idx));
	return NULL;
}

//------------------------------------ c.addr() function
LUA_METHOD(c, addr) {
	pushValue(L, "pointer");
	// lua_pushstring(L, "pointer");
	// lua_pushlightuserdata(L, topointer(L, 1));
	// lua_pushinstance(L, Value, 2);
	return 1;
}

//------------------------------------ c.string() function
LUA_METHOD(c, string) {
	size_t len;
	const char *str = NULL;
	int type = lua_type(L, 1);
	Value *val;
	
	lua_pushstring(L, "string");
	switch (type) {
		case LUA_TTABLE:	if((val = lua_tocinstance(L, 1, &type))) {
								str = obj_topointer(L, val, type);
								lua_pushstring(L, str);
							} else luaL_error(L, "cannot convert '%s' to C string", luaL_typename(L, 1));	
							break;
		case LUA_TSTRING:	lua_pushvalue(L, 1);
							break;
		case LUA_TNUMBER:	{
			lua_Integer n = luaL_checkinteger(L, 1);
			char *str = calloc(1, n);
			lua_pushlstring(L, str, n);
			free(str);
			break;
		}
		default:	lua_pushvalue(L, 1);
	}
	lua_pushinstance(L, Value, 2);
	return 1;
}

//------------------------------------ c.wstring() function
LUA_METHOD(c, wstring) {
	wchar_t *str = NULL;
	int type = lua_type(L, 1);
	Value *val;
	int len;
	
	lua_pushstring(L, "wstring");
	switch (type) {
		case LUA_TTABLE:	if((val = lua_tocinstance(L, 1, &type))) {
								str = obj_topointer(L, val, type);
								lua_pushwstring(L, str);
								str = NULL;
							} else luaL_error(L, "cannot convert '%s' to C wide string", luaL_typename(L, 1));	
							break;
		case LUA_TSTRING:	{
								str = lua_tolwstring(L, 1, &len);
								goto done;
								break;
							}
		case LUA_TNUMBER:	{
								len = luaL_checkinteger(L, 1);
								str = calloc(1, len*sizeof(wchar_t));
done:							lua_pushlwstring(L, str, len);
								break;
							}
		default:	lua_pushvalue(L, 1);
	}
	free(str);
	lua_pushinstance(L, Value, 2);
	return 1;
}

//------------------------------------ c.pointer() function
LUA_METHOD(c, pointer) {
	// Value v = {0};
	// v.kind = _pointer;
	// v.Pointer = topointer(L, 1);
	pushValue(L, "pointer");
	return 1;
}

static int pushValue(lua_State *L, const char *type) {
	int i = lua_gettop(L);
	lua_pushstring(L, type);
	if (i)
		lua_pushvalue(L, 1);
	lua_pushinstance(L, Value, ++i);
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

//------------------------------------ c.type() function
LUA_METHOD(c, type) {
	int type;
	void *val = lua_tocinstance(L, 1, &type);

	if (val) {
		if (type == TValue)
			lua_pushstring(L, cnames[((Value *)val)->kind]);
		else if (type == TStruct)
			lua_pushfstring(L, "Struct<%s>", ((Struct *)val)->name ? ((Struct *)val)->name : "cdef");
		else if (type == TUnion)
			lua_pushfstring(L, "Union<%s>", ((Union *)val)->name ? ((Union *)val)->name : "cdef");
		return 1;
	}
	return 0;
}

//------------------------------------ c.NULL property
LUA_PROPERTY_GET(c, NULL) {
	lua_pushstring(L, "pointer");
	lua_pushlightuserdata(L, NULL);
	lua_pushinstance(L, Value, 2);
	return 1;
}

//--- Registration of module properties and functions        
MODULE_PROPERTIES(c)
	READONLY_PROPERTY(c, NULL)
END

MODULE_FUNCTIONS(c)
    METHOD(c, addr)
	METHOD(c, uchar)
	METHOD(c, char)
	METHOD(c, wchar_t)
	METHOD(c, short)
	METHOD(c, ushort)
	METHOD(c, int)
	METHOD(c, uint)
	METHOD(c, bool)
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
	METHOD(c, pointer)
	METHOD(c, type)
END

//----- "luaopen_ctypes" module registration function
int __declspec(dllexport) luaopen_c(lua_State *L)
{    
    lua_regmodule(L, c);
	lua_regobjectmt(L, Library);
	lua_regobjectmt(L, Value);
	lua_regobjectmt(L, Union);
	lua_regobjectmt(L, Struct);
    return 1;
}