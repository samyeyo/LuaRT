 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | sound.h | LuaRT Sound object header
*/

#pragma once

#include <luart.h>
#include "miniaudio.h"


//---------------- Sound object

typedef struct {
    luart_type  type;
	  ma_sound    sound;
    ma_result   r;
    BOOL        echo;
} Sound;

#define vector_objproperty(obj, property, member, func) \
LUA_PROPERTY_GET(obj, property) { \
  ma_vec3f v = ma_sound_get_##func(&lua_self(L, 1, Sound)->sound); \
  return set_vector(L, &v, member); \
} \
\
LUA_PROPERTY_SET(obj, property) { \
  ma_vec3f v = get_vector(L, member); \
  ma_sound_set_##func(&lua_self(L, 1, Sound)->sound, v.x, v.y, v.z); \
  return 0; \
} 

#define float_objproperty(obj, property, func) \
LUA_PROPERTY_GET(obj, property) { \
  lua_pushnumber(L, ma_sound_get_##func(&lua_self(L, 1, Sound)->sound)); \
  return 1; \
} \
\
LUA_PROPERTY_SET(obj, property) { \
  ma_sound_set_##func(&lua_self(L, 1, Sound)->sound, lua_tonumber(L, 2)); \
  return 0; \
} 

extern luart_type TSound;

LUA_CONSTRUCTOR(Sound);
extern const luaL_Reg Sound_methods[];
extern const luaL_Reg Sound_metafields[];