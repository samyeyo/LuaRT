/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Ftp.h | LuaRT Ftp object header
*/

#pragma once

#include <luart.h>
#include <Http.h>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>

//---------------------------------------- Ftp type

extern luart_type TFtp;

LUA_CONSTRUCTOR(Ftp);
extern const luaL_Reg Ftp_methods[];
extern const luaL_Reg Ftp_metafields[];
typedef Http Ftp;