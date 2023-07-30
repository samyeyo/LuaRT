-- wttr.in Http web service, by Igor Chubin
-- http://www.apache.org/licenses/LICENSE-2.0
-- Execute in Windows Terminal, rather than cmd.exe, for full UTF8 support

local net = require "net"
local console = require "console"


net.Http("https://wttr.in"):get("/Paris?format=3").after = function (client, response)
	console.write(response.content)
end

waitall()