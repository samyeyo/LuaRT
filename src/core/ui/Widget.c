/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Widget.c | LuaRT Widget object implementation
*/


#include <luart.h>
#include <Widget.h>
#include "ui.h"
#include <Buffer.h>
#include <Date.h>
#include <File.h>

#include <math.h>
#include <richedit.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <windowsx.h>
#include "DarkMode.h"


const char *luart_wtypes[] = {
	"Window", "Button", "Label", "Entry", "Edit", "Status", "Checkbox", "Radiobutton", "Groupbox", "Calendar", "List", "Combobox", "Tree", "Tab", "Item", "Menu", "MenuItem", "Picture", "Progressbar", "Panel"
};

const char *events[] = {
	"onHide", "onShow", "onMove", "onResize", "onHover", "onLeave", "onClose", "onClick", "onDoubleClick", "onContext", "onCreate", "onCaret", "onChange", "onSelect", "onTrayClick", "onTrayDoubleClick", "onTrayContext", "onTrayHover", "onClick", "onKey", "onMouseUp", "onMouseDown", "onMaximize", "onMinimize", "onRestore", NULL };

const char *cursors[] = {
	"arrow", "working", "cross", "hand", "help", "ibeam", "forbidden", "cardinal", "horizontal", "vertical", "leftdiagonal", "rightdiagonal", "up", "wait", "none"
};

static const char *align[] = {"left", "right", "center", NULL};
static const int align_values[] = {SS_LEFT, SS_RIGHT, SS_CENTER | SS_CENTERIMAGE};
static const int align_valuesB[] = {BS_LEFT, BS_RIGHT, BS_CENTER};

void widget_noinherit(lua_State *L, int *type, char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt) {
	lua_registerobject(L, type, typename, constructor, methods, mt);
}

void widget_type_new(lua_State *L, int *type, const char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_autosize, BOOL has_textalign, BOOL has_tooltip, BOOL is_parent, BOOL do_pop) {
	lua_registerobject(L, type, typename, constructor, Widget_methods, mt ? mt : Widget_metafields);
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
	if ((methods == ItemWidget_methods) && ((strcmp(typename, "List") == 0 || strcmp(typename, "Tree") == 0)))
		luaL_setrawfuncs(L, sort_methods);
	if (is_parent)
		luaL_setrawfuncs(L, parent_methods);
	if (do_pop)
		lua_pop(L, 1);
}

