#include "zzzJSON.h"
#include <luart.h>

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

        default: SetStrFast(v, lua_tostring(L, -1));
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
            lua_pushnil(L);
            break;

        case JSONTypeString:
        {
            const char *str = GetStr(v);
            if (str == 0)
                lua_pushnil(L);
            else
                lua_pushstring(L, str);
            break;
        }
        case JSONTypeNumber:
        {
            lua_pushnumber(L, *GetNum(v));
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
    ReleaseAllocator(A);
    return 1;
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

LUA_METHOD(json, finalize) {
    ReleaseAllocator(A);
}

//--- luaL_Reg array for module properties, postfixed by "_properties"          
static const luaL_Reg json_properties[] = {
  {NULL, NULL}
};

//--- luaL_Reg array for module functions, postfixed by "lib"  
static const luaL_Reg jsonlib[] = {
  {"decode",		json_decode}, 
  {"encode",		json_encode}, 
  {NULL, NULL}
};

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