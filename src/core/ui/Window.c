/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Window.c | LuaRT Window object implementation
*/

#define LUA_LIB

#include <luart.h>
#include <Widget.h>
#include "ui.h"
#include <Window.h>
#include <windowsx.h>
#include <dwmapi.h>

luart_type TWindow;

static HANDLE hwndPrevious;

#define Stringify(x) case x: return #x

static const char *VKString(int vk) {
    if (vk >= '0' && vk <= '9')
		return NULL;
    if (vk >= 'A' && vk <= 'Z')
		return NULL;
    switch(vk) {
        Stringify(VK_LBUTTON);
        Stringify(VK_RBUTTON);
        Stringify(VK_CANCEL);
        Stringify(VK_MBUTTON);  
        Stringify(VK_XBUTTON1); 
        Stringify(VK_XBUTTON2); 
        Stringify(VK_BACK);
        Stringify(VK_TAB);
        Stringify(VK_CLEAR);
        Stringify(VK_RETURN);
        Stringify(VK_SHIFT);
        Stringify(VK_CONTROL);
        Stringify(VK_MENU);
        Stringify(VK_PAUSE);
        Stringify(VK_CAPITAL);
        Stringify(VK_KANA);
        Stringify(VK_JUNJA);
        Stringify(VK_FINAL);
        Stringify(VK_KANJI);
        Stringify(VK_ESCAPE);
        Stringify(VK_CONVERT);
        Stringify(VK_NONCONVERT);
        Stringify(VK_ACCEPT);
        Stringify(VK_MODECHANGE);
        Stringify(VK_SPACE);
        Stringify(VK_PRIOR);
        Stringify(VK_NEXT);
        Stringify(VK_END);
        Stringify(VK_HOME);
        Stringify(VK_LEFT);
        Stringify(VK_UP);
        Stringify(VK_RIGHT);
        Stringify(VK_DOWN);
        Stringify(VK_SELECT);
        Stringify(VK_PRINT);
        Stringify(VK_EXECUTE);
        Stringify(VK_SNAPSHOT);
        Stringify(VK_INSERT);
        Stringify(VK_DELETE);
        Stringify(VK_HELP);
        Stringify(VK_LWIN);
        Stringify(VK_RWIN);
        Stringify(VK_APPS);
        Stringify(VK_SLEEP);
        Stringify(VK_NUMPAD0);
        Stringify(VK_NUMPAD1);
        Stringify(VK_NUMPAD2);
        Stringify(VK_NUMPAD3);
        Stringify(VK_NUMPAD4);
        Stringify(VK_NUMPAD5);
        Stringify(VK_NUMPAD6);
        Stringify(VK_NUMPAD7);
        Stringify(VK_NUMPAD8);
        Stringify(VK_NUMPAD9);
        Stringify(VK_MULTIPLY);
        Stringify(VK_ADD);
        Stringify(VK_SEPARATOR);
        Stringify(VK_SUBTRACT);
        Stringify(VK_DECIMAL);
        Stringify(VK_DIVIDE);
        Stringify(VK_F1);
        Stringify(VK_F2);
        Stringify(VK_F3);
        Stringify(VK_F4);
        Stringify(VK_F5);
        Stringify(VK_F6);
        Stringify(VK_F7);
        Stringify(VK_F8);
        Stringify(VK_F9);
        Stringify(VK_F10);
        Stringify(VK_F11);
        Stringify(VK_F12);
        Stringify(VK_F13);
        Stringify(VK_F14);
        Stringify(VK_F15);
        Stringify(VK_F16);
        Stringify(VK_F17);
        Stringify(VK_F18);
        Stringify(VK_F19);
        Stringify(VK_F20);
        Stringify(VK_F21);
        Stringify(VK_F22);
        Stringify(VK_F23);
        Stringify(VK_F24);
        Stringify(VK_NUMLOCK);
        Stringify(VK_SCROLL);
        Stringify(VK_OEM_NEC_EQUAL);
        Stringify(VK_OEM_FJ_MASSHOU);
        Stringify(VK_OEM_FJ_TOUROKU);
        Stringify(VK_OEM_FJ_LOYA);
        Stringify(VK_OEM_FJ_ROYA);
        Stringify(VK_LSHIFT);
        Stringify(VK_RSHIFT);
        Stringify(VK_LCONTROL);
        Stringify(VK_RCONTROL);
        Stringify(VK_LMENU);
        Stringify(VK_RMENU);
        Stringify(VK_BROWSER_BACK);
        Stringify(VK_BROWSER_FORWARD);
        Stringify(VK_BROWSER_REFRESH);
        Stringify(VK_BROWSER_STOP);
        Stringify(VK_BROWSER_SEARCH);
        Stringify(VK_BROWSER_FAVORITES);
        Stringify(VK_BROWSER_HOME);
        Stringify(VK_VOLUME_MUTE);
        Stringify(VK_VOLUME_DOWN);
        Stringify(VK_VOLUME_UP);
        Stringify(VK_MEDIA_NEXT_TRACK);
        Stringify(VK_MEDIA_PREV_TRACK);
        Stringify(VK_MEDIA_STOP);
        Stringify(VK_MEDIA_PLAY_PAUSE);
        Stringify(VK_LAUNCH_MAIL);
        Stringify(VK_LAUNCH_MEDIA_SELECT);
        Stringify(VK_LAUNCH_APP1);
        Stringify(VK_LAUNCH_APP2);
        Stringify(VK_OEM_1);
        Stringify(VK_OEM_PLUS);
        Stringify(VK_OEM_COMMA);
        Stringify(VK_OEM_MINUS);
        Stringify(VK_OEM_PERIOD);
        Stringify(VK_OEM_2);
        Stringify(VK_OEM_3);
        Stringify(VK_OEM_4);
        Stringify(VK_OEM_5);
        Stringify(VK_OEM_6);
        Stringify(VK_OEM_7);
        Stringify(VK_OEM_8);
        Stringify(VK_OEM_AX);
        Stringify(VK_OEM_102);
        Stringify(VK_ICO_HELP);
        Stringify(VK_ICO_00);
        Stringify(VK_PROCESSKEY);
        Stringify(VK_ICO_CLEAR);
        Stringify(VK_PACKET);
    }
	return NULL;
}

