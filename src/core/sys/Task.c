/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
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
	lua_newinstance(L, create_task(L), Task);
	return 1;
}

//----------------------------------[ Task.cancel() ]
LUA_METHOD(Task, cancel) {
	Task *t = lua_self(L, 1, Task);
	if (t->status < TTerminated) {
		lua_pushboolean(L, TRUE);
		close_task(L, t);
	} else lua_pushboolean(L, FALSE); 
	return 1;
}

//----------------------------------[ Task.wait() method ]
LUA_METHOD(Task, wait) {
	Task *t = lua_self(L, 1, Task);
	int nresults = lua_gettop(L);

	t->waiting = search_task(L);
	t->waiting->status = TWaiting;	
	do
		update_tasks(L);
	while(t->status != TTerminated);	
	return lua_gettop(L)-nresults;
}

//----------------------------------[ Task.terminated property ]
LUA_PROPERTY_GET(Task, terminated) {
	lua_pushboolean(L, lua_self(L, 1, Task)->status == TTerminated);
	return 1;
}

//----------------------------------[ Task.status property ]
LUA_PROPERTY_GET(Task, status) {
	static const char *status[] = { "running", "created", "sleeping", "waiting", "terminated"};
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