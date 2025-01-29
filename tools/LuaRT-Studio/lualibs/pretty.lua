-- | LuaRT - A Windows programming framework for Lua
-- | Luart.org, Copyright (c) Tine Samir 2025.
-- | See Copyright Notice in LICENSE.TXT
-- |------------------------------------------------
-- | pretty.lua | Pretty print a LuaRT value




function pretty(value, isobj, name)
  local t = type(value)
  local mt = getmetatable(value) or false
  local isobj = isobj or false
  local result = ""
  
  if t == "string" then
    if value:ufind("[\r\n]") then
      result = "[["..value:gusub("\\","/").."]]"
    else
      result = '"'..value:gusub("\\","/")..'"'
    end
  elseif t == "number" or t == "boolean" then
    result = tostring(value)
  elseif t == "function" then
    if isobj then
      local meta = getmetatable(isobj)
      result = name:ufind("^[gs]et_") and (meta and meta.__type and isobj[name:ufind("^[gs]et_")] or "property") or (meta and (meta.__type or meta.__mt) and "method" or "function")
    else
      result = t
    end
  elseif name == nil and t ~= "userdata" and (t == "table" or mt) then
    local out = {}
    local obj = type(mt) == "table" and ((mt.__type or mt.__properties) or (type(mt.__name) == "string"))
    result = "{"
    if #value > 0 then
      result = result.."\n  "
      for k, v in ipairs(value) do
        result = result..pretty(v, obj, k)..", "
        out[k] = v
      end
    end
    local n = 0
    for k, v in pairs(value) do
      local prop = k
      if obj then           
        prop, n = k:gsub("^set_", "get_"):gsub("^[gs]et_", "")
      end
      local space = string.len(prop) < 6 and "\t\t\t= " or "\t\t= "      
      if out[prop] == nil then
        if obj == true then
          result = result.."\n  "..prop..space..pretty(v, value, k)..","
        else
          result = result.."\n  "..prop..space..pretty(n and value[prop] or v, value, k)..","
        end
        out[prop] = v
      end
    end
    n = 0
    if obj ~= false and obj ~= true then
      local isobject = mt.__name == "Object"
      for k, v in pairs(obj) do
        local prop, n = k:gsub("^set_", "get_"):gsub("^[gs]et_", "")
        local space = string.len(prop) < 6 and "\t\t\t= " or "\t\t= "      
        if isobject and out[prop] == nil then
          result = result.."\n  "..prop..space..pretty(v, value, k)..","
        elseif out[prop] == nil then
          result = result.."\n  "..prop..space..pretty(n and value[prop] or v, value, k)..","
        end
        out[prop] = v
      end
    end
    result = result.."}"
    result = result:gsub(",%s*}", "\n}")
  elseif t == "table" then
    result = "{}"
  else
    result = t
  end
  return result
end