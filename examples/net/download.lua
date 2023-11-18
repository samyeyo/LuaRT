local net = require "net"
local console = require "console"

console.cursor = false

----- HTTP download function to show progress
local function download(host, uri)
	local request = net.Http(host)
    local task = request:download(uri)
	local size, length
	console.writecolor("cyan", "Downloading "..sys.File(uri).name.." | ")
	while not task.terminated do
		sleep()
		length = length or (request.headers["Content-length"] or false)
		if length then
			size = string.format("%0.0f%%    ", request.received/length*100)
		else
			size = string.format("%0.2fMb", request.received/1000000)
		end
		console.writecolor("yellow", size)
		console.write(string.rep("\b", #size))
	end
	if not request.response then
		console.writecolor("red", "error:", net.error)
	end
	console.write("\n")
end

-- wait for the downloading task to finish
await(download, "http://download.thinkbroadband.com", "/10MB.zip")
