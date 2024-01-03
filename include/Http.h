/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Http.h | LuaRT Http object header
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
#include <map>

#define INTERNET_OPTION_HTTP_DECODING 65

#define REQ_STATE_SEND_REQ             0
#define REQ_STATE_SEND_REQ_WITH_BODY   1
#define REQ_STATE_POST_SEND_HEADERS	   2
#define REQ_STATE_POST_GET_DATA        3
#define REQ_STATE_POST_SEND_DATA       4
#define REQ_STATE_POST_COMPLETE        5
#define REQ_STATE_RESPONSE_RECV_DATA   6
#define REQ_STATE_COMPLETE             7

typedef struct Http {
	luart_type		type;
	HINTERNET 		handle;
	HINTERNET 		hRequest;
	HINTERNET 		hConnect;

	int				state;
	int 			port;
	bool	 		ssl;
	std::string		scheme;
	std::string 	useragent = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
	std::string		received;
	bool			download;
	std::string		fname;
	FILE			*file;
	size_t			recvsize;
	std::string 	method;
	std::string 	host;
	std::string 	url;
	std::string		proxy;
	bool 			failed = false;
	bool 			done = false;
	DWORD 			sended;
	std::string		outbuffer;
	char			inbuffer[65536];
    CRITICAL_SECTION CriticalSection;
    BOOL 			CritSecInitialized;
    DWORD 			HandleUsageCount;
    bool 			Closing;
	DWORD			started;
	std::map <std::string, std::string> cookies; 
	std::map<std::string, std::string> header = { {"",""} };
} Http;

static char *get_header(Http *h, const char *field);

extern luart_type THttp;

//---------------------------------------- Http object
LUA_CONSTRUCTOR(Http);
extern const luaL_Reg Http_methods[];
extern const luaL_Reg Http_metafields[];
