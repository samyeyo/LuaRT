/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | File.c | LuaRT File object implementation
*/

#include <File.h>
#include <Directory.h>
#include <Buffer.h>

#include "lrtapi.h"
#include <luart.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <limits.h>
#include <fcntl.h>
#include <shlwapi.h>

luart_type TFile;
const char *file_modes[] = { "read", "append", "write", "readwrite", NULL };
const wchar_t *file_values[] = { L"rb", L"ab+", L"wb" , L"rb+" };
const char* encodings[] = { "binary", "utf8", "unicode", NULL };
const char* bom[] = { "", "\xEF\xEB\x00", "\xFF\xFE" };
const int bom_size[] = {0, 3, 2};
const int encoding_size[]= {1, 1, 2};

const char* seek_modes[] = { "start", "here", "end", NULL };
const unsigned long seek_values[] = { SEEK_SET , SEEK_CUR, SEEK_END};

static BOOL update_time(File *f) {
	FILETIME c, la, lw, ft;
	HANDLE h = f->h ? f->h : CreateFileW(f->fullpath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING,  FILE_FLAG_BACKUP_SEMANTICS, 0);
	if (h && GetFileTime(h, &c, &la, &lw)) {
		FileTimeToLocalFileTime(&c, &ft);
		FileTimeToSystemTime(&ft, &f->created);
		FileTimeToLocalFileTime(&la, &ft);
		FileTimeToSystemTime(&ft, &f->accessed);
		FileTimeToLocalFileTime(&lw, &ft);
		FileTimeToSystemTime(&ft, &f->modified);
		CloseHandle(h);
	}
	return h != INVALID_HANDLE_VALUE;
}

void init_fullpath(lua_State *L, File *f) {
	int size;
	wchar_t *p;
	extern int sys_getcurrentdir(lua_State *L);

	if ((f->type == TDirectory) && ((lua_gettop(L) == 1) || lua_isnil(L, 2))) {
		sys_getcurrentdir(L);
		f->fname = lua_tolwstring(L, -1, &size);
	} else
		f->fname = lua_tolwstring(L, 2, &size);
	if (f->type == TDirectory && (f->fname[size-1] == L'\\' || f->fname[size-1] == L'/'))
		f->fname[--size] = 0;
	if (PathIsRelativeW(f->fname) || (f->type == TDirectory && (size != 2 && f->fname[1] != ':') )) {
		size = (GetFullPathNameW(f->fname, 0, NULL, NULL))*sizeof(wchar_t);
		f->fullpath = calloc(size, sizeof(wchar_t));
		GetFullPathNameW(f->fname, (DWORD)size, f->fullpath, NULL);
	} else f->fullpath = wcsdup(f->fname);
	if (f->type == TDirectory)
		while((p = wcschr(f->fullpath, L'/')))
			*p = L'\\';
	update_time(f);
}

//-------------------------------------[ File Constructor ]
LUA_CONSTRUCTOR(File) {
	File *f = calloc(1, sizeof(File));
	f->type = TFile;
	init_fullpath(L, f);
	lua_newinstance(L, f, File);
	return 1;
}

wchar_t *luaL_checkFilename(lua_State *L, int idx) {
	return (wchar_t*)(lua_type(L, idx) == LUA_TSTRING ? lua_towstring(L, idx) : wcsdup(lua_self(L, idx, File)->fullpath)); 
}

//-------------------------------------[ File.open() ]
LUA_METHOD(File, open) {
	File *f;
	int mode = luaL_checkoption(L, 2, "read", file_modes);
	int narg = lua_gettop(L);
	unsigned char b[2] = {0};

	f = lua_self(L, 1, File);
	if (!f->std) {
		if (f->stream)
			fclose(f->stream);
		f->stream = _wfopen(f->fullpath, file_values[mode]);
		if (!f->stream)
			luaL_error(L, "File open failed '%s'", strerror(errno));
		f->mode = mode;
		if (mode < 2) {
			f->encoding = ASCII;
			_fseeki64(f->stream, 0, SEEK_SET);
			fread(b,1,2, f->stream);
			if( b[0] == 0xFF && b[1] == 0xFE)
				f->encoding = UNICODE;
			else if (b[0] == 0xEF && b[1] == 0xBB) {
				fread(b, 1, 1, f->stream);
				if (b[0] == 0xBF)
					f->encoding = UTF8;
				else
					goto def;
			}
			else
			def:
				_fseeki64(f->stream, 0, SEEK_SET);
			if (narg == 1 && f->encoding == ASCII)
				f->encoding = UTF8;
			if (narg == 3)
				f->encoding = luaL_checkoption(L, 3, "binary", encodings);
            if (mode == 2)
                _fseeki64(f->stream, 0, SEEK_END);
		}
		else {
			f->encoding = luaL_checkoption(L, 3, "utf8", encodings);
			switch(f->encoding) {
				case 1 : fwrite("\xEF\xBB\xBF", 3, 1, f->stream); break;
				case 2 : fwrite("\xFF\xFE", 2, 1, f->stream);
			}
		}
		if (f->stream) {
			lua_pushvalue(L, 1);
			f->h = (HANDLE)_get_osfhandle(_fileno(f->stream));
			return 1;
		}
	}
	return 0;
}

