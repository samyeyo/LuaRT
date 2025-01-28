/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | sys.c | LuaRT sys module
*/

#define LUA_LIB

#include <luart.h>
#include "lrtapi.h"

#include <locale.h>
#include <Buffer.h>
#include <Task.h>
#include <File.h>
#include <Directory.h>
#include <Pipe.h>
#include <Date.h>
#include <Com.h>
#include <wininet.h>
#include <winreg.h>
#include <shlobj.h>
#include <shlwapi.h>

/* ------------------------------------------------------------------------ */

/*Halt flags */
const char *halt_modes[] = { "logoff", "shutdown", "reboot",  NULL };
const unsigned long halt_values[] = { EWX_LOGOFF, EWX_SHUTDOWN | EWX_POWEROFF, EWX_REBOOT };
const char *reg_keys[] = { "HKEY_CLASSES_ROOT", "HKEY_CURRENT_CONFIG", "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE", "HKEY_USERS", NULL };
const HKEY reg_values[] = { HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS };
wchar_t temp_path[MAX_PATH];

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

//-------------------------------------[ sys.cmd() ]
LUA_METHOD(sys, cmd) {
	int len;
	wchar_t *exec = lua_tolwstring(L, 1, &len);
	size_t count = 12 + len;
	wchar_t *buff = malloc(sizeof(wchar_t)*count);
	STARTUPINFOW info = {0};
    PROCESS_INFORMATION procInfo = {0};
	HANDLE hJob;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION job = {0};

	hJob = CreateJobObject(NULL, NULL);
    job.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &job, sizeof(job));

	_snwprintf(buff, count, L"cmd.exe /C %s", exec);
    info.cb = sizeof(info);
	info.dwFlags = STARTF_USESHOWWINDOW;
    info.wShowWindow = SW_HIDE;
    if (CreateProcessW(NULL, buff, NULL, NULL, TRUE, lua_toboolean(L, 2) ? CREATE_NO_WINDOW : 0, NULL, NULL, &info, &procInfo)) {
		DWORD status = 0;
		if (!lua_toboolean(L, 3))
			AssignProcessToJobObject(hJob, procInfo.hProcess);
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		Sleep(100);
		GetExitCodeProcess(procInfo.hProcess,&status);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		lua_pushboolean(L, !status);
	} else lua_pushboolean(L, FALSE);
	CloseHandle(hJob);
	free(buff);
	free(exec);
	return 1;
}