void page_resize(Widget *w, BOOL isfocused) {
	int sel = TabCtrl_GetCurSel(w->handle);
	double dpi = GetDPIForSystem();
	int count = get_count(w);
	RECT r, rr;
	double hfactor;

	if (count) {
		TabCtrl_GetItemRect(w->handle, 0, &rr);
		hfactor = rr.bottom-rr.top+5;
	} else hfactor =  24.25*dpi; 
	GetClientRect(w->handle, &r);
	int width = r.right-r.left-(DarkMode ? 3 : 6)*dpi;
	int height = r.bottom-r.top-hfactor-dpi;
	if (w->user) {
		SetWindowPos(w->user, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
		BringWindowToTop(w->user);
	}
	else {
		for (int i = 0; i < count; i++) {
			TCITEMW *page = __get_item(w, i, NULL);
			SetWindowPos((HWND)page->lParam, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
			EnumChildWindows((HWND)page->lParam, ResizeChilds, (LPARAM)page->lParam);
			free(page->pszText);
			free(page);	
			if (isfocused && (i == sel)) {
				BringWindowToTop((HWND)page->lParam);
				InvalidateRect((HWND)page->lParam, NULL, TRUE);
				SetFocus((HWND)page->lParam);
			} 
		}
	}
}

static HWND HitTest(TCHITTESTINFO *hti, HWND h, DWORD msg, UINT flags, LPARAM lParam) {
	hti->pt.x =  GET_X_LPARAM(lParam);
	hti->pt.y =  GET_Y_LPARAM(lParam);
	ScreenToClient(h, &hti->pt);
	hti->flags = flags;
	return (HWND)SendMessage(h, msg, 0, (LPARAM)hti);
}

INT_PTR widget_setcolors(Widget *w, HDC dc, HWND h)
{
	Widget *c = (Widget*)GetWindowLongPtr(h, GWLP_USERDATA);	
	if (c) {
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, c->color);
		if (c->brush)
			return (INT_PTR)c->brush;
		if (w->brush)
			return (INT_PTR)w->brush;
	}
	while ((h = GetParent(h)))
		if ((w = (Widget*)GetWindowLongPtr(h, GWLP_USERDATA)) && w->wtype && w->brush)
			return (INT_PTR)w->brush;
	return (INT_PTR)GetStockBrush(NULL_BRUSH);
}	

int ProcessUIMessage(HWND hwnd, Widget *w, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT uIdSubclass, BOOL *result) {
	LPNMHDR lpNmHdr;

	switch(uMsg) {
		case WM_COMMAND: {
			WPARAM cmd = HIWORD(wParam);
			if (w->wtype == UIGroup)
				w = (Widget*)GetWindowLongPtr(GetDlgItem(w->handle, LOWORD(wParam)), GWLP_USERDATA);
			if (!w)
				break;
			if ((w->wtype == UIButton) || (w->wtype == UILabel) || (w->wtype >= UICheck && w->wtype <= UIGroup) || (w->wtype == UIPicture)) {
				switch (cmd) {
					case BN_CLICKED:	lua_callevent(w, onClick); SetFocus(NULL); break;
					case BN_DOUBLECLICKED: 
					case STN_DBLCLK:	lua_callevent(w, onDoubleClick); 
				}
				return 0;
			} 				
			else if (((w->wtype == UIEntry) || (w->wtype == UIEdit)) && (cmd == EN_CHANGE || (w->wtype == UIEntry && cmd == EN_SELCHANGE))) {
				lua_callevent(w, onChange);
				*result = FALSE;
				return TRUE;
			}
			else if (w->wtype == UICombo) {
				if (cmd == CBN_SELCHANGE) {
					int idx = ComboBox_GetCurSel(w->handle);
					if (idx != CB_ERR) {
						wchar_t *text = calloc(SendMessageW(w->status, CB_GETLBTEXTLEN, idx, 0)+1, sizeof(wchar_t));
						SendMessageW((HWND)w->status, CB_GETLBTEXT, idx, (LPARAM)text);
						SendMessageW((HWND)w->handle, WM_SETTEXT, 0, (LPARAM)text);
						lua_callevent(w, onChange);
						InvalidateRect(w->handle, NULL, TRUE);
						SetFocus(GetParent(w->handle));
						SendMessageW(w->handle, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET,UISF_HIDEFOCUS), 0);
						free(text);
						UpdateWindow(GetParent(w->handle));
						lua_indexevent(w, onSelect, idx);
						return FALSE;
					}
				}
				return TRUE;	
			}						
		} break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			lua_paramevent(w, onMouseUp, (uMsg-WM_LBUTTONUP)/3, lParam);
			break;

		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			lua_paramevent(w, onMouseDown, (uMsg-WM_LBUTTONDOWN)/3, lParam);
			break;

		case WM_LBUTTONDOWN:
			lua_paramevent(w, onMouseDown, 0, lParam);
			if ((w->wtype != UIButton) && (w->wtype != UILabel) && (w->wtype != UICheck) && (w->wtype != UIRadio) && (w->wtype != UIPicture))
				lua_paramevent(w, onClick, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));	
			if (w->wtype == UIList) {
				int index;
				LVHITTESTINFO hti;
				hti.pt.x =  GET_X_LPARAM(lParam);
				hti.pt.y =  GET_Y_LPARAM(lParam);
				hti.flags = LVHT_ONITEM | LVHT_ONITEMICON;
				index = SendMessage(w->handle, LVM_HITTEST, 0, (LPARAM)&hti);
				if (index > -1) {
					SetFocus(w->handle);
					ListView_EnsureVisible(w->handle, index, FALSE);
					ListView_SetItemState(w->handle, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
				return TRUE;
			}	
			break;
			
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
notify:		if ((w->wtype == UIEdit) && (lpNmHdr->code == EN_SELCHANGE)) {
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
					lua_indexevent(w, onContext, HitTest(&hti, (HWND)w->handle, TCM_HITTEST, TCHT_ONITEM | TCHT_ONITEMICON, GetMessagePos()));
				} 
			}
			else if (w->wtype == UIList) {
				SendMessage(w->handle, WM_CHANGEUISTATE, MAKELONG(UIS_SET, UISF_HIDEFOCUS), 0);
				if (lpNmHdr->code == LVN_ITEMCHANGED && (((LPNMLISTVIEW)lParam)->uNewState & LVNI_SELECTED)) {
					int i = SendMessage(w->handle, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED );
					if (i != -1) 
						lua_indexevent(w, onSelect, i);
					return TRUE;
				} 
				if ((lpNmHdr->code == LVN_ITEMCHANGED && (((LPNMLISTVIEW)lParam)->uChanged & LVIF_TEXT)) || (lpNmHdr->code == LVN_INSERTITEM) || (lpNmHdr->code == LVN_DELETEITEM)) {
					lua_callevent(w, onChange);
					return TRUE;
				} 
				else if (lpNmHdr->code == NM_RCLICK) {
					lua_indexevent(w, onContext, ((NMITEMACTIVATE*)lParam)->iItem+1);
					return TRUE;
				}
				else if (lpNmHdr->code == NM_CLICK) {
					SetFocus(w->handle);
					return TRUE;
				} 
				break;
			}
			else if ((w->wtype == UICombo))
				switch(lpNmHdr->code) {
					case CBEN_ENDEDITW: lua_callevent(w, onChange);
										return FALSE;
					case CBEN_BEGINEDIT: lua_callevent(w, onClick);
										return FALSE;
				}
			else if (w->wtype == UITree) {
				TVHITTESTINFO hti;
				switch(lpNmHdr->code) {		
					case TVN_SELCHANGEDW:	lua_paramevent(w, onSelect, 0, ((LPNMTREEVIEW)lParam)->itemNew.hItem);
											SendMessage(w->handle, WM_UPDATEUISTATE, (WPARAM) MAKELONG(UIS_SET, UISF_HIDEFOCUS), 0);
											return TRUE;
					case NM_RCLICK:			lua_indexevent(w, onContext, HitTest((TCHITTESTINFO *)&hti, w->handle, TVM_HITTEST, TVHT_ONITEM, GetMessagePos())); return TRUE;
					case NM_DBLCLK:			lua_indexevent(w, onDoubleClick, HitTest((TCHITTESTINFO *)&hti, w->handle, TVM_HITTEST, TVHT_ONITEM, GetMessagePos())); return TRUE;
					case TVN_ENDLABELEDITW:	lua_paramevent(w, onChange, 0, (LPARAM)((LPNMTVDISPINFOW)lParam)->item.hItem); return TRUE;
					case TVN_DELETEITEMW:	{
												TVITEMW *item = __get_item(w, 0, ((LPNMTREEVIEW)lParam)->itemOld.hItem);
												lua_paramevent(w, onChange, 1, (LPARAM)wcsdup(item->pszText));
												free(item->pszText);
												free(item);
											} break;
				}
			}
			else if (w->wtype == UIGroup) {
				if ((w = (Widget*)GetWindowLongPtr(GetDlgItem(w->handle, lpNmHdr->idFrom), GWLP_USERDATA)))
					goto notify;
			}
			return 0;
		case WM_CONTEXTMENU:
			if (((w->wtype < UIList) || (w->wtype >> UITab)) || (wParam == 0)) {
				lua_indexevent(w, onContext, 0);
				return TRUE;
			}
			break;
		case WM_CHAR:
			if (wParam == VK_RETURN && (w->wtype == UIEntry)) {
				Edit_SetSel(hwnd, 0, 0);
				SetFocus(GetParent(hwnd));
				lua_callevent(w, onSelect);
				return VK_RETURN;
			} 
			break;
		case WM_KEYDOWN:
			if (w->wtype == UIEntry)
				SendMessage(w->handle, WM_COMMAND, EN_CHANGE, 0);
			SendMessage(GetParent(w->handle), WM_KEYDOWN, wParam, lParam);
			if (w->wtype == UIGroup)
				return 0;				
			break;
		case WM_SYSKEYDOWN:
			SendMessage(GetParent(w->handle), WM_SYSKEYDOWN, wParam, lParam);
			break;
		case WM_MOUSEMOVE:		
			CallWindowProc(WindowProc, w->handle, uMsg, wParam, lParam);
			break;
		case WM_SETCURSOR:	
			if ((w->wtype != UIEdit) || ((BOOL)w->cursor == TRUE)) {
				SetCursor(w->hcursor);
				return TRUE;
			} 
			break;
		case WM_WINDOWPOSCHANGED:	
			if (((WINDOWPOS*)lParam)->flags & SWP_HIDEWINDOW)
				lua_callevent(w, onHide);
			else if (((WINDOWPOS*)lParam)->flags & SWP_SHOWWINDOW)
				lua_callevent(w, onShow);
			break;
		case WM_NCHITTEST:
			if (w->isactive)
				return HTTRANSPARENT;
			if (w->wtype == UIGroup) 
				return HTCLIENT;
			return DefSubclassProc(w->handle, uMsg, wParam, lParam);
		case WM_MOUSELEAVE:
			lua_callevent(w, onLeave);
			break;
		case WM_SETFOCUS:
			if ((w->wtype == UIEdit) && ((BOOL)w->cursor == TRUE)) {
				HideCaret(w->handle);
				return TRUE;
			}
			if (!(w->wtype == UIList || w->wtype == UIEntry || w->wtype == UIEdit || w->wtype == UITree)) {
				*result = FALSE;
				return TRUE;
			}
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(w->handle, WidgetProc, uIdSubclass);
			break;
		case WM_SIZE:
			if (w->wtype == UIGroup || w->wtype == UITab || w->wtype == UIPanel) {
				if (w->wtype == UITab) 
					page_resize(w, FALSE);
				EnumChildWindows(w->handle, ResizeChilds, (LPARAM)w->handle);
			} else if (w->wtype == UIList) {
				adjust_listvscroll(w, -1, 0);
				RedrawWindow(w->handle, NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);
			} else if ((w->wtype == UIPicture) && (w->status)) {
				IWICBitmapScaler *scaler;
				if (SUCCEEDED(ui_factory->lpVtbl->CreateBitmapScaler(ui_factory, &scaler))) {
					IWICBitmap *bmp; 
					if (SUCCEEDED(ui_factory->lpVtbl->CreateBitmapFromHBITMAP(ui_factory, (HBITMAP)w->status, NULL, WICBitmapUsePremultipliedAlpha, &bmp) && SUCCEEDED(scaler->lpVtbl->Initialize(scaler, (IWICBitmapSource *)bmp, LOWORD(lParam), HIWORD(lParam), WICBitmapInterpolationModeHighQualityCubic)))) {
						w->status = ConvertToBitmap(bmp);
						SendMessage(hwnd, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)w->status);
						bmp->lpVtbl->Release(bmp);
					}
					scaler->lpVtbl->Release(scaler);
				}
			}
			break;
		case WM_HSCROLL:
		case WM_VSCROLL:
			if (w->wtype == UIEdit && (((int)wParam & 0xFFFF) == 5))
		       return DefSubclassProc(hwnd, uMsg, (WPARAM)(((int)wParam & ~0xFFFF) | 4), lParam);
			break;			
		case WM_PRINTCLIENT:
		case WM_PAINT: 
			*result = TRUE;
			if (w->wtype == UITab)
				return TabSubclassProc(hwnd, uMsg, wParam, lParam, uIdSubclass, (DWORD_PTR)w);
			if (DarkMode && (w->wtype == UIGroup))
				return GroupBoxSubclassProc(hwnd, uMsg, wParam, lParam, uIdSubclass, (DWORD_PTR)w);
			break;
		case WM_CTLCOLOREDIT: 
			*result = TRUE;
			if (DarkMode) {
				SetBkMode((HDC)wParam, OPAQUE);
				SetTextColor((HDC)wParam, 0xFFFFFF);
				SetBkColor((HDC)wParam, 0x181818);
				return (LRESULT)CreateSolidBrush(0x181818);
			}
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC: 
			return widget_setcolors(w, (HDC)wParam, (HWND)lParam); break;
	}		
	*result = FALSE;
	return 0;
}

