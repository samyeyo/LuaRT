/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Widget.c | LuaRT Widget object implementation
*/

#include <luart.h>
#include "Widget.h"
#include <Window.h>
#include <File.h>
#include <Buffer.h>
#include <Date.h>

#include <math.h>
#include <richedit.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <windowsx.h>

//luart_type TWidget;

const char *luart_wtypes[] = {
	"Window", "Button", "Label", "Entry", "Edit", "Status", "Checkbox", "Radiobutton", "Groupbox", "Calendar", "List", "Combobox", "Tree", "Tab", "Item", "Menu", "MenuItem", "Picture"
};

const char *events[] = {
	"onHide", "onShow", "onMove", "onResize", "onHover", "onLeave", "onClose", "onClick", "onDoubleClick", "onContext", "onCreate", "onCaret", "onChange", "onSelect", "onTrayClick", "onTrayDoubleClick", "onTrayContext", "onTrayHover", "onClick" };

const char *cursors[] = {
	"arrow", "working", "cross", "hand", "help", "ibeam", "forbidden", "cardinal", "horizontal", "vertical", "leftdiagonal", "rightdiagonal", "up", "wait", "none"
};

int getStyle(Widget *w, const int *values, const char *names[]) {
	LONG style = GetWindowLongPtr(w->handle, GWL_STYLE);
	int i;	
	for(i = 0; names[i]; i++)
		if (style & values[i])
			return i;
	return 0;
}

static const char *align[] = {"left", "right", "center", NULL};
static const int align_values[] = {SS_LEFT, SS_RIGHT, SS_CENTER};

LUA_API void widget_noinherit(lua_State *L, int *type, char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt) {
	lua_registerobject(L, type, typename, constructor, methods, mt);
}

LUA_API void widget_type_new(lua_State *L, int *type, const char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_autosize, BOOL has_textalign, BOOL has_tooltip) {
	lua_registerobject(L, type, typename, constructor, Widget_methods, mt ?: Widget_metafields);
	lua_getfield(L, LUA_REGISTRYINDEX, typename);
	if (methods)
		luaL_setrawfuncs(L, methods);	
	if (has_text)
		luaL_setrawfuncs(L, WidgetText_methods);	
	if (has_cursor)
		luaL_setrawfuncs(L, WidgetCursor_methods);	
	if (has_font)
		luaL_setrawfuncs(L, WidgetFont_methods);
	if (has_icon) {
		lua_pushstring(L, "loadicon");
		lua_pushcfunction(L, Widget_loadicon);
		lua_rawset(L, -3);
	}
	if (has_autosize) {
		lua_pushstring(L, "autosize");
		lua_pushcfunction(L, Widget_autosize);
		lua_rawset(L, -3);
	}
	if (has_textalign)
		luaL_setrawfuncs(L, WidgetTextAlign_methods);
	if (has_tooltip)
		luaL_setrawfuncs(L, WidgetTooltip_methods);
	lua_pop(L, 1);
}

static void page_resize(Widget *w, BOOL isfocused) {
	TCITEMW *item = get_item(w, TabCtrl_GetCurSel(w->handle));
	RECT r;
	GetClientRect(w->handle, &r);
	if (isfocused) {
		BringWindowToTop((HWND)item->lParam);
		SetFocus((HWND)item->lParam);
	} else SetWindowPos((HWND)item->lParam, NULL, 0, 0, r.right-r.left-6, r.bottom-r.top-12, SWP_NOMOVE | SWP_NOZORDER);
	free(item->pszText);
	free(item);	
}

static HANDLE HitTest(TCHITTESTINFO *hti, HWND h, DWORD msg, UINT flags, LPARAM lParam) {
	hti->pt.x =  GET_X_LPARAM(lParam);
	hti->pt.y =  GET_Y_LPARAM(lParam);
	ScreenToClient(h, &hti->pt);
	hti->flags = flags;
	return (HANDLE)SendMessage(h, msg, 0, (LPARAM)hti);
}


