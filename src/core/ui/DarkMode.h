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

#ifdef __GNUC__
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#ifndef _SHELLSCALINGAPI_H_
#define _SHELLSCALINGAPI_H_

#include <shtypes.h>
#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

#ifndef SCALING_ENUMS_DECLARED
typedef enum {
  SCF_VALUE_NONE = 0x00,
  SCF_SCALE = 0x01,
 SCF_PHYSICAL = 0x02
} SCALE_CHANGE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(SCALE_CHANGE_FLAGS);

typedef enum {
  DEVICE_PRIMARY = 0,
  DEVICE_IMMERSIVE = 1
} DISPLAY_DEVICE_TYPE;

#define SCALING_ENUMS_DECLARED
#endif /* SCALING_ENUMS_DECLARED */

#if NTDDI_VERSION >= NTDDI_WIN8
STDAPI_(DEVICE_SCALE_FACTOR) GetScaleFactorForDevice(DISPLAY_DEVICE_TYPE deviceType);
STDAPI RegisterScaleChangeNotifications(DISPLAY_DEVICE_TYPE displayDevice, HWND hwndNotify, UINT uMsgNotify, DWORD *pdwCookie);
STDAPI RevokeScaleChangeNotifications(DISPLAY_DEVICE_TYPE displayDevice, DWORD dwCookie);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#ifndef DPI_ENUMS_DECLARED
typedef enum MONITOR_DPI_TYPE {
  MDT_EFFECTIVE_DPI = 0,
  MDT_ANGULAR_DPI = 1,
  MDT_RAW_DPI = 2,
  MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

typedef enum PROCESS_DPI_AWARENESS {
  PROCESS_DPI_UNAWARE = 0,
 PROCESS_SYSTEM_DPI_AWARE = 1,
  PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

#define DPI_ENUMS_DECLARED
#endif /* DPI_ENUMS_DECLARED */

#if NTDDI_VERSION >= NTDDI_WINBLUE
STDAPI GetScaleFactorForMonitor(HMONITOR hMon, DEVICE_SCALE_FACTOR *pScale);
STDAPI RegisterScaleChangeEvent(HANDLE hEvent, DWORD_PTR *pdwCookie);
STDAPI UnregisterScaleChangeEvent(DWORD_PTR dwCookie);

STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);
STDAPI GetProcessDpiAwareness(HANDLE hprocess, PROCESS_DPI_AWARENESS *value);
STDAPI SetProcessDpiAwareness(PROCESS_DPI_AWARENESS value);
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#if NTDDI_VERSION >= NTDDI_WINTHRESHOLD
#ifndef SHELL_UI_COMPONENT_ENUMS_DECLARED
typedef enum {
  SHELL_UI_COMPONENT_TASKBARS = 0,
  SHELL_UI_COMPONENT_NOTIFICATIONAREA = 1,
  SHELL_UI_COMPONENT_DESKBAND = 2
} SHELL_UI_COMPONENT;

#define SHELL_UI_COMPONENT_ENUMS_DECLARED
#endif /* SHELL_UI_COMPONENT_ENUMS_DECLARED */

UINT WINAPI GetDpiForShellUIComponent(SHELL_UI_COMPONENT component);
#endif /* NTDDI_VERSION >= NTDDI_WINTHRESHOLD */

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */
#endif /* _SHELLSCALINGAPI_H_ */

#endif

#ifdef __cplusplus
}
#endif