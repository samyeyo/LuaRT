/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Value.c | LuaRT Value object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Buffer.h>
#include <stdint.h>
#include <string.h>
#include "Value.h"
#include <stdio.h>
#include <math.h>

luart_type TValue;

//-------------------------------------[ Value Constructor ]

static set_value(lua_State *L, int idx, Value *v) {
	switch (v->kind) {
		case _char:			v->Char = luaL_checkstring(L, idx)[0]; break;
		case _wchar_t:		{
								int len = 1;
								wchar_t *ch = lua_tolwstring(L, idx, &len);
								v->WChar = *ch;
								free(ch);
								v->size = sizeof(wchar_t);
								break;
							}
		case _bool:			
		case _int:			v->Int = luaL_checkinteger(L, idx); break;
		case _short:		v->Short = luaL_checkinteger(L, idx); break;
		case _uchar:		v->UChar = luaL_checkinteger(L, idx); break;
		case _ushort:		v->UShort = luaL_checkinteger(L, idx); break;
		case _uint:			v->UInt = luaL_checkinteger(L, idx); break;
		case _ulong:		v->ULong = luaL_checkinteger(L, idx); break;
		case _size:			v->Size = luaL_checknumber(L, idx); break;
		case _ulonglong:	v->ULongLong = strtoull(lua_tostring(L, idx), NULL, 0); break;
		case _float:		v->Float = luaL_checknumber(L, idx); break;
		case _double:		v->Double = luaL_checknumber(L, idx); break;
		case int16:			v->Int16 = luaL_checkinteger(L, idx); break;
		case int32:			v->Int32 = luaL_checkinteger(L, idx); break;
		case int64:			v->Int64 = luaL_checkinteger(L, idx); break;
		case uint16:		v->UInt16 = luaL_checkinteger(L, idx); break;
		case uint32:		v->UInt32 = luaL_checkinteger(L, idx); break;
		case uint64:		v->UInt64 = luaL_checknumber(L, idx); break;
		case _pointer:		v->Pointer = topointer(L, idx); v->owned = TRUE; break;
		case _string:		v->String = strdup(luaL_checklstring(L, idx, &v->size)); break;
		case _wstring:		{
								int len = luaL_len(L, idx);
								v->WString = lua_tolwstring(L, idx, &len);
								v->size = len*sizeof(wchar_t);
							}
	}	
}

static void get_value(lua_State *L, int idx, Value *v) {
	switch(lua_type(L, idx)) {
		case LUA_TSTRING:
		case LUA_TBOOLEAN:
		case LUA_TNUMBER:			if (lua_isinteger(L, idx)) {
										v->kind = _int;
										v->Int = lua_tointeger(L, idx);
									} else if (lua_isnumber(L, idx)) {
										v->kind = _double;
										v->Double = lua_tonumber(L, idx);
									} else {
										v->kind = _string;
										v->String = (char *)lua_tostring(L, idx);
									}
									break;
		case LUA_TLIGHTUSERDATA:	v->kind = _pointer;
									v->Pointer = lua_touserdata(L, idx);
									break;
		case LUA_TNIL:				v->kind = _pointer;
									v->Pointer = NULL;
									break;
		case LUA_TTABLE:			{
										Value *from;
										luart_type t;
										if ((from = lua_tocinstance(L, idx, &t)) && (t == TValue)) {
											memcpy(v, from, sizeof(Value));
											break;
										}
									}
		default:					luaL_error(L, "cannot convert %s to Value", luaL_typename(L, idx));
	}
}

