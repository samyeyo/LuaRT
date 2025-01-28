--
-- LuaRT C FFI example calling Windows API functions 'GetDesktopWindow()' and 'GetWindowRect()'
--

local ui = require "ui"
local c = require "c"

-- load the user32 library
local user32 = c.Library("user32.dll")

-- prototype of the  Windows API function GetWindowRect(), to get the size of a window
user32.GetWindowRect = "(pp)B"

-- prototype of the  Windows API function GetDesktopWindow(), to get the Desktop window handle
user32.GetDesktopWindow = "()p"

-- RECT struct definition
local RECT = c.Struct("iiii", "left", "top", "right", "bottom")

-- Creates a RECT struct
local r = RECT()

-- Gets the Desktop window handle
local handle = user32.GetDesktopWindow()

-- Calls the GetWindowRect() function using the Desktop window handle, and the RECT value (seamlessly converted to a pointer)
user32.GetWindowRect(handle, r)

-- 
ui.info("Desktop resolution is "..r.right.."x"..r.bottom)