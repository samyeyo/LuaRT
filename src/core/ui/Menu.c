/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Menu.c | LuaRT Menu object implementation
*/



#include "lrtapi.h"
#include <luart.h>
#include <Widget.h>
#include "ui.h"

#include <windowsx.h>
#include <commctrl.h>
#include <wincodec.h>


//--------------- Menu items utilities functions

#define SetMenuItemText(L, w, idx, str) 	SetMenuItem(L, w, idx, str, NULL, NULL, NULL)
#define SetMenuItemBMP(L, w, idx, bmp) 		SetMenuItem(L, w, idx, NULL, bmp, NULL, NULL)
#define SetMenuItemData(L, w, idx, data) 	SetMenuItem(L, w, idx, NULL, NULL, NULL, data)

static void SetMenuItem(lua_State *L, HMENU menu, int idx, wchar_t *str, HBITMAP bmp, HMENU popup, void *data) {
	MENUITEMINFOW mi = {0};
	
	mi.cbSize = sizeof(MENUITEMINFOW); 
	if (str) {
		size_t len = wcslen(str);
		if (len) {
			mi.fMask |= MIIM_STRING;
			mi.dwTypeData = str;
			mi.cch = len+1;
		} else {
			mi.fMask |= MIIM_FTYPE;
			mi.fType |= MFT_SEPARATOR;
		}
	}
	if (data) {
		mi.fMask |= MIIM_DATA;
		mi.dwItemData = (ULONG_PTR)data;
	}
	if (bmp) {
		mi.fMask |= MIIM_BITMAP;
		mi.hbmpItem = bmp;
	}
	if (popup) {
		mi.fMask |= MIIM_SUBMENU;
		mi.hSubMenu = popup;
	}
	if (!SetMenuItemInfoW(menu, idx == -1 ? GetMenuItemCount(menu)-1 : idx, TRUE, &mi)) {
		luaL_getlasterror(L, GetLastError());
		printf("SetMenuItemInfo error : %s ", lua_tostring(L, -1));
	}
	free(str);
}

#define GetMenuItem(m, i) GetMenuItemMask(m, i, 0);

static MENUITEMINFOW *GetMenuItemMask(HMENU menu, int idx, UINT mask) {
	MENUITEMINFOW *mi = calloc(1, sizeof(MENUITEMINFOW));
	
	mi->cbSize = sizeof(MENUITEMINFOW);
	mi->fMask = mask ? mask : MIIM_FTYPE | MIIM_SUBMENU | MIIM_DATA | MIIM_BITMAP;
	GetMenuItemInfoW(menu, idx, TRUE, mi);
	if (!mask) {
		if (mi->dwTypeData == MFT_STRING) {
			mi->fMask = MIIM_STRING;
			GetMenuItemInfoW(menu, idx, TRUE, mi);
			mi->dwTypeData = malloc((++(mi->cch))*sizeof(wchar_t));;
			GetMenuItemInfoW(menu, idx, TRUE, mi);
		}
	}
	return mi;
}

#define add_menuitem(L, w, item) add_menupopupitem(L, w, item, NULL)

static void add_menupopupitem(lua_State *L, Widget *w, wchar_t *item, HMENU popup) {
	UINT flags = MF_SEPARATOR;
	if (wcslen(item)) {
		flags = MF_STRING;
		if (popup)
			flags |= MF_POPUP;
	}
	AppendMenuW(w->handle, flags, (UINT_PTR)popup, item);
	SetMenuItemData(L, w->handle, -1, w);
	free(item);
}

void copy_menuitems(lua_State *L, HMENU from, HMENU to) {
	size_t i, count = GetMenuItemCount(from);
	for (i = 0; i < count; i++) {
		MENUITEMINFOW *mi = GetMenuItem(from, i);
		AppendMenuW(to, mi->cch ? MF_STRING : MF_SEPARATOR, (UINT_PTR)mi->hSubMenu, mi->dwTypeData);
		SetMenuItem(L, to, i, NULL, mi->hbmpItem, NULL, NULL);
		free(mi->dwTypeData);
		free(mi);
	}
}

void clear_menu(Widget *w) {
	int i = GetMenuItemCount(w->handle);
	while ( --i >= 0)
		RemoveMenu(w->handle, i, MF_BYPOSITION);
	DrawMenuBar(w->parent);
}

