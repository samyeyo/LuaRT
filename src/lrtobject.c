/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lrtobject.c | LuaRT C API object implementation
*/

#include "luart.h"
#include "lrtapi.h"
#include <stdlib.h>
#include <windows.h>

typedef enum { Function, Method, PropertyGet, PropertySet } CallMethod;

static int type_isproperty(lua_State *L, CallMethod prop) {
	lua_pushfstring(L, "%s_%s", prop == PropertyGet ? "get" : "set", lua_tostring(L, 2));
	return lua_rawget(L, -2);
}

static void call_field(lua_State *L, int nargs, int nresults, const char *type, const char *field, CallMethod cm) {
	static const char *msg[] = { "in function %s", "in method", "error while getting property", "error while setting property"};
	if (lua_pcall(L, nargs, nresults, 0)) {
		char buff[256];
		const char *err = lua_tostring(L, -1);
		const char *_type = type;

		luaL_where(L, 1); 
		if (cm > Function) {
			const char *errmsg;
			if (!type) {
				luaL_getmetafield(L, 1, "__name");
				_type = lua_tostring(L, -1);
				lua_pop(L, 1);
			}
			if (!(errmsg = strstr(err, "'?'")))
				sprintf_s(buff, 256, "%s '%s.%s' : ", msg[cm], _type, field);
			else {
				sprintf_s(buff, 256, "%s '%s.%s'", msg[cm], _type, field);
				err = errmsg+3;
			}
			if (!strstr(err, buff)) {
				lua_pushstring(L, buff);
				lua_concat(L, 2);
			}
			lua_pushstring(L, err);
			lua_concat(L, 2);
			lua_error(L);
		} else luaL_error(L, msg[cm], field);
	}
}

static int get_mixins_field(lua_State *L, const char *field, CallMethod prop) {
	int type, i, len;
	//----- Expect that the top of the stack is a __type metafield of an object
	if (luaL_getmetafield(L, -1, "__mixins") && (len = luaL_len(L, -1)) ) {
		for (i = 1; i <= len; i++) {
			lua_rawgeti(L, -1, i);
			if (lua_getfield(L, -1, field))
				return 1;
			lua_pop(L, 1);
			if ((type = type_isproperty(L, prop))) 
				return type;
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
	}	
	return LUA_TNIL;
}

static int super_proxy(lua_State *L) {
	int nargs = lua_gettop(L);
	if (lua_getmetatable(L, 1)) {
		lua_pushstring(L, "__super");
		lua_pushvalue(L, lua_upvalueindex(1));
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}
	lua_pushvalue(L, lua_upvalueindex(2));
	lua_insert(L, -lua_gettop(L));
	lua_call(L, nargs, LUA_MULTRET);
	return lua_gettop(L);
}

LUA_METHOD(type, __index) {
	int n = lua_gettop(L);
	const char *field = lua_tostring(L, 2);
	int type;

	if (lua_type(L, 2) != LUA_TSTRING) 
		goto __index;
	lua_pushvalue(L, 1);
	while (!(type = type_isproperty(L, PropertyGet)) ){
		lua_pop(L, 1);
		lua_pushvalue(L, 2);
		if ((type = lua_rawget(L, -2))) {
			if (!lua_iscfunction(L, -1) && (type == LUA_TFUNCTION))
				lua_pushcclosure(L, super_proxy, 2);
			return 1;
		}
		lua_pop(L, 1);
		if ( (type = get_mixins_field(L, field, PropertyGet)) )
			goto __done;
		if (!luaL_getmetafield(L, -1, "__type")) {
__index:	return luaL_getmetafield(L, 1, "__metaindex") ? lua_tocfunction(L, -1)(L) : 0;
		}
	}
__done:
	if (type == LUA_TFUNCTION) {
		lua_pushvalue(L, 1);
		call_field(L, 1, LUA_MULTRET, NULL, lua_isstring(L, 2) ? field : NULL, PropertyGet);
		if ( (n = lua_gettop(L)-n ) == 0)
			luaL_error(L, "property %s must return a value", field);
	}
	return 1;
}

LUA_METHOD(type, __newindex) {
	int type = 0;
	const char *field = lua_tostring(L, 2);

	if (lua_type(L, 2) != LUA_TSTRING) 
		goto __newindex;
	lua_pushvalue(L, 1);
	while (!(type = type_isproperty(L, PropertySet))) {
		lua_pop(L, 1);
		lua_pushvalue(L, 2);
		if (lua_rawget(L, -2))
			goto setfield;
		lua_pop(L, 1);
		if ( (type = get_mixins_field(L, field, PropertySet)) )
			goto __done;
		if (!luaL_getmetafield(L, -1, "__type")) {
__newindex:	if ( luaL_getmetafield(L, 1, "__metanewindex" ))
				return lua_tocfunction(L, -1)(L);
setfield:	lua_pushvalue(L, 2);
			if (lua_isfunction(L, 3)) {
				luaL_getmetafield(L, 1, "__type");
				lua_pushvalue(L, 3);
				lua_pushcclosure(L, super_proxy, 2);
			}
			else
				lua_pushvalue(L, 3);
			lua_rawset(L, 1);
			return 0;
		}
	}
__done:
	if (type == LUA_TFUNCTION) {
		lua_pushvalue(L, 1);
		lua_pushvalue(L, 3);
		call_field(L, 2, 0, NULL, lua_isstring(L, 2) ? lua_tostring(L, 2) : NULL, PropertySet);
	} else goto setfield;
	return 0;
}

LUA_METHOD(type, __tostring) {
	luaL_getmetafield(L, 1, "__name");
	lua_pushfstring(L, "%s: %p", lua_tostring(L, -1), lua_topointer(L, 1));
	return 1;
}

LUA_METHOD(type, __gc) {
	if (lua_getfield(L, 1, "destructor") == LUA_TFUNCTION) {
		lua_pushvalue(L, 1);
		call_field(L, 1, 0, NULL, "destructor", Method);
	}
	if (luaL_getmetafield(L, 1, "__userdata")) {
		luaL_getmetafield(L, 1, "__type");
		luaL_getmetafield(L, -1, "__type");
		luaL_getmetafield(L, -1, "__mt");
		if (lua_getfield(L, -1, "__gc") != LUA_TNIL && lua_tocfunction(L, -1) != type___gc)
			lua_tocfunction(L, -1)(L);
		else {
			luaL_getmetafield(L, 1, "__userdata");
			free(lua_touserdata(L, -1));
		}
	}
	return 0;
}

static const luaL_Reg usertype_mt[] = {
	{"__gc",		type___gc},
	{"__tostring",	type___tostring},
	{"__index",		type___index},
	{"__newindex",	type___newindex},
	{NULL, NULL}
};

LUA_API void *lua_pushnewinstancebyname(lua_State *L, luart_type type, int narg) {
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUART_OBJECTS);
	if (lua_rawgeti(L, -1, type) != LUA_TSTRING)
		luaL_error(L, "unknown object");
	return lua_pushnewinstance(L, lua_tostring(L, -1), narg);
}

