/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Widget.h | LuaRT Widget object header file
*/

#pragma once

#include <luart.h>
#include <Widget.h>
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <wincodec.h>

#ifdef __cplusplus
extern "C" {
#endif

void widget_type_new(lua_State *L, int *type, const char *_typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_autosize, BOOL has_textalign, BOOL has_tooltip, BOOL is_parent, BOOL do_pop);
#define RegisterWidget(L, type, _typename, constructor, methods, mt, has_text, has_font, has_cursor, has_icon, has_autosize, has_textalign, has_tooltip, is_parent) widget_type_new(L, type, _typename, constructor, methods, mt, has_text, has_font, has_cursor, has_icon, has_autosize, has_textalign, has_tooltip, is_parent, TRUE)

void widget_noinherit(lua_State *L, int *type, char *_typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt);
void *Widget_init(lua_State *L, Widget **wp, double *dpi, BOOL *isdark);
Widget *Widget__constructor(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc);
Widget *Widget_destructor(lua_State *L);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#define lua_newtype_widget(L, typename) widget_type_new(L, #typename, typename##_constructor, Widget_methods, Widget_metafields, NULL)
#define lua_newtype_extwidget(L, typename) widget_type_new(L, #typename, typename##_constructor, Widget_methods, Widget_metafields, typename##_methods)

extern IWICImagingFactory *ui_factory;
extern HBITMAP ConvertToBitmap(void *pSource);
extern const char *cursors[], *luart_wtypes[], *events[];
extern HBRUSH DARK_BRUSH;
extern HBRUSH CBDARK_BRUSH;

#define NCURSORS 14
static const PSTR cursors_values[] = {
	IDC_ARROW, IDC_APPSTARTING, IDC_CROSS, IDC_HAND, IDC_HELP, IDC_IBEAM, IDC_NO, IDC_SIZEALL, IDC_SIZEWE, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZENESW, IDC_UPARROW, IDC_WAIT, NULL
};

LUA_API  luart_type TWidgetItem;

void new_items_mt(lua_State *L, Widget *w);
#define get_item(w, i) __get_item(w, i, NULL)
void *__get_item(Widget *w, int idx, HTREEITEM hti);
#define push_item(L, w, i) __push_item(L, w, i, NULL)
void *__push_item(lua_State *L, Widget *w, int idx, HTREEITEM hti);
#define free_item(w, i) __free_item(w, i, NULL)
void __free_item(Widget *w, size_t idx, HTREEITEM hti);
size_t get_count(Widget *w);
void add_column(Widget *w);
BOOL CALLBACK ResizeChilds(HWND h, LPARAM lParam);

void copy_menuitems(lua_State *L, HMENU from, HMENU to);

void do_align(Widget *w);
HBITMAP LoadImg(wchar_t *filename);
BOOL SaveImg(wchar_t *fname, HBITMAP hBitmap);
LUA_API BOOL LoadFont(LPCWSTR file, LPLOGFONTW lf);
LUA_API int fontsize_fromheight(int height);
LOGFONTW *Font(Widget *w);
void UpdateFont(Widget *w, LOGFONTW *l);
void SetFontFromWidget(Widget *w, Widget *wp);
LRESULT CALLBACK WidgetProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
BOOL CALLBACK EnumMonitor(HMONITOR h, HDC hdc, LPRECT r, LPARAM data);

LUA_METHOD(Widget, __metanewindex); //----- for event registration
LUA_METHOD(Widget, show);
LUA_METHOD(Widget, hide);
LUA_METHOD(Widget, align);
LUA_METHOD(Widget, autosize);
LUA_PROPERTY_GET(Widget, parent);
LUA_PROPERTY_GET(Widget, width);
LUA_PROPERTY_SET(Widget, width);
LUA_PROPERTY_GET(Widget, height);
LUA_PROPERTY_SET(Widget, height);
LUA_PROPERTY_GET(Widget, x);
LUA_PROPERTY_SET(Widget, x);
LUA_PROPERTY_GET(Widget, y);
LUA_PROPERTY_SET(Widget, y);
LUA_PROPERTY_GET(Widget, cursor);
LUA_PROPERTY_SET(Widget, cursor);
LUA_PROPERTY_GET(Widget, font);
LUA_PROPERTY_SET(Widget, font);
LUA_PROPERTY_GET(Widget, fontsize);
LUA_PROPERTY_SET(Widget, fontsize);
LUA_PROPERTY_GET(Widget, fontstyle);
LUA_PROPERTY_SET(Widget, fontstyle);
LUA_PROPERTY_GET(Widget, width);
LUA_PROPERTY_SET(Widget, width);
LUA_PROPERTY_GET(Widget, height);
LUA_PROPERTY_SET(Widget, height);
LUA_PROPERTY_GET(Widget, enabled);
LUA_PROPERTY_SET(Widget, enabled);
LUA_PROPERTY_GET(Widget, visible);
LUA_PROPERTY_SET(Widget, visible);
LUA_PROPERTY_GET(Widget, text);
LUA_PROPERTY_SET(Widget, text);
LUA_PROPERTY_GET(Widget, textalign);
LUA_PROPERTY_SET(Widget, textalign);
LUA_PROPERTY_GET(Widget, hastext);
LUA_PROPERTY_SET(Widget, hastext);
LUA_PROPERTY_GET(Widget, bgcolor);
LUA_PROPERTY_SET(Widget, bgcolor);
LUA_PROPERTY_GET(Panel, border);
LUA_PROPERTY_SET(Panel, border);
LUA_PROPERTY_GET(Widget, allowdrop);
LUA_PROPERTY_SET(Widget, allowdrop);
LUA_METHOD(Widget, center);
LUA_METHOD(Widget, vscroll);
LUA_METHOD(Widget, hscroll);

LUA_CONSTRUCTOR(Window);
LUA_PROPERTY_GET(Window, childs);

extern const luaL_Reg parent_methods[];
extern luaL_Reg Widget_cursor[];
extern luaL_Reg Widget_textalign[];
extern luaL_Reg Widget_tooltip[];
extern luaL_Reg Widget_methods[];
extern luaL_Reg Widget_metafields[];
extern const luaL_Reg Window_methods[];
extern const luaL_Reg Window_metafields[];
extern luaL_Reg WidgetText_methods[];
extern luaL_Reg WidgetTextAlign_methods[];
extern luaL_Reg WidgetTooltip_methods[];
extern luaL_Reg WidgetCursor_methods[];
extern luaL_Reg WidgetFont_methods[];
extern luaL_Reg Edit_methods[];
extern luaL_Reg Entry_methods[];
extern luaL_Reg ItemWidget_methods[];
extern luaL_Reg TreeItem_methods[];
extern luaL_Reg ItemsWidget_metafields[];
extern luaL_Reg Combobox_methods[];
extern luaL_Reg Checkbox_methods[];
extern luaL_Reg Calendar_methods[];
extern luaL_Reg Item_methods[];
extern luaL_Reg Page_methods[];
extern luaL_Reg Item_metafields[];
extern luaL_Reg Menu_methods[];
extern luaL_Reg Menu_metafields[];
extern luaL_Reg MenuItem_methods[];
extern luaL_Reg MenuItem_metafields[];
extern luaL_Reg Picture_methods[];
extern luaL_Reg color_methods[];
extern luaL_Reg Progressbar_methods[];
extern luaL_Reg Panel_methods[];
extern luaL_Reg hastext_methods[];
extern luaL_Reg sort_methods[];

extern DWORD uiLayout;

void Edit_update_scrollbar(HANDLE hwnd);
extern int Widget_loadicon(lua_State *L);
extern void pushDate(lua_State *L, HANDLE h);

INT_PTR widget_setcolors(Widget *w, HDC dc, HWND h);
Widget *check_widget(lua_State *L, int idx, WidgetType t);
Widget *Widget_create(lua_State *L, WidgetType type, DWORD exstyle, const wchar_t *classname, DWORD style, int caption, int autosize);
HICON widget_loadicon(lua_State *L, BOOL islarge);
void fontstyle_createtable(lua_State *L, LOGFONTW *l);
void fontstyle_fromtable(lua_State *L, int idx, LOGFONTW *l);
LUA_API int fontsize_fromheight(int height);
Widget *format(lua_State *L, Widget *w, DWORD mask, CHARFORMAT2W *cf, int scf, BOOL isset);
void get_fontstyle(lua_State *L, Widget *w, LOGFONTW *lf, int scf);
LOGFONTW *Font(Widget *w);
int GetText(lua_State *L, HANDLE h);
void FreeMenu(lua_State *L, Widget *w);
void remove_menuitem(lua_State *L, Widget *w, int idx);
int adjust_listvscroll(Widget *w, int start, int end);
void page_resize(Widget *w, BOOL isfocused);
BOOL CALLBACK AdjustThemeProc(HWND h, LPARAM isDark);
int ThemedMsgBox(const wchar_t *title, wchar_t *msg, UINT options);

LUA_CONSTRUCTOR(Item);
LUA_CONSTRUCTOR(Menu);
LUA_CONSTRUCTOR(MenuItem); 
LUA_METHOD(Widget, __gc);

//--------------------------------------------------| GUI Events

#define	WM_LUAMIN			(WM_USER+2)
#define WM_LUAHIDE 			(WM_LUAMIN)
#define WM_LUASHOW 			(WM_LUAMIN + 1)
#define WM_LUAMOVE 			(WM_LUAMIN + 2)
#define WM_LUARESIZE 		(WM_LUAMIN + 3)
#define WM_LUAHOVER 		(WM_LUAMIN + 4)
#define WM_LUALEAVE 		(WM_LUAMIN + 5)
#define WM_LUACLOSE 		(WM_LUAMIN + 6)
#define WM_LUACLICK 		(WM_LUAMIN + 7)
#define WM_LUADBLCLICK		(WM_LUAMIN + 8)
#define WM_LUACONTEXT 		(WM_LUAMIN + 9)
#define WM_LUACREATE	 	(WM_LUAMIN + 10)
#define WM_LUACARET 		(WM_LUAMIN + 11)
#define WM_LUACHANGE 		(WM_LUAMIN + 12)
#define WM_LUASELECT 		(WM_LUAMIN + 13)
#define WM_LUATRAYCLICK 	(WM_LUAMIN + 14)
#define WM_LUATRAYDBLCLICK	(WM_LUAMIN + 15)
#define WM_LUATRAYCONTEXT 	(WM_LUAMIN + 16)
#define WM_LUATRAYHOVER 	(WM_LUAMIN + 17)
#define WM_LUANOTIFYCLICK 	(WM_LUAMIN + 18)
#define WM_LUAMENU 			(WM_LUAMIN + 19)
#define WM_LUAKEY 			(WM_LUAMIN + 20)
#define WM_LUAMOUSEUP 		(WM_LUAMIN + 21)
#define WM_LUAMOUSEDOWN 	(WM_LUAMIN + 22)
#define WM_LUAMAXIMIZE  	(WM_LUAMIN + 23)
#define WM_LUAMINIMIZE   	(WM_LUAMIN + 24)
#define WM_LUARESTORE    	(WM_LUAMIN + 25)
#define WM_LUADROP    	    (WM_LUAMIN + 26)

typedef enum {
	onHide			= WM_LUAHIDE,
    onShow 			= WM_LUASHOW, 
    onMove 			= WM_LUAMOVE, 
    onResize		= WM_LUARESIZE,
    onHover 		= WM_LUAHOVER,
    onLeave			= WM_LUALEAVE,
    onClose 		= WM_LUACLOSE,
    onClick 		= WM_LUACLICK,
    onDoubleClick 	= WM_LUADBLCLICK,
    onContext 		= WM_LUACONTEXT,
    onCreate		= WM_LUACREATE,
    onCaret 		= WM_LUACARET,
    onChange 		= WM_LUACHANGE,
    onSelect 		= WM_LUASELECT,
    onTrayClick		= WM_LUATRAYCLICK,
    onTrayDoubleClick=WM_LUATRAYDBLCLICK,
    onTrayContext 	= WM_LUATRAYCONTEXT,
    onTrayHover 	= WM_LUATRAYHOVER,
    onNotificationClick	= WM_LUANOTIFYCLICK,
	onMenu			= WM_LUAMENU,
	onKey			= WM_LUAKEY,
    onMouseUp       = WM_LUAMOUSEUP,
    onMouseDown     = WM_LUAMOUSEDOWN,
    onMaximize      = WM_LUAMAXIMIZE,
    onMinimize      = WM_LUAMINIMIZE,
    onRestore       = WM_LUARESTORE,
    onDrop          = WM_LUADROP,
} WidgetEvent;

//---- call close event a associated with window w
#define lua_closeevent(w, e) PostMessage(w->handle, WM_LUACLOSE, 0, 0)
//--- call menu event references in LUA_REGISTRYINDEX with i 
#define lua_menuevent(i, idx) PostMessage(NULL, WM_LUAMENU, (WPARAM)i, (LPARAM)idx) 

#ifdef __cplusplus
}
#endif