LUA_CONSTRUCTOR(Value) {
	if (lua_isuserdata(L, 2)) {
		Value *v = calloc(1, sizeof(Value));
		memcpy(v, lua_touserdata(L, 2), sizeof(Value));
		lua_newinstance(L, v, Value);
	} else {
		const char *type = luaL_checkstring(L, 2);
		size_t i = 0;

		while (ctypes[i]) {
			if (strcmp(type, ctypes[i]) == 0) {
				Value *v = (Value*)calloc(1, sizeof(Value));
				v->kind = i;
				v->size = -1;
				if (lua_gettop(L) > 2) {
					Value *val;
					luart_type type;
					if (lua_isuserdata(L, 3)) {
						v->Pointer = lua_touserdata(L, 3);
						// memcpy(&v->UChar, lua_touserdata(L, 3), csizes[i]);
						v->owned = TRUE;
					} else if ((val = lua_tocinstance(L, 3, &type)) && (type == TValue)) {
						// as(val, v);
						if (v->kind >= _string)
							val->Pointer = v->Pointer;
					}
					else set_value(L, 3, v);
				}
				if (v->size == -1)
					v->size = csizes[i];
				lua_newinstance(L, v, Value);
				return 1;
			}
			i++;
		}
		lua_pushnil(L);
	}
	return 1;
} 

//-------------------------------------[ Value.__call() ]
LUA_METHOD(Value, __call) {
	set_value(L, 2, lua_self(L, 1, Value));
	return 0;
}

//-------------------------------------[ Value.__tostring() ]
static void push_unsigned_string(lua_State *L, unsigned long long value) {
	char result[128];

	_snprintf(result, 128, "%llu", value);
	lua_pushstring(L, result);
}

static int tostring(lua_State *L, Value *v) {
	switch (v->kind) {
		case _char:			lua_pushlstring(L, &v->Char, 1); break;
		case _wchar_t:		lua_pushlwstring(L, &v->WChar, 1); break;
		case _uchar:		push_unsigned_string(L, v->UChar); break;
		case _ushort:		push_unsigned_string(L, v->UShort); break;
		case _uint:			push_unsigned_string(L, v->UInt); break;
		case _ulong:		push_unsigned_string(L, v->ULong); break;
		case _size:
		case _ulonglong:	push_unsigned_string(L, v->ULongLong); break;
		case _int:			lua_pushfstring(L, "%d", v->Int); break;
		case _short:		lua_pushfstring(L, "%d", v->Short); break;
		case _bool:			lua_pushstring(L, v->Bool ? "true" : "false"); break;
		case _float:		lua_pushfstring(L, "%f", v->Float); break;
		case _double:		lua_pushfstring(L, "%f", v->Double); break;
		case int16:			lua_pushfstring(L, "%d", v->Int16); break;
		case int32:			lua_pushfstring(L, "%d", v->Int32); break;
		case int64:			lua_pushfstring(L, "%d", v->Int64); break;
		case uint16:		push_unsigned_string(L, v->UInt16); break;
		case uint32:		push_unsigned_string(L, v->UInt32); break;
		case uint64:		push_unsigned_string(L, v->UInt64); break;
		case _string:		lua_pushstring(L, v->String); break;
		case _wstring:		lua_pushwstring(L, v->WString); break;
		case _pointer:		lua_pushfstring(L, "0x%p", v->Pointer); break;
	}
	return 1;
}

LUA_METHOD(Value, __tostring) {
	return tostring(L, lua_self(L, 1, Value));
}

LUA_METHOD(Value, __concat) {
	luaL_tolstring(L, 1, NULL);
	luaL_tolstring(L, 2, NULL);
	lua_concat(L, 2);
	return 1;
}

#include <intsafe.h>
#include <float.h>
#include <limits.h>
#include <wchar.h>

static int64_t asSigned(Value *v) {
	switch (v->kind) {
		case _uchar:		return v->UChar; break;
		case _char:			return v->Char; break;
		case _wchar_t:		return v->WChar; break;
		case _ushort:		return v->UShort; break;
		case _uint:			return v->UInt; break;
		case _ulong:		return v->ULong; break;
		case _size:			return v->Size; break;
		case _long:			return v->Long; break;
		case _longlong:		return v->LongLong; break;
		case _ulonglong:	return v->ULongLong; break;
		case _int:			return v->Int; break;
		case _short:		return v->Short; break;
		case _bool:			return v->Bool; break;
		case _float:		return v->Float; break;
		case _double:		return v->Double; break;
		case int16:			return v->Int16; break;
		case int32:			return v->Int32; break;
		case int64:			return v->Int64; break;
		case uint16:		return v->UInt16; break;
		case uint32:		return v->UInt32; break;
		case uint64:		return v->UInt64; break;
		case _string:		strtod(v->String, NULL); break;
		case _wstring:		wcstod(v->WString, NULL); break;
		case _pointer:		return *((double *)v->Pointer);
	}	
	return 0;
}

