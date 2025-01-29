 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | sound.c | LuaRT Sound object implementation
*/

#include <luart.h>
#include <File.h>
#include "audio.h"
#include "sound.h"
#include "ma_reverb_node.h"

luart_type TSound;

//-------------------------------------[ Sound Constructor ]
LUA_CONSTRUCTOR(Sound) {
	Sound *s = (Sound *)calloc(1, sizeof(Sound));
  wchar_t *fname = luaL_checkFilename(L, 2);
  ma_result r;
  ma_uint32 flags = lua_toboolean(L, 3) ? MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC : 0;

  flags |= lua_toboolean(L, 4) ? MA_SOUND_FLAG_NO_PITCH : 0;
  flags |= lua_toboolean(L, 5) ? MA_SOUND_FLAG_NO_SPATIALIZATION : 0;

  if ((r = ma_sound_init_from_file_w(engine, fname, flags, NULL, NULL, &s->sound)) != MA_SUCCESS) {
      free(s);
      luaL_error(L, ma_result_description(r));
  } 
	lua_newinstance(L, s, Sound);
	return 1;
}

vector_objproperty(Sound, 2, position, vector_members, position);
vector_objproperty(Sound, 2, direction, vector_members, direction);
vector_objproperty(Sound, 2, velocity, vector_members, velocity);

LUA_PROPERTY_GET(Sound, cone) {
  ma_vec3f v;
  ma_sound_get_cone(&lua_self(L, 1, Sound)->sound, &v.x, &v.y, &v.z);
  return set_vector(L, &v, cone_members);
}

LUA_PROPERTY_SET(Sound, cone) { 
  ma_vec3f v = get_vector(L, 2, cone_members); 
  ma_sound_set_cone(&lua_self(L, 1, Sound)->sound, v.x, v.y, v.z); 
  return 0; 
} 

float_objproperty(Sound, attenuation, directional_attenuation_factor);
float_objproperty(Sound, doppler, doppler_factor);
float_objproperty(Sound, mindistance, min_distance);
float_objproperty(Sound, maxdistance, max_distance);
float_objproperty(Sound, mingain, min_gain);
float_objproperty(Sound, maxgain, max_gain);
float_objproperty(Sound, volume, volume);
float_objproperty(Sound, pan, pan);
float_objproperty(Sound, rolloff, rolloff);

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
  lua_pushnumber(L, ma_sound_get_pitch(&lua_self(L, 1, Sound)->sound));
  return 1;
}

LUA_PROPERTY_GET(Sound, duration) {
  float f;
  ma_sound_get_length_in_seconds(&lua_self(L, 1, Sound)->sound, &f);
  lua_pushinteger(L, f);
  return 1;
}

LUA_PROPERTY_GET(Sound, cursor) {
  float f;
  ma_sound_get_cursor_in_seconds(&lua_self(L, 1, Sound)->sound, &f);
  lua_pushnumber(L, f); 
  return 1;
}

LUA_METHOD(Sound, echo) {
  Sound *s = lua_self(L, 1, Sound);
  ma_delay_node_config delayNodeConfig;
  int n = lua_gettop(L);

  if (s->delayNode) {
    ma_node_attach_output_bus(&s->sound, 0, ma_engine_get_endpoint(engine), 0); 
    ma_node_set_state(s->delayNode, ma_node_state_stopped);
    ma_delay_node_uninit(s->delayNode, NULL);
    free(s->delayNode);
    s->delayNode = NULL;
  }
  if ((n == 1) || !lua_isnil(L, 2)) {
    s->delayNode = calloc(1, sizeof(ma_delay_node));
    delayNodeConfig = ma_delay_node_config_init(ma_engine_get_channels(engine), ma_engine_get_sample_rate(engine), (ma_uint32)ma_engine_get_sample_rate(engine) * luaL_optnumber(L, 2, 0.4), luaL_optnumber(L, 3, 0.3));
    ma_delay_node_init(ma_engine_get_node_graph(engine), &delayNodeConfig, NULL, s->delayNode);
    ma_delay_node_set_dry(s->delayNode, luaL_optnumber(L, 4, 1));
    ma_delay_node_set_wet(s->delayNode, luaL_optnumber(L, 5, 1));
    ma_node_attach_output_bus(s->delayNode, 0, ma_engine_get_endpoint(engine), 0); 
    ma_node_attach_output_bus(&s->sound, 0, s->delayNode, 0);
  }
  return 0;
}

