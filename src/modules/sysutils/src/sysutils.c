#include <luart.h>
#include <File.h>
#include <Task.h>
#include <Date.h>
#include "Process.h"
#include <Directory.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <KnownFolders.h>

static int folders_ref;

//------------------------------------ sysutils.diskusage() function
LUA_METHOD(sysutils, diskusage)
{
  ULARGE_INTEGER freeBytesAvail;
  ULARGE_INTEGER totalBytesAvail;
  wchar_t *diskname = lua_towstring(L, 1);
  int nresult = 2;

  if (GetDiskFreeSpaceExW(diskname, &freeBytesAvail, &totalBytesAvail, NULL)) {
      lua_pushinteger(L, (lua_Integer)freeBytesAvail.QuadPart);
      lua_pushinteger(L, (lua_Integer)totalBytesAvail.QuadPart);
  } else {
    luaL_pushfail(L);
    nresult = 1;
  }
  free(diskname);
  return nresult;
}

//------------------------------------ sysutils.dirsize() function
static lua_Number calcsize(wchar_t *dir) {
    WIN32_FIND_DATAW data;
    HANDLE h = NULL;
    size_t len = wcslen(dir)+3;
    wchar_t *path = calloc(1, len*sizeof(wchar_t));
    lua_Integer dirsize = 0;

    _snwprintf(path, len, L"%s\\*", dir);

    if ((h = FindFirstFileW((path), &data)) != INVALID_HANDLE_VALUE ) {
      do {
        if (!(data.cFileName[0] == L'.' && (data.cFileName[1] == L'.' || data.cFileName[1] == 0))) {
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY) {
              size_t size = wcslen(dir)+wcslen(data.cFileName)+3;
              wchar_t *filepath = calloc(1, size*sizeof(wchar_t));
              _snwprintf(filepath, size, L"%ls\\%ls", dir, data.cFileName);
              dirsize += calcsize(filepath);
              free(filepath);
            } else
                dirsize += (lua_Integer)(data.nFileSizeHigh *(MAXDWORD+1) + data.nFileSizeLow);
        }
      } while (FindNextFileW(h, &data)); 
      FindClose(h);    
    }
    free(path);
    return dirsize;
}

LUA_METHOD(sysutils, dirsize)
{
  lua_pushinteger(L, calcsize(luaL_checkDirname(L, 1)));
  return 1;
}

//------------------------------------ sysutils.where() function
LUA_METHOD(sysutils, where)
{
  wchar_t* buffer = calloc(MAX_PATH, sizeof(wchar_t));
  wchar_t *fname = luaL_checkFilename(L, 1);
  DWORD len;
  
searchpath:
  if ((len = SearchPathW(NULL, fname, NULL, MAX_PATH, buffer, NULL)) > MAX_PATH) {
    buffer = realloc(buffer, len);
    goto searchpath;    
  }
  if (len > 0) {
    lua_pushlwstring(L, buffer, len);
    lua_pushinstance(L, File, 1);
  }
  else lua_pushnil(L);
  free(buffer);
  free(fname);
  return 1;
}

//------------------------------------ sysutils.watch() function

typedef struct {
  wchar_t     *path;
  HANDLE      hdir;
  OVERLAPPED  ovl;
  BYTE        buffer[1024];
  int         ref;
} WatchData;

static int gc_WatchData(lua_State *L) {
  WatchData *wd = lua_self(L, 1, Task)->userdata;
  CloseHandle(wd->ovl.hEvent);
  CloseHandle(wd->hdir);
  luaL_unref(L, LUA_REGISTRYINDEX, wd->ref);
  free(wd->path);
  free(wd);
  return 0;
}

static const char *actions[] = { "new", "removed", "modified", "renamed", "renamed"};