LRESULT CALLBACK WidgetProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	BOOL result = TRUE;
	Widget *w;

	if ((w = (Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA)) && (w->wtype)) {
		LRESULT r = ProcessUIMessage(hwnd, w, uMsg, wParam, lParam, uIdSubclass, &result);
		if (result)
			return r;
	}	
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

//--- Autosize data
//--- Button, Label, Entry, Edit, Checkbox, Radiobutton, Groupbox, Calendar, List, Combobox, Tree, Tab, Picture, Progressbar, Panel

static int width[] = {40, 0, 50, 250, 0, 22, 22, 160, 225, 120, 100, 120, 300, 300, 150, 200};
static int height[] = {12, 0, 9, 200, 0, 4, 4, 200, 160, 160, 150, 160, 200, 200, 16, 180};
static int defheight[] = {24, 18, 22, 200, 0, 24, 24, 200, 160, 160, 150, 160, 200, 200, 16, 200};

static void WidgetAutosize(Widget *w) {	
    HDC dc = GetWindowDC(w->handle);
    int len = GetWindowTextLengthW(w->handle);
    wchar_t *str = malloc((len + 1)*sizeof(wchar_t));
    RECT r = {0}, orig = {0};
    double dpi = GetDPIForSystem();
 
    GetWindowRect(w->handle, &orig);
    orig.right -= orig.left; 
    orig.bottom -= orig.top;
    GetWindowTextW(w->handle, str, len + 1);
    SelectObject(dc, w->font);
    DrawTextW(dc, str, len, &r, DT_CALCRECT); 
    if (w->wtype == UIButton) {
        if ((GetWindowLong(w->handle, GWL_STYLE) & ~BS_ICON) && len) {
            RECT rr = {0};
            rr.left = 6*dpi;
            Button_SetTextMargin(w->handle, &rr);        
		} else {
			SetWindowPos(w->handle, 0, 0, 0, 24*dpi, 24*dpi, SWP_NOMOVE | SWP_NOZORDER);
			goto done;
		}
	}
	int _width = floor(width[w->wtype-UIWindow-1]*dpi);
	int _height = floor(height[w->wtype-UIWindow-1]*dpi); 
	if (w->wtype < UIGroup)
    		SetWindowPos(w->handle, 0, 0, 0,r.right+_width, r.bottom+_height, SWP_NOMOVE | SWP_NOZORDER);
	else
    		SetWindowPos(w->handle, 0, 0, 0, _width, _height, SWP_NOMOVE | SWP_NOZORDER);
done:
	if (w->wtype == UIEntry)
	 	Edit_SetSel(w->handle, 0, 0);
	InvalidateRect(w->handle, NULL, TRUE);
	UpdateWindow(w->handle);
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

void *Widget_init(lua_State *L, Widget **wp, double *dpi, BOOL *isdark) {
	if (lua_isuserdata(L, 2)) {
        lua_pushnewinstancebyname(L, ((Widget*)lua_touserdata(L, 2))->wtype, 1);
        return lua_self(L, -1, Widget);
    }
    *wp = luaL_checkcinstance(L, 2, Widget);
	*dpi = GetDPIForSystem();
	*isdark = DarkMode;
    if (((*wp)->wtype != UIPanel) && ((*wp)->wtype != UIWindow) && ((*wp)->wtype != UIGroup) && (((*wp)->wtype != UIItem) || ((*wp)->item.itemtype != UITab)))
        luaL_typeerror(L, 2, "Groupbox, TabItem, Panel or Window");
    return (void*)((*wp)->wtype == UIItem ? (HWND)(*wp)->item.tabitem->lParam : (*wp)->handle);
}

Widget *Widget__constructor(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc) {
	Widget *w = calloc(1, sizeof(Widget));
	w->hcursor = wp->hcursor ? wp->hcursor : LoadCursor(NULL, IDC_ARROW);
    w->wtype = type;
	w->align = -1;
    lua_newinstance(L, w, Widget);
    lua_pushvalue(L, 1);
    w->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (h) {
		w->handle = h;
    	SetWindowLongPtr(h, GWLP_USERDATA, (ULONG_PTR)w);
    	SetFontFromWidget(w, wp);
    	SetWindowSubclass(h, proc ? proc : WidgetProc, 0, 0);
		GetWindowPlacement(h, &w->wp);
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
    HMENU id;
	int _width = width[type-UIWindow-1];
	int _height = type == UICombo ? height[UICombo-UIWindow-1] : defheight[type-UIWindow-1]; 
	BOOL isDark;
	double dpi;
            
    hParent = Widget_init(L, &wp, &dpi, &isDark);
    text = caption && (((type < UIList) || (type > UITab)) && (type != UIEdit) && (type < UIPicture)) ? lua_towstring(L, idx-1) : NULL;
    if (!hInstance)
        hInstance = GetModuleHandle(NULL);
    if (wp->wtype == UIGroup)
#ifdef _MSC_VER
        id = (HMENU)((++(UINT_PTR)wp->status));
#else
        id = (HMENU)(((UINT_PTR)++wp->status));
#endif
	int width = luaL_optinteger(L, idx+2, _width);
	int height =luaL_optinteger(L, idx+3, _height);
	if (type != UIPicture) {
		width = (int)floor(width*dpi); 
		height = (int)floor(height*dpi);
	}
    w = calloc(1, sizeof(Widget));
    h = CreateWindowExW(exstyle, classname, text, WS_CHILD | WS_CLIPSIBLINGS | style, (int)floor(luaL_optinteger(L, idx, 8)*dpi), (int)floor(luaL_optinteger(L, idx+1, 10)*dpi), width, height, hParent, id, hInstance, NULL);
    free(text);
    w->handle = h;
    w->wtype = type;
    SetWindowLongPtr(h, GWLP_USERDATA, (ULONG_PTR)w);
    SetFontFromWidget(w, wp);
	AdjustThemeProc(h, DarkMode);
	w->wp.length = sizeof(WINDOWPLACEMENT);
    if (type != UIEdit) {
    	w->hcursor = wp->hcursor ? wp->hcursor : LoadCursor(NULL, IDC_ARROW);
	}
	w->align = -1;	
	if ((w->autosize = autosize)) {
		if (lua_gettop(L) < idx+2)
        	WidgetAutosize(w);
		else
			w->autosize = FALSE;
	}
	GetWindowPlacement(h, &w->wp);
    lua_newinstance(L, w, Widget);
    lua_pushvalue(L, 1);
    w->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if ((type >= UIList) && (type <= UITab))
        init_items(L, w, idx-1);
    SetWindowSubclass(h, WidgetProc, 0, 0);
    lua_callevent(w, onCreate);
    return w;
}

extern BOOL CALLBACK ResizeChilds(HWND h, LPARAM lParam);

LUA_METHOD(Widget, show) {
	Widget *w = lua_self(L, 1, Widget);

	SetWindowPos(w->handle, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	if (w->wtype == UIWindow) {
		GetWindowPlacement(w->handle, &w->wp);
		SendMessage(w->handle, WM_SETREDRAW, 1, 0);
		DrawMenuBar(w->handle);
		RedrawWindow(w->handle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	}
	if (w->wtype == UIWindow)
		SetForegroundWindow(w->handle);
	SetFocus(w->handle);
	EnumChildWindows(w->handle, ResizeChilds, (LPARAM)w->handle);
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

LUA_METHOD(Widget, tofront) {
	SetWindowPos(lua_self(L, 1, Widget)->handle, lua_gettop(L) > 1 ? lua_self(L, 2, Widget)->handle : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return 0;
}

LUA_METHOD(Widget, toback) {
	Widget *w = lua_gettop(L) > 1 ? lua_self(L, 2, Widget) : NULL;
	SetWindowPos(lua_self(L, 1, Widget)->handle,  w ? w->handle : HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	if (w)
		InvalidateRect(w->handle, NULL, TRUE);
	return 0;
}

void do_align(Widget *w) {
	if (w->align >= 0) {
		RECT r, rect;
		int dtop = 0;

		GetClientRect(w->handle, &rect);
		if (w->wtype != UIWindow) {
			Widget *win = (Widget*)GetWindowLongPtr(GetParent(w->handle), GWLP_USERDATA);
			if (win) {
				MapWindowPoints(w->handle, win->handle, (LPPOINT) &rect, 4);
				GetClientRect(win->handle, &r);
				if (win->status) {	
					RECT sr;
					GetWindowRect(win->status, &sr);
					r.bottom -= sr.bottom-sr.top;
				}
				if (win->wtype == UITab) {
					int i = SendMessageW(win->handle, TCM_GETCURSEL, 0, 0);
					TCITEMW *item = __get_item(win, i, NULL);
					GetClientRect((HWND)item->lParam, &r);
					r.left +=2;
					free(item->pszText);
					free(item);
				} else if (win->wtype == UIGroup) {
					dtop = 18;
					r.bottom -= 4;
					r.left += 2;
					r.right -= 2;
				}
			} else GetClientRect(GetParent(w->handle), &r);
		} else
			GetClientRect(GetDesktopWindow(), &r);
		switch (w->align) {
			case 0: SetWindowPos(w->handle, NULL, r.left, r.top+dtop, r.right-r.left, r.bottom-r.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
			case 1:	SetWindowPos(w->handle, NULL, r.left, r.top+dtop, rect.right-rect.left, r.bottom-r.top-dtop, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
			case 2:	SetWindowPos(w->handle, NULL, r.right-(rect.right-rect.left), r.top+dtop, rect.right-rect.left, r.bottom-r.top-dtop, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
			case 3:	SetWindowPos(w->handle, NULL, r.left, r.bottom-(rect.bottom-rect.top), r.right-r.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
			case 4:	SetWindowPos(w->handle, NULL, r.left, r.top+dtop, r.right-r.left, rect.bottom-rect.top, SWP_NOZORDER | SWP_NOOWNERZORDER); break;
		}
	
		if ((w->wtype == UIWindow) || (w->wtype == UIGroup))
			EnumChildWindows(w->handle, ResizeChilds, (LPARAM)w->handle);	
		else if ((w->wtype == UITab))
			page_resize(w, TRUE);
		else if (w->wtype == UIPicture)
			InvalidateRect(GetParent(w->handle), NULL, TRUE);
		else
			InvalidateRect(w->handle, NULL, TRUE);
	}
}

static const char *align_options[] = { "all", "left", "right", "bottom", "top", NULL };

LUA_PROPERTY_SET(Widget, align) {
	Widget *w = lua_self(L, 1, Widget);	
	w->align = lua_isnil(L, 2) ? -1 : luaL_checkoption(L, 2, NULL, align_options);
	SetWindowPlacement(w->handle, &w->wp);
	do_align(w);
	return 0;
}

LUA_PROPERTY_GET(Widget, align) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->align == -1)
		lua_pushnil(L);
	else lua_pushstring(L, align_options[w->align]);
	return 1;
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
	UpdateWindow(GetParent(w->handle));
	return 0;
}

LUA_PROPERTY_GET(Widget, visible) {
	lua_pushboolean(L, IsWindowVisible(lua_self(L, 1, Widget)->handle));	
	return 1;
}

LUA_PROPERTY_GET(Widget, text) {
	Widget *w = lua_self(L, 1, Widget);
	int len = GetText(L, w->handle);
	if (!len && (w->wtype == UICombo)) {		
		COMBOBOXEXITEMW *item = __get_item(w, SendMessage(w->status, CB_GETCURSEL, 0, 0), NULL);
		SendMessageW((HWND)w->handle, WM_SETTEXT, 0, (LPARAM)item->pszText);
		lua_pushwstring(L, item->pszText);
		free(item->pszText);
		free(item);
	} else lua_pushlwstring(L, (const wchar_t *)lua_tostring(L, -1), len);
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
	UpdateWindow(GetParent(w->handle));
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
		SendMessage(w->handle, EM_SETSEL, 0, 0);
	else if (w->autosize)
		WidgetAutosize(w);
	UpdateWindow(w->handle);
	free(text);
	return 0;
}

int size(Widget *widget, lua_State *L, int offset_from, int offset_to, BOOL set, LONG value, BOOL iswidth) {
	Widget *w = widget ? widget : lua_self(L, 1, Widget);
	HWND h = w->handle;
	RECT r;
	LONG len;
	
	if(set)
		value = floor(value*GetDPIForSystem());
	if (w->wtype == UIWindow) {
		GetClientRect(h,&r);
		if (w->menu && IsWindowVisible(h)) {
			MENUBARINFO mbi;
			mbi.cbSize = sizeof(MENUBARINFO);
			GetMenuBarInfo(h, OBJID_MENU, 0, &mbi);
			r.bottom += mbi.rcBar.bottom - mbi.rcBar.top+1;				
		}
	} else
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
		} else if (w->autosize)
			w->autosize = FALSE;
		if ((w->wtype == UICombo) && (r.bottom-r.top < 100))
			r.top += 100;
		SetWindowPos(h, NULL, 0, 0, iswidth ? len : r.right-r.left, iswidth ? r.bottom-r.top : len, SWP_NOMOVE | SWP_NOZORDER);
		RedrawWindow(GetParent(h), NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
		GetWindowPlacement(h, &w->wp);
	}
	else {
		lua_pushinteger(L, floor(len/GetDPIForSystem()));
	}
	return 1;
}

LUA_PROPERTY_GET(Widget, width) {
	return size(NULL, L, offsetof(RECT, right), offsetof(RECT, left), FALSE, 0, TRUE);
}

LUA_PROPERTY_SET(Widget, width) {
	return size(NULL, L, offsetof(RECT, right), offsetof(RECT, left), TRUE, luaL_checknumber(L, 2), TRUE);
}

LUA_PROPERTY_GET(Widget, height) {
	return size(NULL, L, offsetof(RECT, bottom), offsetof(RECT, top), FALSE, 0, FALSE);
}

LUA_PROPERTY_SET(Widget, height) {
	return size(NULL, L, offsetof(RECT, bottom), offsetof(RECT, top), TRUE, luaL_checknumber(L, 2), FALSE);
}

int position(lua_State *L, Widget *w, BOOL isWindow, int offset, BOOL set, lua_Number setvalue, int flag) {
	RECT Rect, Frame;
	HWND h = w->handle;
	LONG *value = (LONG*)(((char*)&Rect)+offset);

	if(set)
		setvalue = floor(setvalue*GetDPIForSystem());
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
		GetWindowPlacement(h, &w->wp);
	}
	else
		lua_pushinteger(L, (lua_Integer)floor(*value/GetDPIForSystem()));
	return !set;
}


LUA_PROPERTY_GET(Widget, x) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w, w->wtype == UIWindow, offsetof(RECT, left), FALSE, 0, SWP_NOSIZE);
}

LUA_PROPERTY_SET(Widget, x) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w, w->wtype == UIWindow, offsetof(RECT, left), TRUE, luaL_checknumber(L, 2), SWP_NOSIZE);
}

LUA_PROPERTY_GET(Widget, y) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w, w->wtype == UIWindow, offsetof(RECT, top), FALSE, 0, SWP_NOSIZE);
}

LUA_PROPERTY_SET(Widget, y) {
	Widget *w = lua_self(L, 1, Widget);
	return position(L, w, w->wtype == UIWindow, offsetof(RECT, top), TRUE, luaL_checknumber(L, 2), SWP_NOSIZE);
}

LUA_PROPERTY_GET(Widget, bgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	COLORREF color;
	if (w->brush)
found:
	{
		LOGBRUSH lbr;
		GetObject(w->brush, sizeof(lbr), &lbr);
		color = lbr.lbColor;
		lua_pushinteger(L, GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color));
	} else {
		HWND h = w->handle;
		while ((h = GetParent(h)))
			if ((w = (Widget*)GetWindowLongPtr(h, GWLP_USERDATA)) && w->wtype && w->brush)
				goto found;
		lua_pushnil(L);
	}
	return 1;
}

