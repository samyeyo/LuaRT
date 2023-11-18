/*
 | Canvas for LuaRT
 | Luart.org, Copyright (c) Tine Samir 2023.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Canvas.cpp | LuaRT binary module
*/

#include <luart.h>
#include <Widget.h>
#include <windows.h>

#include "Direct2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Gradient.h"

//--- Canvas type
luart_type TCanvas;
UINT onPaint, onMouseRelease;

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
      RECT r;
      ID2D1Bitmap *bitmap;

      GetClientRect(hwnd, &r);
      BeginPaint(hwnd, &ps);
      d->DCRender->BindDC(ps.hdc, &r);
      d->Render->GetBitmap(&bitmap);
      d->DCRender->BeginDraw();
      d->DCRender->DrawBitmap(bitmap);
      d->DCRender->EndDraw();
      bitmap->Release();
      EndPaint(hwnd, &ps);      
      return 0;
    }	
    case WM_LBUTTONUP:
      PostMessage(hwnd, onMouseRelease, 0, 0);
      break;
  case WM_SIZE:
      {
        RECT r = {0, 0, LOWORD(lParam), HIWORD(lParam)};
        ID2D1Bitmap *oldbitmap;
        
        d->Render->GetBitmap(&oldbitmap);
        d->Render->Release();
        d->DCRender->BindDC(GetDC(hwnd), &r);
        d->DCRender->CreateCompatibleRenderTarget(&d->Render);
        d->Render->BeginDraw();
        d->Render->Clear(d->bgcolor);
        d->Render->DrawBitmap(oldbitmap);
        d->Render->EndDraw();
        oldbitmap->Release();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
      }
    case WM_TIMER:
      PostMessage(hwnd, onPaint, 0, 0);
      return 0;
  }

  return lua_widgetproc(hwnd, uMsg, wParam, lParam, 0, 0);
}

//------------------------------------ Canvas constructor
LUA_CONSTRUCTOR(Canvas)
{   
    Widget *w, *wp;
    HWND h, hParent = (HWND)lua_widgetinitialize(L, &wp);     
	int width = (int)luaL_optinteger(L, 5, 320), height = (int)luaL_optinteger(L, 6, 240);
    h = CreateWindowExW(0, L"Window", NULL, WS_VISIBLE | WS_CHILD, (int)luaL_optinteger(L, 3, 0), (int)luaL_optinteger(L, 4, 0), width, height, hParent, 0, GetModuleHandle(NULL), NULL);    
    w = lua_widgetconstructor(L, h, TCanvas, wp, (SUBCLASSPROC)CanvasProc);
	w->user = new Direct2D(h, width, height);
	if (((Direct2D *)w->user)->error)
		luaL_error(L, ((Direct2D *)w->user)->error); 
  ((Direct2D *)w->user)->Render->BeginDraw();
  ((Direct2D *)w->user)->Render->Clear(((Direct2D *)w->user)->bgcolor);
  ((Direct2D *)w->user)->Render->EndDraw();    
  InvalidateRect(h, NULL, TRUE);
	SetTimer(h, 1001, 30, NULL);
  return 1;
}

LUA_METHOD(Canvas, __gc) {
  Widget *w = lua_widgetdestructor(L);
  delete (Direct2D*)w->user;
  free(w);
  return 0;
}

