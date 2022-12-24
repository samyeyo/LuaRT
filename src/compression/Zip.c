
/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Zip.c | LuaRT Zip object implementation
*/

#include <Zip.h>
#include <File.h>
#include <Directory.h>
#include <Buffer.h>
#include "lrtapi.h"
#include <luart.h>

#include <stdio.h>
#include <wchar.h>
#include <shlwapi.h>

#include "lib\zip.h"
#include <compression\lib\zip.h>


const char *zip_modes[] = { "read", "append", "write", "delete", NULL };
extern struct zip_t *fs;

/* ------------------------------------------------------------------------ */

LUALIB_API LUA_CONSTRUCTOR(Zip) {
	Zip *z;
	struct zip_t *zip;
	int level, idx;
	wchar_t *fname = NULL;
	char mode;
	
	if (lua_islightuserdata(L, 2)) {
		z = calloc(1, sizeof(Zip));	
		z->zip = lua_touserdata(L, 2);
		z->mode = 'r';
		goto done;
	} else {
		level = luaL_optint(L, 4, MZ_DEFAULT_COMPRESSION);
		idx = luaL_checkoption(L, 3, "read", zip_modes);
		fname = luaL_checkFilename(L, 2);
		mode = *zip_modes[idx];
		if ( (zip = zip_open(fname, level, mode))) {
			z = calloc(1, sizeof(Zip));	
			z->zip = zip;
			z->mode = mode;
			z->fname = fname;
done:		lua_newinstance(L, z, Zip);
		} else {
			free(fname);
			luaL_error(L, zip_lasterror(zip));
		}
	}
	return 1;
}

LUA_METHOD(Zip, close) {
	Zip *z = lua_self(L, 1, Zip);
	if (z->fname)
		zip_close(z->zip);
	z->zip = NULL;
	return 0;
}

LUA_METHOD(Zip, __gc) {
	Zip *z = lua_self(L, 1, Zip);
	zip_close(z->zip);
	free(z->fname);
	free(z);
	return 0;
}

LUA_PROPERTY_GET(Zip, count) {
	lua_pushinteger(L, zip_entries_total(lua_self(L, 1, Zip)->zip));
	return 1;
}

LUA_PROPERTY_GET(Zip, size) {
	lua_pushinteger(L, lua_self(L, 1, Zip)->zip->archive.m_archive_size);
	return 1;
}

LUA_PROPERTY_GET(Zip, error)
{
	lua_pushstring(L, zip_lasterror(lua_self(L, 1, Zip)->zip));
	return 1;
}

LUA_PROPERTY_GET(Zip, iszip64) {
	lua_pushboolean(L, zip_is64(lua_self(L, 1, Zip)->zip));
	return 1;
}

LUA_METHOD(Zip, reopen) {
	Zip *z = lua_self(L, 1, Zip);
	char mode;
	
	if (fs && (z->zip == fs))
		luaL_error(L, "cannot reopen bundled Zip archive");
	mode = *zip_modes[luaL_checkoption(L, 2, "read", zip_modes)];
	zip_close(z->zip);
	z->level = luaL_optint(L, 3, MZ_DEFAULT_COMPRESSION);
	if ( (z->zip = zip_open(z->fname, z->level, mode)))
		z->mode = mode;
	else luaL_error(L, strerror(errno));
	return 0;
}

#define max(a,b) (((a) > (b)) ? (a) : (b))

wchar_t *remove_trailing_sep(wchar_t *str) {
	size_t len = wcslen(str);
	while (str[--len] == L'/' || str[len] == L'\\')
		str[len] = L'\0';
	return str;
}

static wchar_t *append_path(wchar_t *str, wchar_t* path) {
	size_t len = wcslen(str) + wcslen(path)+2;
	wchar_t *result = calloc(sizeof(wchar_t)*len, 1);
	_snwprintf(result, len, L"%s/%s", str, path);
	return result;
}

