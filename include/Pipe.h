/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Pipe.h | LuaRT Pipe object header
*/

#include <luart.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Pipe object
typedef struct {
	luart_type	type;
	HANDLE	out_write;
	HANDLE	out_read;
	HANDLE	in_write;
	HANDLE	in_read;
	HANDLE	err_write;
	HANDLE	err_read;
	PROCESS_INFORMATION pi;
	BOOL	echo;
} Pipe;

extern luart_type TPipe;

LUA_CONSTRUCTOR(Pipe);
extern const luaL_Reg Pipe_methods[];
extern const luaL_Reg Pipe_metafields[];

#ifdef __cplusplus
}
#endif