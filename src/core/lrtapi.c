/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lrtapi.c | LuaRT API implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Task.h>
#include "lrtapi.h"
#include "sys\async.h"
#include <windows.h>

//-------------------------------------------------[UTF8 strings conversion functions]
char *wchar_toutf8(const wchar_t *str, int *len) {
	char *buff;
	int size = WideCharToMultiByte(CP_UTF8, 0, str, *len, NULL, 0, NULL, NULL);
	buff = (char *)malloc(size);
	WideCharToMultiByte(CP_UTF8, 0, str, *len, buff, size, NULL, NULL);
	if (*len == -1)
		size--;
	*len = size;
	return buff;
}

wchar_t *utf8_towchar(const char *str, int *len) {
	int size = MultiByteToWideChar(CP_UTF8, 0, str, *len, NULL, 0);
	wchar_t *buff = (wchar_t*)malloc(size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, *len, buff, size);
	*len = size;
	return buff;
}

//-------------------------------------------------[UTF8 strings LuaRT C API]
LUA_API wchar_t *lua_tolwstring(lua_State *L, int idx, int *size) {
	int len = -1;
	wchar_t *result = utf8_towchar(luaL_checkstring(L, idx), &len);
	if (size)
		*size = len-1;
	return result;
}

LUA_API void lua_pushlwstring(lua_State *L, const wchar_t *str, int len) {
	char *s;
	if (len) {
		s = wchar_toutf8(str, &len);
		lua_pushlstring(L, s, len); 
		free(s);
	}
	else lua_pushlstring(L, "", 0);
}

LUA_API int lua_optstring(lua_State *L, int idx, const char *options[], int def) {
	if (lua_isstring(L, idx)) {
		int i = -1;
		const char *name = lua_tostring(L, idx);
		while(options[++i])
			if (strcmp(options[i], name) == 0)
				return i;
	}
	return def;
} 

static int WaitTask(lua_State *L) {
	return lua_yieldk(L, 0, (lua_KContext)lua_touserdata(L, lua_upvalueindex(1)), (lua_KFunction)lua_touserdata(L, lua_upvalueindex(2)));
}

LUA_API int lua_pushtask(lua_State *L, lua_KFunction taskfunc, void *userdata, lua_CFunction gc) {
	lua_pushlightuserdata(L, userdata);
	lua_pushlightuserdata(L, taskfunc);
	lua_pushcclosure(L, WaitTask, 2);
	Task *t = lua_pushinstance(L, Task, 1);
	t->userdata = userdata;
	if (gc)
		t->gc_func = gc;
	lua_pushvalue(L, -1);
	lua_call(L, 0, 0);
	return 1;	
}

//-------------------------------------------------[LuaL_setfuncs alternative with lua_rawset]
LUALIB_API void luaL_setrawfuncs(lua_State *L, const luaL_Reg *l) {
  for (; l->name != NULL; l++) { 
	lua_pushstring(L, l->name);
    lua_pushcfunction(L, l->func); 
    lua_rawset(L, -3);
  }
}

//-------------------------------------------------[LuaL_require alternative with luaL_requiref]
static int module_error(lua_State *L) {
	return luaL_error(L, "required module '%s' not found", lua_tostring(L, 1));
}

LUALIB_API void luaL_require(lua_State *L, const char *modname) {
	luaL_requiref(L, modname, module_error, 0);
}

LUA_API void lua_setupdate(lua_CFunction func) {
	set_lua_update(func);
}

//-------------------------------------------------[lua_schedule() function]
LUA_API int lua_schedule(lua_State *L) {
	return update_tasks(L);
}

//-------------------------------------------------[lua_throwevent() function]
LUA_API int lua_throwevent(lua_State *L, const char *name, int nparams) {
  if (lua_getfield(L, -nparams, name) == LUA_TFUNCTION) {
    int nres;
    Task *t = (Task *)lua_pushinstance(L, Task, 1);

    lua_pop(L, 2);
    t->status = TRunning;
    lua_xmove(L, t->L, nparams);
    int status = lua_resume(t->L, L, nparams, &nres);
    if (status > LUA_YIELD)
      lua_error(t->L);
    t->status = TTerminated;
	lua_xmove(t->L, L, nres);
	return nres;
  } else lua_pop(L, 1);
  return 0;
} 

//-------------------------------------------------[lua_wait() function]
LUA_API int lua_wait(lua_State *L, int idx) {
	return waitfor_task(L, idx);
}

//-------------------------------------------------[lua_sleep() function]
int do_sleep(lua_State *L, lua_Integer delay) {
	Task *t = search_task(L);

	t->sleep =  GetTickCount64() + delay;
	t->status = TSleep;
	if (lua_isyieldable(L))
		return lua_yield(L, 0);
	else while (t->status == TSleep)
		update_tasks(L);
	return 0;
}

