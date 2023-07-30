local net = require 'net'
local console = require 'console'


console.write("Use TLS/SSL encryption [Y/N] ? ")
local use_encryption = console.read(1):upper() == "Y" and true or false

if net.isalive then
	console.writecolor("cyan", use_encryption and "\nConnecting to https://www.google.com..." or "\nConnecting to http://www.google.com...")
	sock = net.Socket("www.google.com", use_encryption and 443 or 80)
	if sock:connect() then
        console.writecolor("lightgreen", " success !\n")
        if use_encryption then 
            console.writecolor("cyan", "Starting TLS encryption... ")
            if sock:starttls() then
                console.writecolor("lightgreen", " success !\n")
            else
                console.writecolor("lightred", "unable to start TLS encryption !")
                sys.exit(-1)
            end     
        end               
        request = "GET /humans.txt HTTP/1.1\r\nHost: www.google.com\r\n\r\n" 
        sock:send(request)
        console.writecolor("cyan", "Waiting for HTTP GET response... ")
        response = sock:recv()
        if not response then 
            console.writecolor("lightred", "unable to recv !\n")
        else
            console.writecolor("lightgreen", "success !\n")
            local size = tonumber(response:match('Content%-Length: (%d+)\r\n'))
            response = response:match("\r\n\r\n(.*)")
            while(#response < size) do
                response = response..tostring(sock:recv())
            end
            console.writecolor("cyan", "\nGoogle's humans.txt:")
            console.writecolor("yellow", "\n"..response)
            sock:close()
        end
	else
		console.writecolor("lightred", "unable to connect to LuaRT server !")
	end
else
	console.writecolor("lightred", "\t\tNo internet access available. Please check your network connection.")
end
console.writecolor("cyan", "\nPress a key to exit...\n")
console.readchar()