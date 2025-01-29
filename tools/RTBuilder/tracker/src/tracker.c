#include <luart.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <Widget.h>

#define IDTRACKER 255

#define SizeLeft          0xF001
#define SizeRight         0xF002
#define SizeTop           0xF003
#define SizeTopLeft       0xF004
#define SizeTopRight      0xF005
#define SizeBottom        0xF006
#define SizeBottomRight   0xF008
#define SizeBottomLeft    0xF007
#define DragMove          0xF012

static LPCSTR cursors[] = { IDC_SIZEWE, IDC_SIZEWE, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZENESW, IDC_SIZENS, IDC_SIZENESW, IDC_SIZENWSE, IDC_SIZEWE, IDC_SIZEALL, IDC_ARROW };
static HCURSOR hcursors[11];
static luart_type TTracker;


static UINT onDelete, onTrack;

static UINT getdirection(HWND hwnd, LPARAM lParam) {
  DWORD X = GET_X_LPARAM(lParam);
  DWORD Y = GET_Y_LPARAM(lParam);
  DWORD Width, Height;
  RECT r;

  GetWindowRect(hwnd, &r); 
  Width = r.right-r.left;
  Height = r.bottom-r.top;
  if ((X >= Width - 2) && !((Y <= 2)  || (Y >= Height - 4)))
    return SizeRight;
  else if (!((X <= 2) || (X >= Width -2)) && (Y <= 4)) 
    return SizeTop;
  else if ((X <= 2) && (Y <= 4))
   return SizeTopLeft;
  else if ((X >= Width -2) && (Y <= 4))
   return SizeTopRight;
  else if (!((X <= 2) || (X >= Width -2)) && (Y >= Height - 4))  
   return SizeBottom;
  else if ((Y >= Height -4)  && (X <= 2))
   return SizeBottomLeft;
  else if ((Y >= Height -4) && (X >= Width - 2))
   return SizeBottomRight;
  else if (!((Y <= 4) || (Y >= Height - 4)) && (X <= 2))  
   return SizeLeft;
  else
   return DragMove;
}

static void stoptracking(Widget *w) {
  if (w->user && w->parent) {
    RECT r;
    ShowWindow(w->handle, SW_HIDE);
    ReleaseCapture();
    GetWindowRect(w->handle, &r);
    SetParent(w->user, w->parent);
    MapWindowPoints(HWND_DESKTOP, w->parent, (LPPOINT) &r, 2);
    w->hcursor = LoadCursor(NULL, IDC_ARROW);
    SetWindowPos(w->user, NULL, r.left+2, r.top+2, r.right-r.left-4, r.bottom-r.top-4, SWP_NOZORDER);
    Widget *user = (Widget*)GetWindowLongPtr(w->user, GWLP_USERDATA);
    if (user)
      user->isactive = FALSE;
    w->parent = NULL;
    w->user = NULL; 
  }
}

int event_onDelete(lua_State *L, Widget *w, MSG *msg) {
  lua_throwevent(L, "onDelete", 1);
  return 0;
}

int event_onTrack(lua_State *L, Widget *w, MSG *msg) {
  lua_throwevent(L, "onTrack", 1);
  return 0;
}