static int WatchTaskContinue(lua_State* L, int s, lua_KContext ctx) {	
	WatchData *wd = (WatchData *)ctx;
  DWORD result;

  DWORD status = WaitForSingleObject(wd->ovl.hEvent, 0);
  switch (status) {
    case WAIT_TIMEOUT:  break;
    case WAIT_OBJECT_0: {
                          DWORD offset = 0, dw;
                          wchar_t fileName[MAX_PATH] = L"";
                          FILE_NOTIFY_INFORMATION *fni = NULL;
                          const char *action;

                          GetOverlappedResult(wd->hdir, &wd->ovl, &dw, FALSE);                            
                          do {
                            fni = (FILE_NOTIFY_INFORMATION*)(&wd->buffer[offset]);
                            if (fni->Action != FILE_ACTION_RENAMED_NEW_NAME) {
                              lua_rawgeti(L, LUA_REGISTRYINDEX, wd->ref);
                              lua_pushstring(L, actions[fni->Action-1]);
                            }
                            lua_pushwstring(L, fni->FileName);
                            wchar_t buffer[1024];
                            _snwprintf(buffer, 1024, L"%s/%s",wd->path, fni->FileName);
                            if (GetFileAttributesW(buffer) & FILE_ATTRIBUTE_DIRECTORY)
                              lua_pushinstance(L, Directory, 1);
                            else 
                              lua_pushinstance(L, File, 1);
                            lua_remove(L, -2);
                            if (fni->Action == FILE_ACTION_RENAMED_OLD_NAME)
                              goto next;
                            lua_call(L, 2+(fni->Action == FILE_ACTION_RENAMED_NEW_NAME), 0);
                       next:  offset += fni->NextEntryOffset;
                          } while (fni->NextEntryOffset != 0);
                          if (ResetEvent(wd->ovl.hEvent) == 0)
                            luaL_error(L, "Internal error during directory watching Task");
                          if (!(result = ReadDirectoryChangesW(wd->hdir, wd->buffer, 1024, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME, NULL, &wd->ovl, NULL))) {
                            result = GetLastError();
                            if (result != ERROR_IO_PENDING)
                              luaL_error(L, "Internal error during directory watching Task");
                          }
                      } break;
      default:        return luaL_error(L, "Internal error during directory watching Task");
  }
  return lua_yieldk(L, 0, (lua_KContext)wd, WatchTaskContinue);
}

