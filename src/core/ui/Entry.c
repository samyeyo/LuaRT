/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Entry.c | LuaRT Entry and Edit object implementation
*/


#include <luart.h>
#include <Widget.h>
#include "ui.h"
#include <File.h>

#include <share.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include "DarkMode.h"

LUA_METHOD(Edit, append) {
	Widget *w = lua_self(L, 1, Widget);
	wchar_t *chars = lua_towstring(L, 2);
	SendMessage(w->handle, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
 	SendMessageW(w->handle,EM_REPLACESEL,(WPARAM) FALSE,(LPARAM) chars);	
 	if (!w->cursor)
		SetFocus(w->handle);
	free(chars);
 	return 0;
}

LUA_METHOD(Entry, redo) {
	SendMessage(lua_self(L, 1, Widget)->handle, EM_REDO, 0, 0);
	return 0;
}

LUA_METHOD(Entry, undo) {
	SendMessage(lua_self(L, 1, Widget)->handle, EM_UNDO, 0, 0);
	return 0;
}

LUA_METHOD(Entry, paste) {
	Widget *w= lua_self(L, 1, Widget);
	if (w->wtype == UIEntry)
		SendMessage(w->handle, WM_PASTE, 0, 0);
	else
		SendMessage(w->handle, EM_PASTESPECIAL, CF_UNICODETEXT, 0);
	return 0;
}

LUA_METHOD(Entry, cut) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->wtype == UIEntry)
		SendMessage(w->handle,EM_SETSEL,0,-1);
	SendMessage(w->handle, WM_CUT, 0, 0);
	return 0;
}

LUA_METHOD(Entry, copy) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->wtype == UIEntry)
		SendMessage(w->handle,EM_SETSEL,0,-1);
	SendMessage(w->handle, WM_COPY, 0, 0);
	return 0;
}

LUA_PROPERTY_SET(Edit, line) {
	HWND h = lua_self(L, 1, Widget)->handle;
	LONG pos;
	if  ( (pos = SendMessage(h, EM_LINEINDEX, luaL_checkinteger(L, 2)-1, 0)) > -1) {
		CHARRANGE cr = {pos, pos};
		SendMessage(h, EM_EXSETSEL, (WPARAM)0, (LPARAM)&cr);
		SendMessage(h, EM_SCROLLCARET, 0, 0);
		SetFocus(h);
	}
	return 0;
}

LUA_PROPERTY_GET(Edit, mousepos) {
	HWND h = lua_self(L, 1, Widget)->handle;
	POINT p;
	lua_Integer pos = -1;
	RECT r;
	POINTL pl;

	GetClientRect(h, &r);
	GetCursorPos(&p);
	ScreenToClient(h, &p);
	if (p.x > 0 && p.y < (r.bottom-r.top) ) {
		POINTL pl = {p.x, p.y};
		pos = SendMessage(h, EM_CHARFROMPOS, 0, (LPARAM)&pl);
	}
	lua_pushinteger(L, pos+1);
	return 1;
}

LUA_PROPERTY_GET(Edit, line) {
	lua_pushinteger(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_LINEFROMCHAR, -1, 0)+1);
	return 1;
}

LUA_PROPERTY_GET(Edit, wordwrap) {
	lua_pushboolean(L, !lua_self(L, 1, Widget)->status);
	return 1;
}

LUA_PROPERTY_SET(Edit, wordwrap) {
	Widget *w = lua_self(L, 1, Widget);
	w->status = (HANDLE)(UINT_PTR)(lua_toboolean(L, 2) ? 0 : 1);
	SendMessage(w->handle, EM_SETTARGETDEVICE, 0, (LPARAM)w->status);
	return 0;
}

