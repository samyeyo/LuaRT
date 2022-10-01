/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | ui.c | LuaRT ui module
*/

#include <luart.h>
#include <Widget.h>
#include <Window.h>
#include <File.h>
#include <Directory.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <richedit.h>
#include <wincodec.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <wincodec.h>

#include "..\resources\resource.h"

static HMODULE richeditlib;

BOOL SaveImg(wchar_t *fname, HBITMAP hBitmap) {
	BITMAP Bitmap;
	IWICStream *pStream = NULL; 
	IWICBitmap *pWICBitmap; 
	IWICBitmapEncoder *pEncoder = NULL;
	IWICBitmapFrameEncode *pBitmapFrame = NULL;
	WICPixelFormatGUID fmt = GUID_WICPixelFormatDontCare;
	BOOL result;

	GetObject(hBitmap,sizeof(BITMAP), &Bitmap);
	if (SUCCEEDED(ui_factory->lpVtbl->CreateStream(ui_factory, &pStream))) {
		if (SUCCEEDED(ui_factory->lpVtbl->CreateBitmapFromHBITMAP(ui_factory, hBitmap, NULL, WICBitmapUseAlpha, &pWICBitmap))) {
			if (SUCCEEDED(pStream->lpVtbl->InitializeFromFilename(pStream, fname, GENERIC_WRITE)))
				if (SUCCEEDED(ui_factory->lpVtbl->CreateEncoder(ui_factory, &GUID_ContainerFormatPng, NULL, &pEncoder))) {
					if (SUCCEEDED(pEncoder->lpVtbl->Initialize(pEncoder, (IStream *)pStream, WICBitmapEncoderNoCache)))
						if (SUCCEEDED(pEncoder->lpVtbl->CreateNewFrame(pEncoder, &pBitmapFrame, NULL)))
							if (SUCCEEDED(pBitmapFrame->lpVtbl->Initialize(pBitmapFrame, NULL))) 
								if (SUCCEEDED(pBitmapFrame->lpVtbl->SetSize(pBitmapFrame,(UINT)Bitmap.bmWidth, (UINT)Bitmap.bmHeight)))
									if (SUCCEEDED(pBitmapFrame->lpVtbl->SetPixelFormat(pBitmapFrame, &fmt)))
										if (SUCCEEDED(pBitmapFrame->lpVtbl->WriteSource(pBitmapFrame, (IWICBitmapSource *)pWICBitmap, NULL)))
											if (SUCCEEDED(pBitmapFrame->lpVtbl->Commit(pBitmapFrame)))
												if (SUCCEEDED(pEncoder->lpVtbl->Commit(pEncoder))) {
													pBitmapFrame->lpVtbl->Release(pBitmapFrame);
													result = TRUE;
												}
					pEncoder->lpVtbl->Release(pEncoder);			
				}
			pWICBitmap->lpVtbl->Release(pWICBitmap);
		}
		pStream->lpVtbl->Release(pStream);		
	}
	free(fname);
	return result;
}

HBITMAP ConvertToBitmap(void *pSource) {
	HBITMAP result = NULL;
    IWICFormatConverter *pConverter = NULL;
	
	if (SUCCEEDED(ui_factory->lpVtbl->CreateFormatConverter(ui_factory, &pConverter)))
	{
		if (SUCCEEDED(pConverter->lpVtbl->Initialize(pConverter, pSource, &GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)))
		{
			UINT x, y;
			if (SUCCEEDED(pConverter->lpVtbl->GetSize(pConverter, &x, &y)))
			{
				BYTE *buff;
				BITMAPINFO bi;

				ZeroMemory(&bi, sizeof(bi));
				bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bi.bmiHeader.biPlanes = 1;
				bi.bmiHeader.biCompression = BI_RGB;
				bi.bmiHeader.biWidth = x;
				bi.bmiHeader.biHeight = -y;
				bi.bmiHeader.biBitCount = 32;
				if ( (result = CreateDIBSection((HDC)GetDC(NULL), &bi, DIB_RGB_COLORS, (void**)&buff, NULL, 0)) )  
				{
					const UINT stridelen = x * sizeof(DWORD);
					const UINT bufflen = y * stridelen;
					if (FAILED(pConverter->lpVtbl->CopyPixels(pConverter, NULL, stridelen, bufflen, buff))) {
						DeleteObject(result);
						result = NULL;
					}
				}
			}
		}
		pConverter->lpVtbl->Release(pConverter);
	}
	return result;
}

