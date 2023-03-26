 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | sound.c | LuaRT Sound object implementation
*/

#include <luart.h>
#include <File.h>
#include "audio.h"
#include "sound.h"

luart_type TSound;

//-------------------------------------[ Sound Constructor ]
LUA_CONSTRUCTOR(Sound) {
	Sound *s = (Sound *)calloc(1, sizeof(Sound));
    wchar_t *fname = luaL_checkFilename(L, 2);
    ma_result r;
    ma_uint32 flags = lua_toboolean(L, 3) ? FALSE : MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC;

    flags |= lua_toboolean(L, 4) ? FALSE : MA_SOUND_FLAG_NO_PITCH;
    flags |= lua_toboolean(L, 5) ? FALSE : MA_SOUND_FLAG_NO_SPATIALIZATION;

    if ((r = ma_sound_init_from_file_w(engine, fname, flags, NULL, NULL, &s->sound)) != MA_SUCCESS) {
        free(s);
        luaL_error(L, ma_result_description(r));
    }
	lua_newinstance(L, s, Sound);
	return 1;
}

vector_objproperty(Sound, position, vector_members, position);
vector_objproperty(Sound, direction, vector_members, direction);
vector_objproperty(Sound, velocity, vector_members, velocity);

float_objproperty(Sound, attenuation, directional_attenuation_factor);
float_objproperty(Sound, doppler, doppler_factor);
float_objproperty(Sound, mindistance, min_distance);
float_objproperty(Sound, maxdistance, max_distance);
float_objproperty(Sound, mingain, min_gain);
float_objproperty(Sound, maxgain, max_gain);
float_objproperty(Sound, volume, volume);
float_objproperty(Sound, pan, pan);

LUA_PROPERTY_GET(Sound, loop) {
  lua_pushboolean(L, ma_sound_is_looping(&lua_self(L, 1, Sound)->sound));
  return 1;
}

LUA_PROPERTY_SET(Sound, loop) {
  ma_sound_set_looping(&lua_self(L, 1, Sound)->sound, lua_toboolean(L, 2));
  return 0;
}

LUA_PROPERTY_GET(Sound, ended) {
  lua_pushboolean(L, ma_sound_at_end(&lua_self(L, 1, Sound)->sound));
  return 1;
}

LUA_PROPERTY_GET(Sound, playing) {
  lua_pushboolean(L, ma_sound_is_playing(&lua_self(L, 1, Sound)->sound));
  return 1;
}

LUA_PROPERTY_GET(Sound, pitch) {
  lua_pushnumber(L, ma_sound_group_get_pitch(&lua_self(L, 1, Sound)->sound));
  return 1;
}

LUA_PROPERTY_GET(Sound, duration) {
  float f;
  ma_sound_get_length_in_seconds(&lua_self(L, 1, Sound)->sound, &f);
  lua_pushnumber(L, f);
  return 1;
}

LUA_PROPERTY_GET(Sound, cursor) {
  float f;
  ma_sound_get_cursor_in_seconds(&lua_self(L, 1, Sound)->sound, &f);
  lua_pushnumber(L, f); 
  return 1;
}

LUA_PROPERTY_GET(Sound, echo) {
  lua_pushboolean(L, lua_self(L, 1, Sound)->echo && delayNode);
  return 1;
}

LUA_PROPERTY_SET(Sound, echo) {
  if (delayNode) {
    Sound *s = lua_self(L, 1, Sound);
    if ((s->echo = lua_toboolean(L, 2))) {
      ma_node_attach_output_bus(&s->sound, 0, delayNode, 0);
      puts("Echo sound enabled");
    }
    else
      ma_node_detach_output_bus(&s->sound, 0);
  }
  return 0;
}

LUA_PROPERTY_SET(Sound, cursor) {
  Sound *s = lua_self(L, 1, Sound);
  ma_uint32 sampleRate;

  ma_data_source_get_data_format(s->sound.pDataSource, NULL, NULL, &sampleRate, NULL, 0);
  s->r = ma_data_source_seek_to_pcm_frame(s->sound.pDataSource, luaL_checknumber(L, 2)*sampleRate);
  return 0;
}

LUA_PROPERTY_SET(Sound, pitch) {
  ma_sound_group_set_pitch(&lua_self(L, 1, Sound)->sound, lua_tonumber(L, 2));
  return 1;
}

LUA_METHOD(Sound, fade) {
  ma_sound_set_fade_in_milliseconds(&lua_self(L, 1, Sound)->sound, luaL_optnumber(L, 2, 0), luaL_optnumber(L, 3, 1), luaL_optnumber(L, 3, 1000));
  return 0;
}

LUA_METHOD(Sound, seek) {
  Sound *s = lua_self(L, 1, Sound);
  s->r = ma_sound_seek_to_pcm_frame(&s->sound, luaL_optnumber(L, 2, 0));
  lua_pushboolean(L, s->r == MA_SUCCESS);
  return 1; 
}

LUA_METHOD(Sound, play) {
  Sound *s = lua_self(L, 1, Sound);
  if (lua_gettop(L) > 1)
    ma_sound_set_start_time_in_milliseconds(&s->sound, luaL_checknumber(L, 2));
  s->r = ma_sound_start(&s->sound);
  lua_pushboolean(L, s->r == MA_SUCCESS);
  return 1; 
}

LUA_METHOD(Sound, stop) {
  Sound *s = lua_self(L, 1, Sound);
  if (lua_gettop(L) > 1)
    ma_sound_set_stop_time_in_milliseconds(&s->sound, luaL_checknumber(L, 2));
  s->r = ma_sound_stop(&s->sound);
  lua_pushboolean(L, s->r == MA_SUCCESS);
  return 1; 
}

OBJECT_MEMBERS(Sound)
    READWRITE_PROPERTY(Sound, position)
    READWRITE_PROPERTY(Sound, direction)
    READWRITE_PROPERTY(Sound, velocity)
    READWRITE_PROPERTY(Sound, doppler)
    READWRITE_PROPERTY(Sound, maxdistance)
    READWRITE_PROPERTY(Sound, mindistance)
    READWRITE_PROPERTY(Sound, maxgain)
    READWRITE_PROPERTY(Sound, mingain)
    READWRITE_PROPERTY(Sound, volume)
    READWRITE_PROPERTY(Sound, loop)
    READWRITE_PROPERTY(Sound, pitch)
    READWRITE_PROPERTY(Sound, cursor)
    READWRITE_PROPERTY(Sound, echo)
    READONLY_PROPERTY(Sound, ended)
    READONLY_PROPERTY(Sound, playing)
    READONLY_PROPERTY(Sound, duration)
    METHOD(Sound, fade)
    METHOD(Sound, seek)
    METHOD(Sound, play)
    METHOD(Sound, stop)
END

LUA_METHOD(Sound, __gc) {
    Sound *s = lua_self(L, 1, Sound);
    ma_sound_uninit(&s->sound);
    free(s);
    return 0;
}

OBJECT_METAFIELDS(Sound)
    METHOD(Sound, __gc)
END