DWORD CALLBACK ReadStreamCB(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{	
	BOOL done = (*pcb = (LONG)fread(lpBuff, 1, cb, (FILE *)dwCookie)) == 0;
	if (done)
		fclose((FILE *)dwCookie);
	return done;
}

DWORD CALLBACK WriteStreamCB(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{	
	return (*pcb = (LONG)fwrite(lpBuff, 1, cb, (FILE *)dwCookie)) == 0;
}

typedef struct {
	LONG 		pos;
	size_t 		length;
	char *value;
} streaminfo;

DWORD CALLBACK ReadStringCB(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{	
	streaminfo *si = (streaminfo*)dwCookie;
	LONG count = (si->length - si->pos);
	if( count > cb )
		count = cb;
	memcpy(lpBuff, &(si->value[si->pos]), count);
	si->pos += count;
	if (pcb) 
		*pcb = count;
	return 0;
}

DWORD CALLBACK WriteStringCB(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{	
	streaminfo *si = (streaminfo*)dwCookie;
	si->value = si->value ? realloc(si->value, si->length+cb) : malloc(cb);
	memcpy(si->value+si->length, lpBuff, cb);
	si->length += cb;	
	*pcb = cb;
	return 0;
}

static Encoding detectBOM(FILE *f) {
	static unsigned char b[2] = {0};
	
	_fseeki64(f, 0, SEEK_SET);
	fread(b, 1, 2, f);
	if( b[0] == 0xFF && b[1] == 0xFE)
		return _UNICODE;
	if (b[0] == 0xEF && b[1] == 0xBB) {
		fread(b, 1, 1, f);
		if (b[0] == 0xBF)
			return UTF8;
	}
	_fseeki64(f, 0, SEEK_SET);
	return ASCII;;
}

static int do_file_operation(lua_State *L, const wchar_t *mode) {
	HANDLE handle = lua_self(L, 1, Widget)->handle;
	BOOL result = FALSE;
	FILE *f;
  	EDITSTREAM es = { 0 };
	DWORD type;
	BOOL iswrite = (*mode == L'w');
	wchar_t *fname = luaL_checkFilename(L, 2);

	if (!(f = _wfopen(fname, mode)))
		luaL_error(L, "error while accessing '%s'", lua_tostring(L, 2));

	es.dwCookie = (DWORD_PTR)f;
	es.pfnCallback = iswrite ? WriteStreamCB : ReadStreamCB;
	if (((lua_gettop(L) == 3) && lua_toboolean(L, 3)) || (SendMessage(handle, EM_GETTEXTMODE, 0, 0) & TM_RICHTEXT))
		type = SF_RTF | (!iswrite && (detectBOM(f) == _UNICODE ? SF_UNICODE : (SF_USECODEPAGE | (CP_UTF8 << 16))));
	else 
		type = SF_TEXT | (!iswrite && (detectBOM(f) == _UNICODE ? SF_UNICODE : (SF_USECODEPAGE | (CP_UTF8 << 16))));
	if ( (result = SendMessageW(handle, iswrite ? EM_STREAMOUT : EM_STREAMIN, type, (LPARAM)&es)) && es.dwError == 0) 
		SendMessage(handle, EM_SETMODIFY, !iswrite, 0);
	lua_pushboolean(L, result); 
	fclose(f);
	free(fname);
	return 1;
}

static int edit_search(lua_State *L, LONG flags) {
	FINDTEXTEXW ft = {0};
	HWND h = lua_self(L, 1, Widget)->handle;

	if (lua_toboolean(L, 3))
		flags |= FR_WHOLEWORD;
	if (lua_toboolean(L, 4))
		flags |= FR_MATCHCASE;
	ft.lpstrText = lua_towstring(L, 2);
	SendMessageW(h, EM_GETSEL, 0, (LPARAM)&ft.chrg);
	if (flags & FR_DOWN)
		ft.chrg.cpMax = -1;
	else {
		ft.chrg.cpMax = 0;
		ft.chrg.cpMin--;
	}

	if ((SendMessage(h, EM_FINDTEXTEXW, (WPARAM)flags, (LPARAM)&ft)) > -1) {
		lua_pushinteger(L, ft.chrgText.cpMin+1);
		lua_pushinteger(L, ft.chrgText.cpMax+1);
		free((wchar_t*)ft.lpstrText);
		return 2;
	}
	lua_pushnil(L);
	free((wchar_t*)ft.lpstrText);
	return 1;
}

LUA_METHOD(Edit, searchup) {
	return edit_search(L, 0);
}

LUA_METHOD(Edit, searchdown) {
	return edit_search(L, FR_DOWN);
}

LUA_METHOD(Edit, loadfrom)
{
	return do_file_operation(L, L"rb");
}

LUA_METHOD(Edit, saveto)
{
	return do_file_operation(L, L"wb");
}

LUA_PROPERTY_GET(Edit, rtf) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_GETTEXTMODE, 0, 0) & TM_RICHTEXT);
	return 1;
}
 
LUA_PROPERTY_SET(Edit, text);
LUA_PROPERTY_SET(Edit, richtext);
LUA_PROPERTY_GET(Edit, text);

LUA_PROPERTY_SET(Edit, rtf) {
	BOOL tortf = lua_toboolean(L, 2);
	HWND h = lua_self(L, 1, Widget)->handle;	
	lua_pushcfunction(L, tortf ? Edit_setrichtext : Edit_settext);
	lua_pushvalue(L, 1);
	lua_pushcfunction(L, Edit_gettext);
	lua_pushvalue(L, 1);
	lua_call(L, 1, 1);	
	SendMessage(h, WM_SETTEXT, (WPARAM)"", (LPARAM)0);
	SendMessage(h, EM_SETTEXTMODE, tortf ? TM_RICHTEXT : TM_PLAINTEXT, 0);
	lua_call(L, 2, 0);
	return 0;
}

LUA_PROPERTY_GET(Edit, column) {
	HWND h = lua_self(L, 1, Widget)->handle;
	DWORD dwStart = 0, dwEnd = 0;
	SendMessageW(h, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
    lua_pushinteger(L, 1+dwStart - SendMessageW(h, EM_LINEINDEX, SendMessage(h, EM_LINEFROMCHAR, dwStart, 0), 0));
	return 1;
}

LUA_PROPERTY_SET(Edit, column) {
	HWND h = lua_self(L, 1, Widget)->handle;
	DWORD dwStart, dwEnd;
	dwStart = SendMessageW(h, EM_LINEINDEX, SendMessage(h, EM_LINEFROMCHAR, -1, 0), 0) + luaL_checkinteger(L, 2)-1;
	dwEnd = dwStart;
	SendMessageW(h, EM_SETSEL, (WPARAM)dwStart, (LPARAM)dwEnd);
	SetFocus(h);
	return 0;
}

//---------------------------------------- Edit.lines property

static lua_Integer getlines_widget(lua_State *L, HWND *h) {
	lua_Integer line;
	if	( (line = lua_tointeger(L, 2)) ) {
		luaL_getmetafield(L, 1, "__widget");
		*h = lua_self(L, -1, Widget)->handle;
	} else {
		const char *err = (luaL_getmetafield(L, 2, "__name") == LUA_TSTRING) ? lua_tostring(L, -1) : luaL_typename(L, 2);
		luaL_error(L, "incorrect line index (integer expected, found %s)", err);
	}
	return line;
}

static int pushline(lua_State *L, HWND h, lua_Integer line) {
	LONG len = SendMessageW(h,  EM_LINELENGTH,  SendMessageW(h, EM_LINEINDEX, line, 0),  0);
	if (len) {
		wchar_t *buffer = calloc(len+1, sizeof(wchar_t));
		*((LPWORD)buffer) = len;
		SendMessageW(h, EM_GETLINE, (WPARAM)line, (LPARAM)buffer);
		lua_pushwstring(L, buffer);
		free(buffer);
	} else lua_pushstring(L, "");
	return 1;
}

LUA_METHOD(Lines, __index) {
	HWND h;
	lua_Integer line = getlines_widget(L, &h);
	if (line <=  SendMessage(h, EM_GETLINECOUNT, 0, 0))
		return pushline(L, h, --line);
	return 0;
}

LUA_METHOD(Lines, __newindex) {
	HWND h;
	lua_Integer line = getlines_widget(L, &h);
	if (line <= SendMessage(h, EM_GETLINECOUNT, 0, 0)) {
		wchar_t *newline;
		lua_pushvalue(L, 3);
		lua_pushstring(L, "\r\n");
		lua_concat(L, 2);
		newline = lua_towstring(L, -1);
		DWORD start = SendMessageW(h, EM_LINEINDEX, --line, 0);
		DWORD end = SendMessageW(h, EM_LINEINDEX, line+1, 0);
		SendMessageW(h, EM_SETSEL, (WPARAM)start, (LPARAM)end);
		SendMessageW(h, EM_REPLACESEL, TRUE, (LPARAM)newline);
		free(newline);
	} luaL_error(L, "index overflow (too high index %d)", line);
	return 0;
}

static int lines_iter(lua_State *L) {
#if _WIN64 || __x86_64__	
	HWND h = (HWND)lua_tointeger(L, lua_upvalueindex(1));
#else
	HWND h = (HWND)(int)lua_tointeger(L, lua_upvalueindex(1));
#endif
	lua_Integer line = lua_tointeger(L, lua_upvalueindex(2));
	if (line < SendMessage(h, EM_GETLINECOUNT, 0, 0)) {
		lua_pushinteger(L, line+1);
		lua_replace(L, lua_upvalueindex(2));
		pushline(L, h, line);
		return 1;
	}
	return 0;
}

LUA_METHOD(Lines, __iterate) {
	luaL_getmetafield(L, 1, "__widget");
#if _WIN64 || __x86_64__	
	lua_pushinteger(L, (lua_Integer)(lua_self(L, -1, Widget)->handle));
#else
	lua_pushinteger(L, (int)(lua_self(L, -1, Widget)->handle));
#endif
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, lines_iter, 2);
	return 1;	
}

LUA_METHOD(Lines, __len) {
	luaL_getmetafield(L, 1, "__widget");
	lua_pushinteger(L, SendMessage(lua_self(L, -1, Widget)->handle, EM_GETLINECOUNT, 0, 0));
	return 1;
}

luaL_Reg Lines_metafields[] = {
	{"__len",		Lines___len},
	{"__index",		Lines___index},
	{"__newindex",	Lines___newindex},
	{"__iterate",	Lines___iterate},
	{NULL, NULL}
};

LUA_PROPERTY_GET(Edit, lines) {
	lua_createtable(L, 0, 0);
	lua_createtable(L, 0, 2);
	luaL_setfuncs(L, Lines_metafields, 0);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "__widget");
	lua_setmetatable(L, -2);
	return 1;
}

