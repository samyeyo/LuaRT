/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | audio.c | LuaRT audio module
*/

#include <luart.h>
#include <File.h> 
#include "encoder.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#undef L

#include "audio.h"
#include "sound.h"

const char *vector_members[] = { "x", "y", "z" };
const char *cone_members[] = { "inangle", "outangle", "outgain" };

typedef struct {
  ma_device_id    *id;
  char            name[256];
  Encoder         *encoder;
} ma_record;

ma_engine *engine = NULL;
ma_record record = {0};

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

LUA_PROPERTY_GET(audio, playdevice) {
  ma_device *dev = ma_engine_get_device(engine);
  lua_pushstring(L, dev->playback.name);
  return 1;
}

LUA_PROPERTY_GET(audio, recdevice) {
  if (record.name[0] == 0) {
    ma_device_config deviceConfig;
    ma_device device;
    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 2;
    deviceConfig.sampleRate = 44100;
    deviceConfig.dataCallback = NULL;
    deviceConfig.pUserData = NULL;
    if (ma_device_init(NULL, &deviceConfig, &device) == MA_SUCCESS) {
      strncpy(record.name, device.capture.name, 256);
      ma_device_uninit(&device);
    } else strncpy(record.name, "unknown", 7);
  }
  lua_pushstring(L, record.name); 
  return 1;
}

static BOOL enum_devices(ma_device_info **pPlaybackDeviceInfos, ma_uint32 *pcount, ma_device_info ** pRecDeviceInfos, ma_uint32 *rcount) {
  ma_context context; 
  BOOL result = !ma_context_init(NULL, 0, NULL, &context) && !ma_context_get_devices(&context, pPlaybackDeviceInfos, pcount, pRecDeviceInfos, rcount);
  ma_context_uninit(&context); 
  return result;
}

LUA_PROPERTY_SET(audio, playdevice) {
  const char *device_name = luaL_checkstring(L, 1);
  ma_device_info *devices;
  ma_uint32 count; 

  if (enum_devices(&devices, &count, NULL, NULL)) {
    for (int i = 0; i < count; i++) {
      if (strcmp(devices[i].name, device_name) == 0) {
        ma_engine_config engineConfig = ma_engine_config_init();
        ma_engine_uninit(engine);
        engineConfig.pPlaybackDeviceID = &(devices[i].id);
        engineConfig.noDevice = MA_FALSE;

        if (ma_engine_init(&engineConfig, engine) || ma_engine_start(engine)) {
          ma_engine_uninit(engine);
          ma_engine_init(NULL, engine);
        } else break;
      }
    }
  }
  return 0;
}

LUA_PROPERTY_SET(audio, recdevice) {
  const char *device_name = luaL_checkstring(L, 1);
  ma_device_info *devices;
  ma_uint32 count; 

  if (enum_devices(NULL, NULL, &devices, &count)) {
    for (int i = 0; i < count; i++) {
      if (strcmp(devices[i].name, device_name) == 0) {
        record_stop(L);
        record.id = &devices[i].id;
        strncpy(record.name, devices[i].name, 256);
      }
    }
  }
  return 0;
}

LUA_PROPERTY_GET(audio, devices) {
  ma_device_info *devices, *recdevices;
  ma_uint32 pcount, rcount; 
  int i; 

  lua_createtable(L, 0, 2);
  if (enum_devices(&devices, &pcount, &recdevices, &rcount)) {
    lua_createtable(L, pcount, 0);
    for (i = 0; i < pcount; i++) {
      lua_pushstring(L, devices[i].name);
      lua_rawseti(L, -2, i+1);
    }
    lua_setfield(L, -2, "playback");
    lua_createtable(L, rcount, 0);
    for (i = 0; i < rcount; i++) {
      lua_pushstring(L, recdevices[i].name);
      lua_rawseti(L, -2, i+1);
    }
    lua_setfield(L, -2, "recording");
  } 
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

typedef void(*EncoderCallback)(ma_device*, void*, const void*, ma_uint32);

LUA_METHOD(record, stop) {
    if (record.encoder) {
      Encoder_Release(record.encoder);
      memset(&record, 0, sizeof(record));
  }
  return 0;
}

LUA_METHOD(record, start) {
  record_stop(L);
  wchar_t *fname = luaL_checkFilename(L, 1);
  ma_device_config deviceConfig;
  EncoderType format = lua_optstring(L, 2, AudioFormatNames, -1);
  int channels = luaL_optinteger(L, 3, 2);
  int sampleRate = luaL_optinteger(L, 4, 44100);
  int bitrate = luaL_optinteger(L, 5, 0);

  if (format == -1)
    luaL_error(L, "'%s' encoder not supported", lua_tostring(L, 2));
  if (!(record.encoder = Encoder_Create(fname, format, channels, sampleRate, bitrate, &record.id)))
    luaL_error(L, "failed to initialize %s encoder", AudioFormatNames[format]);
  free(fname);
  return 0;
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
  READWRITE_PROPERTY(audio, playdevice)
  READWRITE_PROPERTY(audio, recdevice)
  READONLY_PROPERTY(audio, devices)
END

//------------------------------- audio module functions definition

MODULE_FUNCTIONS(audio)
  METHOD(audio, play)
END

MODULE_FUNCTIONS(record)
  METHOD(record, start)
  METHOD(record, stop)
END

//------------------------------- audio module finalizer

int audio_finalize(lua_State *L)
{ 
  record_stop(L);
  if (engine) {
    ma_engine_uninit(engine);
    free(engine); 
    engine = NULL;
  }
  MFShutdown();
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
  if (ma_engine_init(NULL, engine) != MA_SUCCESS) {
    free(engine);
    luaL_error(L, "fatal error initalizing audio module");
  }
  MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
  lua_regmodulefinalize(L, audio);
  lua_pushstring(L, "record");
  lua_createtable(L, 0, 2);
  luaL_setfuncs(L, recordlib, 0);
  lua_rawset(L, -3);
  lua_regobjectmt(L, Sound);
  return 1;
}