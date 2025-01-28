--
-- LuaRT C FFI example to search for an opened Window by title name
--

local c = require "c"

-- load user32.dll library
local user32 = c.Library("user32.dll")

-- define for this library the FindWindowW function prototype
-- the function take two wide strings ('W') as parameters and returns a pointer ('p')
user32.FindWindowW = "(WW)p"

-- function that returns TRUE if a window with the given title is open
local function find_window(title)
    return user32.FindWindowW(c.NULL, title) ~= c.NULL
end

-- check if a "Calculator" titled Window is open
-- The title name might change depending on your current locale
if find_window("Calculator") then
    print("Calculator Window is open !")
else
    print("Calculator Window is not open !")
end