/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Process.c | LuaRT Process object implementation
*/

#define LUA_LIB

#include "Process.h"
#include <Buffer.h>
#include <luart.h>
#include <stdlib.h>
#include <windows.h>

luart_type TProcess;

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

static int ProcessTaskContinue(lua_State* L, int status, lua_KContext ctx) {
	Process *p = (Process *)ctx;
	DWORD CompletionCode;
 	ULONG_PTR CompletionKey;
 	LPOVERLAPPED Overlapped;
	
	 if (GetQueuedCompletionStatus(p->hIO, &CompletionCode, &CompletionKey, &Overlapped, 0) && ((HANDLE)CompletionKey == p->hJob) && (CompletionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)) {
		GetExitCodeProcess(p->pi.hProcess,&p->pi.dwProcessId);
		lua_pushboolean(L, p->pi.dwProcessId == EXIT_SUCCESS);
		return 1;
	}
	if (pipe_read(L, p->out_read)) {
		if (lua_getfield(L, lua_upvalueindex(2), "stdout") == LUA_TFUNCTION) {
			lua_pushvalue(L, lua_upvalueindex(2));
			lua_insert(L, -2);
			lua_call(L, 2, LUA_MULTRET);
		}
	}
	if (p->err_read && pipe_read(L, p->err_read)) {
		if (lua_getfield(L, lua_upvalueindex(2), "stderr") == LUA_TFUNCTION) {
			lua_pushvalue(L, lua_upvalueindex(2));
			lua_insert(L, -2);
			lua_call(L, 2, LUA_MULTRET);
		}
	}
	return lua_yieldk(L, 0, ctx, ProcessTaskContinue);
}

static int WaitProcessTask(lua_State *L) {
	return lua_yieldk(L, 0, (lua_KContext)lua_touserdata(L, lua_upvalueindex(1)), ProcessTaskContinue);
}


//-------------------------------------[ Process Constructor ]
LUA_CONSTRUCTOR(Process) {
	int len;
	wchar_t *exec = lua_tolwstring(L, 2, &len);
	size_t count = sizeof(wchar_t)*(30+len);
	wchar_t *buff = malloc(count);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION job = {0};
	Process *p = (Process*)calloc(1, sizeof(Process));
	SECURITY_ATTRIBUTES sa;
	STARTUPINFOW si = {0};

	sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    si.cb = sizeof(STARTUPINFOW);
	si.wShowWindow = lua_toboolean(L, 3) ? SW_SHOW : SW_HIDE;
    si.dwFlags |= STARTF_USESHOWWINDOW;

	if (lua_toboolean(L, 4)) {
		CreatePipe(&p->out_read, &p->out_write, &sa, 0);
		SetHandleInformation(&p->out_read, HANDLE_FLAG_INHERIT, 0);
		CreatePipe(&p->in_read, &p->in_write, &sa, 0);
		SetHandleInformation(&p->in_read, HANDLE_FLAG_INHERIT, 0);
		p->err_write = p->out_write;
		if (lua_toboolean(L, 5)) {
			CreatePipe(&p->err_read, &p->err_write, &sa, 0);
			SetHandleInformation(&p->err_read, HANDLE_FLAG_INHERIT, 0);
		}
		si.dwFlags |= STARTF_USESTDHANDLES;
		si.hStdInput = p->in_read;
		si.hStdError = p->err_write;
		si.hStdOutput = p->out_write;
	}
	p->hJob = CreateJobObject(NULL, NULL);
	job.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(p->hJob, JobObjectExtendedLimitInformation, &job, sizeof(job));

	_snwprintf(buff, count, L"%s", exec);
    if (CreateProcessW(NULL, buff, NULL, NULL, TRUE, si.wShowWindow == SW_HIDE ? CREATE_NO_WINDOW | CREATE_SUSPENDED : CREATE_SUSPENDED, NULL, NULL, &si, &p->pi)) {
		AssignProcessToJobObject(p->hJob, p->pi.hProcess);
		p->hIO = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		JOBOBJECT_ASSOCIATE_COMPLETION_PORT Port;
 		Port.CompletionKey = p->hJob;
 		Port.CompletionPort = p->hIO;
 		SetInformationJobObject(p->hJob, JobObjectAssociateCompletionPortInformation, &Port, sizeof(Port));
		p->suspended = TRUE;
		lua_newinstance(L, p, Process);
	} else {
		lua_pushnil(L);
		CloseHandle(p->hJob);
	}
	free(buff);
	free(exec);
	return 1;
}