LUA_API void *lua_pushnewinstance(lua_State *L, const char *typename, int narg) {
	lua_getfield(L, LUA_REGISTRYINDEX, typename);
	for (int i = 0; i < narg; i++) 
		lua_pushvalue(L, -narg-1);
	call_field(L, narg, 1, typename, "constructor", Method); 
	return lua_tocinstance(L, -1, NULL);
}

static int module_getfield(lua_State *L, const char* prefix) {
	luaL_getmetafield(L, 1, "__properties");
	lua_pushstring(L, prefix);
	lua_pushvalue(L, 2);
	lua_concat(L, 2);
	return lua_rawget(L, -2);
}

LUA_METHOD(module, __index) {
	if (module_getfield(L, "get_"))
		return lua_tocfunction(L, -1)(L);
	return 1;
}

LUA_METHOD(module, __newindex) {
	const char *modname;
	luaL_getmetafield(L, 1, "__name");
	modname = lua_tostring(L, 1);
	lua_pop(L, 1);
	if (module_getfield(L, "set_")) {
		lua_pushvalue(L, 3);
		call_field(L, 1, 0, modname, lua_tostring(L, 2), PropertySet);
	}
	return 0;
}

LUA_API void lua_registermodule(lua_State *L, const char *name, const luaL_Reg *functions, const luaL_Reg *properties, lua_CFunction finalizer) {
	luaL_checkversion(L);
	lua_createtable(L, 0, 3);
	luaL_setrawfuncs(L, functions);
	lua_createtable(L, 0, 4);
	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);
	lua_pushstring(L, "__properties");
	luaL_checkversion(L);
	lua_createtable(L, 0, 3);
	luaL_setrawfuncs(L, properties);
	lua_rawset(L, -3);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, module___index); 
	lua_rawset(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, module___newindex); 
	lua_rawset(L, -3);
	if (finalizer) {
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, finalizer); 
		lua_rawset(L, -3);
	}
	lua_setmetatable(L, -2);
}

int type_tostring(lua_State *L) {
	lua_pushfstring(L, "Object: %p", lua_topointer(L, 1));
	return 1;
}

