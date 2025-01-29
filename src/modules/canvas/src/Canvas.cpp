/*
 | Canvas for LuaRT
 | Luart.org, Copyright (c) Tine Samir 2025.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Canvas.cpp | LuaRT binary module
*/

#include <luart.h>
#include <Widget.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>

#include "Direct2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Gradient.h"

//--- Canvas type
luart_type TCanvas;
UINT on_Hover;
UINT on_MouseDown;
UINT on_MouseClick;
UINT on_MouseUp;
UINT on_Paint;

//--- Brush table helpers
const char *style_values[] = { "normal", "oblique","italic" };

//--- Canvas procedure
LRESULT CALLBACK CanvasProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
  Widget *w = (Widget *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  Direct2D *d = (Direct2D *)w->user;


  switch(uMsg) {   
    case WM_PAINT:
    case WM_DISPLAYCHANGE:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      d->DCRender->EndDraw();
      EndPaint(hwnd, &ps);
      return FALSE;
    }

    case WM_SIZE:
        {
          d->DCRender->Release();
          d->colorBrush->Release();
          auto property = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0, 0.0, D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING, D2D1_FEATURE_LEVEL_DEFAULT);
          auto hwndprop = D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)), D2D1_PRESENT_OPTIONS_IMMEDIATELY);
          d->Factory->CreateHwndRenderTarget(&property, &hwndprop, &d->DCRender);
          d->DCRender->CreateSolidColorBrush(d->color, &d->colorBrush);
          for(LinearGradient *g : d->gradients) {
            g->collection->Release();
            g->d->DCRender->CreateGradientStopCollection(&g->stops[0], g->stops.size(), &g->collection);
            if (g->type = TLinearGradient) {
              D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = {};    
              g->linear->Release();
              d->DCRender->CreateLinearGradientBrush(props, g->collection, &g->linear);
            } else {
              D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};    
              ((RadialGradient *)g)->radial->Release(); 
              d->DCRender->CreateRadialGradientBrush(props, g->collection, &(((RadialGradient *)g)->radial));
            }
          } 
          for(Image *img : d->images) {
            img->Bitmap->Release(); 
            img->d->DCRender->CreateBitmapFromWicBitmap(img->IWICConverter, nullptr, &img->Bitmap);
          }
          d->DCRender->BeginDraw();
          d->DCRender->Clear(((Direct2D *)w->user)->bgcolor);   
          return TRUE; 
        }

      case WM_MOUSEMOVE:
        PostMessage(hwnd, on_Hover, wParam, lParam);
        return 0;

      case WM_LBUTTONUP:
      case WM_MBUTTONUP:
      case WM_RBUTTONUP:
        PostMessage(hwnd, on_MouseUp, wParam, lParam);
        return 0;
      case WM_SHOWWINDOW:
        if (wParam)
          PostMessage(hwnd, on_Paint, 0, 0);
        break;
      case WM_RBUTTONDOWN:
        PostMessage(hwnd, onContext, 0, 0);
        goto process_event;
      case WM_LBUTTONDOWN:
        PostMessage(hwnd, on_MouseClick, wParam, lParam);
      case WM_MBUTTONDOWN:
process_event:
        PostMessage(hwnd, on_MouseDown, wParam, lParam);
        return 0;
  }
  return lua_widgetproc(hwnd, uMsg, wParam, lParam, 0, 0);
}

//------------------------------------ Canvas constructor

static int onPaintContinue(lua_State* L, int status, lua_KContext ctx) {
  Widget *w = (Widget*)ctx;
  if (IsWindowVisible((HWND)w->handle)) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, w->ref);
    if (lua_getfield(L, -1, "onPaint") == LUA_TFUNCTION) {
        lua_insert(L, -2);
        if (lua_pcall(L, 1, 0, 0))
          lua_error(L);
    } else lua_pop(L, 2);
    return lua_yieldk(L, 0, ctx, onPaintContinue);
  }
  return 0;
}

