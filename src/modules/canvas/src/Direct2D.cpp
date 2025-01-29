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

Direct2D::Direct2D(HWND h, int width, int height)
{
	error = NULL;
    this->hwnd = h;
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &Factory)))
        error = "Failed to create Direct2D object";
    else if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&DWriteFactory))))
        error = "Failed to create Direct2D object";
    else if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,  __uuidof(IWICImagingFactory), (void**)&WICFactory)))
        error = "Failed to create WIC Imaging object";
    else {
        float dpiX, dpiY;
        Factory->GetDesktopDpi(&dpiX, &dpiY);
        auto property = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0, 0.0, D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING, D2D1_FEATURE_LEVEL_DEFAULT);
        auto hwndprop = D2D1::HwndRenderTargetProperties(h, D2D1::SizeU(width, height), D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        if (FAILED(Factory->CreateHwndRenderTarget(&property, &hwndprop, &DCRender)))
			error = "Failed to create Direct2D DCRender target";
        DCRender->SetDpi(dpiX, dpiY);
        DWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &textFormat);        
        DCRender->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
        color = D2D1::ColorF(D2D1::ColorF::Black, 1.0f);
        bgcolor = D2D1::ColorF(D2D1::ColorF::White, 1.0f);
        DCRender->CreateSolidColorBrush(color, &colorBrush);
    }
}

bool Direct2D::BeginDraw() {
    if (!painting) {
        DCRender->BeginDraw();
        painting = true;
        return true;
    }
    return false;
}

void Direct2D::EndDraw() {
    InvalidateRect(hwnd, NULL, FALSE);
    painting = false;
}

Direct2D::~Direct2D()
{
    SafeRelease(&Factory);
    SafeRelease(&DWriteFactory);
    SafeRelease(&DCRender);
    SafeRelease(&WICFactory);
    SafeRelease(&colorBrush);
    SafeRelease(&textFormat);
}