LUA_API void lua_sleep(lua_State *L, lua_Integer delay) {
	do_sleep(L, delay);
}

LUA_API Task *lua_gettask(lua_State *L) {
	return search_task(L);
}

//-------------------------------------------------[LuaRT Extended base library]

//----------------------------------[ await() function ]
LUA_METHOD(luaB, await) {
	Task *t;
	int idx = lua_gettop(L)-1, nresults;
	
	if ( !(t = (Task *)lua_iscinstance(L, 1, TTask )) ) {		
		luaL_checktype(L, 1, LUA_TFUNCTION);
		lua_pushvalue(L, 1);
		t = (Task *)lua_pushinstance(L, Task, 1);
	}
	t->waiting = search_task(L);
	if (t->status == TCreated) 
		start_task(L, t, idx);
	t->waiting->status = TWaiting;
	while(t->status != TTerminated)
		nresults = update_tasks(L);
	return nresults;
}

//----------------------------------[ sleep() ]
LUA_METHOD(luaB, sleep) {	
	do_sleep(L, luaL_optinteger(L, 1, 1));
	return 0;
}

//----------------------------------[ waitall() ]
LUA_METHOD(luaB, waitall) {	
	return waitall_tasks(L);
}

//----------------------------------[ async() function ]
LUA_METHOD(luaB, async) {
	int nargs = lua_gettop(L)-1;

	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushvalue(L, 1);
	Task *t = (Task *)lua_pushinstance(L, Task, 1);
	int nresults = start_task(L, t, nargs);			
	if (t->status == TTerminated)
		return nresults;
	return 1;
}

//-------------------------------------------------[is() function]
int luaB_is(lua_State *L) {
	BOOL result = FALSE;
	if (luaL_getmetafield(L, 1, "__name") && luaL_getmetafield(L, 2, "__name")) {
		luaL_getmetafield(L, 1, "__type");
		if (!luaL_getmetafield(L, -1, "__mt")) {
			lua_pushvalue(L, 1);
			while (luaL_getmetafield(L, -1, "__type")) {
				if ((result = lua_compare(L, 2, -1, LUA_OPEQ)))
					break;
			}
		}
		else if (luaL_getmetafield(L, 2, "__mt"))
			result = lua_compare(L, -1, -2, LUA_OPEQ);
	} else if (luaL_getmetafield(L, 1, "__index"))
		result = lua_compare(L, 2, -1, LUA_OPEQ);
	else
		luaL_typeerror(L, 2, "Object or table");
	lua_pushboolean(L, result);
	return 1;
}

//-------------------------------------------------[each() iterator]
static int each_iter(lua_State *L) {
	lua_pushvalue(L, lua_upvalueindex(2));
	if (lua_next(L, lua_upvalueindex(1)) != 0) {
		lua_pushvalue(L, -2);
		lua_replace(L, lua_upvalueindex(2));
		return 1;
	}
	return 0;
}

int obj_each_iter(lua_State *L) {
	int results = lua_gettop(L);
	if (!lua_getfield(L, lua_upvalueindex(1), "iterator"))
		luaL_error(L, "could not iterate over %s instance (iterator function not found)", luaL_typename(L, lua_upvalueindex(1)));
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_pushvalue(L, lua_upvalueindex(2));
	lua_call(L, 2, LUA_MULTRET);
	results = lua_gettop(L)-results;
	if (lua_isnil(L, -results))
		return 0;
	lua_replace(L, lua_upvalueindex(2));
	return results-1;
}

static int luaB_each (lua_State *L) {
	int type = lua_type(L, 1);
	if (type == LUA_TFUNCTION)
		lua_pushvalue(L, 1);
	else {
			 if (luaL_getmetafield(L, 1, "__iterate") == LUA_TFUNCTION ) {
				lua_pushvalue(L, 1);
				lua_call(L, 1, 1);
			} else if (!luaL_getmetafield(L, 1, "__name")) {
					luaL_checktype(L, 1, LUA_TTABLE);
					lua_pushvalue(L, 1);
					lua_pushnil(L);
					lua_pushcclosure(L, each_iter, 2);
			} else if (lua_getfield(L, 1, "iterator") == LUA_TFUNCTION) {
					lua_pushvalue(L, 1);
					lua_pushnil(L);
					lua_pushcclosure(L, obj_each_iter, 2);
			} else luaL_error(L, "cannot iterate over %s", lua_tostring(L, -1));
	}
	return 1;
}

//-------------------------------------------------[type() function]