LUA_CONSTRUCTOR(Canvas)
{   
  Widget *w, *wp;
  double dpi;
  BOOL isdark;
  HWND h, hParent = (HWND)lua_widgetinitialize(L, &wp, &dpi, &isdark);
	int width = (int)luaL_optinteger(L, 5, 320)*dpi, height = (int)luaL_optinteger(L, 6, 240)*dpi;

  h = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP, L"Window", NULL, WS_CHILD | WS_VISIBLE, (int)luaL_optinteger(L, 3, 0)*dpi, (int)luaL_optinteger(L, 4, 0)*dpi, width, height, hParent, 0, GetModuleHandle(NULL), NULL);    
  w = lua_widgetconstructor(L, h, TCanvas, wp, (SUBCLASSPROC)CanvasProc);
	w->user = new Direct2D(h, width, height);
	if (((Direct2D *)w->user)->error)
	  luaL_error(L, ((Direct2D *)w->user)->error); 
  ((Direct2D *)w->user)->DCRender->BeginDraw();
  ((Direct2D *)w->user)->bgcolor = isdark ? D2D1::ColorF(0.0,0.0,0.0) : D2D1::ColorF(1.0,1.0,1.0);
  ((Direct2D *)w->user)->DCRender->Clear(((Direct2D *)w->user)->bgcolor);   
  RedrawWindow(h, NULL, NULL, RDW_UPDATENOW);
  lua_pushtask(L, onPaintContinue, w, NULL);
  lua_pop(L, 1);
  return 1;
}

LUA_METHOD(Canvas, __gc) {
  Widget *w = lua_widgetdestructor(L);
  delete (Direct2D*)w->user;
  w->user = NULL;
  free(w);
  return 0;
}

static ID2D1Brush *getBrush(lua_State *L, Direct2D *d, int idx, double *dpi) {
  ID2D1Brush *brush = NULL;
  
  lua_uigetinfo(dpi, NULL);
  if (lua_gettop(L) >= idx)
    switch(lua_type(L, idx)) {
      case LUA_TNUMBER: {
                          lua_Integer color = luaL_checkinteger(L, idx);
                          d->DCRender->CreateSolidColorBrush(D2D1::ColorF(GetR(color)/255, GetG(color)/255, GetB(color)/255, GetA(color)/255), reinterpret_cast<ID2D1SolidColorBrush **>(&brush));
                        } break;
      case LUA_TTABLE:  {
                          luart_type t;
                          void *ptr = lua_tocinstance(L, idx, &t);                          
                          if (ptr) {
                            if (t == TImage)
                              d->DCRender->CreateBitmapBrush(((Image*)ptr)->Bitmap, D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), reinterpret_cast<ID2D1BitmapBrush **>(&brush));
                            else if (t == TLinearGradient) {
                              brush = ((LinearGradient *)ptr)->linear;
                              brush->AddRef();
                            } else if (t == TRadialGradient) {
                              brush = ((RadialGradient *)ptr)->radial;
                              brush->AddRef();
                            }
                          }
                        } break;
      default:  luaL_typeerror(L, idx, "color, Image or gradient");
    }
  return brush;
}

LUA_METHOD(Canvas, point) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 4, &dpi);
  float x = static_cast<FLOAT>(luaL_checknumber(L, 2));
  float y = static_cast<FLOAT>(luaL_checknumber(L, 3));

	d->DCRender->DrawRectangle(D2D1::RectF(x, y, x+0.5f, y+0.5f), brush ? brush : d->colorBrush, 0.5f, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, line) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 6, &dpi);
  
	d->DCRender->DrawLine(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f)*dpi, NULL);
  if (brush)
    brush->Release();  
	return 0;
}

