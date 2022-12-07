/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | sys.c | LuaRT sys module
*/

#include <luart.h>
#include <locale.h>

#include <Buffer.h>
#include <File.h>
#include <Directory.h>
#include <Pipe.h>
#include <Date.h>
#include <Com.h>
#include <wininet.h>
#include <winreg.h>

/* ------------------------------------------------------------------------ */

/*Halt flags */
const char *halt_modes[] = { "logoff", "shutdown", "reboot",  NULL };
const unsigned long halt_values[] = { EWX_LOGOFF, EWX_SHUTDOWN | EWX_POWEROFF, EWX_REBOOT };
const char *reg_keys[] = { "HKEY_CLASSES_ROOT", "HKEY_CURRENT_CONFIG", "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE", "HKEY_USERS", NULL };
const HKEY reg_values[] = { HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS };

/* clock definitions */
static LARGE_INTEGER freq;
static BOOL perfc, elevated;

//-------------------------------------[ sys.clock() ]
LUA_METHOD(sys, clock) {
    if (perfc) {
		LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        lua_pushinteger(L, (lua_Integer)((1000LL * now.QuadPart) / freq.QuadPart));
    }
	else 
		lua_pushinteger(L, GetTickCount64());
	return 1;
}

//-------------------------------------[ sys.exit() ]
LUA_METHOD(sys, exit) {
	int ret = (int)luaL_optinteger(L, 1, EXIT_SUCCESS);
	exit(ret);
	return 0;
}

//-------------------------------------[ sys.sleep() ]
LUA_METHOD(sys, sleep) {
	Sleep((int)luaL_optinteger(L, 1, 1));
	return 0;
}

//-------------------------------------[ sys.cmd() ]
LUA_METHOD(sys, cmd) {
	int len;
	wchar_t *exec = lua_tolwstring(L, 1, &len);
	size_t count = 12 + len;
	wchar_t *buff = malloc(sizeof(wchar_t)*count);
	STARTUPINFOW info = {0};
    PROCESS_INFORMATION procInfo = {0};

	_snwprintf(buff, count, L"cmd.exe /C %s", exec);
    info.cb = sizeof(info);
	info.dwFlags = STARTF_USESHOWWINDOW;
    info.wShowWindow = SW_HIDE;
    if (CreateProcessW(NULL, buff, NULL, NULL, TRUE, lua_toboolean(L, 2) ? CREATE_NO_WINDOW : 0, NULL, NULL, &info, &procInfo)) {
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		GetExitCodeProcess(procInfo.hProcess,&procInfo.dwProcessId);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		lua_pushboolean(L, TRUE);
	} else lua_pushboolean(L, FALSE);
	free(buff);
	free(exec);
	return 1;
}

static int pushtmp(lua_State *L, BOOL isdir) {
	wchar_t *prefix = NULL, tmp[MAX_PATH], path[MAX_PATH];
	if (lua_gettop(L))
		prefix = lua_towstring(L, 1);
	GetTempPathW(MAX_PATH, path);
	if (GetTempFileNameW(path, prefix, 0, tmp)) {
		lua_pushwstring(L, tmp);
		if (isdir) {
			DeleteFileW(tmp);
			CreateDirectory(luaL_gsub(L, lua_tostring(L, -1), ".tmp", ""), NULL);
			lua_pushinstance(L, Directory, 1);
		}
		else
			lua_pushinstance(L, File, 1);
	}
	else
		lua_pushnil(L);
	free(prefix);
	return 1;
}
//-------------------------------------[ sys.tempfile() ]
LUA_METHOD(sys, tempfile) {
	return pushtmp(L, FALSE);
}

//-------------------------------------[ sys.tempdir() ]
LUA_METHOD(sys, tempdir) {
	return pushtmp(L, TRUE);
}

//-------------------------------------[ sys.beep ]
LUA_METHOD(sys, beep) {
	Beep(550, 200);
	return 0;
}

