--
-- LuaRT C FFI example manipulating a C Array of `char`
--

local c = require "c"

-- Helper function to print Array content
local function print_array(a, title)
    local function tochar(ch)
        return ch == '\0' and '\\0' or ch
    end
    print("\n"..title.." :")
    for i=0, 2 do
        print("Row "..(i+1)..':', tochar(a[i][0]), tochar(a[i][1]), tochar(a[i][2]), tochar(a[i][3]))
    end
end

-- Creates a C array with 3 rows of 4 chars
local CharArray = c.Array("c", 3, 4)

local array = CharArray {
    { "a", "b", "c", '\0' },
    { "d", "e", "f", '\0' },
    { "g", "h", "i", '\0' }
}

print_array(array, "Just created C Array of char")

-- Set values individualy for the first row
array:set(0, 0, 65) -- 'A'
array:set(0, 1, 66) -- 'B'
array:set(0, 2, 67) -- 'C'
array:set(0, 3, 0)

-- Set values individualy for the second row
array:set(1, 0, 68) -- 'D'
array:set(1, 1, 69) -- 'E'
array:set(1, 2, 70) -- 'F'
array:set(1, 3, 0)

-- Set values individualy for the third row
array:set(2, 0, 71) -- 'G'
array:set(2, 1, 72) -- 'H'
array:set(2, 2, 73) -- 'I'
array:set(2, 3, 0) 

print_array(array, "Values set invidivdualy using Array:set()")

-- Set the first row
array[0] =  { "1", "2", "3", '\0' }

-- Set the first row
array[1] =  { "4", "5", "6", '\0' }

-- Set the first row
array[2] =  { "7", "8", "9", '\0' }

print_array(array, "Rows set using Array indexing []")

print("\nRows as C char* pointers:")
for i=0, 2 do
    print("Row "..(i+1)..':', '"'..tostring(c.Pointer(array[i]))..'"')
end


