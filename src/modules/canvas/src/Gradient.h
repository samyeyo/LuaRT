 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Gradient.h | LuaRT Gradient object header
*/

#pragma once

#include <luart.h>
#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif

#include "Direct2D.h"

//---------------- Linear Gradient object

typedef struct {
    luart_type  type;
    Direct2D    *d;
	ID2D1LinearGradientBrush *linear;
} LinearGradient;

extern luart_type TLinearGradient;

LUA_CONSTRUCTOR(LinearGradient);
extern const luaL_Reg LinearGradient_methods[];
extern const luaL_Reg LinearGradient_metafields[];

//---------------- Radial Gradient object

typedef struct {
    luart_type  type;
    Direct2D    *d;
	ID2D1RadialGradientBrush *radial;
} RadialGradient;

extern luart_type TRadialGradient;

LUA_CONSTRUCTOR(RadialGradient);
extern const luaL_Reg RadialGradient_methods[];
extern const luaL_Reg RadialGradient_metafields[];