int ProcessUIMessage(Widget *w, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT uIdSubclass) {
	LPNMHDR lpNmHdr;

	switch(uMsg) {
		case WM_COMMAND: {
			WPARAM cmd = HIWORD(wParam);
			if (w->wtype == UIGroup)
				w = (Widget*)GetWindowLongPtr(GetDlgItem(w->handle, LOWORD(wParam)), GWLP_USERDATA);
			if ((w->wtype == UIButton) || (w->wtype == UILabel) || (w->wtype >= UICheck && w->wtype <= UIGroup) || (w->wtype == UIPicture)) {
				switch (cmd) {
					case BN_CLICKED: 	lua_callevent(w, onClick); break;										
					case BN_DOUBLECLICKED: 
					case STN_DBLCLK:	lua_callevent(w, onDoubleClick);
				}
				return 0;
			} 
			else if (((w->wtype == UIEntry) || (w->wtype == UIEdit)) && (cmd == EN_CHANGE || (w->wtype == UIEntry && cmd == EN_SELCHANGE))) {
				lua_callevent(w, onChange);
				return 0;
			}
			else if (w->wtype == UICombo) {
				if (cmd == CBN_SELCHANGE) {
					int idx = ComboBox_GetCurSel(w->handle);
					if (idx != CB_ERR) {
						wchar_t *text = calloc(SendMessageW(w->status, CB_GETLBTEXTLEN, idx, 0)+1, sizeof(wchar_t));
						SendMessageW((HWND)w->status, CB_GETLBTEXT, idx, (LPARAM)text);
						SendMessageW((HWND)w->handle, WM_SETTEXT, 0, (LPARAM)text);
						InvalidateRect(w->handle, NULL, TRUE);
						SetFocus(GetParent(w->handle));
						SendMessageW(w->handle, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET,UISF_HIDEFOCUS), 0);
						free(text);
						lua_indexevent(w, onSelect, idx);
						return FALSE;
					}
				}
				return TRUE;	
			}						
		} break;
		case WM_LBUTTONDBLCLK:
			if (w->wtype == UIList) {
				int index;
				LVHITTESTINFO hti;
				hti.pt.x =  GET_X_LPARAM(lParam);
				hti.pt.y =  GET_Y_LPARAM(lParam);
				hti.flags = LVHT_ONITEM | LVHT_ONITEMICON;
				index = SendMessage(w->handle, LVM_HITTEST, 0, (LPARAM)&hti);
				lua_indexevent(w, onDoubleClick, index+1);
			}
			break;	
		case WM_NOTIFY:	
			lpNmHdr = (LPNMHDR) lParam;
			if ((w->wtype == UIEdit) && (lpNmHdr->code == EN_SELCHANGE)) {
				SELCHANGE *sc = (SELCHANGE *)lParam;
				if (sc->seltyp == SEL_EMPTY)
					lua_callevent(w, onCaret); 
				else
					lua_paramevent(w, onSelect, sc->chrg.cpMin+1, sc->chrg.cpMax+1);
				return TRUE;
			}
			else if (w->wtype == UIDate) {
				if (lpNmHdr->code == MCN_SELECT) {
					w = (Widget*)GetWindowLongPtr(lpNmHdr->hwndFrom, GWLP_USERDATA);
					SendMessage(w->handle, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
					lua_callevent(w, onSelect);
				}
				return TRUE;
			}
			else if (w->wtype == UITab) {
				if (lpNmHdr->code == TCN_SELCHANGE) {
					page_resize(w, TRUE);
					lua_indexevent(w, onSelect, TabCtrl_GetCurSel(w->handle));
					return TRUE;
				} else if (lpNmHdr->code == NM_RCLICK) {
					TCHITTESTINFO hti;
					lua_indexevent(w, onContext, HitTest(&hti, w->handle, TCM_HITTEST, TCHT_ONITEM | TCHT_ONITEMICON, GetMessagePos())+1);
				}
			}
			else if (w->wtype == UIList) {
				if ((lpNmHdr->code == NM_CLICK) || (lpNmHdr->code == LVN_ITEMCHANGED && (((LPNMLISTVIEW)lParam)->uNewState & LVNI_SELECTED))) {
					int i = ListView_GetNextItem(w->handle, -1, LVNI_SELECTED | LVNI_FOCUSED);
					if (i != -1) {
						SendMessage(w->handle, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
						lua_indexevent(w, onSelect, i);
						return TRUE;
					}
				} 
				else if ((lpNmHdr->code == LVN_ITEMCHANGED && (((LPNMLISTVIEW)lParam)->uChanged & LVIF_TEXT)) || (lpNmHdr->code == LVN_INSERTITEM) || (lpNmHdr->code == LVN_DELETEITEM)) {
					lua_callevent(w, onChange);
					return TRUE;
				} 
				else if (lpNmHdr->code == NM_RCLICK) {
					lua_indexevent(w, onContext, ((NMITEMACTIVATE*)lParam)->iItem+1);
					return TRUE;
				}
			}
			else if ((w->wtype == UICombo) && (lpNmHdr->code == CBEN_ENDEDITW)) {
				lua_callevent(w, onChange);
				return FALSE;
			}
			else if (w->wtype == UITree) {
				TVHITTESTINFO hti;
				switch(lpNmHdr->code) { 
					case TVN_SELCHANGEDW:	lua_paramevent(w, onSelect, 0, ((LPNMTREEVIEW)lParam)->itemNew.hItem); return TRUE;
					case NM_RCLICK:			lua_indexevent(w, onContext, HitTest((TCHITTESTINFO *)&hti, w->handle, TVM_HITTEST, TVHT_ONITEM, GetMessagePos())); return TRUE;
					case NM_DBLCLK:			lua_indexevent(w, onDoubleClick, HitTest((TCHITTESTINFO *)&hti, w->handle, TVM_HITTEST, TVHT_ONITEM, GetMessagePos())); return TRUE;
					case TVN_ENDLABELEDITW:	lua_paramevent(w, onChange, 0, (LPARAM)((LPNMTVDISPINFOW)lParam)->item.hItem); return TRUE;
					case TVN_DELETEITEMW:	{
												TVITEMW *item = __get_item(w, 0, ((LPNMTREEVIEW)lParam)->itemOld.hItem);
												lua_paramevent(w, onChange, 1, (LPARAM)(item->pszText)); break;
												free(item);
											}	
				}
			}
			return 0;
		case WM_CONTEXTMENU:
			if (((w->wtype < UIList) || (w->wtype > UITab)) || (wParam == 0))
				lua_indexevent(w, onContext, 0);
			return TRUE;
		case WM_CHAR:
			if (wParam == VK_RETURN && (w->wtype == UIEntry)) {
				lua_callevent(w, onSelect);
				return VK_RETURN;
			} 
			break;
		case WM_KEYDOWN:
			if (w->wtype == UIEntry)
				SendMessage(w->handle, WM_COMMAND, EN_CHANGE, 0);
			break; 
		case WM_MOUSEMOVE:		
			CallWindowProc(WindowProc, w->handle, uMsg, wParam, lParam);
			return TRUE;
		case WM_SETCURSOR:	
			if (w->wtype != UIEdit || ((w->wtype == UIEdit) && ((BOOL)w->cursor == TRUE))) {
				SetCursor(w->hcursor);
				return TRUE;
			} break;
		case WM_WINDOWPOSCHANGED:	
			if (((WINDOWPOS*)lParam)->flags & SWP_HIDEWINDOW)
				lua_callevent(w, onHide);
			else if (((WINDOWPOS*)lParam)->flags & SWP_SHOWWINDOW)
				lua_callevent(w, onShow);
			break;
		case WM_NCHITTEST:
			if (w->wtype == UIEdit) {
				if (w->cursor)
					return HTBORDER;
				return DefWindowProc(w->handle, uMsg, wParam, lParam);
			};
			break;
		case WM_MOUSELEAVE:
			lua_callevent(w, onLeave);
			break;
		case WM_SETFOCUS:
			if ((w->wtype == UIEdit) && ((BOOL)w->cursor == TRUE)) {
				HideCaret(w->handle);
				return TRUE;
			}
			if (!(w->wtype == UIList || w->wtype == UIEntry || w->wtype == UIEdit))
				return 0;
			break;
		case WM_NCDESTROY:
			// if (uIdSubclass)
			RemoveWindowSubclass(w->handle, WidgetProc, uIdSubclass);
			break;
		case WM_SIZE:
			if (w->wtype == UITab) 
				page_resize(w, FALSE);
			break;
		case WM_PAINT:
			if (w->wtype == UITab) { //----- Tab control transparency hack
				RECT r, rr;
				size_t count = TabCtrl_GetItemCount(w->handle);
				HBRUSH brush, pbrush = ((Widget*)GetWindowLongPtr(GetParent(w->handle), GWLP_USERDATA))->brush ?: w->brush;
				HDC hdc = GetDC(w->handle);
				DefSubclassProc(w->handle, uMsg, wParam, lParam);
				GetClientRect(w->handle, &r);
				COLORREF cr;
				LOGBRUSH lbr;
				GetObject(pbrush, sizeof(lbr), &lbr);
				SelectObject(hdc, CreateSolidBrush(lbr.lbColor));
				int sel = TabCtrl_GetCurSel(w->handle);
				for (size_t i = 0; i < count; i++) { 
					if (i != sel) {
						TabCtrl_GetItemRect(w->handle, i, &rr);	
						cr = GetPixel(hdc, rr.left+r.left+2, rr.top+r.top-2);				
						ExtFloodFill(hdc, rr.left+r.left+10, rr.top+r.top-2, cr, FLOODFILLSURFACE);
					}
				}
				cr = GetPixel(hdc, r.right-2, 3);
				ExtFloodFill(hdc, r.right-2, 3, cr, FLOODFILLSURFACE);
				cr = GetPixel(hdc, r.right-2, r.bottom-1);
				ExtFloodFill(hdc, r.right-2, r.bottom-1, cr, FLOODFILLSURFACE);
				SelectObject(hdc, GetStockObject(WHITE_BRUSH));
				ReleaseDC(w->handle, hdc);
				return 0;
			} break;
		case WM_ERASEBKGND:
			if (w->wtype == UIGroup) {
				HDC hdc = (HDC)(wParam); 
				RECT rc; GetClientRect(w->handle, &rc); 
				FillRect(hdc, &rc, w->brush); 
				return TRUE;
			}
	}			
	return -1;
}

LRESULT CALLBACK WidgetProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Widget *w = (Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	int result;

	if (w && (w->wtype))
		if ((result = ProcessUIMessage(w, uMsg, wParam, lParam, uIdSubclass)) >= 0)
			return result;	
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

int GetText(lua_State *L, HANDLE h) {
	int len = GetWindowTextLengthW(h);
	wchar_t *buff = malloc((len + 1)*sizeof(wchar_t));
	GetWindowTextW(h, buff, len + 1);
	lua_pushlstring(L, (const char*)buff, len*sizeof(wchar_t));	
	free(buff);
	return len;
}

static int width[] = {0, 20, 0, 60, 250, 16, 24, 24, 180, 226, 80, 100, 120, 300};
static int height[] = {0, 24, 0, 20, 200, 9, 9, 9, 200, 160, 100, 150, 160, 200};
static int margins[] = {0, 0, 2, 7, 100, 9, 9, 9, 10, 0, 150, 20, 100, 0, 160};

static void WidgetAutosize(Widget *w) {	
    HDC dc = GetWindowDC(w->handle);
    int len = GetWindowTextLengthW(w->handle);
    wchar_t *str = malloc((len + 1)*sizeof(wchar_t));
    RECT r = {0}, orig = {0};
 
    GetWindowRect(w->handle, &orig);
    orig.right -= orig.left;
    orig.bottom -= orig.top;
    GetWindowTextW(w->handle, str, len + 1);
    SelectObject(dc, w->font);
    DrawTextW(dc, str, len, &r, DT_CALCRECT); 
    if (w->wtype == UIButton) {
        if ((GetWindowLong(w->handle, GWL_STYLE) & ~BS_ICON) && len) {
            RECT rr = {0};
            rr.left = 6;
            Button_SetTextMargin(w->handle, &rr);
            if (w->icon)
                r.right += GetSystemMetrics(SM_CXICON)/2;
        }
        r.right += 12;
        r.bottom += 12;
    }
    SetWindowPos(w->handle, 0, 0, 0, max(r.right + margins[w->wtype-UIWindow]*2, orig.right), max(r.bottom + margins[w->wtype-UIWindow], orig.bottom), SWP_NOMOVE | SWP_NOZORDER);
    free(str);
}

Widget *check_widget(lua_State *L, int idx, WidgetType t) {
	Widget *w = lua_self(L, idx, Widget);
	if (w->wtype != t) 
		luaL_typeerror(L, idx, luart_wtypes[t]);
	return w;	
}

LUA_METHOD(Widget, autosize) {
	WidgetAutosize(lua_self(L, 1, Widget));
	return 0;
}

LUA_CONSTRUCTOR(Widget) {
	Widget *w = lua_touserdata(L, 1);
	lua_newinstance(L, w, Widget);
	return 1;
}

static void init_items(lua_State *L, Widget *w, int idx) {
	extern int Listbox_setitems(lua_State *L);
	size_t count;
	if (w->wtype == UIList)
		add_column(w);
	if (lua_istable(L, idx)) {
		lua_pushcfunction(L, Listbox_setitems);
		lua_pushvalue(L, 1);
		lua_pushvalue(L, idx);
		lua_call(L, 2, 0); 
	} else luaL_typeerror(L, idx, "table");
	count = get_count(w);
	w->imglist = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32|ILC_MASK, count, 1);
	ImageList_SetImageCount(w->imglist, count);
}

void *Widget_init(lua_State *L, Widget **wp) {
	if (lua_isuserdata(L, 2)) {
        lua_pushnewinstancebyname(L, ((Widget*)lua_touserdata(L, 2))->wtype, 1);
        return lua_self(L, -1, Widget);
    }
    *wp = luaL_checkcinstance(L, 2, Widget);
    if (((*wp)->wtype != UIWindow) && ((*wp)->wtype != UIGroup) && (((*wp)->wtype != UIItem) || ((*wp)->item.itemtype != UITab)))
        luaL_typeerror(L, 2, "Groupbox, TabItem or Window");
    return (void*)((*wp)->wtype == UIItem ? (HWND)(*wp)->item.tabitem->lParam : (*wp)->handle);
}

Widget *Widget__constructor(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc) {
	Widget *w = calloc(1, sizeof(Widget));
	w->hcursor = wp->hcursor ?: LoadCursor(NULL, IDC_ARROW);
    w->wtype = type;
    lua_newinstance(L, w, Widget);
    lua_pushvalue(L, 1);
    w->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (h) {
		w->handle = h;
    	SetWindowLongPtr(h, GWLP_USERDATA, (ULONG_PTR)w);
    	SetFontFromWidget(w, wp);
    	SetWindowSubclass(h, proc ?: WidgetProc, 0, 0);
    	lua_callevent(w, onCreate);
	}
    return w;
}

Widget *Widget_create(lua_State *L, WidgetType type, DWORD exstyle, const wchar_t *classname, DWORD style, int caption, int autosize)
{
    Widget *wp;
    HWND hParent;
    static HINSTANCE hInstance;
    Widget *w;
    int idx = 3+caption;
    HANDLE h;
    wchar_t *text;
    HMENU id = 0;
            
    hParent = Widget_init(L, &wp);
    text = caption && ((type < UIList) || (type > UITab)) ? lua_towstring(L, idx-1) : NULL;
    if (!hInstance)
        hInstance = GetModuleHandle(NULL);
    if (wp->wtype == UIGroup)
        id = ++wp->status;
    h = CreateWindowExW(exstyle, classname, text, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | style, (int)luaL_optinteger(L, idx, 8), (int)luaL_optinteger(L, idx+1, 10), (int)luaL_optinteger(L, idx+2, width[type-UIWindow]), (int)luaL_optinteger(L, idx+3, height[type-UIWindow]), hParent, id, hInstance, NULL);
    free(text);
    w = calloc(1, sizeof(Widget));
    w->handle = h;
    SetWindowLongPtr(h, GWLP_USERDATA, (ULONG_PTR)w);
    SetFontFromWidget(w, wp);
    if (type != UIEdit)
        w->hcursor = wp->hcursor ?: LoadCursor(NULL, IDC_ARROW);
    w->wtype = type;
    if ((w->autosize = autosize) && (lua_gettop(L) < 6))
        WidgetAutosize(w);
    lua_newinstance(L, w, Widget);
    lua_pushvalue(L, 1);
    w->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((type >= UIList) && (type <= UITab))
        init_items(L, w, idx-1);
    SetWindowSubclass(h, WidgetProc, 0, 0);
    lua_callevent(w, onCreate);
    return w;
}

LUA_METHOD(Widget, show) {
	Widget *w = lua_self(L, 1, Widget);

	if (w->wtype == UIWindow) {
		SendMessage(w->handle, WM_SETREDRAW, 1, 0);
		DrawMenuBar(w->handle);
		RedrawWindow(w->handle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		ShowWindow(w->handle, SW_HIDE);
	}
	ShowWindow(w->handle, SW_SHOWNORMAL);
	SetWindowPos(w->handle, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	SetForegroundWindow(w->handle);
	SetFocus(w->handle);
	return 0;
}

LUA_METHOD(Widget, hide) {
	Widget *w = lua_self(L, 1, Widget);
	HWND h = GetParent(w->handle);
	ShowWindow(w->handle, SW_HIDE);
	RECT r;
	GetClientRect(w->handle, &r);
	if (w->wtype != UIWindow) {
		MapWindowPoints(w->handle, h, (LPPOINT) &r, 2);
	    RedrawWindow(h, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	    InvalidateRect(h, &r, TRUE);
	} else if (w->tooltip) {
		EnableWindow(w->tooltip, TRUE);
		SetActiveWindow(w->tooltip);
		w->tooltip = NULL;
	}
	return 0;
}

static void do_align(HWND h, int type, RECT r, RECT rect) {
	switch (type) {
		case 0:	SetWindowPos(h, NULL, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
		case 1:	SetWindowPos(h, NULL, r.left, r.top, rect.right-rect.left, r.bottom-r.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
		case 2:	SetWindowPos(h, NULL, r.right-(rect.right-rect.left), r.top, rect.right-rect.left, r.bottom-r.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
		case 3:	SetWindowPos(h, NULL, r.left, r.bottom-(rect.bottom-rect.top), r.right-r.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
		case 4:	SetWindowPos(h, NULL, r.left, r.top, r.right-r.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOOWNERZORDER);
	}	
}

LUA_METHOD(Widget, align) {
	Widget *w = lua_self(L, 1, Widget);
	static const char *options[] = { "all", "left", "right", "bottom", "top", NULL };
	RECT r, rect;
	int type = luaL_checkoption(L, 2, NULL, options);

	GetClientRect(w->handle, &rect);
	if (w->wtype != UIWindow) {
		Widget *win = (Widget*)GetWindowLongPtr(GetParent(w->handle), GWLP_USERDATA);
		GetClientRect(win->handle, &r);
		if (win->status) {	
			RECT sr;
			GetWindowRect(win->status, &sr);
			r.bottom -= sr.bottom-sr.top;
		}
		if (win->wtype == UITab) {
			r.bottom -= AdjustTab_height(win->handle);
		} else if (win->wtype == UIGroup)
			r.top += 22;

	} else
		GetClientRect(GetDesktopWindow(), &r);
	do_align(w->handle, type, r, rect);
	if (w->wtype == UIPicture)
		InvalidateRect(GetParent(w->handle), NULL, TRUE);
	return 0;
}

LUA_PROPERTY_SET(Widget, enabled) {
	Widget *w = lua_self(L, 1, Widget);
	EnableWindow(w->handle, lua_toboolean(L, 2));
	return 0;
}

LUA_PROPERTY_GET(Widget, enabled) {
	lua_pushboolean(L, IsWindowEnabled(lua_self(L, 1, Widget)->handle));	
	return 1;
}

LUA_PROPERTY_SET(Widget, visible) {
	Widget *w = lua_self(L, 1, Widget);
	ShowWindow(w->handle, lua_toboolean(L, 2) ? SW_SHOWNORMAL : SW_HIDE);
	return 0;
}

LUA_PROPERTY_GET(Widget, visible) {
	lua_pushboolean(L, IsWindowVisible(lua_self(L, 1, Widget)->handle));	
	return 1;
}

LUA_PROPERTY_GET(Widget, text) {
	int len = GetText(L, lua_self(L, 1, Widget)->handle);
	lua_pushlwstring(L, (const wchar_t *)lua_tostring(L, -1), len);
	return 1;
}

LUA_PROPERTY_GET(Widget, hastext) {
	lua_pushboolean(L, !((GetWindowLong(lua_self(L, 1, Widget)->handle, GWL_STYLE) & BS_ICON) == BS_ICON));
	return 1;		
}

LUA_PROPERTY_SET(Widget, hastext) {
	Widget *w = lua_self(L, 1, Widget);
	DWORD style = GetWindowLong(w->handle, GWL_STYLE);

	style = lua_toboolean(L, 2) ? style & ~BS_ICON : style | BS_ICON;
	SetWindowLong(w->handle, GWL_STYLE, style);
	if ((style & BS_ICON) == BS_ICON) {
		SetWindowPos(w->handle, NULL, 0, 0, 24, 24, SWP_NOZORDER | SWP_NOMOVE);
		SetWindowTextA(w->handle, "");
	}
	return 1;		
}

LUA_PROPERTY_SET(Widget, text) {
	Widget *w = lua_self(L, 1, Widget);
	int len;
	wchar_t *text = lua_tolwstring(L, 2, &len);
	if (w->wtype != UIWindow) {
		RECT r;
		GetClientRect(w->handle, &r);
		MapWindowPoints(w->handle, GetParent(w->handle), (LPPOINT) &r, 2);
		InvalidateRect(GetParent(w->handle), &r, TRUE);
	}
	SetWindowTextW(w->handle, text);
	if (w->wtype == UIEntry)
		SendMessage(w->handle, EM_SETSEL, len, len);
	else if (w->autosize)
		WidgetAutosize(w);
	free(text);
	return 0;
}

int size(Widget *widget, lua_State *L, int offset_from, int offset_to, BOOL set, LONG value, BOOL iswidth) {
	Widget *w = widget ?: lua_self(L, 1, Widget);
	HWND h = w->handle;
	RECT r;
	LONG len;
	if (w->wtype == UIWindow)
		GetClientRect(h,&r);
	else
		GetWindowRect(h, &r);
	len = set ? floor(value) : (*(LONG*)(((char*)&r)+offset_from))-(*(LONG*)(((char*)&r)+offset_to));
	if (set) {
		if (w->wtype == UIWindow) {
			if (iswidth)
				r.right = floor(value);
			else
				r.bottom = floor(value);
			AdjustWindowRectEx(&r, GetWindowLongPtr(h, GWL_STYLE), FALSE, GetWindowLongPtr(h, GWL_EXSTYLE));
			len = (*(LONG*)(((char*)&r)+offset_from))-(*(LONG*)(((char*)&r)+offset_to));
		}
		SetWindowPos(h, NULL, 0, 0, iswidth ? len : r.right-r.left, iswidth ? r.bottom-r.top : len, SWP_NOMOVE | SWP_NOZORDER);
		RedrawWindow(GetParent(h), NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	else 
		lua_pushinteger(L, len);
	return 1;
}

LUA_PROPERTY_GET(Widget, width) {
	return size(NULL, L, offsetof(RECT, right), offsetof(RECT, left), FALSE, 0, TRUE);
}

LUA_PROPERTY_SET(Widget, width) {
	return size(NULL, L, offsetof(RECT, right), offsetof(RECT, left), TRUE, lua_tonumber(L, 2), TRUE);
}

LUA_PROPERTY_GET(Widget, height) {
	return size(NULL, L, offsetof(RECT, bottom), offsetof(RECT, top), FALSE, 0, FALSE);
}

LUA_PROPERTY_SET(Widget, height) {
	return size(NULL, L, offsetof(RECT, bottom), offsetof(RECT, top), TRUE, lua_tonumber(L, 2), FALSE);
}

int position(lua_State *L, HWND h, BOOL isWindow, int offset, BOOL set, lua_Number setvalue, int flag) {
	RECT Rect, Frame;
	LONG *value = (LONG*)(((char*)&Rect)+offset);
    GetWindowRect(h, &Rect);
    if (isWindow) {
    	LONG *frame_value = (LONG*)(((char*)&Frame)+offset);
		if (DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &Frame, sizeof(RECT)) == S_OK)
			*value = set ? floor(setvalue) - (*frame_value - *value) : *value + (*frame_value - *value);
	} 
	MapWindowPoints(HWND_DESKTOP, GetParent(h), (LPPOINT) &Rect, 1);	
	if (set) {
		if (!isWindow)
			*value = floor(setvalue);
	    SetWindowPos(h, NULL, Rect.left, Rect.top, 0, 0, flag | SWP_NOZORDER);
		if (!isWindow)
			RedrawWindow(h, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	else
		lua_pushinteger(L, (lua_Integer)*value);
	return !set;
}


LUA_PROPERTY_GET(Widget, x) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w->handle, w->wtype == UIWindow, offsetof(RECT, left), FALSE, 0, SWP_NOSIZE);
}

LUA_PROPERTY_SET(Widget, x) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w->handle, w->wtype == UIWindow, offsetof(RECT, left), TRUE, lua_tonumber(L, 2), SWP_NOSIZE);
}

LUA_PROPERTY_GET(Widget, y) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w->handle, w->wtype == UIWindow, offsetof(RECT, top), FALSE, 0, SWP_NOSIZE);
}

LUA_PROPERTY_SET(Widget, y) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w->handle, w->wtype == UIWindow, offsetof(RECT, top), TRUE, lua_tonumber(L, 2), SWP_NOSIZE);
}

LUA_PROPERTY_GET(Widget, bgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->brush) {
		LOGBRUSH lbr;
		GetObject(w->brush, sizeof(lbr), &lbr);
		lua_pushinteger(L, GetRValue(lbr.lbColor) + GetGValue(lbr.lbColor)*256 + GetBValue(lbr.lbColor)*65536);
	} else lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_SET(Widget, bgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if(w->brush)
        DeleteObject(w->brush);
	if (lua_isnil(L, 2))
		w->brush = w->wtype == UIWindow ? GetSysColorBrush(COLOR_BTNFACE) : NULL;
	else {
		DWORD32 color = luaL_checkinteger(L, 2);
		w->brush = CreateSolidBrush(RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		if (w->wtype == UIWindow)
			SetClassLongPtr(w->handle, GCLP_HBRBACKGROUND, (LONG)w->brush);
	}
	InvalidateRect(w->handle, NULL, TRUE);
	return 0;	
}

LUA_PROPERTY_GET(Widget, fgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->color)
		lua_pushinteger(L, GetRValue(w->color) << 16 | GetGValue(w->color) << 8 | GetBValue(w->color));
	else lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_SET(Widget, fgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if (lua_isnil(L, 2))
		w->color = 0;
	else {
		lua_Integer color = luaL_checkinteger(L, 2);
		w->color = RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
	}
	InvalidateRect(w->handle, NULL, TRUE);
	return 0;	
}

LUA_PROPERTY_SET(Widget, textalign) {
	Widget *w = lua_self(L, 1, Widget);
	int i = luaL_checkoption(L, 2, align[0], align);
	LONG style = GetWindowLongPtr(w->handle, GWL_STYLE );

	if (i == -1)
		luaL_error(L, "unknown alignment '%s'", lua_tostring(L, 2));
	style = ( style & ~( SS_LEFT | SS_CENTER | SS_RIGHT | SS_LEFTNOWORDWRAP ) ) | align_values[i];
	SetWindowLongPtr(w->handle, GWL_STYLE, style);
	InvalidateRect(w->handle, NULL, TRUE);
	return 0;
}

LUA_PROPERTY_GET(Widget, textalign) {
	lua_pushstring(L, align[getStyle(lua_self(L, 1, Widget), align_values, align)]);
	return 1;
}

LUA_PROPERTY_GET(Widget, tooltip) {
	HANDLE t = lua_self(L, 1, Widget)->tooltip;
	if (t)
		GetText(L, t);
	else
		lua_pushliteral(L, "");
	return 1;
}

LUA_PROPERTY_SET(Widget, tooltip) {
	Widget *w = lua_self(L, 1, Widget);
	HANDLE t = w->tooltip;
	RECT rect = {0};
	TOOLINFOW ti;
	wchar_t *str = lua_towstring(L, 2);
	

	if (!t) {
		t = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL,WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0, 0, 0, w->handle, NULL, NULL, NULL );
		SetWindowPos(t, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		w->tooltip = t;
	}
	GetClientRect(w->handle, &rect);
	ZeroMemory(&ti, sizeof (TOOLINFOW));
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = w->handle;
    ti.hinst = NULL;
    ti.uId = (UINT_PTR)w->handle;
    ti.lpszText = str;
    ti.rect = rect;
	SendMessageW(t, TTM_ADDTOOLW, 0, (LPARAM) (LPTOOLINFOW) &ti);	
	free(str);
	return 0;
}

LOGFONTW *Font(Widget *w) {
	LOGFONTW *l;
	int size = GetObjectW(w->font, 0, NULL);
	l = calloc(1, size);
	GetObjectW(w->font, size, l);
	return l;
}

static BOOL CALLBACK SetChildFont(HWND h, LPARAM lParam) {
	Widget *w;
	if ((w = (Widget*)GetWindowLongPtr(h, GWLP_USERDATA)))
		UpdateFont(w, (LOGFONTW *)lParam);
	return TRUE;
}

void UpdateFont(Widget *w, LOGFONTW *l) {
	HFONT f = CreateFontIndirectW(l);
	if (f) {
		if (w->wtype) {
			SendMessage(w->handle, WM_SETFONT, (WPARAM)f, MAKELPARAM(TRUE, 0));
			UpdateWindow(w->handle);
		}
		DeleteObject(w->font);
		w->font = f;	
	}
	if (w->autosize)
		WidgetAutosize(w);
	if (w->wtype == UIList)
		ListView_Arrange(w->handle, LVA_DEFAULT);	
	else if (w->wtype == UITab) {
        int i = get_count(w);
        TCITEM item;
        item.mask = TCIF_PARAM;
        while (--i > -1) {
            TabCtrl_GetItem(w->handle, i, &item);
            EnumChildWindows((HWND)item.lParam, SetChildFont, (LPARAM)l); 
        }
    }
    else if (w->wtype == UIGroup || (w->wtype == UIItem && w->item.itemtype == UITab) || (w->wtype == UIWindow))

		EnumChildWindows(w->handle, SetChildFont, (LPARAM)l); 
	RedrawWindow(w->handle, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

void SetFontFromWidget(Widget *w, Widget *wp) {
    LOGFONTW *l = Font(wp);
    w->font = CreateFontIndirectW(l);
    free(l);
    SendMessage(w->handle, WM_SETFONT, (WPARAM)w->font, FALSE);
    UpdateWindow(w->handle);    
}

LUA_PROPERTY_SET(Widget, font) {
	Widget *w = lua_self(L, 1, Widget);
	wchar_t *str = luaL_checkFilename(L, 2);
	LOGFONTW *l = Font(w);
	LOGFONTW lf = {0};
	BOOL result;

	if ((result = LoadFont(str, &lf))) {
		wcsncpy(l->lfFaceName, lf.lfFaceName, LF_FACESIZE);
		UpdateFont(w, l);
	}
	free(l);
	if (!result)
		luaL_error(L, "failed to load font");
	return 0;
}

LUA_PROPERTY_GET(Widget, font) {
	LOGFONTW *l = Font(lua_self(L, 1, Widget));
	lua_pushwstring(L, l->lfFaceName);
	free(l);
	return 1;
}

LUA_PROPERTY_SET(Widget, fontsize) {
	Widget *w = lua_self(L, 1, Widget);
	long height = luaL_checkinteger(L, 2);
	LOGFONTW *l = Font(w);
	l->lfHeight = -MulDiv(height, GetDeviceCaps(GetDC(0), LOGPIXELSY), 72);
	UpdateFont(w, l);
	free(l);
	return 0;        
}

LUA_PROPERTY_GET(Widget, fontsize) {
	LOGFONTW *l = Font(lua_self(L, 1, Widget));
	lua_pushinteger(L, fontsize_fromheight(l->lfHeight));
	free(l);
	return 1;
}

void fontstyle_fromtable(lua_State *L, int idx, LOGFONTW *l) {	
	int type, n = lua_gettop(L);
	
	luaL_checktype(L, idx, LUA_TTABLE);
	l->lfItalic = (lua_getfield(L, idx, "italic") != LUA_TNIL) && lua_toboolean(L, -1); 
	l->lfUnderline = (lua_getfield(L, idx, "underline") != LUA_TNIL) && lua_toboolean(L, -1); 
	l->lfStrikeOut = lua_getfield(L, idx, "strike") != LUA_TNIL && lua_toboolean(L, -1); 
	l->lfWeight = FW_THIN;
	if ((type = lua_getfield(L, idx, "bold")) != LUA_TNIL && lua_toboolean(L, -1))
		l->lfWeight = FW_BOLD;
	else if ((type = lua_getfield(L, idx, "thin")) != LUA_TNIL && lua_toboolean(L, -1))
		l->lfWeight = FW_THIN;
	else if ((type = lua_getfield(L, idx, "heavy")) != LUA_TNIL && lua_toboolean(L, -1))
		l->lfWeight = FW_HEAVY;
	lua_pop(L, lua_gettop(L)-n);
}

LUA_PROPERTY_SET(Widget, fontstyle) {
	Widget *w = lua_self(L, 1, Widget);
	LOGFONTW *l = Font(w);

	fontstyle_fromtable(L, 2, l);
	UpdateFont(w, l);
	free(l);
	return 0;
}

void fontstyle_createtable(lua_State *L, LOGFONTW *l) {
	lua_createtable(L, 0, 6);
	lua_pushboolean(L, l->lfItalic);
	lua_setfield(L, -2, "italic");
	lua_pushboolean(L, l->lfUnderline);
	lua_setfield(L, -2, "underline");
	lua_pushboolean(L, l->lfStrikeOut);
	lua_setfield(L, -2, "strike");
	lua_pushboolean(L, l->lfWeight == FW_BOLD);
	lua_setfield(L, -2, "bold");
	lua_pushboolean(L, l->lfWeight == FW_THIN);
	lua_setfield(L, -2, "thin");
	lua_pushboolean(L, l->lfWeight == FW_HEAVY);
	lua_setfield(L, -2, "heavy");	
}

LUA_PROPERTY_GET(Widget, fontstyle) {
	LOGFONTW *l = Font(lua_self(L, 1, Widget));
	fontstyle_createtable(L, l);
	free(l);
	return 1;
}

LUA_PROPERTY_SET(Widget, cursor) {
	Widget *w = lua_self(L, 1, Widget);
	w->cursor = luaL_checkoption(L, 2, "arrow", cursors);
	w->hcursor = LoadCursor(NULL, cursors_values[w->cursor]);
	return 0;
}

LUA_PROPERTY_GET(Widget, cursor) {
	Widget *w = lua_self(L, 1, Widget);
	lua_pushstring(L, cursors[w->cursor]);
	return 1;
}

LUA_PROPERTY_GET(Widget, parent) {
	Widget *wp, *w = lua_self(L, 1, Widget);
	HWND parent = GetParent(w->handle);

	if ((w->wtype == UIItem) && (w->item.itemtype == UITree)) {
		HTREEITEM hti = TreeView_GetParent(w->handle, w->item.treeitem->hItem);
		if (hti) 
			__push_item(L, w, 0, hti);
		else lua_rawgeti(L, LUA_REGISTRYINDEX, ((Widget*)GetWindowLongPtr(w->handle, GWLP_USERDATA))->ref);
	} else if (parent) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ((Widget*)GetWindowLongPtr(w->wtype != UIItem ? parent : w->handle, GWLP_USERDATA))->ref);
		wp = lua_self(L, -1, Widget);
		if ((w->wtype != UIItem) && (wp->wtype == UITab)) {
			size_t i;
			lua_pushnil(L);
			for (i = 0; i < get_count(wp); i++) {
				TCITEMW *item = get_item(wp, i);
				if ((HWND)item->lParam == parent) {
					free(item);
					push_item(L, wp, i);
					break;	
				}
				free(item);
			}
		}
	} else lua_pushnil(L);
	return 1;
}

//-------------------------------------------- Icon functions

HICON widget_loadicon(lua_State *L) {
	wchar_t *file;
	HICON icon = 0;
	luart_type t = 0;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	SHFILEINFOW sfi = {0};
	Buffer *buff;

	if (lua_gettop(L) > 1)
		switch(lua_type(L, 2)) {
			case LUA_TNIL:		break;
			case LUA_TTABLE:	buff = lua_tocinstance(L, 2, &t);
								if (t == TBuffer) {
									icon = CreateIconFromResourceEx(buff->bytes, buff->size, TRUE, 0x00030000, 16, 16, 0);	
									break;						
								} else if (t == TWidget) {
									icon = DuplicateIcon(hInstance, ((Widget*)buff)->icon);
									break;
								}
			default:			{	
									wchar_t ch;
									UINT result;
									file = luaL_checkFilename(L, 2);
									ch = file[wcslen(file)-1];
									result = ExtractIconExW(file, luaL_optinteger(L, 3, 1)-1, NULL, &icon, 1);
									if ( result == 0 || result == UINT_MAX ) {
										DWORD attrib = GetFileAttributesW(file);
										SHGetFileInfoW(file,  ch == L'\\' || ch == L'/' ? FILE_ATTRIBUTE_DIRECTORY : attrib == INVALID_FILE_ATTRIBUTES ? 0 : attrib, &sfi, sizeof(sfi), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);
										icon = sfi.hIcon;
									} 
									free(file);
								}
		}
	return icon;
}

LUA_METHOD(Widget, loadicon) {
	Widget *w = lua_self(L, 1, Widget);
	HICON icon = widget_loadicon(L);
	BOOL result = FALSE;
	if (w->icon)
		DestroyIcon(w->icon);
	w->icon = icon; 
	if (w->wtype == UIWindow) {
		result = SendMessage(w->handle, WM_SETICON, (WPARAM)ICON_SMALL,(LPARAM)icon);
		SendMessage(w->handle, WM_SETICON, (WPARAM)ICON_BIG,(LPARAM)icon);
	} 
	else if (w->wtype == UIButton) {
		result = SendMessage(w->handle, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon); 
		SetWindowPos(w->handle, NULL, 0, 0, 24, 24, SWP_NOMOVE | SWP_NOZORDER);
		WidgetAutosize(w);
	}	
	lua_pushboolean(L, result);
	return 1;
}

LUA_METHOD(Widget, center) {
	Widget *w = lua_self(L, 1, Widget);
    HWND hParent = GetParent(w->handle);
    RECT rp, rw;
    GetClientRect(hParent, &rp);
    GetWindowRect(w->handle, &rw);
	
	if (w->wtype != UIWindow) {
		rw.bottom -= rw.top;
		rw.right -= rw.left;
		rw.left = rw.top = 0;
		rw.left = (rp.right - rw.right)/2;
		rw.top = (rp.bottom - rw.bottom)/2;
		SetWindowPos(w->handle, NULL, rw.left, rw.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	} else SetWindowPos(w->handle, 0, GetSystemMetrics(SM_CXSCREEN)/2 - (rw.right-rw.left)/2, GetSystemMetrics(SM_CYSCREEN)/2 - (rw.bottom-rw.top)/2, rw.right, rw.bottom, SWP_NOZORDER | SWP_NOSIZE);
	return 0;	
}

//------------------------------------ Picture methods/properties

LUA_METHOD(Picture, load) {
	Widget *w = lua_self(L, 1, Widget);
	HANDLE h = (HANDLE)LoadImg(luaL_checkFilename(L, 2));
	BITMAP bm; 
	if (h) {
		DeleteObject(w->status);
		w->status = h;
		GetObject(h, sizeof(BITMAP), &bm);
		SetWindowPos(w->handle, NULL, 0, 0, bm.bmWidth, bm.bmHeight,SWP_NOZORDER | SWP_NOMOVE);
		SendMessage(w->handle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)w->status);
	}
	lua_pushboolean(L, (int)h);
	return 1;
}

LUA_METHOD(Picture, save) {
	Widget *w = lua_self(L, 1, Widget);
	lua_pushboolean(L, w->status ? SaveImg(luaL_checkFilename(L, 2), w->status) : FALSE);
	return 1;
}

LUA_METHOD(Picture, resize) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->status) {
		lua_Number n = luaL_checknumber(L, 2);
		BITMAP bm;
		GetObject(w->status, sizeof(BITMAP), &bm);
		SetWindowPos(w->handle, NULL, 0, 0,bm.bmWidth*n, bm.bmHeight*n, SWP_NOZORDER | SWP_NOMOVE | SWP_NOCOPYBITS);
		InvalidateRect(GetParent(w->handle), NULL, TRUE);
	}
	return 0;
}

luaL_Reg Picture_methods[] = {
	{"load",	Picture_load},
	{"save",	Picture_save},
	{"resize",	Picture_resize},
	{NULL, NULL}
};

//------------------------------------ Checkbox/RadioButton properties

LUA_PROPERTY_GET(Checkbox, checked) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, BM_GETCHECK, 0, 0));
	return 1;
}

