/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | async.c | LuaRT async module
*/

#define LUA_LIB

#include <luart.h>
#include <Task.h>
#include <list>
#include "async.h"

lua_State *mainL;
static std::list<Task *> Tasks;

//-------- Search for the current running Task
Task *search_task(lua_State *L) {
	if (Tasks.size())
		for (auto it = Tasks.begin(); it != Tasks.end(); ++it) 
			if ((*it)->L == L)
				return (*it);		
	return NULL;
} 

//-------- Start a created Task
int start_task(lua_State *L, Task *t, int args) {
	int nargs = args;// != -1 ? args : lua_gettop(L);
	
	if (nargs) {
		for (int i = 2; i <= nargs+1; i++)
			lua_pushvalue(L, i);
		lua_xmove(L, t->L, nargs);
	}
	Tasks.push_back(t);
	t->status = TRunning;
	nargs = resume_task(L, t, nargs);
	if (nargs == -1)
		lua_error(t->L);
	return t->status == TTerminated ? nargs : 0;
}

//-------- Close a Task
void close_task(lua_State *L, Task *t) {
	t->status = TTerminated;
	if ((t->ref > 0) && Tasks.size()) {
		for (auto it = Tasks.begin(); it != Tasks.end(); ++it) {
			Task *tt = *it;
			if (t->from && tt->from == t)
				tt->status = TTerminated;
		}
	}	
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
		if (lua_rawgeti(t->L, LUA_REGISTRYINDEX, t->taskref)) {
			if (lua_getfield(t->L, -1, "after") == LUA_TFUNCTION) {
				lua_remove(t->L, -2);
				lua_insert(t->L, -nresults-1);
				lua_call(t->L, nresults, LUA_MULTRET);
				close_task(L, t);
				return 0;
			} 
			lua_pop(t->L, 1);
		} 
		lua_pop(t->L, 1);
		lua_xmove(t->L, from, nresults);
		close_task(L, t);
	} 
	return nresults;
}

//-------- Task scheduler
int update_tasks(lua_State *waitingState, Task *waitedTask) {
loop:
	if (Tasks.size())
		for (auto it = Tasks.begin(); it != Tasks.end(); ++it) {
			Task *t = *it;
			switch (t->status) {
				case TSleep:	
								if (t->sleep < GetTickCount64()) {	
									t->sleep = 0;
									t->status = TRunning;
								} break;

				case TRunning:	{
									if (t->L != waitingState) {
run:									int nresults = resume_task(waitingState, waitedTask ? waitedTask : t, -1);
										if (nresults == -1)
											lua_error(t->L);
										if (t == waitedTask)
											return nresults;
									}
									if (t->status == TTerminated) {
										if (t->waiting) {
											t = t->waiting;
											t->status = TRunning;
											goto run;
										} 
										goto loop;
									}
								}
				default:		break;

			}
		}
	if (Tasks.size())		
		for (auto it = Tasks.begin(); it != Tasks.end();) {
			Task *tt = *it;
			if (tt->status == TTerminated) {
				luaL_unref(mainL, LUA_REGISTRYINDEX, tt->ref);
				luaL_unref(mainL, LUA_REGISTRYINDEX, tt->taskref);
				tt->ref = -1;
				it = Tasks.erase(it);
			} else ++it;
		}	
	return 0;
}

//-------- Wait for all tasks
int waitall_tasks(lua_State *L) {
    while (Tasks.size())
		update_tasks(L, NULL);
    return 0;
}