static ID2D1Brush *getBrush(lua_State *L, Direct2D *d, int idx) {
  ID2D1Brush *brush = NULL;
  if (lua_gettop(L) >= idx)
    switch(lua_type(L, idx)) {
      case LUA_TNUMBER: {
                          lua_Integer color = luaL_checkinteger(L, idx);
                          d->Render->CreateSolidColorBrush(D2D1::ColorF(GetR(color)/255, GetG(color)/255, GetB(color)/255, GetA(color)/255), reinterpret_cast<ID2D1SolidColorBrush **>(&brush));
                        } break;
      case LUA_TTABLE:  {
                          luart_type t;
                          void *ptr = lua_tocinstance(L, idx, &t);                          
                          if (ptr) {
                            if (t == TImage)
                              d->Render->CreateBitmapBrush(((Image*)ptr)->Bitmap, D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), reinterpret_cast<ID2D1BitmapBrush **>(&brush));
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

LUA_METHOD(Canvas, map) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  size_t len;
  const char *str = luaL_checklstring(L, 2, &len);
  ID2D1Bitmap *bmp;
  D2D1_SIZE_F size;
  D2D1_RECT_U rect;

  d->Render->GetBitmap(&bmp);
  size = bmp->GetSize();
  rect = D2D1::RectU(0, 0, size.width, size.height);
  if (len < (size.width * size.height * 4) || FAILED(bmp->CopyFromMemory(NULL, str, size.width * 4)))
    len = 0;
  lua_pushboolean(L, len);
  bmp->Release();
	return 1;
}

LUA_METHOD(Canvas, point) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 4);
  float x = static_cast<FLOAT>(luaL_checknumber(L, 2));
  float y = static_cast<FLOAT>(luaL_checknumber(L, 3));
	d->Render->DrawRectangle(D2D1::RectF(x, y, x+0.5f, y+0.5f), brush ? brush : d->colorBrush, 0.5f, NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, line) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 6);
	d->Render->DrawLine(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f), NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, rect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 6);
	d->Render->DrawRectangle(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f), NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 6);
	d->Render->FillRectangle(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, roundrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 8);
	d->Render->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)),static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), static_cast<FLOAT>(luaL_checknumber(L, 6)), static_cast<FLOAT>(luaL_checknumber(L, 7))), brush ? brush : d->colorBrush, luaL_optnumber(L, 9, 1.0f), NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillroundrect) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 8);
	d->Render->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)),static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), static_cast<FLOAT>(luaL_checknumber(L, 6)), static_cast<FLOAT>(luaL_checknumber(L, 7))), brush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, ellipse) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 6);

	d->Render->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush, luaL_optnumber(L, 7, 1.0f), NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillellipse) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 6);

	d->Render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, circle) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  lua_Number radius = luaL_checknumber(L, 4);
  ID2D1Brush *brush = getBrush(L, d, 5);
  
	d->Render->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), brush ? brush : d->colorBrush, luaL_optnumber(L, 6, 1.0f), NULL);
  if (brush)
    brush->Release();
	return 0;
}

LUA_METHOD(Canvas, fillcircle) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  ID2D1Brush *brush = getBrush(L, d, 5);

	d->Render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3))), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 4))), brush ? brush : d->colorBrush);
  if (brush)
    brush->Release();
	return 0;
}

static IDWriteTextLayout *prepare_text(lua_State *L, Direct2D **d) {
  Widget *w = lua_self(L, 1, Widget);
	*d = (Direct2D*)w->user;
  int len;
  wchar_t *str = lua_tolwstring(L, 2, &len);
  IDWriteTextLayout* layout = NULL;
	FLOAT dpix, dpiy;
	RECT re;

	GetClientRect((HWND)w->handle, &re);
	dpix = static_cast<float>(re.right - re.left);
	dpiy = static_cast<float>(re.bottom - re.top);
	(*d)->DWriteFactory->CreateTextLayout(str, len+1, (*d)->textFormat, dpix, dpiy, &layout);
  free(str);
  return layout;
}

LUA_METHOD(Canvas, print) {
  Direct2D *d;
  IDWriteTextLayout* layout = prepare_text(L, &d);

	if (layout) {
    ID2D1Brush *brush = getBrush(L, d, 5);
		d->Render->DrawTextLayout(D2D1::Point2F(static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4))), layout, brush ? brush : d->colorBrush);
		layout->Release();
    if (brush)
      brush->Release();    
  }
  return 0;
}

