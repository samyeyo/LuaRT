/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Http.c | LuaRT Http object implementation
*/

// #include "lrtapi.h"
#include <luart.h>
#include <Http.h>
#include <Buffer.h>
#include <File.h>
#include <Ftp.h>

#include <windns.h>
#include <stdio.h>

// __declspec(dllexport) luart_type THttp;

static wchar_t *wcsndup(wchar_t* src, size_t len) {
	wchar_t *result = calloc(1, len*2+1);
	wcsncpy(result, src, len);
	return result;
}

static BOOL connect_to(lua_State *L, Http *http) {
	wchar_t *str = NULL, *host, *username, *password;
	URL_COMPONENTSW *url;
	int n = lua_gettop(L);
	int service = http->type == THttp ? INTERNET_SERVICE_HTTP : INTERNET_SERVICE_FTP;
	BOOL auth = http->type == TFtp && n > 2;
	BOOL result = FALSE;

	if (http->hcon)
		Http_close(L);
	if ((url = get_url(L, 2, &str))) {
		host     = url->dwHostNameLength ? wcsndup(url->lpszHostName, url->dwHostNameLength) : NULL;
		username = auth ? lua_towstring(L, 3) : (url->dwUserNameLength ? wcsndup(url->lpszUserName, url->dwUserNameLength) : NULL);
		password = auth ? lua_towstring(L, 4) : (url->dwPasswordLength ? wcsndup(url->lpszPassword, url->dwPasswordLength) : NULL);
		if ( (service == INTERNET_SERVICE_FTP && (url->nScheme != INTERNET_SCHEME_FTP)) || (service == INTERNET_SERVICE_HTTP && ((url->nScheme != INTERNET_SCHEME_HTTP) && (url->nScheme != INTERNET_SCHEME_HTTPS))) )
			SetLastError(ERROR_INTERNET_INVALID_URL);
		else if ( (http->hcon = InternetConnectW(http->h, host, url->nPort, username, password, service, http->type == TFtp ? INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_PASSIVE : 0, 0)) ) {
			http->hostname = host;
			http->scheme  = url->nScheme; 
			http->port = url->nPort;
			if (url->nScheme == INTERNET_SERVICE_FTP) {
				http->username = username;
				http->password = password;	
			}
			result = TRUE;
		} else {
			free(host); 
			free(username);
			free(password);	
		}
	}	
	free(url);
	free(str);
	return result;	
}

int constructor(lua_State *L, BOOL isFTP) {
	Http *http;
	HINTERNET h;
	int flag = 1;		
		
	if ((h = InternetOpen("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.74 Safari/537.36 Edg/79.0.309.43", PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0))) {
		http = calloc(1, sizeof(Http));
		http->h = h;
		if (isFTP) {
			http->type = TFtp;
			lua_newinstance(L, http, Ftp);
		}
		else {
			InternetSetOption(http->h, 65, &flag,sizeof(int));
			http->type = THttp;
			lua_newinstance(L, http, Http);
		}
		if (connect_to(L, http))
			return 1;
	}
	luaL_getlasterror(L, GetLastError());
	return lua_error(L);	
}

//--------------------------------- [ Http:constructor }
LUA_CONSTRUCTOR(Http) {
	return constructor(L, FALSE);
}

//--------------------------------- [ Http:open }
LUA_METHOD(Http, open) {
	lua_pushboolean(L, connect_to(L, lua_self(L, 1, Http)));
	return 1;
}

//--------------------------------- [ Http:close }
LUA_METHOD(Http, close) {
	Http *http = lua_self(L, 1, Http);
	if (http->request)
		InternetCloseHandle(http->request);
	if (http->hcon)
		InternetCloseHandle(http->hcon);
	free(http->hostname);
	free(http->password);
	free(http->username);
	http->scheme = 0;
	http->hostname = NULL;
	http->hcon = 0;
	http->port = 0;
	return 0;
}

