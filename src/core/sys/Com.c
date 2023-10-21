/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | COM.c | LuaRT COM Objects implementation
*/

#define CINTERFACE
#define COBJMACROS
#define LUA_LIB

#include <com.h>
#include <luart.h>

#include <ole2.h>
#include <oleauto.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <propvarutil.h>
#include <stdint.h>

luart_type TCOM;

//-------------------------------------[ COM Constructor ]
LUA_CONSTRUCTOR(COM) {
	COM *obj = (COM *)calloc(1, sizeof(COM));
	UINT count;
	CLSID clsid;
	char *err;


	if (lua_islightuserdata(L, 2))
		obj->this = lua_touserdata(L, 2);
	else {
		wchar_t *name = lua_towstring(L, 2);
		if ( FAILED(CLSIDFromProgID(name, &clsid)) && FAILED(CLSIDFromString(name, &clsid)) ) {
notfound:	err = "COM object '%s' not registered";
			goto error;
		} else if ( lua_toboolean(L, 3) ) {
			if (FAILED(GetActiveObject(&clsid, NULL, &obj->getobject)))
				goto notfound;
			if (FAILED(IUnknown_QueryInterface(obj->getobject, &IID_IDispatch, (LPVOID*)&obj->this)))
				goto notfound;
		} else if ( FAILED(CoCreateInstance(&clsid, 0, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, &IID_IDispatch, (LPVOID*)&obj->this))) {
			err = "IDispatch interface not implemented by COM object '%s'";
error:		lua_pushfstring(L, err, lua_tostring(L, 2));		
			free(name);
			free(obj);
			lua_error(L);
		}					
		free(name);
	}
	if ( SUCCEEDED(IDispatch_GetTypeInfoCount(obj->this, &count)) && count && SUCCEEDED(IDispatch_GetTypeInfo(obj->this, 0, 0, &obj->typeinfo)) ) {
		BSTR bstr = NULL;
		if ( SUCCEEDED(ITypeInfo_GetDocumentation(obj->typeinfo, MEMBERID_NIL, &bstr, NULL, NULL, NULL)) ) {
			obj->name = wcsdup((LPCWSTR)bstr);
			SysFreeString(bstr);
		}
	}
	lua_newinstance(L, obj, COM);
	return 1;
}

