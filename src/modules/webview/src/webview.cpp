/*
 | Webview for LuaRT - HTML/JS/CSS render Widget
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | webview.c | LuaRT binary module with Widget implementation
*/


#include <stddef.h>
#include <string.h>

#include <luart.h>
#include <Widget.h>
#include <File.h>
#include <Zip.h>	

#include <windows.h>
#include "handler.h"


//--- Webview type
static luart_type TWebview;
HANDLE URIEvent;
LPWSTR URI;
UINT onResult, onReady, onMessage, onLoaded;

//--- Webview procedure
LRESULT CALLBACK WebviewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
  Widget *w = (Widget *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  WebviewHandler *wv = static_cast<WebviewHandler*>(w->user);
  int result;

	switch (uMsg) {
		case WM_SIZE:
			wv->Resize();
			break;
		default:
			if ((result = lua_widgetproc(w, uMsg, wParam, lParam, 0)) < 0)
			    return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

//------------------------------------ Webview constructor
LUA_CONSTRUCTOR(Webview)
{   
    Widget *w, *wp;
    HWND h, hParent = (HWND)lua_widgetinitialize(L, &wp);     
    WebviewHandler *wv = new WebviewHandler;
    h = CreateWindowExW(0, L"Window", NULL, WS_VISIBLE | WS_CHILD, (int)luaL_optinteger(L, 4, 0), (int)luaL_optinteger(L, 5, 0), (int)luaL_optinteger(L, 6, 320), (int)luaL_optinteger(L, 7, 240), hParent, 0, GetModuleHandle(NULL),  NULL);
	wv->CreateWebview(h, luaL_checkstring(L, 3));
    w = lua_widgetconstructor(L, h, TWebview, wp, (SUBCLASSPROC)WebviewProc);
    w->user = wv;
    return 1;
}

LUA_METHOD(Webview, eval) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	wchar_t *js = lua_towstring(L, 2);
  	if (wv->webview2)
		wv->webview2->lpVtbl->ExecuteScript(wv->webview2, js, wv); 
	free(js);
	return 0;
}

LUA_METHOD(Webview, reload) {
	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	if (wv->webview2)
		wv->webview2->lpVtbl->Reload(wv->webview2);
	return 0;
}

LUA_METHOD(Webview, goback) {
	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	if (wv->webview2)
		wv->webview2->lpVtbl->GoBack(wv->webview2);
	return 0;
}

LUA_METHOD(Webview, goforward) {
	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	if (wv->webview2)
		wv->webview2->lpVtbl->GoForward(wv->webview2);
	return 0;
}

LUA_METHOD(Webview, stop) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->webview2)
    	wv->webview2->lpVtbl->Stop(wv->webview2);
  	return 0;
}

LUA_METHOD(Webview, opendevtools) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->webview2)
    	wv->webview2->lpVtbl->OpenDevToolsWindow(wv->webview2);
  	return 0;
}

LUA_METHOD(Webview, addinitscript) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->webview3) {
    	wchar_t *msg = lua_towstring(L, 2);
    	wv->webview3->lpVtbl->AddScriptToExecuteOnDocumentCreated(wv->webview3, msg, wv);
    	free(msg);
  	}
  	return 0;
}

LUA_METHOD(Webview, postmessage) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->webview2) {
    	wchar_t *msg = lua_towstring(L, 2);
    	result = SUCCEEDED(wv->webview2->lpVtbl->PostWebMessageAsJson(wv->webview2, msg));    
    	free(msg);
  	}
  	lua_pushboolean(L, result);
  	return 1;
}

LUA_METHOD(Webview, loadstring) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->webview2) {
    	wchar_t *str = toUTF16(lua_tostring(L, 2));
    	result = SUCCEEDED(wv->webview2->lpVtbl->NavigateToString(wv->webview2, str));
    	GlobalFree(str);
  } 
  lua_pushboolean(L, result);
  return 1;
}

LUA_PROPERTY_SET(Webview, url) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->webview2) {
    	wchar_t *uri = toUTF16(lua_tostring(L, 2));
    	result = SUCCEEDED(wv->webview2->lpVtbl->Navigate(wv->webview2, uri));
    	GlobalFree(uri);
  } 
  lua_pushboolean(L, result);
  return 1;
}

LUA_METHOD(Webview, hostfromfolder) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
	if (wv->webview3) {
		wchar_t *host = lua_towstring(L, 2);
		wchar_t *path = lua_towstring(L, 3);
		result = SUCCEEDED(wv->webview3->lpVtbl->SetVirtualHostNameToFolderMapping(wv->webview3, host, path, COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY));
		free(path);
		free(host);
	}
	lua_pushboolean(L, result);
	return 1;
}

LUA_METHOD(Webview, restorehost) {
   	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
	if (wv->webview3) {
		wchar_t *host = lua_towstring(L, 2);
		result = SUCCEEDED(wv->webview3->lpVtbl->ClearVirtualHostNameToFolderMapping(wv->webview3, host));
		free(host);
	}
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_GET(Webview, zoom) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	double zoom = 1;
  	if (wv->controller)
    	wv->controller->lpVtbl->get_ZoomFactor(wv->controller, &zoom);
  	lua_pushnumber(L, zoom);
  	return 1;
}