static uint64_t asUnsigned(Value *v) {
	switch (v->kind) {
		case _uchar:		return v->UChar; break;
		case _char:			return v->Char; break;
		case _wchar_t:		return v->WChar; break;
		case _ushort:		return v->UShort; break;
		case _uint:			return v->UInt; break;
		case _ulong:		return v->ULong; break;
		case _size:			return v->Size; break;
		case _long:			return v->Long; break;
		case _longlong:		return v->LongLong; break;
		case _ulonglong:	return v->ULongLong; break;
		case _int:			return v->Int; break;
		case _short:		return v->Short; break;
		case _bool:			return v->Bool; break;
		case _float:		return v->Float; break;
		case _double:		return v->Double; break;
		case int16:			return v->Int16; break;
		case int32:			return v->Int32; break;
		case int64:			return v->Int64; break;
		case uint16:		return v->UInt16; break;
		case uint32:		return v->UInt32; break;
		case uint64:		return v->UInt64; break;
		case _string:		strtod(v->String, NULL); break;
		case _wstring:		wcstod(v->WString, NULL); break;
		case _pointer:		return *((double *)v->Pointer);
	}	
	return 0;
}

static double asDouble(Value *v) {
	switch (v->kind) {
		case _uchar:		return v->UChar; break;
		case _char:			return v->Char; break;
		case _wchar_t:		return v->WChar; break;
		case _ushort:		return v->UShort; break;
		case _uint:			return v->UInt; break;
		case _ulong:		return v->ULong; break;
		case _size:			return v->Size; break;
		case _long:			return v->Long; break;
		case _longlong:		return v->LongLong; break;
		case _ulonglong:	return v->ULongLong; break;
		case _int:			return v->Int; break;
		case _short:		return v->Short; break;
		case _bool:			return v->Bool; break;
		case _float:		return v->Float; break;
		case _double:		return v->Double; break;
		case int16:			return v->Int16; break;
		case int32:			return v->Int32; break;
		case int64:			return v->Int64; break;
		case uint16:		return v->UInt16; break;
		case uint32:		return v->UInt32; break;
		case uint64:		return v->UInt64; break;
		case _string:		strtod(v->String, NULL); break;
		case _wstring:		wcstod(v->WString, NULL); break;
		case _pointer:		return *((double *)v->Pointer);
	}	
	return 0;
}

double get_number(lua_State *L, Value *v) {
	switch (v->kind) {
		case _ushort:		return v->UShort; break;
		case _uint:			return v->UInt; break;
		case _ulong:		return v->ULong; break;
		case _size:			return v->Size; break;
		case _ulonglong:	return v->ULongLong; break;
		case _int:			return v->Int; break;
		case _short:		return v->Short; break;
		case _bool:			return v->Bool; break;
		case _float:		return v->Float; break;
		case _double:		return v->Double; break;
		case int16:			return v->Int16; break;
		case int32:			return v->Int32; break;
		case int64:			return v->Int64; break;
		case uint16:		return v->UInt16; break;
		case uint32:		return v->UInt32; break;
		case uint64:		return v->UInt64;
	}	
	return luaL_error(L, "cannot convert %s Value to number", cnames[v->kind]);
}

//-------------------------------------[ Value.__eq() ]
LUA_METHOD(Value, __eq) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2;
	
	get_value(L, 2, &v2);
	if (v1->kind > _double) {
		switch(v1->kind) {
			case _string: 
			case _wstring:
			case _pointer: lua_pushboolean(L, v1->Pointer == v2.Pointer); break;	
		}
	} else {
		double num1 = get_number(L, v1);
		double num2 = get_number(L, &v2);
		lua_pushboolean(L, num1 == num2);
	}
	return 1;
}

//-------------------------------------[ Value.__lt() ]
LUA_METHOD(Value, __lt) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2;
	
	get_value(L, 2, &v2);
	if (v1->kind > _double)
		lua_pushboolean(L, v1->Pointer < v2.Pointer);	
	else {
		double num1 = get_number(L, v1);
		double num2 = get_number(L, &v2);
		lua_pushboolean(L, num1 < num2);
	}
	return 1;
}