LUA_PROPERTY_SET(Widget, bgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if(w->brush)
        DeleteObject(w->brush);
	if (lua_isnil(L, 2)) {
		if (w->brush)
			DeleteObject(w->brush);
		w->brush = w->wtype == UIWindow ? (DarkMode ? GetStockObject(BLACK_BRUSH) : GetSysColorBrush(COLOR_BTNFACE)) : NULL;
	} else {
		DWORD32 color = luaL_checkinteger(L, 2);
		w->brush = CreateSolidBrush(RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
	}
	InvalidateRect(w->handle, NULL, TRUE);
	RedrawWindow(w->handle, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
	return 0;	
}

LUA_PROPERTY_GET(Widget, fgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	COLORREF color = w->color ? w->color : DarkMode ? 0xFFFFFF : GetSysColor(COLOR_BTNTEXT);
	lua_pushinteger(L, GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color));
	return 1;
}

LUA_PROPERTY_SET(Widget, fgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	if (lua_isnil(L, 2))
		w->color = DarkMode ? 0xFFFFFF : GetSysColor(COLOR_BTNTEXT);
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
	LONG style = GetWindowLongPtr(w->handle, GWL_STYLE);

	if (i == -1)
		luaL_error(L, "unknown alignment '%s'", lua_tostring(L, 2));
	style = w->wtype == UIButton ? ( style & ~BS_CENTER ) | align_valuesB[i] : ( style & ~( SS_LEFT | SS_CENTER | SS_CENTERIMAGE | SS_RIGHT | SS_LEFTNOWORDWRAP ) ) | align_values[i];
	SetWindowLongPtr(w->handle, GWL_STYLE, style);
	InvalidateRect(w->handle, NULL, TRUE);
	UpdateWindow(w->handle);
	return 0;
}

LUA_PROPERTY_GET(Widget, textalign) {
	Widget *w = lua_self(L, 1, Widget);
	LONG style = GetWindowLongPtr(w->handle, GWL_STYLE);
	const int *values = w->wtype == UIButton ? align_valuesB : align_values;
	int i, result = 0;
	for(i = 0; align[i]; i++)
		if (style & values[i]) {
			if (((i == 0) && (style & values[1])) || ((i == 1) && (style & values[2])))
				result = 2;
			else result = i;
			break;
		}
	lua_pushstring(L, align[result]);
	return 1;
}

LUA_PROPERTY_GET(Widget, tooltip) {
	Widget *w = lua_self(L, 1, Widget);
	wchar_t text[512] = {0};
	TOOLINFOW ti = {0};
	ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uId = (UINT_PTR)w->handle;
	ti.lpszText = text;
	ti.hwnd = w->wtype == UICombo ? (HWND)SendMessage(w->handle, CBEM_GETEDITCONTROL, 0, 0) : w->handle;
	SendMessageW(w->tooltip, TTM_GETTEXTW, 512, (LPARAM)&ti);
	lua_pushwstring(L, text);
	return 1;
}

LUA_PROPERTY_SET(Widget, tooltip) {
	Widget *w = lua_self(L, 1, Widget);
	HANDLE t = w->tooltip;
	RECT rect = {0};
	TOOLINFOW ti;
	wchar_t *str = lua_towstring(L, 2);
	HWND h =  w->wtype == UICombo ? (HWND)SendMessage(w->handle, CBEM_GETEDITCONTROL, 0, 0) : w->handle;
	
	if (!t) {
		t = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL,WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0, 0, 0, h, NULL, NULL, NULL );
		SetWindowPos(t, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		w->tooltip = t;
	}
	GetClientRect(w->handle, &rect);
	ZeroMemory(&ti, sizeof (TOOLINFOW));
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
    ti.hwnd = w->handle;
    ti.hinst = NULL;
    ti.uId = (UINT_PTR)h;
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
    else if (w->wtype == UIGroup || (w->wtype == UIItem && w->item.itemtype == UITab) || (w->wtype == UIWindow) || (w->wtype == UIPanel))
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
	free(str);
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
	HDC hdc = GetDC(NULL);
	l->lfHeight = -MulDiv(height, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(NULL, hdc);
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
		if (w->autosize)
			WidgetAutosize(w);
	}
	lua_pushboolean(L, result);
	return 1;
}

LUA_METHOD(Widget, center) {
	Widget *w = lua_self(L, 1, Widget);
    RECT rw;
    GetWindowRect(w->handle, &rw);
	
	if (w->wtype != UIWindow) {
		RECT rp;
		HWND hParent = GetParent(w->handle);
		Widget *win = (Widget*)GetWindowLongPtr(hParent, GWLP_USERDATA);
		
		GetClientRect(hParent, &rp);
		rw.bottom -= rw.top;
		rw.right -= rw.left;
		rw.left = rw.top = 0;
		rw.top = (rp.bottom - rw.bottom)/2;
		rw.left = (rp.right - rw.right)/2;
		if (win && (win->wtype == UIWindow) && win->status) {	
			RECT sr;
			GetWindowRect(win->status, &sr);
			rw.top -= (sr.bottom-sr.top)/2;
		} 		
		SetWindowPos(w->handle, NULL, rw.left, rw.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	} else 
		SetWindowPos(w->handle, 0, GetSystemMetrics(SM_CXSCREEN)/2 - (rw.right-rw.left)/2, GetSystemMetrics(SM_CYSCREEN)/2 - (rw.bottom-rw.top)/2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
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
		SetWindowPos(w->handle, NULL, 0, 0, luaL_optinteger(L, 3, bm.bmWidth), luaL_optinteger(L, 4, bm.bmHeight),SWP_NOZORDER | SWP_NOMOVE);
		SendMessage(w->handle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)w->status);
	}
	lua_pushboolean(L, h != 0);
	do_align(w);
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
		RECT r;
		double n = luaL_checknumber(L, 2);
		GetClientRect(w->handle, &r);
		SetWindowPos(w->handle, NULL, 0, 0, (r.right-r.left)*n, (r.bottom-r.top)*n, SWP_NOZORDER | SWP_NOMOVE | SWP_NOCOPYBITS);
	}
	return 0;
}