LUA_METHOD(Canvas, measure) {
  Direct2D *d;
  IDWriteTextLayout* layout = prepare_text(L, &d);
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

LUA_METHOD(Canvas, flip) {
  Widget *w = lua_self(L, 1, Widget);
	Direct2D *d = (Direct2D*)w->user;
  d->Render->EndDraw();
  InvalidateRect((HWND)w->handle, NULL, TRUE);
  d->transform = D2D1::Matrix3x2F::Identity();
  d->Render->SetTransform(d->transform);  
	return 0;
}

LUA_METHOD(Canvas, rotate) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;

  if (lua_gettop(L) == 2) {
    const D2D1_SIZE_F& size = d->Render->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(luaL_checknumber(L, 3), luaL_checknumber(L, 4));
  d->transform = d->transform * D2D1::Matrix3x2F::Rotation(luaL_checknumber(L, 2), center);
  d->Render->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, scale) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;

  if (lua_gettop(L) == 3) {
    const D2D1_SIZE_F& size = d->Render->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  d->transform = d->transform * D2D1::Matrix3x2F::Scale(D2D1::SizeF(luaL_checknumber(L, 2), luaL_checknumber(L, 3)), center);
  d->Render->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, translate) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);

  d->transform = d->transform * D2D1::Matrix3x2F::Translation(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  d->Render->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, skew) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  D2D1_POINT_2F center;

  if (lua_gettop(L) == 3) {
    const D2D1_SIZE_F& size = d->Render->GetSize();
    center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
  } else center = D2D1::Point2F(luaL_checknumber(L, 4), luaL_checknumber(L, 5));
  d->transform = d->transform * D2D1::Matrix3x2F::Skew(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  d->Render->SetTransform(d->transform);
	return 0;
}

LUA_METHOD(Canvas, identity) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget)->user);
  d->transform = D2D1::Matrix3x2F::Identity();
  d->Render->SetTransform(d->transform);
	return 0;  
}

LUA_METHOD(Canvas, begin) {
	((Direct2D*)(lua_self(L, 1, Widget))->user)->Render->BeginDraw();
	return 0;
}

LUA_METHOD(Canvas, clear) {
	Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  D2D1_COLOR_F color = d->bgcolor;

  if (lua_gettop(L) > 1) {
    lua_Integer rgba = luaL_checkinteger(L, 2);
    color = D2D1::ColorF(GetR(rgba)/255, GetG(rgba)/255, GetB(rgba)/255, GetA(rgba)/255);
  }
	d->Render->Clear(color);
	return 0;
}

static int new_instance(lua_State *L, const char *type) {
	int i, n = lua_gettop(L);
  lua_pushlightuserdata(L,(lua_self(L, 1, Widget))->user);
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
  d->Render->CreateSolidColorBrush(d->color, reinterpret_cast<ID2D1SolidColorBrush **>(&d->colorBrush));
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
  d->Render->BeginDraw();
  d->Render->Clear(d->bgcolor);
  d->Render->EndDraw();    
  InvalidateRect((HWND)w->handle, NULL, TRUE);
  return 0;
}

LUA_PROPERTY_GET(Canvas, sync) {
  Direct2D *d = (Direct2D*)(lua_self(L, 1, Widget))->user;
  lua_pushboolean(L, d->synced);
	return 1;
}

LUA_PROPERTY_SET(Canvas, sync) {
  Widget *w = lua_self(L, 1, Widget);
  Direct2D *d = (Direct2D*)w->user;
  BOOL flag = lua_toboolean(L, 2);
  
  if (flag != d->synced) {
    if (d->synced)
      KillTimer((HWND)w->handle, 1001);
    else  
      SetTimer((HWND)w->handle, 1001, 30, NULL);
    d->synced = flag;
  }
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
  lua_pushinteger(L, ((Direct2D*)(lua_self(L, 1, Widget))->user)->textFormat->GetFontStyle());
	return 1;
}

int event_onPaint(lua_State *L) {
  Widget *w = lua_self(L, 1, Widget);
  Direct2D *d = (Direct2D*)w->user;
  if (lua_getfield(L, 1, "onPaint") == LUA_TFUNCTION) {
    d->Render->BeginDraw();
	  lua_pushvalue(L, 1);
    lua_call(L, 1, 0);
    d->Render->EndDraw();
    InvalidateRect((HWND)w->handle, NULL, TRUE);
    d->transform = D2D1::Matrix3x2F::Identity();
    d->Render->SetTransform(d->transform);    
  }
  else lua_pop(L, 1);
  lua_pushnil(L);
  return 1;
}

int event_onRelease(lua_State *L) {
  lua_getfield(L, 1, "onRelease");
  return 1;
}

OBJECT_MEMBERS(Canvas)
	METHOD(Canvas, print)
	METHOD(Canvas, measure)
	METHOD(Canvas, map)
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
  METHOD(Canvas, flip)
  METHOD(Canvas, begin)
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
  READWRITE_PROPERTY(Canvas, sync)
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
        onPaint = lua_registerevent(L, NULL, event_onPaint);
        onMouseRelease = lua_registerevent(L, NULL, event_onRelease);
        return 0;
    }
}
