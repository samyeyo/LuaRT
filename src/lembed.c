/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | lembed.c | LuaRT embed module implementation
*/

#include <luart.h>
#include <File.h>

#define MINIZ_HEADER_FILE_ONLY
#include <compression\lib\zip.h>

extern int Zip_extract(lua_State *L); 
struct zip_t *fs = NULL;
BYTE *datafs  = NULL;

struct zip_t *open_fs(void *ptr, size_t size) {
	struct zip_t *zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
	zip->level = MZ_DEFAULT_LEVEL;
	if (mz_zip_reader_init_mem(&zip->archive, ptr, size,  MZ_DEFAULT_LEVEL | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == FALSE) {
		free(zip);
		zip = NULL;
	}
	return zip;
}

//-------------------------------------------------[luaRT embeded package loader]
static void *fsload(lua_State *L, const char *fname) {
	 void *buff = NULL; 
	 int idx = 0;
	 size_t size;

	 if (zip_entry_open(fs, fname) == 0) {
        zip_entry_read(fs, &buff, &size);
		if (memcmp(buff, "\xEF\xBB\xBF", 3)==0)
			idx = 3;
		if (luaL_loadbuffer(L, buff+idx, size-idx, fname))
			lua_error(L);
		zip_entry_close(fs);
		return buff;
    }
	return NULL;
}

static int luart_fsloader(lua_State *L) {
	const char *modname = luaL_gsub(L, luaL_checkstring(L, 1), ".", "/");
	size_t len = strlen(modname)+6;
	void *buff;
	char *fname;

	fname = calloc(1, len);
	_snprintf(fname, len, "%s.lua", modname);
	if ( !(buff = fsload(L, fname)) ) {
		_snprintf(fname, len, "%s.wlua", modname);
		buff = fsload(L, fname);
	}
	if (!buff)
		lua_pushfstring(L, "no embedded module '%s' found", modname);
 	free(fname);
	free(buff);
	return 1;
 }

//-------------------------------------------------[luaL_embedclose() luaRT C API]
LUALIB_API int luaL_embedclose(lua_State *L) {
	free(datafs);
	return 0;
}

//-------------------------------------------------[luaL_embedopen() luaRT C API]
LUALIB_API int luaL_embedopen(lua_State *L, const wchar_t *exename) {
  DWORD read;
  HANDLE hFile;
  BYTE buff[4096];
  IMAGE_DOS_HEADER* dosheader;
#ifdef _WIN64
#define HEADERS	IMAGE_NT_HEADERS64
#else
#define HEADERS	IMAGE_NT_HEADERS32
#endif
  HEADERS* header;
  DWORD maxpointer = 0, exesize = 0;
  int i;  
  DWORD filesize;
  size_t fssize;
  IMAGE_SECTION_HEADER* sectiontable;

  luaL_embedclose(L);
  hFile = CreateFileW(exename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(INVALID_HANDLE_VALUE == hFile)
      return FALSE;
  ReadFile(hFile, buff, sizeof(buff), &read, NULL);
  dosheader = (IMAGE_DOS_HEADER*)buff;

  // Locate PE header
  header = (HEADERS*)((DWORD_PTR)buff + dosheader->e_lfanew);
  if(dosheader->e_magic != IMAGE_DOS_SIGNATURE || header->Signature != IMAGE_NT_SIGNATURE) {
    CloseHandle(hFile);
    return FALSE;
  }
  sectiontable = (IMAGE_SECTION_HEADER*)((BYTE*)header + sizeof(HEADERS));
  for(i = 0; i < header->FileHeader.NumberOfSections; i++) {
    if(sectiontable->PointerToRawData > maxpointer) {
      maxpointer = sectiontable->PointerToRawData;
      exesize = sectiontable->PointerToRawData +
        sectiontable->SizeOfRawData;
    }
    sectiontable++;
  }
  filesize = GetFileSize(hFile, NULL);
  SetFilePointer(hFile, exesize, NULL, FILE_BEGIN);
  if ((fssize = filesize - exesize)) {
	  datafs = malloc(fssize);
	  ReadFile(hFile, datafs, fssize, &read, NULL);
	  CloseHandle(hFile);
	  if ((fs = open_fs(datafs, fssize))) {
	  	lua_getglobal(L, "package");
		lua_getfield(L, -1, "searchers");
		lua_pushcfunction(L, luart_fsloader);
		lua_rawseti(L, -2, luaL_len(L, -2)+1);
		lua_pop(L, 2);
		return TRUE;
	  }
  }
  return FALSE;
}

LUA_METHOD(embed, File) {
    wchar_t tmp[MAX_PATH];

	GetTempPathW(MAX_PATH, tmp);
    lua_getglobal(L, "embed");
    lua_pushcfunction(L, Zip_extract);
    lua_getfield(L, -2, "zip");
    lua_pushvalue(L, 1);
    lua_pushwstring(L, tmp);
    lua_pcall(L, 3, LUA_MULTRET, 0);
    return 1;
}

static const luaL_Reg embedlib[] = {
	{"File",	embed_File},
	{NULL, NULL}
};

static const luaL_Reg embed_properties[] = {
	{NULL, NULL}
};

//-------------------------------------------------[luaopen_embed() "embed" module]
int luaopen_embed(lua_State *L) {
	lua_registermodule(L, "embed", embedlib, embed_properties, luaL_embedclose);
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaded");
    lua_getfield(L, -1, "compression");
    lua_pushstring(L, "zip");
    lua_getfield(L, -2, "Zip");
    lua_pushlightuserdata(L, fs);
    lua_pcall(L, 1, 1, 0);
    lua_rawset(L, -6);
    lua_pop(L, 3);
	return 1;
}
