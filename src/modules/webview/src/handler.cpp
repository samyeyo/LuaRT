/*
 | Webview for LuaRT - HTML/JS/CSS render Widget
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE
 |-------------------------------------------------
 | handler.cpp | Webview2 c++ interface class wrapper
*/

#define CINTERFACE
#include "handler.h"
#include <WebView2.h>
#include <string>

wchar_t *toUTF16(const char *s) {
	DWORD size = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);
	WCHAR *ws = (WCHAR *)GlobalAlloc(GMEM_FIXED, sizeof(WCHAR) * size);
	if (ws == NULL)
		return NULL;
	MultiByteToWideChar(CP_UTF8, 0, s, -1, ws, size);
	return ws;
}

static HRESULT STDMETHODCALLTYPE EventInterfaces_EnvironmentCompleted_Invoke(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* This, HRESULT result, ICoreWebView2Environment* created_environment) {
	if (FAILED(result))
		return E_FAIL;

	WebviewHandler *self = static_cast<WebviewHandler*>(This);
	created_environment->lpVtbl->CreateCoreWebView2Controller(created_environment, self->hwnd, self);
	return S_OK;
}

static ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl EventInterfaces_EnvironmentCompletedHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_EnvironmentCompleted_Invoke,
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_ControllerCompleted_Invoke(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* This, HRESULT result, ICoreWebView2Controller* new_controller)
{
	if (FAILED(result))
		return E_FAIL;
	
	WebviewHandler *self = static_cast<WebviewHandler*>(This);
	EventRegistrationToken uritoken, msgtoken, navigtoken;

	self->controller = new_controller;
	self->controller->lpVtbl->AddRef(self->controller);
	self->controller->lpVtbl->get_CoreWebView2(self->controller, &self->webview2);
	self->webview2->lpVtbl->AddRef(self->webview2);
	self->webview2->lpVtbl->QueryInterface(self->webview2, IID_ICoreWebView2_3, (void**)&self->webview3);
  	self->webview2->lpVtbl->AddWebResourceRequestedFilter(self->webview2, NULL, COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
	self->webview2->lpVtbl->add_WebMessageReceived(self->webview2, self, &msgtoken);
	self->webview2->lpVtbl->add_NavigationCompleted(self->webview2, self, &navigtoken);
  	self->webview2->lpVtbl->add_WebResourceRequested(self->webview2, self, &uritoken);	
	self->webview2->lpVtbl->Navigate(self->webview2, self->url.c_str());
  	self->webview2->lpVtbl->get_Settings(self->webview2, (ICoreWebView2Settings**)&self->settings);	
	PostMessage(self->hwnd, onReady, 0, 0);
	self->Resize();
	return S_OK;
}

static ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl EventInterfaces_ControllerCompletedHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_ControllerCompleted_Invoke,
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_WebResourceRequested_Invoke(ICoreWebView2WebResourceRequestedEventHandler *This, ICoreWebView2 *sender, ICoreWebView2WebResourceRequestedEventArgs *args) {
	// WebviewHandler *self = static_cast<WebviewHandler*>(This);	
	// ICoreWebView2WebResourceRequest *request;
  
  	// if (SUCCEEDED(args->lpVtbl->get_Request(args, &request))) {
    // 	LPWSTR wuri;
    // 	if (SUCCEEDED(request->lpVtbl->get_Uri(request, &wuri)))
	// 		MessageBoxW(NULL, wuri, NULL, MB_OK);
	// 	GlobalFree(wuri);
	// }
	return S_OK;
}

static ICoreWebView2WebResourceRequestedEventHandlerVtbl EventInterfaces_WebResourceRequestedHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_WebResourceRequested_Invoke,
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_WebMessageReceived_Invoke(ICoreWebView2WebMessageReceivedEventHandler * This, ICoreWebView2 *webView, ICoreWebView2WebMessageReceivedEventArgs *args) {
	WebviewHandler *self = static_cast<WebviewHandler*>(This);	
	LPWSTR webMessage;

 	if (SUCCEEDED(args->lpVtbl->get_WebMessageAsJson(args, &webMessage)))
    	PostMessage(self->hwnd, onMessage, (WPARAM)_wcsdup(webMessage), 0);
	return S_OK;
}

static ICoreWebView2WebMessageReceivedEventHandlerVtbl EventInterfaces_WebMessageReceivedEventHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_WebMessageReceived_Invoke,
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_NavigationCompleted_Invoke(ICoreWebView2NavigationCompletedEventHandler *This, ICoreWebView2 *sender, ICoreWebView2NavigationCompletedEventArgs *args) {
	WebviewHandler *self = static_cast<WebviewHandler*>(This);	
	BOOL result;
	ICoreWebView2NavigationCompletedEventArgs2 *args2;
	int status;
	
	args->lpVtbl->QueryInterface(args, IID_ICoreWebView2NavigationCompletedEventArgs2, (void**)&args2);
	args->lpVtbl->get_IsSuccess(args, &result);
	args2->lpVtbl->get_HttpStatusCode(args2, &status);
	args2->lpVtbl->Release(args2);
	PostMessage(self->hwnd, onLoaded, (WPARAM)result, (LPARAM)status);
	return S_OK;
}

static ICoreWebView2NavigationCompletedEventHandlerVtbl EventInterfaces_NavigationCompletedEventHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_NavigationCompleted_Invoke
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_ExecScriptCompleted_Invoke(ICoreWebView2ExecuteScriptCompletedHandler *This, HRESULT hr, LPCWSTR resultObjectAsJson) {
	ExecuteScriptCompletedCallback *self = static_cast<ExecuteScriptCompletedCallback*>(This);	

	if (SUCCEEDED(hr))
		self->result = _wcsdup(resultObjectAsJson);
	self->done = TRUE;
	return S_OK;
}

static ICoreWebView2ExecuteScriptCompletedHandlerVtbl EventInterfaces_ExecuteScriptCompletedHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_ExecScriptCompleted_Invoke	
};