LUA_METHOD(Canvas, rect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 6, &dpi);

	d->DCRender->DrawRectangle(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f)*dpi, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 6, &dpi);

	d->DCRender->FillRectangle(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, roundrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 8, &dpi);

	d->DCRender->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), static_cast<FLOAT>(luaL_checknumber(L, 6)), static_cast<FLOAT>(luaL_checknumber(L, 7))), brush ? brush : d->colorBrush, luaL_optnumber(L, 9, 1.0f)*dpi, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillroundrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 8, &dpi);

	d->DCRender->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), static_cast<FLOAT>(luaL_checknumber(L, 6)), static_cast<FLOAT>(luaL_checknumber(L, 7))), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, ellipse) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 6, &dpi);

	d->DCRender->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f)*dpi, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillellipse) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 6, &dpi);

  d->DCRender->FillEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, circle) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  lua_Number radius = luaL_checknumber(L, 4);
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 5, &dpi);

  radius *= dpi;
	d->DCRender->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), brush ? brush : d->colorBrush, luaL_optnumber(L, 6, 1.0f)*dpi, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillcircle) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  lua_Number radius = luaL_checknumber(L, 4);
  double dpi;
  ID2D1Brush *brush = getBrush(L, d, 5, &dpi);

  radius *= dpi;
	d->DCRender->FillEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

static IDWriteTextLayout *prepare_text(lua_State *L, Direct2D **d, double *dpi) {
  Widget *w = lua_self(L, 1, Widget);
	*d = (Direct2D*)w->user;
  int len;
  wchar_t *str = lua_tolwstring(L, 2, &len);
  IDWriteTextLayout* layout = NULL;
	FLOAT dpix, dpiy;
	RECT re;

  lua_uigetinfo(dpi, NULL);
  const D2D1_SIZE_F size = (*d)->DCRender->GetSize(); 
	(*d)->DWriteFactory->CreateTextLayout(str, len+1, (*d)->textFormat, size.width, size.height, &layout);
  free(str);
  return layout;
}

LUA_METHOD(Canvas, print) {
  Widget *w = lua_self(L, 1, Widget);
	Direct2D *d;
  double dpi;
  IDWriteTextLayout* layout = prepare_text(L, &d, &dpi);

  if (layout) {
    ID2D1Brush *brush = getBrush(L, d, 5, &dpi);
    d->DCRender->DrawTextLayout(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4))), layout, brush ? brush : d->colorBrush);
    layout->Release();
    if (brush)
      brush->Release();    
  }
  return 0;
}

LUA_METHOD(Canvas, measure) {
  Direct2D *d;
  double dpi;
  IDWriteTextLayout* layout = prepare_text(L, &d, &dpi);
  DWRITE_TEXT_METRICS tm;

  if (layout && SUCCEEDED(layout->GetMetrics(&tm))) {
    lua_createtable(L, 0, 2);
    lua_pushnumber(L, tm.widthIncludingTrailingWhitespace);
    lua_setfield(L, -2, "width");
    lua_pushnumber(L, tm.height);
    lua_setfield(L, -2, "height");
    return 1;
  }
  return 0;
}

LUA_METHOD(Canvas, rotate) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;
  double dpi;

  lua_uigetinfo(&dpi, NULL);
  if (lua_gettop(L) == 2) {
    const D2D1_SIZE_F& size = d->DCRender->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)));
  d->transform = d->transform * D2D1::Matrix3x2F::Rotation(luaL_checknumber(L, 2), center);
  d->DCRender->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, scale) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;
  double dpi;

  lua_uigetinfo(&dpi, NULL);
  if (lua_gettop(L) == 3) {
    const D2D1_SIZE_F& size = d->DCRender->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5)));
  d->transform = d->transform * D2D1::Matrix3x2F::Scale(D2D1::SizeF(luaL_checknumber(L, 2), luaL_checknumber(L, 3)), center);
  d->DCRender->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, translate) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  double dpi;

  lua_uigetinfo(&dpi, NULL);
  d->transform = d->transform * D2D1::Matrix3x2F::Translation(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)));
  d->DCRender->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, skew) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;
  double dpi;
  
  lua_uigetinfo(&dpi, NULL);
  if (lua_gettop(L) == 3) {
    const D2D1_SIZE_F& size = d->DCRender->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5)));
  d->transform = d->transform * D2D1::Matrix3x2F::Skew(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  d->DCRender->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, identity) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  d->transform = D2D1::Matrix3x2F::Identity();
  d->DCRender->SetTransform(d->transform);
	return 0;  
}