//-------------------------------------[ sys.halt ]
LUA_METHOD(sys, halt) {
  LUID luid;
  TOKEN_PRIVILEGES privs;
  HANDLE token;
   
  OpenProcessToken (GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);
  LookupPrivilegeValue (NULL, "SeShutdownPrivilege", &luid);   
  privs.PrivilegeCount = 1;
  privs.Privileges[0].Luid = luid;
  privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  AdjustTokenPrivileges (token, FALSE, &privs, 0, NULL, NULL);
  
  if ( ExitWindowsEx(halt_values[luaL_checkoption(L, 1, "shutdown", halt_modes)], 0) )
    luaL_error(L, "The system is shutting down NOW !!!");
  luaL_pushfail(L);
  return 1;
}

//-------------------------------------[ sys.lasterror ]
int lasterror(lua_State *L, DWORD err) {
	char* msg = 0;
	HMODULE mod = NULL;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;
    if (err) {
    	if (err >= 12000) {
    		mod = GetModuleHandle("wininet.dll");
    		flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE;
    		if (err == ERROR_INTERNET_EXTENDED_ERROR) {
    			char errmsg[MAX_PATH];
    			DWORD len = MAX_PATH;
				InternetGetLastResponseInfoA( &err, errmsg, &len );
				lua_pushlstring(L, errmsg, len);
				return 1;
    		}
    	}
		size_t size = FormatMessageA(flags, mod, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
		lua_pushlstring(L, msg, size);
		LocalFree(msg);
	}
	else
		lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_GET(sys, lasterror) {
	return lasterror(L, GetLastError());
}

LUA_PROPERTY_GET(sys, atexit) {
	lua_getfield(L, LUA_REGISTRYINDEX, "atexit");
	return 1;
}

LUA_PROPERTY_SET(sys, atexit) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushvalue(L, 1);
	lua_setfield(L, LUA_REGISTRYINDEX, "atexit");
	return 0;
}

LUA_PROPERTY_GET(sys, clipboard) {
	HANDLE hData;
	wchar_t *str;

	lua_pushnil(L);
	if (OpenClipboard(NULL) && (hData = GetClipboardData(CF_UNICODETEXT)) && (str = GlobalLock(hData))) {
		lua_pushwstring(L, str);
		GlobalUnlock(hData);
	}
	CloseClipboard();
	return 1;
}

LUA_PROPERTY_SET(sys, clipboard) {
	int len;
	wchar_t *str = lua_tolwstring(L, 1, &len);
	HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, (++len)*sizeof(wchar_t));
	memcpy(GlobalLock(hMem), str, len*sizeof(wchar_t));
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	return 0;
}

LUA_PROPERTY_GET(sys, currentdir) {
	size_t size = GetCurrentDirectoryW(0, NULL);
	wchar_t *path = malloc(sizeof(wchar_t)*size);
	GetCurrentDirectoryW(size, path);
	lua_pushlwstring(L, path, size-1);
	free(path);
	return 1;
}

LUA_PROPERTY_SET(sys, currentdir) {
	wchar_t *path = lua_towstring(L, 1);
	SetCurrentDirectoryW(path);
	free(path);
	return 0;
}

LUA_METHOD(env, newindex) {
	int lvar, lvalue;
	wchar_t *var, *value, *buff;
	size_t len;

	if (lua_type(L, 2) != LUA_TSTRING)
		luaL_error(L, "invalid index in sys.env (string expected, got %s)", luaL_typename(L, 2));	
	if (lua_type(L, 3) != LUA_TSTRING)
		luaL_error(L, "invalid value setting sys.env (string expected, got %s)", luaL_typename(L, 3));
	var = lua_tolwstring(L, 2, &lvar);
	value = lua_tolwstring(L, 3, &lvalue);
	len = lvar+lvalue+2;
	buff = calloc(len, sizeof(wchar_t));
	_snwprintf(buff, len, L"%s=%s", var, value);
	_wputenv(buff);
	free(buff);
	free(var);
	free(value);
	return 0;
}

LUA_METHOD(env, index) {
	wchar_t *var, *value;
	if (lua_type(L, 2) != LUA_TSTRING)
		luaL_error(L, "invalid index in sys.env (string expected, got %s)", luaL_typename(L, 2));	
	var = lua_towstring(L, 2);
	value = _wgetenv(var);
	if (value)
		lua_pushwstring(L, value);
	free(var);
	return value != NULL;
}

