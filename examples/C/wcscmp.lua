--
-- LuaRT C FFI example calling 'wcscmp()' C function
--

local c = require "c"

-- use the C runtime library
local ucrt = c.Library()

-- prototype of the  C function wcscmp(), a function to compare two wide strings
ucrt.wcscmp = "(WW)i"

-- first argument: a standard Lua string
local arg1 = "Héllo"

-- second argument: a C wide string
local arg2 = c.wstring("Héllo")

-- first string argument is converted seamlessly to a wide string before calling wcscmp()
if ucrt.wcscmp(arg1, arg2) == 0 then
    print("Both strings are equal")
else
    print("Strings are not equal ! (should not happen)")
end