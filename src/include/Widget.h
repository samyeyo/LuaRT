/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Widget.h | LuaRT Widget object header file
*/

#pragma once

#include <luart.h>
#include <luart_ui.h>
#include <window.h>
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <wincodec.h>

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

void widget_noinherit(lua_State *L, int *type, char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt);
void widget_type_new(lua_State *L, int *type, const char *typename, lua_CFunction constructor, const luaL_Reg *methods, const luaL_Reg *mt, BOOL has_text, BOOL has_font, BOOL has_cursor, BOOL has_icon, BOOL has_autosize, BOOL has_textalign, BOOL has_tooltip);
void *Widget_init(lua_State *L, Widget **wp);
Widget *Widget_finalize(lua_State *L, HWND h, WidgetType type, Widget *wp, SUBCLASSPROC proc);

#define lua_newtype_widget(L, typename) widget_type_new(L, #typename, typename##_constructor, Widget_methods, Widget_metafields, NULL)
#define lua_newtype_extwidget(L, typename) widget_type_new(L, #typename, typename##_constructor, Widget_methods, Widget_metafields, typename##_methods)

extern IWICImagingFactory *ui_factory;
extern HBITMAP ConvertToBitmap(void *pSource);
extern const char *cursors[], *luart_wtypes[], *events[];

#define NCURSORS 14
static const PSTR cursors_values[] = {
	IDC_ARROW, IDC_APPSTARTING, IDC_CROSS, IDC_HAND, IDC_HELP, IDC_IBEAM, IDC_NO, IDC_SIZEALL, IDC_SIZEWE, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZENESW, IDC_UPARROW, IDC_WAIT, NULL
};

extern luart_type TWidgetItem;

void new_items_mt(lua_State *L, Widget *w);
LRESULT CALLBACK WidgetProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
#define get_item(w, i) __get_item(w, i, NULL)
void *__get_item(Widget *w, int idx, HTREEITEM hti);
#define push_item(L, w, i) __push_item(L, w, i, NULL)
void *__push_item(lua_State *L, Widget *w, int idx, HTREEITEM hti);
#define free_item(w, i) __free_item(w, i, NULL)
void __free_item(Widget *w, size_t idx, HTREEITEM hti);
size_t get_count(Widget *w);
void add_column(Widget *w);

int getStyle(Widget *w, const int *values, const char *names[]);
void copy_menuitems(lua_State *L, HMENU from, HMENU to);

HBITMAP LoadImg(wchar_t *filename);
BOOL SaveImg(wchar_t *fname, HBITMAP hBitmap);
BOOL LoadFont(LPCWSTR file, LPLOGFONTW lf);
int fontsize_fromheight(int height);
LOGFONTW *Font(Widget *w);
void UpdateFont(Widget *w, LOGFONTW *l);
void SetFontFromWidget(Widget *w, Widget *wp);

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
LUA_METHOD(Widget, center);

LUA_CONSTRUCTOR(Window);

extern luaL_Reg Widget_cursor[];
extern luaL_Reg Widget_textalign[];
extern luaL_Reg Widget_tooltip[];
extern luaL_Reg Widget_methods[];
extern luaL_Reg Widget_metafields[];
extern luaL_Reg Window_methods[];
extern luaL_Reg Window_metafields[];
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
LUA_METHOD(Listbox, sort);
LUA_METHOD(Item, sort);

void Edit_update_scrollbar(HANDLE hwnd);
extern int Widget_loadicon(lua_State *L);
extern void pushDate(lua_State *L, HANDLE h);

Widget *check_widget(lua_State *L, int idx, WidgetType t);
Widget *Widget_create(lua_State *L, WidgetType type, DWORD exstyle, const wchar_t *classname, DWORD style, int caption, int autosize);
HICON widget_loadicon(lua_State *L);
void fontstyle_createtable(lua_State *L, LOGFONTW *l);
void fontstyle_fromtable(lua_State *L, int idx, LOGFONTW *l);
int fontsize_fromheight(int height);
Widget *format(lua_State *L, Widget *w, DWORD mask, CHARFORMAT2W *cf, int scf, BOOL isset);
Widget *get_fontstyle(lua_State *L, Widget *w, LOGFONTW *lf);
LOGFONTW *Font(Widget *w);
int GetText(lua_State *L, HANDLE h);
int AdjustTab_height(HWND h);
void FreeMenu(lua_State *L, Widget *w);
void remove_menuitem(lua_State *L, Widget *w, int idx);

LUA_CONSTRUCTOR(Item);
LUA_CONSTRUCTOR(Menu);
LUA_CONSTRUCTOR(MenuItem); 
LUA_METHOD(Widget, __gc);