--
--  LuaRT server.lua example
--  Wait for connections on 127.0.0.1:5000
-- 

local net = require "net"
local ui = require "ui"

local win = ui.Window("Server running on 127.0.0.1:5000", "fixed", 320, 200)
local edit = ui.Edit(win, "")
edit:align("all")
edit.readonly = true
edit.font = "Consolas"
edit.fonsize = 8
edit.bgcolor = 0
edit.color = 0xA0A0A0

-- create the server listening Socket
local server = net.Socket("127.0.0.1", 5000)

-- array of clients Sockets
local sockets = {server}
server.disconnected = false

if not server:bind() then
	error("Network error : cannot create the server Socket")
end

win:show()

while win.visible do
  ui.update()
	if net.select(sockets) == false then
		error("Network error : "..net.error)
		break
	end
	
	local alive_sockets = {}
	for client in each(sockets) do
		-- check for readability 
		if client.canread then
			if client == server then
				if server.canread then
					-- check for new connection
					local newclient = server:accept()
          edit.selection.color = 0x007000
					edit:append(newclient.ip.." has connected\n")
          edit.selection.color = 0xA0A0A0
					sockets[#sockets+1] = newclient
				end
			else
				local data = client:recv()
				if data == false then
          edit.selection.color = 0xD00000
					edit:append(client.ip.." has disconnected\n")
          edit.selection.color = 0xA0A0A0
					client.disconnected = true
        else
          edit:append(client.ip..": "..tostring(data).."\n")
				end
			end			
		end
		-- check for error
		if client.failed then
			if client == server then
				error("fatal network error with server")
			else
        edit.selection.color = 0xD00000
				edit:append(client.ip.." has encountered a fatal network error")
        edit.selection.color = 0xA0A0A0
				client.disconnected = true
			end
		end
		if not client.disconnected then
			alive_sockets[#alive_sockets+1] = client
		end
	end
	-- keep only connected clients
	sockets = alive_sockets
end