LUA_METHOD(Canvas, clear) {
  Widget *w = lua_self(L, 1, Widget);
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  D2D1_COLOR_F color = d->bgcolor;

  if (lua_gettop(L) > 1) {
    lua_Integer rgba = luaL_checkinteger(L, 2);
    color = D2D1::ColorF(GetR(rgba)/255, GetG(rgba)/255, GetB(rgba)/255, GetA(rgba)/255);
  }
  d->DCRender->Clear(color);
	return 0;
}

static int new_instance(lua_State *L, const char *type) {
	int i, n = lua_gettop(L);
  lua_pushlightuserdata(L, (lua_self(L, 1, Widget))->user);
  lua_insert(L, 2);
  lua_pushnewinstance(L, type, n);
  return 1;
}

LUA_METHOD(Canvas, Image) {
	return new_instance(L, "Image");
}

LUA_METHOD(Canvas, LinearGradient) {
	return new_instance(L, "LinearGradient");
}

LUA_METHOD(Canvas, RadialGradient) {
	return new_instance(L, "RadialGradient");
}

LUA_PROPERTY_GET(Canvas, color) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;

  lua_pushinteger(L, (UINT32)RGBA((int)(d->color.r*255), (int)(d->color.g*255), (int)(d->color.b*255), int(d->color.a*255)));
  return 1;
}

LUA_PROPERTY_SET(Canvas, color) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  lua_Integer color = luaL_checkinteger(L, 2);
  
  d->colorBrush->Release();
  d->color = D2D1::ColorF(GetR(color)/255, GetG(color)/255, GetB(color)/255, GetA(color)/255);
  d->DCRender->CreateSolidColorBrush(d->color, reinterpret_cast<ID2D1SolidColorBrush **>(&d->colorBrush));
  return 0;
}

LUA_PROPERTY_GET(Canvas, bgcolor) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;

  lua_pushinteger(L, (UINT32)RGBA((int)(d->bgcolor.r*255), (int)(d->bgcolor.g*255), (int)(d->bgcolor.b*255), int(d->bgcolor.a*255)));
  return 1;
}

LUA_PROPERTY_SET(Canvas, bgcolor) {
  Widget *w = lua_self(L, 1, Widget);
  Direct2D *d = (Direct2D*)w->user;
  lua_Integer bgcolor = luaL_checkinteger(L, 2);
  
  d->bgcolor = D2D1::ColorF(GetR(bgcolor)/255, GetG(bgcolor)/255, GetB(bgcolor)/255, GetA(bgcolor)/255);
  d->DCRender->Clear(d->bgcolor);
  return 0;
}

LUA_PROPERTY_GET(Canvas, font) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  wchar_t fontname[256];
  d->textFormat->GetFontFamilyName(fontname, 256);
	lua_pushwstring(L, fontname);
	return 1;
}

static int setFont(lua_State *L, wchar_t *fontname, float size, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STRETCH stretch, DWRITE_FONT_STYLE style) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  wchar_t oldfontname[256];

  if (!weight)
    weight = d->textFormat->GetFontWeight();
  if (!size)
    size = d->textFormat->GetFontSize();
  if (!stretch)
    stretch = d->textFormat->GetFontStretch();
  if (!style)
    style = d->textFormat->GetFontStyle();
  d->textFormat->GetFontFamilyName(oldfontname, 256);
  d->textFormat->Release();

  if (FAILED(d->DWriteFactory->CreateTextFormat(fontname ? fontname : oldfontname, NULL, weight, style, stretch, size, L"en-us", &d->textFormat)))
    d->DWriteFactory->CreateTextFormat(oldfontname, NULL, weight, style, stretch, size, L"en-us", &d->textFormat);      
  free(fontname);    
  return 0;
}

LUA_PROPERTY_SET(Canvas, font) {
	return setFont(L, lua_towstring(L, 2), 0, (DWRITE_FONT_WEIGHT)0, (DWRITE_FONT_STRETCH)0, (DWRITE_FONT_STYLE)0);
}

