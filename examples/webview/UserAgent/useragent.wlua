local ui = require "ui"
require "webview"

local win = ui.Window("Webview.useragent property", "fixed", 320, 200)
local wv = ui.Webview(win, "")
wv.align = "all"

win:center()

function wv:onReady()
    -- change the Webview userAgent
    wv.useragent = "Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:42.0) Gecko/20100101 Firefox/42.0"
    wv.statusbar = false
    wv.devtools = false
    wv.contextmenu = false
    wv.acceleratorkeys = false
    -- load an HTML page that will display the current userAgent
    wv:loadstring([[
<!DOCTYPE html>
<html>
<head>
    <title>Print User Agent</title>
</head>
<body style="color : ]]..(ui.theme == "dark" and "#FFFFFF" or "#000000").."; background-color: "..(ui.theme == "dark" and "#000000" or "#FFFFFF")..[[;">
    <h1>Your User Agent</h1>
    <p id="userAgent"></p>

    <script>
        document.getElementById("userAgent").textContent = navigator.userAgent;
    </script>
</body>
</html>]])
end

ui.run(win):wait()