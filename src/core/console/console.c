/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | console.c | LuaRT console module
*/

#define LUA_LIB

#include <File.h>
#include <luart.h>
#include <locale.h>
#include <conio.h>
#include <windows.h>
#include <winuser.h>
#include <io.h>


/* ------------------------------------------------------------------------ */
const char* colors[] = { "black", "blue", "green", "cyan", "red", "purple", "yellow", "white", "gray", "lightblue", "lightgreen", "lightcyan", "lightred", "lightpurple", "lightyellow", "brightwhite"};
const unsigned long colors_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static WORD reset;
static HANDLE std;
static int refin, refout, referr;
static BOOL isfullscreen = FALSE;
wchar_t echochar = 0;

//function helper to set console.stdin.stdout.stderr
int console_setstd(lua_State *L, int ref, File *f) {
	File *std;
	if (f->std)
		luaL_error(L, "cannot assign a standard file with another one");
	if (ref != refin && f->mode != 2)
		luaL_error(L, "File is not open in 'write' mode");
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	std = lua_self(L, -1, File);
	std->stdstream = std->stream;
	std->stream = f->stream;
	std->std = FALSE;
	std->encoding = f->encoding;
	return 0;
}

//-------------------------------------[ console.height ]
LUA_METHOD(console, getheight) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(std, &info);
	lua_pushinteger(L, info.srWindow.Bottom - info.srWindow.Top + 1);
	return 1;
}

//-------------------------------------[ console.width ]
LUA_METHOD(console, getwidth) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(std, &info);
	lua_pushinteger(L, info.dwMaximumWindowSize.X);
	return 1;
}

//-------------------------------------[ console.keyhit ]
LUA_METHOD(console, getkeyhit) {
	lua_pushboolean(L, _kbhit());
	return 1;
}
//-------------------------------------[ console.stdin ]
LUA_METHOD(console, setstdin) {
	return console_setstd(L, refin, lua_self(L, 1, File));
}

LUA_METHOD(console, getstdin) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, refin);
	return 1;
}
//-------------------------------------[ console.stdout ]
LUA_METHOD(console, setstdout) {
	return console_setstd(L, refout, lua_self(L, 1, File));
}

LUA_METHOD(console, getstdout) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, refout);
	return 1;
}
//-------------------------------------[ console.stderr ]
LUA_METHOD(console, setstderr) {
	return console_setstd(L, referr, lua_self(L, 1, File));
}

LUA_METHOD(console, getstderr) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, referr);
	return 1;
}
//-------------------------------------[ console.x ]
LUA_METHOD(console, getx)
{
	CONSOLE_SCREEN_BUFFER_INFO info = {0};
	GetConsoleScreenBufferInfo(std, &info);
	lua_pushinteger(L, info.dwCursorPosition.X);
    return 1;
}

LUA_METHOD(console, setx)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD coord;
	GetConsoleScreenBufferInfo(std, &info);
	coord.X = (short)luaL_checkinteger(L, 1)-1;
	coord.Y = info.dwCursorPosition.Y;
	SetConsoleCursorPosition(std, coord);
	return 0;
}
//-------------------------------------[ console.y ]
LUA_METHOD(console, gety)
{
	CONSOLE_SCREEN_BUFFER_INFO info = {0};
	GetConsoleScreenBufferInfo(std, &info);
	lua_pushinteger(L, info.dwCursorPosition.Y);
    return 1;
}

LUA_METHOD(console, sety)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD coord;
	GetConsoleScreenBufferInfo(std, &info);
	coord.X = info.dwCursorPosition.X;
	coord.Y = (short)(luaL_checkinteger(L, 1)-1+info.srWindow.Top);
	SetConsoleCursorPosition(std, coord);
	return 0;
}

//-------------------------------------[ console.title ]
LUA_METHOD(console, gettitle)
{
    wchar_t buff[255];
    GetConsoleTitleW(buff, 255);
    lua_pushwstring(L, buff);
    return 1;
}
LUA_METHOD(console, settitle)
{
	wchar_t *title = lua_towstring(L, 1);
    SetConsoleTitleW(title);
	free(title);
	return 0;
}

