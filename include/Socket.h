/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Socket.h | LuaRT Socket object header
*/

#pragma once

#include <ws2tcpip.h>
#include <luart.h>

#include <stdlib.h>

#define SECURITY_WIN32
#include <security.h>
#include <schnlsp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
	DWORD						WrapperVersion;
	SOCKET						sock;
	PCredHandle					pCredHandle;
	PCtxtHandle					pCtxtHandle;
	SecPkgContext_StreamSizes	sizes;
	SecBuffer					ExtraData;
	SecBuffer					RemainingDecryptData;
	PCCERT_CONTEXT				pCertContext;
	BOOL						acceptSuccess;
} TLS;

typedef struct {
	luart_type	type;
	SOCKET		sock;
	union {
		SOCKADDR_IN		addr;
		SOCKADDR_IN6	addr6;
	};
	char		ip[INET6_ADDRSTRLEN];
	BOOL		blocking;
	int			sizeaddr;
	BOOL		isServerContext;
	TLS			*tls;
	BOOL		read;
	BOOL		write;
	BOOL		error;
	int			protocol;
} Socket;
extern luart_type TSocket;

//---------------------------------------- Socket type
LUA_CONSTRUCTOR(Socket);
extern const luaL_Reg Socket_methods[];
extern const luaL_Reg Socket_metafields[];

#ifdef __cplusplus
}
#endif