int usertype_constructor(lua_State *L) {
	BOOL cusertype = TRUE;
	int nargs = lua_gettop(L);
	const char *typename;

	lua_createtable(L, 0, 1);						//--- instance
	lua_createtable(L, 0, 1);						//--- instance metatable
	if (!luaL_getmetafield(L, 1, "__typename")) { 	//-- adjust __typename for Lua objects
		lua_Debug ar;
		lua_getstack(L, 0, &ar);
		lua_getinfo(L, "n", &ar);
		lua_pushstring(L, ar.name);
		cusertype = FALSE;
	}
	typename = lua_tostring(L, -1);
	lua_pop(L, 1);
	if (luaL_getmetafield(L, 1, "__mt")) {
		lua_pushnil(L);
        while(lua_next(L, -2) != 0) {
			lua_pushvalue(L, -2);
            lua_insert(L, -2);
			lua_settable(L, -5);
		}
		lua_pop(L, 1);
	}
	if (!cusertype) {
		lua_pushcfunction(L, type___gc);
		lua_setfield(L, -2, "__gc");
	}
	lua_pushstring(L, typename);
	lua_setfield(L, -2, "__name");
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "__type");

	if (!cusertype) {
		lua_pushcfunction(L, type___index); 
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, type___newindex); 
		lua_setfield(L, -2, "__newindex");
		lua_pushvalue(L, -2);
		lua_pushnil(L);
		lua_pushcclosure(L, obj_each_iter, 2);
		lua_setfield(L, -2, "__call");
	}
	lua_setmetatable(L, -2);
	lua_pushvalue(L, -1);
	lua_insert(L, -nargs-1);
	if (lua_getfield(L, 1, "constructor") == LUA_TFUNCTION)  {
		lua_insert(L, -nargs-1);
		lua_insert(L, -nargs);
		call_field(L, nargs, cusertype, typename, "constructor", Method);
		if (cusertype) {
			if (lua_isnil(L, -1)) 
				return 1;
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 1);
	return 1;
}

int lua_registerobject(lua_State *L, int *type, const char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt) {
	luart_type t;
	int count = lua_gettop(L);

	lua_newtable(L);					//--- new Usertype table
	if (methods) {
		luaL_setrawfuncs(L, methods);	//--- C Usertype
		lua_pushstring(L, "constructor");
		lua_pushcfunction(L, constructor);
		lua_rawset(L, -3);
	}
	lua_createtable(L, 0, 7);			//-- Usertype metatable
	lua_pushcfunction(L, usertype_constructor);
	lua_setfield(L, -2, "__call");
	if (methods) {
		const luaL_Reg *l;
		lua_createtable(L, 0, 2);
		if (mt)
			luaL_setrawfuncs(L, mt);
		for (l = usertype_mt; l->func; l++) {
			lua_pushstring(L, l->name);
			if (lua_rawget(L, -2) == LUA_TNIL) {
				lua_pushstring(L, l->name);
				lua_pushcfunction(L, l->func);
				lua_rawset(L, -4);	
			}
			lua_pop(L, 1);
		}
		lua_setfield(L, -2, "__mt");
	} else if (luaL_getmetafield(L, 1, "__mt")) //--- parent is a C Object
		lua_setfield(L, -2, "__mt");
	else luaL_setrawfuncs(L, usertype_mt);
	if (typename) {
		lua_pushstring(L, typename);
		lua_setfield(L, -2, "__typename");
	} else {
	 	lua_pushvalue(L, 1);
	 	lua_setfield(L, -2, "__type");
	}
	if (!methods && !type && (count > 1) ) {				//----  fill the mixins table for Lua objects
		int i;
		lua_createtable(L, 0, count-1);	//---- table for mixins
		for (i = 2; i <= count; i++) {
			luaL_checktype(L, i, LUA_TTABLE);
			if (luaL_getmetafield(L, i, "__call"))
				luaL_typeerror(L, i, "mixin table");
			lua_pushvalue(L, i);
			lua_rawseti(L, -2, i-1);
		}
		lua_setfield(L, -2, "__mixins");
	}
	lua_pushcfunction(L, type_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, type___index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, type___newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "Object");
	lua_setfield(L, -2, "__name");
	lua_setmetatable(L, -2);
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUART_OBJECTS);
	lua_pushstring(L, typename);
	t = luaL_len(L, -2)+1;
	lua_rawseti(L, -2, t);
	lua_pop(L, 1);
	if (type) {
		*type = t;
		lua_pushstring(L, typename);
		lua_pushvalue(L, -2);
		lua_rawset(L, -4);
		lua_setfield(L, LUA_REGISTRYINDEX, typename);
	}
	return t;
}

int lua_createinstance(lua_State *L, int idx) {
	if (!lua_istable(L, idx))
		luaL_argerror(L, 1, "object expected");
	lua_registerobject(L, NULL, NULL, NULL, NULL, NULL);
	return 1;
}

int lua_isinstance(lua_State *L, int idx, const char **objectname) {
	if (lua_istable(L, idx)) {
		lua_getmetatable(L, idx);
		if (lua_getfield(L, -1, "__type")) {
			lua_pop(L, 1);
			if (lua_getfield(L, -1, "__name")) {
				*objectname = lua_tostring(L, -1);
				lua_pop(L, 2);
				return 1;
			}
		}
		lua_pop(L, 1);
	}
	return 0;
}