luaL_Reg Picture_methods[] = {
	{"load",	Picture_load},
	{"save",	Picture_save},
	{"resize",	Picture_resize},
	{NULL, NULL}
};

//------------------------------------ Progressbar methods/properties
LUA_METHOD(Progressbar, advance) {
    SendMessage(lua_self(L, 1, Widget)->handle, PBM_DELTAPOS, luaL_checkinteger(L, 2), 0);
    return 0;
}

LUA_PROPERTY_GET(Progressbar, themed) {
	lua_pushboolean(L, lua_self(L, 1, Widget)->index);
	return 1;
}

LUA_PROPERTY_SET(Progressbar, themed) {
	Widget *w = lua_self(L, 1, Widget);
	w->index = lua_toboolean(L, 2);

	if (w->index)
		SetWindowTheme(w->handle, L"Explorer", NULL);
	else
		SetWindowTheme(w->handle, L"", L"");
	return 0;
}

LUA_PROPERTY_GET(Progressbar, fgcolor) {
	DWORD color = (DWORD)SendMessage(lua_self(L, 1, Widget)->handle, PBM_GETBARCOLOR, 0, 0);	
	if (color == CLR_DEFAULT)
		color =  DarkMode ? 0xE16941 : GetSysColor(COLOR_HIGHLIGHT);
	lua_pushinteger(L, GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color));
	return 1;
}

