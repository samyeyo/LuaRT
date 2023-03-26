/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Date.h | LuaRT Date object header
*/

#pragma once

#include <luart.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _SECOND ((__int64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)
#define _MONTH  (31 * _DAY)
#define _YEAR   (12 * _MONTH)

typedef struct {
	luart_type		type;
	void			*owner;
	void			(*update)(lua_State *L, void *owner);
	SYSTEMTIME	*st;
} Datetime;

extern luart_type TDatetime;

LUA_CONSTRUCTOR(Datetime);
extern const luaL_Reg Datetime_methods[];
extern const luaL_Reg Datetime_metafields[];


#ifdef __cplusplus
}
#endif