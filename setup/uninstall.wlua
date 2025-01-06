local VERSION = '1.9.0'
--[[
    | LuaRT - A Windows programming framework for Lua
    | Luart.org, Copyright (c) Tine Samir 2023.
    | See Copyright Notice in LICENSE.TXT
    |--------------------------------------------------
    | uninstall.wlua | LuaRT installation remove script
    --]]
    
    
    
local ui = require "ui"

local File = embed and embed.File or sys.File

local exe = sys.File(arg[-1])
local tmpexe = sys.File(sys.env.TEMP..'\\'..exe.name)

if exe.fullpath ~= tmpexe.fullpath then
    tmpexe:remove()
    exe:copy(tmpexe.fullpath)
    sys.cmd('start '..tmpexe.fullpath, true, true)
    sys.exit()
end

local win = ui.Window("Remove LuaRT", "raw", 320, 200)
win.bgcolor = ui.theme == "light" and 0xFFFFFF or 0
win.font = "Segoe UI"
win.installation = false

local factor
if ui.dpi < 1.5 then
    factor = 1
elseif ui.dpi < 1.75 then
    factor = 1.5
elseif ui.dpi >= 1.75 then
    factor = 2
end

local img = ui.Picture(win, File(("logox"..factor..".png"):gsub(",", ".")).fullpath)
win.width = img.width
img:center()
img.y = 20
win:center()

local x = ui.Label(win, "\xc3\x97", win.width-22, -4)
x.fontsize = 16
x.fgcolor = 0x808080
x.bgcolor = ui.theme == "light" and 0xFFFFFF or 0
x.cursor = "hand"
win.bgcolor = x.bgcolor

function x:onHover()
    x.fgcolor = 0x404040
end

function x:onLeave()
    x.fgcolor = 0x808080
end

function x:onClick()
    win.visible = win.installation and ui.confirm("LuaRT uninstall is in progress. Are you really want to quit ?", "LuaRT uninstall") ~= "yes" or false
end

local button = ui.Button(win, "Uninstall LuaRT "..VERSION)
button:center()
button.y = win.height-40
local path = sys.registry.read("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT", "InstallLocation") or error("Cannot find LuaRT installation.")

function button:onClick()
    win.installation = true
end

win:show()

while win.visible do
    ui.update()
    if win.installation then
        local user_path = sys.registry.read("HKEY_CURRENT_USER", "Environment", "Path", false)
        sys.currentdir = "C:\\"
        ui.update()
        sys.Directory(path.."/bin"):removeall()
        sys.Directory(path.."/LuaRT-Studio"):removeall()
        sys.Directory(path.."/QuickRT"):removeall()
        sys.Directory(path.."/include"):removeall()
        sys.Directory(path.."/lib"):removeall()
        sys.Directory(path.."/modules"):removeall()
        sys.Directory(path.."/examples"):removeall()
        sys.Directory(path.."/RTBuilder"):removeall()
        ui.update()
        sys.File(path.."\\CHANGELOG.md"):remove()
        sys.File(path.."\\LICENSE"):remove()
        sys.File(path.."\\README.md"):remove()
        sys.File(path.."\\LuaRT-remove.exe"):remove()
        local luart_dir = sys.Directory(path)
        if luart_dir.isempty then
            luart_dir:removeall()
        end
        ui.update()
        sys.registry.write("HKEY_CURRENT_USER", "Environment", "Path", user_path:gsub(";"..path.."\\bin", ""), true)
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT")
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\.lua");
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\lua")
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\.wlua");
        sys.registry.delete("HKEY_CURRENT_USER", "Software\\Classes\\wlua")
        ui.update()
        sys.Directory(sys.env.USERPROFILE.."\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\LuaRT"):removeall()
        win:hide()
    end
end 