 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Image.cpp | LuaRT Canvas Image object
*/


#include <luart.h>
#include <File.h>
#include <Widget.h>

#ifdef _MSC_VER
#include <d2d1.h>
#else
#include "d2d1.h"
#endif
#include "Direct2D.h"
#include "Image.h"
#include "Canvas.h"

luart_type TImage;

//-------------------------------------[ Image Constructor ]
LUA_CONSTRUCTOR(Image) {
	Image *img = (Image *)calloc(1, sizeof(Image));
    int type = lua_type(L, 3);
    img->d = (Direct2D *)lua_touserdata(L, 2);
    wchar_t *fname = luaL_checkFilename(L, 3);
    D2D1_BITMAP_PROPERTIES BitmapProperties = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    
    img->d->WICFactory->CreateDecoderFromFilename(fname, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &img->IWICDecoder);
    if (!img->IWICDecoder)
        luaL_error(L, "Failed to load Image from file");
    img->IWICDecoder->GetFrame(0, &img->IWICFrame);
    img->d->WICFactory->CreateFormatConverter(&img->IWICConverter);
    img->IWICConverter->Initialize(img->IWICFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);      
    if (FAILED(img->d->DCRender->CreateBitmapFromWicBitmap(img->IWICConverter, nullptr, &img->Bitmap)))
        luaL_error(L, "Failed to create Image object from Bitmap");
    img->d->images.push_back(img);
	lua_newinstance(L, img, Image);
	return 1;
}

static const char *interpolation[] = { "nearest", "linear", NULL};

LUA_METHOD(Image, draw) {
    Image *img = lua_self(L, 1, Image);
    double dpi;
    lua_uigetinfo(&dpi, NULL);
    const D2D1_SIZE_U size = img->Bitmap->GetPixelSize();
    float x = static_cast<FLOAT>(luaL_checknumber(L, 2));
    float y = static_cast<FLOAT>(luaL_checknumber(L, 3));
    float ratio = static_cast<FLOAT>(luaL_optnumber(L, 4, 1.0f));
    
    img->d->DCRender->DrawBitmap(img->Bitmap, D2D1::RectF(x, y, x+size.width*ratio, y+size.height*ratio), luaL_optnumber(L, 5, 1.0f), (D2D1_BITMAP_INTERPOLATION_MODE)luaL_checkoption(L, 6, "nearest", interpolation), NULL);
    return 0;
}

LUA_METHOD(Image, drawrect) {
    Image *img = lua_self(L, 1, Image);
   
    img->d->DCRender->DrawBitmap(img->Bitmap, D2D1::RectF(static_cast<FLOAT>(luaL_checknumber(L, 2)), static_cast<FLOAT>(luaL_checknumber(L, 3)), static_cast<FLOAT>(luaL_checknumber(L, 4)), static_cast<FLOAT>(luaL_checknumber(L, 5))), luaL_optnumber(L, 6, 1.0f), (D2D1_BITMAP_INTERPOLATION_MODE)luaL_checkoption(L, 7, "nearest", interpolation), NULL);
    return 0;
}

LUA_PROPERTY_GET(Image, width) {
    lua_pushinteger(L, lua_self(L, 1, Image)->Bitmap->GetPixelSize().width);
    return 1;
}

LUA_PROPERTY_GET(Image, height) {
    lua_pushinteger(L, lua_self(L, 1, Image)->Bitmap->GetPixelSize().height);
    return 1;
}

OBJECT_MEMBERS(Image)
    READONLY_PROPERTY(Image, width)
    READONLY_PROPERTY(Image, height)
    METHOD(Image, draw)
    METHOD(Image, drawrect)
END

LUA_METHOD(Image, __gc) {
    Image *img = lua_self(L, 1, Image);
    img->Bitmap->Release();
    img->IWICDecoder->Release();
    img->IWICFrame->Release();
    img->IWICConverter->Release();
    img->d->images.erase(std::remove(img->d->images.begin(), img->d->images.end(), img), img->d->images.end());
    free(img);
    return 0;
}

OBJECT_METAFIELDS(Image)
    METHOD(Image, __gc)
END