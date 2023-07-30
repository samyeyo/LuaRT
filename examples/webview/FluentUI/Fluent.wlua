local ui = require "ui"

-- Get the File 'Fluent.html' depending on if it is embeded or not
local file = embed and embed.File('Fluent.html') or sys.File(sys.File(arg[1]).path.."/Fluent.html")

require "webview"

local win = ui.Window("FluentUI application with LuaRT", "fixed", 400, 150)
win:loadicon(sys.env.WINDIR.."/System32/shell32.dll", 278)
win.bgcolor = 0xFFFFFF

local wv = ui.Webview(win, "", 0, 46)
wv.align = "all"

function wv:onLoaded()
    wv:postmessage('{ "id": "cpuname", "text": "'..sys.registry.read('HKEY_LOCAL_MACHINE', 'Hardware\\Description\\System\\CentralProcessor\\0', 'ProcessorNameString')..'"}', true)
    wv:postmessage(' {"show": true}')
    wv:postmessage('{ "id": "graphic", "text": "'..sys.registry.read('HKEY_LOCAL_MACHINE', 'SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinSAT', 'PrimaryAdapterString')..'"}', true)
    wv:postmessage('{ "id": "memorysize", "text": "'..math.floor(await(sys.Pipe("wmic computersystem get TotalPhysicalMemory"):read()):match("(%d+)")/1000000000).." Gb"..'"}', true)
end

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
    -- Add initialization JS script before any loaded page
    -- This JS script processes posted messages from Lua script
    wv:addinitscript([[
    window.chrome.webview.addEventListener('message', arg => {
        if ("id" in arg.data) {
            const item = document.getElementById(arg.data.id);
            item.innerHTML = arg.data.text;
        }
        if ("show" in arg.data) {
            document.body.style.visibility = "visible";
        }
    }); ]])
    wv.url = file.fullpath
end

win:center()

ui.run(win)