LUA_PROPERTY_SET(Progressbar, fgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	lua_Integer color;
	if (lua_isnil(L, 2)) 
		color = CLR_DEFAULT;
	else {
		color = luaL_checkinteger(L, 2);
		color = RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
	}
	SendMessage(w->handle,(WPARAM) PBM_SETBARCOLOR,0,(LPARAM)color);
	InvalidateRect(w->handle, NULL, TRUE);
	return 0;
}

LUA_PROPERTY_GET(Progressbar, bgcolor) {
	DWORD color = (DWORD)SendMessage(lua_self(L, 1, Widget)->handle, PBM_GETBKCOLOR, 0, 0);	
	if (color == CLR_DEFAULT)
		color =  DarkMode ? 0x484848 : GetSysColor(COLOR_3DFACE);
	lua_pushinteger(L, GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color));
	return 1;
}

LUA_PROPERTY_SET(Progressbar, bgcolor) {
	Widget *w = lua_self(L, 1, Widget);
	lua_Integer color;
	if (lua_isnil(L, 2)) 
		color = DarkMode ? 0x484848 : GetSysColor(COLOR_3DFACE);
	else {
		color = luaL_checkinteger(L, 2);
		color = RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
	}
	SendMessage(w->handle,(WPARAM) PBM_SETBKCOLOR,0, (LPARAM)color);
	InvalidateRect(w->handle, NULL, TRUE);
	return 0;
}