//-------------------------------------[ Value.__le() ]
LUA_METHOD(Value, __le) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2;
	
	get_value(L, 2, &v2);
	if (v1->kind > _double)
		lua_pushboolean(L, v1->Pointer <= v2.Pointer);
	else {
		double num1 = get_number(L, v1);
		double num2 = get_number(L, &v2);
		lua_pushboolean(L, num1 <= num2);
	}
	return 1;
}


//-------------------------------------[ Value.__add() ]
LUA_METHOD(Value, __add) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2, result;
	
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _uchar:		result.UChar = v1->UChar + (unsigned char)asUnsigned(&v2); break;
		case _ushort:		result.UShort = v1->UShort + (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt + (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong + (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size + (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong + (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int + (int)asSigned(&v2); break;
		case _char:			result.Char = v1->Char + (char)asSigned(&v2); break;
		case _wchar_t:		result.WChar = v1->WChar + (wchar_t)asSigned(&v2); break;
		case _short:		result.Short = v1->Short + (short)asSigned(&v2); break;
		case _float:		result.Float = v1->Float + asDouble(&v2); break;
		case _double:		result.Double = v1->Double + asDouble(&v2); break;
		case int16:			result.Int16 = v1->Int16 + (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 + (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 + asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 + (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 + (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 + asUnsigned(&v2); break;
		case _string:		result.String = v1->String + asSigned(&v2); result.owned = TRUE; result.size = strlen(result.String); break;
		case _wstring:		result.WString = v1->WString + asSigned(&v2); result.owned = TRUE;  result.size = wcslen(result.WString); break;
		case _pointer:		result.Pointer = ((char*)v1->Pointer) + asSigned(&v2); break;
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__sub() ]
LUA_METHOD(Value, __sub) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2, result;
	
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _uchar:		result.UChar = v1->UChar - (unsigned char)asUnsigned(&v2); break;
		case _ushort:		result.UShort = v1->UShort - (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt - (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong - (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size - (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong - (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int - (int)asSigned(&v2); break;
		case _char:			result.Char = v1->Char - (char)asSigned(&v2); break;
		case _wchar_t:		result.WChar = v1->WChar - (wchar_t)asSigned(&v2); break;
		case _short:		result.Short = v1->Short - (short)asSigned(&v2); break;
		case _float:		result.Float = v1->Float - asDouble(&v2); break;
		case _double:		result.Double = v1->Double - asDouble(&v2); break;
		case int16:			result.Int16 = v1->Int16 - (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 - (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 - asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 - (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 - (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 - asUnsigned(&v2); break;
		case _string:		result.String = v1->String - asSigned(&v2); result.size = strlen(result.String); break;
		case _wstring:		result.WString = v1->WString - asSigned(&v2); result.size = wcslen(result.WString); break;
		case _pointer:		result.Pointer = ((char*)v1->Pointer) - asSigned(&v2); break;
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;

}

//-------------------------------------[ Value.__mul() ]
LUA_METHOD(Value, __mul) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2, result;
	
	if (v1->kind > _double) 
		luaL_error(L, "cannot perform multiplication on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort * (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt * (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong * (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size * (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong * (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int * (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short * (short)asSigned(&v2); break;
		case _float:		result.Float = v1->Float * asDouble(&v2); break;
		case _double:		result.Double = v1->Double * asDouble(&v2); break;
		case int16:			result.Int16 = v1->Int16 * (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 * (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 * asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 * (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 * (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 * asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__div() ]
LUA_METHOD(Value, __div) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2, result;
	
	if (v1->kind > _double) 
		luaL_error(L, "cannot perform division on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort / (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt / (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong / (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size / (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong / (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int / (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short / (short)asSigned(&v2); break;
		case _float:		result.Float = v1->Float / asDouble(&v2); break;
		case _double:		result.Double = v1->Double / asDouble(&v2); break;
		case int16:			result.Int16 = v1->Int16 / (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 / (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 / asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 / (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 / (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 / asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;

}

//-------------------------------------[ Value.__mod() ]
LUA_METHOD(Value, __mod) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform modulo on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort % (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt % (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong % (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size % (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong % (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int % (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short % (short)asSigned(&v2); break;
		case int16:			result.Int16 = v1->Int16 % (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 % (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 % asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 % (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 % (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 % asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__band() ]
LUA_METHOD(Value, __band) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'and' on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort & (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt & (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong & (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size & (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong & (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int & (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short & (short)asSigned(&v2); break;
		case int16:			result.Int16 = v1->Int16 & (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 & (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 & asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 & (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 & (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 & asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__bor() ]
LUA_METHOD(Value, __bor) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'or' on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort | (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt | (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong | (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size | (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong | (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int | (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short | (short)asSigned(&v2); break;
		case int16:			result.Int16 = v1->Int16 | (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 | (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 | asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 | (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 | (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 | asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__bxor() ]
LUA_METHOD(Value, __bxor) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'xor' on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort ^ (unsigned short)asUnsigned(&v2); break;
		case _uint:			result.UInt = v1->UInt ^ (unsigned int)asUnsigned(&v2); break;
		case _ulong:		result.ULong = v1->ULong ^ (unsigned long)asUnsigned(&v2); break;
		case _size:			result.Size = v1->Size ^ (size_t)asUnsigned(&v2); break;
		case _ulonglong:	result.ULongLong = v1->ULongLong ^ (unsigned long long)asUnsigned(&v2); break;
		case _bool:			
		case _int:			result.Int = v1->Int ^ (int)asSigned(&v2); break;
		case _short:		result.Short = v1->Short ^ (short)asSigned(&v2); break;
		case int16:			result.Int16 = v1->Int16 ^ (int16_t)asSigned(&v2); break;
		case int32:			result.Int32 = v1->Int32 ^ (int32_t)asSigned(&v2); break;
		case int64:			result.Int64 = v1->Int64 ^ asSigned(&v2); break;
		case uint16:		result.UInt16 = v1->UInt16 ^ (uint16_t)asUnsigned(&v2); break;
		case uint32:		result.UInt32 = v1->UInt32 ^ (uint32_t)asUnsigned(&v2); break;
		case uint64:		result.UInt64 = v1->UInt64 ^ asUnsigned(&v2);
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__bnot() ]
LUA_METHOD(Value, __bnot) {
	Value *v1 = lua_self(L, 1, Value);
	Value result;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'not'on a %s Value", cnames[v1->kind]);
	
	switch(v1->kind) {
		case _ushort:		result.UShort = ~v1->UShort; break;
		case _uint:			result.UInt = ~v1->UInt; break;
		case _ulong:		result.ULong = ~v1->ULong ; break;
		case _size:			result.Size = ~v1->Size ; break;
		case _ulonglong:	result.ULongLong = ~v1->ULongLong ; break;
		case _int:			result.Int = ~v1->Int; break;
		case _short:		result.Short = ~v1->Short ; break;
		case _bool:			result.Bool = ~v1->Bool ; break;
		case int16:			result.Int16 = ~v1->Int16 ; break;
		case int32:			result.Int32 = ~v1->Int32 ; break;
		case int64:			result.Int64 = ~v1->Int64 ; break;
		case uint16:		result.UInt16 = ~v1->UInt16 ; break;
		case uint32:		result.UInt32 = ~v1->UInt32 ; break;
		case uint64:		result.UInt64 = ~v1->UInt64 ;
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__shr() ]
LUA_METHOD(Value, __shr) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	lua_Integer num;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'right shift' on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	num = get_number(L, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort >> num; break;
		case _uint:			result.UInt = v1->UInt >> num; break;
		case _ulong:		result.ULong = v1->ULong >> num; break;
		case _size:			result.Size = v1->Size >> num; break;
		case _ulonglong:	result.ULongLong = v1->ULongLong >> num; break;
		case _int:			result.Int = v1->Int >> num; break;
		case _short:		result.Short = v1->Short >> num; break;
		case _bool:			result.Bool = v1->Bool >> num; break;
		case int16:			result.Int16 = v1->Int16 >> num; break;
		case int32:			result.Int32 = v1->Int32 >> num; break;
		case int64:			result.Int64 = v1->Int64 >> num; break;
		case uint16:		result.UInt16 = v1->UInt16 >> num; break;
		case uint32:		result.UInt32 = v1->UInt32 >> num; break;
		case uint64:		result.UInt64 = v1->UInt64 >> num;
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__shl() ]
LUA_METHOD(Value, __shl) {
	Value *v1 = lua_self(L, 1, Value);
	Value v2 = {0};
	Value result;
	lua_Integer num;
	
	if (v1->kind < _short && v1->kind > uint64) 
		luaL_error(L, "cannot perform bitwise 'left shift' on a %s Value", cnames[v1->kind]);
	get_value(L, 2, &v2);
	num = get_number(L, &v2);
	switch(v1->kind) {
		case _ushort:		result.UShort = v1->UShort << num; break;
		case _uint:			result.UInt = v1->UInt << num; break;
		case _ulong:		result.ULong = v1->ULong << num; break;
		case _size:			result.Size = v1->Size << num; break;
		case _ulonglong:	result.ULongLong = v1->ULongLong << num; break;
		case _int:			result.Int = v1->Int << num; break;
		case _short:		result.Short = v1->Short << num; break;
		case _bool:			result.Bool = v1->Bool << num; break;
		case int16:			result.Int16 = v1->Int16 << num; break;
		case int32:			result.Int32 = v1->Int32 << num; break;
		case int64:			result.Int64 = v1->Int64 << num; break;
		case uint16:		result.UInt16 = v1->UInt16 << num; break;
		case uint32:		result.UInt32 = v1->UInt32 << num; break;
		case uint64:		result.UInt64 = v1->UInt64 << num;
	}
	result.kind = v1->kind;
	lua_pushlightuserdata(L, &result);
	lua_pushinstance(L, Value, 1);
	return 1;
}

//-------------------------------------[ Value.__gc() ]
LUA_METHOD(Value, __gc) {
	Value *v = lua_self(L, 1, Value);

	if (v->kind >= _string && !v->owned) {
		free(v->Pointer);
		v->Pointer = NULL;
	}
	free(v);
	return 0;
}

//-------------------------------------[ Value.__len() ]
LUA_METHOD(Value, __len) {
	lua_pushinteger(L, lua_self(L, 1, Value)->size);
	return 1;
}

//-------------------------------------[ Value.as() ]
LUA_METHOD(Value, as) {
	lua_pushvalue(L, 2);
	lua_pushlightuserdata(L, lua_self(L, 1, Value)->Pointer);
	lua_pushinstance(L, Value, 2);
	return 1;
}

//-------------------------------------[ Value.type ]
LUA_PROPERTY_GET(Value, type) {
	lua_pushstring(L, ctypes[lua_self(L, 1, Value)->kind]);
	return 1;
}

// //-------------------------------------[ Value.__concat() ]
// LUA_METHOD(Value, __concat) {
// 	Value *v1 = lua_self(L, 1, Value);
// 	Value v2 = {0};
// 	Value result;
	
// 	if (v1->kind != _string && v1->kind != _wstring) 
// 		luaL_error(L, "cannot perform contatenation on a %s Value", cnames[v1->kind]);
// 	get_value(L, 2, &v2);
// 	return 1;
// }

OBJECT_METAFIELDS(Value)
	METHOD(Value, __call)
	METHOD(Value, __tostring)
	METHOD(Value, __concat)
	METHOD(Value, __eq)
	METHOD(Value, __add)
	METHOD(Value, __sub)
	METHOD(Value, __mul)
	METHOD(Value, __div)
	METHOD(Value, __mod)
	METHOD(Value, __band)
	METHOD(Value, __bor)
	METHOD(Value, __bxor)
	METHOD(Value, __bnot)
	METHOD(Value, __shr)
	METHOD(Value, __shl)
	METHOD(Value, __gc)
	METHOD(Value, __len)
END

OBJECT_MEMBERS(Value)
	METHOD(Value, as)
	READONLY_PROPERTY(Value, type)
END
