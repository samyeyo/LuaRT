local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas - Hello world example", "fixed", 320, 240)

local c = ui.Canvas(win)
c.align = "all"
c.pos = -100
c.font = "Impact"
c.fontsize = 24
c.color = 0xA2D2FFFF

local x = 125
local y = 100

-- Uses internal timer for 30fps drawing
function c:onPaint()
	self:clear(0xffffffff)	
	c:print("Hello World !", c.pos, 20)
	c.pos = c.pos + 3
	if c.pos > win.width then
		c.pos = -100
	end
	c:fillrect(125, 100, 175, 150, 0xCDB4DBFF)
	c:fillellipse(185, 125, 25, 25, 0xe640407F)
end

ui.run(win):wait()

