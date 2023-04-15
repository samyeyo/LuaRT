 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | audio.h | LuaRT audio module header
*/

#pragma once

#include <luart.h>
#include "miniaudio.h"

//------ macro helpers

#define vector_property(obj, idx, property, member, func) \
LUA_PROPERTY_GET(obj, property) { \
  ma_vec3f v = ma_engine_listener_get_##func(engine, 0); \
  return set_vector(L, &v, member); \
} \
\
LUA_PROPERTY_SET(obj, property) { \
  ma_vec3f v = get_vector(L, idx, member); \
  ma_engine_listener_set_##func(engine, 0, v.x, v.y, v.z); \
  return 0; \
} 

int set_vector(lua_State *L, ma_vec3f *v, const char **members);
ma_vec3f get_vector(lua_State *L, int idx, const char **members);

extern const char *vector_members[];
extern const char *cone_members[];
extern ma_engine *engine;
extern ma_delay_node *delayNode;
