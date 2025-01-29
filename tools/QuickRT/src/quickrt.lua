-- | LuaRT - A Windows programming framework for Lua
-- | Luart.org, Copyright (c) Tine Samir 2025
-- | See Copyright Notice in LICENSE.TXT
-- |------------------------------------------------
-- | QuickRT.lua | Powerful REPL for LuaRT




local console = require 'console'
require "readline"
local indent = require "indent"
local pretty = require "pretty"
local ui = require "ui"

local luart_path = sys.registry.read('HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT', 'InstallLocation', false) or false

if not luart_path then
	luart_path = sys.File(arg[0] or arg[-1]).directory.parent.parent.fullpath.."/modules/?/?.dll"
end

package.cpath = package.cpath..";"..luart_path

console.title = "QuickRT - REPL for LuaRT"
console.font = "consolas"

-- clear command : clears the screen
function clear()
	console.clear()
end

-- help command : provides help on the specified topic
help = require "help"

-- Welcome message 
console.clear()
console.writecolor("lightblue", string.char(0xE2, 0x95, 0xAD))
console.writecolor("lightblue", string.rep(string.char(0xE2, 0x94, 0x81), 36)..string.char(0xE2, 0x95, 0xAE).."\n")
console.writecolor("lightblue", string.char(0xE2, 0x94, 0x82).. " ")
console.writecolor("lightblue", "Quick")
console.writecolor("blue", "RT")
console.write(" - Powerful REPL for ")
console.writecolor("brightwhite", "Lua")
console.writecolor('yellow', "RT ")
console.writecolor("blue", " "..string.char(0xE2, 0x94, 0x82).."\n")
console.writecolor("lightblue", string.char(0xE2, 0x94, 0x82).." ")
console.write("Copyright "..string.char(0xC2, 0xA9).." 2025, Samir Tine       ")
console.writecolor("blue", string.char(0xE2, 0x94, 0x82),"\n")
console.writecolor("blue", string.char(0xE2, 0x95, 0xB0)..string.rep(string.char(0xE2, 0x94, 0x81), 36)..string.char(0xE2, 0x95, 0xAF).."\n")

-- Environment for Lua commands
env = { console = console }
setmetatable(env, { __index = function(t, key)
						return _G[key]
					end,
					__newindex = function(t, k, v)
						_G[k] = v
					end
				})

local function print_result(val, ch)
	console.writecolor('gray', highlight(pretty(val)..ch))
end
			
-- Read Eval Print Loop : REPL
async (function ()
	while true do
		sleep()
		ui.update()
		console.write("\n")
		-- Read
		local cmd = indent(readline(nil, env))
		if cmd ~= "" then
			local var = cmd:umatch("^([%a%s_]+)$") or false
			if var then
				print_result(env[var], "\n")
			else
				-- Eval
				local func, err = load("return "..cmd, nil, nil, env)	
				if func == fail then
					func, err = load(cmd, nil, nil, env)
				end	
				if func ~= fail then
					local results = table.pack(pcall(func, true))
					if results[1] == true then
						if #results > 1 then
							for i = 2, #results do
								print_result(results[i], " ")
							end
							if #results == 2 then
								env._ = results[2]
							else
								env._ = results
								table.remove(env._, 1)
							end
							console.write("\n")
						end
					else
						console.writecolor("lightred", ">>> "..results[2]:gsub("^%[.*%d%: ", "").."\n")				
					end
				else
					console.writecolor("lightred", ">>> "..err:gsub("^%[.*%d%: ", "").."\n")				
				end
			end
		end
	-- Loop
	end
end)

waitall()