void lua_checkinstance(lua_State *L, int idx, const char *objectname) {
	char *name;
	if (!(lua_isinstance(L, idx, (const char **)&name) && strcmp(objectname, name) == 0)) 
		luaL_typeerror(L, idx, objectname);
}

void lua_createcinstance(lua_State *L, void *t, luart_type type) {
	lua_getmetatable(L, 1);
	lua_pushstring(L, "__userdata");
	lua_pushlightuserdata(L, t);
	lua_rawset(L, -3);
	lua_pop(L, 1);
	*((luart_type *)t) = type;
	lua_pushvalue(L, 1);
}

void *lua_checkcinstancebyname(lua_State *L, int idx, const char *name) {
	void *instance;
	lua_checkinstance(L, idx, name);
	luaL_getmetafield(L, idx, "__userdata");
	instance = lua_touserdata(L, -1);
	lua_pop(L, 2);
	return lua_tocinstance(L, idx, instance);
}

void *lua_checkcinstance(lua_State *L, int idx, luart_type t) {
	void *p;
	if ( (p = lua_iscinstance(L, idx, t)) )
		return p;
	else
		luaL_typeerror(L, idx, "object instance");
	return NULL;
}

void *lua_toself(lua_State *L, int idx, luart_type t) {
	void *f = NULL;
	if ( (f = lua_tocinstance(L, idx, &t)) )
		return f;
	return lua_checkcinstance(L, idx, t);
}

void *lua_tocinstance(lua_State *L, int idx, luart_type *t) {
    void *obj = NULL;
    if ( lua_istable(L, idx) && (luaL_getmetafield(L, idx, "__userdata")) ) {
        obj = lua_touserdata(L, -1);
        if (t)
            *t = *(int*)obj;
    	lua_pop(L, 1);
    }
    return obj;
}

void *lua_iscinstance(lua_State *L, int idx, luart_type t) {
	void *obj;
	if ( (obj = lua_tocinstance(L, idx, NULL)) ) {
		if ( (*(luart_type*)obj) != t)
			obj = NULL;
	}
	return obj;
}

static void property(lua_State *L, const char *prop) {
	char buffer[255];
	const char *formats[] = { "get_%s", "set_%s" };

	for (int i = 0; i < 2; i++) {
		snprintf(buffer, 255, formats[i], prop);
		lua_pushstring(L, buffer);
		lua_pushvalue(L, -1);
		lua_rawget(L, -3);
		lua_rawset(L, -4);	
	}
}

void lua_registerwidget(lua_State *L, int *type, char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_tooltip) {
	lua_registerobject(L, type, typename, constructor, methods, mt);
	lua_getfield(L, LUA_REGISTRYINDEX, typename);
	if (!lua_getfield(L, LUA_REGISTRYINDEX, "Radiobutton"))
		luaL_error(L, "ui module not found");
	if (has_text)
		property(L, "text");	
	if (has_cursor)
		property(L, "cursor");	
	if (has_font) {
		property(L, "font");
		property(L, "fontsize");
		property(L, "fontstyle");
	}
	if (has_icon)
		property(L, "loadicon");
	if (has_tooltip)
		property(L, "tooltip");
	lua_pop(L, 1);
}

lua_Integer lua_registerevent(lua_State *L, const char *methodname, lua_CFunction func) {
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "LuaRT Events");
	WM_LUAMAX += luaL_len(L, -1)+1;
	if (methodname)
		lua_pushstring(L, methodname);
	else lua_pushcfunction(L, func);
	lua_rawseti(L, -2, WM_LUAMAX);
	lua_pop(L, 1);
	return WM_LUAMAX;
}

void *lua_getevent(lua_State *L, lua_Integer eventid, int *type) {
	char *methodname = NULL;
	if (eventid <= WM_LUAMAX) {
		luaL_getsubtable(L, LUA_REGISTRYINDEX, "LuaRT Events");
		if ( (*type = lua_rawgeti(L, -1, eventid)) == LUA_TSTRING ) {		
			methodname = (char *)lua_tostring(L, -1);
			lua_pop(L, 2);
		}
		else { 
			lua_insert(L, -2);
			lua_pop(L, 1);	
		}
	}
	return methodname;
}

lua_Integer			WM_LUAMAX = WM_USER+1;
WIDGET_INIT 		lua_widgetinitialize = NULL;
WIDGET_CONSTRUCTOR	lua_widgetconstructor = NULL;
WIDGET_DESTRUCTOR	lua_widgetdestructor = NULL;
WIDGET_PROC			lua_widgetproc = NULL;
luaL_Reg 			*WIDGET_METHODS = NULL;
luart_type			TWidget = 0;
