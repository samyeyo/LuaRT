local ui = require "ui"

-- Get the File 'calc.html' depending on if it is embeded or not
local file = embed and 'calc.html' or sys.File(sys.File(arg[0]).path.."/calc.html").fullpath

require "webview"

local win = ui.Window("Calculator - web application with LuaRT", "fixed", 290, 310)
win:loadicon(sys.env.WINDIR.."/System32/calc.exe")

local wv = ui.Webview(win, "file:///"..file)
wv.align = "all"

win:center()

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
    wv.acceleratorkeys = false
end

ui.run(win):wait()