LUA_API const char *lua_objectname(lua_State *L, int idx) {
	if (luaL_getmetafield(L, idx, "__name")) {
		const char *typename = lua_tostring(L, -1);
		lua_pop(L, 1);
		return typename;
	} 
	return lua_typename(L, lua_type(L,idx));
}

#ifndef RTCOMPAT
static int luaB_type (lua_State *L) {
  int t = lua_type(L, 1);
  luaL_argcheck(L, t != LUA_TNONE, 1, "value expected");
  lua_pushstring(L, lua_objectname(L, 1));
  return 1;
}
#endif

//-------------------------------------------------[super() function]

static int luaB_super(lua_State *L) {
	if (!lua_super(L, 1))
		lua_pushnil(L);
	return 1;
}

#ifndef RTCOMPAT
static int luaB_rawget (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  if (lua_tocinstance(L, 1, NULL))
	luaL_typeerror(L, 1, "object");
  luaL_checkany(L, 2);
  lua_settop(L, 2);
  lua_rawget(L, 1);
  return 1;
}

static int luaB_rawset (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  if (lua_tocinstance(L, 1, NULL))
	luaL_typeerror(L, 1, "table");
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);
  lua_settop(L, 3);
  lua_rawset(L, 1);
  return 1;
}
#endif

static int luaB_Object(lua_State *L) {
	lua_createinstance(L, 1);
	return 1;
}

//----------------------------[debug.settaskhook() function]

static int settaskhook(lua_State *L) {
	if (lua_isnoneornil(L, 1)) {
    	lua_pushnil(L);
		lua_setfield(L, LUA_REGISTRYINDEX, "_TASKHOOK");
		unhook_tasks(L);
		return 0;
    }
    luaL_checktype(L, 1, LUA_TFUNCTION);
	const char *smask = luaL_checkstring(L, 2);
    int count = (int)luaL_optinteger(L, 3, 0);
	int mask = 0;
	if (strchr(smask, 'c')) mask |= LUA_MASKCALL;
	if (strchr(smask, 'r')) mask |= LUA_MASKRET;
	if (strchr(smask, 'l')) mask |= LUA_MASKLINE;
	if (count > 0) mask |= LUA_MASKCOUNT;
	lua_createtable(L, 0, 3);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "hook");
	lua_pushinteger(L, count);
	lua_setfield(L, -2, "count");
	lua_pushinteger(L, mask);
	lua_setfield(L, -2, "mask");
	lua_setfield(L, LUA_REGISTRYINDEX, "_TASKHOOK");
	return 0;
}

//-------------------------------------------------[LuaRT base library extension]
static const luaL_Reg baselib_ext[] = {
	{"is",		luaB_is},
	{"each",	luaB_each},
	{"Object",	luaB_Object},
	{"await", 	luaB_await},
	{"async", 	luaB_async},
	{"waitall",	luaB_waitall},
	{"sleep", 	luaB_sleep},
  	{"super",	luaB_super},
#ifndef RTCOMPAT
  	{"rawget", luaB_rawget},
  	{"rawset", luaB_rawset},
	{"type",	luaB_type},
#endif
	{NULL, NULL}
};

static void register_module(lua_State *L, const char *modname, lua_CFunction openf) {
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
	lua_pushcfunction(L, openf);
	lua_setfield(L, -2, modname);
	lua_pop(L, 1);
}

//-------------------------------------------------[luaL_openlibs() luaRT C API]

static const luaL_Reg def_libs[] = {
  {LUA_GNAME,		luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME,	luaopen_coroutine},
  {LUA_TABLIBNAME,	luaopen_table},
  {LUA_STRLIBNAME, 	luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, 	luaopen_debug},
  {"io",	luaopen_io },
  {"os",	luaopen_os },
  {"utf8",	luaopen_utf8 },
  {"sys",		luaopen_sys},
  {"compression",	luaopen_compression },
#ifdef AIO
  {"ui",		luaopen_ui },
#endif  
  {NULL, NULL}
};

LUALIB_API void luaL_openlibs(lua_State *L) {
	const luaL_Reg *lib;

	for (lib = def_libs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
	register_module(L, "console", luaopen_console);
	lua_pushglobaltable(L);
	luaL_setfuncs(L, baselib_ext, 0);
	/* set global _ARCH */
	lua_pushliteral(L, LUA_ARCH);
	lua_setfield(L, -2, "_ARCH");
	/* set LuaRT _VERSION */
	lua_pushliteral(L, LUA_VERSION);
	lua_setfield(L, -2, "_VERSION");
	lua_pop(L, 1);
	lua_getglobal(L, "debug");
	lua_pushcfunction(L, settaskhook);
	lua_setfield(L, -2, "settaskhook");
	lua_pop(L, 1);
}
