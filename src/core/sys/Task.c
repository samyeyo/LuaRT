/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Task.c | LuaRT Task object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Task.h>

#include "async.h"

luart_type TTask;


//----------------------------------[ Task constructor ]
LUA_CONSTRUCTOR(Task) {
	Task *tt, *t = calloc(1, sizeof(Task));

	t->L = lua_newthread(L);
	t->status = TCreated;
	t->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	if ((tt = search_task(L)))
		t->from = tt;
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_pushvalue(L, 2);
	lua_xmove(L, t->L, 1);	
	lua_pushvalue(L, 1);
	t->taskref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_newinstance(L, t, Task);
	return 1;
}

//----------------------------------[ Task.cancel() ]
LUA_METHOD(Task, cancel) {
	Task *t = lua_self(L, 1, Task);
	if (t->status >= TRunning) {
		lua_pushboolean(L, TRUE);
		close_task(L, t);
	} else lua_pushboolean(L, FALSE); 
	return 1;
}

//----------------------------------[ Task.wait() method ]
LUA_METHOD(Task, wait) {
	Task *t = lua_self(L, 1, Task);
	int nresults = lua_gettop(L);
	
	while(t->status != TTerminated)
		update_tasks(L, t);
	return lua_gettop(L)-nresults;
}

//----------------------------------[ Task.terminated property ]
LUA_PROPERTY_GET(Task, terminated) {
	lua_pushboolean(L, lua_self(L, 1, Task)->status == TTerminated);
	return 1;
}

//----------------------------------[ Task.status property ]
LUA_PROPERTY_GET(Task, status) {
	static const char *status[] = { "running", "created", "sleeping", "terminated"};
	lua_pushstring(L, status[lua_self(L, 1, Task)->status]);
	return 1;
}

//----------------------------------[ Task object definition ]
OBJECT_MEMBERS(Task)
	READONLY_PROPERTY(Task, terminated)
	READONLY_PROPERTY(Task, status)
	METHOD(Task, cancel)
	METHOD(Task, wait)
END

//----------------------------------[ Task instance call ]
LUA_METHOD(Task, __call) {
	return start_task(L, lua_self(L, 1, Task), lua_gettop(L));
}

//----------------------------------[ Task destructor ]
LUA_METHOD(Task, __gc) {
	Task *t = lua_self(L, 1, Task);
	if (t->status != TTerminated)
		close_task(L, t);
	free(t);
	return 0;
}

OBJECT_METAFIELDS(Task)
	METHOD(Task, __call)
	METHOD(Task, __gc)
END