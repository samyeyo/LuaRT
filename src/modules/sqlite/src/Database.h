/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Database.h | LuaRT Database object header
*/

#pragma once

#include <luart.h>

#define CPP_SQLITE_NOTHROW
#include "sqlite3.h"

//---------------- Database object

typedef struct {
    luart_type  type;
	sqlite3		*database;
    wchar_t     *fname;
} Database;

extern luart_type TDatabase;

LUA_CONSTRUCTOR(Database);
extern const luaL_Reg Database_methods[];
extern const luaL_Reg Database_metafields[];