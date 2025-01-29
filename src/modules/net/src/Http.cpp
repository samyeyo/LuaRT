/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Http.cpp | LuaRT Http object implementation
*/



#include <luart.h>
#include <File.h>
#include <stdio.h>
#include <string>
#include <Http.h>

luart_type THttp;

const char* spaces = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = spaces)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = spaces)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(const std::string& s, const char* t = spaces)
{
    return ltrim(rtrim((std::string&)s, t), t);
}

bool lockRequest(Http *h) {
    BOOL result = TRUE;

    EnterCriticalSection(&h->CriticalSection);
    result = !h->Closing;
    if (result)
        h->HandleUsageCount++;
    LeaveCriticalSection(&h->CriticalSection);    
    
    return result;
}

void unlockRequest(Http *h) {
    BOOL toclose;

    EnterCriticalSection(&h->CriticalSection);
    h->HandleUsageCount--;   
    toclose = (h->Closing == TRUE && h->HandleUsageCount == 0);
    LeaveCriticalSection(&h->CriticalSection);

    if(toclose)
        InternetCloseHandle(h->hRequest);         
    return;
}

static void ProcessRequest(Http *h, DWORD *error) {
  
    if (lockRequest(h)) {
        while(*error == ERROR_SUCCESS && h->state != REQ_STATE_COMPLETE) {
            switch(h->state) {
                case REQ_STATE_SEND_REQ:
                    h->state = REQ_STATE_RESPONSE_RECV_DATA;
                    if (!HttpSendRequestA(h->hRequest, NULL, 0, NULL, 0))
                        *error = GetLastError();
                    break;

                case REQ_STATE_SEND_REQ_WITH_BODY:    
                    INTERNET_BUFFERS BuffersIn;    

                    ZeroMemory(&BuffersIn, sizeof(INTERNET_BUFFERS));
                    BuffersIn.dwStructSize = sizeof(INTERNET_BUFFERS);
                    BuffersIn.dwBufferTotal = h->outbuffer.length();
                    
                    h->state = REQ_STATE_POST_SEND_DATA;
                    if (!HttpSendRequestExA(h->hRequest, &BuffersIn, NULL, 0, (DWORD_PTR)h))                                
                        *error = GetLastError();
                    break;

                case REQ_STATE_POST_SEND_DATA:
                    h->state = REQ_STATE_POST_COMPLETE;
                    h->outbuffer = h->outbuffer.substr(h->sended, std::string::npos);
                    if (!InternetWriteFile(h->hRequest, h->outbuffer.c_str(), h->outbuffer.length(), &h->sended))
                        *error = GetLastError();
                    break;
                    
                case REQ_STATE_POST_COMPLETE:                     
                    h->state = REQ_STATE_RESPONSE_RECV_DATA;
                    if (!HttpEndRequest(h->hRequest, NULL, HSR_INITIATE, 0))
                        *error = GetLastError();
                    break;                
                    
                case REQ_STATE_RESPONSE_RECV_DATA:                                 
                    DWORD len;
                    INTERNET_BUFFERS ib;

                    ZeroMemory(&ib, sizeof(INTERNET_BUFFERS));
                    ib.dwStructSize = sizeof(INTERNET_BUFFERS);
                    ib.lpvBuffer = h->inbuffer;
                    ib.dwBufferLength = sizeof(h->inbuffer) - 1;
                    if(!InternetReadFileExA(h->hRequest, &ib, IRF_SYNC | IRF_NO_WAIT, (LPARAM)h))
                        if ((*error = GetLastError()) == ERROR_IO_PENDING)
                            break;
                    len = ib.dwBufferLength;
                    if (len == 0) {
                        h->state = REQ_STATE_COMPLETE;
                        h->done = true;
                        h->started = GetTickCount()-h->started;
                        goto done;
                    }
                    if(h->file)
                        fwrite(h->inbuffer, 1, len, h->file);
                    else
                        h->received.append(h->inbuffer);
                    h->recvsize += len;
                    break;
            }
        } 
    } else  *error = ERROR_OPERATION_ABORTED;
    if (*error && (*error != ERROR_IO_PENDING))
        h->failed = true;
done:
    unlockRequest(h);
    return;
}

