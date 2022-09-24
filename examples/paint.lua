-- luaRT example : paint.lua

local console = require "console"

local colors = { "black", "blue", "green", "cyan", "red", "purple", "yellow", "white", "gray",
"lightblue", "lightgreen", "lightcyan", "lightred", "lightpurple", "lightyellow", "brightwhite"}

console.clear()
console.locate(math.floor(console.width-36), console.height)
console.writecolor("gray", "< Draw by clicking with the mouse >")

console.locate(1, console.height)
console.writecolor("gray", "Palette: ")
for color in each(colors) do 
	console.bgcolor = color
	console.write("  ");
end

console.bgcolor = "yellow"

while true do
	local x, y, button = console.readmouse()
	if button == "left" and x <= console.width and y < console.height then
		console.locate(x, y)
		console.write(" ")
	elseif button == "left" and y == console.height and x > 9 and x < 42 then
		console.bgcolor = colors[math.floor((x-8)/2)]
	elseif button == "right" then
		break;
	end
end

console.reset()
console.clear()