BOOL CALLBACK ResizeChilds(HWND h, LPARAM lParam)
{
	if (GetParent(h) == (HWND)lParam) {	
		Widget *w = (Widget*)GetWindowLongPtr(h, GWLP_USERDATA);
		if (w)
			do_align(w);
	}
	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {	
	Widget *w = (Widget*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	UINT flags;

	if (w) {
		switch(Msg) {
			case WM_COMMAND:
				if (lParam) {
					PostMessageW((HWND)lParam, WM_COMMAND, wParam, lParam);
					return 0;
				}
				else if (HIWORD(wParam)) {
					lua_menuevent(LOWORD(wParam), 0);
					return 0;
				}
				break;
			case WM_MENUCOMMAND: {
									MENUITEMINFOW mi = {0};
									mi.cbSize = sizeof(MENUITEMINFOW);
									mi.fMask = MIIM_DATA | MIIM_ID;
									GetMenuItemInfoW((HMENU)lParam, wParam, TRUE, &mi);
									if (mi.wID)
										lua_menuevent(mi.wID, -1);
									else if (mi.dwItemData)
										lua_menuevent(((Widget*)mi.dwItemData)->ref, wParam);
	    							return 0;
								 }
			case WM_WINDOWPOSCHANGING:
				if (!(((WINDOWPOS*)lParam)->flags & SWP_NOMOVE))
					lua_callevent(w, onMove);
				if (!(((WINDOWPOS*)lParam)->flags & SWP_NOSIZE))					
					lua_callevent(w, onResize);
				break;
			case WM_WINDOWPOSCHANGED:
				if (w->status) {
					RECT r;
					GetClientRect(hWnd, &r);
					SendMessage(w->status, WM_SIZE, 0, MAKELPARAM(r.right, r.bottom));
				}
				flags = ((WINDOWPOS*)lParam)->flags;
				if (flags & SWP_HIDEWINDOW)
					lua_callevent(w, onHide);
				else if (flags & SWP_SHOWWINDOW)
					lua_callevent(w, onShow);
				if (!(flags & SWP_NOSIZE)) {
					if (w->status)
						SendMessage(w->status, WM_SIZE, 0, 0);
					EnumChildWindows(hWnd, ResizeChilds, (LPARAM)hWnd);
					lua_callevent(w, onResize);
				}
				break;
			case WM_MOUSEMOVE:	
				if (hwndPrevious != hWnd) {
					if (hwndPrevious)
						PostMessage(hwndPrevious, WM_MOUSELEAVE,0,0);
					hwndPrevious = hWnd;
				}
				lua_paramevent(w, onHover, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				if (w->status)
					BringWindowToTop(w->status);
				return 0;
			case WM_NOTIFY:
				if (w->status != ((LPNMHDR)lParam)->hwndFrom)
					return SendMessageW(((LPNMHDR)lParam)->hwndFrom, WM_NOTIFY, wParam, lParam);	
				break;
			case WM_CLOSE: {
				lua_closeevent(w, onClose);
				return 0;
			}

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				lua_paramevent(w, onMouseUp, (Msg-WM_LBUTTONUP)/3, lParam);
				break;

			case WM_MBUTTONDOWN:
				lua_paramevent(w, onMouseDown, 2, lParam);
				break;
			case WM_RBUTTONDOWN:
				lua_paramevent(w, onMouseDown, 1, lParam);
				lua_callevent(w, onContext);
				break;
			case WM_LBUTTONDOWN:
				lua_paramevent(w, onMouseDown, 0, lParam);
				lua_paramevent(w, onClick, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				SetFocus(w->handle);
        		return 0;
			case WM_DESTROY:		
				PostQuitMessage(0);
				return 0;
			case WM_SIZE: {
				RECT sbRect;
				UINT sbheight;				
				if (w->status && IsWindowVisible(hWnd)) {
					GetWindowRect(w->status, &sbRect);
					sbheight = sbRect.bottom - sbRect.top;
					SetWindowPos(w->status, HWND_TOP, 0, HIWORD(lParam)-sbheight, LOWORD(lParam), sbheight, SWP_SHOWWINDOW);
				}
				lua_callevent(w, onResize);
				return 0;
             }
			case WM_SIZING:
			    EnumChildWindows(hWnd, ResizeChilds, (LPARAM)hWnd);
				break;
            case WM_SETCURSOR:
				if (LOWORD(lParam) == HTCLIENT) {
					POINT p;
					GetCursorPos(&p);
					ScreenToClient(w->handle, &p);
					if (w->handle == ChildWindowFromPoint(w->handle, p)) {
						SetCursor(w->hcursor);
						return TRUE;
					}
				}
				break;
			case WM_APP:
				switch (lParam) {
					case WM_RBUTTONUP:		lua_callevent(w, onTrayContext);
											break;
					case WM_LBUTTONDOWN:	lua_callevent(w, onTrayClick);
											break;
					case WM_LBUTTONDBLCLK:	lua_callevent(w, onTrayDoubleClick);
											break;
					case WM_MOUSEMOVE:		lua_callevent(w, onTrayHover);
											break;
				} return 0;
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				lua_paramevent(w, onKey, VKString(wParam), wParam);
				break;
			case WM_PAINT:	{
				PAINTSTRUCT ps;
				RECT rc;
				HDC hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rc);
				FillRect(hdc, &rc, (HBRUSH)w->brush);
				EndPaint(hWnd, &ps);
				return 0;
			}
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORSTATIC: return widget_setcolors(w, (HDC)wParam, (HWND)lParam);
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

static const char *styles[] = {  "dialog", "fixed", "float", "raw", "single", NULL };
static int style_values[] = { WS_OVERLAPPEDWINDOW, WS_DLGFRAME | WS_SYSMENU, WS_EX_PALETTEWINDOW, WS_POPUP, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX};

extern int size(Widget *w, lua_State *L, int offset_from, int offset_to, BOOL set, LONG value, BOOL iswidth);

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
//---------- in case dwmapi.h is not uptodate
#define DWMWA_WINDOW_CORNER_PREFERENCE  33
#endif

#ifndef _MSC_VER
typedef enum {
    DWMWCP_DEFAULT                                 = 0,
    DWMWCP_DONOTROUND                              = 1,
    DWMWCP_ROUND                                   = 2,
    DWMWCP_ROUNDSMALL                              = 3

} DWM_WINDOW_CORNER_PREFERENCE;
#endif

LUA_CONSTRUCTOR(Window) {
	Widget *w = (Widget*)calloc(1, sizeof(Widget));
	wchar_t *title;
	int i;
	DWORD style; RECT r = {0};
	static HINSTANCE hInstance = NULL;
	NOTIFYICONDATAW *nid;
	NONCLIENTMETRICS ncm;
	if (!hInstance)
		hInstance = GetModuleHandle(NULL);
	title = lua_towstring(L, 2);
	i = lua_type(L, 3) == LUA_TSTRING ? 4 : 3;
	style = i == 4 ? luaL_checkoption(L, 3, "dialog", styles) : 0;
	r.bottom = luaL_optinteger(L, i+1, 480);
	r.right = luaL_optinteger(L, i, 640);
	w->handle = CreateWindowExW(uiLayout, L"Window", title, style_values[style] | WS_EX_CONTROLPARENT | DS_CONTROL, CW_USEDEFAULT, CW_USEDEFAULT, r.right, r.bottom, HWND_DESKTOP, NULL, hInstance, NULL);
	switch(style) {
		case 3: 	{
						DWM_WINDOW_CORNER_PREFERENCE d = DWMWCP_ROUND;
						DwmSetWindowAttribute(w->handle, DWMWA_WINDOW_CORNER_PREFERENCE, &d, sizeof(DWM_WINDOW_CORNER_PREFERENCE));
					} 
					w->style = WS_POPUP; break;
		case 0:		w->style = WS_CAPTION | WS_THICKFRAME; break;
		case 4:		
		case 1:		SetWindowLong(w->handle, GWL_STYLE, GetWindowLongPtr(w->handle, GWL_STYLE) & ~WS_MAXIMIZEBOX); 
		case 2:		
		default:	w->style = WS_CAPTION;
	}
  	AdjustWindowRectEx(&r, GetWindowLongPtr(w->handle, GWL_STYLE), FALSE, GetWindowLongPtr(w->handle, GWL_EXSTYLE));
	SetWindowPos(w->handle, 0, 0, 0, r.right-r.left, r.bottom-r.top, SWP_HIDEWINDOW | SWP_NOMOVE);
	free(title); 
	SetWindowLongPtr(w->handle, GWLP_USERDATA, (ULONG_PTR)w);
	w->brush = GetSysColorBrush(COLOR_BTNFACE);
	w->wtype = UIWindow;
	w->align = -1;
	w->wp.length = sizeof(WINDOWPLACEMENT);
	w->hcursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	w->font = CreateFontIndirect(&ncm.lfMessageFont);
	SendMessage(w->handle, WM_SETFONT, (WPARAM)w->font, MAKELPARAM(TRUE, 0));
	lua_newinstance(L, w, Widget);
	lua_pushvalue(L, 1);
	w->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	nid = calloc(1, sizeof(NOTIFYICONDATAW));
	nid->hWnd = w->handle;
	nid->uID  = (UINT)w->ref;
	nid->uCallbackMessage = WM_APP;
	w->imglist =  (HIMAGELIST)nid;
	lua_callevent(w, onCreate);
	return 1;
}

LUA_METHOD(Window, maximize) {
	ShowWindow(lua_self(L, 1, Widget)->handle, SW_MAXIMIZE);
	return 0;
}

LUA_METHOD(Window, minimize) {
	ShowWindow(lua_self(L, 1, Widget)->handle, SW_MINIMIZE);
	return 0;
}

LUA_METHOD(Window, showmodal) {
	HWND parent = lua_self(L, 1, Widget)->handle;
	Widget *child = lua_self(L, 2, Widget);
	
	child->tooltip = parent;
	ShowWindow(child->handle, SW_SHOWDEFAULT);
	EnableWindow(parent, FALSE);
	return 0;
}

LUA_PROPERTY_GET(Window, fullscreen) {
	Widget *w = lua_self(L, 1, Widget);
	HWND h = w->handle;

  	lua_pushboolean(L, !(GetWindowLongPtr(h, GWL_STYLE) & w->style));
	return 1;
}

LUA_PROPERTY_SET(Window, fullscreen) {
	Widget *w = lua_self(L, 1, Widget);
	HWND h = w->handle;
	DWORD dwStyle = GetWindowLongPtr(h, GWL_STYLE);

	if (lua_toboolean(L, 2)) {
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetWindowPlacement(h, &w->wp);
		SetWindowLongPtr(h, GWL_STYLE, dwStyle & ~w->style);
		if (GetMonitorInfo(MonitorFromWindow(h, MONITOR_DEFAULTTOPRIMARY), &mi))
			SetWindowPos(h, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	} else {
		SetWindowPlacement(h, &w->wp);
		SetWindowLongPtr(h, GWL_STYLE, dwStyle | w->style);
		SetWindowPos(h, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	return 0;
}

LUA_METHOD(Window, loadtrayicon) {
	Widget *w = lua_self(L, 1, Widget);
	NOTIFYICONDATAW *nid = (NOTIFYICONDATAW *)w->imglist;
	DWORD action = NIM_DELETE;
	
	if (lua_gettop(L) > 1) {		
		action = NIM_ADD;
		nid->uFlags           = NIF_ICON | NIF_MESSAGE;
		nid->hIcon			 = widget_loadicon(L);
	}
	Shell_NotifyIconW(action, nid);
	return 0;
}

LUA_METHOD(Window, status) {
	Widget *win = lua_self(L, 1, Widget);
	int i, n;
	HANDLE handle;
	
	if (!(handle = win->status)) {
		handle = CreateWindowExW(0, STATUSCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_CLIPCHILDREN, 0, 0, 0, 0, win->handle, NULL, GetModuleHandle(NULL), NULL);
		win->status = handle;
	}
	if ((n = (lua_gettop(L)-1))) {
		HDC dc = GetWindowDC(handle);
		int *parts = calloc(n, sizeof(int));
		int len;
		RECT r;
		wchar_t **str = calloc(n,sizeof(wchar_t*));
		LONG size = 0;
		for(i = 0; i < n; i++) {
			luaL_tolstring(L, i+2, NULL);
			str[i] = lua_tolwstring(L, -1, &len);
			if (i+1 == n)
				parts[i] = -1;
			else {
				DrawTextW(dc, str[i], len, &r, DT_CALCRECT);
				size += r.right-r.left+8;
				parts[i] = size;
				memset(&r, 0, sizeof(RECT));
			}
		}
		SendMessage(handle, SB_SETPARTS, n, (LPARAM)parts);
		for(i = 0; i < n; i++) {
			SendMessageW(handle, SB_SETTEXTW, i, (LPARAM)str[i]);
			free(str[i]);
		}
		free(str);
		free(parts);
	}
	BringWindowToTop(handle);
	return 0;
}

LUA_PROPERTY_SET(Window, traytooltip) {
	Widget *w = lua_self(L, 1, Widget);
	wchar_t *tip = lua_towstring(L, 2);

	((NOTIFYICONDATAW *)w->imglist)->uFlags = NIF_TIP;
	wcsncpy(((NOTIFYICONDATAW *)w->imglist)->szTip, tip, 64);
	free(tip);	
	Shell_NotifyIconW(NIM_MODIFY, (NOTIFYICONDATAW *)w->imglist);
	return 0;
}

LUA_PROPERTY_GET(Window, traytooltip) {
	Widget *w = lua_self(L, 1, Widget);

	lua_pushwstring(L, ((NOTIFYICONDATAW *)w->imglist)->szTip);
	return 1;
}
LUA_PROPERTY_GET(Window, menu) {
	Widget *w = lua_self(L, 1, Widget);
	
	lua_pushnil(L);
	if (w->menu)
		lua_rawgeti(L, LUA_REGISTRYINDEX, w->menu);	
	return 1;
}

LUA_METHOD(Window, popup) {
	Widget *w = check_widget(L, 2, UIMenu);
	HANDLE h = NULL, hwin = lua_self(L, 1, Widget)->handle;
	POINT p;	
	
	if (w->parent) {
		h = CreatePopupMenu();
		copy_menuitems(L, w->handle, h);
	}
	GetCursorPos(&p);
	SetForegroundWindow(hwin);
	TrackPopupMenu(h ? h : w->handle, TPM_RIGHTBUTTON, p.x, p.y, 0, hwin,  NULL);
	DestroyMenu(h);
	return 0;

}

extern void init_menu(HMENU h);

LUA_PROPERTY_SET(Window, menu) {
	Widget *w = lua_self(L, 1, Widget);
	Widget *menu = lua_isnil(L,2) ? NULL : check_widget(L, 2, UIMenu);
		
	if (menu && (w->menu != menu->ref)) {
		HMENU new = CreateMenu();
		copy_menuitems(L, menu->handle, new);
		FreeMenu(L, menu);
		menu->handle = new;
		init_menu(new);
		w->menu = menu->ref;
		menu->parent = w->handle;
		SetMenu(w->handle, new);
	} else {
		if (w->menu) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, w->menu);	
			luaL_getmetafield(L, -1, "__userdata");
			FreeMenu(L, lua_touserdata(L, -1));
		}
	}
	DrawMenuBar(w->handle);
	return 0;
}

LUA_METHOD(Window, shortcut) {
	Widget *parent = lua_self(L, 1, Widget);
	ACCEL *accel_array;
	ACCEL accel = {0};
	int count = CopyAcceleratorTableW(parent->accel_table, NULL, 0);
    HKL hk = GetKeyboardLayout(GetCurrentThreadId());
    wchar_t *str = lua_towstring(L, 2);
	int n;

	accel.key = VkKeyScanExW(*str, hk); 	
	free(str);
	accel.fVirt = FVIRTKEY;
	luaL_checktype(L, 3, LUA_TFUNCTION);
	if ( lua_toboolean(L, 4) )
		accel.fVirt |= FCONTROL;
	if ( lua_toboolean(L, 5) )
		accel.fVirt |= FSHIFT;
	if ( lua_toboolean(L, 6) )
		accel.fVirt |= FALT;
	lua_pushvalue(L, 3);
	n = luaL_ref(L, LUA_REGISTRYINDEX);
	accel.cmd = (WORD) (n > USHRT_MAX ? luaL_error(L, "shortcuts limit exceeded") : n);
	accel_array = calloc(count+1, sizeof(ACCEL));
	CopyAcceleratorTableW(parent->accel_table, accel_array, count);
	accel_array[count] = accel;
	DestroyAcceleratorTable(parent->accel_table);
	parent->accel_table = CreateAcceleratorTableW(accel_array, count+1);
	free(accel_array);
	return 0;
}

LUA_METHOD(Window, onClose) {
	lua_pushboolean(L, TRUE);
	return 1;	
}

LUA_METHOD(Window, __gc) {
	Widget *w = lua_self(L, 1, Widget);
	SendMessage(w->handle, WM_CLOSE, 0, 0);
	DestroyWindow(w->status);
	free(w->imglist);
	w->imglist = NULL;
	return Widget___gc(L);
}

OBJECT_METAFIELDS(Window)
	METHOD(Window, __gc)
END

OBJECT_MEMBERS(Window)
	METHOD(Window, onClose)
	METHOD(Window, showmodal)
	METHOD(Window, minimize)
	METHOD(Window, maximize)
	METHOD(Window, status)
	METHOD(Window, loadtrayicon)
	METHOD(Widget, loadicon)
	METHOD(Window, shortcut)
	METHOD(Window, popup)
	METHOD(Widget, center)
	READWRITE_PROPERTY(Window, traytooltip)
	READWRITE_PROPERTY(Window, fullscreen)
	READWRITE_PROPERTY(Window, menu)
	READWRITE_PROPERTY(Widget, bgcolor)
 	{"set_title",		Widget_settext},
 	{"get_title",		Widget_gettext},
END