/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | luart.c | LuaRT interpreter
*/

#include <windows.h>
#include <commctrl.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <malloc.h>
#include <stdint.h>
#include <luart.h>
#include "lrtapi.h"
#include <Task.h>
#include <wchar.h>
#include <stdlib.h>
#include <shlwapi.h>


#ifndef _MSC_VER
	#ifdef __cplusplus
	extern "C" {
	#endif
		extern int _CRT_glob;
		void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*);
	#ifdef __cplusplus
	}
	#endif
#endif

extern struct zip_t *fs;
extern BYTE *datafs;
static lua_State *L;
static WCHAR exename[MAX_PATH];

#ifdef RTC

#include <stdio.h>
#include <stdint.h>
#include <File.h>

int link(lua_State *L) {
	wchar_t *fname = lua_towstring(L, 1);
	wchar_t *fexe = lua_towstring(L, 2);
	HANDLE hFile = CreateFileW(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD FileSize = GetFileSize(hFile, NULL);
	BYTE *pBuffer = calloc(1, FileSize);
	DWORD dwBytesRead;
	HANDLE hExeFile;
	BOOL result = FALSE;
	
	if (ReadFile(hFile, pBuffer, FileSize, &dwBytesRead, NULL)) {
		hExeFile = BeginUpdateResourceW(fexe, FALSE);
		result = UpdateResource(hExeFile, RT_RCDATA, MAKEINTRESOURCE(100), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPVOID)pBuffer, FileSize);
	}
	EndUpdateResource(hExeFile, FALSE);
	CloseHandle(hFile);
	free(pBuffer);
	free(fname);
	free(fexe);
	return result;
}

#pragma pack(2)
struct resource_directory
{
	int8_t width;
	int8_t height;
	int8_t color_count;
	int8_t reserved;
	int16_t planes;
	int16_t bit_count;
	int32_t bytes_in_resource;
	int16_t id;
};

struct header
{
	int16_t reserved;
	int16_t type;
	int16_t count;
};

struct icon_header
{
	int8_t width;
	int8_t height;
	int8_t color_count;
	int8_t reserved;
	int16_t planes;
	int16_t bit_count;
	int32_t bytes_in_resource;
	int32_t image_offset;
};

struct icon_image
{
	BITMAPINFOHEADER header;
	RGBQUAD colors;
	int8_t xors[1];
	int8_t ands[1];
};

struct icon
{
	int count;
	struct header *header;
	struct resource_directory *items;
	struct icon_image **images;
};

static int update_exe_icon(lua_State *L) {
	
	wchar_t *exe_path = luaL_checkFilename(L, 1);
	wchar_t *ico_path = luaL_checkFilename(L, 2);
	FILE *file;
	BOOL result = FALSE;

	if ((file = _wfopen(ico_path, L"rb"))) {
		int i, id = 1;
		struct icon icon;
		HANDLE handle;
		struct header file_header;
   		if ((handle = BeginUpdateResourceW(exe_path, FALSE))) {
			fread(&file_header, sizeof(struct header), 1, file);
			icon.count = file_header.count;
			icon.header = malloc(sizeof(struct header) + icon.count * sizeof(struct resource_directory));
			icon.header->reserved = 0;
			icon.header->type = 1;
			icon.header->count = icon.count;
			icon.items = (struct resource_directory *)(icon.header + 1);
			struct icon_header *icon_headers = malloc(icon.count * sizeof(struct icon_header));
			fread(icon_headers, icon.count * sizeof(struct icon_header), 1, file);
			icon.images = malloc(icon.count * sizeof(struct icon_image *));

			for (i = 0; i < icon.count; i++) {
				struct icon_image** image = icon.images + i;
				struct icon_header* icon_header = icon_headers + i;
				struct resource_directory *item = icon.items + i;

				*image = malloc(icon_header->bytes_in_resource);
				fseek(file, icon_header->image_offset, SEEK_SET);
				fread(*image, icon_header->bytes_in_resource, 1, file);

				memcpy(item, icon_header, sizeof(struct resource_directory));
				item->id = (int16_t)(i + 1);
			}
			
			UpdateResource(handle, RT_GROUP_ICON, MAKEINTRESOURCE(101), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), icon.header, sizeof(struct header) + icon.count * sizeof(struct resource_directory));
			for (i = 0; i < icon.count; i++)
				UpdateResource(handle, RT_ICON, MAKEINTRESOURCE(id++), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), icon.images[i], icon.items[i].bytes_in_resource);
			result = EndUpdateResource(handle, FALSE);
			for (i = 0; i < icon.count; i++)
				free(icon.images[i]);
			free(icon.images);
			free(icon.header);
		}
		fclose(file);
	}
	free(ico_path);
	free(exe_path);
    return result;
}
#endif

