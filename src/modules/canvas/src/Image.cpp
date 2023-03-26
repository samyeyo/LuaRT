 /*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Image.cpp | LuaRT Canvas Image object
*/

extern "C" {
    #include <luart.h>
    #include <File.h>
}
#include "d2d1.h"
#include "Image.h"
#include "Direct2D.h"

luart_type TImage;

//-------------------------------------[ Image Constructor ]
LUA_CONSTRUCTOR(Image) {
	Image *img = (Image *)calloc(1, sizeof(Image));
    int type = lua_type(L, 3);
    img->d = (Direct2D *)lua_touserdata(L, 2);

    if (type == LUA_TNUMBER) {
        if (FAILED(img->d->Render->CreateBitmap(D2D1::SizeU(luaL_checknumber(L, 3), luaL_checknumber(L, 4)), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED)), &img->Bitmap)))
            luaL_error(L, "Failed to create Image object");
    } else {
        wchar_t *fname = luaL_checkFilename(L, 3);
        IWICBitmapDecoder	  *IWICDecoder	 = nullptr;
        IWICBitmapFrameDecode *IWICFrame	 = nullptr;
        IWICFormatConverter	  *IWICConverter = nullptr;

        if (SUCCEEDED(CoCreateInstance(CLSID_WICBmpDecoder, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&IWICDecoder))))
                if (SUCCEEDED(img->d->WICFactory->CreateDecoderFromFilename(fname, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &IWICDecoder)))
                    if (SUCCEEDED(IWICDecoder->GetFrame(0, &IWICFrame)))
                        if (SUCCEEDED(img->d->WICFactory->CreateFormatConverter(&IWICConverter)))
                            if (SUCCEEDED(IWICConverter->Initialize(IWICFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom)));
                                goto done;
        luaL_error(L, "Failed to load Direct2D bitmap from file");
    done:            
        D2D1_BITMAP_PROPERTIES BitmapProperties = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        if (FAILED(img->d->Render->CreateBitmapFromWicBitmap(IWICConverter, nullptr, &img->Bitmap)))
            luaL_error(L, "Failed to create Image object from Bitmap");

        IWICDecoder->Release();
        IWICFrame->Release();
        IWICConverter->Release();
    }
	lua_newinstance(L, img, Image);
	return 1;
}

LUA_METHOD(Image, draw) {
    Image *img = lua_self(L, 1, Image);
    const D2D1_SIZE_U size = img->Bitmap->GetPixelSize();
    float x = static_cast<FLOAT>(luaL_checknumber(L, 2));
    float y = static_cast<FLOAT>(luaL_checknumber(L, 3));
    float ratio = static_cast<FLOAT>(luaL_optnumber(L, 5, 1.0f));
    
    img->d->Render->DrawBitmap(img->Bitmap, D2D1::RectF(x, y, x+size.width*ratio, y+size.height*ratio), luaL_optnumber(L, 4, 1.0f), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
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
END

LUA_METHOD(Image, __gc) {
    Image *img = lua_self(L, 1, Image);
    img->Bitmap->Release();
    free(img);
    return 0;
}

OBJECT_METAFIELDS(Image)
    METHOD(Image, __gc)
END