HBITMAP LoadImg(wchar_t *filename) {
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
	HBITMAP result = NULL;

	if (SUCCEEDED(ui_factory->lpVtbl->CreateDecoderFromFilename(ui_factory, filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder)))
		{
		if (SUCCEEDED(pDecoder->lpVtbl->GetFrame(pDecoder, 0, &pSource)))
		{	
			result = ConvertToBitmap(pSource);
			pSource->lpVtbl->Release(pSource);
		}
		pDecoder->lpVtbl->Release(pDecoder);
	}
	free(filename);
	return result;
}

#ifdef RTWIN
static HHOOK hMsgBoxHook = NULL;
static HICON hMsgIcon;

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_ACTIVATE && hMsgIcon) {
		SendMessage((HWND)wParam, WM_SETICON, ICON_SMALL, (LPARAM)hMsgIcon);
		return 0;
	}
	return CallNextHookEx(hMsgBoxHook, nCode, wParam, lParam);
}
#endif

static int MsgBox(lua_State *L, UINT options, const wchar_t *def) {
	static const char *values[] = {NULL, "ok", "cancel", NULL, NULL, NULL, "yes", "no"};
	int result;
	HWND h = GetFocus();
	wchar_t *msg = lua_towstring(L, 1);
	wchar_t *title = lua_gettop(L) > 1 ? lua_towstring(L, 2) : NULL;
#ifdef RTWIN
	Widget *w = (Widget*)GetWindowLongPtr(GetForegroundWindow(), GWLP_USERDATA);
	if (w) {
		hMsgBoxHook = SetWindowsHookEx(WH_CBT, CBTProc, NULL, GetCurrentThreadId());
		hMsgIcon = w->icon;
	}
#endif
	result = MessageBoxW(NULL, msg, title ? title : def, options | MB_SYSTEMMODAL);
#ifdef RTWIN
	if (w)
		UnhookWindowsHookEx(hMsgBoxHook);
#endif
	free(title);
	free(msg);
	lua_pushstring(L, values[result]);
	SetFocus(h);
	return 1;
}

LUA_METHOD(ui, msg) {
	return MsgBox(L, 0, L"Message");
}

LUA_METHOD(ui, info) {
	return MsgBox(L, MB_ICONINFORMATION | MB_OK, L"Information");
}

LUA_METHOD(ui, error) {
	return MsgBox(L, MB_ICONERROR | MB_OK, L"Error");
}

LUA_METHOD(ui, warn) {
	return MsgBox(L, MB_ICONWARNING | MB_OK, L"Warning");
}

LUA_METHOD(ui, confirm) {
	return MsgBox(L, MB_ICONQUESTION | MB_YESNOCANCEL, L"Confirmation");
}

static int dialog(lua_State *L, BOOL save, DWORD flags) {
	OPENFILENAMEW ofn = {0};
	int n = lua_gettop(L);
	wchar_t szFile[MAX_PATH];
	wchar_t *filter = NULL;
	BOOL ismultiple = FALSE;
	HWND current = GetFocus();
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);

	if (n) {
		ofn.lpstrTitle = lua_towstring(L, 1);
		if (n > 1) {
			ismultiple = lua_toboolean(L, 2);
			if (n > 2) {
				int i, len;
				luaL_checkstring(L, 3);
				lua_pushvalue(L, 3);
				lua_pushstring(L, "|");
				lua_concat(L, 2);
				filter = lua_tolwstring(L, -1, &len);
				for (i=0; i<len; i++)
					if (filter[i] == '|')
						filter[i] = 0;
				ofn.lpstrFilter = filter;
				ofn.nFilterIndex = 1;
			}	 
		}
	}	
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = ismultiple ? flags|OFN_ALLOWMULTISELECT|OFN_EXPLORER : flags;
	if ( (n = save ? GetSaveFileNameW(&ofn) : GetOpenFileNameW(&ofn)) ) {
		if ((ismultiple) && (szFile[ofn.nFileOffset - 1] == 0))  {
			wchar_t *fname = ofn.lpstrFile+wcslen(ofn.lpstrFile)+1;
			n = 0;
			while (*fname) {
				lua_pushwstring(L, ofn.lpstrFile);
				lua_pushlwstring(L, L"\\", 1);
				lua_pushwstring(L, fname);
				lua_concat(L, 3);
				lua_pushinstance(L, File, 1);	
				lua_remove(L, -2);			
				n++;
				fname += wcslen(fname)+1;
			}	
		} else {
			lua_pushwstring(L, szFile);
			if (ofn.nFilterIndex && !wcschr(szFile, L'.')) {
				long unsigned int i = 1, idx = 0;
				while (i++ < ofn.nFilterIndex*2)
  					idx += wcslen(ofn.lpstrFilter+idx) + 1;
				if (*((ofn.lpstrFilter+idx+2)) != L'*') {
					lua_pushwstring(L, ofn.lpstrFilter+idx+1);
					lua_concat(L, 2);
				}
			}
			lua_pushinstance(L, File, 1);
		}
	} else lua_pushnil(L);
	free(ofn.lpstrFileTitle);
	free(filter);
	SetForegroundWindow(ofn.hwndOwner);
	SetActiveWindow(ofn.hwndOwner);
	SetFocus(current);
	return n;
}

