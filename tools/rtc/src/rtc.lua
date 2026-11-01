-- | RTc - Lua script to executable compiler
-- | Luart.org, Copyright (c) Tine Samir 2026
-- | See Copyright Notice in LICENSE.TXT
-- |---------------------------------------------------
-- | RTc.lua | LuaRT executable compiler

local zip = require "compression"

local output				-- output executable filename
local icon					-- executable icon
local directory	= ""		-- Directory to be embedded in the executable
local file					-- main Lua File to be executed when running the executable
local target = "luart.exe"	-- target interpreter for console or window subsystem
local setoutput, set_icon, forceconsole = false, false, false
local console, gui
static = false

libs = {}
local files = {}
local libpaths = { sys.File(arg[-1]).directory.parent.fullpath.."/modules" }

if (arg[-1]:find("wrtc") or arg[-1]:find("wluart")) == nil then
	console = require "console"
	if arg[1] == nil then
		console.writecolor("lightblue", "rt")
		console.writecolor('yellow', "c ")
		print(_VERSION:match(" (.*)$")..[[ - Lua script to executable compiler.
Copyright (c) 2026, Samir Tine.
	
usage:	rtc.exe [-s][-c][-w][-i icon][-o output] [-lmodname] [directory] main.lua [file1.lua file2.lua...]
	
	-s		create static executable (without LUA55.DLL dependency)
	-c		create executable for console (default)
	-w		create executable for Windows desktop
	-i icon		set executable icon (expects an .ico file)
	-o output	set executable name to 'output'
	-lmodname	link the LuaRT binary module 'modname.dll'
	-Lmoddir	add a new search path for modules
	directory	the content of the directory to be embedded in the executable
	main.lua   	the Lua script to be executed]])
		sys.exit()
	end

	local function normalize(path)
		return path:gsub('^"(.*)"$', "%1"):gsub("^'(.*)'$", "%1"):gsub("\\", "/")
	end

	------------------------------------| Parse commande line
	for i=1, #arg do
		local option = arg[i]
		if setoutput then
			output = normalize(option)
			setoutput = false
		elseif set_icon then
			icon = normalize(option)
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
		elseif option:find("-L") == 1 then
			libpaths[#libpaths+1] = normalize(option:sub(3, -1))
		elseif option:usub(1,1) == "-" then 
			print("invalid option "..option)
			sys.exit(-1)
		elseif directory == "" and #files > 0 and not sys.File(option).exists then
			directory = sys.Directory(normalize(option))
			if not directory.exists then
				error("cannot find directory "..option)
			end
		else 
			local f = sys.File(normalize(option))
			if not f.exists then
				error("cannot find file "..option)
			end
			files[#files+1] = f
		end
	end
else
	ui = require "ui"
	local data = require("gui")
	file, directory, target, output, icon = table.unpack(data) 
	ui.info("Compiling "..file.name.." to "..output)
	files = { file }
end

file = files[1]

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
	target = sys.File(target)
end


if not target.exists then
    error("Fatal error: compilation failed, check your LuaRT installation")
end

local fs = sys.tempfile("luartc_")
local z = zip.Zip(fs.fullpath, "write")

local _error = error

local function error(err)
	_error(err:gsub('%[rtc%.lua"([%w%.]+)"%]', "%1"))
end

local function link_file(f, path)
	local path = path or ""
	local content = f:open():read()
	local result, err = load(content, f.name)
	if result == nil then
		error(err)
	end
	local fname = sys.tempfile("luartc_file_")
	fname:open("write", "binary")
	fname:write(string.dump(result, true))
	fname:close()
	z:write(fname, (path ~= "" and path.."/"..f.name or f.name))
	fname:remove()
end

for f in each(files) do
	if directory == "" or f.directory.fullpath:usearch(directory.fullpath) == nil then
		link_file(f)
	end
end

local fname = sys.tempfile("luartc_file_"):open("write", "binary")
fname:write("load(tostring(embed.File('"..files[1].name.."'):open('read', 'binary'):read()), '"..files[1].name.."')()")
fname:close()
z:write(fname, "__mainLuaRTStartup__.lua")
fname:remove()


local ext = static and "-static.dll" or ".dll"


for lib in each(libs) do
	local libpath
	for path in each(libpaths) do
		libpath = path.."/"..lib
		if sys.Directory(libpath).exists then
			goto done
		end
	end
	for path in package.cpath:gmatch("[^;]+") do
		libpath = path:gsub("%.", "%%."):gsub("%?", lib)
		if sys.Directory(libpath).exists then
			break
		end
	end
::done::
	local _moddir = sys.Directory(libpath or "")
	if _moddir.exists then
		print("Linking "..lib.." module")
		for entry in each(_moddir) do
			if type(entry) == "File" then
				if entry.extension == ".dll" and entry.name:find(lib)  then
					if string.lower(entry.name) == lib..(static and ".dll" or "-static.dll") then
						goto continue
					end
				end
			end
			z:write(entry.fullpath, "__modules/"..lib.."/"..entry.name)
::continue::			
		end
	else
		libfile = sys.File(lib..ext)
		if not libfile.exists then
			error("Module '"..lib.."' not found")
		else
			print("Linking "..lib.." module")
			z:write(libfile)
		end
	end
end

local function link_dir(d, path)
	local path = path or ""
	for entry in each(d) do
		if type(entry) == "File" and entry.extension:match("%a?lua") then
			link_file(entry, path)
		elseif type(entry) == "Directory" then
			print("linking folder "..(path ~= "" and path.."/"..entry.name or entry.name))
			link_dir(entry, path ~= "" and path.."/"..entry.name or entry.name)
		else
			z:write(entry, path ~= "" and path.."/"..entry.name or entry.name)
		end
	end
end

if directory ~= nil or #directory > 0 then
	link_dir(directory)
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
	print("Successfully compiled "..output.name)
end
fs:close()
fs:remove()