LUA_PROPERTY_GET(Edit, caret) {
	HWND h = lua_self(L, 1, Widget)->handle;
	CHARRANGE cr = {0};
	SendMessageW(h, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	lua_pushinteger(L, cr.cpMax+1);
	return 1;
}

LUA_PROPERTY_SET(Edit, caret) {
	HWND h = lua_self(L, 1, Widget)->handle;
	CHARRANGE cr = {0};
	cr.cpMin = luaL_checknumber(L, 2)-1;
	cr.cpMax = cr.cpMin;
	SendMessageW(h, EM_EXSETSEL, (WPARAM)0, (LPARAM)&cr);
	return 0;
}

LUA_PROPERTY_SET(Entry, textlimit) {
	SendMessage(lua_self(L, 1, Widget)->handle, EM_SETLIMITTEXT, (WPARAM)luaL_checkinteger(L, 2), 0);
	return 0;
}

LUA_PROPERTY_GET(Entry, textlimit) {
	lua_pushinteger(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_GETLIMITTEXT, 0, 0));
	return 1;
}

LUA_PROPERTY_GET(Entry, modified) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_GETMODIFY, 0, 0));
	return 1;
}

LUA_PROPERTY_SET(Entry, modified) {
	luaL_checktype(L, 2, LUA_TBOOLEAN);
	SendMessage(lua_self(L, 1, Widget)->handle, EM_SETMODIFY, (WPARAM)lua_tointeger(L, 2), 0);
	return 0;
}

