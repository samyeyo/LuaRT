--
--  LuaRT tasks.wlua example
--  Concurrent Tasks that advance Progressbars
--

local ui = require "ui"

local win = ui.Window("Concurrent Tasks example", 320, 200)
local y = 30

-- Add a new Progressbar
-- and make it advance asynchronously
-- using sleep() to set progression speed
local function bar(i)
    ui.Label(win, "Task #"..i, 12, y)
    local pb = ui.Progressbar(win, 60, y, 230)
    y = y + 30

    -- throw a task that increase the Progressbar position
    -- uses a different speed for each task
    async(function()
        while pb.position < 100 do
            sleep(i*20)
            pb:advance(1)
        end
    end)
end

for i=1,5 do
    bar(i)
end

ui.run(win):wait()