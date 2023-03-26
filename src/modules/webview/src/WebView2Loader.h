/*
 | Webview for LuaRT - HTML/JS/CSS render Widget
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE
 |-------------------------------------------------
 | WebView2Loader.h | static reimplementation of WebView2Loader for MinGW-w64
*/


#pragma once
#include <Windows.h>

struct ICoreWebView2EnvironmentOptions;
struct ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler;

STDAPI CreateCoreWebView2EnvironmentWithOptions(
    PCWSTR browserExecutableFolder, PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions *environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        *environmentCreatedHandler);

STDAPI CreateCoreWebView2Environment(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        *environmentCreatedHandler);

STDAPI GetAvailableCoreWebView2BrowserVersionString(
    PCWSTR browserExecutableFolder, LPWSTR *versionInfo);

STDAPI CompareBrowserVersions(PCWSTR version1, PCWSTR version2, int *result);