static const luaL_Reg sys_env[] = {
	{"__index",		env_index},
	{"__newindex",	env_newindex},
	{NULL, NULL}
};

LUA_PROPERTY_GET(sys, env) {
	lua_createtable(L, 0, 0);
	luaL_newlib(L, sys_env);
	lua_setmetatable(L, -2);
	return 1;
}

#define RRF_SUBKEY_WOW6464KEY 0x00010000
#define RRF_SUBKEY_WOW6432KEY 0x00020000

LUA_METHOD(reg, read) {
	DWORD type, size = 0;
	int n = lua_gettop(L);
	HKEY rootkey;
	wchar_t *subkey = lua_towstring(L, 2);
	wchar_t *value = (n == 2) || lua_isnil(L, 3) ? NULL : lua_towstring(L, 3);
	DWORD flags = (n == 5) && lua_toboolean(L, 5) ? RRF_RT_ANY : RRF_RT_ANY | RRF_NOEXPAND;

	lua_pushnil(L);
	if ( RegOpenKeyExW(reg_values[luaL_checkoption(L, 1, "HKEY_CURRENT_USER", reg_keys)], subkey, 0, KEY_WOW64_64KEY | KEY_READ, &rootkey) == ERROR_SUCCESS) {	
		if (RegGetValueW(rootkey, L"", value, flags, &type, NULL, &size) == ERROR_SUCCESS) {
			PBYTE buff = malloc(size); 
			if (RegGetValueW(rootkey, L"", value, flags, &type, buff, &size) == ERROR_SUCCESS) {
				switch (type) { 
					case REG_QWORD: 			lua_pushnumber(L, (lua_Number)*((unsigned __int64*)buff)); break;
					case REG_DWORD:				lua_pushinteger(L, (lua_Integer)*((DWORD*)buff)); break;
					case REG_EXPAND_SZ:
					case REG_LINK:
					case REG_SZ:				lua_pushwstring(L, (wchar_t*)buff); break;
					case REG_MULTI_SZ:	    	{
													__int64  start = 0, x = -1, idx = 0;
													wchar_t *str = (wchar_t*)buff;
													lua_createtable(L, 0, 0);
													size = size/sizeof(wchar_t);
													while(x < size) {
														if (str[++x] == L'\0') {
															lua_pushlwstring(L, str+start, x-start);
															lua_rawseti(L, -2, ++idx);
															start = x+1;
															if (str[x+1] == L'\0')
																break;
														}
													} 
												}	break;
					case REG_BINARY:			lua_pushbuffer(L, buff, size);
				}
			}
			free(buff);
		}
		RegCloseKey(rootkey);
	}
	free(value);
	free(subkey);
	return 1;
}