LUA_PROPERTY_GET(Entry, canredo) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_CANREDO, 0, 0));
	return 1;
}

LUA_PROPERTY_GET(Entry, canundo) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_CANUNDO, 0, 0));
	return 1;
}

LUA_PROPERTY_GET(Entry, masked) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, EM_GETPASSWORDCHAR , 0, 0));
	return 1;
}

LUA_PROPERTY_SET(Entry, masked) {
	HANDLE hwnd = lua_self(L, 1, Widget)->handle;
	SendMessageW(hwnd, EM_SETPASSWORDCHAR , lua_toboolean(L, 2) ? L'*' : 0, 0);
	InvalidateRect(hwnd, NULL, 0);
	return 0;
}

LUA_PROPERTY_GET(Edit, readonly) {
	lua_pushboolean(L, (UINT_PTR)lua_self(L, 1, Widget)->status);
	return 1;
}

LUA_PROPERTY_SET(Edit, readonly) {
	Widget *w = lua_self(L, 1, Widget);
	
	if ((w->cursor = lua_toboolean(L, 2)))
		w->hcursor = LoadCursor(NULL, IDC_ARROW);
	SendMessage(w->handle, EM_SETREADONLY, lua_toboolean(L, 2), 0);
	return 0;
}

lua_Integer get_length(HWND h, DWORD flags) {
	GETTEXTLENGTHEX gtl = {0};
	gtl.codepage = CP_WINUNICODE;
	gtl.flags = flags;
	return SendMessageW(h, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, (LPARAM)NULL)+1;
}

LUA_PROPERTY_GET(Edit, textlength) {
	lua_pushinteger(L, get_length(lua_self(L, 1, Widget)->handle, GTL_NUMCHARS | GTL_USECRLF)-1);
	return 1;
}

LUA_PROPERTY_SET(Edit, text) {
	Widget *w = lua_self(L, 1, Widget);
	int len;
	SETTEXTEX st;
	wchar_t *text = lua_tolwstring(L, 2, &len);
	st.codepage = CP_WINUNICODE;
	st.flags = ST_NEWCHARS | ST_UNICODE;
	SendMessageW(w->handle, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)text);
	SendMessage(w->handle, EM_SETSEL, (WPARAM)len, (LPARAM)len);
	free(text);
	SendMessage(w->handle, WM_VSCROLL, SB_TOP, 0);
	return 0;
}

LUA_PROPERTY_GET(Edit, text) {
	HWND h = lua_self(L, 1, Widget)->handle;
	GETTEXTEX gt;
	wchar_t *buff;
	size_t len = get_length(h, GTL_NUMBYTES | GTL_USECRLF);
	gt.cb = len*sizeof(wchar_t);
	gt.codepage = CP_WINUNICODE;
	gt.flags = GT_USECRLF;
	buff = calloc(1, gt.cb); 
	SendMessageW(h, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)buff);
	lua_pushwstring(L, buff);
	free(buff);
	return 1;
}

LUA_PROPERTY_SET(Edit, richtext) {
	Widget *w = lua_self(L, 1, Widget);
	EDITSTREAM es = { 0 };
	streaminfo si = { 0 };
	WPARAM type;
	
	si.value = (char *)lua_tolstring(L, 2, &si.length);
	es.dwCookie = (DWORD_PTR)&si;
	es.pfnCallback = ReadStringCB;
	if (SendMessage(w->handle, EM_GETTEXTMODE, 0, 0) & TM_RICHTEXT)
		type = SF_RTF; // | (SF_USECODEPAGE | (CP_UTF8 << 16));
	else 
		type = SF_TEXT | (SF_USECODEPAGE | (CP_UTF8 << 16));
	if ( SendMessage(w->handle, EM_STREAMIN, type, (LPARAM)&es) && (es.dwError == 0))
		SendMessage(w->handle, EM_SETMODIFY, FALSE, 0);
	SendMessage(w->handle, WM_VSCROLL, SB_TOP, 0);
	SendMessage(w->handle, WM_HSCROLL, SB_LEFT, 0);
	return 0;
}
LUA_PROPERTY_GET(Edit, richtext) {
	Widget *w = lua_self(L, 1, Widget);
	EDITSTREAM es = { 0 };
	streaminfo si = { 0 };
	WPARAM type;
	
	es.dwCookie = (DWORD_PTR)&si;
	es.pfnCallback = WriteStringCB;
	if (SendMessage(w->handle, EM_GETTEXTMODE, 0, 0) & TM_RICHTEXT)
		type = SF_RTF | (SF_USECODEPAGE | (CP_UTF8 << 16));
	else 
		type = SF_TEXT | (SF_USECODEPAGE | (CP_UTF8 << 16));
	if (SendMessage(w->handle, EM_STREAMOUT, type, (LPARAM)&es) && (es.dwError == 0))
		lua_pushlstring(L, si.value, si.length);
	else lua_pushliteral(L, "");
	return 1;
}

