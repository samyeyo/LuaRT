local VERSION = '1.7.0'

--[[
    | LuaRT - A Windows programming framework for Lua
    | Luart.org, Copyright (c) Tine Samir 2023.
    | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | install.wlua | LuaRT setup script
--]]



local ui = require "ui"
local compression = require "compression"

local File = embed == nil and sys.File  or embed.File

local win = ui.Window("", "raw", 320, 240)
win.font = "Segoe UI"
win.installation = false

local x = ui.Label(win, "\xc3\x97", 378, -4)
x.fontsize = 16
x.fgcolor = 0x808080
x.bgcolor = 0xFFFFFF
x.cursor = "hand"

function x:onHover()
    x.fgcolor = 0x202020
end

function x:onLeave()
    x.fgcolor = 0x808080
end

function x:onClick()
    if not (win.installation and ui.confirm("Installation is in progress. Are you really want to quit ?", "LuaRT installation") ~= "yes" or false) then
        sys.exit()
    end
end

local caption = "Install"
local update = sys.registry.read("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT", "InstallLocation") or false

if update then
    local p = sys.Pipe('luart -e print(_VERSION)') or false
    if p then
        local current = (await(p:read()) or VERSION):match("%d%.%d%.%d")
        local numver = VERSION:gsub("%.", "")
        local numcurr = current:gsub("%.", "")
        if current == VERSION then
            caption = "Reinstall"
        elseif numcurr > numver then
            caption = "Downgrade to"
        else
            caption = "Update to"
        end
    else
        caption = "Repair"
    end
end

local img = ui.Picture(win, File("img/luaRT.png").fullpath, 0, 20)
win.width = img.width
win.bgcolor = 0xFFFFFF
win:center()

local button = ui.Button(win, caption.." LuaRT "..VERSION.." for "..(arg[0]:find("x64") and "x64" or "x86"))
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

local function isopen(path, name)
    local f = sys.File(path)
    if f.exists then
        local try, err = pcall(f.open, f, "write")
        if try == false then
            win:hide()
            error(name.." is currently running\nPlease close "..name.." and retry installation")
        else
            f:close()
            f:remove()
        end
    end
end

function button:onClick()
    local dir = update and sys.Directory(update) or ui.dirdialog("Select a directory to install LuaRT")
    if update then 
        ui.warn("Before installing, make sure to close any LuaRT-related programs.")
    end
    if dir ~= nil then
        isopen(dir.fullpath.."/bin/luart.exe", "LuaRT console interpreter")        
        isopen(dir.fullpath.."/bin/wluart.exe", "LuaRT console interpreter")       
        isopen(dir.fullpath.."/bin/rtc.exe", "rtc compiler")        
        isopen(dir.fullpath.."/bin/wrtc.exe", "wrtc compiler")        
        isopen(dir.fullpath.."/bin/rtcheck.exe", "LuaRT update checker")        
        isopen(dir.fullpath.."/bin/luart-static.exe", "LuaRT console interpreter")      
        isopen(dir.fullpath.."/bin/wluart-static.exe", "LuaRT console interpreter")     
        isopen(dir.fullpath.."/LuaRT-Studio/LuaRT Studio.exe", "LuaRT Studio")      
        isopen(dir.fullpath.."/QuickRT/QuickRT.exe", "QuickRT")      
        win.installation = true
        local label = ui.Label(win, "", 40, 190, 312)
        label.autosize = false        
        label.fontsize = 8
        label.textalign = "left"
        label.fgcolor = 0x002A5A
        local bar = ui.Progressbar(win, 40, 170, 306)
        bar.themed = false
        bar.fgcolor = 0xEFB42C
        bar.bgcolor = 0xFFFFFF
        local archive = compression.Zip(File("luaRT.zip"))
        bar.range = {0, archive.count}
        local size = 0
        self:hide()
        for entry, isdir in each(archive) do
            if not isdir then
                local fname = entry:gsub('/', '\\')
                label.text = "Extracting "..fname:sub(1, 40).."..."
                local result = archive:extract(entry, dir)
                if not result then
                    error("Error extracting "..fname.."\n"..(sys.error or ""))
                end
            end
            bar:advance(1)
            ui.update()
        end
        archive:close()
        local user_path = sys.registry.read("HKEY_CURRENT_USER", "Environment", "Path", false) or ""
        user_path = user_path:gsub(dir.fullpath.."\\bin;", "")
        sys.registry.write("HKEY_CURRENT_USER", "Environment", "Path", user_path..";"..dir.fullpath.."\\bin", true)
        reg.InstallLocation = dir.fullpath;
        reg.DisplayIcon = dir.fullpath.."\\LuaRT-remove.exe,-101"
        reg.UninstallString = dir.fullpath.."\\LuaRT-remove.exe"
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
        if not startmenu_dir.exists then
            startmenu_dir:make()
        end
        shortcut("QuickRT", dir.fullpath.."\\QuickRT\\QuickRT.exe")
        shortcut("LuaRT Update", dir.fullpath.."\\bin\\rtcheck.exe")
        shortcut("LuaRT Studio", dir.fullpath.."\\LuaRT-Studio\\LuaRT Studio.exe")
        shortcut("LuaRT Documentation", "https://luart.org/doc/index.html", sys.env.windir.."\\system32\\shell32.dll, 13")
        win.installation = false
        label.text = "LuaRT "..VERSION.." has been successfully installed"
        label.textalign = "center"
    end
end

ui.run(win):wait()
