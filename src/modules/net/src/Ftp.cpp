/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Ftp.cpp | LuaRT Ftp object implementation
*/



#include <luart.h>
#include <File.h>
#include <stdio.h>
#include <string>
#include <wininet.h>
#include <Ftp.h>

luart_type TFtp;

static asyncTask *newFTPTask(Ftp *f) {
    asyncTask *task = new asyncTask;
    task->ftp = f;
    return task;
}

static int gc_asyncTask(lua_State *L) {
    asyncTask *t = (asyncTask*)lua_self(L, 1, Task)->userdata;
    CloseHandle(t->thread);
    free(t->str1);
    free(t->str2);
    free(t);
    return 0;
}

static int FtpTaskContinue(lua_State* L, int status, lua_KContext ctx) {
    asyncTask *t = (asyncTask*)ctx;

    if ( WaitForSingleObject(t->thread, 0) == WAIT_OBJECT_0) {
        TaskResult result;
        GetExitCodeThread(t->thread, (LPDWORD)&result); 
        switch(result) {  
            case RString:   lua_pushstring(L, t->received.c_str()); break;
            case RFile:     lua_pushwstring(L, t->str2);
                            lua_pushinstance(L, File, 1);
                            break;
            case RError:    SetLastError(t->error);
            default:        lua_pushboolean(L, result == RBoolean ? t->boolean : (int)result);     
        };
        CloseHandle(t->thread);
        free(t->str1);
        free(t->str2);
        free(t);
        return 1;
    }
    return lua_yieldk(L, 0, ctx, FtpTaskContinue);
}

static void push_waitTask(lua_State *L, asyncTask *t, LPTHREAD_START_ROUTINE thread) {
    lua_pushtask(L, FtpTaskContinue, t, gc_asyncTask);
    lua_pushvalue(L, -1);
    if ((t->thread = CreateThread(NULL, 0, thread, t, 0, NULL)))
        lua_call(L, 0, 0); 
    else {
        luaL_getlasterror(L, GetLastError());
        luaL_error(L, "async error : %s", lua_tostring(L, -1));
    }   
}

static void log(Ftp *f) {
    char *msg;
    DWORD info, len;

    InternetGetLastResponseInfoA( &info, NULL, &len );
    msg = (char*)calloc(len+1, sizeof(char));
    InternetGetLastResponseInfoA( &info, msg, &len );
    f->log = msg;
    free(msg);  
}

