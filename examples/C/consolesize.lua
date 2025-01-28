--
-- LuaRT C FFI example calling Windows API functions 'GetConsoleScreenBufferInfo()' to get console size in characters
--

local c = require "c"

local kernel32 = c.Library("kernel32.dll")

kernel32.GetConsoleScreenBufferInfo = "(pp)B"
kernel32.GetStdHandle = "(I)p"
local COORD = c.Struct("ss", "x", "y")
local SMALL_RECT = c.Struct("ssss", "Left", "Top", "Right", "Bottom")
local CONSOLE_SCREEN_BUFFER_INFO = c.Struct("..I..", {dwSize = COORD}, {dwCursorPosition = COORD}, "dwAttributes", {srWindow = SMALL_RECT}, {dwMaximumWindowSize = COORD})
local handle = kernel32.GetStdHandle(-11)

local csbi = CONSOLE_SCREEN_BUFFER_INFO()
csbi.dwSize = COORD { x = 1, y = 1}


if kernel32.GetConsoleScreenBufferInfo(handle, csbi) then
	print("console character columns :", csbi.dwSize.x)
	print("console character rows :", csbi.dwSize.y)
else
	error(sys.error)
end