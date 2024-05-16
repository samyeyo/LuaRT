/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lembed.c | LuaRT embed module implementation
*/

#define LUA_LIB

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
  char * buffer = (char*)ptr;
  DWORD old, new;

  if (VirtualProtect(ptr, 1, PAGE_READWRITE, &old)) {
    *buffer = 0x50;   
    VirtualProtect(ptr, 1, old, &new);
  }
  zip->level = MZ_DEFAULT_LEVEL;
	if (mz_zip_reader_init_mem(&zip->archive, ptr, size,  MZ_DEFAULT_LEVEL | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == FALSE) {
		free(zip);
		zip = NULL;
	}
	return zip;
}

static const char *getnextfilename (char **path, char *end) {
  char *sep;
  char *name = *path;
  if (name == end)
    return NULL;  /* no more names */
  else if (*name == '\0') {  /* from previous iteration? */
    *name = *LUA_PATH_SEP;  /* restore separator */
    name++;  /* skip it */
  }
  sep = strchr(name, *LUA_PATH_SEP);  /* find next separator */
  if (sep == NULL)  /* separator not found? */
    sep = end;  /* name goes until the end */
  *sep = '\0';  /* finish file name */
  *path = sep;  /* will start next search from here */
  return name;
}

static void pusherrornotfound (lua_State *L, const char *path) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  luaL_addstring(&b, "no file '");
  luaL_addgsub(&b, path, LUA_PATH_SEP, "'\n\tno file '");
  luaL_addstring(&b, "'");
  luaL_pushresult(&b);
}

static const char *searchpath (lua_State *L, const char *name,
                                             const char *path,
                                             const char *sep,
                                             const char *dirsep) {
  luaL_Buffer buff;
  char *pathname;  /* path with name inserted */
  char *endpathname;  /* its end */
  const char *filename;
  /* separator is non-empty and appears in 'name'? */
  if (*sep != '\0' && strchr(name, *sep) != NULL)
    name = luaL_gsub(L, name, sep, dirsep);  /* replace it by 'dirsep' */
  luaL_buffinit(L, &buff);
  /* add path to the buffer, replacing marks ('?') with the file name */
  luaL_addgsub(&buff, path, LUA_PATH_MARK, name);
  luaL_addchar(&buff, '\0');
  pathname = luaL_buffaddr(&buff);  /* writable list of file names */
  endpathname = pathname + luaL_bufflen(&buff) - 1;
  while ((filename = getnextfilename(&pathname, endpathname)) != NULL) {
    if (filename[0] == '.' && filename[1] == '\\')
      filename += 2; 
    if ((zip_entry_open(fs, filename) == 0))  /* does file exist and is readable? */
      return lua_pushstring(L, filename);  /* save and return name */
  }
  luaL_pushresult(&buff);  /* push path to create error message */
  pusherrornotfound(L, lua_tostring(L, -1));
  return NULL;  /* not found */
}

static const char *findfile (lua_State *L, const char *name,
                                           const char *pname,
                                           const char *dirsep) {
  const char *path;
  lua_getfield(L, lua_upvalueindex(1), pname);
  path = lua_tostring(L, -1);
  if (l_unlikely(path == NULL))
    luaL_error(L, "'package.%s' must be a string", pname);
  return searchpath(L, name, path, ".", dirsep);
}

static int searcher_embedded_Lua (lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  const char *filename = findfile(L, name, "path", "/");
  if (filename) {
    char *buff = NULL; 
	  int idx = 0;
	  size_t size;

    zip_entry_read(fs, (void**)&buff, &size);
    if (memcmp(buff, "\xEF\xBB\xBF", 3)==0)
      idx = 3;
    if (luaL_loadbuffer(L, (const char*)(buff+idx), size-idx, filename))
      luaL_error(L, "error loading module '%s' from embedded file '%s':\n\t%s", lua_tostring(L, 1), filename, lua_tostring(L, -1));
    zip_entry_close(fs);
    lua_pushstring(L, filename); 
    return 2;   
  }
  return 1;
}

extern BOOL make_path(wchar_t *folder);

static int searcher_embedded_C (lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  const char *filename = findfile(L, name, "cpath", "/");
  if (filename) {
found:    
    lua_getfield(L, LUA_REGISTRYINDEX, CMEMLIBS);
    lua_pushwstring(L, temp_path);
    lua_pushstring(L, filename);
    lua_concat(L, 2);
    if (zip_entry_open(fs, filename) == 0) {
      wchar_t *tmp = lua_towstring(L, -1);
      if ((GetFileAttributesW(tmp) != 0xFFFFFFFF) || (make_path(tmp) && (zip_entry_fread(fs, tmp) == 0))) {
        HMODULE hm;
        char funcname[MAX_PATH];
        if ( (hm = LoadLibraryW(tmp)) ) {
          _snprintf(funcname, MAX_PATH, "luaopen_%s", luaL_gsub(L, name, ".", "_"));
          lua_pop(L, 1);
          lua_CFunction f = (lua_CFunction)(voidf)GetProcAddress(hm, funcname);
          if (f) {
            lua_pushlightuserdata(L, (void*)hm);
            lua_rawset(L, -3);
            lua_pushcfunction(L, f);
          } else FreeLibrary(hm);
        } else {
          luaL_getlasterror(L, GetLastError());
          luaL_error(L, "error loading module '%s' from embedded file '%s' : %s", lua_tostring(L, 1), filename, lua_tostring(L, -1));
        }
      }
      free(tmp);
      zip_entry_close(fs);
      if (lua_isfunction(L, -1)) {
        lua_pushstring(L, filename); 
        return 2;   
      }
    }
  }
  const char *p = strchr(name, '.');
  if (p == NULL) return 0;  /* is root */
  lua_pushlstring(L, name, p - name);
  if ((filename = findfile(L, lua_tostring(L, -1), "cpath", "/")))
    goto found;
  lua_pushfstring(L,"no embedded file '%s'", filename);
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
LUA_API BYTE *luaL_embedopen(lua_State *L) {
  if (!datafs) {
    HRSRC hres = FindResource(NULL, MAKEINTRESOURCE(EMBED), RT_RCDATA);
    HGLOBAL hdata = LoadResource(NULL, hres);
    datafs = LockResource(hdata);
    size_t size = SizeofResource(NULL, hres);
    if (datafs && (size > 2) && (fs = open_fs(datafs, size))) {
      lua_Integer len;
      lua_getglobal(L, "package");
      lua_getfield(L, -1, "searchers");
      len = luaL_len(L, -1);
      lua_pushvalue(L, -2);
      lua_pushcclosure(L, searcher_embedded_Lua, 1);
      lua_rawseti(L, -2, ++len);
      lua_pushvalue(L, -2);
      lua_pushcclosure(L, searcher_embedded_C, 1);
      lua_rawseti(L, -2, ++len);
      lua_pop(L, 2);
      SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
      AddDllDirectory(temp_path);
    } else fs = NULL;
  }
  return (BYTE*)fs;
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