static HRESULT STDMETHODCALLTYPE EventInterfaces_AddScriptToExecuteOnDocumentCreatedCompleted_Invoke(ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *This, HRESULT error, PCWSTR i) {
	return S_OK;
}

static ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl EventInterfaces_AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl = {
	Null_QueryInterface,
	Null_AddRef,
	Null_Release,
	EventInterfaces_AddScriptToExecuteOnDocumentCreatedCompleted_Invoke
};

WebviewHandler::WebviewHandler() :
	ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler		{&EventInterfaces_EnvironmentCompletedHandlerVtbl},
	ICoreWebView2CreateCoreWebView2ControllerCompletedHandler  		{&EventInterfaces_ControllerCompletedHandlerVtbl},
	ICoreWebView2WebResourceRequestedEventHandler					{&EventInterfaces_WebResourceRequestedHandlerVtbl},
	ICoreWebView2WebMessageReceivedEventHandler						{&EventInterfaces_WebMessageReceivedEventHandlerVtbl},
	ICoreWebView2NavigationCompletedEventHandler					{&EventInterfaces_NavigationCompletedEventHandlerVtbl},
	ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler{&EventInterfaces_AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl}
{
}

WebviewHandler::~WebviewHandler() {
	this->webview2->lpVtbl->RemoveWebResourceRequestedFilter(this->webview2, L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
    this->webview3->lpVtbl->Release(this->webview3);
	this->webview2->lpVtbl->Release(this->webview2);
    this->controller->lpVtbl->Release(this->controller);
}

bool WebviewHandler::CreateWebview(HWND h, const char *URL) {
	std::wstring dataPath;
	wchar_t temp[1024];
	wchar_t *wstr;

	GetEnvironmentVariableW(L"AppData", temp, _countof(temp));
	dataPath = temp;
	GetModuleFileNameW(nullptr, temp, _countof(temp));
	wchar_t *szExeName = wcsrchr(temp, L'\\');
	szExeName = szExeName ? szExeName + 1 : temp;
	dataPath += L"\\";
	dataPath += szExeName;
	this->hwnd = h;
	wstr = toUTF16(URL);
	this->url = wstr;
	GlobalFree(wstr);
	return SUCCEEDED(CreateCoreWebView2EnvironmentWithOptions(nullptr, dataPath.substr(0, dataPath.find_last_of(L'.')).c_str(), nullptr, this));
}

void WebviewHandler::Resize() {
	if (!controller)
		return;

	RECT bounds;
	GetClientRect(this->hwnd, &bounds);
	controller->lpVtbl->put_Bounds(controller, bounds);
}

ExecuteScriptCompletedCallback::ExecuteScriptCompletedCallback() : ICoreWebView2ExecuteScriptCompletedHandler {&EventInterfaces_ExecuteScriptCompletedHandlerVtbl}
{	
}

ExecuteScriptCompletedCallback::~ExecuteScriptCompletedCallback()
{	
	if (this->result)
		free(result);
}
