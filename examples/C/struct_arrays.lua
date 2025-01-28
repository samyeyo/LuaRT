--
-- LuaRT C FFI example manipulating a Struct that contains an array of structs`
--

local c = require "c"

-- Helper function to print Array content
local function print_RECT(title, rect)
    print(title)
    print("\tleft:", rect.left)
    print("\ttop:", rect.top)
    print("\tright:", rect.right)
    print("\tbottom:", rect.bottom, "\n")
end

-- Defines Windows API RECT struct
local RECT = c.Struct("iiii", "left", "top", "right", "bottom")

-- Defines a RECT2 struct that contains a field 'rects', an array of 4 RECT
local RECT2 = c.Struct(".[4]pB", {rects = RECT}, "handle", "isok")

-- Creates a new RECT2 struct
local r = RECT2()

-- Fill r struct with arbitrary data
for i=0,3 do
    r.rects[i] = RECT {left = i, top = i, right = i, bottom = i}
end

-- Prints each RECT struct in the r.rects[] array
for i=0,3 do
    print_RECT("r.rects["..i.."] :", r.rects[i])
end