static void CALLBACK RequestCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength) {
    Http *h = (Http *)dwContext;

    if ((h->type == THttp) && (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE)) {
        if (((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwResult)
            ProcessRequest(h, &((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwError);
        else h->failed = true;
    }
}

//----------------------------------[ Http() constructor ]
LUA_CONSTRUCTOR(Http) {
    Http *h = new Http();
    bool gzip = true;
    URL_COMPONENTSA urlcomps;
	CHAR buf0[256], buf1[256], buf2[256], buf3[256], buf4[1024], buf5[1024];

    ZeroMemory(&urlcomps, sizeof(URL_COMPONENTSA));
    h->host = luaL_checkstring(L, 2);
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
	InternetCrackUrlA(h->host.c_str(), (DWORD)strlen(h->host.c_str()), 0, &urlcomps);
	if (!strlen(urlcomps.lpszUserName) && (lua_gettop(L) > 3)) {
        urlcomps.lpszUserName = (LPSTR)luaL_checkstring(L, 3);
        urlcomps.lpszPassword = (LPSTR)luaL_checkstring(L, 4);
    }
    h->ssl = urlcomps.nScheme==INTERNET_SCHEME_HTTPS;
    h->scheme = urlcomps.lpszScheme;
    h->handle = InternetOpen(h->useragent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);
	h->port = urlcomps.nPort;
	h->host = urlcomps.lpszHostName;
	h->url = urlcomps.lpszScheme;
	h->url += "://";
	h->url += urlcomps.lpszUrlPath;
    if (h->handle) {
        if ( !(h->hConnect = InternetConnectA(h->handle,*urlcomps.lpszHostName ? urlcomps.lpszHostName : h->host.c_str(), urlcomps.nPort, urlcomps.lpszUserName, urlcomps.lpszPassword, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)h)) )
            goto err;
        InternetSetOption(h->handle, INTERNET_OPTION_HTTP_DECODING, (LPVOID)&gzip, sizeof(gzip));
        InternetSetStatusCallback(h->handle, RequestCallback);
    } else
err:    luaL_error(L, "Failed to connect to host '%s'", h->host.c_str());
    InitializeCriticalSectionAndSpinCount(&h->CriticalSection, 4000);
    lua_newinstance(L, h, Http);
    return 1;
}

//----------------------------------[ Http.proxy() ]
LUA_METHOD(Http, proxy) {
    Http *h = lua_self(L, 1, Http);
    INTERNET_PROXY_INFO ipi;
    
    ZeroMemory(&ipi, sizeof(INTERNET_PROXY_INFO));
    if ((lua_gettop(L) == 1) || lua_isnil(L, 2))
        ipi.dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
    else {
        ipi.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
        ipi.lpszProxy = luaL_checkstring(L, 2);
    }
    if (InternetSetOptionA(h->handle, INTERNET_OPTION_PROXY, &ipi, sizeof(INTERNET_PROXY_INFO))) {
        if (lua_gettop(L) > 2) {
            size_t len = 0;
            lua_pushboolean(L, (InternetSetOptionA(h->handle,INTERNET_OPTION_PROXY_USERNAME, (LPVOID)luaL_checklstring(L, 3, &len), len) == TRUE) && \
                               (InternetSetOptionA(h->handle, INTERNET_OPTION_PROXY_PASSWORD, (LPVOID)luaL_checklstring(L, 4, &len), len) == TRUE));                
        } else lua_pushboolean(L, true);
    } else lua_pushboolean(L, false);
    InternetSetOption(0, INTERNET_OPTION_REFRESH, NULL, 0);    
    return 1;
}

static void push_headerfield(lua_State *L, std::string to, const char delim, std::string scheme = "https", const char *host = NULL) {
    for (size_t i = 0; i < to.length(); ++i) {  
        if (to[i] == delim) {
            std::string key = trim(to.substr(0, i));
            std::string value = trim(to.substr(i+1, std::string::npos));

            if ((delim == ':') && (key == "Set-Cookie") && host) {
                std::string cname = value.substr(0, value.find("="));
                std::string url = scheme+"://";
                size_t domain, Domain, path, Path;
                domain = value.find("domain=") + 7; Domain = value.find("Domain=") + 7; path = value.find("path=") + 5; Path = value.find("Path=") + 5;
                if ((domain != std::string::npos) || (Domain != std::string::npos) || (path != std::string::npos) || (Path != std::string::npos)) {
                    if (domain != std::string::npos)
                        url += value.substr(domain);
                    else if (Domain != std::string::npos)
                        url += value.substr(Domain);
                    if (url.find(";") != std::string::npos)
                        url = url.substr(0, url.find(";"));
                    if (path != std::string::npos)
                        url += value.substr(path);
                    else if (Path != std::string::npos)
                        url += value.substr(Path);
                    if (url.find(";") != std::string::npos)
                        url = url.substr(0, url.find(";"));
                } else url = host;
                InternetSetCookieA(url.c_str(), cname.c_str(), value.c_str());
                lua_getfield(L, -2, "cookies");
                lua_pushstring(L, (value.substr(value.find("=")+1)).c_str());
                lua_setfield(L, -2, cname.c_str());
                lua_pop(L, 1);
            } 
            lua_pushstring(L, value.c_str());
            lua_setfield(L, -2, key.c_str()); 
            return;                           
        }
    }
}

//--------------------------------- [ Http.hostname }
LUA_PROPERTY_GET(Http, hostname) {
	Http *http = lua_self(L, 1, Http);
	if (http->handle)
		lua_pushstring(L, http->host.c_str());
	else luaL_pushfail(L);
	return 1;
}

//--------------------------------- [ Http.port ]
LUA_PROPERTY_GET(Http, port) {
	Http *http = lua_self(L, 1, Http);
	if (http->handle)
		lua_pushinteger(L, http->port);
	else luaL_pushfail(L);
	return 1;
}

static int RequestTaskContinue(lua_State* L, int status, lua_KContext ctx) {
	Http *h = (Http*)ctx;
    if (lockRequest(h)) {
        if (h->done || h->failed) {            
            if (h->file) {
                fflush(h->file);
                fclose(h->file);
            }
            lua_pushvalue(L, lua_upvalueindex(1));
            if (h->done) {
                DWORD len = 255;
                char *buff;
                
                lua_createtable(L, 0, 2);
                lua_createtable(L, 0, 1);
                lua_setfield(L, -2, "cookies");

                if (!h->download) {
                    lua_pushlstring(L, h->received.c_str(), h->recvsize);
                    lua_setfield(L, -2, "content");
                }

                lua_pushinteger(L, h->started);
                lua_setfield(L, -2, "elapsed");

                DWORD status = 0, length = sizeof(DWORD);
                if (HttpQueryInfo(h->hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &length, NULL)) {
                    if (h->file && status != 200)
                        DeleteFileA(h->fname.c_str());
                    lua_pushinteger(L, status);
                    lua_setfield(L, -2, "status");
                    lua_pushboolean(L, status < 400);
                    lua_setfield(L, -2, "ok");
                }

                lua_createtable(L, 0, 6);
                do {
                    buff = (char*)calloc(1, len+1);
                    if (!HttpQueryInfoA(h->hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, buff, &len, NULL)) {
                        free(buff);
                        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                            buff = (char*)calloc(1, len+1);
                        else
                            luaL_error(L, "failed to retrieve response headers");
                    } else break;
                } while (true);
                length = 128;
                char statustext[128];            
                if (HttpQueryInfo(h->hRequest, HTTP_QUERY_STATUS_TEXT, statustext, &length, NULL)) {
                    lua_pushlstring(L, statustext, length);
                    lua_setfield(L, -3, "reason");
                }

                std::string::size_type pos = 0;
                std::string::size_type prev = 0;
                std::string str = buff;                
                
                free(buff);
                while ((pos = str.find("\r\n", prev)) != std::string::npos)
                {
                    push_headerfield(L, str.substr(prev, pos - prev), ':', h->scheme.c_str(), h->host.c_str());               
                    prev = pos + 2;
                }
                push_headerfield(L, str.substr(prev), ':', h->scheme.c_str(), h->host.c_str()); 
                lua_setfield(L, -2, "headers");
                if (h->download) {
                    lua_pushstring(L, h->fname.c_str());
                    lua_pushinstance(L, File, 1);
                    lua_remove(L, -2);
                    lua_setfield(L, -2, "file");
                }            
                lua_pushvalue(L, -1);
                lua_setfield(L, lua_upvalueindex(1), "response"); 
            } else if (h->failed) {
                DeleteFileA(h->fname.c_str());
                lua_pushnil(L);
            }
            h->header.clear();
            h->file = NULL;
            h->download = false;
            unlockRequest(h);
            return 2;
        }
    }
    unlockRequest(h);
    return lua_yieldk(L, 0, (lua_KContext)h, RequestTaskContinue);
}

static int WaitTask(lua_State *L) {	
    Http *h = lua_self(L, lua_upvalueindex(1), Http);
    
    h->recvsize = 0;
    if (h->download && !h->file) {
        if (h->file == NULL) {
            char *header = get_header(h, "content-disposition");
            char *pos;
            if (header && (pos = strchr(header, '=')+1))
                h->fname = pos;
            else
                h->fname = PathFindFileNameA(h->url.c_str());
            h->file = fopen(h->fname.c_str(), "wb");
            if (!h->file) {
                h->fname = "download.bin";
                if ( !(h->file = fopen(h->fname.c_str(), "wb")) )
                    luaL_error(L, "failed to create downloaded file on disk");
            }
            free(header);
        }
    }   				
    return lua_yieldk(L, 0, (lua_KContext)h, RequestTaskContinue);
}

static int do_request(lua_State *L, Http *h, const char *verb) {
    int nargs = lua_gettop(L);
    std::string args;     
	char encURL[INTERNET_MAX_URL_LENGTH];
	DWORD size = sizeof(encURL);
	

    if (h->hRequest && !h->done) 
        luaL_error(L, "current %s request is not finished", h->method);

    std::string uri = (const char*)luaL_optstring(L, 2, "/");

	if ((*verb == 'G') && !h->download) {
		if (nargs > 2) {
			luaL_checktype(L, 3, LUA_TTABLE);
			lua_pushnil(L);
			while (lua_next(L, 3)) {
				args += !args.length() ? "?" : "&";
				if (!lua_isstring(L, -2) || !lua_isstring(L, -1))
					luaL_error(L, "malformed table #3, expecting string keys and values");
			args += lua_tostring(L, -2);
			args += "=";
			args += lua_tostring(L, -1);
			lua_pop(L, 1);
			}
		}
		if (!InternetCanonicalizeUrl(args.c_str(), encURL, &size, ICU_BROWSER_MODE))
			luaL_error(L, "failed to encode URI '%s'", uri.c_str());
		uri += encURL;
	} 					
    h->method = verb;   
    h->sended = 0; 
    h->done = false;
    h->received = "";
    h->url += h->host + uri;
    if (h->hRequest)
        InternetCloseHandle(h->hRequest);
    if ( (h->hRequest = HttpOpenRequestA(h->hConnect, h->method.c_str(), uri.c_str(), NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | (h->ssl ? INTERNET_FLAG_SECURE : 0), (DWORD_PTR)h)) ) {
        DWORD error = ERROR_SUCCESS;
        
        //---- prepare headers
        if ( (h->header.find("content-type") == h->header.end()) && (h->header.find("Content-Type") == h->header.end()) )
            HttpAddRequestHeadersA(h->hRequest, "Content-Type: text/plain", -1L, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
        for (auto entry : h->header)
            if (entry.first.length()) {
                if (entry.first == "Cookie")  {
                    std::string::size_type pos = 0;
                    std::string::size_type prev = 0;
                    while ((pos = entry.second.find(";", prev)) != std::string::npos) {
						std::string cookie = entry.second.substr(prev, pos - prev);
						std::string::size_type sep = entry.second.find("=", prev);
						InternetSetCookieA(h->url.c_str(), cookie.substr(0, sep).c_str(), cookie.substr(sep+1, std::string::npos).c_str());
                        prev = pos + 1;
                    }
                } else HttpAddRequestHeadersA(h->hRequest, (entry.first + ": " + entry.second).c_str(), -1L, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
            }

        InternetSetStatusCallback(h->hRequest, RequestCallback);
        h->state = (*verb != 'P') ? REQ_STATE_SEND_REQ : REQ_STATE_SEND_REQ_WITH_BODY;
        h->started = GetTickCount();
        ProcessRequest(h, &error);
        if (error == ERROR_IO_PENDING) {
            lua_pushvalue(L, 1);
            lua_pushcclosure(L, WaitTask, 1);
            lua_pushinstance(L, Task, 1);
            lua_pushvalue(L, -1);
            lua_call(L, 0, 0);
            return 1;
        }
    }
    lua_pushboolean(L, false);
    return 1;
}

//----------------------------------[ Http.get() ]
LUA_METHOD(Http, get) {
    return do_request(L, lua_self(L, 1, Http), "GET");
}

//----------------------------------[ Http.post() ]
static wchar_t *GetMIME(lua_State *L, int idx) {
	LPWSTR mime = NULL;
    wchar_t *fname = luaL_checkFilename(L, idx);
	if (SUCCEEDED(FindMimeFromData(NULL, fname, NULL, 0, NULL, FMFD_URLASFILENAME, &mime, 0))) {
		lua_pushwstring(L, mime);
		CoTaskMemFree(mime);
	}
	else lua_pushstring(L, "text/plain");//"application/octet-stream");
	return fname;    
}

static std::string GenBoundaryString()
{
    GUID guid;
    char buff[128];
    static const char* szFormat = "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";

    ::CoCreateGuid(&guid);

    _snprintf(buff, 128, szFormat,
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    return buff;
}

static void readfile(lua_State *L, Http *h, wchar_t *fname) {
    HANDLE hfile;
    LARGE_INTEGER fsize;
    char readbuff[4096];
    DWORD read, done = 0;

    if ( (hfile = CreateFileW(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)  {
err:	luaL_getlasterror(L, GetLastError());
        luaL_error(L, "cannot open File : %s", lua_tostring(L, -1));
    }
    GetFileSizeEx(hfile, &fsize); 
    do 
        if (ReadFile(hfile, readbuff, 4096, &read, NULL)) {
            done += read;
            h->outbuffer.append(readbuff, read);
        } else goto err;
    while (done < fsize.QuadPart);
    CloseHandle(hfile);	
    free(fname);
}

LUA_METHOD(Http, post) {
    Http *h = lua_self(L, 1, Http);
    
    if (lua_istable(L, 3)) {
        std::string boundary = GenBoundaryString();
        h->header["Content-Type"] = "multipart/form-data; boundary="+boundary;
        boundary = "--"+boundary;

        lua_pushnil(L);
        while (lua_next(L, 3)) {
            h->outbuffer += boundary + "\r\nContent-Disposition: form-data; name=\"";
            if (!lua_isstring(L, -2))
                luaL_error(L, "malformed table #2, expecting string keys (found %s key)", luaL_typename(L, -2));
            if (!lua_isstring(L, -1) && !lua_iscinstance(L, -1, TFile))
                luaL_error(L, "malformed table #2, expecting string or File as values (found %s)", luaL_typename(L, -1));
            h->outbuffer += lua_tostring(L, -2);
            h->outbuffer += "\"";
            if (lua_isstring(L, -1)) {
                h->outbuffer += "\r\n\r\n";
                h->outbuffer += lua_tostring(L, -1);
            }
            else {
                h->outbuffer += "; filename=\"";
                wchar_t *fname = GetMIME(L, -1);
                lua_pushwstring(L, PathFindFileNameW(fname));
                h->outbuffer += lua_tostring(L, -1);
                h->outbuffer += "\"\r\nContent-Type: ";
                h->outbuffer += lua_tostring(L, -2);
                h->outbuffer += "\r\n\r\n";
                readfile(L, h, fname);
                lua_pop(L, 2);
            }
        	lua_pop(L, 1);
        }
        h->outbuffer += "\r\n"+boundary+"--\r\n";
    } else if (lua_isstring(L, 3)) {
        h->outbuffer += lua_tostring(L, 3);
    }
    return do_request(L, h, "POST");
}

//----------------------------------[ Http.download() ]
LUA_METHOD(Http, download) {
    Http *h = lua_self(L, 1, Http);
    h->download = true;
    if (lua_gettop(L) > 2) {
        h->fname = luaL_checkstring(L, 3);
        h->file = fopen(h->fname.c_str(), "wb");
    }
    return do_request(L, h, "GET");
}

//----------------------------------[ Http.delete() ]
LUA_METHOD(Http, delete) {
    return do_request(L, lua_self(L, 1, Http), "DELETE");
}

//----------------------------------[ Http.put() ]
LUA_METHOD(Http, put) {
    Http *h = lua_self(L, 1, Http);
    if (lua_isstring(L, 3)) 
        h->outbuffer = lua_tostring(L, 3);
    else readfile(L, h, luaL_checkFilename(L, 3));
    return do_request(L, h, "PUT");
}

//----------------------------------[ Http.received property ]
LUA_PROPERTY_GET(Http, received) {
    lua_pushinteger(L, lua_self(L, 1, Http)->recvsize);
    return 1;
}

//----------------------------------[ Http.content property ]
LUA_PROPERTY_GET(Http, content) {
    Http *h = lua_self(L, 1, Http);
    lua_pushlstring(L, h->received.c_str(), h->recvsize);
    return 1;
}

//----------------------------------[ Http.headers property ]
static char *get_header(Http *h, const char *field) {	
	if ( (h->header.find(field) == h->header.end()) && h->hRequest) {
		size_t len = strlen(field);	
		char *header = (char *)calloc(1, ++len);		
retry:
        strncpy(header, field, len);
        if(!HttpQueryInfo(h->hRequest,HTTP_QUERY_CUSTOM, (LPVOID)header, (LPDWORD)&len, NULL)) {
            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                free(header);
                header = (char*)calloc(1, ++len);;
                goto retry;
            } else {
				free(header);
				return NULL;
			}
        } else
            return header;
    }
	return strdup(h->header[field].c_str());
}

LUA_METHOD(headers, index) {
    luaL_getmetafield(L, 1, "http");
    char *value = get_header(lua_self(L, -1, Http), lua_tostring(L, 2));

    if (value) {
        lua_pushstring(L, value);
        free(value);
    }

    return value != NULL;
}

LUA_METHOD(headers, newindex) {
    std::string field = luaL_checkstring(L, 3);
    luaL_getmetafield(L, 1, "http");
    lua_self(L, -1, Http)->header[luaL_checkstring(L, 2)] = trim(field);
    return 0;
}

static int headers_iterator(lua_State *L) {	
	Http *h = (Http *)lua_touserdata(L, lua_upvalueindex(1));
    int idx = lua_tointeger(L, lua_upvalueindex(2));
    auto it = h->header.begin();

    for (int i = 0; i < idx; i++)
        it++;
    if (it != h->header.end()) {
        lua_pushinteger(L, ++idx);
        lua_replace(L, lua_upvalueindex(2));
        lua_pushstring(L, ((*it).first).c_str());
        lua_pushstring(L, ((*it).second).c_str());
        return 2;
    }
    return 0;
}

LUA_METHOD(headers, iterate) {
    luaL_getmetafield(L, 1, "http");
	lua_pushlightuserdata(L, lua_self(L, -1, Http));
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, headers_iterator, 2);
    return 1;
}

static const luaL_Reg headers[] = {
	{"__newindex",	headers_newindex},
	{"__index",		headers_index},
	{"__iterate",	headers_iterate},
	{NULL, NULL}
};

LUA_PROPERTY_GET(Http, headers) {
	lua_createtable(L, 0, 0);
	luaL_newlib(L, headers);
    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "http");
	lua_setmetatable(L, -2);
	return 1;
}

//----------------------------------[ Http.cookie property ]
LUA_PROPERTY_GET(Http, cookies) {
    Http *h = lua_self(L, 1, Http);
    std::string cookie;
    
    if (h->header.find("Cookie") == h->header.end())
        return 0;

    cookie = h->header["Cookie"];
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    
    lua_createtable(L, 0, 2);
    while ((pos = cookie.find(";", prev)) != std::string::npos) {
        push_headerfield(L, cookie.substr(prev, pos - prev), '=');
        prev = pos + 1;
    }
    return 1;
}


LUA_PROPERTY_SET(Http, cookies) {
    Http *h = lua_self(L, 1, Http);
    std::string result;

    luaL_checktype(L, 2, LUA_TTABLE);
    lua_pushnil(L);
    while (lua_next(L, 2)) {
        char *cookie;        
        DWORD size;
        if (!lua_isstring(L, -1) || !lua_isstring(L, -2))
            luaL_error(L, "malformed table, expecting string for key/value (found %s key and %s value)", luaL_typename(L, -2), luaL_typename(L, -1));
        std::string temp = lua_tostring(L, -2);
        temp += "=";
        temp += lua_tostring(L, -1);
        size = (luaL_len(L, -1)+luaL_len(L, -2))*2;
        cookie = (char*)calloc(1, size);
        if (!InternetCanonicalizeUrl((trim(temp)+";").c_str(), cookie, &size, ICU_BROWSER_MODE))            
            result += temp;
        else
            result += strdup(cookie);
        free(cookie);
        lua_pop(L, 1);
    }
    h->header["Cookie"] = result;
	return 0;
}

//----------------------------------[ Http.close() ]
LUA_METHOD(Http, close) {
	Http *h = lua_self(L, 1, Http);
	if (h->handle) {
		InternetCloseHandle(h->handle);
        h->handle = NULL;
    }
	return 0;
}

LUA_METHOD(Http, __gc) {
	Http *h = lua_self(L, 1, Http);
    h->Closing = true;
    if (h->handle)
        InternetCloseHandle(h->handle);
    if (h->hRequest)
        InternetCloseHandle(h->hRequest);
    if (h->hConnect)
        InternetCloseHandle(h->hConnect);
    InternetSetStatusCallback(h->handle, NULL);
    DeleteCriticalSection(&h->CriticalSection);        
    delete h;
	return 0;
}

//----------------------------------[ Http object definition ]
OBJECT_MEMBERS(Http)
    METHOD(Http, get)
    METHOD(Http, post)
    METHOD(Http, put)
    METHOD(Http, download)
    METHOD(Http, delete)
    METHOD(Http, proxy)
    METHOD(Http, close)
    READONLY_PROPERTY(Http, hostname)
    READONLY_PROPERTY(Http, port)
    READONLY_PROPERTY(Http, received)
    READONLY_PROPERTY(Http, content)
    READONLY_PROPERTY(Http, headers)
    READWRITE_PROPERTY(Http, cookies)
END

OBJECT_METAFIELDS(Http)
  METHOD(Http, __gc)
END