URL_COMPONENTSW *get_url(lua_State *L, int idx, wchar_t **url_str) {
	int len;
	URL_COMPONENTSW *url = calloc(1, sizeof(URL_COMPONENTSW));
	
	*url_str = lua_tolwstring(L, idx, &len);
	url->dwStructSize 	 = sizeof(URL_COMPONENTSW);
	url->dwHostNameLength = 1;
    url->dwUserNameLength = 1;
    url->dwPasswordLength = 1;
    url->dwUrlPathLength  = 1;
    url->dwSchemeLength	  = 1;
	if (InternetCrackUrlW(*url_str, (size_t)len, 0, url))
		return url;
	SetLastError(ERROR_INTERNET_INVALID_URL);
	free(url);
	free(*url_str);
	return NULL;
}

//--------------------------------- [ Response helper function ]

static lua_Integer get_field(lua_State *L, Http *http, DWORD flag, BOOL isnumber) {
	DWORD err = 0;
	if (!http)
		http = lua_self(L, 1, Http);
	luaL_pushfail(L);
	if (http->request) {
		DWORD len = 12;
		char *buff = calloc(1, 12);
				
		while (!HttpQueryInfo(http->request, flag, buff, &len, NULL))
			if ( (err = GetLastError()) )  {		
				free(buff);
				if (err == ERROR_INSUFFICIENT_BUFFER)
					buff = calloc(1, len+1);
				else return 0;
			}
		lua_pushstring(L, buff);
		if (isnumber)
			lua_pushinteger(L, lua_tointeger(L, -1));
		free(buff);
	}
	return 1;
}

static BOOL prep_request(lua_State *L, wchar_t *action, DWORD flags) {
	Http *http = lua_self(L, 1, Http);
	wchar_t *path = lua_towstring(L, 2);
	wchar_t *types[] = {L"*/*", NULL};
	BOOL result = FALSE;

	if (http->request)
		InternetCloseHandle(http->request);
	if (http->scheme == INTERNET_SCHEME_HTTPS)
		flags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
	if ( (http->request = HttpOpenRequestW(http->hcon, action, path, NULL, NULL, (const wchar_t**)types, flags, 0)) && HttpAddRequestHeaders(http->request, "Accept-Encoding: gzip, deflate\r\n",-1, HTTP_ADDREQ_FLAG_ADD))
		result = TRUE;
	free(path);
	return result;
}

