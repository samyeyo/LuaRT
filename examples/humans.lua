--
--  luaRT humans.lua example
--  Get humans.txt from Google server
--

local net = require 'net'
local console = require 'console'

if net.isalive then
	console.writecolor("cyan", "Connecting to https:\\\\www.google.com... ")
	sock = net.Socket("www.google.com", 443)
	sock:connect()
	if sock:starttls() then
		console.writecolor("lightgreen", " success !\n")
		request = "GET /humans.txt HTTP/1.1\r\nHost: www.google.com\r\n\r\n" 
		sock:sendall(request)
		console.writecolor("cyan", "Waiting for HTTP GET response... ")
		response = tostring(sock:recv())
		if not response then 
			console.writecolor("lightred", "unable to get response from server !\n")
		else
			console.writecolor("lightgreen", "success !\n")
			local size = tonumber(response:match('Content%-Length: (%d+)\r\n'))
			response = response:match("\r\n\r\n(.*)")
			while(#response < size) do
				response = response..tostring(sock:recv())
			end
			console.writecolor("cyan", "\nGoogle's humans.txt:\n")
			console.writecolor("yellow", "\n"..response)
			sock:close()
		end
	else
		console.writecolor("lightred", "unable to connect to Google server !")
	end
else
	console.writecolor("lightred", "\t\tNo internet access available. Please check your network connection.")
end
console.writecolor("cyan", "\nPress a key to exit...\n")
console.readchar()