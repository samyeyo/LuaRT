--
--  LuaRT geo.lua example
--  Find the location where you currently are
--
local net = require "net" 

-- use the free IP-API localization web API
local url = "http://ip-api.com"
local client = net.Http(url)
-- make a GET request, returning the response as a string
local uri = "/json/"
local response = client:get(uri)
-- parse the response (a JSON string)
print("You are located in "..response:umatch('"country":"(%w+)').." near "..response:umatch('"city":"(%w+)'))