static int pushtmp(lua_State *L, BOOL isdir) {
	wchar_t *prefix = NULL, tmp[MAX_PATH];
	if (lua_gettop(L))
		prefix = lua_towstring(L, 1);
	if (GetTempFileNameW(temp_path, prefix, 0, tmp)) {
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

//-------------------------------------[ sys.fsentry() ]
LUA_METHOD(sys, fsentry) {
	wchar_t *entry = lua_towstring(L, 1);
	DWORD dwAttrib = GetFileAttributesW(entry);

	if (dwAttrib != INVALID_FILE_ATTRIBUTES) {
		lua_pushvalue(L, 1);
		if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			lua_pushinstance(L, Directory, 1);
		else lua_pushinstance(L, File, 1);
	} else lua_pushnil(L);
	free(entry);
	return 1;
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

//-------------------------------------[ sys.error ]
LUA_API int luaL_getlasterror(lua_State *L, DWORD err) {
	char* msg = 0;
	HMODULE mod = NULL;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;

    if (err) {
    	if (err >= 12000) {
    		mod = GetModuleHandle("wininet.dll");
    		flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK;			
    	}
		DWORD size = FormatMessageA(flags, mod, err, locale, (LPSTR)&msg, 0, NULL);
		int len = MultiByteToWideChar(CP_ACP, 0, msg, size, NULL, 0);
		wchar_t *buff = (wchar_t*)malloc(len*sizeof(wchar_t*));
		MultiByteToWideChar(CP_ACP, 0, msg, size, buff, size);
		lua_pushlwstring(L, buff, len);		
		free(buff);
		LocalFree(msg);
	}
	else
		lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_GET(sys, error) {
	return luaL_getlasterror(L, GetLastError());
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

LUA_API HGLOBAL table_to_HDROPFormat(lua_State *L, int idx) {
    luaL_checktype(L, idx, LUA_TTABLE);
    size_t totalLength = sizeof(DROPFILES) + 1; // +1 for the double null terminator
    int numFiles = lua_rawlen(L, idx);

    for (int i = 1; i <= numFiles; ++i) {
        lua_rawgeti(L, idx, i);
        wchar_t *path = luaL_checkFilename(L, -1);
		totalLength += (wcslen(path)+1) * sizeof(wchar_t); // +1 for the null terminator
		free(path);
        lua_pop(L, 1);
    }

    // Allocate global memory for the DROPFILES structure
    HGLOBAL hGlobal;
    if (!(hGlobal = GlobalAlloc(GHND | GMEM_SHARE, totalLength)))
memerr:	luaL_error(L, "memory allocation failed");

    DROPFILES* dropFiles = (DROPFILES*)GlobalLock(hGlobal);
    if (!dropFiles) {
        GlobalFree(hGlobal);
        goto memerr;
    }

    dropFiles->pFiles = sizeof(DROPFILES);
    dropFiles->fWide = TRUE;

    wchar_t* dest = (wchar_t*)((BYTE*)dropFiles + sizeof(DROPFILES));
    for (int i = 1; i <= numFiles; ++i) {
        wchar_t* path = (lua_rawgeti(L, idx, i) == LUA_TSTRING) ? lua_towstring(L, -1) : luaL_checkFilename(L, -1);
		size_t length = wcslen(path);
		wcscpy_s(dest, length + 1, path);
		free(path);
		dest += length + 1;
        lua_pop(L, 1);
    }
    *dest = L'\0';
    GlobalUnlock(hGlobal);
    return hGlobal;
}

LUA_PROPERTY_GET(sys, clipboard) {
	HANDLE hData = NULL;
	char *kind = "unknown";

    if (OpenClipboard(NULL)) {
        if (IsClipboardFormatAvailable(CF_UNICODETEXT) && (hData = GetClipboardData(CF_UNICODETEXT))) {
            wchar_t* pszText = GlobalLock(hData); 
            if (pszText) {
				lua_pushwstring(L, pszText);
				kind = "text";
                GlobalUnlock(hData);
            }		
    	} else if (IsClipboardFormatAvailable(CF_TEXT) && (hData = GetClipboardData(CF_TEXT))) {
            char* pszText = GlobalLock(hData); 
            if (pszText) {
				lua_pushstring(L, pszText);
				kind = "text";
                GlobalUnlock(hData);
            }
		} else if (IsClipboardFormatAvailable(CF_HDROP) && (hData = GetClipboardData(CF_HDROP))) {
            UINT fileCount = DragQueryFileW((HDROP)hData, 0xFFFFFFFF, NULL, 0);
			kind = "files";
			lua_createtable(L, 0, fileCount);
            for (UINT i = 0; i < fileCount; ++i) {
                UINT pathLength = DragQueryFileW((HDROP)hData, i, NULL, 0) + 1;
                wchar_t* filePath = calloc(pathLength, sizeof(wchar_t));;
                DragQueryFileW((HDROP)hData, i, filePath, pathLength);
				lua_pushwstring(L, filePath);
				if ((PathIsDirectoryW(filePath) == FILE_ATTRIBUTE_DIRECTORY) || (filePath[pathLength-1] == L'\\'))
					lua_pushinstance(L, Directory, 1);
				else lua_pushinstance(L, File, 1);
				lua_remove(L, -2);
				lua_rawseti(L, -2, i+1);
                free(filePath);
            }
		}
    }
	lua_createtable(L, 2, 0);
	lua_pushstring(L, kind);
	lua_setfield(L, -2, "kind");
	if (hData) {
		lua_pushvalue(L, -2);
		lua_setfield(L, -2, "content");
	}
	CloseClipboard();
	return 1;
}

LUA_PROPERTY_SET(sys, clipboard) {
	int type = lua_type(L, 1);

    if (OpenClipboard(NULL)) {
		switch (lua_type(L, 1)) {
			case LUA_TSTRING:	if (EmptyClipboard()) {
									int length;
									wchar_t *pGlobal, *text = lua_tolwstring(L, 1, &length);
								    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, length * sizeof(wchar_t));
									if (hGlobal) {
										if (pGlobal = (wchar_t*)GlobalLock(hGlobal)) {
											wcscpy(pGlobal, text);
											GlobalUnlock(hGlobal);
											SetClipboardData(CF_UNICODETEXT, hGlobal);
										}
										GlobalFree(hGlobal);
									}
								} break;
			case LUA_TTABLE:	if (EmptyClipboard()) {
									HGLOBAL hGlobal = table_to_HDROPFormat(L, 1);
									SetClipboardData(CF_HDROP, hGlobal);
									GlobalFree(hGlobal);
								} break;
			case LUA_TNIL:		EmptyClipboard();
		}
	}
    CloseClipboard();
	return 0;
}

LUA_PROPERTY_GET(sys, currentdir) {
	wchar_t *path = GetCurrentDir();
	lua_pushwstring(L, path);
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

static int get_locale(lua_State *L, LCTYPE lc) {
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

    if (LCIDToLocaleName(locale, localeName, LOCALE_NAME_MAX_LENGTH, 0)) {
		int len = GetLocaleInfoEx(localeName, lc, NULL, 0);
  		wchar_t *localeData = calloc(len, sizeof(wchar_t));

		if (GetLocaleInfoEx(localeName, lc, localeData, len)) {
			lua_pushlwstring(L, localeData, len-1);
  			free(localeData);
			return 1;
		}
		free(localeData);
	}
	lua_pushnil(L);
  	return 1;
}

//------------------------------------ sys.language property
LUA_PROPERTY_GET(sysutils, language) {
  return get_locale(L, LOCALE_SLOCALIZEDDISPLAYNAME);
}

//------------------------------------ sys.locale property
LUA_PROPERTY_GET(sysutils, locale) {
  return get_locale(L, LOCALE_SNAME);
}

LUA_PROPERTY_SET(sysutils, locale) {
	wchar_t* lname = lua_towstring(L, 1);
	
	locale = LocaleNameToLCID(lname, 0);
    if (!locale)
		luaL_error(L, "unknown locale '%s'", lname);
    free(lname);
	return 0;
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

MODULE_FUNCTIONS(sys)
	METHOD(sys, beep)
	METHOD(sys, clock)
	METHOD(sys, exit)
	METHOD(sys, tempfile)
	METHOD(sys, tempdir)
	METHOD(sys, cmd)
	METHOD(sys, halt)
	METHOD(sys, fsentry)
END

MODULE_PROPERTIES(sys)
	READONLY_PROPERTY(sys, env)
	READONLY_PROPERTY(sys, registry)
	READONLY_PROPERTY(sys, error)
	READWRITE_PROPERTY(sys, currentdir)
	READWRITE_PROPERTY(sys, clipboard)
	READWRITE_PROPERTY(sys, atexit)
	READWRITE_PROPERTY(sysutils, locale)
  	READONLY_PROPERTY(sysutils, language)
END

LUAMOD_API int luaopen_sys(lua_State *L) {
	perfc = QueryPerformanceFrequency(&freq);
	elevated = FALSE;
	SetConsoleOutputCP(65001); 
	setlocale(LC_ALL, ".UTF8");
	setlocale(LC_TIME, "");
	lua_regmodule(L, sys);
	lua_regobjectmt(L, Task);
	lua_regobjectmt(L, File);
	lua_regobjectmt(L, Buffer);
	lua_regobjectmt(L, Pipe);
	lua_regobjectmt(L, Directory);
	lua_regobjectmt(L, Datetime);
	lua_regobjectmt(L, COM);
	GetTempPathW(MAX_PATH, temp_path);
	return 1;
}