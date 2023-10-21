/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Items.c | LuaRT Tree, List, Combobox, Tab and associated items object implementation
*/



#include <luart.h>
#include <Widget.h>
#include "ui.h"
#include <windowsx.h>
#include <uxtheme.h>
#include <stdint.h>


typedef struct sort_info {
	Widget *w;
	int  mode;
} sort_info;

static int CALLBACK sort_func(LPARAM lp1, LPARAM lp2, LPARAM data)
{
	int result = 0;
    sort_info *si = (sort_info*)(data);
    LVITEMW *item1 = get_item(si->w, lp1);
    LVITEMW *item2 = get_item(si->w, lp2);
	if (si->mode == -1)
    	result = item1->iItem < item2->iItem ? -1 : 1;
    else {
    	int cmp = wcscmp(item1->pszText, item2->pszText);
		result = si->mode ? cmp <= 0 : cmp >= 0;
	}
	free(item1->pszText);
	free(item1);
	free(item2->pszText);
	free(item2);
	return result;
}

int AdjustTab_height(HWND h) {
	RECT r;
	TabCtrl_GetItemRect(h, 0, &r);
	return r.bottom-r.top+1;
}

HTREEITEM treeview_iter(HWND handle, HTREEITEM hChild, HTREEITEM *hPrev, BOOL remove)
{
	if (!*hPrev) {
		*hPrev = TreeView_GetChild(handle, hChild);
		return *hPrev;
	}
	*hPrev = hChild;
	hChild = TreeView_GetNextSibling(handle, hChild);
	if (remove)
		TreeView_DeleteItem(handle, *hPrev);
	return hChild;
}