LUA_METHOD(ui, opendialog) {
	return dialog(L, FALSE, OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST);
}

LUA_METHOD(ui, savedialog) {
	return dialog(L, TRUE, OFN_PATHMUSTEXIST);
}

LUA_METHOD(ui, dirdialog) {
	wchar_t path[MAX_PATH];
	LPITEMIDLIST pitems;
	LPMALLOC _malloc;
	BROWSEINFOW bi = { 0 };
    
    bi.hwndOwner  = GetActiveWindow();
    bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpszTitle  = lua_gettop(L) ? lua_towstring(L, 1) : NULL;
    
    if ( (pitems = SHBrowseForFolderW(&bi)) ) {
	    SHGetPathFromIDListW(pitems, path);
	    if ( SUCCEEDED(SHGetMalloc (&_malloc)) ) {
	        _malloc->lpVtbl->Free(_malloc, pitems);
	        _malloc->lpVtbl->Release(_malloc);
	    }
	    lua_pushwstring(L, path);
	    lua_pushinstance(L, Directory, 1);
	} else luaL_pushfail(L);
	free((void*)bi.lParam);
	SetFocus(bi.hwndOwner);
	return 1;
}

LUA_METHOD(ui, remove) {
	Widget *w;
	if (lua_gettop(L) && (lua_type(L, 1) == LUA_TTABLE)) {
		w = lua_self(L, 1, Widget);
		if (w->ref)
			luaL_unref(L, LUA_REGISTRYINDEX, w->ref);
		switch (w->wtype) {
			case UIMenu:	FreeMenu(L, w); break;
			case UIItem:	{
								Widget *parent = (Widget*)GetWindowLongPtr(w->item.itemtype == UITab ? GetParent(w->handle) : w->handle, GWLP_USERDATA);
								free_item(parent, w->index);
							} break;
			case UIMenuItem:remove_menuitem(L, w, -1); break;
			default:		DestroyWindow(w->handle);
		}
		w->handle = NULL;
		w->ref = 0;
	}
	return 0;
}

