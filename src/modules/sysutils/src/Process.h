/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Process.h | LuaRT Process object header
*/

#include <luart.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Process object
typedef struct {
	luart_type	type;
	HANDLE	out_write;
	HANDLE	out_read;
	HANDLE	in_write;
	HANDLE	in_read;
	HANDLE	err_write;
	HANDLE	err_read;
	PROCESS_INFORMATION pi;
	HANDLE hIO;
	HANDLE hJob;
	BOOL suspended;
} Process;

extern luart_type TProcess;

LUA_CONSTRUCTOR(Process);
extern const luaL_Reg Process_methods[];
extern const luaL_Reg Process_metafields[];

#ifdef __cplusplus
}
#endif