void init_menu(HMENU h) {
	MENUINFO mi = {0};
	mi.cbSize = sizeof(MENUINFO);
	mi.dwStyle = MNS_NOTIFYBYPOS;
	mi.fMask = MIM_STYLE | MIM_APPLYTOSUBMENUS;
	mi.dwStyle = MNS_NOTIFYBYPOS;
	SetMenuInfo(h, &mi);	
}

LUA_CONSTRUCTOR(Menu) {
	Widget *m;
	int n = lua_gettop(L), idx = 2;
	
	m = calloc(1, sizeof(Widget));
	m->handle = CreatePopupMenu();
	init_menu(m->handle);
	m->wtype = UIMenu;

	
	while (n >= idx)
		add_menuitem(L, m, lua_towstring(L, idx++));
	lua_newinstance(L, m, Widget);
	lua_pushvalue(L, 1);
	m->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	return 1;
}

static int add_menuitems(lua_State *L, lua_Integer idx, int stridx) {
	Widget *w = lua_self(L, 1, Widget);
	MENUITEMINFOW mi = {0};
	int count = GetMenuItemCount(w->handle)+1;
	int len, index = idx == -1 ? count : idx;
	wchar_t *str = lua_tolwstring(L, stridx, &len); 
	
	if (index > count)
		luaL_argerror(L, 2, "index exceed menu item count");
	mi.cbSize = sizeof(MENUITEMINFOW);
	mi.fMask = MIIM_STRING;
	if (len) {
		mi.cch = len+1;
		mi.dwTypeData = str;
		stridx++;
		if (lua_gettop(L) == stridx && lua_istable(L, stridx)) {
			mi.fMask |= MIIM_SUBMENU | MIIM_DATA;
			mi.dwItemData = (ULONG_PTR)check_widget(L, stridx, UIMenu);
			mi.hSubMenu = ((Widget*)mi.dwItemData)->handle;
		}
	} else {
		mi.fMask |= MIIM_FTYPE;
		mi.fType = MFT_SEPARATOR;
	}
	InsertMenuItemW(w->handle, --index, MF_BYPOSITION, &mi);
	free(str);
	DrawMenuBar(w->handle);
	lua_pushvalue(L, 1);
	lua_pushinteger(L, index);
	lua_pushinstance(L, MenuItem, 2);
	return 1;
}

LUA_METHOD(Menu, insert) {
	int idx = luaL_checkinteger(L, 2);
	if (idx < 1)
		luaL_argerror(L, 2, "invalid index value");
	return add_menuitems(L, idx, 3);
}

LUA_METHOD(Menu, add) {
	return add_menuitems(L, -1, 2);
}

void remove_menuitem(lua_State *L, Widget *w, int idx) {
	int index = idx == -1 ? w->index : idx;
	MENUITEMINFOW *mi = GetMenuItem(w->handle, index);
	luaL_unref(L, LUA_REGISTRYINDEX, mi->wID);
	RemoveMenu(w->handle, index, MF_BYPOSITION);
	DrawMenuBar(w->parent);
	free(mi->dwTypeData);
	free(mi);
}

LUA_METHOD(Menu, remove) {
	Widget *w = lua_self(L, 1, Widget);
	int idx = -1;

	if (lua_isinteger(L, 2))
		idx = lua_tointeger(L, 2)-1;
	else 
		w = check_widget(L, 2, UIMenuItem);
	remove_menuitem(L, w, idx);
	return 0;
}

LUA_METHOD(Menu, clear) {
	clear_menu(lua_self(L, 1, Widget));
	return 0;
}

LUA_PROPERTY_GET(Menu, count) {
	lua_pushinteger(L, GetMenuItemCount(lua_self(L, 1, Widget)->handle));
	return 1;
}

LUA_METHOD(MenuItems, __len) {
	luaL_getmetafield(L, 1, "__widget");
	lua_pushinteger(L, GetMenuItemCount(lua_self(L, -1, Widget)->handle));
	return 1;
}