LUA_METHOD(ui, update) {
	MSG msg;
	ULONGLONG delay = (ULONGLONG)luaL_optinteger(L, 1, 10);
	ULONGLONG start = GetTickCount64();

	while (GetTickCount64()-start < delay) {
peek:	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {	
	    	if ((msg.message >= WM_LUAMIN) && (msg.message < WM_LUAMAX)) {
				Widget *w = NULL;
				int n = lua_gettop(L), nargs;
				if (msg.hwnd) {
					if (!(w = (Widget*)GetWindowLongPtr(msg.hwnd, GWLP_USERDATA)))
						goto do_msg; //--- private control msg
					if (lua_rawgeti(L, LUA_REGISTRYINDEX, w->ref) != LUA_TTABLE)
						continue;						
					else if (lua_getfield(L, -1, events[msg.message - WM_LUAMIN])) {
						lua_insert(L, -2);
						switch (msg.message) {
							case WM_LUADBLCLICK:
							case WM_LUACONTEXT:	if (((w->wtype >= UIList) && (w->wtype <= UITab)) && (msg.wParam > 0))
													__push_item(L, w, msg.wParam-1, w->wtype == UITree ? (HTREEITEM)msg.wParam : NULL);
												break;							
							case WM_LUACLICK:	if (w->wtype == UIWindow || w->wtype == UIMenuItem) {
	push_params:									lua_pushinteger(L, msg.wParam);
													lua_pushinteger(L, msg.lParam);	
												} 
												break;
							case WM_LUAHOVER:	goto push_params;
							case WM_LUACHANGE:	if (w->wtype == UICombo)
													GetText(L, (HANDLE)SendMessage(w->handle, CBEM_GETEDITCONTROL, 0, 0));
												else if (w->wtype == UITree) {
													if (msg.wParam) {
														lua_pushwstring(L, (wchar_t *)msg.lParam);
														free((wchar_t *)msg.lParam);
													}
													else __push_item(L, w, 0, (HTREEITEM)msg.lParam);
													lua_pushstring(L, msg.wParam ? "removed" : "edited");
												} else if (w->wtype == UIEdit)
													SendMessage(w->handle, EM_SETEVENTMASK, 0, ENM_MOUSEEVENTS);
												break;
							case WM_LUASELECT:	if (w->wtype == UIEntry)
													GetText(L, w->handle);
												else if (w->wtype >= UIList && w->wtype <= UITab)
													__push_item(L, w, msg.wParam, (HTREEITEM)msg.lParam);
												else if (w->wtype == UIDate)
													pushDate(L, w->handle);
												else if (w->wtype == UIEdit)
													goto push_params;
												break;
						}
					} else lua_pop(L, 1);					
				} else if (msg.message == WM_LUAMENU) {
					lua_rawgeti(L, LUA_REGISTRYINDEX, msg.wParam);
					if (lua_type(L, -1) == LUA_TTABLE && lua_getfield(L, -1, "onClick")) {
						lua_insert(L, -2);
						if (msg.lParam > -1) {
							lua_pushinteger(L, msg.lParam);
							lua_pushinstance(L, MenuItem, 2);
							lua_remove(L, -2);
						}
					} else lua_pop(L, 1);	
				} else goto do_msg;
				if ((nargs = lua_gettop(L)-n-1) || lua_isfunction(L, -1)) {
					if (lua_pcall(L, nargs, LUA_MULTRET, 0))
						lua_error(L);
					if (msg.message == WM_LUACHANGE && w->wtype == UIEdit)
						SendMessage(w->handle, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | ENM_MOUSEEVENTS);
					else if (msg.message == WM_LUACLOSE) {
						int result = lua_gettop(L);
						if (!result || lua_toboolean(L, -1)) {
							ShowWindow(w->handle, SW_HIDE);
							if (w->wtype == UIWindow && w->tooltip) {
								EnableWindow(w->tooltip, TRUE);
								SetActiveWindow(w->tooltip);
								w->tooltip = NULL;
							}
						}
						lua_pop(L, result);
				 	} 
				}
				goto peek;
			} else {				 
				Widget *wp = (Widget*)GetWindowLongPtr(msg.hwnd, GWLP_USERDATA);
				while(wp && (wp->wtype != UIWindow))
					wp = (Widget*)GetWindowLongPtr(GetParent(wp->handle), GWLP_USERDATA);					
				if (wp) {
					if ((msg.message == WM_KEYDOWN) && (msg.wParam == VK_TAB)) {
						HWND h;
						Widget *w = (Widget*)GetWindowLongPtr(msg.hwnd, GWLP_USERDATA);
						if (w && (w->wtype != UIEdit) && (h = GetNextDlgTabItem(wp->handle, msg.hwnd, GetAsyncKeyState(VK_SHIFT) & 0x8000 ? TRUE : FALSE))) {
							SetFocus(h);
							continue;
						}
					}
					if (TranslateAcceleratorW(wp->handle, wp->accel_table, &msg))
						goto dispatch;
				}
do_msg:			TranslateMessage(&msg);
dispatch:		DispatchMessage(&msg);
			}
		}	
	}
	Sleep(1);
	return 0;
}

LUA_METHOD(ui, mousepos) {
	POINT p;	
	
	GetCursorPos(&p);
	lua_pushinteger(L, p.x);
	lua_pushinteger(L, p.y);
	return 2;
}

