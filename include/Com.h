/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | COM.h | LuaRT COM object header
*/

#include <luart.h>
#include <stdlib.h>
#include <ole2.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- COM object
typedef struct {
	luart_type	type;
	IDispatch 	*this;
	ITypeInfo	*typeinfo;
	wchar_t		*name;
} COM;

extern luart_type TCOM;

//---------------------------------------- COM type
LUA_CONSTRUCTOR(COM);
extern const luaL_Reg COM_methods[];
extern const luaL_Reg COM_metafields[];

#ifdef __cplusplus
}
#endif