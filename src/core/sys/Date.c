/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2024
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Date.c | LuaRT Date object implementation
*/

#define LUA_LIB

#include <Date.h>
#include "lrtapi.h"
#include <luart.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <oleauto.h>


luart_type TDatetime;

typedef union  {
		FILETIME fTime;
		ULARGE_INTEGER ul;
} Timeunion;

const char *intervals[] = {"seconds", "minutes", "hours", "days", "months", "years", NULL};
const __int64 intervals_values[] = { _SECOND, _MINUTE, _HOUR, _DAY, _MONTH, _YEAR };

//-------------------------------------[ Date Constructor ]
LUA_CONSTRUCTOR(Datetime) {
	Datetime *d = calloc(1, sizeof(Datetime));

	if (lua_gettop(L) == 4) {
		d->st = lua_touserdata(L, 4);
		d->update = lua_touserdata(L, 3);
		d->owner = lua_touserdata(L, 2);
	}
	else {
		d->st = calloc(1, sizeof(SYSTEMTIME));
		if (lua_gettop(L) == 1)
			GetLocalTime(d->st);
		else if (lua_isuserdata(L, 2))
			*d->st = *(SYSTEMTIME*)lua_touserdata(L, 2);
		else {
			wchar_t *str = lua_towstring(L, 2);
			DATE date;
			if (FAILED(VarDateFromStr(str, LOCALE_USER_DEFAULT, 0, &date)))
				luaL_error(L, "Date format not recognized");
			VariantTimeToSystemTime(date, d->st);
			free(str);
		}
	}
	lua_newinstance(L, d, Datetime);
	return 1;
}

//-------------------------------------[ Date.format() ]
#ifndef _MSC_VER
typedef int (__attribute__((stdcall)) *formatfunc)(LCID,  DWORD,  const SYSTEMTIME *, const WCHAR *, WCHAR *, int);
#else
typedef int (__stdcall *formatfunc)(LCID,  DWORD,  const SYSTEMTIME *, const WCHAR *, WCHAR *, int);
#endif

static void Datetimeformat(lua_State *L, wchar_t *format, formatfunc ffunc) {
	wchar_t * buffer;
	Datetime *d = lua_self(L, 1, Datetime);
	int len = ffunc(LOCALE_USER_DEFAULT, 0, d->st, format, NULL, 0);
	buffer = malloc(sizeof(wchar_t)*len);
	ffunc(LOCALE_USER_DEFAULT, 0, d->st, format, buffer, len);
	lua_pushlwstring(L, buffer, len-1);
	free(buffer);
	free(format);
}

LUA_METHOD(Datetime, format) {
	int n, len = -1;
	const char *datef = luaL_optstring(L, 2, NULL);
	const char *timef = luaL_optstring(L, 3, NULL);
	
	n = lua_gettop(L);
	if (datef)
		Datetimeformat(L, utf8_towchar(datef, &len), GetDateFormatW);
	if (timef)
		Datetimeformat(L, utf8_towchar(timef, &len), GetTimeFormatW);
	len = lua_gettop(L)-n;
	if (len == 0) {
		Datetimeformat(L, NULL, GetDateFormatW);
		Datetimeformat(L, NULL, GetTimeFormatW);
		return 2;
	}
	return len;
}

//-------------------------------------[ Date.interval() ]
LUA_METHOD(Datetime, interval) {
	Timeunion t1, t2;
	int interval = luaL_checkoption(L, 3, "days", intervals);

	SystemTimeToFileTime(lua_self(L, 1, Datetime)->st, &t2.fTime);
	SystemTimeToFileTime(lua_self(L, 2, Datetime)->st, &t1.fTime);
	lua_pushinteger(L, (lua_Integer)((t2.ul.QuadPart - t1.ul.QuadPart)/intervals_values[interval]));
	return 1;
}

//-------------------------------------[ Date Properties ]
LUA_PROPERTY_GET(Datetime, time) {
	Datetimeformat(L, NULL, GetTimeFormatW);
	return 1;
}

LUA_PROPERTY_GET(Datetime, date) {
	Datetimeformat(L, NULL, GetDateFormatW);
	return 1;
}

LUA_PROPERTY_GET(Datetime, dayname) { Datetimeformat(L, _wcsdup(L"dddd"), GetDateFormatW); return 1; }
LUA_PROPERTY_GET(Datetime, monthname) { Datetimeformat(L, _wcsdup(L"MMMM"), GetDateFormatW); return 1; }

