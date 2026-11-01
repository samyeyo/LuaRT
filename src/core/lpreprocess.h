#pragma once

#include <luart.h>


int lua_preload_searcher(lua_State* L);
char* preprocess_lua(lua_State *L, const char* src);
char* preprocess_file(lua_State *L, const char* filename);