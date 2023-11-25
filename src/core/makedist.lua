--[[
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | makedist.lua | Builds the LuaRT setup executable
--]]


local console = require("console")
local compression = require("compression")

local VERSION = arg[2]
local PLATFORM = arg[3]
local dist = sys.Directory(sys.Directory(sys.currentdir).parent.path.."\\dist")
local parent = dist.parent.parent.path
local bin = sys.Directory(dist.path.."\\bin")
dist:removeall()
dist:make()
bin:make()

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

copy("../../bin/lua54.dll", "bin/lua54.dll")
copy("../../bin/luart.exe", "bin/luart.exe")
copy("../../bin/rtc.exe", "bin/rtc.exe")
copy("../../bin/wrtc.exe", "bin/wrtc.exe")
copy("../../bin/rtcheck.exe", "bin/rtcheck.exe")
copy("../../QuickRT/quickrt.exe", "bin/quickrt.exe")
copy("../../bin/wluart.exe", "bin/wluart.exe")
copy("../../bin/luart-static.exe", "bin/luart-static.exe")
copy("../../bin/wluart-static.exe", "bin/wluart-static.exe")
copy(parent.."\\README.md", "README.md")
copy(parent.."\\LICENSE", "LICENSE")
copy(parent.."\\CHANGELOG.md")
file_add("../../setup/uninstall.wlua", "local VERSION = '"..VERSION.."'")
sys.cmd('..\\..\\bin\\rtc.exe -i "../../setup/img/logo.ico" -s -w -o ..\\dist\\LuaRT-remove.exe ../../setup/uninstall.wlua ../../setup/img >nul')
console.write("■")

local version = sys.File(parent.."/tools/LuaRT-Studio/src/version.lua"):open("write")
version:write("ide.VERSION = [["..VERSION.."]]")
version:close()

z = compression.Zip(parent.."/setup/luaRT.zip", "write", 9)
z:write(dist)
console.write("■")
z:write(parent.."/include", "include")
console.write("■")
z:write(parent.."/lib", "lib")
console.write("■")
z:write(parent.."/modules", "modules")
console.write("■")
z:write(parent.."/examples", "examples")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio", "LuaRT-Studio")
console.write("■")
z:write(parent.."/tools/QuickRT/lua54.dll", "QuickRT/lua54.dll")
console.write("■")
z:write(parent.."/tools/QuickRT/QuickRT.exe", "QuickRT/QuickRT.exe")
console.write("■")
z:close()
dist:removeall()

file_add("../../setup/install.wlua", "local VERSION = '"..VERSION.."'")
local exe = 'LuaRT-'..VERSION..'-'..PLATFORM
sys.cmd('..\\..\\bin\\rtc.exe -i "resources/box.ico" -s -w -o "'..exe..'.exe" ../../setup/install.wlua ../../setup >nul')

local compressed = compression.Zip(exe..".zip", "write")
compressed:write(exe..".exe")
compressed:close()
console.write("■")

sys.File(exe..'.exe'):remove()