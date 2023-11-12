-- Echo client example
-- Using non blocking sockets and asynchronous tasks

local net = require "net"
local ui = require "ui"

local win = ui.Window("Chat client", 320, 200, "fixed")
local edit = ui.Edit(win, "", 4, 2, 312, 173)
edit.font = "Consolas"
edit.fontsize = 9
edit.readonly = true
edit.bgcolor = 0
edit.fgcolor = 0xAAAAAA
edit.wordwrap = true

local entry = ui.Entry(win, "", 3, 176, 313, 22)

function win:onClose()
    sys.exit()
end

local client = net.Socket("127.0.0.1", 5000)
client.blocking = false

function entry:onSelect()
    client:send(entry.text)
    entry.text = "" 
end

function win:onShow()
    edit:append("Connecting... ")
    if await(client:connect()) then
        edit:append("done !\nConnected to the server !\n")
        async(function()
            while true do
                local msg = await(client:recv()) or error("Network error: "..net.error)
                edit:append(msg.."\n")
            end
        end)
    else
        error("Network error: failed to connect to the server")
    end
end

ui.run(win):wait()
