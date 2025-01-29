 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Image.h | LuaRT Image object header
*/

#pragma once

#include <luart.h>
#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif

#include "Direct2D.h"

//---------------- Image object

typedef struct {
    luart_type  type;
    Direct2D    *d;
	ID2D1Bitmap *Bitmap;
    IWICBitmapDecoder	  *IWICDecoder;
    IWICBitmapFrameDecode *IWICFrame;
    IWICFormatConverter	  *IWICConverter;
} Image;

extern luart_type TImage;

LUA_CONSTRUCTOR(Image);
extern const luaL_Reg Image_methods[];
extern const luaL_Reg Image_metafields[];