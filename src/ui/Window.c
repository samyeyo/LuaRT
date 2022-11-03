/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Window.c | LuaRT Window object implementation
*/

#include <luart.h>
#include "Widget.h"
#include <Window.h>

#include <windowsx.h>

luart_type TWindow;

static HANDLE hwndPrevious;

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
				if (!(((WINDOWPOS*)lParam)->flags & SWP_NOSIZE)) {
					lua_callevent(w, onResize);
					return 0;
				}
				break;
			case WM_WINDOWPOSCHANGED:	
				if (w->status) {
					RECT r;
					GetClientRect(w->handle, &r);
					SendMessage(w->status, WM_SIZE, 0, MAKELPARAM(r.right, r.bottom));
				}
				flags = ((WINDOWPOS*)lParam)->flags;
				if (!(flags & SWP_NOMOVE)) {
					lua_callevent(w, onMove);
					return 0;
				}
				if (flags & SWP_HIDEWINDOW)
					lua_callevent(w, onHide);
				else if (flags & SWP_SHOWWINDOW)
					lua_callevent(w, onShow);
				if (!(flags & SWP_NOSIZE)) {
					if (w->status)
						SendMessage(w->status, WM_SIZE, 0, 0);
					lua_callevent(w, onResize);
				}
				return 0;
			case WM_MOUSEMOVE:		
				if (hwndPrevious != hWnd) {
					if (hwndPrevious)
						PostMessage(hwndPrevious, WM_MOUSELEAVE,0,0);
					hwndPrevious = hWnd;
				}
				lua_paramevent(w, onHover, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			case WM_NOTIFY:
				if (w->status != ((LPNMHDR)lParam)->hwndFrom)
					return SendMessageW(((LPNMHDR)lParam)->hwndFrom, WM_NOTIFY, wParam, lParam);	
				break;
			case WM_CLOSE: {
				lua_closeevent(w, onClose);
				return 0;
			}
			case WM_RBUTTONDOWN:
				lua_callevent(w, onContext);
				break;
			case WM_LBUTTONDOWN:
				lua_paramevent(w, onClick, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        		return 0;
			case WM_DESTROY:		
				PostQuitMessage(0);
				return 0;
			case WM_SIZE: {
				RECT sbRect;
				UINT sbheight;
				if (w->status) {
					GetWindowRect(w->status, &sbRect);
					sbheight = sbRect.bottom - sbRect.top;
					MoveWindow(w->status, 0, HIWORD(lParam)-sbheight, LOWORD(lParam), sbheight, TRUE);
				}
				lua_callevent(w, onResize);
				return 0;
             }
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
			case WM_CTLCOLORSTATIC: {
				if (IsWindowEnabled((HWND)lParam)) {
					Widget *c = (Widget*)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);	
					if (c) {			
						SetBkMode((HDC)wParam, TRANSPARENT);	
						SetTextColor((HDC)wParam, c->color);
						return (LRESULT)(c->brush ?: w->brush);
					}
				}
			}
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

static const char *styles[] = {  "dialog", "fixed", "float", "raw", NULL };
static int style_values[] = { WS_OVERLAPPEDWINDOW, WS_DLGFRAME | WS_SYSMENU, WS_EX_PALETTEWINDOW, WS_POPUP};

extern int size(Widget *w, lua_State *L, int offset_from, int offset_to, BOOL set, LONG value, BOOL iswidth);

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
	w->handle = CreateWindowExW(0, L"Window", title, style_values[style] | WS_EX_CONTROLPARENT | DS_CONTROL, CW_USEDEFAULT, CW_USEDEFAULT, r.right, r.bottom, HWND_DESKTOP, NULL, hInstance, NULL);
	if (style == 1)
		SetWindowLong(w->handle, GWL_STYLE, GetWindowLongPtr(w->handle, GWL_STYLE) & ~WS_MAXIMIZEBOX);
  	AdjustWindowRectEx(&r, GetWindowLongPtr(w->handle, GWL_STYLE), FALSE, GetWindowLongPtr(w->handle, GWL_EXSTYLE));
	SetWindowPos(w->handle, 0, 0, 0, r.right-r.left, r.bottom-r.top, SWP_HIDEWINDOW | SWP_NOMOVE);
	free(title);
	SetWindowLongPtr(w->handle, GWLP_USERDATA, (ULONG_PTR)w);
	w->brush = GetSysColorBrush(COLOR_BTNFACE);
	w->wtype = UIWindow;
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
			str[i] = lua_tolwstring(L, i+2, &len);
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
	TrackPopupMenu(h ?: w->handle, TPM_RIGHTBUTTON, p.x, p.y, 0, hwin,  NULL);
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

luaL_Reg Window_metafields[] = {
	{"__gc", Window___gc},
	{NULL, NULL}
};

luaL_Reg Window_methods[] = {
	{"onClose",			Window_onClose},
	{"showmodal",		Window_showmodal},
	{"minimize",		Window_minimize},
	{"maximize",		Window_maximize},
	{"status",			Window_status},
	{"loadtrayicon",	Window_loadtrayicon},
	{"get_traytooltip", Window_gettraytooltip},
	{"set_traytooltip", Window_settraytooltip},
	{"loadicon",		Widget_loadicon},
	{"shortcut",		Window_shortcut},
	{"popup",			Window_popup},
	{"set_title",		Widget_settext},
	{"get_title",		Widget_gettext},
	{"get_menu",		Window_getmenu},
	{"set_menu",		Window_setmenu},
	{"center",			Widget_center},
	{"get_bgcolor",		Widget_getbgcolor},
	{"set_bgcolor",		Widget_setbgcolor},
	{NULL, NULL}
};