int color(lua_State *L, int idx, BOOL isforeground)
{
	CONSOLE_SCREEN_BUFFER_INFO info = {0};
	int color_index, bg , fg;

	GetConsoleScreenBufferInfo(std, &info);
	bg = (info.wAttributes & 0xFF) >> 4;
	fg = (info.wAttributes & 0xFF) & 15;
	if (!idx)
		return isforeground ? fg : bg;
	else {
		if ((color_index = lua_optstring(L, idx, colors, -1)) == -1)
			luaL_error(L, "unknown color '%s'", lua_tostring(L, idx));
		SetConsoleTextAttribute(std, isforeground ? bg * 16 + colors_values[color_index] : colors_values[color_index] * 16 + fg);
	}
	return 0;
}

LUA_METHOD(console, getcolor)
{
	lua_pushstring(L, colors[color(L, 0, TRUE)]);
	return 1;
}

LUA_METHOD(console, setcolor)
{
	return color(L, 1, TRUE);
}

LUA_METHOD(console, getbgcolor)
{
	lua_pushstring(L, colors[color(L, 0, FALSE)]);
	return 1;
}

LUA_METHOD(console, setbgcolor)
{
	return color(L, 1, FALSE);
}

LUA_METHOD(console, getecho) {
	if (echochar < 2)
		lua_pushboolean(L, !echochar);
	else
		lua_pushlwstring(L, &echochar, 1);
	return 1;
}

LUA_METHOD(console, setecho) {
	if (!lua_isboolean(L, 1)) {
		int len;	
		wchar_t *str = lua_tolwstring(L, 1, &len);
		echochar = *str;
		free(str);
		if (len > 1)
			luaL_error(L, "single character expected");
	} else 
		echochar = lua_toboolean(L, 1) ? 0 : 1;
	return 0;
}
//-------------------------------------[ console.clear() ]
LUA_METHOD(console, clear) {
	COORD coord = {0, 0};
	DWORD n;
  	CONSOLE_SCREEN_BUFFER_INFO info = {0};
	
	console_getbgcolor(L);
	if (lua_gettop(L) == 2)
		color(L, 1, FALSE);
	GetConsoleScreenBufferInfo(std, &info);
	FillConsoleOutputCharacter(std, ' ', info.dwMaximumWindowSize.X * info.dwSize.Y, coord, &n);
	FillConsoleOutputAttribute(std, info.wAttributes, info.dwMaximumWindowSize.X * info.dwSize.Y, coord, &n);
	SetConsoleCursorPosition(std, coord);
	color(L, -1, FALSE);
	return 0;
}
//-------------------------------------[ console.locate() ]
LUA_METHOD(console, locate) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD coord;
	GetConsoleScreenBufferInfo(std, &info);
	coord.X = luaL_checknumber(L,1)-1;
	coord.Y = luaL_checknumber(L,2)-1+info.srWindow.Top;
	SetConsoleCursorPosition(std, coord);
	return 0;
}

LUA_METHOD(console, read)
{
	extern int File_read(lua_State *L);
	luaL_checkinteger(L, 1);
	lua_pushcfunction(L, File_read);
	lua_rawgeti(L, LUA_REGISTRYINDEX, refin);
	lua_pushvalue(L, 1);
	lua_call(L, 2, 1);
	return 1;
}

extern int File_write(lua_State *L);
extern int File_writeln(lua_State *L);

static int con_write(lua_State *L, lua_CFunction func, int start) {
	int n = lua_gettop(L);
	int i = start-1;
	lua_pushcfunction(L, func);
	lua_rawgeti(L, LUA_REGISTRYINDEX, refout);
	while (i < n) {
		lua_pushvalue(L, -2);
		lua_pushvalue(L, -2);
		luaL_tolstring(L, ++i, NULL);
		if (i < n) {
			lua_pushstring(L, " ");
			lua_concat(L, 2);
		}
		lua_call(L, 2, 0);
	}
	return 0;
}

