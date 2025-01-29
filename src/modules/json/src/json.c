#include "zzzJSON.h"
#include <luart.h>
#include <File.h>
#include <math.h>

static Allocator *A;

static void LuaToJson(lua_State *L, Value *v) {
    switch(lua_type(L, -1)) {
        case LUA_TNIL:
            SetNull(v);
            break;
        case LUA_TNUMBER:
            SetNum(v, lua_tonumber(L, -1));
            break;
        case LUA_TBOOLEAN:
            SetBool(v, lua_toboolean(L, -1));
            break;
        case LUA_TTABLE: {
            lua_Integer len;
            if ((len = luaL_len(L, -1))) { //---- table is an array
                SetArray(v);
            	lua_pushnil(L);
	            while (lua_next(L, -2)) {
                    Value *toadd = NewValue(v->A);
                    LuaToJson(L, toadd);
		            if (!ArrayAddFast(v, toadd))
                        return;
                    lua_pop(L, 1);
                }
            } else {
                SetObj(v);
            	lua_pushnil(L);
	            while (lua_next(L, -2)) {
                    Value *toadd = NewValue(v->A);
                    SetKeyFast(toadd, lua_tostring(L, -2));
                    LuaToJson(L, toadd);
		            if (!ObjAddFast(v, toadd))
                        return;
                    lua_pop(L, 1);
                }
            }
        } break;

        default: if (strcmp(lua_tostring(L, -1), "null") == 0)
                    SetNull(v);
                 else
                    SetStrFast(v, lua_tostring(L, -1));
    }
}

static void JsonToLua(lua_State *L, Value *v) {
    const JSONType *t = Type(v);
    if (t == 0)
        lua_pushnil(L);
    switch (*t)
    {
        case JSONTypeArray:
        {
            lua_createtable(L, SizeOf(v), 0);
            Value *next = Begin(v);
            int i = 0;
            while (next != 0)
            {
                JsonToLua(L, next);
                lua_rawseti(L, -2, ++i);
                next = Next(next);
            }
            break;
        }
        case JSONTypeObject:
        {
            lua_createtable(L, 0, SizeOf(v));
            Value *next = Begin(v);
            while (next != 0)
            {
                lua_pushstring(L, GetKey(next));
                JsonToLua(L, next);
                lua_rawset(L, -3);
                next = Next(next);
            }
            break;
        }
        case JSONTypeBool:
            lua_pushboolean(L, *GetBool(v));
            break;

        case JSONTypeNull:
            lua_pushstring(L, "null");
            break;

        case JSONTypeString:
        {
            const char *str = GetUnEscapeStr(v);
            if (str == 0)
                lua_pushnil(L);
            else
                lua_pushstring(L, str);
            break;
        }
        case JSONTypeNumber:
        {
            double num = *GetDouble(v);
            if (fmod(num, 1) == 0.0)
                lua_pushinteger(L, num);
            else lua_pushnumber(L, num);
            break;
        }
    }
}    


//------------------------------------ json.decode() function
LUA_METHOD(json, decode)
{
    Value *src_v = NewValue(A);
    const char *src = luaL_checkstring(L, 1);

    if (!ParseFast(src_v, src))
        return 0;
    JsonToLua(L, src_v);
    return 1;
}

//------------------------------------ json.load() function
LUA_METHOD(json, load)
{
    Value *src_v = NewValue(A);
    wchar_t *fname = luaL_checkFilename(L, 1);
    FILE *f = _wfopen(fname, L"rb");
    int result = 0;
    
    if (f) {
      fseek(f, 0, SEEK_END);
      long long fsize = _ftelli64(f);
      fseek(f, 0, SEEK_SET);
      char *src = calloc(1, fsize + 1);
      if(fread(src, fsize, 1, f) && ParseFast(src_v, src)) {
        JsonToLua(L, src_v);
        result = 1;
      }
      free(src);
    }
    free(fname);
    fclose(f);
    return result;
}
//------------------------------------ json.encode() function
LUA_METHOD(json, encode)
{
    Value *v = NewValue(A);
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushvalue(L, 1);
    LuaToJson(L, v);
    lua_pushstring(L, Stringify(v));
    return 1;
}

//------------------------------------ json.save() function
LUA_METHOD(json, save)
{
    wchar_t *fname = luaL_checkFilename(L, 1);
    FILE *f = _wfopen(fname, L"wb");
    
    if (f) {
        Value *v = NewValue(A);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_pushvalue(L, 2);
        LuaToJson(L, v);
        const char *str = Stringify(v);
        lua_pushboolean(L, fwrite(str, strlen(str), 1, f));
        fclose(f);      
    }
    free(fname);
    return 1;
}

LUA_METHOD(json, finalize) {
    ReleaseAllocator(A);
    return 0;
}

//--- luaL_Reg array for module properties, postfixed by "_properties"          
static const luaL_Reg json_properties[] = {
  {NULL, NULL}
};

//--- Register module functions, postfixed by "lib"  
MODULE_FUNCTIONS(json)
  METHOD(json, decode)
  METHOD(json, encode)
  METHOD(json, load)
  METHOD(json, save)
END

//----- "calc" module registration function
int __declspec(dllexport) luaopen_json(lua_State *L)
{
    //--- lua_regmodulefinalize() registers the specified module with a finalizer function
    //--- and pushes the module on the stack
    A = NewAllocator();
    lua_regmodulefinalize(L, json);
    //--- returns one value (the just pushed calc module)
    return 1;
}