LUA_PROPERTY_GET(Progressbar, position) {
    lua_pushinteger(L, SendMessage(lua_self(L, 1, Widget)->handle, PBM_GETPOS, 0, 0));
    return 1;
}

LUA_PROPERTY_SET(Progressbar, position) {
    PBRANGE range;
    SendMessage(lua_self(L, 1, Widget)->handle, PBM_SETPOS, luaL_checkinteger(L, 2), (LPARAM)&range);
    return 0;
}

LUA_PROPERTY_GET(Progressbar, range) {
    PBRANGE range;
    SendMessage(lua_self(L, 1, Widget)->handle, PBM_GETRANGE, 0, (LPARAM)&range);
    lua_createtable(L, 2, 0);
    lua_pushinteger(L, range.iLow);
    lua_setfield(L, -2, "min");
    lua_pushinteger(L, range.iHigh);
    lua_setfield(L, -2, "max");
    return 1;
}

LUA_PROPERTY_SET(Progressbar, range) {
    lua_Integer low, high;
    luaL_checktype(L, 2, LUA_TTABLE);
    if (lua_rawgeti(L, 2, 1) == LUA_TNUMBER)
        low = lua_tointeger(L, -1);
    else
error:  luaL_error(L, "bad value in range table (integer expected, found %s", luaL_typename(L, -1));
    if (lua_rawgeti(L, 2, 2) == LUA_TNUMBER)
        high = lua_tointeger(L, -1);
    else goto error;
    SendMessage(lua_self(L, 1, Widget)->handle, PBM_SETRANGE32, low, high);
    return 0;
}

