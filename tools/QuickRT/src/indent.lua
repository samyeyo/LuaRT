-- | LuaRT - A Windows programming framework for Lua
-- | Luart.org, Copyright (c) Tine Samir 2025.
-- | See Copyright Notice in LICENSE.TXT
-- |------------------------------------------------
-- | indent.lua | Indentation of a line of code




local console = require "console"
require "readline"

local ismultiline = {
	["function"] = {"function", "end"},
	["if"] = {"if", "end"},
	["for"] = {"for", "end"},
	["while"] = {"while", "end"},
	["repeat"] = {"repeat", "until"},
	["{"] = { "{", "}" }
}

local sep = string.char(0xE2, 0x94, 0x82)

local function indent(cmd, level)
	local multiline = ismultiline[cmd:match("^(%w+)")] or ismultiline[cmd:match("({)$")]
	if multiline then
		if not (cmd:match("("..multiline[2]..")$") or (multiline[1]=='{' and cmd:match("(})$"))) then
			if level == nil then
				level = "  "..sep.."   "
			else
				level=level..sep.."   "
			end
		else if level==nil then
				return cmd
			  end
		end
		local result = cmd
		local str = readline(level, env)
		while true do
			if str:match("(%w+)$") == multiline[2] or str:match("(})$")==multiline[2] or str:match("^(%w+)")==multiline[2] then
				cmd = result.."\n"..indent(str, level);
				break
			end
			result = result.."\n"..indent(str, level)
			str = readline(level)
		end
	end
	return cmd
end

return indent