//----------------------------------[ Ftp() constructor ]
LUA_CONSTRUCTOR(Ftp) {
    Ftp *f = new Ftp();
    URL_COMPONENTSA urlcomps;
	CHAR buf0[256], buf1[256], buf2[256], buf3[256], buf4[1024], buf5[1024];
    
    ZeroMemory(&urlcomps, sizeof(URL_COMPONENTSA));
    f->host = luaL_checkstring(L, 2);
	urlcomps.dwStructSize = sizeof(urlcomps);
	urlcomps.lpszScheme = buf0;
	urlcomps.dwSchemeLength = sizeof(buf0);
	urlcomps.lpszHostName = buf1;
	urlcomps.dwHostNameLength = sizeof(buf1);
	urlcomps.lpszUserName = buf2;
	urlcomps.dwUserNameLength = sizeof(buf2);
	urlcomps.lpszPassword = buf3;
	urlcomps.dwPasswordLength = sizeof(buf3);
	urlcomps.lpszUrlPath = buf4;
	urlcomps.dwUrlPathLength = sizeof(buf4);
	urlcomps.lpszExtraInfo = buf5;
	urlcomps.dwExtraInfoLength = sizeof(buf5);
	InternetCrackUrlA(f->host.c_str(), (DWORD)strlen(f->host.c_str()), 0, &urlcomps);
    if (!strlen(urlcomps.lpszUserName) && (lua_gettop(L) > 3)) {
        urlcomps.lpszUserName = (LPSTR)luaL_checkstring(L, 3);
        urlcomps.lpszPassword = (LPSTR)luaL_checkstring(L, 4);
    }
    f->port = urlcomps.nPort;
    f->handle = InternetOpen(f->useragent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (f->handle) {
        lua_newinstance(L, f, Ftp);
        if ( (f->ftp = InternetConnectA(f->handle,*urlcomps.lpszHostName ? urlcomps.lpszHostName : f->host.c_str(), urlcomps.nPort, urlcomps.lpszUserName, urlcomps.lpszPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_PASSIVE, 0)) )
            return 1;
    }
    luaL_getlasterror(L, GetLastError());
    return lua_error(L);
}

//--------------------------------- [ Ftp.download()]
static DWORD __stdcall Ftp_download(LPVOID data) {
    asyncTask *t = (asyncTask*)data;
    bool result;

    result = FtpGetFileW(t->ftp->ftp, t->str1, t->str2, false, FILE_ATTRIBUTE_NORMAL, INTERNET_FLAG_NEED_FILE | FTP_TRANSFER_TYPE_BINARY, 0);
    t->error = GetLastError();
    log(t->ftp);
    return result ? RFile : RError;
}

LUA_METHOD(Ftp, download) {
	Ftp *f = lua_self(L, 1, Ftp);
    asyncTask *t = newFTPTask(f);
    t->str1 = lua_towstring(L, 2);
    t->str2 = lua_gettop(L) == 3 ? lua_towstring(L, 3) : wcsdup(PathFindFileNameW(t->str1)); 

    push_waitTask(L, t, Ftp_download);
	return 1;
}

//----------------------------------[ Ftp.upload() ]
static DWORD __stdcall Ftp_upload(LPVOID data) {
    asyncTask *t = (asyncTask*)data;

    t->boolean = FtpPutFileW(t->ftp->ftp, t->str1, t->str2, INTERNET_FLAG_NEED_FILE | FILE_ATTRIBUTE_NORMAL | INTERNET_FLAG_RELOAD, 0);
    t->error = GetLastError();
    log(t->ftp);
    return t->boolean ? RBoolean : RError;
}

LUA_METHOD(Ftp, upload) {
	Ftp *f = lua_self(L, 1, Ftp);
    asyncTask *t = newFTPTask(f);

    t->str1 = lua_towstring(L, 2);
    t->str2 = lua_gettop(L) == 3 ? lua_towstring(L, 3) : wcsdup(PathFindFileNameW(t->str1)); 
    push_waitTask(L, t, Ftp_upload);
	return 1;
}

//--------------------------------- [ Ftp:command]
static DWORD __stdcall Ftp_command(LPVOID data) {
    asyncTask *t = (asyncTask*)data;
    bool result; 
    HINTERNET handle = NULL;
    char buffer[2048];
	DWORD count = 0;
    
    result = FtpCommandW(t->ftp->ftp, t->boolean, FTP_TRANSFER_TYPE_ASCII, t->str1, 0, &handle);
    t->error = GetLastError();
    log(t->ftp);
	if ( !result )
error:  
        return RError;
    if(handle != NULL) {
        do     
            if (InternetReadFile(handle, buffer, 2048, &count))
                t->received.append(buffer, count);
            else goto error;
        while (count);    
        InternetCloseHandle(handle);
        return RString;
    }
    t->boolean = result;
    return RBoolean;
}

LUA_METHOD(Ftp, command) {
	Ftp *f = lua_self(L, 1, Ftp);
    asyncTask *t = newFTPTask(f); 

	t->str1 = lua_towstring(L, 2);	
    t->boolean = lua_gettop(L) == 3 ? lua_toboolean(L, 3) : false;	
	push_waitTask(L, t, Ftp_command);
	return 1;
}

//------------ ftp_call() function helper
static DWORD __stdcall Ftp_thread(LPVOID data) {
    asyncTask *t = (asyncTask*)data;

    t->boolean = t->ftpfunc(t->ftp->ftp, t->str1);
    t->error = GetLastError();
    log(t->ftp);
    return t->boolean ? RBoolean : RError;
}

static int ftp_call(lua_State *L, FTPFUNC ftpfunc) {
	Ftp *ftp = lua_self(L, 1, Ftp);
    asyncTask *t = newFTPTask(ftp); 

	t->str1 = lua_towstring(L, 2);	
	t->ftpfunc = ftpfunc;
	push_waitTask(L, t, Ftp_thread);
	return 1; 
}

//--------------------------------- [ Ftp:makedir]
LUA_METHOD(Ftp, makedir) {
	return ftp_call(L, FtpCreateDirectoryW);
}

//--------------------------------- [ Ftp:removedir]
LUA_METHOD(Ftp, removedir) {
	return ftp_call(L, FtpRemoveDirectoryW);
}

//--------------------------------- [ Ftp:removefile]
LUA_METHOD(Ftp, removefile) {
	return ftp_call(L, FtpDeleteFileW);
}

//--------------------------------- [ Ftp:movefile]
static DWORD __stdcall Ftp_movefile(LPVOID data) {
    asyncTask *t = (asyncTask*)data;

    t->boolean = FtpRenameFileW(t->ftp->ftp, t->str1, t->str2);
    t->error = GetLastError();
    log(t->ftp);
    return t->boolean ? RBoolean : RError;
}

LUA_METHOD(Ftp, movefile) {
	Ftp *ftp = lua_self(L, 1, Ftp);
    asyncTask *t = newFTPTask(ftp); 

	t->str1 = lua_towstring(L, 2);	
	t->str2 = lua_towstring(L, 3);	
	push_waitTask(L, t, Ftp_movefile);
	return 1;     
}

//--------------------------------- [ Ftp.list]
static int Ftp_iter(lua_State *L) {
	Ftp *ftp = (Ftp*)lua_touserdata(L, lua_upvalueindex(1));
	BOOL first_time = lua_type(L, lua_upvalueindex(2)) == LUA_TSTRING;
	HINTERNET h = first_time ? NULL : (HINTERNET)(UINT_PTR)lua_tointeger(L, lua_upvalueindex(2));
	WIN32_FIND_DATAW data;

	if ( first_time ) {
		if ((h = FtpFindFirstFileW(ftp->ftp, (wchar_t*)lua_tostring(L, lua_upvalueindex(2)), &data, INTERNET_FLAG_RELOAD, (DWORD_PTR)NULL))== NULL)
			goto done;
        log(ftp);
		goto found;
	}
next:
	if (InternetFindNextFileW(h, &data)) {
found:
		if ( data.cFileName[0] == L'.')
			goto next;
		else {
			lua_pushwstring(L, data.cFileName);
			lua_pushboolean(L, data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}
	}
	else {
done:
		if (h)
			InternetCloseHandle(h);
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)(UINT_PTR)h);
	lua_replace(L, lua_upvalueindex(2));
	return 2;
}

LUA_METHOD(Ftp, list) {
	int len = 1;
	wchar_t *filter = lua_gettop(L) == 2 ? lua_tolwstring(L, 2, &len) : NULL;

	lua_pushlightuserdata(L, lua_self(L, 1, Ftp));
	lua_pushlstring(L, (const char*)(filter ? filter : L""), len*sizeof(wchar_t));
	lua_pushcclosure(L, Ftp_iter, 2);
	free(filter);
	return 1;
}

//----------------------------------[ Ftp.proxy() ]
LUA_METHOD(Ftp, proxy) {
    Ftp *f = lua_self(L, 1, Ftp);
    INTERNET_PROXY_INFO ipi;
    
    ZeroMemory(&ipi, sizeof(INTERNET_PROXY_INFO));
    if ((lua_gettop(L) == 1) || lua_isnil(L, 2))
        ipi.dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
    else {
        ipi.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
        ipi.lpszProxy = luaL_checkstring(L, 2);
    }
    if (InternetSetOptionA(f->handle, INTERNET_OPTION_PROXY, &ipi, sizeof(INTERNET_PROXY_INFO))) {
        if (lua_gettop(L) > 2) {
            size_t len = 0;
            lua_pushboolean(L, (InternetSetOptionA(f->handle,INTERNET_OPTION_PROXY_USERNAME, (LPVOID)luaL_checklstring(L, 3, &len), len) == TRUE) && \
                               (InternetSetOptionA(f->handle, INTERNET_OPTION_PROXY_PASSWORD, (LPVOID)luaL_checklstring(L, 4, &len), len) == TRUE));                
        } else lua_pushboolean(L, true);
    } else lua_pushboolean(L, false);
    InternetSetOption(0, INTERNET_OPTION_REFRESH, NULL, 0);  
    return 1;
}

//----------------------------------[ Ftp.close() ]
static Ftp *close_ftp(lua_State *L) {
    Ftp *f = lua_self(L, 1, Ftp);
	if (f->handle) {
		InternetCloseHandle(f->handle);
        f->handle = NULL;
        InternetCloseHandle(f->ftp);
        delete f->task;
    }
    return f;
}
LUA_METHOD(Ftp, close) {
	close_ftp(L);
    return 0;
}

//----------------------------------[ Ftp.log ]
LUA_PROPERTY_GET(Ftp, log) {
    lua_pushstring(L, lua_self(L, 1, Ftp)->log.c_str());
    return 1;
}

//--------------------------------- [ Ftp.currentdir]
LUA_PROPERTY_GET(Ftp, currentdir) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	DWORD len = 0;
	wchar_t *path;
	
	FtpGetCurrentDirectoryW(ftp->ftp, NULL, &len);
	path = (wchar_t*)malloc(len*sizeof(wchar_t));
	if (FtpGetCurrentDirectoryW(ftp->ftp, path, &len))
		lua_pushlwstring(L, path, len-1);
	else 
		lua_pushnil(L);
	free(path);
	return 1;
}