LUA_METHOD(MenuItems, __index) {
	UINT idx = (UINT)luaL_checkinteger(L, 2)-1;

	luaL_getmetafield(L, 1, "__widget");
	if (idx < (UINT)(GetMenuItemCount(lua_self(L, -1, Widget)->handle))) {
		lua_pushinteger(L, idx);
		lua_pushinstance(L, MenuItem, 2);
	} else lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_GET(MenuItem, submenu) {
	Widget *w = lua_self(L, 1, Widget);
	
	lua_pushnil(L);
	if (w->item.mi->dwItemData)
		lua_rawgeti(L, LUA_REGISTRYINDEX, ((Widget *)w->item.mi->dwItemData)->ref);
	return 1;
}

LUA_PROPERTY_SET(MenuItem, submenu) {
	Widget *w = lua_self(L, 1, Widget);
	
	if (lua_isnil(L, 2))
		w->item.mi->dwItemData = 0;
	else {
		Widget *popup = check_widget(L, 2, UIMenu);
		SetMenuItem(L, w->handle, w->index, NULL, NULL, popup->handle, popup);	
		w->item.mi->dwItemData = (ULONG_PTR)popup;
	}
	return 0;
}


LUA_PROPERTY_GET(MenuItem, onClick) {
	Widget *w = lua_self(L, 1, Widget);
	MENUITEMINFOW mi = {0};
	
	mi.cbSize = sizeof(MENUITEMINFOW);
	mi.fMask = MIIM_ID;
	GetMenuItemInfoW(w->handle, w->index, TRUE, &mi);
	lua_rawgeti(L, LUA_REGISTRYINDEX, mi.wID);
	lua_getfield(L, -1, "onClick");
	return 1;
}

LUA_PROPERTY_SET(MenuItem, onClick) {
	Widget *w = lua_self(L, 1, Widget);
	MENUITEMINFOW mi = {0};
	UINT id = 0;
	
	mi.cbSize = sizeof(MENUITEMINFOW);
	mi.fMask = MIIM_ID;
	GetMenuItemInfoW(w->handle, w->index, TRUE, &mi);
	if (lua_isnil(L, 2))
	 	luaL_unref(L, LUA_REGISTRYINDEX, mi.wID);
	else {
		if (lua_type(L, 2) != LUA_TFUNCTION)
			luaL_typeerror(L, 2, "function");
		lua_pushstring(L, "onClick");
		lua_pushvalue(L, 2);
		lua_rawset(L, 1);
		lua_pushvalue(L, 1);
		id = (UINT)luaL_ref(L, LUA_REGISTRYINDEX); 
	}	
 	mi.wID = id;
 	SetMenuItemInfoW(w->handle, w->index, TRUE, &mi);
	return 0;	
}

LUA_METHOD(MenuItems, __newindex) {
	Widget *w;
	int idx = lua_tointeger(L, 2);
	
	luaL_getmetafield(L, 1, "__widget");
	w = lua_self(L, -1, Widget);
	if (idx < 1)
		luaL_error(L, "invalid MenuItem index value %d", idx);
	remove_menuitem(L, w, --idx);
	if (!lua_isnil(L, 3)) {
		int len;
		wchar_t *str = lua_tolwstring(L, 3, &len);
		MENUITEMINFOW mi = {0};
		mi.cch = len+1;
		mi.cbSize = sizeof(MENUITEMINFOW);
		mi.dwTypeData = str;
		mi.fMask = MIIM_STRING | MIIM_DATA;
		mi.dwItemData = (ULONG_PTR)w;
		InsertMenuItemW(w->handle, idx, TRUE, &mi);
		DrawMenuBar(w->parent);
		free(str);	
	}
	return 0;
}

static int Menu_iter(lua_State *L) {
	Widget *w = lua_self(L, lua_upvalueindex(1), Widget); 
	int idx = (int)lua_tointeger(L, lua_upvalueindex(2));
	if (idx < GetMenuItemCount(w->handle)) {
		lua_pushvalue(L, lua_upvalueindex(1));
		lua_pushinteger(L, idx);
		lua_pushinstance(L, MenuItem, 2);
		lua_pushinteger(L, ++idx);
		lua_replace(L, lua_upvalueindex(2));
		return 1;
	}
	return 0;
}

LUA_METHOD(MenuItems, __iterate) {
	luaL_getmetafield(L, 1, "__widget");
	lua_pushinteger(L, 0);
	lua_pushcclosure(L, Menu_iter, 2);
	return 1;
}

luaL_Reg MenuItems_metafields[] = {
	{"__len",		MenuItems___len},
	{"__index",		MenuItems___index},
	{"__newindex",	MenuItems___newindex},
	{"__iterate", 	MenuItems___iterate},
	{NULL, NULL}
};

LUA_PROPERTY_GET(Menu, items) {
	lua_createtable(L, 0, 0);
	lua_createtable(L, 0, 3);
	luaL_setfuncs(L, MenuItems_metafields, 0);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "__widget");
	lua_setmetatable(L, -2);
	return 1;
}

