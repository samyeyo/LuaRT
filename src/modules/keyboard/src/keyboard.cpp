#include <luart.h>
#include <File.h>
#include <stdio.h>
#include <stdlib.h>
#include "SendKeys.h"

typedef struct {
  const char *name;
  DWORD       vk;
} Key;

CSendKeys sk;

static const Key Keys[] = {
  { "LBUTTON" , 0x01 }, 
  { "RBUTTON" , 0x02 },
  { "CANCEL" , 0x03 },
  { "MBUTTON" , 0x04 },
  { "XBUTTON1" , 0x05 },
  { "XBUTTON2" , 0x06 },
  { "BACK" , 0x08 },
  { "TAB" , 0x09 },
  { "CLEAR" , 0x0C },
  { "RETURN" , 0x0D },
  { "SHIFT" , 0x10 },
  { "CONTROL" , 0x11 },
  { "MENU" , 0x12 },
  { "PAUSE" , 0x13 },
  { "CAPITAL" , 0x14 },
  { "KANA" , 0x15 },
  { "HANGUEL" , 0x15 },
  { "HANGUL" , 0x15 },
  { "IME_ON" , 0x16 },
  { "JUNJA" , 0x17 },
  { "FINAL" , 0x18 },
  { "HANJA" , 0x19 },
  { "KANJI" , 0x19 },
  { "IME_OFF" , 0x1A },
  { "ESCAPE" , 0x1B },
  { "CONVERT" , 0x1C },
  { "NONCONVERT" , 0x1D },
  { "ACCEPT" , 0x1E },
  { "MODECHANGE" , 0x1F },
  { "SPACE" , 0x20 },
  { "PRIOR" , 0x21 },
  { "NEXT" , 0x22 },
  { "END" , 0x23 },
  { "HOME" , 0x24 },
  { "LEFT" , 0x25 },
  { "UP" , 0x26 },
  { "RIGHT" , 0x27 },
  { "DOWN" , 0x28 },
  { "SELECT" , 0x29 },
  { "PRINT" , 0x2A },
  { "EXECUTE" , 0x2B },
  { "SNAPSHOT" , 0x2C },
  { "INSERT" , 0x2D },
  { "DELETE" , 0x2E },
  { "HELP" , 0x2F },
  { "0" , 0x30 },
  { "1" , 0x31 },
  { "2" , 0x32 },
  { "3" , 0x33 },
  { "4" , 0x34 },
  { "5" , 0x35 },
  { "6" , 0x36 },
  { "7" , 0x37 },
  { "8" , 0x38 },
  { "9" , 0x39 },
  { "A" , 0x41 },
  { "B" , 0x42 },
  { "C" , 0x43 },
  { "D" , 0x44 },
  { "E" , 0x45 },
  { "F" , 0x46 },
  { "G" , 0x47 },
  { "H" , 0x48 },
  { "I" , 0x49 },
  { "J" , 0x4A },
  { "K" , 0x4B },
  { "L" , 0x4C },
  { "M" , 0x4D },
  { "N" , 0x4E },
  { "O" , 0x4F },
  { "P" , 0x50 },
  { "Q" , 0x51 },
  { "R" , 0x52 },
  { "S" , 0x53 },
  { "T" , 0x54 },
  { "U" , 0x55 },
  { "V" , 0x56 },
  { "W" , 0x57 },
  { "X" , 0x58 },
  { "Y" , 0x59 },
  { "Z" , 0x5A },
  { "LWIN" , 0x5B },
  { "RWIN" , 0x5C },
  { "APPS" , 0x5D },
  { "SLEEP" , 0x5F },
  { "NUMPAD0" , 0x60 },
  { "NUMPAD1" , 0x61 },
  { "NUMPAD2" , 0x62 },
  { "NUMPAD3" , 0x63 },
  { "NUMPAD4" , 0x64 },
  { "NUMPAD5" , 0x65 },
  { "NUMPAD6" , 0x66 },
  { "NUMPAD7" , 0x67 },
  { "NUMPAD8" , 0x68 },
  { "NUMPAD9" , 0x69 },
  { "MULTIPLY" , 0x6A },
  { "ADD" , 0x6B },
  { "SEPARATOR" , 0x6C },
  { "SUBTRACT" , 0x6D },
  { "DECIMAL" , 0x6E },
  { "DIVIDE" , 0x6F },
  { "F1" , 0x70 },
  { "F2" , 0x71 },
  { "F3" , 0x72 },
  { "F4" , 0x73 },
  { "F5" , 0x74 },
  { "F6" , 0x75 },
  { "F7" , 0x76 },
  { "F8" , 0x77 },
  { "F9" , 0x78 },
  { "F10" , 0x79 },
  { "F11" , 0x7A },
  { "F12" , 0x7B },
  { "F13" , 0x7C },
  { "F14" , 0x7D },
  { "F15" , 0x7E },
  { "F16" , 0x7F },
  { "F17" , 0x80 },
  { "F18" , 0x81 },
  { "F19" , 0x82 },
  { "F20" , 0x83 },
  { "F21" , 0x84 },
  { "F22" , 0x85 },
  { "F23" , 0x86 },
  { "F24" , 0x87 },
  { "NUMLOCK" , 0x90 },
  { "SCROLL" , 0x91 },
  { "LSHIFT" , 0xA0 },
  { "RSHIFT" , 0xA1 },
  { "LCONTROL" , 0xA2 },
  { "RCONTROL" , 0xA3 },
  { "LMENU" , 0xA4 },
  { "RMENU" , 0xA5 },
  { "BROWSER_BACK" , 0xA6 },
  { "BROWSER_FORWARD" , 0xA7 },
  { "BROWSER_REFRESH" , 0xA8 },
  { "BROWSER_STOP" , 0xA9 },
  { "BROWSER_SEARCH" , 0xAA },
  { "BROWSER_FAVORITES" , 0xAB },
  { "BROWSER_HOME" , 0xAC },
  { "VOLUME_MUTE" , 0xAD },
  { "VOLUME_DOWN" , 0xAE },
  { "VOLUME_UP" , 0xAF },
  { "MEDIA_NEXT_TRACK" , 0xB0 },
  { "MEDIA_PREV_TRACK" , 0xB1 },
  { "MEDIA_STOP" , 0xB2 },
  { "MEDIA_PLAY_PAUSE" , 0xB3 },
  { "LAUNCH_MAIL" , 0xB4 },
  { "LAUNCH_MEDIA_SELECT" , 0xB5 },
  { "LAUNCH_APP1" , 0xB6 },
  { "LAUNCH_APP2" , 0xB7 },
  { "OEM_1" , 0xBA },
  { "OEM_PLUS" , 0xBB },
  { "OEM_COMMA" , 0xBC },
  { "OEM_MINUS" , 0xBD },
  { "OEM_PERIOD" , 0xBE },
  { "OEM_2" , 0xBF },
  { "OEM_3" , 0xC0 },
  { "OEM_4" , 0xDB },
  { "OEM_5" , 0xDC },
  { "OEM_6" , 0xDD },
  { "OEM_7" , 0xDE },
  { "OEM_8" , 0xDF },
  { "OEM_102" , 0xE2 },
  { "PROCESSKEY" , 0xE5 },
  { "PACKET" , 0xE7 },
  { "ATTN" , 0xF6 },
  { "CRSEL" , 0xF7 },
  { "EXSEL" , 0xF8 },
  { "EREOF" , 0xF9 },
  { "PLAY" , 0xFA },
  { "ZOOM" , 0xFB },
  { "NONAME" , 0xFC },
  { "PA1" , 0xFD },
  { "OEM_CLEAR" , 0xFE },
  {NULL, 0}
};

