#define WIDGET_IMPLEMENTATION
#include <Widget.h>
#include "DarkMode.h"
#include "IatHook.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <dwmapi.h>
#include <map>
#include <string>
#include <Richedit.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")

extern "C" {
	BOOL g_darkModeSupported = FALSE;
	BOOL g_darkModeEnabled = FALSE;
	DWORD g_buildNumber = 0;
	BOOL DarkMode = FALSE;	
}

using fnRtlGetNtVersionNumbers = void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
using fnSetWindowCompositionAttribute = BOOL (WINAPI *)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);
// 1809 17763
using fnShouldAppsUseDarkMode = BOOL (WINAPI *)(); // ordinal 132
using fnAllowDarkModeForWindow = BOOL (WINAPI *)(HWND hWnd, BOOL allow); // ordinal 133
using fnAllowDarkModeForApp = BOOL (WINAPI *)(BOOL allow); // ordinal 135, in 1809
using fnFlushMenuThemes = void (WINAPI *)(); // ordinal 136
using fnRefreshImmersiveColorPolicyState = void (WINAPI *)(); // ordinal 104
using fnIsDarkModeAllowedForWindow = BOOL (WINAPI *)(HWND hWnd); // ordinal 137
using fnGetIsImmersiveColorUsingHighContrast = BOOL (WINAPI *)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
using fnOpenNcThemeData = HTHEME(WINAPI *)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
// 1903 18362
using fnShouldSystemUseDarkMode = BOOL (WINAPI *)(); // ordinal 138
using fnSetPreferredAppMode = PreferredAppMode (WINAPI *)(PreferredAppMode appMode); // ordinal 135, in 1903
using fnIsDarkModeAllowedForApp = BOOL (WINAPI *)(); // ordinal 139

fnSetWindowCompositionAttribute _SetWindowCompositionAttribute = NULL;
fnShouldAppsUseDarkMode _ShouldAppsUseDarkMode = NULL;
fnAllowDarkModeForWindow _AllowDarkModeForWindow = NULL;
fnAllowDarkModeForApp _AllowDarkModeForApp = NULL;
fnFlushMenuThemes _FlushMenuThemes = NULL;
fnRefreshImmersiveColorPolicyState _RefreshImmersiveColorPolicyState = NULL;
fnIsDarkModeAllowedForWindow _IsDarkModeAllowedForWindow = NULL;
fnGetIsImmersiveColorUsingHighContrast _GetIsImmersiveColorUsingHighContrast = NULL;
fnOpenNcThemeData _OpenNcThemeData = NULL;
fnShouldSystemUseDarkMode _ShouldSystemUseDarkMode = NULL;
fnSetPreferredAppMode _SetPreferredAppMode = NULL;

