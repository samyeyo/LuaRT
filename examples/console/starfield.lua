-- luaRT example : starfield.lua

local console = require "console"

local colors = { "black", "blue", "green", "cyan", "red", "purple", "yellow", "white", "gray",
"lightblue", "lightgreen", "lightcyan", "lightred", "lightpurple", "lightyellow", "brightwhite"}

console.clear()

math.randomseed(sys.clock())
for i=1,console.height*console.width/3 do
	console.x = math.random(console.width)
	console.y = math.random(console.height)
	console.writecolor(colors[math.random(#colors)], "+")
end
console.readchar()
console.clear()