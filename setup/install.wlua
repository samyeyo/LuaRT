local VERSION = '1.0.0'

--[[
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | install.wlua | LuaRT setup script
--]]



local ui = require "ui"
local zip = require "zip"

local File = embed == nil and sys.File  or embed.File

local win = ui.Window("", "fixed", 320, 200)
win.font = "Segoe UI"

local img = ui.Picture(win, File("img/luaRT.png").fullpath, 0, 0)
win:loadicon(File("img/setup.ico"))
win.width = img.width
win.bgcolor = 0xFFFFFF
win:center()

local button = ui.Button(win, "Install LuaRT "..VERSION)
button:loadicon(File("img/install.ico"))
button.cursor = "hand"
button:center()
button.y = win.height-40

local reg = {
    DisplayName = "LuaRT - Windows programming framework for Lua",
    DisplayVersion = VERSION,
    HelpLink = "https://www.luart.org",
    NoModify = 1,
    NoRepair = 1,
    Publisher = "Samir Tine"
}

local function shortcut(name, target, icon)
    local shell = sys.COM("WScript.Shell")
    local shortcut = shell:CreateShortcut(startmenu_dir.fullpath.."/"..name..".lnk")
    shortcut.TargetPath = target
    shortcut.IconLocation = icon or (target..",0")
    shortcut:Save()
end

function button:onClick()
    local dir = ui.dirdialog("Select a directory to install LuaRT")
    if dir ~= nil then
        local label = ui.Label(win, "", 80, 160)
        label.y = 160        
        local archive = zip.Zip(File("luaRT.zip"))
        local size = 0
        self:hide()
        for entry in each(archive) do
            label.text = "Extracting "..entry:sub(1, 32).."..."
            local result = archive:extract(entry, dir)
            if not result then
                error("Error extracting "..entry.."\n"..(sys.error or ""))
            end
            ui.update()
        end
        archive:close()
        local user_path = sys.registry.read("HKEY_CURRENT_USER", "Environment", "Path", false) or ""
        user_path = user_path:gsub(dir.fullpath.."\\bin;", "")
        sys.registry.write("HKEY_CURRENT_USER", "Environment", "Path", user_path..";"..dir.fullpath.."\\bin", true)
        reg.InstallLocation = dir.fullpath;
        reg.DisplayIcon = dir.fullpath.."\\LuaRT-remove.exe,-103"
        reg.UninstallString = dir.fullpath.."\\LuaRT-remove.exe"
        -- reg.EstimatedSize = math.floor(size/100)
        for key, value in pairs(reg) do
            sys.registry.write("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT", key, value);
        end
        -- Register *.lua file association
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\.lua", nil, "lua");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\lua", nil, "Lua script");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\lua\\DefaultIcon", nil, dir.fullpath.."\\LuaRT-remove.exe,-102");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\lua\\shell\\open\\command", nil, '"'..dir.fullpath..'\\bin\\luart.exe" "%1"');
        
        -- Register *.wlua file association
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\.wlua", nil, "wlua");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\wlua", nil, "Lua script");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\wlua\\DefaultIcon", nil, dir.fullpath.."\\LuaRT-remove.exe,-102");
        sys.registry.write("HKEY_CURRENT_USER", "Software\\Classes\\wlua\\shell\\open\\command", nil, '"'..dir.fullpath..'\\bin\\wluart.exe" "%1"');

        -- Create shortcuts
        startmenu_dir = sys.Directory(sys.env.USERPROFILE.."\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\LuaRT")
        startmenu_dir:make()
        shortcut("QuickRT", dir.fullpath.."\\QuickRT\\QuickRT.exe")
        shortcut("LuaRT Studio", dir.fullpath.."\\LuaRT-Studio\\LuaRT Studio.exe")
        shortcut("LuaRT Documentation", "https://luart.org/doc/index.html", sys.env.windir.."\\system32\\shell32.dll, 13")
        ui.info("LuaRT "..VERSION.." has been successfully installed", "")
        win:hide()
    end
end

win:show()

while win.visible do
    ui.update()
end 