//-------------------------------------[ File.write() ]
LUA_METHOD(File, write) {
	File *f = lua_self(L, 1, File);
	if (f->stream) {
		char *buf;
		size_t wsize;
		int len;
		size_t r, done = 0;
		if (!f->mode)
			luaL_error(L, "error: File not opened for writing");
		if (f->encoding == UNICODE) {
			if (!lua_isstring(L, 2))
				buf = (char*)luaL_tolstring(L, 2, &wsize);
			else {
				buf = (char*)lua_tolwstring(L, 2, &len);
				wsize = len * sizeof(wchar_t);
			}
		}
		else
			buf = (char*)luaL_tolstring(L, 2, &wsize);
		while (done < wsize && (r = fwrite(buf + done, 1, wsize - done, f->stream)))
			done += r;
		if (f->encoding == UNICODE)
			free(buf);
		lua_pushinteger(L, done);
		return 1;
	}
	return luaL_error(L, "error: File not opened for writing");
}
//-------------------------------------[ File.writeln() ]
LUA_METHOD(File, writeln) {
	File *f = lua_self(L, 1, File);
	if (lua_gettop(L) > 1)
		File_write(L);
	if (f->encoding == UNICODE)
		fputws(L"\r\n", f->stream);
	else
		fputs("\r\n", f->stream);
	return 1;
}

static int FileRead(lua_State *L, File *f, size_t size, BOOL line) {
	luaL_Buffer b;
	char buff[sizeof(wchar_t)];
	wchar_t *c = (wchar_t*)buff;
	int nbytes = f->std ? 2 : encoding_size[f->encoding];
	size_t todo = size;
	size_t utf8size;
	size_t i, last = 0;
	extern wchar_t echochar;

	if (f->mode > 0 && f->mode < 3)
		luaL_error(L, "cannot read on a File opened in '%s' mode", file_modes[f->mode]);
	luaL_buffinit(L, &b);
	if (f->std) {
		FILE *fout;
		wchar_t ch;
		HANDLE std = GetStdHandle(STD_INPUT_HANDLE);

		console_getstdout(L);
		fout = lua_self(L, -1, File)->stream;
		lua_pop(L, 1);
		if (echochar) {
			DWORD mode;
			GetConsoleMode(std, &mode);
			SetConsoleMode(std, mode & ~ENABLE_ECHO_INPUT & ~ENABLE_LINE_INPUT & ~ENABLE_PROCESSED_INPUT);
readstd:	while (ReadConsoleW(f->h, &ch, 1, (LPDWORD)&i, NULL)) {	
				if (line && ch == 13)
					break;
				else if (ch == 3) {
					lua_pushstring(L, "^C");
					lua_error(L);				
				}
				else if ((ch == 8) && (b.n > 0)) {
					todo++;
					luaL_buffsub(&b, 2);
					if (echochar != 1)
						fputws(L"\b \b", fout);
					continue;
				}
				else if (echochar != 1)
					fputwc(echochar, fout);
				else if (ch > 31)
					fputwc(ch, fout);
				luaL_addlstring(&b, (char*)&ch, sizeof(wchar_t));
				if (!line && (--todo == 0))
					break;
			}
			SetConsoleMode(std, mode);
		} else if (line) {
			wchar_t buffer[4096];
			int save = _setmode(_fileno(f->stream), _O_U16TEXT);
			if (fgetws(buffer, 4096, f->stream)) {
				luaL_addlstring(&b, (char*)buffer, sizeof(wchar_t)*wcslen(buffer)-2);
				fgetwc(f->stream);
				_setmode(_fileno(f->stream), save);
			}
			else {
				lua_pushstring(L, "");
				lua_error(L);
			}
		} else goto readstd;
	}
	else if (f->stream) {
		while ( (fread(buff, nbytes, 1, f->stream)) ) {
			if (f->encoding == UTF8 && ((utf8size = utf8_charsize(buff)) > 1)) {
				fread(buff+1, utf8size-1, 1, f->stream);
				luaL_addlstring(&b, buff, utf8size);
				continue;
			}
			if (((nbytes == 2) && (*c == 10)) || ((nbytes == 1) && (*buff == 10))) {
				if (f->encoding && last == 13)
					b.n = b.n-nbytes;
				if (line)
					break;
			}
			last = nbytes == 2 ? *c : *buff;
			luaL_addlstring(&b, buff, nbytes);
			if (!--todo)
				break;
		}
	}
	else
		luaL_error(L, "could not read, File is not open");
	luaL_pushresult(&b);
	if (b.n == 0 && !line) {
		return FALSE;
	}
	if (f->encoding == UNICODE || f->std) {
		wchar_t *str = (wchar_t*)lua_tolstring(L, -1, &size);
		lua_pushlwstring(L, str, size/sizeof(wchar_t));
	}
	return TRUE;
}

