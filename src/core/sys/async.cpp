/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
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
static lua_CFunction lua_update = NULL;

void set_lua_update(lua_CFunction func) {
	lua_update = func;
}

static void hookf (lua_State *L, lua_Debug *ar) {
  static const char *const hooknames[] =
    {"call", "return", "line", "count", "tail call"};
  lua_getfield(L, LUA_REGISTRYINDEX, "_HOOKKEY");
  lua_pushthread(L);
  if (lua_rawget(L, -2) == LUA_TFUNCTION) {  /* is there a hook function? */
    lua_pushstring(L, hooknames[(int)ar->event]);  /* push event name */
    if (ar->currentline >= 0)
      lua_pushinteger(L, ar->currentline);  /* push current line */
    else lua_pushnil(L);
    lua_assert(lua_getinfo(L, "lS", ar));
    lua_call(L, 2, 0);  /* call hook function */
  }
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
	if (lua_getfield(L, LUA_REGISTRYINDEX, "_TASKHOOK")) {
		int mask, count;
		lua_getfield(L, -1, "mask");
		mask = lua_tointeger(L, -1);
		lua_getfield(L, -2, "count");
		count = lua_tointeger(L, -1);
		lua_pop(L, 2);
		if (!luaL_getsubtable(L, LUA_REGISTRYINDEX, "_HOOKKEY")) {
			lua_pushliteral(L, "k");
			lua_setfield(L, -2, "__mode"); 
			lua_pushvalue(L, -1);
			lua_setmetatable(L, -2);
		}
		if (l_unlikely(L != t->L && !lua_checkstack(t->L, 1)))
    		luaL_error(L, "stack overflow");
		lua_pushthread(t->L); 
		lua_xmove(t->L, L, 1); 
		lua_getfield(L, -3, "hook");  
		lua_rawset(L, -3); 
		lua_sethook(t->L, hookf, mask, count);
	}
	lua_pop(L, 1);
	return t;	
}

//-------- Search for the current running Task
Task *search_task(lua_State *L) {
	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) 
		if ((*it)->L == L)
			return (*it);		
	return NULL;
} 

//-------- Disable debug hoook on Tasks
void unhook_tasks(lua_State *L) {
	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) 
		lua_sethook((*it)->L, hookf, 0, 0);
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
	if (nargs == -1) {
		lua_xmove(t->L, L, 1);
		lua_error(L);
	}
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
		if (nresults)
			lua_xmove(t->L, from, nresults);
		close_task(t);
	} 
	return nresults;
}

//-------- Task scheduler
int update_tasks(lua_State *L) {

	if (lua_update)
		lua_update(L);

	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) {
		Task *t = *it;
		if (t->status == TSleep) {	
			if (t->sleep < GetTickCount64()) {	
				t->sleep = 0;
				t->status = TRunning;
			}
		}
	}

	auto it = Tasks.begin();
	while (it != Tasks.end()) {
		Task *tt = *it;
		if (tt->status == TTerminated) {
			luaL_unref(L, LUA_REGISTRYINDEX, tt->ref);
			luaL_unref(L, LUA_REGISTRYINDEX, tt->taskref);
			tt->ref = -1;
			it = Tasks.erase(it);
		} else ++it;
	}
			
	for (auto it = Tasks.begin(); it != Tasks.end(); ++it) {
		Task *t = *it;
		if (t->status == TRunning) {
			int status = lua_status(t->L);
			if (status == LUA_YIELD) {
				int nresults = resume_task(L, t, -1);
				if (nresults == -1) {
					lua_xmove(t->L, L, 1);
					return -1;
				}
				if (t->status == TTerminated) 
					return nresults;			
			}		
		}
	}
	return 0;
}

//-------- Wait for a Task at specific index
int waitfor_task(lua_State *L, int idx) {
	Task *t = lua_self(L, idx, Task);
	int nresults = lua_gettop(L);

	do {
		nresults = update_tasks(L);
		if (nresults == -1)
			lua_error(L);	
	} while(t->status != TTerminated);	
	return nresults;
}

//-------- Wait for all Tasks
int waitall_tasks(lua_State *L) {
    do
		if (update_tasks(L) == -1)
			lua_error(L);
	while (Tasks.size() > 1);		
    return 0;
}