luaL_Reg Progressbar_methods[] = {
	{"get_position",	Progressbar_getposition},
	{"set_position",	Progressbar_setposition},
	{"get_bgcolor",		Progressbar_getbgcolor},
	{"set_bgcolor",		Progressbar_setbgcolor},
	{"get_fgcolor",		Progressbar_getfgcolor},
	{"set_fgcolor",		Progressbar_setfgcolor},
	{"advance",	    	Progressbar_advance},
	{"set_range",		Progressbar_setrange},
	{"get_range",		Progressbar_getrange},
	{"set_themed",		Progressbar_setthemed},
	{"get_themed",		Progressbar_getthemed},
	{NULL, NULL}
};

//------------------------------------ Checkbox/RadioButton properties

LUA_PROPERTY_GET(Checkbox, checked) {
	lua_pushboolean(L, SendMessage(lua_self(L, 1, Widget)->handle, BM_GETCHECK, 0, 0));
	return 1;
}

LUA_PROPERTY_SET(Checkbox, checked) {
	Widget *w = lua_self(L, 1, Widget);
	SendMessage(w->handle, BM_SETCHECK, lua_toboolean(L, 2), 0);
	UpdateWindow(w->handle);
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
	int idx = 2;
	if (lua_type(L, 2) == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_pushinstance(L, Datetime, 1);
		idx = -1;
	}
	SendMessage(lua_self(L, 1, Widget)->handle, MCM_SETCURSEL, 0, (LPARAM) luaL_checkcinstance(L, idx, Datetime)->st);
	return 0;
}

luaL_Reg Calendar_methods[] = {
	{"set_selected",	Calendar_setdate},
	{"get_selected",	Calendar_getdate},
	{NULL, NULL}
};

//------------------------------------ Panel properties

LUA_PROPERTY_GET(Panel, border) {
	lua_pushboolean(L, (GetWindowLong(lua_self(L, 1, Widget)->handle, GWL_STYLE) & WS_BORDER));
	return 1;
}

LUA_PROPERTY_SET(Panel, border) {
	HWND h = lua_self(L, 1, Widget)->handle;
	DWORD style = GetWindowLong(h, GWL_STYLE);

	style = lua_toboolean(L, 2) ? style | WS_BORDER : style & ~WS_BORDER;
	SetWindowLong(h, GWL_STYLE, style);
	SetWindowPos(h, 0, 0, 0, 0, 0, SWP_DRAWFRAME|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
	return 0;
}



luaL_Reg Panel_methods[] = {
	{"set_border",		Panel_setborder},
	{"get_border",		Panel_getborder},
	{"get_bgcolor",		Widget_getbgcolor},
	{"set_bgcolor",		Widget_setbgcolor},
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
	if (w->user && (w->wtype == UITab))
		DestroyWindow(w->user);
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
	{"get_align",		Widget_getalign},
	{"set_align",		Widget_setalign},
	{"get_parent",		Widget_getparent},
	{"center",			Widget_center},
	{"tofront",			Widget_tofront},
	{"toback",			Widget_toback},
	{NULL, NULL}
};

luaL_Reg hastext_methods[] = {
	{"get_hastext", Widget_gethastext},
	{"set_hastext", Widget_sethastext},
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