LUA_METHOD(ui, colordialog) {
	CHOOSECOLORA cc = {0};
	int result = 0;
	static COLORREF colors[16];

	cc.lStructSize = sizeof(CHOOSECOLORA);
	cc.hwndOwner = GetActiveWindow();
	cc.rgbResult = lua_gettop(L) ? luaL_checkinteger(L, 2) : 0;
	cc.Flags = CC_FULLOPEN;
	cc.lpCustColors = (LPDWORD)colors;
	if (ChooseColorA(&cc)==TRUE) {
		lua_pushinteger(L, cc.rgbResult);
		result = 1;
	}
	SetFocus(cc.hwndOwner);
	return result;
}

LUA_METHOD(ui, fontdialog) {
	CHOOSEFONTW cf = {0};
	int n = lua_gettop(L), idx = 1;

	if (n && (lua_type(L, 1) == LUA_TTABLE)) {
		Widget *w = lua_self(L, 1, Widget);
		if (w->wtype == UIEdit) {
			CHARFORMAT2W chf;
			cf.lpLogFont = calloc(1, sizeof(LOGFONTW));
			format(L, w, CFM_FACE, &chf, SCF_SELECTION, FALSE);
			wcscpy((wchar_t*)&cf.lpLogFont->lfFaceName, (wchar_t*)chf.szFaceName);
			format(L, w, CFM_SIZE, &chf, SCF_SELECTION, FALSE);
			cf.lpLogFont->lfHeight = -(chf.yHeight * GetDeviceCaps(GetDC(NULL), LOGPIXELSY)) / (20*72);
			get_fontstyle(L, w, (LOGFONTW*)cf.lpLogFont);
		} else 	cf.lpLogFont = (LPLOGFONTW)Font(w);
		cf.Flags = CF_INITTOLOGFONTSTRUCT;
		idx++;
	} else cf.lpLogFont = calloc(1, sizeof(LOGFONTW));
	cf.lStructSize = sizeof(CHOOSEFONTW);
	if (n >= idx) {
		int color = lua_tointeger(L, idx);
		cf.rgbColors = RGB((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, (color & 0x0000FF));
	}
	cf.Flags |= CF_SCREENFONTS | CF_EFFECTS | CF_NOSCRIPTSEL;
	cf.hwndOwner = GetActiveWindow();
	lua_pushnil(L);
	if (ChooseFontW(&cf)) {
		lua_pushwstring(L, cf.lpLogFont->lfFaceName);
		lua_pushinteger(L, fontsize_fromheight(cf.lpLogFont->lfHeight));
		fontstyle_createtable(L, cf.lpLogFont);
		lua_pushinteger(L, cf.rgbColors);
		free(cf.lpLogFont);
		SetFocus(cf.hwndOwner);
		return 4;
	}
	return 1;
}

LUA_CONSTRUCTOR(Button) {
	static const luaL_Reg funcs[] = {
		{"get_hastext", Widget_gethastext},
		{"set_hastext", Widget_sethastext},
		{NULL, NULL}
	};
	Widget_create(L, UIButton, 0, WC_BUTTONW, WS_TABSTOP | BS_PUSHBUTTON, TRUE, TRUE);
	luaL_setrawfuncs(L, funcs);
	return 1;
}

LUA_CONSTRUCTOR(Label) {
	Widget *w = Widget_create(L, UILabel, 0, WC_STATICW, SS_NOTIFY | SS_LEFT, TRUE, TRUE);
	SetWindowTheme(w->handle, L"", L"");
	luaL_setrawfuncs(L, color_methods);
	return 1;
}

LUA_CONSTRUCTOR(Picture) {
	Widget *w = Widget_create(L, UIPicture, 0, WC_STATICW, SS_NOTIFY | SS_BITMAP | SS_REALSIZECONTROL | WS_CHILD | WS_VISIBLE, TRUE, TRUE);
	BITMAP bm; 
	w->status = (HANDLE)LoadImg(luaL_checkFilename(L, 3));
	GetObject(w->status, sizeof(BITMAP), &bm);
	SetWindowPos(w->handle, NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
    SendMessage(w->handle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)w->status);
	return 1;
}
LUA_CONSTRUCTOR(Entry) {
	Widget *w = Widget_create(L, UIEntry, WS_EX_CLIENTEDGE, WC_EDITW, WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL, TRUE, TRUE);
	w->cursor = 5;
	w->hcursor = LoadCursor(NULL, IDC_IBEAM);
	return 1;
}

LUA_CONSTRUCTOR(Checkbox) {
	Widget_create(L, UICheck, 0, WC_BUTTONW, WS_TABSTOP | ES_LEFT | BS_AUTOCHECKBOX | BS_FLAT, TRUE, TRUE);
	return 1;
}

LUA_CONSTRUCTOR(Radiobutton) {
	Widget_create(L, UIRadio, 0, WC_BUTTONW, WS_TABSTOP | ES_LEFT | BS_AUTORADIOBUTTON | BS_FLAT, TRUE, TRUE);
	return 1;
}

LUA_CONSTRUCTOR(Groupbox) {
	Widget_create(L, UIGroup, 0, WC_BUTTONW, ES_LEFT | BS_GROUPBOX | BS_FLAT, TRUE, FALSE);
	return 1;
}

LUA_CONSTRUCTOR(Calendar) {
	Widget_create(L, UIDate, 0, MONTHCAL_CLASSW, WS_TABSTOP | WS_CHILD | WS_VISIBLE | MCS_NOTODAYCIRCLE, FALSE, TRUE);
	return 1;
}

LUA_CONSTRUCTOR(Tab) {
	HTHEME t;
	COLORREF color;
	Widget *w = Widget_create(L, UITab, 0, WC_TABCONTROLW, WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, TRUE, TRUE);
	LONG style = GetWindowLongPtr(w->handle, GWL_STYLE ) & ~WS_BORDER;
	SetWindowLongPtr(w->handle, GWL_STYLE, style);
	if ( (t = OpenThemeData(w->handle, L"Tab")) ) {
		if (GetThemeColor(t, TABP_PANE, TILES_SELECTED, TMT_FILLCOLORHINT, &color) == S_OK)
			w->brush = CreateSolidBrush(color);
		CloseThemeData(t);
	}
	return 1;
}

LUA_CONSTRUCTOR(Listbox) {
	Widget *w = Widget_create(L, UIList, WS_EX_CLIENTEDGE, WC_LISTVIEWW, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_NOCOLUMNHEADER | LVS_REPORT | LVS_SHOWSELALWAYS | WS_TABSTOP | LVS_SINGLESEL, TRUE, FALSE);
	HIMAGELIST imglist = ImageList_Create(1, 1, ILC_COLOR32|ILC_MASK, ImageList_GetImageCount(w->imglist), 1);
	ListView_SetImageList(w->handle, imglist, LVSIL_SMALL);
	lua_pushcfunction(L, Listbox_sort);
	lua_setfield(L, 1, "sort");
	return 1;
}

LUA_CONSTRUCTOR(Tree) {
	Widget_create(L, UITree, WS_EX_CLIENTEDGE, WC_TREEVIEWW, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_TABSTOP | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_EDITLABELS, TRUE, FALSE);
	lua_pushcfunction(L, Item_sort);
	lua_setfield(L, 1, "sort");
	return 1;
}

LUA_CONSTRUCTOR(Combobox) {
	int caption = 2;
	DWORD style;
	if (lua_istable(L, 3)) {
		caption = 1;
		style = CBS_DROPDOWN;
	} else style = lua_toboolean(L, 3) ? CBS_DROPDOWN : CBS_DROPDOWNLIST;
	Widget *w = Widget_create(L, UICombo, 0, WC_COMBOBOXEXW, WS_TABSTOP | style | WS_CHILD | WS_VISIBLE, caption, FALSE);
	SendMessage(w->handle, CBEM_SETIMAGELIST, 0, (LPARAM)w->imglist);
	SendMessage(w->handle, CBEM_SETEXTENDEDSTYLE, CBES_EX_NOEDITIMAGEINDENT, CBES_EX_NOEDITIMAGEINDENT);
	SendMessage(w->handle, CBEM_SETUNICODEFORMAT, 1, 0);
	SendMessageW(w->handle, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET,UISF_HIDEFOCUS), 0);
	w->status = (HANDLE)SendMessageW(w->handle, CBEM_GETCOMBOCONTROL, 0, 0);
	return 1;
}

