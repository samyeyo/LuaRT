
#pragma once

#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>

class Direct2D;

#include "Gradient.h"
#include "Image.h"

template <class Type> void SafeRelease(Type **obj);

class Direct2D {

public:
	Direct2D(HWND handle, int width, int height);
	~Direct2D();
    void BeginDraw();

	ID2D1Factory            *Factory{};
    D2D1::Matrix3x2F        transform = D2D1::Matrix3x2F::Identity();
	IWICImagingFactory      *WICFactory;
    IDWriteFactory          *DWriteFactory;
    ID2D1HwndRenderTarget   *DCRender;
    ID2D1SolidColorBrush    *colorBrush;
    D2D1_COLOR_F            color;
    D2D1_COLOR_F            bgcolor;
    IDWriteTextFormat       *textFormat;

    std::vector<LinearGradient*> gradients;                    
    std::vector<Image*> images;                    

    const char              *error;
    bool                    painting;
};