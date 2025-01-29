/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Array.h | LuaRT Array object header
*/

#pragma once

#include <luart.h>
#include "Struct.h"
#include "Union.h"
#include "Pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Array Array;
typedef struct _Pointer Pointer;

//---------------------------------------- Array object
typedef struct _Array {
	luart_type 	type;
	ctype		kind;			//--- type of data
	size_t 		dimensions[63];	//--- list of dimensions
	size_t		ndim;			//--- number of dimensions
	size_t 		size;			//--- size of element
	char		*data;			//--- content of Array
	const char  *name;
	BOOL		owned;
	union {
		Struct  *Struct;
		Union	*Union;
		Array   *Array;
		Pointer *Pointer;
	};
} Array;

extern luart_type TArray;

LUA_CONSTRUCTOR(Array);
extern const luaL_Reg Array_methods[];
extern const luaL_Reg Array_metafields[];

size_t array_size(Array *a);
size_t array_length(Array *a);

#ifdef __cplusplus
}
#endif