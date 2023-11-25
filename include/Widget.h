#define LUA_LIB

#include <luart.h>
#include <commctrl.h>
#include <windows.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern int UIWindow;
extern int UIButton;
extern int UIGroup;
extern int UICheck;
extern int UIRadio;
extern int UILabel;
extern int UIEntry;
extern int UIPicture;
extern int UIDate;
extern int UIMenu;
extern int UIMenuItem;
extern int UITab;
extern int UIList;
extern int UICombo;
extern int UITree;
extern int UIEdit;
extern int UIItem;
extern int UIProgressbar;
extern int UIPanel;

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
	DWORD		style;
	int			cursor;
	HCURSOR  	hcursor;
	BOOL 		autosize;
	HBRUSH		brush;
	COLORREF	color;
	int			align;
	WINDOWPLACEMENT	wp;
	void		*user;
	BOOL		ismain;
} Widget;

//--- Register a widget type
LUA_API void lua_registerwidget(lua_State *L, int *type, const char *__typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_tooltip);
#define lua_regwidget(L, __typename, methods, mt, has_text, has_font, has_cursor, has_icon, has_tooltip) lua_registerwidget(L, &T##__typename, #__typename, __typename##_constructor, methods, mt, has_text, has_font, has_cursor, has_icon, has_tooltip)
#define lua_regwidgetmt(L, __typename, methods, has_text, has_font, has_cursor, has_icon, has_tooltip) lua_registerwidget(L, &T##__typename, #__typename, __typename##_constructor, methods, __typename##_metafields, has_text, has_font, has_cursor, has_icon, has_tooltip)

LUA_API lua_Integer lua_registerevent(lua_State *L, const char *methodname, lua_CFunction event) ;
LUA_API void *lua_getevent(lua_State *L, lua_Integer eventid, int *type);

#define WM_LUAEVENTRESULT 0

//---- call event e associated with widget w
#define lua_callevent(w, e) PostMessage((HWND)w->handle, e, 0, 0)
//---- call event e associated with widget w with parameters p and pp
#define lua_paramevent(w, e, p, pp) PostMessage((HWND)w->handle, e, (WPARAM)p, (LPARAM)pp)
//---- call event e associated with widget w with an index value i
#define lua_indexevent(w, e, i) PostMessage((HWND)w->handle, e, (WPARAM)(i), 0)

//--- Widget helper functions to be called inside widget constructor/destructor
typedef void *(*WIDGET_INIT)(lua_State *L, Widget **wp);
typedef Widget *(*WIDGET_CONSTRUCTOR)(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc);
typedef Widget *(*WIDGET_DESTRUCTOR)(lua_State *L);
typedef LRESULT (CALLBACK *WIDGET_PROC)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

LUA_API  lua_Integer			WM_LUAMAX;
LUA_API  WIDGET_INIT 			lua_widgetinitialize;
LUA_API  WIDGET_CONSTRUCTOR		lua_widgetconstructor;
LUA_API  WIDGET_DESTRUCTOR		lua_widgetdestructor;
LUA_API  WIDGET_PROC			lua_widgetproc;
LUA_API  luaL_Reg 				*WIDGET_METHODS;
LUA_API  luart_type 			TWidget;

#ifdef __cplusplus
}
#endif

