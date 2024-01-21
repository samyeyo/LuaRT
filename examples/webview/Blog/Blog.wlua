local ui = require "ui"

-- get the the path to the messages.wlua folder
local homedir = sys.File(arg[0]).directory

require "webview"

local win = ui.Window("Blog application using Tailwind CSS - Powered by LuaRT", "fixed", 414, 776)

local wv = ui.Webview(win, homedir.path.."/Blog.html")
wv.align = "all"

win:show()

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
end

while win.visible do
    ui.update()
end