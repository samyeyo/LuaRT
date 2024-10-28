-- LuaRT stamp example
-- Blit an Image on the canvas when the user clicks on it

local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas stamp example", 640, 480)

local c = ui.Canvas(win)
c.align = "all"
c.cursor = "cross"
c.bgcolor = 0x000000FF
c:clear()

-- Create an Image instance from file
local img = c:Image(sys.File(arg[0]).path.."\\LuaRT.png")
img.centerx = img.width/4
img.centery = img.height/4

function c:onHover(x, y, buttons)
    -- Draws only if onClick() or if onHover with left button pressed
	if not buttons or buttons.left then		
		img:draw(x-img.centerx, y-img.centery, 0.5, 0.3)
	end
end

c.onClick = c.onHover

ui.run(win):wait()

