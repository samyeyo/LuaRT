-- LuaRT stamp example
-- Blit an Image on the canvas when the user clicks on it

local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas stamp example", 640, 480)

local c = ui.Canvas(win)
c.align = "all"
c.cursor = "cross"
c.bgcolor = 0x000000FF

-- Create an Image instance from file
local img = c:Image(sys.File(arg[0]).path.."\\LuaRT.png")
img.centerx = img.width*0.5/2
img.centery = img.height*0.5/2

function c:onHover(x, y, buttons)
	c:begin()
	if not buttons or buttons.left then		
		img:draw(x - img.centerx, y - img.centery, 0.5, 0.3)
	end
	c:flip()
end

c.onClick = c.onHover

ui.run(win):wait()

