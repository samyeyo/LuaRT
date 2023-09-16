local ui = require "ui"
require "webview"

local win = ui.Window("YouTube - Powered by LuaRT", 640, 540)
local wv = ui.Webview(win, "https://www.youtube.com/?app=mobile")

wv.align = "all"

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
end

win:center()

ui.run(win):wait()