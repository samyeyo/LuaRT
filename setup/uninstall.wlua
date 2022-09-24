local VERSION = '1.0.0'
--[[
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |--------------------------------------------------
 | uninstall.wlua | LuaRT installation remove script
--]]



local ui = require "ui"
local zip = require "zip"

local File = embed.File

local exe = sys.File(arg[0])
local tmpexe = sys.File(sys.env.TEMP..'\\'..exe.name)

if exe.fullpath ~= tmpexe.fullpath then
    tmpexe:remove()
    exe:copy(tmpexe.fullpath)
    sys.cmd('start '..tmpexe.fullpath)
    sys.exit()
end

local win = ui.Window("", "fixed", 320, 200)
win:loadicon(arg[0])
win.bgcolor = 0xFFFFFF
win.font = "Segoe UI"

local img = ui.Picture(win, File("LuaRT.png").fullpath, 0, 0)
win.width = img.width
win:center()

local button = ui.Button(win, "Uninstall LuaRT "..VERSION)
button:center()
button.y = win.height-40
local path = sys.registry.read("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT", "InstallLocation") or error("Cannot find LuaRT installation.")

win:loadicon(File("setup.ico"))

function button:onClick()
    if ui.confirm("You are about to uninstall LuaRT "..VERSION..". Continue ?", "") == "yes" then
        local user_path = sys.registry.read("HKEY_CURRENT_USER", "Environment", "Path", false)
        sys.currentdir = "C:\\"
        if sys.cmd('rmdir "'..path..'" /s /q') == false then
            ui.error("Failed to remove LuaRT installation directory")
            sys.exit(-1)
        end
        sys.registry.write("HKEY_CURRENT_USER", "Environment", "Path", user_path:gsub(";"..path.."\\bin", ""), true)
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT")
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\.lua");
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\lua")
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\.wlua");
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\wlua")
        ui.info("LuaRT "..VERSION.." has been successfully uninstalled", "")
        sys.Directory(sys.env.USERPROFILE.."\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\LuaRT"):removeall()
        win:hide()
    end
end

win:show()

while win.visible do
    ui.update()
end 