static int COM_method_call(lua_State *L) {
	COM				*obj;
	wchar_t 		*field = lua_towstring(L, lua_upvalueindex(1));
	int 			method = lua_tointeger(L, lua_upvalueindex(2));
	HREFTYPE 		hreftype = lua_tointeger(L, lua_upvalueindex(4));
	int 			n = lua_gettop(L);
	int				idx, i;
	DISPPARAMS 		params = {NULL, NULL, 0, 0};
	DISPID			id, id_setprop = DISPID_PROPERTYPUT;
	EXCEPINFO		execpInfo = {0};
	UINT			puArgErr = 0;
	wchar_t 		*str;
	VARIANT			result = {0};
	HRESULT 		hr = S_OK;
	VARIANT			*args = NULL;
	ITypeInfo 		*t = NULL;
	int restype = -1;
	TYPEATTR *attr = NULL;	
	
	if (method & DISPATCH_METHOD) {
		obj = lua_self(L, 1, COM);
		if (method & DISPATCH_PROPERTYGET) {
			id_setprop = DISPID_VALUE;
			params.cNamedArgs = 1;
			params.rgdispidNamedArgs = &id_setprop;	
		}
		n--;
		idx = 2;	
	} else {
		obj = lua_self(L, lua_upvalueindex(3), COM);
		idx = 1;
	}

	if (hreftype && SUCCEEDED(ITypeInfo_GetRefTypeInfo(obj->typeinfo, hreftype, &t)))
		if (SUCCEEDED(ITypeInfo_GetTypeAttr(t, &attr)))
			restype = attr->typekind;
		else
			restype = -1;
	params.cArgs = n;
	args = calloc(n+1, sizeof(VARIANT));
	for (i = (n - 1); i >= 0; i--) {
		VARIANT *var;
		var = &args[i];
		VariantInit(var);
		switch(lua_type(L, idx)) {
			case LUA_TNIL:		var->vt = VT_NULL; break;
			case LUA_TBOOLEAN:	var->vt = VT_BOOL; var->boolVal = lua_toboolean(L, idx); break;
number:		case LUA_TNUMBER:	if (lua_isinteger(L, idx)) {
									var->vt = VT_I4;
									V_I4(var) = (__int32)lua_tointeger(L, idx);
								} else {
									var->vt = VT_R8;
									var->dblVal = lua_tonumber(L, idx);
								}
								break;
			case LUA_TSTRING:	if (lua_isnumber(L, idx))
									goto number;
								str = lua_towstring(L, idx);
								if ((method & DISPATCH_PROPERTYPUT) && restype == TKIND_ENUM) {
									wchar_t *name;
									BOOL done = FALSE;
									VARDESC *vardesc;
									for (UINT i = 0; !done && (i < attr-> cVars); i++) {
										ITypeInfo_GetVarDesc(t, i, &vardesc);
										ITypeInfo_GetDocumentation(t, vardesc->memid, &name, NULL, NULL, NULL);
										if (wcscmp(str, name) == 0) {
											var->vt = VT_I4;
											V_I4(var) = (__int32)vardesc->lpvarValue->intVal;
											done = TRUE;
										}
										SysFreeString(name);
										ITypeInfo_ReleaseVarDesc(t, vardesc);									
									}
								} else {
									V_BSTR(var) = SysAllocString(str);
									V_VT(var) = VT_BSTR;
								}
								free(str);
								break;
			case LUA_TTABLE:	{
									COM *o = lua_self(L, idx, COM);
									V_DISPATCH(var) = o->this;
									IDispatch_AddRef(o->this);
									if (method & DISPATCH_PROPERTYPUT)
										method = DISPATCH_PROPERTYPUTREF;
									V_VT(var) = VT_DISPATCH;
									break;
								} 
			default: luaL_error(L, "COM error : unsupported Lua type"); 
		}
		idx++;
	}
	params.rgvarg = args;
	if ( SUCCEEDED( (hr = IDispatch_GetIDsOfNames(obj->this, &IID_NULL, &field, 1, 0, &id)) ) ) {
		if ((method & DISPATCH_PROPERTYPUT) || (method & DISPATCH_PROPERTYPUTREF)) {
			params.cNamedArgs = 1;
			params.rgdispidNamedArgs = &id_setprop;
		}
		VariantInit(&result);
		if (FAILED( (hr = IDispatch_Invoke(obj->this, id, &IID_NULL, 0, method, &params, &result, &execpInfo, &puArgErr)) )) {			
			switch(hr) {
				case DISP_E_BADPARAMCOUNT:	lua_pushstring(L, "bad parameter count"); break;
				case DISP_E_MEMBERNOTFOUND:	lua_pushfstring(L, "no member '%s' found", lua_tostring(L, lua_upvalueindex(1)));
											break;
				case DISP_E_EXCEPTION:		if (id_setprop == DISPID_VALUE) {
												hr = S_OK;
												goto done;
											}
											lua_pushwstring(L, (LPCWSTR)execpInfo.bstrDescription);
											SysFreeString(execpInfo.bstrDescription);
											if (execpInfo.bstrHelpFile)
												SysFreeString(execpInfo.bstrHelpFile);
											if (execpInfo.bstrSource)
												SysFreeString(execpInfo.bstrSource);
											break;
				case DISP_E_BADVARTYPE:		lua_pushstring(L, "Bad argument type");							
				case DISP_E_TYPEMISMATCH:	lua_pushfstring(L, "type mismatch for parameter %d", puArgErr); break;
				default: 					lua_pushstring(L, "unknown error");
			} 
		} else switch(result.vt) {
			case VT_EMPTY:
done:		case VT_NULL:		lua_pushnil(L); break;
			case VT_BSTR:		lua_pushwstring(L, (LPCWSTR)V_BSTR(&result)); break;						
			case VT_BOOL:  		lua_pushboolean(L, result.boolVal); break;
			case VT_I1:
			case VT_I2:
			case VT_I4:
			case VT_I8:
			case VT_INT:		if (restype == TKIND_ENUM) {
									wchar_t *name;
									VARDESC *vardesc;
									BOOL done = FALSE;

									for (UINT i = 0; !done && (i < attr->cVars); i++) {
										ITypeInfo_GetVarDesc(t, i, &vardesc);
										if (V_I8(&result) == vardesc->lpvarValue->intVal) {
											ITypeInfo_GetDocumentation(t, vardesc->memid, &name, NULL, NULL, NULL);
											lua_pushwstring(L, name);
											done = TRUE;
											SysFreeString(name);
										}
										ITypeInfo_ReleaseVarDesc(t, vardesc);									
									}									
								} else {
									VariantChangeType(&result, &result, 0, VT_I8);
									lua_pushinteger(L, V_I8(&result));
								}
								break;
			case VT_R4:
			case VT_R8:
			case VT_DECIMAL:	VariantChangeType(&result, &result, 0, VT_R8);
								lua_pushnumber(L, V_R8(&result)); break;
			case VT_DISPATCH:	{
									lua_pushlightuserdata(L, result.pdispVal);
									lua_pushinstance(L, COM, 1);
									goto cleanup;
								}
			case VT_DATE:		{
									SYSTEMTIME st;
									VariantTimeToSystemTime(V_DATE(&result), &st);
									lua_pushlightuserdata(L, &st);
									lua_pushinstance(L, Datetime, 1);
									goto cleanup;
								}
			default: 			luaL_error(L, "COM error : unsupported result type %d", result.vt); 
		}
	} else
		lua_pushfstring(L, "COM error : no member '%s' found", lua_tostring(L, lua_upvalueindex(1)));
	VariantClear(&result);
cleanup:
	if (attr)
		ITypeInfo_ReleaseTypeAttr(t, attr);
	if (t)
		ITypeInfo_Release(t);
	free(field);
	while (n--)
		VariantClear(&params.rgvarg[n]);
	free(params.rgvarg);
	if (FAILED(hr))
		luaL_error(L, "COM error: %s", lua_tostring(L, -1));
	return 1;
}