LUA_PROPERTY_SET(Checkbox, checked) {
	SendMessage(lua_self(L, 1, Widget)->handle, BM_SETCHECK, lua_toboolean(L, 2), 0);
	return 0;
}

luaL_Reg Checkbox_methods[] = {
	{"set_checked",	Checkbox_setchecked},
	{"get_checked",	Checkbox_getchecked},
	{NULL, NULL}
};

//------------------------------------ Calendar properties

void pushDate(lua_State *L, HANDLE h) {
	LPSYSTEMTIME st = calloc(1, sizeof(SYSTEMTIME));
	SendMessage(h, MCM_GETCURSEL, 0, (LPARAM) st);
	lua_pushlightuserdata(L, NULL);
	lua_pushlightuserdata(L, NULL);
	lua_pushlightuserdata(L, st);
	lua_pushinstance(L, Datetime, 3);
	lua_insert(L, -4);
	lua_pop(L, 3);
}

LUA_PROPERTY_GET(Calendar, date) {
	pushDate(L, lua_self(L, 1, Widget)->handle);
	return 1;
}

LUA_PROPERTY_SET(Calendar, date) {
	SendMessage(lua_self(L, 1, Widget)->handle, MCM_SETCURSEL, 0, (LPARAM) luaL_checkcinstance(L, 2, Datetime)->st);
	return 0;
}

