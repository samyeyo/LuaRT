--[[
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | makedist.lua | Builds the LuaRT setup executable
--]]


local console = require("console")
local compression = require("compression")

local VERSION = arg[2]
local PLATFORM = arg[3]
local dist = sys.Directory(sys.Directory(sys.currentdir).parent.path.."\\dist")
local parent = dist.parent.path
local bin = sys.Directory(dist.path.."\\bin")
dist:removeall()
dist:make()
bin:make()
sys.Directory(bin.fullpath.."\\std"):make()

console.write("Building LuaRT-"..VERSION.."-"..PLATFORM..".exe...")

local function copy(fname, new_fname)
    local f = sys.File(fname)
    local new_fname = new_fname or f.name
    f:copy(dist.path.."/"..new_fname)
end

local function file_add(fname, line)
    local file = sys.File(fname)
    local content = line.."\n"..file:open():read():gsub("local VERSION =.-\n", "")
    file:close()
    file:open("write"):write(content)
    file:close()
end

copy("../bin/lua54.dll", "bin/lua54.dll")
copy("../bin/std/lua54.dll", "bin/std/lua54.dll")
copy("../bin/luart.exe", "bin/luart.exe")
copy("../bin/rtc.exe", "bin/rtc.exe")
copy("../bin/wrtc.exe", "bin/wrtc.exe")
copy("../QuickRT/quickrt.exe", "bin/quickrt.exe")
copy("../bin/wluart.exe", "bin/wluart.exe")
copy("../bin/luart-static.exe", "bin/luart-static.exe")
copy("../bin/wluart-static.exe", "bin/wluart-static.exe")
copy(parent.."\\README.txt", "README.txt")
copy(parent.."\\LICENSE.txt", "LICENSE.txt")
copy(parent.."\\CHANGELOG.txt")
file_add("../setup/uninstall.wlua", "local VERSION = '"..VERSION.."'")
sys.cmd('luart-static.exe ../tools/rtc/src/rtc.lua -s -w -o "'..parent..'\\dist\\LuaRT-remove.exe" ../setup/uninstall.wlua ../setup/img >nul', false)

z = compression.Zip(parent.."/setup/luaRT.zip", "write", 9)
z:write(dist)
z:write(parent.."/examples", "examples")
z:write(parent.."/tools/LuaRT-Studio", "LuaRT-Studio")
z:write(parent.."/tools/QuickRT/lua54.dll", "QuickRT/lua54.dll")
z:write(parent.."/tools/QuickRT/QuickRT.exe", "QuickRT/QuickRT.exe")
z:close()
dist:removeall()

file_add("../setup/install.wlua", "local VERSION = '"..VERSION.."'")
sys.cmd('luart-static.exe ../tools/rtc/src/rtc.lua -s -w -o "LuaRT-'..VERSION..'-'..PLATFORM..'.exe" ../setup/install.wlua ../setup >nul', false)

console.write(" done\n")