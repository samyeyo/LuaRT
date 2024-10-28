/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | async.cpp | LuaRT async module
*/

#define LUA_LIB

#include <luart.h>
#include <Task.h>
#include <list>
#include "async.h"
#include "../lrtapi.h"

static std::list<Task *> Tasks;
extern "C" {
	extern lua_CFunction 		update;
}

Task *create_task(lua_State *L) {
	Task *tt, *t = (Task*)calloc(1, sizeof(Task));

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
	Tasks.push_back(t);
	return t;	
}

//-------- Search for the current running Task
LUA_API Task *search_task(lua_State *L) {
	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) 
		if ((*it)->L == L)
			return (*it);		
	return NULL;
} 

//-------- Start a created Task
int start_task(lua_State *L, Task *t, int nargs) {	
	if (nargs) {
		for (int i = 2; i <= nargs+1; i++)
			lua_pushvalue(L, i);
		lua_xmove(L, t->L, nargs);
	}
	t->status = TRunning;
	nargs = resume_task(L, t, nargs);
	if (nargs == -1)
		lua_error(t->L);
	return t->status == TTerminated ? nargs : 0;
}

//-------- Resume a Task
int resume_task(lua_State *L, Task *t, int args) {
	int nresults = 0, status;
	int nargs = args != -1 ? args : lua_gettop(t->L)-1;
	lua_State *from = t->from ? t->from->L : L;

	if ( (t->status != TTerminated) && (status = lua_resume(t->L, from, t->status == TSleep ? 0 : nargs, &nresults)) > 1 )
		return -1;
	else if (status == LUA_YIELD) {
		lua_xmove(t->L, from, nresults);
		t->status = TSleep;	
	} else if (status == LUA_OK) {
		if (nresults) {
			if (lua_rawgeti(t->L, LUA_REGISTRYINDEX, t->taskref)) {
				if (lua_getfield(t->L, -1, "after") == LUA_TFUNCTION) {
					lua_insert(t->L, -nresults-2);
					lua_insert(t->L, -nresults-2);
					lua_call(t->L, nresults, LUA_MULTRET);
					close_task(t);
					return 0;
				} 
				lua_pop(t->L, 1);
			} 
			lua_pop(t->L, 1);
			lua_xmove(t->L, from, nresults);
		}
		close_task(t);
	} 
	return nresults;
}

//-------- Task scheduler
int update_tasks(lua_State *L) {
	bool terminated = false;

	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) {
		Task *t = *it;
		switch (t->status) {
			case TTerminated:	terminated = true;
								break;
			case TSleep:	
							if (t->sleep < GetTickCount64()) {	
								t->sleep = 0;
								t->status = TRunning;
							} break;

			case TRunning:	{
								if (lua_status(t->L) == LUA_YIELD) {
									int nresults = resume_task(L, t, -1);
									if (nresults == -1)
										lua_error(t->L);
									if ((t->status == TTerminated) && (t->waiting)) {
										t->waiting->status = TRunning;
										t->waiting = NULL;	
										return nresults;									
									}										
								}	
							}
			default:		break;
		}		
	}
	if (terminated)
		for (auto it = Tasks.begin(); it != Tasks.end();) {
			Task *tt = *it;
			if (tt->status == TTerminated) {
				luaL_unref(L, LUA_REGISTRYINDEX, tt->ref);
				luaL_unref(L, LUA_REGISTRYINDEX, tt->taskref);
				tt->ref = -1;
				it = Tasks.erase(it);
			} else ++it;
		}	
	return 0;
}

//-------- Wait for a Task at specific index
int waitfor_task(lua_State *L, int idx) {
	Task *t = lua_self(L, idx, Task);
	int nresults = lua_gettop(L);

	t->waiting = search_task(L);
	t->waiting->status = TWaiting;	
	do
		nresults = update_tasks(L);
	while(t->status != TTerminated);	
	return nresults;
}

//-------- Wait for all Tasks
int waitall_tasks(lua_State *L) {
    do
		update_tasks(L);
	while (Tasks.size() > 1);		
    return 0;
}