LUA_METHOD(console, readln)
{
	extern int File_readln(lua_State *L);
	
	if (lua_gettop(L))
		con_write(L, File_write, 1);
	lua_pushcfunction(L, File_readln);
	lua_rawgeti(L, LUA_REGISTRYINDEX, refin);
	lua_call(L, 1, 1);
	return 1;
}

LUA_METHOD(console, write) {
	return con_write(L, File_write, 1);
}

LUA_METHOD(console, writecolor) {
	int prev = color(L, 0, TRUE);
	CONSOLE_SCREEN_BUFFER_INFO info;

	color(L, 1, TRUE); //-- set color
	con_write(L, File_write, 2);
	GetConsoleScreenBufferInfo(std, &info);
	SetConsoleTextAttribute(std, ((info.wAttributes & 0xFF) >> 4) * 16 + colors_values[prev]);
	return 0;
}

LUA_METHOD(console, writeln) {
	if (lua_gettop(L) == 0)
		lua_pushstring(L, "\n");
	return con_write(L, File_writeln, 1);
}

LUA_METHOD(console, readmouse) {
	HANDLE hin = GetStdHandle(STD_INPUT_HANDLE)	;
	INPUT_RECORD InputRecord;
	DWORD Events, mode = 0;
	CONSOLE_CURSOR_INFO cci;
	
	GetConsoleMode(hin, &mode);
    cci.dwSize = 25;
    cci.bVisible = FALSE;
	SetConsoleCursorInfo(std, &cci);
	SetConsoleMode(hin, ((mode | ENABLE_MOUSE_INPUT) & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_EXTENDED_FLAGS);
	while(TRUE) {
		ReadConsoleInput(hin, &InputRecord, 1, &Events);
		if (Events && (InputRecord.EventType == MOUSE_EVENT)) {
			lua_pushinteger(L, InputRecord.Event.MouseEvent.dwMousePosition.X + 1);
			lua_pushinteger(L, InputRecord.Event.MouseEvent.dwMousePosition.Y + 1);
			switch (InputRecord.Event.MouseEvent.dwButtonState) {
				case 0: lua_pushnil(L); break;
				case FROM_LEFT_1ST_BUTTON_PRESSED: lua_pushstring(L, "left"); break;
				case RIGHTMOST_BUTTON_PRESSED:	lua_pushstring(L, "right"); break;
				default: lua_pushstring(L, "middle");
			}
			break;
		} 
	}	 
	SetConsoleMode(hin, mode);
	return 3;
}

LUA_METHOD(console, readchar) {
	DWORD cc;
    INPUT_RECORD irec;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	BOOL isspecial = FALSE;
 
	if (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &cc) == FALSE) {
		wchar_t ch;
		ReadFile(h, &ch, 2, &cc, NULL);
		lua_pushlwstring(L, &ch, 1);
	} else while (TRUE) {
		ReadConsoleInputW( h, &irec, 1, &cc );
		if ((irec.EventType == KEY_EVENT) &&  irec.Event.KeyEvent.bKeyDown) {
			if (irec.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY) {
				char c = 34+irec.Event.KeyEvent.wVirtualKeyCode;
				lua_pushlstring(L, &c, 1);
				isspecial = TRUE;
			} else if (irec.Event.KeyEvent.uChar.AsciiChar)
				lua_pushlwstring(L, &irec.Event.KeyEvent.uChar.UnicodeChar, 1);
			else continue;
			break;
		}
	}
	lua_pushboolean(L, isspecial);
	return 2;
}

LUA_METHOD(console, inverse) {
	WORD inverse;
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(std, &info);
	inverse = ((info.wAttributes & 0x0F) << 4) | ((info.wAttributes & 0xF0) >> 4);
	SetConsoleTextAttribute(std, inverse);
	return 0;
}

LUA_METHOD(console, reset) {
	SetConsoleTextAttribute(std, reset);
	return 0;
}