luaL_Reg Calendar_methods[] = {
	{"set_selected",	Calendar_setdate},
	{"get_selected",	Calendar_getdate},
	{NULL, NULL}
};

Widget *Widget_destructor(lua_State *L) {
	Widget *w = lua_self(L, 1, Widget);
	DestroyWindow(w->handle);
	if (w->ref)
		luaL_unref(L, LUA_REGISTRYINDEX, w->ref);
	if (w->font)
		DeleteObject(w->font);
	if (w->icon)
		DestroyIcon(w->icon);
	if (w->imglist)
		ImageList_Destroy(w->imglist);
	if (w->brush)
		DeleteObject(w->brush);
	if (w->wtype == UIPicture)
		DeleteObject(w->status);
	return w;
}

LUA_METHOD(Widget, __gc) {
	free(Widget_destructor(L));
	return 0;
}

luaL_Reg WidgetText_methods[] = {
	{"set_text",		Widget_settext},
	{"get_text",		Widget_gettext},
	{NULL, NULL}
};

luaL_Reg WidgetCursor_methods[] = {
	{"get_cursor",		Widget_getcursor},
	{"set_cursor",		Widget_setcursor},
	{NULL, NULL}
};

luaL_Reg WidgetFont_methods[] = {
	{"set_font",		Widget_setfont},
	{"get_font",		Widget_getfont},
	{"set_fontsize",	Widget_setfontsize},
	{"get_fontsize",	Widget_getfontsize},
	{"set_fontstyle",	Widget_setfontstyle},
	{"get_fontstyle",	Widget_getfontstyle},
	{NULL, NULL}
};

