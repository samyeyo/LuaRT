/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lembed.c | LuaRT embed module implementation
*/

#include "lrtapi.h"
#include <luart.h>

#include <File.h>
#include <shlwapi.h>

#define MINIZ_HEADER_FILE_ONLY
#include <compression\lib\zip.h>
#include "resources\resource.h"

#define CMEMLIBS "DLL binary modules"

typedef void (*voidf)(void);
extern int Zip_extract(lua_State *L); 
extern zip_t *fs;
BYTE *datafs  = NULL;
wchar_t path[MAX_PATH];

struct zip_t *open_fs(void *ptr, size_t size) {
	struct zip_t *zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
	zip->level = MZ_DEFAULT_LEVEL;
	if (mz_zip_reader_init_mem(&zip->archive, ptr, size,  MZ_DEFAULT_LEVEL | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == FALSE) {
		free(zip);
		zip = NULL;
	}
	return zip;
}

//-------------------------------------------------[luaRT embeded package loader]
static void *fsload(lua_State *L, const char *fname) {
	 void *buff = NULL; 
	 int idx = 0;
	 size_t size;

	 if (zip_entry_open(fs, fname) == 0) {
        zip_entry_read(fs, &buff, &size);
		if (memcmp(buff, "\xEF\xBB\xBF", 3)==0)
			idx = 3;
		if (luaL_loadbuffer(L, (const char*)(buff+idx), size-idx, fname))
			lua_error(L);
		zip_entry_close(fs);
		return buff;
    }
	return NULL;
}

static int luart_fsloader(lua_State *L) {
	const char *modname = luaL_gsub(L, luaL_checkstring(L, 1), ".", "/");
	size_t len = strlen(modname)+MAX_PATH;
	void *buff;
	char *fname;

	fname = (char *)calloc(1, len);
	_snprintf(fname, len, "%s.lua", modname);
	if ( !(buff = fsload(L, fname)) ) {
    lua_pushfstring(L, "no embedded module '%s' found", fname);
		_snprintf(fname, len, "%s.wlua", modname);
    if (!(buff = fsload(L, fname))) {
      lua_pushfstring(L, "no embedded module '%s' found", fname);
      _snprintf(fname, len, "%s.dll", modname);
      if (zip_entry_open(fs, fname) == 0) {
            lua_getfield(L, LUA_REGISTRYINDEX, CMEMLIBS);
            lua_pushwstring(L, temp_path);
            lua_pushstring(L, fname);
            lua_concat(L, 2);
            wchar_t *tmp = lua_towstring(L, -1);
            if (zip_entry_fread(fs, tmp) == 0) {
              HMODULE hm;
              if ( (hm = LoadLibraryW(tmp)) ) {
                _snprintf(fname, len, "luaopen_%s", PathFindFileNameA(modname));
                lua_CFunction f = (lua_CFunction)(voidf)GetProcAddress(hm, fname);
                if (f) {
                  lua_pushlightuserdata(L, (void*)hm);
                  lua_rawset(L, -3);
                  lua_pushcfunction(L, f);
                  free(tmp);  
                  zip_entry_close(fs);
                  goto done;          
                } else FreeLibrary(hm);
              }
            }
            free(tmp);
    		zip_entry_close(fs);
      }
      lua_pushfstring(L, "no embedded module '%s.dll' found", modname);
    }
	}
done:
 	free(fname);
	free(buff);
	return 1;
 }

//-------------------------------------------------[luaL_embedclose() luaRT C API]
LUA_API int luaL_embedclose(lua_State *L) {
  if (lua_getfield(L, LUA_REGISTRYINDEX, CMEMLIBS)) {
    wchar_t *path;
    lua_pushnil(L);
    while (lua_next(L, -2)) {
      path = lua_towstring(L, -2);
      FreeLibrary(lua_touserdata(L, -1));
      DeleteFileW(path);
      free(path);
      lua_pop(L, 1);
    }
  }
	return 0;
}

//-------------------------------------------------[luaL_embedopen() luaRT C API]
LUA_API int luaL_embedopen(lua_State *L, const wchar_t *exename) {
  HRSRC hres = FindResource(NULL, MAKEINTRESOURCE(EMBED), RT_RCDATA);
  HGLOBAL hdata = LoadResource(NULL, hres);
  datafs = LockResource(hdata);
  size_t size = SizeofResource(NULL, hres);
  if (datafs && (size > 2) && (fs = open_fs(datafs, size))) {
	  lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");
    lua_pushcfunction(L, luart_fsloader);
    lua_rawseti(L, -2, luaL_len(L, -2)+1);
    lua_pop(L, 2);
    return TRUE;
  }
  return FALSE;
}

LUA_METHOD(embed, File) {
    lua_getglobal(L, "embed");
    lua_pushcfunction(L, Zip_extract);
    lua_getfield(L, -2, "zip");
    lua_pushvalue(L, 1);
    lua_pushwstring(L, temp_path);
    lua_pcall(L, 3, LUA_MULTRET, 0);
    return 1;
}

static const luaL_Reg embedlib[] = {
	{"File",	embed_File},
	{NULL, NULL}
};

static const luaL_Reg embed_properties[] = {
	{NULL, NULL}
};

//-------------------------------------------------[luaopen_embed() "embed" module]
LUAMOD_API int luaopen_embed(lua_State *L) {
  luaL_getsubtable(L, LUA_REGISTRYINDEX, CMEMLIBS);
	lua_registermodule(L, "embed", embedlib, embed_properties, luaL_embedclose);
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "compression");
  lua_pushstring(L, "zip");
  lua_getfield(L, -2, "Zip");
  lua_pushlightuserdata(L, fs);
  lua_pcall(L, 1, 1, 0);
  lua_rawset(L, -6);
  lua_pop(L, 3);
	return 1;
};
