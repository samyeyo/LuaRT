 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Gradient.cpp | LuaRT Canvas Gradient objects implementation
*/

#include <luart.h>
#include <Widget.h>
#include <File.h>

#include <vector>
#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif
#include "Direct2D.h"
#include "Gradient.h"
#include "Canvas.h"

luart_type TLinearGradient, TRadialGradient;


static void table_togradient(lua_State *L, int idx, LinearGradient *g, BOOL isLinear) {    
    D2D1_GRADIENT_STOP stop;
    UINT32 rgba;
    double dpi;
    
    lua_uigetinfo(&dpi, NULL);
    lua_pushnil(L);
	while (lua_next(L, idx)) {
		if (lua_type(L, -2) != LUA_TNUMBER)
			luaL_error(L, "invalid gradient table (number expected found %s)", luaL_typename(L, -1));
        stop.position = (float)(lua_tonumber(L, -2));
        rgba = (UINT32) lua_tointeger(L, -1);
        stop.color = D2D1::ColorF(GetR(rgba)/255, GetG(rgba)/255, GetB(rgba)/255, GetA(rgba)/255);		
        g->stops.push_back(stop);
		lua_pop(L, 1);
	}
    g->d->DCRender->CreateGradientStopCollection(g->stops.data(), g->stops.size(), &g->collection);    
}

//-------------------------------------[ LinearGradient Constructor ]
LUA_CONSTRUCTOR(LinearGradient) {
	LinearGradient *g = new LinearGradient();
    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};
    
    g->d = (Direct2D *)lua_touserdata(L, 2);
    table_togradient(L, 3, g, TRUE);
    g->d->DCRender->CreateLinearGradientBrush(props, g->collection, &g->linear);
    g->d->gradients.push_back(g);
	lua_newinstance(L, g, LinearGradient);
	return 1;
}

//-------------------------------------[ RadialGradient Constructor ]
LUA_CONSTRUCTOR(RadialGradient) {
	RadialGradient *g = (RadialGradient *)calloc(1, sizeof(RadialGradient));
    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    
    g->d = (Direct2D *)lua_touserdata(L, 2);
    table_togradient(L, 3, (LinearGradient *)g, FALSE);
    g->d->DCRender->CreateRadialGradientBrush(props, g->collection, &g->radial);
    g->d->gradients.push_back((LinearGradient*)g);
	lua_newinstance(L, g, RadialGradient);
	return 1;
}

static D2D1_POINT_2F table_topoint(lua_State *L, int idx) {
    double dpi;
    lua_uigetinfo(&dpi, NULL);
    luaL_checktype(L, idx, LUA_TTABLE);
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    return D2D1::Point2F(static_cast<FLOAT>(lua_tonumber(L, -2)*dpi), static_cast<FLOAT>(lua_tonumber(L, -1)*dpi));
}

static void point_totable(lua_State *L, D2D1_POINT_2F p) {
    double dpi;
    lua_uigetinfo(&dpi, NULL);
    lua_createtable(L, 0, 2);
    lua_pushnumber(L, floor(p.x/dpi));
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, floor(p.y/dpi));
    lua_setfield(L, -2, "y");
}

LUA_PROPERTY_SET(LinearGradient, start) {
    lua_self(L, 1, LinearGradient)->linear->SetStartPoint(table_topoint(L, 2));
    return 0;
}

LUA_PROPERTY_GET(LinearGradient, start) {
    point_totable(L, lua_self(L, 1, LinearGradient)->linear->GetStartPoint());
    return 1;
}

LUA_PROPERTY_SET(LinearGradient, stop) {
    lua_self(L, 1, LinearGradient)->linear->SetEndPoint(table_topoint(L, 2));
    return 0;
}

LUA_PROPERTY_GET(LinearGradient, stop) {
    point_totable(L, lua_self(L, 1, LinearGradient)->linear->GetEndPoint());
    return 1;
}

LUA_PROPERTY_SET(LinearGradient, opacity) {
    lua_self(L, 1, LinearGradient)->linear->SetOpacity(luaL_checknumber(L, 2));
    return 0;
}

LUA_PROPERTY_GET(LinearGradient, opacity) {
    lua_pushnumber(L, lua_self(L, 1, LinearGradient)->linear->GetOpacity());
    return 1;
}

LUA_PROPERTY_SET(RadialGradient, radius) {
    RadialGradient *g = lua_self(L, 1, RadialGradient);
    D2D1_POINT_2F p = table_topoint(L, 2);

    g->radial->SetRadiusX(p.x);
    g->radial->SetRadiusY(p.y);
    return 0;
}

LUA_PROPERTY_GET(RadialGradient, radius) {
    RadialGradient *g = lua_self(L, 1, RadialGradient);
    point_totable(L, D2D1::Point2F(g->radial->GetRadiusX(), g->radial->GetRadiusY()));
    return 1;
}

LUA_PROPERTY_SET(RadialGradient, center) {
    lua_self(L, 1, RadialGradient)->radial->SetCenter(table_topoint(L, 2));
    return 0;
}

LUA_PROPERTY_GET(RadialGradient, center) {
    point_totable(L, lua_self(L, 1, RadialGradient)->radial->GetCenter());
    return 1;
}

LUA_PROPERTY_SET(RadialGradient, opacity) {
    lua_self(L, 1, RadialGradient)->radial->SetOpacity(luaL_checknumber(L, 2));
    return 0;
}

LUA_PROPERTY_GET(RadialGradient, opacity) {
    lua_pushnumber(L, lua_self(L, 1, RadialGradient)->radial->GetOpacity());
    return 1;
}

OBJECT_MEMBERS(LinearGradient)
    READWRITE_PROPERTY(LinearGradient, start)
    READWRITE_PROPERTY(LinearGradient, stop)
    READWRITE_PROPERTY(LinearGradient, opacity)
END

LUA_METHOD(LinearGradient, __gc) {
    LinearGradient *g = lua_self(L, 1, LinearGradient);
    g->linear->Release();
    g->d->gradients.erase(std::remove(g->d->gradients.begin(), g->d->gradients.end(), g), g->d->gradients.end());
    delete g;
    return 0;
}

OBJECT_METAFIELDS(LinearGradient)
    METHOD(LinearGradient, __gc)
END

OBJECT_MEMBERS(RadialGradient)
    READWRITE_PROPERTY(RadialGradient, radius)
    READWRITE_PROPERTY(RadialGradient, center)
    READWRITE_PROPERTY(RadialGradient, opacity)
END

LUA_METHOD(RadialGradient, __gc) {
    RadialGradient *g = lua_self(L, 1, RadialGradient);
    g->radial->Release();
    delete g;
    return 0;
}

OBJECT_METAFIELDS(RadialGradient)
    METHOD(RadialGradient, __gc)
END