//-------------------------------------[ File.read() ]
LUA_METHOD(File, read) {
	File *f = lua_self(L, 1, File);
	if (FileRead(L, f, lua_gettop(L) > 1 ? (size_t)luaL_checkinteger(L, 2) : 0, FALSE)) {
		if (!f->encoding)
			lua_pushinstance(L, Buffer, 1);
	}
	else
		lua_pushstring(L, "");
	return 1;           
}

//-------------------------------------[ File.readln() ]
LUA_METHOD(File, readln) {
	File *f = lua_self(L, 1, File);
	if (FileRead(L, f, 0, TRUE)) {
		if (!f->encoding)
			lua_pushinstance(L, Buffer, 1);
	}
	else
		lua_pushstring(L, "");
	return 1;          
}

//-------------------------------------[ File.flush() ]
LUA_METHOD(File, flush) {
	File *f = lua_self(L, 1, File);
	if (f->stream)
		fflush(f->stream);
	return 0;
}

//-------------------------------------[ File.close() ]
LUA_METHOD(File, close) {
	File *f = lua_self(L, 1, File);
	if (!f->std && f->stream) {
		fflush(f->stream);
		fclose(f->stream);
		f->stream = 0;
		f->h = 0;
	}
	if (f->stdstream) {
		f->std = TRUE;
		f->encoding = UTF8;
		f->stream = f->stdstream;
		f->stdstream = 0;
		f->h = (HANDLE)_get_osfhandle(_fileno(f->stream));
	}
	return 0;
}

//-------------------------------------[ File.remove / Directory.remove ]

LUA_METHOD(File, remove) {
	File *f = lua_self(L, 1, File);
	lua_pushboolean(L, GetFileAttributesW(f->fullpath) & FILE_ATTRIBUTE_DIRECTORY ? RemoveDirectoryW(f->fullpath) : DeleteFileW(f->fullpath));
	return 1;
}

//-------------------------------------[ File.copy ]
LUA_METHOD(File, copy) {
	File *f = lua_self(L, 1, File);
	wchar_t *new_name = lua_towstring(L, 2);
	if (CopyFileW(f->fullpath, new_name, TRUE)) {
		lua_pushwstring(L, new_name);
		lua_pushinstance(L, File, 1);
	} else lua_pushnil(L);
	free(new_name);
	return 1;
}

//-------------------------------------[ File.move ]
LUA_METHOD(File, move) {
	File *f = lua_self(L, 1, File);
	wchar_t *new_name;

	new_name = lua_towstring(L, 2);
	lua_pushboolean(L, MoveFileW(f->fullpath, new_name));
	free(new_name);
	return 1;
}

//-------------------------------------[ File.fullpath ]
LUA_METHOD(File, getfullpath) {
	lua_pushwstring(L, ((File *)lua_self(L, 1, File))->fullpath);
	return 1;
}