//----------------------------- Edit.selection property

Widget *format(lua_State *L, Widget *w, DWORD mask, CHARFORMAT2W *cf, int scf, BOOL isset) {
	w = w ? w : lua_self(L, lua_gettop(L), Widget);
	cf->cbSize = sizeof(CHARFORMAT2W);
	cf->dwMask = mask;
	SendMessageW(w->handle, isset ? EM_SETCHARFORMAT: EM_GETCHARFORMAT, scf, (LPARAM)cf);
	return w;
}

LUA_PROPERTY_GET(Selection, visible) {
	lua_pushboolean(L, GetFocus() == lua_self(L, lua_gettop(L), Widget)->handle);
	return 1;
}

LUA_PROPERTY_SET(Selection, visible) {
	HWND h = lua_self(L, lua_gettop(L), Widget)->handle;
	BOOL set = lua_toboolean(L, 3);
	SetFocus(set ? h :GetParent(h));
	SendMessageA(h, EM_HIDESELECTION, !set, 0);
	return 0;
}

static int get_font(lua_State *L, Widget *w, int scf) {
	CHARFORMAT2W cf = {0};
	format(L, w, CFM_FACE, &cf, scf, FALSE);
	lua_pushwstring(L, cf.szFaceName);
	return 1;
}

static int set_font(lua_State *L, Widget *w, int scf) {
	CHARFORMAT2W cf = {0};
	LOGFONTW lf;
	wchar_t *font = luaL_checkFilename(L, w ? 2 : 3);

	if (!LoadFont(font, &lf))
		luaL_error(L, "failed to load font");
	wcsncpy(cf.szFaceName, lf.lfFaceName, LF_FACESIZE);
	format(L, w, CFM_FACE, &cf, scf, TRUE);
	free(font);
	return 0;
}

static int set_fontsize(lua_State *L, Widget *w, int scf) {
	CHARFORMAT2W cf;
	cf.yHeight = (lua_tointeger(L, w ? 2 : 3)) * 20;
	format(L, w, CFM_SIZE, &cf, scf, TRUE);
	return 0;
}

static int get_fontsize(lua_State *L, Widget *w, int scf) {
	CHARFORMAT2W cf;
	format(L, w, CFM_SIZE, &cf, scf, FALSE);
	lua_pushinteger(L, cf.yHeight/20);
	return 1;
}

LUA_PROPERTY_GET(Edit, font) {
	return get_font(L, lua_self(L, 1, Widget), SCF_ALL);
}

LUA_PROPERTY_SET(Edit, font) {
	return set_font(L, lua_self(L, 1, Widget), SCF_ALL);
}

LUA_PROPERTY_GET(Selection, font) {
	return get_font(L, NULL, SCF_SELECTION);
}

LUA_PROPERTY_SET(Selection, font) {
	return set_font(L, NULL, SCF_SELECTION);
}

LUA_PROPERTY_GET(Edit, fontsize) {
	return get_fontsize(L, lua_self(L, 1, Widget), SCF_ALL);
}

LUA_PROPERTY_SET(Edit, fontsize) {
	return set_fontsize(L, lua_self(L, 1, Widget), SCF_ALL);
}

LUA_PROPERTY_GET(Selection, fontsize) {
	return get_fontsize(L, NULL, SCF_SELECTION);
}

LUA_PROPERTY_SET(Selection, fontsize) {
	return set_fontsize(L, NULL, SCF_SELECTION);
}

void get_fontstyle(lua_State *L, Widget *w, LOGFONTW *lf, int scf) {
	CHARFORMAT2W cf;
	format(L, w, CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT, &cf, scf, FALSE);
	lf->lfStrikeOut = cf.dwEffects & CFE_STRIKEOUT;
	lf->lfUnderline = cf.dwEffects & CFE_UNDERLINE;
	lf->lfItalic = cf.dwEffects & CFE_ITALIC;
	lf->lfWeight = cf.dwEffects & CFE_BOLD ? FW_BOLD : FW_THIN;
}

static int set_fontstyle(lua_State *L, Widget *w, int scf) {
	CHARFORMAT2W cf = {0};
	LOGFONTW lf = {0};
	
	get_fontstyle(L, w, &lf, SCF_SELECTION);
	fontstyle_fromtable(L, w ? 2 : 3, &lf);
	cf.dwEffects |= lf.lfStrikeOut ? CFE_STRIKEOUT : 0;
	cf.dwEffects |= lf.lfUnderline ? CFE_UNDERLINE : 0;
	cf.dwEffects |= lf.lfItalic ? CFE_ITALIC : 0;
	cf.dwEffects |= (lf.lfWeight == FW_BOLD || lf.lfWeight == FW_HEAVY) ? CFE_BOLD : 0;
	format(L, w, CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT, &cf, scf, TRUE);
	return 0;
}

