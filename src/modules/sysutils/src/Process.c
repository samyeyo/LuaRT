/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
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
#include <tlhelp32.h>

luart_type TProcess;

static int abort_process(Process *p) {
	BOOL result = TRUE;
	if (p && p->pi.hProcess) {
		CloseHandle(p->out_write);
		CloseHandle(p->out_read);
		CloseHandle(p->in_write);
		CloseHandle(p->in_read);
		if (!p->detached) {
			CloseHandle(p->pi.hProcess);
			p->pi.hProcess = NULL;
			CloseHandle(p->pi.hThread);
			result = TerminateJobObject(p->hJob, -1);
		} 
	}
	memset(p, 0, sizeof(Process));
	return result;
}

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

LUA_PROPERTY_SET(Process, onRedirect) {
	Process *p = lua_self(L, 1, Process);
	int t = lua_type(L, 2);

	if (p->funcref)
		luaL_unref(L, LUA_REGISTRYINDEX, p->funcref);
	if (t == LUA_TNIL)
		p->funcref = 0;
	else if (t == LUA_TFUNCTION) {
		lua_pushvalue(L, 2);
		p->funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	} else luaL_argerror(L, 2, "function or nil");
	return 0;
}

LUA_PROPERTY_GET(Process, onRedirect) {
	Process *p = lua_self(L, 1, Process);
	if (p->funcref > 0)
		lua_rawgeti(L, LUA_REGISTRYINDEX, p->funcref);
	else lua_pushnil(L);
	return 1;
}

static int ProcessTaskContinue(lua_State* L, int status, lua_KContext ctx) {
	Process *p =  (Process *)ctx;
	DWORD CompletionCode;
 	ULONG_PTR CompletionKey;
 	LPOVERLAPPED Overlapped;

	if (GetQueuedCompletionStatus(p->hIO, &CompletionCode, &CompletionKey, &Overlapped, 0) && ((HANDLE)CompletionKey == p->hJob) && (CompletionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)) {
		abort_process(p);
		GetExitCodeProcess(p->pi.hProcess,&p->pi.dwProcessId);
		lua_pushboolean(L, p->pi.dwProcessId == EXIT_SUCCESS);
		return 1;
	}
	if (p->funcref && pipe_read(L, p->out_read)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, p->funcref);
		lua_rawgeti(L, LUA_REGISTRYINDEX, p->ref);
		lua_pushvalue(L, -3);
		lua_pushinstance(L, Buffer, 1);
		lua_remove(L, -2);
		if (lua_pcall(L, 2, LUA_MULTRET, 0))
			lua_error(L);
	}
	return lua_yieldk(L, 0, ctx, ProcessTaskContinue);
}

//-------------------------------------[ Process Constructor ]
LUA_CONSTRUCTOR(Process) {
	int len;
	wchar_t *exec = lua_tolwstring(L, 2, &len);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION job = {0};
	Process *p = (Process*)calloc(1, sizeof(Process));
	SECURITY_ATTRIBUTES sa;
	STARTUPINFOW si = {0};
	int n = lua_gettop(L);

	sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    si.cb = sizeof(STARTUPINFOW);
	si.wShowWindow = (n > 2 && lua_toboolean(L, 3)) ? SW_HIDE : SW_SHOW;
    si.dwFlags |= STARTF_USESHOWWINDOW;

	if (n > 3 && lua_toboolean(L, 4)) {
		CreatePipe(&p->out_read, &p->out_write, &sa, 0);
		SetHandleInformation(&p->out_read, HANDLE_FLAG_INHERIT, 0);
		CreatePipe(&p->in_read, &p->in_write, &sa, 0);
		SetHandleInformation(&p->in_read, HANDLE_FLAG_INHERIT, 0);
		p->err_write = p->out_write;
		si.dwFlags |= STARTF_USESTDHANDLES;
		si.hStdInput = p->in_read;
		si.hStdError = p->err_write;
		si.hStdOutput = p->out_write;
	}
	p->hJob = CreateJobObject(NULL, NULL);
	p->detached = n > 4 && lua_toboolean(L, 5) ? CREATE_BREAKAWAY_FROM_JOB : 0;
	job.BasicLimitInformation.LimitFlags = p->detached ? JOB_OBJECT_LIMIT_BREAKAWAY_OK : JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(p->hJob, JobObjectExtendedLimitInformation, &job, sizeof(job));

    if (CreateProcessW(NULL, exec, NULL, NULL, TRUE, si.wShowWindow == SW_HIDE ? CREATE_NO_WINDOW | CREATE_SUSPENDED | p->detached : CREATE_SUSPENDED | p->detached, NULL, NULL, &si, &p->pi)) {
		AssignProcessToJobObject(p->hJob, p->pi.hProcess);
		p->hIO = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		JOBOBJECT_ASSOCIATE_COMPLETION_PORT Port;
 		Port.CompletionKey = p->hJob;
 		Port.CompletionPort = p->hIO;
 		SetInformationJobObject(p->hJob, JobObjectAssociateCompletionPortInformation, &Port, sizeof(Port));
		p->suspended = TRUE;
		lua_newinstance(L, p, Process);
		lua_pushvalue(L, -1);
		p->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		lua_pushnil(L);
		CloseHandle(p->hJob);
	}
	free(exec);
	return 1;
}