//-------------------------------------[ File.size ]
LUA_METHOD(File, getsize) {
	File *f = lua_self(L, 1, File);
	HANDLE h = f->h;
	LARGE_INTEGER size;

	if (!h)
		h = CreateFileW(f->fullpath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	lua_pushinteger(L, GetFileSizeEx(h, &size) ? size.QuadPart - bom_size[f->encoding] : 0);
	if (f->h == 0)
		CloseHandle(h);
	return 1;
}
//-------------------------------------[ File.name ]
typedef LPWSTR (__stdcall *pathfunc)(LPCWSTR);

static int call_pathfunc(lua_State *L, pathfunc p) {
	File *f = ((File *)lua_self(L, 1, File));
	wchar_t *result;

	result = p(f->fullpath);
	if (result == f->fullpath)
		lua_pushlstring(L, NULL, 0);
	else
		lua_pushwstring(L, result);
	return 1;
}

LUA_METHOD(File, getfilename) {
	return call_pathfunc(L, PathFindFileNameW);
}

//-------------------------------------[ File.extension ]
LUA_METHOD(File, getextension) {
	return call_pathfunc(L, PathFindExtensionW);
}

//-------------------------------------[ File.parent ]
LUA_METHOD(File, getparent) {
	wchar_t *fpath = ((File *)lua_self(L, 1, File))->fullpath;
	wchar_t *str = wcsdup(fpath);

	PathRemoveFileSpecW(str);
	if (wcscmp(str, fpath) == 0)
		lua_pushnil(L);
	else {
		lua_pushwstring(L, str);
		lua_pushinstance(L, Directory, 1);
	}
	free(str);
	return 1;
}

//-------------------------------------[ File.path ]
LUA_METHOD(File, getpath) {
	File *f = ((File *)lua_self(L, 1, File));
	if (f->type == TFile)
		lua_pushlwstring(L, f->fullpath, PathFindFileNameW(f->fullpath)-f->fullpath);
	else
		lua_pushwstring(L, f->fname);
	return 1;
}

//-------------------------------------[ File.exists ]
LUA_METHOD(File, getexists) {
	DWORD dwAttrib = GetFileAttributesW(((File *)lua_self(L, 1, File))->fullpath);
	lua_pushboolean(L, (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)));
	return 1;
}

//-------------------------------------[ File.readonly ]
static int setattrib(lua_State *L, DWORD attrib) {
	File *f = ((File *)lua_self(L, 1, File));
	DWORD fattrib = GetFileAttributesW(f->fullpath);
	SetFileAttributesW(f->fullpath, lua_toboolean(L, 2) ? fattrib | attrib : fattrib & ~attrib);
	return 0;
}

LUA_METHOD(File, getreadonly) {
	lua_pushboolean(L, GetFileAttributesW(((File *)lua_self(L, 1, File))->fullpath) & FILE_ATTRIBUTE_READONLY);
	return 1;
}

LUA_METHOD(File, setreadonly) {
	return setattrib(L, FILE_ATTRIBUTE_READONLY);
}

//-------------------------------------[ File.hidden ]
LUA_METHOD(File, gethidden) {
	lua_pushboolean(L, GetFileAttributesW(((File *)lua_self(L, 1, File))->fullpath) & FILE_ATTRIBUTE_HIDDEN);
	return 1;
}

LUA_METHOD(File, sethidden) {
	return setattrib(L, FILE_ATTRIBUTE_HIDDEN);
}

//-------------------------------------[ File.temporary ]
LUA_METHOD(File, gettemporary) {
	lua_pushboolean(L, GetFileAttributesW(lua_self(L, 1, File)->fullpath) & FILE_ATTRIBUTE_TEMPORARY);
	return 1;
}

LUA_METHOD(File, settemporary) {
	return setattrib(L, FILE_ATTRIBUTE_TEMPORARY);
}

//-------------------------------------[ File.created ]
void file_timestamp(lua_State *L, File *f) {
	FILETIME ftc, fta, ftm;
	HANDLE h;
	if (( h = CreateFileW(f->fullpath, FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0) ) != INVALID_HANDLE_VALUE) {
		SystemTimeToFileTime(&f->created, &ftc);
		SystemTimeToFileTime(&f->accessed, &fta);
		SystemTimeToFileTime(&f->modified, &ftm);
		if (!SetFileTime(h, &ftc, &fta, &ftm))
			lua_error(L);
		CloseHandle(h);
	}
}

int get_datetime(lua_State *L, LPSYSTEMTIME st) {
	File *f = ((File *)lua_self(L, 1, File));
	if (update_time(f)) {
		lua_pushlightuserdata(L, f);
		lua_pushlightuserdata(L, &file_timestamp);
		lua_pushlightuserdata(L, st);
		lua_pushinstance(L, Datetime, 3);
	} else lua_pushnil(L);
	return 1;
}

int set_datetime(lua_State *L, SYSTEMTIME *st, File *f) {
	Datetime *dt = luaL_checkcinstance(L, 2, Datetime);
	memcpy(st, dt->st, sizeof(SYSTEMTIME));
	file_timestamp(L, f);
	return 0;
}

LUA_METHOD(File, getcreated) {
	return get_datetime(L, &((File *)lua_self(L, 1, File))->created);
}

LUA_METHOD(File, setcreated) {
	File *f =((File *)lua_self(L, 1, File));
	return set_datetime(L, &f->created, f);
}