LRESULT CALLBACK PageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	Widget *w = (Widget*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch(Msg) {
		case WM_ERASEBKGND:	{
			RECT rect;
			GetClientRect(hWnd, &rect);
			FillRect((HDC)wParam, &rect, w->brush);
			return TRUE;
		}
		case WM_LBUTTONDOWN:lua_paramevent(w, onMouseDown, 0, MAKELPARAM(GET_X_LPARAM(lParam), AdjustTab_height(w->handle)+GET_Y_LPARAM(lParam)));
							lua_paramevent(w, onClick, GET_X_LPARAM(lParam), AdjustTab_height(w->handle)+GET_Y_LPARAM(lParam));
							break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			lua_paramevent(w, onMouseUp, (Msg-WM_LBUTTONUP)/3, MAKELPARAM(GET_X_LPARAM(lParam), AdjustTab_height(w->handle)+GET_Y_LPARAM(lParam)));
			break;

		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			lua_paramevent(w, onMouseDown, (Msg-WM_LBUTTONDOWN)/3, MAKELPARAM(GET_X_LPARAM(lParam), AdjustTab_height(w->handle)+GET_Y_LPARAM(lParam)));
			break;

		case WM_NOTIFY: 
			return SendMessage(((LPNMHDR)lParam)->hwndFrom, Msg, wParam, lParam);
		case WM_COMMAND:
			return SendMessage((HWND)lParam, Msg, wParam, lParam);
		case WM_CONTEXTMENU: 
		case WM_MOUSEMOVE:
			lParam = MAKELPARAM(GET_X_LPARAM(lParam), AdjustTab_height(w->handle)+GET_Y_LPARAM(lParam));		
		case WM_MOUSELEAVE:
			return WidgetProc(hWnd, Msg, 0, lParam, 0, 0);
		case WM_KEYDOWN:
			SendMessage(GetParent(w->handle), WM_KEYDOWN, wParam, lParam);
			break;
		case WM_SYSKEYDOWN:
			SendMessage(GetParent(w->handle), WM_SYSKEYDOWN, wParam, lParam);
			break;			
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC: return widget_setcolors(w, (HDC)wParam, (HWND)lParam);
		default: break;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//---- utility functions for Windows control with items

size_t get_count(Widget *w) {
	static int msg[] = {LVM_GETITEMCOUNT, CB_GETCOUNT, TVM_GETCOUNT, TCM_GETITEMCOUNT};
	WidgetType type = w->wtype == UIItem ? w->item.itemtype : w->wtype;
	HWND h = w->handle;
	if (type == UITree)
		return w->item.treeitem ? (size_t)w->item.treeitem->cChildren : TreeView_GetCount(h);
	if ((w->wtype == UIItem) && type == UITab)
		h = GetParent(h);
	return SendMessage(w->status ? w->status : h, msg[type-UIList], 0, 0); 
}

static wchar_t *get_text(HWND h, void *item, wchar_t **buff, int *len, UINT msg, int idx) { 
	int size = 64;
	int result = FALSE;
	
	do {
	      size *= 2;
	      free(*buff);
	      *buff = (wchar_t*)calloc(size, sizeof(wchar_t));
	      *len = size;
	      result = (int)SendMessageW(h, msg, (WPARAM)idx, (LPARAM)item);
	} while(result == FALSE);
	return *buff;
}

void *__get_item(Widget *w, int idx, HTREEITEM hti) {
	wchar_t **text = NULL;
	int *len;
	void *result;
	int i = 0;
	UINT msg;
	int t = w->wtype == UIItem ? w->item.itemtype : w->wtype;

	if (t == UITree) {
		if(!hti)
			return NULL;
		result = calloc(1, sizeof(TVITEMW));
		((TVITEMW*)result)->hItem = hti;
		((TVITEMW*)result)->mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_CHILDREN;
		text = &(((TVITEMW*)result)->pszText);
		len = &(((TVITEMW*)result)->cchTextMax);
		msg = TVM_GETITEMW;
	}
	else if (t == UITab) {
		result = calloc(1, sizeof(TCITEMW));
		((TCITEMW*)result)->mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
		text = &(((TCITEMW*)result)->pszText);
		len = &(((TCITEMW*)result)->cchTextMax);
		msg = TCM_GETITEMW;		
		i = idx;			
	}
	else if (t == UIList) {
		result = calloc(1, sizeof(LVITEMW));
		((LVITEMW*)result)->mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		((LVITEMW*)result)->iItem = idx;
		text = &(((LVITEMW*)result)->pszText);
		len = &(((LVITEMW*)result)->cchTextMax);
		msg = LVM_GETITEMW;
	}
	else if (t == UICombo) {
		result = calloc(1, sizeof(COMBOBOXEXITEMW));
		((COMBOBOXEXITEMW*)result)->mask = CBEIF_TEXT | CBEIF_LPARAM | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		((COMBOBOXEXITEMW*)result)->iItem = idx;
		text = &(((COMBOBOXEXITEMW*)result)->pszText);
		len = &(((COMBOBOXEXITEMW*)result)->cchTextMax);
		msg = CBEM_GETITEMW;		
	}
	get_text(w->handle, result, text, len, msg, i);
	return result;
}

void *__push_item(lua_State *L, Widget *w, int idx, HTREEITEM hti) {
	static const char *itemtypes[] = { "ListItem", "ComboItem", "TreeItem", "TabItem", "TreeItem"};
	void *item = __get_item(w, idx, hti);
	int narg = 3;
	if (item) {
		lua_pushlightuserdata(L, item);
		lua_pushinteger(L, w->wtype == UIItem ? UITree : w->wtype);
		if ((w->wtype != UIItem) && !hti)
			lua_pushinteger(L, idx);
		else narg = 2;
		lua_pushnewinstance(L, itemtypes[w->wtype == UIItem ? w->item.itemtype-UIList : w->wtype-UIList], narg);	
		lua_insert(L, -narg-1);
		lua_pop(L, narg);
	}
	else lua_pushnil(L);
	return item;
}

int adjust_listvscroll(Widget *w, int start, int end) {
	int i, size, width;
	if (start == -1) {
		RECT r;
		end = get_count(w);
		GetClientRect(w->handle, &r);
		ListView_SetColumnWidth(w->handle, 0, r.right-10);
		start = 0;
	}
	else
		end++;
	width = ListView_GetColumnWidth(w->handle, 0);
	for (i = start; i < end; i++) {
		LVITEMW *item = get_item(w, i);
		size = SendMessageW(w->handle, LVM_GETSTRINGWIDTHW, 0, (LPARAM)item->pszText);
		if (size > width) {
			ListView_SetColumnWidth(w->handle, 0, size);
			width = size;
		}
		free(item->pszText);
		free(item);
	}
	return width;
}

#define add_item(w, s) __add_item(w, s, NULL)
#if _WIN64 || __x86_64__	
static lua_Integer __add_item(Widget *w, wchar_t *str, HTREEITEM hti) {
	lua_Integer result = 0;
#else
static int __add_item(Widget *w, wchar_t *str, HTREEITEM hti) {
	int result = 0;
#endif
	HWND hh = 0, h = w->handle;
	UINT msg = 0;
	size_t count = get_count(w);
	int idx = 0;
	void *item = NULL;
	
	if (w->wtype == UIItem) {
		hti = hti ? hti : w->item.treeitem->hItem;
		goto tree;
	}
	else if (w->wtype == UITree) {
tree:	item = calloc(1, sizeof(TVINSERTSTRUCTW));
		((TVINSERTSTRUCTW*)item)->item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		((TVINSERTSTRUCTW*)item)->item.pszText = str;
		((TVINSERTSTRUCTW*)item)->item.lParam =(LPARAM)h;
		((TVINSERTSTRUCTW*)item)->hParent = hti;
		((TVINSERTSTRUCTW*)item)->hInsertAfter = TVI_LAST; 
		((TVINSERTSTRUCTW*)item)->item.iImage = INT16_MAX;
		((TVINSERTSTRUCTW*)item)->item.iSelectedImage = INT16_MAX;								
		msg = TVM_INSERTITEMW;	
	}
	else if (w->wtype == UITab) {
		item = calloc(1, sizeof(TCITEMW));
		RECT r; 
		GetClientRect(w->handle, &r);
		hh = CreateWindowExW(0, L"Window", NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, 1, 26, r.right-r.left-4, r.bottom-r.top-30, h, NULL, GetModuleHandle(NULL), NULL);
		SetWindowLongPtr(hh, GWLP_WNDPROC, (ULONG_PTR)PageProc);
		SetWindowLongPtr(hh, GWLP_USERDATA, (ULONG_PTR)w);
		BringWindowToTop(hh);
		((TCITEMW*)item)->mask = TCIF_TEXT | TCIF_PARAM;
		((TCITEMW*)item)->pszText = str;
		((TCITEMW*)item)->lParam = (LPARAM)hh;
		((TCITEMW*)item)->iImage = -1;
		msg = TCM_INSERTITEMW;
		idx = count;
	}
	else if (w->wtype == UIList) {
		item = calloc(1, sizeof(LVITEMW));	
		((LVITEMW*)item)->mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		((LVITEMW*)item)->pszText = str;
		((LVITEMW*)item)->lParam = (LPARAM)h;
		((LVITEMW*)item)->iImage = -1;
		((LVITEMW*)item)->iSubItem = 0;
		((LVITEMW*)item)->iItem = count;
		((LVITEMW*)item)->state = LVIS_FOCUSED;
		((LVITEMW*)item)->stateMask = LVIS_FOCUSED;
		msg = LVM_INSERTITEMW;
	}
	else if (w->wtype == UICombo) {
		item = calloc(1, sizeof(COMBOBOXEXITEMW));	
		((COMBOBOXEXITEMW*)item)->mask = CBEIF_TEXT | CBEIF_LPARAM | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		((COMBOBOXEXITEMW*)item)->pszText = str;
		((COMBOBOXEXITEMW*)item)->lParam =(LPARAM)h;
		((COMBOBOXEXITEMW*)item)->iItem = count;
		((COMBOBOXEXITEMW*)item)->iImage = -1;
		((COMBOBOXEXITEMW*)item)->iSelectedImage = -1;
		msg = CBEM_INSERTITEMW;
	}
	ImageList_SetImageCount(w->imglist, ImageList_GetImageCount(w->imglist)+1);
	result = SendMessageW(h, msg, idx, (LPARAM)item);
	if (w->wtype == UIList)
		adjust_listvscroll(w, count, count);
	free(str);
	free(item);
	return result;
}

#define free_item(w, i) __free_item(w, i, NULL)
void __free_item(Widget *w, size_t idx, HTREEITEM hti) {
	if (!hti && (w->wtype == UITree))
		hti = w->item.treeitem->hItem;
	if (hti) {
		TVITEMW *item = __get_item(w, 0, hti);
		TreeView_DeleteItem(w->handle, hti);
		if (get_count(w) == 0) {
			ImageList_RemoveAll(w->imglist);
			w->index = 0;
		}
		free(item->pszText);
		free(item);
	} else if (idx < (size_t)get_count(w)) {
		if(w->wtype == UIItem)
			w = ((Widget*)GetWindowLongPtr(w->item.itemtype == UITab ? GetParent(w->handle) : w->handle, GWLP_USERDATA));
		if (w->wtype == UITab) {
			TCITEMW *item = get_item(w, idx);
			size_t n;
			DestroyWindow((HWND)item->lParam);
			free(item->pszText);
			free(item);
			TabCtrl_DeleteItem(w->handle, idx);
			ImageList_Remove(w->imglist, idx);
			if ((n = get_count(w))) {
				idx = idx < n ? idx : idx-1;
				TCITEMW *newitem = get_item(w, idx);
				TabCtrl_SetCurFocus(w->handle, idx);
				BringWindowToTop((HWND)newitem->lParam);
				SetFocus((HWND)newitem->lParam);
				free(newitem->pszText);
				free(newitem);
			}
		} 
		else if (w->wtype == UIList) {
			sort_info si;
			ListView_DeleteItem(w->handle, idx);
			ImageList_Remove(w->imglist, idx);
			si.w = w;
			si.mode = -1;
			ListView_SortItemsEx(w->handle, sort_func, &si);
			ListView_SetColumnWidth(w->handle, 0, adjust_listvscroll(w, -1, 0));
		}
		else if (w->wtype == UICombo) {
			SendMessage(w->handle, CBEM_DELETEITEM, idx, 0);
			ImageList_Remove(w->imglist, idx);
		}	
	}
}

LUA_CONSTRUCTOR(Item) {
    Widget *wp, *w = (Widget*)calloc(1, sizeof(Widget));
    void *ptr = lua_touserdata(L, 2);

	if ( (w->item.itemtype = (int)lua_tointeger(L, 3)) != UITree)
		w->index = (int)lua_tointeger(L, 4);
	w->wtype = UIItem;
	if (w->item.itemtype == UITree) {
		w->item.treeitem = ptr;
		w->handle = (HWND)w->item.treeitem->lParam;
	}
	else if (w->item.itemtype == UITab) {
		w->item.tabitem = ptr;
		w->handle = (HWND)(w->item.tabitem->lParam);
	}
	if (w->item.itemtype == UIList) {
		w->item.listitem = ptr;
		w->handle = (HWND)w->item.listitem->lParam;
	}
	if (w->item.itemtype == UICombo) {
		w->item.cbitem = ptr;
		w->handle = (HWND)w->item.cbitem->lParam;
	}
    wp = (Widget*)GetWindowLongPtr(w->handle, GWLP_USERDATA);
    w->imglist = wp->imglist;
	if (w->item.itemtype == UITab) 
        SetFontFromWidget(w, wp);
	lua_newinstance(L, w, Widget);
	lua_pop(L, 1);
	lua_pushvalue(L, 1);
	return 1;
}

LUA_METHOD(Items, __len) {
	Widget *w;
	size_t count = 0;

	w = (luaL_getmetafield(L, 1, "__widget")) ? lua_self(L, -1, Widget) : lua_self(L, 1, Widget);
	if (w->wtype == UIItem) {
		HTREEITEM hti = w->item.treeitem->hItem, prev = NULL;
		while ( (hti = treeview_iter(w->handle, hti, &prev, FALSE)) )
			++count;
	} else count = get_count(w);
	lua_pushinteger(L, count);
	return 1;
}

static int find_item_bytext(lua_State *L, Widget *w, int idx, HTREEITEM *hti) {
	HTREEITEM result = NULL;
	DWORD start = TVGN_ROOT;
	wchar_t *text = lua_towstring(L, idx);
	
	idx = -1;
	if (w->wtype == UIItem) {
		result = w->item.treeitem->hItem;
		start = TVGN_CHILD;
		goto start;
	}
	else if (w->wtype == UITree) {	
start:		
		while((idx == -1) && (result = (HTREEITEM)SendMessageW(w->handle, TVM_GETNEXTITEM, (WPARAM)start, (LPARAM)result))) {
			if (result) {
				TVITEMW *item = __get_item(w, 0, result);
				if ( wcscmp(item->pszText, text) == 0 ) {
					result = item->hItem;
					idx = TRUE;
				}
				free(item->pszText);
				free(item);
				start = TVGN_NEXT;
			} else break;
		}
	}
	else if (w->wtype == UITab) {
		int i = get_count(w);
		while (--i > -1) {
			TCITEMW *item = get_item(w, i);
			if ( wcscmp(item->pszText, text) == 0 )
				idx = i;
			free(item->pszText);
			free(item);
			if (idx > -1)
				break;
		}
	}
	else if (w->wtype == UIList) { 
		LVFINDINFOW fi;
		fi.flags = LVFI_STRING;
		fi.psz = text;
		idx = SendMessageW(w->handle, LVM_FINDITEMW, -1, (LPARAM)&fi);
	}
	else if (w->wtype == UICombo)
		idx = SendMessageW(w->handle, CB_FINDSTRINGEXACT, -1, (LPARAM)text);
	free(text);
	*hti = result;
	return idx;
}

static void set_newitems(lua_State *L, Widget *w, int idx, HTREEITEM subitem) {
 
    if (lua_isstring(L, idx))
        __add_item(w, lua_towstring(L, idx), subitem);
    else if (!lua_istable(L, idx))
        luaL_error(L, "incompatible value in table (table expected, found %s)", lua_objectname(L, -1));
    else {
        lua_pushvalue(L, idx);
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            int keytype = lua_type(L, -2);
            if ((w->wtype == UITree || w->item.itemtype == UITree) && keytype == LUA_TSTRING) {
                HTREEITEM hti, new;
                find_item_bytext(L, w, -2, &hti);
                if (hti)
                    __free_item(w, 0, hti);
				new = (HTREEITEM)__add_item(w, lua_towstring(L, -2), (w->wtype == UIItem && subitem == NULL) ? w->item.treeitem->hItem : subitem);
                set_newitems(L, w, -1, new);            
            } else if ((keytype == LUA_TNUMBER) && lua_isinteger(L, -2)) {
                if (lua_isstring(L, -1))
                    __add_item(w, lua_towstring(L, -1), subitem);
                else luaL_error(L, "incompatible value in table (string expected, found %s)", lua_objectname(L, -1));
            } else luaL_error(L, "incompatible index in table (%s expected, found %s)", w->wtype == UITree ? "string or integer" : "integer", lua_objectname(L, -2));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
}

LUA_METHOD(Items, __index) {
    Widget *w;
    lua_Integer idx = -1;
    int isnum = 0;
    HTREEITEM hti = NULL;
	lua_Integer d = lua_tointegerx(L, 2, &isnum);

    luaL_getmetafield(L, 1, "__widget");
    w = lua_self(L, -1, Widget);

	if (isnum)
        idx = d-1;
	else if (lua_type(L, 2) == LUA_TSTRING)
        idx = find_item_bytext(L, w, 2, &hti);
	else luaL_error(L, "bad index while indexing items property (string or integer expected, found %s)", luaL_typename(L, 2));
	if ((idx >= 0) && ((w->wtype == UIItem) || (hti) || (idx < (int)get_count(w))))
        __push_item(L, w, idx, hti);
    else lua_pushnil(L);
    return 1; 
} 


LUA_METHOD(Items, __newindex) {
	Widget *w;

	luaL_getmetafield(L, 1, "__widget");
	w = lua_self(L, -1, Widget);
	if ((w->wtype != UITree) && (w->item.itemtype != UITree)) {
		lua_Integer idx;		
		int type = lua_type(L, 2);
		if (type != LUA_TNUMBER && type != LUA_TSTRING)
			luaL_error(L, "error : expecting an integer or string as key (found %s key)", lua_objectname(L, 2));
		idx = type == LUA_TNUMBER ? luaL_checkinteger(L, 2)-1 : find_item_bytext(L, w, 2, NULL);
		if ((idx > 0) && (idx < (lua_Integer)get_count(w)))
			free_item(w, idx);
		add_item(w, lua_towstring(L, 3));
	} else if (lua_type(L, 2) != LUA_TSTRING)
			luaL_error(L, "error : expecting a string key (found %s key)", lua_objectname(L, 2));
	else {
			HTREEITEM hti;
			find_item_bytext(L, w, 2, &hti);
			if (!hti)
				hti = (HTREEITEM)add_item(w, lua_towstring(L, 2));
			set_newitems(L, w, 3, hti);			
	}
	return 0;
}

static int Item_iter(lua_State *L) {
	Widget *w = lua_self(L, lua_upvalueindex(1), Widget);
	HTREEITEM hitem; 
#if _WIN64 || __x86_64__	
	lua_Integer idx = (lua_Integer)lua_tointeger(L, lua_upvalueindex(2));
#else
	int idx = (int)lua_tointeger(L, lua_upvalueindex(2));
#endif
	if (w->wtype == UIItem) {
		HTREEITEM hPrev = (HTREEITEM)idx;
		hitem = (HTREEITEM)(idx ? (HTREEITEM)idx : w->item.treeitem->hItem);
		if ((hitem = treeview_iter(w->handle, hitem, &hPrev, FALSE))) 
			goto push_treeitem;
		return 0;
	} else if (w->wtype == UITree) {
		hitem = idx ? TreeView_GetNextSibling(w->handle, (HTREEITEM)idx) : TreeView_GetRoot(w->handle);
		if (hitem) {
push_treeitem:
			__push_item(L, w, 0, hitem);
			lua_pushinteger(L, (lua_Integer)hitem);
			goto done;
		}
		return 0;
	} else {
		if ((idx > 0) && ((size_t)idx < get_count(w))) {
			push_item(L, w, idx);
			lua_pushinteger(L, ++idx);
done:		lua_replace(L, lua_upvalueindex(2));
			return 1;
		}
	}
	return 0;
}

LUA_METHOD(Items, __iterate) {
	luaL_getmetafield(L, 1, "__widget");
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, Item_iter, 2);
	return 1;
}

luaL_Reg Items_metafields[] = {
	{"__len",		Items___len},
	{"__index",		Items___index},
	{"__newindex",	Items___newindex},
	// {"__iterate", Items___iterate},
	{NULL, NULL}
};

void add_column(Widget *w) {
    LVCOLUMNW lvc;
	RECT r;

	GetClientRect(w->handle, &r);
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
    lvc.pszText = L"";
    lvc.cx = r.right-10;           
    SendMessageW(w->handle, LVM_INSERTCOLUMNW, 0, (LPARAM)&lvc);
}

LUA_METHOD(Listbox, clear) {
	Widget *w = lua_self(L, 1, Widget);
	if (w->wtype == UITree) {
		TreeView_DeleteAllItems(w->handle);
		w->index = 0;
	}
	else if (w->wtype == UITab)	{
		int i, count = get_count(w);
		for (i = count; i >= 0; i--)
			free_item(w, i);
		TabCtrl_DeleteAllItems(w->handle);
	}				
	else if(w->wtype == UIList) {
		ListView_DeleteAllItems(w->handle);
		adjust_listvscroll(w, -1, 0);
	}
	else if (w->wtype == UICombo) {
		ComboBox_SetText(w->handle, "");
		SendMessage(w->handle, CB_RESETCONTENT, 0, 0);		
	}
	ImageList_RemoveAll(w->imglist);
	return 0;
}

LUA_METHOD(Listbox, remove) {
	Widget *w = lua_self(L, 1, Widget);
	static const char *types[] = { "ListItem", "ComboItem", "TreeItem", "TabItem" };
	int idx = 0;
	HTREEITEM hti = NULL;
	
	switch (lua_type(L, 2)) {
		case LUA_TNUMBER:	idx = lua_tointeger(L, 2);
							break;
		case LUA_TSTRING:	if ((idx = find_item_bytext(L, w, 2, &hti)) == -1)
								luaL_error(L, "item not found");
							break;
		case LUA_TTABLE:	{
								Widget *item = check_widget(L, 2, UIItem);
								if (w->wtype != item->item.itemtype)
									luaL_typeerror(L, 2, types[w->wtype-UIList]);
								if (item->item.itemtype == UITree)
									hti = item->item.treeitem->hItem;
								else idx = item->index;
							} break;
				default:	luaL_typeerror(L, 2, "number, string or table");
	}
	__free_item(w, idx, hti);
	return 0;
}

LUA_METHOD(Listbox, add) {
	Widget *w = lua_self(L, 1, Widget);
	HTREEITEM hti = w->item.itemtype == UITree ? w->item.treeitem->hItem : NULL;
	int i = 1, count = lua_gettop(L);
#if _WIN64 || __x86_64__	
	lua_Integer last;
#else
	int last;
#endif
	if (w->wtype == UIList) 
		ShowScrollBar(w->handle, SB_VERT, TRUE);
	while(++i <= count)
		last = __add_item(w, lua_towstring(L, i), hti);
	__push_item(L, w, last, (w->wtype == UITree || w->item.itemtype == UITree) ? (HTREEITEM)last : NULL);
	return 1;
}

static const char* styles[] = { "text", "icons", NULL };

LUA_PROPERTY_GET(Listbox, style) {
	lua_pushstring(L, styles[lua_self(L, 1, Widget)->item.iconstyle]);
	return 1;
}

LUA_PROPERTY_SET(Listbox, style) {
	Widget *w = lua_self(L, 1, Widget);
	int style = luaL_checkoption(L, 2, "text", styles);
	HIMAGELIST imglist;
	
	if (style != w->item.iconstyle) {
		if (w->wtype == UICombo)
			SendMessage(w->handle, CBEM_SETEXTENDEDSTYLE, CBES_EX_NOEDITIMAGEINDENT, style ? 0: CBES_EX_NOEDITIMAGEINDENT);
		else if (w->wtype == UITree) {
			DWORD wstyle = GetWindowLongPtr(w->handle, GWL_STYLE);
			if (style) {
				SetWindowTheme(w->handle, L"Explorer", NULL);
				SetWindowLong(w->handle, GWL_STYLE, wstyle & ~TVS_HASLINES);
			}
			else {
				SetWindowTheme(w->handle, NULL, NULL);
				SetWindowLong(w->handle, GWL_STYLE, wstyle | TVS_HASLINES);
			}
			TreeView_SetImageList(w->handle, style ? w->imglist : NULL, TVSIL_NORMAL);
		} else if (w->wtype == UITab)
			TabCtrl_SetImageList(w->handle, style ? w->imglist : NULL);
		else {
			int count = ImageList_GetImageCount(w->imglist);
			if (style) {
				ImageList_Destroy((HIMAGELIST)SendMessage(w->handle, LVM_GETIMAGELIST, LVSIL_SMALL, 0));
				ImageList_SetImageCount(w->imglist, count+1);
				imglist = w->imglist;
			} else imglist = ImageList_Create(1, 1, ILC_COLOR32|ILC_MASK, count+1, 1);
			ListView_SetImageList(w->handle, imglist, LVSIL_SMALL);
		}
		w->item.iconstyle = style;			
	}
	return 0;
}

LUA_METHOD(Listbox, sort) {
	static const char* sort_modes[] = { "none", "ascend", "descend", NULL };
	sort_info si;
	
	si.w = lua_self(L, 1, Widget);
	si.mode = luaL_checkoption(L, 2, "none", sort_modes)-1;
	ListView_SortItemsEx(si.w->handle, sort_func, &si);
	return 0;
}

LUA_PROPERTY_GET(Listbox, count) {
	lua_pushinteger(L, get_count(lua_self(L, 1, Widget)));
	return 1;
}

LUA_PROPERTY_SET(Listbox, items) {
    Widget *w = lua_self(L, 1, Widget);
    static int resetmsg[] = {LVM_DELETEALLITEMS, CB_RESETCONTENT, TVM_DELETEITEM, TCM_DELETEALLITEMS};

	SendMessageW(w->handle, resetmsg[w->wtype-UIList], 0, (LPARAM)TVI_ROOT);
    set_newitems(L, w, 2, w->item.itemtype == UITree ? w->item.treeitem->hItem : NULL);
    if (w->wtype == UITab && get_count(w)) {
        TCITEMW *item = get_item(w, 0);
        BringWindowToTop((HWND)item->lParam);
        free(item->pszText);
        free(item);
    }
    InvalidateRect(w->handle, NULL, FALSE);
    return 0;
}
 
LUA_PROPERTY_GET(Listbox, items) {
	lua_createtable(L,  0, 0);
	lua_createtable(L, 0, 3);
	luaL_setfuncs(L, Items_metafields, 0);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "__widget");
	lua_setmetatable(L, -2);
	return 1;	
}

LUA_PROPERTY_GET(Listbox, selected) {
	Widget *w = lua_self(L, 1, Widget);
	LONG i = -1;
	HTREEITEM hti = NULL;
	
	lua_pushnil(L);
	if (w->wtype == UITree)
		hti = (HTREEITEM)SendMessageW(w->handle, TVM_GETNEXTITEM, TVGN_CARET, 0);
	else if (w->wtype == UITab)		
		i = SendMessageW(w->handle, TCM_GETCURSEL, 0, 0);
	else if (w->wtype == UIList)
		i = SendMessage(w->handle, LVM_GETNEXTITEM, -1,  LVNI_SELECTED | LVNI_FOCUSED);
	else if (w->wtype == UICombo)
		i = SendMessage(w->status, CB_GETCURSEL, 0, 0);
	if ( hti || (i != -1 && (LONG)get_count(w) >= i))
		__push_item(L, w, i, hti);
	return 1;
}

LUA_PROPERTY_SET(Listbox, selected) {
	Widget *w = lua_self(L, 1, Widget);
	HANDLE h = GetParent(w->handle);
	Widget *sel = lua_self(L, 2, Widget);
	UINT_PTR id = GetDlgCtrlID(w->handle);
	
	if (w->wtype != sel->item.itemtype)
		luaL_typeerror(L, 2, w->wtype == UITab ? "TabItem" : (w->wtype == UICombo ? "ComboItem" : "ListItem"));
	if ((w->handle == sel->handle || w->handle == GetParent(sel->handle)) && (sel->index >= 0 && sel->index <= (int)get_count(w))) {
		if (w->wtype == UITree)
			TreeView_SelectItem(w->handle, sel->item.treeitem->hItem);
		else if (w->wtype == UIList) {
			SetFocus(w->handle);
			ListView_EnsureVisible(w->handle, sel->index, FALSE);
			ListView_SetItemState(w->handle, sel->index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		}
		else if (w->wtype == UITab)
			SendMessageW(w->handle, TCM_SETCURFOCUS, sel->index, 0);
		else if (w->wtype == UICombo) {
			SendMessageW(w->status ? w->status : w->handle, CB_SETCURSEL, sel->index, 1);
			SetWindowTextW(w->handle, sel->item.cbitem->pszText);
		}
	}
	else 
		luaL_error(L, "cannot select an item that don't belong to this %s object", luaL_typename(L, 1));
	if (w->wtype == UITab) {
		NMHDR lparam = {0};
		lparam.code = TCN_SELCHANGE;
		lparam.idFrom = id;
		lparam.hwndFrom = w->handle;
		SendMessageW(h, WM_NOTIFY, (WPARAM)lparam.hwndFrom, (LPARAM)&lparam );
	}
	else if (w->wtype == UICombo)
		SendMessageW(w->handle , WM_COMMAND, MAKEWPARAM(id, CBN_SELCHANGE), (LPARAM)w->handle);
	UpdateWindow(w->handle);
	return 0;
}	

LUA_METHOD(Item, expand) {
	Widget *w = lua_self(L, 1, Widget);
	SendMessage(w->handle, TVM_EXPAND, lua_gettop(L) > 1 ? (lua_toboolean(L, 2) ? TVE_EXPAND : TVE_COLLAPSE) : TVE_TOGGLE, (LPARAM)w->item.treeitem->hItem);
	return 0;
}

LUA_METHOD(Item, show) {
	Widget *w = lua_self(L, 1, Widget);
	SendMessage(w->handle, TVM_ENSUREVISIBLE, 0, (LPARAM)w->item.treeitem->hItem);
	return 0;
}

LUA_METHOD(Item, edit) {
	Widget *w = lua_self(L, 1, Widget);
	SendMessageW(w->handle, TVM_EDITLABELW, 0, (LPARAM)w->item.treeitem->hItem);
	return 0;
}

LUA_METHOD(Item, loadicon) {	
	Widget *w = lua_self(L, 1, Widget);
	HANDLE h = w->handle;
	HICON icon = widget_loadicon(L);
	void *item = NULL;
	UINT msg = 0;
	int idx = w->index;
	
	if (icon) {
		if (w->icon)
			DestroyIcon(w->icon);
		if(w->item.itemtype == UITree) {
			msg = TVM_SETITEMW;
			item = w->item.treeitem;
			w->item.treeitem->mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			if (w->item.treeitem->iImage == INT16_MAX) {
				w->index = ((Widget*)GetWindowLongPtr(h, GWLP_USERDATA))->index++;
				w->item.treeitem->iImage = w->index;
			} else 
				w->index = w->item.treeitem->iImage;
			w->item.treeitem->iSelectedImage = w->index;
			idx = 0;
		}
		else if(w->item.itemtype == UITab) {
			msg = TCM_SETITEMW;
			w->item.tabitem->iImage = w->index;
			item = w->item.tabitem;
			h = GetParent(h);
		}
		else if(w->item.itemtype == UIList) {
			msg = LVM_SETITEMW;
			w->item.listitem->iImage = w->index;
			idx = 0;
			item = w->item.listitem;
		}
		else if(w->item.itemtype == UICombo) {
			msg = CBEM_SETITEMW;
			w->item.cbitem->iImage = w->index;
			w->item.cbitem->iSelectedImage = w->index;
			idx = 0;
			item = w->item.cbitem;
		}
		SendMessageW(h, msg, (WPARAM)idx, (LPARAM) item);
		if (ImageList_ReplaceIcon(w->imglist, w->index, icon) == -1) {
			DestroyIcon(icon);
			w->icon = NULL;
		} else {
			if (w->item.itemtype == UICombo)
				SendMessage(w->handle, CBEM_SETIMAGELIST, 0, (LPARAM)w->imglist);
			w->icon = icon;
		}
	}
	lua_pushboolean(L, icon && w->icon);
	return 1;
}

LUA_PROPERTY_GET(Item, index) {
	lua_pushinteger(L, lua_self(L, 1, Widget)->index+1);
	return 1;
}

LUA_PROPERTY_SET(Item, index) {
	Widget *w = lua_self(L, 1, Widget);
	sort_info si;
	int idx = (int)luaL_checkinteger(L, 2)-1;
	if (idx >= 0 && idx < (int)get_count(w)) {
		if(w->item.itemtype == UITab) {
			SendMessage(GetParent(w->handle), TCM_DELETEITEM, w->index, 0);
			SendMessageW(w->handle, TCM_INSERTITEMW, (WPARAM)idx, (LPARAM)w->item.tabitem);
		}
		else if(w->item.itemtype == UIList) {
		 	SendMessage(w->handle, LVM_DELETEITEM, (WPARAM)w->index, 0); 
			w->item.listitem->iItem = idx;
			SendMessageW(w->handle, LVM_INSERTITEMW, 0, (LPARAM)w->item.listitem);
			si.w = w;
			si.mode = -1;
			ListView_SortItemsEx(w->handle, sort_func, &si);
			ListView_SetItemState(w->handle, idx, LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
			SetFocus(w->handle);	
		}
		else if(w->item.itemtype == UICombo) {
			SendMessage(w->handle, CBEM_DELETEITEM, (WPARAM)w->index, 0);
			w->item.cbitem->iItem = idx;
			SendMessageW(w->handle, CBEM_INSERTITEMW, 0, (LPARAM)w->item.cbitem);
		}
		w->index = idx;
	}
	return 0;
}

LUA_METHOD(Item, sort) {
	Widget *w = lua_self(L, 1, Widget);
	TreeView_SortChildren(w->handle, w->item.treeitem ? w->item.treeitem->hItem : NULL, (LPARAM)lua_toboolean(L, 2));
	return 0;
}

LUA_METHOD(Item, clear) {
	Widget *w = lua_self(L, 1, Widget);
	HTREEITEM hti = w->item.treeitem->hItem, prev = NULL;
	while ( (hti = treeview_iter(w->handle, hti, &prev, TRUE)) );
	return 0;
}

LUA_METHOD(Item, remove) {
	Widget *w = lua_self(L, 1, Widget);
	HTREEITEM hti = NULL;
	
	if (w->item.itemtype == UITree) {
		if (lua_isstring(L, 2))
			find_item_bytext(L, w, 2, &hti);
		else
			hti = w->item.treeitem->hItem;
	} 
	__free_item(w, w->index, hti);
	return 0;
}

LUA_PROPERTY_GET(Item, text) {	
	Widget *w = lua_self(L, 1, Widget);
	wchar_t *text = NULL;
	
	if (w->item.itemtype == UITree)
		text = w->item.treeitem->pszText;
	else if(w->item.itemtype == UITab)
	  	text = w->item.tabitem->pszText;
	else if(w->item.itemtype == UIList)
		text = w->item.listitem->pszText;
	else if(w->item.itemtype == UICombo)
		text = w->item.cbitem->pszText;
	lua_pushwstring(L, text);
	return 1;
}

LUA_PROPERTY_SET(Item, text) {
	Widget *w = lua_self(L, 1, Widget);
	HWND h = w->handle;
	wchar_t *str = lua_towstring(L, 2);
	UINT msg = 0;
	int idx = 0;
	void *item = NULL;
	
	if(w->item.itemtype == UITree) {
		w->item.treeitem->mask = TVIF_TEXT | TVIF_PARAM;
		free(w->item.treeitem->pszText);
		w->item.treeitem->pszText = str;
		w->item.treeitem->lParam = (LPARAM)w->handle;
		item = w->item.treeitem;
		msg = TVM_SETITEMW;
	}
	else if(w->item.itemtype == UITab) {
		w->item.tabitem->mask = TCIF_TEXT | TCIF_PARAM;
		free(w->item.tabitem->pszText);
		w->item.tabitem->pszText = str;
		w->item.tabitem->lParam = (LPARAM)w->handle;
		h = GetParent(h);
		item = w->item.tabitem;
		idx = w->index;
		msg = TCM_SETITEMW;
	}
	else if(w->item.itemtype == UIList) {
		w->item.listitem->mask = LVIF_TEXT | LVIF_PARAM;
		free(w->item.listitem->pszText);
		w->item.listitem->pszText = str;
		w->item.listitem->lParam = (LPARAM)w->handle;
		item = w->item.listitem;
		msg = LVM_SETITEMW;
	}
	else if(w->item.itemtype == UICombo) {
		w->item.cbitem->mask = CBEIF_TEXT | LVIF_PARAM;
		free(w->item.cbitem->pszText);
		w->item.cbitem->pszText = str;
		w->item.cbitem->lParam = (LPARAM)w->handle;
		item = w->item.cbitem;
		msg = CBEM_SETITEMW;
	}
	SendMessageW(h, msg, (WPARAM)idx, (LPARAM)item);	
	return 1;
}

LUA_METHOD(Item, __eq) {
	Widget *item1 = lua_self(L, 1, Widget);
	Widget *item2 = lua_self(L, 2, Widget);
	if (item1->item.itemtype == UITree)
		lua_pushboolean(L, (item1->handle == item2->handle) && (item1->item.treeitem->hItem == item2->item.treeitem->hItem));
	else
		lua_pushboolean(L, (item1->handle == item2->handle) && (item1->index == item2->index));
	return 1;
}

LUA_METHOD(Item, __gc) {
	Widget *w = lua_self(L, 1, Widget);
	wchar_t *text = NULL;
	if (w->item.itemtype == UITree)
		text = w->item.treeitem->pszText;
	else if(w->item.itemtype == UITab)
	  	text = w->item.tabitem->pszText;
	else if(w->item.itemtype == UIList)
		text = w->item.listitem->pszText;
	else if(w->item.itemtype == UICombo)
		text = w->item.cbitem->pszText;
	free(text);
	free(w->item.listitem);
	DestroyIcon(w->icon);
	DeleteObject(w->font);
	free(w);
	return 0;
}

luaL_Reg ItemWidget_methods[] = {
	{"set_items",		Listbox_setitems},
	{"get_items",		Listbox_getitems},
	{"get_count",		Listbox_getcount},
	{"get_selected",	Listbox_getselected},
	{"set_selected",	Listbox_setselected},
	{"add",				Listbox_add},
	{"remove",			Listbox_remove},
	{"clear",			Listbox_clear},
	{"get_style",		Listbox_getstyle},
	{"set_style",		Listbox_setstyle},
	{NULL, NULL}
};

luaL_Reg Item_methods[] = {
	{"set_text",		Item_settext},
	{"get_text",		Item_gettext},
	{"get_owner", 		Widget_getparent},
	{"get_index",		Item_getindex},
	{"set_index",		Item_setindex},
	{"loadicon",		Item_loadicon},
	{"remove",			Item_remove},
	{NULL, NULL}
};

luaL_Reg TreeItem_methods[] = {
	{"set_text",		Item_settext},
	{"get_text",		Item_gettext},
	{"add",				Listbox_add},
	{"get_count",		Items___len},
	{"get_parent", 		Widget_getparent},
	{"loadicon",		Item_loadicon},
	{"expand",			Item_expand},
	{"show",			Item_show},
	{"edit",			Item_edit},
	{"remove",			Item_remove},
	{"clear",			Item_clear},
	{"sort",			Item_sort},
	{"get_subitems",	Listbox_getitems},
	{"set_subitems",	Listbox_setitems},
	{NULL, NULL}
};

luaL_Reg Page_methods[] = {
	{"get_enabled",		Widget_getenabled},
	{"set_enabled",		Widget_setenabled},
	{NULL, NULL}
};

luaL_Reg Item_metafields[] = {
	{"__eq",		Item___eq},
	{"__gc",		Item___gc},
	{NULL, NULL}
};
