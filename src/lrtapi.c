/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lrtapi.c | LuaRT API implementation
*/

#include "lua.h"
#include "luart.h"
#include "lrtapi.h"
#include "Zip.h"
#include "zip\lib\zip.h"
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

int lua_optstring(lua_State *L, int idx, const char *options[], int def) {
	if (lua_isstring(L, idx)) {
		int i = -1;
		const char *name = lua_tostring(L, idx);
		while(options[++i])
			if (strcmp(options[i], name) == 0)
				return i;
	}
	return def;
} 

//-------------------------------------------------[LuaL_setfuncs alternative with lua_rawset]
LUALIB_API void luaL_setrawfuncs(lua_State *L, const luaL_Reg *l) {
  for (; l->name != NULL; l++) { 
	lua_pushstring(L, l->name);
    lua_pushcfunction(L, l->func); 
    lua_rawset(L, -3);
  }
}

//-------------------------------------------------[LuaRT Extended base library]

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
			if (lua_getfield(L, 1, "iterator") == LUA_TFUNCTION) {
					lua_pushvalue(L, 1);
					lua_pushnil(L);
					lua_pushcclosure(L, obj_each_iter, 2);
			} else if (luaL_getmetafield(L, 1, "__iterate") == LUA_TFUNCTION ) {
				lua_pushvalue(L, 1);
				lua_call(L, 1, 1);
			} else if (!luaL_getmetafield(L, 1, "__name")) {
					luaL_checktype(L, 1, LUA_TTABLE);
					lua_pushvalue(L, 1);
					lua_pushnil(L);
					lua_pushcclosure(L, each_iter, 2);
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
	BOOL is_super = TRUE;

	if ( !(lua_istable(L, 1) && luaL_getmetafield(L, 1, "__name")) )
		luaL_typeerror(L, 1, "Object or instance");
	if (!luaL_getmetafield(L, 1, "__super")) {
		luaL_getmetafield(L, 1, "__type");
		if ((is_super = luaL_getmetafield(L, 1, "__call")))
			lua_pop(L, 1);
		if (!luaL_getmetafield(L, -1, "__name"))
			lua_pushnil(L);
		else lua_pop(L, 1);
	} else return 1;
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

//-------------------------------------------------[LuaRT base library extension]
static const luaL_Reg baselib_ext[] = {
	{"is",		luaB_is},
	{"each",	luaB_each},
	{"Object",	luaB_Object},
#ifndef RTCOMPAT
  	{"rawget", luaB_rawget},
  	{"rawset", luaB_rawset},
#endif
  	{"super",	luaB_super},
#ifndef RTCOMPAT	
	{"type",	luaB_type},
#endif
	{NULL, NULL}
};

//-------------------------------------------------[luaRT embeded package loader]

struct zip_t *fs = NULL;
static BYTE *datafs  = NULL;

static void *fsload(lua_State *L, const char *fname) {
	 void *buff = NULL; 
	 int idx = 0;
	 size_t size;

	 if (zip_entry_open(fs, fname) == 0) {
        zip_entry_read(fs, &buff, &size);
		if (memcmp(buff, "\xEF\xBB\xBF", 3)==0)
			idx = 3;
		if (luaL_loadbuffer(L, buff+idx, size-idx, fname))
			lua_error(L);
		zip_entry_close(fs);
		return buff;
    }
	return NULL;
}

static int luart_fsloader(lua_State *L) {
	const char *modname = luaL_gsub(L, luaL_checkstring(L, 1), ".", "/");
	size_t len = strlen(modname)+6;
	void *buff;
	char *fname;

	fname = calloc(1, len);
	_snprintf(fname, len, "%s.lua", modname);
	if ( !(buff = fsload(L, fname)) ) {
		_snprintf(fname, len, "%s.wlua", modname);
		buff = fsload(L, fname);
	}
	if (!buff)
		lua_pushfstring(L, "no embedded module '%s' found", modname);
 	free(fname);
	free(buff);
	return 1;
 }

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
#ifdef RTCOMPAT
  {"io",	luaopen_io },
  {"os",	luaopen_os },
  {"ui",		luaopen_ui },
  {"crypto",	luaopen_crypto },
  {"net",		luaopen_net },
#endif
  {"sys",		luaopen_sys},
  {NULL, NULL}
};

LUALIB_API void luaL_openlibs(lua_State *L) {
	const luaL_Reg *lib;
	for (lib = def_libs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
	register_module(L, "console", luaopen_console);
	luaL_requiref(L, "zip", luaopen_zip, 0);
	lua_pop(L, 1);
	lua_pushglobaltable(L);
	luaL_setfuncs(L, baselib_ext, 0);
	/* set global _ARCH */
	lua_pushliteral(L, LUA_ARCH);
	lua_setfield(L, -2, "_ARCH");
	/* set LuaRT _VERSION */
	lua_pushliteral(L, LUA_VERSION);
	lua_setfield(L, -2, "_VERSION");
	lua_pop(L, 1);
}

//-------------------------------------------------[luaL_embedclose() luaRT C API]
LUALIB_API int luaL_embedclose(lua_State *L) {
	free(datafs);
	zip_close(fs);
	return 0;
}

//-------------------------------------------------[luaL_embedopen() luaRT C API]
LUALIB_API int luaL_embedopen(lua_State *L, const wchar_t *exename) {
  DWORD read;
  HANDLE hFile;
  BYTE buff[4096];
  IMAGE_DOS_HEADER* dosheader;
#ifdef _WIN64
#define HEADERS	IMAGE_NT_HEADERS64
#else
#define HEADERS	IMAGE_NT_HEADERS32
#endif
  HEADERS* header;
  DWORD maxpointer = 0, exesize = 0;
  int i;  
  DWORD filesize;
  size_t fssize;
  IMAGE_SECTION_HEADER* sectiontable;

  luaL_embedclose(L);
  hFile = CreateFileW(exename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(INVALID_HANDLE_VALUE == hFile)
      return FALSE;
  ReadFile(hFile, buff, sizeof(buff), &read, NULL);
  dosheader = (IMAGE_DOS_HEADER*)buff;

  // Locate PE header
  header = (HEADERS*)((DWORD_PTR)buff + dosheader->e_lfanew);
  if(dosheader->e_magic != IMAGE_DOS_SIGNATURE || header->Signature != IMAGE_NT_SIGNATURE) {
    CloseHandle(hFile);
    return FALSE;
  }
  sectiontable = (IMAGE_SECTION_HEADER*)((BYTE*)header + sizeof(HEADERS));
  for(i = 0; i < header->FileHeader.NumberOfSections; i++) {
    if(sectiontable->PointerToRawData > maxpointer) {
      maxpointer = sectiontable->PointerToRawData;
      exesize = sectiontable->PointerToRawData +
        sectiontable->SizeOfRawData;
    }
    sectiontable++;
  }
  filesize = GetFileSize(hFile, NULL);
  SetFilePointer(hFile, exesize, NULL, FILE_BEGIN);
  if ((fssize = filesize - exesize)) {
	  datafs = malloc(fssize);
	  ReadFile(hFile, datafs, fssize, &read, NULL);
	  CloseHandle(hFile);
	  if ((fs = open_fs(datafs, fssize))) {
	  	lua_getglobal(L, "package");
		lua_getfield(L, -1, "searchers");
		lua_pushcfunction(L, luart_fsloader);
		lua_rawseti(L, -2, luaL_len(L, -2)+1);
		lua_pop(L, 2);
		return TRUE;
	  }
  }
  return FALSE;
}