/*
 | Webview for LuaRT - HTML/JS/CSS render Widget
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE
 |--------------------------------------------------------
 | handler.h | Webview2 c++ interface class wrapper header
*/

#pragma once
#define CINTERFACE
#include <functional>
#include <windows.h>
#include "WebView2.h"
#include <string>

extern UINT onResult, onReady, onMessage, onLoaded;
wchar_t *toUTF16(const char *s);

class WebviewHandler :	public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
                     	public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
						public ICoreWebView2WebResourceRequestedEventHandler,
						public ICoreWebView2WebMessageReceivedEventHandler,
						public ICoreWebView2NavigationCompletedEventHandler,
						public ICoreWebView2ExecuteScriptCompletedHandler,
						public ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler
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
	std::wstring url;

	ICoreWebView2Settings* settings = nullptr;
	ICoreWebView2* webview2 = nullptr;
	ICoreWebView2_3* webview3 = nullptr;
	ICoreWebView2Controller* controller = nullptr;
};