LUA_METHOD(sysutils, watch) {
  WatchData *wd;
  DWORD result;

  luaL_checktype(L, 2, LUA_TFUNCTION);
  wd = calloc(1, sizeof(WatchData));
  wd->path = luaL_checkDirname(L, 1);
  wd->hdir = CreateFileW(wd->path, FILE_LIST_DIRECTORY, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED, NULL); 
  wd->ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if ((wd->hdir == INVALID_HANDLE_VALUE) || !(wd->ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		goto fail;
  if (!(result = ReadDirectoryChangesW(wd->hdir, wd->buffer, 1024, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME, NULL, &wd->ovl, NULL))) {
    result = GetLastError();
    if (result != ERROR_IO_PENDING) {
      CloseHandle(wd->ovl.hEvent);
fail: CloseHandle(wd->hdir);
      free(wd->path);
      free(wd);
      return 0;
    }
  }
  lua_pushtask(L, WatchTaskContinue, wd, gc_WatchData);
  lua_pushvalue(L, 2);
  wd->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	return 1;
}

//------------------------------------ sysutils.drives property
static const char *drivetype[] = { "unknown", "unmounted", "removable", "fixed", "network", "disc", "memory"};

LUA_PROPERTY_GET(sysutils, drives) {
  lua_createtable(L, 3, 0);
  char drives[MAX_PATH] = {0}; 
  char name[MAX_PATH], fs[MAX_PATH];
  DWORD serial, flags;
  DWORD count = GetLogicalDriveStrings(MAX_PATH, drives);
  int n = 0;

  if (count > 0 && count <= MAX_PATH) {
      char* drive = drives;
      while(*drive) {
        lua_createtable(L, 0, 4);
        lua_pushstring(L, drive);
        lua_setfield(L, -2, "root");
        lua_pushstring(L, drivetype[GetDriveTypeA(drive)]);
        lua_setfield(L, -2, "type");
        GetVolumeInformationA((const char*)drive, name, MAX_PATH, NULL, NULL, &flags, fs, MAX_PATH);
        lua_pushstring(L, name);
        lua_setfield(L, -2, "name");
        lua_pushstring(L, fs);
        lua_setfield(L, -2, "filesystem");
        lua_pushboolean(L, flags & FILE_READ_ONLY_VOLUME);
        lua_setfield(L, -2, "readonly");
        lua_pushboolean(L, flags & FILE_VOLUME_IS_COMPRESSED);
        lua_setfield(L, -2, "compressed");
        lua_rawseti(L, -2, ++n);
        drive += strlen(drive)+1;
      }
  } else lua_pushnil(L);
  return 1;
}

//------------------------------------ sysutils.arch property
LUA_PROPERTY_GET(sysutils, sysarch) {
    #if defined(_WIN64)
      lua_pushstring(L, "x64");
    #else       
      BOOL f = FALSE;
      IsWow64Process(GetCurrentProcess(), &f);
      lua_pushstring(L,  f ? "x64" : "x32");
    #endif
    return 1;
}

//------------------------------------ sysutils.folders property
static const char *folders[] = {  "System", "ProgramFiles", "Windows", "Videos", "Startup", "StartMenu", "Sendto", \
                                  "RoamingAppData", "Recent", "Programs", "Pictures", "Music", "LocalAppData", "Links", \
                                  "Fonts", "Favorites", "Downloads", "Documents", "Desktop", "Contacts", NULL};
static KNOWNFOLDERID fid[20];

static int folders_index(lua_State *L) {
  int id = lua_optstring(L, 2, folders, -1);
  PWSTR path;

  if (id == -1)
    luaL_error(L, "unknown '%s' Windows folder", lua_tostring(L, 2));

  if (SUCCEEDED(SHGetKnownFolderPath(&fid[id], 0, NULL, &path))) {
    lua_pushwstring(L, path);
    lua_pushvalue(L, -1);
    lua_setfield(L, 1,  lua_tostring(L, 2));
    CoTaskMemFree(path);
  } else lua_pushnil(L);
  return 1;
}

//------------------------------------ sysutils.addrecent() function
LUA_METHOD(sysutils, addrecent) {
  wchar_t *fpath = luaL_checkFilename(L, 1);
  SHAddToRecentDocs(SHARD_PATHW, fpath);
  SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment");
  free(fpath);
  return 0;
}

//------------------------------------ sysutils.recyclebin.empty() function
LUA_METHOD(sysutils, empty) {
  lua_pushboolean(L, SUCCEEDED(SHEmptyRecycleBinA(NULL, luaL_checkstring(L, 1), SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND)));
  return 1;
}

//------------------------------------ sysutils.recyclebin.send() function
LUA_METHOD(sysutils, send) {
  int n = lua_gettop(L); 
  IFileOperation *pfo;
  BOOL result = TRUE;
  HRESULT hr = CoCreateInstance(&CLSID_FileOperation, NULL, CLSCTX_ALL, &IID_IFileOperation, (void**)&pfo);
  
  if (SUCCEEDED(hr)) {
    pfo->lpVtbl->SetOperationFlags(pfo, FOFX_ADDUNDORECORD | FOFX_RECYCLEONDELETE | FOF_NOERRORUI | FOF_SILENT);
    for (int i = 1; i <= n; i++) {
      wchar_t *fname;
      IShellItem *psi;
      File *f; 

      if (!result)
        goto done;
      if ( (f = lua_iscinstance(L, i, TFile)) || (f = lua_iscinstance(L, i, TDirectory)) )
        fname = f->fullpath;
      else fname = lua_towstring(L, i);
      if ((SUCCEEDED(SHCreateItemFromParsingName(fname, NULL, &IID_IShellItem, &psi)))) {
        result = SUCCEEDED(pfo->lpVtbl->DeleteItem(pfo, psi, NULL));
        result = SUCCEEDED(pfo->lpVtbl->PerformOperations(pfo));      
        psi->lpVtbl->Release(psi);
      } 
      if (!f)
        free(fname);
    }
done:
    pfo->lpVtbl->Release(pfo);
  }
  lua_pushboolean(L, result);
  return 1;
}

//------------------------------------ sysutils.computer property
LUA_PROPERTY_GET(sysutils, computer) {
  wchar_t name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(name);

	if (!GetComputerNameW(name, &size))
		lua_pushboolean(L, FALSE);
	else lua_pushlwstring(L, name, size);
  return 1;
}

//------------------------------------ sysutils.folder property
LUA_PROPERTY_GET(sysutils, folders) {
  lua_rawgeti(L, LUA_REGISTRYINDEX, folders_ref);
  return 1;
}

//------------------------------------ sysutils.user property
LUA_PROPERTY_GET(sysutils, user) {
  wchar_t name[MAX_PATH + 1];
  DWORD size = sizeof(name);

	if (!GetUserNameW(name, &size))
		lua_pushboolean(L, FALSE);
	else lua_pushlwstring(L, name, size);
  return 1;
}

//------------------------------------ sysutils.isadmin property
LUA_PROPERTY_GET(sysutils, isadmin) {
  BOOL isAdmin = FALSE;
  HANDLE hProcessHeap = GetProcessHeap();
  DWORD cbSid = SECURITY_MAX_SID_SIZE;
  const PSID pSid = HeapAlloc(hProcessHeap, 0, cbSid);

  CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, pSid, &cbSid) && CheckTokenMembership(NULL, pSid, &isAdmin);
  lua_pushboolean(L, isAdmin);
  HeapFree(hProcessHeap, 0, pSid);
  return 1;
}

//------------------------------------ sysutils.shellexec() function
LUA_METHOD(sysutils, shellexec) {
    BOOL success;
    SHELLEXECUTEINFOW sei = {0};
    wchar_t *verb = lua_towstring(L, 1);
    wchar_t *path = luaL_checkFilename(L, 2);
    wchar_t *param = lua_isstring(L, 3) ? lua_towstring(L, 3) : NULL;
    wchar_t *dir = lua_isstring(L, 4) ? lua_towstring(L, 4) : NULL;

    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_ASYNCOK;
    sei.lpVerb = verb;
    sei.lpParameters = param;
    sei.lpDirectory = dir;
    sei.lpFile = path;
    sei.nShow = SW_SHOWDEFAULT;

    lua_pushboolean(L, ShellExecuteExW(&sei));
    free(dir);
    free(param);
    free(path);
    free(verb);
    lua_sleep(L, 500);
    return 1;
}

LUA_METHOD(sysutils, shellupdate) {
  SHChangeNotify(SHCNE_ALLEVENTS, 0, NULL, NULL);
  return 0;
}

LUA_METHOD(sysutils, finalize) {
  luaL_unref(L, LUA_REGISTRYINDEX, folders_ref);
  return 0;
}

LUA_METHOD(sysutils, isinstance) {
  HANDLE mutex = CreateMutexA(NULL, TRUE, luaL_checkstring(L, 1));
  if (GetLastError() == ERROR_ALREADY_EXISTS)
    mutex = NULL;
  lua_pushboolean(L, mutex == NULL);
  return 1;
}

//--- Registration of module properties and functions        
MODULE_PROPERTIES(sysutils)
  READONLY_PROPERTY(sysutils, drives)
  READONLY_PROPERTY(sysutils, folders)
  READONLY_PROPERTY(sysutils, isadmin)
  READONLY_PROPERTY(sysutils, sysarch)
  READONLY_PROPERTY(sysutils, computer)
  READONLY_PROPERTY(sysutils, user)
END

MODULE_FUNCTIONS(sysutils)
  METHOD(sysutils, addrecent)
  METHOD(sysutils, diskusage)
  METHOD(sysutils, dirsize)
  METHOD(sysutils, where)
  METHOD(sysutils, watch)
  METHOD(sysutils, shellexec)
  METHOD(sysutils, shellupdate)
  METHOD(sysutils, isinstance)
END

MODULE_FUNCTIONS(recyclebin)
  METHOD(sysutils, empty)
  METHOD(sysutils, send)
END

//----- "luaopen_sysutils" module registration function
int __declspec(dllexport) luaopen_sysutils(lua_State *L)
{    
    lua_regmodulefinalize(L, sysutils);
    lua_pushstring(L, "recyclebin");
    lua_createtable(L, 0, 2);
    luaL_setfuncs(L, recyclebinlib, 0);
    lua_rawset(L, -3);
    lua_regobjectmt(L, Process);
    lua_createtable(L, 0, 3);
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, folders_index);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    folders_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    fid[0] = FOLDERID_System;
    fid[1] = FOLDERID_ProgramFiles;
    fid[2] = FOLDERID_Windows;
    fid[4] = FOLDERID_Videos;
    fid[4] = FOLDERID_Startup;
    fid[5] = FOLDERID_StartMenu;
    fid[6] = FOLDERID_SendTo;
    fid[7] = FOLDERID_RoamingAppData;
    fid[8] = FOLDERID_Recent;
    fid[9] = FOLDERID_Programs;
    fid[10] = FOLDERID_Pictures;
    fid[11] = FOLDERID_Music;
    fid[12] = FOLDERID_LocalAppData;
    fid[13] = FOLDERID_Links;
    fid[14] = FOLDERID_Fonts;
    fid[15] = FOLDERID_Favorites;
    fid[16] = FOLDERID_Downloads;
    fid[17] = FOLDERID_Documents;
    fid[18] = FOLDERID_Desktop;
    fid[19] = FOLDERID_Contacts;
    return 1;
}