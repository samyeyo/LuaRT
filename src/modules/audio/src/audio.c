/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | audio.c | LuaRT audio module
*/

#include <luart.h>
#include <File.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"   

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#undef L

#include "audio.h"
#include "sound.h"

const char *vector_members[] = { "x", "y", "z" };
const char *cone_members[] = { "inangle", "outangle", "outgain" };

ma_engine *engine;
static float volume = 1, gain = 0;

int set_vector(lua_State *L, ma_vec3f *v, const char **members) {
  lua_createtable(L, 3, 0);
  for (int i = 0; i < 3; i++) {
    lua_pushstring(L, members[i]);
    lua_pushnumber(L, *(float*)(((char*)v)+(i*sizeof(float))));
    lua_rawset(L, -3);
  }
  return 1;
}

ma_vec3f get_vector(lua_State *L, int idx, const char **members) {
  ma_vec3f v;
  luaL_checktype(L, idx, LUA_TTABLE);
  for (int i = 0; i < 3; i++) {
    lua_pushstring(L, members[i]);
    if (lua_rawget(L, idx) == LUA_TNUMBER) {
      *(float*)(((char*)&v)+(i*sizeof(float))) = lua_tonumber(L, -1);
      lua_pop(L, 1);
    } else luaL_argerror(L, 2, members == vector_members ? "table {x, y, z} expected" : "table {inangle, outangle, outgain} expected");
  }    
  return v;
}

vector_property(audio, 1, position, vector_members, position);
vector_property(audio, 1, direction, vector_members, direction);
vector_property(audio, 1, velocity, vector_members, velocity);
vector_property(audio, 1, worldup, vector_members, world_up);

LUA_PROPERTY_GET(audio, cone) {
  ma_vec3f v;
  ma_engine_listener_get_cone(engine, 0, &v.x, &v.y, &v.z);
  return set_vector(L, &v, cone_members);
}

LUA_PROPERTY_SET(audio, cone) { 
  ma_vec3f v = get_vector(L, 2, cone_members); 
  ma_engine_listener_set_cone(engine, 0, v.x, v.y, v.z); 
  return 0; 
} 

LUA_PROPERTY_GET(audio, volume) {
  lua_pushnumber(L, volume);
  return 1;
}

LUA_PROPERTY_SET(audio, volume) {
  volume = lua_tonumber(L, 1);
  ma_engine_set_volume(engine, volume);
  return 1;
}


LUA_PROPERTY_GET(audio, gain) {
  lua_pushnumber(L, gain);
  return 1;
}

LUA_PROPERTY_GET(audio, channels) {
  lua_pushinteger(L, ma_engine_get_channels(engine));
  return 1;
}

LUA_PROPERTY_GET(audio, samplerate) {
  lua_pushinteger(L, ma_engine_get_sample_rate(engine));
  return 1;
}

LUA_PROPERTY_GET(audio, device) {
  ma_device *dev = ma_engine_get_device(engine);
  lua_pushstring(L, dev->playback.name);
  return 1;
}

LUA_PROPERTY_SET(audio, gain) {
  gain = lua_tonumber(L, 1);
  ma_engine_set_gain_db(engine, gain);
  return 1;
}

LUA_METHOD(audio, play) {
  wchar_t *fname = luaL_checkFilename(L, 1);
  ma_result r = ma_engine_play_sound(engine, fname, NULL);
 
  free(fname);
  if (r != MA_SUCCESS)
    luaL_error(L, ma_result_description(r));
  lua_pushboolean(L, r == MA_SUCCESS);
  return 1;
}

//------------------------------- audio module properties definition

MODULE_PROPERTIES (audio)
  READWRITE_PROPERTY(audio, position)
  READWRITE_PROPERTY(audio, direction)
  READWRITE_PROPERTY(audio, velocity)
  READWRITE_PROPERTY(audio, worldup)
  READWRITE_PROPERTY(audio, cone)
  READWRITE_PROPERTY(audio, volume)
  READWRITE_PROPERTY(audio, gain)
  READONLY_PROPERTY(audio, channels)
  READONLY_PROPERTY(audio, samplerate)
  READONLY_PROPERTY(audio, device)
END

//------------------------------- audio module functions definition

MODULE_FUNCTIONS(audio)
  METHOD(audio, play)
END

//------------------------------- audio module finalizer

int audio_finalize(lua_State *L)
{ 
  if (engine) {
    ma_engine_uninit(engine);
    free(engine); 
    engine = NULL;
  }
  return 0;
}

static void freeout_list(ma_node_output_bus *out) {
  ma_node_output_bus *next;
  do { 
    next = out->pNext;
    ma_free(out, NULL);
  } while (next);
}

//---- Desesperate attempte to uninitialize in case of brutal exit
MA_API void ma_engine_uninit_rescue(ma_engine* pEngine) {
  ma_uint32 i;
  
  if (pEngine->ownsDevice) {
    CloseHandle(pEngine->pDevice->thread);
    ma_data_converter_uninit(&pEngine->pDevice->playback.converter, NULL);        
    ma_free(pEngine->pDevice->playback.pInputCache, NULL);
    ma_free(pEngine->pDevice->capture.pIntermediaryBuffer, NULL);
    ma_free(pEngine->pDevice->playback.pIntermediaryBuffer, NULL);
    ma_free(pEngine->pDevice, NULL);
    for (;;) {
      ma_sound_inlined* pSoundToDelete = pEngine->pInlinedSoundHead;
      if (pSoundToDelete == NULL)
        break;
      pEngine->pInlinedSoundHead = pSoundToDelete->pNext;
      ma_free(pSoundToDelete, NULL);            
    }
    for (i = 0; i < pEngine->listenerCount; i++)
      ma_free(&pEngine->listeners[i]._pHeap, NULL);
    if (pEngine->ownsResourceManager) {
      for (i = 0; i < pEngine->nodeGraph.endpoint.outputBusCount; i++)
       freeout_list(&pEngine->nodeGraph.endpoint._outputBuses[i]);
      ma_free(&pEngine->nodeGraph.endpoint._pHeap, NULL);
      ma_resource_manager_delete_all_data_buffer_nodes(pEngine->pResourceManager);
      ma_free(pEngine->pResourceManager->config.ppCustomDecodingBackendVTables, NULL);      
      ma_free(pEngine->pResourceManager, NULL);
    }
  }
}

BOOL WINAPI DllMain( IN HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved ) {
  switch ( nReason ) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls( hDllHandle );
      break;
    case DLL_PROCESS_DETACH:
      if (engine) {
        ma_engine_uninit_rescue(engine);
        free(engine);
        engine = NULL;
      }
      break;
    case DLL_THREAD_DETACH:
      break;
   }
  return TRUE;
 }

//------------------------------- audio module entry

int __declspec(dllexport) luaopen_audio(lua_State *L)
{
  engine = calloc(1, sizeof(ma_engine));
  ma_result r = ma_engine_init(NULL, engine);

  if (r != MA_SUCCESS) {
    free(engine);
    luaL_error(L, ma_result_description(r));
  }
  lua_regmodulefinalize(L, audio);
  lua_regobjectmt(L, Sound);
  return 1;
}