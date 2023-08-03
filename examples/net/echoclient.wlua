-- Echo client example
-- Using non blocking sockets and asynchronous tasks

local net = require "net"
local ui = require "ui"
require "console"

local win = ui.Window("Echo client", 320, 200)
local edit = ui.Edit(win, "", 4, 2, 312, 173)
edit.font = "Consolas"
edit.fontsize = 9
edit.readonly = true
edit.bgcolor = 0
edit.color = 0xAAAAAA

local entry = ui.Entry(win, "", 3, 176, 313, 22)

function win:onClose()
    sys.exit()
end

local client = net.Socket("127.0.0.1", 5000)
client.blocking = false

local t = sys.Task(function()
    while true do
        print("Waiting for data...")
        local msg = await(client:recv()) or error("Network error: "..net.error)
        print("Received "..msg)
        edit:append(msg.."\n")
    end
end)

function entry:onSelect()
    await(client:send(entry.text))
    print("data sent !")
    entry.text = "" 
end

function win:onShow()
    edit:append("Connecting... ")
    if await(client:connect()) then
        edit:append("connected to server !\n")
        t()
    else
        error("Network error: failed to connect to the server")
    end
end

async(function()
	win:show()
	while win.visible do
		sleep()
		ui.update()
	end
	sys.exit()
end)

waitall()
