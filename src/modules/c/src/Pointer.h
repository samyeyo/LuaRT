/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Pointer.h | LuaRT Pointer object header
*/

#pragma once

#include <luart.h> 
#include <stdint.h>
#include "Value.h"
#include "Union.h"
#include "Array.h"
#include "Struct.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *pnames[] = {
	"unsigned char*", "char*", "wchar_t*", "bool*", "short*", "unsigned short*", "int*", "unsigned int*", "long*", "long long*", "unsigned long*", "unsigned long long*", "size_t*", "int16_t*", "int32_t*", "int64_t*", "uint16_t*", "uint32_t*", "uint64_t*", "float*", "double*", "string", "void*", "wstring", NULL
};

//---------------------------------------- Pointer object
typedef struct _Pointer {
	luart_type 	type;
	ctype		kind;
	BOOL		autorelease;
	void		*ptr;
	union {
		Struct  *Struct;
		Union	*Union;
		struct _Array	*Array;
	};
} Pointer;


extern luart_type TPointer;

LUA_CONSTRUCTOR(Pointer);
extern const luaL_Reg Pointer_methods[];
extern const luaL_Reg Pointer_metafields[];

#ifdef __cplusplus
}
#endif