LUA_PROPERTY_SET(Webview, zoom) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	double zoom = 1;
  	if (wv->controller)
    	wv->controller->lpVtbl->put_ZoomFactor(wv->controller, (double)luaL_checknumber(L, 2));
  	return 0;
}

LUA_PROPERTY_GET(Webview, cangoback) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = false;
  	if (wv->webview2)
		wv->webview2->lpVtbl->get_CanGoBack(wv->webview2, &result);
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_GET(Webview, cangoforward) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = false;
  	if (wv->webview2)
		wv->webview2->lpVtbl->get_CanGoForward(wv->webview2, &result);
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_GET(Webview, url) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->webview2) {
		wchar_t *result = NULL;
		wv->webview2->lpVtbl->get_Source(wv->webview2, &result);
		lua_pushwstring(L, result);
		CoTaskMemFree(result);
	} else lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_GET(Webview, title) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->webview2) {
		wchar_t *result;
		wv->webview2->lpVtbl->get_DocumentTitle(wv->webview2, &result);
		lua_pushwstring(L, result);
		CoTaskMemFree(result);
	} else lua_pushnil(L);
	return 1;
}

LUA_PROPERTY_GET(Webview, devtools) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->settings)
   		wv->settings->lpVtbl->get_AreDevToolsEnabled(wv->settings, &result);
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_SET(Webview, devtools) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->settings)
   		wv->settings->lpVtbl->put_AreDevToolsEnabled(wv->settings, lua_toboolean(L, 2));
	return 0;
}

LUA_PROPERTY_GET(Webview, contextmenu) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->settings)
   		wv->settings->lpVtbl->get_AreDefaultContextMenusEnabled(wv->settings, &result);
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_SET(Webview, contextmenu) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->settings)
   		wv->settings->lpVtbl->put_AreDefaultContextMenusEnabled(wv->settings, lua_toboolean(L, 2));
	return 0;
}

LUA_PROPERTY_GET(Webview, statusbar) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
	int result = FALSE;
  	if (wv->settings)
   		wv->settings->lpVtbl->get_IsStatusBarEnabled(wv->settings, &result);
	lua_pushboolean(L, result);
	return 1;
}

LUA_PROPERTY_SET(Webview, statusbar) {
  	WebviewHandler *wv = static_cast<WebviewHandler*>(lua_self(L, 1, Widget)->user);
  	if (wv->settings)
   		wv->settings->lpVtbl->put_IsStatusBarEnabled(wv->settings, lua_toboolean(L, 2));
	return 0;
}

LUA_METHOD(Webview, __gc) {
  Widget *w = lua_widgetdestructor(L);

  delete (WebviewHandler *)w->user;
  free(w);
  return 0;
}

OBJECT_MEMBERS(Webview)
  METHOD(Webview, eval)
  METHOD(Webview, addinitscript)
  METHOD(Webview, reload)
  METHOD(Webview, goback)
  METHOD(Webview, goforward)
  METHOD(Webview, stop)
  METHOD(Webview, opendevtools)
  METHOD(Webview, loadstring)
  METHOD(Webview, hostfromfolder)
  METHOD(Webview, restorehost)
  METHOD(Webview, postmessage)
  READWRITE_PROPERTY(Webview, zoom)
  READWRITE_PROPERTY(Webview, devtools)
  READWRITE_PROPERTY(Webview, contextmenu)
  READWRITE_PROPERTY(Webview, statusbar)
  READWRITE_PROPERTY(Webview, url)
  READONLY_PROPERTY(Webview, title)
  READONLY_PROPERTY(Webview, cangoback)
  READONLY_PROPERTY(Webview, cangoforward)
END

OBJECT_METAFIELDS(Webview)
  METHOD(Webview, __gc)
END

static int event_stringarg(lua_State *L, const char *eventname) {
  if (lua_getfield(L, 1, eventname)) {
  	MSG *msg = (MSG *)lua_touserdata(L, 2);
    lua_pushwstring(L, (wchar_t*)msg->wParam);
    free((wchar_t*)msg->wParam);
    return 2;
  }
  return 1;
}

int event_onResult(lua_State *L) {
  return event_stringarg(L, "onResult");
}

int event_onReady(lua_State *L) {
  lua_getfield(L, 1, "onReady");
  return 1;
}

int event_onContext(lua_State *L) {
  lua_getfield(L, 1, "onContext");
  return 1;
}

int event_onLoaded(lua_State *L) {
	if (lua_getfield(L, 1, "onLoaded")) {
		MSG *msg = (MSG *)lua_touserdata(L, 2);
		lua_pushboolean(L, msg->wParam);
		lua_pushinteger(L, msg->lParam);
		return 3;
	}
  return 1;
}

int event_onMessage(lua_State *L) {
  return event_stringarg(L, "onMessage");
}

extern "C" {
	int __declspec(dllexport) luaopen_webview(lua_State *L) {
		onReady = lua_registerevent(L, NULL, event_onReady);
		onResult = lua_registerevent(L, NULL, event_onResult);
		onMessage = lua_registerevent(L, NULL, event_onMessage);
		onLoaded = lua_registerevent(L, NULL, event_onLoaded);
		luaL_require(L, "ui");
		lua_regwidgetmt(L, Webview, WIDGET_METHODS, FALSE, FALSE, FALSE, FALSE, FALSE);
		luaL_setrawfuncs(L, Webview_methods);
		return 0;
	}
}