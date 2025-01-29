 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Database.c | LuaRT Database object implementation
*/
#include <luart.h>
#include <File.h>

#include "sqlite3.h"
#include "Database.h"


luart_type TDatabase;

#define CHECK(db, result) check_error(L, db, result, NULL)
#define CHECK_STMT(db, result, stmt) check_error(L, db, result, stmt)

static void check_error(lua_State *L, Database *db, BOOL result, sqlite3_stmt *stmt) {
	if (result) {
		if (stmt)
			sqlite3_finalize(stmt);
		luaL_error(L, "SQLite error: %s", sqlite3_errmsg(db->database));
	}
}

//-------------------------------------[ Database Constructor ]
LUA_CONSTRUCTOR(Database) {
	Database *db = (Database *)calloc(1, sizeof(Database));
	wchar_t *fname = luaL_checkFilename(L, 2); 
	
	CHECK(db, sqlite3_open16(fname, &db->database));
	db->fname = fname;
	lua_newinstance(L, db, Database);
	return 1;
}

//-------------------------------------[ Database.close() ]
LUA_METHOD(Database, close) {
	Database *db = lua_self(L, 1, Database);
	CHECK(db, sqlite3_close(db->database));
	return 0;
}

static sqlite3_stmt *run_query(lua_State *L, Database *db) {
	int len;	
	wchar_t *str = lua_tolwstring(L, 2, &len);
	int nargs = lua_gettop(L);
	sqlite3_stmt *stmt;
	int type, bind_count;
	
	CHECK(db, sqlite3_prepare16_v2(db->database, str, len*sizeof(wchar_t), &stmt, 0));
	free(str);
	
	if ((bind_count = sqlite3_bind_parameter_count(stmt)) > nargs-2)
		luaL_error(L, "SQLite error: not enough parameters (expecting %d binding values, found only %d)", bind_count, nargs-2);

	for (int i = 3; i <= nargs; i++) {
		switch ((type = lua_type(L, i))) {
			case LUA_TSTRING:	{
									wchar_t *text = lua_tolwstring(L, i, &len);
									CHECK_STMT(db, sqlite3_bind_text16(stmt, i-2, text, len*sizeof(wchar_t), SQLITE_TRANSIENT), stmt);
									free(text);
									break;
								}
			case LUA_TNUMBER:	if (lua_isinteger(L, i))
									CHECK_STMT(db, sqlite3_bind_int64(stmt, i-2,lua_tointeger(L, i)), stmt);
								else
									CHECK_STMT(db, sqlite3_bind_double(stmt, i-2, lua_tonumber(L, i)), stmt);
								break;
			default:			CHECK_STMT(db, sqlite3_bind_text(stmt, i-2, luaL_tolstring(L, i, NULL), -1, SQLITE_TRANSIENT), stmt);
								lua_pop(L, 1);
		}
	}
	return stmt;	
}

static int push_row(lua_State *L, Database *db, sqlite3_stmt *stmt, int count) {
	if (sqlite3_step(stmt) == SQLITE_ROW ) {
		lua_createtable(L, 0, count);
		for (int i = 0; i < count; i++) {
			const wchar_t *name = sqlite3_column_name16(stmt, i);
			lua_pushwstring(L, name);
			switch(sqlite3_column_type(stmt, i)) {
				case SQLITE_INTEGER:	lua_pushinteger(L, sqlite3_column_int64(stmt, i)); break;
				case SQLITE_FLOAT:		lua_pushnumber(L, sqlite3_column_double(stmt, i)); break;
				case SQLITE_TEXT:		lua_pushwstring(L, sqlite3_column_text16(stmt, i)); break;
				default:				lua_pushlstring(L, sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i));
			}
			printf("%ls = %s\n", name, lua_tostring(L, -1));
			lua_rawset(L, -3);
		}

		return 1;
	}
	CHECK(db, sqlite3_finalize(stmt));
	return 0;
}

//-------------------------------------[ Database.exec() ]
LUA_METHOD(Database, exec) {
	Database *db = lua_self(L, 1, Database);
	sqlite3_stmt *stmt = run_query(L, db);
	int count = 0, colcount = sqlite3_column_count(stmt);
	
	while(push_row(L, db, stmt, colcount))
		count++;
	return count;
}

//-------------------------------------[ Database.query() ]
static int row_iterator(lua_State *L) {	
	return push_row(L, (Database*)lua_touserdata(L, lua_upvalueindex(1)), (sqlite3_stmt*)lua_touserdata(L, lua_upvalueindex(2)), lua_tointeger(L, lua_upvalueindex(3)));
}

LUA_METHOD(Database, query) {
	Database *db = lua_self(L, 1, Database);
	sqlite3_stmt *stmt = run_query(L, db);
	int idx = 0;
	
	lua_pushlightuserdata(L, db);
	lua_pushlightuserdata(L, stmt);
	lua_pushinteger(L, sqlite3_column_count(stmt));
	lua_pushcclosure(L, row_iterator, 3);
	return 1;
}

//-------------------------------------[ Database.file ]
LUA_PROPERTY_GET(Database, file) {
	Database *db = lua_self(L, 1, Database);
	if (wcscmp(L":memory:", db->fname)) {
		lua_pushwstring(L, db->fname);
		lua_pushinstance(L, File, 1);
	} else lua_pushstring(L, ":memory:");
	return 1;
}

OBJECT_MEMBERS(Database)
	METHOD(Database, exec)
	METHOD(Database, query)
	METHOD(Database, close)
	READONLY_PROPERTY(Database, file)
END

LUA_METHOD(Database, __gc) {
	Database *db = lua_self(L, 1, Database);
	if (db->database)
		sqlite3_close(db->database);
	free(db->fname);
    free(db);
    return 0;
}

OBJECT_METAFIELDS(Database)
    METHOD(Database, __gc)
END
