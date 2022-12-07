/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Ftp.c | LuaRT Ftp object implementation
*/

#include <luart.h>
#include <Ftp.h>
#include "../include/Http.h"
#include <Buffer.h>
#include <File.h>

#include <windns.h>
#include <stdio.h>

// __declspec(dllexport) luart_type TFtp;

//--------------------------------- [ Ftp:constructor }
LUA_CONSTRUCTOR(Ftp) {
	luaL_checkstring(L, 3);
	luaL_checkstring(L, 4);	
	return constructor(L, TRUE);
}

typedef WINBOOL (__attribute__((stdcall)) *FTPFUNC)(void *, const WCHAR *);

static int ftp_call(lua_State *L, FTPFUNC ftpfunc) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* str = lua_towstring(L, 2);	 
	
	lua_pushboolean(L, ftpfunc(ftp->hcon, str));
	free(str);
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
LUA_METHOD(Ftp, movefile) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* old = lua_towstring(L, 2);	 
	wchar_t* new = lua_towstring(L, 3);	 
	
	lua_pushboolean(L, FtpRenameFileW(ftp->hcon, old, new));
	free(old);
	free(new);
	return 1;
}
//--------------------------------- [ Ftp:download]
LUA_METHOD(Ftp, download) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* file = lua_towstring(L, 2);
	wchar_t* fname = PathFindFileNameW(file); 

	if (FtpGetFileW(ftp->hcon, file, fname, FALSE, FILE_ATTRIBUTE_NORMAL, INTERNET_FLAG_NEED_FILE | FTP_TRANSFER_TYPE_BINARY, (DWORD_PTR)NULL)) {
		lua_pushwstring(L, fname);
		lua_pushinstance(L, File, 1);
	}
	else 
		luaL_pushfail(L);
	free(file);
	return 1;
}

//--------------------------------- [ Ftp:upload]
LUA_METHOD(Ftp, upload) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* file = luaL_checkFilename(L, 2);
	wchar_t* path = lua_gettop(L) == 3 ? lua_towstring(L, 3) : NULL; 
	
	lua_pushboolean(L, FtpPutFileW(ftp->hcon, file, path ? path : PathFindFileNameW(file), FILE_ATTRIBUTE_NORMAL | INTERNET_FLAG_RELOAD, (DWORD_PTR)NULL));
	free(file);
	free(path);
	return 1;
}

//--------------------------------- [ Ftp:command]
LUA_METHOD(Ftp, command) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	wchar_t* cmd = lua_towstring(L, 2);	 
	
	if (FtpCommandW(ftp->hcon, TRUE, FTP_TRANSFER_TYPE_BINARY, cmd, (DWORD_PTR)NULL, &ftp->request)) {
		get_response(L, ftp);
		ftp->request = 0;
	}
	else
		lua_pushboolean(L, FALSE);
	free(cmd);
	return 1;
}

//--------------------------------- [ Ftp.currentdir]
LUA_PROPERTY_GET(Ftp, currentdir) {
	Ftp *ftp = lua_self(L, 1, Ftp);
	DWORD len = 0;
	wchar_t *path;
	
	FtpGetCurrentDirectoryW(ftp->hcon, NULL, &len);
	path = malloc(len*sizeof(wchar_t));
	if (FtpGetCurrentDirectoryW(ftp->hcon, path, &len))
		lua_pushlwstring(L, path, len-1);
	else 
		lua_pushboolean(L, FALSE);
	free(path);
	return 1;
}

LUA_PROPERTY_SET(Ftp, currentdir) {
	ftp_call(L, FtpSetCurrentDirectoryW);
	return 0;
}

//--------------------------------- [ Ftp.active]
LUA_PROPERTY_GET(Ftp, active) {
	lua_pushboolean(L, lua_self(L, 1, Ftp)->active);
	return 1;
}

LUA_PROPERTY_SET(Ftp, active) {
	BOOL value = lua_toboolean(L, 2);
	Ftp *ftp = lua_self(L, 1, Ftp);
	if (ftp->hcon &&  value != ftp->active) {
		DWORD len;
		wchar_t *path;
		FtpGetCurrentDirectoryW(ftp->hcon, NULL, &len);
		path = malloc(len*sizeof(wchar_t));
		if (!FtpGetCurrentDirectoryW(ftp->hcon, path, &len)) {
			free(path);
			path = NULL;	
		}
		InternetCloseHandle(ftp->hcon);
		if ((ftp->hcon = InternetConnectW(ftp->h, ftp->hostname, ftp->port, ftp->username, ftp->password, ftp->scheme, value ? INTERNET_FLAG_KEEP_CONNECTION : INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_PASSIVE, 0)))
			ftp->active = value;
		else
			ftp->hcon = InternetConnectW(ftp->h, ftp->hostname, ftp->port, ftp->username, ftp->password, ftp->scheme, ftp->active ? INTERNET_FLAG_KEEP_CONNECTION : INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_PASSIVE, 0);			
		if (path) {
			FtpSetCurrentDirectoryW(ftp->hcon, path);
			free(path);
		}
	}
	return 0;
}

//--------------------------------- [ Ftp.list]
static int Ftp_iter(lua_State *L) {
	Ftp *ftp = lua_touserdata(L, lua_upvalueindex(1));
	BOOL first_time = lua_type(L, lua_upvalueindex(2)) == LUA_TSTRING;
	HINTERNET h = first_time ? NULL : (HINTERNET)(int)lua_tointeger(L, lua_upvalueindex(2));
	WIN32_FIND_DATAW data;

	if ( first_time ) {
		if ((h = FtpFindFirstFileW(ftp->hcon, (wchar_t*)lua_tostring(L, lua_upvalueindex(2)), &data, INTERNET_FLAG_RELOAD, (DWORD_PTR)NULL))== NULL)
			goto done;
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
	lua_pushinteger(L, (lua_Integer)(int)h);
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

const luaL_Reg Ftp_metafields[] = {
	{"__gc", Http___gc},
	{NULL, NULL}
};

const luaL_Reg Ftp_methods[] = {
	{"open",			Http_open},
	{"close",			Http_close},
	{"makedir",			Ftp_makedir},
	{"removedir",		Ftp_removedir},
	{"removefile",		Ftp_removefile},
	{"movefile",		Ftp_movefile},
	{"download",		Ftp_download},
	{"upload",			Ftp_upload},
	{"list",			Ftp_list},
	{"command",			Ftp_command},
	{"get_hostname",	Http_gethostname},
	{"get_port",		Http_getport},
	{"get_currentdir", 	Ftp_getcurrentdir},
	{"set_currentdir", 	Ftp_setcurrentdir},
	{"get_active",		Ftp_getactive},
	{"set_active",		Ftp_setactive},
	{NULL, NULL}
};