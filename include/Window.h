/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Window.h | LuaRT Window object header file
*/

#pragma once
#include <luart.h>

#ifdef __cplusplus
extern "C" {
#endif

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LUA_CONSTRUCTOR(Window);

extern luart_type TWindow;

extern luaL_Reg Window_methods[];
extern luaL_Reg Window_metafields[];

#ifdef __cplusplus
}
#endif