//-------------------------------------[ Process.start() ]
LUA_METHOD(Process, start) {
	Process *p = lua_self(L, 1, Process);
	lua_pushlightuserdata(L, p);
	lua_pushvalue(L, 1);
	lua_pushcclosure(L, WaitProcessTask, 2);
	lua_pushinstance(L, Task, 1);
	lua_pushvalue(L, -1);
	lua_call(L, 0, 0);
	ResumeThread(p->pi.hThread);
	p->suspended = FALSE;
	return 1;
}

//-------------------------------------[ Process.write() ]
LUA_METHOD(Process, write) {
	Process *p = lua_self(L, 1, Process);
	DWORD written, done = 0;
	int len;
	luaL_tolstring(L, 2, NULL);
	
	wchar_t *str = lua_tolwstring(L, -1, &len);
	int size = WideCharToMultiByte(CP_OEMCP, 0, str, len, NULL, 0, NULL, NULL);
	char *buff = calloc(1, size);
	WideCharToMultiByte(CP_OEMCP, 0, str, len, buff, size, NULL, NULL);
	if (p->in_write) {
		while(done < size) {
			if (!WriteFile( p->in_write, buff+done, size-done, &written, NULL))
				break;
			done += written;
		}
	}
	free(buff);
	free(str);
	return 0;
}

//-------------------------------------[ Process.suspend() ]
LUA_METHOD(Process, suspend) {
	Process *p = lua_self(L, 1, Process);
	if (p->pi.hProcess) {
		SuspendThread(p->pi.hThread);
		p->suspended = TRUE;
	}
	return 0;
}

//-------------------------------------[ Process.resume() ]
LUA_METHOD(Process, resume) {
	Process *p = lua_self(L, 1, Process);
	if (p->pi.hProcess) {
		ResumeThread(p->pi.hThread);
		p->suspended = FALSE;
	}
	return 0;
}

//-------------------------------------[ Process.suspended ]
LUA_PROPERTY_GET(Process, suspended) {
	lua_pushboolean(L, (int)lua_self(L, 1, Process)->suspended);
	return 1;
}

//-------------------------------------[ Process.terminated ]
LUA_PROPERTY_GET(Process, terminated) {
	lua_pushboolean(L, lua_self(L, 1, Process)->pi.hProcess == NULL);
	return 1;
}

//-------------------------------------[ Process.close() ]
LUA_METHOD(Process, abort) {
	Process *p = lua_self(L, 1, Process);
	if (p->pi.hProcess) {
		CloseHandle(p->out_write);
		CloseHandle(p->out_read);
		CloseHandle(p->in_write);
		CloseHandle(p->in_read);
		if (p->err_read) {
			CloseHandle(p->err_write);
			CloseHandle(p->err_read);
		}
		CloseHandle(p->pi.hProcess);
		CloseHandle(p->pi.hThread);
		TerminateProcess(p->pi.hProcess, -1);
	}
	memset(p, 0, sizeof(Process));
	return 0;
}

LUA_METHOD(Process, __gc) {
	Process_abort(L);
	free(lua_self(L, 1, Process));
	return 0;
}

OBJECT_METAFIELDS(Process)
	METHOD(Process, __gc)
END

OBJECT_MEMBERS(Process)
	METHOD(Process, write)
	METHOD(Process, abort)
	METHOD(Process, suspend)
	METHOD(Process, resume)
	METHOD(Process, start)
	READONLY_PROPERTY(Process, suspended)
	READONLY_PROPERTY(Process, terminated)
END