LRESULT CALLBACK TrackerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    Widget *w = (Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    LRESULT result;
    RECT r;
       
    switch(msg) {
      case WM_SIZE:
      case WM_MOVE:
      case WM_MOVING:
      case WM_SIZING:
        lua_callevent(w, onTrack);
        break;

      case WM_LBUTTONDOWN:
        ReleaseCapture();
        SendMessage(hwnd, WM_SYSCOMMAND, w->index ? getdirection(hwnd, lParam) : DragMove, 0);
        break;

      case WM_MOUSEMOVE: {
        POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};   
        GetClientRect(hwnd, &r);
        if (PtInRect(&r, p)) {
          UINT dir = getdirection(hwnd, lParam)-0xF001;
          SetCursor(hcursors[dir < 0x0011 ? dir : 9]);         
          SetCapture(hwnd);
          w->index = TRUE;
           return FALSE;
        } else {
          SetCursor(hcursors[10]);
          ReleaseCapture();
          w->index = FALSE;
        }
        break;
      }

      case WM_PAINT: {
        PAINTSTRUCT ps;  
        HDC hdc = BeginPaint(hwnd, &ps);
        HPEN hpen = CreatePen(PS_DOT, 1, 0xCC7A00);
        HPEN oldpen = SelectObject(hdc, hpen);
        HBRUSH brush = CreateSolidBrush(0xCC7A00);
        GetClientRect(hwnd, &r);        
        SetWindowPos(w->user, NULL, 2, 2, r.right-r.left-4, r.bottom-r.top-4, SWP_NOZORDER);
        FillRect(hdc, &r, w->brush);
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, GetStockBrush(NULL_BRUSH));
        Rectangle(hdc, 0, 0, r.right, r.bottom);
        DeleteObject(hpen);
        hpen = CreatePen(PS_SOLID, 1, 0xCC7A00);
        SelectObject(hdc, hpen);
        SelectObject(hdc, brush);
        Rectangle(hdc, 0, 0, 4, 4);
        Rectangle(hdc, r.right-4, 0, r.right, 4);
        Rectangle(hdc, 0, r.bottom-4, 4, r.bottom);       
        Rectangle(hdc, r.right-4, r.bottom-4, r.right, r.bottom);       
        DeleteObject(brush);
        DeleteObject(hpen);
        SelectObject(hdc, oldpen);
        EndPaint(hwnd, &ps);
        return FALSE;
      }

      case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
          stoptracking(w);
        else if (wParam == VK_DELETE)
          lua_callevent(w, onDelete);
        else if (wParam >= VK_LEFT && wParam <= VK_DOWN) {
          GetWindowRect(hwnd, &r);
          MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT) &r, 1);
          switch(wParam) {
            case VK_UP:     r.top--; break; 
            case VK_DOWN:   r.top++; break;
            case VK_LEFT:   r.left--; break; 
            case VK_RIGHT:  r.left++;
          }
          SetWindowPos(hwnd, NULL, r.left, r.top, 0, 0, SWP_NOSIZE);
        }
        return FALSE;

      case WM_CTLCOLORBTN:
      case WM_CTLCOLORSTATIC: {
            Widget *c = (Widget*)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);	
            if (c) {
                SetBkMode((HDC)wParam, TRANSPARENT);
                SetTextColor((HDC)wParam, c->color);
                if (c->brush)
                  return (INT_PTR)c->brush;
                if (w->brush)
                  return (INT_PTR)w->brush;
                return (INT_PTR)((Widget*)GetWindowLongPtr(GetParent(w->handle), GWLP_USERDATA))->brush;
              }
              return (INT_PTR)GetStockBrush(NULL_BRUSH);        
           }
    }
  return lua_widgetproc(hwnd, msg, wParam, lParam, 0, 0);
}

LUA_CONSTRUCTOR(Tracker) {
  Widget *wp, *w;
  double dpi;
  BOOL isdark;
	lua_widgetinitialize(L, &wp, &dpi, &isdark);
  w = lua_widgetconstructor(L, CreateWindowExW(0, L"Window", NULL, WS_CHILD, 0, 0, 0, 0, wp->handle, NULL, GetModuleHandle(NULL), NULL), TTracker, wp, TrackerProc);
  return 1;
}

LUA_METHOD(Tracker, stop) {
  stoptracking(lua_self(L, 1, Widget));
  return 0;
}

LUA_METHOD(Tracker, start) {
  Widget *w, *self = lua_self(L, 1, Widget);
  RECT r;
 
  if (self->parent)
    stoptracking(self);
	w = luaL_checkcinstance(L, 2, Widget);
  self->parent = GetParent(w->handle);
  GetWindowRect(w->handle, &r);
  MapWindowPoints(HWND_DESKTOP, GetParent(w->handle), (LPPOINT) &r, 2);
  SetWindowPos(self->handle, NULL, r.left-2, r.top-2, r.right-r.left+4, r.bottom-r.top+4, SWP_NOZORDER | SWP_SHOWWINDOW);
  self->brush = ((Widget*)GetWindowLongPtr(self->parent, GWLP_USERDATA))->brush;
  if (!self->brush)
    self->brush = ((Widget*)GetWindowLongPtr(GetParent(self->parent), GWLP_USERDATA))->brush;
  SetParent(w->handle, self->handle);
  self->user = w->handle;
  RedrawWindow(w->user, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASENOW | RDW_UPDATENOW);
  w->isactive = TRUE;
  BringWindowToTop(self->handle);
  SetFocus(self->handle);
  return 0;
}

