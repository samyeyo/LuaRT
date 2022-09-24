local net = require "net"
local console = require "console"
local db = require "help_data"

if #arg == 1 then
	console.writecolor("brightwhite", "Lua")
	console.writecolor('yellow', "RT")
	print([[ 0.9.8 - LuaRT interactive help.
Copyright (c) 2022, Samir Tine.

usage:	rthelp.exe topic	

	topic   search for help topic 'topic' in LuaRT documentation]])
	sys.exit()
end


local function searchfor(name, pattern, func)
	for key, item in pairs(db) do
		if key:search(name) and key:match(pattern) then
			func(key, item)
			return true
		end
	end
	return false
end

local function write_module(name, mod)
	console.writecolor("cyan", "\nHelp item  : ")
	console.writecolor("brightwhite", name:match("%a+"))
	name = name:match("%a+").."."
	console.writecolor("cyan", "\nCategory   : ")
	console.writecolor("gray", "Module\n")
	console.writecolor("cyan", "\nDescription: ")
	console.write(mod.desc)
	console.writecolor("cyan", "\n\nContains: ")
	for key, item in pairs(db) do
		if key:search(name) and not key:search("constructor") then
			local helpitem = key:gsub(name, ""):gsub("%s?%b()", "()"):gsub("readonly property", ""):gsub("read/write property", ""):gsub("iterator", "")
			if helpitem:lower() == helpitem  then
				console.write(helpitem.." ")
			else
				console.writecolor("lightcyan", helpitem.." ")
			end
		end
	end
end

local function write_property(name, prop)
	console.writecolor("cyan", "\nHelp item  : ")
	console.writecolor("brightwhite", name:match("[%.%a]+"))
	console.writecolor("cyan", "\nCategory   : ")
	local proptype = name:match(" (.- property)")
	console.writeln(proptype:capitalize())
	console.writecolor("cyan", "\nDescription: ")
	console.write(prop.desc)
end

local function write_object(name, obj)
	console.writecolor("cyan", "\nHelp item  : ")
	console.writecolor("brightwhite", name:match("%.(%a+)"))
	console.writecolor("cyan", "\nCategory   : ")
	console.writecolor("gray", "Object\n")
	console.writecolor("cyan", "Module     : ")
	console.writecolor("yellow", name:match("(%a+)%."))
	console.writecolor("cyan", "\n\nDescription: ")
	console.write(obj.desc)
end

local function write_method(name, func)
	console.writecolor("cyan", "\nHelp item  : ")
	local n = name:gsub("%b()", ""):gsub(" method", ""): gsub(" : ", ":")
	console.writecolor("brightwhite", n)
	console.writecolor("gray", name:match("%([%a%p%s]+%)") or "()")
	console.writecolor("cyan", "\nCategory   : ")
	console.writecolor("gray", "Method\n")
	console.writecolor("cyan", "\nDescription: ")
	console.writeln(func.desc)
	if func.params[1] ~= nil then
		console.writecolor("cyan", "Parameters: ")
		for i, param in ipairs(func.params) do
			console.writecolor(param.color, "\n   "..param.name..":")
			console.write(string.rep(" ", 10-#param.name))
			console.writecolor("white", param.desc)
		end
	end
end

local function write_function(name, func)
	console.writecolor("cyan", "\nHelp item  : ")
	console.writecolor("brightwhite", name:match("^([%a%.]+)%s*%("))
	console.writecolor("gray", (name:match("%([%a%p%s]+%)") or "()"))
	console.writecolor("cyan", "\nCategory   : ")
	console.writecolor("gray", "Function\n")
	local mod = name:match("(%a+)%.")
	if mod ~= nil then
		console.writecolor("cyan", "Module     : ")
		console.writecolor("yellow", mod)
	end
	console.writecolor("cyan", "\nDescription: ")
	console.writeln(func.desc)
	if func.params[1] ~= nil then
		console.writecolor("cyan", "Parameters: ")
		for i, param in ipairs(func.params) do
			console.writecolor(param.color, "\n   "..param.name..":")
			console.write(string.rep(" ", 10-#param.name))
			console.writecolor("white", param.desc)
		end
	end
end

local function search(helpitem)
	if helpitem == nil then
		console.write("no help entry provided.")
	else
		local result = searchfor(helpitem, "module", write_module) or
		searchfor(helpitem, "^"..helpitem.."%(", write_function) or
		searchfor(helpitem:capitalize(), "^%a+%.%u%a+$", write_object) or
		searchfor(helpitem, "^%a+%.%u%a+$", write_object) or
		searchfor(helpitem:capitalize(), "property", write_property) or
		searchfor(helpitem, "property", write_property) or
		searchfor(helpitem:capitalize():gsub("%.", ":"), "method", write_method) or
		searchfor(helpitem, "method", write_method) or
		searchfor(helpitem, "%(", write_function)
		if not result then
			console.writecolor("lightred", "no help entry for ")
			console.writecolor("lightgreen", helpitem)
		end
	end
	console.write("\n")
end

search(arg[2])
