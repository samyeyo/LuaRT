-- 
-- LuaRT moving ball example  : Move a red ball using arrow keys
--

local ui = require "ui"
local kb = require "keyboard"
require "canvas"

local win = ui.Window("keyboard example - Move the ball using arrow keys", "fixed", 410, 464)

local c = ui.Canvas(win)
c.align = "all"

-- load the background Image
local bg = c:Image(sys.File(arg[0]).path.."back.png")

-- set initial ball position to the center
x = c.width/2
y = c.height/2

-- start the ui updating Task
ui.run(win)

-- start the keyboard handler Task
async(function()
    while win.visible do
        if kb.isdown("LEFT") and x > 11 then
            x = x - 6
        end
        if kb.isdown("RIGHT") and x < c.width-11 then
            x = x + 6
        end
        if kb.isdown("UP") and y > 10 then
            y = y - 5
        end
        if kb.isdown("DOWN") and y < c.height-10 then
            y = y + 5
        end
        sleep()
    end
end)

-- Canvas onPaint() event
function c:onPaint()
    self:begin()
    -- draw the image on the whole Window
    bg:drawrect(0, 0, 410, 464)
    -- draw the ball shadow
    self:fillcircle(x+3, y+2, 7, 0x00000080)
    -- draw the red ball
    self:fillcircle(x, y, 7, 0xFF0000FF)
    self:flip()
end

-- wait for all tasks to finish
waitall()