LUA_PROPERTY_GET(Tracker, widget) {
  Widget *w = lua_self(L, 1, Widget);
	if (w->user)
		lua_rawgeti(L, LUA_REGISTRYINDEX, ((Widget*)GetWindowLongPtr(w->user, GWLP_USERDATA))->ref );
	else lua_pushnil(L);
  return 1;
}

LUA_PROPERTY_SET(Tracker, widget) {
  Tracker_stop(L);
  return Tracker_start(L);
}

LUA_PROPERTY_GET(Tracker, x) {
  Widget *w = lua_self(L, 1, Widget);
  if (w->user) {
    RECT r;
    GetWindowRect(w->handle, &r);
    MapWindowPoints(HWND_DESKTOP, w->parent, (LPPOINT) &r, 1);
    lua_pushinteger(L, r.left+2);
    return 1;
  } else lua_pushnil(L);
  return 1;  
}

LUA_PROPERTY_GET(Tracker, y) {
  Widget *w = lua_self(L, 1, Widget);
  if (w->user) {
    RECT r;
    GetWindowRect(w->handle, &r);
    MapWindowPoints(HWND_DESKTOP, w->parent, (LPPOINT) &r, 2);
    lua_pushinteger(L, r.top+2);
    return 1;
  } else lua_pushnil(L);
  return 1;  
}

LUA_PROPERTY_GET(Tracker, width) {
  Widget *w = lua_self(L, 1, Widget);
  if (w->user) {
    RECT r;
    GetWindowRect(w->handle, &r);
    MapWindowPoints(HWND_DESKTOP, GetParent(w->handle), (LPPOINT) &r, 2);
    lua_pushinteger(L, r.right-r.left-4);
    return 1;
  } else lua_pushnil(L);
  return 1;  
}

LUA_PROPERTY_GET(Tracker, height) {
  Widget *w = lua_self(L, 1, Widget);
  if (w->user) {
    RECT r;
    GetWindowRect(w->handle, &r);
    MapWindowPoints(HWND_DESKTOP, GetParent(w->handle), (LPPOINT) &r, 2);
    lua_pushinteger(L, r.bottom-r.top-4);
    return 1;
  } else lua_pushnil(L);
  return 1;  
}

LUA_METHOD(Tracker, __gc) {
  Tracker_stop(L);
  free(lua_widgetdestructor(L));
  return 0;
}

static luaL_Reg Tracker_methods[] = {
  {"start",         Tracker_start},
  {"stop",          Tracker_stop},
  {"get_x",         Tracker_getx},
  {"get_y",         Tracker_gety},
  {"get_widget",    Tracker_getwidget},
  {"set_widget",    Tracker_setwidget},
  {"get_width",     Tracker_getwidth},
  {"get_height",    Tracker_getheight},
  {"get_widget",    Tracker_getwidget},
  {"set_widget",    Tracker_setwidget},
  {NULL, NULL}
};

static luaL_Reg Tracker_metafields[] = {
  {"__gc",      Tracker___gc},
  {NULL, NULL}
};

int __declspec(dllexport) luaopen_tracker(lua_State *L)
{
  luaL_require(L, "ui");
  lua_regwidgetmt(L, Tracker, WIDGET_METHODS, FALSE, FALSE, FALSE, FALSE, FALSE);
  luaL_setrawfuncs(L, Tracker_methods);
  onDelete = lua_registerevent(L, NULL, event_onDelete);
  onTrack = lua_registerevent(L, NULL, event_onTrack);
  for (int i = 0; i < 11; i++)
    hcursors[i] = LoadCursor(NULL, cursors[i]);
  return 0;
}