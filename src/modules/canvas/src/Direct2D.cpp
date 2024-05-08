#include "Direct2D.h"
#include <wincodec.h>
#include <stdio.h>
#include <Widget.h>

template <class Type> void SafeRelease(Type **obj) 
{
	if (*obj) {
		(*obj)->Release();
		*obj = NULL;
	}
}

Direct2D::Direct2D(HWND handle, int width, int height)
{
	error = NULL;
    synced = TRUE;
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &Factory)))
        error = "Failed to create Direct2D object";
    else if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&DWriteFactory))))
        error = "Failed to create Direct2D object";
    else if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,  __uuidof(IWICImagingFactory), (void**)&WICFactory)))
        error = "Failed to create WIC Imaging object";
    else {
        auto property = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0, 0.0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
        if (FAILED(Factory->CreateDCRenderTarget(&property, &DCRender)))
			error = "Failed to create Direct2D DCRender target";
        RECT r;
        DWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &textFormat);        
        GetClientRect(handle, &r);
        DCRender->BindDC(GetDC(handle), &r);        
        DCRender->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
        if (FAILED(DCRender->CreateCompatibleRenderTarget(&Render)))
			error = "Failed to create Direct2D bitmap DCRender target";
        else {
            color = D2D1::ColorF(D2D1::ColorF::Black, 1.0f);
            bgcolor = D2D1::ColorF(D2D1::ColorF::White, 1.0f);
            Render->CreateSolidColorBrush(color, &colorBrush);
        }
    }
}

Direct2D::~Direct2D()
{
    SafeRelease(&Factory);
    SafeRelease(&DWriteFactory);
    SafeRelease(&DCRender);
    SafeRelease(&Render);
    SafeRelease(&WICFactory);
    SafeRelease(&colorBrush);
    SafeRelease(&textFormat);
}