//-------------------------------------[ Process.spawn() ]
LUA_METHOD(Process, spawn) {
	Process *p = lua_self(L, 1, Process);
	p->suspended = lua_toboolean(L, 2);
	lua_pushtask(L, ProcessTaskContinue, p, NULL);
	lua_pushvalue(L, -1);
	lua_call(L, 0, 0);
	if (!p->suspended)
		ResumeThread(p->pi.hThread);
	return 1;
}

//-------------------------------------[ Process.write() ]
LUA_METHOD(Process, write) {
	Process *p = lua_self(L, 1, Process);
	DWORD written, done = 0;
	int len;
	
	if (p->in_write) {
		luaL_tolstring(L, 2, NULL);
		wchar_t *str = lua_tolwstring(L, -1, &len);
		int size = WideCharToMultiByte(CP_OEMCP, 0, str, len, NULL, 0, NULL, NULL);
		char *buff = calloc(1, size);
		WideCharToMultiByte(CP_OEMCP, 0, str, len, buff, size, NULL, NULL);
			while(done < size) {
				if (!WriteFile( p->in_write, buff+done, size-done, &written, NULL))
					break;
				done += written;
			}
		free(buff);
		free(str);
	}
	return 0;
}

//-------------------------------------[ Process.close() ]
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD windowProcessID;
    GetWindowThreadProcessId(hwnd, &windowProcessID);
    if (windowProcessID == (DWORD)lParam) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        return FALSE; 
    }
    return TRUE;
}

LUA_METHOD(Process, close) {
	lua_pushboolean(L, !EnumWindows(EnumWindowsProc, (LPARAM)lua_self(L, 1, Process)->pi.dwProcessId));
    return 1;
}

//-------------------------------------[ Process.suspend() ]
BOOL AllThreads(Process *p, BOOL suspend) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
	BOOL result = FALSE;

    if (hSnapshot != INVALID_HANDLE_VALUE) {
		if (Thread32First(hSnapshot, &te32)) {
			do {
				if (te32.th32OwnerProcessID == p->pi.dwProcessId) {
					HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
					result = hThread == NULL;
					if (!result) {
						result = suspend ? SuspendThread(hThread) != (DWORD)-1 : ResumeThread(hThread) != (DWORD)-1;
						CloseHandle(hThread);
						if (!result)
							break;
					} else break;
				}
			} while (Thread32Next(hSnapshot, &te32));			
		}
		CloseHandle(hSnapshot);
	}
	return result;
}

LUA_METHOD(Process, suspend) {
	Process *p = lua_self(L, 1, Process);
	if (p->pi.hProcess && !p->suspended)
		if (AllThreads(p, TRUE)) {
			p->suspended = TRUE;
			lua_pushboolean(L, TRUE);
			return 1;
		}
	lua_pushboolean(L, FALSE);
	return 1;
}

//-------------------------------------[ Process.resume() ]
LUA_METHOD(Process, resume) {
	Process *p = lua_self(L, 1, Process);
	if (p->pi.hProcess && p->suspended)
		if (AllThreads(p, FALSE)) {
			p->suspended = FALSE;
			lua_pushboolean(L, TRUE);
			return 1;
		}
	lua_pushboolean(L, FALSE);
	return 1;
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
	lua_pushboolean(L, abort_process(lua_self(L, 1, Process)));
	return 1;
}

LUA_METHOD(Process, __gc) {
	Process *p = lua_self(L, 1, Process);
	abort_process(p);
	if (p->funcref)
		luaL_unref(L, LUA_REGISTRYINDEX, p->funcref);
	luaL_unref(L, LUA_REGISTRYINDEX, p->ref);
	free(p);
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
	METHOD(Process, spawn)
	METHOD(Process, close)
	READONLY_PROPERTY(Process, suspended)
	READONLY_PROPERTY(Process, terminated)
	READWRITE_PROPERTY(Process, onRedirect)
END
