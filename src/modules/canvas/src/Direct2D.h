
#pragma once

#include "d2d1.h"
#include <dwrite.h>
#include <wincodec.h>

template <class Type> void SafeRelease(Type **obj);

class Direct2D {

public:
	Direct2D(HWND handle, int width, int height);
	~Direct2D();

	ID2D1Factory            *Factory{};
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