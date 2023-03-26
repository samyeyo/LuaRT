-- wttr.in Http web service, by Igor Chubin
-- http://www.apache.org/licenses/LICENSE-2.0
-- Execute in Windows Terminal, rather than cmd.exe, for full UTF8 support

local net = require "net"
local console = require "console"

local http = net.Http("https://wttr.in")
local town = ""

if #arg > 1 then
	town = "~"..arg[2]
end

console.write(http:get("/"..town.."?format=3"))