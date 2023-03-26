/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | COM.c | LuaRT COM COM implementation
*/

#define CINTERFACE
#define COBJMACROS
#include <com.h>
#include <luart.h>

#include <ole2.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <propvarutil.h>

// #include <activscp.h>

luart_type TCOM;

//-------------------------------------[ COM Constructor ]
LUA_CONSTRUCTOR(COM) {
	COM *obj = (COM *)calloc(1, sizeof(COM));
	UINT count;
	CLSID clsid;

	if (lua_islightuserdata(L, 2))
		obj->this = lua_touserdata(L, 2);
	else {
		wchar_t *name = lua_towstring(L, 2);
		if ( FAILED(CLSIDFromProgID(name, &clsid)) && FAILED(CLSIDFromString(name, &clsid)) ) {
			lua_pushfstring(L, "COM object '%s' not registered", lua_tostring(L, 2));
			goto error;
		} else if ( FAILED(CoCreateInstance(&clsid, 0, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, &IID_IDispatch, (LPVOID*)&obj->this))) {
			lua_pushfstring(L, "IDispatch interface not implemented by COM object '%s'", lua_tostring(L, 2));		
error:		free(name);
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
	params.cArgs = n;
	args = calloc(n+1, sizeof(VARIANT));
	for (i = (n - 1); i >= 0; i--) {
		VARIANT *var;
		var = &args[i];
		VariantInit(var);
		switch(lua_type(L, idx)) {
			case LUA_TNIL:		var->vt = VT_NULL; break;
			case LUA_TBOOLEAN:	var->vt = VT_BOOL; var->boolVal = lua_toboolean(L, idx); break;
			case LUA_TNUMBER:	if (lua_isinteger(L, idx)) {
									var->vt = VT_I4;
									V_I4(var) = (__int32)lua_tointeger(L, idx);
								} else {
									var->vt = VT_R8;
									var->dblVal = lua_tonumber(L, idx);
								}
								break;
			case LUA_TSTRING:	str = lua_towstring(L, idx);
								V_BSTR(var) = SysAllocString(str);
								V_VT(var) = VT_BSTR;
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
				case DISP_E_BADPARAMCOUNT:	lua_pushstring(L, "COM error : bad parameter count"); break;
				case DISP_E_MEMBERNOTFOUND:	lua_pushfstring(L, "COM error : no member '%s' found", lua_tostring(L, lua_upvalueindex(1)));
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
				case DISP_E_TYPEMISMATCH:	lua_pushfstring(L, "COM error : type mismatch for parameter %d", puArgErr); break;
				default: 					lua_pushstring(L, "COM error : unknown error");
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
			case VT_INT:		VariantChangeType(&result, &result, 0, VT_I8);
								lua_pushinteger(L, V_I8(&result)); break;
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
			default: 			luaL_error(L, "COM error : unsupported result type"); 
		}
	} else
		lua_pushfstring(L, "COM error : no member '%s' found", lua_tostring(L, lua_upvalueindex(1)));
	VariantClear(&result);
cleanup:
	free(field);
	while (n--)
		VariantClear(&params.rgvarg[n]);
	free(params.rgvarg);
	if (FAILED(hr))
		luaL_error(L, "%s", lua_tostring(L, -1));
	return 1;
}

LUA_METHOD(COM, __newindex) {
	lua_pushvalue(L, 2);
	lua_pushinteger(L, INVOKE_PROPERTYPUT);
	lua_pushvalue(L, 1);
	lua_pushcclosure(L, COM_method_call, 3);
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
	TYPEATTR	*attr;
	FUNCDESC	*funcdesc = NULL;
	BSTR name;

	if (SUCCEEDED(IDispatch_GetIDsOfNames(obj->this, &IID_NULL, &field, 1, 0, &id))) {
		BOOL found = FALSE;
		if ( obj->typeinfo && SUCCEEDED(ITypeInfo_GetTypeAttr(obj->typeinfo, &attr))) {
		UINT count;
		for (WORD i = 0; i < attr->cFuncs; i++)
			if ( SUCCEEDED(ITypeInfo_GetFuncDesc(obj->typeinfo, i, &funcdesc)) && SUCCEEDED(ITypeInfo_GetNames(obj->typeinfo, funcdesc->memid, &name, 1, &count)) ) {
				if ((funcdesc->invkind == INVOKE_FUNC) && (lstrcmpiW(name, field) == 0)) {
					ITypeInfo_ReleaseFuncDesc(obj->typeinfo, funcdesc);
					found = TRUE;
					break;
				}
				ITypeInfo_ReleaseFuncDesc(obj->typeinfo, funcdesc);
			}
		}
		ITypeInfo_ReleaseTypeAttr(obj->typeinfo, attr);			
		if (found)
			goto method;			
		LONG value = IDispatch_Invoke(obj->this, id, &IID_NULL, 0, DISPATCH_PROPERTYGET, &params, &result, &execpInfo, &puArgErr);
		if (value == DISP_E_BADPARAMCOUNT) {
			lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_PROPERTYGET | DISPATCH_METHOD);
			lua_pushvalue(L, 1);
			lua_pushcclosure(L, COM_method_call, 3);
		} else if (value == 0) {		
			VariantInit(&result);	
			lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_PROPERTYGET);
			lua_pushvalue(L, 1);
			lua_pushcclosure(L, COM_method_call, 3);
			lua_call(L, 0, 1);
		} else {
method:		lua_pushvalue(L, 2);
			lua_pushinteger(L, DISPATCH_METHOD);
			lua_pushcclosure(L, COM_method_call, 2);
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
	if (obj->this)
		IDispatch_Release(obj->this);
	if (obj->typeinfo)
		ITypeInfo_Release(obj->typeinfo);
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