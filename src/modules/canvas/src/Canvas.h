/*
 | Canvas for LuaRT
 | Luart.org, Copyright (c) Tine Samir 2023.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Canvas.h | LuaRT binary module 
*/

#include "d2d1.h"
#include <dwrite.h>

#define GetA(c)((float)((c) & 0xff))
#define GetB(c)((float)((c >> 8) & 0xff))
#define GetG(c)((float)((c >> 16) & 0xff))
#define GetR(c)((float)((c >> 24) & 0xff))

#define RGBA(r,g,b,a) ((r << 24) | (g << 16) | (b << 8) | a)


extern luart_type TCanvas;

LUA_CONSTRUCTOR(Canvas);
extern const luaL_Reg Canvas_methods[];
extern const luaL_Reg Canvas_metafields[];

void register_canvas(lua_State *L);