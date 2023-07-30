local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas - Hello world example", "fixed", 320, 240)

local c = ui.Canvas(win)
c.align = "all"
c.pos = -100
c.font = "Segoe Script"
c.fontsize = 24
c.color = 0xffcc00ff

local degree = 0
local scale = 1
local delta = 0.1
local centerx = c.width/2-65
local centery = c.height/2-20

-- Uses internal timer for 30fps drawing
function c:onPaint()
	self:clear(0xffffffff)
	c:rotate(degree)
	c:scale(scale, scale)
	c:print("Hello LuaRT !", centerx, centery)
	degree = degree + 2
	scale = scale + delta
	delta = (scale > 2.5 and delta == 0.1) and -0.1 or ((scale < 1 and delta == -0.1) and 0.1) or delta
end

ui.run(win)