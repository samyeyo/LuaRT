local ui = require "ui"
require "webview"

-- Get the File 'hello.html' (even if the script is running from another folder)
local file = sys.File(sys.File(arg[0]).path.."/hello.html")

local win = ui.Window("Hello World with Webview example", 640, 540)

local wv = ui.Webview(win, file.fullpath, 0, 46)
wv.align = "all"

win:center()
win:show()

while win.visible do
    ui.update()
end