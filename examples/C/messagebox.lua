--
-- LuaRT C FFI example calling Windows API MessageBoxW() function
--

local c = require "c"

-- load the user32 library
local user32 = c.Library("user32.dll")

-- prototype of the Windows API function MessageBoxW()
user32.MessageBoxW = "(lWWl)B"

-- Execute MessageBox() with seamless Lua string to C wide string conversion
user32.MessageBoxW(0, "Hello World !", "LuaRT C FFI module", 1)