LUA_PROPERTY_GET(console, fullscreen) {
	lua_pushboolean(L, isfullscreen);
	return 1;
}

LUA_PROPERTY_SET(console, fullscreen) {
	BOOL value = lua_toboolean(L, 1);

	if (value != isfullscreen) {
		keybd_event(VK_MENU,0x38,0,0);
		keybd_event(VK_RETURN,0x1c,0,0);
		keybd_event(VK_RETURN,0x1c,KEYEVENTF_KEYUP,0);
		keybd_event(VK_MENU,0x38,KEYEVENTF_KEYUP,0);
		Sleep(50);
		isfullscreen = value;
		ShowScrollBar(GetConsoleWindow(), SB_VERT, !value);
	}
	return 0;
}

LUA_API int fontsize_fromheight(int height) {
	return height < 0 ? MulDiv(-height, 72, GetDeviceCaps(GetDC(0), LOGPIXELSY)) : height;
}

LUA_PROPERTY_GET(console, font) {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if (GetCurrentConsoleFontEx(std, FALSE, &cfi))
		lua_pushwstring(L, cfi.FaceName);
	else luaL_pushfail(L);
	return 1;
}

LUA_PROPERTY_GET(console, fontsize) {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if (GetCurrentConsoleFontEx(std, FALSE, &cfi))
		lua_pushinteger(L, cfi.dwFontSize.Y); 
	else luaL_pushfail(L);
	return 1;
}

LUA_PROPERTY_SET(console, fontsize) {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(std, FALSE, &cfi);
	cfi.dwFontSize.Y = luaL_checkinteger(L, 1);
	SetCurrentConsoleFontEx(std, FALSE, &cfi);
	return 1;
}


int CALLBACK EnumSystemFonts(const LOGFONTW *lf, const TEXTMETRICW *tm, DWORD ft, LPARAM lparam)
{	
	return 0;
}

LUA_API BOOL LoadFont(LPCWSTR file, LPLOGFONTW lf) {
	typedef BOOL(WINAPI *PGetFontResourceInfo)(LPCWSTR, LPDWORD, LPVOID, DWORD);
	HDC dc = GetDC(NULL);
	BOOL result;

	if (_waccess(file, 0) == 0) {
  		while (RemoveFontResourceW(file));
  		if (AddFontResourceExW(file, 0, 0)) {
  			DWORD n = sizeof(LOGFONTW);
  			PGetFontResourceInfo GetFontResourceInfo = (PGetFontResourceInfo)(void*)GetProcAddress(GetModuleHandleA("gdi32.dll"), "GetFontResourceInfoW");
  			GetFontResourceInfo(file, &n, lf, 2);
			RemoveFontResourceW(file);
		} else return FALSE;
  	} else wcsncpy(lf->lfFaceName, file, LF_FACESIZE);	
	result = !EnumFontFamiliesExW(dc, lf, EnumSystemFonts, (LPARAM)NULL, 0);
	ReleaseDC(NULL, dc);
	return result;
}

LUA_PROPERTY_SET(console, font) {
	CONSOLE_FONT_INFOEX cfi;
	wchar_t *str = luaL_checkFilename(L, 1);
	LOGFONTW lf = {0};

	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(std, FALSE, &cfi);
	if (!LoadFont(str, &lf))
		luaL_error(L, "failed to load font");
	free(str);
	cfi.nFont = 0;
	cfi.FontFamily = TMPF_TRUETYPE | TMPF_VECTOR | TMPF_FIXED_PITCH;
	wcsncpy(cfi.FaceName, lf.lfFaceName, LF_FACESIZE);
	if (!SetCurrentConsoleFontEx(std, FALSE, &cfi))
		luaL_error(L, "invalid console font");
	return 0;
}

LUA_PROPERTY_SET(console, cursor) {
   	CONSOLE_CURSOR_INFO info;
   	info.dwSize = 20;
   	info.bVisible = lua_toboolean(L, 1);
   	SetConsoleCursorInfo(std, &info);
	return 0;
}

