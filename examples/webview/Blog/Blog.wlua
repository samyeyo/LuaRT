local ui = require "ui"

-- Get the File 'Blog.html' depending on if it is embeded or not
local file = embed and 'Blog.html' or sys.File(sys.File(arg[0]).path.."/Blog.html").fullpath

require "webview"

local win = ui.Window("Blog application using Tailwind CSS - Powered by LuaRT", "fixed", 414, 776)

local wv = ui.Webview(win, "file:///"..file)
wv.align = "all"

function wv:onReady()
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
end

ui.run(win):wait()