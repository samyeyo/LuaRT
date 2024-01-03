/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Task.h | LuaRT Task object header
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <luart.h>
#include <windows.h>

typedef struct _Task Task;

typedef enum { TRunning, TCreated, TSleep, TWaiting, TTerminated } TaskStatus;

typedef struct _Task {
	luart_type	type;
	lua_State 	*L;
	Task		*from;
	TaskStatus	status;
	int			ref;
	int 		taskref;
	Task 		*waiting;
	BOOL		isevent;
	ULONGLONG	sleep;
 } Task;

extern luart_type TTask;

//---------------------------------------- Task object
LUA_CONSTRUCTOR(Task);
extern const luaL_Reg Task_methods[];
extern const luaL_Reg Task_metafields[];

#ifdef __cplusplus
}
#endif