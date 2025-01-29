#include <luart.h>
#include <windows.h>


typedef struct {
	lua_Integer ref;
    WNDPROC proc;
} Tracker;

#define Tracker(w) ((Tracker *)(w->parent))