LUA_PROPERTY_SET(Menu, items) {
	Widget *w = lua_self(L, 1, Widget);

	luaL_checktype(L, 2, LUA_TTABLE);
	clear_menu(w);
	lua_pushvalue(L, 2);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		if (lua_isinteger(L, -2) && (lua_isstring(L, -1)))
			add_menuitem(L, w, lua_towstring(L, -1));
		else luaL_argerror(L, 2, "table with ordered strings expected");
		lua_pop(L, 1);
	}
	DrawMenuBar(w->parent);
	return 0;
}

HICON BMPToIcon(HBITMAP bmp) { 
	int x = GetSystemMetrics(SM_CXSMICON);
	int y = GetSystemMetrics(SM_CYSMICON);
	HBITMAP mask_bmp;
	unsigned char *mask;
	HICON icon = NULL;
  
  	mask = calloc(1, (x/8)*y);
	if ( (mask_bmp = CreateBitmap(x, y, 1, 1, mask)) ) {
	  	ICONINFO iconInfo;
	  	iconInfo.fIcon = TRUE;
	  	iconInfo.xHotspot = 0;
	  	iconInfo.yHotspot = 0;
	  	iconInfo.hbmMask = mask_bmp;
		iconInfo.hbmColor = bmp; 
		icon = CreateIconIndirect(&iconInfo); 
	}
	free(mask);
	return icon;
}

LUA_CONSTRUCTOR(MenuItem) {
	Widget *w = (Widget*)calloc(1, sizeof(Widget));
	Widget *parent = lua_self(L, 2, Widget);
	
	w->handle = parent->handle; // HMENU
	w->parent = parent->parent;
	w->index = (int)lua_tointeger(L, 3); //--- index
	w->item.itemtype = UIMenuItem;
	w->item.mi = GetMenuItem(w->handle, w->index);
	w->wtype = UIMenuItem;
	w->icon = BMPToIcon(w->item.mi->hbmpItem);
	lua_newinstance(L, w, Widget);
	lua_pop(L, 1);
	lua_pushvalue(L, 1);
	return 1;
}

int menuitem_state(lua_State *L, UINT state, BOOL get) {
	Widget *w = lua_self(L, 1, Widget);
	MENUITEMINFOW mi = {0};
	int result = 0;
	
	mi.fMask = MIIM_STATE;
	mi.cbSize = sizeof( mi );
	GetMenuItemInfoW(w->handle, w->index, TRUE, &mi);
	if (get) {
		lua_pushboolean(L, !(mi.fState == state));		
		result = 1;
	} else {
		mi.fState = state;
		SetMenuItemInfoW(w->handle, w->index, TRUE, &mi);			
	}
	DrawMenuBar(w->parent);
	return result;
}

LUA_METHOD(MenuItem, remove) {
	remove_menuitem(L, lua_self(L, 1, Widget), -1);
	return 0;
}

LUA_METHOD(MenuItem, loadicon) {
	Widget *w = lua_self(L, 1, Widget);
	IWICBitmap *ibmp;

	if (w->icon)
		DestroyIcon(w->icon);
	w->icon = widget_loadicon(L, FALSE);
	DeleteObject(w->item.mi->hbmpItem);
	if (w->icon && SUCCEEDED(ui_factory->lpVtbl->CreateBitmapFromHICON(ui_factory, w->icon, &ibmp)))
	{
		w->item.mi->hbmpItem = ConvertToBitmap(ibmp);
		ibmp->lpVtbl->Release(ibmp);
	} else
		w->item.mi->hbmpItem = NULL;
	SetMenuItemBMP(L, w->handle, w->index, w->item.mi->hbmpItem);
	lua_pushboolean(L, w->icon != NULL);
	return 1;
}

LUA_PROPERTY_SET(MenuItem, enabled) {
	return menuitem_state(L, lua_toboolean(L, 2) ? MFS_ENABLED : MFS_DISABLED, FALSE);
}

LUA_PROPERTY_GET(MenuItem, enabled) {
	return menuitem_state(L, MFS_DISABLED, TRUE);
}