LUA_CONSTRUCTOR(Edit) {
	Widget *w = Widget_create(L, UIEdit, WS_EX_STATICEDGE, RICHEDIT_CLASSW, WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, TRUE, FALSE);
	SendMessage(w->handle, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | ENM_MOUSEEVENTS);
	SendMessage(w->handle, EM_EXLIMITTEXT, 0, 0x7FFFFFF0);
	return 1;
}

IWICImagingFactory *ui_factory;

LUALIB_API int ui_finalize(lua_State *L) {
	ui_factory->lpVtbl->Release(ui_factory);
	FreeLibrary(richeditlib);
	return 0;
}

static const luaL_Reg ui_properties[] = {
	{NULL, NULL}
};

static const luaL_Reg uilib[] = {
	{"msg",				ui_msg},
	{"info",			ui_info},
	{"warn",			ui_warn},
	{"confirm",			ui_confirm},
	{"opendialog",		ui_opendialog},
	{"dirdialog",		ui_dirdialog},
	{"savedialog",		ui_savedialog},
	{"fontdialog",		ui_fontdialog},
	{"colordialog",		ui_colordialog},
	{"mousepos",		ui_mousepos},
	{"error",			ui_error},
	{"update",			ui_update},
	{"remove",			ui_remove},
	{NULL, NULL}
};

