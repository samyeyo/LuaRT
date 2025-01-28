--
-- LuaRT C FFI example that uses 2 unions to use RGBA color value
--

local c = require "c"

-- Defines a RGBA Struct C definition
local RGBA = c.Struct("CCCC", "red", "green", "blue", "alpha")

-- Creates a COLOR Union C definition (using RGBA struct)
local COLOR = c.Union(".I", {rgba = RGBA}, "value")

-- Creates a new COLOR instance with arguments initalization
local red = COLOR { value = 0xFF0000FF }

print(string.format("Color.value is 0x%X", red.value))
print(string.format("\t|- Red:\t\t0x%.2X", red.rgba.red))
print(string.format("\t|- Green:\t0x%.2X", red.rgba.green))
print(string.format("\t|- Blue:\t0x%.2X",  red.rgba.blue))
print(string.format("\t|- Alpha:\t0x%.2X\n", red.rgba.alpha))

-- Creates a COLOR Union C definition (using 4 bytes array)
COLOR = c.Union("C[4]I", "rgba", "value")

-- Creates a new COLOR instance with arguments initalization
local red = COLOR { value = 0xFF0000FF }

print(string.format("Color.value is 0x%X", red.value))
print(string.format("\t|- Red:\t\t0x%.2X", red.rgba[0]))
print(string.format("\t|- Green:\t0x%.2X", red.rgba[1]))
print(string.format("\t|- Blue:\t0x%.2X", red.rgba[2]))
print(string.format("\t|- Alpha:\t0x%.2X", red.rgba[3]))