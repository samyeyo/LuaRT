
#pragma once

#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

template <class Type> void SafeRelease(Type **obj);

class Direct2D {

public:
	Direct2D(HWND handle, int width, int height);
	~Direct2D();

	ID2D1Factory            *Factory{};
    D2D1::Matrix3x2F        transform = D2D1::Matrix3x2F::Identity();
	IWICImagingFactory      *WICFactory;
    IDWriteFactory          *DWriteFactory;
    ID2D1DCRenderTarget   	*DCRender;
    ID2D1BitmapRenderTarget *Render;
    ID2D1SolidColorBrush    *colorBrush;
    D2D1_COLOR_F            color;
    D2D1_COLOR_F            bgcolor;
    IDWriteTextFormat       *textFormat;
    BOOL                    synced;

    const char              *error;
};