/* ------------------------------------------------------------------------ */

LUAMOD_API int luaopen_ui(lua_State *L)
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "Window";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(WINICON));
	RegisterClassEx(&wcex);
	lua_regmodulefinalize(L, ui); 
	widget_type_new(L, UIWindow, Window_constructor, Window_methods, NULL, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE);
	widget_type_new(L, UIButton, Button_constructor, NULL, NULL, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE);
	widget_type_new(L, UIGroup, Groupbox_constructor, NULL, NULL, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	widget_type_new(L, UICheck, Checkbox_constructor, Checkbox_methods, NULL, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
	widget_type_new(L, UIRadio, Radiobutton_constructor, Checkbox_methods, NULL, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
	widget_type_new(L, UILabel, Label_constructor, NULL, NULL, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
	widget_type_new(L, UIEntry, Entry_constructor, Entry_methods, NULL, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE);
	widget_type_new(L, UIPicture, Picture_constructor, Picture_methods, NULL, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE);
	widget_type_new(L, UIDate, Calendar_constructor, Calendar_methods, NULL, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE);
	widget_noinherit(L, "Menu", Menu_constructor, Menu_methods, Menu_metafields);
	lua_registerobject(L, NULL, "MenuItem", MenuItem_constructor, MenuItem_methods, MenuItem_metafields);
	lua_setfield(L, LUA_REGISTRYINDEX, "MenuItem");
	widget_type_new(L, UITab, Tab_constructor, ItemWidget_methods, NULL, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	lua_registerobject(L, NULL, "TabItem", Item_constructor, Item_methods, Item_metafields);
	luaL_setrawfuncs(L, Page_methods);
	lua_setfield(L, LUA_REGISTRYINDEX, "TabItem");
	widget_type_new(L, UICombo, Combobox_constructor, ItemWidget_methods, NULL, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	lua_registerobject(L, NULL, "ComboItem", Item_constructor, Item_methods, Item_metafields);
	lua_setfield(L, LUA_REGISTRYINDEX, "ComboItem");
	widget_type_new(L, UIList, Listbox_constructor, ItemWidget_methods, NULL, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	lua_registerobject(L, NULL, "ListItem", Item_constructor, Item_methods, Item_metafields);
	lua_setfield(L, LUA_REGISTRYINDEX, "ListItem");
	widget_type_new(L, UITree, Tree_constructor, ItemWidget_methods, NULL, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	lua_registerobject(L, NULL, "TreeItem", Item_constructor, TreeItem_methods, Item_metafields);
	lua_setfield(L, LUA_REGISTRYINDEX, "TreeItem");
	if ( (richeditlib = LoadLibraryA("Riched20.dll")) )
		widget_type_new(L, UIEdit, Edit_constructor, Edit_methods, NULL, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
	if (FAILED(CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, (LPVOID*)&ui_factory)))
        luaL_error(L, "Failed to open 'ui' module : WIC Imaging Factory could not be created");
	return 1;
}