-- LuaRT stamp example
-- Blit an Image on the canvas when the user clicks on it

local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas stamp example")
local c = ui.Canvas(win)
c.align = "all"
c.cursor = "cross"
c.sync = false

-- Create an Image instance from file
local img = c:Image(sys.File(arg[1]).path.."\\LuaRT.png")

-- Flag to draw on the canvas or not
c.drawing = false

-- Canvas:onRelease() event handler
-- Thrown when user releases the left mouse button
function c:onRelease()
	self.drawing = false
end

-- Canvas:onClick() event handler
-- Thrown when user clicks on the canvas with the left mouse button
function c:onClick(x, y)
	self.drawing = true
	self:onHover(x, y)
end

function c:onHover(x, y)
    -- draws only when a previous mouse click occured
	if self.drawing then
		self:begin()
		img:draw(x-img.width/2*0.4, y-img.height/2*0.4, 0.4, 0.6)
		self:flip()		
	end
end

ui.run(win)