extern "C" {

	double GetDPIForSystem() {
		double scMon = 1.0;
		HWND activeWindow = GetActiveWindow();
		HMONITOR monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);
		UINT x, y;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &x, &y)) && (x > 0) && (y > 0))
  			return x/96.0;
		return 1;
	}

	BOOL IsDarkModeEnabled() {
		return g_darkModeSupported && _ShouldSystemUseDarkMode() && !IsHighContrast();
	}

	BOOL ShouldAppUseDarkMode() {
		return _ShouldAppsUseDarkMode();
	}

	BOOL AllowDarkModeForWindow(HWND hWnd, BOOL allow) {
		if (g_darkModeSupported)
			return _AllowDarkModeForWindow(hWnd, allow);
		return FALSE;
	}

	WidgetType GetWidgetTypeFromHWND(HWND h) {
		wchar_t classname[64];
		WidgetType wtype = -1;
		static std::map<std::wstring, int> WidgetsClass = {
			{L"Window", UIWindow},
			{WC_BUTTONW, UIButton},
			{WC_STATICW, UILabel},
			{PROGRESS_CLASSW, UIProgressbar},
			{WC_EDITW, UIEntry},
			{MONTHCAL_CLASSW, UIDate},
			{WC_TABCONTROLW, UITab},
			{WC_TREEVIEWW, UITree},
			{WC_COMBOBOXEXW, UICombo},
			{RICHEDIT_CLASSW, UIEdit}
		};

		if (GetClassNameW(h, classname, 64)) {
			auto it = WidgetsClass.find(classname);
			if (it != WidgetsClass.end()) {
				wtype = it->second;
				if (wtype == UIButton) {
					switch(GetWindowLongPtr(h, GWL_STYLE) & BS_TYPEMASK) {
						case BS_AUTORADIOBUTTON: 	return UIRadio;
						case BS_AUTOCHECKBOX:		return UICheck;
						case BS_GROUPBOX:			return UIGroup;
					}
				}
			}
		}
		return wtype;
	}

	void FlushMenuThemes(void) {
		_FlushMenuThemes();
	}

	BOOL IsHighContrast()
	{
		HIGHCONTRASTW highContrast = { sizeof(highContrast) };
		if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
			return highContrast.dwFlags & HCF_HIGHCONTRASTON;
		return FALSE;
	}

	void RefreshTitleBarThemeColor(HWND hWnd, BOOL dark)
	{
		BOOL isvisible = IsWindowVisible(hWnd);
		DwmSetWindowAttribute(hWnd, (int)(g_buildNumber > 18985 ? DWMWA_USE_IMMERSIVE_DARK_MODE : DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1), &dark, sizeof(int));					
		SetWindowPos(hWnd, 0, 0, 0, 0, 0,  SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
		if (isvisible)
			SetWindowPos(hWnd, 0, 0, 0, 0, 0,  SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    }	

	BOOL IsColorSchemeChangeMessageLParam(LPARAM lParam)
	{
		BOOL is = FALSE;
		if (lParam && CompareStringOrdinal(reinterpret_cast<LPCWCH>(lParam), -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
		{
			_RefreshImmersiveColorPolicyState();
			is = TRUE;
		}
		_GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
		return is;
	}

	BOOL IsColorSchemeChangeMessage(UINT message, LPARAM lParam)
	{
		if (message == WM_SETTINGCHANGE)
			return IsColorSchemeChangeMessageLParam(lParam);
		return FALSE;
	}

	void AllowDarkModeForApp(BOOL allow)
	{
		if (_AllowDarkModeForApp)
			_AllowDarkModeForApp(allow);
		else if (_SetPreferredAppMode)
			_SetPreferredAppMode(allow ? ForceDark : ForceLight);
	}

	static BOOL isdarkScrollBar;

	void FixDarkScrollBar(BOOL theme) {
		isdarkScrollBar = theme;
		HMODULE hComctl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hComctl)
		{
			auto addr = FindDelayLoadThunkInModule(hComctl, "uxtheme.dll", 49); // OpenNcThemeData
			if (addr)
			{
				DWORD oldProtect;
				if (VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), PAGE_READWRITE, &oldProtect))
				{
					auto MyOpenThemeData = [](HWND hWnd, LPCWSTR classList) -> HTHEME {
						if (wcscmp(classList, L"ScrollBar") == 0)
						{
							hWnd = NULL;
							classList = isdarkScrollBar ? L"DarkMode_Explorer::ScrollBar" : L"Explorer::ScrollBar";
						}
						return _OpenNcThemeData(hWnd, classList);
					};

					addr->u1.Function = reinterpret_cast<ULONG_PTR>(static_cast<fnOpenNcThemeData>(MyOpenThemeData));
					VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), oldProtect, &oldProtect);
				}
			}
		}
	}
	
	void InitDarkMode()
	{
		auto RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
		if (RtlGetNtVersionNumbers)
		{
			DWORD major, minor;
			RtlGetNtVersionNumbers(&major, &minor, &g_buildNumber);
			g_buildNumber &= ~0xF0000000;
			if (major == 10 && minor == 0 && g_buildNumber >= 17763)
			{
				HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
				if (hUxtheme)
				{
					_OpenNcThemeData = reinterpret_cast<fnOpenNcThemeData>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(49)));
					_RefreshImmersiveColorPolicyState = reinterpret_cast<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
					_GetIsImmersiveColorUsingHighContrast = reinterpret_cast<fnGetIsImmersiveColorUsingHighContrast>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106)));
					_ShouldAppsUseDarkMode = reinterpret_cast<fnShouldAppsUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)));
					_AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
					_ShouldSystemUseDarkMode = reinterpret_cast<fnShouldSystemUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(138)));
					auto ord135 = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
					if (g_buildNumber < 18362)
						_AllowDarkModeForApp = reinterpret_cast<fnAllowDarkModeForApp>(ord135);
					else
						_SetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(ord135);

					_FlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
					_IsDarkModeAllowedForWindow = reinterpret_cast<fnIsDarkModeAllowedForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(137)));

					_SetWindowCompositionAttribute = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

					if (_OpenNcThemeData &&
						_RefreshImmersiveColorPolicyState &&
						_ShouldAppsUseDarkMode &&
						_AllowDarkModeForWindow &&
						(_AllowDarkModeForApp || _SetPreferredAppMode) &&
						_IsDarkModeAllowedForWindow)
					{
						g_darkModeSupported = TRUE;
						_RefreshImmersiveColorPolicyState();
				
						DarkMode = IsDarkModeEnabled();
						isdarkScrollBar = DarkMode;
					}
				}
			}
		}
	}

	LRESULT MenuBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, HBRUSH brush) {
		HTHEME theme = NULL;

		if (!DarkMode)
			return 0;
		switch (msg)
		{
			case WM_MENUBAR_DRAWMENU: {
				MenubarMenu* pMbm =(MenubarMenu*)lParam;
				RECT rc = { 0 };
				MENUBARINFO mbi = { sizeof(mbi) };
				RECT rcWindow;

				GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);
				GetWindowRect(hWnd, &rcWindow);
				rc = mbi.rcBar;
				OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
				FillRect(pMbm->hdc, &rc, brush);
				return TRUE;
			}
			case WM_MENUBAR_DRAWMENUITEM: {
				MenubarDrawmenuitem* pMdi = (MenubarDrawmenuitem*)lParam;
				wchar_t menuString[256] = { 0 };
				MENUITEMINFOW mii = { sizeof(mii), MIIM_STRING };
				DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
				int iTextStateID = MPI_NORMAL;
				int iBackgroundStateID = MPI_NORMAL;
				
				mii.dwTypeData = menuString;
				mii.cch = (sizeof(menuString) / 2) - 1;
				GetMenuItemInfoW(pMdi->um.hMenu, pMdi->umi.iPosition, TRUE, &mii);			
				if ((pMdi->dis.itemState & ODS_INACTIVE) | (pMdi->dis.itemState & ODS_DEFAULT)) {
					iTextStateID = MPI_NORMAL;
					iBackgroundStateID = MPI_NORMAL;
				}
				if (pMdi->dis.itemState & ODS_HOTLIGHT) {
					iTextStateID = MPI_HOT;
					iBackgroundStateID = MPI_HOT;
				}
				if (pMdi->dis.itemState & ODS_SELECTED) {
					iTextStateID = MPI_HOT;
					iBackgroundStateID = MPI_HOT;
				}
				if ((pMdi->dis.itemState & ODS_GRAYED) || (pMdi->dis.itemState & ODS_DISABLED)) {
					iTextStateID = MPI_DISABLED;
					iBackgroundStateID = MPI_DISABLED;
				}
				if (pMdi->dis.itemState & ODS_NOACCEL)
					dwFlags |= DT_HIDEPREFIX;

				if (!theme)
					theme = OpenThemeData(hWnd, L"Menu");

				if (iBackgroundStateID == MPI_NORMAL || iBackgroundStateID == MPI_DISABLED)
					FillRect(pMdi->um.hdc, &pMdi->dis.rcItem, brush);
				else if (iBackgroundStateID == MPI_HOT || iBackgroundStateID == MPI_DISABLEDHOT) {
					HBRUSH hot = CreateSolidBrush(0x404040);
					FillRect(pMdi->um.hdc, &pMdi->dis.rcItem, hot);
					DeleteObject(hot);
				} else DrawThemeBackground(theme, pMdi->um.hdc, MENU_POPUPITEM, iBackgroundStateID, &pMdi->dis.rcItem, nullptr);

				DTTOPTS dttopts = { sizeof(dttopts) };
				if (iTextStateID == MPI_NORMAL || iTextStateID == MPI_HOT) {
					dttopts.dwFlags |= DTT_TEXTCOLOR;
					dttopts.crText = 0xCFCFCF;
				}
				DrawThemeTextEx(theme, pMdi->um.hdc, MENU_POPUPITEM, iTextStateID, menuString, mii.cch, dwFlags, &pMdi->dis.rcItem, &dttopts);
				CloseThemeData(theme);
				return TRUE;
			}
			case WM_NCPAINT: 
			case WM_NCACTIVATE: {
				HDC hdc = GetWindowDC(hWnd);
				RECT rcClient = { 0 };
				RECT rcWindow = { 0 };
				RECT rcLine;
				HBRUSH br = CreateSolidBrush(0x545454);
				
				DefWindowProc(hWnd, msg, wParam, lParam);
				GetClientRect(hWnd, &rcClient);
				MapWindowPoints(hWnd, nullptr, (POINT*)&rcClient, 2);
				GetWindowRect(hWnd, &rcWindow);
				OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);
				rcLine = rcClient;
				rcLine.bottom = rcLine.top;
				rcLine.top--;
				FillRect(hdc, &rcLine, br);
				ReleaseDC(hWnd, hdc);			
				return TRUE;
			}
		}
		return -1;
	}

	LRESULT CALLBACK StatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		PAINTSTRUCT ps;
		Widget *w = (Widget*)dwRefData;
		RECT r;

		switch (msg)
		{
			case WM_ERASEBKGND: {
				if (DarkMode) {
					GetClientRect(hwnd, &r);
					FillRect((HDC)wParam, &r, w->brush);
				}
				return TRUE;
			}

			case WM_PAINT:
			{
				if (DarkMode) {
					typedef struct {
						int horizontal;
						int vertical;
						int between;
					} Borders;
					Borders borders = {0};
					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(hwnd, &ps);
					HPEN pen = CreatePen(PS_SOLID, 1, 0x585858);
					HPEN holdPen = (HPEN)SelectObject(hdc, pen);
					HFONT holdFont = (HFONT)SelectObject(hdc, w->font);
					const DWORD style = GetWindowLongPtr(hwnd, GWL_STYLE);
					BOOL isSizeGrip = style & SBARS_SIZEGRIP;
					int nParts = (int)SendMessage(hwnd, SB_GETPARTS, 0, 0);
					HBRUSH soft = CreateSolidBrush(0x686868);
					HBRUSH brush = CreateSolidBrush(0x303030);

					SendMessage(hwnd, SB_GETBORDERS, 0, (LPARAM)&borders);
					GetClientRect(hwnd, &r);
					FillRect(hdc, &ps.rcPaint, brush);

					for (int i = 0; i < nParts; ++i)
					{
						RECT rcPart = {0};
						RECT rcIntersect = {0};
						SendMessage(hwnd, SB_GETRECT, i, (LPARAM)&rcPart);
						if (!IntersectRect(&rcIntersect, &rcPart, &ps.rcPaint))
							continue;
						if (nParts > 2) {
							POINT edges[] = {
								{rcPart.right - 2, rcPart.top + 1},
								{rcPart.right - 2, rcPart.bottom - 3}
							};
							Polyline(hdc, edges, 2);
						}

						RECT rcDivider = { rcPart.right - borders.vertical, rcPart.top, rcPart.right, rcPart.bottom };
						DWORD cchText = 0;
						cchText = LOWORD(SendMessage(hwnd, SB_GETTEXTLENGTH, i, 0));
						wchar_t *str = (wchar_t*)calloc(1, sizeof(wchar_t)*(cchText+1));
						LRESULT lr = SendMessageW(hwnd, SB_GETTEXTW, i, (LPARAM)str);

						SetBkMode(hdc, TRANSPARENT);
						SetTextColor(hdc, 0xC0C0C0);
						rcPart.left += borders.between;
						rcPart.right -= borders.vertical;
						DrawTextW(hdc, str, cchText, &rcPart, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

						if (!isSizeGrip && i < (nParts - 1))
							FillRect(hdc, &rcDivider, soft);
					}

					if (isSizeGrip) {
						HTHEME hTheme = OpenThemeData(hwnd, L"Status");
						SIZE gripSize;
						RECT rc = r;
						GetThemePartSize(hTheme, hdc, SP_GRIPPER, 0, &r, TS_DRAW, &gripSize);
						rc.left = rc.right - gripSize.cx;
						rc.top = rc.bottom - gripSize.cy;
						DrawThemeBackground(hTheme, hdc, SP_GRIPPER, 0, &rc, NULL);
					}
					MoveToEx(hdc, 0, 0, NULL);
					LineTo(hdc, r.right, 0);
					SelectObject(hdc, holdFont);
					SelectObject(hdc, holdPen);
					DeleteObject(pen);
					DeleteObject(soft);
					DeleteObject(brush);
					EndPaint(hwnd, &ps);
					return 0;
				}
			}
			case WM_NCDESTROY: {
				RemoveWindowSubclass(hwnd, StatusProc, uIdSubclass);
				break;
			}
		}
		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK ComboBoxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		switch (uMsg)
		{

		case WM_CTLCOLOREDIT:
			{
				HDC hdc = (HDC)wParam;
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, 0xFFFFFF);
				SetBkColor(hdc, 0x383838);
				return (LRESULT)CreateSolidBrush(0x383838);
			}
		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
				HDC hDC = pDIS->hDC;
				RECT rc = pDIS->rcItem;
				wchar_t itemText[1024] = { 0 };
				Widget *w = (Widget *)dwRefData;
				COMBOBOXEXITEMW cbi = { 0 };

				cbi.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_OVERLAY | CBEIF_INDENT;
				cbi.iItem = pDIS->itemID;
				cbi.cchTextMax = 1024;
				cbi.pszText = itemText;

				HWND cwnd = GetParent(hWnd);
				if (SendMessageW(cwnd, CBEM_GETITEMW, 0, (LPARAM)&cbi)) {
					HIMAGELIST imglist = (HIMAGELIST)SendMessage(cwnd, CBEM_GETIMAGELIST, 0, 0);
					rc.left += cbi.iIndent * 10;
					int img = (pDIS->itemState & LVIS_SELECTED) ? cbi.iSelectedImage : cbi.iImage;
					if (pDIS->itemState & LVIS_FOCUSED)
						SetBkColor(hDC, 0x686868);
					else
						SetBkColor(hDC, 0x383838);
					ExtTextOutW(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

					if (imglist && (img != -1) && (img != INT16_MAX))
					{
						ImageList_Draw(imglist, img, hDC, rc.left, rc.top, ILD_TRANSPARENT | INDEXTOOVERLAYMASK(cbi.iOverlay));
					}
					rc.left += w ? (w->item.iconstyle ? 22 : 4) : 4;
					SetTextColor(pDIS->hDC, 0xFFFFFF);
					SetBkMode(hDC, TRANSPARENT);
					DrawTextW(hDC, cbi.pszText, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
					return TRUE;
				}
			}
			break;
		case WM_DESTROY:
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, ComboBoxSubclassProc, 1234);
			break;
		}
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT GroupBoxSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		PAINTSTRUCT ps;
		HWND h;
		HDC hdc = BeginPaint(hwnd, &ps);
		Widget *w = (Widget*)dwRefData;
		RECT r = {0}, rt;
		Widget *wp = (Widget*)GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA);
		
		RECT rect;
		GetClientRect(hwnd, &rect);
		SelectObject(hdc, w->font);
		int len = GetWindowTextLengthW(hwnd);
		wchar_t *buff = (wchar_t *)malloc((len + 1)*sizeof(wchar_t));
		GetWindowTextW(hwnd, buff, len + 1);
		DrawTextW(hdc, buff, -1, &r, DT_SINGLELINE | DT_CALCRECT);
		HBRUSH brush =  (wp && wp->brush) ? wp->brush : (HBRUSH)GetStockObject(BLACK_BRUSH);
		HPEN old = (HPEN)SelectObject(hdc, GetStockPen(DC_PEN));
		SetDCPenColor(hdc, 0x848484);
		SelectObject(hdc, brush);
		FillRect(hdc, &rect, brush);
		Rectangle(hdc, rect.left+4, ++rect.top+r.bottom/2, rect.right-4, rect.bottom-r.bottom/2);
		SelectObject(hdc, old);
		if (len) {
			r.left = 10;
			r.right += 11;
			r.bottom -= r.top;
			SetBkColor(hdc, 0);
			SetTextColor(hdc, 0xFFFFFF);
			rt = r,
			rt.left = 8;			
			FillRect(hdc, &rt, brush);
			DrawTextW(hdc, buff, -1, &r, DT_SINGLELINE);
		}
		free(buff);
		EndPaint(hwnd, &ps);
		return TRUE;
	}

	static void drawTab(HWND hwnd, HDC hdc, int i, PAINTSTRUCT *ps, RECT *rc, HIMAGELIST himl, BOOL isSelected, int cx, int cy) {
		WCHAR label[MAX_PATH];
		
		rc->top = 0;
		rc->bottom += 2;
		rc->left -=2;
		TC_ITEMW tci;
		tci.mask = TCIF_TEXT | TCIF_IMAGE;
		tci.pszText = label;
		tci.cchTextMax = MAX_PATH - 1;
		SendMessage(hwnd, TCM_GETITEMW, i, (LPARAM)&tci);
		FillRect(hdc, rc, GetStockBrush(DC_BRUSH));
		if (isSelected) {
			MoveToEx(hdc, rc->left, rc->bottom, NULL);
			LineTo(hdc, rc->left, rc->top);
			LineTo(hdc, rc->right, rc->top);
			LineTo(hdc, rc->right, rc->bottom);
			SetTextColor(hdc, 0xFFFFFF);
		} else {
			rc->top += 2;
			Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);
			SetTextColor(hdc, 0x646464);
		}
		if (tci.iImage >= 0) {
			rc->top +=2;
			int x = rc->left + 6;
			int y = rc->top + ((rc->bottom - rc->top) - cy) / 2;
			ImageList_Draw(himl, tci.iImage, hdc, x, y, ILD_TRANSPARENT);
			rc->left += cx;
		}
		DrawTextW(hdc, label, -1, rc, DT_HIDEPREFIX | DT_SINGLELINE | DT_VCENTER | DT_PATH_ELLIPSIS | DT_CENTER);
	}

	LRESULT TabSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		PAINTSTRUCT ps;
		RECT rc;
		DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
		Widget *w= (Widget*)dwRefData;
		Widget *wp = (Widget*)GetWindowLongPtr(GetParent((HWND)w->handle), GWLP_USERDATA);
		HBRUSH pbrush = wp->brush ? wp->brush : w->brush;
		
		if (DarkMode) {
			HDC hdc = BeginPaint(hwnd, &ps);
			int count = TabCtrl_GetItemCount(hwnd);
			SetBkMode(hdc, TRANSPARENT);
			SetDCPenColor(hdc, 0x848484);
			SelectObject(hdc, GetStockPen(DC_PEN));
			SelectObject(hdc, w->brush);
			FillRect(hdc, &ps.rcPaint, pbrush);
			if (count) {
				TabCtrl_GetItemRect(hwnd, 0, &rc);
				MoveToEx(hdc, 0, rc.bottom+1, NULL);
				LineTo(hdc, 0, ps.rcPaint.bottom-1);
				LineTo(hdc, ps.rcPaint.right-1, ps.rcPaint.bottom-1);
				LineTo(hdc, ps.rcPaint.right-1, rc.bottom+1);
				HGDIOBJ hFont = (HGDIOBJ)SendMessage(hwnd, WM_GETFONT, 0, 0);
				SelectObject(hdc, hFont);
				int nSelected = TabCtrl_GetCurSel(hwnd);
				HIMAGELIST himl = TabCtrl_GetImageList(hwnd);
				int cx, cy;
				ImageList_GetIconSize(himl, &cx, &cy);
				for (int i = 0; i < count; i++) {
					if (i != nSelected) {
						TabCtrl_GetItemRect(hwnd, i, &rc);
						drawTab(hwnd, hdc, i, &ps, &rc, himl, FALSE, cx, cy);
					}
				}
				TabCtrl_GetItemRect(hwnd, nSelected, &rc);
				SetDCBrushColor(hdc, 0x282828);
				drawTab(hwnd, hdc, nSelected, &ps, &rc, himl, TRUE, cx, cy);
				MoveToEx(hdc, rc.right, rc.bottom-1, NULL);
				LineTo(hdc, ps.rcPaint.right, rc.bottom-1);
			} else Rectangle(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
			EndPaint(hwnd, &ps);
		} else DefSubclassProc(hwnd, msg, wParam, lParam);
		RECT r, rr;
		int count = TabCtrl_GetItemCount((HWND)w->handle);
		HDC hdc = GetDC((HWND)w->handle);
		GetClientRect((HWND)w->handle, &r);
		COLORREF cr;
		LOGBRUSH lbr;
		GetObject(pbrush, sizeof(lbr), &lbr);
		HBRUSH c = CreateSolidBrush(lbr.lbColor);
		SelectObject(hdc, c);
		int sel = TabCtrl_GetCurSel((HWND)w->handle);
		for (int i = 0; i < count; i++) {
			if (i != sel) {
				TabCtrl_GetItemRect((HWND)w->handle, i, &rr);
				cr = GetPixel(hdc, rr.left+r.left+2, rr.top+r.top-2);
				ExtFloodFill(hdc, rr.left+r.left+10*GetDPIForSystem(), rr.top+r.top-2, cr, FLOODFILLSURFACE);
			}
		}
		DeleteObject(c);
		ReleaseDC((HWND)w->handle, hdc);
		return FALSE;
	}
}