LUA_METHOD(reg, write) {
	DWORD type = REG_NONE, size;
	int len;
	HKEY rootkey = reg_values[luaL_checkoption(L, 1, "HKEY_CURRENT_USER", reg_keys)];
	wchar_t *subkey = lua_towstring(L, 2);
	wchar_t *value = lua_isnil(L, 3) ? NULL : lua_towstring(L, 3);
	luaL_Buffer b;
	const char *str;
	int n = lua_gettop(L);

	luaL_buffinit(L, &b);
	if (n < 4)
		luaL_argerror(L, 4, "registry data expected");
	switch(lua_type(L, 4)) {
		case LUA_TNUMBER: 	if (lua_isinteger(L, 4)) {
								DWORD v = lua_tointeger(L, 4);
								luaL_addlstring(&b, (const char*)&v, 4);
								type = REG_DWORD;
							} else {
								unsigned __int64 v = (unsigned __int64)lua_tonumber(L, 4);
								luaL_addlstring(&b, (const char*)&v, 8);
								type = REG_QWORD;
							} break;
		case LUA_TSTRING:	{
								wchar_t *str = lua_tolwstring(L, 4, &len);
								luaL_addlstring(&b, (const char*)str, (++len)*sizeof(wchar_t));
								type = (n == 5) && lua_toboolean(L, 5) ? REG_EXPAND_SZ : REG_SZ;
								break;
							}							
		case LUA_TTABLE:	{
								luart_type t;
								Buffer *buff = lua_tocinstance(L, 4, &t);
								if (buff && (t == TBuffer)) {
									luaL_addlstring(&b, (const char*)buff->bytes, buff->size);
									type = REG_BINARY;
								} else {
									lua_pushvalue(L, 4);
									lua_pushnil(L);
									type = REG_MULTI_SZ;
									while(lua_next(L, -2)) {
										int len;
										wchar_t *str;
										if (!lua_isstring(L, -1))
											luaL_argerror(L, 4, "table of strings");
										str = lua_tolwstring(L, -1, &len);
										luaL_addlstring(&b, (const char *)str, (++len)*sizeof(wchar_t));								
										lua_pop(L, 1);
									}
								}							
								break;
							}
		case LUA_TNIL:		type = REG_NONE;
							break;
	}
	size = b.n;
	luaL_pushresult(&b);
	str = lua_tostring(L, -1);
	lua_pushboolean(L, RegSetKeyValueW(rootkey, subkey, value, type, str, size) == ERROR_SUCCESS);
	free(value);
	free(subkey);
	return 1;
}

LUA_METHOD(reg, delete) {
	BOOL remove_key = lua_gettop(L) == 2;
	HKEY key, rootkey = reg_values[luaL_checkoption(L, 1, "HKEY_CURRENT_USER", reg_keys)];
	wchar_t *subkey = lua_towstring(L, 2);
	wchar_t *value = remove_key ? NULL : lua_towstring(L, 3);
	BOOL result;
	if (remove_key) {
		if ( RegOpenKeyExW(rootkey, NULL, 0, KEY_WOW64_64KEY | KEY_ENUMERATE_SUB_KEYS | DELETE | KEY_QUERY_VALUE, &key) == ERROR_SUCCESS ) {
			result = RegDeleteTreeW(key, subkey);
		}
	} else result = RegDeleteKeyValueW(rootkey, subkey, value);
	free(subkey);
	free(value);
	lua_pushboolean(L, result == ERROR_SUCCESS);
	return 1;
}
static const luaL_Reg sys_reg[] = {
	{"read",	reg_read},
	{"write",	reg_write},
	{"delete",	reg_delete},
	{NULL, NULL}
};

LUA_PROPERTY_GET(sys, registry) {
	luaL_newlib(L, sys_reg);
	return 1;
}

/* ------------------------------------------------------------------------ */

static const luaL_Reg syslib[] = {
	{"beep",		sys_beep},
	{"clock",		sys_clock},
	{"exit",		sys_exit},
	{"sleep",		sys_sleep},
	{"tempfile",	sys_tempfile},
	{"tempdir",		sys_tempdir},
	{"cmd",			sys_cmd},
	{"halt",		sys_halt},
	{NULL, NULL}
};

static const luaL_Reg sys_properties[] = {
	{"get_env",			sys_getenv},
	{"get_registry",	sys_getregistry},
	{"get_error",		sys_getlasterror},
	{"get_currentdir",	sys_getcurrentdir},
	{"set_currentdir",	sys_setcurrentdir},
	{"get_clipboard",	sys_getclipboard},
	{"set_clipboard",	sys_setclipboard},
	{"get_atexit",		sys_getatexit},
	{"set_atexit",		sys_setatexit},
	{NULL, NULL}
};

LUAMOD_API int luaopen_sys(lua_State *L) {
	perfc = QueryPerformanceFrequency(&freq);
	elevated = FALSE;
	SetConsoleOutputCP(65001); 
	setlocale(LC_ALL, ".UTF8");
	setlocale(LC_TIME, "");
	lua_regmodule(L, sys);
	lua_regobjectmt(L, File);
	lua_regobjectmt(L, Buffer);
	lua_regobjectmt(L, Pipe);
	lua_regobjectmt(L, Directory);
	lua_regobjectmt(L, Datetime);
	lua_regobjectmt(L, COM);
	return 1;
}