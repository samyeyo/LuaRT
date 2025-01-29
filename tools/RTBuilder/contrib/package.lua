local Zip = require("compression").Zip
local console = require "console"
local VERSION = require "src.version"

local z = Zip("RTBuilder-"..VERSION.."-".._ARCH..".zip", "write", 9)
z:write("context/", "RTBuilder/context")
console.write("■")
z:write("inspector/", "RTBuilder/inspector")
console.write("■")
z:write("resources/", "RTBuilder/resources")
console.write("■")
z:write("widgets/", "RTBuilder/widgets")
console.write("■")
z:write("config.json", "RTBuilder/config.json")
console.write("■")
z:write("LICENSE", "RTBuilder/LICENSE")
console.write("■")
z:write("lua54.dll", "RTBuilder/lua54.dll")
console.write("■")
z:write("README.md", "RTBuilder/README.md")
console.write("■")
z:write("RTBuilder.exe", "RTBuilder/RTBuilder.exe")
console.write("■")
