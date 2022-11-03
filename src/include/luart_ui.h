/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | luart_ui.h | LuaRT ui header file
*/

#pragma once

#include <luart.h>
#include <commctrl.h>

//--------------------------------------------------| Widget object definition

typedef int WidgetType;

typedef struct WidgetItem {
	WidgetType	itemtype;
	LRESULT		iconstyle;
	union {
		TVITEMW				*treeitem;
		LVITEMW 			*listitem;
		TCITEMW 			*tabitem;
		COMBOBOXEXITEMW 	*cbitem;
		MENUITEMINFOW		*mi;
	};
} WidgetItem;

typedef struct Widget {
	luart_type	type;
	void		*handle;
	WidgetType	wtype;
	union {
		HANDLE		tooltip; //------ For standard Widgets, or Parent for Window:showmodal()
		HANDLE		parent;	 //------ For Menu
		int			menu;    //------ For Window
	};
	HANDLE		status;
	int			ref;
	HIMAGELIST	imglist;
	int 		index;
	WidgetItem	item;
	HFONT		font;
	HICON		icon;
	ACCEL		accel;
	HACCEL		accel_table;
	int			cursor;
	HCURSOR  	hcursor;
	BOOL 		autosize;
	UINT		events;
	HBRUSH		brush;
	COLORREF	color;
} Widget;

//--------------------------------------------------| Widget object definition
typedef enum{
    onHide 			= 0,
    onShow 			= 1, 
    onMove 			= 2, 
    onResize		= 3,
    onHover 		= 4,
    onLeave			= 5,
    onClose 		= 6,
    onClick 		= 7,
    onDoubleClick 	= 8,
    onContext 		= 9,
    onCreate		= 10,
	onCaret 		= 11,
    onChange 		= 12,
    onSelect 		= 13,
    onTrayClick		= 14,
    onTrayDoubleClick=15,
    onTrayContext 	= 16,
    onTrayHover 	= 17,
	onMenu			= 18
} WidgetEvent;

#define	WM_LUAMIN			(WM_USER+2)
#define WM_LUAHIDE 			(WM_LUAMIN + onHide)
#define WM_LUASHOW 			(WM_LUAMIN + onShow)
#define WM_LUAMOVE 			(WM_LUAMIN + onMove)
#define WM_LUARESIZE 		(WM_LUAMIN + onResize)
#define WM_LUAHOVER 		(WM_LUAMIN + onHover)
#define WM_LUALEAVE 		(WM_LUAMIN + onLeave)
#define WM_LUACLOSE 		(WM_LUAMIN + onClose)
#define WM_LUACLICK 		(WM_LUAMIN + onClick)
#define WM_LUADBLCLICK		(WM_LUAMIN + onDoubleClick)
#define WM_LUACREATE	 	(WM_LUAMIN + onCreate)
#define WM_LUACONTEXT 		(WM_LUAMIN + onContext)
#define WM_LUACHANGE 		(WM_LUAMIN + onChange)
#define WM_LUASELECT 		(WM_LUAMIN + onSelect)
#define WM_LUATRAYCLICK 	(WM_LUAMIN + onTrayClick)
#define WM_LUATRAYDBLCLICK	(WM_LUAMIN + onTrayDoubleClick)
#define WM_LUATRAYCONTEXT 	(WM_LUAMIN + onTrayContext)
#define WM_LUATRAYHOVER 	(WM_LUAMIN + onTrayHover)
#define WM_LUAMENU 			(WM_LUAMIN + onMenu)
#define WM_LUAMAX 			(WM_LUAMENU+1)

#define lua_callevent(w, e) PostMessage(w->handle, WM_LUAMIN+e, 0, 0)
#define lua_paramevent(w, e, p, pp) PostMessage(w->handle, WM_LUAMIN+e, (WPARAM)p, (LPARAM)pp)
#define lua_indexevent(w, e, i) PostMessage(w->handle, WM_LUAMIN+e, (WPARAM)(i), 0)
#define lua_closeevent(w, e) PostMessage(w->handle, WM_LUACLOSE, 0, 0)
#define lua_menuevent(i, idx) PostMessage(NULL, WM_LUAMENU, (WPARAM)i, (LPARAM)idx)

void lua_registerwidget(lua_State *L, int *type, char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_tooltip);
#define lua_regwidget(L, typename, methods, mt, has_text, has_font, has_cursor, has_icon, has_tooltip) lua_registerwidget(L, &T##typename, #typename, typename##_constructor, methods, mt, has_text, has_font, has_cursor, has_icon, has_tooltip)

typedef void *(*WIDGET_INIT)(lua_State *L, Widget **wp);
typedef Widget *(*WIDGET_FINALIZE)(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc);

extern WIDGET_INIT 		lua_widgetinitialize;
extern WIDGET_FINALIZE	lua_widgetfinalize;
extern luaL_Reg 		*WIDGET_METHODS;
extern luart_type 		TWidget;