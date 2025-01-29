/*
 | Sqlite for LuaRT
 | Luart.org, Copyright (c) Tine Samir 2025.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Sqlite.c | LuaRT sqlite binary module
*/

#include <luart.h>
#include "Database.h"

//-------------------------------------[ sqlite.version ]
LUA_PROPERTY_GET(sqlite, version) {	
	lua_pushstring(L, SQLITE_VERSION);
	return 1;
}

//------------------------------- sqlite module properties definition

MODULE_PROPERTIES(sqlite)
	READONLY_PROPERTY(sqlite, version)
END

//------------------------------- sqlite module functions definition

MODULE_FUNCTIONS(sqlite)
END

//----- sqlite module registration function
int __declspec(dllexport) luaopen_sqlite(lua_State *L)
{
	lua_regmodule(L, sqlite);
	lua_regobjectmt(L, Database);
	return 1;
}
