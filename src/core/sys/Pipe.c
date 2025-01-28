/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Pipe.c | LuaRT Pipe object implementation
*/

#define LUA_LIB

#include <Pipe.h>
#include <Buffer.h>
#include <luart.h>
#include <stdlib.h>
#include <windows.h>

luart_type TPipe;

//-------------------------------------[ Pipe Constructor ]
LUA_CONSTRUCTOR(Pipe) {
	wchar_t *cmd = lua_towstring(L, 2);
	Pipe *p = (Pipe*)calloc(1, sizeof(Pipe));
	SECURITY_ATTRIBUTES sa;
	STARTUPINFOW si = {0};

	//---- initialize pipe
	sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
	CreatePipe(&p->out_read, &p->out_write, &sa, 0);
	SetHandleInformation(&p->out_read, HANDLE_FLAG_INHERIT, 0);
	CreatePipe(&p->in_read, &p->in_write, &sa, 0);
	SetHandleInformation(&p->in_read, HANDLE_FLAG_INHERIT, 0);
	CreatePipe(&p->err_read, &p->err_write, &sa, 0);
	SetHandleInformation(&p->err_read, HANDLE_FLAG_INHERIT, 0);

	//---- start process
	si.cb = sizeof(STARTUPINFOW);
    si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = p->in_read;
    si.hStdError = p->out_write;
    si.hStdOutput = p->out_write;
	if (CreateProcessW(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &p->pi) == FALSE) {
		free(p);
		lua_pushnil(L);
	}
	else
		lua_newinstance(L, p, Pipe);
	free(cmd);
	return 1;
}

//-------------------------------------[ Pipe.write() ]
LUA_METHOD(Pipe, write) {
	Pipe *p = lua_self(L, 1, Pipe);
	DWORD written, done = 0;
	size_t len;
	const char *str = luaL_tolstring(L, 2, &len);

	if (p->in_write) {
		while(done < len) {
			if (!WriteFile( p->in_write, str+done, len-done, &written, NULL))
				break;
			done += written;
		}
	}
	return 0;
}

//-------------------------------------[ Pipe.read() ]
static int pipe_read(lua_State *L, HANDLE h) {
	DWORD read, done, avail = 1;
	char *buff;

	while (PeekNamedPipe(h, NULL, 0, NULL, &avail, NULL) && avail) {	
		buff = calloc(1, avail+1);
		done = 0;
		while (!ReadFile(h, buff+done, avail-done, &read, NULL) || read == 0)
			done += read;
		int size = MultiByteToWideChar(CP_OEMCP, 0, buff, avail, NULL, 0);
		wchar_t *newbuff = (wchar_t *)malloc(size*sizeof(wchar_t));
		MultiByteToWideChar(CP_OEMCP, 0, buff, avail, newbuff, size);
		lua_pushlwstring(L, newbuff, avail);
		free(newbuff);			
		free(buff);
		return 1;
	}
	return 0;
}

static int PipeReadTaskContinue(lua_State* L, int status, lua_KContext ctx) {	
	Pipe *p = (Pipe*)ctx;
	int count;

	if (!p->out_read)
		return 0;
	count = pipe_read(L, p->out_read);
	count += pipe_read(L, p->err_read);
    return count ? count : lua_yieldk(L, 0, ctx, PipeReadTaskContinue);
}

LUA_METHOD(Pipe, read) {
	Sleep(luaL_optinteger(L, 2, 100));
	lua_pushtask(L, PipeReadTaskContinue, lua_self(L, 1, Pipe), NULL);
	return 1;
}

//-------------------------------------[ Pipe.close() ]
LUA_METHOD(Pipe, close) {
	Pipe *p = lua_self(L, 1, Pipe);
	TerminateProcess(p->pi.hProcess, -1);
	CloseHandle(p->out_write);
	CloseHandle(p->out_read);
	CloseHandle(p->in_write);
	CloseHandle(p->in_read);
	CloseHandle(p->err_write);
	CloseHandle(p->err_read);
	CloseHandle(p->pi.hProcess);
	CloseHandle(p->pi.hThread);
	memset(p, 0, sizeof(Pipe));
	return 0;
}

LUA_METHOD(Pipe, __gc) {
	Pipe_close(L);
	free(lua_self(L, 1, Pipe));
	return 0;
}

const luaL_Reg Pipe_metafields[] = {
	{"__gc", Pipe___gc},
	{NULL, NULL}
};

const luaL_Reg Pipe_methods[] = {
	{"write",		Pipe_write},
	{"read",		Pipe_read},
	{"close",		Pipe_close},
	{NULL, NULL}
};