static void make_dir_path(Zip *z, char *dir) {
	char *start = dir;
	char ch;
	while((ch = *dir))
	{
		dir++;
		if (ch == '\\' || ch == '/') {
			ch = *dir;
			*dir = 0;
			if (mz_zip_reader_locate_file(&z->zip->archive, start, NULL, 0) < 0) {
				zip_entry_open(z->zip, start);
				zip_entry_close(z->zip);
			}
			*dir = ch;
		}
	}
}

static BOOL write_dir(Zip *z, wchar_t *dir, BOOL isfullpath, wchar_t* dest) {
	HANDLE hFind;
	WIN32_FIND_DATAW FindFileData;
	wchar_t *path;
	BOOL result = TRUE;

	if (dest) {
		int s = -1;
		wchar_t *trailing = append_path(dest, L"");
		char *dirname = wchar_toutf8(trailing, &s);
		make_dir_path(z, dirname);
        free(trailing);
        free(dirname);
	}
	path = append_path(remove_trailing_sep(dir), L"*");
	if ((hFind = FindFirstFileW(path, &FindFileData)) != INVALID_HANDLE_VALUE) {
	    do {
			if ( !(FindFileData.cFileName[0] == L'.' && (FindFileData.cFileName[1] == L'.' || FindFileData.cFileName[1] == 0)) ) {
				wchar_t *newpath = append_path(dir, FindFileData.cFileName);
				wchar_t *newdest = dest ? append_path(dest, FindFileData.cFileName) : NULL;

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					result &= write_dir(z, newpath, isfullpath, newdest ?: FindFileData.cFileName);
				else {
					int s = -1;
					if ((FindFileData.nFileSizeHigh && FindFileData.nFileSizeLow) || (wcscmp(FindFileData.cFileName, z->fname) != 0)) {
						char *entry = wchar_toutf8(newdest ?: FindFileData.cFileName, &s);
						result &= !zip_entry_open(z->zip, entry) && !zip_entry_fwrite(z->zip, newpath);
	        			zip_entry_close(z->zip);
			    		free(entry);
					}
				}
				free(newpath);
				free(newdest);				
			}
	  	} while (FindNextFileW(hFind, &FindFileData));
	    FindClose(hFind);
	}
	free(path);
	return result;
}

LUA_METHOD(Zip, write) {
	Zip *z = lua_self(L, 1, Zip);
	int is_entry = lua_gettop(L) == 3;
	BOOL result = FALSE;
	DWORD attrib = 0;
	wchar_t *dest = is_entry ? lua_towstring(L, 3) : NULL;
	const char *dest_entry = is_entry ? luaL_checkstring(L, 3) : NULL;
	char *entry = NULL;
	int size = -1;
	wchar_t *fname = NULL;
	
	if (lua_isstring(L, 2)) {
		fname = lua_towstring(L, 2);		
		attrib = GetFileAttributesW(fname);
		if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)) {
			result = write_dir(z, fname, !PathIsRelativeW(fname), dest);
			goto done;
		}
		else
			result = (zip_entry_open(z->zip, dest_entry ?: PathFindFileNameA(lua_tostring(L, 2))) == 0) && (attrib != INVALID_FILE_ATTRIBUTES && ((zip_entry_fwrite(z->zip, fname) == 0) || luaL_error(L, strerror(errno))));	
	} else {
		luart_type t;
		void *obj;
		
		if ( !(obj = lua_tocinstance(L, 2, &t)) )
			luaL_typeerror(L, 1, "String, Buffer, File or Directory");
		if (t == TBuffer) {
			size_t len;
			const char *buff = luaL_tolstring(L, 2, &len);
			if (!dest_entry)
				luaL_argerror(L, 3, "string expected when writing Buffer to Zip archive");
			result = (zip_entry_open(z->zip, dest_entry) == 0) && ((zip_entry_write(z->zip, buff, len) == 0) || luaL_error(L, strerror(errno)));				
		}
		else if (t == TFile) {
			size = -1;
			entry = wchar_toutf8(PathFindFileNameW(((File*)obj)->fullpath), &size);
			result = (zip_entry_open(z->zip, dest_entry ?: entry) == 0) && ((zip_entry_fwrite(z->zip, ((File*)obj)->fullpath) == 0) || luaL_error(L, strerror(errno)));	
			free(entry);
		}	
		else if (t == TDirectory) {
			result = write_dir(z, lua_self(L, 2, Directory)->fullpath, TRUE, dest);
			goto done;
		}			
	}
	zip_entry_close(z->zip);