static const Key *VKtoKey(const DWORD vk) {
  int i = -1;

  while (Keys[++i].vk)
    if (vk == Keys[i].vk)
      return &Keys[i];      
  return NULL;
}

static const Key *StringToKey(lua_State *L, const char *key) {
  int i = -1;
  char *str = strdup(key);

  CharUpperA(str);
  while (Keys[++i].vk)
    if (strcmp(str,Keys[i].name) == 0) {
      free(str);
      return &Keys[i];      
    }
  luaL_error(L, "unknown key '%s'", str);
  return NULL;
}

static const Key *checkkey(lua_State *L, int idx) {
  int type = lua_type(L, idx);
  if (type == LUA_TSTRING)
    return StringToKey(L, lua_tostring(L, idx));
  else if (type == LUA_TNUMBER)
    return VKtoKey(luaL_checkinteger(L, idx));
  return NULL;
}

static const DWORD luaL_checkVK(lua_State *L, int idx) {
  const Key *key = checkkey(L, idx);
  return key ? key->vk : luaL_typeerror(L, 1, "virtual key code or string");
}

//------------------------------------ keyboard.char() function
LUA_METHOD(keyboard, char)
{
  int nargs = lua_gettop(L);
  DWORD vk = luaL_checkVK(L, 1);
  BYTE keys[256] = {0};
  wchar_t buff = 0;
  BOOL isdead;

  if (nargs > 1) {
    for (int i = 2; i <= nargs; i++)
      keys[luaL_checkVK(L, i)] = 0xFF;
  } else {
    keys[VK_SHIFT] = GetAsyncKeyState(VK_SHIFT) & 0x8000 ? 0xFF : 0;
    keys[VK_CONTROL] = GetAsyncKeyState(VK_CONTROL) & 0x8000 ? 0xFF : 0;
    keys[VK_MENU] = GetAsyncKeyState(VK_MENU) & 0x8000 ? 0xFF : 0;
  }
  isdead = ToUnicode(vk, 0, keys, &buff, 1, 0) < 0;
  lua_pushlwstring(L, &buff, 1);
  lua_pushboolean(L, isdead);
  return 2;
}