static BOOL GetResultType(COM *obj, wchar_t *field, HREFTYPE *restype) {
	BOOL found = FALSE;
	TYPEATTR	*attr;
	FUNCDESC	*funcdesc = NULL;
	BSTR name;
	
	if ( obj->typeinfo && SUCCEEDED(ITypeInfo_GetTypeAttr(obj->typeinfo, &attr))) {
		UINT count;
		for (WORD i = 0; i < attr->cFuncs; i++)
			if ( SUCCEEDED(ITypeInfo_GetFuncDesc(obj->typeinfo, i, &funcdesc)) && SUCCEEDED(ITypeInfo_GetNames(obj->typeinfo, funcdesc->memid, &name, 1, &count)) ) {
				if (lstrcmpiW(name, field) == 0) {
					if (funcdesc->elemdescFunc.tdesc.vt == 29)
						*restype = funcdesc->elemdescFunc.tdesc.hreftype;
					if ((funcdesc->invkind == INVOKE_FUNC) && (lstrcmpiW(name, field) == 0)) {
						ITypeInfo_ReleaseFuncDesc(obj->typeinfo, funcdesc);
						found = TRUE;
						break;
					}
				}
				ITypeInfo_ReleaseFuncDesc(obj->typeinfo, funcdesc);
			}
	}
	if (!found)
		restype = NULL;
	ITypeInfo_ReleaseTypeAttr(obj->typeinfo, attr);			
	return found;	
}

LUA_METHOD(COM, __newindex) {
	wchar_t		*field = lua_towstring(L, 2);
	HREFTYPE restype;

	lua_pushvalue(L, 2);
	lua_pushinteger(L, INVOKE_PROPERTYPUT);
	lua_pushvalue(L, 1);
	GetResultType(lua_self(L, 1, COM), field, &restype);
	free(field);					
	lua_pushinteger(L, restype);
	lua_pushcclosure(L, COM_method_call, 4);
	lua_pushvalue(L, 3);
	lua_call(L, 1, 0);	
	return 0;
}