LUA_PROPERTY_SET(Ftp, currentdir) {
    Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* str = lua_towstring(L, 2);	 
	
	lua_pushboolean(L, FtpSetCurrentDirectoryW(ftp->handle, str));
	free(str);
	return 0;
}

//--------------------------------- [ Http.hostname }
LUA_PROPERTY_GET(Ftp, hostname) {
    lua_pushstring(L, lua_self(L, 1, Ftp)->host.c_str());
	return 1;
}

//--------------------------------- [ Http.port ]
LUA_PROPERTY_GET(Ftp, port) {
    lua_pushinteger(L, lua_self(L, 1, Ftp)->port);
	return 1;
}

//--------------------  [ Ftp destructor ]
LUA_METHOD(Ftp, __gc) {
	delete close_ftp(L);
	return 0;
}

//----------------------------------[ Ftp object definition ]
OBJECT_MEMBERS(Ftp)
    READONLY_PROPERTY(Ftp, hostname)
    READONLY_PROPERTY(Ftp, port)
    READONLY_PROPERTY(Ftp, log)
    READWRITE_PROPERTY(Ftp, currentdir)
    METHOD(Ftp, download)
    METHOD(Ftp, upload)
    METHOD(Ftp, command)
    METHOD(Ftp, close)
    METHOD(Ftp, makedir)
	METHOD(Ftp, removedir)
	METHOD(Ftp, removefile)
	METHOD(Ftp, movefile)
    METHOD(Ftp, list)
    METHOD(Ftp, proxy)
END

OBJECT_METAFIELDS(Ftp)
    METHOD(Ftp, __gc)
END