LUA_PROPERTY_GET(Selection, fontstyle) {
	LOGFONTW lf = {0};
	get_fontstyle(L, NULL, &lf, SCF_SELECTION);
	fontstyle_createtable(L, &lf);
	return 1;
}

LUA_PROPERTY_SET(Selection, fontstyle) {
	return set_fontstyle(L, NULL, SCF_SELECTION);
}

LUA_PROPERTY_GET(Edit, fontstyle) {
	LOGFONTW lf = {0};
	get_fontstyle(L, lua_self(L, 1, Widget), &lf, SCF_ALL);
	fontstyle_createtable(L, &lf);
	return 1;
}

LUA_PROPERTY_SET(Edit, fontstyle) {
	return set_fontstyle(L, lua_self(L, 1, Widget), SCF_ALL);
}

LUA_PROPERTY_GET(Selection, from) {
	CHARRANGE cr;
	SendMessageW(lua_self(L, -1, Widget)->handle, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	lua_pushinteger(L, cr.cpMin+1);
	return 1;
}

LUA_PROPERTY_SET(Selection, from) {
	CHARRANGE cr;
	HWND hwnd = lua_self(L, -1, Widget)->handle;
	SendMessageW(hwnd, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	cr.cpMin = lua_tointeger(L, 3)-1;
	cr.cpMax = cr.cpMin;
	SendMessageW(hwnd, EM_EXSETSEL, (WPARAM)0, (LPARAM)&cr);
	return 0;
}

LUA_PROPERTY_GET(Selection, to) {
	CHARRANGE cr;
	SendMessageW(lua_self(L, -1, Widget)->handle, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	lua_pushinteger(L, cr.cpMax+1);
	return 1;
}

LUA_PROPERTY_SET(Selection, to) {
	CHARRANGE cr;
	HWND hwnd = lua_self(L, -1, Widget)->handle;
	SendMessageW(hwnd, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	cr.cpMax = lua_tointeger(L, 3)-1;
	SendMessageW(hwnd, EM_EXSETSEL, (WPARAM)0, (LPARAM)&cr);
	return 0;
}

static int get_color(lua_State *L, Widget *w, int scf, BOOL isbg) {
	CHARFORMAT2W cf;
	lua_Integer result;
	COLORREF col;

	format(L, w, isbg ? CFM_BACKCOLOR : CFM_COLOR, &cf, scf, FALSE);
	col = isbg ? cf.crBackColor : cf.crTextColor;
	if ((isbg && !(cf.dwEffects & CFE_AUTOBACKCOLOR)) || (!isbg && !(cf.dwEffects & CFE_AUTOCOLOR)))
		result = GetRValue(col) << 16 | GetGValue(col) << 8 | GetBValue(col);
	else
		result =  GetSysColor(isbg ? COLOR_WINDOW : COLOR_WINDOWTEXT); 
	lua_pushinteger(L, result);
	return 1;	
}

static int set_color(lua_State *L, Widget *w, int scf, BOOL isbg) {
	CHARFORMAT2W cf = {0};
	COLORREF *col = isbg ? &cf.crBackColor : &cf.crTextColor;
	if (lua_isnil(L, scf + 2))
		*col = isbg ? (DarkMode ? 0x383838 : 0xFFFFFF) : (DarkMode ? 0xFFFFFF : GetSysColor(COLOR_BTNTEXT));
	else {
		DWORD32 color = luaL_checkinteger(L, scf + 2);
		*col = RGB(GetBValue(color), GetGValue(color), GetRValue(color));
	}
	format(L, w, isbg ? CFM_BACKCOLOR : CFM_COLOR, &cf, scf, TRUE);
	if (isbg && scf == SCF_DEFAULT) 
		SendMessage(w->handle, EM_SETBKGNDCOLOR, 0, cf.crBackColor);
	return 0;
}

LUA_PROPERTY_GET(Edit, border) {
  	lua_pushboolean(L, GetWindowLongPtr(lua_self(L, 1, Widget)->handle, GWL_EXSTYLE) & WS_EX_STATICEDGE);
	return 1;
}

LUA_PROPERTY_SET(Edit, border) {
	HWND h = lua_self(L, 1, Widget)->handle;
	DWORD style =  GetWindowLongPtr(h, GWL_EXSTYLE);
	SetWindowLongPtr(h, GWL_EXSTYLE, lua_toboolean(L, 2) ? style | WS_EX_STATICEDGE : style & ~WS_EX_STATICEDGE);
	RedrawWindow(h, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_ERASE);
	SetWindowPos(h, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	return 0;
}

LUA_PROPERTY_GET(Edit, fgcolor) {
	return get_color(L, lua_self(L, 1, Widget), SCF_DEFAULT, FALSE);
}

LUA_PROPERTY_SET(Edit, fgcolor) {
	return set_color(L, lua_self(L, 1, Widget), SCF_DEFAULT, FALSE);
}

LUA_PROPERTY_SET(Edit, bgcolor) {
	return set_color(L, lua_self(L, 1, Widget), SCF_DEFAULT, TRUE);
}

LUA_PROPERTY_GET(Edit, bgcolor) {
	return get_color(L, lua_self(L, 1, Widget), SCF_DEFAULT, TRUE);
}

LUA_PROPERTY_GET(Selection, color) {
	return get_color(L, NULL, SCF_SELECTION, FALSE);
}

LUA_PROPERTY_SET(Selection, color) {
	return set_color(L, NULL, SCF_SELECTION, FALSE);
}

LUA_PROPERTY_GET(Selection, bgcolor) {
	return get_color(L, NULL, SCF_SELECTION, TRUE);
}

LUA_PROPERTY_SET(Selection, bgcolor) {
	return set_color(L, NULL, SCF_SELECTION, TRUE);
}

LUA_PROPERTY_GET(Selection, text) {
	HWND h = lua_self(L, 3, Widget)->handle;
	wchar_t *buff;
	CHARRANGE cr = {0};

	lua_pushstring(L, "");
	SendMessageW(h, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	if (cr.cpMax != cr.cpMin) {
		size_t len = abs(cr.cpMax-cr.cpMin);
		if (len > INT_MAX)
			luaL_error(L, "Selection text too long");
		buff = malloc(sizeof(wchar_t)*(len+1));
		SendMessageW(h, EM_GETSELTEXT, 0, (LPARAM)buff);
		lua_pushlwstring(L, buff, len);
		free(buff);
	}
	return 1;
}

LUA_PROPERTY_SET(Selection, text) {
	HWND h = lua_self(L, 4, Widget)->handle;
	wchar_t *str = lua_towstring(L, 3);
	SendMessageW(h, EM_REPLACESEL, TRUE, (LPARAM)str);
	free(str);
	return 0;
}

static int align_values[] = { PFA_LEFT, PFA_RIGHT, PFA_CENTER, PFA_JUSTIFY};
static const char *align_options[] = { "left", "right", "center", NULL};

static Widget *paraformat(lua_State *L, DWORD mask, PARAFORMAT *pf, BOOL isset) {
	Widget *w = lua_self(L, lua_gettop(L), Widget);

	pf->cbSize = sizeof(PARAFORMAT);
	pf->dwMask = mask;
	SendMessageW(w->handle, isset ? EM_SETPARAFORMAT: EM_GETPARAFORMAT, SCF_SELECTION, (LPARAM)pf);
	return w;
}

LUA_PROPERTY_GET(Selection, align) {
	PARAFORMAT pf = {0};
	paraformat(L, PFM_ALIGNMENT, &pf, FALSE);
	lua_pushstring(L, align_options[pf.wAlignment-1]);
	return 1;
}

LUA_PROPERTY_SET(Selection, align) {
	PARAFORMAT pf = {0};
	pf.wAlignment = align_values[luaL_checkoption(L, 3, "left", align_options)];
	paraformat(L, PFM_ALIGNMENT, &pf, TRUE);
	return 0;
}

LUA_PROPERTY_GET(Selection, indent) {
	PARAFORMAT pf = {0};
	paraformat(L, PFM_OFFSETINDENT, &pf, FALSE);
	lua_pushinteger(L, pf.dxStartIndent);
	return 1;
}

LUA_PROPERTY_SET(Selection, indent) {
	PARAFORMAT pf = {0};
	pf.dxStartIndent = lua_tointeger(L, 3);
	paraformat(L, PFM_OFFSETINDENT, &pf, TRUE);
	return 0;
}

luaL_Reg Selection_getfuncs[] = {
	{"from",		Selection_getfrom},
	{"to",			Selection_getto},
	{"text",		Selection_gettext},
	{"bgcolor",		Selection_getbgcolor},
	{"fgcolor",		Selection_getcolor},
	{"font",		Selection_getfont},
    {"fontsize",	Selection_getfontsize},
	{"fontstyle",	Selection_getfontstyle},
	{"align",		Selection_getalign},
	{"indent",		Selection_getindent},
	{"visible",		Selection_getvisible},
	{NULL, NULL}
};

luaL_Reg Selection_setfuncs[] = {
	{"from",		Selection_setfrom},
	{"to",			Selection_setto},
	{"text",		Selection_settext},
	{"bgcolor",		Selection_setbgcolor},
	{"fgcolor",		Selection_setcolor},
	{"font",		Selection_setfont},
    {"fontsize",	Selection_setfontsize},
	{"fontstyle",	Selection_setfontstyle},
	{"align",		Selection_setalign},
	{"indent",		Selection_setindent},
	{"visible",		Selection_setvisible},
	{NULL, NULL}
};

LUA_METHOD(Selection, __len) {
	CHARRANGE cr;
	luaL_getmetafield(L, 1, "__widget");

	SendMessageW(lua_self(L, -1, Widget)->handle, EM_EXGETSEL, (WPARAM)0, (LPARAM)&cr);
	lua_pushinteger(L, cr.cpMax-cr.cpMin);
	return 1;
}

LUA_METHOD(Selection, __index) {
	if	(lua_type(L, 2) == LUA_TSTRING) {
		const luaL_Reg *func;
		const char *str = lua_tostring(L, 2);
		for (func = Selection_getfuncs; func->func; func++)
			if (strcmp(func->name, str) == 0) {
				lua_pushcfunction(L, func->func);
				lua_insert(L, 1);
				luaL_getmetafield(L, 2, "__widget");
				if (lua_pcall(L, 3, 1, 0)) {
					luaL_gsub(L, lua_tostring(L, -1), "index", func->name);
					lua_error(L);
				}
			}
	} else lua_pushnil(L);
	return 1;
}

LUA_METHOD(Selection, __newindex) {
	if	(lua_type(L, 2) == LUA_TSTRING) {
		const luaL_Reg *func;
		const char *str = lua_tostring(L, 2);
		for (func = Selection_setfuncs; func->func; func++)
			if (strcmp(func->name, str) == 0) {
				lua_pushcfunction(L, func->func);
				lua_insert(L, 1);
				luaL_getmetafield(L, 2, "__widget");
				if (lua_pcall(L, 4, 1, 0)) {
					const char *msg = lua_tostring(L, -1);
					luaL_where(L, 1);
					lua_pushstring(L, "bad assignment for selection.");
					luaL_gsub(L, msg, "bad argument #3 to '?'", func->name);
					lua_concat(L, 3);
					lua_error(L);
				}
				return 1;
			}
	} 
	return 0;
}

luaL_Reg Selection_metafields[] = {
	{"__len",		Selection___len},
	{"__index",		Selection___index},
	{"__newindex",	Selection___newindex},
	{NULL, NULL}
};

LUA_PROPERTY_GET(Edit, selection) {
	lua_createtable(L, 0, 0);
	lua_createtable(L, 0, 3);
	luaL_setfuncs(L, Selection_metafields, 0);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "__widget");
	lua_setmetatable(L, -2);
	return 1;
}

luaL_Reg Edit_methods[] = {
	{"load",			Edit_loadfrom},
	{"save",			Edit_saveto},
	{"append",			Edit_append},
	{"vscroll",			Widget_vscroll},
	{"hscroll",			Widget_hscroll},
	{"get_selection",	Edit_getselection},
	{"set_font",		Edit_setfont},
	{"get_font",		Edit_getfont},
    {"set_fontsize",	Edit_setfontsize},
    {"get_fontsize",	Edit_getfontsize},
    {"get_fontstyle",	Edit_getfontstyle},
	{"set_fontstyle",	Edit_setfontstyle},
	{"set_fgcolor",		Edit_setfgcolor},
	{"get_fgcolor",		Edit_getfgcolor},
	{"set_bgcolor",		Edit_setbgcolor},
	{"get_bgcolor",		Edit_getbgcolor},
	{"get_line",		Edit_getline},
	{"set_line",		Edit_setline},
	{"get_wordwrap",	Edit_getwordwrap},
	{"set_wordwrap",	Edit_setwordwrap},
	{"get_richtext",	Edit_getrichtext},
	{"set_richtext",	Edit_setrichtext},
	{"get_column",		Edit_getcolumn},
	{"set_column",		Edit_setcolumn},
	{"get_lines",		Edit_getlines},
	{"set_modified",	Entry_setmodified},
	{"get_modified",	Entry_getmodified},
	{"set_readonly",	Edit_setreadonly},
	{"get_readonly",	Edit_getreadonly},
	{"get_caret",		Edit_getcaret},
	{"set_caret",		Edit_setcaret},
	{"get_border",		Edit_getborder},
	{"set_border",		Edit_setborder},
	{"undo",			Entry_undo},
	{"redo",			Entry_redo},
	{"paste",			Entry_paste},
	{"cut",				Entry_cut},
	{"searchup",		Edit_searchup},
	{"searchdown",		Edit_searchdown},
	{"copy",			Entry_copy},
	{"set_text",		Edit_settext},
	{"get_text",		Edit_gettext},
	{"set_richtext",	Edit_setrichtext},
	{"get_richtext",	Edit_getrichtext},
	{"set_rtf",			Edit_setrtf},
	{"get_rtf",			Edit_getrtf},
	{"get_canundo",		Entry_getcanundo},
	{"get_canredo",		Entry_getcanredo},
	{"get_mousepos",	Edit_getmousepos},
	{NULL, NULL}
};

luaL_Reg Entry_methods[] = {
	{"undo",			Entry_undo},
	{"paste",			Entry_paste},
	{"cut",				Entry_cut},
	{"copy",			Entry_copy},
	{"set_masked",		Entry_setmasked},
	{"get_masked",		Entry_getmasked},
	{"set_modified",	Entry_setmodified},
	{"get_modified",	Entry_getmodified},
	{"set_textlimit",	Entry_settextlimit},
	{"get_textlimit",	Entry_gettextlimit},
	{"get_canundo",		Entry_getcanundo},
	{NULL, NULL}
};
