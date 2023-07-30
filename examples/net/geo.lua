--
--  LuaRT geo.lua example
--  Find the location where you currently are, using asynchronous networking
--
local net = require "net" 
local json = require "json"

-- use the free IP-API localization web API
local url = "http://ip-api.com"
local client = net.Http(url)

-- make a GET request, and after its terminated, call the provided function, to process the response
client:get("/json/").after =  function (self, response)
                                 if not response then
                                    error(net.error)
                                 end
                                 local result = json.decode(response.content)
                                 print("You are located in "..result.country.." near "..result.city)
                              end
-- be sure to wait for task to finish before exiting                             
waitall()