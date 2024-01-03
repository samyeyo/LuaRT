/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | net.c | LuaRT net module
*/

#define LUART_LIB

#include <Socket.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <luart.h>
#include <File.h>
#include "Http.h"
#include "Ftp.h"

#include <windns.h>
#include <Ws2ipdef.h>
#include <wininet.h>
#include <stdio.h>
#include <sensapi.h>
#include <time.h>
#include <iphlpapi.h>

static WSADATA wsadata;
static const char *ip_type[] = {"ipv4", "ipv6"};

extern "C" {
	extern int dns(lua_State *L, const char *str, WORD type);
}

LUA_METHOD(net, resolve) {
	return dns(L, (char *)luaL_checkstring(L,1), lua_optstring(L, 2, ip_type, 0) ? DNS_TYPE_AAAA : DNS_TYPE_A);
}

char *reverse_ip6(struct in6_addr *in6, int count)
{
  char hex[] = "0123456789abcdef";
  static char buff[65] = {0};
  int i = 0;
  int nibble = 0;

  while (nibble < count)
    {
      if (i)
        buff[i++] = '.';
      buff[i++] = nibble % 2 ? hex[in6->s6_addr[15 - nibble / 2] / 16] : hex[in6->s6_addr[15 - nibble / 2] % 16];
      nibble++;
    }
  return buff;
}

LUA_METHOD(net, reverse) {
	struct sockaddr_in sa;
	struct sockaddr_in6 sa6;
	char *ip = (char*)luaL_checkstring(L, 1);
	char final_ip[100];
	const char *type = NULL;
	char ip2[29];
    
	if ( inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0 ) {
		int a,b,c,d, result;
		result = sscanf(ip,"%d.%d.%d.%d",&a,&b,&c,&d);
		if (result == EOF || result != 4)
			luaL_error(L, "malformed IPv4 address");
		_snprintf(ip2, 29, "%d.%d.%d.%d", d, c, b, a);
		ip = ip2;
		type = "IN-ADDR.ARPA";
	}
	else if ( inet_pton(AF_INET6, ip, &(sa6.sin6_addr)) != 0 ) {
		type = "IP6.ARPA";
		ip = reverse_ip6((struct in6_addr *)&sa6.sin6_addr.s6_addr, 32);
	}
	else
		luaL_error(L, "ip adress '%s' is not a valid IPv4/IPv6 address", ip);
	_snprintf(final_ip, 100, "%s.%s", ip, type);
	return dns(L, final_ip, DNS_TYPE_PTR);
}

LUA_METHOD(net, select) {
	fd_set read, write, err;
	TIMEVAL timeout = {};
	int result, i, idx = 0;
	Socket **list;

	ZeroMemory(&timeout, sizeof(TIMEVAL));
	luaL_checktype(L, 1, LUA_TTABLE);
	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_ZERO(&err);
	timeout.tv_usec = luaL_optinteger(L, 2, 0);
	list = (Socket**)malloc(sizeof(Socket)*(size_t)luaL_len(L, 1));
	lua_pushvalue(L, 1);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		Socket *s = lua_self(L, -1, Socket);
		FD_SET(s->sock, &read);
		FD_SET(s->sock, &write);
		FD_SET(s->sock, &err);
		list[idx++] = s;
		lua_pop(L, 1);
	}
	if (( result = select(0, &read, &write, &err, &timeout) > 0)) {
		lua_pushboolean(L, TRUE);	//----- events happened
		for (i = 0; i<idx; i++) {
			Socket *s = list[i];
			s->read = FD_ISSET(s->sock, &read);
			s->write = FD_ISSET(s->sock, &write);
			s->error = FD_ISSET(s->sock, &err);
		}
	}
	else if (result == SOCKET_ERROR) 
		lua_pushboolean(L, FALSE);	//----- network error
	else 
		luaL_pushfail(L); //----- nothing happened
	free(list);
	return 1;
}