//-------------------------------------[ File.accessed ]

LUA_METHOD(File, getaccessed) {
	return get_datetime(L, &((File *)lua_self(L, 1, File))->accessed);
}

LUA_METHOD(File, setaccessed) {
	File *f = ((File *)lua_self(L, 1, File));
	return set_datetime(L, &f->accessed, f);
}

//-------------------------------------[ File.modified ]

LUA_METHOD(File, getmodified) {
	return get_datetime(L, &((File *)lua_self(L, 1, File))->modified);
}

LUA_METHOD(File, setmodified) {
	File *f = ((File *)lua_self(L, 1, File));
	return set_datetime(L, &f->modified, f);
}

//-------------------------------------[ File.position ]
LUA_METHOD(File, getposition) {
	File *f = lua_self(L, 1, File);
	if (f->stream)
		lua_pushinteger(L, _ftelli64( f->stream)-1);
	else
		lua_pushnil(L);
	return 1;
}

LUA_METHOD(File, setposition) {
	File *f = lua_self(L, 1, File);
	lua_Integer pos = luaL_checkinteger(L, 2);

	if (pos == 0)
		luaL_error(L, "zero is an invalid File.position value");

	if (f->stream)
		_fseeki64(f->stream, (pos-1) + bom_size[f->encoding], SEEK_SET);
	return 0;
}

//-------------------------------------[ File.eof ]
LUA_METHOD(File, geteof) {
	File *f = lua_self(L, 1, File);
	if (f->stream)
		lua_pushboolean(L, feof(f->stream));
	else
		lua_pushnil(L);
	return 1;
}

//-------------------------------------[ File.buffered ]
LUA_PROPERTY_SET(File, buffered) {
	setvbuf(lua_self(L, 1, File)->stream, NULL, luaL_checkinteger(L, 2) ? _IOFBF : _IONBF, LUAL_BUFFERSIZE);
	return 0;
}

//-------------------------------------[ File.buffered ]
LUA_PROPERTY_GET(File, buffered) {
	return lua_self(L, 1, File)->stream->_flag & 0x100 ? _IOFBF : _IONBF;
}

//-------------------------------------[ File.encoding ]
LUA_METHOD(File, getencoding) {
	File *f = lua_self(L, 1, File);
	lua_pushstring(L, encodings[f->encoding]);
	return 1;
}

//-------------------------------------[ File.lines ]
static int iterate_lines(lua_State *L) {
	File *f = lua_self(L, lua_upvalueindex(1), File);
	if (feof(f->stream))
		return 0;
	FileRead(L, f, 0, TRUE);
	return 1;
}

LUA_METHOD(File, getlines) {
	lua_pushvalue(L, 1);
	lua_pushcclosure(L, iterate_lines, 1);
	return 1;
}

const luaL_Reg File_methods[] = {
	{"open",			File_open},
	{"close",			File_close},
	{"write",			File_write},
	{"writeln",			File_writeln},
	{"read",			File_read},
	{"readln",			File_readln},
	{"flush",			File_flush},
	{"remove",			File_remove},
	{"copy",			File_copy},
	{"move",			File_move},
	{"get_fullpath",	File_getfullpath},
	{"get_size",		File_getsize},
	{"get_name",		File_getfilename},
	{"get_extension",	File_getextension},
	{"get_directory",	File_getparent},
	{"get_path",		File_getpath},
	{"get_exists",		File_getexists},
	{"get_readonly",	File_getreadonly},
	{"get_hidden",		File_gethidden},
	{"get_temporary",	File_gettemporary},
	{"set_readonly",	File_setreadonly},
	{"set_hidden",		File_sethidden},
	{"set_temporary",	File_settemporary},
	{"get_position",	File_getposition},
	{"set_position",	File_setposition},
	{"get_eof",			File_geteof},
	{"get_encoding",	File_getencoding},
	{"get_lines",		File_getlines},
	{"get_created",		File_getcreated},
	{"get_modified",	File_getmodified},
	{"get_accessed",	File_getaccessed},
	{"set_created",		File_setcreated},
	{"set_modified",	File_setmodified},
	{"set_accessed",	File_setaccessed},
	{NULL, NULL}
};

//-------------------------------------[ File Metatable ]
LUA_METHOD(File, gc) {
	File *f = lua_self(L, 1, File);
	File_close(L);
	free(f->fullpath);
	free(f->fname);
	free(f);
	return 0;
}

const luaL_Reg File_metafields[] = {
	{"__gc", File_gc},
	{NULL, NULL}
};