LUA_PROPERTY_SET(Canvas, fontsize) {
  return setFont(L, NULL, luaL_checknumber(L, 2), (DWRITE_FONT_WEIGHT)0, (DWRITE_FONT_STRETCH)0, (DWRITE_FONT_STYLE)0);
}

LUA_PROPERTY_GET(Canvas, fontsize) {
  lua_pushnumber(L, ((Direct2D*)(lua_self(L, 1, Widget))->user)->textFormat->GetFontSize());
	return 1;
}

LUA_PROPERTY_SET(Canvas, fontweight) {
  return setFont(L, NULL, 0, (DWRITE_FONT_WEIGHT)luaL_checkinteger(L, 2), (DWRITE_FONT_STRETCH)0, (DWRITE_FONT_STYLE)0);
}

LUA_PROPERTY_GET(Canvas, fontweight) {
  lua_pushinteger(L, ((Direct2D*)(lua_self(L, 1, Widget))->user)->textFormat->GetFontWeight());
	return 1;
}

LUA_PROPERTY_SET(Canvas, fontstretch) {
  return setFont(L, NULL, 0, (DWRITE_FONT_WEIGHT)0,  static_cast<DWRITE_FONT_STRETCH>(luaL_checkinteger(L, 2)), (DWRITE_FONT_STYLE)0);
}

LUA_PROPERTY_GET(Canvas, fontstretch) {
  lua_pushinteger(L, ((Direct2D*)(lua_self(L, 1, Widget))->user)->textFormat->GetFontStretch());
	return 1;
}

LUA_PROPERTY_SET(Canvas, fontstyle) {
  return setFont(L, NULL, 0, (DWRITE_FONT_WEIGHT)0, (DWRITE_FONT_STRETCH)0, static_cast<DWRITE_FONT_STYLE>(luaL_checkoption(L, 2, "normal", style_values)));
}

LUA_PROPERTY_GET(Canvas, fontstyle) {
  lua_pushstring(L, style_values[((Direct2D*)(lua_self(L, 1, Widget))->user)->textFormat->GetFontStyle()]);
	return 1;
}

LUA_METHOD(Canvas, begin) {
  ((Direct2D*)(lua_self(L, 1, Widget))->user)->DCRender->BeginDraw();
  return 0;
}

LUA_METHOD(Canvas, flip) {
  ((Direct2D*)(lua_self(L, 1, Widget))->user)->EndDraw();
  RedrawWindow(((Direct2D*)lua_self(L, 1, Widget)->user)->hwnd, NULL, NULL, RDW_UPDATENOW);
  return 0;
}

D2D1_POINT_2F toDIP(Direct2D *d, int x, int y) { 
  float dpiX, dpiY;
  D2D1_POINT_2F dip;
  POINT pt = { x, y };
  
  d->Factory->GetDesktopDpi(&dpiX, &dpiY);
  dip.x = pt.x * 96.0f / dpiX;
  dip.y = pt.y * 96.0f / dpiY;
  return dip;
}

static void push_mouseargs(lua_State *L, Direct2D *d, MSG *msg) {
  D2D1_POINT_2F point = toDIP(d, GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));

  lua_pushnumber(L, point.x);
  lua_pushnumber(L, point.y);
  lua_createtable(L, 0, 5);
  lua_pushboolean(L, msg->wParam & MK_CONTROL);
  lua_setfield(L, -2, "control");
  lua_pushboolean(L, msg->wParam & MK_SHIFT);
  lua_setfield(L, -2, "shift");
  lua_pushboolean(L, msg->wParam & MK_RBUTTON);
  lua_setfield(L, -2, "right");
  lua_pushboolean(L, msg->wParam & MK_LBUTTON);
  lua_setfield(L, -2, "left");
  lua_pushboolean(L, msg->wParam & MK_MBUTTON);
  lua_setfield(L, -2, "middle"); 
}

