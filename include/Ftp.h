/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Ftp.h | LuaRT Ftp object header
*/

#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 5208)
#endif

#include <luart.h>
#include <Task.h>
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <string>

#define STATE_CONNECT			0
#define STATE_PUTFILE			1
#define STATE_GETFILE			2
#define STATE_COMMAND			3
#define STATE_READ				4
#define STATE_READDONE			5

struct asyncTask;

typedef struct Ftp {
	luart_type		type;
	HINTERNET 		handle;
	HINTERNET 		ftp = NULL;
	DWORD			result;
	std::string 	useragent = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
	std::string 	host;
	INTERNET_PORT	port;
	std::string		log;
	asyncTask		*task;
} Ftp;

typedef BOOL (__stdcall *FTPFUNC)(HINTERNET, LPCWSTR);

typedef struct asyncTask {
	HANDLE		thread;
	Ftp 		*ftp;
	DWORD		error = 0;
	std::string	received;
	wchar_t 	*str1 = NULL;
	wchar_t 	*str2 = NULL;
	bool		boolean = false;
	FTPFUNC 	ftpfunc;
} asyncTask;

enum TaskResult { RError, RString, RBoolean, RFile };

extern luart_type TFtp;

//---------------------------------------- Ftp object
LUA_CONSTRUCTOR(Ftp);
extern const luaL_Reg Ftp_methods[];
extern const luaL_Reg Ftp_metafields[];
