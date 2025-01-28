--
-- LuaRT C FFI example manipulating a C Struct 
--

local c = require "c"

-- Helper function to print a RECT struct content
local function print_RECT(title, rect)
    print(title)
    print("\tleft:", rect.left)
    print("\ttop:", rect.top)
    print("\tright:", rect.right)
    print("\tbottom:", rect.bottom, "\n")
end

-- Defines Windows API RECT struct
local RECT = c.Struct("iiii", "left", "top", "right", "bottom")

-- Creates a new RECT struct with initialized values
local r = RECT { left = 4, top = 3, right = 2, bottom = 1 }
print_RECT("RECT struct r : ", r)

-- Creates a C Pointer to r
local pr = c.Pointer(r);

-- Changes r fields using Pointer.content property (using binary data)
pr.content = sys.Buffer("\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00")
print_RECT("Changed r fields using Pointer.content and binary data:", r)

-- Changes r fields using Pointer.content property fields by fields
pr.content.left = 4
pr.content.top = 3
pr.content.right = 2
pr.content.bottom = 1
print_RECT("Changed r fields using Pointer.content (fields by fields):", r)