typedef struct {
	WORD	*field;
	int		offset;
	int 	idx;
} Datefield;

static const char *props[] = { "day", "weekday", "hour", "milliseconds", "minute", "month", "second", "year", NULL };

static Datefield get_datefield(lua_State *L, Datetime *d) {
	WORD *value[] = { &d->st->wDay, &d->st->wDayOfWeek, &d->st->wHour, &d->st->wMilliseconds, &d->st->wMinute, &d->st->wMonth, &d->st->wSecond, &d->st->wYear };
	int idx = lua_optstring(L, 2, props, -1);
	Datefield df;

	df.field = idx == -1 ? NULL : value[idx];
	df.offset = idx == 1;
	df.idx = idx;
	return df;
}

#ifdef _WIN32

#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)

static inline BOOL IsLeapYear(int Year)
{
    return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0);
}

#endif

LUA_METHOD(Datetime, index) {	
	Datetime *d = lua_self(L, 1, Datetime);
	Datefield df = get_datefield(L, d);
	if (df.field)
		lua_pushinteger(L, (*df.field) + df.offset);
	return df.field != NULL;
}

LUA_METHOD(Datetime, newindex) {	
	Datetime *d = lua_self(L, 1, Datetime);
	Datefield df = get_datefield(L, d);
	static WORD month_days[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (df.field) { 
		*df.field = (WORD)lua_tointeger(L,3)-df.offset;
	   	if ((d->st->wMonth == 0 || d->st->wMonth > 12) ||  d->st->wDay == 0 || d->st->wDay > month_days[d->st->wMonth] || d->st->wHour > 23 || d->st->wMinute > 59 || d->st->wSecond > 59 || d->st->wMilliseconds > 999 || (d->st->wMonth == 2 && d->st->wDay == 29 && !IsLeapYear(d->st->wYear)) )
    		luaL_error(L, "invalid Datetime.%s value",  props[df.idx]);
		if (d->owner)
			d->update(L, d->owner);
	}
	return 0;
}

LUA_METHOD(Datetime, tostring) {
	Datetime_format(L);
	lua_pushstring(L, " ");
	lua_insert(L, -2);
	lua_concat(L, 3);
	return 1;
}

LUA_METHOD(Datetime, concat) {
	luaL_tolstring(L, 1, NULL);
	luaL_tolstring(L, 2, NULL);
	lua_concat(L, 2);
	return 1;
}

LUA_METHOD(Datetime, eq) {	
	lua_pushboolean(L, memcmp(lua_self(L, 1, Datetime)->st, lua_self(L, 2, Datetime)->st, sizeof(SYSTEMTIME)) == 0);
	return 1;
}

LUA_METHOD(Datetime, lt) {	
	FILETIME ft1, ft2;

	SystemTimeToFileTime(lua_self(L, 1, Datetime)->st, &ft1);
	SystemTimeToFileTime(lua_self(L, 2, Datetime)->st, &ft2);
	lua_pushboolean(L, CompareFileTime(&ft1, &ft2) == -1);
	return 1;
}

LUA_METHOD(Datetime, le) {	
	FILETIME ft1, ft2;

	SystemTimeToFileTime(lua_self(L, 1, Datetime)->st, &ft1);
	SystemTimeToFileTime(lua_self(L, 2, Datetime)->st, &ft2);
	lua_pushboolean(L, CompareFileTime(&ft1, &ft2) <= 0 );
	return 1;
}


LUA_METHOD(Datetime, gc) {
	Datetime *d = lua_self(L, 1, Datetime);
	if (!d->owner)
		free(d->st);
	free(d);
	return 0;
}

const luaL_Reg Datetime_methods[] = {
	{"format",			Datetime_format},
	{"interval",		Datetime_interval},
	{"get_time",		Datetime_gettime},
	{"get_date",		Datetime_getdate},
	{"get_dayname",		Datetime_getdayname},
	{"get_monthname",	Datetime_getmonthname},
	{NULL, NULL}
};

const luaL_Reg Datetime_metafields[] = {
	{"__metaindex", Datetime_index},
	{"__metanewindex", Datetime_newindex},
	{"__tostring",	Datetime_tostring},
	{"__concat",	Datetime_concat},
	{"__gc",		Datetime_gc},
	{"__eq",		Datetime_eq},
	{"__lt",		Datetime_lt},
	{"__le",		Datetime_le},
	{NULL, NULL}
};