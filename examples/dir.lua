--
--  luaRT dir.lua example
--  alternative to the "dir" Windows command
--
local console = require "console"

local function format_int(number)
  local i, j, minus, int, fraction = tostring(number):wfind('([-]?)(%d+)([.]?%d*)')
  int = int:wreverse():gwsub("(%d%d%d)", "%1 ")
  local result = minus .. int:wreverse():gsub("^,", "") .. fraction
  result = result:gwsub("^%s+", "")
  local len = string.wlen(result)
  result = string.rep(" ", 16-len)..result
  return result
end

local dir = sys.Directory(arg[2] or sys.currentdir)

console.writecolor("gray", "\n"..string.rep("\xe2\x94\x80", 2).."\xe2\x94\xac\xe2\x94\x80 ")
console.writecolor("yellow", dir.fullpath.."\\\n")
console.writecolor("gray", "  \xe2\x94\x82\n")

for entry in each(dir) do
	console.writecolor("gray", "  \xe2\x94\x9c\xe2\x94\x80 ")
	console.writecolor("gray", entry.modified or "\t\t\t")
	console.writecolor("gray", entry.size and " "..format_int(entry.size) or "                 ")
	if is(entry, sys.Directory) then
		console.writecolor("yellow", "  "..entry.name.."\\\n")
	elseif entry.extension == ".exe" then
		console.writecolor("lightgreen",  "  "..entry.name.."\n")
	elseif entry.extension == ".bat" then
		console.writecolor("cyan",  "  "..entry.name.."\n")
	elseif entry.extension == ".dll" then
		console.writecolor("purple",  "  "..entry.name.."\n")
	else
		local color = "white"
		if entry.extension == ".exe" then
			color = "lightgreen"
		end
		console.writecolor(color, "  "..entry.name.."\n")
	end
end