int event_onHover(lua_State *L, Widget *w, MSG *msg) {
  push_mouseargs(L, (Direct2D*)w->user, msg);
  lua_throwevent(L, "onHover", 4);
  return 0;
}

int event_onPaint(lua_State *L, Widget *w, MSG *msg) {
  lua_pushtask(L, onPaintContinue, w, NULL);
  return 0;
}

int event_onMouseClick(lua_State *L, Widget *w, MSG *msg) {
  D2D1_POINT_2F point = toDIP((Direct2D*)w->user, GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));
  lua_pushnumber(L, point.x);
  lua_pushnumber(L, point.y);
  lua_throwevent(L, "onClick", 3);
  return 0;
}

int event_onMouseDown(lua_State *L, Widget *w, MSG *msg) {
  push_mouseargs(L, (Direct2D*)w->user, msg);
  lua_throwevent(L, "onMouseDown", 4);
  return 0;
}

int event_onMouseUp(lua_State *L, Widget *w, MSG *msg) {
  push_mouseargs(L, (Direct2D*)w->user, msg);
  lua_throwevent(L, "onMouseDown", 4);
  return 0;
}

OBJECT_MEMBERS(Canvas)
	METHOD(Canvas, flip)
	METHOD(Canvas, begin)
	METHOD(Canvas, print)
	METHOD(Canvas, measure)
	METHOD(Canvas, point)
	METHOD(Canvas, line)
  METHOD(Canvas, rect)
  METHOD(Canvas, fillrect)
  METHOD(Canvas, roundrect)
  METHOD(Canvas, fillroundrect) 
  METHOD(Canvas, ellipse) 
  METHOD(Canvas, fillellipse) 
  METHOD(Canvas, clear)
  METHOD(Canvas, circle) 
  METHOD(Canvas, fillcircle) 
  METHOD(Canvas, rotate)
  METHOD(Canvas, scale)
  METHOD(Canvas, translate)
  METHOD(Canvas, skew)
  METHOD(Canvas, identity)
  METHOD(Canvas, Image)
  METHOD(Canvas, LinearGradient)
  METHOD(Canvas, RadialGradient)
  READWRITE_PROPERTY(Canvas, color)
  READWRITE_PROPERTY(Canvas, bgcolor)
  READWRITE_PROPERTY(Canvas, font)
  READWRITE_PROPERTY(Canvas, fontsize)
  READWRITE_PROPERTY(Canvas, fontstyle)
  READWRITE_PROPERTY(Canvas, fontweight)
  READWRITE_PROPERTY(Canvas, fontstretch)
END

OBJECT_METAFIELDS(Canvas)
  METHOD(Canvas, __gc)
END

extern "C" {
    int __declspec(dllexport) luaopen_canvas(lua_State *L) {
        TImage = lua_registerobject(L, NULL, "Image", Image_constructor, Image_methods, Image_metafields);
	      lua_setfield(L, LUA_REGISTRYINDEX, "Image");
        TLinearGradient = lua_registerobject(L, NULL, "LinearGradient", LinearGradient_constructor, LinearGradient_methods, LinearGradient_metafields);
	      lua_setfield(L, LUA_REGISTRYINDEX, "LinearGradient");
        TRadialGradient = lua_registerobject(L, NULL, "RadialGradient", RadialGradient_constructor, RadialGradient_methods, RadialGradient_metafields);
	      lua_setfield(L, LUA_REGISTRYINDEX, "RadialGradient");
        luaL_require(L, "ui");
        lua_regwidgetmt(L, Canvas, WIDGET_METHODS, FALSE, FALSE, TRUE, FALSE, FALSE);
        luaL_setrawfuncs(L, Canvas_methods);
        on_Hover = lua_registerevent(L, NULL, event_onHover);
        on_Paint = lua_registerevent(L, NULL, event_onPaint);
        on_MouseDown = lua_registerevent(L, NULL, event_onMouseDown);
        on_MouseUp = lua_registerevent(L, NULL, event_onMouseUp);
        on_MouseClick = lua_registerevent(L, NULL, event_onMouseClick);
        return 0;
    }
}