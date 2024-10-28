/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | async.h | LuaRT async functions header
*/


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    #include <luart.h>
    #include <Task.h>

    Task *create_task(lua_State *L);

    //-------- Start a created Task
    int start_task(lua_State *L, Task *t, int nargs);

    //-------- Search for the current running Task
    LUA_API Task *search_task(lua_State *L);

    //-------- Close a Task
    #define close_task(t) (t)->status = TTerminated

    //-------- Resume a Task
    int resume_task(lua_State *L, Task *t, int args);

    //-------- Task scheduler
    int update_tasks(lua_State *L);

    //-------- Wait for a Task at specific index
    int waitfor_task(lua_State *L, int idx);

    //-------- Wait for all tasks
    int waitall_tasks(lua_State *L);


#ifdef __cplusplus
}
#endif