LUA_METHOD(COM, __index) {
	COM	*obj = lua_self(L, 1, COM);
    wchar_t		*field = lua_towstring(L, 2);
	DISPID		id;
	DISPPARAMS 	params = {NULL, NULL, 0, 0};
	EXCEPINFO	execpInfo = {0};
	UINT		puArgErr = 0;
	VARIANT		result = {0};
	HREFTYPE	restype = (HREFTYPE)0;

	if (SUCCEEDED(IDispatch_GetIDsOfNames(obj->this, &IID_NULL, &field, 1, 0, &id))) {
		if (GetResultType(obj, field, &restype))
			goto method;			
		LONG value = IDispatch_Invoke(obj->this, id, &IID_NULL, 0, DISPATCH_PROPERTYGET, &params, &result, &execpInfo, &puArgErr);
		if (value == DISP_E_BADPARAMCOUNT) {
			lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_PROPERTYGET | DISPATCH_METHOD);
			lua_pushvalue(L, 1);
			lua_pushinteger(L, restype);			
			lua_pushcclosure(L, COM_method_call, 4);
		} else if (value == 0) {		
			VariantInit(&result);	
			lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_PROPERTYGET);
			lua_pushvalue(L, 1);
			lua_pushinteger(L, restype);			
			lua_pushcclosure(L, COM_method_call, 4);
			lua_call(L, 0, 1);
		} else {
method:		lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_METHOD);
			lua_pushinteger(L, restype);
			lua_pushcclosure(L, COM_method_call, 3);
		}
	} else lua_pushnil(L);
	free(field);
	return 1;
}

static int COM_iter(lua_State *L) {
	COM	*obj = lua_self(L, lua_upvalueindex(1), COM);
	int 		i = lua_tointeger(L, lua_upvalueindex(2));
	int 		max = lua_tointeger(L, lua_upvalueindex(3));
	FUNCDESC 	*funcdesc = NULL;	
    UINT 		count; 				

	if (i < max) {
		if ( SUCCEEDED(ITypeInfo_GetFuncDesc(obj->typeinfo, i, &funcdesc)) ) {
			BSTR name;

			ITypeInfo_GetNames(obj->typeinfo, funcdesc->memid, &name, 1, &count);
			lua_pushwstring(L, (LPOLESTR)name);
			lua_pushstring(L, funcdesc->invkind & INVOKE_FUNC ? "function" : (funcdesc->invkind > INVOKE_PROPERTYGET ? "set property" : "get property"));
			lua_pushinteger(L, ++i);
			lua_replace(L, lua_upvalueindex(2));
			ITypeInfo_ReleaseFuncDesc(obj->typeinfo, funcdesc);
			return 2;
		}
	}
	return 0;
}

LUA_METHOD(COM, __iterate) {
	TYPEATTR* attr;
	COM *obj = lua_self(L, 1, COM);
	if ( obj->typeinfo && SUCCEEDED(ITypeInfo_GetTypeAttr(obj->typeinfo, &attr)) && attr->cFuncs ) {
		lua_pushvalue(L, 1);
		lua_pushinteger(L, 0);
		lua_pushinteger(L, attr->cFuncs);
		lua_pushcclosure(L, COM_iter, 3);
		ITypeInfo_ReleaseTypeAttr(obj->typeinfo, attr);
		return 1; 
	} 
	luaL_where(L, 2);
	lua_pushstring(L, "COM error : this COM is not iterable");
	lua_concat(L, 2);
	lua_error(L);
	return 0;
}

LUA_METHOD(COM, __tostring) {
	lua_pushstring(L, "COM: <");
	lua_pushwstring(L, lua_self(L, 1, COM)->name);
	lua_pushstring(L, ">");
	lua_concat(L, 3);
	return 1; 
}

LUA_METHOD(COM, __gc) {
	COM *obj = lua_self(L, 1, COM);
	if (obj->typeinfo)
		ITypeInfo_Release(obj->typeinfo);
	if (obj->this)
		IDispatch_Release(obj->this);
	if (obj->getobject)
		IUnknown_Release(obj->getobject);
	free(obj->name);
	free(obj);
	return 0;
}

const luaL_Reg COM_metafields[] = {
	{"__gc", 			COM___gc},
	{"__metaindex", 	COM___index},
	{"__metanewindex", 	COM___newindex},
	{"__iterate", 		COM___iterate},
	{"__tostring", 		COM___tostring},
	{NULL, NULL}
};

const luaL_Reg COM_methods[] = {
	{NULL, NULL}
};