-- | LuaRT - A Windows programming framework for Lua
-- | Luart.org, Copyright (c) Tine Samir 2025.
-- | See Copyright Notice in LICENSE.TXT
-- |------------------------------------------------
-- | highlight.lua | Highlight a line of code




local console = require "console"

local keywords = { ["and"] = true, ["break"] = true, ["do"] = true, ["else"] = true, ["elseif"] = true, ["end"] = true, ["false"] = true, ["for"]  = true, ["function"] = true, ["goto"] = true, ["if"] = true, ["in"] = true, ["local"] = true, ["nil"] = true, ["not"] = true, ["or"] = true, ["repeat"] = true, ["return"] = true, ["then"] = true, ["true"] = true, ["until"] = true, ["while"] = true, ["each"] = true }

local types = { ["Object"] = true, ["property"] = true, ["method"] = true, ["function"] = true, ["table"] = true }


function highlight(line)
	local word = "" 
	local str = ""
	local result = ""
	local start_str = nil
	local prev = ""

	for ch in each(line) do
		if ch:umatch("['\"]") then
			if start_str ~= nil then 
				if start_str == ch and prev ~= '\\' then
					if #word > 0 then
						console.writecolor("lightyellow", word)
						word = ""
					end
					console.writecolor("lightpurple", str..ch)
					str = ""
					start_str = nil
					goto continue
				end
			else
				start_str = ch
			end
		elseif ch:umatch("[%s%p]") then
			if start_str == nil then
				if ch == '(' and #word then
					console.writecolor("lightyellow", word)
				elseif #word > 0 then
					if line:ufind(word) == 1 and (word == "else" or word == "end" or word == "elseif" or word == "until") then
						console.write("\b\b\b")
						console.write(string.rep(" ", 25))
						console.write(string.rep("\b", 26))
					end
					console.writecolor(types[word] and "gray" or (keywords[word] and "cyan") or (tonumber(word) and "green" or "white"), word)
				end
				word = ""
				console.write(ch)
				goto continue
			end
		end
		if start_str ~= nil then
			str = str..ch
		else
			word = word..ch
		end
::continue::
		prev=ch
	end
	if #word > 0 then
		console.writecolor(keywords[word] and "cyan" or (tonumber(word) and "green" or "white"), word)
	end
	if start_str ~= nil then
		console.writecolor("lightpurple", str)
	end
end

return highlight