int get_response(lua_State *L, Http *http) {
	size_t len;
	const char *uri = lua_tolstring(L, 2, &len);
	const char *fname = uri[len-1] == '/' ? NULL : PathFindFileName(uri);
	luaL_Buffer b;

	luaL_pushfail(L);
	if (http->request) {
		char buffer[2048];
		DWORD written, count = 0;		
	
		if ((http->type == TFtp) || ((get_field(L, NULL, HTTP_QUERY_CONTENT_TYPE, FALSE) && strstr(lua_tostring(L, -1), "text/")) || !fname)) {  //-------------------> text content		
text:
			luaL_buffinit(L, &b);
			do 	if (InternetReadFile(http->request, buffer, 2048, &count))
					luaL_addlstring(&b, buffer, count);
			 	else { 
					lua_pushboolean(L, FALSE);	    	
					return 1;
				}	
			while (count);	
			luaL_pushresult(&b);
		} else { //-----------------------------------------------------------> File content
			HANDLE h;			
			if (get_field(L, NULL, HTTP_QUERY_CONTENT_DISPOSITION, FALSE) && lua_isstring(L, -1)) {  
				char *temp, *ch1, *ch2;

				temp = strdup(lua_tostring(L, -1));
				if ((ch1 = strchr(temp, '"'))) {
					ch1++;
					ch2 = strchr(ch1+1, '"');
					*ch2 = 0;
					fname = strdup(ch1);
				} 
				free(temp);
			} else if (fname)
				fname = strdup(fname);
			else {
				fname = malloc(MAX_PATH);
				GetTempFileNameA(".", "get", 0, (LPSTR)fname);
			}
			lua_pop(L, 1);
			if ((h = CreateFile(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
				do if (InternetReadFile(http->request, buffer, 2048, &count))
						WriteFile(h, buffer, count, &written, NULL);
					else break;	
				while (count);	
				CloseHandle(h);
				lua_pushstring(L, fname);
				lua_pushinstance(L, File, 1);
			} else if (GetLastError() == ERROR_INVALID_NAME)
				goto text;
			free((char *)fname);
		}
	}
	return 1;
}

//--------------------------------- [ Http:get ]
LUA_METHOD(Http, get) {
	Http *http = lua_self(L, 1, Http);

	luaL_pushfail(L);	    	
	if (prep_request(L, NULL, INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE) && HttpSendRequestW(http->request, NULL, 0, NULL, 0))
		get_response(L, http);
	return 1;
}

#define __BOUNDARY__	"----L_u_a_R_T_x"
#define CONTENT__  		"------L_u_a_R_T_x\r\nContent-Disposition: form-data; name=\""
#define __STRING__		"\"\r\n\r\n"
#define __CFILE__ 		"\"; filename=\""
#define __CFILE			"\"\r\nContent-Type: application/octet-stream\r\nContent-Transfer-Encoding: binary\r\n\r\n" 
#define __END 			"\r\n"
#define __TRAIL  		"------L_u_a_R_T_x--"

static size_t ExtractRequest(lua_State *L, HINTERNET req) {
	size_t total_size = 0, len, pos = 0;
	HANDLE h;
	LARGE_INTEGER size;
	File *file;
	char  buffer[1024];
	const char *str;
	DWORD written, read;

	lua_pushvalue(L, 3);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (!lua_isstring(L, -2))
			luaL_error(L, "error during table traversal (%s key found, string expected)", luaL_getmetafield(L, 2, "__name") ? lua_tostring(L, -1) : luaL_typename(L, 2));
		if (req) {
			memset(buffer, 0, 1024);
			pos = _snprintf(buffer+pos, sizeof(CONTENT__)-1, CONTENT__);
			str = lua_tolstring(L, -2, &len);
			pos += _snprintf(buffer+pos, len, str);
		} else 	total_size += luaL_len(L, -2); 
		switch(lua_type(L, -1)) {
			case LUA_TBOOLEAN:
			case LUA_TNUMBER:	
			case LUA_TSTRING:	str = lua_tolstring(L, -1, &len);
								if (req) {
									pos += _snprintf(buffer+pos, sizeof(__STRING__)-1, __STRING__);
									pos += _snprintf(buffer+pos, len, str);
									if (!InternetWriteFile(req, buffer, pos, &written))
										return FALSE;
								} else total_size +=  sizeof(CONTENT__) + len + sizeof(__STRING__) + sizeof(__END) - 3;
								break;
			case LUA_TTABLE:	file = luaL_checkcinstance(L, -1, File);
								if ( (h = CreateFileW(file->fullpath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)  {
									luaL_getlasterror(L, GetLastError());
									luaL_error(L, "cannot open File : %s", lua_tostring(L, -1));
								}
								GetFileSizeEx(h, &size); 
								if (req) {
									char readbuff[2048];
									DWORD done = 0;
									
									pos += _snprintf(buffer+pos, sizeof(__CFILE__)-1, __CFILE__);
									lua_pushwstring(L, PathFindFileNameW(file->fullpath));
									str = lua_tolstring(L, -1, &len);
									pos += _snprintf(buffer+pos, len, str);
									pos += _snprintf(buffer+pos, sizeof(__CFILE)-1, __CFILE);
									if (!InternetWriteFile(req, buffer, pos, &written))
										return FALSE;
									do 
									   if (ReadFile(h, readbuff, 2048, &read, NULL) && InternetWriteFile(req, readbuff, read, &written))
										   	done += written;
										else return FALSE;	
									while (done < size.QuadPart);
									lua_pop(L, 1);
								} else total_size += sizeof(CONTENT__) + sizeof(__CFILE__) + wcslen(PathFindFileNameW(file->fullpath)) + sizeof(__CFILE) + size.QuadPart + sizeof(__END) - 4;
								CloseHandle(h);
								break;
			default:			luaL_error(L, "error during table traversal (%s value found, string or File expected)", luaL_getmetafield(L, 2, "__name") ? lua_tostring(L, -1) : luaL_typename(L, 2));
		}
		lua_pop(L, 1);
		if (req)
			if (!InternetWriteFile(req, __END, sizeof(__END)-1, &written))
				return FALSE;
	}	
	if (req && InternetWriteFile(req, __TRAIL, sizeof(__TRAIL)-1, &written))
		return HttpEndRequest(req, NULL, 0, 0);
	return total_size + sizeof(__TRAIL)-1;
}

//--------------------------------- [ Http:post ]
LUA_METHOD(Http, post) {
	Http *http = lua_self(L, 1, Http);
	
	luaL_checktype(L, 3, LUA_TTABLE);
	luaL_pushfail(L);
	if (luaL_getmetafield(L, 3, "__name"))
		luaL_typeerror(L, 3, "table");
	if ( prep_request(L, L"POST", INTERNET_FLAG_NO_CACHE_WRITE) ) {
		if ( HttpAddRequestHeaders(http->request, "Content-Type: multipart/form-data; boundary="__BOUNDARY__, -1, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD)) {	 
			INTERNET_BUFFERSA ibuff;
			
			memset(&ibuff, 0, sizeof(INTERNET_BUFFERS));
			ibuff.dwStructSize  = sizeof(INTERNET_BUFFERS);  
			ibuff.dwBufferTotal = ExtractRequest(L, NULL);
			if (HttpSendRequestEx(http->request, &ibuff, NULL, 0, 0) && ExtractRequest(L, http->request))
				get_response(L, http);
		}
	} 
	return 1;
}

//--------------------------------- [ Http.protocol }
LUA_PROPERTY_GET(Http, protocol) {
	Http *http = lua_self(L, 1, Http);
	if (http->hcon)
		lua_pushstring(L, (http->scheme == INTERNET_SCHEME_HTTPS)	? "https" : (http->scheme == INTERNET_SCHEME_FTP ? "ftp" : "http"));
	else luaL_pushfail(L);
	return 1;
}

//--------------------------------- [ Http.hostname }
LUA_PROPERTY_GET(Http, hostname) {
	Http *http = lua_self(L, 1, Http);
	if (http->hcon)
		lua_pushwstring(L, http->hostname);
	else luaL_pushfail(L);
	return 1;
}

//--------------------------------- [ Http.port ]
LUA_PROPERTY_GET(Http, port) {
	Http *http = lua_self(L, 1, Http);
	if (http->hcon)
		lua_pushinteger(L, http->port);
	else luaL_pushfail(L);
	return 1;
}

//--------------------------------- [ Http.statuscode]
LUA_PROPERTY_GET(Http, statuscode) {
	return get_field(L, NULL, HTTP_QUERY_STATUS_CODE, TRUE);
}
//--------------------------------- [ Http.status]
LUA_PROPERTY_GET(Http, status) {
	return get_field(L, NULL, HTTP_QUERY_STATUS_TEXT, FALSE);
}
//--------------------------------- [ Http.headers]
LUA_PROPERTY_GET(Http, headers) {
	get_field(L, NULL, HTTP_QUERY_RAW_HEADERS_CRLF, FALSE);
	return 1;
}

//--------------------------------- [ Http.mime]
LUA_PROPERTY_GET(Http, mime) {
	return get_field(L, NULL, HTTP_QUERY_CONTENT_TYPE, FALSE);
}

LUA_METHOD(Http, __gc) {
	Http_close(L);
	free(lua_self(L, 1, Http));
	return 0;
}

const luaL_Reg Http_metafields[] = {
	{"__gc", Http___gc},
	{NULL, NULL}
};

const luaL_Reg Http_methods[] = {
	{"open",		Http_open},
	{"get",			Http_get},
	{"post",		Http_post},
	{"close",		Http_close},
	{"get_scheme",	Http_getprotocol},
	{"get_hostname",Http_gethostname},
	{"get_port",	Http_getport},
	{"get_status",	Http_getstatus},
	{"get_statuscode",	Http_getstatuscode},
	{"get_headers",	Http_getheaders},
	{"get_mime",	Http_getmime},
	{NULL, NULL}
};