done:
	free(fname);
	free(dest);
	lua_pushboolean(L, result);
	return 1;
}

static size_t on_extract(void *arg, unsigned long long offset, const void *data, size_t size) {
	luaL_addlstring((luaL_Buffer *)arg, data, size);
    return size; 
}

LUA_METHOD(Zip, read) {
	Zip *z = lua_self(L, 1, Zip);
	luaL_Buffer b;
	
	lua_pushboolean(L, FALSE);
	if (z->mode != 'r')
		luaL_error(L, "cannot read a Zip archive opened in write/append mode");
	
	if (zip_entry_open(z->zip, luaL_checkstring(L, 2)) == 0) {
		if (!zip_entry_isdir(z->zip)) {
			luaL_buffinit(L, &b);
			if (zip_entry_extract(z->zip, on_extract, &b) == 0) {
				luaL_pushresult(&b);
				lua_pushinstance(L, Buffer, 1);
			}
		} else mz_zip_set_last_error(&z->zip->archive, MZ_ZIP_FILE_NOT_FOUND);
		zip_entry_close(z->zip);
	}
	return 1;
}

extern BOOL make_path(wchar_t *folder);

__int64 extract_zip(struct zip_t *z, const char *dir) {
	int	entry = 0, slen;
	size_t size = 0, len = dir ? strlen(dir) : 0, count = 0;
	BOOL success = TRUE;

	while(success && (zip_entry_openbyindex(z, entry++) == 0)) {
		const char *name = zip_entry_name(z);
		size = strlen(name);
		char *fname = malloc(++size);
		wchar_t *wfname;
		int i;
		
		if (dir && (strncmp(dir, name, len) == 0)) {
			count++;
			goto next;
		}
		strncpy(fname, name, size);
		slen = (int)size;
		wfname = utf8_towchar(fname, &slen);
		wfname[slen-1] = 0;
		for (i = 0; i < slen; i++) {
			if (wfname[i] == L'/') {
				wfname[i] = 0;
				if (GetFileAttributesW(wfname) == INVALID_FILE_ATTRIBUTES)
					CreateDirectoryW(wfname, NULL);
				wfname[i] = L'\\';
			}
		}
		if ( !zip_entry_isdir(z) )
			success = (zip_entry_fread(z, wfname) == 0);
		count++;
		free(wfname);
next:
		free(fname);
		zip_entry_close(z);
	}
	return(__int64)count;
}

static wchar_t *prep_destdir(lua_State *L, int idx) {
	wchar_t *dir = lua_isstring(L, idx) ? lua_towstring(L, idx): _wcsdup(luaL_checkcinstance(L, idx, Directory)->fullpath);
	wchar_t *oldpath = GetCurrentDir();
	make_path(dir);
	SetCurrentDirectoryW(dir);
	free(dir);
	return oldpath;
}

LUA_METHOD(Zip, extract)
{
	Zip *z = lua_self(L, 1, Zip);
	size_t len;
	int narg = lua_gettop(L);
	const char *name = lua_tolstring(L, 2, &len);
	BOOL include_path = narg > 3 ? lua_toboolean(L, 4) : TRUE;
	wchar_t *oldpath = NULL, *fname = lua_towstring(L, 2);
	char *dname = calloc(1, len+2);

	strncpy(dname, name, len);
	dname[len] = '/';	
	if (narg > 2)
		oldpath = prep_destdir(L, 3);
	lua_pushboolean(L, FALSE);
	if (z->mode != 'r')
		luaL_error(L, "cannot extract from a Zip archive opened in write/append mode");
	if (zip_entry_open(z->zip, name) == 0) {
dir:	if (zip_entry_isdir(z->zip)) {
			zip_entry_close(z->zip);
			extract_zip(z->zip, dname);		
			lua_pushvalue(L, 2);
			lua_pushinstance(L, Directory, 1);
		}
		else {
			if (include_path) {
				if (make_path(fname) && (zip_entry_fread(z->zip, fname) == 0))
					goto done;
			} else if (zip_entry_fread(z->zip, PathFindFileNameW(fname)) == 0) {
done:			lua_pushstring(L, name);
				lua_pushinstance(L, File, 1);
				zip_entry_close(z->zip);
			}
		} 
	} else if (zip_entry_open(z->zip, dname) == 0)
		goto dir;
	free(fname);
	free(dname);
	if (oldpath) {
		SetCurrentDirectoryW(oldpath);
		free(oldpath);
	}
	return 1;
}

