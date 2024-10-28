local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas - example", 800, 600)

local c = ui.Canvas(win)
c.align = "all"
c.pos = -100
c.font = "Impact"
c.fontsize = 24
c.color = 0xA2D2FFFF
c.bgcolor = 0x000000FF

function win:onKey(key)
	if key == "VK_RETURN" then
		win.fullscreen = not win.fullscreen
	end
end

function c:onPaint()
	self:clear()	
	c:print("Press RETURN Key", c.pos, 20)
	c.pos = c.pos + 2
	if c.pos > win.width then
		c.pos = -100
	end
	c:fillrect(125, 100, 175, 150, 0xCDB4DBFF)
	c:fillellipse(185, 125, 25, 25, 0xe640407F)
end

ui.run(win):wait()