#ifndef AIO
//------- LuaRT modules included in luart.exe/wluart.exe
	#if defined(LUART_STATIC) || defined(RTWIN)
		static luaL_Reg luaRT_libs[] = {
			#ifdef LUART_STATIC
				{"crypto",	luaopen_crypto },
				{"net",		luaopen_net },
			#endif
			#ifdef RTWIN
				{ "ui",				luaopen_ui },
			#endif
			{ NULL,		NULL }
		};
	#endif
#endif

void lua_stop(void) {
	if (L) {
		if (lua_getfield(L, LUA_REGISTRYINDEX, "atexit") == LUA_TFUNCTION) {
			if (lua_pcall(L, 0, 0, 0))
				puts(lua_tostring(L, -1));
		}
		lua_close(L);
		CoUninitialize();
		L = NULL;
	}
}

#ifdef RTWIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main() {
#endif
	INITCOMMONCONTROLSEX icex;
	int i, result = EXIT_SUCCESS;
	BOOL is_embeded = FALSE;
	wchar_t **envp, **wargv;
	int argc, si = 0, argfile = 1;
#ifdef _MSC_VER	
    static int (*__wgetmainargs)(int *, wchar_t ***, wchar_t ***, int, int *);
    HMODULE h = LoadLibraryA("msvcrt.dll");
    __wgetmainargs = (void *)GetProcAddress(h, "__wgetmainargs");
    (*__wgetmainargs)(&argc, &wargv, &envp, 1, &si);
    FreeLibrary(h);
#else	
	__wgetmainargs(&argc, &wargv, &envp, _CRT_glob, &si);
#endif
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	L = luaL_newstate();
	luaL_openlibs(L);
#ifndef AIO	
	#if defined(LUART_STATIC) || defined(RTWIN)
		const luaL_Reg *lib;
		for (lib = luaRT_libs; lib->func; lib++) {
			luaL_requiref(L, lib->name, lib->func, 0);
			lua_pop(L, 1);
		}
	#endif
#endif
	GetModuleFileNameW(NULL, (WCHAR*)exename, sizeof(exename));
	if ((is_embeded = luaL_embedopen(L, exename))) {
		luaL_requiref(L, "embed", luaopen_embed, 2);
		lua_pop(L, 1);
	}

	atexit(lua_stop);
#ifdef RTC
	lua_register(L, "seticon", update_exe_icon);
	lua_register(L, "link", link);
#endif
	if (argc == 1 && !is_embeded)
		puts(LUA_VERSION " " LUA_ARCH " - Windows programming framework for Lua.\nCopyright (c) 2023, Samir Tine.\nusage:\tluart.exe [-e statement] [script] [args]\n\n\t-e statement\tExecutes the given Lua statement\n\tscript\t\tRun a Lua script file\n\targs\t\tArguments for Lua interpreter");
	else {
		lua_createtable(L, argc, 0);	
		lua_pushwstring(L, exename);
		lua_rawseti(L, -2, 0);
		for (i=1; i < argc; i++) {
			lua_pushwstring(L, wargv[i]);
			lua_rawseti(L, -2, i);
		}
		lua_setglobal(L, "arg");
		lua_gc(L, LUA_GCGEN, 0, 0);
		if (is_embeded) {			
			if (luaL_dostring(L, "require '__mainLuaRTStartup__'"))
				goto error;
		}
		else {
			if (*wargv[1] == L'-') {
				if ((wargv[1][1] == L'e')) {
					if (argc > 2) {
						lua_pushwstring(L, wargv[2]);
						if (luaL_dostring(L, lua_tostring(L, -1))) {
							is_embeded = TRUE;
							goto error;
						}
						else if (__argc > 3) {
							argfile = 3;
							goto execscript;
						}
					} else
						fputs("error: -e option expects a statement argument\n", stderr);					
				} else {
					lua_pushstring(L, "error: unknown option ");
					lua_pushwstring(L, wargv[1]);
					lua_concat(L, 2);
					goto error;
				}
			} else { 
execscript:		if (luaL_loadfile(L, __argv[argfile]) == LUA_OK) {
					Task *t = lua_pushinstance(L, Task, 1);
					t->status = TRunning;
					if (lua_pcall(L, 0, 0, 0)) {
error:			
						{
#ifdef RTWIN
						const wchar_t *err = lua_towstring(L, -1);

						if (is_embeded) {
							wchar_t *err_embed;
							if ( (err_embed = wcsstr(err, L": ")) )
								err = err_embed + 2;
						}
						MessageBoxW(NULL, err, L"Runtime error", MB_ICONERROR | MB_OK);
						free(err);
#else
						const char *err = lua_tostring(L, -1);

						if (is_embeded) {
							char *err_embed;
							if ( (err_embed = strstr(err, ": ")) )
								err = err_embed + 2;
						}
						fputs(err, stderr);
						fputs("\n", stderr);
#endif
						result = EXIT_FAILURE;
						}
					} else do 
						lua_schedule(L);
					while (lua_status(t->L) != LUA_OK);
				} else goto error;
			}
		}
	}
	lua_stop();
	return result;
}
