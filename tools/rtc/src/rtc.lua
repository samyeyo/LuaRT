-- | RTc - Lua script to executable compiler
-- | Luart.org, Copyright (c) Tine Samir 2025
-- | See Copyright Notice in LICENSE.TXT
-- |---------------------------------------------------
-- | RTc.lua | LuaRT executable compiler

local zip = require "compression"

local output				-- output executable filename
local icon					-- executable icon
local directory				-- Directory to be embedded in the executable
local file					-- main Lua File to be executed when running the executable
local target = "luart.exe"	-- target interpreter for console or window subsystem
local setoutput, set_icon, static, forceconsole = false, false, false, false
local console, gui

local libs = {}
local files = {}

if (arg[-1]:find("wrtc") or arg[-1]:find("wluart")) == nil then
	console = require "console"
	if arg[1] == nil then
		console.writecolor("lightblue", "rt")
		console.writecolor('yellow', "c ")
		print(_VERSION:match(" (.*)$")..[[ - Lua script to executable compiler.
Copyright (c) 2025, Samir Tine.
	
usage:	rtc.exe [-s][-c][-w][-i icon][-o output] [-lmodname] [directory] main.lua [file1.lua file2.lua...]
	
	-s		create static executable (without LUA54.DLL dependency)
	-c		create executable for console (default)
	-w		create executable for Windows desktop
	-i icon		set executable icon (expects an .ico file)
	-o output	set executable name to 'output'
	-lmodname	link the LuaRT binary module 'modname.dll'
	directory	the content of the directory to be embedded in the executable
	main.lua   	the Lua script to be executed]])
		sys.exit()
	end

	------------------------------------| Parse commande line
	for i=1, #arg do
		local option = arg[i]
		if setoutput then
			output = option
			setoutput = false
		elseif set_icon then
			icon = option
			set_icon = false
		elseif option == "-i" then
			set_icon = true
		elseif option == "-c" then
			forceconsole = true
			target = "luart.exe"
		elseif option == "-w" then
			target = "wluart.exe"
		elseif option == "-o" then
			setoutput = true
		elseif option == "-s" then
			static = true
		elseif option:find("-l") == 1 then
			libs[#libs+1] = option:match("%-l(%w+)")
		elseif option:usub(1,1) == "-" then 
			print("invalid option "..option)
			sys.exit(-1)
		elseif directory == nil and not sys.File(option).exists then
			directory = sys.Directory(option)
			if not directory.exists then
				error("cannot find file "..option)
			end
		else 
			local f = sys.File(option)
			if not f.exists then
				error("cannot find file "..option)
			end
			files[#files+1] = f
		end
	end
else
	ui = require "ui"
	local result = require("gui")
	file, directory, target, output, icon, libs = table.unpack(result)
	files = { file }
end

file = files[1]

if #libs > 0 and static then
	error("could not link modules in static mode")
end

if file == nil then
	error("no input file")
end

if file.extension == ".wlua" and target == "luart.exe" and not forceconsole then
	target = "wluart.exe"
end

if static then
	if target == "luart.exe" then
		target = "luart-static.exe"
	else
		target = "wluart-static.exe"
	end
end

if embed == nil then
	target = sys.File(sys.File(arg[-1]).path..target)
else
	target = embed.File(target)
end

if not target.exists then
    error("Fatal error: compilation failed, check your LuaRT installation")
end

local fs = sys.tempfile("luartc_")
local z = zip.Zip(fs.fullpath, "write")

local _error = error

local function error(err)
	_error(err:gsub('%[string "([%w%.]+)"%]', "%1"))
end


for f in each(files) do
	local content = f:open():read()
	local result, err = load(content, f.name)
	if result == nil then
		error(err)
	end
	local fname = sys.tempfile("luartc_file_")
	fname:open("write", "binary")
	fname:write(content)
	fname:close()
	z:write(fname, f.name)
	fname:remove()
end

local fname = sys.tempfile("luartc_file_"):open("write", "binary")
fname:write("load(embed.File('"..files[1].name.."'):open():read(), '"..files[1].name.."')()")
fname:close()
z:write(fname, "__mainLuaRTStartup__.lua")
fname:remove()


for lib in each(libs) do
	local libpath = sys.Directory(sys.File(arg[-1]).path.."/../modules/").exists and sys.File(arg[-1]).path.."/../modules/" or sys.File(arg[-1]).path.."/modules/"
	local libfile = sys.File(libpath..lib.."/"..lib..".dll")
	if not libfile.exists then
		libfile = sys.File(libpath.."/"..lib..".dll")
		if not libfile.exists then
			libfile = sys.File(lib..".dll")
		end
	end
	if libfile.exists then
		z:write(libfile)
	else
		error("module '"..lib.."' not found")
	end
end

if directory ~= nil then
	z:write(directory)
end
z:close()

output = sys.File(output or file.name:gusub("(%w+)$", "exe"))
output:remove()
target:copy(output.fullpath)
target:remove()

if icon ~= nil then
	seticon(output.fullpath, icon)
end

if link(fs.fullpath, output.fullpath) == false then
	error('Cannot write "'..output.fullpath..'" to disk')
end

if ui then
	print("Successfully compiled "..output.name.." ("..math.floor(output.size/1024).."Kb)")
else
	print(output.name)
end
fs:close()
fs:remove()