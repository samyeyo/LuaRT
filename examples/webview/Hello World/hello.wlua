local ui = require "ui"
require "webview"

-- Get the File 'hello.html' depending on if it is embeded or not
local file = embed and 'hello.html' or sys.File(sys.File(arg[0]).path.."/hello.html").fullpath

local win = ui.Window("Hello World with Webview example", 640, 540)

local wv = ui.Webview(win, "file:///"..file, 0, 46)
wv.align = "all"

win:center()

ui.run(win):wait()