#pragma once

#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

#ifdef __cplusplus
extern "C" {
#endif
	extern BOOL g_darkModeSupported;
	extern BOOL g_darkModeEnabled;
	extern DWORD g_buildNumber;
	extern BOOL DarkMode;

	typedef enum 
	{
		IHCM_USE_CACHED_VALUE,
		IHCM_REFRESH
	} IMMERSIVE_HC_CACHE_MODE;

	// 1903 18362
	typedef enum 
	{
		Default,
		AllowDark,
		ForceDark,
		ForceLight,
		Max
	} PreferredAppMode;

	typedef enum 
	{
		WCA_UNDEFINED = 0,
		WCA_NCRENDERING_ENABLED = 1,
		WCA_NCRENDERING_POLICY = 2,
		WCA_TRANSITIONS_FORCEDISABLED = 3,
		WCA_ALLOW_NCPAINT = 4,
		WCA_CAPTION_BUTTON_BOUNDS = 5,
		WCA_NONCLIENT_RTL_LAYOUT = 6,
		WCA_FORCE_ICONIC_REPRESENTATION = 7,
		WCA_EXTENDED_FRAME_BOUNDS = 8,
		WCA_HAS_ICONIC_BITMAP = 9,
		WCA_THEME_ATTRIBUTES = 10,
		WCA_NCRENDERING_EXILED = 11,
		WCA_NCADORNMENTINFO = 12,
		WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
		WCA_VIDEO_OVERLAY_ACTIVE = 14,
		WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
		WCA_DISALLOW_PEEK = 16,
		WCA_CLOAK = 17,
		WCA_CLOAKED = 18,
		WCA_ACCENT_POLICY = 19,
		WCA_FREEZE_REPRESENTATION = 20,
		WCA_EVER_UNCLOAKED = 21,
		WCA_VISUAL_OWNER = 22,
		WCA_HOLOGRAPHIC = 23,
		WCA_EXCLUDED_FROM_DDA = 24,
		WCA_PASSIVEUPDATEMODE = 25,
		WCA_USEDARKMODECOLORS = 26,
		WCA_LAST = 27
	} WINDOWCOMPOSITIONATTRIB;

	struct WINDOWCOMPOSITIONATTRIBDATA
	{
		WINDOWCOMPOSITIONATTRIB Attrib;
		PVOID pvData;
		SIZE_T cbData;
	};

	#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19

	double GetDPIForSystem();
	BOOL ShouldAppUseDarkMode();
	BOOL AllowDarkModeForWindow(HWND hWnd, BOOL allow);
	BOOL IsHighContrast();
	void RefreshTitleBarThemeColor(HWND hWnd, BOOL dark);
	BOOL IsColorSchemeChangeMessageLParam(LPARAM lParam);
	BOOL IsColorSchemeChangeMessage(UINT message, LPARAM lParam);
	BOOL IsDarkModeEnabled();
	void AllowDarkModeForApp(BOOL allow);
	void FixDarkScrollBar(BOOL theme);
	void FlushMenuThemes(void);
	void InitDarkMode();
	WidgetType GetWidgetTypeFromHWND(HWND h);
	LRESULT MenuBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, HBRUSH brush);
	LRESULT CALLBACK StatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT CALLBACK ComboBoxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT GroupBoxSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT TabSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	#define WM_MENUBAR_DRAWMENU 0x0091 
	#define WM_MENUBAR_DRAWMENUITEM 0x0092

	typedef union 
	{
		struct
		{
			DWORD cx;
			DWORD cy;
		} rgSizeBar[2];
		struct
		{
			DWORD cx;
			DWORD cy;
		} rgSizePopup[4];
	} MenubarMenuitemmetrics;

	typedef struct 
	{
		DWORD rgCx[4];
		DWORD fUpdateMaxWidths : 2;
	} MenubarMenupopupmetrics;

	typedef struct 
	{
		HMENU hMenu; // main window menu
		HDC hdc;
		DWORD dwFlags;
	} MenubarMenu;

	typedef struct
	{
		int iPosition; // 0-based position
		MenubarMenuitemmetrics umIm;
		MenubarMenupopupmetrics umpm;
	} MenubarMenuitem;

	typedef struct 
	{
		DRAWITEMSTRUCT dis; // itemID looks uninitialized
		MenubarMenu um;
		MenubarMenuitem umi;
	} MenubarDrawmenuitem;
 
	typedef struct 
	{
		MEASUREITEMSTRUCT mis;
		MenubarMenu um;
		MenubarMenuitem umi;
	} MenubarMeasuremenuitem;

#ifdef __cplusplus
}
#endif