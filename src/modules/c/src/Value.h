/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Value.h | LuaRT Value object header
*/

#pragma once 

#include <luart.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Value object

typedef enum {
	_uchar,	_char, _wchar_t, _bool, _short, _ushort, _int, _uint, _long, _longlong, _ulong,  _ulonglong, _size, int16, int32, int64, uint16, uint32, uint64,_float, _double, _string, _void, _wstring, _struct, _union, _array
} ctype;

static const char *csig[] = {
	"C", "c", "w", "B", "s", "S", "i", "I", "j", "l", "J", "L", "L", "#", "j", "l", "S", "J", "L", "f", "d", "z", "p", "W", ".", "u", NULL};

static const int csizes[] = {
	sizeof(unsigned char), sizeof(char), sizeof(wchar_t), sizeof(BOOL), sizeof(short), sizeof(unsigned short), sizeof(int), sizeof(unsigned int), sizeof(long), sizeof(long long), sizeof (unsigned long), sizeof(unsigned long long), sizeof(size_t), sizeof(int16_t), sizeof(int32_t), sizeof(int64_t), sizeof(uint16_t), sizeof(uint32_t), sizeof(uint64_t), sizeof(float), sizeof(double), 1, 1, 2
};

static const char *cnames[] = {
	"unsigned char", "char", "wchar_t", "bool", "short", "unsigned short", "int", "unsigned int", "long", "long long", "unsigned long", "unsigned long long", "size_t", "int16_t", "int32_t", "int64_t", "uint16_t", "uint32_t", "uint64_t", "float", "double", "char*", "void*", "wchar_t*", NULL
};

static const char *ctypes[] = {
	"uchar", "char", "wchar_t", "bool", "short", "ushort", "int", "uint", "long", "longlong", "ulong", "ulonglong", "size_t", "int16_t", "int32_t", "int64_t", "uint16_t", "uint32_t", "uint64_t", "float", "double", "string", "void", "wstring", NULL
};

typedef struct {
	luart_type 	type;
	ctype		kind;
	size_t		size;
	BOOL		owned;
	ctype		pkind;
	union {
		unsigned char		UChar;
		char				Char;
		wchar_t				WChar;
		short int			Short;
		unsigned short int  UShort;
		int					Int;
		unsigned int		UInt;
		int					Bool;
		long				Long;
		unsigned long		ULong;
		long long			LongLong;
		unsigned long long	ULongLong;
		float				Float;
		double				Double;
		size_t				Size;
		int16_t				Int16;
		int32_t				Int32;
		int64_t				Int64;
		uint16_t			UInt16;
		uint32_t			UInt32;
		uint64_t			UInt64;
		char*				String;
		wchar_t*			WString;
	};
} Value;

extern luart_type TValue;

LUA_CONSTRUCTOR(Value);
extern const luaL_Reg Value_methods[];
extern const luaL_Reg Value_metafields[];

ctype sigchar_toctype(lua_State *L, char sig);

void get_value(lua_State *L, int idx, Value *v);
extern void *topointer(lua_State *L, int idx);
extern void *obj_topointer(lua_State *L, void *val, luart_type type, size_t *size);

#ifdef __cplusplus
}
#endif