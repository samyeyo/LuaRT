/*
 | Webview for LuaRT - HTML/JS/CSS render Widget
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE
 |--------------------------------------------------------
 | handler.h | Webview2 c++ interface class wrapper header
*/

#pragma once
#define CINTERFACE
#include <functional>
#include <windows.h>
#include <Zip.h>
#include "WebView2.h"
#include <string>
#include <vector>
#include <memory>

extern UINT onReady, onMessage, onLoaded, onFullscreen;
wchar_t *toUTF16(const char *s);

typedef struct {
	HANDLE thread;
	wchar_t *result;
} evalresult;

template <typename T>
static HRESULT STDMETHODCALLTYPE Null_QueryInterface(T* This, REFIID riid, void** ppvObject) {
	return E_NOINTERFACE;
}

template <typename T>
static ULONG STDMETHODCALLTYPE Null_AddRef(T* This) {
	return 1;
}

template <typename T>
static ULONG STDMETHODCALLTYPE Null_Release(T* This) {
	return 1;
}

class WebviewHandler :	public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
                     	public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
						public ICoreWebView2WebResourceRequestedEventHandler,
						public ICoreWebView2WebMessageReceivedEventHandler,
						public ICoreWebView2NavigationCompletedEventHandler,						
						public ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler,
						public ICoreWebView2ContainsFullScreenElementChangedEventHandler
{
public:
	WebviewHandler();
	virtual ~WebviewHandler();

	WebviewHandler(const WebviewHandler&) = delete;
	WebviewHandler(WebviewHandler&&) = delete;
	WebviewHandler& operator=(const WebviewHandler&) = delete;
	WebviewHandler& operator=(WebviewHandler&&) = delete;

	void Resize();
	bool CreateWebview(HWND h, const char *URL);

	HWND hwnd; 
	zip_t *archive = NULL;
	std::wstring url;
	std::vector<std::unique_ptr<evalresult*>> results;

	ICoreWebView2Settings3* settings = nullptr;
	ICoreWebView2* webview2 = nullptr;
	ICoreWebView2_3* webview3 = nullptr;
	ICoreWebView2Controller* controller = nullptr;
};

class ExecuteScriptCompletedCallback : public ICoreWebView2ExecuteScriptCompletedHandler
{
public:
	ExecuteScriptCompletedCallback();
	~ExecuteScriptCompletedCallback();
    
	wchar_t *result = NULL;
	BOOL done = FALSE;
};
