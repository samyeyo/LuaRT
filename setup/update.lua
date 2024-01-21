--
--  LuaRT updater
--  Check and install LuaRT updates
--
local net = require "net" 
local ui = require "ui"
local compression = require "compression"

local File = embed == nil and sys.File or embed.File
local github = net.Http("https://github.com")

local win = ui.Window("", "raw", 320, 240)

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
    win:hide()
end

local img = ui.Picture(win, File("luaRT.png").fullpath, 0, 20)
win.width = img.width
win.bgcolor = 0xFFFFFF
win:center()

local button = ui.Button(win, "")
button:loadicon(File("install.ico"))
button.cursor = "hand"
button:hide()

local label = ui.Label(win, "Checking for update...", 40, 190)
label.autosize = false   
label.fontsize = 8
label.width = 312
label.textalign = "center"
label.fgcolor = 0x002A5A

local bar = ui.Progressbar(win, 40, 170, 306)
bar.themed = false
bar.fgcolor = 0xEFB42C
bar.bgcolor = 0xFFFFFF
bar.range = {0, 100}
bar:hide()

local function check_update()
    local _, response = await(github:get("/samyeyo/LuaRT/releases/latest"))
    if response == nil then
        error(net.error)
    elseif response.status ~= 200 then
        error(response.reason)
    end

    return response.content:match("(LuaRT [%d%.]+)")
end

local function download_update(progressbar, version)
    if ui.confirm("New "..version.." is available !\nDo you want to proceed with installation ?") == "yes" then
        win:show()
        bar:show()
        local v = version:match("([%d%.]+)")
        github.headers["Accept-Encoding"] = "None"
        local task = github:download("/samyeyo/LuaRT/releases/download/v"..v.."/LuaRT-"..v.."-".._ARCH..".zip")
        sleep(800)
        label.text = "Downloading "..version.." for ".._ARCH.."... "
        while not task.terminated do
            sleep()
            length = length or github.headers["content-length"] or github.headers["Content-length"] or github.headers["Content-Length"]  or github.headers["content-Length"] or false
            if length == false then                
                ui.error("Network error: failed to donwload LuaRT update")
                win:hide()
            end
            bar.position = math.floor(github.received/length*100)
        end        
        return github.response.file
    end
    win:hide()
    return false
end

local function version_num(v)
  local num = v:gsub("LuaRT ", ""):gsub("(%d)%.(%d)%.(%d)", "%1%2%3")
  return num
end

function win:onShow()
    local version = check_update(bar)   
    if version and (version_num(_VERSION) < version_num(version)) then
        local file = download_update(bar, version)
        if file then
            local tmpdir = sys.tempdir()
            local tmpfile = sys.File(tmpdir.fullpath.."/"..file.name)
            if file:move(tmpfile.fullpath) then
                win:hide()
                local zip = compression.Zip(tmpfile)
                zip:extractall(tmpdir)
                zip:close()
                sys.cmd(tmpfile.fullpath:gsub(".zip", ".exe"), false, false)
                ui.info("done")
                tmpdir:removeall()
            else
                error("Failed to execute LuaRT setup")
            end
        end
    else
        ui.info("Your LuaRT installation is up to date")
        win:hide()
    end
end

ui.run(win):wait()