URL_COMPONENTSW *get_url(lua_State *L, int idx, wchar_t **url_str) {
	int len;
	URL_COMPONENTSW *url = (URL_COMPONENTSW *)calloc(1, sizeof(URL_COMPONENTSW));
	
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

LUA_METHOD(net, urlparse) {
	wchar_t *urlstr;
	URL_COMPONENTSW *url;
	int n = lua_gettop(L);
	
	if ((url = get_url(L, 1, &urlstr))) {
		if (url->dwSchemeLength)
			lua_pushlwstring(L, url->lpszScheme, url->dwSchemeLength);
		if (url->dwHostNameLength)
			lua_pushlwstring(L, url->lpszHostName, url->dwHostNameLength);
		if (url->dwUrlPathLength)
			lua_pushlwstring(L, url->lpszUrlPath, url->dwUrlPathLength);
		if (url->dwExtraInfoLength)
			lua_pushlwstring(L, url->lpszExtraInfo, url->dwExtraInfoLength);
	} else lua_pushboolean(L, FALSE);
	free(url);
	free(urlstr);
	return lua_gettop(L)-n;
}

LUA_METHOD(net, getmime) {
	LPWSTR mime = NULL;
	wchar_t *uri = luaL_checkFilename(L, 1);
	if (SUCCEEDED(FindMimeFromData(NULL, uri, NULL, 0, NULL, FMFD_URLASFILENAME, &mime, 0))) {
		lua_pushwstring(L, mime);
		CoTaskMemFree(mime);
	}
	else lua_pushnil(L);
	free(uri);
	return 1;
}

LUA_PROPERTY_GET(net, isalive) {
	DWORD flags = NETWORK_ALIVE_LAN;
	lua_pushboolean(L, IsNetworkAlive(&flags));
	return 1;
}

 LUA_PROPERTY_GET(net, publicip) {
	 dns(L, "resolver1.opendns.com", DNS_TYPE_A);
	 if (lua_isstring(L, -1)) {
		 PDNS_RECORD pDnsRecord;
		 PIP4_ARRAY pSrvList = (PIP4_ARRAY)LocalAlloc(LPTR,sizeof(IP4_ARRAY));
		 IN_ADDR ipaddr;

		 pSrvList->AddrCount = 1;
		 pSrvList->AddrArray[0] = inet_addr(lua_tostring(L, -1));
		 if ( pSrvList->AddrArray[0] != INADDR_NONE )
			 if (DnsQuery_A("myip.opendns.com", DNS_TYPE_A, DNS_QUERY_BYPASS_CACHE, pSrvList, &pDnsRecord, NULL) == 0) {
				 ipaddr.S_un.S_addr = (pDnsRecord->Data.A.IpAddress);
				 lua_pushstring(L, inet_ntoa(ipaddr));
				 return 1;
			 }
		LocalFree(pSrvList);
	 }
	 lua_pushboolean(L, FALSE);
	 return 1;
 }
 
LUA_PROPERTY_GET(net, ip) {
	char hname[255];
	struct hostent *host_entry;
	gethostname(hname, 255);
	host_entry=gethostbyname(hname);
	lua_pushstring(L, inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list));
	return 1;
}

static int adapters_iter(lua_State *L) {
	PIP_ADAPTER_ADDRESSES addritem = (PIP_ADAPTER_ADDRESSES)lua_touserdata(L, lua_upvalueindex(1));
	do {
		if (addritem && addritem->IfType != IF_TYPE_SOFTWARE_LOOPBACK && addritem->OperStatus == IfOperStatusUp) {
			char buffer[128];
			PIP_ADAPTER_UNICAST_ADDRESS_LH addr;
			int i;
			
			memset(buffer, 0, 128);
			lua_pushwstring(L, addritem->Description);
			lua_createtable(L, 2, 0);
			addr = addritem->FirstUnicastAddress;
			for (i = 0; addr != NULL; i++) {
				getnameinfo(addr->Address.lpSockaddr, addr->Address.iSockaddrLength, buffer, 128, NULL, 0, NI_NUMERICHOST);
				lua_pushstring(L, buffer);
				lua_rawseti(L, -2, i+1);
				addr = addr->Next;
			}
			lua_pushlightuserdata(L, addritem->Next);
			lua_replace(L, lua_upvalueindex(1));
			return 2;	
		} 
	} while ((addritem = addritem->Next));
	free(lua_touserdata(L, lua_upvalueindex(2)));
	return 0;
}
 
LUA_METHOD(net, adapters) {
	DWORD size;

	if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST, NULL, NULL, &size) == ERROR_BUFFER_OVERFLOW) {
		PIP_ADAPTER_ADDRESSES addrlist = (PIP_ADAPTER_ADDRESSES)malloc(size);
		if (GetAdaptersAddresses(lua_optstring(L, 2, ip_type, 0) ? AF_INET6 : AF_INET, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST, NULL, addrlist, &size) == ERROR_SUCCESS) {
			lua_pushlightuserdata(L, addrlist);
			lua_pushlightuserdata(L, addrlist);
			lua_pushcclosure(L, adapters_iter, 2);
			return 1;	
		}
	}
	return 0;
}

LUA_PROPERTY_GET(net, ipv6) {
	return dns(L, "localhost", DNS_TYPE_AAAA);
}

LUA_PROPERTY_GET(net, error) {
	return luaL_getlasterror(L, WSAGetLastError());
}

static const luaL_Reg net_properties[] = {
	{"get_error",	net_geterror},
	{"get_isalive",	net_getisalive},
	{"get_ip",		net_getip},
	{"get_publicip",net_getpublicip},
	{NULL, NULL}
};

static const luaL_Reg netlib[] = {
	{"getmime",		net_getmime},
	{"select",		net_select},
	{"resolve",		net_resolve},
	{"reverse",		net_reverse},
	{"urlparse",	net_urlparse},
	{"adapters",	net_adapters},
	{NULL, NULL}
};

int net_finalize(lua_State *L) {
	WSACleanup();
	return 0;
}

//------------------------------- net module entry

extern "C" {
	int __declspec(dllexport) luaopen_net(lua_State *L) {
		WSAStartup(MAKEWORD(2, 2), &wsadata); 
		lua_regmodulefinalize(L, net);
		lua_regobjectmt(L, Socket);
		lua_regobjectmt(L, Http);
		lua_regobjectmt(L, Ftp);
		return 1;
	}
}