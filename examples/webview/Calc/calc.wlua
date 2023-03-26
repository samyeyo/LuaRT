local ui = require "ui"

-- Get the File 'calc.html' depending on if it is embeded or not
local file = embed and embed.File('calc.html') or sys.File(sys.File(arg[1]).path.."/calc.html")

require "webview"

local win = ui.Window("Calculator - web application with LuaRT", "fixed", 290, 310)
win:loadicon(sys.env.WINDIR.."/System32/calc.exe")

local wv = ui.Webview(win, file.fullpath)
wv.align = "all"

win:center()
win:show()

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
end

while win.visible do
    ui.update()
end