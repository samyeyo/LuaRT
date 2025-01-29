--[[
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | makedist.lua | Builds the LuaRT setup executable
--]]


local console = require("console")
local compression = require("compression")

local VERSION = arg[1]
local PLATFORM = arg[2]
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
z:write(parent.."/tools/LuaRT-Studio/bin", "LuaRT-Studio/bin")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/api", "LuaRT-Studio/api")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/build", "LuaRT-Studio/build")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/cfg", "LuaRT-Studio/cfg")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/examples", "LuaRT-Studio/examples")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/interpreters", "LuaRT-Studio/interpreters")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/lualibs", "LuaRT-Studio/lualibs")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/modules", "LuaRT-Studio/modules")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/packages", "LuaRT-Studio/packages")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/src", "LuaRT-Studio/src")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/spec", "LuaRT-Studio/spec")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/studio", "LuaRT-Studio/studio")
console.write("■")
z:write(parent.."/tools/LuaRT-Studio/CHANGELOG.md", "LuaRT-Studio/CHANGELOG.md")
z:write(parent.."/tools/LuaRT-Studio/README.md", "LuaRT-Studio/README.md")
z:write(parent.."/tools/LuaRT-Studio/LICENSE", "LuaRT-Studio/LICENSE")
z:write(parent.."/tools/LuaRT-Studio/LuaRT Studio.exe", "LuaRT-Studio/LuaRT Studio.exe")
console.write("■")
z:write(parent.."/tools/QuickRT/lua54.dll", "QuickRT/lua54.dll")
console.write("■")
z:write(parent.."/tools/QuickRT/QuickRT.exe", "QuickRT/QuickRT.exe")
console.write("■")
z:write(parent.."/tools/RTBuilder/context/", "RTBuilder/context")
console.write("■")
z:write(parent.."/tools/RTBuilder/inspector/", "RTBuilder/inspector")
console.write("■")
z:write(parent.."/tools/RTBuilder/resources/", "RTBuilder/resources")
console.write("■")
z:write(parent.."/tools/RTBuilder/widgets/", "RTBuilder/widgets")
console.write("■")
z:write(parent.."/tools/RTBuilder/config.json", "RTBuilder/config.json")
console.write("■")
z:write(parent.."/bin/lua54.dll", "RTBuilder/lua54.dll")
console.write("■")
z:write(parent.."/tools/RTBuilder/RTBuilder.exe", "RTBuilder/RTBuilder.exe")
console.write("■")
z:close()

file_add("../../setup/install.wlua", "local VERSION = '"..VERSION.."'")
local exe = 'LuaRT-'..VERSION..'-'..PLATFORM
sys.cmd('..\\..\\bin\\rtc.exe -i "resources/box.ico" -s -w -o "'..exe..'.exe" ../../setup/install.wlua ../../setup >nul')

if not sys.env.GITHUB_ACTIONS then
    local compressed = compression.Zip(exe..".zip", "write")
    compressed:write(exe..".exe")
    compressed:close()
    sys.File(exe..'.exe'):remove()
    console.write("■")
end

dist:removeall()