//------------------------------------ keyboard.ispressed() function
LUA_METHOD(keyboard, ispressed)
{
  int nargs = lua_gettop(L);
  for (int i = 1; i <= nargs; i++)
    if (!(GetAsyncKeyState(luaL_checkVK(L, i)) & 0x08000)) {
      lua_pushboolean(L, FALSE);
      return 1;
    }
  lua_pushboolean(L, TRUE);
  return 1;
}

//------------------------------------ keyboard.isup() function
LUA_METHOD(keyboard, isup)
{
  lua_pushboolean(L, !(GetKeyState(luaL_checkVK(L, 1)) & 0x8000));
  return 1;
}

//------------------------------------ keyboard.isdown() function
LUA_METHOD(keyboard, isdown)
{
  lua_pushboolean(L, GetKeyState(luaL_checkVK(L, 1)) & 0x8000);
  return 1;
}

//------------------------------------ keyboard.waitforkey() function

typedef struct {
  const Key *key;
  int funcref;
} BindKey;

static int gc_bind(lua_State *L) {
  free(lua_self(L, 1, Task)->userdata);
  return 0;
}

static int waitkey(lua_State* L, int status, lua_KContext ctx)  {
  BindKey *bk = (BindKey *)ctx;
  MSG msg;

  if (!bk->key) {
    for (int i = 0; i < 256; i++)
      if (GetKeyState(i) & 0x8000) {
        const Key *k = VKtoKey(i & 0xFF);
        if (k && ((k->vk < 0x10) || (k->vk > 0x12)) && ((k->vk < 0xA0) || (k->vk > 0xA5))) {
          lua_pushstring(L, k->name);
          return 1;
        }
      }
  } else if (GetKeyState(bk->key->vk) & 0x8000) {
    if (!bk->funcref) {
      lua_pushstring(L, bk->key->name);
      return 1;
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, bk->funcref);
      lua_pushboolean(L, GetKeyState(VK_SHIFT) & 0x8000);
      lua_pushboolean(L, GetKeyState(VK_CONTROL) & 0x8000);
      lua_call(L, 2, 0);
    }
  }
  return lua_yieldk(L, 0, ctx, waitkey);
}

//------------------------------------ keyboard.bind() function
LUA_METHOD(keyboard, bind)
{
  BYTE keys[256] = {0};
  BindKey *bk = (BindKey*)calloc(1, sizeof(BindKey));
  
  bk->key = checkkey(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_pushvalue(L, 2);
  bk->funcref = luaL_ref(L, LUA_REGISTRYINDEX);
  SetKeyboardState(keys);
  lua_pushtask(L, waitkey, bk, gc_bind);
  return 1;
}


LUA_METHOD(keyboard, waitfor)
{
  BYTE keys[256] = {0};
  SetKeyboardState(keys);
  BindKey *bk = (BindKey *)calloc(1, sizeof(BindKey));
  bk->key = checkkey(L, 1);
  lua_pushtask(L, waitkey, (void*)bk, gc_bind);
  lua_pushvalue(L, -1);
  lua_call(L, 0, 0);
  lua_wait(L, -1);
  return 1;
}

//------------------------------------ keyboard.status property
LUA_PROPERTY_GET(keyboard, status) {
  BYTE allkeys[256];

  GetKeyboardState(allkeys);
  lua_createtable(L, 256, 0);
  for (int i = 1; i < 256; i++) {
    const Key *k = VKtoKey(i & 0xFF);
    if (k) {
      lua_pushstring(L, k->name);
      lua_pushboolean(L, (allkeys[i] & 0x80));
      lua_rawset(L, -3);
    }
  }
  return 1;
}

//------------------------------------ keyboard.sendkeys()
LUA_METHOD(keyboard, sendkeys) {
  wchar_t *keys = lua_towstring(L, 1);
  lua_pushboolean(L, sk.SendKeys(keys));
  free(keys);
  return 1;
}

//------------------------------------ keyboard.keyname()
LUA_METHOD(keyboard, keyname) {
  wchar_t *ch = lua_towstring(L, 1);
  SHORT result = VkKeyScanW(ch[0]);
  const Key *key = VKtoKey(LOBYTE(result));
  
  free(ch);
  if (key) {
    lua_pushstring(L, key->name);
    lua_pushboolean(L, HIBYTE(result) & 1);
    lua_pushboolean(L, HIBYTE(result) & 2);
    lua_pushboolean(L, HIBYTE(result) & 4);
    return 4;
  }
  lua_pushnil(L);
  return 1;
}

MODULE_PROPERTIES(keyboard)
  READONLY_PROPERTY(keyboard, status)
END

MODULE_FUNCTIONS(keyboard)
  METHOD(keyboard, char)
  METHOD(keyboard, ispressed)
  METHOD(keyboard, isup)
  METHOD(keyboard, isdown)
  METHOD(keyboard, waitfor)
  METHOD(keyboard, sendkeys)
  METHOD(keyboard, keyname)
  METHOD(keyboard, bind)
END

extern "C" {

  int __declspec(dllexport) luaopen_keyboard(lua_State *L)
  {
    lua_regmodule(L, keyboard);
    return 1;
  }

}