LUA_METHOD(Zip, remove) {
	Zip *z = lua_self(L, 1, Zip);	
	int i, n = lua_gettop(L)-1;
	char **fnames = malloc(sizeof(char*)*n);
	
	if (z->mode != 'd')
		luaL_error(L, "Zip file must be opened in 'delete' mode to remove entries");
	for (i=0; i < n; i++)
		fnames[i] = (char*)luaL_checkstring(L, i+2);
	lua_pushboolean(L, (n = zip_entries_delete(z->zip, fnames, (size_t)n)) > 0);
	if (n == 0)
	    z->zip->archive.m_last_error = MZ_ZIP_ENTRY_NOT_FOUND;
	free(fnames);
	return 1;
}

LUA_METHOD(Zip, extractall) {
	wchar_t *oldpath = NULL;

	if (lua_gettop(L) > 1)
		oldpath = prep_destdir(L, 2);
	lua_pushinteger(L, extract_zip(lua_self(L, 1, Zip)->zip, NULL));
	if (oldpath) {
		SetCurrentDirectoryW(oldpath);
		free(oldpath);
	}
	return 1;
}

LUA_METHOD(Zip, isdirectory) {
	Zip *z =lua_self(L, 1, Zip);
	size_t len = 0;
	char tmp[len+2];
	
	snprintf(tmp, len+2, "%s/", luaL_checklstring(L, 2, &len));
	lua_pushboolean(L, FALSE);
	if (zip_entry_open(z->zip, tmp) == 0) {
		lua_pushboolean(L, zip_entry_isdir(z->zip));
		zip_entry_close(z->zip);
	}
	return 1;
}

LUA_PROPERTY_GET(Zip, file) {
	lua_pushwstring(L, lua_self(L, 1, Zip)->fname);
	lua_pushinstance(L, File, 1);
	return 1;
}

static int Zip_iter(lua_State *L) {
	Zip *z = lua_self(L, lua_upvalueindex(1), Zip); 
	int entry = (int)lua_tointeger(L, lua_upvalueindex(2));
	if (zip_entry_openbyindex(z->zip, entry) == 0) {
		const char *str = zip_entry_name(z->zip);	
		size_t len = strlen(str);
		lua_pushlstring(L, str, str[len-1]=='/' ? len-1 : len);
		lua_pushboolean(L, zip_entry_isdir(z->zip));
		lua_pushinteger(L, ++entry);
		lua_replace(L, lua_upvalueindex(2));
		zip_entry_close(z->zip);
		return 2;
	}
	return 0;
}

LUA_METHOD(Zip,__iterate) {
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, Zip_iter, 2);
	return 1;
}

const luaL_Reg Zip_metafields[] = {
	{"__gc",		Zip___gc},
	{"__iterate",	Zip___iterate},
	{NULL, NULL}
};

const luaL_Reg Zip_methods[] = {
	{"close",		Zip_close},
	{"write",		Zip_write},
	{"read",		Zip_read},
	{"extract",		Zip_extract},
	{"reopen",		Zip_reopen},
	{"isdirectory",	Zip_isdirectory},
	{"extractall",	Zip_extractall},
	{"remove",		Zip_remove},
	{"get_count",	Zip_getcount},
	{"get_size",	Zip_getsize},
	{"get_file",	Zip_getfile},
	{"get_iszip64",	Zip_getiszip64},
	{"get_error",	Zip_geterror},
	{NULL, NULL}
};

