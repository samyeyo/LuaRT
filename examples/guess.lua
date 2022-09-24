--
--  luaRT guess.lua example
--  Guess a word game
--

local console = require 'console'

function title(msg)
	local length = math.floor((console.width-string.len(msg))/2)
	console.clear("black")
	console.bgcolor = "lightblue"
	console.writecolor("blue", string.rep(string.char(0xE2, 0x96, 0x80), console.width))
	console.writecolor("brightwhite", string.rep(' ', length)..msg)
	console.write(string.rep(' ', console.width-length-string.len(msg)))
	console.bgcolor = "blue"
	console.writecolor("lightblue", string.rep(string.char(0xE2, 0x96, 0x80), console.width))
	console.bgcolor = "black"
	console.writeln("\n")
end

title("The 'guess a word' game")
print("\n\n")

local word = "moon"

while true do
	console.writecolor("lightcyan", "Guess the word : ")
	console.color = "brightwhite"
	local response = console.stdin:readln() --:gsub("[\r\n]+$", "")
	if response ~= word then
		console.writecolor("cyan", "The word '"..response.."' is not the response !\n")
		console.writecolor("cyan", "Your response is nearly", response:similarity(word:lower()).."% similar\n\n")
	else
		title("CONGRATULATION !! YOU HAVE FOUND THE WORD '"..word.."'")
		break
	end
end