luaL_Reg WidgetTooltip_methods[] = {
	{"set_tooltip",		Widget_settooltip},
	{"get_tooltip",		Widget_gettooltip},
	{NULL, NULL}
};

luaL_Reg WidgetTextAlign_methods[] = {
	{"set_textalign",	Widget_settextalign},
	{"get_textalign",	Widget_gettextalign},
	{NULL, NULL}
};

luaL_Reg Widget_methods[] = {
	{"get_width",		Widget_getwidth},
	{"set_width",		Widget_setwidth},
	{"get_height",		Widget_getheight},
	{"set_height",		Widget_setheight},
	{"get_x",			Widget_getx},
	{"set_x",			Widget_setx},
	{"get_y",			Widget_gety},
	{"set_y",			Widget_sety},
	{"get_enabled",		Widget_getenabled},
	{"set_enabled",		Widget_setenabled},
	{"get_visible",		Widget_getvisible},
	{"set_visible",		Widget_setvisible},
	{"show",			Widget_show},
	{"hide",			Widget_hide},
	{"align",			Widget_align},
	{"get_parent",		Widget_getparent},
	{"center",			Widget_center},
	{NULL, NULL}
};

luaL_Reg Widget_metafields[] = {
	{"__gc", Widget___gc},
	{NULL, NULL}
};

luaL_Reg color_methods[] = {
	{"get_fgcolor",		Widget_getfgcolor},
	{"set_fgcolor",		Widget_setfgcolor},
	{"get_bgcolor",		Widget_getbgcolor},
	{"set_bgcolor",		Widget_setbgcolor},
	{NULL, NULL}
};