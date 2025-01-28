--
-- LuaRT C FFI example to calculate Fibonacci series using Lua numbers and C.Values
-- C.Values calculation are way slower than with Lua numbers
-- If you want to calculate from C.Values, convert to number with Values:tonumber() method
--

local c = require "c"
local console = require "console"

local function benchmark(lang, func, ...)
  local start = sys.clock()
  console.write("Fibonacci series with "..lang)
  func(...)
  console.writecolor("yellow", "\t"..(sys.clock()-start).."ms\n")
end

local function fib(n)
  return (type(n) == Value and n:tonumber() <= 1 or n <= 1) and n or fib(n-1) + fib(n-2)
end

benchmark("Lua values\t", fib, 27)
benchmark("C values\t\t", fib, c.int64_t(27))

-- Values:tonumber() is faster (the method returns a Lua integer when the value fits in it)
benchmark("C values (tonumber)", fib, c.int64_t(27):tonumber())