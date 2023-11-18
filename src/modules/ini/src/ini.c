#define INI_IMPLEMENTATION
#include "ini.h"
#include <luart.h>
#include <File.h>
#include <stdio.h>
#include <stdlib.h>


//------------------------------------ ini.decode() function
static void push_section(lua_State *L, ini_t *ini, int section) {
  int count = ini_property_count(ini, section);
  
  lua_createtable(L, 0, count);
  for (int i = 0; i < count; i++) {
    lua_pushstring(L, ini_property_name(ini, section, i));
    lua_pushstring(L, ini_property_value(ini, section, i));
    lua_rawset(L, -3);
  }
}

LUA_METHOD(Ini, decode)
{
    const char *str = luaL_checkstring(L, 1);
    ini_t* ini = ini_load(str, NULL);
    int count = ini_section_count(ini);
    
    push_section(L, ini, 0);
    for (int i = 1; i < count; i++) {
      lua_pushstring(L, ini_section_name(ini, i));
      push_section(L, ini, i);
      lua_rawset(L, -3);
    }
    ini_destroy(ini);
    return 1;
}

//------------------------------------ ini.load() function
LUA_METHOD(Ini, load)
{
    wchar_t *fname = luaL_checkFilename(L, 1);
    FILE *f = _wfopen(fname, L"rb");
    
    if (f) {
      fseek(f, 0, SEEK_END);
      long long fsize = _ftelli64(f);
      fseek(f, 0, SEEK_SET);
      char *str = calloc(1, fsize + 1);
      if(fread(str, fsize, 1, f)) {
        ini_t* ini = ini_load(str, NULL);
        int count = ini_section_count(ini);
        
        push_section(L, ini, 0);
        for (int i = 1; i < count; i++) {
          lua_pushstring(L, ini_section_name(ini, i));
          push_section(L, ini, i);
          lua_rawset(L, -3);
        }
        ini_destroy(ini);
      } else lua_pushnil(L);
      fclose(f);   
      free(str);
    } else lua_pushnil(L);
    free(fname);
    return 1;
}


static void write_section(lua_State *L, ini_t* ini) {
  size_t len1 = 0, len2 = 0;
  int section = 0;
  int idx;

  if (lua_isstring(L, -2) && lua_gettop(L) != 2)
    section = ini_section_add(ini, lua_tolstring(L, -2, &len1), len1);

  lua_pushnil(L);
  while (lua_next(L, -2)) {
    idx = -2;
    if (!lua_isstring(L, idx))
error:  luaL_error(L, "malformed table (string key expected, but %s key found)", luaL_typename(L, idx));
    idx = -1;
    int type = lua_type(L, idx);
    if (type ==  LUA_TTABLE)
      write_section(L, ini);
    else if ((type == LUA_TSTRING) || (type == LUA_TNUMBER)) {
      const char *str1 = lua_tolstring(L, -2, &len1);
      const char *str2 = lua_tolstring(L, -1, &len2);
      ini_property_add(ini, section, str1, len1, str2, len2);
    }
    else goto error;     
    lua_pop(L, 1);
  }
}

//------------------------------------ ini.encode() function
LUA_METHOD(Ini, encode)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    ini_t* ini = ini_create(NULL);
    write_section(L, ini);
    int size = ini_save( ini, NULL, 0 ); 
    char* data = (char*)malloc(size);
    size = ini_save(ini, data, size); 
    lua_pushlstring(L, data, size-2);
    free(data);
    ini_destroy(ini);
    return 1;
}

//------------------------------------ ini.save() function
LUA_METHOD(Ini, save)
{
    wchar_t *fname = luaL_checkFilename(L, 1);
    FILE *f = _wfopen(fname, L"wb");
    
    if (f) {
      luaL_checktype(L, 2, LUA_TTABLE);
      ini_t* ini = ini_create(NULL);
      write_section(L, ini);
      int size = ini_save( ini, NULL, 0 );
      char* data = (char*) malloc(size);
      size = ini_save(ini, data, size)-2; 
      lua_pushboolean(L, fwrite(data, size, 1, f));
      fclose(f);
      ini_destroy(ini);
      free(data);
      free(fname);
    } else lua_pushboolean(L, FALSE);
    return 1;
}

//--- luaL_Reg array for module properties, postfixed by "_properties"          
static const luaL_Reg ini_properties[] = {
  {NULL, NULL}
};

//--- luaL_Reg array for module functions, postfixed by "lib"  

MODULE_FUNCTIONS(ini)
  METHOD(Ini, decode)
  METHOD(Ini, encode)
  METHOD(Ini, load)
  METHOD(Ini, save)
END

//----- "calc" module registration function
int __declspec(dllexport) luaopen_ini(lua_State *L)
{
    lua_regmodule(L, ini);
    return 1;
}