LUA_METHOD(Sound, reverb) {
  Sound *s = lua_self(L, 1, Sound);
  ma_reverb_node_config reverbNodeConfig;
  int n = lua_gettop(L);

  if (s->reverbNode) {
    ma_node_attach_output_bus(&s->sound, 0, ma_engine_get_endpoint(engine), 0); 
    ma_node_set_state(s->reverbNode, ma_node_state_stopped);
    ma_reverb_node_uninit(s->reverbNode, NULL);
    free(s->reverbNode);
    s->reverbNode = NULL;
  }
  if ((n == 1) || !lua_isnil(L, 2)) {
    s->reverbNode = calloc(1, sizeof(ma_reverb_node));
    reverbNodeConfig = ma_reverb_node_config_init(ma_engine_get_channels(engine), ma_engine_get_sample_rate(engine));
    reverbNodeConfig.roomSize   = luaL_optnumber(L, 2, verblib_initialroom);
    reverbNodeConfig.damping    = luaL_optnumber(L, 3, verblib_initialdamp);
    reverbNodeConfig.width      = luaL_optnumber(L, 4, verblib_initialwidth);
    reverbNodeConfig.dryVolume  = luaL_optnumber(L, 5, verblib_initialdry);
    reverbNodeConfig.wetVolume  = luaL_optnumber(L, 6, verblib_initialwet);
    reverbNodeConfig.mode       = verblib_initialmode;
    ma_reverb_node_init(ma_engine_get_node_graph(engine), &reverbNodeConfig, NULL, s->reverbNode);
    ma_node_attach_output_bus(s->reverbNode, 0, ma_engine_get_endpoint(engine), 0); 
    ma_node_attach_output_bus(&s->sound, 0, s->reverbNode, 0);
  }
  return 0;
}

LUA_PROPERTY_SET(Sound, cursor) {
  Sound *s = lua_self(L, 1, Sound);
  ma_uint32 sampleRate;

  ma_data_source_get_data_format(s->sound.pDataSource, NULL, NULL, &sampleRate, NULL, 0);
  ma_data_source_seek_to_pcm_frame(s->sound.pDataSource, luaL_checknumber(L, 2)*sampleRate);
  return 0;
}

LUA_PROPERTY_SET(Sound, pitch) {
  ma_sound_set_pitch(&lua_self(L, 1, Sound)->sound, luaL_checknumber(L, 2));
  return 0;
}

LUA_METHOD(Sound, fade) {
  ma_sound_set_fade_in_milliseconds(&lua_self(L, 1, Sound)->sound, luaL_optnumber(L, 2, 0), luaL_optnumber(L, 3, 1), luaL_optinteger(L, 4, 1000));
  return 0;
}

LUA_METHOD(Sound, seek) {
  Sound *s = lua_self(L, 1, Sound);
  ma_uint32 sampleRate;

  ma_data_source_get_data_format(s->sound.pDataSource, NULL, NULL, &sampleRate, NULL, 0);
  ma_sound_seek_to_pcm_frame(&s->sound, luaL_optnumber(L, 2, 0)*sampleRate);
  return 0; 
}

static int PlayTaskContinue(lua_State* L, int status, lua_KContext ctx) {	
	Sound *s = (Sound *)ctx;

	if (!ma_sound_is_playing(&s->sound))
		return 0;
  return lua_yieldk(L, 0, ctx, PlayTaskContinue);
}

LUA_METHOD(Sound, play) {
  Sound *s = lua_self(L, 1, Sound);
  if (lua_gettop(L) > 1)
    ma_sound_set_start_time_in_milliseconds(&s->sound, ma_engine_get_time(engine)+luaL_checkinteger(L, 2));
  ma_sound_start(&s->sound);
  lua_pushtask(L, PlayTaskContinue, s, NULL);
  return 1; 
}

LUA_METHOD(Sound, stop) {  
  Sound *s = lua_self(L, 1, Sound);
  if (lua_gettop(L) > 1)
    ma_sound_set_stop_time_in_milliseconds(&s->sound, ma_engine_get_time(engine)+luaL_checkinteger(L, 2));
  else
    ma_sound_stop(&s->sound);
  return 0; 
}

OBJECT_MEMBERS(Sound)
    READWRITE_PROPERTY(Sound, position)
    READWRITE_PROPERTY(Sound, direction)
    READWRITE_PROPERTY(Sound, velocity)
    READWRITE_PROPERTY(Sound, cone)
    READWRITE_PROPERTY(Sound, doppler)
    READWRITE_PROPERTY(Sound, maxdistance)
    READWRITE_PROPERTY(Sound, mindistance)
    READWRITE_PROPERTY(Sound, maxgain)
    READWRITE_PROPERTY(Sound, mingain)
    READWRITE_PROPERTY(Sound, volume)
    READWRITE_PROPERTY(Sound, loop)
    READWRITE_PROPERTY(Sound, pitch)
    READWRITE_PROPERTY(Sound, pan)
    READWRITE_PROPERTY(Sound, rolloff)
    READWRITE_PROPERTY(Sound, cursor)
    READONLY_PROPERTY(Sound, ended)
    READONLY_PROPERTY(Sound, playing)
    READONLY_PROPERTY(Sound, duration)
    METHOD(Sound, fade)
    METHOD(Sound, seek)
    METHOD(Sound, play)
    METHOD(Sound, stop)
    METHOD(Sound, echo)
    METHOD(Sound, reverb)
END

LUA_METHOD(Sound, __gc) {
    Sound *s = lua_self(L, 1, Sound);
    if (s->delayNode) {
      ma_delay_node_uninit(s->delayNode, NULL);
      free(s->delayNode);
    }
    if (s->reverbNode) {
      ma_reverb_node_uninit(s->reverbNode, NULL);
      free(s->reverbNode);
    }
    ma_sound_uninit(&s->sound);
    free(s);
    return 0;
}

OBJECT_METAFIELDS(Sound)
    METHOD(Sound, __gc)
END