LUA_PROPERTY_GET(console, cursor) {
   	CONSOLE_CURSOR_INFO info;
  	GetConsoleCursorInfo(std, &info);
	lua_pushboolean(L, info.bVisible);
	return 1;
}

/* ------------------------------------------------------------------------ */

static const luaL_Reg consolelib[] = {
	{"read",		console_read},
	{"readchar",	console_readchar},
	{"readmouse",	console_readmouse},
	{"clear",		console_clear},
	{"readln",		console_readln},
	{"write",		console_write},
	{"writeln",		console_writeln},
	{"writecolor",	console_writecolor},
	{"inverse",		console_inverse},
	{"reset",		console_reset},
	{"locate",		console_locate},
	{NULL, NULL}
};

static const luaL_Reg console_properties[] = {
	{"get_width",	console_getwidth},
	{"get_height",	console_getheight},
	{"get_x",		console_getx},
	{"get_y",		console_gety},
	{"set_x",		console_setx},
	{"set_y",		console_sety},
	{"get_keyhit",	console_getkeyhit},
	{"get_stdin",	console_getstdin},
	{"set_stdin",	console_setstdin},
	{"get_stdout",	console_getstdout},
	{"set_stdout",	console_setstdout},
	{"get_stderr",	console_getstderr},
	{"set_stderr",	console_setstderr},
	{"get_title",	console_gettitle},
	{"set_title",	console_settitle},
	{"get_color",	console_getcolor},
	{"set_color",	console_setcolor},
	{"get_bgcolor",	console_getbgcolor},
	{"set_bgcolor",	console_setbgcolor},
	{"get_echo",	console_getecho},
	{"set_echo",	console_setecho},
	{"get_fullscreen",	console_getfullscreen},
	{"set_fullscreen",	console_setfullscreen},
	{"get_font"		 ,	console_getfont},
	{"set_font"		 ,	console_setfont},
	{"get_fontsize"	,	console_getfontsize},
	{"set_fontsize"	 ,	console_setfontsize},
	{"get_cursor"	,	console_getcursor},
	{"set_cursor"	 ,	console_setcursor},
	{NULL, NULL}
};

int create_stdfile(lua_State *L, FILE *f, wchar_t *name, int mode) {
	File *F;
	lua_pushwstring(L, name);
	F = lua_pushinstance(L, File, 1);
	F->stream = f;
	F->encoding = UTF8;
	free(F->fullpath);
	F->fullpath = _wcsdup(name);
	F->std = TRUE;
	F->mode = mode;
	F->h = (HANDLE)_get_osfhandle(_fileno(f));
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

#include <fcntl.h>

LUALIB_API int console_finalize(lua_State *L) {
	return console_reset(L);
}

LUAMOD_API int luaopen_console(lua_State *L) {
	CONSOLE_SCREEN_BUFFER_INFO info = {0};
	DWORD mode = 0;
	if ((GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode) == FALSE) && AllocConsole()) {
    	FILE* fDummy;
		DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);
 		HANDLE hConOut = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hConIn = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
		SetStdHandle(STD_ERROR_HANDLE, hConOut);
		SetStdHandle(STD_INPUT_HANDLE, hConIn);		
		SetConsoleOutputCP(65001);
		setlocale(LC_ALL, ".UTF8");
	}
	std = GetStdHandle(STD_OUTPUT_HANDLE);

    RECT a, b;
    GetWindowRect(GetConsoleWindow(), &a);
    GetWindowRect(GetDesktopWindow(), &b);
    isfullscreen = memcmp(&a, &b, sizeof(RECT)) == 0;
	GetConsoleScreenBufferInfo(std, &info);
	reset = info.wAttributes;
	refin = create_stdfile(L, stdin, L"stdin", 0);
	refout = create_stdfile(L, stdout, L"stdout", 2);
	referr = create_stdfile(L, stderr, L"stderr", 2);
	lua_regmodulefinalize(L, console);
	return 1;
}