LUA_PROPERTY_SET(MenuItem, checked) {
	return menuitem_state(L, lua_toboolean(L, 2) ? MFS_CHECKED : MFS_UNCHECKED, FALSE);
}

LUA_PROPERTY_GET(MenuItem, checked) {
	return menuitem_state(L, MFS_UNCHECKED, TRUE);
}

LUA_PROPERTY_GET(MenuItem, index) {
	lua_pushinteger(L, lua_self(L, 1, Widget)->index+1);
	return 1;
}

LUA_PROPERTY_SET(MenuItem, index) {
	Widget *w = lua_self(L, 1, Widget);
	int index = luaL_checkinteger(L, 2);
	
	if (w->item.mi->dwItemData) {
		w->item.mi->fMask |= MIIM_DATA | MIIM_SUBMENU;
		w->item.mi->hSubMenu = ((Widget*)w->item.mi->dwItemData)->handle;
	}
	RemoveMenu(w->handle, w->index, MF_BYPOSITION);
	if (index < 1)
		luaL_argerror(L, 2, "invalid index value");
	w->index = --index;
	InsertMenuItemW(w->handle, w->index, TRUE, w->item.mi);
	DrawMenuBar(w->parent);
	return 0;	
}
LUA_PROPERTY_GET(MenuItem, text) {
	lua_pushwstring(L, lua_self(L, 1, Widget)->item.mi->dwTypeData);
	return 1;	
}

LUA_PROPERTY_SET(MenuItem, text) {
	Widget *w = lua_self(L, 1, Widget);
	
	free(w->item.mi->dwTypeData);
	free(w->item.mi);
	SetMenuItemText(L, w->handle, w->index, lua_towstring(L, 2));
	w->item.mi = GetMenuItem(w->handle, w->index);
	DrawMenuBar(w->parent);
	return 0;
}

LUA_METHOD(MenuItem, __gc) {
	Widget *w = lua_self(L, 1, Widget);
	free(w->item.mi->dwTypeData);
	free(w->item.mi);
	return Widget___gc(L);
}

void FreeMenu(lua_State *L, Widget *w) {
	int count = GetMenuItemCount(w->handle);
	MENUITEMINFOW mi = {0};
	
	mi.cbSize = sizeof(MENUITEMINFOW);
	mi.fMask = MIIM_BITMAP | MIIM_ID;
	while(count > -1) {
		GetMenuItemInfoW(w->handle, --count, TRUE, &mi);
		if (mi.hbmpItem) 
			DeleteObject(mi.hbmpItem);
		mi.hbmpItem = NULL;
		luaL_unref(L, LUA_REGISTRYINDEX, mi.wID);
	}
	if (w->parent)
		SetMenu(w->parent, NULL);
	DestroyMenu(w->handle);
	w->parent = NULL;
	w->handle = NULL;
}

LUA_METHOD(Menu, __gc) {
	Widget *w = lua_self(L, 1, Widget);
	FreeMenu(L, w);
	return Widget___gc(L);
}

luaL_Reg Menu_metafields[] = {
	{"__gc", Menu___gc},
	{NULL, NULL}
};

luaL_Reg Menu_methods[] = {
	{"add",			Menu_add},
	{"insert",		Menu_insert},
	{"remove",		Menu_remove},
	{"clear",		Menu_clear},
	{"get_count",	Menu_getcount},
	{"get_items",	Menu_getitems},
	{"set_items",	Menu_setitems},
	{NULL, NULL}
};

luaL_Reg MenuItem_methods[] = {
	{"loadicon",	MenuItem_loadicon},
	{"remove",		MenuItem_remove},
	{"set_onClick",	MenuItem_setonClick},
	{"get_onClick",	MenuItem_getonClick},
	{"get_index",	MenuItem_getindex},
	{"set_index",	MenuItem_setindex},
	{"get_text",	MenuItem_gettext},
	{"set_text",	MenuItem_settext},
	{"get_submenu",	MenuItem_getsubmenu},
	{"set_submenu",	MenuItem_setsubmenu},
	{"get_enabled",	MenuItem_getenabled},
	{"set_enabled",	MenuItem_setenabled},
	{"get_checked",	MenuItem_getchecked},
	{"set_checked",	MenuItem_setchecked},
	{NULL, NULL}
};

luaL_Reg MenuItem_metafields[] = {
	{"__gc", MenuItem___gc},
	{NULL, NULL}
};


